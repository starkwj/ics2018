#include "fs.h"

extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
extern size_t ramdisk_write(const void *buf, size_t offset, size_t len);
extern size_t serial_write(const void *buf, size_t offset, size_t len);
extern size_t dispinfo_read(void *buf, size_t offset, size_t len);
extern size_t fb_write(const void *buf, size_t offset, size_t len);
extern size_t events_read(void *buf, size_t offset, size_t len);

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  off_t disk_offset;
  off_t open_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin", 0, 0, 0, invalid_read, invalid_write},
  {"stdout", 0, 0, 0, invalid_read, serial_write},
  {"stderr", 0, 0, 0, invalid_read, serial_write},
  {"/dev/fb", 0, 0, 0, invalid_read, fb_write},
  {"/proc/dispinfo", 0, 0, 0, dispinfo_read, invalid_write},
  {"/dev/events", 0, 0, 0, events_read, invalid_write},
  {"/dev/tty", 0, 0, 0, invalid_read, serial_write},
#include "files.h"
};
enum {
  FB = 3, DISPINFO, EVENTS, TTY
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
  file_table[FB].size = screen_width() * screen_height() * sizeof(int);
}

size_t fs_filesz(int fd) {
  assert(fd < NR_FILES);
  return file_table[fd].size;
}

int fs_open(const char *pathname, int flags, int mode) {
  int i;
  for (i = 0; i < NR_FILES; i++) {
    printf("test %d  pathname:%x  filetable[i]name%x\n", i, pathname, file_table[i].name);
    if (!strcmp(pathname, file_table[i].name)) {
      file_table[i].open_offset = 0;
      printf("fs_open: %s\n", pathname);
      return i;
    }
  }
  printf("file ‘%s’ not found\n", pathname);
  assert(0);
  return -1;
}

ssize_t fs_read(int fd, void *buf, size_t len) {
  size_t sz = fs_filesz(fd);
  off_t curp = file_table[fd].disk_offset + file_table[fd].open_offset;
  if (file_table[fd].read != NULL) {
    len = file_table[fd].read(buf, curp, len);
    file_table[fd].open_offset += len;
  }
  else {
    if (curp + len > file_table[fd].disk_offset + sz) {
      len = file_table[fd].disk_offset + sz - curp;
    }
    if (len > 0) {
      ramdisk_read(buf, curp, len);
      file_table[fd].open_offset += len;
    }
  }
  return len;
}

ssize_t fs_write(int fd, const void *buf, size_t len) {
  size_t sz = fs_filesz(fd);
  off_t curp = file_table[fd].disk_offset + file_table[fd].open_offset;
  if (fd < 3)
    return file_table[fd].write(buf, curp, len);
  
  if (file_table[fd].write != NULL) {
    len = file_table[fd].write(buf, curp, len);
    file_table[fd].open_offset += len;
  }
  else {
    if (curp + len > file_table[fd].disk_offset + sz) {
      len = file_table[fd].disk_offset + sz - curp;
    }
    if (len > 0) {
      ramdisk_write(buf, curp, len);
      file_table[fd].open_offset += len;
    }
  }
  return len;
}

off_t fs_lseek(int fd, off_t offset, int whence) {
  assert(fd < NR_FILES);
  off_t new = -1;
  switch (whence) {
    case SEEK_SET:
      new = offset;
      break;
    case SEEK_CUR:
      new = file_table[fd].open_offset + offset;
      break;
    case SEEK_END:
      new = file_table[fd].size + offset;
      break;
    default:
      panic("should not reach here");
      break;
  }
  assert(new >= 0 && new <= file_table[fd].size);
  file_table[fd].open_offset = new;
  return new;
}

int fs_close(int fd) {
  return 0;
}

