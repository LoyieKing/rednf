const char* StringDictionary::addString(const char* s)
{
    // [b28c-11] AE：m_curPtr 由 int 改 `unsigned char*`（ORIG `mov -0x1c,%edx`
    // 单段直落，int 形多一条 eax→edx 中转）。68/68 条全等（kBufSize=0x10000 经
    // ORIG rodata 0x8e2ca64 实测确认，头注释 0x100 为笔误）。
    // ORIG 函数级 static（0x8e2ca64 = 0x10000；块分配默认容量）。
    static const int kBufSize = 0x10000;
    const size_t len = strlen(s);
    const int need = static_cast<int>(len) + 1;

    unsigned char* ret;
    if (m_curRemain >= need)
    {
        ret = m_curPtr;
        m_curPtr += need;
        m_curRemain -= need;
    }
    else
    {
        unsigned int cap = std::max(need, kBufSize);
        unsigned char* ptr = new unsigned char[cap];
        blocks_.push_back(ptr);
        m_curPtr = ptr + need;
        m_curRemain = cap - static_cast<unsigned int>(need);
        ret = ptr;
    }

    memcpy(ret, s, need);
    return reinterpret_cast<const char*>(ret);
