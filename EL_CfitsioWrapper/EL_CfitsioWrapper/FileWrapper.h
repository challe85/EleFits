/**
 * @file EL_CfitsioWrapper/FileWrapper.h
 * @date 07/23/19
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

#ifndef _EL_CFITSIOWRAPPER_FILEWRAPPER_H
#define _EL_CFITSIOWRAPPER_FILEWRAPPER_H

#include <cfitsio/fitsio.h>
#include <string>

namespace Cfitsio {

/**
 * @brief File-related functions.
 */
namespace File {

/**
 * @brief File creation policy.
 */
enum class CreatePolicy {
    CREATE_ONLY, ///< Create only (abort if file exists)
    OVER_WRITE ///< Allow overwriting if file already exists.
};

/**
 * @brief File opening policy.
 */
enum class OpenPolicy {
    READ_ONLY, ///< Read persmission only
    READ_WRITE ///< Read and write permissions
};

/**
 * @brief Create or overwrite a Fits file and open it.
 */
fitsfile* create_and_open(std::string filename, CreatePolicy policy);

/**
 * @brief Open an existing Fits file with optional write permission.
 */
fitsfile* open(std::string filename, OpenPolicy policy);

/**
 * @brief Check whether a Fits file is open with write permission.
 */
bool is_writable(fitsfile* fptr);

/**
 * @brief Close a Fits file.
 */
void close(fitsfile *fptr);

/**
 * @brief Close and delete a Fits file.
 * 
 * @warning Throw an exception if writing is not permitted.
 */
void close_and_delete(fitsfile *fptr);

/**
 * @brief Check whether a file is writable.
 */
bool is_writable(fitsfile* fptr);

}
}

#endif
