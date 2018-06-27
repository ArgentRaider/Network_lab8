#pragma once

#include <string>
using std::string;

class HttpRequest
{
public:
	const static int GET;
	const static int POST;
	HttpRequest(const char header[], const size_t headerLen);
	~HttpRequest();

	const int Method();
	const string& FilePath();

private:
	int method;
	string filePath;
};

