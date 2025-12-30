#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "engines_tools.h"

void gnome_sort(ENGINES* database, const unsigned count)
{
	int index = 1;
	while (index < count)
	{
		if (sqrt(database[index].polygons) >= sqrt(database[index - 1].polygons))
			index++;
		else
		{
			ENGINES temp = database[index];
			database[index] = database[index - 1];
			database[index - 1] = temp;
			index--;
		}
	}
}
int print_record(ENGINES* database, const unsigned count)
{
	for (int i = 0; i < count; ++i)
	{
		printf("=============================================================================================================================\n");
		printf("Запись %d\n", i+1);
		printf("-----------------------------------------------------------------------------------------------------------------------------\n");
		printf("| наименование    | тех.рендер    | макс.полигонов |                       поддерживаемые платформы                         |\n");
		printf("-----------------------------------------------------------------------------------------------------------------------------\n");
		printf("| %-15s | %-13s | %-14d | ", database[i].name, \
			database[i].tech_render, \
			database[i].polygons);
		struct supported_platforms* supported_platforms = database[i].supported_platforms;
		while (supported_platforms)
		{
			printf("%-8s | ", supported_platforms->name);
			supported_platforms = supported_platforms->next;
		}
		printf("\n---------------------------------------------------------------------------------------------------------------------------\n");
		printf("| качество физики | цена лицензии | сообщество                            | документация                          | рейтинг |\n");
		printf("-----------------------------------------------------------------------------------------------------------------------------\n");
		printf("| %-15c | %-13d | %-14s | %-12s | %-7f |\n", database[i].physics_quality, \
			database[i].license_cost, \
			database[i].community, \
			database[i].doc, \
			database[i].rating);
	}

	return 0;
}
int init_record(ENGINES* database, const unsigned count, \
	const char* name, \
	const char* tech_render, \
	const unsigned polygons, \
	const struct supported_platforms* supported_platforms, \
	const char physics_quality, \
	const unsigned license_cost, \
	const char* community, \
	const char* doc, \
	const float rating)
{
	database[count - 1].name = (char*)malloc(strlen(name) + 1);
	database[count - 1].tech_render = (char*)malloc(strlen(tech_render) + 1);
	database[count - 1].community = (char*)malloc(strlen(community) + 1);
	database[count - 1].doc = (char*)malloc(strlen(doc) + 1);

	if (!database[count - 1].name || !database[count - 1].tech_render ||
		!database[count - 1].community || !database[count - 1].doc) {
		return 1;  // Ошибка выделения памяти
	}

	strcpy(database[count - 1].name, name);
	strcpy(database[count - 1].tech_render, tech_render);
	database[count - 1].polygons = polygons;
	database[count - 1].supported_platforms = supported_platforms;
	database[count - 1].physics_quality = physics_quality;
	database[count - 1].license_cost = license_cost;
	strcpy(database[count - 1].community, community);
	strcpy(database[count - 1].doc, doc);
	database[count - 1].rating = rating;

	return 0;
}