#include "tack-string.h"
#include "tty.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

static void tty_set_size(TTY *tty) {
  struct winsize ws;
  ioctl(tty->fd, TIOCGWINSZ, &ws);
  tty->rows = ws.ws_row;
  tty->columns = ws.ws_col;
}

static void tty_set_state(TTY *tty) {
  struct termios *state = malloc(sizeof(struct termios));
  tcgetattr(tty->fd, state);
  tty->original_state = state;
  struct termios new_state;
  memcpy(&new_state, state, sizeof(struct termios));
  cfmakeraw(&new_state);
  new_state.c_lflag &= ~(ECHO | ICANON);
  tcsetattr(tty->fd, TCSANOW, &new_state);
}

TTY *tty_new() {
  TTY *tty = malloc(sizeof(TTY));
  tty->read_buf = calloc(2, sizeof(unsigned char));
  tty->read_buf[1] = '\0';
  tty->fd = open("/dev/tty", O_RDWR);
  tty_set_size(tty);
  tty_set_state(tty);
  return tty;
}

void tty_teardown_and_free(TTY *tty) {
  if (tty == NULL) return;
  tcsetattr(tty->fd, TCSANOW, tty->original_state);
  close(tty->fd);
  free(tty->read_buf);
  free(tty->original_state);
  free(tty);
}

void tty_write(TTY *tty, String *s) {
  write(tty->fd, s->buf, s->length);
}

unsigned char tty_read_char(TTY *tty) {
  read(tty->fd, tty->read_buf, 1);
  return tty->read_buf[0];
}
