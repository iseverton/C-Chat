# C-Chat

## Descrição

C-Chat é uma aplicação simples de bate-papo cliente-servidor baseada em terminal, escrita em C. Ela permite que múltiplos usuários se conectem a um servidor central e troquem mensagens em tempo real através da rede local. A interface utiliza cores e formatação básica no console do Windows.

## Funcionalidades

*   **Chat Multi-Cliente:** Suporta múltiplos clientes conectados simultaneamente.
*   **Comunicação em Tempo Real:** Mensagens são enviadas e recebidas instantaneamente.
*   **Interface de Terminal Colorida:** Utiliza cores para diferenciar informações (nomes de usuário, timestamps, mensagens próprias) no console do Windows.
*   **Comandos Básicos:**
    *   `/lista`: Lista os usuários atualmente conectados ao chat.
    *   `/limpar`: Limpa a tela do console do cliente.
    *   `/sair`: Desconecta o cliente do chat.
*   **Timestamp:** Exibe o horário em que as mensagens foram enviadas/recebidas.

## Tecnologias Utilizadas

*   **Linguagem:** C
*   **Rede:** Sockets TCP/IP (utilizando a biblioteca Winsock específica do Windows).
*   **Interface:** API do Console do Windows para cores, limpeza de tela e obtenção da largura do terminal.
*   **Concorrência:** Threads do Windows (`CreateThread`) e Seções Críticas (`CRITICAL_SECTION`) para lidar com múltiplos clientes e acesso seguro a dados compartilhados no servidor.

**só pode ser compilado e executado no sistema operacional Windows** na sua forma atual.

## Componentes

1.  **Servidor (`server.c`):**
    *   Aguarda e aceita conexões de novos clientes.
    *   Gerencia a lista de clientes conectados.
    *   Recebe mensagens dos clientes.
    *   Retransmite (broadcast) as mensagens para todos os outros clientes conectados.
    *   Processa comandos como `/lista`.
2.  **Cliente (`client.c`):**
    *   Conecta-se ao servidor.
    *   Solicita um nome de usuário.
    *   Permite ao usuário enviar mensagens e comandos.
    *   Recebe e exibe mensagens do servidor (de outros usuários ou respostas a comandos) em uma thread separada.
3.  **Interface do Terminal (`terminal_ui.h`, `terminal_ui.c`):**
    *   Módulo auxiliar com funções para manipular a aparência do console (cores, linhas, título, limpeza de tela).


## Compilação (Windows)

Você precisará de um compilador C para Windows que suporte Winsock, como o MinGW (GCC) ou o Microsoft Visual C++ (MSVC).

**Usando GCC (MinGW):**

1.  **Compilar o Servidor:**
    ```bash
    gcc server.c terminal_ui.c -o server.exe -lws2_32
    ```
2.  **Compilar o Cliente:**
    ```bash
    gcc client.c terminal_ui.c -o client.exe -lws2_32
    ```

*   A flag `-lws2_32` é essencial para linkar a biblioteca Winsock.


## Execução

1.  **Inicie o Servidor:** Abra um terminal (Prompt de Comando ou PowerShell) no diretório onde `server.exe` foi compilado e execute:
    ```bash
    ./server.exe
    ```
    O servidor exibirá uma mensagem indicando que está aguardando conexões na porta 8080.

2.  **Inicie um ou mais Clientes:** Abra um novo terminal para cada cliente que deseja conectar. Navegue até o diretório onde `client.exe` foi compilado e execute:
    ```bash
    ./client.exe
    ```
3.  **Uso do Cliente:**
    *   O cliente solicitará um nome de usuário. Digite o nome desejado e pressione Enter.
    *   Após conectar, um menu de boas-vindas e comandos será exibido.
    *   Digite suas mensagens e pressione Enter para enviá-las.
    *   Mensagens de outros usuários aparecerão no seu console.
    *   Use os comandos `/lista`, `/limpar` ou `/sair` conforme necessário.