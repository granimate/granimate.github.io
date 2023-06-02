#include "animate.h"

int main(int argc, const char* argv[])
{
	if(argc != 8) {
		std::cout << "error: granimate-cc: expected 7 parameters, got " << argc << std::endl;
		return 1;
	}

	std::string img_filename = argv[1],
		img_path = std::string("uploads/") + img_filename;

	int r_center  = std::atoi(argv[2]),
		r_radius  = std::atoi(argv[3]),
		c_center  = std::atoi(argv[4]),
		c_radius  = std::atoi(argv[5]),
		scale     = std::atoi(argv[6]),
		no_frames = std::atoi(argv[7]);

	std::system((std::string("convert ") + img_path + " " + img_path + ".png").c_str());

	Img img(img_path + ".png");

    int rows = int(double(img.rows)/double(img.cols)*1024.0);
	if(rows%2)
		++rows;

	std::system((std::string("convert -resize 1024x") + std::to_string(rows) + std::string("! ") + img_path + ".png " + img_path + ".png").c_str());
	if(!dir_exists(img_path + "-frames"))
		std::system((std::string("mkdir ") + img_path + "-frames").c_str());
	else {
		std::system((std::string("rm ./") + img_path + std::string("-frames/*.png")).c_str());
		//std::system((std::string("rm ./") + img_path + std::string(".mp4")).c_str());
	}
	Img resized_img(img_path + ".png");

	animate_point_ellipse_offset_hue(resized_img,
		                             r_center, //r-center
		                             c_center,//c-center
		                             r_radius, //r-radius
		                             c_radius, //c-radius
		                             scale,   //scale
		                             no_frames,  //no frames
		                             10,  //fps
		                             std::string(img_filename));

	std::system((std::string("rm ./") + img_path + std::string("-frames/*.png")).c_str());
	std::system((std::string("rmdir ") + img_path + "-frames").c_str());
	std::system((std::string("rm ") + img_path).c_str());
	std::system((std::string("rm ") + img_path + ".png").c_str());

	return 0;
}
