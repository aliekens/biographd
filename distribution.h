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
