#*****************************************************************************#
#    Copyright (C) 2009 Aali132                                               #
#    Copyright (C) 2018 quantumpencil                                         #
#    Copyright (C) 2018 Maxime Bacoux                                         #
#    Copyright (C) 2020 myst6re                                               #
#    Copyright (C) 2020 Chris Rizzitello                                      #
#    Copyright (C) 2020 John Pritchard                                        #
#    Copyright (C) 2021 Julian Xhokaxhiu                                      #
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

if (NOT VGMSTREAM_FOUND)
	find_package(FFMPEG REQUIRED)

	find_library(
		VGMSTREAM_LIBRARY
		libvgmstream
		PATH_SUFFIXES
		lib
	)

	find_path(
		VGMSTREAM_INCLUDE_DIR
		vgmstream.h
		PATH_SUFFIXES
		include/libvgmstream
	)

	add_library(VGMSTREAM::VGMSTREAM STATIC IMPORTED)

	set_target_properties(
		VGMSTREAM::VGMSTREAM
		PROPERTIES
		IMPORTED_LOCATION
		"${VGMSTREAM_LIBRARY}"
		INTERFACE_INCLUDE_DIRECTORIES
		"${VGMSTREAM_INCLUDE_DIR}"
		INTERFACE_LINK_LIBRARIES
		"FFMPEG::AVUtil;FFMPEG::SWResample;FFMPEG::AVCodec;FFMPEG::AVFormat;FFMPEG::SWScale"
		INTERFACE_COMPILE_DEFINITIONS
		"USE_ALLOCA=1"
	)

	find_package_handle_standard_args(VGMSTREAM DEFAULT_MSG VGMSTREAM_LIBRARY VGMSTREAM_INCLUDE_DIR)
endif()