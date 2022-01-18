#pragma once

#include <vector>
#include <math.h>
#include "Convolution.h"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>

using namespace std;
using namespace cv;

// Khoi tao ma tran Wx cua toan tu Sobel
vector<float> tempSobelKernel1 = { -1, 0, 1,
								   -2, 0, 2,
								   -1, 0, 1 };

// Khoi tao ma tran Wy cua toan tu Sobel
vector<float> tempSobelKernel2 = { -1,-2,-1,
							        0, 0, 0,
							        1, 2, 1 };

// Khoi tao ma tran Wx cua toan tu Prewitt
vector<float> tempPrewittKernel1 = { 5, 5, 5,
							        -3, 0,-3,
							        -3,-3,-3 };

// Khoi tao ma tran Wy cua toan tu Prewitt
vector<float> tempPrewittKernel2 = { 5, -3, -3,
							         5, 0, -3,
							         5, -3, -3 };

// Khoi tao ma tran cua toan tu Laplace
vector<float> tempLaplaceKernel = { 0, 1, 0,
							        1, -4, 1,
							        0, 1, 0 };

class EdgeDetector
{
public:
	/*
	Hàm phát hiện biên cạnh của ảnh xám
	sourceImage: ảnh input
	destinationImage: ảnh output
	method: phương pháp detect
	kWidth, kHeight: kích thước kernel
		1: Sobel
		2: Prewitt
		3: Laplace
	Hàm trả về
		0: nếu detect thành công
		1: nếu detect thất bại (không đọc được ảnh input,...)
	*/

	// Ham phat hien bien canh bang Sobel
	int Sobel(const Mat& sourceImage, Mat& destinationImage)
	{
		int nRow = sourceImage.rows;
		int nCol = sourceImage.cols;
		Convolution SobelKernel;
		Mat Img1, Img2;

		// Ma tran Sobel 1
		SobelKernel.SetKernel(tempSobelKernel1, 3, 3);

		SobelKernel.DoConvolution(sourceImage, Img1);
		// Ma tran Sobel 2
		SobelKernel.SetKernel(tempSobelKernel2, 3, 3);

		SobelKernel.DoConvolution(sourceImage, Img2);

		// Tạo mma tran de luu gi tri pixel
		destinationImage.create(Size(nCol, nRow), CV_8UC1);

		for (int i = 0; i < nRow; i++)
		{
			// Lay dia chi dong cua anh dich de luu ket qua vao
			uchar* data = destinationImage.ptr<uchar>(i);

			// Lay dia chi dong cua Image1
			uchar* data1 = Img1.ptr<uchar>(i);

			//Lay dia chi dong cua Image2
			uchar* data2 = Img2.ptr<uchar>(i);

			// Gan tong gia tri tuyet doi cua hai mang vua tich chap vao anh dich 
			for (int j = 0; j < nCol; j++)
			{
				int val = 0;

				if (data1[j] >= 0) val += data1[j];
				else val += data1[j];

				if (data2[j] >= 0) val += data2[j];
				else val += data2[j];

				// Gan gia tri cho ma tran dich
				data[j] = val;
			}
		}

		return 0;
	}

	// Ham phat hien bien canh bang Prewitt
	int Prewitt(const Mat& sourceImage, Mat& destinationImage)
	{
		int nRow = sourceImage.rows;
		int nCol = sourceImage.cols;
		Convolution PrewittKernel;
		Mat Img1, Img2;

		// Ma tran Prewitt 1
		PrewittKernel.SetKernel(tempPrewittKernel1, 3, 3);

		PrewittKernel.DoConvolution(sourceImage, Img1);
		// Ma tran Prewitt 2
		PrewittKernel.SetKernel(tempPrewittKernel2, 3, 3);

		PrewittKernel.DoConvolution(sourceImage, Img2);

		// Tao ma tran de luu gia tri pixel
		destinationImage.create(Size(nCol, nRow), CV_8UC1);

		for (int i = 0; i < nRow; i++)
		{
			// Lay dia chi dong cua anh dich de luu ket qua vao
			uchar* data = destinationImage.ptr<uchar>(i);

			// Lay dia chi dong cua Image1
			uchar* data1 = Img1.ptr<uchar>(i);

			//Lay dia chi dong cua Image2
			uchar* data2 = Img2.ptr<uchar>(i);

			// Gan tong gia tri tuyet doi cua hai mang vua tich chap vao anh dich 
			for (int j = 0; j < nCol; j++)
			{
				int val = 0;

				if (data1[j] > 0) val += data1[j];
				else val += -data1[j];

				if (data2[j] > 0) val += data2[j];
				else val += -data2[j];

				// Gan gia tri cho ma tran dich
				data[j] = val;
			}
		}

		return 0;
	}

	// Ham phat hien bien canh bang Laplace
	int Laplace(const Mat& sourceImage, Mat& destinationImage)
	{
		Convolution LaplaceKernel;

		// Ma tran Laplace
		LaplaceKernel.SetKernel(tempLaplaceKernel, 3, 3);

		LaplaceKernel.DoConvolution(sourceImage, destinationImage);
		return 0;
	}

	// Ham goi cac ham chuc nang
	int DetectEdge(const Mat& sourceImage, Mat& destinationImage, int kWidth, int kHeight, int method) {
		if (sourceImage.ptr(0) != NULL)
		{
			EdgeDetector func;
			if (method == 1) func.Sobel(sourceImage, destinationImage);
			else if (method == 2) func.Prewitt(sourceImage, destinationImage);
			else if (method == 3) func.Laplace(sourceImage, destinationImage);
			else return 1;
			return 0;
		}
		return 1;
	}
	EdgeDetector() {};
	~EdgeDetector() {};
};

