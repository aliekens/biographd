#ifndef DISTRIBUTION__H
#define DISTRIBUTION__H

#include "node.h"

#include <map>
#include <vector>
#include <string>

class Distribution : public std::map< int, double > {
public:
	double getMean() const;
	double getVariance( const double mean ) const;

	void ensureStochasticity();
	void ensureStandardScore();
	void setItems( const Distribution& toBeSet );
	
	Distribution computeScores( const Distribution& prior ) const;

	Distribution getTopItems( int n = 10 ) const;
	Distribution keep( const Distribution& toBeRemoved ) const;
	Distribution remove( const Distribution& toBeRemoved ) const;
	Distribution keepByType( const std::map<int, Node>& nodes, int keeptype ) const; // 0 = keep all
	
	
	int getPosition( int target ) const;

	std::vector< unsigned char > convertToByteVector( int number = 10 ) const;
	
	
};

#endif
