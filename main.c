/*
* Автор файла: Илья Зеленев, бТИИ-251.
* 
* Главная функция программы "Файловый менеджер базы данных игровых движков"
* Каждая команда реализована как блок case в switch,
* каждый блок case использует функции из соответствующих модулей по мере необходимости.
*/

#define _CRT_SECURE_NO_DEPRECATE
#define FIELD_SIZE 128

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "engines_tools.h"
#include "engines_storage.h"
#include "engines_sort.h"

int main()
{
	/*
	* Перенаправление отображения формата кодировки 
	* со стандартного потока вывода в виртуальное нулевое устройство.
	*/
	system("chcp 1251 > nul");

	/*
	* База данных реализована как динамический массив записей.
	* p_database является указателем на базу данных, которую пользователю предстоит инициализировать
	* либо данными из стандартного входного потока, либо из файлового потока.
	* Во время выполнения программы возможно открыть/инициализировать только 1 базу данных,
	* поэтому используется 1 указатель на всю программу.
	*/
	ENGINES *p_database = NULL;
	size_t records_count;
	char choice;

	printf("Файловый менеджер базы данных игровых движков\nРазработал: Зеленев Илья бТИИ-251\n");
	printf("----------------------------------------------\n\
(d) Загрузка данных из файла\n\
(v) Просмотр всех записей\n\
(o) Отображение веса базы данных в ОЗУ\n\
(r) Поиск по заданному диапазону\n\
(s) Сортировка данных\n\
(a) Добавление новой записи\n\
(t) Добавление новой записи с тестовыми данными\n\
(w) Сохранение данных в файл\n\
(m) Отображение меню\n\
(q) Выход\n");
	do {
		printf("----------------------------------------------\nВыберите команду: ");
		scanf(" %c", &choice);
		getchar(); /*Необходимо для очистки стандартного потока ввода от символа новой строки*/

		switch (choice) {
		/*Команда инициализации базы данных записями из файла*/
		case 'd': {
			char dataname[FIELD_SIZE];

			/*Валидация запуска команды*/
			if (p_database) { 
				char confirm;

				printf("Ошибка чтения файла: база данных уже инициализирована.\nЧтение данных из файла удалит текущие несохраненные записи. \
Убедитесь, что вы сохранили текущие записи и вернитесь к чтению файла.\nВы хотите прочитать новую базу данных?\n\
Введите 'y', если вы хотите, введите 'n' в ином случае: ");
				scanf(" %c", &confirm);
				getchar(); /*Необходимо для очистки стандартного потока ввода от символа новой строки*/

				if (confirm != 'y') {
					printf("Операция чтения файла отменена пользователем.\n");
					break;
				}
				free_database(p_database, records_count);
			}
			printf("Введите имя файла: ");

			/*Инициализация строки с пробелами*/
			fgets(dataname, sizeof(dataname), stdin);
			dataname[strcspn(dataname, "\n")] = '\0';

			/*Валидация файлового потока*/
			if (!(p_database = read_data(dataname, &records_count)))
			{
				printf("Ошибка чтения файла: \"%s\" не существует в данном каталоге.\nПроверьте ввод и попробуйте ещё раз.\n", dataname);
				break;
			}
			printf("Файл \"%s\" успешно прочитан. Количество записей: %d.\n", dataname, records_count);
			break;
		}
		/*Команда просмотра записей*/
		case 'v': {
			/*Валидация запуска команды*/
			if (!(p_database)) {
				printf("Ошибка просмотра записей: база данных не инициализирована.\nИнициализируйте базу данных и попробуйте ещё раз.\n");
				break;
			}
			for (int i = 0; i < records_count; i++)
				print_record(p_database, i);
			break;
		}
		/*
		* Команда поиска записей в заданном диапазоне
		* (По наименовании технологии рендеринга и поддерживаемым платформам).
		*/
		case 'r': {
			/*Валидация запуска команды*/
			if (!(p_database)) {
				printf("Ошибка поиска записей: база данных не инициализирована.\nИнициализируйте базу данных и попробуйте ещё раз.\n");
				break;
			}
			unsigned success = 0; /*Счётчик количества совпадений параметров поиска*/
			unsigned begin;
			unsigned end;
			char user_tech_render[FIELD_SIZE]; /*Параметр наименовазния технологии рендеринга*/
			/*
			* Параметр наименования поддерживаемых платформ в виде набора лексем,
			* которые останутся единой неизменной строкой для того, чтобы
			* искать вхождение поля наименования тех. рендеринга элемента связного списка
			* записи открытой базы данных в строке, введенной пользователем.
			*/
			char user_supported_platforms_lexemes[FIELD_SIZE];

			/*
			* Ожидается, что диапазон в итоге будет содержать хотя бы 3 записи,
			* а также порядковый номер записи (не индекс) для удобства пользователя.
			*/
			printf("Введите начальное значение интервала поиска (от %d до %d включительно): ", 1, records_count - 3);
			scanf(" %d", &begin); 
			--begin; /*Для последующих операций индексации*/

			/*Валидация ввода*/
			if (begin >= records_count - 1) {
				printf("Ошибка инициализации начального значения интервала поиска: %d больше или равно %d.\nПопробуйте задать значение ещё раз.\n", begin + 1, records_count);
				break;
			}
			else if (begin == records_count - 2) {
				printf("Ошибка инициализации начального значения интервала поиска: %d равно индексу предпоследнего элемента базы %d.\nПопробуйте задать значение ещё раз.\n", begin + 1, records_count - 1);
				break;
			}
			else if (begin < 0) {
				printf("Ошибка инициализации начального значения интервала поиска: %d меньше 1.\nПопробуйте задать значение ещё раз.\n", begin + 1);
				break;
			}
			printf("Введите конечное значение интервала поиска (от %d до %d включительно): ", begin + 1, records_count);
			scanf(" %d", &end); 
			getchar(); /*Необходимо для очистки стандартного потока ввода от символа новой строки*/
			--end; /*Для последующих операций индексации*/

			/*Валидация ввода*/
			if (end > records_count) {
				printf("Ошибка инициализации конечного значения интервала поиска: %d больше %d.\nПопробуйте задать значение ещё раз.\n", end + 1, records_count);
				break;
			}
			else if (end <= begin + 1) {
				printf("Ошибка инициализации конечного значения интервала поиска: %d меньше или равно %d.\nПопробуйте задать значение ещё раз.\n", end + 1, begin + 2);
				break;
			}
			/*Инициализация строки с пробелами*/
			printf("Введите требуемое название технологии рендеринга: ");
			fgets(user_tech_render, sizeof(user_tech_render), stdin);
			user_tech_render[strcspn(user_tech_render, "\n")] = '\0';

			printf("Введите требуемые(-ое) названия(-ие) поддерживаемых(-ой) платформ(-ы) (через запятую с пробелами, в конце ввода запята не нужна): ");
			fgets(user_supported_platforms_lexemes, sizeof(user_supported_platforms_lexemes), stdin);
			user_supported_platforms_lexemes[strcspn(user_supported_platforms_lexemes, "\n")] = '\0';

			/*
			* Просматривается каждая запись в открытой базе данных по очереди:
			* сперва проверяется соответствие наименования технологии рендеринга
			* путем проверки вхождения наименования тех. рендеринга записи
			* в строку-параметр, введенную пользователем.
			* Затем, в случае совпадения, проверяется вхождение каждой поддерживаемой платформы записи
			* в строку лексем, введенную пользователем.
			* В случае хотя бы одного вхождения поддерживаемых платформ записи в строку лексем
			* счётчик совпадения параметров поиска инкрементируется.
			*/
			for (int i = begin + 1; i < end; i++) {
				if (!(strcmp(p_database[i].tech_render, user_tech_render))) {
						struct supported_platforms* reader_p_database = p_database[i].supported_platforms;

						while (reader_p_database) {
							if (strstr(user_supported_platforms_lexemes, reader_p_database->name)) {
								printf("Совпадение найдено в записи %d.\n", i + 1);
								++success;
								break;
							}
							reader_p_database = reader_p_database->next;
						}
					}
			}
			/*Валидация поиска по заданному диапазону*/
			if (!(success))
			{
				printf("Совпадение не найдено.\n");
			}
			break;
		}
		/*Команда сортировки записей*/
		case 's': {
			/*Валидация запуска команды*/
			if (!(p_database)) {
				printf("Ошибка сортировки записей: база данных не инициализирована.\nИнициализируйте базу данных и попробуйте ещё раз.\n");
				break;
			}
			/*
			* Сортировка не требует валидации, поэтому сообщении об успешном окончании команды печается каждый раз.
			* Причина заключается в отсутствии неудачных случаев сортировки:
			* она происходит всегда, если существует хотя бы 1 запись в базе данных.
			*/
			gnome_sort(p_database, records_count);
			printf("Сортировка завершена успешно.\n");
			break;
		}
		/*Команда добавления записи с пользовательскими данными в текущую (открытую) базу данных*/
		case 'a': {
			/*Валидация запуска команды*/
			if (!(p_database)) {
				printf("Внимание: База данных не была инициализирована!\nИнициализация базы данных...\n");

				/*Валидация выделения памяти*/
				if (!(p_database = (ENGINES*)malloc(sizeof(ENGINES)))) {
					printf("Ошибка инициализации: указатель на выделенную память не возвращен. Попробуйте ещё раз.\n");
					break;
				}
				records_count = 1;
			} else {
				/*Валидация выделения памяти*/
				if (!(p_database = realloc(p_database, (++records_count) * sizeof(ENGINES)))) {
					printf("Ошибка добавления записи: указатель на новую выделенную память не возвращен. Попробуйте ещё раз.\n");
					break;
				}
			}
			/*
			* Пользовательские поля ввода.
			* По возможности описаны как статические поля данных.
			* После инициализации соответствующих полей функция init_record()
			* инициализирует уже саму (в текущем блоке - последнюю) запись в базе данных
			* через механизм использования динамической памяти, а именно
			* запрашивает у системы ровно столько байт, сколько нужно для хранения поля,
			* что значительно экономит занимаемое пространство в ОЗУ.
			*/
			char name[FIELD_SIZE];
			char tech_render[FIELD_SIZE];
			unsigned polygons;
			struct supported_platforms *supported_platforms = NULL;
			char supported_platforms_lexemes[FIELD_SIZE];
			char* supported_platforms_lexeme = NULL;
			char physics_quality;
			unsigned license_cost;
			char community[FIELD_SIZE];
			char doc[FIELD_SIZE];
			float rating;

			/*Инициализация строки с пробелами*/
			printf("Введите наименование игрового движка: ");
			fgets(name, sizeof(name), stdin);
			name[strcspn(name, "\n")] = '\0';

			printf("Введите наименование технологии рендеринга: ");
			fgets(tech_render, sizeof(tech_render), stdin);
			tech_render[strcspn(tech_render, "\n")] = '\0';

			printf("Укажите количество полигонов: ");
			scanf(" %d", &polygons); 
			getchar(); /*Необходимо для очистки стандартного потока ввода от символа новой строки*/

			printf("Введите наименования поддерживаемых платформ (через запятую с пробелом, запятая в конце не нужна): ");
			fgets(supported_platforms_lexemes, sizeof(supported_platforms_lexemes), stdin);
			supported_platforms_lexemes[strcspn(supported_platforms_lexemes, "\n")] = '\0';

			/*
			* Строка введенных лексем разбивается на реальные лексемы и инициализирует связный список.
			* Ввиду особенности использования библиотечной функции strtok(), механизм инициализации связного списка
			* разбит на два блока: if и while.
			*/
			if (supported_platforms_lexeme = strtok(supported_platforms_lexemes, ", ")) {
				supported_platforms = (struct supported_platforms*)malloc(sizeof(struct supported_platforms));
				supported_platforms->name = (char*)malloc(strlen(supported_platforms_lexeme) + 1);
				strcpy(supported_platforms->name, supported_platforms_lexeme);
				supported_platforms->next = NULL;
			}

			while (supported_platforms_lexeme = strtok('\0', ", ")) {
				struct supported_platforms* new_supported_platform = NULL;

				new_supported_platform = (struct supported_platforms*)malloc(sizeof(struct supported_platforms));
				new_supported_platform->name = (char*)malloc(strlen(supported_platforms_lexeme) + 1);
				strcpy(new_supported_platform->name, supported_platforms_lexeme);
				new_supported_platform->next = supported_platforms;
				supported_platforms = new_supported_platform;
			}
			printf("Обозначте качество физики игрового движка символом из набора (A, B, C): ");
			scanf(" %c", &physics_quality);

			/*Валидация ввода символа*/
			while (physics_quality != 'A' && physics_quality != 'B' && physics_quality != 'C')
			{
				printf("Ошибка записи поля: '%c' не является символом из предложенного набора.\nПопробуйте ещё раз: ", physics_quality);
				scanf(" %c", &physics_quality);
			}
			printf("Укажите цену лицензии: ");
			scanf(" %d", &license_cost); 
			getchar();

			printf("Введите ссылку на сообщество разработчиков движка: ");
			fgets(community, sizeof(community), stdin);
			community[strcspn(community, "\n")] = '\0';

			printf("Введите ссылку на документацию к движку: ");
			fgets(doc, sizeof(doc), stdin);
			doc[strcspn(doc, "\n")] = '\0';

			printf("Укажите рейтинг игрового движка: ");
			scanf(" %f", &rating);

			/*Валидация инициализации полей последней записи*/
			if (init_record(p_database, records_count - 1, \
				name, \
				tech_render, \
				polygons, \
				supported_platforms, \
				physics_quality, \
				license_cost, \
				community, \
				doc, \
				rating)) {
				printf("Ошибка инициализации полей последнего элемента массива структур: невозможно выделить память.\nПопробуйте ещё раз.\n");
				break;
			}
			printf("Запись завершена...\nКоличество записей: %zu\n", records_count);
			break;
		}
		/*Команда добавления записи с тестовыми случайными данными в текущую (открытую) базу данных*/
		case 't': {
			/*Валидация запуска команды*/
			if (!(p_database)) {
				printf("Внимание: База данных не была инициализирована!\nИнициализация базы данных...\n");
				/*Валидация выделения памяти*/
				if (!(p_database = (ENGINES*)malloc(sizeof(ENGINES)))) {
					printf("Ошибка инициализации: указатель на выделенную память не возвращен. Попробуйте ещё раз.\n");
					break;
				}
				records_count = 1;
			} else {
				/*Валидация выделения памяти*/
				if (!(p_database = realloc(p_database, (++records_count) * sizeof(ENGINES)))) {
					printf("Ошибка добавления записи: указатель на новую выделенную память не возвращен. Попробуйте ещё раз.\n");
					break;
				}
			}
			/*Наборы данных полей для инициализации записи базы данных случайными из них, разумеется, в соответсвии своему полю*/
			const char* names[] = {
				"Unreal Engine", 
				"Unity", 
				"Godot", 
				"CryEngine",
				"Frostbite", 
				"Source", 
				"id Tech", 
				"GameMaker"
			};

			const char* techs[] = {
				"Deferred Shading", 
				"Forward+", 
				"Ray Tracing",
				"Voxel Cone Tracing", 
				"Tiled Deferred"
			};

			const char* platforms[] = {
				"PC", 
				"PlayStation", 
				"Xbox", 
				"Nintendo Switch",
				"Android", 
				"iOS", 
				"Linux", 
				"MacOS"
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

			const char qualities[] = { 'A', 'B', 'C' };

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

			/*
			* Механизм инициализации полей ничем не отличается от уже описанного 
			* в команде "Добавление записи с пользовательскими данными..."
			*/
			strcpy(name, names[rand() % (sizeof(names) / sizeof(names[0]))]);
			strcpy(tech_render, techs[rand() % (sizeof(techs) / sizeof(techs[0]))]);
			polygons = 1000000 + (rand() % 99000000);
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

			physics_quality = qualities[rand() % 3];
			license_cost = rand() % 100001;
			strcpy(community, communities[rand() % (sizeof(communities) / sizeof(communities[0]))]);
			strcpy(doc, docs[rand() % (sizeof(docs) / sizeof(docs[0]))]);
			rating = (rand() % 51) / 10.0f;

			if (init_record(p_database, records_count - 1, \
				name, \
				tech_render, \
				polygons, \
				supported_platforms, \
				physics_quality, \
				license_cost, \
				community, \
				doc, \
				rating)) {
				printf("Ошибка инициализации полей последнего элемента массива структур: невозможно выделить память.\nПопробуйте ещё раз.\n");
				break;
			}
			printf("Запись завершена...\nКоличество записей: %zu\n", records_count);
			break;
		}
		/*Команда записи текущей (открытой) базы данных в файл*/
		case 'w': {
			/*Валидация запуска команды*/
			if (!(p_database)) {
				printf("Ошибка записи в файл: база данных не инициализирована.\nИнициализируйте базу данных и попробуйте ещё раз.\n");
				break;
			}
			char dataname[FIELD_SIZE];

			/*Инициализация строки с пробелами*/
			printf("Введите название файла: ");
			fgets(dataname, sizeof(dataname), stdin);
			dataname[strcspn(dataname, "\n")] = '\0';

			/*Валидация записи в файл*/
			if (store_data(dataname, p_database, records_count)) {
				printf("Ошибка записи файла: невозможно открыть файл \"%s\" для записи.\nПроверьте ввод и попробуйте ещё раз.\n", dataname);
				break;
			}
			printf("Файл \"%s\" успешно записан. Количество записей: %d.\n", dataname, records_count);
			break;
		}
		/*Команда отображения веса текущей (открытой) базы данных в байтах*/
		case 'o': {
			/*Валидация запуска команды*/
			if (!(p_database)) {
				printf("Ошибка отображения веса базы данных в ОЗУ: база данных не инициализирована. Инициализируйте базу данных и попробуйте снова.\n");
				break;
			}
			size_t records_size = 0;

			for (int i = 0; i < records_count; i++)
				records_size += calculate_record_size(&p_database[i]);
			printf("Вес базы данных в ОЗУ: %zuБ\n", records_size);
			break;
		}
		/*Команда отображения меню*/
		case 'm': {
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
			break;
		}
		/*Команда завершения работы программы*/
		case 'q': {
			/*
			* Валидация существования базы данных:
			* если она была инициализорована, то память из-под неё освобождается.
			*/
			if (p_database)
				free_database(p_database, records_count);
			printf("----------------------------------------------\nЗавершение работы...");
			break;
		}
		/*Валидация ввода команд*/
		default: printf("Символ '%c' не является командой ввода. Повторите ввод ещё раз.\n", choice); break;
		}
	} while (choice != 'q');
	return 0;
}