#ifndef __fileio_h__
#define __fileio_h__

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err.h>
#include <unistd.h>

extern "C"{
  size_t fileread(const char *file_name, uint8_t** data, size_t size){
    FILE* fd;
    if(!file_name)
      errx(1, "Filename not specified");
    if((fd = fopen(file_name, "rb")) == NULL)
      errx(1, "Error opening file");
    if(*data){
      if(fread(*data, 1, size, fd) < size)
	errx(1, "File read failed");
    }else{
      fseek(fd, 0L, SEEK_END);
      size = ftell(fd);
      rewind(fd);
    }
    fclose(fd);
    return size;
  }
  size_t filewrite(const char *file_name, uint8_t* data, size_t size){
    int fd;
    if (!file_name)
        errx(1, "Filename not specified");
    if (!data)
        errx(1, "Data buffer not specified");
    if ((fd = creat(file_name, 0666)) < 1)
        errx(1, "Error creating file");
    if (write(fd, data, size) < size)
        errx(1, "Error writing samples");
    close(fd);
    return size;
  }
}

#endif // __fileio_h__
