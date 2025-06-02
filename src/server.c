#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "../src/common/config.h"
#include "../src/common/ui/terminal_ui.h"

#pragma comment(lib, "Ws2_32.lib")

#define BUFFER_SIZE 1024

typedef struct
{
    SOCKET socket;
    char name[40];
} ClientInfo;

ClientInfo clients[MAX_CLIENTS];
int clients_count = 0;

CRITICAL_SECTION clients_lock;

DWORD WINAPI handle_client(LPVOID client_socket_ptr);
void broadcast_message(const char *formatted_msg, SOCKET sender);
void remove_client(SOCKET client_socket);
void send_active_users(SOCKET requester);

int main()
{
    // verifica se a biblioteca inicializou corretamente
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("Erro ao inicializar Winsock\n");
        return 1;
    }

    InitializeCriticalSection(&clients_lock);

    // cria o socket do servidor
    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket == INVALID_SOCKET)
    {
        printf("Erro ao criar socket\n");
        return 1;
    }

    // configura o endereco que vai ser usado pelo socket
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    memset(server_addr.sin_zero, 0, 8);

    // associa o endereco ao socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
    {
        printf("Erro no bind\n");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    // socket comeca a escutar as tentativas de conexoes
    if (listen(server_socket, MAX_PENDING_CONNECTIONS) == SOCKET_ERROR)
    {
        printf("Erro ao escutar\n");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }
    printf("Servidor aguardando conexoes na porta %d...\n", SERVER_PORT);

    struct sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);

    while (1)
    {
        // aceita a conexao do cliente e cria um novo socket especifico para aquele cliente
        SOCKET client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket == INVALID_SOCKET)
        {
            printf("Erro no accept: %d\n", WSAGetLastError());
            continue;
        }

        printf("Novo cliente conectado: %s:%d\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        EnterCriticalSection(&clients_lock);
        if (clients_count < MAX_CLIENTS)
        {
            clients[clients_count].socket = client_socket;
            clients[clients_count].name[0] = '\0';
            clients_count++;
        }
        else
        {
            printf("Numero maximo de clientes atingido\n");
            closesocket(client_socket);
            LeaveCriticalSection(&clients_lock);
            continue;
        }
        LeaveCriticalSection(&clients_lock);

        // cria um thread para lidar com cada cliente
        HANDLE thread = CreateThread(NULL, 0, handle_client, (LPVOID)client_socket, 0, NULL);

        if (thread == NULL)
        {
            printf("Erro ao criar thread: %d\n", GetLastError());
            closesocket(client_socket);
            EnterCriticalSection(&clients_lock);
            remove_client(client_socket);
            LeaveCriticalSection(&clients_lock);
        }
        else
        {
            CloseHandle(thread);
        }
    }

    DeleteCriticalSection(&clients_lock);
    closesocket(server_socket);
    WSACleanup();
    return 0;
}

// recebe mensagens do cliente
DWORD WINAPI handle_client(LPVOID client_socket_ptr)
{
    SOCKET client_socket = (SOCKET)client_socket_ptr;
    char buffer[BUFFER_SIZE];
    int bytes_received;
    char name[40] = {0};

    while (1)
    {
        memset(buffer, 0, sizeof(buffer));
        bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);

        if (bytes_received <= 0)
        {
            printf("Cliente %s desconectou\n", name[0] ? name : "desconhecido");
            break;
        }

        buffer[bytes_received] = '\0';

        char *sep = strchr(buffer, '|');
        if (sep != NULL)
        {
            *sep = '\0';
            char *recv_name = buffer;
            char *message = sep + 1;

            if (name[0] == '\0')
            {
                strncpy(name, recv_name, sizeof(name) - 1);
                name[sizeof(name) - 1] = '\0';

                EnterCriticalSection(&clients_lock);
                for (int i = 0; i < clients_count; i++)
                {
                    if (clients[i].socket == client_socket)
                    {
                        strncpy(clients[i].name, name, sizeof(clients[i].name) - 1);
                        clients[i].name[sizeof(clients[i].name) - 1] = '\0';
                        break;
                    }
                }
                LeaveCriticalSection(&clients_lock);
            }

            if (strcmp(message, "!list") == 0 || strcmp(message, "/lista") == 0)
            {
                send_active_users(client_socket);
            }
            else
            {
                time_t now = time(NULL);
                struct tm *t = localtime(&now);
                char time_str[9];
                strftime(time_str, sizeof(time_str), "%H:%M:%S", t);

                char formatted_msg[BUFFER_SIZE + 150];
                snprintf(formatted_msg, sizeof(formatted_msg),
                         "[%s%s%s] %s: %s",
                         ANSI_COLOR_GRAY, time_str, ANSI_COLOR_RESET,
                         name, message);
                printf("%s\n", formatted_msg);
                broadcast_message(formatted_msg, client_socket);
            }
        }
    }

    EnterCriticalSection(&clients_lock);
    remove_client(client_socket);
    LeaveCriticalSection(&clients_lock);

    closesocket(client_socket);
    return 0;
}

// envia a mensagem para todos os clientes conectados, menos para o cliente que enviou a mensagem original
void broadcast_message(const char *formatted_msg, SOCKET sender)
{
    EnterCriticalSection(&clients_lock);
    for (int i = 0; i < clients_count; i++)
    {
        SOCKET sock = clients[i].socket;
        if (sock != sender)
        {
            int sent = send(sock, formatted_msg, (int)strlen(formatted_msg), 0);
            if (sent == SOCKET_ERROR)
            {
                printf("Erro ao enviar para cliente %s: %d\n", clients[i].name, WSAGetLastError());
            }
        }
    }
    LeaveCriticalSection(&clients_lock);
}

// remove o cliente da lista
void remove_client(SOCKET client_socket)
{
    for (int i = 0; i < clients_count; i++)
    {
        if (clients[i].socket == client_socket)
        {
            printf("Removendo cliente: %s\n", clients[i].name[0] ? clients[i].name : "desconhecido");
            clients[i] = clients[clients_count - 1];
            clients_count--;
            break;
        }
    }
}

// lista de usuarios conectados
void send_active_users(SOCKET requester)
{
    char user_list[BUFFER_SIZE];
    user_list[0] = '\0';

    EnterCriticalSection(&clients_lock);
    strcat(user_list, "Usuarios ativos:\n");
    for (int i = 0; i < clients_count; i++)
    {
        if (clients[i].name[0] != '\0')
        {
            strcat(user_list, " - ");
            strcat(user_list, clients[i].name);
            strcat(user_list, "\n");
        }
    }
    LeaveCriticalSection(&clients_lock);

    send(requester, user_list, (int)strlen(user_list), 0);
}