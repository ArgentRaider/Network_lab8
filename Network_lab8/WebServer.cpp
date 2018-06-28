#include <thread>
#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "../common.h"

#include <Windows.h>
#include <Mswsock.h>
#include "ClientQueue.h"

const int BACKLOG = 10;

using std::string;
using std::cin;

int main(){
#ifdef _DEBUG
#ifdef __GNUC__
	printf("Compiled in GCC.\n");
#endif
#ifdef _MSC_VER
	printf("Compiled in MSC.\n");
#endif
#endif // _DEBUG

	//Start up Web Server API
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if(WSAStartup(sockVersion, &wsaData) != 0){
		printf("FAILED to start up the WSA.\n");
		return 1;
	}

	//Assure the Winsock DLL supports version 2.2£º
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		printf("Invalid Winsock version!\n");
		return 1;
	}

	//Initialize the ClientQueue
	if (ClientQueue::Init()) {
#ifdef _DEBUG
		printf("ClientQueue initialized!\n");
#endif // _DEBUG
	}

	//===============================================
	//Create a socket for listening
	SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (slisten == INVALID_SOCKET) {
		WSACleanup();
		printf("ERROR for creating listening socket.\n");
		return 1;
	}

	//================================================
	//Bind the ip address of the host onto the listening socket
	SOCKADDR_IN sock_in;	// Addr of the server
	sock_in.sin_addr.S_un.S_addr = INADDR_ANY;
	sock_in.sin_port = htons(SERVER_PORT);
	sock_in.sin_family = AF_INET;
	//Bind the ip and port
	int ret = bind(slisten, (LPSOCKADDR)&sock_in, sizeof(sock_in));
	if (SOCKET_ERROR == ret ){
		closesocket(slisten);
		WSACleanup();
		printf("ERROR when binding.\n");
		return 1;
	}

	//====================================================
	//Begin to listen
	if (listen(slisten, BACKLOG) == SOCKET_ERROR) {
		closesocket(slisten);
		WSACleanup();
		printf("ERROR when listening.\n");
		return 1;
	}

	//====================================================
	// Start the thread to accept clients
	std::thread mainThread(ClientQueue::AcceptClient_t, slisten);

	// Accept commands from the administrator.
	bool quit = false;
	string cmd;
	while (!quit) {
		cin.clear();
		cin >> cmd;
		if ("quit" == cmd) {
			quit = true;
			ClientQueue::Finish();
			closesocket(slisten);
		}
		else {
			printf("Use 'quit' to close the server.(This is the only supported command, by the way.)\n");
		}
	}

	// clean up
	mainThread.join();
	WSACleanup();
	printf("Server Closed.\n");
	system("pause");
	return 0;
}
