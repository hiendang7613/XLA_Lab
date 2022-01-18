#include "Blur.h"
#include <cmath>
#include "Convolution.h"

void InsertionSort(vector<uchar>& a)
{
	int i, key, j;
	for (i = 1; i < a.size(); i++)
	{
		key = a[i];
		j = i - 1;

		while (j >= 0 && a[j] > key)
		{
			a[j + 1] = a[j];
			j = j - 1;
		}
		a[j + 1] = key;
	}
}

int mean_blurring(const Mat& sourceImage, Mat& destinationImage, int kWidth, int kHeight)
{
	if (!sourceImage.data)
	{
		return 1;
	}
	Convolution kernel;
	vector<float> mean_value(kWidth * kHeight, float(1) / float(kWidth * kHeight));
	kernel.SetKernel(mean_value, kWidth, kHeight);
	return kernel.DoConvolution(sourceImage, destinationImage);
}

int Blur::BlurImage(const Mat& sourceImage, Mat& destinationImage, int kWidth, int kHeight, int method)
{

	switch (method)
	{

	case 0:
	{
		return mean_blurring(sourceImage, destinationImage, kWidth, kHeight);
	}
	case 1:
	{

		if (!sourceImage.data)
		{
			return 1;
		}
		if ((kWidth % 2 == 0) || (kHeight % 2 == 0))
		{
			return 1;
		}

		int nchannel = sourceImage.channels();

		// Tạo ảnh đích với kích cỡ cũ
		destinationImage.create(sourceImage.rows, sourceImage.cols, sourceImage.type());

		//Tạo bảng offsets trỏ tới các phần tử pixel cần tính trung vị 
		vector<int> offsets;
		for (int y = -kHeight / 2; y <= kHeight / 2; y++)
			for (int x = -kWidth / 2; x <= kWidth / 2; x++)
				offsets.push_back(y * sourceImage.step[0] + x * nchannel);

		// Matran trung vi
		vector<uchar> h;
		uchar* pDes, * pSrc;
		for (int i = kHeight / 2; i < sourceImage.rows - kHeight / 2; i++)
		{
			pDes = (uchar*)destinationImage.ptr<uchar>(i);
			pSrc = (uchar*)sourceImage.ptr<uchar>(i);
			for (int j = kWidth / 2 * nchannel; j < (sourceImage.cols - kWidth / 2) * nchannel; j++, pSrc += 1, pDes += 1)
			{
				// Tạo mảng cho matran trung vị
				for (int offset : offsets)
					h.push_back(pSrc[offset]);

				// Sắp xếp mảng
				InsertionSort(h);

				// Gán giá trị pixel với trung vị của mảng
				*pDes = h[h.size() / 2];
				h.clear();
			}
		}
		break;
	}
	case 2:
	{
		if (!sourceImage.data)
		{
			return 1;
		}

		Convolution conv;
		//Tạo kernel với trường hợp lọc Gaussian
		vector<float> kernel;

		float sigma = 1;
		float sigma2 = 2 * sigma * sigma;
		float k = sqrt(2 * PI) * sigma;

		float sum = 0;
		float h;

		//Tính kernel 
		for (int y = -kHeight / 2; y <= kHeight / 2; y++)
		{
			for (int x = -kWidth / 2; x <= kWidth / 2; x++)
			{
				h = expf(-(y * y + x * x) / sigma2) / k;
				kernel.push_back(h);
				sum += h;
			}
		}

		//Chuẩn hóa kernel 
		for (float& x : kernel) {
			x /= sum;
		}

		// Set kernel
		conv.SetKernel(kernel, kWidth, kHeight);
		// Tính tích chập với kernel trên
		return conv.DoConvolution(sourceImage, destinationImage);
	}
	default:
		cout << "wrong parameter!!\n";
		break;
	}
}

Blur::Blur() {}

Blur::~Blur() {}
