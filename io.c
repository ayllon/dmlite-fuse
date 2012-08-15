#include <errno.h>
#include <string.h>
#include "dmlite-fuse.h"

int dmlite_fuse_open(const char *path, struct fuse_file_info *finfo)
{
  DMLITE_FUSE_LOG_FMT("open %s %o", path, finfo->flags);
  if (finfo->flags & 077 != O_RDONLY)
    return -EACCES;
  return 0;
}



int dmlite_fuse_read(const char *path, char *buffer, size_t size, off_t offset,
                     struct fuse_file_info *finfo)
{
  const char pending[] = {"TO-DO\n"};
  memcpy(buffer, pending, sizeof(pending) - 1);
  DMLITE_FUSE_LOG_FMT("read %s %ld:%ld", path, offset, size);
  return sizeof(pending) - 1;
}



int dmlite_fuse_release(const char *path, struct fuse_file_info *finfo)
{
  DMLITE_FUSE_LOG_FMT("close %s", path);
  return 0;
}
