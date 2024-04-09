#pragma once
#include <vector>
#include <string>

enum ErrorKind {
	SDL,
	OTHER
};
void logErr(ErrorKind kind, std::string message);