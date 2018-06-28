#pragma once
#ifndef _CLIENT_QUEUE_H
#define _CLIENT_QUEUE_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
using std::string;
#include <ctime>
#include "../common.h"
#include "WebAgent.h"

#define MAX_SOCKETS 127
const char* SERVER_NAME = "Lab8 Web server";

namespace ClientQueue {							// Organized as a Binary Tree (implemented in array)

	std::mutex connChangeMutex;			// If connection status may change, lock this mutex first.
	std::condition_variable allBusy;

	int freeNum = MAX_SOCKETS;
	bool initialized = false;			// AcceptClient_t() works only when initialized == true

	SOCKET socketArray[MAX_SOCKETS];
	SOCKADDR_IN socketAddrArray[MAX_SOCKETS];
	std::thread Threads[MAX_SOCKETS];
	std::mutex ThreadSocketMutex[MAX_SOCKETS];	// locked when send information through socket i
	unsigned int subFreeInx[MAX_SOCKETS];		// first free node in the sub tree with root i
												// subFreeInx[i] == i means node i is free

	// Constructor
	bool Init() {
		std::lock_guard<std::mutex> lock(connChangeMutex);
		if (!initialized) {
			initialized = true;
			freeNum = MAX_SOCKETS;
			for (int i = 0; i < MAX_SOCKETS; i++) {
				subFreeInx[i] = i;
			}
			return true;
		}
		return false;
	}
	// Destructor
	void Finish() {
		connChangeMutex.lock();
		initialized = false;
		for (int i = 0; i < MAX_SOCKETS; i++) {
			if (Threads[i].joinable()) {
				// Is this safe enough?
				closesocket(socketArray[i]);	// close the socket so that the thread terminates itself
				connChangeMutex.unlock();
				Threads[i].join();
				connChangeMutex.lock();
			}
		}

		freeNum = MAX_SOCKETS;
		allBusy.notify_one();		// in case the AcceptClient_t() is blocked
		connChangeMutex.unlock();
	}

	// Main THREAD Function to accept sockets
	void AcceptClient_t(SOCKET slisten);

	// Sub threads for resolving requests from clients
	void Client_t(unsigned int inx);

// Basic operations

	// It is assumed that:
	//  (1)this function will only be called by AcceptClient_t()
	//	(2)the connChangeMutex is already locked before being called
	//  (3)freeNum > 0
	// AddClient may retrieve the resource of finished threads.
	int AddClient(SOCKET sClient, SOCKADDR_IN clientAddr);

	// It is assumed that:
	//  (1)this function will only be called by Client_t()
	//	(2)the connChangeMutex is already locked before being called
	// The thread called RemoveClient() may not be joined at once.
	int RemoveClient(unsigned int inx);
};


// Main THREAD Function to accept sockets
void ClientQueue::AcceptClient_t(SOCKET slisten) {
	SOCKADDR_IN remoteAddr;
	int nAddrlen = sizeof(remoteAddr);

	std::unique_lock<std::mutex> connChange(connChangeMutex);

	while (initialized) {
		allBusy.wait(connChange, []() {return freeNum != 0; });		// this thread will block only if freeNum == 0
		if (!initialized)
			break;

		connChange.unlock();						// manually unlock the mutex since accpet could be blocked...
		printf("Waiting for connecting request...\nFree ports:%d\n", freeNum);
		SOCKET sClient = accept(slisten, (SOCKADDR *)&remoteAddr, &nAddrlen);
		connChange.lock();							// then manually lock it again...

		if (!initialized) {
			closesocket(sClient);
			break;
		}

		if (sClient == INVALID_SOCKET) {
			printf("An invalid socket received!\n");
			continue;
		}
		AddClient(sClient, remoteAddr);
	}
	connChange.unlock();
}

// Sub threads for resolving requests from clients
void ClientQueue::Client_t(unsigned int inx) {
	SOCKET sClient = socketArray[inx];
	SOCKADDR_IN sAddr = socketAddrArray[inx];

	char ipAddr[20];
	printf("Received a socket.\nIndex: %u\nIP: %s\n\n", inx, inet_ntop(AF_INET, &(sAddr.sin_addr), ipAddr, 20));

	//==================  WORK AREA ===================================
	WebAgent agent = WebAgent(sClient);
	agent.work();

	closesocket(sClient);
	//=================================================================
	std::lock_guard<std::mutex> lock(connChangeMutex);
	RemoveClient(inx);
}

int ClientQueue::AddClient(SOCKET sClient, SOCKADDR_IN clientAddr) {		// Set a node to be busy in the binary tree.
	unsigned int freeInx = subFreeInx[0];
	socketArray[freeInx] = sClient;
	socketAddrArray[freeInx] = clientAddr;
	// update the binary tree
	int node = freeInx;
	subFreeInx[node] = (node * 2 + 1 < MAX_SOCKETS) ? subFreeInx[node*2+1] : 2 * MAX_SOCKETS + 2;	// Invalid number
	if (node * 2 + 2 < MAX_SOCKETS && subFreeInx[node * 2 + 2] < subFreeInx[node])
		subFreeInx[node] = subFreeInx[node * 2 + 2];
	node = (node - 1);
	while (node >= 0) {
		node >>= 1;

		unsigned int origin, min;
		origin = subFreeInx[node];
		//if (origin == node) break;	//impossible
		min = subFreeInx[node * 2 + 1];
		if (node * 2 + 2 < MAX_SOCKETS && subFreeInx[node * 2 + 2] < min)
			min = subFreeInx[node * 2 + 2];

		if (origin == min)	//no changes
			break;
		else
			subFreeInx[node] = min;
		node = (node - 1);		// move to the parent node
	}

	freeNum--;
	if (Threads[freeInx].joinable()) {		// if it finished, collect the resourses
		Threads[freeInx].join();
	}
	Threads[freeInx] = std::thread(Client_t, freeInx);
	printf("=================================\n");
	printf("A new client socket accpeted.\n");
	printf("Free ports: %d\n", freeNum);
	printf("=================================\n");
	return freeInx;					// Useless return
}

int ClientQueue::RemoveClient(unsigned int inx) {
	subFreeInx[inx] = inx;
	int node = inx;
	while (node > 0) {
		int parent = (node - 1) >> 1;
		if (subFreeInx[parent] > subFreeInx[node])
			subFreeInx[parent] = subFreeInx[node];
		else break;
		node = parent;
	}
	freeNum++;
	printf("=================================\n");
	printf("A client closed its connection.\n");
	printf("Free ports: %d\n", freeNum);
	printf("=================================\n");
	allBusy.notify_one();	// notify the AcceptClient_t() thread if it is blocked
	return inx;				// Useless return
}


#endif