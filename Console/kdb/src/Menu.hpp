#pragma once
#include "../src/Framework.hpp"

namespace Console {

	// TODO: Оптимізувати швидкість навігації в меню, оскільки з ітераторами маємо болючу лінійну часову комлексію.

    constexpr size_t DEFAULT_STRING_LENGTH = 64;
    inline wchar_t FILL_WITH = L' ';

    enum KeyCodes {
        BACKSPACE = 8,
        TAB = 9,
        ENTER = 13,
        ESCAPE = 27,
        F1 = 59,
        F2 = 60,
        ARROW_ALT = 224,
        ARROW_UP = 72,
        ARROW_LEFT = 75,
        ARROW_RIGHT = 77,
        ARROW_DOWN = 80,
    };
   
    class MenuItem {
    public:
      
        virtual void help()        = 0;
        virtual void printm()      = 0;
        virtual void execute()     = 0;
        virtual bool try_exit()    = 0;
        virtual void clear_cache() = 0;

        virtual ~MenuItem() = default;

    protected:
        using action = function<void()>;
        using process = pair<wstring, action>;
        using process_vector = vector<process>;
        using process_vector_iterator = process_vector::const_iterator;
    };

    class Menu : protected MenuItem
    {
    public:

        Menu();
        Menu(wstring _heading, size_t _str_len);

        Menu(const Menu&) = delete;
        Menu& operator=(const Menu&) = delete;
        Menu(Menu&&) noexcept = default;
        Menu& operator=(Menu&&) noexcept = default;

        ~Menu() override;
        
        [[nodiscard]] const wstring& get_heading() const { return _heading; }
        [[nodiscard]] size_t get_str_max_len() const     { return _max_str_len; }

        void set_heading(const wstring& heading) { _heading = heading; }
        void set_str_max_len(size_t str_len);

        void help() override;
        void printm() override;
        void execute() override;

        bool try_exit() override;
        void clear_cache() override;

        virtual bool is_confirmed(const wstring& prompt);

        virtual void emplace_process(const process& parent_process_module);
        virtual bool remove_process(const wstring& process_heading);

        static void printf
    	(
            const wstring& target_string, 
            const wstring& params, 
            const wstring& params_clear, 
            size_t max_size, 
            int ratio
        );

    protected:
        wstring _heading;
        size_t  _max_str_len;
    private:
        process_vector _parent_process_vector;
        process_vector_iterator _parent_iterator_position;
    };

    class SubMenu final : public Menu {
    public:
        SubMenu();

        SubMenu(const SubMenu&) = delete;
        SubMenu& operator=(const SubMenu&) = delete;
        SubMenu(SubMenu&&) = default;
        SubMenu& operator=(SubMenu&&) = default;
        ~SubMenu() override;

        void emplace_process(const process& child_process_module) override;
        bool remove_process(const wstring& process_heading) override;
        void help() override;
        void printm() override;
        void execute() override;
        bool try_exit() override;
        void clear_cache() override;
        bool is_confirmed(const wstring& prompt) override;

    private:
        process_vector _child_process_vector;
        process_vector_iterator _child_iterator_position;
    };
}


