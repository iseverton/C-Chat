#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "terminal_ui.h"

// cores
void set_console_text_color(ConsoleColor color)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void reset_console_text_color()
{
    set_console_text_color(COLOR_DEFAULT);
}

void print_colored(const char *text, ConsoleColor color)
{
    set_console_text_color(color);
    printf("%s", text);
    reset_console_text_color();
}

// Obtem a largura do console (ou 80 se falhar)
int get_console_width()
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int width = 80; // fallback padrão

    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
    {
        width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }

    return width;
}

// Imprime uma linha horizontal cheia com o caractere fornecido
void print_horizontal_line(char ch, ConsoleColor color)
{
    int width = get_console_width();
    set_console_text_color(color);
    for (int i = 0; i < width; i++)
        putchar(ch);
    putchar('\n');
    reset_console_text_color();
}

void print_stylish_title()
{
    int width = get_console_width();

    const char *title = "C - C H A T";
    int title_len = (int)strlen(title);

    // Linha superior
    print_horizontal_line('*', COLOR_BLUE);

    // Espaços para centralizar o título
    int padding_left = (width - title_len) / 2;
    if (padding_left < 0)
        padding_left = 0;

    for (int i = 0; i < padding_left; i++)
        putchar(' ');

    // Imprime o título
    printf("%s\n", title);

    // Linha inferior
    print_horizontal_line('*', COLOR_BLUE);
}

// Limpa o console no Windows
void clear_console()
{
    system("cls");
}
