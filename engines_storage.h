#include "engines.h"

//int read_data(const char* dataname, ENGINES* database, const unsigned count);
ENGINES* read_data(const char* dataname, unsigned* records_count);
int store_data(const char* data_name, ENGINES* database, const unsigned records_count);