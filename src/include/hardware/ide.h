#ifndef IDE_H_
#define IDE_H_

#include <xjos/types.h>
#include <xjos/mutex.h>


#define SECTOR_SIZE 512 

#define IDE_CTRL_NR 2
#define IDE_DISK_NR 2

typedef struct ide_disk_t {
    char name[8];               // disk name
    struct ide_ctrl_t *ctrl;    // ctrl pointer
    u8 selector;                // disk select
    bool master;                // master disk
    u32 total_lba;               // total lba count
    u32 cylinders;               // cylinder count
    u32 heads;                   // head count
    u32 sectors;                 // sector count
}ide_disk_t;

typedef struct ide_ctrl_t {
    char name[8];                   // ctrl name
    mutex_t lock;                   // lock
    u16 iobase;                     // IO reg base
    ide_disk_t disks[IDE_DISK_NR];  // disk
    ide_disk_t *active;             // current select disk
    u8 control;                     // control Byte
    task_t *waiter;          // waiting task
}ide_ctrl_t;

int ide_pio_read(ide_disk_t *disk, void *buf, u8 count, idx_t lba);
int ide_pio_write(ide_disk_t *disk, void *buf, u8 count, idx_t lba);

#endif /* IDE_H_ */