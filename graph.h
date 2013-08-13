#ifndef GRAPH__H
#define GRAPH__H

#include "path.h"
#include "node.h"
#include "distribution.h"

#include <map>
#include <vector>

class Graph {
public:
	std::map< int, Node > nodes;
	std::map< int /* from */, std::map< int /* to */, double /* weight */ > > edges;
	Distribution pagerank;
	
public:

	// construction of a graph
	Graph();
	void readNodes( std::string filename ); // read nodes from a TSV file
	void readEdges( std::string filename ); // read edges from a TSV file
	void insertEdge( int from, int to, double weight ); // add an edge to the graph
	bool removeNodesByEdgeCount( int minimumNumberOfEdges ); // true = edges removed, false = no edges removed
	void makeGraphStochastic(); // make sure that the sum of outgoing edges equals 1

	// pagerank and allies
	Distribution computePagerank( double dampening = 0.25, double epsilon = 1e-6 );
	Distribution computeRandomWalkWithRestart( const Distribution& restart, double dampening = 0.25, double espilon = 1e-6 );
	Distribution computeRandomWalkWithRestart( const Distribution& restart, std::string filename, int timeout = -1, double dampening = 0.25, double espilon = 1e-6 );
	bool isConverged( const Distribution& oldDistribution, const Distribution& newDistribution, double epsilon = 1e-6 ); // is the new distribution close enough to the old?
	
	// backtracking paths
	double computePathProbability( const Path& path, const Distribution& distribution, const Distribution& sources, int target )  const;
	PathSet computePaths( const Distribution& distribution, const Distribution& sources, int target, int n ) const;

	// read/write distributions as binary files
	Distribution loadDistributionFromFile( std::string filename ) const;
	void saveDistributionToFile( const Distribution& d, std::string filename ) const;
	Distribution removeNeighbors( const Distribution& distribution, const Distribution& toBeRemoved ); // TODO should be made const
	Distribution keepNeighbors( const Distribution& distribution, const Distribution& toBeRemoved ); // TODO should be made const
	
};

#endif
