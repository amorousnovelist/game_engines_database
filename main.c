#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "engines_tools.h"
#include "engines_storage.h"
#include "engines_sort.h"
#define FIELD_SIZE 128

int main()
{
	system("chcp 1251 > nul");

	ENGINES* database = NULL;
	unsigned records_count;
	char choice;

	printf("Файловый менеджер базы данных игровых движков\nРазработал: Зеленев Илья бТИИ-251\n");

	
	do {
		printf("----------------------------------------------\n\
(d) Загрузка данных из файла\n\
(v) Просмотр всех записей\n\
(o) Отображение веса базы данных в ОЗУ\n\
(r) Поиск по заданному диапазону\n\
(s) Сортировка данных\n\
(a) Добавление новой записи\n\
(t) Добавление новой записи с тестовыми данными\n\
(w) Сохранение данных в файл\n\
(q) Выход\n");
		printf("----------------------------------------------\nВыберите команду: ");
		scanf(" %c", &choice); getchar();

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
Введите 'y', если вы хотите, введите 'n' в ином случае: ");
				scanf(" %c", &confirm); getchar();

				if (confirm != 'y')
				{
					printf("Операция чтения файла отменена пользователем.\n");
					break;
				}
				free_database(database, records_count);
			}
			printf("Введите имя файла: ");
			fgets(dataname, sizeof(dataname), stdin);
			dataname[strcspn(dataname, "\n")] = '\0';
			if (!(database = read_data(dataname, &records_count)))
			{
				printf("Ошибка чтения файла: \"%s\" не существует в данном каталоге.\nПроверьте ввод и попробуйте ещё раз.\n", dataname);
				break;
			}
			printf("Файл \"%s\" успешно прочитан. Количество записей: %d.\n", dataname, records_count);
			break;
		}
		case 'v':
		{
			if (!(database))
			{
				printf("Ошибка просмотра записей: база данных не инициализирована.\nИнициализируйте базу данных и попробуйте ещё раз.\n");
				break;
			}
			for (int i = 0; i < records_count; i++)
				print_record(database, i);
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
			printf("Введите начальное значение интервала поиска (от %d до %d включительно): ", 1, records_count - 3);
			scanf(" %d", &begin); --begin;
			if (begin >= records_count - 1)
			{
				printf("Ошибка инициализации начального значения интервала поиска: %d больше или равно %d.\nПопробуйте задать значение ещё раз.\n", begin + 1, records_count);
				break;
			}
			else if (begin == records_count - 2)
			{
				printf("Ошибка инициализации начального значения интервала поиска: %d равно индексу предпоследнего элемента базы %d.\nПопробуйте задать значение ещё раз.\n", begin + 1, records_count - 1);
				break;
			}
			else if (begin < 0)
			{
				printf("Ошибка инициализации начального значения интервала поиска: %d меньше 1.\nПопробуйте задать значение ещё раз.\n", begin + 1);
				break;
			}
			printf("Введите конечное значение интервала поиска (от %d до %d включительно): ", begin + 1, records_count);
			scanf(" %d", &end); getchar(); --end;
			if (end > records_count)
			{
				printf("Ошибка инициализации конечного значения интервала поиска: %d больше %d.\nПопробуйте задать значение ещё раз.\n", end + 1, records_count);
				break;
			}
			else if (end <= begin + 1)
			{
				printf("Ошибка инициализации конечного значения интервала поиска: %d меньше или равно %d.\nПопробуйте задать значение ещё раз.\n", end + 1, begin + 2);
				break;
			}
			printf("Введите требуемое название технологии рендеринга: ");
			fgets(user_tech_render, sizeof(user_tech_render), stdin);
			user_tech_render[strcspn(user_tech_render, "\n")] = '\0';
			printf("Введите требуемые(-ое) названия(-ие) поддерживаемых(-ой) платформ(-ы) (через запятую с пробелами, в конце ввода запята не нужна): ");
			fgets(user_supported_platforms_lexemes, sizeof(user_supported_platforms_lexemes), stdin);
			user_supported_platforms_lexemes[strcspn(user_supported_platforms_lexemes, "\n")] = '\0';
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
								++success;
								break;
							}
							reader_database = reader_database->next;
						}
					}
			}
			if (!(success))
			{
				printf("Совпадение не найдено.\n");
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
			fgets(name, sizeof(name), stdin);
			name[strcspn(name, "\n")] = '\0';
			printf("Введите наименование технологии рендеринга: ");
			fgets(tech_render, sizeof(tech_render), stdin);
			tech_render[strcspn(tech_render, "\n")] = '\0';
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
			scanf(" %c", &physics_quality);
			while (physics_quality != 'A' && physics_quality != 'B' && physics_quality != 'C')
			{
				printf("Ошибка записи поля: '%c' не является символом из предложенного набора.\nПопробуйте ещё раз: ", physics_quality);
				scanf(" %c", &physics_quality);
			}
			printf("Укажите цену лицензии: ");
			scanf(" %d", &license_cost); getchar();
			printf("Введите ссылку на сообщество разработчиков движка: ");
			fgets(community, sizeof(community), stdin);
			community[strcspn(community, "\n")] = '\0';
			printf("Введите ссылку на документацию к движку: ");
			fgets(doc, sizeof(doc), stdin);
			doc[strcspn(doc, "\n")] = '\0';
			printf("Укажите рейтинг игрового движка: ");
			scanf(" %f", &rating);

			if (init_record(database, records_count - 1, name, tech_render, polygons, supported_platforms, physics_quality, license_cost, community, doc, rating))
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

			strcpy(name, names[rand() % (sizeof(names) / sizeof(names[0]))]);
			strcpy(tech_render, techs[rand() % (sizeof(techs) / sizeof(techs[0]))]);
			polygons = 1000000 + (rand() % 99000000);
			unsigned platforms_count = 1 + (rand() % 4);
			supported_platforms = NULL;

			for (unsigned i = 0; i < platforms_count; i++) 
			{
				struct supported_platforms* new_platform = malloc(sizeof(struct supported_platforms));
				char* platform_name = platforms[rand() % (sizeof(platforms) / sizeof(platforms[0]))];
				new_platform->name = (char*)malloc(strlen(platform_name) + 1);
				strcpy(new_platform->name, platform_name);
				new_platform->next = supported_platforms;
				supported_platforms = new_platform;
			}

			const char qualities[] = { 'A', 'B', 'C' };

			physics_quality = qualities[rand() % 3];
			license_cost = rand() % 100001;
			strcpy(community, communities[rand() % (sizeof(communities) / sizeof(communities[0]))]);
			strcpy(doc, docs[rand() % (sizeof(docs) / sizeof(docs[0]))]);
			rating = (rand() % 51) / 10.0f;

			if (init_record(database, records_count - 1, name, tech_render, polygons, supported_platforms, physics_quality, license_cost, community, doc, rating))
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
			fgets(dataname, sizeof(dataname), stdin);
			dataname[strcspn(dataname, "\n")] = '\0';
			if (store_data(dataname, database, records_count))
			{
				printf("Ошибка записи файла: невозможно открыть файл \"%s\" для записи.\nПроверьте ввод и попробуйте ещё раз.\n", dataname);
				break;
			}
			printf("Файл \"%s\" успешно записан. Количество записей: %d.\n", dataname, records_count);
			break;
		}
		case 'o':
		{
			if (!(database))
			{
				printf("Ошибка отображения веса базы данных в ОЗУ: база данных не инициализирована. Инициализируйте базу данных и попробуйте снова.\n");
				break;
			}
			size_t records_size = 0;
			for (int i = 0; i < records_count; i++)
				records_size += calculate_record_size(&database[i]);
			printf("Вес базы данных в ОЗУ: %zuБ\n", records_size);
			break;
		}
		case 'q':
		{
			if (database)
				free_database(database, records_count);
			printf("----------------------------------------------\nЗавершение работы...");
			break;
		}
		default: printf("Символ '%c' не является командой ввода. Повторите ввод ещё раз.\n", choice); break;
		}
	} while (choice != 'q');
	return 0;
}