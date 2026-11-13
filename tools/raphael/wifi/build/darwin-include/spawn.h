#ifndef _SPAWN_H
#define _SPAWN_H
#include <sys/types.h>
#include <stdint.h>
#include <stddef.h>
typedef int pid_t;
typedef struct { int __opaque[64]; } posix_spawn_file_actions_t;
typedef struct { int __opaque[64]; } posix_spawnattr_t;
int posix_spawn(pid_t *, const char *, const void *, const void *, char *const [], char *const []);
int posix_spawn_file_actions_init(posix_spawn_file_actions_t *);
int posix_spawn_file_actions_addopen(posix_spawn_file_actions_t *, int, const char *, int, int);
int posix_spawn_file_actions_destroy(posix_spawn_file_actions_t *);
#define POSIX_SPAWN_CLOEXEC_DEFAULT 0x4000
#endif
