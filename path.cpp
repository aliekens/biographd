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

#include "path.h"

#include <assert.h>
#include <algorithm>
#include <iostream>

void
Path::print() const {
	std::cout << probability << "\t";
	for( const_iterator i = begin(); i != end(); i++ ) {
		std::cout << *i << ", ";
	}
}

struct  pathComparator {
	bool operator()(const Path &lhs, const Path &rhs) {
		return lhs.probability > rhs.probability;
	}
};

PathSet
PathSet::getTopItems( int n ) {
	if( size() < n )
		return *this;
	std::partial_sort(begin(), begin() + n, end(), pathComparator());
	PathSet result;
	for (int i = 0; i < n; ++i) {
		result.push_back( (*this)[ i ] );
	}
	return result;
}

void
PathSet::print() const {
	for( const_iterator i = begin(); i != end(); i++ ) {
		std::cout << "{ ";
		i->print();
		std::cout << " } ";
	}
	std::cout << std::endl;
}

std::vector< unsigned char >
PathSet::convertToByteVector( int number ) const {
	
	// each path consists of
	// * weight of the path as a double
	// * number of steps in a path as an int
	// * steps in a path as ints
	
	std::vector< unsigned char > result;
	
	// determine and assign size of resulting bytearray
	int size = 0;
	int counter = 0;
	size += sizeof(int);
	for( const_iterator i = begin(); i != end() && counter < number; i++ ) {
		size += sizeof( double ) + sizeof( int ) + i->size() * sizeof( int );
	}
	result.assign( size, 0 );
	
	counter = 0;
	int offset = 0;
	
	int npaths = this->size();
	memcpy( &( result[ offset ] ), &npaths, sizeof( unsigned int ) );
	offset += sizeof( unsigned int );
	
	for( const_iterator i = begin(); i != end() && counter < number; i++ ) {
		
		// probability of a path
		double pathWeight = i->probability;
		memcpy( &( result[ offset] ), &pathWeight, sizeof( double ) );
		offset += sizeof( double );
		
		// size of a path
		int pathSize = i->size();
		memcpy( &( result[ offset ] ), &pathSize, sizeof( unsigned int ) );
		offset += sizeof( unsigned int );
		
		// steps of a path
		for( Path::const_iterator j = i->begin(); j != i->end(); j++ ) {
			int step = *j;
			memcpy( &( result[ offset ] ), &step, sizeof( unsigned int ) );
			offset += sizeof( unsigned int );
		}
		
		counter++;
		
	}
	return result;
}
