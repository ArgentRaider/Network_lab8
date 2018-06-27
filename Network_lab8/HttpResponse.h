#pragma once

#include <string>
using std::string;

class HttpResponse
{
public:
	HttpResponse(int status = 200);
	~HttpResponse();
	int getStatusCode();
	void setStatusCode(const int& status);
	const string toString();

private:
	int status;
};

