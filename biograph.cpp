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

int main() {
	
	// create a graph from default files, compute prior pagerank
	
	Graph graph;
	
	// define schizophrenia as a distribution
	
	Distribution sources;
	sources[ 8534 ] = 0.5;
	sources[ 1238 ] = 0.5;
	
	// compute RWR of the schizophrenia distribution
	
	std::cerr << "Schizophrenia" << std::flush;
	Distribution mapped = graph.computeRandomWalkWithRestart( sources, "schizophreniabipolar.bin" );
	std::cerr << std::endl;
	
	// compute and filter top scores
	Distribution scores = mapped.computeScores( graph.pagerank );
	Distribution topItems = scores.getTopItems();
	
	// report
	
	for( Distribution::iterator probability = topItems.begin(); probability != topItems.end(); probability++ ) {
		std::cout << graph.nodes[ probability->first ].name << "\t" << probability->second << std::endl;
	}
	
	// compute path from schizophreniaset to cannabidiol
	
	const clock_t begin_time = clock();
	std::vector< Path > paths = graph.computePaths( mapped, sources, 1561, 10 );
	std::cerr << "(" << float( clock () - begin_time ) /  CLOCKS_PER_SEC << " seconds)" << std::endl;
	for( std::vector< Path >::iterator path = paths.begin(); path != paths.end(); path++ ) {
		std::cout << path->probability << "\t";
		for( Path::reverse_iterator i = path->rbegin(); i != path->rend(); i++ ) {
			std::cout << graph.nodes[ *i ].name << "\t";
		}
		std::cout << std::endl;
	}
	
	
}
