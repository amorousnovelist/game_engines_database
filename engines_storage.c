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
* @param p_database - указатель на массив структур ENGINES, содержащий записи базы данных
* @param count - количество записей в базе данных
* @return нет
*/
void free_database(ENGINES* p_database, const size_t count) {
	for (size_t i = 0; i < count; ++i) {
		free(p_database[i].name);
		free(p_database[i].tech_render);
		free(p_database[i].community);
		free(p_database[i].doc);
		free_supported_platforms(p_database[i].supported_platforms);
	}
	free(p_database);
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
ENGINES *read_data(const char *data_name, size_t *records_count) {
	FILE *input_file;

	/*Валидация открытия файлового потока*/
	if (!(input_file = fopen(data_name, "rb")))
		return NULL;
	fread(records_count, sizeof(size_t), 1, input_file);

	ENGINES *p_database = (ENGINES*)malloc(*records_count * sizeof(ENGINES));

	for (size_t i = 0; i < *records_count; i++) {
		/* Сперва читаются статические поля записи (числа и символ) */
		fread(&p_database[i].polygons, sizeof(unsigned), 1, input_file);
		fread(&p_database[i].physics_quality, sizeof(char), 1, input_file);
		fread(&p_database[i].license_cost, sizeof(unsigned), 1, input_file);
		fread(&p_database[i].rating, sizeof(float), 1, input_file);

		/*
		* Затем читаются динамические поля записи (строки)
		* Механизм происходит в 3 этапа:
		* 1) Чтение длины строки, которая уже содержит +1Б для символа конца строки.
		* 2) Выделение памяти у системы под длину строки.
		* 3) Чтение строки и инициализация поля записи.
		*/
		size_t name_len;

		fread(&name_len, sizeof(size_t), 1, input_file);
		p_database[i].name = (char*)malloc(name_len);
		fread(p_database[i].name, sizeof(char), name_len, input_file);

		size_t tech_render_len;

		fread(&tech_render_len, sizeof(size_t), 1, input_file);
		p_database[i].tech_render = (char*)malloc(tech_render_len);
		fread(p_database[i].tech_render, sizeof(char), tech_render_len, input_file);

		size_t community_len;

		fread(&community_len, sizeof(size_t), 1, input_file);
		p_database[i].community = (char*)malloc(community_len);
		fread(p_database[i].community, sizeof(char), community_len, input_file);

		size_t doc_len;

		fread(&doc_len, sizeof(size_t), 1, input_file);
		p_database[i].doc = (char*)malloc(doc_len);
		fread(p_database[i].doc, sizeof(char), doc_len, input_file);

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
		p_database[i].supported_platforms = supported_platforms;
	}
	fclose(input_file);
	return p_database;
}

/*
* Сохранение базы данных в бинарный файл
* @param dataname - имя файла
* @param p_database - указатель на массив структур ENGINES, содержащий записи для сохранения
* @param records_count - количество записей в базе данных
* @return 0 в случае успешного завершения; 1 в случае ошибки открытия файла
*/
int store_data(const char* dataname, ENGINES* p_database, const size_t records_count) {
	FILE *output_file;
	/*Валидация открытия файлового потока*/
	if (!(output_file = fopen(dataname, "wb")))
		return 1;
	fwrite(&records_count, sizeof(size_t), 1, output_file);

	for (int i = 0; i < records_count; i++) {
		/* Для удобства сперва записываются статические поля записи (числа и символ) */
		fwrite(&p_database[i].polygons, sizeof(unsigned), 1, output_file);
		fwrite(&p_database[i].physics_quality, sizeof(char), 1, output_file);
		fwrite(&p_database[i].license_cost, sizeof(unsigned), 1, output_file);
		fwrite(&p_database[i].rating, sizeof(float), 1, output_file);

		/*
		* Затем записываются динамические поля записи (строки)
		* Механизм происходит в 3 этапа:
		* 1) Подсчет длины строки поля, включая символ конца строки.
		* 2) Запись длины строки поля в файл.
		* 3) Запись самой строки поля в файл.
		*/
		size_t name_len = strlen(p_database[i].name) + 1;

		fwrite(&name_len, sizeof(size_t), 1, output_file);
		fwrite(p_database[i].name, sizeof(char), name_len, output_file);

		size_t tech_render_len = strlen(p_database[i].tech_render) + 1;

		fwrite(&tech_render_len, sizeof(size_t), 1, output_file);
		fwrite(p_database[i].tech_render, sizeof(char), tech_render_len, output_file);

		size_t community_len = strlen(p_database[i].community) + 1;

		fwrite(&community_len, sizeof(size_t), 1, output_file);
		fwrite(p_database[i].community, sizeof(char), community_len, output_file);

		size_t doc_len = strlen(p_database[i].doc) + 1;

		fwrite(&doc_len, sizeof(size_t), 1, output_file);
		fwrite(p_database[i].doc, sizeof(char), doc_len, output_file);

		struct supported_platforms *supported_platforms = p_database[i].supported_platforms;
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