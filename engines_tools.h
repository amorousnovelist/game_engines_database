/* Автор файла: Илья Зеленев, бТИИ-251. */

#ifndef ENGINESTOOLS_H

/*
* Файл содержит объявления функций для работы с записями базы данных,
* а именно для инициализации записи переданными данными, чтения записи и подсчета веса записи (в байтах).
*/

#define ENGINESTOOLS_H

#include "engines.h"

/*
* Подсчёт веса записи (в байтах) базы данных
* @param ENGINES* record - указатель на адрес записи
* @return размер (типа size_t) записи в оперативной памяти
*/
size_t calculate_record_size(ENGINES *record);

/*
* Печать записи базы данных
* @param p_database - указатель на массив структур ENGINES
* @param count - индекс записи в базе данных
* @return 0 в случае успешного завершения
*/
void print_record(ENGINES *p_database, const size_t count);

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
	const float rating);

#endif