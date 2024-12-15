#pragma once
#include "../src/Framework.hpp"
#include "../src/MovieDatabase.hpp"
#include "../src/Utils.hpp"

namespace MvSchema
{
    class Schema {

        using deref_schema = const
            pair<const unsigned long long,
            pair<wstring,
            pair<shared_ptr<MvDataBase::MovieDatabase>, wstring>>>;

        using repetovsky_db_ptr = shared_ptr<MvDataBase::MovieDatabase>;
        using repetovsky_schema_entry = pair<wstring, pair<repetovsky_db_ptr, wstring>>;
        inline static size_t track_id { 0 };

    public:

        class Backup {

        public:
            Backup(Backup& backup) = delete;
            Backup(const Schema& schemas, size_t id, chrono::minutes interval = chrono::minutes(5));
            ~Backup();

            void set_backup_interval(chrono::minutes new_interval);
            void trigger_one(size_t id, bool suppress_message);
            void trigger_all();

        private:

            void backup_loop();
            void backup_now(size_t id, bool suppress_message) const;
            static wstring backup_file_name_generator(const wstring& schema_name, const wstring& label);

            size_t schema_id;
            const Schema& schemas;

            thread backup_thread;
            chrono::minutes backup_interval;
            atomic<bool> stop_thread;
            mutex mtx;
            condition_variable cv;

        };

        struct Update final : MvException::MovieException {
            explicit Update(size_t id, repetovsky_schema_entry& sch);
        };

        Schema();
        ~Schema() { MvUtils::LogHandle::success(L"[/\\] Схеми очищено! [/\\]"); }

        [[nodiscard]] const wstring& get_name(size_t id) const;
        [[nodiscard]] const wstring& get_description(size_t id) const;

        void set_description(size_t id, const wstring& new_description);
        void set_name(size_t id, const wstring& new_name);

        size_t emplace(const wstring& schema_name, const wstring& description);
        void remove(size_t id);
        void clear();

        [[nodiscard]] size_t size() const { return schemas.size(); }

        [[nodiscard]] auto begin() { return schemas.begin(); }
        [[nodiscard]] auto end() { return schemas.end(); }
        [[nodiscard]] auto begin() const { return schemas.begin(); }
        [[nodiscard]] auto end() const { return schemas.end(); }

        [[nodiscard]] const repetovsky_schema_entry& at(const size_t index) const { return schemas.at(index);}
        [[nodiscard]] repetovsky_schema_entry& at(const size_t index) { return schemas.at(index); }
        [[nodiscard]] const repetovsky_db_ptr& database_ptr_at(const size_t index) const { return at(index).second.first; }

    	shared_ptr<MvDataBase::MovieDatabase>& last_db_ptr() { return db_ptr; }

        static size_t getid() { return track_id; }
    	static void setid(const size_t value) { track_id = value; }

        static void printf(const deref_schema& sch);

        friend wostream& operator<<(wostream& os, const pair<wstring, pair<repetovsky_db_ptr, wstring>>& sch);

    private:
        shared_ptr<MvDataBase::MovieDatabase> db_ptr;
        map<size_t, repetovsky_schema_entry> schemas;
    };

    inline wostream& operator<<(wostream& os, const pair<wstring, pair<Schema::repetovsky_db_ptr, wstring>>& sch) {

        const auto& title = sch.second.second;
        const auto& description = sch.second.second;
        const auto db_contains_records = !sch.second.first->empty();
        const auto amount = sch.second.first->size();
        Console::Menu m;

        auto any_records = [&]() -> wstring {
            return db_contains_records ? L"Так" : L"Ні";
            };

        auto rows_info = [&] {
            return db_contains_records ? to_wstring(amount) : L"Дані відсутні.";
        };

        streamsize WIDTH = 2;
        return os
            << MvUtils::LIGHT_CYAN
            << wstring(64, '=')
    	    << endl
            << left << L"|" << left << setw(WIDTH) << MvUtils::YELLOW << L"[Іменування схеми]: " << title         << MvUtils::LIGHT_CYAN << endl
            << left << L"|" << left << setw(WIDTH) << MvUtils::YELLOW << L"[Опис]: "             << description   << MvUtils::LIGHT_CYAN << endl
            << left << L"|" << left << setw(WIDTH) << MvUtils::YELLOW << L"[Містить записи]: "   << any_records() << MvUtils::LIGHT_CYAN << endl
            << left << L"|" << left << setw(WIDTH) << MvUtils::YELLOW << L"[К-сть рядків]: "     << rows_info()   << MvUtils::LIGHT_CYAN << endl
            << wstring(64, '=')
    	    << endl
            << MvUtils::CLEAR_COLOR;
    }
}
