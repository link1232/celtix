#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

#include "ext2fs.h"

struct ext2_opts {
        int             summary_only;
        char            *target_path;
        char            *usage_type;
        char            *volume_label;
        unsigned int    block_size;
        unsigned int    bytes_per_inode;
        unsigned int    number_of_inodes;
        unsigned long   fs_size;
};

void usage(void);
void parse_cmdline(int argc, char *[], struct ext2_opts *);
void init_ext2_super_block(struct ext2_super_block *,
                           struct ext2_opts        *);
unsigned long get_block_size(unsigned long);
unsigned long get_target_size(unsigned long, char *);
void get_inode_ratio(char *,
                     unsigned long,
                     unsigned long,
                     unsigned long *,
                     unsigned long *);
void init_ext2_group_desc(struct ext2_super_block *,
                          struct ext2_group_descriptor *);
struct ext2_inode * init_ext2_resvd_inodes(struct ext2_super_block *,
                                           struct ext2_group_descriptor *);
