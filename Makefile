OBJECTS=graph.o node.o distribution.o path.o xmlrpcmethods.o xmlrpcparameters.o mysqlconnectorclient.o pvalue.o

XMLRPC_C_CONFIG = /usr/local/bin/xmlrpc-c-config
LDADD_SERVER_ABYSS = \
	$(shell $(XMLRPC_C_CONFIG) c++2 abyss-server --ldadd)
LDADD_MYSQL_CONNECTOR = \
	-lmysqlclient_r -lmysqlcppconn -I/usr/local/include/cppconn -L/usr/lib64/mysql/ -L/usr/local/lib -L/usr/local/mysql-5.1.54-osx10.6-x86_64/lib/

all: biograph biographd

biographd: biographd.cpp $(OBJECTS)
	g++ -O3 biographd.cpp -o biographd $(OBJECTS) $(LDADD_SERVER_ABYSS) $(LDADD_MYSQL_CONNECTOR)

biograph: biograph.cpp $(OBJECTS)
	g++ -O3 biograph.cpp -o biograph $(OBJECTS) $(LDADD_MYSQL_CONNECTOR)

%.o: %.cpp %.h
	g++ -O3 -c -o $@ $< -I/usr/local/include/cppconn

clean:
	rm -f *~ *.o biographd biograph
