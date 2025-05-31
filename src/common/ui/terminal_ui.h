#ifndef TERMINAL_UI_H
#define TERMINAL_UI_H

// Cores ANSI para o terminal (texto colorido)
typedef enum
{
    COLOR_DEFAULT = 7, // Cinza claro padrão
    COLOR_DARK_GRAY = 8,
    COLOR_RED = FOREGROUND_RED | FOREGROUND_INTENSITY,
    COLOR_GREEN = FOREGROUND_GREEN | FOREGROUND_INTENSITY,
    COLOR_YELLOW = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
    COLOR_BLUE = FOREGROUND_BLUE | FOREGROUND_INTENSITY,
    COLOR_MAGENTA = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
    COLOR_CYAN = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
    COLOR_WHITE = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
} ConsoleColor;

#define ANSI_COLOR_GRAY "\x1b[90m"
#define ANSI_COLOR_RESET "\x1b[0m"
void set_console_text_color(ConsoleColor color);
void reset_console_text_color();

void print_colored(const char *text, ConsoleColor color);

// Imprime título estilizado no console, centralizado e com linhas de '*'
void print_stylish_title();

// Limpa o console (Windows)
void clear_console();

int get_console_width();
void print_horizontal_line(char ch, ConsoleColor color);

#endif
