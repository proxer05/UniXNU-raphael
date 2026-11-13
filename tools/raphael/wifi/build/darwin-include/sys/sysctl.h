#ifndef _SYS_SYSCTL_H
#define _SYS_SYSCTL_H
#include <stddef.h>
int sysctl(int *, unsigned, void *, size_t *, void *, size_t);
int sysctlbyname(const char *, void *, size_t *, void *, size_t);
#define CTL_KERN 1
#define KERN_PROC 14
#define KERN_PROC_ALL 0
/* xnu-11215 arm64: KERN_PROC_ALL returns the real 648-byte kinfo_proc. The
 * field offsets were verified on the device (diagnostics/pslist.c): p_pid at
 * 40, p_comm (MAXCOMLEN + 1 = 17 bytes) at 243. A fake small stride parsed
 * garbage, and bridge.c's configd restart never found its target. */
struct extern_proc {
	char __pad0[40];
	int p_pid;
	char __pad1[199];
	char p_comm[17];	/* MAXCOMLEN + 1 */
};
struct kinfo_proc {
	struct extern_proc kp_proc;
	char __pad2[648 - 260];	/* struct eproc and the rest */
};
#endif
