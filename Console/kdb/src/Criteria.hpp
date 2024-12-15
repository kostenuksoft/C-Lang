#pragma once
#include "../src/Movie.hpp"

namespace MvCriteria
{
    using namespace Mv;

    namespace MvSort {
        inline auto by_alphabet = [](const Movie& lhs, const Movie& rhs) -> bool {

            const wstring& lhs_title = lhs.get_title();
            const wstring& rhs_title = rhs.get_title();

            return lexicographical_compare
        	(
                lhs_title.begin(), lhs_title.end(),
                rhs_title.begin(), rhs_title.end(),
                [](wchar_t ch, wchar_t _ch) {
                    return towlower(ch) < towlower(_ch);
                }
            );
        };
        inline auto by_countries = [](const Movie& lhs, const Movie& rhs) -> bool {
            const wstring& lhs_countries = lhs.get_countries();
            const wstring& rhs_countries = rhs.get_countries();

            return lexicographical_compare(
                lhs_countries.begin(), lhs_countries.end(),
                rhs_countries.begin(), rhs_countries.end(),
                [](const wchar_t ch, const wchar_t _ch) {
                    return towlower(ch) < towlower(_ch);
                }
            );
        };
        inline auto by_year = [](const Movie& lhs, const Movie& rhs) -> bool {
            return lhs.get_year() < rhs.get_year();
        };
        inline auto by_duration = [](const Movie& lhs, const Movie& rhs) -> bool {
        	return lhs.get_duration() < rhs.get_duration();
        };
        inline auto by_avg_rating = [](const Movie& lhs, const Movie& rhs) -> bool {
            return lhs.get_avrg_rating() < rhs.get_avrg_rating();
        };
        inline auto by_no_of_ratings = [](const Movie& lhs, const Movie& rhs) -> bool {
            return lhs.get_no_of_ratings() < rhs.get_no_of_ratings();
        };
        inline auto by_budget = [](const Movie& lhs, const Movie& rhs) -> bool {
            return lhs.get_budget() < rhs.get_budget();
        };
        inline auto by_revenue = [](const Movie& lhs, const Movie& rhs) -> bool {
            return lhs.get_revenue() < rhs.get_revenue();
        };
    };

	class Criteria {
	public:

        // Допоміжний класс для пошуку кінотайтлів, містить корисні методи такі як
        //   -- перевантажений оператор Movie, копіює об'єкт
        //   -- перевірка чи успішно знайдено записи у базі - is_found
        //   -- повернути Movie без лишнього копіювання - get_movie
        //   -- загорнути Movie у допоміжний клас optional - to_optional
        //
        class Search {
        public:
            Search() : movie(), found(false) {}

            explicit Search(Movie movie) : movie(std::move(movie)), found(true) {}
            explicit operator Movie() const { return movie; }
            [[nodiscard]] bool is_found() const { return found; }
            [[nodiscard]] const Movie& get_movie() const { return movie; }
            [[nodiscard]] optional<Movie> to_optional() const { return found ? optional(movie) : nullopt; }

        private:
            // Наш екземпляр кінотайтлу.
            Movie movie;
            // його предикат
            bool found;
        };

        /* Ініціалізована обгортка, яка перевіряє лексикографічний вміст іменувань кінотайтлів, при цьому конвертує їх у нижній регістр при кінецвому порівнянні.*/
        function<bool(const Movie&, const Movie&)> title_comparator      = MvSort::by_alphabet;

        /* Ініціалізована обгортка, яка перевіряє лексикографічний вміст країн кінотайтлів, при цьому конвертує їх у нижній регістр при кінцевому порівнянні.*/
        function<bool(const Movie&, const Movie&)> сountries_comparator  = MvSort::by_countries;

        /* Ініціалізована обгортка, яка порівнює поля 'рік' у переданих об'єктах Movie.*/
        function<bool(const Movie&, const Movie&)> year_comparator       = MvSort::by_year;

        /* Ініціалізована обгортка, яка порівнює поля 'продовжуваність' у переданих об'єктах Movie.*/
        function<bool(const Movie&, const Movie&)> duration_comparator   = MvSort::by_duration;

        /* Ініціалізована обгортка, яка порівнює поля 'сер. оцінка' у переданих об'єктах Movie.*/
        function<bool(const Movie&, const Movie&)> avg_rating_comparator = MvSort::by_avg_rating;

        /* Ініціалізована обгортка, яка порівнює поля 'заг. к-сть оцінок' у переданих об'єктах Movie.*/
        function<bool(const Movie&, const Movie&)> no_of_ratings_comparator = MvSort::by_no_of_ratings;

        /* Ініціалізована обгортка, яка порівнює поля 'бюджет' у переданих об'єктах Movie.*/
        function<bool(const Movie&, const Movie&)> budget_comparator     = MvSort::by_budget;

        /* Ініціалізована обгортка, яка порівнює поля 'дохід' у переданих об'єктах Movie.*/
        function<bool(const Movie&, const Movie&)> revenue_comparator    = MvSort::by_revenue;

        /* Щоб досягти оберненого результату для кожного компаратора шляхів є декілька:
			- скористатись negate-функцією not1,
            - передати аргументи навпаки, first - second | second - first
            - заперечити результат лямбди оператором "!" під час повернення результату, зазвичай загорнуту в конкретну функцію з відповідною назвою
		*/

        // Критерії з UNICODE рядками, що чутливі до регістру.
        Criteria& title(const wstring& title_str,       bool sensitive = false);
        Criteria& director(const wstring& director_str, bool sensitive = false);
        Criteria& countries(const wstring& country_str, bool sensitive = false);
        Criteria& age_rating(const wstring& age_str,    bool sensitive = false);
        Criteria& genre(const wstring& genre_str,       bool sensitive = false);

        // Заперечимо результат кожної, щоб отримати обернені числові критерії
        Criteria& avg_rating_above(float min_rating); 
        Criteria& duration_above(size_t duration);
        Criteria& no_of_ratings_above(size_t n_of_r);
        Criteria& year_range(size_t start_year, size_t end_year);

        // Перевіряє чи міститься ввесь перелік критерій у vector-контейнері criteria_list, формує остаточний предикат */
        [[nodiscard]] function<bool(const Movie&)> build() const;

        // Довільна критерія, використана для завдань зазначених у варіанті курсового проєкту.
        Criteria& custom(function<bool(const Movie&)> my_criteria);

        // Ініціалізує критерію в criteria_list.
        void init(function<bool(const Movie&)> criteria);

        // Викидає останню критерію з criteria_list.
        void pop_last();

        // Перевірка чи конкретно введений літерал входить у інший.
        static bool contains(const wstring& target_str, const wstring& substr);

        //1. Переводить два UNICODE рядки у нижній регістр
        //2. Перевіряє чи конкретно введений UNICODE рядок входить у інший.
        static bool _contains(wstring target_str, wstring substr);

	private:
        // Репрезентує колекцію з критеріями.
        vector<function<bool(const Movie&)>> criteria_list;
    };
}
