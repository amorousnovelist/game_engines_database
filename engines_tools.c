/*
* Автор файла: Илья Зеленев, бТИИ-251.
* 
* Файл содержит определения функций для работы с записями базы данных,
* а именно для инициализации записи переданными данными, чтения записи и подсчета веса записи (в байтах),
* объявленные в engines_tools.h
*/

#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "engines_tools.h"

/*
* Подсчёт веса записи (в байтах) базы данных
* @param ENGINES* record - указатель на адрес записи
* @return размер (типа size_t) записи в оперативной памяти
*/
size_t calculate_record_size(ENGINES *record) {
	size_t total = 0;

	total += sizeof(ENGINES);

	/*Валидация существования данных полей записи*/
	if (record->name)
		total += strlen(record->name) + 1;
	if (record->tech_render)
		total += strlen(record->tech_render) + 1;
	if (record->community)
		total += strlen(record->community) + 1;
	if (record->doc)
		total += strlen(record->doc) + 1;

	struct supported_platforms* platform_reader = record->supported_platforms;

	while (platform_reader) {
		total += sizeof(struct supported_platforms);

		if (platform_reader->name)
			total += strlen(platform_reader->name) + 1;
		platform_reader = platform_reader->next;
	}
	return total;
}

/*
* Печать записи базы данных
* @param p_database - указатель на массив структур ENGINES
* @param count - индекс записи в базе данных
* @return 0 в случае успешного завершения
*/
int print_record(ENGINES *p_database, const size_t count) {
	printf("=============================================================================================================================\n");
	printf("Запись %d\n", count+1);
	printf("-----------------------------------------------------------------------------------------------------------------------------\n");
	printf("| наименование    | тех.рендер    | макс.полигонов |                       поддерживаемые платформы                         |\n");
	printf("-----------------------------------------------------------------------------------------------------------------------------\n");
	printf("| %-15s | %-13s | %-14d | ", p_database[count].name, \
		p_database[count].tech_render, \
		p_database[count].polygons);
	
	struct supported_platforms *supported_platforms = p_database[count].supported_platforms;

	while (supported_platforms) {
		printf("%-8s | ", supported_platforms->name);
		supported_platforms = supported_platforms->next;
	}
	printf("\n---------------------------------------------------------------------------------------------------------------------------\n");
	printf("| качество физики | цена лицензии | сообщество                            | документация                          | рейтинг |\n");
	printf("-----------------------------------------------------------------------------------------------------------------------------\n");
	printf("| %-15c | %-13d | %-37s | %-37s | %-7.1f |\n", p_database[count].physics_quality, \
		p_database[count].license_cost, \
		p_database[count].community, \
		p_database[count].doc, \
		p_database[count].rating);
	printf("-----------------------------------------------------------------------------------------------------------------------------\n");
	printf("Вес памяти: %zuБ\n", calculate_record_size(&p_database[count]));
	return 0;
}

/*
* Инициализация полей записи базы данных
* @param p_database - указатель на массив структур ENGINES
* @param count - индекс записи в базе данных
* @param name - наименование игрового движка
* @param tech_render - технология рендеринга
* @param polygons - максимальное количество полигонов
* @param supported_platforms - указатель на связный список поддерживаемых платформ
* @param physics_quality - качество физики
* @param license_cost - цена лицензии
* @param community - ссылка на сообщество разработчиков
* @param doc - ссылка на документацию
* @param rating - рейтинг игрового движка
* @return 0 в случае успешного завершения; 1 в случае ошибки выделения памяти
*/
int init_record(ENGINES *p_database, const size_t count, \
	const char *name, \
	const char *tech_render, \
	const unsigned polygons, \
	const struct supported_platforms *supported_platforms, \
	const char physics_quality, \
	const unsigned license_cost, \
	const char *community, \
	const char *doc, \
	const float rating) {
	/*
	* Механизм инициализации полей записи заключается в том, чтобы
	* сперва запросить память у системы ровно под длину строки и символ конца строки,
	* а затем скопировать значения из переданных в функцию буферов в выделенную память.
	* 
	* Это очень эффективно экономит место. Если бы поля структуры (строки)
	* были бы статическими, то они бы занимали в системе лишнее неиспользуемое пространство.
	* Например, при строке статической name, которая занимала бы 128Б и при вводе пользователем
	* в буфер строки "Linux", в строке оставались бы неиспользованными целых 123Б!
	*/
	p_database[count].name = (char*)malloc(strlen(name) + 1);
	p_database[count].tech_render = (char*)malloc(strlen(tech_render) + 1);
	p_database[count].community = (char*)malloc(strlen(community) + 1);
	p_database[count].doc = (char*)malloc(strlen(doc) + 1);
	/*Валидация выделения памяти*/
	if (!p_database[count].name || !p_database[count].tech_render ||
		!p_database[count].community || !p_database[count].doc) {
		return 1;
	}
	strcpy(p_database[count].name, name);
	strcpy(p_database[count].tech_render, tech_render);
	p_database[count].polygons = polygons;
	p_database[count].supported_platforms = supported_platforms;
	p_database[count].physics_quality = physics_quality;
	p_database[count].license_cost = license_cost;
	strcpy(p_database[count].community, community);
	strcpy(p_database[count].doc, doc);
	p_database[count].rating = rating;
	return 0;
}