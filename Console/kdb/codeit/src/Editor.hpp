#pragma once
#include "LinkedList.hpp"

namespace MvEditor
{
    using namespace SLL;
    enum Keybinds
    {
        NEW_LINE = 10,
        ENTER = 13,
        SPACE = 32,
        BACKSPACE = 8,
        TAB = 9,
        ESC = 27,

        ARROW_ALT = 224,
        ARROW_UP = 72,
        ARROW_DOWN = 80,
        ARROW_RIGHT = 77,
        ARROW_LEFT = 75,

        y_VALUE = 121,
        Y_VALUE = 89,
        n_VALUE = 110,
        N_VALUE = 78,
    };

    template <typename Ty>
    class Editor 
    {
	    List<Ty>* sequence;

        struct Point {
            short x,
                  y,
                  index;
        } Pointer;

    public:

        Editor() = default;
        Editor(const Editor& codeit) = delete;
        Editor& operator=(const Editor& codeit) = delete;

        Editor(Editor&& codeit) = default;
        Editor& operator=(Editor&& codeit) = default;

        explicit Editor(List<Ty>* seq) : sequence(seq), Pointer{ 0,0,0 } {
            sequence = new List<Ty>();
        }

        ~Editor() {
            delete sequence;
            wprintf(L"Ресурси успішно очищено.");
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }

        void display_content() const {
            rlutil::cls();
            for (typename List<Ty>::Node* temp = sequence->get_head();
                temp != nullptr;
                temp = temp->next) {
                    wprintf(L"%lc", temp->data);
            }
        }


        bool run() {

            wchar_t key;
            while ((key = _getwch()) != ESC)
            {
                switch (key)
                {
                case ARROW_ALT:
                    switch (key = _getwch())
                    {
                    case ARROW_UP:
                        if (Pointer.y == 0) continue;
                        if (Pointer.x > get_line_length(Pointer.y)) {
                            Pointer.x = get_line_length(Pointer.y);
                            --Pointer.y;
                            Pointer.index = get_index(Pointer);
                            locate(Pointer);
                            continue;
                        }
                        --Pointer.y;
                        Pointer.index = get_index(Pointer);
                        locate(Pointer);
                        continue;
                    case ARROW_DOWN:
                        if (Pointer.y == get_height()) continue;
                        if (Pointer.x > get_line_length(Pointer.y + 1))
                        {
                            Pointer.x = get_line_length(Pointer.y + 1);
                            ++Pointer.y;
                            Pointer.index = get_index(Pointer);
                            locate(Pointer);
                            continue;
                        }
                        ++Pointer.y;
                        Pointer.index = get_index(Pointer);
                        locate(Pointer);
                        continue;
                    case ARROW_RIGHT:
                        if (Pointer.y == get_height() &&
                            Pointer.x == get_line_length(Pointer.y + 1)) continue;
                        if (Pointer.x == get_line_length(Pointer.y + 1)) {
                            Pointer.x = 0;
                            ++Pointer.y;
                            Pointer.index = get_index(Pointer);
                            locate(Pointer);
                            continue;
                        }
                        ++Pointer.x;
                        Pointer.index = get_index(Pointer);
                        locate(Pointer);
                        continue;
                    case ARROW_LEFT:
                        if (Pointer.x == 0 &&
                            Pointer.y == 0) break;

                        if (Pointer.x == 0)
                        {
                            Pointer.x = get_line_length(Pointer.y);
                            --Pointer.y;
                            Pointer.index = get_index(Pointer);
                            locate(Pointer);
                            continue;
                        }
                        --Pointer.x;
                        Pointer.index = get_index(Pointer);
                        locate(Pointer);
                    default: continue;
                    }
                case BACKSPACE:
                    if (Pointer.x == 0 && Pointer.y == 0) continue;
                    if (Pointer.x == 0) {
                        Pointer.x = get_line_length(Pointer.y);
                        --Pointer.y;
                        sequence->remove_by(--Pointer.index);
                    }
                    else {
                        std::wcout << static_cast<wchar_t>(BACKSPACE)
								   << static_cast<wchar_t>(SPACE)
                                   << static_cast<wchar_t>(BACKSPACE);
                        sequence->remove_by(--Pointer.index);
                        --Pointer.x;
                    }
                    break;

                case TAB:
                    for (int i = 0; i <= 2; ++i) {
                        sequence->insert_by(typename List<Ty>::Node::allocate(SPACE), Pointer.index);
                        ++Pointer.x;
                        ++Pointer.index;
                    }
                    break;

                case ENTER:
					case NEW_LINE:
                    sequence->insert_by(typename List<Ty>::Node::allocate(NEW_LINE), Pointer.index);
                    Pointer.x = 0;
                    ++Pointer.y;
                    ++Pointer.index;
                    break;
                default:
                    sequence->insert_by(typename List<Ty>::Node::allocate(key), Pointer.index);
                    ++Pointer.x;
                    ++Pointer.index;
                    break;
                }
                display_content();
                wprintf(L"\n------------- [ESC] - Вихід ---------------\n (c) kostenuksoft");
                locate(Pointer);

            }
            Pointer.y = get_height();
            locate(Pointer);
           
            return try_exit();
        }

        friend std::wifstream& operator>>(std::wifstream& wifs, Editor& codeit) {
            wifs.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));
            if (wifs.fail()) { return wifs; }

            wchar_t character;
            while (wifs.get(character))
            {
                if (character == '\n') {
                    codeit.sequence->insert_by(typename List<Ty>::Node::allocate(character), codeit.Pointer.index);
                    wprintf(L"%lc", character);

                    ++codeit.Pointer.y;
                    codeit.Pointer.x = 0;
                    ++codeit.Pointer.index;
                }
                else
                {
                    codeit.sequence->insert_by(typename List<Ty>::Node::allocate(character), codeit.Pointer.index);
                    wprintf(L"%lc", character);
                   
                    ++codeit.Pointer.x;
                    ++codeit.Pointer.index;
                }
            }
            return wifs;
        }
        friend std::wofstream& operator<<(std::wofstream& wofs, const Editor& codeit) {
            wofs.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));
            if (wofs.fail() || wofs.eof()) {
                rlutil::cls();
                wprintf(L"<!> Не вдалось зебергти файл. <!>");
                rlutil::anykey();
                return wofs;
            }

            typename List<Ty>::Node* temp = codeit.sequence->get_head();
            while (temp) {
                wofs.put(temp->data);
                temp = temp->next;
            }
            rlutil::cls();
            wprintf(L"Успіх.");
            return wofs;
        }

        [[nodiscard]] short get_height() const
        {
            short height = 0;
            for (typename List<Ty>::Node* temp = sequence->get_head(); nullptr != temp; temp = temp->next)
                if (temp->data == '\n') ++height;

            return height;
        }
        [[nodiscard]] short get_line_length(const int line_num) const
        {
            int i = 1;
            short linelen = 0;

            for (typename List<Ty>::Node* temp = sequence->get_head(); temp != nullptr; temp = temp->next) {
                if (i == line_num)
                    if (temp->data != '\n') ++linelen;
                    else break;
                else if (i < line_num) if (temp->data == '\n') ++i;
            }
            return linelen;
        }
        [[nodiscard]] short get_index(const Point& cursor) const
        {
            short i = 0, index = 0,
                x = cursor.x,
                y = cursor.y;
            for (typename List<Ty>::Node* temp = sequence->get_head();
                temp != nullptr;
                temp = temp->next)
            {
                if (i == y) {
                    for (i = 0; i < x; ++i) ++index;
                    return index;
                }
                if (temp->data == '\n') ++i;
                ++index;
            }
            return 0;
        }

        friend void locate(const Point& point) {
            HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
            CONST COORD Cord{point.x, point.y};
            SetConsoleCursorPosition(hStdout, Cord);
        }

    private:
	    [[nodiscard]] bool try_exit() 
	    {
            while (true)
            {
                rlutil::cls();
                display_content();
                wprintf(
                    L"+------------------------------------+\n"
                    L"| Зберігаємо зміни ? (y/n) | (r)esume\n"
                    L"+------------------------------------+\n"
                    L"[Ввід]: "
                );
               
                bool resume;
                switch (const int key = _getwch())
                {
	                case 'y':
						case 'Y':
	                        printf("%c", key);
	                        std::this_thread::sleep_for(std::chrono::milliseconds(250));
	                        rlutil::cls();
	                        display_content();
	                        return false;

					case 'n':
						case 'N':
                            printf("%c", key);
                            std::this_thread::sleep_for(std::chrono::milliseconds(250));
                            rlutil::cls();
                            return true;
                        default:
	                        case 'r':
								case 'R':
                                    resume = true;
		                            printf("%c", key);
		                            std::this_thread::sleep_for(std::chrono::milliseconds(250));
		                            rlutil::cls();
		                            display_content();
		                            break;
                }
                if (resume) run();
            }
        }
    };
}










