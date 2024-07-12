#ifndef ELe_MEM_H
#define ELe_MEM_H

#include "ELe_PLAT.h"



#define _tag_set(x) _Generic((x), char*:                 NOWARN(-Weverything, ({ele_tag _tag_in_ = {x, NAME_TAG}; _tag_in_;})),       \
                                  char[sizeof(x)]:       NOWARN(-Weverything, ({ele_tag _tag_in_ = {x, NAME_TAG}; _tag_in_;})),       \
                                  const char*:           NOWARN(-Weverything, ({ele_tag _tag_in_ = {x, NAME_TAG}; _tag_in_;})),       \
                                  const char[sizeof(x)]: NOWARN(-Weverything, ({ele_tag _tag_in_ = {x, NAME_TAG}; _tag_in_;})),       \
                                  int:                   NOWARN(-Weverything, ({ele_tag _tag_in_ = {x, ENUM_TAG}; _tag_in_;})),       \
                                  const int:             NOWARN(-Weverything, ({ele_tag _tag_in_ = {x, ENUM_TAG}; _tag_in_;})),       \
                                  default:               NOWARN(-Weverything, ({ele_tag _tag_in_ = {0, INVALID_TAG}; _tag_in_;})))

#define ELe_Initialize(tagStringArray, tagCount)            ELe_Initialize(tagStringArray, tagCount)
#define ELe_Destroy()                                       ELe_Destroy()

#define ELe_Print_Tag_Stats(tag)                            ELe_Print_Tag_Stats(_tag_set(tag))

#define ELe_Alloc(elementCount, elementSize, tag)           ELe_Alloc(elementCount, elementSize, _tag_set(tag), FALSE)
#define ELe_Free(block)                                     ELe_Free(&block)

#define ELe_Push(block, dataIn, dataInSize)                 ELe_Push(block, &dataIn, dataInSize)
#define ELe_Pop(block, pDest)                                ELe_Pop(block, pDest, sizeof(*pDest));
#define ELe_Resize(block, newElementCount)                  ELe_Resize(block, newElementCount)

#define ELe_Set(block, value)                               ELe_Set(block, value)
#define ELe_Set_At(block, element, value)                   ELe_Set_At(block, element, value)

#define ELe_Insert(block, dataIn, element, dataInSize)      ELe_Insert(block, &dataIn, element, dataInSize)
#define ELe_Remove(block, element)                          ELe_Remove(block, element)
#define ELe_Copy(block, dest, destSize)                     ELe_Copy(block, dest, destSize)

#define ELe_Clear(block)                                    ELe_Clear(block)

#define ELe_Block_Properties(block)                         ELe_Block_Properties(block)
#define ELe_AllocationInfo()                                ELe_Allocation_Info()


#endif // ELe_MEM_H