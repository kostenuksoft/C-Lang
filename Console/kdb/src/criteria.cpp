#include "../src/Framework.hpp"
#include "../src/Criteria.hpp"

using namespace MvCriteria;

/**************************************************************************************************/
Criteria& Criteria::title(const wstring& title_str, bool sensitive) {
	init([title_str, sensitive](const Movie& m) {
		return sensitive ?
			contains(m.get_title(), title_str)
		:
		    _contains(m.get_title(), title_str);
	});
	return *this;
}
Criteria& Criteria::director(const wstring& director_str, bool sensitive)
{
	init([director_str, sensitive](const Movie& m) {
		return sensitive ?
			contains(m.get_director(), director_str)
		:
		   _contains(m.get_director(), director_str);
	});
	return *this;
}
Criteria& Criteria::countries(const wstring& country_str, bool sensitive)
{
	init([country_str, sensitive](const Movie& m) {
		return sensitive ?
			contains(m.get_countries(), country_str)
		:
		   _contains(m.get_countries(), country_str);
	});
	return *this;
}
Criteria& Criteria::year_range(size_t start_year, size_t end_year)
{
	init([start_year, end_year](const Movie& m) {
		return m.get_year() >= start_year && m.get_year() <= end_year;
	});
	return *this;
}
Criteria& Criteria::duration_above(size_t duration)
{
	init([duration](const Movie& m) {
		return m.get_duration() >= duration;
	});
	return *this;
}
Criteria& Criteria::no_of_ratings_above(size_t n_of_r)
{
	init([n_of_r](const Movie& m) {
		return m.get_no_of_ratings() >= n_of_r;
		});
	return *this;
}
Criteria& Criteria::age_rating(const wstring& age_str, bool sensitive)
{
	init([age_str, sensitive](const Movie& m) {
		return sensitive ?
			 contains(m.get_age_rating(), age_str)
		:
			_contains(m.get_age_rating(), age_str);
	});
	return *this;
}
Criteria& Criteria::avg_rating_above(float min_rating)
{
	init([min_rating](const Movie& m) {
		return m.get_avrg_rating() >= min_rating;
	});
	return *this;
}
Criteria& Criteria::genre(const wstring& genre_str, bool sensitive)
{
	init([genre_str, sensitive](const Movie& m) {
		return sensitive ?
			contains(m.get_genres(), genre_str)
		:
			_contains(m.get_genres(), genre_str);
	});
	return *this;
}
Criteria& Criteria::custom(function<bool(const Movie&)> my_criteria) {
	init(std::move(my_criteria));
	return *this;
}

function<bool(const Movie&)> Criteria::build() const {
	return [criteria = criteria_list](const Movie& m) {
		return all_of(criteria.begin(), criteria.end(),
			[&m](const auto& my_criteria) { return my_criteria(m); });
		};
}
/**************************************************************************************************/

/**************************************************************************************************/
void Criteria::init(function<bool(const Movie&)> criteria) {
	criteria_list.push_back(std::move(criteria));
}
void Criteria::pop_last() {
	criteria_list.pop_back();
}
bool Criteria::contains(const wstring& target_str, const wstring& substr) {
	return target_str.find(substr) != wstring::npos;
}
bool Criteria::_contains(wstring target_str, wstring substr) {
    transform(target_str.begin(), target_str.end(), target_str.begin(), towlower);
    transform(substr.begin(), substr.end(), substr.begin(), towlower);
    return contains(target_str, substr);
}
/**************************************************************************************************/

