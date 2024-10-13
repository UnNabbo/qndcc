#define ARRAY_DEF_SIZE 16
typedef struct{
    s32 Size;
    s32 Reserved;
    u64 Stride;
} _internal_array_layout;

#define _ArrayInternal(Array) ((_internal_array_layout*)(((s8*)Array) - sizeof(_internal_array_layout)))

#define _Temporary(x, Name) __typeof__((x)) Name = (x)

void * _ArrayAllocInternal(u64 Stride){
    _internal_array_layout* Array = MemAlloc(sizeof(_internal_array_layout) + Stride * ARRAY_DEF_SIZE);

    Array->Size = 0;
    Array->Reserved = ARRAY_DEF_SIZE;
    Array->Stride = Stride;
    
    s8* Data = ((s8*)Array) + sizeof(_internal_array_layout);
    return Data;
}

void _ArrayReallocInternal(s8 ** Data, u64 NewSize){
    _internal_array_layout* Layout = _ArrayInternal(*Data);
    Layout->Reserved *= 8;
    Layout = MemRealloc(Layout, sizeof(_internal_array_layout) + Layout->Stride * Layout->Reserved);
}

void _ArrayAppendInternal(s8 ** Base, void * Data){
    _internal_array_layout* Array = _ArrayInternal(*Base);

    if(Array->Size + 1 > Array->Reserved){
        _ArrayReallocInternal(Base, Array->Reserved * 8);
    }
    
    s8* Slot = *Base + Array->Size++ * Array->Stride;
    MemCopy(Slot, Data, Array->Stride);
}

#define ArrayAlloc(Type) _ArrayAllocInternal(sizeof(Type))
#define ArrayFree(Array) MemFree((void*)_ArrayInternal(Array))
#define ArraySize(Array) _ArrayInternal(Array)->Size
#define ArrayReserved(Array) _ArrayInternal(Array)->Reserved

#define ArrayAppendRef(Array, Data) _ArrayAppendInternal((s8**)&Array, &Data)
#define ArrayAppend(Array, Data) do{ _Temporary(Data, Temp); _ArrayAppendInternal((s8**)&Array, &Temp); }while(0)

#define For(Var, Array)\
for(int Var##Index = 0, _=1;_;_=0) \
for(__typeof__((Array[0])) Var = (Array[0]); Var##Index < ArraySize(Array); Var = Array[++Var##Index])
