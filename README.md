biographd
=========

BioGraph computing kernel daemon

Provides an XMLRPC service to compute pageranks, find related entities, find paths between entities, from entities or entity sets, does comparisons between collections. An example client library for ruby (`biographclient.rb`) and test client (`client.rb`) are provided.


Requirements
------------

* Mysql Connector Client C++: http://dev.mysql.com/tech-resources/articles/building-mysql-connector-cpp.html
    * TODO: requires tweaking of Makefile for correct path of mysqldev & mysqlconnector

Run  it
-------

* Create directory structure with `cache/`, `cache/pagerank/`, `cache/entities/`, `cache/sets/`
* Start `./biographd`
    * TODO: daemonize
* Test with `./client.rb`
    * TODO: know where the mysql database is, now hardcoded
