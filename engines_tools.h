#include "engines.h"

int print_record(ENGINES* database, const unsigned count);
int init_record(ENGINES* database, const unsigned count, \
	const char* name, \
	const char* tech_render, \
	const unsigned polygons, \
	const struct supported_platforms* supported_platforms, \
	const char physics_quality, \
	const unsigned license_cost, \
	const char* community, \
	const char* doc, \
	const float rating);