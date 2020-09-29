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

#ifdef _EL_FITSDATA_RASTER_IMPL

#include <functional> // multiplies
#include <numeric> // accumulate

#include "EL_FitsData/Raster.h"

namespace Euclid {
namespace FitsIO {

/// @cond INTERNAL
namespace Internal {

/**
 * @brief nD-index recursive implementation.
 * @tparam i The dimension of the current recursion step.
 * @details
 * We need a class for partial specialization.
 */
template <long i>
struct IndexImpl {

  /**
   * @brief Index of given position in given shape for Raster::index.
   */
  template <long n>
  static long offset(const Position<n> &shape, const Position<n> &pos);
};

/**
 * @brief Recurse: dimension i.
 */
template <long i>
template <long n>
inline long IndexImpl<i>::offset(const Position<n> &shape, const Position<n> &pos) {
  return std::get<n - 1 - i>(pos) + std::get<n - 1 - i>(shape) * IndexImpl<i - 1>::template offset<n>(shape, pos);
}

/**
 * @brief Base case: dimension 0.
 */
template <>
template <long n>
inline long IndexImpl<0>::offset(const Position<n> &shape, const Position<n> &pos) {
  return std::get<n - 1>(pos);
}

} // namespace Internal
/// @endcond

template <typename T, long n>
Raster<T, n>::Raster(Position<n> rasterShape) : shape(rasterShape) {
}

template <typename T, long n>
template <long i>
inline long Raster<T, n>::length() const {
  return std::get<i>(shape);
}

template <typename T, long n>
inline long Raster<T, n>::size() const {
  return std::accumulate(shape.begin(), shape.end(), 1, std::multiplies<long>());
}

template <typename T, long n>
inline long Raster<T, n>::index(const Position<n> &pos) const {
  return Internal::IndexImpl<n - 1>::template offset<n>(shape, pos);
}

template <typename T, long n>
inline const T &Raster<T, n>::operator[](const Position<n> &pos) const {
  return data()[index(pos)];
}

template <typename T, long n>
inline T &Raster<T, n>::operator[](const Position<n> &pos) {
  return const_cast<T &>(const_cast<const Raster *>(this)->operator[](pos));
}

template <typename T, long n>
PtrRaster<T, n>::PtrRaster(Position<n> rasterShape, const T *data) : Raster<T, n>(rasterShape), m_data(data) {
}

template <typename T, long n>
const T *PtrRaster<T, n>::data() const {
  return m_data;
}

template <typename T, long n>
VecRefRaster<T, n>::VecRefRaster(Position<n> shape, const std::vector<T> &data) : Raster<T, n>(shape), m_ref(data) {
}

template <typename T, long n>
const T *VecRefRaster<T, n>::data() const {
  return m_ref.data();
}

template <typename T, long n>
const std::vector<T> &VecRefRaster<T, n>::vector() const {
  return m_ref;
}

template <typename T, long n>
VecRaster<T, n>::VecRaster(Position<n> shape, std::vector<T> data) : Raster<T, n>(shape), m_vec(data) {
}

template <typename T, long n>
VecRaster<T, n>::VecRaster(Position<n> rasterShape) : Raster<T, n>(rasterShape), m_vec(this->size()) {
}

template <typename T, long n>
const T *VecRaster<T, n>::data() const {
  return m_vec.data();
}

template <typename T, long n>
T *VecRaster<T, n>::data() {
  return const_cast<T *>(const_cast<const VecRaster *>(this)->data());
}

template <typename T, long n>
const std::vector<T> &VecRaster<T, n>::vector() const {
  return m_vec;
}

template <typename T, long n>
std::vector<T> &VecRaster<T, n>::vector() {
  return m_vec;
}

} // namespace FitsIO
} // namespace Euclid

#endif
