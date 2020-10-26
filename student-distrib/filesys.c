#include "filesys.h"

/*
*   @TODO ----------------
*   FUNCTIONS: ret type - int32_t, -1 on failure
*       FILESYS:
*           read_dentry_by_name();
*           read_dentry_by_index();
*           read_data();
*       FILES:
*           file_read();
*           file_write();
*           file_open();
*           file_close();
*       DIRS:
*           dir_read();
*           dir_write();
*           dir_open();
*           dir_close();
*/

/* local variables */
uint32_t filesys_addr;    /* Address of filesys in memory          */
boot_block_t boot_block;  /* Local copy of boot_block from filesys */
uint32_t dr_index;        /* Indexing variable of files in filesys */

/*
 * init_filesys
 * DESCRIPTION: sets up local variables associated with filesys
 * INPUTS: none
 * OUTPUTS: none
 * RETURNS: none
 * SIDE EFFECTS: initializes local variables
 */
void
init_filesys(uint32_t fs_addr)
{
    filesys_addr = fs_addr;
    boot_block = *((boot_block_t*)filesys_addr);
    dr_index = 0;
}

/*
 * read_dentry_by_name
 * DESCRIPTION: copies matching dentry data into location of pointer
 * INPUTS: fname  -- the name of the file
 *         dentry -- the location at which to save dentry data
 * OUTPUTS: data to dentry location
 * RETURNS: -1 if failure, 0 if success
 * SIDE EFFECTS: none
 */
int32_t
read_dentry_by_name (const uint8_t* fname, dentry_t* dentry)
{
    /* Purposefully not checking if fname input is too long */

    if (fname == NULL || dentry == NULL) return -1;
    if (fname[0] == '\0') return -1;  /* empty string */

    int i;    /* looping variable */
    int j;    /* looping variable */

    for (i = 0; i < boot_block.inode_count; i++) {
        for (j = 0; j < FNAME_MAX_LEN; j++) {
            /* if two characters don't match before the end,    */
            /*   there's no match                               */
            if (boot_block.dentries[i].name[j] != fname[j]) break;

            /* if we reach EOS at the same time in both strings */
            /*     OR                                           */
            /*   we have reached the end of the string and we   */
            /*   we haven't broken out in previous conditional  */
            /*   .... then we match                             */
            if (((boot_block.dentries[i].name[j] | fname[j]) == '\0')
                    || (j == FNAME_MAX_LEN - 1))
            {
                *dentry = boot_block.dentries[i];
                return 0;
            }
        }
    }

    /* We got to the end of all entries and didn't find it */
    return -1;

}

/*
 * read_dentry_by_index
 * DESCRIPTION: copies data to dentry location based on given dentry index
 * INPUTS: index  -- index of dentry in boot block
 *         dentry -- location of dentry file
 * OUTPUTS: data to dentry location
 * RETURNS: -1 if failure, 0 if success
 * SIDE EFFECTS: none
 */
int32_t
read_dentry_by_index (uint32_t index, dentry_t* dentry)
{
    /* Only fails if index is out of bounds */
    if (index < 0 || index >= boot_block.dentry_count) return -1;
    if (dentry == NULL) return -1;

    /* valid, so copy it in */
    *dentry = boot_block.dentries[index];
    return 0;
}

/*
 * read_data
 * DESCRIPTION: copies data from inode in filesys.img to buffer
 * INPUTS: inode  -- the inode index
 *         offset -- the byte offset within the file at which to start copying
 *         buffer -- the location of the byte buffer to which to copy
 *         length -- the number of bytes to copy
 * OUTPUTS: file data to buffer
 * RETURNS: -1 if failure, otherwise the number of bytes copied
 * SIDE EFFECTS: none
 */
int32_t
read_data (uint32_t inode_i, uint32_t offset, uint8_t* buffer, uint32_t length)
{
    // FYI
    // This function can only check that the given inode is within the
    // valid range. It does not check that the inode actually
    // corresponds to a file.
    if (inode_i < 0 || inode_i >= boot_block.inode_count) return -1;
    if (buffer == NULL) return -1;

    inode_t inode = ((inode_t*)(filesys_addr + BLOCK_SIZE))[inode_i];
    data_block_t* data_blocks = (data_block_t*)(filesys_addr + BLOCK_SIZE*(1 + boot_block.inode_count));
    uint32_t byte_count; /* indexing variable */

    /* Not the most efficient way. To avoid this many operations each    */
    /*   cycle, we can do three separate loops:                          */
    /*     1. copy bytes until the end of the first data block           */
    /*     2. copy data from as many data blocks as we have length       */
    /*        divided by 4096                                            */
    /*     3. copy data from one more block until we reach length        */
    /* But we're not doing this... do this for now                       */
    for (byte_count = 0; (byte_count + offset < length) && (byte_count + offset < inode.length); byte_count++) {
        /* Don't *have* to do this every time, so this is lazy way */
        buffer[byte_count] = data_blocks[inode.data_indices[(byte_count + offset) >> BLOCK_SIZE_LOG_2]]
                                .data[(byte_count + offset) & BLOCK_MASK];
    }

    return byte_count;
}

/*
 * directory_read
 * DESCRIPTION: Reads the next file in the filesys_img
 * INPUTS: buffer in which to put the name
 * OUTPUTS: none
 * RETURNS: 0 if success, -1 if end of directory reached.
 *          Should never fail.
 * SIDE EFFECTS: increments dr_index
 */
uint32_t
directory_read(uint8_t buf[FNAME_MAX_LEN + 1])
{
    dentry_t dentry;
    int i; /* character read index */
    buf[FNAME_MAX_LEN] = '\0';    /* make sure we include a failsafe EOS */
    int read_result = read_dentry_by_index(dr_index++, &dentry);

    if (read_result < 0) {
        /* Reached the end of dentries */
        dr_index = 0;
        return -1;
    }
    for (i = 0; (i < FNAME_MAX_LEN) && (dentry.name[i - 1] != '\0'); i++) {
        buf[i] = (uint8_t)(dentry.name[i]);
    }
    return 0;
}

/*
 * file_read
 * DESCRIPTION: Reads the next file in the filesys_img
 * INPUTS: buffer in which to put the data
 * OUTPUTS: none
 * RETURNS: 0 if success, -1 if end of directory reached.
 *          Should never fail.
 * SIDE EFFECTS:
 */
uint32_t file_read(uint8_t file_name, uint8_t* buf, uint32_t num_bytes)
{
    dentry_t* dentry;
    // use file_name to read from dentry
    uint8_t* file = read_directory();
    while(file != file_name)
    {
        file = read_directory();
        if (file == NULL)
        {
            printf("There is no such file");
            return NULL;
        }
    }

    int32_t dentry_loc = read_dentry_by_name(file, dentry);
    int32_t data_read = read_data(dentry_loc->inode_index, offset, buf, num_bytes);

}
