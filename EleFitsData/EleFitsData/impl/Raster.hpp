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

#if defined(_ELEFITSDATA_RASTER_IMPL) || defined(CHECK_QUALITY)

#include "EleFitsData/FitsError.h"
#include "EleFitsData/Raster.h"

#include <functional> // multiplies
#include <numeric> // accumulate

namespace Euclid {
namespace Fits {

/// @cond INTERNAL
namespace Internal {

/**
 * @brief nD-index recursive implementation.
 * @tparam N The raster dimension.
 * @tparam i The dimension of the current recursion step, should be initialized with `N - 1`.
 */
template <long N, long I = N - 1>
struct IndexRecursionImpl {

  /**
   * @brief Index of given position in given shape for Raster::index.
   */
  static long index(const Position<N>& shape, const Position<N>& pos) {
    return std::get<N - 1 - I>(pos.container()) +
        std::get<N - 1 - I>(shape.container()) * IndexRecursionImpl<N, I - 1>::index(shape, pos);
  }
};

/**
 * @brief Terminal case: dimension 0.
 */
template <long N>
struct IndexRecursionImpl<N, 0> {

  /**
   * @brief pos[N - 1]
   */
  static long index(const Position<N>& shape, const Position<N>& pos) {
    (void)(shape);
    return std::get<N - 1>(pos.container());
  }
};

/**
 * @brief Variable dimension case.
 */
template <long I>
struct IndexRecursionImpl<-1, I> {

  /**
   * @brief pos[0] + shape[1] * (pos[1] + shape[2] * (pos[2] + shape[3] * (...)))
   */
  static long index(const Position<-1>& shape, const Position<-1>& pos) {
    const auto n = shape.size();
    if (pos.size() != n) {
      throw FitsError(
          "Dimension mismatch. Raster is of dimension " + std::to_string(n) + " while position is of dimension " +
          std::to_string(pos.size()));
    }
    long res = 0;
    for (std::size_t j = 0; j < shape.size(); ++j) {
      res = pos[n - 1 - j] + shape[n - 1 - j] * res;
    }
    return res;
  }
};

} // namespace Internal
/// @endcond

// Raster

template <typename T, long N, typename TContainer>
const Position<N>& Raster<T, N, TContainer>::shape() const {
  return m_shape;
}

template <typename T, long N, typename TContainer>
Region<N> Raster<T, N, TContainer>::domain() const {
  return Region<N>::fromShape(Position<N>::zero(), m_shape);
}

template <typename T, long N, typename TContainer>
inline long Raster<T, N, TContainer>::dimension() const {
  return m_shape.size();
}

template <typename T, long N, typename TContainer>
template <long I>
inline long Raster<T, N, TContainer>::length() const {
  return std::get<I>(m_shape.container());
}

template <typename T, long N, typename TContainer>
inline long Raster<T, N, TContainer>::index(const Position<N>& pos) const {
  return Internal::IndexRecursionImpl<N>::index(m_shape, pos);
}

template <typename T, long N, typename TContainer>
inline const T& Raster<T, N, TContainer>::operator[](const Position<N>& pos) const {
  return (*this)[index(pos)];
}

template <typename T, long N, typename TContainer>
inline T& Raster<T, N, TContainer>::operator[](const Position<N>& pos) {
  return const_cast<T&>(const_cast<const Raster&>(*this)[pos]);
}

template <typename T, long N, typename TContainer>
inline const T& Raster<T, N, TContainer>::at(const Position<N>& pos) const {
  auto boundedPos = pos;
  for (long i = 0; i < dimension(); ++i) {
    auto& b = boundedPos[i];
    const auto& s = m_shape[i];
    OutOfBoundsError::mayThrow("pos[" + std::to_string(i) + "]", b, {-s, s - 1});
    if (b < 0) {
      b += s;
    }
  }
  return operator[](boundedPos);
}

template <typename T, long N, typename TContainer>
inline T& Raster<T, N, TContainer>::at(const Position<N>& pos) {
  return const_cast<T&>(const_cast<const Raster&>(*this).at(pos));
}

template <typename T, long N, typename TContainer>
Subraster<T, N, TContainer> Raster<T, N, TContainer>::subraster(const Region<N>& region) {
  return {*this, region};
}

template <typename T, long N, typename TContainer>
const Subraster<T, N, TContainer> Raster<T, N, TContainer>::subraster(const Region<N>& region) const {
  return {*this, region};
}

template <typename T, long N, typename TContainer>
template <long M>
const PtrRaster<const T, M> Raster<T, N, TContainer>::slice(const Region<N>& region) const {
  // FIXME resolve
  if (not isContiguous<M>(region)) {
    throw FitsError("Cannot slice: Region is not contiguous."); // FIXME clarify
  }
  const auto& f = region.front;
  const auto& b = region.back;
  Position<M> reduced(M);
  for (long i = 0; i < M; ++i) {
    reduced[i] = b[i] - f[i] + 1;
  }
  return {reduced, &operator[](region.front)};
}

template <typename T, long N, typename TContainer>
template <long M>
PtrRaster<T, M> Raster<T, N, TContainer>::slice(const Region<N>& region) {
  if (not isContiguous<M>(region)) {
    throw FitsError("Cannot slice: Region is not contiguous."); // FIXME clarify
  }
  const auto& f = region.front;
  const auto& b = region.back;
  Position<M> reduced(M);
  for (long i = 0; i < M; ++i) {
    reduced[i] = b[i] - f[i] + 1;
  }
  // FIXME duplication
  return {reduced, &operator[](region.front)};
}

template <typename T, long N, typename TContainer>
const PtrRaster<const T, N> Raster<T, N, TContainer>::section(long front, long back) const {
  auto region = domain();
  const auto last = dimension() - 1;
  region.front[last] = front;
  region.back[last] = back;
  return slice<N>(region);
}

template <typename T, long N, typename TContainer>
PtrRaster<T, N> Raster<T, N, TContainer>::section(long front, long back) {
  auto region = domain();
  const auto last = dimension() - 1;
  region.front[last] = front;
  region.back[last] = back;
  return slice<N>(region);
}

template <typename T, long N, typename TContainer>
const PtrRaster<const T, N == -1 ? -1 : N - 1> Raster<T, N, TContainer>::section(long index) const {
  auto region = domain();
  const auto last = dimension() - 1;
  region.front[last] = index;
  region.back[last] = index;
  return slice < N == -1 ? -1 : N - 1 > (region);
  // FIXME duplication => return section(index, index).slice<N-1>(Region<N>::whole());
}

template <typename T, long N, typename TContainer>
PtrRaster<T, N == -1 ? -1 : N - 1> Raster<T, N, TContainer>::section(long index) {
  auto region = domain();
  const auto last = dimension() - 1;
  region.front[last] = index;
  region.back[last] = index;
  return slice < N == -1 ? -1 : N - 1 > (region);
  // FIXME duplication
}

template <typename T, long N, typename TContainer>
template <long M>
bool Raster<T, N, TContainer>::isContiguous(const Region<N>& region) const {
  const auto& f = region.front;
  const auto& b = region.back;
  for (long i = 0; i < M - 1; ++i) {
    if (f[i] != 0 || b[i] != m_shape[i] - 1) { // Doesn't span across the full axis => index jump
      return false;
    }
  }
  for (long i = M; i < dimension(); ++i) {
    if (b[i] != f[i]) { // Not flat along axis i >= M => dimension >= M
      return false;
    }
  }
  return true;
}

#ifndef DECLARE_RASTER_CLASSES
#define DECLARE_RASTER_CLASSES(type, unused) \
  extern template class Raster<type, -1, type*>; \
  extern template class Raster<type, 2, type*>; \
  extern template class Raster<type, 3, type*>; \
  extern template class Raster<const type, -1, const type*>; \
  extern template class Raster<const type, 2, const type*>; \
  extern template class Raster<const type, 3, const type*>; \
  extern template class Raster<type, -1, std::vector<type>>; \
  extern template class Raster<type, 2, std::vector<type>>; \
  extern template class Raster<type, 3, std::vector<type>>;
ELEFITS_FOREACH_RASTER_TYPE(DECLARE_RASTER_CLASSES)
#undef DECLARE_COLUMN_CLASSES
#endif

} // namespace Fits
} // namespace Euclid

#endif