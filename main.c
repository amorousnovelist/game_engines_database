#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "engines_tools.h"
#include "engines_storage.h"
#include "engines_search.h"
#include "engines_sort.h"
#define FIELD_SIZE 128

int main()
{
	system("chcp 1251 > nul");

	ENGINES* database = NULL;
	unsigned records_count;
	char choice;

	printf("(d) Загрузка данных из файла\n\
(v) Просмотр всех записей\n\
(r) Поиск по заданному диапазону\n\
(s) Сортировка данных\n\
(a) Добавление новой записи\n\
(t) Добавление новой записи с тестовыми данными\n\
(w) Сохранение данных в файл\n\
(q) Выход\n");
	do {
		printf("----------\nВыберите команду: ");
		scanf(" %c", &choice); //getchar();

		switch (choice)
		{
		case 'd':
		{
			char dataname[FIELD_SIZE];

			if (database)
			{
				char confirm;
				printf("Ошибка чтения файла: база данных уже инициализирована.\nЧтение данных из файла удалит текущие несохраненные записи. \
Убедитесь, что вы сохранили текущие записи и вернитесь к чтению файла.\nВы хотите прочитать новую базу данных?\n\
(y|n): ");
				scanf(" %c", &confirm);

				if (confirm != 'y')
				{
					printf("Операция чтения файла отменена пользователем.\n");
					break;
				}

				//free_database(database, records_count); - написать функцию освобождения памяти
				database = NULL;
				records_count = 0;
			}
			printf("Введите имя файла: ");
			scanf(" %s", dataname);
			if (!(database = read_data(dataname, &records_count)))
			{
				printf("Ошибка чтения файла: \"%s\" не существует в данном каталоге.\nПроверьте ввод и попробуйте ещё раз.\n", dataname);
				break;
			}
			break;
		}
		case 'v':
		{
			if (!(database))
			{
				printf("Ошибка просмотра записей: база данных не инициализирована.\nИнициализируйте базу данных и попробуйте ещё раз.\n");
				break;
			}

			print_record(database, records_count);
			break;
		}
		case 'r':
		{
			if (!(database))
			{
				printf("Ошибка поиска записей: база данных не инициализирована.\nИнициализируйте базу данных и попробуйте ещё раз.\n");
				break;
			}
			unsigned success = 0;
			unsigned begin;
			unsigned end;
			char user_tech_render[FIELD_SIZE];
			char user_supported_platforms_lexemes[FIELD_SIZE];
			printf("Введите начальное значение интервала поиска (целое неотрицательное значение): ");
			scanf(" %d", &begin);
			printf("Введите конечное значение интервала поиска (целое неотрицательное значение, большее %d): ", begin + 1);
			scanf(" %d", &end);
			/*while (end < begin + 1)
			{
				printf("Ошибка инициализации конечного значения интервала поиска: %d меньше %d.\nПопробуйте задать значение ещё раз.\n", end, begin + 1);
				scanf(" %s", &end);
			}*/
			printf("Введите требуемое название технологии рендеринга: ");
			scanf(" %s", user_tech_render);
			printf("Введите требуемые(-ое) названия(-ие) поддерживаемых(-ой) платформ(-ы) (через запятую с пробелами, в конце ввода запята не нужна): ");
			scanf(" %s", user_supported_platforms_lexemes);
			for (int i = begin + 1; i < end; i++)
			{
				if (!(strcmp(database[i].tech_render, user_tech_render)))
					{
						struct supported_platforms* reader_database = database[i].supported_platforms;
						while (reader_database)
						{
							if (strstr(user_supported_platforms_lexemes, reader_database->name))
							{
								printf("Совпадение найдено в записи %d.\n", i + 1);
								success = 1;
								break;
							}
							reader_database = reader_database->next;
						}
						if (!(success))
						{
							printf("Совпадение не найдено.\n");
						}
					}
			}
			break;
		}
		case 's':
		{
			if (!(database))
			{
				printf("Ошибка сортировки записей: база данных не инициализирована.\nИнициализируйте базу данных и попробуйте ещё раз.\n");
				break;
			}
			gnome_sort(database, records_count);
			printf("Сортировка завершена успешно.\n");
			break;
		}
		case 'a':
		{
			if (!(database))
			{
				printf("Внимание: База данных не была инициализирована!\nИнициализация базы данных...\n");
				if (!(database = (ENGINES*)malloc(sizeof(ENGINES))))
				{
					printf("Ошибка инициализации: указатель на выделенную память не возвращен. Попробуйте ещё раз.\n");
					break;
				}
				records_count = 1;
			}
			else
			{
				if (!(database = realloc(database, (++records_count) * sizeof(ENGINES))))
				{
					printf("Ошибка добавления записи: указатель на новую выделенную память не возвращен. Попробуйте ещё раз.\n");
					break;
				}
			}
			char name[FIELD_SIZE];
			char tech_render[FIELD_SIZE];
			unsigned polygons;
			struct supported_platforms* supported_platforms = NULL;
			char supported_platforms_lexemes[FIELD_SIZE];
			char* supported_platforms_lexeme = NULL;
			char physics_quality;
			unsigned license_cost;
			char community[FIELD_SIZE];
			char doc[FIELD_SIZE];
			float rating;

			printf("Введите наименование игрового движка: ");
			scanf(" %s", name);
			printf("Введите наименование технологии рендеринга: ");
			scanf(" %s", tech_render);
			printf("Укажите количество полигонов: ");
			scanf(" %d", &polygons); getchar();
			printf("Введите наименования поддерживаемых платформ (через запятую с пробелом, запятая в конце не нужна): ");
			fgets(supported_platforms_lexemes, sizeof(supported_platforms_lexemes), stdin);
			supported_platforms_lexemes[strcspn(supported_platforms_lexemes, "\n")] = '\0';

			if (supported_platforms_lexeme = strtok(supported_platforms_lexemes, ", "))
			{
				supported_platforms = (struct supported_platforms*)malloc(sizeof(struct supported_platforms));
				supported_platforms->name = (char*)malloc(strlen(supported_platforms_lexeme) + 1);
				strcpy(supported_platforms->name, supported_platforms_lexeme);
				supported_platforms->next = NULL;
			}

			while (supported_platforms_lexeme = strtok('\0', ", "))
			{
				struct supported_platforms* new_supported_platform = NULL;
				new_supported_platform = (struct supported_platforms*)malloc(sizeof(struct supported_platforms));
				new_supported_platform->name = (char*)malloc(strlen(supported_platforms_lexeme) + 1);
				strcpy(new_supported_platform->name, supported_platforms_lexeme);
				new_supported_platform->next = supported_platforms;

				supported_platforms = new_supported_platform;
			}
			printf("Обозначте качество физики игрового движка символом из набора (A, B, C): ");
			scanf(" %c", &physics_quality); //потенциальные проблемы с вводом
			while (physics_quality != 'A' && physics_quality != 'B' && physics_quality != 'C')
			{
				printf("Ошибка записи поля: '%c' не является символом из предложенного набора.\nПопробуйте ещё раз: ", physics_quality);
				scanf(" %c", &physics_quality);
			}
			printf("Укажите цену лицензии: ");
			scanf(" %d", &license_cost);
			printf("Введите ссылку на сообщество разработчиков движка: ");
			scanf(" %s", community);
			printf("Введите ссылку на документацию к движку: ");
			scanf(" %s", doc);
			printf("Укажите рейтинг игрового движка: ");
			scanf(" %f", &rating);

			if (init_record(database, records_count, name, tech_render, polygons, supported_platforms, physics_quality, license_cost, community, doc, rating))
			{
				printf("Ошибка инициализации полей последнего элемента массива структур: невозможно выделить память.\nПопробуйте ещё раз.\n");
				break;
			}
			break;
		}
		case 't':
		{
			if (!(database))
			{
				printf("Внимание: База данных не была инициализирована!\nИнициализация базы данных...\n");
				if (!(database = (ENGINES*)malloc(sizeof(ENGINES))))
				{
					printf("Ошибка инициализации: указатель на выделенную память не возвращен. Попробуйте ещё раз.\n");
					break;
				}
				records_count = 1;
			}
			else
			{
				if (!(database = realloc(database, (++records_count) * sizeof(ENGINES))))
				{
					printf("Ошибка добавления записи: указатель на новую выделенную память не возвращен. Попробуйте ещё раз.\n");
					break;
				}
			}
			//init_random_engine(&database[records_count - 1]);
			const char* names[] = {
		"Unreal Engine", "Unity", "Godot", "CryEngine",
		"Frostbite", "Source", "id Tech", "GameMaker"
			};
			const char* techs[] = {
				"Deferred Shading", "Forward+", "Ray Tracing",
				"Voxel Cone Tracing", "Tiled Deferred"
			};
			const char* platforms[] = {
				"PC", "PlayStation", "Xbox", "Nintendo Switch",
				"Android", "iOS", "Linux", "MacOS"
			};
			const char* communities[] = {
				"https://unrealengine.com/community",
				"https://unity.com/community",
				"https://godotengine.org/community",
				"https://answers.unrealengine.com"
			};
			const char* docs[] = {
				"https://docs.unrealengine.com",
				"https://docs.unity3d.com",
				"https://docs.godotengine.org",
				"https://docs.cryengine.com"
			};
			char name[FIELD_SIZE];
			char tech_render[FIELD_SIZE];
			unsigned polygons;
			struct supported_platforms* supported_platforms = NULL;
			char supported_platforms_lexemes[FIELD_SIZE];
			char* supported_platforms_lexeme = NULL;
			char physics_quality;
			unsigned license_cost;
			char community[FIELD_SIZE];
			char doc[FIELD_SIZE];
			float rating;
			// Имя
			strcpy(name, names[rand() % (sizeof(names) / sizeof(names[0]))]);

			// Технология рендеринга
			strcpy(tech_render, techs[rand() % (sizeof(techs) / sizeof(techs[0]))]);

			// Полигоны (от 1M до 100M)
			polygons = 1000000 + (rand() % 99000000);

			// Список платформ (1-4 случайные платформы)
			unsigned platforms_count = 1 + (rand() % 4);
			supported_platforms = NULL;

			for (unsigned i = 0; i < platforms_count; i++) {
				struct supported_platforms* new_platform = malloc(sizeof(struct supported_platforms));
				char* platform_name = platforms[rand() % (sizeof(platforms) / sizeof(platforms[0]))];
				new_platform->name = (char*)malloc(strlen(platform_name) + 1);
				strcpy(new_platform->name, platform_name);
				new_platform->next = supported_platforms;
				supported_platforms = new_platform;
			}

			// Качество физики (A, B, C)
			const char qualities[] = { 'A', 'B', 'C' };
			physics_quality = qualities[rand() % 3];

			// Цена лицензии ($0 - $100000)
			license_cost = rand() % 100001;

			// Сообщество
			//community_rand = communities[rand() % (sizeof(communities) / sizeof(communities[0]))];
			//community = (char*)malloc(strlen(community_rand) + 1);
			strcpy(community, communities[rand() % (sizeof(communities) / sizeof(communities[0]))]);

			// Документация
			//doc = strdup(docs[rand() % (sizeof(docs) / sizeof(docs[0]))]);
			strcpy(doc, docs[rand() % (sizeof(docs) / sizeof(docs[0]))]);

			// Рейтинг (0.0 - 5.0 с одной цифрой после запятой)
			rating = (rand() % 51) / 10.0f;  // 0.0 - 5.0

			if (init_record(database, records_count, name, tech_render, polygons, supported_platforms, physics_quality, license_cost, community, doc, rating))
			{
				printf("Ошибка инициализации полей последнего элемента массива структур: невозможно выделить память.\nПопробуйте ещё раз.\n");
				break;
			}
			break;
		}
		case 'w':
		{
			if (!(database))
			{
				printf("Ошибка записи в файл: база данных не инициализирована.\nИнициализируйте базу данных и попробуйте ещё раз.\n");
				break;
			}
			char dataname[FIELD_SIZE];

			printf("Введите название файла: ");
			scanf(" %s", dataname);
			store_data(dataname, database, records_count);
			break;
		}
		case 'q':
		{
			puts("Завершение работы...");
			break;
		}
		default: printf("Символ '%c' не является командой ввода. Повторите ввод ещё раз.\n", choice); break;
		}
	} while (choice != 'q');
	return 0;
}