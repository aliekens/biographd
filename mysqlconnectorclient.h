#include <map>
#include "distribution.h"

Distribution getEntitySetFromMysql( int id );
int getEntitySetUpdatedAtInSecondsAgoFromMysql( int id );

std::vector< int > getCollectionFromMysql( int id );
