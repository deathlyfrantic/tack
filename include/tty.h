#ifndef TACK_TTY_H
#define TACK_TTY_H

#include <stdint.h>
#include <stdio.h>
#include <termios.h>

#define CTRL_KEY(k) (k & 0x1f)

typedef struct {
  uint16_t rows;
  uint16_t columns;
  int fd;
  struct termios *original_state;
  unsigned char *read_buf;
} TTY;

TTY *tty_new();
void tty_teardown_and_free(TTY *);
void tty_write(TTY *, const char *);
unsigned char tty_read_char(TTY *);

#endif /* end of include guard: TACK_TTY_H */
