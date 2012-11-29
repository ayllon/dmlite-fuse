#include <dmlite/c/io.h>
#include <dmlite/c/pool.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "dmlite-fuse.h"



int dmlite_fuse_open(const char *path, struct fuse_file_info *finfo)
{
  dmlite_fuse_private *private = GET_PRIVATE();

  if (finfo->flags & 077 != O_RDONLY)
    return -EACCES;

  dmlite_location* loc = dmlite_get(private->context, path);
  if (!loc) {
    DMLITE_FUSE_LOG_FMT("open %s %o: Failed on get with %s", path, finfo->flags,
                        dmlite_error(private->context));
    return -dmlite_errno(private->context);
  }

  DMLITE_FUSE_LOG_FMT("open %s %o: To be done on %s:%s", path, finfo->flags,
                      loc->chunks[0].url.domain, loc->chunks[0].url.path);

  char full_path[1024];
  snprintf(full_path, sizeof(full_path), "%s:%s",
           loc->chunks[0].url.domain, loc->chunks[0].url.path);
  dmlite_location_free(private->context, loc);

  dmlite_fd* fd = dmlite_fopen(private->context, full_path,
                               O_RDONLY | O_INSECURE,
                               NULL);
  if (!fd) {
    DMLITE_FUSE_LOG_FMT("open %s %o: Failed on fopen with %s", path, finfo->flags,
                        dmlite_error(private->context));
    return -dmlite_errno(private->context);
  }

  finfo->fh = (uint64_t)fd;

  DMLITE_FUSE_LOG_FMT("open %s %o", path, finfo->flags);
  return 0;
}



int dmlite_fuse_read(const char *path, char *buffer, size_t size, off_t offset,
                     struct fuse_file_info *finfo)
{
  dmlite_fuse_private *private = GET_PRIVATE();
  dmlite_fd* fd = (dmlite_fd*)finfo->fh;

  int e = dmlite_fseek(fd, offset, SEEK_SET);
  if (e != 0) {
    DMLITE_FUSE_LOG_FMT("read %s: Failed on seek with %s", path,
                        dmlite_error(private->context));
    return -e;
  }

  DMLITE_FUSE_LOG_FMT("read %s %ld:%ld", path, offset, size);
  return dmlite_fread(fd, buffer, size);
}



int dmlite_fuse_release(const char *path, struct fuse_file_info *finfo)
{
  dmlite_fclose((dmlite_fd*)finfo->fh);
  DMLITE_FUSE_LOG_FMT("close %s", path);
  return 0;
}
