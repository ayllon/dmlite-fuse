#include <stdio.h>
#include "dmlite-fuse.h"

static struct fuse_operations dmlite_oper = {
  .getattr    = dmlite_fuse_getattr,
  .readlink   = dmlite_fuse_readlink,
  .mkdir      = dmlite_fuse_mkdir,
  .unlink     = dmlite_fuse_unlink,
  .rmdir      = dmlite_fuse_rmdir,
  .symlink    = dmlite_fuse_symlink,
  .rename     = dmlite_fuse_rename,
  .chmod      = dmlite_fuse_chmod,
  .chown      = dmlite_fuse_chown,
  .utime      = dmlite_fuse_utime,
  .opendir    = dmlite_fuse_opendir,
  .readdir    = dmlite_fuse_readdir,
  .releasedir = dmlite_fuse_releasedir,
  .init       = dmlite_fuse_init,
  .destroy    = dmlite_fuse_destroy,
  
  .open       = dmlite_fuse_open,
  .read       = dmlite_fuse_read,
  .release    = dmlite_fuse_release
};


int main(int argc, char *argv[])
{
  return fuse_main(argc, argv, &dmlite_oper, NULL);
}
