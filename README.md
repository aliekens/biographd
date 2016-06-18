biographd
=========

BioGraph computing kernel daemon

Provides an XMLRPC service to compute pageranks, find related entities, find paths between entities, from entities or entity sets, does comparisons between collections. An example client library for ruby (`biographclient.rb`) and test client (`client.rb`) are provided.

About Biograph
--------------

[Biograph](http://biograph.be) (developed at [UA](http://uantwerpen.be) and [VIB](http://www.vib.be)) is a web service that integrates biomedical knowledge bases and allows researchers to relate various biomedical concepts to candidate targets, for example to investigate potential genetic candidates for hereditary diseases, or to find chemical compounds to target disease-related proteins. Biomedical relations among seemingly unrelated concepts are supported by relevant automated functional hypotheses.

Please refer to our open access journal paper for more information about Biograph: 

A.M.L. Liekens, J. De Knijf J, W. Daelemans, B. Goethals, P. De Rijk, J. Del-Favero J, [BioGraph: Unsupervised Biomedical Knowledge Discovery via Automated Hypothesis Generation](https://genomebiology.biomedcentral.com/articles/10.1186/gb-2011-12-6-r57), Genome Biology 12:R57, 2011.

The source code for Biograph is hereby made available under a GPL v3 license.

Requirements
------------

* XMLRPC for C: http://xmlrpc-c.sourceforge.net/
* Mysql Connector Client C++: http://dev.mysql.com/tech-resources/articles/building-mysql-connector-cpp.html
    * TODO: requires tweaking of Makefile for correct path of mysqldev & mysqlconnector

Compile it
----------

* Copy `mysqlconfig.h.sample` to `mysqlconfig.h` and configure
* Run `make`

Run  it
-------

* Start `./biographd`
* Test with `./client.rb`
    * TODO: know where the mysql database is, now hardcoded
