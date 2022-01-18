#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "ColorTransformer.h"
#include "Converter.h"
#include "GeometricTransformer.h"
#include <string.h>
#include <iomanip>
#include <math.h>

using namespace cv;
using namespace std;

bool cmdCompare(char* cmd, string type) {
	for (int len = 0; len < type.size(); len++) {
		if (cmd[len] != type[len])
			return false;
	}
	return true;
}

int main(int argc, char* argv[]) {
	ColorTransformer transfer;
	Converter convert;
	GeometricTransformer geotransfer;
	if (cmdCompare(argv[1], "--bright"))
	{ // Tăng giảm độ sáng của ảnh
		Mat inputImage = imread(argv[2], IMREAD_ANYCOLOR);
		Mat outputImage = inputImage.clone();
		transfer.ChangeBrighness(inputImage, outputImage, atoi(argv[3]));
		namedWindow("Show Image");
		imshow("Show Image", outputImage);
		waitKey(0);
	}
	if (cmdCompare(argv[1], "--contrast"))
	{ // Tăng giảm độ tương phản của ảnh
		Mat inputImage = imread(argv[2], IMREAD_ANYCOLOR);
		Mat outputImage = inputImage.clone();
		transfer.ChangeContrast(inputImage, outputImage, atof(argv[3]));
		namedWindow("Show Image");
		imshow("Show Image", outputImage);
		waitKey(0);
	}
	if (cmdCompare(argv[1], "--hist")) { // Tính histogram của ảnh
		Mat inputImage = imread(argv[2], IMREAD_ANYCOLOR);
		Mat histMatrix;
		transfer.CalcHistogram(inputImage, histMatrix);
		for (int i = 0; i < 256; i++) {
			cout << "\t" << i << " : ";
			for (int n = 0; n < 3; n++) {

				cout << (int)histMatrix.at<uchar>(n, i) << cout.width(8);
			}
			cout << endl;
		}
		cout << endl;
		waitKey(0);
	}
	if (cmdCompare(argv[1], "--rgb2gray")) {
		Mat inputImage = imread(argv[2], IMREAD_ANYCOLOR);

		// Khởi tạo ảnh đầu ra
		Mat outputImage = Mat(inputImage.rows, inputImage.cols, CV_8UC1);

		// Gọi hàm xử lý ảnh - chuyển đổi hệ rgb sang grayscale
		convert.Convert(inputImage, outputImage, 0);

		// Xuất ảnh đầu ra ra màn hình
		namedWindow("Show Image");
		imshow("Show Image", outputImage);

		/*if (outputImage.empty())
		{
			std::cerr << "Something is wrong with the webcam, could not get frame." << std::endl;
		}*/
		// Save the frame into a file
		imwrite("gray.jpg", outputImage); // A JPG FILE IS BEING SAVED
		waitKey(0);
	}
	if (cmdCompare(argv[1], "--gray2rgb"))
	{ // --gray2rgb
		Mat inputImage = imread(argv[2], IMREAD_ANYCOLOR);
		Mat outputImage = Mat(inputImage.rows, inputImage.cols, CV_8UC3);
		convert.Convert(inputImage, outputImage, 1);
		namedWindow("Show Image");
		imshow("Show Image", outputImage);
		waitKey(0);
	}
	if (cmdCompare(argv[1], "--rgb2hsv"))
	{ // --rgb2hsv
		Mat inputImage = imread(argv[2], IMREAD_ANYCOLOR);
		Mat outputImage = Mat(inputImage.rows, inputImage.cols, CV_8UC3);
		convert.Convert(inputImage, outputImage, 2);
		namedWindow("Show Image");
		imshow("Show Image", outputImage);
		imwrite("hsv.jpg", outputImage); // A JPG FILE IS BEING SAVED
		waitKey(0);
	}
	if (cmdCompare(argv[1], "--hsv2rgb"))
	{ // Chuyển đổi ảnh hệ HSV sang hệ RGB
		Mat inputImage = imread(argv[2], IMREAD_ANYCOLOR);
		Mat outputImage = Mat(inputImage.rows, inputImage.cols, CV_8UC3);
		convert.Convert(inputImage, outputImage, 3);
		namedWindow("Show Image");
		imshow("Show Image", outputImage);
		waitKey(0);
	}
	if (cmdCompare(argv[1], "--drawhist"))
	{

		Mat sourceImage = imread(argv[2], IMREAD_ANYCOLOR);
		Mat histMatrix;
		Mat histImage;
		int i = transfer.CalcHistogram(sourceImage, histMatrix);
		int check = transfer.DrawHistogram(histMatrix, histImage);
		if (check == 1) {
			//Show old Image
			namedWindow("Old Image");
			imshow("Old Image", sourceImage);
			namedWindow("Histogram");
			imshow("Histogram", histImage);
			waitKey(0);
		}
	}
	if (cmdCompare(argv[1], "--equalhist"))
	{
		Mat sourceImage = imread(argv[2], IMREAD_ANYCOLOR);
		Mat desImage;
		int check = transfer.HistogramEqualization(sourceImage, desImage);
		if (check == 1)
		{
			namedWindow("Source Image");
			imshow("Source Image", sourceImage);
			namedWindow("Equalized Image");
			imshow("Equalized Image", desImage);
			waitKey(0);
		}
	}
	if (cmdCompare(argv[1], "--compare"))
	{
		Mat image1 = imread(argv[2], IMREAD_ANYCOLOR);
		Mat image2 = imread(argv[3], IMREAD_ANYCOLOR);
		float compare = transfer.CompareImage(image1, image2);

		namedWindow("Image 1");
		imshow("Image 1", image1);
		namedWindow("Image 2");
		imshow("Image 2", image2);
		cout << "Ti le trung anh: " << compare << endl;
		waitKey(0);
	}
	if (cmdCompare(argv[1], "--zoom"))
	{
		if (cmdCompare(argv[2], "--bl"))
		{
			Mat srcImage = imread(argv[3], IMREAD_ANYCOLOR);
			Mat desImage;
			PixelInterpolate* pi;
			BilinearInterpolate bi;
			pi = &bi;
			float x = atof(argv[4]);
			float y = atof(argv[5]);
			int check = geotransfer.Scale(srcImage, desImage, x, y, pi);
			if (check == 1)
			{
				imshow("Source Image", srcImage);
				imshow("Destination Image", desImage);
				waitKey(0);
			}
		}
		else if (cmdCompare(argv[2], "--nn"))
		{
			Mat srcImage = imread(argv[3], IMREAD_ANYCOLOR);
			Mat desImage;
			PixelInterpolate* pi;
			NearestNeighborInterpolate ni;
			pi = &ni;
			float x = atof(argv[4]);
			float y = atof(argv[5]);
			int check = geotransfer.Scale(srcImage, desImage, x, y, pi);
			if (check == 1)
			{
				imshow("Source Image", srcImage);
				imshow("Destination Image", desImage);
				waitKey(0);
			}
		}
	}
	if (cmdCompare(argv[1], "--resize"))
	{
		if (cmdCompare(argv[2], "--bl"))
		{
			Mat srcImage = imread(argv[3], IMREAD_ANYCOLOR);
			Mat desImage;
			PixelInterpolate* pi;
			BilinearInterpolate bi;
			pi = &bi;
			float x = atof(argv[4]);
			float y = atof(argv[5]);
			int check = geotransfer.Resize(srcImage, desImage, x, y, pi);
			if (check == 1)
			{
				imshow("Source Image", srcImage);
				imshow("Destination Image", desImage);
				waitKey(0);
			}
		}
		else if (cmdCompare(argv[2], "--nn"))
		{
			Mat srcImage = imread(argv[3], IMREAD_ANYCOLOR);
			Mat desImage;
			PixelInterpolate* pi;
			NearestNeighborInterpolate ni;
			pi = &ni;
			float x = atof(argv[4]);
			float y = atof(argv[5]);
			int check = geotransfer.Resize(srcImage, desImage, x, y, pi);
			if (check == 1)
			{
				imshow("Source Image", srcImage);
				imshow("Destination Image", desImage);
				waitKey(0);
			}
		}

	}
	
	if (cmdCompare(argv[1], "--rotateN"))
	{
		if (cmdCompare(argv[2], "--bl"))
		{
			Mat srcImage = imread(argv[3], IMREAD_ANYCOLOR);
			Mat desImage;
			PixelInterpolate* pi;
			BilinearInterpolate bi;
			pi = &bi;
			float angle = atof(argv[4]);
			int check = geotransfer.RotateUnkeepImage(srcImage, desImage, angle, pi);
			if (check == 1)
			{
				imshow("Source Image", srcImage);
				imshow("Destination Image", desImage);
				waitKey(0);
			}
		}
		else if (cmdCompare(argv[2], "--nn"))
		{
			Mat srcImage = imread(argv[3], IMREAD_ANYCOLOR);
			Mat desImage;
			PixelInterpolate* pi;
			NearestNeighborInterpolate ni;
			pi = &ni;
			float angle = atof(argv[4]);
			int check = geotransfer.RotateUnkeepImage(srcImage, desImage, angle, pi);
			if (check == 1)
			{
				imshow("Source Image", srcImage);
				imshow("Destination Image", desImage);
				waitKey(0);
			}
		}
		

	}
	else
	{
		if (cmdCompare(argv[1], "--rotate"))
		{
			if (cmdCompare(argv[2], "--bl"))
			{
				Mat srcImage = imread(argv[3], IMREAD_ANYCOLOR);
				Mat desImage;
				PixelInterpolate* pi;
				BilinearInterpolate bi;
				pi = &bi;
				float angle = atof(argv[4]);
				int check = geotransfer.RotateKeepImage(srcImage, desImage, angle, pi);
				if (check == 1)
				{
					imshow("Source Image", srcImage);
					imshow("Destination Image", desImage);
					waitKey(0);
				}
			}
			else if (cmdCompare(argv[2], "--nn"))
			{
				Mat srcImage = imread(argv[3], IMREAD_ANYCOLOR);
				Mat desImage;
				PixelInterpolate* pi;
				NearestNeighborInterpolate ni;
				pi = &ni;
				float angle = atof(argv[4]);
				int check = geotransfer.RotateKeepImage(srcImage, desImage, angle, pi);
				if (check == 1)
				{
					imshow("Source Image", srcImage);
					imshow("Destination Image", desImage);
					waitKey(0);
				}
			}

		}
	}
	if (cmdCompare(argv[1], "--flip"))
	{
		if (cmdCompare(argv[2], "--bl"))
		{
			Mat srcImage = imread(argv[3], IMREAD_ANYCOLOR);
			Mat desImage;
			PixelInterpolate* pi;
			BilinearInterpolate bi;
			pi = &bi;
			float direction = atof(argv[4]);
			int check = geotransfer.Flip(srcImage, desImage, direction, pi);
			if (check == 1)
			{
				imshow("Source Image", srcImage);
				imshow("Destination Image", desImage);
				waitKey(0);
			}
		}
		else if (cmdCompare(argv[2], "--nn"))
		{
			Mat srcImage = imread(argv[3], IMREAD_ANYCOLOR);
			Mat desImage;
			PixelInterpolate* pi;
			NearestNeighborInterpolate ni;
			pi = &ni;
			float direction = atof(argv[4]);
			int check = geotransfer.Flip(srcImage, desImage, direction, pi);
			if (check == 1)
			{
				imshow("Source Image", srcImage);
				imshow("Destination Image", desImage);
				waitKey(0);
			}
		}

	}
	system("pause");
	return 0;
}
