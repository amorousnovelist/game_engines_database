/* Автор файла: Илья Зеленев, бТИИ-251. */

#ifndef ENGINESSORT_H

/*
* Файл содержит объявление функции сортировки записей базы данных
* по максимальному количеству полигонов в сцене
* в порядке возрастания.
*/

#define ENGINESSORT_H

#include "engines.h"

/*
* Гномья сортировка по количеству полигонов
* @param p_database - указатель на массив структур ENGINES
* @param count - количество записей в базе данных
* @return нет
*/
void gnome_sort(ENGINES *p_database, const size_t count);

#endif