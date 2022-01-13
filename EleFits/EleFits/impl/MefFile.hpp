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

#if defined(_ELEFITS_MEFFILE_IMPL) || defined(CHECK_QUALITY)

#include "EleFits/MefFile.h"

namespace Euclid {
namespace Fits {

template <class T>
const T& MefFile::access(long index) {
  Cfitsio::HduAccess::gotoIndex(m_fptr, index + 1); // CFitsIO index is 1-based
  const auto hduType = Cfitsio::HduAccess::currentType(m_fptr);
  auto& ptr = m_hdus[index];
  if (ptr == nullptr) {
    if (hduType == HduCategory::Image) {
      ptr.reset(new ImageHdu(Hdu::Token {}, m_fptr, index));
    } else if (hduType == HduCategory::Bintable) {
      ptr.reset(new BintableHdu(Hdu::Token {}, m_fptr, index));
    } else {
      ptr.reset(new Hdu(Hdu::Token {}, m_fptr, index));
    }
  }
  return ptr->as<T>();
}

template <class T>
const T& MefFile::accessFirst(const std::string& name, long version) {
  Cfitsio::HduAccess::gotoName(m_fptr, name, version, HduCategory::forClass<T>());
  return access<T>(Cfitsio::HduAccess::currentIndex(m_fptr) - 1); // -1 because CFitsIO index is 1-based
}

template <class T>
const T& MefFile::access(const std::string& name, long version) {
  const auto category = HduCategory::forClass<T>();
  const Hdu* hduPtr = nullptr;
  for (long i = 0; i < hduCount(); ++i) {
    const auto& hdu = access<Hdu>(i);
    const bool cMatch = (category == HduCategory::Any || hdu.type() == category);
    const bool cnMatch = cMatch && (name == "" || hdu.readName() == name);
    const bool cnvMatch = cnMatch && (version == 0 || hdu.readVersion() == version);
    if (cnvMatch) {
      if (hduPtr) {
        throw FitsError("Several HDU matches."); // TODO specific exception?
      } else {
        hduPtr = &hdu; // FIXME Cppcheck reports "Using object that is out of scope."
      }
    }
  }
  if (not hduPtr) {
    throw FitsError("No HDU match."); // TODO specific exception?
  }
  return hduPtr->as<T>();
}

template <class T>
const T& MefFile::accessPrimary() {
  return access<T>(MefFile::primaryIndex);
}

template <typename THdu>
HduSelector<THdu> MefFile::select(const HduFilter& filter) {
  return {*this, filter * HduCategory::forClass<THdu>()};
}

template <typename T, long N>
const ImageHdu& MefFile::initImageExt(const std::string& name, const Position<N>& shape) {
  Cfitsio::HduAccess::initImageExtension<T, N>(m_fptr, name, shape);
  const auto size = m_hdus.size();
  m_hdus.push_back(std::make_unique<ImageHdu>(Hdu::Token {}, m_fptr, size, HduCategory::Created));
  return m_hdus[size]->as<ImageHdu>();
}

template <typename TRaster>
const ImageHdu& MefFile::assignImageExt(const std::string& name, const TRaster& raster) {
  Cfitsio::HduAccess::assignImageExtension(m_fptr, name, raster);
  const auto size = m_hdus.size();
  m_hdus.push_back(std::make_unique<ImageHdu>(Hdu::Token {}, m_fptr, size, HduCategory::Created));
  return m_hdus[size]->as<ImageHdu>();
}

template <typename... TInfos>
const BintableHdu& MefFile::initBintableExt(const std::string& name, const TInfos&... infos) {
  Cfitsio::HduAccess::initBintableExtension(m_fptr, name, infos...);
  const auto size = m_hdus.size();
  m_hdus.push_back(std::make_unique<BintableHdu>(Hdu::Token {}, m_fptr, size, HduCategory::Created));
  return m_hdus[size]->as<BintableHdu>();
}

template <typename... TColumns>
const BintableHdu& MefFile::assignBintableExt(const std::string& name, const TColumns&... columns) {
  Cfitsio::HduAccess::assignBintableExtension(m_fptr, name, columns...);
  const auto size = m_hdus.size();
  m_hdus.push_back(std::make_unique<BintableHdu>(Hdu::Token {}, m_fptr, size, HduCategory::Created));
  return m_hdus[size]->as<BintableHdu>();
}

template <typename TColumns, std::size_t count>
const BintableHdu& MefFile::assignBintableExt(const std::string& name, const TColumns& columns) {
  Cfitsio::HduAccess::assignBintableExtension<TColumns, count>(m_fptr, name, columns);
  const auto size = m_hdus.size();
  m_hdus.push_back(std::make_unique<BintableHdu>(Hdu::Token {}, m_fptr, size, HduCategory::Created));
  return m_hdus[size]->as<BintableHdu>();
}

#ifndef DECLARE_ASSIGN_IMAGE_EXT
#define DECLARE_ASSIGN_IMAGE_EXT(type, unused) \
  extern template const ImageHdu& MefFile::assignImageExt(const std::string&, const PtrRaster<type, -1>&); \
  extern template const ImageHdu& MefFile::assignImageExt(const std::string&, const PtrRaster<type, 2>&); \
  extern template const ImageHdu& MefFile::assignImageExt(const std::string&, const PtrRaster<type, 3>&); \
  extern template const ImageHdu& MefFile::assignImageExt(const std::string&, const VecRaster<type, -1>&); \
  extern template const ImageHdu& MefFile::assignImageExt(const std::string&, const VecRaster<type, 2>&); \
  extern template const ImageHdu& MefFile::assignImageExt(const std::string&, const VecRaster<type, 3>&);
ELEFITS_FOREACH_RASTER_TYPE(DECLARE_ASSIGN_IMAGE_EXT)
#undef DECLARE_ASSIGN_IMAGE_EXT
#endif

} // namespace Fits
} // namespace Euclid

#endif
