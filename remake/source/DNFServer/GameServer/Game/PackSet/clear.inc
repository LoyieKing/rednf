void IndexEntry::clear()   // @0x08ac3e1a
{
    type_ = ENTRY_PACK;
    size_ = 0;
    aligned_size_ = 0;
    crc_ = 0;
    offset_ = 0;
    content_ = 0;
