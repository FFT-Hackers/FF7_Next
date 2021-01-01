/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2021 Julian Xhokaxhiu                                   //
//                                                                          //
//    This file is part of FFNx                                             //
//                                                                          //
//    FFNx is free software: you can redistribute it and/or modify          //
//    it under the terms of the GNU General Public License as published by  //
//    the Free Software Foundation, either version 3 of the License         //
//                                                                          //
//    FFNx is distributed in the hope that it will be useful,               //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of        //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         //
//    GNU General Public License for more details.                          //
/****************************************************************************/

#pragma once

#include <chrono>
#include <cstring>
#include <io.h>
#include <sstream>

#include <pugiconfig.hpp>
#include <pugixml.hpp>

#include "log.h"
#include "md5.h"

class Metadata
{
private:
	pugi::xml_document doc;

	std::string now;
	std::string userID;
	char savePath[260]{ 0 };
	std::vector<std::string> saveHash;

	void updateFF7();
	void updateFF8();

public:
	void apply();
};

extern Metadata metadataPatcher;
