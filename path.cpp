#include "path.h"

#include <assert.h>
#include <algorithm>
#include <iostream>

void
Path::print() const {
	std::cout << probability << "\t";
	for( const_iterator i = begin(); i != end(); i++ ) {
		std::cout << *i << "\t";
	}
	std::cout << std::endl;
}

struct  pathComparator {
	bool operator()(const Path &lhs, const Path &rhs) {
		return lhs.probability > rhs.probability;
	}
};

PathSet
PathSet::getTopItems( int n ) {
	assert( size() >= n );
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
		i->print();
	}
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
	for( const_iterator i = begin(); i != end() && counter < number; i++ ) {
		size += sizeof( double ) + sizeof( int ) + i->size() * sizeof( int );
	}
	result.assign( size, 0 );
	
	counter = 0;
	int offset = 0;
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
