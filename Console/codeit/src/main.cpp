#include "Editor.hpp"


int wmain(int argc, wchar_t *argv[])
{
	SetConsoleTitleA("codeit ver. 0.01 - kostenuksoft");
	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);
	_wsetlocale(LC_CTYPE, L"ukr");
	std::ios_base::sync_with_stdio(false);

    if (argc != 2) {
		std::wcerr << L"[Використання програми]: codeit [шлях до файлу]\n"
			       << L"[Приклад]: codeit ../codeit/examples/example.txt\n" << std::endl;
		std::wcout << L"Натисність будь-яку клавішу..." << std::endl;
		rlutil::anykey();
		return -1;
    }

	const std::wstring filepath = *++argv;

	
	SLL::List<wchar_t>* content = nullptr;
	MvEditor::Editor codeit(content);

	rlutil::cls();
	auto filename = filepath.substr(filepath.find_last_of(L'/') + 1);

	if (!std::filesystem::exists(filepath)) {
		std::wcerr << L"Файл " << filename << L" не існує, створюю..." << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	
	std::wifstream buff;
	std::wofstream file;

	buff.open(filepath, std::ios::in);
	buff >> codeit;
	buff.close();

	while (true) {
		if (!codeit.run()) {
			file.open(filepath, std::ios::out);
			file << codeit;
			file.close();
			wprintf(L"\n");
			break;
		}
		return 0;
	}
}
