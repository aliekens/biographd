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

#include "xmlrpcmethods.h"
#include "graph.h"

Collection
getMappedCollection( Graph* graph, int collectionid, int filtertype ) {
	Collection result;
	std::vector< int > collectionentitysetids;
	collectionentitysetids = getCollectionFromMysql( collectionid );
	for( std::vector<int>::iterator entityset = collectionentitysetids.begin(); entityset != collectionentitysetids.end(); entityset++ ) {
		Distribution sources = getEntitySetFromMysql( *entityset );
		int updatedAt = getEntitySetUpdatedAtInSecondsAgoFromMysql( *entityset );
		std::stringstream cachefilename;
		cachefilename << "cache/sets/" << *entityset << ".bin";
		Distribution mapped = graph->computeRandomWalkWithRestart( sources, cachefilename.str(), updatedAt );
//		mapped.ensureStochasticity();
//		mapped.ensureStandardScore();
		mapped.setItems( sources );
		mapped = mapped.keepByType( graph->nodes, filtertype );
		result.push_back( mapped );
	}
	return result;
}

std::map< int, std::pair< double, double > > computeComparison( const Collection& collection1, const Collection& collection2 ) {
	
	std::map< int, std::pair< double, double > > results;
	
	for( Distribution::const_iterator i = collection1[0].begin(); i != collection1[0].end(); i++ ) {
		int concept = i->first;
		
		int n1 = collection1.size();
		int n2 = collection2.size();
		
		// compute means of distributions to be compared
		
		double mean1 = 0;
		for( Collection::const_iterator j = collection1.begin(); j != collection1.end(); j++ ) {
			mean1 += j->at( concept );
		}
		mean1 /= n1;
		
		double mean2 = 0;
		for( Collection::const_iterator j = collection2.begin(); j != collection2.end(); j++ ) {
			mean2 += j->at( concept );
		}
		mean2 /= n2;
		
		// compute variances of distributions to be compared
		
		double variance1 = 0;
		for( Collection::const_iterator j = collection1.begin(); j != collection1.end(); j++ ) {
			variance1 += ( j->at(concept) - mean1 ) * ( j->at(concept) - mean1 );
		}
		variance1 /= n1;
		
		double variance2 = 0;
		for( Collection::const_iterator j = collection2.begin(); j != collection2.end(); j++ ) {
			variance2 += ( j->at(concept) - mean2 ) * ( j->at(concept) - mean2 );
		}
		variance2 /= n2;
		
		// compute fold change
		
		double logratio = log2( mean2 / mean1 );
		
		// compute pvalue
		
		double t = ( mean2 - mean1 ) / ( sqrt( variance1 / n1 + variance2 / n2) ); // t test statistic
		double p = pvalue( t, n1 + n2 ); // compute p value
//		p = p * collection1[ 0 ].size(); // bonferroni correction
		double minuslogp = -log10( p ); // compute -logp for display in volcano plot
		
		results[ concept ] = std::make_pair( logratio, minuslogp );
	}
	
	return results;
	
}
