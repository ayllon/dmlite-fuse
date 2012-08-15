#include <dmlite/common/errno.h>
#include <errno.h>

int dmlite_fuse_map_errno(int dmerrno)
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
