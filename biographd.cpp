#include "graph.h"
#include "xmlrpcmethods.h"

#include <iostream>
#include <fstream>

#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/registry.hpp>
#include <xmlrpc-c/server_abyss.hpp>

#include <cassert>
#include <stdexcept>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <unistd.h>
#include <sys/stat.h>

#define LOCK_FILE	"/tmp/biographdaemon.lock"

void daemonize() {
	int i=fork();
	if (i<0) exit(1); /* fork error */
	if (i>0) exit(0); /* parent exits */
	/* child (daemon) continues */
	setsid(); /* obtain a new process group */
	for (i=getdtablesize();i>=0;--i) close(i); /* close all descriptors */
	i=open("/dev/null",O_RDWR); dup(i); dup(i); /* handle standart I/O */
	umask(027); /* set newly created file permissions */
	int lfp=open(LOCK_FILE,O_RDWR|O_CREAT,0640);
	if (lfp<0) exit(1); /* can not open */
	if (lockf(lfp,F_TLOCK,0)<0) exit(0); /* can not lock */
	/* first instance continues */
	char str[10];
	sprintf(str,"%d\n",getpid());
	write(lfp,str,strlen(str)); /* record pid to lockfile */
}

int main() {
	
	// create a graph from default files, compute prior pagerank
	Graph *graph = new Graph();
	
	std::cerr << "Starting daemon." << std::endl;
	daemonize();	
	
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
