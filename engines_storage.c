/*
* Автор файла: Илья Зеленев, бТИИ-251.
* 
* Файл содержит определения функций для взаимодействия базы данных с файлами
* (файловыми потоками), а именно для записи в файл и чтения из файла,
* а также освобождения оперативной памяти из-под записей базы данных,
* объявленные в engines_tools.h
*/

#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "engines_storage.h"

/*
* Освобождение памяти, выделенной под базу данных
* @param database - указатель на массив структур ENGINES, содержащий записи базы данных
* @param count - количество записей в базе данных
* @return нет
*/
void free_database(ENGINES* database, const unsigned count) {
	for (int i = 0; i < count; ++i) {
		free(database[i].name);
		free(database[i].tech_render);
		free(database[i].community);
		free(database[i].doc);
		free_supported_platforms(database[i].supported_platforms);
	}
	free(database);
}

/*
* Освобождение памяти, выделенной под связный список поддерживаемых платформ
* @param head - указатель на головной элемент связного списка
* @return нет
*/
void free_supported_platforms(struct supported_platforms *head) {
	struct supported_platforms *current = head;
	struct supported_platforms *next;

	while (current != NULL) {
		next = current->next;
		free(current->name);
		free(current);
		current = next;
	}
}

/*
* Чтение базы данных из бинарного файла
* @param dataname - имя файла
* @param records_count - указатель на переменную, в которую будет записано количество записей
* @return указатель на массив структур ENGINES, содержащий записи из файла; NULL в случае ошибки открытия файла
*/
ENGINES *read_data(const char *dataname, unsigned *records_count) {
	FILE *input_file;

	if (!(input_file = fopen(dataname, "rb")))
		return NULL;
	fread(records_count, sizeof(unsigned), 1, input_file);

	ENGINES *database = (ENGINES*)malloc(*records_count * sizeof(ENGINES));

	for (int i = 0; i < *records_count; i++) {
		fread(&database[i].polygons, sizeof(unsigned), 1, input_file);
		fread(&database[i].physics_quality, sizeof(char), 1, input_file);
		fread(&database[i].license_cost, sizeof(unsigned), 1, input_file);
		fread(&database[i].rating, sizeof(float), 1, input_file);

		size_t name_len;

		fread(&name_len, sizeof(size_t), 1, input_file);
		database[i].name = (char*)malloc(name_len);
		fread(database[i].name, sizeof(char), name_len, input_file);

		size_t tech_render_len;

		fread(&tech_render_len, sizeof(size_t), 1, input_file);
		database[i].tech_render = (char*)malloc(tech_render_len);
		fread(database[i].tech_render, sizeof(char), tech_render_len, input_file);

		size_t community_len;

		fread(&community_len, sizeof(size_t), 1, input_file);
		database[i].community = (char*)malloc(community_len);
		fread(database[i].community, sizeof(char), community_len, input_file);

		size_t doc_len;

		fread(&doc_len, sizeof(size_t), 1, input_file);
		database[i].doc = (char*)malloc(doc_len);
		fread(database[i].doc, sizeof(char), doc_len, input_file);

		size_t supported_platforms_count;

		fread(&supported_platforms_count, sizeof(size_t), 1, input_file);
		
		struct supported_platforms *supported_platforms = (struct supported_platforms*)malloc(sizeof(struct supported_platforms));

		size_t supported_platform_name_len;

		fread(&supported_platform_name_len, sizeof(size_t), 1, input_file);
		supported_platforms->name = (char*)malloc(supported_platform_name_len);
		fread(supported_platforms->name, sizeof(char), supported_platform_name_len, input_file);
		supported_platforms->next = NULL;

		for (int j = 1; j < supported_platforms_count; j++) {
			struct supported_platforms* temp = (struct supported_platform*)malloc(sizeof(struct supported_platforms));

			fread(&supported_platform_name_len, sizeof(size_t), 1, input_file);
			temp->name = (char*)malloc(supported_platform_name_len);
			fread(temp->name, sizeof(char), supported_platform_name_len, input_file);
			temp->next = supported_platforms;
			supported_platforms = temp;
		}
		database[i].supported_platforms = supported_platforms;
	}
	fclose(input_file);
	return database;
}

/*
* Сохранение базы данных в бинарный файл
* @param dataname - имя файла
* @param database - указатель на массив структур ENGINES, содержащий записи для сохранения
* @param records_count - количество записей в базе данных
* @return 0 в случае успешного завершения; 1 в случае ошибки открытия файла
*/
int store_data(const char* dataname, ENGINES* database, const unsigned records_count) {
	FILE *output_file;

	if (!(output_file = fopen(dataname, "wb")))
		return 1;
	fwrite(&records_count, sizeof(unsigned), 1, output_file);

	for (int i = 0; i < records_count; i++) {
		fwrite(&database[i].polygons, sizeof(unsigned), 1, output_file);
		fwrite(&database[i].physics_quality, sizeof(char), 1, output_file);
		fwrite(&database[i].license_cost, sizeof(unsigned), 1, output_file);
		fwrite(&database[i].rating, sizeof(float), 1, output_file);

		size_t name_len = strlen(database[i].name) + 1;

		fwrite(&name_len, sizeof(size_t), 1, output_file);
		fwrite(database[i].name, sizeof(char), name_len, output_file);

		size_t tech_render_len = strlen(database[i].tech_render) + 1;

		fwrite(&tech_render_len, sizeof(size_t), 1, output_file);
		fwrite(database[i].tech_render, sizeof(char), tech_render_len, output_file);

		size_t community_len = strlen(database[i].community) + 1;

		fwrite(&community_len, sizeof(size_t), 1, output_file);
		fwrite(database[i].community, sizeof(char), community_len, output_file);

		size_t doc_len = strlen(database[i].doc) + 1;

		fwrite(&doc_len, sizeof(size_t), 1, output_file);
		fwrite(database[i].doc, sizeof(char), doc_len, output_file);

		struct supported_platforms *supported_platforms = database[i].supported_platforms;
		size_t platforms_count = 0;
		struct supported_platforms *temp = supported_platforms;

		while (temp) {
			platforms_count++;
			temp = temp->next;
		}
		fwrite(&platforms_count, sizeof(size_t), 1, output_file);

		temp = supported_platforms;
		while (temp) {
			size_t platform_name_len = strlen(temp->name) + 1;

			fwrite(&platform_name_len, sizeof(size_t), 1, output_file);
			fwrite(temp->name, sizeof(char), platform_name_len, output_file);
			temp = temp->next;
		}
	}
	fclose(output_file);
	return 0;
}