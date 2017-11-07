all: ext2mgr

ext2mgr: ext2fs.h main.c
	$(CC) -g main.c -o $@

check:
	@echo "Check"

clean:
	-rm ext2mgr

.PHONY: all clean check
