#include "WebAgent.h"
#include <new>
#include <iostream>
#include <cstring>
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "WebRoute.h"
using std::bad_alloc;

WebAgent::WebAgent(SOCKET client):client(client), err(false)
{
	try {
		buffer = new char[BUF_SIZE];
	}
	catch (const bad_alloc&) {
		std::cerr << "FATAL ERROR::Bad Allocation for the buffer." << std::endl;
		err = true;
	}

}

WebAgent::~WebAgent()
{
	if (!err) delete[] buffer;
}

void WebAgent::work()
{
	if (err) return;

	int ret;
	while (true) {
		ret = recv(client, buffer, BUF_SIZE, 0);
		if (ret == SOCKET_ERROR || ret == 0) {		// exit the loop
			break;
		}
		if (std::strncmp(buffer + ret - 4, "\r\n\r\n", 4) == 0) {
			char* firstLineEnd = buffer;
			while (*firstLineEnd != '\r') firstLineEnd++;
			string firstLine = string(buffer, firstLineEnd - buffer);
			std::cout << firstLine << std::endl;

			HttpRequest request = HttpRequest(buffer, ret);
			string filePath = request.FilePath();
			string realPath = WebRoute::route(filePath);
			HttpResponse response = HttpResponse();
			if (request.Method() == HttpRequest::GET)
				response.GET(filePath, realPath);
			else if (request.Method() == HttpRequest::POST)
			{
				std::cout << "POST" << std::endl;
				response.POST(filePath, buffer, ret);
			}
			string responseText = response.toString();
			ret = send(client, responseText.c_str(), responseText.size(), 0);
		}
	}
}
