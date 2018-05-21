#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <opencv2/opencv.hpp>
#include <yolo_lib.h>
using namespace cv;

void write_jpeg(Mat &a, const char *filename)
{
	std::vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);

	imwrite(filename, a, compression_params);
}

void write_raw_image(void *raw_image,const char *filename, unsigned int size)
{
	FILE *out_fp;
	out_fp = fopen(filename, "wb");
	fwrite(raw_image,1,size,out_fp);
	fclose(out_fp);

}

int main(int argc, char *argv[])
{
	char *file = argv[1];
	char *res = argv[2];
	FILE *fp;
	long f_size;
	void *yuv422_data;
	void *rgb24_data;
	void *resiz_data;
	int width;
	int height;
	int out_width;
	int out_height;
	char *outfile = argv[4];
	size_t ret;

	if(argc < 5){
        fprintf(stderr, "usage: YUYV2RGB [input] [input resolution] [output resolution] [output] \n");
        fprintf(stderr, "ex: YUYV2RGB resource/car.jpg 640x480 448x448 rgb.out\n");
        return 0;
    }


	width = atoi(strtok(res, "x"));
	height = atoi(strtok(NULL, "x"));

	res = argv[3];
	out_width = atoi(strtok(res, "x"));
	out_height = atoi(strtok(NULL, "x"));

	printf("%s,%dx%d %dx%d %s \n", file, width,height, out_width,out_height, outfile);


	/*
	 * Set up YUV422 input image.
	 * */
	fp = fopen(file, "rb");
	if (fp==NULL) {
		fputs ("File error",stderr);
		exit (1);
	 }

    fseek (fp, 0, SEEK_END);   // non-portable
    f_size=ftell (fp);
    rewind(fp);

	yuv422_data = malloc(f_size);
	ret = fread(yuv422_data, 1, f_size, fp);

	printf("read %ld bytes, error %d\n", ret, ferror (fp));
	fclose(fp);

	//Mat YUV422_Mat(height, width, CV_8UC2, yuv422_data);
	Mat YUV422_Mat(height, width, CV_8UC2, yuv422_data);

	/*
	 * Set up RGB24 output image
	 */
	rgb24_data = malloc(width*height*3);
	Mat RGB24_Mat(height,width,CV_8UC3,rgb24_data);


	cvtColor(YUV422_Mat, RGB24_Mat, COLOR_YUV2RGB_YUYV);

	/*
	 * setup resize RGB image
	 */
	resiz_data = malloc(out_width*out_height*3);
	Mat Resize_Mat(out_height,out_width,CV_8UC3,resiz_data);
	resize(RGB24_Mat, Resize_Mat, Size(out_height,out_width));

	setup_yolo_env("resource/tiny-yolo-xnor.cfg", "resource/tiny-yolo-xnor.weight");
	yolo_inference_with_ptr(Resize_Mat.ptr(), out_width, out_height, 3, 0.2);


#ifdef DEBUG

	write_raw_image(rgb24_data,"rgb24_640480_car_conv.raw",width*height*3 );
	write_raw_image(resiz_data,"rgb24_448x448_car_conv.raw",out_width*out_height*3 );

	/*
	 * Resize_Mat.ptr() = resiz_data.
	 * */
	write_raw_image(Resize_Mat.ptr(),"mptr_rgb24_448x448_car_conv.raw",out_width*out_height*3 );
	/*
	 * imwrite() only accepts BGR format.
	 * */
	cvtColor(RGB24_Mat, RGB24_Mat, COLOR_RGB2BGR);
	write_jpeg(RGB24_Mat, "rgb24_640480_car_conv.jpg");
	cvtColor(Resize_Mat, Resize_Mat, COLOR_RGB2BGR);
	write_jpeg(Resize_Mat, "rgb24_448x448_car_conv.jpg");
#endif
	free(resiz_data);
	free(rgb24_data);
	free(yuv422_data);
	return 0;

}
