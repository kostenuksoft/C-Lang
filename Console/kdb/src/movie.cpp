#include "Menu.hpp"
#include "../src/Framework.hpp"
#include "../src/Movie.hpp"
#include "../src/MovieException.hpp"
#include "../src/Utils.hpp"

using namespace Mv;
using namespace MvUtils;
using namespace MvException;




/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

Movie::Movie() : year(0), duration(0), no_of_ratings(0), budget(0), revenue(0), avg_rating(0), error_state(false) {}

Movie::Movie(const Movie &orig): error_state(false)
{
	this->title = orig.title;
	this->director = orig.director;
	this->country = orig.country;
	this->year = orig.year;
	this->age_rating = orig.age_rating;
	this->genre = orig.genre;
	this->duration = orig.duration;
	this->avg_rating = orig.avg_rating;
	this->no_of_ratings = orig.no_of_ratings;
	this->revenue = orig.revenue;
	this->budget = orig.budget;
}

Movie& Movie::operator=(const Movie& orig)  {
    if (this == &orig) return *this;
    this->title         = orig.title;
    this->director      = orig.director;
    this->country       = orig.country;
    this->year          = orig.year;
    this->age_rating    = orig.age_rating;
    this->genre         = orig.genre;
    this->duration      = orig.duration;
    this->avg_rating    = orig.avg_rating;
    this->no_of_ratings = orig.no_of_ratings;
    this->revenue       = orig.revenue;
    this->budget        = orig.budget;

    return *this;
}

Movie::Movie(Movie&& orig) noexcept: error_state(false)
{
	this->title = orig.title;
	this->director = orig.director;
	this->country = orig.country;
	this->year = orig.year;
	this->age_rating = orig.age_rating;
	this->genre = orig.genre;
	this->duration = orig.duration;
	this->avg_rating = orig.avg_rating;
	this->no_of_ratings = orig.no_of_ratings;
	this->revenue = orig.revenue;
	this->budget = orig.budget;

	orig.title = L"";
	orig.director = L"";
	orig.country = {};
	orig.year = 0;
	orig.age_rating = {};
	orig.genre = {};
	orig.duration = 0;
	orig.avg_rating = 0;
	orig.no_of_ratings = 0;
	orig.revenue = 0;
	orig.budget = 0;
}

Movie& Movie::operator=(Movie&& orig) noexcept {
    if (this == &orig) return *this;
	this->title          = orig.title;
    this->director       = orig.director;
    this->country        = orig.country;
    this->year           = orig.year;
    this->age_rating     = orig.age_rating;
    this->genre          = orig.genre;
    this->duration       = orig.duration;
    this->avg_rating     = orig.avg_rating;
    this->no_of_ratings  = orig.no_of_ratings;
    this->revenue        = orig.revenue;
    this->budget         = orig.budget;

    orig.title         = L"";
    orig.director      = L"";
    orig.country       = {};
    orig.year          = 0;
    orig.age_rating    = {};
    orig.genre         = {};
    orig.duration      = 0;
    orig.avg_rating    = 0;
    orig.no_of_ratings = 0;
    orig.revenue       = 0;
    orig.budget        = 0;

    return *this;
}

Movie::Movie (
    wstring& _title,
    wstring& _director,
    wstring& _country_origin,
    size_t _year,
    wstring& _age_rating, 
    wstring& _genre,
    size_t _duration, 
    float _avg_rating, 
    size_t _no_of_ratings,
    size_t _budget, 
    size_t _revenue
)
: error_state (false)
 {

    bool is_rating_set        { false },
	     genre_set            { false },
         contry_set           { false },
	     age_rating_ex_thrown { false },
	     genre_ex_thrown      { false },
         country_ex_thrown    { false };

	this->title         = _title;
    this->director      = _director;
    this->year          = _year;
    this->duration      = _duration;
    this->avg_rating    = _avg_rating;
    this->no_of_ratings = _no_of_ratings;
    this->revenue       = _revenue;
    this->budget        = _budget;

    while (true) {

    	//TODO: Реалізувати кращу обробку помилок. 
        try
        {
            /***************************************************/
            if (!age_rating_ex_thrown && !is_rating_set) {
                if (_age_rating.empty())
                    throw MovieAgeRatingException("Error during Movie initialization due to emptiness of _age_rating constructor argument.", Priority::FatalError);
                if (_age_rating == L"PG") {
                    this->age_rating.PG = true;
                    is_rating_set = true;
                    continue;
                }
                if (_age_rating == L"PG-13") {
                    this->age_rating.PG13 = true;
                    is_rating_set = true;
                    continue;
                }
                if (_age_rating == L"NR") {
                    this->age_rating.NR = true;
                    is_rating_set = true;
                    continue;
                }
                if (_age_rating == L"PASS") {
                    this->age_rating.PASS = true;
                    is_rating_set = true;
                    continue;
                }
                if (_age_rating == L"APPROVED") {
                    this->age_rating.APPR = true;
                    is_rating_set = true;
                    continue;
                }
                if (_age_rating == L"R") {
                    this->age_rating.R = true;
                    is_rating_set = true;
                    continue;
                }
                if (_age_rating == L"G") {
                    this->age_rating.G = true;
                    is_rating_set = true;
                    continue;
                }
                if (_age_rating == L"M") {
                    this->age_rating.M = true;
                    is_rating_set = true;
                    continue;
                }
                if (_age_rating == L"X") {
                    this->age_rating.X = true;
                    is_rating_set = true;
                    continue;
                }
                if (_age_rating == L"TV-14") {
                    this->age_rating.TV14 = true;
                    is_rating_set = true;
                    continue;
                }
                if (!is_rating_set) {
                    error_state = true;
                    throw MovieAgeRatingException("Error during Movie initialization. Provided invalid age rating:",  Priority::Warning);
                }
            }
            /***************************************************/

            /***************************************************/
            if (!genre_ex_thrown && !genre_set) {
                if (_genre.empty())
                    throw MovieGenreException("Error during Movie initialization due to emptiness of _genre constructor argument.", Priority::FatalError);
                wstring genres = _genre;
                wistringstream genre_stream;
                genre_stream.str(genres);

                while (getline(genre_stream, genres, L'/'))
                {
                    if (genres == L"Бойовик") {
                        this->genre.Action = true;
                        genre_set = true;
                        continue;
                    }
                    if (genres == L"Пригоди") {
                        this->genre.Adventure = true;
                        genre_set = true;
                        continue;
                    }
                    if (genres == L"Анімація") {
                        this->genre.Animation = true;
                        genre_set = true;
                        continue;
                    }
                    if (genres == L"Документальні") {
                        this->genre.Biography = true;
                        genre_set = true;
                        continue;
                    }
                    if (genres == L"Комедії") {
                        this->genre.Comedy = true;
                        genre_set = true;
                        continue;
                    }
                    if (genres == L"Кримінальні") {
                        this->genre.Crime = true;
                        genre_set = true;
                        continue;
                    }
                    if (genres == L"Драма") {
                        this->genre.Drama = true;
                        genre_set = true;
                        continue;
                    }
                    if (genres == L"Сімейні") {
                        this->genre.Family = true;
                        genre_set = true;
                        continue;
                    }
                    if (genres == L"Фентезі") {
                        this->genre.Fantasy = true;
                        genre_set = true;
                        continue;
                    }
                    if (genres == L"Нуар") {
                        this->genre.FilmNoir = true;
                        genre_set = true;
                        continue;
                    }
                    if (genres == L"Історичні") {
                        this->genre.History = true;
                        genre_set = true;
                        continue;
                    }
                    if (genres == L"Жахи") {
                        this->genre.Horror = true;
                        genre_set = true;
                        continue;
                    }
                    if (genres == L"Музика") {
                        this->genre.Music = true;
                        genre_set = true;
                        continue;
                    }
                    if (genres == L"Мюзикл") {
                        this->genre.Musical = true;
                        genre_set = true;
                        continue;
                    }
                    if (genres == L"Містика") {
                        this->genre.Mystery = true;
                        genre_set = true;
                        continue;
                    }
                    if (genres == L"Романтика") {
                        this->genre.Romance = true;
                        genre_set = true;
                        continue;
                    }
                    if (genres == L"Фантастика") {
                        this->genre.SciFi = true;
                        genre_set = true;
                        continue;
                    }
                    if (genres == L"Трилер") {
                        this->genre.Thriller = true;
                        genre_set = true;
                        continue;
                    }
                    if (genres == L"Війна") {
                        this->genre.War = true;
                        genre_set = true;
                        continue;
                    }
                    if (genres == L"Західні") {
                        this->genre.Western = true;
                        genre_set = true;
                        continue;
                    }
                    
                    if (!genre_set) {
                        error_state = true;
                        LogHandle::error(L"<!> У кінотайтлі " + _title + L" не розпізнаний жанр " += genres + L" <!>\n");
                        throw MovieGenreException("Couldn`t create a movie. Provided invalid genre.", Priority::FatalError);
                    }
                        
                }
            }
            /***************************************************/

            /***************************************************/
            if (!country_ex_thrown && !contry_set) {
                if (_country_origin.empty())
                    throw MovieCountryException("Couldn't create movie due to emptiness of _country_origin constructor argument.",   Priority::FatalError);

                wstring countries = _country_origin;
                wistringstream countries_stream;
                countries_stream.str(countries);

                while (getline(countries_stream, countries, L'/')) {
                    if (countries == L"Австралія") {
                        this->country.Austraila = true;
                        contry_set = true;
                        continue;
                    }
                    if (countries == L"Канада") {
                        this->country.Canada = true;
                        contry_set = true;
                        continue;
                    }
                    if (countries == L"Фінляндія") {
                        this->country.Finland = true;
                        contry_set = true;
                        continue;
                    }
                    if (countries == L"Франція") {
                        this->country.France = true;
                        contry_set = true;
                        continue;
                    }
                    if (countries == L"Німеччина") {
                        this->country.Germany = true;
                        contry_set = true;
                        continue;
                    }
                    if (countries == L"Індія") {
                        this->country.India = true;
                        contry_set = true;
                    }
                    if (countries == L"Італія") {
                        this->country.Italy = true;
                        contry_set = true;
                        continue;
                    }
                    if (countries == L"Японія") {
                        this->country.Japan = true;
                        contry_set = true;
                        continue;
                    }
                    if (countries == L"Мексика") {
                        this->country.Mexico = true;
                        contry_set = true;
                        continue;
                    }
                    if (countries == L"Китай") {
                        this->country.PRC = true;
                        contry_set = true;
                        continue;
                    }
                    if (countries == L"Польща") {
                        this->country.Poland = true;
                        contry_set = true;
                        continue;
                    }
                    if (countries == L"Швеція") {
                        this->country.Sweden = true;
                        contry_set = true;
                        continue;
                    }
                    if (countries == L"Сполучене Королівство") {
                        this->country.UK = true;
                        contry_set = true;
                        continue;
                    }
                	if (countries == L"США") {
                        this->country.USA = true;
                        contry_set = true;
                        continue;
                    }
                	
                    if (!contry_set) {
                        error_state = true;
                        LogHandle::error(L"<!> У кінотайтлі " + _title + L" не розпізнана країна " += countries + L" <!>\n");
                        throw MovieCountryException("Couldn`t create a movie. User provided invalid country.", Priority::Warning);
                    }
                }
            }
            /***************************************************/

            break;
        }
        catch ([[maybe_unused]] MovieAgeRatingException& ex) {
            LogHandle::error(L" <!> У кінотайтлі '" + _title + L"' зазначена вікова оцінка  '" += _age_rating + L"' - не існує. <!> \n * Гляньте у довідку, там є перелік оцінок, які база наразі підтримує.");
            age_rating_ex_thrown = true;
        }
        catch ([[maybe_unused]] MovieGenreException& ex) {
            LogHandle::error(L"У кінотайтлі '" + _title + L"' зазначений жанр '" += _genre + L"' не існує. <!> \n * Гляньте у довідку, там є перелік жанрів, які база наразі підтримує.");
            genre_ex_thrown = true;
        }
        catch ([[maybe_unused]] MovieCountryException& ex) {
            LogHandle::error(L"У кінотайтлі '" + _title + L"' зазначена країна '" += _country_origin + L"' не існує або не внесена у базу. <!> \n * Гляньте у довідку, там є перелік країн, які база наразі підтримує.");
            country_ex_thrown = true;
        }
        catch (...) {
            LogHandle::error(L"<<?!>> Виникла невідома помилка при створенні кінотайтлу. <<?!>>");
        	break;
        }
        OHandle::ClearScreenGuard repetovsky_cls;
        IHandle::anykey(L"Натисніть будь-яку клавішу, щоб продовжити...");
    }
}


/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

wstring Movie::get_age_rating() const  {
    wstring age_rating_out;
    if (age_rating.NR)    age_rating_out.assign(L"NR");
    if (age_rating.PG)    age_rating_out.assign(L"PG");
    if (age_rating.PG13)  age_rating_out.assign(L"PG-13");
    if (age_rating.TV14)  age_rating_out.assign(L"TV-14");
    if (age_rating.PASS)  age_rating_out.assign(L"PASSED");
    if (age_rating.APPR)  age_rating_out.assign(L"APPROVED");
    if (age_rating.R)     age_rating_out.assign(L"R");
    if (age_rating.G)     age_rating_out.assign(L"G");
    if (age_rating.M)     age_rating_out.assign(L"M");
    if (age_rating.X)     age_rating_out.assign(L"X");

    if (!age_rating_out.empty()) return age_rating_out;

    return {L"Не встановлено."};
}
wstring Movie::get_genres() const {

    wstring genres_list;
    if (genre.Action)      genres_list.append(L"Бойовик/");
    if (genre.Adventure)   genres_list.append(L"Пригоди/");
    if (genre.Animation)   genres_list.append(L"Анімація/");
    if (genre.Biography)   genres_list.append(L"Документальні/");
    if (genre.Comedy)      genres_list.append(L"Комедії/");
    if (genre.Crime)       genres_list.append(L"Кримінальні/");
    if (genre.Drama)       genres_list.append(L"Драма/");
    if (genre.Family)      genres_list.append(L"Сімейні/");
    if (genre.Fantasy)     genres_list.append(L"Фентезі/");
    if (genre.FilmNoir)    genres_list.append(L"Нуар/");
    if (genre.Horror)      genres_list.append(L"Жахи/");
    if (genre.Music)       genres_list.append(L"Музика/");
    if (genre.Musical)     genres_list.append(L"Мюзикл/");
    if (genre.Mystery)     genres_list.append(L"Містика/");
    if (genre.Romance)     genres_list.append(L"Романтика/");
    if (genre.SciFi)       genres_list.append(L"Фантастика/");
    if (genre.Thriller)    genres_list.append(L"Трилер/");
    if (genre.War)         genres_list.append(L"Війна/");
    if (genre.Western)     genres_list.append(L"Західні/");

    if (!genres_list.empty()) {
        genres_list.pop_back();
        return genres_list;
    }
    return { L"Не встановлено." };
}
wstring Movie::get_countries() const {
    wstring countries_out;
    if (country.Austraila) countries_out.append(L"Австралія/");
    if (country.Canada)    countries_out.append(L"Канада/");
    if (country.Finland)   countries_out.append(L"Фінляндія/");
    if (country.France)    countries_out.append(L"Франція/");
    if (country.Germany)   countries_out.append(L"Німеччина/");
    if (country.India)     countries_out.append(L"Індія/");
    if (country.Italy)     countries_out.append(L"Італія/");
    if (country.Japan)     countries_out.append(L"Японія/");
    if (country.Mexico)    countries_out.append(L"Мексика/");
    if (country.PRC)       countries_out.append(L"Китай/");
    if (country.Poland)    countries_out.append(L"Польща/");
    if (country.Sweden)    countries_out.append(L"Швейцарія/");
    if (country.UK)        countries_out.append(L"Сполучене Королівство/");
    if (country.USA)       countries_out.append(L"США/");

    if (!countries_out.empty()) {
        countries_out.pop_back();
        return countries_out;
    }
    return {L"Не встановлено."};
}

bool Movie::valid() const {
    return !title.empty()             &&
	       !director.empty()          &&
	       !get_countries().empty()   && 
	       year > 1900 && year < 2024 &&
	       !get_age_rating().empty()  && 
	       !get_genres().empty()      && 
	       duration < numeric_limits<size_t>::max() &&
	       avg_rating <= 10.0         &&
	       no_of_ratings < numeric_limits<size_t>::max() &&
	       budget        < numeric_limits<size_t>::max() &&
           revenue       < numeric_limits<size_t>::max();
}

wostream& Movie::to_wostream(wofstream& wofs, const Movie& movie) {
	wofs.imbue(std::locale(std::locale::empty(), new codecvt_utf8<wchar_t>));
	const wchar_t sep = L',';
	return 
		wofs << quoted(movie.title) 
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
		<< movie.budget << L'\n';
}

/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/


