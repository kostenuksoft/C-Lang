#pragma once
#include "../src/Framework.hpp"
#include "../src/Utils.hpp"

namespace Mv
{
    class Movie {
    public:
        Movie();

        Movie(const Movie& orig);
        Movie& operator=(const Movie& orig);
        Movie(Movie&& orig) noexcept;
        Movie& operator=(Movie&& orig) noexcept;

        Movie
        (
            wstring& _title,
            wstring& _director,
            wstring& _country_origin,
            size_t   _year,
            wstring& _age_rating,
            wstring& _genre,
            size_t   _duration,
            float    _avg_rating,
            size_t   _no_of_ratings,
            size_t   _budget,
            size_t   _revenue
        );

        ~Movie() = default;

        [[nodiscard]] wstring get_title() const { return title; }
        [[nodiscard]] wstring get_director() const { return director; }
        [[nodiscard]] size_t get_year() const { return year; }
        [[nodiscard]] size_t get_duration() const { return duration; }
        [[nodiscard]] float get_avrg_rating() const { return avg_rating; }
        [[nodiscard]] size_t get_no_of_ratings() const { return no_of_ratings; }
        [[nodiscard]] size_t get_revenue() const { return revenue; }
        [[nodiscard]] size_t get_budget() const { return budget; }

        [[nodiscard]] wstring get_age_rating()    const;
        [[nodiscard]] wstring get_genres()        const;
        [[nodiscard]] wstring get_countries()     const;

        [[nodiscard]] bool valid() const;
        

        [[nodiscard]] bool has_error_state() const { return error_state; }
        void reset_error_state() { error_state = false; }

        static wostream& to_wostream(wofstream& wofs, const Movie& movie);

        friend wostream& operator<<(wostream& wos, const Movie& movie);
        friend wistream& operator>>(wistream& wis, Movie& movie);
        friend wofstream& operator<<(wofstream& wofs, const Movie& movie);

        friend bool operator==(const Movie& m, const Movie& _m);
        friend bool operator!=(const Movie& m, const Movie& _m);
        friend bool operator< (const Movie& m, const Movie& _m);
        friend bool operator> (const Movie& m, const Movie& _m);
        friend bool operator<=(const Movie& m, const Movie& _m);
        friend bool operator>=(const Movie& m, const Movie& _m);

    private:
        wstring title,
                director;
        size_t  year,
                duration,
                no_of_ratings,
                budget,
                revenue;
        float   avg_rating;

        bool error_state;

        // Приватне біт-поле, що репрезентує набір країн 
        struct Countries {
            bool Canada  { false }, USA       { false }, UK     { false },
                 Germany { false }, Italy     { false }, France { false },
                 Japan   { false }, PRC       { false }, Mexico { false },
                 India   { false }, Poland    { false }, Sweden { false },
                 Finland { false }, Austraila { false };
        };

        // Приватне біт-поле, що репрезентує набір жанрів 
        struct Genres {
            bool Action    { false }, Adventure { false }, Fantasy   { false },
                 Comedy    { false }, Drama     { false }, War       { false },
                 Music     { false }, Romance   { false }, Western   { false },
                 Mystery   { false }, Thriller  { false }, SciFi     { false },
                 Animation { false }, Family    { false }, Biography { false },
                 History   { false }, Horror    { false }, Musical   { false },
                 Crime     { false }, FilmNoir  { false };
        };

        // Приватне біт-поле, що репрезентує набір вікових обмежень 
        struct Age_ratings {
            bool PG13 { false }, R { false }, PG   { false },
                 PASS { false }, G { false }, APPR { false },
                 M    { false }, X { false }, TV14 { false },
                 NR   { false };
        };

        Countries country;
        Genres genre;
        Age_ratings age_rating;
    };

    inline wostream& operator<<(wostream& wos, const Movie& movie) {
        constexpr streamsize WIDTH = 10;
        return wos << MvUtils::LIGHT_CYAN
    	    << L"+--------------------------------------------------------------------------------------------+" << endl
            << "|" << left << setw(WIDTH) << MvUtils::LIGHT_CYAN << L"[Назва кінотайтлу]: " << quoted(movie.title) << endl
            << "|" << left << setw(WIDTH) << MvUtils::LIGHT_CYAN << L"[Режисер]: " << quoted(movie.director) << endl
            << "|" << left << setw(WIDTH) << MvUtils::LIGHT_CYAN << L"[Рік випуску]: " << movie.year << endl
            << "|" << left << setw(WIDTH) << MvUtils::LIGHT_CYAN << L"[Країна походження]: " << quoted(movie.get_countries()) << endl
            << "|" << left << setw(WIDTH) << MvUtils::LIGHT_CYAN << L"[Вікове обмеження]: " << quoted(movie.get_age_rating()) << endl
            << "|" << left << setw(WIDTH) << MvUtils::LIGHT_CYAN << L"[Жанр]: " << quoted(movie.get_genres()) << endl
            << "|" << left << setw(WIDTH) << MvUtils::LIGHT_CYAN << L"[Продовжуваність]: " << movie.duration << L" хв." << endl
            << "|" << left << setw(WIDTH) << MvUtils::LIGHT_CYAN << L"[Середня оцінка]: " << movie.avg_rating << endl
            << "|" << left << setw(WIDTH) << MvUtils::LIGHT_CYAN << L"[Всього оцінок]: " << movie.no_of_ratings << endl
            << "|" << left << setw(WIDTH) << MvUtils::LIGHT_CYAN << L"[Стартовий капітал]: " << movie.budget << " $" << endl
            << "|" << left << setw(WIDTH) << MvUtils::LIGHT_CYAN << L"[Дохід]: " << movie.revenue << " $" << endl
            << "|" << left << setw(WIDTH * 3) << MvUtils::LIGHT_CYAN << L"[<] Натисність [ESC], щоб повернутись назад. [<]" << endl
            << L"+--------------------------------------------------------------------------------------------+"
            << MvUtils::WHITE << endl;
    }

    inline wistream& operator>>(wistream& wis, Movie& movie) {
        wis.imbue(std::locale(std::locale::empty(), new codecvt_utf8<wchar_t>));
        wstring title,
    	        director,
                countries_origin,
    	        age_rating,
    	        genres;

        size_t  year = 0,
    	        duration = 0,
    	        total_rating = 0,
    	        revenue = 0,
    	        budget = 0;
        float   avg_rating = 0;
        wchar_t sep;

        if (wis >> quoted(title)            >> sep >> quoted(director) >> sep
                >> quoted(countries_origin) >> sep >> year                >> sep >> quoted(age_rating) >> sep
                >> quoted(genres)           >> sep >> duration            >> sep >> avg_rating            >> sep
                >> total_rating                >> sep >> revenue             >> sep >> budget) {
            try {
                movie = Movie(title, director, countries_origin, year, age_rating,
                    genres, duration, avg_rating, total_rating, revenue, budget);
                movie.error_state = false;
            }
            catch (...)
            {
                movie.error_state = true;
                rlutil::cls();
                wcout << MvUtils::LIGHT_RED
                    << L"<!> Рекомендований порядок порушено. <!>"
                    << MvUtils::LIGHT_CYAN << endl
                    << L"[?] Порада: Скористатись користувацькою довідкою. [?]"
                    << MvUtils::WHITE << endl;
            }
        }
        else wis.clear(ios_base::failbit);
        return wis;
    }
    inline wofstream& operator<<(wofstream& wofs, const Movie& movie) {
        wofs.imbue(std::locale(std::locale::empty(), new codecvt_utf8<wchar_t>));
        const wchar_t sep = L',';
        if (
            wofs 
            << quoted(movie.title) 
            << sep
            << quoted(movie.director) 
            << sep
            << quoted(movie.get_countries().append(L"/")) 
            << sep
            << movie.year 
            << sep
            << quoted(movie.get_age_rating()) 
            << sep
            << quoted(movie.get_genres().append(L"/")) 
            << sep
            << movie.duration 
            << sep
            << movie.avg_rating 
            << sep
            << movie.no_of_ratings 
            << sep
            << movie.revenue 
            << sep
            << movie.budget 
            << L'\n'
            ) 
        {
            MvUtils::LogHandle::success(L"Успішно експортовано кінотайтл \"" + movie.title + L"\"");
        }
        else {
            MvUtils::LogHandle::error(L"Не вдалось експортувати кінотайтл \"" + movie.title + L"\"");
            MvUtils::LogHandle::warn(L"Без паніки!");
            MvUtils::LogHandle::info(L"Зверніться до папки 'backup'. \nВміст імпортованих кінотайтлів ви знайдете там.");
            MvUtils::IHandle::anykey(L"Натисність будь-яку клавішу...");
            wofs.clear(ios_base::failbit);
        }
        return wofs;
   }

    inline bool operator==(const Movie& m, const Movie& _m) { return m.title == _m.title; }
    inline bool operator!=(const Movie& m, const Movie& _m) { return !(m == _m); }
    inline bool operator>(const Movie& m, const Movie& _m)  { return tie(m.title, m.year) >  tie(_m.title, _m.year); }
    inline bool operator<(const Movie& m, const Movie& _m)  { return tie(m.title, m.year) <  tie(_m.title, _m.year); }
    inline bool operator>=(const Movie& m, const Movie& _m) { return tie(m.title, m.year) >= tie(_m.title, _m.year); }
    inline bool operator<=(const Movie& m, const Movie& _m) { return tie(m.title, m.year) <= tie(_m.title, _m.year); }

}





