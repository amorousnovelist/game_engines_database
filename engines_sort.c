/*
* Автор файла: Илья Зеленев, бТИИ-251.
* 
* Файл содержит определение функции сортировки записей
* по максимальному количеству полигонов в сцене,
* в порядке возрастания,
* объявленной в engines_sort.h
*/

#include <stdlib.h>
#include <math.h>
#include "engines_sort.h"

/*
* Гномья сортировка по количеству полигонов
* @param p_database - указатель на массив структур ENGINES
* @param count - количество записей в базе данных
* @return нет
*/
void gnome_sort(ENGINES* p_database, const size_t count) {
	size_t index = 1;

	while (index < count) {
		if (sqrt(p_database[index].polygons) >= sqrt(p_database[index - 1].polygons))
			index++;
		else {
			ENGINES temp = p_database[index];
			p_database[index] = p_database[index - 1];
			p_database[index - 1] = temp;
			if (index > 1)
				index--;
		}
	}
}

/*
* Пузырьковая сортировка по количеству полигонов
* @param p_database - указатель на массив структур ENGINES
* @param count - количество записей в базе данных
* @return нет
*/
void bubble_sort(ENGINES* p_database, const size_t count) {
	for (int i = 0; i < count - 1; i++)
		for (int j = i + 1; j < count; j++) {
			if (sqrt(p_database[j].polygons) >= sqrt(p_database[i].polygons))
				continue;
			else {
				ENGINES temp = p_database[i];
				p_database[i] = p_database[j];
				p_database[j] = temp;
			}
		}
}