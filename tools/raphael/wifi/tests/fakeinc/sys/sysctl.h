/* Host-test fake: glibc has no sys/sysctl.h; the sysctlbyname symbol is
 * renamed to the host stub by -Dsysctlbyname=host_sysctlbyname in run.sh. */
#include "../../host_stubs.h"
