#pragma once
#include "../src/Menu.hpp"
#include "../src/MovieException.hpp"
#include "../src/Movie.hpp"
#include "../src/Utils.hpp"
#include "../src/Criteria.hpp"

namespace MvDataBase
{
    using namespace Mv;
    using namespace MvCriteria;
    using namespace MvException;
    using namespace MvUtils;

    enum class Mode {
        MODE_ALL = 0x0, MODE_CRITERIA = 0x1, MODE_INDEX = 0x2,
    };

    /* Клас, що репрезентує базу кінотайтлів */
    class MovieDatabase {
    public:

        MovieDatabase() = default;
        ~MovieDatabase();

        MovieDatabase(const MovieDatabase& orig);
        MovieDatabase& operator=(const MovieDatabase& orig);
        MovieDatabase(MovieDatabase&& orig) noexcept;
        MovieDatabase& operator=(MovieDatabase&& orig) noexcept;

        static bool try_import(MovieDatabase& target_db);
        static bool try_export(const MovieDatabase& target_db);

    	[[nodiscard]] auto begin() const  { return movie_list.begin(); }
        [[nodiscard]] auto begin()  { return movie_list.begin(); }
        [[nodiscard]] auto end() const { return movie_list.end(); }
        [[nodiscard]] auto end()  { return movie_list.end(); }

    	[[nodiscard]] const auto& get() const { return movie_list; }
        [[nodiscard]] size_t size() const { return movie_list.size(); }
        [[nodiscard]] bool empty() const { return movie_list.empty(); }
        [[nodiscard]] const Movie& at(const size_t index) const { return movie_list.at(index); }
        [[nodiscard]] Movie& at(const size_t index) { return movie_list.at(index); }
        [[nodiscard]] const Movie& operator[](const size_t index) const noexcept { return movie_list[index]; }

        bool init(const Movie& movie);
        void remove
    	(
            const Movie& movie, 
            Mode mode, 
            size_t index
        );

        Criteria::Search one_of(const function<bool(const Movie&)>& criteria) const;

        template <typename Compare>
        Movie one_of(Compare&& comparison);

    	vector<Movie> all_of(const function<bool(const Movie&)>& criteria) const;

        wostream& serializate(wofstream& wofs) const;
        static wostream& serializate_one(wofstream& wofs, const Movie& movie);

        /*[[nodiscard]] Criteria::Search by_title(const wstring& title, bool sensitive = false) const {
            return one_of(Criteria().title(title, sensitive).build());
        }

        [[nodiscard]] vector<Movie> by_director(const wstring& director, bool sensitive = false) const {
            return all_of(Criteria().director(director, sensitive).build());
        }

        [[nodiscard]] vector<Movie> by_countries(const wstring& genre, bool sensitive = false) const {
            return all_of(Criteria().countries(genre, sensitive).build());
        }

        [[nodiscard]] vector<Movie> by_year(const size_t start_year, const size_t end_year) const {
            return all_of(Criteria().year_range(start_year, end_year).build());
        }

        [[nodiscard]] vector<Movie> by_ager(const wstring& age_rating) const {
            return all_of(Criteria().age_rating(age_rating).build());
        }

        [[nodiscard]] vector<Movie> by_genre(const wstring& genre, bool sensitive = false) const {
            return all_of(Criteria().genre(genre, sensitive).build());
        }

        [[nodiscard]] vector<Movie> by_duration_above(const size_t duration) const {
            return all_of(Criteria().duration_above(duration).build());
        }

        [[nodiscard]] vector<Movie> by_duration_below(const size_t duration) const {
            return all_of(not1(Criteria().duration_above(duration).build()));
        }

        [[nodiscard]] vector<Movie> by_avg_rating_above(const float avg_rating) const {
            return all_of(Criteria().avg_rating_above(avg_rating).build());
        }

        [[nodiscard]] vector<Movie> by_avg_rating_below(const float avg_rating) const {
            return all_of(not1(Criteria().avg_rating_above(avg_rating).build()));
        }

        [[nodiscard]] vector<Movie> by_no_of_ratings_above(const size_t avg_rating) const {
            return all_of(Criteria().no_of_ratings_above(avg_rating).build());
        }

        [[nodiscard]] vector<Movie> by_no_of_ratings_below(const size_t avg_rating) const {
            return all_of(not1(Criteria().no_of_ratings_above(avg_rating).build()));
        }*/

        template <typename Compare>
        void sort(Compare&& comparison);

        void clear
    	(
            MovieDatabase& target_db,
            Mode mode, 
            size_t index
        ) const;


    	template <typename Criteria>
        void clear_if
    	(
            MovieDatabase& target_db, 
            Criteria&& criteria
        ) const;

        void copy_to(const shared_ptr<MovieDatabase>& target_db) const;
        static void copy_one
    	(
            const MovieDatabase& from_db,
            MovieDatabase& to_db, 
            size_t index
        );

    	template <typename Criteria>
        shared_ptr<MovieDatabase> copy_if
    	(
            shared_ptr<MovieDatabase>& target_db, 
            Criteria&& criteria
        ) const;

        template<typename Collection>
        friend void create(Collection& collection);

        template<typename Collection>

    	friend int display
    	(
            const Collection& collection, 
            bool use_as_external_viewer, const wstring& message
        );

        template<typename Collection>
        friend void find_by
    	(
            const function<bool(const Movie& m)>& criteria,
            bool limit, 
            const Collection& collection
        );

        template<typename Collection, typename Comparator>
        friend void sort
    	(
            Collection& collection, 
            Comparator comparator
        );

        template<typename Collection>
        friend void update(Collection& collection);

        template<typename Collection>
        friend void extract
    	(
            const Collection& from, 
            Collection& to
        );

        template<typename Criteria, typename Collection>
        friend void delete_by
        (
            const Criteria& criteria,
            Collection& collection
        );

        template<typename Collection>
        friend void delete_at_index(Collection& collection);

        template<typename Collection>
        friend void delete_all_content(Collection& collection);

        friend wostream&  operator<<(wostream& os, const MovieDatabase& mdb);
        friend wofstream& operator<<(wofstream& wofs, const MovieDatabase& mdb);
        friend wofstream& operator<<(wofstream& wofs, const shared_ptr<MovieDatabase>& mdb);
        friend wifstream& operator>>(wifstream& wifs, MovieDatabase& mdb);

    private:

        static void throw_empty_db_error(const char* method_name);

        /*
         * За сховище кінотайтлів був обраний вектор через суперечливе сусіднє розміщення елементів у пам'яті.
         * Порівнювався з асоціативним контейнером set, у плані продуктивності відмінностей не було помічено.
         * Однак коли кінобаза почне різко зростати, то переважатиме у вставці та видаленні set.
         * Часова комлексія вставки та видалення - log(n).
         * З невеликими зусиллями в майбутньому клас можна адаптувати під set.
         */
        vector<Movie> movie_list;

        /*
         * Порівняльник, по якому в базу заносяться дані.
         * Необхідність у ньому виникла при використанні алгоритмів з предикатами, наприклад lower_bound
        */
        function<bool(Movie, Movie)> comparator = [](const Movie& m, const Movie& _m) { return m < _m; };
    };

    template <typename Compare>
    void MovieDatabase::sort(Compare&& comparison) {
	    if (movie_list.empty()) throw_empty_db_error("sort");
	    stable_sort(movie_list.begin(), movie_list.end(), std::forward<Compare>(comparison));
	    comparator = std::forward<Compare>(comparison);
    }

    template <typename Criteria>
    shared_ptr<MovieDatabase> MovieDatabase::copy_if
	(
        shared_ptr<MovieDatabase>& target_db, 
        Criteria&& criteria
    ) const
    {
        OHandle::ClearScreenGuard repetovsky_guard;
        if (movie_list.empty()) 
        	throw_empty_db_error("copy_if");

        struct Stats {
            size_t movies_copied = 0;
            size_t movies_skipped = 0;
            size_t movies_failed = 0;

            void log() {
                LogHandle::info(L"_______________________________________________________________________");
                LogHandle::log(L"[log] Успішно експортовано: " + to_wstring(movies_copied));
                LogHandle::log(L"[log] Пропущено: " + to_wstring(movies_skipped));
                LogHandle::log(L"[log] Не вдалось експортувати: " + to_wstring(movies_failed));
                LogHandle::info(L"_______________________________________________________________________");
            }

        } stats;

    	auto&& universal_ref = std::forward<Criteria>(criteria);

    	for (const auto& movie : movie_list) {

    		if (!universal_ref(movie)) {
    			++stats.movies_skipped;
    			continue;
    		}

    		if (target_db->init(movie)) {
    			LogHandle::success (L"[+] Успішно скопійовано кінотайтл \"" + movie.get_title() + L"\" [+]");
                   ++stats.movies_copied;
    		}
    		else {
    			LogHandle::error(L"<!> Не вдалось експортувати \"" + movie.get_title() + L"\" <!>");
                   ++stats.movies_failed;
    		}
    	}

        stats.log();
        if (stats.movies_copied == 0) LogHandle::warn(L"[!] Жоден запис не було скопійовано [!]");

    	IHandle::anykey(L"[...] Натисність будь-яку клавішу, щоб повернутись в меню... [...]");

        return target_db;
    }

    template <typename Criteria>
    void MovieDatabase::clear_if
	(
        MovieDatabase& target_db,
        Criteria&& criteria
    ) const {
	    if (movie_list.empty()) 
		    throw_empty_db_error("clear");
	    short count = 0;
	    for_each(movie_list.begin(), movie_list.end(), [&](const Movie& movie){
		    if (criteria(movie)) {
			    target_db.remove(movie, Mode::MODE_CRITERIA, NULL);
			    ++count;
		    }
	    });
        if (count != 0)
            LogHandle::log(L"К-сть видалених кінотайтлів -> " + to_wstring(count));
        else LogHandle::warn(L"[!] За цією критерією нічого не знайдено. [!]");
    }

    template <typename Compare>
    Movie MovieDatabase::one_of(Compare&& comparison) {
        if (movie_list.empty()) throw_empty_db_error("one_of");

        auto mv_it = find_if(movie_list.begin(), movie_list.end(), std::forward<Compare>(comparison));
        if (mv_it != movie_list.end()) return *mv_it;
    	LogHandle::warn(L"[!] За цією критерією нічого не знайдено. [!]");
        throw MovieException("No movie found matching the comparison",  Priority::Warning, 482);
    }

    template<typename Collection>
	void create(Collection& collection)
    {
        OHandle::CursorVisibilityGuard repetovsky_cursor_guard(true);
        OHandle::ClearScreenGuard repetosky_cls_guard;

        wstringstream wss, _wss;
        wstring iwstr, owstr;
        bool succeeded{ false };
        Console::Menu temp;

        while (true) {
            wcout << LIGHT_CYAN << L" [Шаблон]: "
                  << quoted(L"Леон") << L","
                  << quoted(L"Люк Бессон") << L","
                  << quoted(L"Франція") << L","
                  << L"1994" << L","
                  << quoted(L"R") << L","
                  << quoted(L"Бойовик/Драма") << L","
                  << L"110" << L","
                  << L"7.8" << L","
                  << L"162752" << L","
                  << L"16000000" << L","
                  << L"42000000\n";

            LogHandle::warn(L"  Щоб розділити вміст ',' використовуйте одинарні знаки пунктуації, наприклад [ , . : / | ]");
            LogHandle::success(L" [Рекомендація]: Почитайте довідку. ");
            LogHandle::info(L" [Набір доступних команд]: exit, cls, help {");
            LogHandle::info(L"   * exit - очевидно вихід");
            LogHandle::info(L"   * cls - очистити екран");
            LogHandle::info(L"   * help - вивести довідку");

            iwstr = IHandle::wrap_input_string();
            wss.str(iwstr);
            IHandle::to_wlower(iwstr);

            if (iwstr == L"exit") { return; }
            if (iwstr == L"cls")  { repetosky_cls_guard.~ClearScreenGuard(); continue; }
            if (iwstr == L"help") { repetosky_cls_guard.~ClearScreenGuard(); temp.help(); continue; }

            while (getline(wss, owstr)) {
                _wss.str(owstr);
                Movie m;
                _wss >> m;
                if (!wss.fail() && m.valid()) {
                    if (collection.init(m)) {
                        LogHandle::success(L"Успіх. Попередній перегляд: ");
                        wcout << m;
                        succeeded = true;
                    }
                    break;
                }
                    LogHandle::error(L"<<!>> Не вдалось cтворити кінотайтл. <<!>>");
                    LogHandle::warn(L"[?] Ймовірно, введення не по шаблону. [?]");
                    LogHandle::warn(L"[!] Порівняйте ваш ввід з запропоновним шаблоном [!]");
                    break;
            }
            _wss.clear();
            wss.clear();

            if (succeeded)
                LogHandle::info(L"Натисність будь-яку клавішу, щоб продовжити ввід.");
            else
            	LogHandle::info(L"Натисність [ESC], щоб повернутись назад. Якщо бажаєте продовжити - будь-яку іншу клавішу.");

            switch(_getch()) case Console::ARROW_ALT: if( _getch() == Console::ESCAPE) return;
            
				
            repetosky_cls_guard.~ClearScreenGuard();
        }
    }

	template<typename Collection>
	int display
	(
        const Collection& collection,
        const bool use_as_external_viewer,
        const wstring& message
    )
    {
        if (collection.empty()) MovieDatabase::throw_empty_db_error("display");
        int index = 0, try_interact;

    	auto print_page_info = [&] {
            rlutil::locate(36, 15);
            LogHandle::info(YELLOW + L"[Сторінка -> " + to_wstring(index + 1) + L" з " + to_wstring(collection.size()) + L"]");
        };

        auto print_message = [&] {
            if (use_as_external_viewer) {
                rlutil::locate(25, 16);
                LogHandle::info(message + YELLOW + L"[" + to_wstring(index + 1) + L"]");
            }
        };

        auto navigate_pages = [&](const int direction) {
            index += direction;
            if (index < 0) index = static_cast<int>(collection.size()) - 1;
            if (index >= static_cast<int>(collection.size())) index = 0;
            rlutil::cls();
            };

        do {
            print_page_info();
            print_message();

            rlutil::locate(1, 1);
            wcout << collection.at(index);

            try_interact = _getch();

            if (try_interact == Console::ARROW_RIGHT)
                navigate_pages(1);
            else if (try_interact == Console::ARROW_LEFT)
                navigate_pages(-1);
            else if (use_as_external_viewer && try_interact == Console::ENTER)
                return index;

        } while (try_interact != Console::ESCAPE);

        rlutil::cls();
        return -1;
    }

    template<typename Collection>
	void update(Collection& collection) {

        OHandle::ClearScreenGuard repetovsky_cls_guard;
        for (;;) {

            const int index = display
            (
                collection,
                true,
                L"Натисність [ЕНТЕР], щоб " + LIGHT_CYAN + L"РЕДАГУВАТИ запис "
            );
            repetovsky_cls_guard.~ClearScreenGuard();
            if (index == -1) return;
            LogHandle::info(L" * Підтвердження редагування екземпляру кінотайтлу - [ЕНТЕР]\n * Вихід - [ESC]");
            for (;;) {
                const int decision = _getch();
                if (decision == Console::ENTER) {

                    FHandle::FileGuard my_temp(L".\\temp\\temp.txt");
                    OHandle::CursorVisibilityGuard repetovsky_cursor_guard(true);

                    wofstream wofs("temp\\temp.txt", ios::out);
                    auto _ptr = &collection.at(index);
                    collection.serializate_one(wofs, collection.at(index));
                    wofs.close();

                    const filesystem::path temp = ".\\codeit\\codeit.exe temp/temp.txt";
                    auto command = temp.lexically_normal();
                    if (temp.is_relative()) command = absolute(command);

                    if (_wsystem(command.c_str()) != 0) {
                        my_temp._remove_temp();
                        LogHandle::error(L"<<?!>> Помилка при спробі використати редактор 'codeit'. <<?!>>");
                        IHandle::anykey(L"[Abcd] Натисніть будь-яку клавішу... [Abcd]");
                        return;
                    }
                    rlutil::cls();

                    SetConsoleTitleA(APP_TITLE);
                    wifstream wifs("temp\\temp.txt", ios::in);

                    Movie temp_obj;
                    wifs >> temp_obj;
                    wifs.close();

                    if (temp_obj.valid())  *_ptr = std::move(temp_obj);
                    else throw MovieException("Movie object initialization failed during editing.", Priority::FatalError);
                    my_temp._remove_temp();
                	
                    break;
                }
                if (decision == Console::ESCAPE) return;
            }
        }
    }

    template<typename Collection>
	void extract
	(
        const Collection& from, 
			  Collection& to
    )
    {
        for (;;)
        {
            const int index = display
            (
                from,
                true,
                L"Натисність [ЕНТЕР], щоб " + LIGHT_GREEN + L"ЕКСПОРТУВАТИ" += LIGHT_CYAN + L" запис "
            );

            rlutil::cls();

            if (index == -1)
                wcout << LIGHT_CYAN
                << L"Вихід - [ESC]" << endl
                << L"Продовжити - будь-яку іншу клавішу" << endl
                << WHITE;
            else
                wcout << LIGHT_CYAN
                      << L"Підтвердження експорту екземпляру кінотайтлу : [ЕНТЕР]" << endl
                      << L"Вихід - [ESC]" << endl;
              
            for (;;) {
                const int decision = _getch();
                if (decision == Console::ENTER) {
                    Collection::copy_one(from, to, index);
                    IHandle::anykey(L"[...] Натиисніть будь-яку клавішу.. [...]");
                    break;
                }
                if (decision == Console::ESCAPE) return;
                break;
            }
        }
    }

    template<typename Collection>
	void find_by(
        const function<bool(const Movie& m)>& criteria, 
        const bool limit, 
        const Collection& collection
    ) {
        OHandle::ClearScreenGuard repetovsky_cls;
        if (limit) {
            auto result = collection.one_of(criteria);
            if (result.get_movie().valid())
                wcout << result.get_movie() << endl;
        }
        else {
            const auto result_many = collection.all_of(criteria);
            for (auto& m : result_many) wcout << m;
        }

        this_thread::sleep_for(1s);
        LogHandle::info(L"Натисність [ESC], щоб повернутися назад...");
        while (_getch() != Console::ESCAPE) {}
    }

    template<typename Collection, typename Comparator>
    void sort
	(
        Collection& collection, 
        Comparator comparator
    )
	{

       enum class SortOrder { Ascending, Descending };
       const wchar_t* instructions = LR"(
	   [?] Для сортування за зростанням, натисніть стрілку вгору. [?]
	   [?] Для сортування за спаданням, натисніть стрілку вниз. [?]
	   [?] Для виходу натисніть [ESC]. [?]
	   )";
       OHandle::ClearScreenGuard repetosky_guard;

       LogHandle::info(instructions);

       auto my_sort = [&](const SortOrder order) {
           if (order == SortOrder::Ascending) {
               collection.sort(comparator);
               LogHandle::success(L"Успішно відсортовано за зростанням.");
           }
           else {
               collection.sort([&comparator](const auto& a, const auto& b) {
                   return comparator(b, a);
               });
               LogHandle::success(L"Успішно відсортовано за спаданням.");
           }
       };

       int descision;
       while ((descision = _getch()) != Console::ESCAPE) {

           if (descision == Console::ARROW_ALT) {
               switch (_getch()) {
	               case Console::ARROW_UP: {
	                   my_sort(SortOrder::Ascending);
	                   break;
	               }
	               case Console::ARROW_DOWN: {
	                   my_sort(SortOrder::Descending);
	                   break;
	               }
	               default:;
               }
           }
       }
    }

    template<typename Collection>
	void delete_at_index (Collection& collection)
    {
        for (;;) {
            const int index = display
            (
                collection,
                true,
                L"Натисність [ЕНТЕР], щоб " + LIGHT_RED + L"ВИДАЛИТИ запис "
            );
            rlutil::cls();

            if (index == -1)
                LogHandle::info(L" * Продовжити - будь-яку іншу клавішу  \n * Вихід - [ESC]");
            else
                LogHandle::info(L" * Підтвердження видалення екземпляру кінотайтлу - [ENTER]\n * Вихід - [ESC]");

            for (;;) {
                const int decision = _getch();
                if (decision == Console::ENTER) {
                    collection.clear(collection, Mode::MODE_INDEX, index);
                    break;
                }
                if (decision == Console::ESCAPE) return;
                break;
            }
        }
    }

    template<typename Collection>
	void delete_all_content(Collection& collection) {
        collection.clear(collection, Mode::MODE_ALL, NULL);
        this_thread::sleep_for(1s);
        LogHandle::info(L"Натисність [ESC], щоб повернутися назад...");
        while (_getch() != Console::ESCAPE) {}
    }

    template<typename Criteria, typename Collection>
	void delete_by
	(
        const Criteria& criteria, 
        Collection& collection
    )
	{
        if (!Console::Menu().is_confirmed(L"Підтвердити видалення ?")) return;
        OHandle::ClearScreenGuard repetosvsky_guard;
        collection.clear_if(collection, criteria);
        this_thread::sleep_for(1s);
        LogHandle::info(L"Натисність [ESC], щоб повернутися назад...");
        while (_getch() != Console::ESCAPE) {}
    }

    inline wostream& operator<<(wostream& os, const MovieDatabase& mdb) {
        for (auto& m : mdb.movie_list) os << m;
        return os;
    }

	inline wofstream& operator<<(wofstream& wofs, const MovieDatabase& mdb) {
        size_t movies_in_base   = mdb.size(),
    		   movies_exported  = 0,
    	       movies_undefined = 0;

        wcout << L"\nЕкспортую..." << '\n';

        for (auto& m : mdb.movie_list)
            if (wofs << m)
                ++movies_exported;
            else ++movies_undefined;

        wcout << LIGHT_GREEN << L"\nГотово." << WHITE << '\n';

        wclog << L"\nКількість кінотайтлів у базі -> "      << movies_in_base << '\n'
	          << L"Кількість експортованих кінотайтлів -> " << movies_exported << '\n'
	          << L"Кількість нерозпізнаних кінотайтлів -> " << movies_undefined << '\n';

        if (movies_exported == movies_in_base)
	        LogHandle::success(L"[|] Успіх при експортуванні. Увесь вміст кінобази експортовано. [|]\n");
        if (movies_exported > 0 && movies_undefined > 0)
	        LogHandle::warn(L"[!] Помилки при експортуванні. Частина кінотайлтів не експортована. [!]\n");
        if (movies_exported == 0)
	        LogHandle::error(L" <<!>> Невдача при експортуванні. Жоден кінотайтл не експортовано. <<!>> \n");
        return wofs;
    }

    inline wofstream& operator<<(wofstream& wofs, const shared_ptr<MovieDatabase>& mdb) {
        for (auto& m : mdb->movie_list)
            wofs << m;
        return wofs;
    }

    inline wifstream& operator>>(wifstream& wifs, MovieDatabase& mdb)
    {
        wstring line;
        size_t  lines_in_file    = 0,
    	        movies_added     = 0,
				movies_with_errors_added = 0,
                movies_skiped    = 0,
                movies_undefined = 0;

        rlutil::hidecursor();
        wcout << L"\n\nІмпортую кінотайтли..." << endl;

        wifs.imbue(locale(locale::empty(), new codecvt_utf8<wchar_t>));
        wstringstream wss;
        wss << wifs.rdbuf();
        while (getline(wss, line, L'\n'))
        {
            if (line.empty()) break;

            Movie movie;
            wstringstream _wss;
            _wss.str(line);
            _wss >> movie;
            ++lines_in_file;

            if (!_wss.fail() && movie.valid()) {

                if (mdb.init(movie)) {
                    ++movies_added;
                    
                    if (movie.has_error_state()) {
                        LogHandle::warn(L"Додано \"" + movie.get_title() + L"\"" + L" з незначними помилками.");
                        ++movies_with_errors_added;
                        movie.reset_error_state();
                    }
                    else LogHandle::success(L"Успішно додано \"" + movie.get_title() + L"\"");
                    
                }
                else {
                    ++movies_skiped;
                    LogHandle::warn(L"\"" + movie.get_title() + L"\"" + L" пропущено.");
                }
            }
            else {
                ++movies_undefined;
                LogHandle::error(L"Не вдалось імпортувати кінотайтл, що на " + to_wstring(lines_in_file) + L"-ій лінії з файлу.");
                wss.clear();
                movie.reset_error_state();
            }
        }

        wcout << LIGHT_GREEN << L"\n\nГотово." << WHITE << '\n';
        wclog << L"\nКількість кінотайтлів у файлі -> " << lines_in_file << '\n'
              << L"Кількість доданих тайтлів, які необхідно правити -> " << movies_with_errors_added << '\n'
              << L"Кількість доданих тайтлів -> " << movies_added << '\n'
              << L"Кількість пропущених тайтлів -> " << movies_skiped << endl
              << L"Кількість нерозпізнаних тайтлів -> " << movies_undefined << '\n';

        return wifs;
    }
}

