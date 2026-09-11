// ORIG W @0x08ac7a50：隐式逐成员拷贝
IndexEntry::IndexEntry(const IndexEntry& rhs)
    : type_(rhs.type_), name_(rhs.name_), size_(rhs.size_),
      aligned_size_(rhs.aligned_size_), crc_(rhs.crc_), offset_(rhs.offset_),
      content_(rhs.content_)
{
}