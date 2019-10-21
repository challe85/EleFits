/**
 * @file EL_CfitsioWrapper/ImageWrapper.h
 * @date 07/25/19
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

#ifndef _EL_CFITSIOWRAPPER_IMAGEWRAPPER_H
#define _EL_CFITSIOWRAPPER_IMAGEWRAPPER_H

#include <cfitsio/fitsio.h>
#include <string>

#include "EL_FitsData/Raster.h"

#include "EL_CfitsioWrapper/ErrorWrapper.h"
#include "EL_CfitsioWrapper/FileWrapper.h"
#include "EL_CfitsioWrapper/TypeWrapper.h"


namespace Euclid {
namespace Cfitsio {

/**
 * @brief Image-related functions.
 */
namespace Image {

/**
 * @brief Read a Raster in current Image HDU.
 */
template<typename T, std::size_t n=2>
FitsIO::Raster<T, n> read_raster(fitsfile* fptr);

/**
 * @brief Write a Raster in current Image HDU.
 */
template<typename T, std::size_t n=2>
void write_raster(fitsfile* fptr, const FitsIO::Raster<T, n>& raster);


/////////////////////
// IMPLEMENTATION //
///////////////////


template<typename T, std::size_t n>
FitsIO::Raster<T, n> read_raster(fitsfile* fptr) {
	FitsIO::Raster<T, n> raster;
	int status = 0;
	fits_get_img_size(fptr, n, &raster.shape[0], &status);
	may_throw_cfitsio_error(status);
	const auto size = raster.size();
	raster.data.resize(size);
	fits_read_img(fptr, TypeCode<T>::for_image(), 1, size, nullptr, &raster.data.data()[0], nullptr, &status);
	// Number 1 is a 1-base offset (so we read the whole raster here)
	may_throw_cfitsio_error(status);
	return raster;
}

template<typename T, std::size_t n>
void write_raster(fitsfile* fptr, const FitsIO::Raster<T, n>& raster) {
	may_throw_readonly_error(fptr);
	int status = 0;
	std::vector<T> nonconst_data = raster.data; //TODO const-correctness issue?
	fits_write_img(fptr, TypeCode<T>::for_image(), 1, raster.size(), nonconst_data.data(), &status);
	may_throw_cfitsio_error(status);
}

}
}
}

#endif
