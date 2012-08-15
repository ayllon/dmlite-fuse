#define FUSE_USE_VERSION 26

#include <dmlite/c/dmlite.h>
#include <dmlite/c/catalog.h>
#include <errno.h>
#include <fuse/fuse.h>
#include <stdio.h>
#include <syslog.h>
#include <unistd.h>

#define DMLITE_FUSE_LOG(msg)          syslog(LOG_USER | LOG_DEBUG, "%s", msg)
#define DMLITE_FUSE_LOG_FMT(fmt, ...) syslog(LOG_USER | LOG_DEBUG, fmt, __VA_ARGS__)

#define GET_PRIVATE() ((dmlite_fuse_private*)(fuse_get_context()->private_data))

typedef struct dmlite_fuse_private {
  dmlite_manager *manager;
  dmlite_context *context;
  dmlite_credentials creds;
} dmlite_fuse_private;



static int dmlite_fuse_map_errno(int dmerrno)
{
  switch (dmerrno) {
    case DM_NO_ERROR:
      return 0;

    case DM_UNKNOWN_ERROR:
    case DM_UNEXPECTED_EXCEPTION:
    case DM_INTERNAL_ERROR:
      return -ENOTRECOVERABLE;
      
    case DM_NO_SUCH_SYMBOL:
      return -ENOENT;
      
    case DM_API_VERSION_MISMATCH:
      return -EINVAL;
      
    case DM_NO_FACTORY:
    case DM_NO_POOL_MANAGER:
    case DM_NO_CATALOG:
    case DM_NO_INODE:
    case DM_NO_AUTHN:
    case DM_NO_IO:
      return -EBADF;
      
    case DM_NO_SECURITY_CONTEXT:
    case DM_EMPTY_SECURITY_CONTEXT:
      return -EACCES;
    

    case DM_MALFORMED_CONF:
    case DM_UNKNOWN_OPTION:
    case DM_UNKNOWN_KEY:
      return -EINVAL;

    case DM_UNKNOWN_HOST:
    case DM_CONNECTION_ERROR:
    case DM_SERVICE_UNAVAILABLE:
      return -EHOSTUNREACH;
      
    case DM_QUERY_FAILED:
    case DM_UNKNOWN_FIELD:
      return -EBADRQC;

    case DM_NOT_IMPLEMENTED:
      return -ENOSYS;
      
    case DM_NULL_POINTER:
      return -EFAULT;
      
    case DM_BAD_OPERATION:
      return -EPERM;
      
    case DM_NO_SUCH_FILE:
      return -ENOENT;
      
    case DM_BAD_DESCRIPTOR:
      return -EBADF;
      
    case DM_RESOURCE_UNAVAILABLE:
      return -EBUSY;
      
    case DM_OUT_OF_MEMORY:
      return -ENOMEM;
      
    case DM_FORBIDDEN:
      return -EACCES;
      
    case DM_EXISTS:
      return -EEXIST;
      
    case DM_NOT_DIRECTORY:
      return -ENOTDIR;
      
    case DM_IS_DIRECTORY:
      return -EISDIR;
      
    case DM_INVALID_VALUE:
      return -EINVAL;
      
    case DM_NO_SPACE_LEFT:
      return -ENOSPC;
      
    case DM_NAME_TOO_LONG:
      return -E2BIG;
      
    case DM_TOO_MANY_SYMLINKS:
      return -ELOOP;
      
    case DM_NO_COMMENT:
      return -ENOENT;
      
    case DM_NO_REPLICAS:
    case DM_PUT_ERROR:
      return -EBADFD;
      
    case DM_IS_CWD:
      return -EINVAL;
      
    case DM_NO_SUCH_REPLICA:
      return -ENOENT;
      
    case DM_NOT_REGULAR:
      return -EINVAL;

    case DM_NO_USER_MAPPING:
    case DM_NO_SUCH_USER:
    case DM_NO_SUCH_GROUP:
      return -EACCES;

    case DM_INVALID_ACL: case DM_UNKNOWN_POOL_TYPE:
      return -EINVAL;
    case DM_NO_SUCH_FS: case DM_NO_SUCH_POOL:
      return -ENOENT;
    default:
      return -EINVAL;
  }  
}



static int dmlite_fuse_getattr(const char * path, struct stat * stat)
{
  dmlite_fuse_private *private = GET_PRIVATE();
  int e = dmlite_statl(private->context, path, stat);
  
  if (e == 0)
    DMLITE_FUSE_LOG_FMT("getattr %s", path);
  else
    DMLITE_FUSE_LOG_FMT("getattr %s failed: %s", path,
                        dmlite_error(private->context));
  
  return dmlite_fuse_map_errno(e);
}



static int dmlite_fuse_readlink(const char *path, char *target, size_t tsize)
{
  dmlite_fuse_private *private = GET_PRIVATE();
  int e = dmlite_readlink(private->context, path, target, tsize);
  
  if (e == 0)
    DMLITE_FUSE_LOG_FMT("readlink %s => %s", path, target);
  else
    DMLITE_FUSE_LOG_FMT("readlink %s failed: %s", path,
                        dmlite_error(private->context));
  
  return dmlite_fuse_map_errno(e);
}



static int dmlite_fuse_mkdir(const char *path, mode_t mode)
{
  dmlite_fuse_private *private = GET_PRIVATE();
  int e = dmlite_mkdir(private->context, path, mode | S_IFDIR);
  
  if (e == 0)
    DMLITE_FUSE_LOG_FMT("mkdir %s", path);
  else
    DMLITE_FUSE_LOG_FMT("mkdir %s failed: %s", path,
                        dmlite_error(private->context));
  
  return dmlite_fuse_map_errno(e);
}



static int dmlite_fuse_unlink(const char * path)
{
  dmlite_fuse_private *private = GET_PRIVATE();
  int e = dmlite_unlink(private->context, path);
  
  if (e == 0)
    DMLITE_FUSE_LOG_FMT("unlink %s", path);
  else
    DMLITE_FUSE_LOG_FMT("unlink %s failed: %s", path,
                        dmlite_error(private->context));
  
  return dmlite_fuse_map_errno(e);
}



static int dmlite_fuse_rename(const char *oldname, const char *newname)
{
  dmlite_fuse_private *private = GET_PRIVATE();
  int e = dmlite_rename(private->context, oldname, newname);
  
  if (e == 0)
    DMLITE_FUSE_LOG_FMT("rename %s => %s", oldname, newname);
  else
    DMLITE_FUSE_LOG_FMT("rename %s failed: %s", oldname,
                        dmlite_error(private->context));
  
  return dmlite_fuse_map_errno(e);
}



static int dmlite_fuse_rmdir(const char* path)
{
  dmlite_fuse_private *private = GET_PRIVATE();
  int e = dmlite_rmdir(private->context, path);
  
  if (e == 0)
    DMLITE_FUSE_LOG_FMT("rmdir %s", path);
  else
    DMLITE_FUSE_LOG_FMT("rmdir %s failed: %s", path,
                        dmlite_error(private->context));
  
  return dmlite_fuse_map_errno(e);
}



static int dmlite_fuse_symlink(const char *oldpath, const char *newpath)
{
  dmlite_fuse_private *private = GET_PRIVATE();
  int e = dmlite_symlink(private->context, oldpath, newpath);
  
  if (e == 0)
    DMLITE_FUSE_LOG_FMT("symlink %s => %s", oldpath, newpath);
  else
    DMLITE_FUSE_LOG_FMT("symlink %s failed: %s", oldpath,
                        dmlite_error(private->context));
  
  return dmlite_fuse_map_errno(e);
}



static int dmlite_fuse_chmod(const char *path, mode_t mode)
{
  dmlite_fuse_private *private = GET_PRIVATE();
  int e = dmlite_chmod(private->context, path, mode & ~S_IFMT);
  
  if (e == 0)
    DMLITE_FUSE_LOG_FMT("chmod %o %s", mode, path);
  else
    DMLITE_FUSE_LOG_FMT("chmod %s failed: %s", path,
                        dmlite_error(private->context));
  
  return dmlite_fuse_map_errno(e);
}



static int dmlite_fuse_chown(const char *path, uid_t uid, gid_t gid)
{
  dmlite_fuse_private *private = GET_PRIVATE();
  int e = dmlite_chown(private->context, path, uid, gid);
  
  if (e == 0)
    DMLITE_FUSE_LOG_FMT("chown %d %d %s", uid, gid, path);
  else
    DMLITE_FUSE_LOG_FMT("chmod %s failed: %s", path,
                        dmlite_error(private->context));
  
  return dmlite_fuse_map_errno(e);
}



static int dmlite_fuse_utime(const char *path, struct utimbuf *buf)
{
  dmlite_fuse_private *private = GET_PRIVATE();
  int e = dmlite_utime(private->context, path, buf);
  
  if (e == 0)
    DMLITE_FUSE_LOG_FMT("utime %s", path);
  else
    DMLITE_FUSE_LOG_FMT("utime %s failed: %s", path,
                        dmlite_error(private->context));
  
  return dmlite_fuse_map_errno(e);
}



static void* dmlite_fuse_init(struct fuse_conn_info *conn)
{
  dmlite_fuse_private *private = (dmlite_fuse_private*)calloc(1,
                                                              sizeof(dmlite_fuse_private));

  private->manager = dmlite_manager_new();
  
  if (dmlite_manager_load_configuration(private->manager, "/etc/dmlite.conf") != 0) {
    DMLITE_FUSE_LOG_FMT("Could not load the configuration: %s",
                        dmlite_manager_error(private->manager));
    dmlite_manager_free(private->manager);
    abort();
  }
  
  private->context = dmlite_context_new(private->manager);  
  if (private->context == NULL) {
    DMLITE_FUSE_LOG_FMT("Could not get a context: %s",
                        dmlite_manager_error(private->manager));
    dmlite_manager_free(private->manager);
    abort();
  }
  
  private->creds.client_name = getlogin();
  if (private->creds.client_name == NULL)
    private->creds.client_name = "root";
  
  DMLITE_FUSE_LOG_FMT("Using user %s", private->creds.client_name);
  if (dmlite_setcredentials(private->context, &private->creds) != 0) {
    DMLITE_FUSE_LOG_FMT("Could not set the credentials: %s",
                        dmlite_error(private->context));
    dmlite_context_free(private->context);
    dmlite_manager_free(private->manager);
    abort();
  }
  
  DMLITE_FUSE_LOG("Driver initialized");

  return private;
}



static void dmlite_fuse_destroy(void* p)
{
  dmlite_fuse_private *private = (dmlite_fuse_private*)p;
  dmlite_context_free(private->context);
  dmlite_manager_free(private->manager);
  free(private);
  
  DMLITE_FUSE_LOG("Driver destroyed");
}



static int dmlite_fuse_opendir(const char * path, struct fuse_file_info *finfo)
{
  dmlite_fuse_private *private = GET_PRIVATE();
  dmlite_dir *dir = dmlite_opendir(private->context, path);
  if (!dir) {
    DMLITE_FUSE_LOG_FMT("opendir %s failed: %s", path, dmlite_error(private->context));
    return -dmlite_errno(private->context);
  }
  finfo->fh = (uint64_t)dir;
  DMLITE_FUSE_LOG_FMT("opendir %s", path);
  return 0;
}



static int dmlite_fuse_readdir(const char *path, void *buf,
                               fuse_fill_dir_t filler, off_t offset,
			                         struct fuse_file_info *finfo)
{
  dmlite_fuse_private *private = GET_PRIVATE();
  dmlite_dir *dir = (dmlite_dir*)finfo->fh;
  dmlite_xstat *xs;
  
  filler(buf, ".", NULL, 0);
  filler(buf, "..", NULL, 0);
  
  while ((xs = dmlite_readdirx(private->context, dir)) != NULL) {
    filler(buf, xs->name, &xs->stat, 0);
  }
}



static int dmlite_fuse_releasedir(const char *path, struct fuse_file_info *finfo)
{
  dmlite_fuse_private *private = GET_PRIVATE();
  dmlite_dir *dir = (dmlite_dir*)finfo->fh;
  dmlite_closedir(private->context, dir);
  DMLITE_FUSE_LOG_FMT("closedir %s", path);
  return 0;
}



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
  .destroy    = dmlite_fuse_destroy
};



int main(int argc, char *argv[])
{
  return fuse_main(argc, argv, &dmlite_oper, NULL);
}
