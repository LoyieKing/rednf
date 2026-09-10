int STExpertJobScript::GetPortableDisjointPossibleLevel(int level)
{
    std::map<int, unsigned short>::iterator it = m_portableDisjointMap.find(level);
    if (it == m_portableDisjointMap.end())
        return -1;
    // [b28c-L18] ORIG 尾双段 `movzwl 0x4(%eax),%eax; movzwl %ax,%eax` = int 返回
    // + (unsigned short) 提升（探针 dj.cpp f_d 全等）。
    return (unsigned short)it->second;
