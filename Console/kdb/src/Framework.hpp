#pragma once

#define APP_NAME    "KINOSTUDIA"
#define APP_VERSION "0.1"
#define APP_LABEL   "RDB" // Runtime DataBase
#define APP_TITLE   APP_NAME " - " APP_LABEL " ver. " APP_VERSION
#define APP_LOCALE_UKRAINIAN L"ukr"
// You can try to localize using gettext with .po or refer to WINAPI localization methods.

#define CURRENT_YEAR 2024

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifdef _MSC_VER
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <string>
#include <iostream>
#include <sstream>
#include <optional>
#include <iomanip>

#include <vector>
#include <set>
#include <map>
#include <unordered_map>

#include <algorithm>
#include <iterator>
#include <functional>
#include <random>

#include <filesystem>
#include <fstream>
#include <codecvt>

#include <chrono>
#include <thread>
#include <mutex>

#include <kdb/src/rlutil.h>
#include <windows.h>

using namespace std;