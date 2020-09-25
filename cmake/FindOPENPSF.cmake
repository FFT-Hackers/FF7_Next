#*****************************************************************************#
#    Copyright (C) 2009 Aali132                                               #
#    Copyright (C) 2018 quantumpencil                                         #
#    Copyright (C) 2018 Maxime Bacoux                                         #
#    Copyright (C) 2020 Julian Xhokaxhiu                                      #
#    Copyright (C) 2020 myst6re                                               #
#    Copyright (C) 2020 Chris Rizzitello                                      #
#    Copyright (C) 2020 John Pritchard                                        #
#                                                                             #
#    This file is part of FFNx                                                #
#                                                                             #
#    FFNx is free software: you can redistribute it and/or modify             #
#    it under the terms of the GNU General Public License as published by     #
#    the Free Software Foundation, either version 3 of the License            #
#                                                                             #
#    FFNx is distributed in the hope that it will be useful,                  #
#    but WITHOUT ANY WARRANTY; without even the implied warranty of           #
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            #
#    GNU General Public License for more details.                             #
#*****************************************************************************#

include(FindPackageHandleStandardArgs)

if (NOT OPENPSF_FOUND)
	find_package(ZLib REQUIRED)

	# PSXCore
	find_library(
		OPENPSF_PSXCORE_LIBRARY
		PSXCore
		PATH_SUFFIXES
		lib
	)

	add_library(OPENPSF::PSXCORE STATIC IMPORTED)

	set_target_properties(
		OPENPSF::PSXCORE
		PROPERTIES
		IMPORTED_LOCATION
		"${OPENPSF_PSXCORE_LIBRARY}"
	)

	# psflib
	find_path(
		OPENPSF_PSFLIB_INCLUDE_DIR
		psflib.h
		PATH_SUFFIXES
		include
	)

	find_library(
		OPENPSF_PSFLIB_LIBRARY
		psflib
		PATH_SUFFIXES
		lib
	)

	add_library(OPENPSF::PSFLIB STATIC IMPORTED)

	set_target_properties(
		OPENPSF::PSFLIB
		PROPERTIES
		IMPORTED_LOCATION
		"${OPENPSF_PSFLIB_LIBRARY}"
		INTERFACE_INCLUDE_DIRECTORIES
		"${OPENPSF_PSFLIB_INCLUDE_DIR}"
		INTERFACE_LINK_LIBRARIES
		"ZLib::ZLib"
	)

	# OpenPSF
	find_path(
		OPENPSF_INCLUDE_DIR
		openpsf.h
		PATH_SUFFIXES
		include/libopenpsf
	)

	find_library(
		OPENPSF_LIBRARY
		openpsf
		PATH_SUFFIXES
		lib
	)

	add_library(OPENPSF::OPENPSF STATIC IMPORTED)

	set_target_properties(
		OPENPSF::OPENPSF
		PROPERTIES
		IMPORTED_LOCATION
		"${OPENPSF_LIBRARY}"
		INTERFACE_INCLUDE_DIRECTORIES
		"${OPENPSF_INCLUDE_DIR}"
		INTERFACE_LINK_LIBRARIES
		"OPENPSF::PSXCORE;OPENPSF::PSFLIB"
	)

	find_package_handle_standard_args(OPENPSF DEFAULT_MSG
		OPENPSF_PSXCORE_LIBRARY
		OPENPSF_PSFLIB_LIBRARY
		OPENPSF_LIBRARY
		OPENPSF_INCLUDE_DIR
	)
endif()
