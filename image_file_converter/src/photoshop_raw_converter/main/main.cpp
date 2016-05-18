#include <iostream>
#include <fstream>
#include <vector>

#include <gflags/gflags.h>

#include "praw/praw.hpp"

DEFINE_string(in,     "",       "* raw image");
DEFINE_string(out,     "",       "* tif image");
DEFINE_int64(w, 0,      "image width");
DEFINE_int64(h, 0,      "image height");
DEFINE_int64(c, 0,      "image channel");
DEFINE_bool(lzw, false,      "LZW compress");

void main(int argc, char ** argv)
{
	google::ParseCommandLineFlags(&argc, &argv, true);

	do 
	{
		if (FLAGS_in.empty() || FLAGS_out.empty())
		{
			std::cout<<"error: invalid input\/output parameters."<<std::endl;
			std::cout<<FLAGS_in<<" "<<FLAGS_out<<std::endl;
			break;
		}

		if (FLAGS_w <= 0 || FLAGS_h <= 0 || FLAGS_c <= 0)
		{
			std::cout<<"error: invalid image parameters"<<std::endl;
			std::cout<< FLAGS_w <<" " << FLAGS_h <<" " << FLAGS_c << std::endl;
			break;
		}

		praw::PhotoshopRawReader raw_reader(FLAGS_in, FLAGS_w, FLAGS_h, FLAGS_c);
		if (raw_reader.ToTiff(FLAGS_out, FLAGS_lzw))
		{
			std::cout<<"error: converting failed."<<std::endl;
			break;
		}

	} while (0);

	google::ShutDownCommandLineFlags();

	std::cout<<"photoshop_raw_converter."<<std::endl;
}