#include "fs.h"

extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
extern size_t ramdisk_write(const void *buf, size_t offset, size_t len);

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
  {"stdout", 0, 0, 0, invalid_read, invalid_write},
  {"stderr", 0, 0, 0, invalid_read, invalid_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
}

size_t fs_filesz(int fd) {
  assert(fd < NR_FILES);
  return file_table[fd].size;
}

int fs_open(const char *pathname, int flags, int mode) {
  int i;
  for (i = 0; i < NR_FILES; i++) {
    if (!strcmp(pathname, file_table[i].name)) {
      file_table[i].open_offset = 0;
      printf("open %s, fd = %d\n", pathname, i);
      return i;
    }
  }
  assert(0);
  return -1;
}

ssize_t fs_read(int fd, void *buf, size_t len) {
  size_t sz = fs_filesz(fd);
  off_t curp = file_table[fd].disk_offset + file_table[fd].open_offset;
  printf("read fd=%d  curp=%d  len=%d\n", fd, curp, len);
  if (fd < 3) {
    return 0;
  }
  if (curp + len > file_table[fd].disk_offset + sz) {
    len = file_table[fd].disk_offset + sz - curp;
  }
  if (len > 0) {
    ramdisk_read(buf, curp, len);
    file_table[fd].open_offset += len;
  }
  return len;
}

ssize_t fs_write(int fd, const void *buf, size_t len) {
  size_t sz = fs_filesz(fd);
  off_t curp = file_table[fd].disk_offset + file_table[fd].open_offset;
  printf("write fd=%d  curp=%d  len=%d\n", fd, curp, len);
  if (curp + len > file_table[fd].disk_offset + sz) {
    len = file_table[fd].disk_offset + sz - curp;
  }
  if (len > 0) {
    ramdisk_write(buf, curp, len);
    file_table[fd].open_offset += len;
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

