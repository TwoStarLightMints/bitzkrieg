#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define CLEAR ' '

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

// Used to represent directions
enum DIRECTIONS {
  UP,
  DOWN,
  RIGHT,
  LEFT,
  UNINIT,
};

// Stores the body of the player
enum PLAYER_STATE {
  UP_OR = 'A',
  DOWN_OR = 'v',
  RIGHT_OR = '>',
  LEFT_OR = '<',
};

// Stores the different cell types as labels
enum CELL_TYPE {
  ENTITY,
  WALL,
  SPACE,
  DENSE,
};

// Used to couple the dimensions of the terminal
struct dimension {
  int width;
  int height;
};

// Used to represent a point in a cartesian plane
struct point {
  int x;
  int y;
};

// Dummy type to use in Cell type union
typedef struct Wall {
  char color;
} Wall;

typedef struct Space {
  char color;
} Space;

typedef struct Entity {
  char body;
  enum DIRECTIONS orientation;
} Entity;

typedef struct Cell {
  struct point pos;
  enum CELL_TYPE cell_type;
  union {
    Entity e;
    Wall w;
    Space s;
  };
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



void place_cell(Cell c, Cell* board, struct dimension bounds) {
  board[from_point(c.pos, bounds)] = c;
}



Cell create_cell(struct point pos, enum CELL_TYPE cell_type, char graphic) {
  Cell cell; 

  if (cell_type == WALL) {

    cell.pos = pos;
    cell.cell_type = cell_type;
    cell.w.color = graphic;

  } else if (cell_type == ENTITY) {

    cell.pos = pos;
    cell.cell_type = cell_type;
    cell.e.body = graphic;

  } else if (cell_type == SPACE) {

    cell.pos = pos;
    cell.cell_type = cell_type;
    cell.s.color = graphic;

  }

  return cell;
}



Cell* create_board(struct dimension bounds) {
  const int SIZE = bounds.height * bounds.width;

  Cell* board = malloc(sizeof(Cell) * SIZE);

  for (int i = 0; i < SIZE; i++) {

    // These are the bounds of the screen
    if (i % bounds.width == 0 || i / bounds.width == 0 || i % bounds.width == (bounds.width - 1) ||  i / bounds.width == (bounds.height - 1)) {

      board[i] = create_cell(to_point(i, bounds), WALL, '#');

    } else {

      board[i] = create_cell(to_point(i, bounds), SPACE, CLEAR);

    }
  }

  return board;
}



void print_board(const Cell* board, struct dimension bounds) {
  for (int i = 0; i < bounds.width * bounds.height; i++) {
    const Cell curr = board[i];
    switch (curr.cell_type) {
      case SPACE:
        printf("%c", curr.s.color);
        break;
      case ENTITY:
        printf("%c", curr.e.body);
        break;
      case WALL:
        printf("%c", curr.w.color);
        break;
      default:
        goto CELL_TYPE_ERR;
    }
    if ((i + 1) % bounds.width == 0)
      printf("\n");
  }

  return;

  CELL_TYPE_ERR:
    printf("Error: Invalid cell type passed\n");
    exit(1);
}



int get_move_direction() {
  int pref1 = getch();
  int pref2 = getch();

  int value = getch();

  switch (value) {
    case 65:
      return UP;
    case 66:
      return DOWN;
    case 67:
      return RIGHT;
    case 68:
      return LEFT;
    default:
      return -1;
  }
}

Cell move_player(Cell player) {
    switch (get_move_direction()) {
      case 0:
        player.pos.y -= 1;
        player.e.body = UP_OR;
        player.e.orientation = UP;
        break;
      case 1:
        player.pos.y += 1;
        player.e.body = DOWN_OR;
        player.e.orientation = DOWN;
        break;
      case 2:
        player.pos.x += 1;
        player.e.body = RIGHT_OR;
        player.e.orientation = RIGHT;
        break;
      case 3:
        player.pos.x -= 1;
        player.e.body = LEFT_OR;
        player.e.orientation = LEFT;
        break;
    }

  return player;
}



int main(int argc, char** argv) {
  srand(time(NULL));
  struct dimension bounds = get_dimensions();

  Cell* board = create_board(bounds);

  Cell player = { {bounds.width / 2, bounds.height / 2}, ENTITY, .e = { 'A', UNINIT } };

  board[from_point(player.pos, bounds)] = player;

  print_board(board, bounds);

  free(board);

  return 0;
}
