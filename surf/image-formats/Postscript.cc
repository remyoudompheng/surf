/*
 *   surf - visualizing algebraic curves and algebraic surfaces
 *   Copyright (C) 1996-1997 Friedrich-Alexander-Universitaet
 *                           Erlangen-Nuernberg
 *                 1997-2000 Johannes Gutenberg-Universitaet Mainz
 *   Authors: Stephan Endrass, Hans Huelf, Ruediger Oertel,
 *            Kai Schneider, Ralf Schmitt, Johannes Beigel
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */


#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif


#include <sys/param.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif

#include <netdb.h>
#include <time.h>
#include <stdio.h>

#include <errno.h>
#include <string.h>
#include <stdio.h>

#include <bit_buffer.h>
#include <FileWriter.h>
#include <ScriptVar.h>

#ifdef NO_GETHOSTNAME_PROTO
extern "C" int gethostname (char *, int);
#endif

#include <Postscript.h>

#include<iostream>

namespace ImageFormats {

	Postscript imgFmt_Postscript;
	
	bool Postscript::saveDitheredImage(const char* filename, bit_buffer& pixel)
	{
		FileWriter fw(filename);
		FILE* file;

		if ((file = fw.openFile()) == 0) {
		        std::cerr << "Couldn't open file for writing.\n";
			return false;
		}

		//  Bounding box: center by default, assume a4 paper
		int resolution = ScriptVar::print_resolution_array_data[ScriptVar::print_resolution_data];
		int width  = (ScriptVar::main_width_data*72)/resolution;
		int height = (ScriptVar::main_height_data*72)/resolution;
		int x1 = (595 - width)/2;
		int x2 = width + x1;
		int y1 = (839 - height)/2;
		int y2 = height + y1;
		
		passwd* passwd_user = getpwuid( getuid( ) );
		
		char* name_user = passwd_user->pw_name;
		char* info_user = passwd_user->pw_gecos;
		
		char hostname[MAXHOSTNAMELEN];
		
		gethostname(hostname, MAXHOSTNAMELEN);
		
		time_t time_local = time(0);
		char* the_time = ctime(&time_local);
		
		fprintf( file,"%%!PS-Adobe-1.0\n" );
		fprintf( file,"%%%%BoundingBox: %d %d %d %d\n",x1-1,y1-1,x2+1,y2+1 );
		fprintf( file,"%%%%Creator: %s:%s (%s)\n",hostname,name_user,info_user );
		fprintf( file,"%%%%Title: image created with surf\n");
		fprintf( file,"%%%%CreationDate: %s",the_time );
		fprintf( file,"%%%%EndComments\n" );
		fprintf( file,"%%%%Pages: 1\n");
		fprintf( file,"%%%%EndProlog\n" );
		fprintf( file,"%%%%Page: 1 1\n" );
		fprintf( file,"\n" );
		fprintf( file,"/bitdump %% stk: width, height, iscale\n" );
		fprintf( file,"%% dump a bit image with lower left corner at current origin,\n" );
		fprintf( file,"%% scaling by iscale (iscale=1 means 1/300 inch per pixel)\n" );
		fprintf( file,"{\n" );
		fprintf( file,"        %% read arguments\n" );
		fprintf( file,"        /iscale exch def\n" );
		fprintf( file,"        /height exch def\n" );
		fprintf( file,"        /width exch def\n" );
		fprintf( file,"\n" );
		fprintf( file,"        %% scale appropriately\n" );
		fprintf( file,"        width iscale mul height iscale mul scale\n" );
		fprintf( file,"\n" );
		fprintf( file,"        %% allocate space for one scanlne of input\n" );
		fprintf( file,"        /picstr %% picstr holds one scan line\n" );
		fprintf( file,"                width 7 add 8 idiv %% width of image in bytes = ceiling( width/8)\n" );
		fprintf( file,"                string\n" );
		fprintf( file,"                def\n" );
		fprintf( file,"\n" );
		fprintf( file,"        %% read and dump the image\n" );
		fprintf( file,"        width height 1 [width 0 0 height neg 0 height]\n" );
		fprintf( file,"        { currentfile picstr readhexstring pop }\n" );
		fprintf( file,"        image\n" );
		fprintf( file,"} def\n" );
		fprintf( file,"72 %d div dup scale\n",resolution );
		fprintf( file,"%d %d translate\n",x1*resolution/72,y1*resolution/72 );
		fprintf( file,"%d %d 1 bitdump\n", ScriptVar::main_width_data, ScriptVar::main_height_data);
		
		for (int py = 0; py < ScriptVar::main_height_data; py++) {
			for (int px = 0; px < ScriptVar::main_width_data; px += 8) {
				fprintf(file, "%.2x", 255 - pixel.getByte(px,py));
			}
			fprintf(file, "\n");
		}
		fprintf( file,"showpage\n" );
		fprintf( file,"%%%%Trailer\n" );
		fprintf( file,"end\n" );
		fprintf( file,"%%%%EOF" );
		
		return true;
	}

} // namespace ImageFormats

