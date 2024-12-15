#pragma once
#include "Menu.hpp"
#include "../src/Framework.hpp"

namespace MvUtils
{
	namespace ANSI {
        const wstring LIGHT_RED = L"\033[01;31m",
				      RED = L"\033[22;31m",
					  YELLOW = L"\033[01;33m",
                      LIGHT_CYAN = L"\033[01;36m",
                      CYAN = L"\033[22;36m",
                      LIGHT_GREEN = L"\033[01;32m",
                      GREEN = L"\033[22;32m",
                      WHITE = L"\033[01;37m",
                      MAGENTA = L"\033[22;35m",

					  LIGHT_GREEN_BG = L"\033[01;42m", 
					  GREEN_BG = L"\033[22;42m",       
					  LIGHT_RED_BG = L"\033[01;41m",   
					  RED_BG = L"\033[22;41m",

                      CLEAR_COLOR = L"\033[0m",
					  CLEAR_BG = L"\033[0m",
                      CLEAR_LINE = L"\033[2K",
                      CLEAR_SCREEN = L"\\033[K",
                      UNDERLINE = L"\033[4m",
					  CLEAR_UNDERLINE = L"\033[24m";
			          
	}

    using namespace ANSI;

    struct LogHandle {
        static void log(const wstring& message) {
            wclog << LIGHT_CYAN << message << WHITE << endl;
        }
        static void info(const wstring& message) {
            wcout << LIGHT_CYAN << message << WHITE << endl;
        }
        static void success(const wstring& message) {
            wcout << LIGHT_GREEN << message << WHITE << endl;
        }
        static void warn(const wstring& message) {
            wcout << YELLOW << message << WHITE << endl;
        }
        static void error(const wstring& message) {
            wcerr << LIGHT_RED << message << WHITE << endl;
        }
    };

    class OHandle {
    public:

        struct CursorVisibilityGuard {

            explicit CursorVisibilityGuard(const bool show) : hide_on_destruct(show) {
                if (show) rlutil::showcursor();
                else rlutil::hidecursor();
            }
            ~CursorVisibilityGuard() {
                if (hide_on_destruct) rlutil::hidecursor();
                else rlutil::showcursor();
            }

            CursorVisibilityGuard(const CursorVisibilityGuard&) = delete;
            CursorVisibilityGuard& operator=(const CursorVisibilityGuard&) = delete;

            CursorVisibilityGuard(CursorVisibilityGuard&& other) noexcept
                : hide_on_destruct(other.hide_on_destruct) {
                other.hide_on_destruct = false;
            }

        private:
            bool hide_on_destruct;
        };

        struct ClearScreenGuard {
            ~ClearScreenGuard() { rlutil::cls(); }
        };

        struct CursorPosition {
            static void save() { wcout << L"\033[s"; }
            static void restore() { wcout << L"\033[u"; }
        };

    };

    class IHandle
	{
    public:

        static wstring input_multiline(const wstring& message = L"Ввід") {
            OHandle::CursorVisibilityGuard repetovsky_guard(true);
            wstring result, line;
            LogHandle::info(L"При введенні, натисніть [TAB], якщо бажаєте почати з нового рядка.");
            LogHandle::info(L"Щоб закінчити ввід, зберети зміни, натисність [ENTER].");
            wcout << L"[" << message << L"] -> ";
            while (true) {
	            const wchar_t ch = _getwch();
                if (ch == Console::ENTER) { 
                    wcout << endl;
                    break;
                }
            	if (ch == Console::TAB) {
                    result += line + L'\n';
                    line.clear();
                    wcout << endl;
                }
                else if (ch == Console::BACKSPACE) {
                    if (!line.empty()) {
                        line.pop_back();
                        wcout << L"\b \b";
                    }
                }
                else {
                    line += ch;
                    wcout << ch;
                }
            }
            result += line; 
            return result;
        }

        template<typename T>
        static optional<T> try_input(const wstring& message) {
            OHandle::CursorVisibilityGuard guard(true);
            wcout << L"[" << message << "] -> ";
            T value;
            const auto& valid = wcin >> value;
            if (valid) return value;
            handle_input_error(L"Не числовий ввід.");
            return nullopt;
        }

        static optional<wstring> try_input_string(const wstring& message) {
            OHandle::CursorVisibilityGuard repetovsky_guard(true);
            wcout << L"["<< message <<"] -> ";
            wstring input = L" ";
            getline(wcin, input);
            if (!input.empty()) return input;
            handle_input_error(L"<!> Не слід вводити порожній рядок. <!>");
            return nullopt;
        }

        template<typename T>
        static T wrap_input(const wstring& message = L"Ввід") {
            auto result = try_input<T>(message);
            while (!result) 
                result = try_input<T>(message);
            return *result;
        }

        static wstring wrap_input_string(const wstring& message = L"Ввід") {
            auto result = try_input_string(message);
            while (!result) 
                result = try_input_string(message);
            return *result;
        }

        template<typename T>
        static T input_with_validation
    	(
            const function<bool(const T&)>& validator,
            const wstring& error_message = L"Ввід не по шаблону.",
            const wstring& custom_message = L"Ввід"
        )
    	{
            while (true) {
                auto input = wrap_input<T>(custom_message);
                if (validator(input)) return input;
                LogHandle::error(error_message);
            }
        }

        static wstring wstring_with_validation
    	(
            const function<bool(const wstring&)>& validator,
            const wstring& error_message = L"Ввід не по шаблону.",
            const wstring& custom_message = L"Ввід"
        )
    	{
            while (true) {
                auto input = wrap_input_string(custom_message);
                if (validator(input)) return input;
                LogHandle::error(error_message);
            }
        }

        static void anykey(const wstring& message = L"Натисніть будь-яку клавішу...") {
            LogHandle::info(message);
            rlutil::anykey();
        }

        static void handle_input_error(const wstring& message = L"<<!>> Некоректний ввід. <<!>>") {
            LogHandle::error(message);
            reset_input_stream();
        }

        static const wstring& to_wlower(wstring& str) {
            transform(str.begin(), str.end(), str.begin(), towlower);
            return str;
        }

    /*    template<typename T>
        T convert(const wstring& str) {
            if (!str.empty()) {
                if      constexpr (is_same_v<T, wstring>) return str;
                else if constexpr (is_same_v<T, int>)     return stoi(str);
                else if constexpr (is_same_v<T, float>)   return stof(str);
                else if constexpr (is_same_v<T, size_t>)  return static_cast<size_t>(stoul(str));
            }
            return {};
        }*/

        static void reset_input_stream() {
            wcin.clear();
            wcin.ignore(numeric_limits<streamsize>::max(), L'\n');
        }
    };

    class FHandle {
    public:

        struct FileGuard {

            explicit FileGuard(wstring path) : file_path(std::move(path)) {
            	remove_temp = [](const wstring& _path) {
	               const filesystem::path temp = _path;
                   auto final_path = temp.lexically_normal();
                   if (final_path.is_relative()) final_path = absolute(final_path);
                   filesystem::remove(final_path);
                };

            }

        	void _remove_temp() const {
                try {
                    if (filesystem::exists(file_path)) remove_temp(file_path);
                }
                catch ([[maybe_unused]] filesystem::filesystem_error& e) {
                    LogHandle::error(L"Помилка при видаленні тимчасового файлу.");
                    IHandle::anykey(L"Натисність будь-яку клавішу...");
                }
            }

            ~FileGuard() = default;
        private:
            wstring file_path;
        	function<void(const wstring&)> remove_temp;
        };
    };
}

