//
// Created by Joelb on 8/11/2025.
//
#pragma once

#include <winsock2.h>
#ifndef CLIENTETCP_SOCKET_THREAD_H
#define CLIENTETCP_SOCKET_THREAD_H

int connect_to_server(SOCKET sock, const char *ip, int port);
int socket_thread();
int retry_connection();

#endif //CLIENTETCP_SOCKET_THREAD_H