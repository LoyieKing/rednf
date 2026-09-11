// ORIG 静态池实例：MemPool<CAdvertisementExposeInfo>（ctor count 0x32 =
// 50 块，实证 __static_initialization_and_destruction_0 @0x080fdc63 push $0x32）。
MemPool<CAdvertisementExposeInfo> CAdvertisementExposeInfo::advertisementExposeInfoPool_(0x32);

// ORIG T 0x080fc998：直调 MemPool::alloc（反编译实证）。
void* CAdvertisementExposeInfo::operator new(unsigned int size)
{
    (void)size;
    return advertisementExposeInfoPool_.alloc();
}