void CItemDictionary::getData(char* out) const
{
    char* dst = out;
    std::memcpy(dst + 4, m_data, kBinaryLength);
