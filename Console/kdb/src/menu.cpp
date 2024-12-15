#include "../src/Menu.hpp"
#include "../src/Utils.hpp"
using namespace MvUtils;
using namespace Console;


/*::::::::::::::::::::::::::::::::::::::::::::[Допоміжна friend функція для відцентрування тексту]::::::::::::::::::::::::::::::::::::::::::::*/
void Menu::printf
(
    const wstring& target_string, 
    const wstring& params, 
    const wstring& params_clear = CLEAR_COLOR,
    const size_t max_size = DEFAULT_STRING_LENGTH,
    const int ratio = 2
)
{
	const size_t space_size = (max_size - target_string.size()) / ratio;
	for (size_t i = 1; i <= max_size; ++i) {
		i <= space_size || i > space_size + target_string.size() ?
			wcout << FILL_WITH
		:
			wcout << params << target_string[i - space_size - 1] << params_clear;
	}

}
/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/


/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
Menu::Menu() : _heading(L"КІНОСТУДІЯ - ГОЛОВНЕ МЕНЮ"), _max_str_len(DEFAULT_STRING_LENGTH),
               _parent_process_vector({}), _parent_iterator_position(_parent_process_vector.begin())
{}

Menu::~Menu() {
	Menu::clear_cache();
    _max_str_len = 0;
}

Menu::Menu(wstring _heading, const size_t _str_len)
 : _heading(std::move(_heading)), _max_str_len(_str_len), _parent_iterator_position(_parent_process_vector.begin())
{
	if (_str_len < 64 || _str_len >= LONG_MAX) {
        LogHandle::error(L"[error]: назва меню повинна бути хоча б 64 cимволи у довжину і точно невідємна. За замовчуванням буде виставлено 64.");
        _max_str_len = DEFAULT_STRING_LENGTH;
        this_thread::sleep_for(2s);
		return;
	}
	_max_str_len = _str_len;
}

void Menu::set_str_max_len(const size_t str_len) {

    if (_max_str_len < 64 || _max_str_len >= LONG_MAX) {
    	_max_str_len = DEFAULT_STRING_LENGTH;
        return;
    }

    _max_str_len = str_len;
}
void Menu::emplace_process(const process& parent_process_module) {
    _parent_process_vector.emplace_back(parent_process_module.first, parent_process_module.second);
    _parent_iterator_position = _parent_process_vector.begin();
}
bool Menu::remove_process(const wstring& process_heading) {

    for (auto it = _parent_process_vector.begin(); it != _parent_process_vector.end(); ++it) {
        if (it->first == process_heading) {
        	_parent_process_vector.erase(it);
            _parent_iterator_position = _parent_process_vector.begin();
            return true;
        }
    }

    LogHandle::error(L"<<!?>> Процес з таким іменуванням відсутній. <<?!>> ");
    return false;
}

void Menu::clear_cache() { _parent_process_vector.clear(); }
void Menu::printm() {
   
    if (_max_str_len < 64 || _max_str_len > LONG_MAX) {
        LogHandle::error(L"<<!>> Помилка при спробі відмалювати меню. <<!>>\nБудуть застосовані рекомендовані зміни.");
        _max_str_len = DEFAULT_STRING_LENGTH;
        this_thread::sleep_for(2.5s);
        rlutil::cls();
    }

    wcout << wstring(_max_str_len + 2, ':') << endl << "|";
    printf(_heading,  UNDERLINE + LIGHT_RED, CLEAR_UNDERLINE + CLEAR_COLOR);
    wcout << "|" << endl;

    wcout << "|";
    printf(L"[?] Натисніть [F1] для користувацької довідки. [?]", LIGHT_CYAN);
    wcout << "|" << endl;

    wcout << "|";
    printf(L"<!> Натисніть [F2] щоб очистити дисплей. <!>", WHITE);
    wcout << "|" << endl;

    wcout << "|";
    printf(L"<[]> Натисніть [TAB] щоб змінити вигляд меню. [<>]",  MAGENTA);
    wcout << L"|" << endl << wstring(_max_str_len + 2, L':') << endl;
    for (auto it = _parent_process_vector.begin(); it != _parent_process_vector.end(); ++it) {
        wcout << (it != _parent_iterator_position ? "|" : "[");
        printf(it->first,it == _parent_iterator_position ? LIGHT_RED : YELLOW);
        wcout << (it != _parent_iterator_position ? "|" : "]") << endl;
    }
    wcout << wstring(_max_str_len + 2, ':') << endl;
	
}

void Menu::help()  {

    // Кращий варіант використовувати блок літералів LR"()".

    wcout  << LIGHT_CYAN << L" +__________[Клавішний ввід]__________+------[Види меню]------+__________[Примітивні команди]__________+\n" 
                         << L" | * вибір - [↑] [↓] [<] [>],         |     Головне меню      |  * cls - очищення екрану у вклад. меню |\n" 
                         << L" | * підтвердження - [ЕНТЕР]          |       Під-меню        |  * exit - вихід з вкладеного меню      |\n" 
                         << L" | * маніп. з дисплеєм [F2], [TAB]    |     Вкладені меню     |  * help - користувацька довідка (ця)   |\n"
                         << L" | * вихід з меню - [ESC]             |                       |                                        |\n"
                         << L" +____________________________________+-----------------------+________________________________________+\n"
                         << L"/___________________________________ [ Робота з файлом бази даних ] ____________________________________\\\n"
                         << L"| * Допускається імпортування файлів типу .txt або .csv. Файл повинен мати іменування 'movies'.         |\n"
                         << L"|   <!> Слід дотримуватись строгої послідовності у файлі, а саме :                                      |\n"
		                 << L"| * [\"Назва\"],                    _[Приклад]____________________________________________________________|\n"
		                 << L"| * [\"Режисер(и)\"],              |  * Даний формат дійсний і для додавання кінотайтла вручну.           |\n"
		                 << L"| * [\"*Країна(и) походження\"],   |  \"Морія Сувако,\"Погір'я долі\",\"Японія/США\",2004,\"M\",\"Пригоди\",       |\n"
		                 << L"| * [Рік],                       |  127,10,10,666,666 (розділ - Розпочати роботу з схемою [назва])      |\n" 
		                 << L"| * [\"*Вікова оцінка\"],    ______|___________________________________________________________________   |\n"
		                 << L"| * [\"*Жанр(и)\"],         | [Пояснення та вказівки] :                                                   |\n"
		                 << L"| * [Продовжуваність],    | -- Окремі поля, що знаходяться у лапках повинні з ними ж заноситися у файл. |\n"
		                 << L"| * [Середня оцінка],     | -- Поля, що виділені '*' мають вбудовану підтримку.                         |\n"
		                 << L"| * [Кількість оцінок],   |    Нижче буде описано перелік.                                              |\n"
		                 << L"| * [Бюджет],             | -- Дані заносяться послідовно через одинарний пунктуаціний знак.            |\n"
		                 << L"| * [Дохід]               | -- НЕ використовувати для розділення більше як один символ.                 |\n"
		                 << L"|                         | -- НЕ використовувати '\\' для розділення.                                   |\n"
						 << L"|                         | -- Хорошою практикою буде використання символу ',' для обох форматів файлів |\n"
		                 << L"+_________________________+_____________________________________________________________________________+\n\n"
						 << L"                      На даний момент KINOSTUDIA підтримує такий перелік вбудованих полів:                  \n"
						 << L"+__________[Країни]__________+_____________[Жанри]_____________+__________________[Вікові оцінки]___________________+\n"
                         << L"                            |                                  |                                                    |\n"
                         << L"  * Канада                  |   * Бойовик        * Містика     |   * PG - рекомендований батьківський контроль.     |\n"
                         << L"  * Німеччина               |   * Пригоди        * Романтика   |   * PG-13 - батьківський контроль до 13 років.     |\n"
                         << L"  * Японія                  |   * Анімація       * Фантастика  |   * NR - не оцінено.                               |\n"
                         << L"  * Індія                   |   * Документальні  * Трилер      |   * PASS - допущено на розгляд                     |\n"
                         << L"  * Фінлянія                |   * Комедії        * Війна       |   * APPROVED - затверджено на показ                |\n"
                         << L"  * США                     |   * Кримінальні    * Західні     |   * R - Обмеження до 18-ти років.                  |\n"
                         << L"  * Італія                  |   * Драма          * ... (to do) |   * G - General, усі вікові категорії.             |\n"
                         << L"  * Китай                   |   * Сімейні                      |   * M - Mature, зріла вікова категорія.            |\n"
                         << L"  * Польща                  |   * Фентезі                      |   * X - Строго зріла вікова категорія.             |\n"
                         << L"  * Австралія               |   * Нуар                         |   * TV-14 - вміст для глядачів 14+                 |\n"
                         << L"  * Сполучене Королівство   |   * Історичні                    |   * ... (to do)                                    |\n"
                         << L"  * Франція                 |   * Жахи                         |                                                    |\n"
				         << L"  * Мексика                 |   * Музика                       |                                                    |\n"
				         << L"  * Швеція                  |   * Мюзикл                       |                                                    |\n"
						 << L"  * ... (to do)                                                                                                     |\n\n"
                         << L"                                               [Приклади форматування]\n"
						 << L"____________________________________________________________________________________________________________________\n"
                         << LIGHT_GREEN << L"\"Погір'я долі\",\"Морія Сувако\",\"Японія/США\",2004,\"M\",\"Пригоди\",127,10,10,666,666\n" 
                         << LIGHT_GREEN <<  L" - хороша практика.\n"                                                                   
                         << YELLOW << L"\"Морія Сувако\", \"Погір'я долі\",\"Японія/США\",2004, \"M\",\"Пригоди\",127,10,10,666,666\n" 
                         << YELLOW << L" - послідовність порушена, забагато пунктуаційних знаків. Кома та пробіл.\n" 
                         << LIGHT_RED << L"Морія Сувако, Погір'я долі, ЯпоніяСША, ,2004, \"M\",\"Пригоди\",127,10,10,666,666\n" 
                         << LIGHT_RED << L" - погана практика.\n" << LIGHT_CYAN
						 << L"____________________________________________________________________________________________________________________\n\n"
				         << L"                                              [Резервне копіювання]\n"
				         << L"  При запуску програми, за умови, що ваша схема містить хоча б один запис, на протязі 5-ти хвилин (по замовчуванню)\n"
				         << L"  буде проводитися резервне копіювання поточної схеми з відповідним повідомленням про день та дату.\n"
					     << L"  Ви можете змінити значення таймера до наступного резервного копіювання у інтервалі від 5 до 60 хвилин,\n"
						 << L"  проте майте на увазі, що це поки що єдиний механізм автоматичного збереження вашого прогресу.\n"
						 << L"  Усі резервні копії зберігаються у папці backup/movies, що у корінному шляху програми.\n\n"
						 << L"  Відстежувати конкретну резервну копію просто, адже структура файлу наступна: \n"
						 << L"                          repetovsky-mv-20241029-104005\n"
						 << L"                          /         /      /       /\n"
					     << L"                         /         /      /       /\n"
						 << L"                        Назва  Мітка   Повна   Час\n"  
					     << L"                        Схеми          Дата    Год. хв. сек.\n"
						 << L"____________________________________________________________________________________________________________________\n"
						 << WHITE;
}
void Menu::execute() {
    rlutil::hidecursor();
    bool fill_menu = true;

    while (true) {
        rlutil::locate(1 , 1);
        printm();

        switch (_getch())
        {
        case ARROW_UP:
            if (_parent_iterator_position == _parent_process_vector.begin())
                _parent_iterator_position = _parent_process_vector.end() - 1;
            else
                --_parent_iterator_position;
            break;
        case ARROW_DOWN:
            if (_parent_iterator_position == _parent_process_vector.end() - 1)
                _parent_iterator_position = _parent_process_vector.begin();
            else
                ++_parent_iterator_position;
            break;
        case ENTER:
            rlutil::cls();
            if (_parent_iterator_position == _parent_process_vector.end() - 1)
                return;
            _parent_iterator_position->second();
            break;
        case TAB:
            fill_menu = !fill_menu;
            FILL_WITH = fill_menu ? L' ' : L'\0';
            rlutil::cls();
            break;
        case F1: help(); break;
        case F2: rlutil::cls(); break;
        default: continue;
        }
    }
}
bool Menu::try_exit() {
    return is_confirmed(L" * Ви впевнені, що хочете завершити роботу ? \n * Не збережені зміни по класиці жанру будуть" + LIGHT_RED + L" ВТРАЧЕНІ\033[0m.");
}
bool Menu::is_confirmed(const wstring& prompt) {

    OHandle::CursorVisibilityGuard cursor(true);
    OHandle::ClearScreenGuard repetovsky_cls_guard;

    wstring decision;

    do {
        LogHandle::warn(L"\n" + prompt + L" (ТАК/НІ)");

        decision = IHandle::wrap_input_string();

        wstring original_decision = decision;
        transform(decision.begin(), decision.end(), decision.begin(), towlower);

        if (decision == L"ні" || decision == L"ys") {
            rlutil::cls();
            return false;
        }

        if (decision != L"так" && decision != L"nfr") {
            wcerr << LIGHT_RED
                << L"<<?!>> Не можу розпізнати команду '" << original_decision << L"' <<?!>>" << endl
                << L"<<!>> Вхідні дані НЕ відповідають запропонованому шаблону."
                << WHITE
                << L"\nСпробуйте все-таки щось між '"
                << LIGHT_RED
                << L"ТАК"
                << WHITE
                << L"' або '"
                << LIGHT_GREEN
                << L"НІ"
                << WHITE
                << "'" << endl;
        }

    } while (decision != L"так" && decision != L"nfr");
    return true;
}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

SubMenu::SubMenu() { this->_heading = L"ПІД-МЕНЮ"; }

SubMenu::~SubMenu() { clear_cache(); }

void SubMenu::clear_cache() { _child_process_vector.clear(); }

bool SubMenu::is_confirmed(const wstring& prompt) { return Menu::is_confirmed(prompt); }

void SubMenu::emplace_process(const process& child_process_module) {
    _child_process_vector.emplace_back(child_process_module.first, child_process_module.second);
    _child_iterator_position = _child_process_vector.begin();
}

bool SubMenu::remove_process(const wstring& process_heading) {

    for (auto it = _child_process_vector.begin(); it != _child_process_vector.end(); ++it) {
        if (it->first == process_heading) {
            _child_process_vector.erase(it);
            _child_iterator_position = _child_process_vector.begin();
            return true;
        }
    }

    LogHandle::error(L"<<!?>> Процес з таким іменуванням відсутній. <<?!>> ");
    return false;
}

void SubMenu::help() { Menu::help(); }

void SubMenu::printm() {

	if (_max_str_len < 64 || _max_str_len > LONG_MAX) {
		wcerr << LIGHT_RED
		      << L"<<!>> Помилка при спробі відмалювати меню. \nБудуть застосовані рекомендовані зміни. <<!>>"
		      << WHITE << L"\nОчікуйте...";
		_max_str_len = DEFAULT_STRING_LENGTH;
		this_thread::sleep_for(4s);
		rlutil::cls();
	}

	wcout << wstring(_max_str_len + 2, '=') << endl << "|";
	printf(_heading,  UNDERLINE +LIGHT_RED, CLEAR_UNDERLINE + CLEAR_COLOR);
	wcout << "|" << endl;

	wcout << "|";
	printf(L"[?] Натисніть F1 для користувацької довідки. [?]", LIGHT_CYAN);
	wcout << "|" << endl;

	wcout << "|";
	printf(L"<!> Натисність F2 щоб очистити дисплей. <!>", WHITE);
	wcout << L"|" << endl << wstring(_max_str_len + 2, L'=') << endl;

    wcout << "|";
    printf(L"[<] Натисність ESC щоб повернутися в попереднє меню. [<]", LIGHT_CYAN);
    wcout << L"|" << endl << wstring(_max_str_len + 2, L'=') << endl;

	for (auto it = _child_process_vector.begin(); it != _child_process_vector.end(); ++it) 
    {
		wcout << (it != _child_iterator_position ? "|" : "<");
		printf(it->first, it == _child_iterator_position ? LIGHT_RED : YELLOW);
		wcout << (it != _child_iterator_position ? "|" : ">") << endl;
	}
	wcout << wstring(_max_str_len + 2, '=') << endl;

}

void SubMenu::execute() {
	rlutil::hidecursor();
    bool stop { false };

	while (true)  {
		rlutil::locate(1, 1);
		printm();

		switch (_getch())
		{
		case ARROW_UP:
			if (_child_iterator_position == _child_process_vector.begin())
				_child_iterator_position = _child_process_vector.end();
			--_child_iterator_position;
			break;
		case ARROW_DOWN:
			if (_child_iterator_position == _child_process_vector.end() - 1)
				_child_iterator_position = _child_process_vector.begin();
			else ++_child_iterator_position;
			break;
		case ENTER:
			rlutil::cls();
			_child_iterator_position->second();
			break;
		case ESCAPE: stop = true;   break;
		case F1:     help();        break;
		case F2:     rlutil::cls(); break;

		default: 
            continue;
		}
        if (stop) break;
	}
}

bool SubMenu::try_exit()
{
    OHandle::ClearScreenGuard repetovsky_cls;
    wcout << L"Повторно натисніть ["
	      << LIGHT_CYAN
	      << "ESC"
	      << WHITE
	      << L"], щоб підтвердити вихід в попереднє меню. " << endl
          << L"Прибрати повідомлення, продовжити роботу в цьому меню - [" << LIGHT_CYAN << "F2" << WHITE <<  L"]"  << endl;

	if (_getch() == ESCAPE) return true;
	
    return false;
}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

