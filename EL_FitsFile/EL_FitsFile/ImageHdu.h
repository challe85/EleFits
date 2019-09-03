/**
 * @file EL_FitsFile/ImageHdu.h
 * @date 08/30/19
 * @author user
 *
 * @copyright (C) 2012-2020 Euclid Science Ground Segment
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#ifndef _EL_FITSFILE_IMAGEHDU_H
#define _EL_FITSFILE_IMAGEHDU_H

#include "EL_CfitsioWrapper/ImageWrapper.h" // TODO New module EL_FitsData for Raster
#include "EL_FitsFile/Hdu.h"

namespace Euclid {
namespace FitsIO {

class ImageHdu : public Hdu {

public:

	template<typename T, std::size_t n>
	using Raster = Cfitsio::Image::Raster<T, n>;

	ImageHdu(fitsfile* fptr, std::size_t index);

	virtual ~ImageHdu() = default;

	template<typename T, std::size_t n=2>
	Raster<T, n> read_raster() const;

	template<typename T, std::size_t n=2>
	void write_raster(const Raster<T, n>& raster) const;

};


/////////////////////
// IMPLEMENTATION //
///////////////////


template<typename T, std::size_t n>
ImageHdu::Raster<T, n> ImageHdu::read_raster() const {
	return Cfitsio::Image::read_raster<T, n>(m_fptr);
}

template<typename T, std::size_t n>
void ImageHdu::write_raster(const ImageHdu::Raster<T, n>& raster) const {
	Cfitsio::Image::write_raster(m_fptr, raster);
}

}
}

#endif