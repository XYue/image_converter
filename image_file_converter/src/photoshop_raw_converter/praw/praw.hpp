#pragma once

#include <string>

namespace praw
{
	class PhotoshopRawReader
	{
	public:
		PhotoshopRawReader();
		PhotoshopRawReader(std::string raw_filename, size_t w, size_t h, size_t c);
		~PhotoshopRawReader();

		int Init(std::string raw_filename, size_t w, size_t h, size_t c);

		int ToTiff(const std::string & tif_filename);

	protected:

		void init_default();

	// variables

	protected:
		std::string _raw_filename;

		size_t _width;
		size_t _height;
		size_t _channel;

		size_t _block_size;

	};
}