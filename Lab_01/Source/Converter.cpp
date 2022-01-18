#pragma once
#include "Converter.h"


Converter::Converter() {
};
Converter::~Converter() {
};

/*
Hàm chuyển đổi không gian màu của ảnh từ RGB sang GrayScale
sourceImage: ảnh input
destinationImage: ảnh output
Hàm trả về
0: nếu chuyển thành công
1: nếu chuyển thất bại (không đọc được ảnh input,...)
*/
int Converter::RGB2GrayScale(const Mat& sourceImage, Mat& destinationImage) {
	// Nếu ảnh input đầu vào không phải là ảnh RGB thì chuyển thất bại
	if (sourceImage.type() != CV_8UC3) {
		return 0;
	}

	// Khởi tạo ảnh output với kích thước của ảnh input, số kênh màu là 1 
	destinationImage = Mat(sourceImage.rows, sourceImage.cols, CV_8UC1, Scalar(0));

	// Duyệt qua tất cả các pixel của ảnh đầu vào
	for (int row = 0; row < sourceImage.rows; row++)
	{
		for (int col = 0; col < sourceImage.cols; col++)
		{
			// Gán cho từng pixel ảnh output theo công thức Red * 0.1140 + Green * 0.5870 + Blue * 0.2989
			destinationImage.at<uchar>(row, col) =
				(A * sourceImage.at<Vec3b>(row, col)[0] + B * sourceImage.at<Vec3b>(row, col)[1] + C * sourceImage.at<Vec3b>(row, col)[2]);
		}
	}

	// Chuyển đổi ảnh thành công
	return 1;
};

/*
Hàm chuyển đổi không gian màu của ảnh từ GrayScale sang RGB
sourceImage: ảnh input
destinationImage: ảnh output
Hàm trả về
0: nếu chuyển thành công
1: nếu chuyển thất bại (không đọc được ảnh input,...)
*/
int Converter::GrayScale2RGB(const Mat& sourceImage, Mat& destinationImage)
{
	// Nếu ảnh input đầu vào có số kênh màu khác 1 thì không phải ảnh GrayScale
	if (sourceImage.type() != CV_8UC1)
	{
		return 0;
	}

	// Duyệt qua tất cả các pixel của ảnh input
	for (int row = 0; row < sourceImage.rows; row++)
	{
		for (int col = 0; col < sourceImage.cols; col++)
		{
			float grayScale = sourceImage.at<uchar>(row, col);
			// Gán 3 kênh mỗi của mỗi pixel bằng với giá trị của kênh màu ảnh input
			destinationImage.at<Vec3b>(row, col)[0] = grayScale;
			destinationImage.at<Vec3b>(row, col)[1] = grayScale;
			destinationImage.at<Vec3b>(row, col)[2] = grayScale;
		}
	}

	// Chuyển đổi thành công
	return 1;

};


	/*
	Hàm chuyển đổi không gian màu của ảnh từ RGB sang HSV
	sourceImage: ảnh input
	destinationImage: ảnh output
	Hàm trả về
	0: nếu chuyển thành công
	1: nếu chuyển thất bại (không đọc được ảnh input,...)
	*/
int Converter::RGB2HSV(const Mat& sourceImage, Mat& destinationImage) {
	// Duyệt qua tất cả các pixel của ảnh 
	for (int row = 0; row < sourceImage.rows; row++) {
		for (int col = 0; col < sourceImage.cols; col++) {

			// Lấy giá trị kênh màu đỏ tại pixel thứ (row, col)
			float red = sourceImage.at<Vec3b>(row, col)[0] / 255.0f;

			// Lấy giá trị kênh màu xanh lá tại pixel thứ (row, col)
			float green = sourceImage.at<Vec3b>(row, col)[1] / 255.0f;

			// Lấy giá trị kênh màu xanh dương tại pixel thứ (row, col)
			float blue = sourceImage.at<Vec3b>(row, col)[2] / 255.0f;

			// Tính giá trị H - S - V
			// hue = vùng màu
			// saturation = độ bão hòa màu
			// value = giá trị hay độ sáng của màu sắc

			float hue, saturation, value; // h:0-360.0, s:0.0-1.0, v:0.0-1.0

			// Tìm giá trị max, min trong 3 kênh màu
			float max = fmaxf(fmaxf(red, green), blue);
			float min = fminf(fminf(red, green), blue);

			// Độ sáng của màu sắc sẽ bằng giá trị max của 3 kênh màu
			value = max;

			// Nếu value = 0 thì saturation = 0
			if (max == 0.0f) {
				saturation = 0;
				hue = 0;
			}
			else
				if (max - min == 0.0f) {
					saturation = 0;
					hue = 0;
				}
				else {
					saturation = (max - min) / max;
					if (max == red) {
						hue = 60 * ((green - blue) / (max - min)) + 0;
					}
					else
						if (max == green) {
							hue = 60 * ((blue - red) / (max - min)) + 120;
						}
						else {
							hue = 60 * ((red - green) / (max - min)) + 240;
						}
				}

			if (hue < 0) hue += 360.0f;
			destinationImage.at<Vec3b>(row, col)[0] = hue / 2;
			destinationImage.at<Vec3b>(row, col)[1] = saturation * 255;
			destinationImage.at<Vec3b>(row, col)[2] = value * 255;
		}
	}

	// Chuyển đổi ảnh thành công
	return 1;
};


/*
Hàm chuyển đổi không gian màu của ảnh từ HSV sang RGB
sourceImage: ảnh input
destinationImage: ảnh output
Hàm trả về
0: nếu chuyển thành công
1: nếu chuyển thất bại (không đọc được ảnh input,...)
*/
int Converter::HSV2RGB(const Mat& sourceImage, Mat& destinationImage) {
	// Duyệt qua tất cả các pixel của ảnh
	for (int row = 0; row < sourceImage.rows; row++) {
		for (int col = 0; col < sourceImage.cols; col++) {
			float hue = sourceImage.at<Vec3b>(row, col)[0] * 2.0f; // 0-360
			float saturation = sourceImage.at<Vec3b>(row, col)[1] / 255.0f; // 0.0-1.0
			float value = sourceImage.at<Vec3b>(row, col)[2] / 255.0f; // 0.0-1.0

			// Khởi tạo giá trị 3 kênh màu đỏ, xanh dương, xanh lá
			float red, green, blue; // 0.0-1.0

			// Tính giá trị màu theo công thức
			int   hi = (int)(hue / 60.0f) % 6;
			float f = (hue / 60.0f) - hi;
			float p = value * (1.0f - saturation);
			float q = value * (1.0f - saturation * f);
			float t = value * (1.0f - saturation * (1.0f - f));

			switch (hi) {
			case 0: red = value, green = t, blue = p; break;
			case 1: red = q, green = value, blue = p; break;
			case 2: red = p, green = value, blue = t; break;
			case 3: red = p, green = q, blue = value; break;
			case 4: red = t, green = p, blue = value; break;
			case 5: red = value, green = p, blue = q; break;
			}

			// Gán giá trị 3 kênh màu của ảnh đích 
			destinationImage.at<Vec3b>(row, col)[0] = red * 255;
			destinationImage.at<Vec3b>(row, col)[1] = green * 255;
			destinationImage.at<Vec3b>(row, col)[2] = blue * 255;
		}
	}

	// Chuyển đổi ảnh thành công
	return 1;

};

/*
Hàm chuyển đổi không gian màu của ảnh
sourceImage: ảnh input
destinationImage: ảnh output cùng kích thước, cùng loại với ảnh input
type: loại chuyển đổi
	0,1: chuyển từ RGB sang GrayScale và ngược lại
	2,3: chuyển từ RGB sang HSV và ngược lại
Hàm trả về
	0: nếu chuyển thành công
	1: nếu chuyển thất bại (không đọc được ảnh input hay type không chính xác,...)
*/
int Converter::Convert(Mat& sourceImage, Mat& destinationImage, int type) {
	switch (type) {
	case 0: return (RGB2GrayScale(sourceImage, destinationImage));
	case 1: return (GrayScale2RGB(sourceImage, destinationImage));
	case 2: return (RGB2HSV(sourceImage, destinationImage));
	case 3: return (HSV2RGB(sourceImage, destinationImage));
	}

	// Chuyển đổi ảnh thất bại
	return 0;
};




