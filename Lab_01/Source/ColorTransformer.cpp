#pragma once
#include "ColorTransformer.h"


ColorTransformer::ColorTransformer() {
};

ColorTransformer::~ColorTransformer() {
};


int ColorTransformer::ChangeBrighness(const Mat& sourceImage, Mat& destinationImage, short b) {

	// Sao chép ảnh đích từ ảnh nguồn
	destinationImage = sourceImage.clone();

	// số kênh màu của ảnh
	int nChannels = sourceImage.channels();

	// Duyệt qua các pixel
	for (int y = 0; y < sourceImage.rows; y++) {
		for (int x = 0; x < sourceImage.cols; x++) {

			// ảnh RGB 3 kênh màu
			if (nChannels == 3)
				for (int channel = 0; channel < nChannels; channel++)
					(*destinationImage.ptr<Vec3b>(y, x))[channel] = saturate_cast<uchar>((*sourceImage.ptr<Vec3b>(y, x))[channel] + b);

			// ảnh GrayScale 1 kênh màu
			else
				*destinationImage.ptr<uchar>(y, x) = saturate_cast<uchar>(*sourceImage.ptr<uchar>(y, x) + b);
		}
	}

	return 1;
};

int ColorTransformer::ChangeContrast(const Mat& sourceImage, Mat& destinationImage, float c) {

	// Sao chép ảnh đích từ ảnh nguồn
	destinationImage = sourceImage.clone();

	// Lấy số kênh màu của ảnh đầu vào
	int nChannels = sourceImage.channels();

	// Duyệt qua tất cả các pixel của ảnh
	for (int y = 0; y < sourceImage.rows; y++) {
		for (int x = 0; x < sourceImage.cols; x++) {

			// ảnh RGB 3 kênh màu
			if (nChannels == 3)
				for (int channel = 0; channel < nChannels; channel++)
					(*destinationImage.ptr<Vec3b>(y, x))[channel] = saturate_cast<uchar>((*sourceImage.ptr<Vec3b>(y, x))[channel] * c);

			// ảnh GrayScale 1 kênh màu
			else
				*destinationImage.ptr<uchar>(y, x) = saturate_cast<uchar>(*sourceImage.ptr<uchar>(y, x) * c);
		}
	}

	// thành công
	return 1;
};

int ColorTransformer::CalcHistogram(const Mat& sourceImage, Mat& histMatrix) {

	// Lấy số kênh màu của ảnh đầu vào
	int nChannels = sourceImage.channels();

	histMatrix.create(nChannels, 256, CV_8UC3);

	// Khởi tạo histogram với tất cả 0
	histMatrix.setTo(0);

	// Duyệt qua tất cả pixel
	for (int row = 0; row < sourceImage.rows; row++)
	{
		for (int col = 0; col < sourceImage.cols; col++)
		{

			// ảnh RGB 3 kênh màu
			if (nChannels == 3)
				for (int n = 0; n < nChannels; n++)
					(*histMatrix.ptr<uchar>(n, (int)(*sourceImage.ptr<Vec3b>(row, col))[n]))++;

			// ảnh GrayScale 1 kênh màu
			else
				(*histMatrix.ptr<uchar>((int)sourceImage.ptr<uchar>(row, col)))++;
		}
	}

	return 1;
}

