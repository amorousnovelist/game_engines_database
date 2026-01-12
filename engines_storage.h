/* Автор файла: Илья Зеленев, бТИИ-251 */

#ifndef ENGINESSTORAGE_H

/*
* Файл содержит объявления функций для взаимодействия базы данных с файлами
* (файловыми потоками), а именно для записи в файл и чтения из файла,
* а также освобождения оперативной памяти из-под записей базы данных.
*/

#define ENGINESSTORAGE_H

#include "engines.h"

/*
* Освобождение памяти, выделенной под базу данных
* @param database - указатель на массив структур ENGINES, содержащий записи базы данных
* @param count - количество записей в базе данных
* @return нет
*/
void free_database(ENGINES *database, const size_t count);

/*
* Освобождение памяти, выделенной под связный список поддерживаемых платформ
* @param head - указатель на головной элемент связного списка
* @return нет
*/
void free_supported_platforms(struct supported_platforms *head);

/*
* Чтение базы данных из бинарного файла
* @param dataname - имя файла
* @param records_count - указатель на переменную, в которую будет записано количество записей
* @return указатель на массив структур ENGINES, содержащий записи из файла; NULL в случае ошибки открытия файла
 */
ENGINES* read_data(const char *data_name, size_t *records_count);

/*
* Сохранение базы данных в бинарный файл
* @param dataname - имя файла
* @param database - указатель на массив структур ENGINES, содержащий записи для сохранения
* @param records_count - количество записей в базе данных
* @return 0 в случае успешного завершения; 1 в случае ошибки открытия файла
*/
int store_data(const char *data_name, ENGINES *database, const size_t records_count);

#endif