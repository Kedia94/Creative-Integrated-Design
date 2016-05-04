#include "Reader.h"

#include <string.h>
#include <stdio.h>

Reader::Reader(int fd, int sz){
  _fd = fd;
  _sz = sz;
  _remain = 0;
  _buf = _next = new char[sz];
}

Reader::~Reader(){
  delete[] _buf;
}

ssize_t Reader::read(char *usrbuf, size_t n){
  while (_remain <= 0){
    _remain = ::read(_fd, _buf, sizeof(_buf));
    if (_remain < 0){
      if (errno != EINTR)
        return -1;
    } else if (_remain == 0) {
      return 0;
    } else {
      _next = _buf;
    }
  }

  if (n > _remain)
    n = _remain;
  memcpy(usrbuf, _next, n);
  _next += n;
  _remain -= n;
  return n;
}

ssize_t Reader::readn(char *usrbuf, size_t n){
  int remain = n;
  while (remain > 0){
    int res = read(usrbuf, remain);
    if (res < 0){
      return -1;
    }
    else if (res == 0)
      break;
    else {
      usrbuf += res;
      remain -= res;
    }
  }

  return (n - remain);
}

ssize_t Reader::readline(char *usrbuf, size_t maxlen){
  int n, rc;
  char c, *cur = usrbuf;
  for (n=1; n<maxlen; n++){
    if ((rc = read(&c, 1)) == 1){
      *cur++ = c;
      if (c == '\n')
        break;
    }
    else if (rc == 0){
      if (n==1)
        return 0;
      else
        break;
    }
    else
      return -1;
  }
  *cur = 0;
  return n;
}


off_t Reader::seek(off_t offset, int whence){
  _remain = 0;
  _next = _buf;
  return lseek(_fd, offset, whence);
}



