#include "mysqlconnectorclient.h"

/* Standard C++ headers */
#include <iostream>
#include <sstream>
#include <memory>
#include <string>
#include <stdexcept>
#include <map>
#include <vector>
#include <time.h>

/* MySQL Connector/C++ specific headers */
#include <driver.h>
#include <connection.h>
#include <statement.h>
#include <resultset.h>
#include <exception.h>
#include <warning.h>

#include "mysqlconfig.h"

Distribution 
getEntitySetFromMysql( int id ) {

	Distribution result;

	try {
		sql::Driver *driver = get_driver_instance();
		driver->threadInit();

		sql::Connection *con = driver->connect( (std::string)DBHOST, (std::string)USER, (std::string)PASSWORD);
		con->setSchema( (std::string)DATABASE );

		sql::Statement *stmt = con->createStatement();
		std::stringstream query;
		query << "SELECT concept_id, weight FROM entity_set_lines WHERE entity_set_id = " << id;
		sql::ResultSet *res = stmt->executeQuery( query.str() );

		while (res->next()) {
			result[ res->getInt("concept_id") ] = res->getDouble("weight");
		}

		delete res;
		delete stmt;
//		con->close();
		delete con;
		
		driver->threadEnd();

	} catch (sql::SQLException &e) {
		std::cout << "ERROR: SQLException in " << __FILE__;
		std::cout << " (" << __func__<< ") on line " << __LINE__ << std::endl;
		std::cout << "ERROR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << ")" << std::endl;

		if (e.getErrorCode() == 1047) {
			std::cout << "\nYour server does not seem to support Prepared Statements at all. ";
			std::cout << "Perhaps MYSQL < 4.1?" << std::endl;
		}

	} catch (std::runtime_error &e) {

		std::cout << "ERROR: runtime_error in " << __FILE__;
		std::cout << " (" << __func__ << ") on line " << __LINE__ << std::endl;
		std::cout << "ERROR: " << e.what() << std::endl;

	}

	return result;
}

int 
getEntitySetUpdatedAtInSecondsAgoFromMysql( int id ) {

	int result = 0;

	try {
		sql::Driver *driver = get_driver_instance();
		driver->threadInit();

		sql::Connection *con = driver->connect( (std::string)DBHOST, (std::string)USER, (std::string)PASSWORD);
		con->setSchema( (std::string)DATABASE );

		sql::Statement *stmt = con->createStatement();
		std::stringstream query;
		query << "SELECT TIME_TO_SEC(TIMEDIFF(UTC_TIMESTAMP(),updated_at)) as updated_in_seconds FROM entity_sets WHERE id = " << id;
		sql::ResultSet *res = stmt->executeQuery( query.str() );

		while (res->next()) {
			result = res->getInt("updated_in_seconds");
		}
		
		delete res;
		delete stmt;
//		con->close();
		delete con;
		
		driver->threadEnd();

	} catch (sql::SQLException &e) {
		std::cout << "ERROR: SQLException in " << __FILE__;
		std::cout << " (" << __func__<< ") on line " << __LINE__ << std::endl;
		std::cout << "ERROR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << ")" << std::endl;

		if (e.getErrorCode() == 1047) {
			std::cout << "\nYour server does not seem to support Prepared Statements at all. ";
			std::cout << "Perhaps MYSQL < 4.1?" << std::endl;
		}

	} catch (std::runtime_error &e) {

		std::cout << "ERROR: runtime_error in " << __FILE__;
		std::cout << " (" << __func__ << ") on line " << __LINE__ << std::endl;
		std::cout << "ERROR: " << e.what() << std::endl;

	}

	return result;
}


std::vector< int > 
getCollectionFromMysql( int id ) {
	std::vector< int > result;

	try {
		sql::Driver *driver = get_driver_instance();
		driver->threadInit();

		sql::Connection *con = driver->connect( (std::string)DBHOST, (std::string)USER, (std::string)PASSWORD);
		con->setSchema( (std::string)DATABASE );

		sql::Statement *stmt = con->createStatement();
		std::stringstream query;
		query << "SELECT entity_set_id FROM collection_lines WHERE collection_id = " << id;
		sql::ResultSet *res = stmt->executeQuery( query.str() );

		while (res->next()) {
			result.push_back( res->getInt("entity_set_id") );
		}

		delete res;
		delete stmt;
//		con->close();
		delete con;
		
		driver->threadEnd();

	} catch (sql::SQLException &e) {
		std::cout << "ERROR: SQLException in " << __FILE__;
		std::cout << " (" << __func__<< ") on line " << __LINE__ << std::endl;
		std::cout << "ERROR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << ")" << std::endl;

		if (e.getErrorCode() == 1047) {
			std::cout << "\nYour server does not seem to support Prepared Statements at all. ";
			std::cout << "Perhaps MYSQL < 4.1?" << std::endl;
		}

	} catch (std::runtime_error &e) {

		std::cout << "ERROR: runtime_error in " << __FILE__;
		std::cout << " (" << __func__ << ") on line " << __LINE__ << std::endl;
		std::cout << "ERROR: " << e.what() << std::endl;

	}

	return result;

}
