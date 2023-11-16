#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

// Used to get unbuffered character input from user, solution found from:
// https://stackoverflow.com/questions/421860/capture-characters-from-standard-input-without-waiting-for-enter-to-be-pressed
char getch() {
  char buf = 0;

  struct termios old = {0};

  if (tcgetattr(0, &old))
    perror("tcsetattr()");

  old.c_lflag &= ~ICANON;
  old.c_lflag &= ~ECHO;
  old.c_cc[VMIN] = 1;
  old.c_cc[VTIME] = 0;

  if (tcsetattr(0, TCSANOW, &old) < 0)
    perror("tcsetattr ICANON");

  if (read(0, &buf, 1) < 0)
    perror("read()");

  old.c_lflag |= ICANON;
  old.c_lflag |= ECHO;

  if (tcsetattr(0, TCSADRAIN, &old) < 0)
    perror("tcsettattr ~ICANON");

  return buf;
}

struct dimension {
  int width;
  int height;
};

struct point {
  int x;
  int y;
};

typedef struct Cell {
  struct point pos;
  bool is_filled;
  char color;
} Cell;



struct dimension get_dimensions() {
  struct winsize w;

  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

  // At this time row - 2 provides enough separation from the sides of the terminal window as to be visually pleasing
  struct dimension wh = { w.ws_col, w.ws_row - 2 };

  return wh;
}



struct point to_point(int input, struct dimension bounds) {
  struct point p = { input % bounds.width, input / bounds.width };

  return p;
}



int from_point(struct point p, struct dimension bounds) {
  return (p.y * bounds.width) + p.x;
}



Cell* create_board(struct dimension bounds) {
  const int SIZE = bounds.height * bounds.width;

  Cell* board = malloc(sizeof(Cell) * SIZE);

  for (int i = 0; i < SIZE; i++) {

    // These are the bounds of the screen
    if (i % bounds.width == 0 || i / bounds.width == 0 || i % bounds.width == (bounds.width - 1) ||  i / bounds.width == (bounds.height - 1)) {

      struct point p = to_point(i, bounds);
      Cell c = { p, true, '#' };
      board[i] = c;

    } else {

      struct point p = to_point(i, bounds);
      Cell c = { p, false, ' ' };
      board[i] = c;

    }
  }

  return board;
}



int main(int argc, char** argv) {
  srand(time(NULL));
  struct dimension bounds = get_dimensions();

  Cell* board = create_board(bounds);

  Cell player = { { (rand() % (bounds.width - 1)) + 1, (rand() % (bounds.height - 1)) + 1 }, true, '&' };

  board[from_point(player.pos, bounds)] = player;

  while (1) {
    printf("\033[2J");

    for (int i = 0; i < bounds.height * bounds.width; i++) {
      printf("%c", board[i].color);
      if ((i + 1) % bounds.width == 0) {
        printf("\n");
      }
    }

    Cell clear = { player.pos, false, ' ' };

    board[from_point(player.pos, bounds)] = clear;

    player.pos.x = (rand() % (bounds.width - 1)) + 1;
    player.pos.y = (rand() % (bounds.height - 1)) + 1;

    board[from_point(player.pos, bounds)] = player;

    sleep(1);
  }

  return 0;
}
