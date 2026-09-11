pack_ret_t IndexEntry::read_info(unsigned char*& mem)   // @0x08ac4200
{
    _mread_str(name_, mem);
    size_ = *reinterpret_cast<unsigned int*>(mem);
    mem += 4;
    aligned_size_ = (size_ + 3u) & ~3u;
    crc_ = *reinterpret_cast<unsigned long*>(mem);
    mem += 4;
    offset_ = *reinterpret_cast<unsigned int*>(mem);
    mem += 4;
    return PACK_OK;
}