#include "convolution_hls.h"

void img_convolution(hls::stream<mydata_ch_t> &inStream, hls::stream<mydata_ch_t> &outStream)
{
#pragma HLS INTERFACE s_axilite port=return bundle=CTRL_BUS
#pragma HLS INTERFACE axis register both port=outStream
#pragma HLS INTERFACE axis register both port=inStream

	mydata_ch_t inChannel;
	mydata_ch_t outChannel;
	mydata_t pixelIn;
	mydata_t tempVal;
	int idxCol;
	int pixCount;
	int initialWaitPixCount;
	mydata_t accumulator;
	mydata_t kernel[KERNEL_DIM*KERNEL_DIM] = {
				-1.0, -2.0, -1.0,
				 0.0, 0.0, 0.0,
				1.0, 2.0, 1.0
		};
#pragma HLS ARRAY_PARTITION variable=kernel complete dim=0

	//window buffer
	mydata_t lineBuffa[IMG_COLS];
	mydata_t lineBuffb[IMG_COLS];
	mydata_t lineBuffc[IMG_COLS];
	mydata_t windowBuff[KERNEL_DIM][KERNEL_DIM];
#pragma HLS ARRAY_PARTITION variable=windowBuff complete dim=0

	idxCol = 0;
	pixCount = 0;
	initialWaitPixCount = IMG_COLS * (KERNEL_DIM-1) + KERNEL_DIM;

	for(int i = 0;i<IMG_ROWS*IMG_COLS; i++){
//#pragma HLS PIPELINE
		//initialize
		accumulator = 0;
		inChannel = inStream.read();
		pixelIn = inChannel.data;
		// Put data on the LineBuffer
		lineBuffa[idxCol] = lineBuffb[idxCol];
		lineBuffb[idxCol] = lineBuffc[idxCol];
		lineBuffc[idxCol] = pixelIn;

		for(int j=0;j<KERNEL_DIM;j++){
			windowBuff[j][2] = windowBuff[j][1];
			windowBuff[j][1] = windowBuff[j][0];
		}
		windowBuff[0][0] = lineBuffa[idxCol];
		windowBuff[1][0] = lineBuffb[idxCol];
		windowBuff[2][0] = lineBuffc[idxCol];

		//output
		outChannel.keep = 15;
		if(pixCount == IMG_COLS * IMG_ROWS -1){
			outChannel.last = 1;
		}
		else {
			outChannel.last = 0;
		}

		if(pixCount>= (initialWaitPixCount-1) &&  (idxCol >= KERNEL_DIM-1)){
			for(int j=0;j<KERNEL_DIM;j++){
				for(int k=0;k<KERNEL_DIM;k++){
					accumulator += windowBuff[j][k] * kernel[KERNEL_DIM * j + k];
				}
			}
			outChannel.data = accumulator;
			outStream.write(outChannel);
		}
		else{
			accumulator = 0.00;
			outChannel.data = 0.0;
			outStream.write(outChannel);
		}
		if(idxCol == IMG_COLS - 1){
			idxCol = 0;
		}
		else{
			idxCol++;
		}
		pixCount++;
	}
}
