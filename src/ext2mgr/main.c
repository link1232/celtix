#include "ext2mgr.h"

#define BITS_PER_BYTE   8

int main(int argc, char *argv[])
{
        struct ext2_opts                cmd_line      = { 0 };
        struct ext2_super_block         sb            = { 0 };
        struct ext2_group_descriptor    gd            = { 0 };
        struct ext2_inode               *resvd_inodes = NULL;

        if (argc < 2) {
                usage();
        } else {
                parse_cmdline(argc,
                              argv,
                              &cmd_line);
        }

        init_ext2_super_block(&sb, &cmd_line);
        init_ext2_group_desc(&sb, &gd);
        resvd_inodes = init_ext2_resvd_inodes(&sb, &gd);

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

void init_ext2_super_block(struct ext2_super_block *sb,
                           struct ext2_opts        *cmd_line)
{
        int           i;
        unsigned long inode_ratio;
        unsigned long inode_size;
        unsigned long inodes_per_group;
        unsigned long block_size;
        unsigned long blocks_per_group;
        unsigned long total_block_groups;
        unsigned long total_blocks;

        time_t time_now = time(NULL);

        srand(time_now);

        block_size         = get_block_size(cmd_line->block_size);
        blocks_per_group   = block_size * BITS_PER_BYTE;
        total_blocks       = get_target_size(cmd_line->fs_size,
                                             cmd_line->target_path);
        total_blocks      /= block_size;
        total_block_groups = 1 + ((total_blocks - 1) / blocks_per_group);

        get_inode_ratio(NULL != cmd_line->usage_type ? cmd_line->usage_type :
                                                       "default",
                        total_blocks,
                        block_size,
                        &inode_size,
                        &inode_ratio);

        inodes_per_group   =
                ((((total_blocks * block_size) / inode_ratio)
                / total_block_groups) & -7) + (block_size / inode_size);

        sb->total_inodes                = inodes_per_group * total_block_groups;
        sb->total_blocks                = total_blocks;
        sb->total_reserved_blocks       = 0;
        sb->total_free_blocks           = total_blocks;
        sb->total_free_inodes           = sb->total_inodes;
        sb->first_data_block            = 1024 == block_size ? 1 : 0;
        sb->block_size_shift            = 1024 == block_size ? 0 :
                                          2048 == block_size ? 1 :
                                          4096 == block_size ? 2 : 3;
        sb->fragment_size_shift         = sb->block_size_shift;
        sb->blocks_per_group            = blocks_per_group;
        sb->fragments_per_group         = sb->blocks_per_group;
        sb->inodes_per_group            = inodes_per_group;
        sb->last_mount_time             = time_now;
        sb->last_write_time             = time_now;
        sb->mount_count                 = 0;
        sb->max_mount_count             = -1;
        sb->magic_number                = EXT2_MAGIC;
        sb->filesystem_state            = VALID_EXT2_FILESYSTEM;
        sb->error_procedure             = EXT2_ON_ERROR_CONTINUE;
        sb->minor_revision_level        = 0;
        sb->last_filesystem_check       = 0;
        sb->fs_check_interval           = 0;
        sb->creator_os                  = EXT2_LINUX;
        sb->revision_level              = EXT2_DYNAMIC_REVISION;
        sb->def_res_blocks_uid          = 0;
        sb->def_res_blocks_gid          = 0;
        /* Ext2 dynamic revision specific */
        sb->first_usable_inode          = EXT2_OLD_FIRST_INODE;
        sb->inode_size                  = inode_size;
        sb->superblock_block_num        = sb->first_data_block;
        sb->compatible_features         = EXT2_CF_EXT_ATTR | EXT2_CF_DIR_INDEX;
        sb->incompatible_features       = EXT2_ICF_FILETYPE;
        sb->readonly_features           = EXT2_ROCF_SPARSE_SUPER;
        for (i = 0; i < sizeof(sb->volume_uuid); i++)
                sb->volume_uuid[i] = rand() % 255 + 1;

        if (NULL != cmd_line->volume_label) {
                snprintf(sb->volume_name,
                         sizeof(sb->volume_name) - 1,
                         "%s",
                         cmd_line->volume_label);
                sb->volume_name[15] = 0;
        } else {
                memset(sb->volume_name,
                       0,
                       sizeof(sb->volume_name)); 
        }
        memset(sb->last_mounted_path,
               0,
               sizeof(sb->last_mounted_path));
        sb->compression_algorithm       = 0;
        /* Performance hits */
        sb->preallocate_blocks          = 0;
        sb->preallocate_dir_blocks      = 0;
        sb->reserved_gdt_blocks         = 0;
        /* Journaling support */
        memset(sb->journal_uuid,
               0,
               sizeof(sb->journal_uuid));
        sb->journal_inode_number        = 0;
        sb->journal_device              = 0;
        sb->last_orphan                 = 0;
        /* Directory indexing support */
        memset(sb->hash_seed,
               0,
               sizeof(sb->hash_seed));
        sb->def_hash_version            = EXT2_HA_HALF_MD4;
        memset(sb->reserved_padding,
               0,
               sizeof(sb->reserved_padding));
        /* Other options */
        sb->default_mount_options       = EXT2_DEF_MOUNT_XATTR_USER |
                                          EXT2_DEF_MOUNT_ACL;
        sb->first_meta_block_group      = 0;
        sb->filesystem_created          = time_now;
        memset(sb->unused,
               0,
               sizeof(sb->unused));
}

unsigned long get_block_size(unsigned long user_block_size)
{
        if (1024 != user_block_size
        &&  2048 != user_block_size
        &&  4096 != user_block_size
        &&  8192 != user_block_size) {
                printf("Warning: Invalid block size, defaulting to 1KiB\n");
                return 1024;
        } else {
                return user_block_size;
        }
}

unsigned long get_target_size(unsigned long user_fs_size, char *user_target)
{
        struct stat tmp = { 0 };

        if (0 != user_fs_size
        &&  10240 <= user_fs_size)
                return user_fs_size;

        if (NULL != user_target
        &&  stat(user_target, &tmp) != -1) {
                return tmp.st_size;
        } else {
                printf("Error: Failed to get target file size\n");
                exit(EXIT_FAILURE);
        }
}

void get_inode_ratio(char          *user_usage_type,
                     unsigned long  total_blocks,
                     unsigned long  block_size,
                     unsigned long *inode_size,
                     unsigned long *inode_ratio)
{
        unsigned long size_type = 0;

        size_type = (1024 * 1024) / block_size;

        if (total_blocks < (3 * size_type)
        ||  strcmp("floppy", user_usage_type) == 0) {
                *inode_ratio = 8192;
                *inode_size  = 128;
        } else if (total_blocks < (512 * size_type) 
               ||  strcmp("small", user_usage_type) == 0) {
                *inode_ratio = 4096;
                *inode_size  = 128;
        } else if (total_blocks < (4194304 * size_type)
               ||  strcmp("default", user_usage_type) == 0) {
                *inode_ratio = 16384;
                *inode_size  = 256;
        } else if (total_blocks < (16777216 * size_type)
               ||  strcmp("big", user_usage_type) == 0) {
                *inode_ratio = 32768;
                *inode_size  = 256;
        } else {  /* usage_type = huge */
                *inode_ratio = 65536;
                *inode_size  = 256;
        }
}

void init_ext2_group_desc(struct ext2_super_block *sb,
                          struct ext2_group_descriptor *gd)
{
        gd->block_bitmap_block          = sb->first_data_block   + 2;
        gd->inode_bitmap_block          = gd->block_bitmap_block + 1;
        gd->inode_table_block           = gd->inode_bitmap_block + 1;
        gd->group_free_blocks           = sb->blocks_per_group;
        gd->group_free_inodes           = sb->inodes_per_group;
        gd->group_used_directories      = 0;
        gd->padding                     = 0;
        memset(gd->reserved,
               0,
               sizeof(gd->reserved));
}

struct ext2_inode * init_ext2_resvd_inodes(struct ext2_super_block *sb,
                                           struct ext2_group_descriptor *gd)
{
        int                i;
        unsigned int       block_size;
        struct ext2_inode *r_inodes;

        r_inodes = (struct ext2_inode *) calloc(TOTAL_RESERVED_INODES,
                                                    sizeof(struct ext2_inode));
        if (NULL == r_inodes) {
                printf("Error: Failed to get memory for reserved inodes\n");
                exit(EXIT_FAILURE);
        }

        block_size = (1024 << sb->block_size_shift);

        r_inodes[EXT2_RVDI_ROOT].mode               = EXT2_IM_OTHER_EXECUTE
                                                    | EXT2_IM_OTHER_READ
                                                    | EXT2_IM_GROUP_EXECUTE
                                                    | EXT2_IM_GROUP_READ
                                                    | EXT2_IM_USER_EXECUTE
                                                    | EXT2_IM_USER_WRITE
                                                    | EXT2_IM_USER_READ
                                                    | EXT2_IM_DIRECTORY;
        r_inodes[EXT2_RVDI_ROOT].uid                = 0;
        r_inodes[EXT2_RVDI_ROOT].size               = 1 * block_size;
        r_inodes[EXT2_RVDI_ROOT].atime              = sb->filesystem_created;
        r_inodes[EXT2_RVDI_ROOT].ctime              = sb->filesystem_created;
        r_inodes[EXT2_RVDI_ROOT].mtime              = sb->filesystem_created;
        r_inodes[EXT2_RVDI_ROOT].dtime              = sb->filesystem_created;
        r_inodes[EXT2_RVDI_ROOT].gid                = 0;
        r_inodes[EXT2_RVDI_ROOT].links_count        = 3;
        r_inodes[EXT2_RVDI_ROOT].blocks             =
                r_inodes[EXT2_RVDI_ROOT].size / 512;
        r_inodes[EXT2_RVDI_ROOT].flags              = 0;
        r_inodes[EXT2_RVDI_ROOT].block[0]           =
                sb->total_reserved_blocks +
                ((sb->inodes_per_group * sb->inode_size) /
                 (1024 << sb->block_size_shift)) + gd->inode_table_block;
        r_inodes[EXT2_RVDI_ROOT].generation         = 0;
        r_inodes[EXT2_RVDI_ROOT].file_acl           = 0;
        r_inodes[EXT2_RVDI_ROOT].dir_acl            = 0;
        r_inodes[EXT2_RVDI_ROOT].fragment_address   = 0;
        r_inodes[EXT2_RVDI_ROOT].osd2[0]            = 0;

        r_inodes[EXT4_RVDI_REPLICA].mode             = EXT2_IM_USER_EXECUTE
                                                     | EXT2_IM_USER_WRITE
                                                     | EXT2_IM_USER_READ
                                                     | EXT2_IM_DIRECTORY;
        r_inodes[EXT4_RVDI_REPLICA].uid              = 0;
        r_inodes[EXT4_RVDI_REPLICA].size             = 12 * block_size;
        r_inodes[EXT4_RVDI_REPLICA].atime            = sb->filesystem_created;
        r_inodes[EXT4_RVDI_REPLICA].ctime            = sb->filesystem_created;
        r_inodes[EXT4_RVDI_REPLICA].mtime            = sb->filesystem_created;
        r_inodes[EXT4_RVDI_REPLICA].dtime            = sb->filesystem_created;
        r_inodes[EXT4_RVDI_REPLICA].gid              = 0;
        r_inodes[EXT4_RVDI_REPLICA].links_count      = 3;
        r_inodes[EXT4_RVDI_REPLICA].blocks           =
                r_inodes[EXT4_RVDI_REPLICA].size / 512;
        r_inodes[EXT4_RVDI_REPLICA].flags            = 0;
        r_inodes[EXT4_RVDI_REPLICA].block[0]         =
                r_inodes[EXT2_RVDI_ROOT].block[0] + 1;
        for (i = 1; i < (r_inodes[EXT4_RVDI_REPLICA].size / block_size); i++) {
                r_inodes[EXT4_RVDI_REPLICA].block[i] =
                        r_inodes[EXT4_RVDI_REPLICA].block[i - 1] + 1;
        }
        r_inodes[EXT4_RVDI_REPLICA].generation       = 0;
        r_inodes[EXT4_RVDI_REPLICA].file_acl         = 0;
        r_inodes[EXT4_RVDI_REPLICA].dir_acl          = 0;
        r_inodes[EXT4_RVDI_REPLICA].fragment_address = 0;
        r_inodes[EXT4_RVDI_REPLICA].osd2[0]          = 0;

        return r_inodes;
}
