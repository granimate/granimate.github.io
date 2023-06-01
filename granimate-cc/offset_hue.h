/*
	Wrapper library for manipulating images using opencv

	g++ main.cc -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_videoio -o out -I/usr/include/opencv4
*/

#ifndef OFFSET_HUE_H_
#define OFFSET_HUE_H_

#include <unordered_map>

#include "img-cc.h"

Img change_hue(Img& base_img, 
               const double& s_hue, 
               const double& radius, 
               const double& t_hue)
{
	Img img(base_img.rows, base_img.cols);
	RGB rgb;
	HSV hsv;

	for(int r=0; r<img.rows; ++r)
	{
		for(int c=0; c<img.cols; ++c)
		{
			rgb = base_img.at(r,c);
			hsv = rgb2hsv(rgb);

			if(hue_dist(s_hue, hsv.h) < radius)
			{
				hsv.h = mod(t_hue + (hsv.h-s_hue), 360);
				img.at(r,c) = hsv2rgb(hsv);
			}
			else
				img.at(r,c) = rgb;
		}
	}

	return img;
}

Img change_hue(Img& base_img, 
               const RGB& rgb_ignore,
               const double& s_hue, 
               const double& radius, 
               const double& t_hue)
{
	Img img(base_img.rows, base_img.cols);
	RGB rgb;
	HSV hsv;

	for(int r=0; r<img.rows; ++r)
	{
		for(int c=0; c<img.cols; ++c)
		{
			rgb = base_img.at(r,c);

			if(rgb != rgb_ignore &&
				rgb != RGB(140, 224, 253))
			{
				hsv = rgb2hsv(rgb);

				if(hue_dist(s_hue, hsv.h) < radius)
				{
					hsv.h = mod(t_hue + (hsv.h-s_hue), 360);
					img.at(r,c) = hsv2rgb(hsv);
				}
				else
					img.at(r,c) = rgb;
			}
			else
				img.at(r,c) = rgb;
		}
	}

	return img;
}

Img change_colours(Img& base_img, 
                   const RGB& rgb_ignore,
                   std::unordered_map<RGB, RGB, RGB_Hash_Fn> col_map)
{
	Img img(base_img.rows, base_img.cols);
	RGB rgb, rgb_zero(0,0,0);

	for(int r=0; r<img.rows; ++r)
	{
		for(int c=0; c<img.cols; ++c)
		{
			rgb = base_img.at(r,c);

			if(rgb != rgb_ignore &&
			   col_map.count(rgb))
				img.at(r,c) = col_map[rgb];
			else
				img.at(r,c) = rgb;
		}
	}

	return img;
}

Img offset_hue_between(Img& base_img, 
                       const double& s_hue, 
                       const double& radius, 
                       const double& offset)
{
	Img img(base_img.rows, base_img.cols);
	RGB rgb;
	HSV hsv;

	for(int r=0; r<img.rows; ++r)
	{
		for(int c=0; c<img.cols; ++c)
		{
			rgb = base_img.at(r,c);
			hsv = rgb2hsv(rgb);
			if(hue_dist(s_hue, hsv.h) < radius)
			{
				hsv.h = mod(hsv.h + offset, 360);
				img.at(r,c) = hsv2rgb(hsv);
			}
			else
				img.at(r,c) = base_img.at(r,c);
		}
	}

	return img;
}

Img offset_hue(Img& base_img, 
               const double& offset)
{
	return offset_hue_between(base_img, 180, 200, offset);
}

Img point_offset_hue_between(Img& base_img, 
                             Img& orig_img,
                             Img& foreground_img,
                             const double& point_row, 
                             const double& point_col, 
                             const double& scale,
                             const double& s_hue, 
                             const double& radius, 
                             const double& offset)
{
	Img img(base_img.rows, base_img.cols);
	RGB rgb, rgb_orig, rgb_foreground;
	HSV hsv;

	for(int r=0; r<img.rows; ++r)
	{
		for(int c=0; c<img.cols; ++c)
		{
			rgb = base_img.at(r, c);
			rgb_orig = orig_img.at(r, c);
			rgb_foreground = foreground_img.at(r, c);
			hsv = rgb2hsv(rgb);

			if(rgb_foreground != foreground_img.at(1,1))
				img.at(r,c) = orig_img.at(r,c);
			else if(rgb == base_img.at(1,1))
				img.at(r,c) = orig_img.at(r,c);
			else if(hue_dist(s_hue, hsv.h) < radius)
			{
				hsv.h = mod(hsv.h + offset + sqrt(pow(r-point_row, 2) + pow(c-point_col, 2))/scale, 360);
				
				img.at(r,c) = hsv2rgb(hsv);
			}
			else
				img.at(r,c) = orig_img.at(r,c);
		}
	}

	return img;
}

Img point_offset_hue(Img& base_img, 
                     Img& orig_img,
                     Img& foreground_img,
                     const double& point_row, 
                     const double& point_col, 
                     const double& scale,
                     const double& offset)
{
	return point_offset_hue_between(base_img, orig_img, foreground_img, point_row, point_col, scale, 180, 200, offset);
}

//Img mohawk("./png/fire.png");

Img point_offset_hue_between(Img& base_img, 
                             const double& point_row, 
                             const double& point_col, 
                             const double& scale,
                             const double& s_hue, 
                             const double& radius, 
                             const double& offset)
{
	Img img(base_img.rows, base_img.cols);
	RGB rgb;
	HSV hsv;

	for(int r=0; r<img.rows; ++r)
	{
		for(int c=0; c<img.cols; ++c)
		{
			rgb = base_img.at(r, c);

			if(rgb == RGB(0,0,0))
			{
				img.at(r,c) = RGB(0,0,0);
				continue;
			}

			hsv = rgb2hsv(rgb);

			if(rgb != RGB(0,0,0) && hue_dist(s_hue, hsv.h) < radius)
			{
				hsv.h = mod(hsv.h + offset + sqrt(pow(r-point_row, 2) + pow(c-point_col, 2))/scale, 360);
				img.at(r,c) = hsv2rgb(hsv);
			}
			else
				img.at(r,c) = base_img.at(r,c);
		}
	}

	return img;
}

Img point_offset_hue(Img& base_img, 
                     const double& point_row, 
                     const double& point_col, 
                     const double& scale,
                     const double& offset)
{
	return point_offset_hue_between(base_img, point_row, point_col, scale, 180, 200, offset);
}

RGB rgb_zero(0,0,0);

Img points_offset_hue_between(Img& base_img, 
                              Img& orig_img,
                              Img& foreground_img,
                              const std::vector<Point>& points,
                              const double& s_hue, 
                              const double& radius, 
                              const double& offset)
{
	Img img(base_img.rows, base_img.cols);
	RGB rgb, rgb_orig, rgb_foreground;
	HSV hsv;

	for(int r=0; r<img.rows; ++r)
	{
		for(int c=0; c<img.cols; ++c)
		{
			rgb = base_img.at(r,c);

			if(rgb == rgb_zero)
			{
				img.at(r,c) = orig_img.at(r,c);
				continue;
			}
			
			rgb_orig = orig_img.at(r,c);
			rgb_foreground = foreground_img.at(r,c);
			hsv = rgb2hsv(rgb);

			if(rgb_foreground != foreground_img.at(1,1))
				img.at(r,c) = base_img.at(r,c);
			else if(rgb == base_img.at(1,1))
				img.at(r,c) = orig_img.at(r,c);
			else if(hue_dist(s_hue, hsv.h) < radius)
			{
				hsv.h = mod(hsv.h + offset + dist(Point(r, c), points), 360);
				img.at(r,c) = hsv2rgb(hsv);
			}
			else
				img.at(r,c) = orig_img.at(r,c);
		}
	}

	return img;
}

Img points_offset_hue(Img& base_img, 
                      Img& orig_img,
                      Img& foreground_img,
                      const std::vector<Point>& points,
                      const double& offset)
{
	return points_offset_hue_between(base_img, orig_img, foreground_img, points, 180, 200, offset);
}

Img points_offset_hue_between(Img& base_img, 
                             const std::vector<Point>& points,
                             const double& s_hue, 
                             const double& radius, 
                             const double& offset)
{
	Img img(base_img.rows, base_img.cols);
	RGB rgb;
	HSV hsv;

	for(int r=0; r<img.rows; ++r)
	{
		for(int c=0; c<img.cols; ++c)
		{
			RGB rgb = base_img.at(r,c);
			hsv = rgb2hsv(rgb);
			if(hue_dist(s_hue, hsv.h) < radius)
			{
				hsv.h = mod(hsv.h + offset + dist(Point(r, c), points), 360);
				img.at(r,c) = hsv2rgb(hsv);
			}
			else
				img.at(r,c) = base_img.at(r,c);
		}
	}

	return img;
}

Img points_offset_hue(Img& base_img, 
                     const std::vector<Point>& points,
                     const double& offset)
{
	return points_offset_hue_between(base_img, points, 180, 200, offset);
}


#endif //OFFSET_HUE_H_
