
#include <ELe_MEM.h>
#include <stdio.h>

typedef enum Mem_Tags
{
    TAG_ONE,
    TAG_TWO,
    TAG_THREE,
    TAG_FOUR,
    TAG_FIVE,
    TAG_SIX,
    TAG_SEVEN,
    TAG_EIGHT,
    TAG_NINE,
    TAG_TEN,
    TESTTEST
} mem_tags;

int main(int argc, char* argv[])
{
    const char* tagArray[] =
    {
        "TAG_ONE",
        "TAG_TWO",
        "TAG_THREE",
        "TAG_FOUR",
        "TAG_FIVE",
        "TAG_SIX",
        "TAG_SEVEN",
        "TAG_EIGHT",
        "TAG_NINE",
        "TAG_TEN"
    };

    void* testBlock;
    ELe_Initialize(tagArray, 10);

    testBlock = ELe_Alloc(2, 4, "TAG_ONE");
    testBlock = ELe_Alloc(2, 4, "TAG_TWO");
    testBlock = ELe_Alloc(2, 4, "TAG_THREE");
    testBlock = ELe_Alloc(2, 4, "TAG_FOUR");
    testBlock = ELe_Alloc(2, 4, "TAG_FIVE");
    testBlock = ELe_Alloc(2, 4, "TAG_SI");
    testBlock = ELe_Alloc(2, 4, "TAG_SIX");
    testBlock = ELe_Alloc(2, 4, "TAG_SIXd");
    testBlock = ELe_Alloc(2, 4, TAG_SEVEN);
    ELe_Free(testBlock);
    testBlock = ELe_Alloc(2, 4, TAG_EIGHT);
    testBlock = ELe_Alloc(2, 4, TAG_NINE);
    testBlock = ELe_Alloc(2, 4, TAG_TEN);

    l_uint32 toPush = 12;
    testBlock = ELe_Alloc(4, sizeof(l_uint32), TAG_FIVE);

    for(l_uint32 i = 0; i < 7; i++)
    {
        testBlock = ELe_Push(testBlock, toPush, sizeof(toPush));
        toPush++;
    }
    testBlock = ELe_Resize(testBlock, 8);
    //ELe_Clear(testBlock);
    for(l_uint32 i = 0; i < 7; i++)
    {
        testBlock = ELe_Push(testBlock, toPush, sizeof(toPush));
        toPush++;
    }
    l_uint32 insertInt = 156;
    ele_blockinfo blockinfo = ELe_Block_Properties(testBlock);
    ELe_Remove(testBlock, 4);
    ELe_Insert(testBlock, insertInt, 4, sizeof(insertInt));
    l_uint32 dest;
    for(l_uint32 i = 0; i < 7; i++)
        ELe_Pop(testBlock, &dest);
    
    ELe_Clear(testBlock);
    ELe_Set(testBlock, 7);
    
    ELe_Free(testBlock);
    //for(int i = 0; i < 12; i++)
    //    ELe_Print_Tag_Stats(i);
    const char* ELeStateInfo = ELe_AllocationInfo();
    printf("%s", ELeStateInfo);

    //void* testBlock2 = ELe_Alloc(1024, 1024, TAG_ONE);
    //ELe_Push(testBlock2, toPush);
    ELe_Destroy();
    return 0;
}