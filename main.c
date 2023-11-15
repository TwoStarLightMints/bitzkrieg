#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

struct wh {
  int width;
  int height;
};

struct point {
  int x;
  int y;
};

struct wh get_dimensions() {
  struct winsize w;

  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

  struct wh wh = { w.ws_row, w.ws_col };

  return wh;
}

struct point to_point(int input, struct wh bounds) {
  printf("TODO");

  struct point p;

  return p;
}

// char* create_board(struct wh wh) {
//   const int SIZE = wh.height * wh.width;
//   char* board = malloc(SIZE);

//   for (int i = 0; i < SIZE; i++)
//     board[i] = '*';

  

//   for (int i = 0; i < wh.width; i++) {
//     for (int j = 0; j < wh.height; j++) {
//       if (wh.width / 2 == i && wh.height / 2 == j)
//         printf("%c", board[i][j]);
//       else
//         printf(" ");
//     }

//     printf("\n");
//   }

//   return board;
// }

int main(int argc, char** argv) {
  struct wh wh = get_dimensions();


  
  return 0;
}
