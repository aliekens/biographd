/*
   Biograph computing kernel daemon
   Copyright (C) 2013-2016  Anthony Liekens

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef XMLRPCPARAMETERS__H
#define XMLRPCPARAMETERS__H

#include <xmlrpc-c/base.hpp>
#include <map>

class XMLRPCParameters : public std::map< std::string, xmlrpc_c::value> {
public:
	XMLRPCParameters( const std::map< std::string, xmlrpc_c::value>& parameters ) {
		insert( parameters.begin(), parameters.end() );
	}
};

#endif
