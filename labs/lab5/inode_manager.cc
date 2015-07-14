#include "inode_manager.h"

// disk layer -----------------------------------------

disk::disk()
{
  bzero(blocks, sizeof(blocks));
}

void
disk::read_block(blockid_t id, char *buf)
{
  /*
   *your lab1 code goes here.
   *if id is smaller than 0 or larger than BLOCK_NUM 
   *or buf is null, just return.
   *put the content of target block into buf.
   *hint: use memcpy
  */
    if (id < 0 || id >= BLOCK_NUM || buf == NULL)
        return;
    memcpy(buf, blocks[id], BLOCK_SIZE); 
    return;
}

void
disk::write_block(blockid_t id, const char *buf)
{
  /*
   *your lab1 code goes here.
   *hint: just like read_block
  */
    if (id < 0 || id >= BLOCK_NUM || buf == NULL)
        return;
    memcpy(blocks[id], buf, BLOCK_SIZE);
    return;
}

// block layer -----------------------------------------

// Allocate a free disk block.
blockid_t
block_manager::alloc_block()
{
  /*
   * your lab1 code goes here.
   * note: you should mark the corresponding bit in block bitmap when alloc.
   * you need to think about which block you can start to be allocated.

   *hint: use macro IBLOCK and BBLOCK.
          use bit operation.
          remind yourself of the layout of disk.
   */
    // calculate BitMap
    blockid_t firstBitMap = BBLOCK(IBLOCK(sb.ninodes, sb.nblocks));
    blockid_t lastBitMap = BBLOCK(sb.nblocks);
    blockid_t currentBit = IBLOCK(sb.ninodes, sb.nblocks) % (BLOCK_SIZE * 8);
    // store block content
    char* blockContent = (char*)malloc(BLOCK_SIZE * sizeof(char));
    for (blockid_t i = firstBitMap; i < lastBitMap; i++) {
        d->read_block(i, blockContent);
        while (currentBit < BLOCK_SIZE * 8) {
            char byteContent = blockContent[currentBit / 8], tmp = byteContent;
            byteContent |= 1 << (7 - currentBit % 8);
            if (byteContent == tmp)
                currentBit++;
            else {
                blockContent[currentBit / 8] = byteContent;
                d->write_block(i, blockContent);
                delete[] blockContent;
                return (i - 2) * BPB + currentBit;
            }
        }
        currentBit = 0;
    }
    delete[] blockContent;

  return 0;
}

void
block_manager::free_block(uint32_t id)
{
  /* 
   * your lab1 code goes here.
   * note: you should unmark the corresponding bit in the block bitmap when free.
   */
  // free the block and set the flag to zero
  blockid_t bitmapBlock = BBLOCK(id);
  char *bitMapOneBlock = (char *)malloc(BLOCK_SIZE);
  d->read_block(bitmapBlock, bitMapOneBlock);
  uint32_t bufBit = id % (8 * BLOCK_SIZE);
  uint32_t bytePos = bufBit / 8;
  uint32_t bitPos = bufBit - (8 * bytePos);
  char bufByte = bitMapOneBlock[bytePos];
  bufByte = bufByte & (~(0x1 << (7 - bitPos)));
  bitMapOneBlock[bytePos] = bufByte;
  d->write_block(bitmapBlock, bitMapOneBlock);
  delete[] bitMapOneBlock;
  return;
}

// The layout of disk should be like this:
// |<-sb->|<-free block bitmap->|<-inode table->|<-data->|
block_manager::block_manager()
{
  d = new disk();

  // format the disk
  sb.size = BLOCK_SIZE * BLOCK_NUM;
  sb.nblocks = BLOCK_NUM;
  sb.ninodes = INODE_NUM;

}

void
block_manager::read_block(uint32_t id, char *buf)
{
  d->read_block(id, buf);
}

void
block_manager::write_block(uint32_t id, const char *buf)
{
  d->write_block(id, buf);
}

// inode layer -----------------------------------------

inode_manager::inode_manager()
{
  bm = new block_manager();
  uint32_t root_dir = alloc_inode(extent_protocol::T_DIR);
  if (root_dir != 1) {
    printf("\tim: error! alloc first inode %d, should be 1\n", root_dir);
    exit(0);
  }
}

/* Create a new file.
 * Return its inum. */
uint32_t
inode_manager::alloc_inode(uint32_t type)
{
  /* 
   * your lab1 code goes here.
   * note: the normal inode block should begin from the 2nd inode block.
   * the 1st is used for root_dir, see inode_manager::inode_manager().
    
   * if you get some heap memory, do not forget to free it.
   */
    for (uint32_t i = 1; i < INODE_NUM; i++) {
        inode_t* node = get_inode(i);
        if (node == NULL) {
            inode_t inode;
            memset(&inode, 0, sizeof(inode_t));
            inode.type = type;
		time_t currentTime;
      inode.ctime = time(&currentTime);
      inode.mtime = time(&currentTime);
      inode.atime = time(&currentTime);
            put_inode(i, &inode);
            return i;
        }
        else {
            delete[] node;
        }
    }
   return 1;
}

void
inode_manager::free_inode(uint32_t inum)
{
  /* 
   * your lab1 code goes here.
   * note: you need to check if the inode is already a freed one;
   * if not, clear it, and remember to write back to disk.
   * do not forget to free memory if necessary.
   */
    // if freed
    inode_t* inode = get_inode(inum);
    if (inode == NULL)
        return;
    memset(inode, 0, sizeof(inode_t));
    put_inode(inum, inode);
    return;

}


/* Return an inode structure by inum, NULL otherwise.
 * Caller should release the memory. */
struct inode* 
inode_manager::get_inode(uint32_t inum)
{
  struct inode *ino, *ino_disk;
  char buf[BLOCK_SIZE];

  printf("\tim: get_inode %d\n", inum);

  if (inum < 0 || inum >= INODE_NUM) {
    printf("\tim: inum out of range\n");
    return NULL;
  }

  bm->read_block(IBLOCK(inum, bm->sb.nblocks), buf);
  // printf("%s:%d\n", __FILE__, __LINE__);

  ino_disk = (struct inode*)buf + inum%IPB;
  if (ino_disk->type == 0) {
    printf("\tim: inode not exist\n");
    return NULL;
  }

  ino = (struct inode*)malloc(sizeof(struct inode));
  *ino = *ino_disk;

  return ino;
}

void
inode_manager::put_inode(uint32_t inum, struct inode *ino)
{
  char buf[BLOCK_SIZE];
  struct inode *ino_disk;

  printf("\tim: put_inode %d\n", inum);
  if (ino == NULL)
    return;

  bm->read_block(IBLOCK(inum, bm->sb.nblocks), buf);
  ino_disk = (struct inode*)buf + inum%IPB;
  *ino_disk = *ino;
  bm->write_block(IBLOCK(inum, bm->sb.nblocks), buf);
}

#define MIN(a,b) ((a)<(b) ? (a) : (b))

/* Get all the data of a file by inum. 
 * Return alloced data, should be freed by caller. */
void
inode_manager::read_file(uint32_t inum, char **buf_out, int *size)
{
  /*
   * your lab1 code goes here.
   * note: read blocks related to inode number inum,
   * and copy them to buf_out
   */
    inode_t* inode = get_inode(inum);
    if (inode == NULL)
        return;
    *size = inode->size;
    uint32_t blockNum = (*size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    *buf_out = (char*)malloc(blockNum * BLOCK_SIZE);
    uint32_t i = 0;
    for (i = 0; i < MIN(NDIRECT, blockNum); i++)
        bm->read_block(inode->blocks[i], *buf_out + i * BLOCK_SIZE);
    if (i < blockNum) {
        blockid_t* extraBlock = (blockid_t*)malloc(BLOCK_SIZE);
        bm->read_block(inode->blocks[NDIRECT], (char*)extraBlock);
        for (uint32_t i = 0; i < blockNum - NDIRECT; i++) 
            bm->read_block(extraBlock[i], *buf_out + (i + NDIRECT) * BLOCK_SIZE);
        delete[] extraBlock;
    }
}

/* alloc/free blocks if needed */
void
inode_manager::write_file(uint32_t inum, const char *buf, int size)
{
  /*
   * your lab1 code goes here.
   * note: write buf to blocks of inode inum.
   * you need to consider the situation when the size of buf 
   * is larger or smaller than the size of original inode.
   * you should free some blocks if necessary.
   */
    inode_t* inode = get_inode(inum);
    if (inode == NULL) return;
    uint32_t newNum = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    uint32_t oldNum = (inode->size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    // free old
    if (oldNum <= NDIRECT) 
        for (uint32_t i = 0; i < oldNum; i++)
            bm->free_block(inode->blocks[i]);
    else {
        for (uint32_t i = 0; i < NDIRECT; i++)
            bm->free_block(inode->blocks[i]);
        blockid_t* extraBlock = (blockid_t*)malloc(BLOCK_SIZE);
        bm->read_block(inode->blocks[NDIRECT], (char*)extraBlock);
        for (uint32_t i = 0; i < oldNum - NDIRECT; i++)
            bm->free_block(extraBlock[i]);
        bm->free_block(inode->blocks[NDIRECT]);
    }
    // write new
    if (newNum <= NDIRECT) {
        for (uint32_t i = 0; i < newNum; i++) {
            inode->blocks[i] = bm->alloc_block();
            bm->write_block(inode->blocks[i], buf + i * BLOCK_SIZE);
        }
    }
    else {
        for (uint32_t i = 0; i < NDIRECT; i++) {
            inode->blocks[i] = bm->alloc_block();
            bm->write_block(inode->blocks[i], buf + i * BLOCK_SIZE);
        }
        inode->blocks[NDIRECT] = bm->alloc_block();
        blockid_t* extraBlock = (blockid_t*)malloc(BLOCK_SIZE);
        for (uint32_t i = 0; i < newNum - NDIRECT; i++) {
            extraBlock[i] = bm->alloc_block();
            bm->write_block(extraBlock[i], buf + NDIRECT * BLOCK_SIZE + i * BLOCK_SIZE);
        }
        bm->write_block(inode->blocks[NDIRECT], (char*)extraBlock);
    }
    // change inode size
    inode->size = size;
time_t currentTime;
  inode->ctime = time(&currentTime);
  inode->mtime = time(&currentTime);
    put_inode(inum, inode);
    delete inode;
    return;
}

void
inode_manager::getattr(uint32_t inum, extent_protocol::attr &a)
{
  /*
   * your lab1 code goes here.
   * note: get the attributes of inode inum.
   * you can refer to "struct attr" in extent_protocol.h
   */
    inode_t* inode = get_inode(inum);
    if (inode == NULL){
        a.type = a.size = 0;
    }
    else {
        a.type = inode->type;
        a.size = inode->size;
        a.atime = inode->atime;
        a.mtime = inode->mtime;
        a.ctime = inode->ctime;
        delete[] inode;
    }
    return;
}

void
inode_manager::remove_file(uint32_t inum)
{
  /*
   * your lab1 code goes here
   * note: you need to consider about both the data block and inode of the file
   * do not forget to free memory if necessary.
   */
    inode_t* inode = get_inode(inum);
    if (inode == NULL) return;
    uint32_t oldNum = (inode->size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    // free old
    if (oldNum <= NDIRECT) 
        for (uint32_t i = 0; i < oldNum; i++)
            bm->free_block(inode->blocks[i]);
    else {
        for (uint32_t i = 0; i < NDIRECT; i++)
            bm->free_block(inode->blocks[i]);
        blockid_t* extraBlock = (blockid_t*)malloc(BLOCK_SIZE);
        bm->read_block(inode->blocks[NDIRECT], (char*)extraBlock);
        for (uint32_t i = 0; i < oldNum - NDIRECT; i++)
            bm->free_block(extraBlock[i]);
        bm->free_block(inode->blocks[NDIRECT]);
    }
    free_inode(inum);
    return;
}
