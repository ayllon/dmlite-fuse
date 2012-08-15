#define FUSE_USE_VERSION 26

#include <dmlite/c/dmlite.h>
#include <dmlite/c/catalog.h>
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



static int dmlite_fuse_getattr(const char * path, struct stat * stat)
{
  dmlite_fuse_private *private = GET_PRIVATE();
  int e = dmlite_statl(private->context, path, stat);
  
  if (e == 0)
    DMLITE_FUSE_LOG_FMT("getattr %s", path);
  else
    DMLITE_FUSE_LOG_FMT("getattr %s failed: %s", path,
                        dmlite_error(private->context));
  
  return -e;
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
  
  return -e;
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
