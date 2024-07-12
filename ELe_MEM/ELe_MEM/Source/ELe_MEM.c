
// TODO: 
/*
* Make passthrough functions for anything that might take in a literal.
* Move platform definitions into this file.
* Rename non-call functions with starting underscores.
* See if possible to rework the ELe_Alloc function into a non-macro call.
* Rework the block info retrieval system.
*/


#include "../Header/ELe_PLAT.h"

#include <stdio.h>
#include <stdlib.h>

#define ELe_RESIZE_FACTOR 2

typedef struct ELe_State
{
    l_bool                      initialized;
    l_uint64                    totalAllocated;
} ele_state;

typedef struct ELe_Alloc_Tracker
{
    void*                       pAllocElement;
    struct ELe_Alloc_Tracker*   pNextElement;
} ele_tracker;

typedef struct ELe_Memory_Tag
{
    const char*                 tag;
    l_uint32                    enumeration;
    l_uint64                    allocated;
} mem_tag;

typedef struct ELe_Allocation_Element_Meta
{
    mem_tag*        tag;
    l_uint64        size;
    l_uint64        len;
    l_uint64        slots;
} ele_set;

static ele_state        state                   = {0};
static ele_tracker*     pAllocationTracker      = {0};
static mem_tag*         memTagSet               = {0};

#ifdef ELe_WIN32
#include <Windows.h>

void*
ELe_Plat_Alloc(l_uint64 size, l_bool alligned)
{
    return malloc(size);
}

void
ELe_Plat_Free(void* block)
{
    free(block);
}

#endif // ELe_WIN32

l_uint64
ELe_Register_Default_Tags(l_uint32 defaultTagCount, l_uint32 tagCount)
{
    const char* defaultTags[] =
    {
        "ELe_TAG_LIST",
        "ELe_UNTAGGED",
    };
    l_uint64 netTagLen = 0;
    for(l_uint32 i = 0; i < defaultTagCount; i++)
    {
        l_uint32 tagLen = 0;
        while (*(defaultTags[i] + tagLen) != '\0')
        {
            tagLen++;
        } tagLen++;
        netTagLen += tagLen;
        char* tagAllocation = ELe_Plat_Alloc(tagLen, FALSE);
        (memTagSet + (tagCount - (i + 1)))->allocated = 0;
        (memTagSet + (tagCount - (i + 1)))->enumeration = tagCount - (i + 1);
        (memTagSet + (tagCount - (i + 1)))->tag = tagAllocation;
        do{
            *(tagAllocation + tagLen) = *(defaultTags[i] + tagLen);
        } while(tagLen--);
    }
    return netTagLen;
}

l_uint64
ELe_Register_Unique_Tags(l_uint32 uniqueTagCount, const char* memTagArray[])
{
    l_uint64 netTagLen = 0;
    for(l_uint32 i = 0; i < uniqueTagCount; i++)
    {
        l_uint32 tagLen = 0;
        while (*(memTagArray[i] + tagLen) != '\0')
            tagLen++;
        tagLen++;
        netTagLen += tagLen;
        char* tagAllocation = ELe_Plat_Alloc(tagLen, FALSE);
        (memTagSet + i)->allocated = 0;
        (memTagSet + i)->enumeration = i;
        (memTagSet + i)->tag = tagAllocation;
        do{
            *(tagAllocation + tagLen) = *(memTagArray[i] + tagLen);
        } while(tagLen--);
    }
    return netTagLen;
}

void
ELe_Register_Memory_Tags(const char* memTagArray[], l_uint64 uniqueTagCount)
{
    l_uint32 defaultTagCount = 2;
    l_uint32 totalTagCount = uniqueTagCount + defaultTagCount;
    l_uint64 spaceReq = (sizeof(mem_tag) * totalTagCount) + sizeof(ele_set);
    void* startTarget = memTagSet = ELe_Plat_Alloc(spaceReq, FALSE);
    startTarget += sizeof(ele_set);

    ((ele_set*)memTagSet)->len = totalTagCount;
    ((ele_set*)memTagSet)->size = spaceReq;
    ((ele_set*)memTagSet)->tag = (memTagSet + uniqueTagCount);
    memTagSet = startTarget;

    l_uint64 netTagLen = ELe_Register_Default_Tags(defaultTagCount, totalTagCount);
    if(memTagArray)
        netTagLen += ELe_Register_Unique_Tags(uniqueTagCount, memTagArray);

    (memTagSet + totalTagCount - 1)->allocated = (netTagLen + spaceReq);
    state.totalAllocated += (netTagLen + spaceReq);

    return;
}

void
ELe_Free_Memory_Tags()
{
    void* tagSetStart = memTagSet;
    tagSetStart -= sizeof(ele_set);
    l_uint32 uniqueTagCount = ((ele_set*)tagSetStart)->len - 2;
    for(l_uint32 i = 0; i < uniqueTagCount; i ++)
    {
        ELe_Plat_Free((void*)(memTagSet + i)->tag);
        (memTagSet + i)->tag = NULL;
    }
    ELe_Plat_Free(tagSetStart);
    tagSetStart = NULL;
    return;
}

void
ELe_Initialize(const char* tagStringArray[], l_uint64 tagCount)
{
    ELe_Register_Memory_Tags(tagStringArray, tagCount);
    pAllocationTracker = ELe_Plat_Alloc(sizeof(ele_tracker), FALSE);
    pAllocationTracker->pAllocElement = NULL;
    pAllocationTracker->pNextElement = NULL;
    state.initialized = TRUE;
    return;
}

void
ELe_Destroy()
{
   ELe_Free_Memory_Tags();
   ele_tracker* toFree;
   while(pAllocationTracker->pAllocElement != NULL)
   {
        toFree = pAllocationTracker;
        pAllocationTracker = pAllocationTracker->pNextElement;
        ELe_Plat_Free(toFree->pAllocElement);
        ELe_Plat_Free(toFree);
   }
   free(pAllocationTracker);
   pAllocationTracker = NULL;
   state.initialized = FALSE;
   return;
}

void
ELe_Set_Zero(void* block, l_uint32 blockSize)
{
    while(blockSize)
    {
        *(l_uint8*)block = 0;
        blockSize--;
        block++;
    }
}

l_bool
ELe_Mem_Tag_Check(const char* tagIn, const char* tagSet)
{
    l_bool match = TRUE;
    while(*tagSet != '\0' && match)
    {
        match = (*tagIn == *tagSet);
        tagIn++; tagSet++;
    }
    return (*tagIn == '\0' && match);
}

l_uint32
ELe_Set_Enum(ele_tag* pTag)
{
    void* temp = ((void*)memTagSet - sizeof(ele_set));
    l_uint32 memTagCount = ((ele_set*)temp)->len;
    for(l_uint64 i = 0; i < memTagCount; i ++)
    {
        if(ELe_Mem_Tag_Check(pTag->byname, (memTagSet + i)->tag))
            return i;
    }
    pTag->type = INVALID_TAG;
    return 0;
}

void*
ELe_Alloc(l_uint64 elementCount, l_uint64 elementSize, ele_tag tag, l_bool alligned)
{
    l_uint32 enumeration = 3;
    if(tag.type == NAME_TAG)
        enumeration = ELe_Set_Enum(&tag);
    if(tag.type == ENUM_TAG)
        enumeration = tag.byenum;

    void* memTagMeta = memTagSet;
    memTagMeta -= sizeof(ele_set);
    if(tag.type == INVALID_TAG)
        enumeration = ((ele_set*)memTagMeta)->len - 2;
    
    l_uint64 totalSize = elementCount * elementSize + sizeof(ele_set);
    void* block = ELe_Plat_Alloc(totalSize, alligned);
    ((ele_set*)block)->len = 0;
    ((ele_set*)block)->size = totalSize;
    ((ele_set*)block)->tag = (memTagSet + enumeration);
    ((ele_set*)block)->slots = elementCount;
    (memTagSet + enumeration)->allocated += totalSize + sizeof(ele_tracker);
    state.totalAllocated += totalSize +  + sizeof(ele_tracker);

    ele_tracker* tempTracker = ELe_Plat_Alloc(sizeof(ele_tracker), FALSE);
    tempTracker->pAllocElement = block;
    tempTracker->pNextElement = pAllocationTracker;
    pAllocationTracker = tempTracker;

    block += sizeof(ele_set);
    ELe_Set_Zero(block, totalSize - sizeof(ele_set));
    return block;
}

void
ELe_Free(void** pBlock)
{
    void* blockTemp = (*pBlock) - sizeof(ele_set);
    ele_tracker* trackIterFree = pAllocationTracker;
    ele_tracker* trackIterPrev = trackIterFree;
    while(trackIterFree->pAllocElement != blockTemp)
    {
        trackIterPrev = trackIterFree;
        trackIterFree = trackIterFree->pNextElement;
        if(trackIterFree->pAllocElement == NULL)
            return;
    }
    if(trackIterPrev == trackIterFree)
        pAllocationTracker = pAllocationTracker->pNextElement;
    trackIterPrev->pNextElement = trackIterFree->pNextElement;
    ((ele_set*)blockTemp)->tag->allocated -= (((ele_set*)blockTemp)->size +  + sizeof(ele_tracker));
    state.totalAllocated -= (((ele_set*)blockTemp)->size +  + sizeof(ele_tracker));
    ELe_Plat_Free(blockTemp);
    ELe_Plat_Free(trackIterFree);
    *pBlock = NULL;
    return;
}

void
ELe_Set_Element(void* pElement, l_uint64 copySize, void* pValue)
{
    for(l_uint64 i = 0; i < copySize; i++)
        *((l_uint8*)pElement + i) = *((l_uint8*)pValue + i);
    return;
}

void
ELe_Zero_Element(void* pElement, l_uint64 elementSize)
{
    for(l_uint64 i = 0; i < elementSize; i++)
        *((l_uint8*)pElement + i) = 0;
    return;
}

void
ELe_Shift_Push(void* shiftStart, l_uint64 shiftRange, l_uint64 shiftSize)
{
    for(l_uint64 i = 0; i < shiftRange; i++)
        *((l_uint8*)shiftStart - i) = *((l_uint8*)shiftStart - shiftSize - i);
    return;
}

void
ELe_Shift_Pop(void* shiftStart, l_uint64 shiftRange, l_uint64 shiftSize)
{
    for(l_uint64 i = 0; i < shiftRange; i++)
        *((l_uint8*)shiftStart + i) = *((l_uint8*)shiftStart + shiftSize + i);
    return;
}

void*
ELe_Push(void* block, void* dataIn, l_uint64 dataInSize)
{
    void* temp = block - sizeof(ele_set);
    if(((ele_set*)temp)->len == ((ele_set*)temp)->slots)
    {
        block = ELe_Resize(block, ((ele_set*)temp)->slots * ELe_RESIZE_FACTOR);
        temp = block - sizeof(ele_set);
    }
    l_uint64 dataSize = ((ele_set*)temp)->size - sizeof(ele_set);
    l_uint64 elementSize = dataSize / ((ele_set*)temp)->slots;
    l_uint64 shiftSize = dataSize - elementSize;
    l_uint64 copySize = (elementSize < dataInSize) ? elementSize : dataInSize;
    ((ele_set*)temp)->len += (((ele_set*)temp)->len < ((ele_set*)temp)->slots);
    temp = block + dataSize - 1;
    
    ELe_Shift_Push(temp, shiftSize, elementSize);
    ELe_Zero_Element(block, elementSize);
    ELe_Set_Element(block, copySize, dataIn);
    return block;
}

void
ELe_Copy_To_Dest(void* source, void* dest, l_uint64 copySize)
{
    for(l_uint64 i = 0; i < copySize; i++)
        *((l_uint8*)dest + i) = *((l_uint8*)source + i);
    return;
}

void
ELe_Pop(void* block, void* pDest, l_uint64 destSize)
{
    void* temp = block - sizeof(ele_set);
    l_uint64 dataSize = ((ele_set*)temp)->size - sizeof(ele_set);
    l_uint64 elementSize = dataSize / ((ele_set*)temp)->slots;
    l_uint64 shiftSize = dataSize - elementSize;
    ((ele_set*)temp)->len -= (((ele_set*)temp)->len > 0);
    if(pDest)
    {
        l_uint64 copySize = (shiftSize <= destSize) ? shiftSize : destSize;
        ELe_Copy_To_Dest(block, pDest, copySize);
    }

    temp = block + dataSize - elementSize;
    ELe_Shift_Pop(block, shiftSize, elementSize);
    ELe_Zero_Element(temp, elementSize);
    return;
}

void*
ELe_Block_Expand(l_uint64 newElementCount, l_uint64 elementSize, l_uint32 enumeration)
{
    ele_tag tag = {.byenum = enumeration, .type = ENUM_TAG};
    return ELe_Alloc(newElementCount, elementSize, tag, FALSE);
}

void*
ELe_Resize(void* block, l_uint64 newElementCount)
{
    void* temp = block - sizeof(ele_set);
    l_uint64 oldDataSize = ((ele_set*)temp)->size - sizeof(ele_set);
    l_uint64 elementSize = oldDataSize / ((ele_set*)temp)->slots;
    l_uint64 newDataSize = elementSize * newElementCount;
    l_uint64 copySize = (oldDataSize < newDataSize) ? oldDataSize : newDataSize;

    void* newBlock = ELe_Block_Expand(newElementCount, elementSize, ((ele_set*)temp)->tag->enumeration);
    ELe_Copy_To_Dest(block, newBlock, copySize);

    void* newTemp = newBlock - sizeof(ele_set);
    ((ele_set*)newTemp)->len = ((ele_set*)temp)->len;
    ELe_Free(&block);

    return newBlock;
}

void
ELe_Set(void* block, l_uint64 value)
{
    void* temp = block - sizeof(ele_set);
    l_uint64 slots = ((ele_set*)temp)->slots;
    for(l_uint64 i = 0; i < slots; i++)
        ELe_Set_At(block, i, value);
    return;
}

void
ELe_Set_At(void* block, l_uint64 element, l_int64 value)
{
    void* temp = block - sizeof(ele_set);
    if(element >= ((ele_set*)temp)->slots)
        return;

    l_uint64 elementSize = (((ele_set*)temp)->size - sizeof(ele_set)) / ((ele_set*)temp)->slots;
    void* pElement = block + (element * elementSize);
    ELe_Zero_Element(pElement, elementSize);
    if(value != 0)
    {
        l_uint64 copySize = (elementSize < sizeof(l_uint64)) ? elementSize : sizeof(l_int64);
        ELe_Set_Element(pElement, copySize, &value);
    }
    return;
}

void
ELe_Insert(void* block, void* dataIn, l_uint64 element, l_uint64 dataInSize)
{
    void* temp = block - sizeof(ele_set);
    if(element >= ((ele_set*)temp)->slots)
        return;

    l_uint64 dataSize = ((ele_set*)temp)->size - sizeof(ele_set);
    l_uint64 elementSize = dataSize / ((ele_set*)temp)->slots;
    l_uint64 copySize = (elementSize < dataInSize) ? elementSize : dataInSize;
    ((ele_set*)temp)->len += (((ele_set*)temp)->len < ((ele_set*)temp)->slots);
    void* pElement = block + (elementSize * element);
    temp = block + dataSize - 1;
    
    ELe_Shift_Push(temp, dataSize - (elementSize * element), elementSize);
    ELe_Zero_Element(pElement, elementSize);
    ELe_Set_Element(pElement, copySize, dataIn);
    return;
}

void
ELe_Remove(void* block, l_uint64 element)
{
    void* temp = block - sizeof(ele_set);
    if(element >= ((ele_set*)temp)->slots)
        return;

    l_uint64 dataSize = ((ele_set*)temp)->size - sizeof(ele_set);
    l_uint64 elementSize = dataSize / ((ele_set*)temp)->slots;
    ((ele_set*)temp)->len -= (((ele_set*)temp)->len > 0);
    void* pElement = block + (elementSize * element);
    temp = block + dataSize - elementSize;

    ELe_Shift_Pop(pElement, dataSize - (elementSize * element), elementSize);
    ELe_Zero_Element(temp, elementSize);
    return;
}

void
ELe_Copy(void* block, void* dest, l_uint64 destSize)
{
    void* temp = block - sizeof(ele_set);
    l_uint64 dataSize = ((ele_set*)temp)->size - sizeof(ele_set);
    l_uint64 copySize = dataSize <= destSize ? dataSize : destSize;
    ELe_Copy_To_Dest(block, dest, copySize);
}

void
ELe_Clear(void* block)
{
    void* temp = block - sizeof(ele_set);
    ((ele_set*)temp)->len = 0;
    ELe_Set_Zero(block, ((ele_set*)temp)->size - sizeof(ele_set));
    return;
}

ele_blockinfo
ELe_Block_Properties(void* block)
{
    void* temp = block - sizeof(ele_set);
    ele_blockinfo blockInfo = 
    {
        .slotsUsed = ((ele_set*)temp)->len,
        .slotsMax = ((ele_set*)temp)->slots,
        .dataSize = ((ele_set*)temp)->size - sizeof(ele_set),
        .sizeInMem = ((ele_set*)temp)->size,
        .enumeration = ((ele_set*)temp)->tag->enumeration,
        .tag = ((ele_set*)temp)->tag->tag
    };
    return blockInfo;
}

const char*
ELe_Allocation_Info()
{
    void* temp = (void*)memTagSet - sizeof(ele_set);
    l_uint64 totalTagCount = ((ele_set*)temp)->len;
    l_uint64 tagPropsBuffSize = (memTagSet + totalTagCount - 1)->allocated + (totalTagCount * sizeof(l_uint64)) + 256;
    char* buffer = malloc(tagPropsBuffSize);

    ele_tracker* tempTracker = malloc(sizeof(ele_tracker));
    tempTracker->pAllocElement = buffer;
    tempTracker->pNextElement = pAllocationTracker;
    pAllocationTracker = tempTracker;
    
    if(!state.initialized)
    {
        sprintf(buffer, "ELe_MEM is not initialized.\n\nTo initialize: Call ELe_Initialize(arg) and pass either a null pointer or an array of memory tags you's like to use.");
        return buffer;
    }
    l_uint32 charsWritten = sprintf(buffer, "Ele_MEM is initialized:\n\n");
    for(l_uint64 i = 0; i < totalTagCount; i++)
    {
        l_float64 allocated = (memTagSet + i)->allocated;
        char allocUnit[] = "Xb";
        if((memTagSet + i)->allocated >= ELeGb) {
            allocated /= ELeGb;
            allocUnit[0] = 'G';
        } else
        if((memTagSet + i)->allocated >= ELeMb) {
            allocated /= ELeMb;
            allocUnit[0] = 'M';
        }else
        if((memTagSet + i)->allocated >= ELeKb) {
            allocated /= ELeKb;
            allocUnit[0] = 'K';
        } else {
            allocUnit[0] = 'B';
            allocUnit[1] = '\0';
        }
        charsWritten += sprintf (buffer + charsWritten, "%s:\t\t%.2lf%s\n", 
                                (memTagSet + i)->tag, allocated, allocUnit);
    }
    char totalAllocUnit[] = "Xb";
    l_float64 totalAllocated = state.totalAllocated;
    if(state.totalAllocated >= ELeGb) {
        totalAllocated /= ELeGb;
        totalAllocUnit[0] = 'G';
    } else
    if(state.totalAllocated >= ELeMb) {
        totalAllocated /= ELeMb;
        totalAllocUnit[0] = 'M';
    }else
    if(state.totalAllocated >= ELeKb) {
        totalAllocated /= ELeKb;
        totalAllocUnit[0] = 'K';
    } else {
        totalAllocUnit[0] = 'B';
        totalAllocUnit[1] = '\0';
    }
    sprintf(buffer + charsWritten, "Total Allocated:\t%.2lf%s\n",
            totalAllocated, totalAllocUnit);
    return buffer;
}