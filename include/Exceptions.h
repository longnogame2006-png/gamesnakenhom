#pragma once
#include <stdexcept>
struct FileIOException : public std::runtime_error { using std::runtime_error::runtime_error; };
struct GameStateException : public std::runtime_error { using std::runtime_error::runtime_error; };
