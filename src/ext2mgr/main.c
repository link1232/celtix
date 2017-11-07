#include "ext2mgr.h"

int main(int argc, char *argv[])
{
        struct ext2_opts cmd_line  = { 0 };

        if (argc < 2) {
                usage();
        } else {
                parse_cmdline(argc,
                              argv,
                              &cmd_line);
        }

        return EXIT_SUCCESS;
}

void usage(void)
{
        printf("usage: mkext2fs [ -b block-size ] [ -i bytes-per-inode ]"
               " [ -L new-volume-label ] [ -n ] [ -N number-of-inodes ]"
               " [ -T usage-type ] [ -h ] target [ fs-size ]\n\n");
        exit(EXIT_SUCCESS);
}

void parse_cmdline(int argc, char *argv[], struct ext2_opts *cmd_line)
{
        int opt;

        do {
                opt = getopt(argc,
                             argv,
                             "nhb:i:L:N:T:");
                switch (opt) {
                case 'b':
                        cmd_line->block_size = strtoul(optarg,
                                                       NULL,
                                                       10);
                        break;
                case 'i':
                        cmd_line->bytes_per_inode = strtoul(optarg,
                                                            NULL,
                                                            10);
                        break;
                case 'L':
                        cmd_line->volume_label = strdup(optarg);
                        break;
                case 'n':
                        cmd_line->summary_only = 1;
                        break;
                case 'N':
                        cmd_line->number_of_inodes = strtoul(optarg,
                                                             NULL,
                                                             10);
                        break;
                case 'T':
                        cmd_line->usage_type = strdup(optarg);
                        break;
                case 'h':
                        usage();
                        break;
                }
        } while (-1 != opt);

        cmd_line->target_path = strdup(argv[optind]);
        optind++;
        
        if (argc > optind) {
                cmd_line->fs_size = strtoul(argv[optind],
                                            NULL,
                                            10);

                switch (*(argv[optind] + strlen(argv[optind]) - 1)) {
                case 'k':
                        cmd_line->fs_size *= 1024;
                        break;
                case 'm':
                        cmd_line->fs_size *= 1024 * 1024;
                        break;
                case 'g':
                        cmd_line->fs_size *= 1024 * 1024 * 1024;
                        break;
                }
        }

        if (1 == cmd_line->summary_only) {
                printf("device           = %s\n"
                       "usage_type       = %s\n"
                       "volume_label     = %s\n"
                       "fs_size          = %u\n"
                       "block_size       = %u\n"
                       "bytes_per_inode  = %u\n"
                       "number_of_inodes = %u\n",
                       cmd_line->summary_only,
                       cmd_line->target_path,
                       cmd_line->usage_type,
                       cmd_line->volume_label,
                       cmd_line->fs_size,
                       cmd_line->block_size,
                       cmd_line->bytes_per_inode,
                       cmd_line->number_of_inodes);
                exit(EXIT_SUCCESS);
        }
}
