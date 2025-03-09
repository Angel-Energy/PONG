#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

#define WIDTH 80
#define HEIGHT 25

// Глобальные переменные
int paddle1_y = HEIGHT / 2 - 1;
int paddle2_y = HEIGHT / 2 - 1;
int ball_x = WIDTH / 2;
int ball_y = HEIGHT / 2;
int ball_dx = 1;
int ball_dy = 1;
int score1 = 0, score2 = 0;

// ASCII-графика для цифр
const char *digits[10][5] = {
    {"███", "█ █", "█ █", "█ █", "███"}, // 0
    {"  █", "  █", "  █", "  █", "  █"}, // 1
    {"███", "  █", "███", "█  ", "███"}, // 2
    {"███", "  █", "███", "  █", "███"}, // 3
    {"█ █", "█ █", "███", "  █", "  █"}, // 4
    {"███", "█  ", "███", "  █", "███"}, // 5
    {"███", "█  ", "███", "█ █", "███"}, // 6
    {"███", "  █", "  █", "  █", "  █"}, // 7
    {"███", "█ █", "███", "█ █", "███"}, // 8
    {"███", "█ █", "███", "  █", "███"}  // 9
};

// Включение необработанного режима терминала
void enable_raw_mode() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
}

// Выключение raw-режима перед выходом
void disable_raw_mode() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

// Функция для очистки экрана (перемещение курсора)
void clear_screen() {
    printf("\033[H\033[J");
}

// Отрисовка ASCII-цифр счета
void draw_score(int x, int y, int score) {
    char buffer[3];
    snprintf(buffer, sizeof(buffer), "%d", score); // Преобразуем число в строку

    for (int row = 0; row < 5; row++) {
        printf("\033[%d;%dH", y + row, x); // Перемещаем курсор
        for (int i = 0; buffer[i] != '\0'; i++) {
            int digit = buffer[i] - '0'; // Получаем цифру
            printf("%s ", digits[digit][row]);
        }
    }
}

// Функция для отрисовки игрового поля
void draw_field() {
    printf("\033[H"); // Перемещаем курсор в начало

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (x == 0 || x == WIDTH - 1) {
                printf("|");
            } else if (y == ball_y && x == ball_x) {
                printf("O");
            } else if (x == 1 && (y >= paddle1_y && y < paddle1_y + 3)) {
                printf("|");
            } else if (x == WIDTH - 2 && (y >= paddle2_y && y < paddle2_y + 3)) {
                printf("|");
            } else if (x == WIDTH / 2 && y % 4 == 0) {
                printf("|");
            } else {
                printf(" ");
            }
        }
        printf("\n");
    }

    // Отрисовка ASCII-счета
    draw_score(30, 2, score1);
    draw_score(50, 2, score2);

    fflush(stdout);
}

// Функция обновления игры
void update_game() {
    ball_x += ball_dx;
    ball_y += ball_dy;

    if (ball_y <= 0 || ball_y >= HEIGHT - 1) {
        ball_dy = -ball_dy;
    }

    if (ball_x == 2 && ball_y >= paddle1_y && ball_y < paddle1_y + 3) {
        ball_dx = -ball_dx;
    }
    if (ball_x == WIDTH - 3 && ball_y >= paddle2_y && ball_y < paddle2_y + 3) {
        ball_dx = -ball_dx;
    }

    if (ball_x <= 0) {
        score2++;
        ball_x = WIDTH / 2;
        ball_y = HEIGHT / 2;
        ball_dx = -ball_dx;
    }
    if (ball_x >= WIDTH - 1) {
        score1++;
        ball_x = WIDTH / 2;
        ball_y = HEIGHT / 2;
        ball_dx = -ball_dx;
    }
}

// Функция обработки ввода
void process_input() {
    char input;
    while (read(STDIN_FILENO, &input, 1) > 0) {
        if (input == 'a' && paddle1_y > 0) paddle1_y--;
        if (input == 'z' && paddle1_y < HEIGHT - 3) paddle1_y++;
        if (input == 'k' && paddle2_y > 0) paddle2_y--;
        if (input == 'm' && paddle2_y < HEIGHT - 3) paddle2_y++;
        if (input == 'q') exit(0);
    }
}

// Главная функция
int main() {
    enable_raw_mode();
    clear_screen();
    
    while (1) {
        process_input();
        update_game();
        draw_field();
        usleep(100000);
    }

    disable_raw_mode();
    return 0;
}
