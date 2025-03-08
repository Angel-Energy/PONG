#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>  // Добавлено для поддержки локали

#define WIDTH 80
#define HEIGHT 25
#define PADDLE_SIZE 3

// Структура для хранения состояния игры
typedef struct {
    int paddle1_y, paddle2_y;  // Позиции ракеток
    int ball_x, ball_y;        // Позиция мяча
    int ball_dir_x, ball_dir_y; // Направление мяча
    int score1, score2;        // Счет игроков
} GameState;

// Инициализация игры
void initialize(GameState *state) {
    state->paddle1_y = state->paddle2_y = HEIGHT / 2 - PADDLE_SIZE / 2;  // Ракетки по центру
    state->ball_x = WIDTH / 2;  // Мяч по центру
    state->ball_y = HEIGHT / 2;
    state->ball_dir_x = 1;  // Направление мяча
    state->ball_dir_y = 1;
    state->score1 = state->score2 = 0;  // Счет
}

// Отрисовка игрового поля
void draw(const GameState *state) {
    clear();  // Очистка экрана

    // Верхняя граница
    for (int i = 0; i < WIDTH + 2; i++)
        printw("-");
    printw("\n");

    // Игровое поле
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (x == 0 || x == WIDTH - 1)
                printw("|");  // Боковые границы
            else if (x == state->ball_x && y == state->ball_y)
                printw("O");  // Мяч
            else if (x == 1 && y >= state->paddle1_y && y < state->paddle1_y + PADDLE_SIZE)
                printw("|");  // Левая ракетка
            else if (x == WIDTH - 2 && y >= state->paddle2_y && y < state->paddle2_y + PADDLE_SIZE)
                printw("|");  // Правая ракетка
            else
                printw(" ");  // Пустое пространство
        }
        printw("\n");
    }

    // Нижняя граница
    for (int i = 0; i < WIDTH + 2; i++)
        printw("-");
    printw("\n");

    // Счет
    mvprintw(HEIGHT + 1, 0, "Счет: Игрок 1: %d | Игрок 2: %d", state->score1, state->score2);
    mvprintw(HEIGHT + 2, 0, "Управление: A/Z (Игрок 1), K/M (Игрок 2)");
    refresh();  // Обновление экрана
}

// Обработка ввода
void input(GameState *state) {
    int key = getch();  // Чтение клавиши
    switch (key) {
        case 'a': if (state->paddle1_y > 0) state->paddle1_y--; break;  // Левая ракетка вверх
        case 'z': if (state->paddle1_y < HEIGHT - PADDLE_SIZE) state->paddle1_y++; break;  // Левая ракетка вниз
        case 'k': if (state->paddle2_y > 0) state->paddle2_y--; break;  // Правая ракетка вверх
        case 'm': if (state->paddle2_y < HEIGHT - PADDLE_SIZE) state->paddle2_y++; break;  // Правая ракетка вниз
    }
}

// Обновление состояния игры
void update(GameState *state) {
    state->ball_x += state->ball_dir_x;  // Движение мяча по горизонтали
    state->ball_y += state->ball_dir_y;  // Движение мяча по вертикали

    // Столкновение с верхней и нижней границами
    if (state->ball_y <= 0 || state->ball_y >= HEIGHT - 1)
        state->ball_dir_y *= -1;

    // Столкновение с ракетками
    if (state->ball_x == 1 && state->ball_y >= state->paddle1_y && state->ball_y < state->paddle1_y + PADDLE_SIZE)
        state->ball_dir_x *= -1;
    if (state->ball_x == WIDTH - 2 && state->ball_y >= state->paddle2_y && state->ball_y < state->paddle2_y + PADDLE_SIZE)
        state->ball_dir_x *= -1;

    // Гол
    if (state->ball_x <= 0) {
        state->score2++;  // Игрок 2 забил гол
        state->ball_x = WIDTH / 2;
        state->ball_y = HEIGHT / 2;
    }
    if (state->ball_x >= WIDTH - 1) {
        state->score1++;  // Игрок 1 забил гол
        state->ball_x = WIDTH / 2;
        state->ball_y = HEIGHT / 2;
    }
}

// Основная функция
int main() {
    setlocale(LC_ALL, "");  // Устанавливаем локаль на UTF-8
    initscr();  // Инициализация ncurses
    cbreak();   // Отключаем буферизацию ввода
    noecho();   // Отключаем отображение ввода
    nodelay(stdscr, TRUE);  // Неблокирующий ввод
    keypad(stdscr, TRUE);   // Включаем обработку специальных клавиш

    GameState state;
    initialize(&state);  // Инициализация игры

    while (1) {
        draw(&state);  // Отрисовка
        input(&state);  // Ввод
        update(&state);  // Обновление

        // Проверка на победу
        if (state.score1 == 21 || state.score2 == 21) {
            clear();
            mvprintw(0, 0, "Игрок %d выиграл!\n", (state.score1 == 21) ? 1 : 2);
            refresh();
            break;
        }

        napms(100);  // Задержка 100 мс
    }

    endwin();  // Завершение работы с ncurses
    return 0;
}