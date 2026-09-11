// ORIG W 0x0822b1f4（声明补入 STExpertJobScript.h；定义置本 TU 保持
// expert_job 域内聚）。m_portableDisjointMap(+0x18) find(level)，未命中 -1。
int STExpertJobScript::GetPortableDisjointPossibleLevel(int level)
{
    std::map<int, unsigned short>::iterator it = m_portableDisjointMap.find(level);
    if (it == m_portableDisjointMap.end())
        return -1;
    // [b28c-L18] ORIG 尾双段 `movzwl 0x4(%eax),%eax; movzwl %ax,%eax` = int 返回
    // + (unsigned short) 提升（探针 dj.cpp f_d 全等）。
    return (unsigned short)it->second;
}