// ORIG 0x0811d7e0：memcpy(out+4, this+5, 0x1388)，无空检查。
// ORIG out 参数先物化到局部（-0xc）再回读 +4 ⇒ 源码经局部指针。
void CItemDictionary::getData(char* out) const
{
    char* dst = out;
    std::memcpy(dst + 4, m_data, kBinaryLength);
}
