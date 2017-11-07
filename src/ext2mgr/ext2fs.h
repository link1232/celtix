#include <stdint.h>

/* Typedef helpers */
typedef uint32_t u4bytes_t;
typedef uint16_t u2bytes_t;
typedef uint8_t  u1byte_t;

typedef int32_t s4bytes_t;
typedef int16_t s2bytes_t;
typedef int8_t  s1byte_t;

/* Ext2 constants */
#define EXT2_MAGIC 0xEF53

/* Ext2 structures */
struct ext2_super_block {
        u4bytes_t total_inodes;
        u4bytes_t total_blocks;
        u4bytes_t total_reserved_blocks;
        u4bytes_t total_free_blocks;
        u4bytes_t total_free_inodes;
        u4bytes_t first_data_block;
        u4bytes_t block_size_shift;
        u4bytes_t fragment_size_shift;
        u4bytes_t blocks_per_group;
        u4bytes_t fragments_per_group;
        u4bytes_t inodes_per_group;
        u4bytes_t last_mount_time;
        u4bytes_t last_write_time;
        u2bytes_t mount_count;
        u2bytes_t max_mount_count;
        u2bytes_t magic_number;
        u2bytes_t filesystem_state;
        u2bytes_t error_procedure;
        u2bytes_t minor_revision_level;
        u4bytes_t last_filesystem_check;
        u4bytes_t fs_check_interval;
        u4bytes_t creator_os;
        u4bytes_t revision_level;
        u2bytes_t def_res_blocks_uid;
        u2bytes_t def_res_blocks_gid;
        /* Ext2 dynamic revision specific */
        u4bytes_t first_usable_inode;
        u2bytes_t inode_size;
        u2bytes_t superblock_block_num;
        u4bytes_t compatible_features;
        u4bytes_t incompatible_features;
        u4bytes_t readonly_features;
        u1byte_t  volume_uuid[16];
        u1byte_t  volume_name[16];
        u1byte_t  last_mounted_path[64];
        u4bytes_t compression_algorithm;
        /* Performance hits */
        u1byte_t  preallocate_blocks;
        u1byte_t  preallocate_dir_blocks;
        u2bytes_t reserved_gdt_blocks; 
        /* Journaling support */
        u1byte_t  journal_uuid[16];
        u4bytes_t journal_inode_number;
        u4bytes_t journal_device;
        u4bytes_t last_orphan;
        /* Directory indexing support */
        u4bytes_t hash_seed[4];
        u1byte_t  def_hash_version;
        u1byte_t  reserved_padding[3];
        /* Other options */
        u4bytes_t default_mount_options;
        u4bytes_t first_meta_block_group;
        u4bytes_t filesystem_created;
        u1byte_t  unused[756];
};

struct ext2_group_descriptor {
        u4bytes_t block_bitmap_block;
        u4bytes_t inode_bitmap_block;
        u4bytes_t inode_table_block;
        u2bytes_t group_free_blocks;
        u2bytes_t group_free_inodes;
        u2bytes_t group_used_directories;
        u2bytes_t padding;
        u1byte_t reserved[12];
};

struct ext2_inode {
        u2bytes_t mode;
        u2bytes_t uid;
        s4bytes_t size;
        u4bytes_t atime;
        u4bytes_t ctime;
        u4bytes_t mtime;
        u4bytes_t dtime;
        u2bytes_t gid;
        u2bytes_t links_count;
        u4bytes_t blocks;
        u4bytes_t flags;
        u4bytes_t osd1;
        u4bytes_t block[15];
        u4bytes_t generation;
        u4bytes_t file_acl;
        u4bytes_t dir_acl;
        u4bytes_t fragment_address;
        u1byte_t  osd2[12];
};

struct ext2_file {
        u4bytes_t inode;
        u2bytes_t record_length;
        u1byte_t  name_length;
        u1byte_t  file_type;
        u1byte_t  name[255];
};

/* Ext2 enums */
enum _ext2_filesystem_state {
        VALID_EXT2_FILESYSTEM = 1,
        ERRORS_IN_EXT2_FILESYSTEM
};

enum _ext2_errors {
        EXT2_ON_ERROR_CONTINUE = 1,
        EXT2_ON_ERROR_READONLY,
        EXT2_ON_ERROR_PANIC
};

enum _ext2_creator_os {
        EXT2_LINUX,
        EXT2_HURD,
        EXT2_MASIX,
        EXT2_FREEBSD,
        EXT2_LITES
};

enum _ext2_revision_level {
        EXT2_OLD_REVISION,
        EXT2_DYNAMIC_REVISION
}; 

enum _ext2_compatible_feature {
        EXT2_CF_DIR_PREALLOC    = 0x0001,
        EXT2_CF_MAGIC_INODES    = 0x0002,
        EXT3_CF_HAS_JOURNAL     = 0x0004,
        EXT2_CF_EXT_ATTR        = 0x0008,
        EXT2_CF_RESIZE_INO      = 0x0010,
        EXT2_CF_DIR_INDEX       = 0x0020
};

enum _ext2_incompatible_feature {
        EXT2_ICF_COMPRESSION    = 0x0001,
        EXT2_ICF_FILETYPE       = 0x0002,
        EXT3_ICF_RECOVER        = 0x0004,
        EXT3_ICF_JOURNAL_DEV    = 0x0008,
        EXT2_ICF_META_BG        = 0x0010
};

enum _ext2_readonly_compatible_feature {
        EXT2_ROCF_SPARSE_SUPER  = 0x001,
        EXT2_ROCF_LARGE_FILE    = 0x002,
        EXT2_ROCF_BTREE_DIR     = 0x004
};

enum _ext2_algo_bitmap {
        EXT2_ALG_LZV1,
        EXT2_ALG_LZRW3A,
        EXT2_ALG_GZIP,
        EXT2_ALG_BZIP2,
        EXT2_ALG_LZO
};

enum _ext2_hash_algorithms {
        EXT2_HA_LEGACY,
        EXT2_HA_HALF_MD4,
        EXT2_HA_TEA,
        EXT2_HA_LEGACY_UNSIGNED,
        EXT2_HA_HALF_MD4_UNSIGNED,
        EXT2_HA_TEA_UNSIGNED
};

enum _ext2_reserved_inode {
        EXT2_RVDI_BAD = 1,
        EXT2_RVDI_ROOT,
        EXT2_RVDI_ACL_IDX,
        EXT2_RVDI_ACL_DATA,
        EXT2_RVDI_BOOTLOADER,
        EXT2_RVDI_UNDELETE_DIR,
        EXT2_RVDI_RESIZE,
        EXT2_RVDI_JOURNAL,
        EXT2_RVDI_EXCLUDE,
        EXT4_RVDI_REPLICA,
        TOTAL_RESERVED_INODES
};

#define EXT2_OLD_FIRST_INODE 11

enum _ext2_inode_mode {
        EXT2_IM_SOCKET        = 0xC000,
        EXT2_IM_SYMLINK       = 0xA000,
        EXT2_IM_REGULAR_FILE  = 0x8000,
        EXT2_IM_BLOCK_DEVICE  = 0x6000,
        EXT2_IM_DIRECTORY     = 0x4000,
        EXT2_IM_CHAR_DEVICE   = 0x2000,
        EXT2_IM_FIFO          = 0x1000,
        EXT2_IM_SUID          = 0x0800,
        EXT2_IM_SGID          = 0x0400,
        EXT2_IM_SVTX          = 0x0200,
        EXT2_IM_USER_READ     = 0x0100,
        EXT2_IM_USER_WRITE    = 0x0080,
        EXT2_IM_USER_EXECUTE  = 0x0040,
        EXT2_IM_GROUP_READ    = 0x0020,
        EXT2_IM_GROUP_WRITE   = 0x0010,
        EXT2_IM_GROUP_EXECUTE = 0x0008,
        EXT2_IM_OTHER_READ    = 0x0004,
        EXT2_IM_OTHER_WRITE   = 0x0002,
        EXT2_IM_OTHER_EXECUTE = 0x0001
};

enum _ext2_inode_flag {
        EXT2_IFL_SECURE_DELETE    = 0x00000001,
        EXT2_IFL_UNDELETE         = 0x00000002,
        EXT2_IFL_COMPRESSED       = 0x00000004,
        EXT2_IFL_SYNC_UPDATES     = 0x00000008,
        EXT2_IFL_IMMUTABLE        = 0x00000010,
        EXT2_IFL_APPEND_ONLY      = 0x00000020,
        EXT2_IFL_NODUMP           = 0x00000040,
        EXT2_IFL_NOATIME          = 0x00000080,
        /* Reserved for compression usage */
        EXT2_IFL_DIRTY                    = 0x00000100,
        EXT2_IFL_COMPRESSED_BLOCKS        = 0x00000200,
        EXT2_IFL_NO_COMPRESSED_BLOCKS     = 0x00000400,
        EXT2_IFL_COMPRESSION_ERROR        = 0x00000800,
        /* End of compression flags */
        EXT2_IFL_BTREE            = 0x00001000,
        EXT2_IFL_INDEX            = 0x00001000,
        EXT2_IFL_IMAGIC           = 0x00002000,
        EXT2_IFL_JOURNAL_DATA     = 0x00004000,
        EXT2_IFL_RESERVED         = 0x80000000
};

enum _ext2_file_type {
        EXT2_FT_UNKNOWN,
        EXT2_FT_EGULAR_FILE,
        EXT2_FT_DIRECTORY,
        EXT2_FT_CHAR_DEVICE,
        EXT2_FT_BLOCK_DEVICE,
        EXT2_FT_FIFO,
        EXT2_FT_SOCKET,
        EXT2_FT_SYMLINK
};

enum _ext2_default_mount_options {
        EXT2_DEF_MOUNT_DEBUG           = 0x0001,
        EXT2_DEF_MOUNT_BSDGROUPS       = 0x0002,
        EXT2_DEF_MOUNT_XATTR_USER      = 0x0004,
        EXT2_DEF_MOUNT_ACL             = 0x0008,
        EXT2_DEF_MOUNT_UID16           = 0x0010,
        EXT3_DEF_MOUNT_JMODE           = 0x0060,
        EXT3_DEF_MOUNT_JMODE_DATA      = 0x0020,
        EXT3_DEF_MOUNT_JMODE_ORDERED   = 0x0040,
        EXT3_DEF_MOUNT_JMODE_WBACK     = 0x0060,
        EXT4_DEF_MOUNT_NOBARRIER       = 0x0100,
        EXT4_DEF_MOUNT_BLOCK_VALIDITY  = 0x0200,
        EXT4_DEF_MOUNT_DISCARD         = 0x0400,
        EXT4_DEF_MOUNT_NODELALLOC      = 0x0800
};
