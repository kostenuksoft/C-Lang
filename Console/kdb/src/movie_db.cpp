#include "../src/Framework.hpp"
#include "../src/Utils.hpp"
#include "../src/Menu.hpp"
#include "../src/MovieDatabase.hpp"
#include "../src/Criteria.hpp"

using namespace Mv;
using namespace MvException;
using namespace MvDataBase;
using namespace MvUtils;

using namespace Console;

//*********************************[Конструктори|Деструктор]*********************************
MovieDatabase::MovieDatabase(const MovieDatabase& orig)  {
    movie_list = orig.movie_list;
}
MovieDatabase& MovieDatabase::operator=(const MovieDatabase& orig) {
    if (this == &orig) return *this;
    this->movie_list = orig.movie_list;
    return *this;
}
MovieDatabase::MovieDatabase(MovieDatabase&& orig) noexcept : movie_list(std::move(orig.movie_list)) { orig.movie_list.clear(); }
MovieDatabase& MovieDatabase::operator=(MovieDatabase&& orig) noexcept {
    if (this == &orig) return *this;
    this->movie_list = orig.movie_list;
    orig.movie_list.clear();
    return *this;
}
MovieDatabase::~MovieDatabase() {
    movie_list.clear();
    LogHandle::success(L"\n[\\/] Список кінотайтлів успішно очищений! [\\/]");
    this_thread::sleep_for(2s);
}
//*******************************************************************************************

//*************************************[Допоміжні методи]*************************************/
void MovieDatabase::throw_empty_db_error(const char* method_name) {
    wcerr << LIGHT_RED << L"<!> Кінобаза порожня. <!> \n"
          << LIGHT_RED << L"[?] Спробуйте опцію 'Імпортувати' у головному меню. [?]" << WHITE << endl;
    this_thread::sleep_for(2s);
    rlutil::cls();
    throw MovieException("Empty movie DB. Thrown in '" + string(method_name) + "' method.",  Priority::Warning, NULL);
}
wostream& MovieDatabase::serializate(wofstream& wofs) const {
    for (const auto& movie : movie_list) { Movie::to_wostream(wofs, movie); }
    return wofs;
}
wostream& MovieDatabase::serializate_one(wofstream& wofs, const Movie& movie) { return Movie::to_wostream(wofs, movie); }
//********************************************************************************************/

//*************************************[Ініціалізація колекції]*************************************/
bool MovieDatabase::try_import(MovieDatabase& target_db) {
    wifstream db_txt("../build/import/movies.txt", ios_base::in);
    if (db_txt.is_open()) { db_txt >> target_db; return true; }
    LogHandle::error(L"<!> Неможливо відчинити movies.txt, що у import/movies.txt. <!>");

    wifstream db_csv("../build/import/movies.csv", ios_base::in);
    if (db_csv.is_open()) { db_csv >> target_db; return true; }
    LogHandle::error(L"<!> Неможливо відчинити movies.csv, що у import/movies.csv. <!>");

    throw MovieException("Can`t reach movies.txt nor movies.csv. Movies import failed.", Priority::FatalError, 168);
}
bool MovieDatabase::init(const Movie& movie) {
    auto it = lower_bound(movie_list.begin(), movie_list.end(), movie, comparator);
    if (it == movie_list.end() || movie < *it) {
        movie_list.insert(it, movie);
        return true;
    }

    wcerr << LIGHT_RED << quoted(movie.get_title()) << L" наявний у базі." << WHITE << endl;
    this_thread::sleep_for(25ms);
    return false;
}
bool MovieDatabase::try_export(const MovieDatabase& target_db) {
    OHandle::CursorVisibilityGuard cursor(true);

    if (target_db.empty())
        throw_empty_db_error("try_export");

    Menu m;
    wofstream db_txt,
        db_csv;

    bool db_txt_exists = filesystem::exists("../build/export/movies.txt"),
         db_csv_exists = filesystem::exists("../build/export/movies.csv");

    if (!db_txt_exists) db_txt.open("../build/export/movies.txt", ios_base::out);
    else db_txt.open("../build/export/movies.txt", ios_base::out | ios_base::_Nocreate);

    if (!db_csv_exists) db_csv.open("../build/export/movies.csv", ios_base::out);
    else db_csv.open("../build/export/movies.csv", ios_base::out | ios_base::_Nocreate);

    if (!db_txt.is_open() && !db_csv.is_open()) {
        LogHandle::error(L"<!> Неможливо відчинити жоден з файлів для експортування. <!>");
        throw MovieException("Couldn`t create movies.txt nor movies.csv. Movies export failed.", Priority::FatalError, 205);
    }

    if (db_txt.is_open() && filesystem::is_empty("../build/export/movies.txt")) {
        if (db_txt << target_db)
            LogHandle::success(L"[\\/] Успіх при експортуванні у txt. [\\/]\n");
    }
    else if (db_txt.fail()) {
        LogHandle::error(L"<!> Неможливо відчинити txt файл для експортування. <!>");
        this_thread::sleep_for(1.5s);
    }
    else {
        db_txt.close();
        LogHandle::warn(L"[!] Вміст файлу movies.txt з попередніх сесій буде перезаписано. [!] ");

    	if (m.is_confirmed(L"Бажаєте продовжити ? (НІ - перехід до експорту в csv)")) {
            db_txt.open("../build/export/movies.txt", ios_base::out | ios_base::trunc);
            if (db_txt << target_db)
                LogHandle::success(L"[\\/] Успіх при експортуванні у txt. [\\/]\n");
            else LogHandle::error(L"<<?!>> При експортуванні у movies.txt виникла помилка. <<?!>>");
            db_txt.close();
        }
    }

    if (db_csv.is_open() && filesystem::is_empty("../build/export/movies.csv")) {
        if (db_csv << target_db)
            LogHandle::success(L"[!] Успіх при експортуванні у csv. [!]\n");
    }
    else if (db_csv.fail()) {
        LogHandle::error(L"<!> Неможливо відчинити csv файл для експортування. <!>");
        this_thread::sleep_for(1.5s);
    }
    else {
        db_csv.close();
        LogHandle::warn(L"[!] Вміст файлу movies.csv з попередніх сесій буде перезаписано. [!] ");
        if (!m.is_confirmed(L"Бажаєте продовжити ?")) return false;
        db_csv.open("../build/export/movies.txt", ios_base::out | ios_base::trunc);
        if (db_csv << target_db)
            LogHandle::success(L"[!] Успіх при експортуванні у csv. [!]\n");
        else LogHandle::error(L"<<?!>> При експортуванні у movies.csv виникла помилка. <<?!>>");
        db_csv.close();
    }
    IHandle::anykey(L"Натисність будь-яку клавішу...\n");
    return true;
}
//*************************************************************************************************/

//*************************************[Видалення з колекції]*************************************
void MovieDatabase::remove(const Movie& movie, const Mode mode, const size_t index) {
    if (movie_list.empty()) 
    	throw_empty_db_error("remove");
    

    try {
        switch (mode) {
        case Mode::MODE_ALL: {
            movie_list.clear();
            LogHandle::success(L"[\\/] Успішно видалено ввесь вміст схеми. [\\/]");
            return;
        }

        case Mode::MODE_CRITERIA: {
            auto [first, last] = std::equal_range(
                movie_list.begin(),
                movie_list.end(),
                movie,
                comparator
            );

            if (first == last) {
                LogHandle::error(L"<!> За такою критерію в базі записи відсутні. <!>");
                return;
            }

            const wstring deleted_title = first->get_title();
            auto m = movie_list.erase(first, last);

			if(m != movie_list.end())
				LogHandle::success(L"[\\/] Успішно видалено \"" + deleted_title + L"\" [\\/]");

            else LogHandle::error(L"<!> Не вдалось провести видалення за обраною критерією <!>");
            return;
        }

        case Mode::MODE_INDEX: {
            if (index >= movie_list.size()) {
                throw MovieException(
                    "Index out of range: " + std::to_string(index) +
                    " (size: " + to_string(movie_list.size()) + ")",
                    Priority::FatalError,
                    66
                );
            }

            const auto it = movie_list.begin() + static_cast<ptrdiff_t>(index);
            const wstring deleted_title = it->get_title();
            auto m = movie_list.erase(it);
            return;
        }

        default:
            throw MovieException("Invalid deletion mode", Priority::Info, 74);
        }
    }
    catch (const std::exception& e) {
        LogHandle::error(L"<!> Помилка під час видалення: " +
            wstring(e.what(), e.what() + strlen(e.what())) +
            L" <!>");
        throw; 
    }
}
void MovieDatabase::clear(MovieDatabase& target_db, const Mode mode, const size_t index) const {
    if (!movie_list.empty()) {
        target_db.remove({}, mode, index);
        return;
    }
    throw_empty_db_error("clear");
}
//************************************************************************************************

//*************************************[Пошук по колеції]*************************************
Criteria::Search MovieDatabase::one_of(const function<bool(const Movie& m)>& criteria) const {
	if (!movie_list.empty()) {
        auto mv_it = find_if(movie_list.begin(), movie_list.end(), criteria);
        if (mv_it != movie_list.end()) 
            return Criteria::Search(*mv_it);
        LogHandle::error(L"<!> За такою критерією в базі записи відсутні! <!>");
        return {};
	}
    throw_empty_db_error("one_of");
    return {};
}
vector<Movie> MovieDatabase::all_of(const function<bool(const Movie& m)>& criteria) const
{
    if (movie_list.empty()) {
        throw_empty_db_error("all_of");
        this_thread::sleep_for(1s);
        throw MovieException("User tried find by title. DB is empty though.",  Priority::Warning, 90);
    }

    vector<Movie> results;
    std::copy_if(movie_list.begin(), movie_list.end(), back_inserter(results),
        [&criteria](const Movie& m) { return criteria(m); });


    if (results.empty()) 
        LogHandle::error(L"<!> За такою критерією в базі записи відсутні. <!>");

  /* vector<Movie> results;
   for_each(movie_list.begin(), movie_list.end(), [&](const Movie& m) {
        if (criteria(m)) results.push_back(m);
   });*/

   return results;
}
//********************************************************************************************

//*************************************[Копіювання без залучення спец. функцій]*************************************
void MovieDatabase::copy_to(const shared_ptr<MovieDatabase>& target_db) const {
    if (this->movie_list.empty()) throw_empty_db_error("copy_to");
	for_each(movie_list.begin(), movie_list.end(), [&](const Movie& m) {
        if (target_db->init(m)) 
            LogHandle::success(L"[+] Успішно скопійовано кінотайтл \"" + m.get_title() + L"\" [+]");
	});
}
void MovieDatabase::copy_one(const MovieDatabase& from_db, MovieDatabase& to_db, const size_t index) {
    if (from_db.empty()) throw_empty_db_error("copy_one");
	if (to_db.init(from_db.at(index)))
        LogHandle::success(L"[+] Успішно скопійовано. [+]");
}
//******************************************************************************************************************








