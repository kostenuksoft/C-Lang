#include "../src/Schema.hpp"
#include "../src/Utils.hpp"

using namespace MvUtils;
using namespace MvSchema;

// Блок резервного копіювання
/*************************************************************************************************************************/
Schema::Backup::Backup(const Schema& schemas, const size_t id, const chrono::minutes interval) :
	schema_id(id),
    schemas(schemas),
    backup_interval(interval),
    stop_thread(false)
{
	backup_thread = thread(&Backup::backup_loop, this);
}
Schema::Backup::~Backup() {
	stop_thread = true;
	cv.notify_one();
	if (backup_thread.joinable()) 
		backup_thread.join();
}
void Schema::Backup::set_backup_interval(const chrono::minutes new_interval) {
	lock_guard lock(mtx);
	backup_interval = new_interval;
	cv.notify_one();
}
void Schema::Backup::trigger_one(const size_t id, const bool suppress_message = false) {
	lock_guard lock(mtx);
	backup_now(id, suppress_message);
}
void Schema::Backup::trigger_all() {
	for (const auto& sch : schemas) {
		const auto id = sch.first;
		if (!schemas.database_ptr_at(id)->empty()) {
			trigger_one(id, true);
			LogHandle::success(L" -- Створена резервна копія схеми " + schemas.get_name(id));
		}
		else LogHandle::error(L"Резервну копію схеми " + schemas.get_name(id) + L" не створено, оскільки вона порожня.");
	}
}
void Schema::Backup::backup_loop() {
	while (!stop_thread) {
		unique_lock lock(mtx);
		if (cv.wait_for(lock, backup_interval, 
			[this] { return stop_thread.load(); }))
			return;
		backup_now(schema_id, false);
	}
}
void Schema::Backup::backup_now(const size_t id, const bool suppress_message) const {
	if (!schemas.database_ptr_at(id)->empty()) {
	
		filesystem::create_directories(L"backup/movies");
		wstring schema_name = schemas.get_name(id);
		wstring mv = backup_file_name_generator(schema_name, L"mv");
		wstring movies_path = L"backup/movies/" + mv;
		wofstream backup_movies(movies_path);
		if (!backup_movies.is_open()) {
			LogHandle::error(L"<<?!>> Неможливо відкрити файл для резервного копіювання кінотайтлів... <<?!>>");
			this_thread::sleep_for(1.5s);
			return;
		}
		 
		schemas.database_ptr_at(id)->serializate(backup_movies);
		if (!suppress_message) {
			OHandle::CursorPosition::save();
			rlutil::curstb();
			LogHandle::success(L"[\\/] Успішно створено резеврну копію cхеми cтаном на: " + rlutil::wtmstmp(APP_LOCALE_UKRAINIAN) + L" [\\/]");
			OHandle::CursorPosition::restore();
		}
		backup_movies.close();
	}
}
wstring Schema::Backup::backup_file_name_generator(const wstring& schema_name, const wstring& label = L"") {
	auto now = chrono::system_clock::now();
	auto time = chrono::system_clock::to_time_t(now);
	wstringstream ss;
	ss << schema_name << L"-"
		<< label << L"-"
		<< put_time(localtime(&time), L"%Y%m%d-%H%M%S") << L".txt";
	return ss.str();
}
/*******************************************************************************************************************************/

// СХЕМА-блок
/*******************************************************************************************************************************/
Schema::Schema() {
	schemas.emplace(0, repetovsky_schema_entry{ L"BASE", 
		{db_ptr = make_shared<MvDataBase::MovieDatabase>(), L"Ваша стартова схема."} });
}
void Schema::set_description(const size_t id, const wstring& new_description) {
	const auto it = schemas.find(id);
	if (it == schemas.end()) throw out_of_range("Invalid key.");
	it->second.second.second = new_description;
}
size_t Schema::emplace(const wstring& schema_name, const wstring& description) {

	for (const auto& _schema_name : schemas) {
		if (_schema_name.second.first == schema_name) {
			LogHandle::error(L"<<!>> Назва схеми повинна бути унікальною. <<!>>");
			return 0;
		}
	}

	size_t new_key = ++track_id;
	schemas.emplace(new_key, repetovsky_schema_entry{ 
		schema_name, {db_ptr = make_shared<MvDataBase::MovieDatabase>(), description} });
	return new_key;
}
void Schema::remove(const size_t id) {
	if (id == 0) {
		LogHandle::error(L"Забороняється видалення основної схеми.");
		this_thread::sleep_for(1.5s);
		return;
	}
	wstring deleted_name = get_name(id);
	if (schemas.erase(id) == 0) throw MvException::SchemaException("Invalid key provided when tried to remove schema.",  MvException::Priority::FatalError, 100);
	LogHandle::success(L"[\\/] Успішно видалено схему " + deleted_name + L" [\\/]");
}
void Schema::set_name(const size_t id, const wstring& new_name) {
	if (new_name.empty()) return;
	const auto it = schemas.find(id);
	if (it == schemas.end()) throw MvException::SchemaException("Invalid key provided when tried to set schema name.", MvException::Priority::FatalError, 100);
	if (new_name.empty()) {
		LogHandle::error(L"База даних повинна бути названа коректно, жодних пустих рядків.");
		return;
	}
	it->second.first = new_name;
}
const wstring& Schema::get_name(const size_t id) const {
	const auto it = schemas.find(id);
	if (it == schemas.end()) throw MvException::SchemaException("Invalid key provided when tried to get schema name.", MvException::Priority::FatalError, 100);
	return it->second.first;
}
const wstring& Schema::get_description(const size_t id) const {
	const auto it = schemas.find(id);
	if (it == schemas.end()) throw MvException::SchemaException("Invalid key provided when tried to get schema description.", MvException::Priority::FatalError, 100);
	return it->second.second.second;
}
void Schema::printf(deref_schema& sch)
{
	const auto id = sch.first;
	const auto& title = sch.second.first;
	const auto& description = sch.second.second.second;
	const auto db_contains_records = !sch.second.second.first->empty();
	const auto amount = sch.second.second.first->size();

	Console::Menu m;

	auto any_records = [&]() -> wstring {
		return db_contains_records ? L"Так" : L"Ні";
	};

	auto rows_info = [&] {
		return db_contains_records ? to_wstring(amount) : L"Дані відсутні.";
	};

	auto safe_description = [](const wstring& text, const size_t line_length) -> wstring {
		if (text.length() <= line_length) return text;

		wstring result;
		size_t pos = 0;
		size_t last_space = 0;

		while (pos < text.length()) {
			size_t next_space = text.find(L' ', pos);
			size_t chunk_length = min(line_length, text.length() - pos);
			if (next_space != wstring::npos && next_space - pos <= line_length) {
				while (next_space != wstring::npos && next_space - pos <= line_length) {
					last_space = next_space;
					next_space = text.find(L' ', last_space + 1);
				}
				chunk_length = last_space - pos;
			}
			if (!result.empty()) result + LIGHT_CYAN + L"\n|" += YELLOW + wstring(L"[Опис]:"s.length(), L' ');

			result += text.substr(pos, chunk_length);
			pos += chunk_length < line_length && last_space != pos ? chunk_length + 1 : chunk_length;
			last_space = pos;
		}
		return result;
	};

	wcout
		<< LIGHT_CYAN
		<< wstring(32, '=') << L"[№" << id + 1 << L"]" << wstring(32, '=') << endl
	    << L"|" << YELLOW << L"[Іменування схеми]: " << title          << LIGHT_CYAN << endl
		<< L"|" << YELLOW << L"[Опис]: "             << safe_description(description, 60) << LIGHT_CYAN << endl
	    << L"|" << YELLOW << L"[Містить записи]: "   << any_records()  << LIGHT_CYAN << endl
	    << L"|" << YELLOW << L"[К-сть рядків]: "     << rows_info()    << LIGHT_CYAN << endl
		<< wstring(68, '=') << endl
		<< WHITE;

}
Schema::Update::Update(const size_t id, repetovsky_schema_entry& sch)
	: MovieException("Movie db changed to " + rlutil::WideToMultiByteString(sch.first)){
	LogHandle::warn(L"[!] Схему змінено на " + sch.first + L" [!]");
	setid(id);
}
void Schema::clear() {
	schemas.clear();
	schemas.emplace
	(
		0, 
		repetovsky_schema_entry{ L"BASE",
			{db_ptr = make_shared<MvDataBase::MovieDatabase>(), L"Cтартова схема. Не може бути видалена."} }
	);
	track_id = 0;
}
/*******************************************************************************************************************************/


