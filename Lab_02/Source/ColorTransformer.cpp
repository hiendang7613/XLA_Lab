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

// giới hạn giá trị của chiều cao
template<class T>
constexpr T clamp(T v, T lo, T hi)
{
	assert(!(hi < lo));
	return (v < lo) ? lo : (hi < v) ? hi : v;
}

int ColorTransformer::DrawHistogram(const Mat& histMatrix, Mat& histImage)
{
	int rows = histMatrix.rows;
	int cols = histMatrix.cols;

	// thuộc tính của cửa sổ
	const int height = 400;
	const int width = 1200;
	// khoảng cách giữa các điểm
	const float bin_width = static_cast<float>(width / 3) / static_cast<float>(cols); 

	// màu của các đường
	Scalar colours[] =
	{
		{255, 0, 0},
		{0, 255, 0},
		{0, 0, 255}
	};

	// nếu là ảnh xám thì cho đường màu đen 
	if (rows == 1)
	{
		colours[0] = Scalar(0, 0, 0);
	}

	// ảnh kết quả với nền trắng
	histImage = Mat(height, width, CV_8UC3, Scalar(255, 255, 255));


	for (int i = 0; i < rows; i++)
	{
		// xét từng dòng của matrix là hist của từng kênh màu
		const uchar* histogram = histMatrix.ptr<uchar>(i);
		Scalar colour = colours[i % 3];

		for (int j = 1; j < cols; j++)
		{
			// tính vị trí của 2 điểm liền kề
			int x1 = round(bin_width * (j - 1)) + 400 * i;
			int x2 = round(bin_width * (j - 0)) + 400 * i;

			int y1 = clamp(height - static_cast<int>(round(histogram[j - 1])), 3, height - 3);
			int y2 = clamp(height - static_cast<int>(round(histogram[j - 0])), 3, height - 3);

			// vẽ đường thẳng giữa 2 điểm đó
			line(histImage, cv::Point(x1, y1), cv::Point(x2, y2), colour, 1, LINE_AA);
		}
	}
	// nếu ảnh hist rỗng thì return 0
	if (!histImage.data)
	{
		return 0;
	}
	return 1;
}

int ColorTransformer::HistogramEqualization(const Mat& sourceImage, Mat& destinationImage) {
	// nếu ảnh gốc rỗng thì return 0
	if (!sourceImage.data)
	{
		return 0;
	} 
	// nếu là ảnh xám
	if (sourceImage.channels() == 1) {
		// tạo mảng rỗng để lưu hist
		int H[256];
		for (int i = 0; i < 256; i++) {
			H[i] = 0;
		}

		int sRow = sourceImage.rows;
		int sCols = sourceImage.cols;
		int sChanel = sourceImage.channels();
		
		// tính hist và lưu vào H
		for (int i = 0; i < sRow; i++) {
			const Vec3b* pRow = sourceImage.ptr<Vec3b>(i);
			for (int j = 0; j < sCols; j++) {
				H[(int)pRow[j][0]]++;
			}
		}
		// tạo mảng rỗng để lưu hist cân bằng
		int T[256];
		for (int i = 0; i < 256; i++) {
			T[i] = 0;
		}
		T[0] = H[0];
		// tính giá trị các phần tử của T
		for (int i = 1; i < 256; i++) {
			T[i] = T[i - 1] + H[i];
		}
		// chuẩn hóa giá trị
		uchar Tr[256];
		for (int i = 0; i < 256; i++) {
			Tr[i] = (uchar)round(((float)256 - 1) / (sCols * sRow) * (float)T[i]);
		}

		// lưu ảnh kết quả với giá trị hist đã cân bằng
		destinationImage = sourceImage.clone();
		for (int i = 0; i < sRow; i++) {
			Vec3b* pRow = destinationImage.ptr<Vec3b>(i);
			for (int j = 0; j < sCols; j++) {
				pRow[j][0] = (uchar)Tr[(int)pRow[j][0]];
			}
		}
	}
	// tương tự với 3 kênh màu
	if (sourceImage.channels() == 3) {
		int HR[256];
		int HG[256];
		int HB[256];
		for (int i = 0; i < 256; i++) {
			HR[i] = 0;
			HG[i] = 0;
			HB[i] = 0;
		}

		int sRow = sourceImage.rows;
		int sCols = sourceImage.cols;
		int sChanel = sourceImage.channels();

		for (int i = 0; i < sRow; i++) {
			const Vec3b* pRow = sourceImage.ptr<Vec3b>(i);
			for (int j = 0; j < sCols; j++) {
				HR[(int)pRow[j][2]]++;
				HG[(int)pRow[j][1]]++;
				HB[(int)pRow[j][0]]++;
			}
		}
		int TR[256];
		int TG[256];
		int TB[256];
		for (int i = 0; i < 256; i++) {
			TR[i] = 0;
			TG[i] = 0;
			TB[i] = 0;
		}
		TR[0] = HR[0];
		TG[0] = HG[0];
		TB[0] = HB[0];
		for (int i = 1; i < 256; i++) {
			TR[i] = TR[i - 1] + HR[i];
			TG[i] = TG[i - 1] + HG[i];
			TB[i] = TB[i - 1] + HB[i];
		}
		uchar TrR[256];
		uchar TrG[256];
		uchar TrB[256];
		for (int i = 0; i < 256; i++) {

			TrR[i] = (uchar)round(((float)256 - 1) / (sCols * sRow) * (float)TR[i]);
			TrG[i] = (uchar)round(((float)256 - 1) / (sCols * sRow) * (float)TG[i]);
			TrB[i] = (uchar)round(((float)256 - 1) / (sCols * sRow) * (float)TB[i]);
		}


		destinationImage = sourceImage.clone();
		for (int i = 0; i < sRow; i++) {
			Vec3b* pRow = destinationImage.ptr<Vec3b>(i);
			for (int j = 0; j < sCols; j++) {
				pRow[j][2] = (uchar)TrR[(int)pRow[j][2]];
				pRow[j][1] = (uchar)TrR[(int)pRow[j][1]];
				pRow[j][0] = (uchar)TrR[(int)pRow[j][0]];
			}
		}
	}
	return 1;
}

int ColorTransformer::HistogramCal(const Mat& sourceImage, int*& his)
{
	//Kiểm tra sự tồn tại
	if (!sourceImage.data)
	{
		return 0;
	}
	int nr = sourceImage.rows;
	int nc = sourceImage.cols * sourceImage.channels();

	//Đặt tất cả histogram từng giá trị (0->255) bằng 0
	for (int i = 0; i < 256; i++) {
		his[i] = 0;
	}

	//Duyệt điểm ảnh, his[i] = his[i] + 1 tương ứng
	for (int j = 0; j < nr; j++) {
		const uchar* dataSrc = sourceImage.ptr<uchar>(j);
		for (int i = 0; i < nc; i++) {
			his[(int)dataSrc[i]]++;
		}
	}

	return 1;
}

float ColorTransformer::CompareImage(const Mat& image1, Mat& image2) {
	//Kiểm tra sự tồn tại 2 ảnh đầu vào:
	if (!image1.data || !image2.data)
	{
		return 0;
	}

	//Tính histogram ảnh thứ nhất, trả về hist1
	int* hist1 = new int[256];
	HistogramCal(image1, hist1);

	//Tính histogram ảnh thứ hai, trả về hist2
	int* hist2 = new int[256];
	HistogramCal(image2, hist2);
	float result = 0;

	//So sánh sự chênh lệch của từng pixel
	for (int i = 0; i < 256; i++)
	{
		if (hist1[i] == hist2[i])
		{
			result++;
		}
	}
	result = result / 256;
	return result;
}