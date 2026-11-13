/*
 * peek: read physical memory via /dev/pmem (root only). Debugging aid for the
 * modem bring-up: dump MSS RMB status registers where the MBA stalls.
 *
 *   peek <hex-addr> <count>       32-bit little-endian words, one per line
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

int
main(int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stderr, "usage: peek <hex-addr> [count-words=1]\n");
		return 2;
	}
	uint64_t pa = strtoull(argv[1], NULL, 16);
	unsigned n = argc > 2 ? (unsigned)strtoul(argv[2], NULL, 0) : 1;
	int fd = open("/dev/pmem", O_RDONLY);
	if (fd < 0) {
		perror("/dev/pmem");
		return 1;
	}
	for (unsigned i = 0; i < n; i++) {
		uint32_t v = 0;
		if (pread(fd, &v, 4, (off_t)(pa + 4ull * i)) != 4) {
			fprintf(stderr, "peek %#llx: read failed (offset %+u)\n",
			    (unsigned long long)pa, i * 4);
			return 1;
		}
		printf("%#llx = %08x\n", (unsigned long long)(pa + 4ull * i), v);
		fflush(stdout);
	}
	close(fd);
	return 0;
}
