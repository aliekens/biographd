#include "distribution.h"
#include "graph.h"

#include "math.h"
#include "assert.h"
#include <vector>
#include <algorithm>
#include <iostream>

typedef std::pair<int, double> intDoublePair;

struct probabilityComparator {
	bool operator()(const intDoublePair &lhs, const intDoublePair &rhs) {
		return lhs.second > rhs.second;
	}
};

void
Distribution::setItems( const Distribution& toBeSet ) {
	for( const_iterator i = toBeSet.begin(); i != toBeSet.end(); i++ ) {
		if( find( i->first ) != end() )
			(*this)[ i->first ] = i->second;
	}
}


Distribution
Distribution::getTopItems( int n ) const {
	// based on http://stackoverflow.com/a/1367494/361178
	std::vector<intDoublePair> distributionAsVector(begin(), end());
	assert(distributionAsVector.size() >= n);
	std::partial_sort(distributionAsVector.begin(), distributionAsVector.begin() + n, distributionAsVector.end(), probabilityComparator());
	Distribution result;
	for (int i = 0; i < n; ++i) {
		result[ distributionAsVector[i].first ] = distributionAsVector[i].second;
	}
	return result;
}

Distribution 
Distribution::remove( const Distribution& toBeRemoved ) const {
	Distribution result;
	for( const_iterator i = begin(); i != end(); i++ ) {
		if( toBeRemoved.find( i->first ) == toBeRemoved.end() )
			result[ i->first ] = i->second;
	}
	return result;
}

Distribution 
Distribution::keep( const Distribution& toBeKept ) const {
	Distribution result;
	for( const_iterator i = begin(); i != end(); i++ ) {
		if( toBeKept.find( i->first ) != toBeKept.end() )
			result[ i->first ] = i->second;
	}
	return result;
}

Distribution 
Distribution::keepByType( const std::map<int, Node>& nodes, int keeptype ) const {
	Distribution result;
	for( const_iterator probability = begin(); probability != end(); probability++ )
		if( keeptype == 0 ) // keep everything
			result[ probability->first ] = probability->second;
		else if( nodes.at( probability->first ).type == keeptype ) // keep only if keetype matches
			result[ probability->first ] = probability->second;
	return result;
}


Distribution
Distribution::computeScores( const Distribution& prior ) const {
	Distribution result;
	for( const_iterator probability = begin(); probability != end(); probability++ )
		result[ probability->first ] = probability->second / sqrt( prior.at( probability->first ) );
	return result;
}

double
Distribution::getMean() const {
	double sum = 0;
	for( const_iterator i = begin(); i != end(); i++ )
		sum += i->second;
	return sum / size();
}

double
Distribution::getVariance( const double mean ) const {
	double sum = 0;
	for( const_iterator i = begin(); i != end(); i++ )
		sum += ( i->second - mean ) * ( i->second - mean );
	return sum / size();
}

void
Distribution::ensureStandardScore() {
	double mean = getMean();
	double variance = getVariance( mean );
	for( iterator probability = begin(); probability != end(); probability++ ) {
		probability->second = ( probability->second - mean ) / variance;
	}
}

void
Distribution::ensureStochasticity() {
	double sum = 0;
	for( iterator probability = begin(); probability != end(); probability++ )
		sum += probability->second;
	for( iterator probability = begin(); probability != end(); probability++ )
		probability->second /= sum;
}

template< typename tPair >
struct second_t {
    typename tPair::second_type operator()( const tPair& p ) const { return p.second; }
};

template< typename tMap > 
second_t< typename tMap::value_type > second( const tMap& m ) { return second_t< typename tMap::value_type >(); }


int
Distribution::getPosition( int target ) const {
	// create a vector with the values (probabilities) of the distribution map
	std::vector< double > probabilities;
	std::transform( begin(), end(), std::back_inserter( probabilities ), second( (*this) ) );
	
	// sort this vector
	std::sort( probabilities.begin(), probabilities.end(), std::greater<double>() );
	double value = at( target );
	
	// find position of the target's value
	std::vector< double >::iterator positionIterator = std::find( probabilities.begin(), probabilities.end(), value );
	return distance( probabilities.begin(), positionIterator );
}

std::vector< unsigned char >
Distribution::convertToByteVector( int number ) const {
	std::vector< unsigned char > result;
	result.assign( number * ( sizeof( unsigned int ) + sizeof( double ) ), 0 );
	unsigned int counter = 0;
	int offset = 0;
	for( const_iterator i = begin(); i != end() && counter < number; i++ ) {
		int entity = i->first;
		memcpy( &( result[ offset ] ), &entity, sizeof( unsigned int ) );
		offset += sizeof( unsigned int );

		double weight = i->second;
		memcpy( &( result[ offset ] ), &weight, sizeof( double ));
		offset += sizeof( double );
	}
	return result;
}
