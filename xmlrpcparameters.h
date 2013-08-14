#ifndef XMLRPCPARAMETERS__H
#define XMLRPCPARAMETERS__H

#include <xmlrpc-c/base.hpp>
#include <map>

class XMLRPCParameters : public std::map< std::string, xmlrpc_c::value> {
public:
	XMLRPCParameters( const std::map< std::string, xmlrpc_c::value>& parameters ) {
		insert( parameters.begin(), parameters.end() );
	}
};

#endif
