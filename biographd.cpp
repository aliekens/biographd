#include "graph.h"
#include "xmlrpcmethods.h"

#include <iostream>
#include <fstream>

#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/registry.hpp>
#include <xmlrpc-c/server_abyss.hpp>

#include <cassert>
#include <stdexcept>
#include <unistd.h>

int main() {
	
	// create a graph from default files, compute prior pagerank
	Graph *graph = new Graph();
	
	try {
		xmlrpc_c::registry myRegistry;
		
		RelatedEntitiesMethod* relatedEntitiesMethod = new RelatedEntitiesMethod( graph );
		xmlrpc_c::methodPtr const relatedEntitiesMethodPointer( relatedEntitiesMethod );
		myRegistry.addMethod( "biograph.relatedentities", relatedEntitiesMethodPointer );
		
		PathsMethod* pathsMethod = new PathsMethod( graph );
		xmlrpc_c::methodPtr const pathsMethodPointer( pathsMethod );
		myRegistry.addMethod( "biograph.paths", pathsMethodPointer );
		
		PositionMethod* positionMethod = new PositionMethod( graph );
		xmlrpc_c::methodPtr const positionMethodPointer( positionMethod );
		myRegistry.addMethod( "biograph.position", positionMethodPointer );
		
		ComparisonMethod* comparisonMethod = new ComparisonMethod( graph );
		xmlrpc_c::methodPtr const comparisonMethodPointer( comparisonMethod );
		myRegistry.addMethod( "biograph.comparison", comparisonMethodPointer );
		
		ComparisonContextMethod* comparisonContextMethod = new ComparisonContextMethod( graph );
		xmlrpc_c::methodPtr const comparisonContextMethodPointer( comparisonContextMethod );
		myRegistry.addMethod( "biograph.comparisoncontext", comparisonContextMethodPointer );
		
		xmlrpc_c::serverAbyss myAbyssServer( myRegistry, 9018, "/tmp/xmlrpc_log" );
		myAbyssServer.run();
		
		assert(false);
		
	} catch (std::exception const& e) {
		
		std::cerr << "Something failed.  " << e.what() << std::endl;
		
	}
	return 0;
	
	
}
