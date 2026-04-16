#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include "bf.h"
#include "hp_file.h"
#include "record.h"

#define CALL_BF(call)       \
{                           \
  BF_ErrorCode code = call; \
  if (code != BF_OK)        \
  {                         \
    BF_PrintError(code);    \
    return HP_ERROR;        \
  }                         \
}


int HP_CreateFile(char *fileName)
{
  //we create the file
  CALL_BF(BF_CreateFile(fileName));

  int fd; //our file descriptor
  CALL_BF(BF_OpenFile(fileName, &fd));

  BF_Block *block;
  BF_Block_Init(&block);

  CALL_BF(BF_AllocateBlock(fd, block));

  //prepare the metadata (HP_info) for the first block of the heap file
  HP_info hpInfo;
  memset(&hpInfo, 0, sizeof(HP_info));
  hpInfo.maxRecordsPerBlock = (BF_BLOCK_SIZE - sizeof(HP_block_info)) / sizeof(Record);

  //memcpy the metadata into the first block
  char *data = BF_Block_GetData(block);
  memcpy(data, &hpInfo, sizeof(HP_info));
  BF_Block_SetDirty(block);

  //Finally, we unpin the block and close the file
  CALL_BF(BF_UnpinBlock(block));

  BF_Block_Destroy(&block);
  CALL_BF(BF_CloseFile(fd));

  return 0;
}

HP_info* HP_OpenFile(char *fileName, int *file_desc)
{
  BF_Block* infoBlock = NULL;
  char* infoData = NULL;
  HP_info* hpInfo;    
  CALL_BF(BF_OpenFile(fileName, file_desc));


  BF_Block_Init(&infoBlock);

  CALL_BF(BF_GetBlock(*file_desc, 0, infoBlock)); //it pins the block in the memory
  infoData = BF_Block_GetData(infoBlock); //get the data from the first block

  hpInfo = (HP_info*) infoData;
  hpInfo->infoblock = infoBlock;
  hpInfo->infodata = infoData;

  //we return the info we got from the first block
  return hpInfo;
}

int HP_CloseFile(int file_desc, HP_info* hp_info )
{
  if(hp_info->curBlock != NULL)
  {
    CALL_BF(BF_UnpinBlock(hp_info->curBlock));
    hp_info->curBlock = NULL;
    hp_info->curdata = NULL;
    hp_info->curBlockInfo = NULL;
    hp_info->curBlockpos = 0;
    BF_Block_Destroy(&(hp_info->curBlock));
  }

  BF_Block* infoBlock = hp_info->infoblock;
  hp_info->infoblock = NULL;
  hp_info->infodata = NULL;

  CALL_BF(BF_UnpinBlock(infoBlock));
  BF_Block_Destroy(&infoBlock);

  CALL_BF(BF_CloseFile(file_desc));
  hp_info = NULL;

  return 0;
}

int HP_InsertEntry(int file_desc, HP_info* hp_info, Record record)
{
  //do we have room in the last block?
  int capacity = hp_info->maxRecordsPerBlock * hp_info->totalBlocks;
  if (capacity < hp_info->totalRecords + 1)
  {
    if (hp_info->curBlock != NULL)
    {
      CALL_BF(BF_UnpinBlock(hp_info->curBlock));
      hp_info->curBlock = NULL;
      hp_info->curdata = NULL;
      hp_info->curBlockInfo = NULL;
      hp_info->curBlockpos = 0;
    }

    //θελουμε νεο block
    if(hp_info->curBlock == NULL)
      BF_Block_Init(&hp_info->curBlock);

    CALL_BF(BF_AllocateBlock(file_desc, hp_info->curBlock));
    hp_info->curdata = BF_Block_GetData(hp_info->curBlock);

    hp_info->totalBlocks++;
    hp_info->lastBlockID++;
    hp_info->curBlockInfo = (HP_block_info*)(hp_info->curdata + (BF_BLOCK_SIZE - sizeof(HP_block_info)));
  }

  Record* rec = hp_info->curdata;
  rec[(hp_info->curBlockpos)++] = record;
  hp_info->curBlockInfo->numOfRecords++;
  BF_Block_SetDirty(hp_info->curBlock);

  hp_info->totalRecords++;
  memcpy(hp_info->infodata, hp_info, sizeof(HP_info));
  BF_Block_SetDirty(hp_info->infoblock);

  return hp_info->lastBlockID;  // Success
}

int HP_GetAllEntries(int file_desc, HP_info* hp_info, int id)
{
  HP_block_info *blockInfo = NULL;
  Record *recordArray = NULL;

  int found = 0;

  //search ALL the heap file
  for(int idx = 1; idx <= hp_info->totalBlocks; idx++)
  {
    if(hp_info->curBlock != NULL)
      CALL_BF(BF_UnpinBlock(hp_info->curBlock));
      
    if(hp_info->curBlock == NULL)
      BF_Block_Init(&hp_info->curBlock);

    CALL_BF(BF_GetBlock(file_desc, idx, hp_info->curBlock));

    hp_info->curdata = BF_Block_GetData(hp_info->curBlock);

    blockInfo = (HP_block_info*)(hp_info->curdata + (BF_BLOCK_SIZE - sizeof(HP_block_info)));
  
    recordArray = (Record *)hp_info->curdata;

    // Traverse through records in each block
    for (int i = 0; i < blockInfo->numOfRecords; i++)
    {
      if (recordArray[i].id == id)
      {
        // Print the matching record
        printf("|->Record found: ID: %d, Name: %s, Surname: %s, City: %s\n",
                recordArray[i].id, recordArray[i].name, recordArray[i].surname, recordArray[i].city);
        
        //found record!!
        found = idx;
      }
    }

  }

  if(found != 0)
    return found; //returns the num of blocks been read

  return -1; //not found
}