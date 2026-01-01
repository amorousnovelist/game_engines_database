#include <stdlib.h>
#include <math.h>
#include "engines_sort.h"

/*
* Гномья сортировка по количеству полигонов
* @param database - указатель на массив структур ENGINES
* @param count - количество записей в базе данных
* @return нет
*/
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