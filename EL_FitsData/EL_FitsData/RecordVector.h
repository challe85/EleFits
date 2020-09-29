/**
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

#ifndef _EL_FITSDATA_RECORDVECTOR_H
#define _EL_FITSDATA_RECORDVECTOR_H

#include <vector>

#include "EL_FitsData/Record.h"

namespace Euclid {
namespace FitsIO {

/**
 * @brief A collection of records of homogeneous value types.
 * @tparam T The value type of the records
 */
template <typename T>
class RecordVector {

public:
  /**
   * @brief Create a RecordVector with given number of records.
   */
  explicit RecordVector(std::size_t size);

  /**
   * @brief The records.
   */
  std::vector<Record<T>> vector;

  /**
   * @brief Find the first record with given keyword.
   */
  const Record<T> &operator[](const std::string &keyword) const;

  /**
   * @brief Find the first record with given keyword.
   */
  Record<T> &operator[](const std::string &keyword);

  /**
   * @brief Find and cast the first record with given keyword.
   * @tparam TValue The destination value type
   * @details
   * Althoug the method returns a Record, it can itself be sliced as its value,
   * so the following works:
   * \code
   * RecordVector<any> records;
   * // ...
   * int i = records.as<int>("KEYWORD"); // Get the value as int
   * \endcode
   */
  template <typename TValue>
  Record<TValue> as(const std::string &keyword) const;
};

} // namespace FitsIO
} // namespace Euclid

#define _EL_FITSDATA_RECORDVECTOR_IMPL
#include "EL_FitsData/impl/RecordVector.hpp"
#undef _EL_FITSDATA_RECORDVECTOR_IMPL

#endif