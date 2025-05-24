# Projeto: C-chat
## Chat em Grupo em linguagem C 

Este projeto implementa um sistema simples de chat em grupo utilizando a linguagem C e a API Winsock para comunicação em rede no ambiente Windows. Ele segue o modelo cliente-servidor, permitindo que múltiplos clientes se conectem a um servidor central para trocar mensagens em tempo real.

## Arquitetura

O sistema é composto por dois componentes principais:

1.  **Servidor (`server.c`)**: 
    *   Aguarda conexões de clientes em uma porta específica (padrão: 8080).
    *   Utiliza TCP/IP para comunicação confiável.
    *   Gerencia múltiplos clientes simultaneamente usando threads (uma thread por cliente).
    *   Recebe mensagens de um cliente e as retransmite (broadcast) para todos os outros clientes conectados.
    *   Notifica os clientes quando um novo usuário entra ou sai do chat.
    

2.  **Cliente (`client.c`)**: 
    *   Conecta-se ao servidor fornecendo o endereço IP e a porta.
    *   Solicita um nome de usuário ao iniciar.
    *   Envia mensagens digitadas pelo usuário para o servidor.
    *   Possui uma thread dedicada para receber mensagens do servidor (mensagens de outros usuários ou notificações do sistema) e exibi-las no console.
    *   Permite ao usuário desconectar digitando `/quit`.

## Funcionalidades

*   Chat em grupo com múltiplos usuários.
*   Comunicação baseada em TCP/IP.
*   Notificações de entrada e saída de usuários.
*   Interface de linha de comando simples.

## Pré-requisitos

*   Sistema Operacional Windows.
*   Compilador C (como MinGW/GCC ou o compilador do Visual Studio).
*   Biblioteca Winsock 2 (`ws2_32.lib`) - geralmente incluída com os compiladores para Windows.
