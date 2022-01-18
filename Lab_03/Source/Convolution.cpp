#include "Convolution.h"

vector<float> Convolution::GetKernel()
{
	return _kernel;
}

void Convolution::SetKernel(vector<float> kernel, int kWidth, int kHeight)
{
	// nếu giá trị của Width hoặc Height là chẵn thì không thực hiện hàm
	if ((kWidth % 2 == 0) || (kHeight % 2 == 0))
	{
		return;
	}

	// tích 2 kích thước không bằng kích thước của kernel thì khôgn thực hiện hàm
	if (kernel.size() != float(kWidth) * float(kHeight))
	{
		return;
	}
	this->_kernel = kernel;
	this->_kernelWidth = kWidth;
	this->_kernelHeight = kHeight;

}

int Convolution::DoConvolution(const Mat& sourceImage, Mat& destinationImage)
{
	// kiểm trả data ảnh input
	if (!sourceImage.data)
	{
		return 1;
	}
	destinationImage = sourceImage.clone();

	// vị trí của tâm kernel
	int hKernel_center = this->_kernelHeight / 2;
	int wKernel_center = this->_kernelWidth / 2;

	// nếu ảnh xám
	if (sourceImage.channels() == 1)
	{
		// xét giá trị từ kernel_center -> chiều rộng - kernel_center
		// và từ kernel_center -> chiều cao - kernel_center
		// bỏ qua các pixel rìa của ảnh
		for (int j = hKernel_center; j < sourceImage.rows - hKernel_center; j++)
		{

			for (int i = wKernel_center; i < sourceImage.cols - wKernel_center; i++)
			{
				// biến lưu giá trị tích Convolution và biến đếm để xét các giá trị của kernel
				int sum = 0, kernel_idx = 0;

				for (int kernel_j = j - hKernel_center; kernel_j <= j + hKernel_center; kernel_j++)
				{
					for (int kernel_i = i - wKernel_center; kernel_i <= i + wKernel_center; kernel_i++)
					{
						// tính tích convolution với các giá trị của các pixel lân cận và kernel
						sum += (*sourceImage.ptr<uchar>(kernel_j, kernel_i)) * _kernel[kernel_idx];
						kernel_idx++;
					}
				}


				if (sum > 255)
				{
					sum = 255;
				}
				(*destinationImage.ptr<uchar>(j, i)) = sum;
			}
		}
	}
	// tương tự với 3 kênh màu
	else
	{
		for (int j = hKernel_center; j < sourceImage.rows - hKernel_center; j++)
		{

			for (int i = wKernel_center; i < sourceImage.cols - wKernel_center; i++)
			{
				int sum[3], kernel_idx = 0;
				sum[0] = 0, sum[1] = 0, sum[2] = 0;
				for (int kernel_j = j - hKernel_center; kernel_j <= j + hKernel_center; kernel_j++)
				{
					for (int kernel_i = i - wKernel_center; kernel_i <= i + wKernel_center; kernel_i++)
					{
						sum[0] += (*sourceImage.ptr<Vec3b>(kernel_j, kernel_i))[0] * _kernel[kernel_idx];
						sum[1] += (*sourceImage.ptr<Vec3b>(kernel_j, kernel_i))[1] * _kernel[kernel_idx];
						sum[2] += (*sourceImage.ptr<Vec3b>(kernel_j, kernel_i))[2] * _kernel[kernel_idx];
						kernel_idx++;
					}
				}

				for (int k = 0; k < 3; k++)
				{
					if (sum[k] > 255)
					{
						sum[k] = 255;
					}
					(*destinationImage.ptr<Vec3b>(j, i))[k] = sum[k];
				}
			}
		}
	}

	return 0;
}

Convolution::Convolution() {}

Convolution::~Convolution() {}