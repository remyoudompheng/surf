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

#ifndef IMAGEFORMATS_H
#define IMAGEFORMATS_H

#include<string>
#include<iostream>

class bit_buffer;
class RgbBuffer;
class Triangulator;

namespace ImageFormats {

        /**
         * Abstract base class for all special image format classes.
         * This class defines the interface for the image format subclasses
         * and some standard member functions which return false (if some
         * image format subclass doesn't implement one of the image types
         * color, dithered or 3D).
         */
	class Format {
	public:
		/**
                 * Returns name of this image format.
		 * @return Name of image format.
                 */
		virtual std::string getName() const = 0;

                /**
                 * Returns ID of this image format.
                 * Return ID of image format. This is the string that is used
                 * in surf script files to determine the file format.
                 * @return Image format ID
                 */
		virtual std::string getID() const = 0;

                /**
                 * Checks if the extension matches this image format.
                 * @param Filename extension to test.
                 * @return true if extensions matches, otherwise false.
                 */
		virtual bool isExtension(const std::string& ext) const = 0;

		/**
                 * Returns true if this file format supports color images.
                 * @return true if this class can save color images, otherwise false.
                 */
		virtual bool isColorFormat() const {
			return false;
		}

		/**
                 * Returns true if this file format supports dithered images.
                 * @return true if this class can save dithered images, otherwise false.
                 */
		virtual bool isDitherFormat() const {
			return false;
		}

		/**
                 * Returns true if this file format supports 3D images.
                 * @return true if this class can save 3D images, otherwise false.
                 */
//		virtual bool is3DFormat() const {
//			return false;
//		}
		
		/**
                 * Save color image.
                 * @param filename Filename to save data to.
                 * @param data RGB data.
                 * @return true: success, false: error
                 */
		virtual bool saveColorImage(const char* filename, RgbBuffer& data) {
			return false;
		}

		/**
                 * Save dithered image.
                 * @param filename Filename to save data to.
                 * @param data Bitplane data.
                 * @return true: success, false: error
                 */
		virtual bool saveDitheredImage(const char* filename, bit_buffer& data) {
			return false;
		}

		/**
                 * Save 3D image.
                 * @param filename Filename to save data to.
                 * @param data Triangulated data.
                 * @return true: success, false: error
                 */
//		virtual bool save3DImage(const char* filename, Triangulator& data) {
//			return false;
//		}
	};

	bool saveColorImage(const char* filename, RgbBuffer& data);
	bool saveDitheredImage(const char* filename, bit_buffer& data);
//	bool save3DImage(const char* filename, Triangulator& data);
}

#endif //!IMAGEFORMATS_H
