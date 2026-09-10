MemPool<CAdvertisementExposeInfo> CAdvertisementExposeInfo::advertisementExposeInfoPool_(0x32);

// ORIG T 0x080fc998：直调 MemPool::alloc（反编译实证）。
void* CAdvertisementExposeInfo::operator new(unsigned int size)
{
    (void)size;
    return advertisementExposeInfoPool_.alloc();
