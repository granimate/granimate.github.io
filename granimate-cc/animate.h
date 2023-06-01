/*
	Wrapper library for manipulating images using opencv

	g++ main.cc -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_videoio -o out -I/usr/include/opencv4
*/

#ifndef ANIMATE_H_
#define ANIMATE_H_

#include "img-cc.h"
#include "offset_hue.h"

int create_vid(const int& frame_rate, const std::string& filename)
{
	return std::system(("ffmpeg -framerate " + std::to_string(frame_rate) + " -pattern_type glob -i 'uploads/" + filename + "-frames/*.png' -c:v libx264 -pix_fmt yuv420p public/mp4/" + filename + ".mp4").c_str());
}

int animate_point_ellipse_offset_hue_between(Img& base_img, 
                                            const double& r_center, 
                                            const double& c_center,
                                            const double& r_radius,
                                            const double& c_radius,
                                            const double& scale, 
                                            const double& s_hue, 
                                            const double& radius, 
                                            const int& no_frames, 
                                            const int& frame_rate, 
                                            const std::string& filename)
{
	double offset = 0.0,
	       inc = 360.0/no_frames;
	Vint file_no(count_digits(no_frames));

	for(double f=0; f<no_frames; ++f, offset += inc)
	{
		double point_row = r_center + r_radius*sin((f/no_frames)*2*M_PI),
		       point_col = c_center + c_radius*cos((f/no_frames)*2*M_PI);
		point_offset_hue_between(base_img, 
		                         point_row, 
		                         point_col, 
		                         scale, 
		                         s_hue, 
		                         radius, 
		                         offset).save("uploads/" + filename + "-frames/" + file_no.digits + ".png");
		++file_no;
	}


	return create_vid(frame_rate, filename);
}

int animate_point_ellipse_offset_hue(Img& base_img, 
                                    const double& r_center, 
                                    const double& c_center, 
                                    const double& r_radius,
                                    const double& c_radius,
                                    const double& scale,
                                    const int& no_frames, 
                                    const int& frame_rate, 
                                    const std::string& filename)
{
	return animate_point_ellipse_offset_hue_between(base_img, 
	                                               r_center, 
	                                               c_center,
	                                               r_radius,
	                                               c_radius, 
	                                               scale, 
	                                               180, 
	                                               200, 
	                                               no_frames, 
	                                               frame_rate, 
	                                               filename);
}

#endif //ANIMATE_H_