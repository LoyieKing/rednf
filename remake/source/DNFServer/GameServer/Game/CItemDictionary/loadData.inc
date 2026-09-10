bool CItemDictionary::loadData(CUser*, char* data)
{
    // N7 封顶：虚调用函数指针/this 载入寄存器 ORIG(edx/eax) 与 4.4(eax/edx) 互换，
    // 助记符序列全等，跨版本寄存器分配差。
    _reset();
    char* p = data;   // ORIG data 物化到局部（-0xc）再回读 +4
    std::memcpy(m_data, p + 4, kBinaryLength);
    return true;
}
