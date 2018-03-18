//#include <hls_stream.h>
#include <ap_int.h>
#include <hls_video.h>
#define IMG_ROWS 360
#define IMG_COLS 640
#define KERNEL_DIM 3

typedef float mydata_t;

typedef struct{
    float            data;
    ap_uint<4> 		 keep;
    ap_uint<1>       last;
  }mydata_ch_t;

void img_convolution(hls::stream<mydata_ch_t> &inStream, hls::stream<mydata_ch_t> &outStream);
