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