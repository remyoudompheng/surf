// $Id$
//
// Author: Johannes Beigel <jojo@beigel.de>


// these two includes and the 'extern "C" { }' are needed for jpeglib.h
// otherwise I get errors here :-/
#include <ctype.h>
#include <stdio.h>
extern "C" {
#include <jpeglib.h>
}

#include "mytypes.h" // 'byte' typedef


bool write_jpeg_file (byte *rdata, byte *gdata, byte *bdata,
		       int width, int height, FILE *file)
{
	// init cinfo struct:

	int quality = 90; // this has to be set somewhere else (nice Gtk dialog?)

	jpeg_compress_struct cinfo;
	jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error (&jerr);
	jpeg_create_compress (&cinfo);
	jpeg_stdio_dest (&cinfo, file);
	cinfo.image_width = width;
	cinfo.image_height = height;
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;
	jpeg_set_defaults (&cinfo);
	jpeg_set_quality (&cinfo, quality, true);
	jpeg_start_compress (&cinfo, true);


	// compress and write to file:

	int row_stride = width * 3;
	JSAMPLE* row = new JSAMPLE[row_stride];
	JSAMPROW rowpointer[1] = { row };
	JSAMPLE* rptr = rdata;
	JSAMPLE* gptr = gdata;
	JSAMPLE* bptr = bdata;

	while (cinfo.next_scanline < height) {
		JSAMPLE* ptr = row;
		for (int i = 0; i < width; ++i) {
			*ptr++ = *rptr++;
			*ptr++ = *gptr++;
			*ptr++ = *bptr++;
		}
		jpeg_write_scanlines (&cinfo, rowpointer, 1);
	}

	delete [] row;
	
	jpeg_finish_compress (&cinfo);
	jpeg_destroy_compress (&cinfo);
	
	return true;
}
