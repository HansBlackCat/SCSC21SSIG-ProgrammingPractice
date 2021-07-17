#ifndef __DBG_H
#define __DBG_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef DEBUG
#define debug(M, ...) \
  fprintf(stderr, "DEBUG %s: %-4d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define debug(M, ...)
#endif

#define clean_errno() (errno == 0 ? "None" : strerror(errno))

#define log_err(M, ...)                                                                                      \
  do                                                                                                         \
  {                                                                                                          \
    fprintf(stderr, "[ERROR] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__); \
  } while (0)

#define log_warn(M, ...)                                                                                    \
  do                                                                                                        \
  {                                                                                                         \
    fprintf(stderr, "[WARN] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__); \
  } while (0)

#define log_info(M, ...)                                                                                    \
  do                                                                                                        \
  {                                                                                                         \
    fprintf(stderr, "[INFO] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__); \
  } while (0)

#define check(A, M, ...)       \
  if (!(A))                    \
  {                            \
    log_err(M, ##__VA_ARGS__); \
    errno = 0;                 \
    goto error;                \
  }

#define sentinel(M, ...)       \
  do                           \
  {                            \
    log_err(M, ##__VA_ARGS__); \
    errno = 0;                 \
    goto error;                \
  } while (0)

#define check_mem(A) \
  check((A), "Out of memory")

#define check_debug(A, M, ...) \
  if (!(A))                    \
  {                            \
    debug(M, ##__VA_ARGS__);   \
    errno = 0;                 \
    goto error;                \
  }

#endif