#ifndef ELe_PLATFORM_H
#define ELe_PLATFORM_H

typedef unsigned char       l_uint8;
typedef unsigned short      l_uint16;
typedef unsigned int        l_uint32;
typedef unsigned long long  l_uint64;

typedef signed char         l_int8;
typedef signed short        l_int16;
typedef signed int          l_int32;
typedef signed long long    l_int64;

typedef float               l_float32;
typedef double              l_float64;

typedef unsigned char       l_bool;

#if defined(__clang__) || defined(__gcc__)
#define ELe_STATIC_ASSERT _Static_assert
#else
#define ELeSTATIC_ASSERT static_assert
#endif // clang || gcc

// Check defined types for expected size:
ELe_STATIC_ASSERT(sizeof(l_uint8)   == 1, "Expected u8 to be 1 byte.");
ELe_STATIC_ASSERT(sizeof(l_uint16)  == 2, "Expected u16 to be 2 bytes.");
ELe_STATIC_ASSERT(sizeof(l_uint32)  == 4, "Expected u32 to be 4 bytes.");
ELe_STATIC_ASSERT(sizeof(l_uint64)  == 8, "Expected u64 to be 8 bytes.");

ELe_STATIC_ASSERT(sizeof(l_int8)    == 1, "Expected i8 to be 1 byte.");
ELe_STATIC_ASSERT(sizeof(l_int16)   == 2, "Expected i16 to be 2 bytes.");
ELe_STATIC_ASSERT(sizeof(l_int32)   == 4, "Expected i32 to be 4 bytes.");
ELe_STATIC_ASSERT(sizeof(l_int64)   == 8, "Expected i64 to be 8 bytes.");

ELe_STATIC_ASSERT(sizeof(l_float32) == 4, "Expected f32 to be 4 bytes.");
ELe_STATIC_ASSERT(sizeof(l_float64) == 8, "Expected f64 to be 8 bytes.");

#ifndef TRUE
#define TRUE 1
#endif // TRUE
#ifndef FALSE
#define FALSE 0
#endif // FALSE
#ifndef NULL
#define NULL ((void*)0)
#endif // NULL

// Platform detection:
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define ELe_WIN32 1
#ifndef _WIN64
#error "Error: 64-bit Windows required!"
#endif // WIN32
#else
#error "Error: Currently only supported on Windows!"
#endif // NON WIN32

// Exports:
#ifdef ELe_EXPORT
#ifdef _MSC_VER
#define ELeAPI __declspec(dllexport)
#else
#define ELeAPI __attribute__((visibility("default")))
#endif // _MSC_VER
// Imports:
#else
#ifdef _MSC_VER
#define ELeAPI __declspec(dllimport)
#else
#define ELeAPI
#endif // _MSC_VER
#endif // ELe_EXPORT

#define DO_PRAGMA(x) _Pragma(#x)
#define NOWARN(warnoption, ...)                    \
    DO_PRAGMA(GCC diagnostic push)                 \
    DO_PRAGMA(GCC diagnostic ignored #warnoption)  \
    __VA_ARGS__                                    \
    DO_PRAGMA(GCC diagnostic pop)

#define ELeKb   1024
#define ELeMb   ELeKb * 1024
#define ELeGb   ELeMb * 1024

typedef enum ELe_Memory_Tag_Type
{
    ENUM_TAG,
    NAME_TAG,
    INVALID_TAG
} tag_type;

typedef struct ELe_Memory_Tag_Input
{
    union 
    {
    const char*     byname;
    l_int32         byenum;
    };
    tag_type        type;
} ele_tag;

typedef struct ELe_Block_Properties
{
    l_uint64        dataSize;
    l_uint64        sizeInMem;
    l_uint64        slotsUsed;
    l_uint64        slotsMax;
    l_uint32        enumeration;
    const char*     tag;
}ele_blockinfo;

ELeAPI void
ELe_Initialize(const char* memTagStringArray[], l_uint64 memTagCount);

ELeAPI void
ELe_Destroy();

ELeAPI void
ELe_Print_Tag_Stats(const ele_tag tag);

ELeAPI void*
ELe_Alloc(l_uint64 elementCount, l_uint64 elementSize, ele_tag tag, l_bool alligned);

ELeAPI void
ELe_Free(void** pBlock);

ELeAPI void*
ELe_Push(void* block, void* dataIn, l_uint64 dataInSize);

ELeAPI void
ELe_Pop(void* block, void* pDest, l_uint64 destSize);

ELeAPI void*
ELe_Resize(void* block, l_uint64 newElementCount);

ELeAPI void
ELe_Set(void* block, l_uint64 value);

ELeAPI void
ELe_Set_At(void* block, l_uint64 element, l_int64 value);

ELeAPI void
ELe_Insert(void* block, void* dataIn, l_uint64 element, l_uint64 dataInSize);

ELeAPI void
ELe_Remove(void* block, l_uint64 element);

ELeAPI void
ELe_Copy(void* block, void* dest, l_uint64 destSize);

ELeAPI void
ELe_Clear(void* block);

ELeAPI ele_blockinfo
ELe_Block_Properties(void* block);

ELeAPI const char*
ELe_Allocation_Info();

#endif // ELe_PLATFORM_H