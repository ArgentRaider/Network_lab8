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
	while (*tmp != ' ' && *tmp != 0) tmp++;
	filePath = string(hPtr, tmp - hPtr);
	
	string fulltext(header, headerLen);
	size_t argsPos = fulltext.find("\r\n\r\n");
	if (argsPos != std::string::npos) {
		args = fulltext.substr(argsPos + 4);
	}
	else {
		args = "";
	}
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

const string HttpRequest::operator[](const string & attr)
{
	string query = attr + "=";
	string result;
	size_t pos = 0, endPos = 0;
	if ((pos = args.find(query)) == 0) {
		pos += query.size();
	}
	else if ((pos = args.find("&" + query) != std::string::npos)) {
		pos += query.size() + 1;
	}

	if (pos > 0) {
		endPos = args.substr(pos).find('&');
		result = args.substr(pos, endPos);
	}
	else {
		result = "";
	}

	return result;
}

