#ifndef _FILESYS_H
#define _FILESYS_H
#include "types.h"




/* @TODO --------------
*   DEFINE filesys struct?
*   DEFINE inode type/struct?
*   DEFINE dentry type/struct
*   DEFINE data block type/struct
*
*/
typedef struct dataBlock{

}dataBlock;

typedef struct bootBlock{

}bootBlock;

typedef struct inode{

}inode;

typedef struct dentry_t{

}dentry_t;

// filesystem
/* Initializes filesystem */
void init_fs();
/*  */
int32_t read_dentry_by_name (const uint8_t fileName, dentry_t* dentry);
/*  */
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
/*  */
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buffer, uint32_t length);

// file operations
/*  */
int32_t file_read();
/*  */
int32_t file_write();
/*  */
int32_t file_open();
/*  */
int32_t file_close();

// dir operations
/*  */
int32_t dir_read();
/*  */
int32_t dir_write();
/*  */
int32_t dir_open();
/*  */
int32_t dir_close();

#endif //_FILESYS_H
