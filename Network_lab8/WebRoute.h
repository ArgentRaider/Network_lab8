#pragma once
#include <string>
#include "../common.h"

using std::string;

class WebRoute
{
public:
	static string route(const string& path);
private:
	WebRoute();
	~WebRoute();
};

