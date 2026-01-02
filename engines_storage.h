#include "engines.h"

void free_database(ENGINES* database, const unsigned count);
void free_supported_platforms(struct supported_platforms* head);
ENGINES* read_data(const char* dataname, unsigned* records_count);
int store_data(const char* data_name, ENGINES* database, const unsigned records_count);