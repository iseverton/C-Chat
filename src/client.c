#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <string.h>
#include <time.h>
#include "../src/common/ui/terminal_ui.h"

#define BUFFER_SIZE 1024

SOCKET client_socket;
char UserName[40];

DWORD WINAPI receive_thread(LPVOID param);

void show_menu()
{
    printf("\nBem-vindo, ");
    print_colored(UserName, COLOR_YELLOW);
    printf("! Comandos disponiveis:\n\n");

    print_colored("/lista", COLOR_BLUE);
    printf("  -> Listar usuarios ativos\n");

    print_colored("/limpar", COLOR_BLUE);
    printf("  -> Limpar a tela\n");

    print_colored("/sair", COLOR_BLUE);
    printf("  -> Sair do chat\n\n");

    print_horizontal_line('-', COLOR_DEFAULT);
}

int main()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("Erro ao inicializar Winsock\n");
        return 1;
    }

    client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_socket == INVALID_SOCKET)
    {
        printf("Erro ao criar socket\n");
        WSACleanup();
        return 1;
    }

    clear_console();
    print_stylish_title();

    printf("Informe seu userName: ");
    fgets(UserName, sizeof(UserName), stdin);
    UserName[strcspn(UserName, "\n")] = '\0';

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
    {
        printf("Erro ao conectar no servidor\n");
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    clear_console();
    print_stylish_title();
    show_menu();

    printf("Digite sua mensagem normalmente para enviar.\n");

    HANDLE thread = CreateThread(NULL, 0, receive_thread, NULL, 0, NULL);
    if (thread == NULL)
    {
        printf("Erro ao criar thread de recebimento: %d\n", GetLastError());
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }
    CloseHandle(thread);

    char send_buffer[BUFFER_SIZE];
    char combined_buffer[BUFFER_SIZE + 50];

    while (1)
    {
        if (fgets(send_buffer, BUFFER_SIZE, stdin) != NULL)
        {
            send_buffer[strcspn(send_buffer, "\n")] = 0;
            if (strlen(send_buffer) == 0)
                continue;

            if (strcmp(send_buffer, "/limpar") == 0)
            {
                clear_console();
                print_stylish_title();
                show_menu();
                continue;
            }
            else if (strcmp(send_buffer, "/lista") == 0)
            {
                snprintf(combined_buffer, sizeof(combined_buffer), "%s|/lista", UserName);
                send(client_socket, combined_buffer, (int)strlen(combined_buffer), 0);
                continue;
            }
            else if (strcmp(send_buffer, "/sair") == 0)
            {
                printf("Saindo do chat...\n");
                break;
            }

            snprintf(combined_buffer, sizeof(combined_buffer), "%s|%s", UserName, send_buffer);

            int sent = send(client_socket, combined_buffer, (int)strlen(combined_buffer), 0);
            if (sent == SOCKET_ERROR)
            {
                printf("Erro ao enviar mensagem\n");
                break;
            }

            time_t now = time(NULL);
            struct tm *t = localtime(&now);
            char time_str[9];
            strftime(time_str, sizeof(time_str), "%H:%M:%S", t);
            print_colored("(", COLOR_DARK_GRAY);
            print_colored(time_str, COLOR_DARK_GRAY);
            print_colored(") ", COLOR_DARK_GRAY);
            print_colored("voce", COLOR_YELLOW);
            printf(": %s\n", send_buffer);
        }
    }

    closesocket(client_socket);
    WSACleanup();
    return 0;
}

DWORD WINAPI receive_thread(LPVOID param)
{
    char recv_buffer[BUFFER_SIZE];
    int bytes_received;

    while (1)
    {
        bytes_received = recv(client_socket, recv_buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0)
        {
            printf("Conexao encerrada pelo servidor.\n");
            break;
        }

        recv_buffer[bytes_received] = '\0';
        printf("%s\n", recv_buffer);
    }

    return 0;
}