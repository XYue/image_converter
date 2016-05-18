#include "praw/praw.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

#include "gdal_priv.h"

namespace praw
{


	PhotoshopRawReader::PhotoshopRawReader()
	{
		init_default();
	}

	PhotoshopRawReader::PhotoshopRawReader(std::string raw_filename, size_t w, size_t h, size_t c ) :
		_raw_filename(raw_filename), _width(w), _height(h), _channel(c)
	{
		init_default();
	}

	PhotoshopRawReader::~PhotoshopRawReader()
	{
	}

	int PhotoshopRawReader::Init( std::string raw_filename, size_t w, size_t h, size_t c )
	{
		int ret = -1;

		do 
		{
			if (raw_filename.empty() || w <= 0 || h <= 0 || c <= 0)
			{
				std::cout<<"invalid input " << raw_filename << " "
					<< w <<" "
					<< h <<" "
					<< c << std::endl;
			}

			_raw_filename = raw_filename;
			_width = w;
			_height = h;
			_channel = c;

			ret = 0;
		} while (0);

		return ret;
	}

	int PhotoshopRawReader::ToTiff( const std::string & tif_filename, bool lzw_compress /*= false*/ )
	{
		int ret = -1;

		CPLSetConfigOption("GDAL_FILENAME_IS_UTF8","NO");
		GDALAllRegister();

		do 
		{
			if (_raw_filename.empty() || 
				_width <= 0 || _height <= 0 || 
				(_channel != 1 && _channel != 3) )
			{
				std::cout<<"invalid input " << _raw_filename << " "
					<< _width <<" "
					<< _height <<" "
					<< _channel << std::endl;
			}


			std::ifstream is (_raw_filename, std::ifstream::binary);
			if (!is.good()) 
			{		
				std::cout << "cannot open "<<_raw_filename<<std::endl;
				break;
			}


			char **param=NULL;
			std::stringstream sstr_block_size;		
			if (lzw_compress) param = CSLAddString(param,"COMPRESS=LZW");
			param = CSLAddString(param,"TILED=YES");
			param = CSLAddString(param,"BIGTIFF=IF_SAFER");
			sstr_block_size.clear(); sstr_block_size.str("");
			sstr_block_size << "BLOCKXSIZE=" << _block_size;
			param = CSLAddString(param,sstr_block_size.str().c_str());
			sstr_block_size.clear(); sstr_block_size.str("");
			sstr_block_size << "BLOCKYSIZE=" << _block_size;
			param = CSLAddString(param,sstr_block_size.str().c_str());
			GDALDataset* out_ds = 
				GetGDALDriverManager()->GetDriverByName("GTIFF")->
				Create(tif_filename.c_str(), _width, _height, _channel, GDT_Byte, param);
			if (out_ds==NULL)
			{
				std::cout<<"gdal creation error..."<<std::endl;
				break;
			}


			size_t total_size = 0;


			size_t buffer_size = _channel * _block_size * _width;
			char * buffer = new char [buffer_size];
			size_t y_offset = 0;
			while (is)
			{
				memset(buffer, 0, sizeof(char) * buffer_size);
				is.read(buffer, buffer_size);
				size_t count = is.gcount();
				if (!count)  break;
				size_t y_size = count / _channel / _width;
				
				// write data into tiff
				if (out_ds->RasterIO(GF_Write, 0, y_offset, _width, y_size,
					buffer, _width, y_size, GDT_Byte, _channel, NULL, _channel, _channel * _width, 1) != CE_None)
				{
					std::cout<<"warning: write data failed."<<std::endl;
				}

				total_size += count;
				y_offset += y_size;
			}
			delete [] buffer;	

			if (total_size != _width * _height * _channel)
			{
				std::cout<<"warning: some error may occur. " << total_size << " != "<<_width * _height * _channel<<std::endl;
			}


			if (out_ds) GDALClose(out_ds);

			ret = 0;
		} while (0);

		GDALDestroyDriverManager();

		return ret;
	}

	void PhotoshopRawReader::init_default()
	{
		_block_size = 256;
	}

}