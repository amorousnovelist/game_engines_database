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
* @param database - указатель на массив структур ENGINES
* @param count - количество записей в базе данных
* @return нет
*/
void gnome_sort(ENGINES* database, const size_t count) {
	int index = 1;

	while (index < count) {
		if (sqrt(database[index].polygons) >= sqrt(database[index - 1].polygons))
			index++;
		else {
			ENGINES temp = database[index];
			database[index] = database[index - 1];
			database[index - 1] = temp;
			index--;
		}
	}
}