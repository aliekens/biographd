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

#include "graph.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <ctime>
#include <fstream>
#include <sys/stat.h>

#define foreach( i, c )\
	typedef __typeof__( c ) c##_CONTAINERTYPE;\
	for( c##_CONTAINERTYPE::iterator i = c.begin(); i != c.end(); ++i )

#define const_foreach( i, c )\
	typedef __typeof__( c ) c##_CONTAINERTYPE;\
	for( c##_CONTAINERTYPE::const_iterator i = c.begin(); i != c.end(); ++i )

// construction of a graph

Graph::Graph() {

	// read nodes
	std::cerr << "Reading nodes" << std::flush;
	readNodes( "concepts.tsv" );
	std::cerr << " (" << nodes.size() << " nodes)." << std::endl;

	// read edges
	std::cerr << "Reading edges" << std::flush;
	readEdges( "relations.tsv" );
	std::cerr << " (" << edges.size() << " nodes have outgoing edges)." << std::endl;

	// determine connected component by repeatedly removing edges (ends with a false when no more edges removed)
	std::cerr << "Removing loose ends" << std::flush;
	while( removeNodesByEdgeCount( 2 ) );
	std::cerr << " (" << nodes.size() << " nodes and " << edges.size() << " nodes with outgoing edges remain)." << std::endl;
	
	// make sure that the sum of outgoing edges for each node equals 1
	std::cerr << "Ensuring stochasticity" << std::flush;
	makeGraphStochastic();
	std::cerr << "." << std::endl;
	
	std::cerr << "Computing prior pagerank" << std::endl;
	pagerank = computePagerank( 0.25, 1e-8 );
	
}

void
Graph::readNodes( std::string filename ) {
	std::ifstream ifs( filename.c_str() );
	int id, type;
	std::string name;
	while( ifs >> id >> name >> type ) {
		Node node;
		node.id = id;
		node.name = name;
		node.type = type;
		nodes[ id ] = node;
	}
}

void
Graph::readEdges( std::string filename ) {
	std::ifstream ifs( filename.c_str() );
	std::string line;
	// read lines from the file
	while (std::getline(ifs, line))
	{
		std::istringstream iss(line);
		int id, from, to;
		double weight;
		// read fields from the line
		if (!(iss >> id >> from >> to >> weight)) { break; }
		// if these nodes exist, add the connecting edges
		if( ( nodes.find( from ) != nodes.end() ) && ( nodes.find( to ) != nodes.end() ) ) {
			insertEdge( from, to, weight );
			insertEdge( to, from, weight );
		}
	}
}

void
Graph::insertEdge( int from, int to, double weight ) {
	if ( edges.find( from ) == edges.end() ) {
		edges[ from ] = std::map< int, double >();
	}
	edges[ from ][ to ] = weight;
}

bool
Graph::removeNodesByEdgeCount( int minimumNumberOfEdges ) {

	// find nodes that only have less than minimumNumberOfEdges
	std::vector< int > toBeRemoved;
	foreach( node, nodes ) {
		if( edges[ node->first ].size() < minimumNumberOfEdges ) {
			toBeRemoved.push_back( node->first );
		}
	}
	
	// for each node that needs to be removed
	foreach( nodeId, toBeRemoved ) {
		
		// remove this node from its neighbors' edges
		std::map<int, double> neighbors = edges[ *nodeId ];
		foreach( neighbor, neighbors ) {
			edges[ neighbor->first ].erase( *nodeId );
		}
		
		// remove this node from the graph
		edges.erase( *nodeId );
		nodes.erase( *nodeId );
	}

	return( toBeRemoved.size() > 0 );
}

void
Graph::makeGraphStochastic() {
	foreach( edge, edges ) {
		std::map<int, double> neighbors = edge->second;
		double sum = 0;
		foreach( neighbor, neighbors ) {
			sum += neighbor->second;
		}
		for( std::map<int, double>::iterator neighbor = edge->second.begin(); neighbor != edge->second.end(); neighbor++ ) {
			neighbor->second /= sum;
		}
	}
}

// pagerank and allies

Distribution
Graph::computeRandomWalkWithRestart( const Distribution& restart, double dampening, double epsilon ) {
	
	// initial distribution
	Distribution restartDistribution;
	foreach( node, nodes ) {
		restartDistribution[ node->first ] = 0;
	}
	double sum = 0;
	const_foreach( probability, restart ) {
		restartDistribution[ probability->first ] = probability->second;
		sum += probability->second;
	}

	// initial distribution
	Distribution distribution;
	distribution.insert( restartDistribution.begin(), restartDistribution.end() );
	
	// step
	
	bool converged = false;
	do {
		
		// create a new distribution
		Distribution newDistribution;
		foreach( node, nodes ) {
			newDistribution[ node->first ] = 0;
		}
	
		// distribute weights
		foreach( node, nodes ) {
			std::map<int, double> neighbors = edges[ node->first ];
			foreach( neighbor, neighbors ) {
				newDistribution[ neighbor->first ] += distribution[ node->first ] * neighbor->second;
			}
		}
	
		// dampening
		foreach( node, nodes ) {
			newDistribution[ node->first ] = ( 1.0 - dampening ) * newDistribution[ node->first ] + dampening * restartDistribution[ node->first ];
		}
		
		converged = isConverged( distribution, newDistribution, epsilon * sum );
	
		distribution.clear();
		distribution.insert( newDistribution.begin(), newDistribution.end() );
	
	} while( !converged );
	
	return distribution;
	
}

time_t
secondsSinceModification( const std::string& filename ) {
	struct stat attrib;
	stat( filename.c_str() , &attrib );
	time_t now;
	time( &now );
	return now - attrib.st_mtime;
}

Distribution
Graph::computeRandomWalkWithRestart( const Distribution& restart, std::string filename, int timeout, double dampening, double epsilon ) {
	Distribution result;
	const clock_t begin_time = clock();
	std::stringstream logMessage;
	std::ifstream ifs( filename.c_str() );
	if( ifs.good() ) { // cache exists and is readable
		ifs.close();
		time_t cacheTime = secondsSinceModification( filename );
		if( ( cacheTime > timeout ) && ( timeout != -1 ) ) { // cache has timed out, recreate it
			result = computeRandomWalkWithRestart( restart, dampening, epsilon );
			saveDistributionToFile( result, filename );
			logMessage << filename << "\t" << restart.size() << "\t200 OK\t";
		} else { // cache is OK, read and return
			result = loadDistributionFromFile( filename );
			logMessage << filename << "\t" << restart.size() << "\t304 Not Modified\t";
		}
	} else { // cache does not exist, create it
		result = computeRandomWalkWithRestart( restart, dampening, epsilon );
		saveDistributionToFile( result, filename );
		logMessage << filename << "\t" << restart.size() << "\t200 OK\t";
	}
	logMessage << "(" << float( clock () - begin_time ) /  CLOCKS_PER_SEC << " seconds)";
	std::cerr << logMessage.str() << std::endl;
	return result;
}

Distribution
Graph::computePagerank( double dampening, double epsilon ) {
	
	// restart distribution for RWR
	Distribution distribution;
	foreach( node, nodes ) {
		distribution[ node->first ] = 1.0 / nodes.size();
	}
	
	return computeRandomWalkWithRestart( distribution, "cache/pagerank/pagerank.bin", -1, dampening, epsilon );
	
}

bool
Graph::isConverged( const Distribution& oldDistribution, const Distribution& newDistribution, double epsilon ) {
	foreach( node, nodes ) {
		double d = fabs( newDistribution.at( node->first ) - oldDistribution.at( node->first ) );
		if( d > epsilon )
			return false;
	}
	return true;
}

// backtracking paths

double
Graph::computePathProbability( const Path& path, const Distribution& distribution, const Distribution& sources, int target )  const {

	double probability = 1;

	if( sources.find( path.back() ) != sources.end() ) { 
		
		// reached source, probability is based on probability of this source in the source distribution
		probability *= sources.at( path.back() );
		
	} else { 

		// didn't reach source, partial path, probability is based on distribution of random walk
		probability *= distribution.at( path.back() );
		
	}
	
	// probabilities of following the path on its intermediate steps from source to target
	for( int i = 1; i < path.size(); i++ ) {
		probability *= edges.at( path[ i ] ).at( path[ i - 1 ] );
	}
	
	return probability;
}

bool pathComp( const Path& path1, const Path& path2 ) {
	return ( path1.probability > path2.probability );
}

PathSet
Graph::computePaths( const Distribution& distribution, const Distribution& sources, int target, int n ) const {
	
	PathSet result;
	
	// set of paths
	
	PathSet paths;
	// initial path only contains target
	
	Path p;
	p.push_back( target );
	paths.push_back( p );
	
	// extend paths
	
	int counter = 0;
	
	do {
		
		counter += 1;
		
		PathSet newPaths;
		
		// check which paths to extend
		
		foreach( path, paths ) {
			
			if( sources.find( path->back() ) != sources.end() ) { 
			
				// path reached source, no need to extend, add to newPaths as is
				result.push_back( *path );
				
			} else { 
			
				// extend path toward neighbors
				std::map< int, double > neighbors = edges.at( path->back() );
				foreach( neighbor, neighbors ) {

					// avoid cycles
					if( std::find( path->begin(), path->end(), neighbor->first ) == path->end() ) {
						
						// create an extended path and add to newPaths
						Path extendedPath;
						extendedPath.insert( extendedPath.begin(), path->begin(), path->end() );
						extendedPath.push_back( neighbor->first );
						newPaths.push_back( extendedPath );
						
					}
					
				}
			}
		}
		
		if( result.size() < n ) {
			
			// compute all partial paths' probabilities
			foreach( path, newPaths ) {
				path->probability = computePathProbability( *path, distribution, sources, target );
			}
			
			// only keep top 100
			std::sort( newPaths.begin(), newPaths.end(), pathComp );
			paths.clear();
			if( newPaths.size() > 100 )
				paths.insert( paths.begin(), newPaths.begin(), newPaths.begin() + 100 );
			else
				paths.insert( paths.begin(), newPaths.begin(), newPaths.end() );
	
		}
	
	} while( ( result.size() < n ) && ( counter < 20 ) );
	
	foreach( path, result ) {
		path->probability = computePathProbability( *path, distribution, sources, target );
	}
	
	return result.getTopItems( n );
}

// read/write distributions as binary files


void
saveVectorToFile( const std::vector< double > v, std::string filename ) {
	std::ofstream ofs( filename.c_str() );
	ofs.write(
		reinterpret_cast<const char*>(&v[0]), 
		v.size() * sizeof( v[0] )
	);
	ofs.close();
}

void 
Graph::saveDistributionToFile( const Distribution& distribution, std::string filename ) const {
	std::vector< double > values;
	const_foreach( node, nodes ) {
		if( distribution.find( node->first ) != distribution.end() ) {
			values.push_back( distribution.at( node->first ) );
		} else {
			values.push_back( 0 );
		}
	}
	saveVectorToFile( values, filename );
}

std::vector< double >
loadVectorFromFile( std::string filename, int size ) {
	std::ifstream ifs( filename.c_str() );
	std::vector< double > result;
	result.resize( size );
	ifs.read( 
		reinterpret_cast<char*>(&result[0]), 
		size * sizeof( result[0] )
	);
	return result;
}

Distribution
Graph::loadDistributionFromFile( std::string filename ) const {
	std::vector< double > values = loadVectorFromFile( filename, nodes.size() );
	Distribution result;
	std::vector< double >::iterator it = values.begin();
	const_foreach( node, nodes ) {
		result[ node->first ] = *it;
		it++;
	}
	return result;
}

Distribution
Graph::removeNeighbors( const Distribution& distribution, const Distribution& toBeRemoved ) {
	Distribution neighborsToBeRemoved;
	for( Distribution::const_iterator node = toBeRemoved.begin(); node != toBeRemoved.end(); node++ ) {
		std::map< int, double > neighborEdges( edges[ node->first ] );
		for( std::map< int, double >::const_iterator edge = neighborEdges.begin(); edge != neighborEdges.end(); edge++ ) {
			neighborsToBeRemoved[ edge->first ] = 0;
		}
	}
	return distribution.remove( neighborsToBeRemoved );
}

Distribution
Graph::keepNeighbors( const Distribution& distribution, const Distribution& toBeKept ) {
	Distribution neighborsToBeKept;
	for( Distribution::const_iterator node = toBeKept.begin(); node != toBeKept.end(); node++ ) {
		std::map< int, double > neighborEdges( edges[ node->first ] );
		for( std::map< int, double >::const_iterator edge = neighborEdges.begin(); edge != neighborEdges.end(); edge++ ) {
			neighborsToBeKept[ edge->first ] = 0;
		}
	}
	return distribution.keep( neighborsToBeKept );
}
