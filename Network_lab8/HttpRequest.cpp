#include <iostream>
#include <filesystem>
#include <cstring>
#include "HttpRequest.h"

namespace fs = std::experimental::filesystem;

const int HttpRequest::GET = 0;
const int HttpRequest::POST = 1;

HttpRequest::HttpRequest(const char header[], const size_t headerLen)
{
	const char* hPtr = header;
	if (std::strncmp(hPtr, "GET", 3) == 0) {
		this->method = HttpRequest::GET;
		hPtr += 3 + 1;
	}
	else if (std::strncmp(hPtr, "POST", 4) == 0) {
		this->method = HttpRequest::POST;
		hPtr += 4 + 1;
	}
	else {
		this->method = -1;
		while (*hPtr++ != ' ');
	}
	const char* tmp = hPtr;
	while (*tmp != ' ' && *tmp != 0)tmp++;
	filePath = string(hPtr, tmp - hPtr);
	filePath = fs::path(filePath).string();
}


HttpRequest::~HttpRequest()
{
}

const int HttpRequest::Method()
{
	return this->method;
}

const string & HttpRequest::FilePath()
{
	return this->filePath;
}

