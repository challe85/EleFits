/**
 * @file src/program/EL_FitsIO_Example.cpp
 * @date 07/24/19
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

#include <map>
#include <string>

#include <boost/program_options.hpp>
#include "ElementsKernel/ProgramHeaders.h"

#include "EL_CfitsioWrapper/CfitsioFixture.h"
#include "EL_CfitsioWrapper/FileWrapper.h"
#include "EL_CfitsioWrapper/HduWrapper.h"
#include "EL_CfitsioWrapper/ImageWrapper.h"
#include "EL_CfitsioWrapper/RecordWrapper.h"

using boost::program_options::options_description;
using boost::program_options::variable_value;
using boost::program_options::value;

using namespace Cfitsio;


static Elements::Logging logger = Elements::Logging::getLogger("EL_FitsIO_Example");

class EL_FitsIO_Example : public Elements::Program {

public:

	options_description defineSpecificProgramOptions() override {
	
		options_description options {};
		options.add_options()
			("output", value<std::string>()->default_value("/tmp/test.fits"), "Output file");
		return options;
	}

	Elements::ExitCode mainMethod(std::map<std::string, variable_value>& args) override {

		Elements::Logging logger = Elements::Logging::getLogger("EL_FitsIO_Example");

		const std::string filename = args["output"].as<std::string>();

		logger.info();

		logger.info() << "Creating Fits file: " << filename;
		auto fptr = File::create_and_open(filename, File::CreatePolicy::OVER_WRITE);
		logger.info() << "Writing new record: VALUE = 1";
		Record::write_value(fptr, "VALUE", 1);
		logger.info() << "Updating record: VALUE = 2";
		Record::update_value(fptr, "VALUE", 2);
		Test::SmallTable table; // Predefined table for testing purpose
		logger.info() << "Creating bintable extension: SMALLTBL";
		HDU::create_bintable_extension(fptr, "SMALLTBL", table.id_col, table.radec_col, table.name_col, table.dist_mag_col);
		Test::SmallRaster raster; // Predefined image raster for testing purpose
		logger.info() << "Creating image extension: SMALLIMG";
		HDU::create_image_extension(fptr, "SMALLIMG", raster);
		logger.info() << "Closing file.";
		File::close(fptr);

		logger.info();

		logger.info() << "Reopening file.";
		fptr = File::open(filename, File::OpenPolicy::READ_ONLY);
		logger.info() << "Reading record: VALUE = " << Record::parse_value<int>(fptr, "VALUE");

		logger.info();

		logger.info() << "Reading bintable.";
		HDU::goto_name(fptr, "SMALLTBL");
		const auto ids = Bintable::read_column<int>(fptr, "ID").data;
		logger.info() << "First id: " << ids[0];
		const auto names = Bintable::read_column<std::string>(fptr, "NAME").data;
		logger.info() << "Last name: " << names[names.size()-1];

		logger.info();
		
		logger.info() << "Reading image.";
		HDU::goto_name(fptr, "SMALLIMG");
		const auto image = Image::read_raster<float>(fptr);
		logger.info() << "First pixel: " << image[{0, 0}];
		const auto width = image.length<0>();
		const auto height = image.length<1>();
		logger.info() << "Last pixel: " << image[{width-1, height-1}];
		logger.info() << "Reclosing file.";
		File::close(fptr);

		logger.info();

		return Elements::ExitCode::OK;

	}

};

MAIN_FOR(EL_FitsIO_Example)