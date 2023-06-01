/*
	Wrapper library for manipulating images using opencv

	g++ main.cc -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_videoio -o out -I/usr/include/opencv4
*/

#ifndef IMG_CC_H_
#define IMG_CC_H_

#include <cmath>
#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include "filesystem.h"

typedef cv::Vec3b RGB;

struct RGB_Hash_Fn 
{
    size_t operator()(const RGB& rgb) const
    {
        return rgb[0] + 260*rgb[1] + 260*260*rgb[2];
    }
};

bool operator==(const RGB& rgb1, const RGB& rgb2)
{
	return (rgb1[0] == rgb2[0] && rgb1[1] == rgb2[1] && rgb1[2] == rgb2[2]);
}

bool operator!=(const RGB& rgb1, const RGB& rgb2)
{
	return !(rgb1 == rgb2);
}

bool operator<(const RGB& rgb1, const RGB& rgb2)
{
	if(rgb1[0] != rgb2[0])
		return rgb1[0] < rgb2[0];
	else if(rgb1[1] != rgb2[1])
		return rgb1[1] < rgb2[1];
	else
		return rgb1[2] < rgb2[2];
}

struct HSV
{
	double h, s, v;

	HSV()
	{
		h = s = v = 0.0;
	}

	HSV(const double& H, const double& S, const double& V)
	{
		h = H; s = S; v = V;
	}
};

bool operator==(const HSV& hsv1, const HSV& hsv2)
{
	return (hsv1.h == hsv2.h && hsv1.s == hsv2.s && hsv1.v == hsv2.v);
}

bool operator!=(const HSV& hsv1, const HSV& hsv2)
{
	return !(hsv1 == hsv2);
}

std::istream& operator>>(std::istream& is, HSV& hsv)
{
	is >> hsv.h >> hsv.s >> hsv.v;

	return is;
}

std::ostream& operator<<(std::ostream& os, const HSV& hsv)
{
	os << "[" << hsv.h << ", " << hsv.s << ", " << hsv.v << "]";

	return os;
}

double hue_dist(const double& h1, const double& h2)
{
	double h_min = std::min(h1, h2),
	       h_max = std::max(h1, h2);

	return std::min(h_max-h_min, std::abs(h_min+(360-h_max)));
}

double mod(const double& x, const int& m)
{
	if(x < 0)
		return x + std::ceil(-x/m)*m;
	else if(x >= m)
		return x - std::floor(x/m)*m;
	else
		return x;
}

HSV rgb2hsv(const RGB& rgb)
{
	HSV hsv;
    double rgbMin, rgbMax;

    rgbMin = std::min(rgb[0], std::min(rgb[1], rgb[2]));
    rgbMax = std::max(rgb[0], std::max(rgb[1], rgb[2]));

    hsv.v = rgbMax;
    if (hsv.v == 0)
    {
        hsv.h = 0;
        hsv.s = 0;
        return hsv;
    }

    hsv.s = 255*(rgbMax - rgbMin)/hsv.v;
    if (hsv.s == 0)
    {
        hsv.h = 0;
        return hsv;
    }

    if (rgbMax == rgb[2])
        hsv.h = 0 + 60*(rgb[1] - rgb[0])/(rgbMax - rgbMin);
    else if (rgbMax == rgb[1])
        hsv.h = 120 + 60*(rgb[0] - rgb[2])/(rgbMax - rgbMin);
    else
        hsv.h = 240 + 60*(rgb[2] - rgb[1])/(rgbMax - rgbMin);

    hsv.h = mod(hsv.h, 360);

    return hsv;
}

RGB hsv2rgb(const HSV& hsv)
{
	RGB rgb;
    int region, p, q, t;
    double h, s, v;
    double remainder;

    if(hsv.s == 0)
    {
        rgb[0] = hsv.v;
        rgb[1] = hsv.v;
        rgb[2] = hsv.v;

        return rgb;
    }

    // converting to 16 bit to prevent overflow
    h = hsv.h;
    s = hsv.s;
    v = hsv.v;

    region = h/60;
    remainder = (h - (region*60))*6;

    p = std::round((v*(255 - s)))/255;
    q = std::round((v*(255 - ((s*remainder)/360))))/255;
    t = std::round((v*(255 - ((s*(360-remainder))/360))))/255;

    switch(region)
    {
        case 0:
            rgb[2] = v;
            rgb[1] = t;
            rgb[0] = p;
            break;
        case 1:
            rgb[2] = q;
            rgb[1] = v;
            rgb[0] = p;
            break;
        case 2:
            rgb[2] = p;
            rgb[1] = v;
            rgb[0] = t;
            break;
        case 3:
            rgb[2] = p;
            rgb[1] = q;
            rgb[0] = v;
            break;
        case 4:
            rgb[2] = t;
            rgb[1] = p;
            rgb[0] = v;
            break;
        default:
            rgb[2] = v;
            rgb[1] = p;
            rgb[0] = q;
            break;
    }

    return rgb;
}

bool check_conversions()
{
	for(int r=0; r<256; r++)
	{
		for(int g=0; g<256; g++)
		{
			for(int b=0; b<256; b++)
			{
				RGB rgb(r, g, b);

				if(rgb != hsv2rgb(rgb2hsv(rgb)))
				{
					std::cout << "--- problem found ---" << std::endl;
					std::cout << "                  rgb: " << rgb << std::endl;
					std::cout << "              rgb2hsv: " << rgb2hsv(rgb) << std::endl;
					std::cout << "hsv2rgb(rgb2hsv(rgb)): " << hsv2rgb(rgb2hsv(rgb)) << std::endl;
					return 0;
				}
			}
		}
	}

	return 1;
}

int count_digits(const int& x)
{
	if(x == 0)
		return 1;
	else
		return int(log10(x) + 1);
}

struct Img
{
	std::string path;
	cv::Mat img;
	int rows, cols;

	Img(){}

	Img(const std::string& Path)
	{
		path = Path;
		img = cv::imread(path.c_str(), cv::IMREAD_COLOR);
		rows = img.rows;
		cols = img.cols;
	}

	Img(const int& Rows, const int& Cols, const RGB& rgb)
	{
		path = "--unsaved--";
		rows = Rows;
		cols = Cols;
		img = cv::Mat(rows, cols, CV_8UC3, cv::Scalar(rgb[0], rgb[1], rgb[2]));

	}

	Img(const int& Rows, const int& Cols)
	{
		path = "--unsaved--";
		rows = Rows;
		cols = Cols;
		//*this = Img(rows, cols, RGB(0, 0, 0));
		img = cv::Mat(rows, cols, CV_8UC3, cv::Scalar(0, 0, 0));
	}

	void open(const std::string& Path)
	{
		path = Path;
		img = cv::imread(path.c_str(), cv::IMREAD_COLOR);
		rows = img.rows;
		cols = img.cols;
	}

	int save()
	{
		if(path == "--unsaved--")
		{
			std::cout << "cannot save created image without a specified path" << std::endl;
			return 1;
		}

		cv::imwrite(path.c_str(), img);

		return 0;
	}

	void save(const std::string& Path)
	{
		cv::imwrite(Path.c_str(), img);
	}

	void show(const std::string& title)
	{
		cv::namedWindow(title.c_str(), cv::WINDOW_AUTOSIZE);
		cv::imshow(title.c_str(), img);
		cv::waitKey(0);
	}

	void show()
	{
		if(path == "--unsaved--")
			show("untitled");
		else
			show(path);
	}

	Img resize(const double& Rows, const double& Cols)
	{
		Img img(Rows, Cols);

		for(double r=0; r<img.rows; ++r)
		{
			for(double c=0; c<img.cols; ++c)
			{
				img.at(r,c) = at(r*rows/double(img.rows), c*cols/double(img.cols));
			}
		}

		return img;
	}

	Img resize(const double& percent)
	{
		return resize(percent*rows, percent*cols);
	}

	Img flip_horizontally()
	{
		Img flipped(rows, cols);

		for(int r=0; r<rows; r++)
			for(int c=0; c<cols; c++)
				flipped.at(r,c) = at(r, cols-1-c);

		return flipped;
	}

	Img flip_vertically()
	{
		Img flipped(rows, cols);

		for(int r=0; r<rows; r++)
			for(int c=0; c<cols; c++)
				flipped.at(r,c) = at(rows-1-r, c);

		return flipped;
	}

	cv::Vec3b& at(const int& r, const int& c)
	{
		return img.at<cv::Vec3b>(r,c);
	}

	void add_at_tl_rhino(Img& sub_img, const int& r_start, const int& c_start, const RGB& rgb_ignore)
	{
		RGB rgb_sub, rgb;

		int sub_r = 0, r=r_start;
		if(r < 0)
		{
			sub_r -= r;
			r = 0;
		}

		for(; sub_r < sub_img.rows && r < rows; ++sub_r, ++r)
		{
			int sub_c=0, c=c_start;
			if(c < 0)
			{
				sub_c -= c;
				c = 0;
			}
		
			for(; sub_c < sub_img.cols && c < cols; ++sub_c, ++c)
			{
				rgb = at(r, c);
				rgb_sub = sub_img.at(sub_r, sub_c);

				//std::cout << rgb << std::endl;

				//if((rgb == (0,0,0) || rgb != rgb_ignore) && rgb_sub != rgb_ignore)
				if(rgb_sub != rgb_ignore)
				{
					//rgb[0] *= 0.8;
					//rgb[1] *= 0.8;
					//rgb[2] *= 0.8;
					HSV hsv = rgb2hsv(rgb);
					hsv.s *= 0.666;
					at(r, c) = hsv2rgb(hsv);
					//at(r, c) = rgb_sub;
				}
			}
		}
	}

	void add_at_tl(Img& sub_img, const int& r_start, const int& c_start, const RGB& rgb_ignore)
	{
		RGB rgb_sub, rgb;

		int sub_r = 0, r=r_start;
		if(r < 0)
		{
			sub_r -= r;
			r = 0;
		}

		for(; sub_r < sub_img.rows && r < rows; ++sub_r, ++r)
		{
			int sub_c=0, c=c_start;
			if(c < 0)
			{
				sub_c -= c;
				c = 0;
			}
		
			for(; sub_c < sub_img.cols && c < cols; ++sub_c, ++c)
			{
				rgb = at(r, c);
				rgb_sub = sub_img.at(sub_r, sub_c);

				//if((rgb == (0,0,0) || rgb != rgb_ignore) && rgb_sub != rgb_ignore)
				if(rgb_sub != rgb_ignore)
				{
					at(r, c) = rgb_sub;
				}
			}
		}
	}

	void add_at_tl2(Img& sub_img, const int& r_start, const int& c_start, const RGB& rgb_ignore)
	{
		RGB rgb_sub, rgb;

		int sub_r = 0, r=r_start;
		if(r < 0)
		{
			sub_r -= r;
			r = 0;
		}

		for(; sub_r < sub_img.rows && r < rows; ++sub_r, ++r)
		{
			int sub_c=0, c=c_start;
			if(c < 0)
			{
				sub_c -= c;
				c = 0;
			}
		
			for(; sub_c < sub_img.cols && c < cols; ++sub_c, ++c)
			{
				rgb = at(r, c);
				rgb_sub = sub_img.at(sub_r, sub_c);

				//if((rgb == (0,0,0) || rgb != rgb_ignore) && rgb_sub != rgb_ignore)
				if(rgb == rgb_ignore && rgb_sub != rgb_ignore)
				{
					at(r, c) = rgb_sub;
				}
			}
		}
	}

	void add_at_tm(Img& sub_img, const int& r_start, const int& c_start, const RGB& rgb_ignore)
	{
		add_at_tl(sub_img, r_start, c_start - sub_img.cols/2, rgb_ignore);
	}

	void add_at_tr(Img& sub_img, const int& r_start, const int& c_start, const RGB& rgb_ignore)
	{
		add_at_tl(sub_img, r_start, c_start - sub_img.cols + 1, rgb_ignore);
	}

	void add_at_bl(Img& sub_img, const int& r_start, const int& c_start, const RGB& rgb_ignore)
	{
		add_at_tl(sub_img, r_start-sub_img.rows+1, c_start, rgb_ignore);
	}

	void add_at_bm(Img& sub_img, const int& r_start, const int& c_start, const RGB& rgb_ignore)
	{
		add_at_tl(sub_img, r_start-sub_img.rows+1, c_start - sub_img.cols/2, rgb_ignore);
	}

	void add_at_bm2(Img& sub_img, const int& r_start, const int& c_start, const RGB& rgb_ignore)
	{
		add_at_tl2(sub_img, r_start-sub_img.rows+1, c_start - sub_img.cols/2, rgb_ignore);
	}

	void add_at_br(Img& sub_img, const int& r_start, const int& c_start, const RGB& rgb_ignore)
	{
		add_at_tl(sub_img, r_start-sub_img.rows+1, c_start - sub_img.cols + 1, rgb_ignore);
	}

	void add_at_mm(Img& sub_img, const int& r_start, const int& c_start, const RGB& rgb_ignore)
	{
		add_at_tl(sub_img, r_start - sub_img.rows/2, c_start - sub_img.cols/2, rgb_ignore);
	}
};

Img concat_horiz(std::vector<Img>& imgs)
{
	/*int rows = 0, cols = 0;

	for(size_t i=0; i<imgs.size(); ++i)
	{
		rows = std::max(rows, imgs[i].rows);
		cols += imgs[i].cols;
	}

	Img out(rows, cols);

	int c_start = 0;
	for(size_t i=0; i<imgs.size(); c_start += imgs[i++].cols)
	{
		for(int r=0; r<imgs[i].rows; ++r)
		{
			for(int c=c_start; )
		}
	}*/

	return Img(2,2);
}

Img concat_vert(std::vector<Img>& imgs)
{
	return Img(2,2);
}

struct Vint
{
	std::string digits;

	Vint(const int& no_digits)
	{
		digits = std::string(no_digits, '0');
	}

	Vint(const std::string& Digits)
	{
		digits = Digits;
	}

	std::string to_str()
	{
		std::string s;

		for(size_t i=0; i<digits.size(); ++i)
			s += std::to_string(i);

		return s;
	}

	Vint& operator++()
	{
		for(size_t i=digits.size()-1; i>=0; --i)
		{
			if(digits[i] == '9')
				digits[i] = '0';
			else
			{
				++digits[i];
				return *this;
			}
		}
	}

	Vint& operator--()
	{
		for(size_t i=digits.size()-1; i>=0; --i)
		{
			if(digits[i] == '0')
				digits[i] = '9';
			else
			{
				--digits[i];
				return *this;
			}
		}
	}
};

std::ostream& operator<<(std::ostream& os, const Vint& v)
{
	for(size_t i=0; i<v.digits.size(); ++i)
		os << v.digits[i];

	return os;
}

struct Point
{
	int r, c;
	double scale;

	Point(const int& R, const int& C)
	{
		r = R;
		c = C;
		scale = 1.0;
	}

	Point(const int& R, const int& C, const double& Scale)
	{
		r = R; c = C; scale = Scale;
	}
};

double dist(const Point& pt1, const Point& pt2)
{
	return std::sqrt(pow(pt1.r-pt2.r, 2) + pow(pt1.c-pt2.c, 2));
}

double dist(const Point& pt, const std::vector<Point>& pts)
{
	if(!pts.size())
		return -1;

	double min_dist = 99999999999.0;

	for(size_t i=0; i<pts.size(); ++i)
		min_dist = std::min(min_dist, sqrt(pow(pt.r - pts[i].r, 2) + pow(pt.c - pts[i].c, 2))/pts[0].scale);

	return min_dist;
}

#endif //IMG_CC_H_