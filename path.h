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

#ifndef PATH__H
#define PATH__H

#include <vector>

class Path : public std::vector< int > {
public:
	double probability;
	Path() {}
	void print() const;
};

class PathSet : public std::vector< Path > {
public:
	PathSet getTopItems( int n );
	void print() const;
	std::vector< unsigned char > convertToByteVector( int number ) const;
};

#endif
