/*
 *   surf - visualizing algebraic curves and algebraic surfaces
 *   Copyright (C) 1996-1997 Friedrich-Alexander-Universitaet
 *                           Erlangen-Nuernberg
 *                 1997-2001 Johannes Gutenberg-Universitaet Mainz
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



#include<iostream>

#include "NewClip.h"
#include "NewClipSphereParallel.h"
#include "NewClipSphereCentral.h"
#include "NewClipAlgebraic.h"
#include "NewClipAlgebraicTetra.h"
#include "NewClipAlgebraicCube.h"
#include "NewClipAlgebraicOcta.h"
#include "NewClipAlgebraicDode.h"
#include "NewClipAlgebraicIco.h"
#include "NewClipAlgebraicCyl.h"
#include "NewClipAlgebraicUser.h"
#include "NewClipNone.h"

NewClip* NewClip::createSimpleClip (int perspective, int clipmodus)
{
	if (!perspective) { 
		// parallel perspective

		switch (clipmodus) {
		case 0:
			return new NewClipSphereParallel( );
		case 1:
			return new NewClipAlgebraicTetra( );
		case 2:
		        return new NewClipAlgebraicCube( );
		case 3:
			return new NewClipAlgebraicOcta( );
		case 4:
			return new NewClipAlgebraicDode( );
		case 5:
			return new NewClipAlgebraicIco( );
		case 6:
                        return new NewClipAlgebraicCyl( );
		case 7:
                        return new NewClipAlgebraicUser( );
		case 8:
			return new NewClipNone( );
		}
		

	} else {
		switch (clipmodus) {
		case 0:
			return new NewClipSphereCentral();
		case 1:
			return new NewClipAlgebraicTetra( );
		case 2:
			return new NewClipAlgebraicCube( );
		case 3:
			return new NewClipAlgebraicOcta( );
		case 4:
			return new NewClipAlgebraicDode( );
		case 5:
			return new NewClipAlgebraicIco( );
		case 6:
			return new NewClipAlgebraicCyl( );
		case 7:
			return new NewClipAlgebraicUser();
		case 8:
                        return new NewClipNone( );
		}
		// central perspective
	}
	std::cerr << __FILE__ ": WARNING  clipmodus " << clipmodus << " not implemented..using default mode";
	return createSimpleClip(perspective, 0);
}
