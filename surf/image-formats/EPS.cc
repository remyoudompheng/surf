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




#include <sys/param.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <netdb.h>
#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "bit_buffer.h"
#include "Misc.h"
#include "FileWriter.h"

#include "EPS.h"

#ifdef NO_GETHOSTNAME_PROTO
extern "C" int gethostname (char *, int);
#endif

using namespace std;

namespace ImageFormats {

	EPS imgFmt_EPS;
	
	
	bool EPS::saveDitheredImage(const char* filename,
				    bit_buffer& pixel,
				    int paper_width, int paper_height, int resolution,
				    bool fromDlg)
	{
		FileWriter fw(filename);
		FILE* file;
		
		if ((file = fw.openFile()) == 0) {
			Misc::alert("Couldn't open file for writing.");
			return false;
		}
		
		
		// --------------
		//  Bounding box
		// --------------
		
		int     x1 = 0;
		int     x2 = ( paper_width*72 )/resolution;
		int     y1 = 0;
		int     y2 = ( paper_height*72 )/resolution;
		
		// --------------------
		//  User name and info
		// --------------------
		struct  passwd  *passwd_user;
		
		passwd_user = getpwuid( getuid( ) );
		
		char    *name_user = passwd_user->pw_name;
		char    *info_user = passwd_user->pw_gecos;
		
		// ----------
		//  Hostname
		// ----------
		
		char    hostname[MAXHOSTNAMELEN];
		
		gethostname (hostname, MAXHOSTNAMELEN);
		
		// ---------------
		//  Time and date
		// ---------------
		
		time_t  time_local;
		char    *the_time;
		
		time_local = time (NULL);
		the_time   = ctime (&time_local);
		
		// ---------------
		//  Picture title
		// ---------------
		
		const char    *title = "algebraic surface (dithered image)";
		
		
		// -----------------
		//  Number of pages
		// -----------------
		
		int     pages = 1;
		
		fprintf( file,"%%!PS-Adobe-3.0 EPSF-2.0\n" );
		fprintf( file,"%%%%Title: %s\n",title );
		fprintf( file,"%%%%Creator: %s:%s (%s)\n",hostname,name_user,info_user );
		fprintf( file,"%%%%CreationDate: %s",the_time );
		fprintf( file,"%%%%BoundingBox: %d %d %d %d\n",x1,y1,x2+1,y2+1 );
		fprintf( file,"%%%%Pages: %d\n",pages );
		fprintf( file,"%%%%EndComments\n" );
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
		fprintf( file,"%d %d translate\n",0,0 );
		fprintf( file,"%d %d 1 bitdump\n",paper_width,paper_height );
		
		for (int py = 0; py < paper_height; py++) {
			for(int px = 0; px < paper_width; px += 8) {
				fprintf(file, "%.2x", 255 - pixel.getByte(px,py));
			}
			fprintf( file,"\n" );
		}

		return true;
	}

} // namespace ImageFormats

