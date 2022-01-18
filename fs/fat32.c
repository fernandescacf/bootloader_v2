/**
 * @file        fat32.c
 * @author      Carlos Fernandes
 * @version     1.0
 * @date        06 March, 2021
 * @brief       Fat32 Driver file
*/


/* Includes ----------------------------------------------- */
#include <fat32.h>
#include <mmc.h>
#include <string.h>
#include <delay.h>


/* Private types ------------------------------------------ */



/* Private constants -------------------------------------- */
#define ATTR_VFAT   (ATTR_RO | ATTR_HIDDEN | ATTR_SYS | ATTR_VOLUME)

#define FATBUFFBLOCKS   4
#define FATBUFFSIZE     (FatData.sectorSize * FATBUFFBLOCKS)

#define DIRBUFFBLOCKS   (FatData.clusterSize)
#define DIRBUFFSIZE     (DIRBUFFBLOCKS * FatData.sectorSize)
#define DIRMAXSIZE      (sizeof(dir_entry_t) * 0x10000)     // 2MB
#define DIRMAXCLUSTERS  (DIRMAXSIZE / (FatData.clusterSize * FatData.sectorSize))


/* Private macros ----------------------------------------- */
#define ROUND_UP_DIV(dividend, divisor)     \
    (((dividend) / (divisor)) + (((dividend) % (divisor)) ? (1) : (0)))

#define TO_UPPER_CASE(c) ((c >= 'a' && c <= 'z') ? (c - 0x20) : (c))


/* Private variables -------------------------------------- */
static struct
{
    // File System Dev
    int32_t  fd;
    // Fat Offset
    uint32_t fatOffset;
    // FAT Buffer
    uint8_t* fatBuff;
    int32_t  fatBuffNum;
    uint32_t fatDirty;
    // Sector Info
    uint32_t totalSectors;
    uint16_t sectorSize;
    // Cluster Info
    uint16_t clusterSize;
    // FAT Area
    uint32_t fatLength;
    uint32_t fatsNum;
    uint32_t fatStartSector;
    uint32_t fatSectors;
    // Root Directory
    uint32_t rootCluster;
    uint32_t rootDirStartSector;
    uint32_t rootDirSectors;
    // Data Area
    uint32_t dataStartSector;
}FatData;

static uint8_t* DirBuffer = NULL;


/* Private function prototypes ---------------------------- */

int32_t Fat32FlushFat(void)
{
    if(FatData.fatDirty)
    {
        mmc_bwrite(FatData.fd, (FatData.fatOffset + FatData.fatStartSector + FatData.fatBuffNum), FATBUFFBLOCKS, FatData.fatBuff);
        FatData.fatDirty = FALSE;
    }

    return E_OK;
}

int32_t Fat32FlushDir(dir_t* dir)
{
    if(dir->dirty == FALSE)
    {
        return E_OK;
    }

    mmc_bwrite(FatData.fd, dir->sector, DIRBUFFBLOCKS, dir->buff);

    dir->dirty = FALSE;

    return E_OK;
}

uint32_t* Fat32ReadFatSector(uint32_t fatSector)
{
    if(fatSector > FatData.fatSectors)
    {
        return NULL;
    }

    if((FatData.fatBuffNum < 0) || (fatSector >= (FatData.fatBuffNum + FATBUFFBLOCKS)) || ((int32_t)fatSector < FatData.fatBuffNum))
    {
        // Do we need to save the FAT buffer
        Fat32FlushFat();

        // Load new sectors to the FAT buffer
        uint32_t loadbase = ((fatSector > FatData.fatSectors - FATBUFFBLOCKS) ? (FatData.fatSectors - FATBUFFBLOCKS) : (fatSector));
        uint32_t offset = (FatData.fatOffset + FatData.fatStartSector + loadbase);

        if(mmc_bread(FatData.fd, offset, FATBUFFBLOCKS, FatData.fatBuff) == 0)
        {
            return NULL;
        }

        FatData.fatBuffNum = loadbase;
    }

    // Return requested sector
    return (uint32_t*)(FatData.fatBuff + (FatData.sectorSize * (fatSector - FatData.fatBuffNum)));
}

void Fat32GetFatEntry(uint32_t cluster, uint32_t* fatSector, uint32_t* fatEntryOffset)
{
    *fatSector = (cluster * 4) / FatData.sectorSize;
    *fatEntryOffset = (cluster * 4) % FatData.sectorSize;
}

uint32_t Fat32ReadEntry(uint32_t cluster)
{
    uint32_t fatSector, fatEntryOffset;

    Fat32GetFatEntry(cluster, &fatSector, &fatEntryOffset);

    uint32_t* buffer = Fat32ReadFatSector(fatSector);

    return buffer[fatEntryOffset / 4] & 0x0FFFFFFF;
}

int32_t Fat32WriteEntry(uint32_t cluster, uint32_t newEntryVal)
{
    uint32_t fatSector, fatEntryOffset;

    Fat32GetFatEntry(cluster, &fatSector, &fatEntryOffset);

    uint32_t* buffer = Fat32ReadFatSector(fatSector);

    uint32_t tmp = buffer[fatEntryOffset / 4];
    tmp = (tmp & 0xF0000000) | (newEntryVal & 0x0FFFFFFF);
    buffer[fatEntryOffset / 4] = tmp;

    FatData.fatDirty = TRUE;

    return E_OK;
}

uint32_t Fat32FirstSectorOfCluster(uint32_t cluster)
{
    return FatData.dataStartSector + (cluster - 2) * FatData.clusterSize;
}

uint32_t Fat32GetNextCluster(uint32_t cluster)
{
   return Fat32ReadEntry(cluster);
}

uint32_t Fat32AllocateCluster(void)
{
    uint32_t sectourCount = FatData.fatSectors;
    uint32_t sector;

    if(FatData.fatBuffNum == -1)
    {
        // In this case we could read the Fs_Info for a hint
        Fat32ReadFatSector(0);
    }

    for(sector = FatData.fatBuffNum; sectourCount > 0; sectourCount-=FATBUFFBLOCKS)
    {
        uint32_t* buffer = (uint32_t*)FatData.fatBuff;
        uint32_t i;
        for(i=0; i < (FATBUFFSIZE / 4); ++i)
        {
            if(buffer[i] == 0)
            {
                // Reserve cluster
                buffer[i] = 1;
                FatData.fatDirty = TRUE;
                return i + (sector * (FatData.sectorSize / 4));
            }
        }

        // Not free cluster found load more sectors
        sector += FATBUFFBLOCKS;
        if(sector >= FatData.fatSectors) sector = 0;
        Fat32ReadFatSector(sector);
    }

    return 0x0FFFFFF7;
}

uint32_t Fat32AllocateDirEntries(dir_t* dir, uint32_t entries)
{
    const uint32_t EntriesPerBuffer = ((FatData.sectorSize * DIRBUFFBLOCKS) / sizeof(dir_entry_t));

    uint32_t i;
    for(i = 0; i < DIRMAXCLUSTERS; ++i)
    {
        // Load dir if not already loaded or load a new one
        if(dir->curCluster == -1  || i > 0)
        {
            // Flush current buffe
            Fat32FlushDir(dir);
            
            // Get next cluster to load
            uint32_t nextCluster = ((dir->curCluster == -1) ? (dir->firstCluster) : (Fat32GetNextCluster(dir->curCluster)));

            // Do we need to allocate a new cluster?
            if(nextCluster >= EOC)
            {
                // Can we allocate more clusters?
                if(i >= (DIRMAXCLUSTERS - 1))
                {
                    return E_ERROR;
                }
                // Allocate new cluster
                nextCluster = Fat32AllocateCluster();
                // Did we got a new cluster
                if(nextCluster == 0x0FFFFFF7) return -1;
                // Set FAT entries
                Fat32WriteEntry(dir->curCluster, nextCluster);
                Fat32WriteEntry(nextCluster, EOC);
            }

            dir->curCluster = nextCluster;
            dir->sector = Fat32FirstSectorOfCluster(dir->curCluster);

            // Load dir cluster
            if(mmc_bread(FatData.fd, dir->sector, DIRBUFFBLOCKS, dir->buff) == 0)
            {
                return -1;
            }
        }

        // Find entries
        dir_entry_t* dir_entry = (dir_entry_t*)dir->buff;
        uint32_t count = 0;
        int32_t first = -1;
        uint32_t j;
        for(j = 0; j < (EntriesPerBuffer - (entries - count)) && count < entries; ++j)
        {
            if(dir_entry[j].name[0] == 0xE5)
            {
                count += 1;
                first = ((first == -1) ? (j) : (first));
            }
            else if(dir_entry[j].name[0] == 0x0)
            {
                count = entries;
                first = ((first == -1) ? (j) : (first));
            }
            else
            {
                count = 0;
                first = -1;
            }
        }

        if(count == entries)
        {
            return first;
        }
    }

    return -1;
}

void Fat32GenSfn(char* sfn, const char* name, uint32_t namelen)
{
    // Find extension: trailing bytes after last '.'
    uint32_t copySize = 8;
    uint32_t extension = namelen;
    int32_t i, j;
    for(i = namelen - 1; i >= 0; --i)
    {
        if(name[i] == '.')
        {
            extension = i;
            break;
        }
    }

    // Write extension
    for(j = i = 3; i > 0; --i)
    {
        if((extension + i) < namelen)
        {
            sfn[7 + j] = TO_UPPER_CASE(name[extension + i]);
            j -= 1;
        }
        else
        {
            sfn[11 - i] = ' ';
        }
    }

    // Check if name is bigger than 8 bytes (max SFN size)
    if(extension > 8)
    {
        sfn[6] = '~';
        sfn[7] = '1';
        // Now we only need to copy 6 bytes
        copySize = 6;
    }

    // Copy name
    for(i = 0; i < copySize; i++)
    {
        sfn[i] = ((i < extension) ? (TO_UPPER_CASE(name[i])) : (' '));
    }
}

uint8_t Fat32ChecksumSfn(const char* sfn)
{
    uint8_t sum;
    uint32_t i;
    for (i = sum = 0; i < 11; i++)
    {
        sum = (sum >> 1) + (sum << 7) + sfn[i];
    }
    return sum;
}

bool_t Fat32WriteLfnName(char* lfn, const char* name, uint32_t size, bool_t end)
{
    uint32_t i;
    for(i = 0; i < (size*2); i+=2)
    {
        if(end == TRUE)
        {
            lfn[i] = 0xFF;
            lfn[i + 1] = 0xFF;
        }
        else
        {
            lfn[i] = name[i >> 1];
            lfn[i + 1] = 0x0;
            if(lfn[i] == 0x0) end = TRUE;
        }
    }

    return end;
}

void Fat32WriteLfnEntry(dir_t* parent, uint32_t entry, const char* name, uint8_t checksum, uint8_t id, bool_t last)
{
    dir_slot_t* lfn = &((dir_slot_t*)parent->buff)[entry];

    lfn->id = ((last) ? (id | 0x40) :(id));
    lfn->checksum = checksum;
    lfn->attr = ATTR_VFAT;
    lfn->start = 0x0;
    lfn->reserved = 0x0;

    // Note: In LFN characters are 16 bytes
    bool_t nameEnded = FALSE;
    // Copy first 5 characters in name
    nameEnded = Fat32WriteLfnName(lfn->name0_4, &name[0], 5, nameEnded);
    // Copy characters 5 to 10 in name
    nameEnded = Fat32WriteLfnName(lfn->name5_10, &name[5], 6, nameEnded);
    // Copy characters 11 to 12 in name
    (void)Fat32WriteLfnName(lfn->name11_12, &name[11], 2, nameEnded);

    // Set dir buffer as dirty
    parent->dirty = TRUE;
}

void Fat32WriteSfnEntry(dir_t* parent, uint32_t entry, uint8_t attr, uint32_t cluster, uint32_t size, const char* name)
{
    dir_entry_t* sfn = &((dir_entry_t*)parent->buff)[entry];

    sfn->attr = attr;
    sfn->startlo = (cluster & 0xFFFF);
    sfn->starthi = ((cluster >> 16) & 0xFFFF);
    sfn->size = size;
    sfn->nt = 0x0;

    // Copy name
    memcpy(sfn->name, name, sizeof(char) * 11);

    // For now set date info to zero
    memset(&sfn->ctime_ms, 0x0, sizeof(uint8_t) * 7);
    memset(&sfn->time, 0x0, sizeof(uint8_t) * 4);

    // Set dir buffer as dirty
    parent->dirty = TRUE;
}

dir_entry_t* Fat32WriteFile(dir_t* parent, const char* name, uint32_t size, uint8_t attr, const uint8_t* buffer)
{
    // Note 1: Private Function so no sanity checking is required
    // Note 2: It is not required that size is a multiple of sector size (sd/mmc block size)
    // Note 2: but the buffer size has to be a multiple of sector size

    // Check how many dir entries we need
    uint32_t len = strlen(name);
    // slots = 1 SFN + RoundUp(len / 13) LFN
    uint32_t slots = 1 + ROUND_UP_DIV(len, 13);

    // Check how many clusters we need
    // Size is in bytes so we need to dived by cluster size in bytes
    uint32_t clusters = ROUND_UP_DIV(size, FatData.clusterSize * FatData.sectorSize);
    // Ensure that we have at least one cluster
    if(clusters == 0) clusters = 1;

    // We write in blocks of sector size, so we need to know how many sector we will write
    uint32_t sectors  = ROUND_UP_DIV(size, FatData.sectorSize);

    // Now we need to allocate the required dir entries from parent directory
    uint32_t entry = Fat32AllocateDirEntries(parent, slots);

    // At this point parent has buffer allocated for us to write the dir entries

    // First we create the SFN (short file name) and get the checksum
    char sfn[12];
    Fat32GenSfn(sfn, name, len);
    uint8_t checksum = Fat32ChecksumSfn(sfn);

    // Create LFN (Long File Name) entries
    int32_t i;
    for(i = 0; i < (slots - 1); ++i)
    {
        Fat32WriteLfnEntry(parent, entry + i, &name[13 * (slots - 2 - i)], checksum, slots - (i + 1), (i == 0));
    }

    // Allocate all needed clusters
    uint32_t firstCluster = Fat32AllocateCluster();
    uint32_t lastCluster = firstCluster;

    // Write file buffer to cluster
    if(sectors > 0)
    {
        uint32_t blocks = ((clusters > 1) ? (FatData.clusterSize) : (sectors));
        mmc_bwrite(FatData.fd, Fat32FirstSectorOfCluster(firstCluster), blocks, buffer);
        sectors -= blocks;
    }

    for(i = 1; i < clusters; ++i)
    {
        uint32_t newCluster = Fat32AllocateCluster();
        // Write file buffer to cluster
        uint32_t blocks = ((i < (clusters - 1)) ? (FatData.clusterSize) : (sectors));
        mmc_bwrite(FatData.fd, Fat32FirstSectorOfCluster(newCluster), blocks, &buffer[i * FatData.clusterSize * FatData.sectorSize]);
        sectors -= blocks;
        // Set new cluster as next cluster in FAT
        Fat32WriteEntry(lastCluster, newCluster);
        lastCluster = newCluster;
    }

    Fat32WriteEntry(lastCluster, EOC);

    // Creat SFN entry
    Fat32WriteSfnEntry(parent, entry + (slots - 1), attr, firstCluster, size, sfn);

    // Flush Dir to permanent memory
    Fat32FlushDir(parent);

    // Flush FAT
    Fat32FlushFat();

    return &((dir_entry_t*)parent->buff)[entry + (slots - 1)];
}

int32_t Fat32ParseEntry(dir_entry_t* entry, char* lbuffer, char* sbuffer, dir_entry_t** next_entry)
{
    uint32_t sfn_entry = 0;
    *lbuffer = 0x0;
    *sbuffer = 0x0;
    *next_entry = NULL;

    // Do we need to parse a Long File Name
    if(entry[0].attr == ATTR_VFAT)
    {
        dir_slot_t* slot = (dir_slot_t*)&entry[0];
        if((slot->id & 0x40) == 0)
        {
            return E_ERROR;
        }

        uint32_t seq = sfn_entry = (slot->id & 0x3F);
        uint8_t checksum = slot->checksum;

        if(Fat32ChecksumSfn(entry[sfn_entry].name) != checksum)
        {
            return E_ERROR;
        }

        for(; seq > 0; --seq, ++slot)
        {
            if(slot->checksum != checksum || (slot->id & 0x3F) != seq)
            {
                return E_ERROR;
            }

            uint32_t shift = 13 * (seq - 1);

            lbuffer[0 + shift]  = slot->name0_4[0];
            lbuffer[1 + shift]  = slot->name0_4[2];
            lbuffer[2 + shift]  = slot->name0_4[4];
            lbuffer[3 + shift]  = slot->name0_4[6];
            lbuffer[4 + shift]  = slot->name0_4[8];
            lbuffer[5 + shift]  = slot->name5_10[0];
            lbuffer[6 + shift]  = slot->name5_10[2];
            lbuffer[7 + shift]  = slot->name5_10[4];
            lbuffer[8 + shift]  = slot->name5_10[6];
            lbuffer[9 + shift]  = slot->name5_10[8];
            lbuffer[10 + shift] = slot->name5_10[10];
            lbuffer[11 + shift] = slot->name11_12[0];
            lbuffer[12 + shift] = slot->name11_12[2];
        }
    }

    // Get Short File Name
    uint32_t i;
    for(i = 0; i < 8 && entry[sfn_entry].name[i] != ' '; ++i)
    {
        sbuffer[i] = entry[sfn_entry].name[i];
    }

    if(entry[sfn_entry].ext[2] != ' ')
    {
        sbuffer[i] = '.';
        i += 1;
    }

    uint32_t j;
    for(j = 0; j < 3; ++j)
    {
        if(entry[sfn_entry].ext[j] != ' ')
        {
            sbuffer[i] = entry[sfn_entry].ext[j];
            i += 1;
        }
    }

    for(; i < 12; ++i)
    {
        sbuffer[i] = 0;
    }

    *next_entry = &entry[sfn_entry + 1];

    return E_OK;
}

int32_t Fat32SearchDir(dir_t* dir, const char* name, uint32_t size, dir_entry_t** entry)
{
    const uint32_t EntriesPerBuffer = ((FatData.sectorSize * DIRBUFFBLOCKS) / sizeof(dir_entry_t));

    *entry = NULL;

    uint32_t i;
    for(i = 0; i < DIRMAXCLUSTERS; ++i)
    {
        // Load dir if not already loaded or load a new one
        if(dir->curCluster == -1  || i > 0)
        {
            // Flush current buffe
            Fat32FlushDir(dir);
            
            // Get next cluster to load
            uint32_t nextCluster = ((dir->curCluster == -1) ? (dir->firstCluster) : (Fat32GetNextCluster(dir->curCluster)));
            
            // Do we need to allocate a new cluster?
            if(nextCluster >= EOC)
            {
                return E_SRCH;
            }

            dir->curCluster = nextCluster;
            dir->sector = Fat32FirstSectorOfCluster(dir->curCluster);

            // Load dir cluster
            if(mmc_bread(FatData.fd, dir->sector, DIRBUFFBLOCKS, dir->buff) == 0)
            {
                return -1;
            }
        }
        
        dir_entry_t* dir_entry  = (dir_entry_t*)dir->buff;
        dir_entry_t* next_entry = NULL;
        char lfn[256];
        char sfn[12];

        while(dir_entry < (dir_entry_t*)&dir->buff[EntriesPerBuffer * sizeof(dir_entry_t)] && dir_entry->name[0] != 0x0)
        {
            if(dir_entry->name[0] == 0xE5 /*|| (dir_entry->attr & (ATTR_DIR | ATTR_ARCH)) == 0*/)
            {
                dir_entry += 1;
                continue;
            }

            if(Fat32ParseEntry(dir_entry, lfn, sfn, &next_entry) != E_OK)
            {
                //return E_ERROR;
                dir_entry += 1;
                continue;
            }

            // Ensure we compare the terminator /0
            if(memcmp(name, lfn, size + 1) == 0 || (size < 13 && memcmp(name, sfn, size) == 0))
            {
                *entry = next_entry - 1;
                return E_OK;
            }

            dir_entry = next_entry;
        }
    }

    return E_SRCH;
}

int32_t Fat32ResolvePath(dir_t* parent, const char* path, char** remaining, dir_t* dir, dir_entry_t* entry)
{
    char* ptr1 = (char*)path;

    if(parent != NULL)
    {
        memcpy(dir, parent, sizeof(dir_t));
    }
    else
    {
        if(dir->buff == NULL) dir->buff = DirBuffer;
        dir->sector = FatData.dataStartSector;
        dir->dirty  = FALSE;
        dir->firstCluster = FatData.rootCluster;
        dir->curCluster   = dir->firstCluster;
        if(mmc_bread(FatData.fd, dir->sector, DIRBUFFBLOCKS, dir->buff) == 0)
        {
            return E_ERROR;
        }

        // We need at least one '/'
        if(*ptr1 != '/')
        {
            *remaining = ptr1;
            return E_INVAL;
        }
    }

    while(1)
    {
        // Skip any '/'
        while(*ptr1 == '/') ptr1++;

        // Find next '/' or '/0'
        uint32_t len;
        for(len = 0; ptr1[len] != '/'; ++len)
        {
            // If no more '/' we are done
            if(ptr1[len] == 0x0)
            {
                if(len > 0) break;

                *remaining = ptr1;
                return E_OK;
            }
        }

        // Search current directory for next path entry
        dir_entry_t* dir_entry;
        if(Fat32SearchDir(dir, ptr1, len - 1, &dir_entry) != E_OK)
        {
            *remaining = ptr1;
            return E_SRCH;
        }

        // We found a dir move to it
        dir->firstCluster = (dir_entry->starthi << 16) | (dir_entry->startlo);
        dir->curCluster = dir->firstCluster;
        dir->sector = Fat32FirstSectorOfCluster(dir->curCluster);
        mmc_bread(FatData.fd, dir->sector, DIRBUFFBLOCKS, dir->buff);
        dir->dirty = FALSE;

        if(entry != NULL)
        {
            memcpy(entry, dir_entry, sizeof(dir_entry_t));
        }

        ptr1 += len;
    }
}

int32_t Fat32IsNameValid(const char* name)
{
    int32_t len = strlen(name);

    if(len > 255)
    {
        return E_INVAL;
    }

    for(len -= 1; len >= 0; --len)
    {
        if( name[len] < 0x20  || name[len] == 0x22 || name[len] == 0x2A ||
            name[len] == 0x2B || name[len] == 0x2C || /*name[len] == 0x2E ||*/
            name[len] == 0x2F || name[len] == 0x3A || name[len] == 0x3B ||
            name[len] == 0x3C || name[len] == 0x3D || name[len] == 0x3E ||
            name[len] == 0x3F || name[len] == 0x5B || name[len] == 0x5C ||
            name[len] == 0x5D || name[len] == 0x7C)
        {

            return E_INVAL;
        }
    }

    return E_OK;
}

int32_t Fat32LoadNextCluster(dir_t* dir)
{
    // Flush current cluster
    Fat32FlushDir(dir);
    
    // Get next cluster to load
    uint32_t nextCluster = ((dir->curCluster == -1) ? (dir->firstCluster) : (Fat32GetNextCluster(dir->curCluster)));
    
    // Did we reach the last cluster?
    if(nextCluster >= EOC)
    {
        return E_NO_RES;
    }

    dir->curCluster = nextCluster;
    dir->sector = Fat32FirstSectorOfCluster(dir->curCluster);

    // Load next cluster
    if(mmc_bread(FatData.fd, dir->sector, DIRBUFFBLOCKS, dir->buff) == 0)
    {
        return E_ERROR;
    }

    return E_OK;
}

/* Private functions -------------------------------------- */

int32_t Fat32Init(uint32_t fd, uint32_t baseSector, uint8_t* buffer)
{
    FatData.fd = fd;

    // Read Boot Sector and BPB
    if(mmc_bread(FatData.fd, baseSector, 1, buffer) == 0)
    {
        return E_ERROR;
    }

    boot_sector_t* boot_sector = (boot_sector_t*)buffer;
//    volume_info_t* volume_info = (volume_info_t*)&buffer[sizeof(*boot_sector)];

    // Fat Offset
    FatData.fatOffset = boot_sector->hidden;

    // FAT Buffer
    FatData.fatBuff = NULL;
    FatData.fatBuffNum = -1;
    FatData.fatDirty = FALSE;
    // Sector Info
    FatData.totalSectors = boot_sector->total_sect;
    FatData.sectorSize = boot_sector->sector_size;
    // Cluster Info
    FatData.clusterSize = boot_sector->cluster_size;
    // FAT Area
    FatData.fatLength = boot_sector->fat32_length;
    FatData.fatsNum = boot_sector->fats;
    FatData.fatStartSector = boot_sector->reserved;
    FatData.fatSectors = FatData.fatLength * FatData.fatsNum;
    // Root Directory
    FatData.rootDirStartSector = FatData.fatStartSector + FatData.fatSectors;
    FatData.rootDirSectors = 0; //((32 * boot_sector->dir_entries) + (FatData.sectorSize - 1)) / FatData.sectorSize;
    FatData.rootCluster = boot_sector->root_cluster;
    // Data Area
    FatData.dataStartSector = (FatData.rootDirStartSector + FatData.rootDirSectors) + FatData.fatOffset;
    // FAT sub-type
    delay_us(5);    // Workaround for weird crash before performing divisions
    if(((FatData.totalSectors - FatData.dataStartSector) / FatData.clusterSize) < 65526)
    {
        // Invalid type
        return E_ERROR;
    }
    // Set Buffers
    FatData.fatBuff = buffer;
    DirBuffer = buffer + FATBUFFSIZE;

    return E_OK;
}

int32_t Fat32Mkdir(const char* path)
{
    char* remaining = NULL;
    dir_t parent = {0};

    // Resolve path and get parent dir
    Fat32ResolvePath(NULL, path, &remaining, &parent, NULL);

    // Check for unsupported charecters on remaining path
    if(remaining == NULL || Fat32IsNameValid(remaining) != E_OK)
    {
        // Invalid path
        return E_INVAL;
    }
    
    // Set a buffer for the parent directory
    // Already done on Fat32ResolvePath

    dir_entry_t* new_dir_entry = Fat32WriteFile(&parent, remaining, 0x0, ATTR_DIR, NULL);

    if(new_dir_entry == NULL)
    {
        // Failed to create directory
        return E_ERROR;
    }

    Fat32FlushDir(&parent);

    dir_t dir;
    dir.firstCluster = (new_dir_entry->starthi << 16) | (new_dir_entry->startlo);
    dir.curCluster = dir.firstCluster;
    dir.sector = Fat32FirstSectorOfCluster(dir.curCluster);
    dir.buff = DirBuffer;
    mmc_bread(FatData.fd, dir.sector, DIRBUFFBLOCKS, dir.buff);
    dir.dirty = FALSE;

    // Make directory ".."
    Fat32WriteSfnEntry(&dir, 0, ATTR_DIR, ((parent.firstCluster == -1) ? (FatData.rootCluster) : (parent.firstCluster)), 0x0, "..          ");

    // Make directory "."
    Fat32WriteSfnEntry(&dir, 1, ATTR_DIR, dir.firstCluster, 0x0, ".           ");

    Fat32FlushDir(&dir);

    return E_OK;
}

int32_t Fat32MkFile(const char* path, uint32_t size, const uint8_t* buffer)
{
    dir_t parent = {0};

    // Resolve path and get parent dir
    char* remaining = NULL;
    Fat32ResolvePath(NULL, path, &remaining, &parent, NULL);

    // Check for unsupported charecters on remaining path
    if(remaining == NULL || Fat32IsNameValid(remaining) != E_OK)
    {
        // Invalid directory name
        return E_INVAL;
    }
    
    // Set a buffer for the parent directory
    // Already done on Fat32ResolvePath

    dir_entry_t* new_dir_entry = Fat32WriteFile(&parent, remaining, size, ATTR_ARCH, buffer);

    if(new_dir_entry == NULL)
    {
        // Failed to create directory
        return E_ERROR;
    }

    Fat32FlushDir(&parent);

    return E_OK;
}

int32_t Fat32ReadFile(const char* path, uint8_t* buffer, uint32_t offset, uint32_t size)
{
    (void)offset;

    dir_t dir = {0};
    dir_entry_t entry = {0};

    // Resolve path and get parent dir
    char* remaining = NULL;
    if(Fat32ResolvePath(NULL, path, &remaining, &dir, &entry) != E_OK)
    {
        return 0;
    }

    if(size == (uint32_t)-1) size = entry.size;

    uint32_t read = 0;
    while(read < size)
    {
        uint32_t readSize = (((size - read) > DIRBUFFSIZE) ? (DIRBUFFSIZE) : (size - read));

        if((readSize + read) > entry.size) readSize = entry.size - read;

        memcpy(&buffer[read], dir.buff, DIRBUFFSIZE);

        read += readSize;

        if(Fat32LoadNextCluster(&dir) != E_OK)
        {
            return read;
        }
    }

    return read;
}

int32_t Fat32Stat(const char* path, struct stat* stat)
{
    dir_t dir = {0};
    dir_entry_t entry = {0};

    // Resolve path and get parent dir
    char* remaining = NULL;
    if(Fat32ResolvePath(NULL, path, &remaining, &dir, &entry) != E_OK)
    {
        return E_INVAL;
    }

    stat->st_size = entry.size;
    stat->st_blksize = FatData.sectorSize;
    stat->st_blocks = (stat->st_size / stat->st_blksize);

    return E_OK;
}