#ifndef HOOK_INSTANCE_H
#define HOOK_INSTANCE_H

extern int (* old_open)(const char *, int, mode_t);
extern int (* old_fstat)(int, struct stat *);
extern ssize_t (* old_read_chk)(int, void *, size_t, size_t);
extern ssize_t (* old_read)(int, void *, size_t);
extern void * (* old_mmap)(void *, size_t, int, int, int, off_t);
extern int (* old_munmap)(void *, size_t length);
extern pid_t (* old_fork)(void);


int
hook(uint32_t addr, uint32_t fakeaddr, uint32_t ** old_addr);
int
new_open(const char * pathname, int flags, mode_t mode);
int
new_fstat(int fildes, struct stat * buf);
ssize_t
new_read(int fd, void * buf, size_t count);
ssize_t
new_read_chk(int fd, void * buf, size_t nbytes, size_t buflen);
void *
new_mmap(void * start, size_t length, int prot, int flags, int fd, off_t offset);
int
new_munmap(void * start, size_t length);
pid_t
new_fork(void);

#endif // ifndef HOOK_INSTANCE_H
