#pragma once

#ifndef _COMMON_H
#define _COMMON_H
#include <string>
#include <winsock2.h>
#include <WS2tcpip.h>

#ifdef _MSC_VER
#pragma comment(lib, "ws2_32.lib")
#endif

using std::string;

const unsigned short SERVER_PORT = 4435;
const int BUF_SIZE = 2048;
const string BASE_DIR = "../";

#endif // !_COMMON_H
