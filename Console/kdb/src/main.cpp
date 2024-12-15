#include "../src/Framework.hpp"
#include "../src/Menu.hpp"
#include "../src/MovieDatabase.hpp"
#include "../src/Schema.hpp"
#include "../src/Utils.hpp"

using namespace Mv;
using namespace MvDataBase;
using namespace MvSchema;
using namespace MvUtils;
using namespace MvException;

constexpr short BASE_ID = 0;

/* TODO: Localize this piece of s.. software... */

int main(int argc, char* argv[]) {

    /* Встановлення іменування терміналу.
     * У цілях практики були використані макро. див. Framework.hpp
     */
    SetConsoleTitleA(APP_TITLE);

    /*
	 * Встановлює ікнонку програми використовуючи winAPI, об'єктивно лишнє.
     */
    rlutil::SetConsoleIcon(L"./ico/kdb.ico");


    /* Встановлює відображення послідовностей формату ANSI.
     *Послідовності викорситані для виведення 4-бітних кольорів. див. Utils.hpp| reference - rlitil.h
	 */
    rlutil::SetConsoleOutputANSI();


    /*  Встановлення кодування на UTF-8;
     *  1251 - підтримує кирилицю.
	 */
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);
    _wsetlocale(LC_CTYPE, APP_LOCALE_UKRAINIAN);

    // Встановлення обробки сигналів CTRL + C, CTRL + PAUSE
    //TODO : Розібратись детальніше у майбутньому з обробкою інших сигналів, наприклад СTRL_CLOSE_EVENT
    rlutil::SetupSignalHandler();

    /* Створюємо необхідні папки, якщо вони не існують */
    //TODO: Реалізувати повноцінний клас FileHandler для ефективної роботи з файлами.
    filesystem::create_directory("../build/import");
    filesystem::create_directory("../build/export");
    filesystem::create_directory("../build/log");
    filesystem::create_directory("../build/temp");

    /* Ініціалізація основної схеми BASE, ініціалізація smart-вказівника на об'єкт типу MovieDatabase;*/
	unique_ptr<Schema> schema_ptr = make_unique<Schema>();

    /* Рядковий аргумент -import
     * Якщо додаток запущений з відповідним аргументом -import, то відбудеться імпорт або десериалізація рядків у об'єкти в основну схему BASE.
     * При виникненні проблем при експорті буде виведене інформативне повідомлення.
     * Інакше виведемо інструкції як скорстатись імпортом під час запуску програми до ініціалізації меню.
	 */
    if (argc >= 2 && strcmp(argv[1], "-import") == 0) {
        LogHandle::info(  CLEAR_COLOR + L"[+] Імпортування файлів " + LIGHT_GREEN + L"АКТИВОВАНЕ " + CLEAR_COLOR + L"[+]");
        IHandle::anykey(LIGHT_CYAN + L"Після натиснення довільної клавіші почнеться завантаження файлів у базу..." + WHITE);
        try {
            // "Очистить" термінал як тільки залишимо try-scope;
            OHandle::ClearScreenGuard repetovsky_guard;

            // Основна схема знаходиться під 0-вим індексом.
        	MovieDatabase::try_import(*schema_ptr->database_ptr_at(BASE_ID));
            IHandle::anykey(LIGHT_CYAN + L"\nНатисність будь-яку клавішу..." += WHITE);
        }
        catch (...) {
            LogHandle::error(L"<<?!>> При імпортуванні виникла несподівана помилка. <<?!>>");
            IHandle::anykey();
        }
    }
    else {
        OHandle::ClearScreenGuard repetovsky_cls_guard;
        LogHandle::info(CLEAR_COLOR + L"[-] Імпортування файлів " + LIGHT_RED + L"ДЕАКТИВОВАНО " + WHITE += L"[-]");
    	wcout << L"[Використання] : \n" << argv[0] << RED << " -import"  << WHITE << "\n";
        IHandle::anykey(LIGHT_CYAN + L"\nНатисність будь-яку клавішу, щоб розпочати використовувати застосунок..." += WHITE);
    }

    // Статична змінна для відстеження ідентифікатора схеми.
    static size_t schema_id { Schema::getid() };

    // Система резевного копіювання.
    // По замовчуванню кожних 5 хвилин зберігає копію схеми, у якій знаходиться користувач.
    // TODO: В майбутньому планується додати статус та таймер до наступного копіювання, які будуть відображені в меню.
	unique_ptr<Schema::Backup> backup_system = make_unique<Schema::Backup>(*schema_ptr, schema_id);

    /* Ієрархічна задумка для меню. В особливості жахлива)
     * "Якщо необхідно скористатись меню, то виділи мені ресурс під неї."
     * "Вихід з меню - очистка ресурсу."
     * Ресурс меню - це деякий процес, що складається з назви процесу та функції див. Menu.hpp
     */
	unique_ptr<Console::Menu>
	main_menu              = nullptr,
		submenu_1          = nullptr,
		submenu_2          = nullptr,
			submenu_3      = nullptr,
				submenu_4  = nullptr,
			submenu_5      = nullptr;

    // Лямбда-обгортка навколо методу для виконання меню та спроби покинути його.
    // Підчищає ресурс після підтвердження виходу.
    // Не надто продуктивний варіант.
    auto execute = [](Console::Menu& menu) {
        for (;;) {
            menu.execute();
            if (menu.try_exit()) {
                menu.~Menu();
                break;
            }
        }
    };

    
    while (true) 
    {
        try {
            main_menu = make_unique<Console::Menu>();
            /************************************************************************************/
            main_menu->emplace_process(make_pair(L"Розпочати роботу з схемою [" + schema_ptr->get_name(schema_id) + L"]", [&]
            {
                 submenu_1 = make_unique<Console::SubMenu>();
                 /************************************************************************************/
                 submenu_1->emplace_process(make_pair(L"Створити кінотайтл", [&]{
	                 create<MovieDatabase>(*schema_ptr->database_ptr_at(schema_id));
                 }));
                 /************************************************************************************/

                 /************************************************************************************/
                 submenu_1->emplace_process(make_pair(L"Редактор 'codeit'", [&] {
                        submenu_2 = make_unique<Console::SubMenu>();
                        submenu_2->set_heading(L"Редагувати...");
                        submenu_2->emplace_process(make_pair(L"Редагування файлу..", [&] {
                            while (true) {
                                OHandle::CursorVisibilityGuard repetosky_guard(true);
                                LogHandle::info(L" Введіть абсолютний або відносний шлях до файлу.\n  * Приклад абсолютного шляху С:/Users/myName/Desktop/myfile.txt \n  * Приклад відносного  шляху ../myFiles/myfile.txt");
                                LogHandle::info(L" Щоб повернутися назад, введіть 'exit'");
                                LogHandle::warn(L" [До вашого відома]: \n   * Щоб пересуватись між папками використовуйте символ '/', не рекомендується використовувати '\\'.");
                                LogHandle::warn(L"   * Проте, якщо використовуєте символ '\\' у шляху, тоді символів має бути '\\\\'.");
                                LogHandle::warn(L"   * Редактор поки що не може редагувати надто великі об'єми тексту. У майбутньому планується вирішити...");
                                LogHandle::info(L" Приклад: С:\\\\Users\\\\myName\\\\Desktop\\\\myfile.txt");

                            	auto user_input = IHandle::wrap_input_string();
                                auto& potential_command = user_input;

                                transform(potential_command.begin(), potential_command.end(), potential_command.begin(), towlower);
                                if (potential_command == L"exit") { rlutil::cls(); return; }

                                const filesystem::path path = user_input;
                                auto fs_path = path.lexically_normal();

                                if (fs_path.is_relative()) fs_path = absolute(fs_path);
                                wcout << L" Шлях до вашого файлу: " << fs_path;

                                if (submenu_2->is_confirmed(L" Перевірте, чи вірно введено шлях до файлу.")) {
                                    const wstring command = L"start codeit/codeit.exe " + fs_path.wstring();
                                    if (_wsystem(command.c_str()) != 0)
                                        LogHandle::error(L"Виникла помилка про спробі запуску 'codeit'.");
                                    break;
                                }
                            }
                        }));
                        submenu_2->emplace_process(make_pair(L"Редагувати записи схеми [" + schema_ptr->get_name(schema_id) + L"]", [&] {
                            update(*schema_ptr->database_ptr_at(schema_id));
                        }));
                        execute(*submenu_2);
                        }));
                 /************************************************************************************/

                 /************************************************************************************/
                 submenu_1->emplace_process(make_pair(L"Знайти за...", [&] {
                      submenu_2 = make_unique<Console::SubMenu>();
                      submenu_2->set_heading(L"Знайти за критерією...");
                      submenu_2->emplace_process(make_pair(L"Назва", [&] {
                          const auto& repetovsky_title = IHandle::wrap_input_string(L"Назва тайтлу");
                          find_by
                      	  (
                      	  Criteria().title
                      	          (
									repetovsky_title,
                                  false
								  )
                                 .build(),
                              true,
                              *schema_ptr->database_ptr_at(schema_id)
                          );
                      }));
                      submenu_2->emplace_process(make_pair(L"Режисер", [&] {
                          const auto& repetovsky_dirctor = IHandle::wrap_input_string(L"Як звати вуйка ?");
                          find_by
                          (
                              Criteria().director
                              (
                                  repetovsky_dirctor,
                                  false
                              )
                              .build(),
                              false,
                              *schema_ptr->database_ptr_at(schema_id)
                          );
                      }));
                      submenu_2->emplace_process(make_pair(L"Проміжок року випуску", [&] {
                          LogHandle::info(L"Пошук у форматі 'від-до'.");
                      	  LogHandle::info(L"Спочатку вводиться мінімальний, а потім максимальний рік.");
                          const auto repetovsky_year_first = IHandle::wrap_input<size_t>(L"min");
                          const auto repetovsky_year_second = IHandle::wrap_input<size_t>(L"max");
                          find_by
                          (
                              Criteria().year_range
                              (
                                  repetovsky_year_first,
                                  repetovsky_year_second
                              )
                              .build(),
                              false,
                              *schema_ptr->database_ptr_at(schema_id)
                          );
                          IHandle::reset_input_stream();
                      }));
                      submenu_2->emplace_process(make_pair(L"Жанр", [&] {
	                      const auto& repetovsky_genre = IHandle::wrap_input_string(L"Жанр");
                          find_by
                          (
                              Criteria().genre
                              (
                                  repetovsky_genre,
                                  false
                              )
                              .build(),
                              false,
                              *schema_ptr->database_ptr_at(schema_id)
                          );
                      }));
                      submenu_2->emplace_process(make_pair(L"Середня оцінка", [&]{
	                      const auto repetovsky_avg_rating = IHandle::wrap_input<float>(L"Сер. оцінка");
                          find_by
                          (
                              Criteria().avg_rating_above(repetovsky_avg_rating).build(),
                              false,
                              *schema_ptr->database_ptr_at(schema_id)
                          );
                      }));
                      submenu_2->emplace_process(make_pair(L"Загальна оцінка", [&] {
	                      const auto repetovsky_no_of_ratings = IHandle::wrap_input<size_t>(L"Заг. оцінка");
                          find_by
                          (
                              Criteria().no_of_ratings_above(repetovsky_no_of_ratings).build(),
                              false,
                              *schema_ptr->database_ptr_at(schema_id)
                          );
                      }));
                      submenu_2->emplace_process(make_pair(L"<*> Завдання курсового проєкту [1]", [&]
                      {
                          OHandle::ClearScreenGuard repetovsky_guard; 

                          /*Визначити загальний прибуток від фільмів, випущених у вказаний період часу.*/

                          LogHandle::info(L"Визначити загальний прибуток від фільмів, випущених у вказаний період часу.");
                          auto repetovsky_year = IHandle::input_with_validation<size_t>([](const size_t my_year) {
                              return my_year > 1900 && my_year <= CURRENT_YEAR;
                          }, L"<!> Рік не в логічних рамках існування кіноматографу. <!>");
                          MovieDatabase& my_mdb(*schema_ptr->database_ptr_at(schema_id));
                          size_t total_revenue = 0,
							     total_movies_match = 0;
                          LogHandle::info(L"___________________________________________________________________");
                          for_each(my_mdb.begin(), my_mdb.end(), [&](const Movie& m) {
                             if (repetovsky_year == m.get_year()) {
                                 ++total_movies_match;
                                 total_revenue += m.get_revenue();
                                 LogHandle::info(L"____________________________________________________________");
                                 LogHandle::info(L"[Назва кінотайтлу]: " + m.get_title());
                                 LogHandle::info(L"[Рік]: " + to_wstring(m.get_year()));
                                 LogHandle::info(L"[Дохід]: " + to_wstring(m.get_revenue()));
                                 LogHandle::info(L"_____________________________________________________________");
                             }
                          });
                      	  LogHandle::info(L"___________________________________________________________________");
                          LogHandle::log(L"Загальна к-сть кінотайтлів, що підпадають під " + to_wstring(repetovsky_year) 
											  + L"-ий рік складає: " + to_wstring(total_movies_match) );
                          LogHandle::log(L"Загальний дохід у $ США складає: " + to_wstring(total_revenue));
                          IHandle::anykey();
                      }));
                      submenu_2->emplace_process(make_pair(L"<*> Завдання курсового проєкту [2]", [&] 
                      {
                          /*К-сть кінотайтлів, бюджет яких не перевищує прибутку.*/
                          OHandle::ClearScreenGuard repetovsky_guard;
                          MovieDatabase& my_mdb(*schema_ptr->database_ptr_at(schema_id));
                          size_t total_movies_match = 0;
                          LogHandle::info(L"___________________________________________________________________");
                          for_each(my_mdb.begin(), my_mdb.end(), [&](const Movie& m) {
                              if (m.get_budget() <= m.get_revenue()) {
                                  ++total_movies_match;
                                  LogHandle::info(L"________________________________________________________");
                                  LogHandle::info(L"[Назва]: " + m.get_title());
                                  LogHandle::info(L"[Бюджет | Прибуток]: " + to_wstring(m.get_budget()) + L" <= " + to_wstring(m.get_revenue()) + L" (у $ США)");
                                  LogHandle::info(L"________________________________________________________");
                              }
                          });
                          LogHandle::info(L"___________________________________________________________________");
                          LogHandle::log(L"Загальна к-сть кінотайтлів, бюджет яких не перевищує прибуток складає: " + to_wstring(total_movies_match));
                          IHandle::anykey();
                      }));
                      execute(*submenu_2);
                 }));
                 /************************************************************************************/

                 /************************************************************************************/
                 submenu_1->emplace_process(make_pair(L"Сортувати за...", [&] {
                     auto& db = schema_ptr->database_ptr_at(schema_id);
                     submenu_2 = make_unique<Console::SubMenu>();
                     submenu_2->set_heading(L"Сортувати за критерією...");
                     submenu_2->emplace_process(make_pair(L"Назва (абетка)", [&] { sort(*db, Criteria().title_comparator);}));
                     submenu_2->emplace_process(make_pair(L"Рік", [&] {  sort(*db, Criteria().year_comparator); }));
                     submenu_2->emplace_process(make_pair(L"Країни (абетка)", [&] {  sort(*db, Criteria().сountries_comparator); }));
                     submenu_2->emplace_process(make_pair(L"Продовжуваність", [&] {  sort(*db, Criteria().duration_comparator); }));
                     submenu_2->emplace_process(make_pair(L"Сер. оцінка", [&] { sort(*db, Criteria().avg_rating_comparator); }));
                     submenu_2->emplace_process(make_pair(L"Заг. к-сть оцінок", [&] {  sort(*db, Criteria().no_of_ratings_comparator); }));
                     submenu_2->emplace_process(make_pair(L"Бюджет", [&] {  sort(*db, Criteria().duration_comparator); }));
                     submenu_2->emplace_process(make_pair(L"Дохід", [&] {  sort(*db, Criteria().revenue_comparator); }));
                     execute(*submenu_2);
                 }));

                 /************************************************************************************/
                 submenu_1->emplace_process(make_pair(L"Видалити за...", [&] {
                      auto& db = schema_ptr->database_ptr_at(schema_id);
                      submenu_2 = make_unique<Console::SubMenu>();
                      submenu_2->set_heading(L"Видалити за...");
                      submenu_2->emplace_process(make_pair(L"За номером", [&]{
                          backup_system->trigger_one(schema_id, true);
	                      delete_at_index(*db);
                      }));

                      submenu_2->emplace_process(make_pair(L"За критерією...", [&] {

                          submenu_3 = make_unique<Console::SubMenu>();
                          auto at_least_three_chars = [](const wstring& input = L"abc"){
                              return input.size() >= 3;
                          };
                          backup_system->trigger_one(schema_id, false);
                          submenu_3->set_heading(L"Видалити за критерією...");
                          submenu_3->emplace_process(make_pair(L"Назва", [&] {
                          	 delete_by(Criteria()
                                 .title
                                 (
	                                IHandle::wstring_with_validation
	                                (
	                                    at_least_three_chars,
	                                    L"Введіть принаймі 3 літери кінотайтлу, який бажаєте видалити."
	                                ), true
                                 ).build(), *db);
                          }));
                          submenu_3->emplace_process(make_pair(L"Режисер", [&] {
	                          delete_by(Criteria()
                                  .director
                                  ( 
                                      IHandle::wstring_with_validation(
	                                      at_least_three_chars,
	                                      L"Введіть принаймі 3 літери кінотайтлу, який бажаєте видалити."), 
	                                      true
                                  ).build(), *db);
                          }));
                          submenu_3->emplace_process(make_pair(L"Рік випуску", [&] {
                             size_t user_defined_year = IHandle::input_with_validation<size_t>
                          	 (
                                 [](const size_t year)
                                 {
                                     return year > 1900 && year < 2025;
                                 },
                                 L"Ввід не в рамках логічного існування кіноматографу"
                             );
                          	 delete_by(Criteria()
                          		 .custom
                                 (
                                     [user_defined_year](const Movie& m) {
                                         return m.get_year() == user_defined_year;
                                     }
                                 ).build(), *db);
                          }));
                          submenu_3->emplace_process(make_pair(L"Жанр", [&] {
                              delete_by(Criteria()
                                  .genre
                                  (
									  IHandle::wstring_with_validation(
	                                      at_least_three_chars,
	                                      L"Введіть принаймі 3 літери жанру, щоб видалити набір кінотайтлів."),
	                                  true)
	                                  .build(), *db
								  );
                          }));
                          submenu_3->emplace_process(make_pair(L"Середня оцінка вище за...", [&] {
                              delete_by(Criteria()
                                  .avg_rating_above
                                  (
	                                  IHandle::wrap_input<float>())
	                                  .build(), *db
								  );
                          }));
                          submenu_3->emplace_process(make_pair(L"Загална к-сть оцінок вище за..", [&] {
                              delete_by(Criteria()
                                  .no_of_ratings_above
                                  (
									  IHandle::wrap_input<size_t>())
	                                  .build(), *db
								  );
                          }));
                          execute(*submenu_3);
                      }));

                      submenu_2->emplace_process(make_pair(L"Видалити все", [&] {
                          if (submenu_2->is_confirmed(L"Ви збираєтесь видалити увесь вміст схеми [" + schema_ptr->get_name(schema_id) + L"]. Впевнені ?")) {
                              backup_system->trigger_one(schema_id, false);
                              delete_all_content(*db);
                          }
                          else LogHandle::info(L"Відміна операції...");
                      	  this_thread::sleep_for(0.75s);
                      }));
                      execute(*submenu_2);
                 }));
                 /************************************************************************************/
                 /************************************************************************************/
                 submenu_1->emplace_process(make_pair(L"Відобразити вміст схеми...", [&] {
                     display(*schema_ptr->database_ptr_at(schema_id), false, L"");
                 }));
                 /************************************************************************************/
                 execute(*submenu_1);
            }));
        	/************************************************************************************/

            /************************************************************************************/
            main_menu->emplace_process(make_pair(L"Імпортувати у [" + schema_ptr->get_name(schema_id) += L"]", [&] {
                /************************************************************************************/
	                OHandle::ClearScreenGuard repetovsky_guard;
	                MovieDatabase::try_import(*schema_ptr->database_ptr_at(schema_id));
	                IHandle::anykey( L"\nНатисність будь-яку клавішу...");
                /************************************************************************************/
            }));
            /************************************************************************************/

            /************************************************************************************/
		    main_menu->emplace_process(make_pair(L"Експортувати [" + schema_ptr->get_name(schema_id) += L"] у...", [&] {
                submenu_2 = make_unique<Console::SubMenu>();
                submenu_2->set_heading(L"Експортувати схему [" + schema_ptr->get_name(schema_id) += L"] у ...");
                /************************************************************************************/
                if (schema_ptr->size() > 1)
					submenu_2->emplace_process(make_pair(L"Існуючі схеми", [&] {
						submenu_3 = make_unique<Console::SubMenu>();
                        submenu_3->set_heading(L"Вибір схеми...");
						const auto& focused_db_ptr = schema_ptr->database_ptr_at(schema_id);

                        for (const auto& [id , values] : *schema_ptr) {
	                        auto target_db = values.second.first;
                            if (id == schema_id) continue;

                            submenu_3->emplace_process(make_pair(values.first, [&] {
                            	submenu_4 = make_unique<Console::SubMenu>();
                                
                                submenu_4->emplace_process(make_pair(L"Повне копіювання", [&] {
                                    OHandle::ClearScreenGuard repetovsky_guard;
                                    LogHandle::log(L"Клоную схему...");
                                    schema_ptr->database_ptr_at(schema_id)->copy_to(target_db);
                                    IHandle::anykey(L"[...] Натисність будь-яку клавішу... [...]");
                                }));
                                
                                submenu_4->emplace_process(make_pair(L"Вибіркове копіювання", [&] {
                                    extract(*focused_db_ptr, *target_db);
                                }));
                                
                                submenu_4->emplace_process(make_pair(L"За критерією", [&] {
                                    submenu_5 = make_unique<Console::SubMenu>();

                                	submenu_5->set_heading(L"Вибір критерії...");
                                    submenu_5->emplace_process(make_pair(L"Назва", [&] {
	                                    schema_ptr->database_ptr_at(schema_id)->copy_if(
                                            target_db, Criteria().title(
                                                IHandle::wrap_input_string(L"Назва")).build());
                                    }));
                                    submenu_5->emplace_process(make_pair(L"Режисер", [&] {
	                                    schema_ptr->database_ptr_at(schema_id)->copy_if(
                                            target_db, Criteria().director(
                                                IHandle::wrap_input_string(L"Як звати вуйка ?")).build());
                                    }));
                                    submenu_5->emplace_process(make_pair(L"Країна", [&] {
	                                    schema_ptr->database_ptr_at(schema_id)->copy_if(
                                            target_db, Criteria().countries(
                                                IHandle::wrap_input_string(L"Країни")).build());
                                    }));
                                    submenu_5->emplace_process(make_pair(L"Жанр", [&] {
	                                    schema_ptr->database_ptr_at(schema_id)->copy_if(
                                            target_db, Criteria().genre(
                                                IHandle::wrap_input_string(L"Жанр")).build());
                                    }));
                                    submenu_5->emplace_process(make_pair(L"Рік", [&] {
	                                    const auto repetovsky_min = IHandle::wrap_input<size_t>(L"min"),
	                                               repetovsky_max = IHandle::wrap_input<size_t>(L"max");
	                                    schema_ptr->database_ptr_at(schema_id)->copy_if(
                                            target_db, Criteria().year_range(
                                                repetovsky_min
                                                ,
                                                repetovsky_max
                                                ).build());
                                    }));
                                    submenu_5->emplace_process(make_pair(L"Середня оцінка вище за...", [&] {
                                        LogHandle::info(L"Кінотайтли, оцінка яких вища за вказану.");
	                                    schema_ptr->database_ptr_at(schema_id)->copy_if(
                                            target_db, Criteria().avg_rating_above(
                                                IHandle::wrap_input<float>()
                                               ).build());
                                    }));
                                    submenu_5->emplace_process(make_pair(L"Середня оцінка нижче за...", [&] {
                                        LogHandle::info(L"Кінотайтли, оцінка яких нижча за вказану.");
                                        schema_ptr->database_ptr_at(schema_id)->copy_if(
                                            target_db, not1(Criteria().avg_rating_above(
                                                IHandle::wrap_input<float>()).build()));
                                    }));
                                    submenu_5->emplace_process(make_pair(L"Загальна к-сть оцінок вище за..", [&] {
                                        LogHandle::info(L"Кінотайтли, заг. к-сть оцінок вища за вказану.");
	                                    schema_ptr->database_ptr_at(schema_id)->copy_if(
                                            target_db, Criteria().no_of_ratings_above(
                                                IHandle::wrap_input<size_t>())
												.build());

                                    }));
                                    submenu_5->emplace_process(make_pair(L"Загальна к-сть оцінок нижче за..", [&] {
                                        LogHandle::info(L"Кінотайтли, заг. к-сть оцінок нижча за вказану.");
                                        schema_ptr->database_ptr_at(schema_id)->copy_if(
                                            target_db, not1(Criteria().no_of_ratings_above(
                                                IHandle::wrap_input<size_t>())
                                               .build()));
                                    }));
                                    execute(*submenu_5);
                                }));
                                execute(*submenu_4);
                            }));
                        }
	                    execute(*submenu_3);
					}));
                /************************************************************************************/

                /************************************************************************************/
		    	submenu_2->emplace_process(make_pair(L"Файл", [&]  {
		    		OHandle::ClearScreenGuard repetovsky_cls_guard;
		    		MovieDatabase::try_export(*schema_ptr->database_ptr_at(schema_id));
		    	}));
		    	/************************************************************************************/
            	execute(*submenu_2);
            }));
            /************************************************************************************/

            /************************************************************************************/
            main_menu->emplace_process(make_pair(L"Схеми...", [&] {
                submenu_1 = make_unique<Console::SubMenu>();
                submenu_1->set_heading(L"Cхема...");
                /************************************************************************************/
                submenu_1->emplace_process(make_pair(L"Створити нову схему", [&] {
                    OHandle::CursorVisibilityGuard repetovsky_cursor_guard(true);
                    int exit = 0;
                    wstring possible_command;
                    do {
                        OHandle::ClearScreenGuard repetovsky_cls_guard;
                        LogHandle::info(L"Розділ для створення нової схеми.");
                        LogHandle::warn(L"[До вашого відома]:");
                        LogHandle::warn(L"  * Назва схеми повинна бути унікальною.");
                        LogHandle::info(L"[Вказівки]: ");
                        LogHandle::info(L"  * Спершу вводиться назва схеми, а потім опис.");
                        LogHandle::info(L"  * Назва схеми повинна не перевищувати 16 символів.");
                        LogHandle::info(L"Вихід -> exit");

                        wstring user_defined_name = possible_command = IHandle::wstring_with_validation
                    	(
                            [](const wstring& u_in) {
                                return u_in.size() < 24;
                            }
                        	, 
                            L"<!> Назва перевищує 24 символи <!>", 
                            L"Назва"
                        );

                        if (possible_command == L"exit") return;
                        IHandle::to_wlower(possible_command);

                    	wstring description = possible_command = IHandle::input_multiline(L"Опис");
                        IHandle::to_wlower(possible_command);
                        if (possible_command == L"exit") return;

                        LogHandle::info(L"[Назва схеми]: " + user_defined_name);
                        LogHandle::info(L"[Опис схеми]: "  + description);

                        if (!submenu_1->is_confirmed(L"Перевірте, чи дані введено коректно.")) continue;

                    	if (schema_ptr->emplace(user_defined_name, description) != 0) 
                            LogHandle::success(L"[\\/] Успішно додано нову схему \"" + user_defined_name + L"\" [\\/]");
                        else LogHandle::error(L"<!> Не вдалось додати схему. <!>");

                        LogHandle::info(L"Вихід - [ESC], продовжити - будь-яка клавіша.");

                        switch (exit = _getch()) case Console::ARROW_ALT: if (_getch() == Console::ESCAPE) return;

                    } while (exit != Console::ESCAPE);
                }));
                /************************************************************************************/

                /************************************************************************************/
                submenu_1->emplace_process(make_pair(L"'Водоспадний' перегляд схем", [&] {
                    OHandle::ClearScreenGuard repetovsky_cls;
                    for (const auto& s : *schema_ptr)
                        Schema::printf(s);

                    LogHandle::info(L"Натисність будь-яку клавшіу...");
                    rlutil::anykey();
                }));
                /************************************************************************************/

                /************************************************************************************/
                submenu_1->emplace_process(make_pair(L"Змінити схему", [&] {
                        if (schema_ptr->size() == 1) {
                            LogHandle::warn(L"[!] На даний момент немає жодних схем окрім основної (BASE), додайте одну через відповідний пункт в меню. [!]");
                            IHandle::anykey(L"[!] Натисність будь-яку клавішу... [!]");
                            rlutil::cls();
                            return;
                        }

                        OHandle::ClearScreenGuard repetovsky_guard;
                        size_t index = 0, try_interact, schema_size = schema_ptr->size() - 1;


                        auto it = schema_ptr->begin();

                        do {
                            rlutil::locate(5, 7);
                            wcout << L"Схема " + to_wstring(index + 1) + L" з " + to_wstring(schema_ptr->size());
                            rlutil::locate(1, 8);
                            LogHandle::info(wstring(
                                L"  * Щоб навігувати, користуйтесь стрілками [<- ->] \n").append(
                                    L"  * Щоб обрати схему, натисність [ENTER]\n").append(
                                        L"  * Щоб вийти, натисність [ESC] \n")
                            );

                            rlutil::locate(1, 1);
                            Schema::printf(*it);

                            switch (try_interact = _getch()) {
	                            case Console::ARROW_RIGHT: {
	                                repetovsky_guard.~ClearScreenGuard();

                                    if (it == --schema_ptr->end()) { index = 0;  it = schema_ptr->begin(); }
                                    else { ++index; ++it; }
	                                break;
	                            }

	                            case Console::ARROW_LEFT: {
	                                repetovsky_guard.~ClearScreenGuard();
                                    if (it == schema_ptr->begin()) { index = schema_size; it = --schema_ptr->end(); }
                                    else { --index; --it; }
	                                break;
	                            }

	                            case Console::ENTER: {
	                                repetovsky_guard.~ClearScreenGuard();
	                                if (it->first == schema_id) return;
	                                schema_id = index;
	                                throw Schema::Update(index, schema_ptr->at(index));
	                            }

	                            case Console::ESCAPE: {
	                                repetovsky_guard.~ClearScreenGuard();
	                                return;
	                            }
                            }
                        } while (try_interact != Console::ESCAPE);

                }));
                /************************************************************************************/

                /************************************************************************************/
                submenu_1->emplace_process(make_pair(L"Видалити схему...", [&] {
                    if (schema_ptr->size() == 1) {
                        LogHandle::warn(L"[!] На даний момент немає жодних схем окрім основної (BASE), додайте одну через відповідний пункт в меню. [!]");
                        IHandle::anykey(L"[...] Натисність будь-яку клавішу... [...]");
                        rlutil::cls();
                        return;
                    }
                    submenu_2 = make_unique<Console::SubMenu>();

                    for (const auto& sch : *schema_ptr) {
                        size_t local_id = sch.first;
                        if (local_id == 0) continue;
                        submenu_2->emplace_process(make_pair(schema_ptr->get_name(local_id), [&] {

                            if (!submenu_2->is_confirmed(L" * Ви впевнені, що хочете видалити схему ?")) return;

                            backup_system->trigger_one(local_id, false);
                            if (local_id == schema_id) {
                                schema_id = 0;
                                schema_ptr->remove(local_id);
                            	auto& BASE = schema_ptr->at(0);
                                throw Schema::Update(0, BASE);
                            }
                            schema_ptr->remove(local_id);
                            throw Schema::Update(local_id, schema_ptr->at(local_id));
                        }));
                    }
					 execute(*submenu_2);
            	 }));
                 /************************************************************************************/
            	execute(*submenu_1);
            }));
            /************************************************************************************/

            /************************************************************************************/
            main_menu->emplace_process(make_pair(L"Резервне копіювання...", [&] {
                submenu_1 = make_unique<Console::SubMenu>();
                submenu_1->set_heading(L"Налаштування системи резервного копіювання...");
                /************************************************************************************/
                submenu_1->emplace_process(make_pair(L"Створити резервну копію схеми [" + schema_ptr->get_name(schema_id) + L"]", [&] {
                    OHandle::ClearScreenGuard repetovsky_cls_guard;
                    if (schema_ptr->database_ptr_at(schema_id)->empty()) {
                        LogHandle::error(L"Записи відсутні, відхилення резервного копіювання...");
                        this_thread::sleep_for(2s);
                        return;
                    }
                    LogHandle::info(L"\n * Інтервальна затримка - 1 сек.");
                    backup_system->trigger_one(schema_id, false);
                    this_thread::sleep_for(1s);
                    IHandle::anykey(L"[Abcd] Натисніть будь-яку клвішу... [Abcd]");
                }));
                /************************************************************************************/

                /************************************************************************************/
                submenu_1->emplace_process(make_pair(L"Встановити таймер резервного копіювання", [&] {
                    OHandle::ClearScreenGuard repetovsky_cls;

                    LogHandle::info(L"У цьому розділі ви можете встановити часовий проміжок автоматичного резервного копіювання.");
                    LogHandle::warn(L"[До вашого відома]: \n * Мінімальний час - 5 хв. (по замовчуванню) \n * Максимальний час - 60 хв.");
                    while (true) {
                        auto time_in_minutes = IHandle::input_with_validation<size_t>
                    	([](const size_t backup_time) {
                            return backup_time >= 5 && backup_time <= 60;
                        },
                            L"* Максимально допустимий час : 60 хвилин. \n* Мінімально допустимий час : 5 хвилин. (по замовчуванню)"
                        );
                        IHandle::reset_input_stream();
                        backup_system->set_backup_interval(chrono::minutes(time_in_minutes));
                        LogHandle::success(L"[\\/] Встановлено час " + to_wstring(time_in_minutes) + L" хв. [\\/]");
                        if (!submenu_1->is_confirmed(L"Бажаєте продовжити введення ?")) break;
                    }
                }));
                /************************************************************************************/

                submenu_1->emplace_process(make_pair(L"Резервна копія усіх схем", [&]{
                    OHandle::ClearScreenGuard repetovsky_cls_guard;
                    for (const auto& sch : *schema_ptr) {
	                    const auto id = sch.first;
                        if (!schema_ptr->database_ptr_at(id)->empty()) {
                            backup_system->trigger_one(id, true);
                            LogHandle::success(L" -- Створена резервна копія схеми " + schema_ptr->get_name(id));
                        }
                        else LogHandle::error(L"Резервну копію схеми " + schema_ptr->get_name(id) + L" не створено, оскільки вона порожня.");
                    }
                    IHandle::anykey();
                }));
                execute(*submenu_1);
            }));
            /************************************************************************************/

            /************************************************************************************/
            main_menu->emplace_process(make_pair(L"Завершити роботу", nullptr));
            /************************************************************************************/

            // Цикл для виконання головного меню.
        	for (;;) {
                main_menu->execute();
                if (main_menu->try_exit()) {
                	rlutil::RemoveSignalHandler();
                	return EXIT_SUCCESS;
                }
                main_menu.reset();
                break;
            }
        }
        catch (...) {
            if (submenu_4 != nullptr) submenu_4.reset();
            if (submenu_3 != nullptr) submenu_3.reset();
            if (submenu_2 != nullptr) submenu_2.reset();
            if (submenu_1 != nullptr) submenu_1.reset();
            if (main_menu != nullptr) main_menu.reset();
            IHandle::anykey(  L"[/\\] Натисність будь-яку клавішу, щоб продовжити... [/\\]");
        }
    }
}
