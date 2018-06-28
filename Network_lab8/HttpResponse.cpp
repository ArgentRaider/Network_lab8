#include "HttpResponse.h"
#include <iostream>
#include <cstring>
#include <filesystem>

using std::bad_alloc;
using namespace std;
namespace fs = std::experimental::filesystem;

HttpResponse::HttpResponse(int status) : status(status), err(false), buf_ptr(0)
{
	try {
		buffer = new char[BUF_SIZE];
	}
	catch (const bad_alloc&) {
		std::cerr << "FATAL ERROR::Bad Allocation for the buffer." << std::endl;
		err = true;
	}
}


HttpResponse::~HttpResponse()
{
	if (!err) delete[] buffer;
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
	string res = string("HTTP/1.1 " + std::to_string(this->status));
	if (this->status == 200) {
		res += " OK\r\n";
	}
	else if (this->status == 404) {
		res += " Not Found\r\n";
		res += "Content-Length: 0\r\n\r\n";
	}

	if (this->status == 200) {
		res += string(buffer, buf_ptr);
	}

	return std::move(res);
}

ifstream::pos_type HttpResponse::fileSize(string filename)
{
	std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
	return in.tellg();
}

void HttpResponse::parseLoginAndPass(const char header[], const size_t headerLen, string & login, string & pass)
{
	string header_str = string(header, headerLen);
	size_t data_start = header_str.find_last_of("\r\n");

	string data = header_str.substr(data_start);
	size_t login_start = data.find_first_of("=");
	size_t login_end = data.find_first_of("&");
	size_t pass_start = data.find_last_of("=");

	login = data.substr(login_start, login_end - login_start);
	pass = data.substr(pass_start);

	cout << "data: " << data << endl;
	cout << "login: " << login << endl;
	cout << "pass: " << pass << endl;
}

void HttpResponse::GET(string route, string realpath)
{
	if (err)
	{
		this->status = 404;
		return;
	}
	ifstream fin(realpath, std::ios_base::binary);
	if (!fin.is_open())
	{
		this->status = 404;
		return;
	}
	size_t slash_pos = route.find_last_of('\\');
	string folder = route.substr(1, slash_pos - 1);
	cout << "In folder: " << folder << endl;
	int length = fileSize(realpath);
	string file_length = to_string(length);
	cout << "File length: " << file_length << endl;

	string extension = fs::path(realpath).extension().string();
	if (extension.compare(".txt") == 0)
	{
		const string content_type = "Content-Type: text/plain\r\n";
		strncpy_s(buffer + buf_ptr, BUF_SIZE - buf_ptr, content_type.c_str(), content_type.size());
		buf_ptr += content_type.size();
	}
	else if (extension.compare(".jpg") == 0)
	{
		const string content_type = "Content-Type: image/jpeg\r\n";
		strncpy_s(buffer + buf_ptr, BUF_SIZE - buf_ptr, content_type.c_str(), content_type.size());
		buf_ptr += content_type.size();
	}
	else if (extension.compare(".html") == 0)
	{
		const string content_type = "Content-Type: text/html\r\n";
		strncpy_s(buffer + buf_ptr, BUF_SIZE - buf_ptr, content_type.c_str(), content_type.size());
		buf_ptr += content_type.size();
	}
	strncpy_s(buffer + buf_ptr, BUF_SIZE - buf_ptr, "Content-Length: ", 16);
	buf_ptr += 16;
	strncpy_s(buffer + buf_ptr, BUF_SIZE - buf_ptr, file_length.c_str(), file_length.size());
	buf_ptr += file_length.size();
	strncpy_s(buffer + buf_ptr, BUF_SIZE - buf_ptr, "\r\n", 2);
	buf_ptr += 2;

	strncpy_s(buffer + buf_ptr, BUF_SIZE - buf_ptr, "\r\n", 2);
	buf_ptr += 2;
	if (buf_ptr + length > BUF_SIZE)
	{
		cout << "File too large" << endl;
	}
	fin.read(buffer + buf_ptr, length);
	buf_ptr += length;
}

void HttpResponse::POST(string route, const char header[], const size_t headerLen)
{
	if (strncmp(header, "dopost", 6) != 0)
		this->status = 404;

	cout << "in post: " << this->status << endl;
	string response_msg = "";
	string login, pass;
	bool success = false;


	if (this->status == 200)
	{
		parseLoginAndPass(header, headerLen, login, pass);
		if (login == "3150101233" && pass == "1233")
			success = true;
		if (login == "3150104435" && pass == "4435")
			success = true;

		if (success)
			response_msg = "<html><body>µÇÂ½³É¹¦</body></html>";
		else
			response_msg = "<html><body>µÇÂ½Ê§°Ü</body></html>";
	}

	string msg_length = to_string(response_msg.size());

	cout << response_msg << endl;

	strncpy_s(buffer + buf_ptr, BUF_SIZE - buf_ptr, "Content-Type: text/html\r\n", 25);
	buf_ptr += 25;
	strncpy_s(buffer + buf_ptr, BUF_SIZE - buf_ptr, "Content-Length: ", 16);
	buf_ptr += 16;
	strncpy_s(buffer + buf_ptr, BUF_SIZE - buf_ptr, msg_length.c_str(), msg_length.size());
	buf_ptr += msg_length.size();
	strncpy_s(buffer + buf_ptr, BUF_SIZE - buf_ptr, "\r\n", 2);
	buf_ptr += 2;

	strncpy_s(buffer + buf_ptr, BUF_SIZE - buf_ptr, "\r\n", 2);
	buf_ptr += 2;

	if (this->status == 200)
	{
		if (buf_ptr + response_msg.size() > BUF_SIZE)
		{
			cout << "File too large" << endl;
		}
		strncpy_s(buffer + buf_ptr, BUF_SIZE - buf_ptr, response_msg.c_str(), response_msg.size());
		buf_ptr += response_msg.size();
	}
}
