#pragma once

#include <string>
#include <fstream>
#include "../common.h"
using std::string;

class HttpResponse
{
public:
	HttpResponse(int status = 200);
	~HttpResponse();
	int getStatusCode();
	void setStatusCode(const int& status);
	const string toString();
	void GET(string route, string realpath);
	void POST(string route, const char header[], const size_t headerLen);

private:
	const int BUF_SIZE = 65535;
	int status;
	char *buffer;
	size_t buf_ptr;
	bool err;
	std::ifstream::pos_type fileSize(string filename);
	void parseLoginAndPass(const char header[], const size_t headerLen, string & login, string & pass);
};

