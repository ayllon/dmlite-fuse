#ifndef DMLITE_FUSE_H
#define	DMLITE_FUSE_H

#include <dmlite/c/dmlite.h>

#define FUSE_USE_VERSION 26
#include <fuse/fuse.h>

#include <syslog.h>
#include <utime.h>

/* Logging macros */
#define DMLITE_FUSE_LOG(msg)          syslog(LOG_USER | LOG_DEBUG, "%s", msg)
#define DMLITE_FUSE_LOG_FMT(fmt, ...) syslog(LOG_USER | LOG_DEBUG, fmt, __VA_ARGS__)

/* Private context */
typedef struct dmlite_fuse_private {
  dmlite_manager *manager;
  dmlite_context *context;
} dmlite_fuse_private;

#define GET_PRIVATE() ((dmlite_fuse_private*)(fuse_get_context()->private_data))



#ifdef	__cplusplus
extern "C" {
#endif

int   dmlite_fuse_getattr(const char * path, struct stat * stat);
int   dmlite_fuse_readlink(const char *path, char *target, size_t tsize);
int   dmlite_fuse_mkdir(const char *path, mode_t mode);
int   dmlite_fuse_unlink(const char * path);
int   dmlite_fuse_rename(const char *oldname, const char *newname);
int   dmlite_fuse_rmdir(const char* path);
int   dmlite_fuse_symlink(const char *oldpath, const char *newpath);
int   dmlite_fuse_chmod(const char *path, mode_t mode);
int   dmlite_fuse_chown(const char *path, uid_t uid, gid_t gid);
int   dmlite_fuse_utime(const char *path, struct utimbuf *buf);
void* dmlite_fuse_init(struct fuse_conn_info *conn);
void  dmlite_fuse_destroy(void* p);
int   dmlite_fuse_opendir(const char * path, struct fuse_file_info *finfo);
int   dmlite_fuse_readdir(const char *path, void *buf,
                          fuse_fill_dir_t filler, off_t offset,
                          struct fuse_file_info *finfo);
int   dmlite_fuse_releasedir(const char *path, struct fuse_file_info *finfo);

int   dmlite_fuse_open(const char *path, struct fuse_file_info *finfo);
int   dmlite_fuse_read(const char *path, char *buffer, size_t size, off_t offset,
                       struct fuse_file_info *finfo);
int   dmlite_fuse_release(const char *path, struct fuse_file_info *finfo);

#ifdef	__cplusplus
}
#endif

#endif	/* DMLITE_FUSE_H */
