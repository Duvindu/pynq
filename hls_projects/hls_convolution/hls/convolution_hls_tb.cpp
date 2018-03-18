#include "convolution_hls_tb.h"
#include <opencv2/core/core.hpp>
#include <hls_opencv.h>


void saveImage(const std::string path, cv::InputArray inArr){
	double min;
	double max;
	cv::minMaxIdx(inArr, &min, &max);
	cv::Mat adjMap;
	cv::convertScaleAbs(inArr, adjMap, 255/max);
	cv::imwrite(path, adjMap);
}

int main()
{
	mydata_t kernel[KERNEL_DIM*KERNEL_DIM] = {
			-1.0, -2.0, -1.0,
			 0.0, 0.0, 0.0,
			1.0, 2.0, 1.0
	};

	cv::Mat imgSrc;
	mydata_ch_t inChannel;
	mydata_ch_t outChannel;
	hls::stream<mydata_ch_t> inStream;
	hls::stream<mydata_ch_t> outStream;
	mydata_t inPixel;
	mydata_t outPixel;
	mydata_t lastPixel;
	mydata_t outImg[IMG_ROWS][IMG_COLS];

	imgSrc = cv::imread("D:/Data/FPGA/DAC2018/Projects/hls_test_prj/convolution_hls_float/hls/convolution_hls/000001.jpg");

	cv::cvtColor(imgSrc, imgSrc, CV_BGR2GRAY);
	cv::Mat imgCVOut(cv::Size(imgSrc.cols,imgSrc.rows), CV_32FC1, (void*)outImg, cv::Mat::AUTO_STEP); //float
	//cv::Mat imgCVOut(cv::Size(IMG_COLS,IMG_ROWS), CV_8UC1, (void*)outImg, cv::Mat::AUTO_STEP);
	printf("Img height : %d, Width : %d\n", imgSrc.size().height, imgSrc.size().width);
	for(int i=0;i<imgSrc.size().height;i++){
		for(int j=0;j<imgSrc.size().width;j++){
			inPixel = (mydata_t)imgSrc.at<unsigned char>(i,j);
			inChannel.data = inPixel;
			inChannel.keep = 15;
			if(i*j == (imgSrc.size().height * imgSrc.size().width -1)){
				inChannel.last = 1;
			}
			else{
				inChannel.last = 0;
			}
			inStream<<inChannel;
		}
	}
	int validPixCount, invalidPixCount;
	img_convolution(inStream, outStream);

	for(int i=0;i<IMG_ROWS;i++){
		for(int j=0;j<IMG_COLS;j++){
			while(outStream.empty());
			outChannel = outStream.read();
			outPixel = outChannel.data;
			outImg[i][j] = outPixel;
		}
	}
	printf("valid pix count : %d, invalid pix count : %d, Last Pix : %f\n", validPixCount, invalidPixCount, lastPixel);
    saveImage("D:/Data/FPGA/DAC2018/Projects/hls_test_prj/convolution_hls_float/hls/convolution_hls/000001_edited.jpg", imgCVOut);

	return 0;
}

