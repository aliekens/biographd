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

#ifndef XMLRPCMETHODS__H
#define XMLRPCMETHODS__H

#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/registry.hpp>

#include <iostream>
#include <fstream>
#include <sstream>

#include <math.h>

#include "graph.h"
#include "xmlrpcparameters.h"
#include "pvalue.h"

#include "mysqlconnectorclient.h"

class RelatedEntitiesMethod : public xmlrpc_c::method {
private:
	Graph* graph;
public:
	RelatedEntitiesMethod( Graph* graph ) {
		this->graph = graph;
	}
	
	void execute( xmlrpc_c::paramList const& parameters, xmlrpc_c::value *const result) {
		
		// consume parameters
		XMLRPCParameters parameterList( parameters.getStruct( 2 ) );
		std::string cache = "";
		if( parameterList.find( "cache") != parameterList.end() )
			cache = xmlrpc_c::value_string( parameterList[ "cache" ] );
		int timeout = -1;
		if( parameterList.find( "timeout") != parameterList.end() )
			timeout = xmlrpc_c::value_int( parameterList[ "timeout" ] );
		int type = 0;
		if( parameterList.find( "type") != parameterList.end() )
			type = xmlrpc_c::value_int( parameterList[ "type" ] );
		int number = 20;
		if( parameterList.find( "number") != parameterList.end() )
			number = xmlrpc_c::value_int( parameterList[ "number" ] );
		int inferred = 0;
		if( parameterList.find( "inferred") != parameterList.end() )
			inferred = xmlrpc_c::value_int( parameterList[ "inferred" ] );
		parameters.verifyEnd( 3 );
		
		// create sources structure
		Distribution sources;
		if( parameterList.find( "source_entityset" ) != parameterList.end() ) {
			int entityset = xmlrpc_c::value_int( parameterList[ "source_entityset" ] );
			sources = getEntitySetFromMysql( entityset );
		} else {
			std::vector<xmlrpc_c::value> const sourceArray( parameters.getArray( 0 ) );
			std::vector<xmlrpc_c::value> const weightArray( parameters.getArray( 1 ) );
			if( sourceArray.size() == weightArray.size() )
				for( int i = 0; i < sourceArray.size(); i++ )
					sources[ xmlrpc_c::value_int( sourceArray[ i ] ) ] = xmlrpc_c::value_double( weightArray[ i ] );
		}
		
		// create targets structure
		Distribution targets;
		if( parameterList.find( "target_entityset" ) != parameterList.end() ) {
			int entityset = xmlrpc_c::value_int( parameterList[ "target_entityset" ] );
			targets = getEntitySetFromMysql( entityset );
		}
		
		// compute RWR for these sources
		Distribution distribution = graph->computeRandomWalkWithRestart( sources, cache, timeout );
		
		// filters
		if( type != 0 )
			distribution = distribution.keepByType( graph->nodes, type );
		distribution = distribution.remove( sources );
		if( inferred == 1 )
			distribution = graph->removeNeighbors( distribution, sources );
		if( inferred == 2 )
			distribution = graph->keepNeighbors( distribution, sources );

		// only if a list of targets is given, we keep only those
		if( targets.size() > 0 )
			distribution = distribution.keep( targets );
		
		// compute scores
		distribution = distribution.computeScores( graph->pagerank );
		
		// get top results
		if( distribution.size() > number )
			distribution = distribution.getTopItems( number );
		
		// pack result
		std::vector< unsigned char > byteVector = distribution.convertToByteVector( number );
		
		// set return value
		*result = xmlrpc_c::value_bytestring( byteVector );

	}

};

class PathsMethod : public xmlrpc_c::method {
private:
	Graph* graph;
public:
	PathsMethod( Graph* graph ) {
		this->graph = graph;
	}
	
	void execute( xmlrpc_c::paramList const& parameters, xmlrpc_c::value *const result) {
		
		// consume parameters
		int const target( parameters.getInt( 2 ) );
		XMLRPCParameters parameterList( parameters.getStruct( 3 ) );
		std::string cache = "";
		if( parameterList.find( "cache") != parameterList.end() )
			cache = xmlrpc_c::value_string( parameterList[ "cache" ] );
		int timeout = -1;
		if( parameterList.find( "timeout") != parameterList.end() )
			timeout = xmlrpc_c::value_int( parameterList[ "timeout" ] );
		int number = 20;
		if( parameterList.find( "number") != parameterList.end() )
			number = xmlrpc_c::value_int( parameterList[ "number" ] );
		parameters.verifyEnd( 4 );
		
		// create sources structure
		Distribution sources;
		if( parameterList.find( "entityset" ) != parameterList.end() ) {
			int entityset = xmlrpc_c::value_int( parameterList[ "entityset" ] );
			sources = getEntitySetFromMysql( entityset );
		} else {
			std::vector<xmlrpc_c::value> const sourceArray( parameters.getArray( 0 ) );
			std::vector<xmlrpc_c::value> const weightArray( parameters.getArray( 1 ) );
			if( sourceArray.size() == weightArray.size() )
				for( int i = 0; i < sourceArray.size(); i++ )
					sources[ xmlrpc_c::value_int( sourceArray[ i ] ) ] = xmlrpc_c::value_double( weightArray[ i ] );
		}
		
		// compute RWR for these sources
		Distribution distribution = graph->computeRandomWalkWithRestart( sources, cache, timeout );
		
		// compute paths
		PathSet paths = graph->computePaths( distribution, sources, target, number );
		std::cout << paths.size() << std::endl;
		
		// pack result
		std::vector< unsigned char > byteVector = paths.convertToByteVector( number );
		
		// set return value
		*result = xmlrpc_c::value_bytestring( byteVector );

	}

};

class PositionMethod : public xmlrpc_c::method {
private:
	Graph* graph;
public:
	PositionMethod( Graph* graph ) {
		this->graph = graph;
	}
	
	void execute( xmlrpc_c::paramList const& parameters, xmlrpc_c::value *const result) {
		
		// consume parameters
		std::vector< xmlrpc_c::value > const targets( parameters.getArray( 2 ) );
		XMLRPCParameters parameterList( parameters.getStruct( 3 ) );
		std::string cache = "";
		if( parameterList.find( "cache") != parameterList.end() )
			cache = xmlrpc_c::value_string( parameterList[ "cache" ] );
		int timeout = -1;
		if( parameterList.find( "timeout") != parameterList.end() )
			timeout = xmlrpc_c::value_int( parameterList[ "timeout" ] );
		parameters.verifyEnd( 4 );
		
		// create sources structure
		Distribution sources;
		if( parameterList.find( "entityset" ) != parameterList.end() ) {
			int entityset = xmlrpc_c::value_int( parameterList[ "entityset" ] );
			sources = getEntitySetFromMysql( entityset );
		} else {
			std::vector<xmlrpc_c::value> const sourceArray( parameters.getArray( 0 ) );
			std::vector<xmlrpc_c::value> const weightArray( parameters.getArray( 1 ) );
			if( sourceArray.size() == weightArray.size() )
				for( int i = 0; i < sourceArray.size(); i++ )
					sources[ xmlrpc_c::value_int( sourceArray[ i ] ) ] = xmlrpc_c::value_double( weightArray[ i ] );
		}
		
		// compute RWR for these sources
		Distribution distribution = graph->computeRandomWalkWithRestart( sources, cache, timeout );
		
		// remove sources
		distribution = distribution.remove( sources );
		
		// compute scores
		distribution = distribution.computeScores( graph->pagerank );
		
		// result bytestream to be returned
		std::vector< unsigned char > byteVector;
		byteVector.assign( targets.size() * ( sizeof( double ) + 5 * sizeof( int ) ), 0 ); // 5 ints and a double for each target
		int offset = 0;
		
		for( std::vector< xmlrpc_c::value >::const_iterator i = targets.begin(); i != targets.end(); i++ ) {
			
			int target = xmlrpc_c::value_int( *i );
			
			// get position
			double score = distribution[ target ];
			int position = distribution.getPosition( target);
			int total = distribution.size();
			
			// get position by type
			std::map< int, Distribution > distributionByType;
			int type = graph->nodes[ target ].type;
			if( distributionByType.find( type ) == distributionByType.end() )
				distributionByType[ type ] = distribution.keepByType( graph->nodes, type );
			int positionByType = distributionByType[ type ].getPosition( target );
			int totalByType = distributionByType[ type ].size();
			
			memcpy( &( byteVector[ offset ] ), &target, sizeof( int ) );
			offset += sizeof( int );
			memcpy( &( byteVector[ offset ] ), &score, sizeof( double ) );
			offset += sizeof( double );
			memcpy( &( byteVector[ offset ] ), &position, sizeof( int ) );
			offset += sizeof( int );
			memcpy( &( byteVector[ offset ] ), &total, sizeof( int ) );
			offset += sizeof( int );
			memcpy( &( byteVector[ offset ] ), &positionByType, sizeof( int ) );
			offset += sizeof( int );
			memcpy( &( byteVector[ offset ] ), &totalByType, sizeof( int ) );
			offset += sizeof( int );
		
		}
		
		// set return value
		*result = xmlrpc_c::value_bytestring( byteVector );

	}

};

class Collection : public std::vector< Distribution > {};

Collection getMappedCollection( Graph* graph, int collectionid, int filtertype );
std::map< int, std::pair< double, double > > computeComparison( const Collection& collection1, const Collection& collection2 );


class ComparisonMethod : public xmlrpc_c::method {
private:
	Graph* graph;
public:
	ComparisonMethod( Graph* graph ) {
		this->graph = graph;
	}
	
	void execute( xmlrpc_c::paramList const& parameters, xmlrpc_c::value *const result) {
		
		// consume parameters
		XMLRPCParameters parameterList( parameters.getStruct( 0 ) );
		
		// map collections of sources
		
		int filtertype = 3;
		if( parameterList.find( "filtertype" ) != parameterList.end() ) {
			filtertype = xmlrpc_c::value_int( parameterList[ "filtertype" ] );
		}
		
		int collection1id;
		if( parameterList.find( "collection1id" ) != parameterList.end() ) {
			collection1id = xmlrpc_c::value_int( parameterList[ "collection1id" ] );
		}
		
		int collection2id;
		if( parameterList.find( "collection2id" ) != parameterList.end() ) {
			collection2id = xmlrpc_c::value_int( parameterList[ "collection2id" ] );
		}
		
		// compute statistics (logratio, pvalue) for each datapoint
		Collection collection1 = getMappedCollection( graph, collection1id, filtertype );
		Collection collection2 = getMappedCollection( graph, collection2id, filtertype );
		std::map< int, std::pair< double, double > > results = computeComparison( collection1, collection2 );
		
		// return value
		int size = results.size();
		std::vector< unsigned char > byteVector;
		byteVector.assign( sizeof( unsigned int ) + size * ( sizeof( unsigned int ) + 2 * sizeof( double ) ), 0 );
		unsigned int counter = 0;
		int offset = 0;
		memcpy( &( byteVector[ offset ] ), &size, sizeof( unsigned int ) );
		offset += sizeof( unsigned int );
		
		for( std::map< int, std::pair< double, double > >::iterator i = results.begin(); i != results.end(); i++ ) {
			// add result to byteVector to be returned
			
			memcpy( &( byteVector[ offset ] ), &(i->first), sizeof( unsigned int ) );
			offset += sizeof( unsigned int );
			
			memcpy( &( byteVector[ offset ] ), &(i->second.first), sizeof( double ));
			offset += sizeof( double );
			
			memcpy( &( byteVector[ offset ] ), &(i->second.second), sizeof( double ));
			offset += sizeof( double );
		
		}
		
		// set return value
		*result = xmlrpc_c::value_bytestring( byteVector );
		
	}

};

class ComparisonContextMethod : public xmlrpc_c::method {
private:
	Graph* graph;
public:
	ComparisonContextMethod( Graph* graph ) {
		this->graph = graph;
	}
	
	void execute( xmlrpc_c::paramList const& parameters, xmlrpc_c::value *const result) {
		
		// consume parameters
		XMLRPCParameters parameterList( parameters.getStruct( 0 ) );
		
		// map collections of sources
		
		int filtertype = 0;
		
		int collection1id;
		if( parameterList.find( "collection1id" ) != parameterList.end() ) {
			collection1id = xmlrpc_c::value_int( parameterList[ "collection1id" ] );
		}
		
		int collection2id;
		if( parameterList.find( "collection2id" ) != parameterList.end() ) {
			collection2id = xmlrpc_c::value_int( parameterList[ "collection2id" ] );
		}
		
		int conceptid;
		if( parameterList.find( "concept" ) != parameterList.end() ) {
			conceptid = xmlrpc_c::value_int( parameterList[ "concept" ] );
		}
		
		int number = 20;
		if( parameterList.find( "number") != parameterList.end() )
			number = xmlrpc_c::value_int( parameterList[ "number" ] );

		// compute statistics (logratio, pvalue) for each datapoint
		Collection collection1 = getMappedCollection( graph, collection1id, filtertype );
		Collection collection2 = getMappedCollection( graph, collection2id, filtertype );
		std::map< int, std::pair< double, double > > results = computeComparison( collection1, collection2 );
		
		// extract map
		// start with initial set that contains current concept
		
		std::map< int, std::pair< double, double > > relevantNeighbors; // doubles = probability to reach neighbor & foldchange
		relevantNeighbors[ conceptid ] = std::make_pair< double, double >( 1, results[ conceptid ].first );
		std::map< int, std::pair< double, double > > availableNeighbors;
		
		// add neighbors to set of available neighbors
		for( std::map< int /* to */, double /* weight */ >::iterator neighbor = graph->edges[ conceptid ].begin(); neighbor != graph->edges[ conceptid ].end(); neighbor++ ) {
			availableNeighbors[ neighbor->first ] = 
				std::make_pair< double, double >( 
					1.0 / graph->edges[ conceptid ].size(), 
					results[ neighbor->first ].first 
				);
		}
		
		for( int counter = 0; counter < number - 1; counter++ ) {
		
			// find concept among available neighbors with most relevant fold change
			double bestFoldChange = -1000;
			
			int bestNeighbor = 0;
			for( std::map< int, std::pair< double, double > >::iterator i = availableNeighbors.begin(); i != availableNeighbors.end(); i++ ) {
				double foldChange = fabs( i->second.first * i->second.second );
				if( foldChange > bestFoldChange ) {
					bestFoldChange = foldChange;
					bestNeighbor = i->first;
				}
			}
		
			// add concept with most relevant fold change to relevant neighbors
			relevantNeighbors[ bestNeighbor ] = relevantNeighbors[ bestNeighbor ];
		
			// remove concept with most relevant fold change from available neighbors
			availableNeighbors.erase( bestNeighbor );
		
			// add neighbors of bestNeighbor to set of available neighbors
			double probability = relevantNeighbors[ bestNeighbor ].first;
			for( std::map< int /* to */, double /* weight */ >::iterator neighbor = graph->edges[ bestNeighbor ].begin(); neighbor != graph->edges[ bestNeighbor ].end(); neighbor++ ) {
				if( relevantNeighbors.find( neighbor->first ) == relevantNeighbors.end() ) { // only add neighbors that we don't already have
					if( availableNeighbors.find( neighbor->first ) == availableNeighbors.end() ) { // add a neighbor if it's not in the list of availableneighbors
						availableNeighbors[ neighbor->first ] = 
							std::make_pair< double, double >( 
								probability / graph->edges[ bestNeighbor ].size(), 
								results[ neighbor->first ].first 
							);
					} else { // add a neighbor if it's already in the list of available neighbors
						availableNeighbors[ neighbor->first ] = 
							std::make_pair< double, double >( 
								availableNeighbors[ neighbor->first ].first + probability / graph->edges[ bestNeighbor ].size(), 
								results[ neighbor->first ].first 
							);
					}
				}
			}
		
		}
		
		// print out connectivity matrix
		/*
		for( std::map< int, std::pair< double, double > >::iterator i = relevantNeighbors.begin(); i != relevantNeighbors.end(); i++ ) {
			if( conceptid == i->first )
				std::cout << "!";
			else
				std::cout << " ";
			for( std::map< int, std::pair< double, double > >::iterator j = relevantNeighbors.begin(); j != relevantNeighbors.end(); j++ ) {
				if( graph->edges[ i->first ].find( j->first ) != graph->edges[ i->first ].end() )
					std::cout << "X";
				else
					std::cout << ".";
			}
			std::cout << std::endl;
		}
		*/
		
		// return value
		std::vector< unsigned char > byteVector;
		byteVector.assign( 
			sizeof( unsigned int ) + relevantNeighbors.size() * ( sizeof( unsigned int ) + 2 * sizeof( double ) ) + // relevant neighbors with pvalue and foldchange
			sizeof( unsigned int ) + collection1.size() * sizeof( double ) + // collection 1's weights
			sizeof( unsigned int ) + collection2.size() * sizeof( double ) // collection 2's weights
			, 0 );
		int offset = 0;
		
		// relevant neighbors
		// size
		int size = relevantNeighbors.size();
		memcpy( &( byteVector[ offset ] ), &size, sizeof( unsigned int ) );
		offset += sizeof( unsigned int );
		
		// relevant neighbor by ID, with logratio and -logpvalue
		for( std::map< int, std::pair< double, double > >::iterator i = relevantNeighbors.begin(); i != relevantNeighbors.end(); i++ ) {
			// add result to byteVector to be returned
			
			memcpy( &( byteVector[ offset ] ), &(i->first), sizeof( unsigned int ) );
			offset += sizeof( unsigned int );
			
			memcpy( &( byteVector[ offset ] ), &(results[i->first].first), sizeof( double ));
			offset += sizeof( double );
			
			memcpy( &( byteVector[ offset ] ), &(results[i->first].second), sizeof( double ));
			offset += sizeof( double );
		
		}
		
		// weights of conceptid in the collections
		
		// collection1
		// size
		size = collection1.size();
		memcpy( &( byteVector[ offset ] ), &(size), sizeof( unsigned int ) );
		offset += sizeof( unsigned int );
		// elements
		for( Collection::iterator distribution = collection1.begin(); distribution != collection1.end(); distribution++ ) {
			double value = distribution->at( conceptid );
			memcpy( &( byteVector[ offset ] ), &(value), sizeof( double ) );
			offset += sizeof( double );
		}
		
		// collection2
		// size
		size = collection2.size();
		memcpy( &( byteVector[ offset ] ), &(size), sizeof( unsigned int ) );
		offset += sizeof( unsigned int );
		// elements
		for( Collection::iterator distribution = collection2.begin(); distribution != collection2.end(); distribution++ ) {
			double value = distribution->at( conceptid );
			memcpy( &( byteVector[ offset ] ), &(value), sizeof( double ) );
			offset += sizeof( double );
		}
		
		// set return value
		*result = xmlrpc_c::value_bytestring( byteVector );
		
	}

};

#endif
