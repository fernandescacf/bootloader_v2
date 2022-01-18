/**
 * @file        fat32.h
 * @author      Carlos Fernandes
 * @version     1.0
 * @date        06 March, 2021
 * @brief       Fat32 Header File
*/

#ifndef _FAT32_H_
#define _FAT32_H_

#ifdef __cplusplus
    extern "C" {
#endif


/* Includes ----------------------------------------------- */
#include <types.h>


/* Exported types ----------------------------------------- */
typedef struct boot_sector
{
    uint8_t  ignored[3];        /* Bootstrap code */
    char     system_id[8];      /* Name of fs */
    uint16_t sector_size;       /* Bytes/sector */
    uint8_t  cluster_size;      /* Sectors/cluster */
    uint16_t reserved;          /* Number of reserved sectors */
    uint8_t  fats;              /* Number of FATs */
    uint16_t dir_entries;       /* Number of root directory entries */
    uint16_t sectors;           /* Number of sectors */
    uint8_t  media;             /* Media code */
    uint16_t fat_length;        /* Sectors/FAT */
    uint16_t secs_track;        /* Sectors/track */
    uint16_t heads;             /* Number of heads */
    uint32_t hidden;            /* Number of hidden sectors */
    uint32_t total_sect;        /* Number of sectors (if sectors == 0) */

    /* FAT32 only */
    uint32_t fat32_length;      /* Sectors/FAT */
    uint16_t flags;             /* Bit 8: fat mirroring, low 4: active fat */
    uint16_t version;           /* Filesystem version */
    uint32_t root_cluster;      /* First cluster in root directory */
    uint16_t info_sector;       /* Filesystem info sector */
    uint16_t backup_boot;       /* Backup boot sector */
    uint16_t reserved2[6];      /* Unused */
} __attribute__ ((__packed__)) boot_sector_t;

typedef struct volume_info
{
    uint8_t  drive_number;      /* BIOS drive number */
    uint8_t  reserved;          /* Unused */
    uint8_t  ext_boot_sign;     /* 0x29 if fields below exist (DOS 3.3+) */
    uint32_t volume_id;         /* Volume ID number */
    char     volume_label[11];  /* Volume label */
    char     fs_type[8];        /* Typically FAT12, FAT16, or FAT32 */
} __attribute__ ((__packed__)) volume_info_t;

typedef struct dir_entry
{
    char     name[8];   /* Name */
    char     ext[3];    /* Extension */
    uint8_t  attr;      /* Attribute bits */
    uint8_t  nt;        /* Reserved for use by Windows NT. Set value to 0 */
    uint8_t  ctime_ms;  /* Creation time, milliseconds */
    uint16_t ctime;     /* Creation time */
    uint16_t cdate;     /* Creation date */
    uint16_t adate;     /* Last access date */
    uint16_t starthi;   /* High 16 bits of cluster in FAT32 */
    uint16_t time;      /* Time */
    uint16_t date;      /* Date */
    uint16_t startlo;   /* Low 16 bits of cluster in FAT32 */
    uint32_t size;		/* File size in bytes */
}dir_entry_t;

typedef struct dir_slot
{
    uint8_t  id;            /* Sequence number for slot */
    char     name0_4[10];   /* First 5 characters in name */
    uint8_t  attr;          /* Attribute byte */
    uint8_t  reserved;      /* Unused */
    uint8_t  checksum;      /* Checksum for 8.3 alias */
    char     name5_10[12];  /* 6 more characters in name */
    uint16_t start;         /* Unused */
    char     name11_12[4];  /* Last 2 characters in name */
}dir_slot_t;

typedef struct
{
    // Direcoty temporary storage
    uint8_t* buff;
    uint32_t sector;
    uint32_t dirty;
    // 
    uint32_t firstCluster;
    uint32_t curCluster;
}dir_t;

struct stat
{
    size_t   st_size;    /* total size, in bytes */
    uint32_t st_blksize; /* blocksize for file system I/O */
    uint32_t st_blocks;  /* number of 512B blocks allocated */
};

/* Exported constants ------------------------------------- */
#define EOC     0xffffff8

/* File attributes */
#define ATTR_RO     1
#define ATTR_HIDDEN 2
#define ATTR_SYS    4
#define ATTR_VOLUME 8
#define ATTR_DIR    16
#define ATTR_ARCH   32


/* Exported macros ---------------------------------------- */


/* Exported functions ------------------------------------- */

int32_t Fat32Init(uint32_t fd, uint32_t baseSector, uint8_t* buffer);

int32_t Fat32Mkdir(const char* path);

int32_t Fat32MkFile(const char* path, uint32_t size, const uint8_t* buffer);

int32_t Fat32ReadFile(const char* path, uint8_t* buffer, uint32_t offset, uint32_t size);

#ifdef __cplusplus
    }
#endif

#endif /* _FAT32_H_ */
