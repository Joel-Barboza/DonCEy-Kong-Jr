#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define PORT 8080
#define BUFFER_SIZE 1024

static SOCKET global_socket = INVALID_SOCKET;
static int connected = 0;

int connect_to_server(SOCKET *sock, const char *ip, int port) {
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
        printf("Direccion invalida\n");
        return -1;
    }

    if (connect(*sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Conexion fallida\n");
        return -1;
    }

    printf("Conectado al servidor!\n");
    connected = 1;
    return 0;
}

int socket_thread(void *data) {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Error inicializando Winsock\n");
        return -1;
    }
#endif

    if (global_socket != INVALID_SOCKET) {
        closesocket(global_socket);
    }

    global_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (global_socket == INVALID_SOCKET) {
        printf("Error creando socket\n");
#ifdef _WIN32
        WSACleanup();
#endif
        return -1;
    }

    if (connect_to_server(&global_socket, "127.0.0.1", PORT) < 0) {
        closesocket(global_socket);
#ifdef _WIN32
        WSACleanup();
#endif
        return -1;
    }

    char buffer[BUFFER_SIZE] = {0};
    char message[BUFFER_SIZE];

    int valread = recv(global_socket, buffer, BUFFER_SIZE - 1, 0);
    if (valread > 0) {
        buffer[valread] = '\0';
        printf("Servidor: %s\n", buffer);
    }

    while (connected) {
        printf("Tu mensaje: ");
        fgets(message, BUFFER_SIZE, stdin);
        message[strcspn(message, "\n")] = 0;

        char message_with_newline[BUFFER_SIZE + 2];
        snprintf(message_with_newline, sizeof(message_with_newline), "%s\n", message);
        send(global_socket, message_with_newline, strlen(message_with_newline), 0);

        if (strcmp(message, "quit") == 0) {
            printf("Saliendo...\n");
            break;
        }

        memset(buffer, 0, BUFFER_SIZE);
        valread = recv(global_socket, buffer, BUFFER_SIZE - 1, 0);
        if (valread > 0) {
            buffer[valread] = '\0';
            printf("Servidor: %s\n", buffer);
        } else if (valread == 0) {
            printf("Servidor cerró la conexión\n");
            connected = 0;
            break;
        } else {
            printf("Error recibiendo datos\n");
            connected = 0;
            break;
        }
    }

    closesocket(global_socket);
    global_socket = INVALID_SOCKET;

#ifdef _WIN32
    WSACleanup();
#endif
    connected = 0;
    return 0;
}

// Optional helper if you want to reconnect manually (e.g., on button click)
int retry_connection() {
    if (global_socket == INVALID_SOCKET) {
        global_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (global_socket == INVALID_SOCKET) {
            printf("Error creando socket\n");
            return -1;
        }
    }

    if (connect_to_server(&global_socket, "127.0.0.1", PORT) == 0) {
        printf("Reconectado al servidor!\n");
        return 0;
    } else {
        printf("Reconexion fallida.\n");
        return -1;
    }
}
