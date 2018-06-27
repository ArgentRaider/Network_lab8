#include "HttpResponse.h"



HttpResponse::HttpResponse(int status) : status(status)
{
}


HttpResponse::~HttpResponse()
{
}

int HttpResponse::getStatusCode()
{
	return this->status;
}

void HttpResponse::setStatusCode(const int & status)
{
	this->status = status;
}

const string HttpResponse::toString()
{
	string res = string("HTTP/1.1 " + this->status);
	if (this->status == 200) {
		res += " OK";
	}
	else if (this->status == 404) {
		res += " Not Found";
	}
	res += "\r\n\r\n";
	return std::move(res);
}
