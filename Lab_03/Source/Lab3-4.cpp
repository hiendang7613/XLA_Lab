#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "EdgeDetector.h"
#include "Blur.h"
#include "Convolution.h"

using namespace cv;
using namespace std;

// So sanh cau lenh nhap vao
bool cmdCompare(char* cmd, string type) {
	for (int len = 0; len < type.size(); len++) {
		if (cmd[len] != type[len])
			return false;
	}
	return true;
}

int main(int argc, char** argv)
{
	Mat inputImage = imread(argv[2], IMREAD_ANYCOLOR);
	//cvtColor(inputImage, inputImage, COLOR_RGBA2GRAY);

	Mat outputImage = inputImage.clone();

	if (cmdCompare(argv[1], "--mean"))
	{
		// Làm trơn ảnh bằng toán tử trung bình
		Blur blur;
		int w = atoi(argv[3]);
		int h = atoi(argv[4]);
		blur.BlurImage(inputImage, outputImage, w, h, 0);
	}
	else if (cmdCompare(argv[1], "--median"))
	{
		// Làm trơn ảnh bằng toán tử trung vị
		Blur blur;
		int w = atoi(argv[3]);
		int h = atoi(argv[4]);
		blur.BlurImage(inputImage, outputImage, w, h, 1);
	}
	else if (cmdCompare(argv[1], "--gauss"))
	{
		// Làm trơn ảnh bằng toán tử gauss
		Blur blur;
		int w = atoi(argv[3]);
		int h = atoi(argv[4]);
		blur.BlurImage(inputImage, outputImage, w, h, 2);
	}
	else if (cmdCompare(argv[1], "--sobel"))
	{
		cvtColor(inputImage, outputImage, COLOR_RGBA2GRAY);
		// Phat hien bien canh bang Sobel
		EdgeDetector func;
		func.DetectEdge(outputImage.clone(), outputImage, 3, 3, 1);
	}
	else if (cmdCompare(argv[1], "--prewitt"))
	{
		cvtColor(inputImage, outputImage, COLOR_RGBA2GRAY);
		// Phat hien bien canh bang Prewitt
		EdgeDetector func;
		func.DetectEdge(outputImage.clone(), outputImage, 3, 3, 2);
	}
	else if (cmdCompare(argv[1], "--laplace"))
	{
		cvtColor(inputImage, outputImage, COLOR_RGBA2GRAY);
		// Phat hien bien canh bang Laplace
		EdgeDetector func;
		func.DetectEdge(outputImage.clone(), outputImage, 3, 3, 3);
	}

	// Hien thi anh dau vao	
	namedWindow("Input Image");
	imshow("Input Image", inputImage);

	// Hien thi anh dau ra	
	namedWindow("Output Image");
	imshow("Output Image", outputImage);

	waitKey(0);
	return 0;
}