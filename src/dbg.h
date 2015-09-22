#ifndef __dbg_h__
#define __dbg_h__

#include <stdio.h>
#include <errno.h>
#include <string.h>

//
// ##__VA_ARGS__ is not portable. This is problematic when __VA_ARGS__ consists
// of a format string with no argument. For example:
//
// #define debug(M, ...) \
// fprintf(stderr, "DEBUG %s:%d: " M "\n", __FILE__, __LINE__, __VA_ARGS__)
//
// will not compile because there will be a trailing comma after '__LINE__'.
// ##__VA_ARGS__ is designed to swallow the stray comma, but this feature is a
// GNU extension.
//
// The workaround here is to use multiple printf statements.
//

#ifdef NDEBUG
#define debug(...)
#else
#define debug(...) \
do { fprintf(stderr, "DEBUG %s:%d: ", __FILE__, __LINE__); \
     fprintf(stderr, __VA_ARGS__); \
     fprintf(stderr, "\n"); \
   } while(0)
#endif

#define clean_errno() (errno == 0 ? "None" : strerror(errno))

#define log_err(...) \
do { fprintf(stderr, "[ERROR] (%s:%d: errno: %s) ", \
     __FILE__, __LINE__, clean_errno()); \
     fprintf(stderr, __VA_ARGS__); \
     fprintf(stderr, "\n"); \
   } while(0)

#define log_warn(...) \
do { fprintf(stderr, "[WARN] (%s:%d: errno: %s) ", \
     __FILE__, __LINE__, clean_errno()); \
     fprintf(stderr, __VA_ARGS__); \
     fprintf(stderr, "\n"); \
   } while(0)

#define log_info(...) \
do { fprintf(stderr, "[INFO] (%s:%d) ", __FILE__, __LINE__); \
     fprintf(stderr, __VA_ARGS__); \
     fprintf(stderr, "\n"); \
   } while(0)

#define check(A, ...) \
if(!(A)) { log_err(__VA_ARGS__); errno=0; goto error; }

#define sentinel(...) \
{ log_err(__VA_ARGS__); errno=0; goto error; }

#define check_mem(A) check((A), "Out of memory.")

#define check_debug(A, ...) \
if(!(A)) { debug(__VA_ARGS__); errno=0; goto error; }

#endif
