#pragma once
#include "../common.h"


// WebAgent is not responsible for closing the client.
class WebAgent
{
public:
	WebAgent(SOCKET client);
	~WebAgent();

	void work();

private:
	SOCKET client;
	char* buffer;
	bool err;
};

