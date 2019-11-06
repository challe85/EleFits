/**
 * @file EL_FitsData/FitsDataFixture.h
 * @date 10/21/19
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

#ifndef _EL_FITSDATA_FITSDATAFIXTURE_H
#define _EL_FITSDATA_FITSDATAFIXTURE_H

#include <algorithm>
#include <chrono>
#include <complex>
#include <random>
#include <string>
#include <vector>

#include "EL_FitsData/Raster.h"
#include "EL_FitsData/Column.h"


namespace Euclid {
namespace FitsIO {

/**
 * @brief Test-related classes and functions.
 */
namespace Test {


/**
 * @brief A small 2D image raster.
 */
class SmallRaster : public FitsIO::Raster<float> {

public:

	SmallRaster(long width=3, long height=2);
	
	virtual ~SmallRaster() = default;
	
	bool approx(const FitsIO::Raster<float>& other, float tol=0.1) const;
	
	long width, height;

};

/**
 * @brief A small set of columns with various types.
 */
class SmallTable {

public:

	using id_t = int;
	using radec_t = std::complex<float>; // Could be std::pair<float> with width=2
	using name_t = std::string;
	using dist_mag_t = std::vector<double>;

	SmallTable();
	
	std::string extname;
	
	std::vector<id_t> ids;
	std::vector<radec_t> radecs;
	std::vector<name_t> names;
	std::vector<dist_mag_t> dists_mags;
	
	FitsIO::Column<id_t> id_col;
	FitsIO::Column<radec_t> radec_col;
	FitsIO::Column<name_t> name_col;
	FitsIO::Column<dist_mag_t> dist_mag_col;

};

/**
 * @brief A small scalar column of given type.
 */
template<typename T>
class RandomScalarColumn : public FitsIO::Column<T> {

public:

	RandomScalarColumn(std::size_t size=3);
	virtual ~RandomScalarColumn() = default;

};

/**
 * @brief A small string column.
 */
class SmallStringColumn : public FitsIO::Column<std::string> {

public:

	SmallStringColumn();
	virtual ~SmallStringColumn() = default;

};

/**
 * @brief A small vector column of given type.
 */
template<typename T>
class SmallVectorColumn : public FitsIO::Column<std::vector<T>> {

public:

	SmallVectorColumn();
	virtual ~SmallVectorColumn() = default;

};

template<typename T>
T generate_random_value();

template<typename T>
std::vector<T> generate_random_vector(std::size_t size);

template<>
std::vector<std::complex<float>> generate_random_vector<std::complex<float>>(std::size_t size);

template<>
std::vector<std::complex<double>> generate_random_vector<std::complex<double>>(std::size_t size);

template<>
std::vector<std::string> generate_random_vector<std::string>(std::size_t size);


/////////////////////
// IMPLEMENTATION //
///////////////////


template<typename T>
RandomScalarColumn<T>::RandomScalarColumn(std::size_t size) :
		FitsIO::Column<T> { "SCALAR", 1, "m", generate_random_vector<T>(size) } {
}

template<>
RandomScalarColumn<std::string>::RandomScalarColumn(std::size_t size) :
		FitsIO::Column<std::string> { "SCALAR", 1, "m", generate_random_vector<std::string>(size) } {
	for(const auto& v : data)
		if(v.length() + 1 > repeat)
			repeat = v.length() + 1;
}

template<typename T>
SmallVectorColumn<T>::SmallVectorColumn() :
		FitsIO::Column<std::vector<T>> { "VECTOR", 2, "m2", { { T(0.), T(1.) }, { T(2.), T(3.) }, { T(4.), T(5.) } } } {
}

template<typename T>
T generate_random_value() {
	auto vec = generate_random_vector<T>(1);
	return vec[0];
}

template<typename T>
std::vector<T> generate_random_vector(std::size_t size) {
	const auto seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator(seed);
	const double min = std::numeric_limits<T>::min();
	const double max = std::numeric_limits<T>::max();
	std::uniform_real_distribution<double> distribution(min, max);
	std::vector<T> vec(size);
	for(auto&& val : vec)
		val = T(distribution(generator));
	return vec;
}

template<>
std::vector<std::complex<float>> generate_random_vector<std::complex<float>>(std::size_t size) {
	const auto re_im_vec = generate_random_vector<float>(size * 2);
	std::vector<std::complex<float>> vec(size);
	const auto im_begin = re_im_vec.begin() + size;
	std::transform(re_im_vec.begin(), im_begin, im_begin, vec.begin(),
			[](float re, float im) { return std::complex<float> {re, im}; });
	return vec;
}

template<>
std::vector<std::complex<double>> generate_random_vector<std::complex<double>>(std::size_t size) {
	const auto re_vec = generate_random_vector<double>(size);
	const auto im_vec = generate_random_vector<double>(size);
	std::vector<std::complex<double>> vec(size);
	std::transform(re_vec.begin(), re_vec.end(), im_vec.begin(), vec.begin(),
			[](double re, double im) { return std::complex<double> {re, im}; });
	return vec;
}

template<>
std::vector<std::string> generate_random_vector<std::string>(std::size_t size) {
	std::vector<int> int_vec = generate_random_vector<int>(size);
	std::vector<std::string> vec(size);
	std::transform(int_vec.begin(), int_vec.end(), vec.begin(),
			[](int i) { return std::to_string(i); } );
	return vec;
}


}
}
}

#endif