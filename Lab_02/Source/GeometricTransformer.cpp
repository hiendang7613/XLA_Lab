#include "GeometricTransformer.h"
using namespace std;
# define PI 3.14159265358979323846

//NearestNeighborInterpolate
uchar NearestNeighborInterpolate::Interpolate(float tx, float ty, uchar* pSrc, int srcWidthStep, int nChannels) {

	return pSrc[(int)round(ty) * srcWidthStep + (int)round(tx) * 3 + nChannels];
}
NearestNeighborInterpolate::NearestNeighborInterpolate() {
}
NearestNeighborInterpolate::~NearestNeighborInterpolate() {
}
//BilinearInterpolate
uchar BilinearInterpolate::Interpolate(float tx, float ty, uchar* pSrc, int srcWidthStep, int nChannels) {
	int l = (int)round(tx), a = l - tx;
	int k = (int)round(ty), b = ty - k;
	return (uchar)round(((1 - a) * (1 - b) * pSrc[k * srcWidthStep + l * 3 + nChannels] + a * (1 - b) * pSrc[k * srcWidthStep + (l + 1) * 3 + nChannels]
		+ b * (1 - a) * pSrc[(k + 1) * srcWidthStep + l * 3 + nChannels] + a * b * pSrc[(k + 1) * srcWidthStep + (l + 1) * 3 + nChannels]));
}
BilinearInterpolate::BilinearInterpolate() {
}
BilinearInterpolate::~BilinearInterpolate() {
}
//PixelInterpolate
PixelInterpolate::PixelInterpolate() {
}
PixelInterpolate::~PixelInterpolate() {
}

//AffineTransform
AffineTransform::AffineTransform() {
	_matrixTransform = (Mat_<float>(3, 3) << 1, 0, 0, 0, 1, 0, 0, 0, 1);  // ma trận I
}
void AffineTransform::Translate(float dx, float dy) {
	Mat NextTransform = (Mat_<float>(3, 3) << 1, 0, dx, 0, 1, dy, 0, 0, 1); 
	_matrixTransform = _matrixTransform * NextTransform;
}
void AffineTransform::Rotate(float angle) {
	Mat NextTransform = (Mat_<float>(3, 3) << cos(angle * PI / 180), -sin(angle * PI / 180), 0, sin(angle * PI / 180), cos(angle * PI / 180), 0, 0, 0, 1);
	_matrixTransform = _matrixTransform * NextTransform;
}
void AffineTransform::Scale(float sx, float sy) {
	Mat NextTransform = (Mat_<float>(3, 3) << sx, 0, 0, 0, sy, 0, 0, 0, 1);
	_matrixTransform = _matrixTransform * NextTransform;
}
void AffineTransform::TransformPoint(float& x, float& y) {
	Mat invMatrix = _matrixTransform.inv();
	x = (invMatrix.ptr<float>(0)[0] * x + invMatrix.ptr<float>(0)[1] * y + invMatrix.ptr<float>(0)[2]);
	y = (invMatrix.ptr<float>(1)[0] * x + invMatrix.ptr<float>(1)[1] * y + invMatrix.ptr<float>(1)[2]);
}
AffineTransform::~AffineTransform() {
	_matrixTransform.~Mat();
}

//GeometricTransformer
GeometricTransformer::GeometricTransformer() {
}
GeometricTransformer::~GeometricTransformer() {
}
int GeometricTransformer::Transform(const Mat& srcImage, Mat& dstImage, AffineTransform* transformer, PixelInterpolate* interpolator)
{
	// Kiem tra anh co noi dung! 
	if (!srcImage.data)
		return 0;
	int srcWidth = srcImage.cols;
	int srcHeight = srcImage.rows;
	int nChanels = srcImage.channels();
	uchar* pSrc = srcImage.data;
	int srcWidthStep = srcImage.cols * srcImage.channels();

	// Duyet qua tung pixel cua dstImage duoc noi suy tu srcImage
	for (int j = 0; j < dstImage.rows; j++) {
		Vec3b* data = dstImage.ptr<Vec3b>(j);
		for (int i = 0; i < dstImage.cols; i++) {

			float x = (float)i;
			float y = (float)j;

			// tao ma tran Affine cho vector(x,y)
			transformer->TransformPoint(x, y);

			// noi suy tu srcImage
			if ((int)round(y) * srcWidthStep + (int)round(x) * nChanels < 0 ||
				((int)round(y) * srcWidthStep + (int)round(x) * nChanels + 2) >(srcHeight * srcWidthStep)
				|| x < 0 || y < 0 || x > srcWidth || y > srcHeight)
			{
				data[i][0] = 0;
				data[i][1] = 0;
				data[i][2] = 0;
			}
			else {
				data[i][0] = interpolator->Interpolate(x, y, pSrc, srcWidthStep, 0);
				data[i][1] = interpolator->Interpolate(x, y, pSrc, srcWidthStep, 1);
				data[i][2] = interpolator->Interpolate(x, y, pSrc, srcWidthStep, 2);
			}
		}
	}
	return 1;
}


/*
Hàm phóng to, thu nhỏ ảnh theo tỉ lệ cho trước
Tham số
- srcImage: ảnh input
- dstImage: ảnh sau khi thực hiện phép xoay
- sx, sy: tỉ lệ phóng to, thu nhỏ ảnh
- interpolator: biến chỉ định phương pháp nội suy màu
Trả về:
 - 0: Nếu ảnh input ko tồn tại hay ko thực hiện được phép biến đổi
 - 1: Nếu biến đổi thành công
*/
int GeometricTransformer::Scale(const Mat& srcImage, Mat& dstImage, float sx, float sy, PixelInterpolate* interpolator)
{
	// Kiem tra anh co noi dung! 
	if (!srcImage.data)
		return 0;

	int srcWidth = srcImage.cols;
	int srcHeight = srcImage.rows;
	int srcType = srcImage.type();

	// tao dstImage voi kich thuoc ti le (sy,sx);
	dstImage = Mat((int)round(srcHeight * sy), (int)round(srcWidth * sx), srcType);

	// tao matran AffineTransform
	AffineTransform* scale = new AffineTransform();
	scale->Scale(sx, sy);

	return Transform(srcImage, dstImage, scale, interpolator);
}

int GeometricTransformer::Resize(const Mat& srcImage, Mat& dstImage, int newWidth, int newHeight, PixelInterpolate* interpolator)
{
	// xét ảnh rỗng 
	if (!srcImage.data)
	{
		return 0;
	}
	// thuộc tính ảnh gốc
	int srcWidth = srcImage.cols;
	int srcHeight = srcImage.rows;
	int nChanels = srcImage.channels();
	int srcType = srcImage.type();
	// tỉ lệ scale ảnh
	float sx = (float)newWidth / (float)srcWidth;
	float sy = (float)newHeight / (float)srcHeight;
	// tạo ảnh mới với thuộc tính mới
	dstImage = Mat((int)round(srcHeight * sy), (int)round(srcWidth * sx), srcType);
	// tạo AffineTransform
	AffineTransform* scale = new AffineTransform();
	scale->Scale(sx, sy);

	return Transform(srcImage, dstImage, scale, interpolator);
}

int GeometricTransformer::RotateKeepImage(const Mat& srcImage, Mat& dstImage, float angle, PixelInterpolate* interpolator)
{
	// xét ảnh rỗng
	if (!srcImage.data)
	{
		return 0;
	}
	// thuộc tính ảnh gốc
	int srcWidth = srcImage.cols;
	int srcHeight = srcImage.rows;
	int nChanels = srcImage.channels();
	int srcType = srcImage.type();
	uchar* pSrc = srcImage.data;
	int srcWidthStep = srcWidth * nChanels;


	//Chiều rộng mới của ảnh = chiều rộng cũ *cos(angle) + chiều cao cũ* (sin anle)
	int newW = srcImage.rows * abs(cos(angle * PI / 180.0)) + srcImage.cols * abs(sin(angle * PI / 180.0));
	int newH = srcImage.cols * abs(cos(angle * PI / 180.0)) + srcImage.rows * abs(sin(angle * PI / 180.0));

	// tạo ảnh mới
	dstImage = Mat(newH, newW, srcImage.type());

	// tạo AffineTransform
	AffineTransform rotate;

	// dịch chuyển tâm từ ảnh cũ về tâm theo ảnh mới
	rotate.Translate(newW / 2, newH / 2);
	rotate.Rotate(angle);
	rotate.Translate(-(srcImage.cols / 2), -(srcImage.rows / 2));

	// tính giá trị thuộc tính ảnh kết quả
	return Transform(srcImage, dstImage, &rotate, interpolator);
}

int GeometricTransformer::RotateUnkeepImage(const Mat& srcImage, Mat& dstImage, float angle, PixelInterpolate* interpolator)
{
	if (!srcImage.data)
	{
		return 0;
	}

	int srcWidth = srcImage.cols, srcHeight = srcImage.rows, nChanels = srcImage.channels(), srcType = srcImage.type();
	uchar* pSrc = srcImage.data;
	int srcWidthStep = srcWidth * nChanels;

	dstImage = Mat(srcImage.rows, srcImage.cols, srcType);

	AffineTransform rotate = AffineTransform();

	rotate.Translate(srcImage.cols / 2, srcImage.rows / 2);
	rotate.Rotate(angle);
	rotate.Translate(-(srcImage.cols / 2), -(srcImage.rows / 2));

	return Transform(srcImage, dstImage, &rotate, interpolator);

}

int GeometricTransformer::Flip(Mat& srcImage, Mat& dstImage, bool direction, PixelInterpolate* interpolator)
{
	// xét ảnh rỗng
	if (!srcImage.data)
	{
		return 0;
	}
	// thuộc tính ảnh gốc
	int srcWidth = srcImage.cols;
	int srcHeight = srcImage.rows;
	int nChanels = srcImage.channels();
	int srcType = srcImage.type();
	// tạo ảnh kết quả
	dstImage = Mat((int)round(srcHeight), (int)round(srcWidth), srcType);

	//tạo AffineTransform
	AffineTransform* scale = new AffineTransform();
	if (direction == 0)
	{
		// nếu flip theo chiều dọc, dịch chuyển ảnh đến rìa phải và lật lại
		scale->Translate(srcWidth, 0);
		scale->Scale(-1, 1);
	}
	else
	{
		// nếu flip theo chiều ngang, dịch chuyển ảnh đến rìa dưới và lật lại
		scale->Translate(0, srcHeight);
		scale->Scale(1, -1);
	}

	return Transform(srcImage, dstImage, scale, interpolator);
}