#pragma once

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

class Reader {
  public:
    Reader (int, int = 4096);
    ~Reader();
    ssize_t read(char *, size_t);
    ssize_t readn(char *, size_t);
    ssize_t readline(char *, size_t);
    off_t seek(off_t, int);
  private:
    int _sz, _remain, _fd;
    char *_buf, *_next;
};

