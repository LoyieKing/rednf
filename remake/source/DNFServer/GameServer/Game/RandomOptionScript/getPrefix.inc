std::string RandomOptionScript::getPrefix(int prefix, int grade, bool flag)
{
    std::string out;
    // ORIG 0x8a73956 逐条：①第三参数先拷入本地 byte 槽（`mov %al,-0x3c(%ebp)`），
    // 此后两次以该槽作 `cmpb $0` 判；②表指针**先无条件取 m_prefixNormal(+0x1a8)**
    // 存 -0x28，再在 flag 真时用 m_prefixPremium(+0x1c0) 覆盖（非三目选择形）；
    // ③getPrefixGrade 结果入槽后条件 +3；④find/end/pt 全走非 const iterator。
    // [b28d-28] 该 byte 槽是 4.4 对 bool 形参的自动截断 spill；再写具名
    // `bool premium = flag;` 会多一对 movzbl+movb 且把 it/end 槽顶深 4（P244 配套）。
    std::map<int, std::vector<std::string> >* pMap = &m_prefixNormal;
    if (flag)
    {
        pMap = &m_prefixPremium;
    }
    int idx = getPrefixGrade(grade);
    if (flag)
    {
        idx += 3;
    }
    std::map<int, std::vector<std::string> >::iterator it = pMap->find(prefix);
    // [b28d-28] ORIG 0x08a73956 实为「具名 vector 指针 + unsigned 下标 while
    // 扫描」：`pVec=&(*it).second; i=0; while(pVec->size()>i){ if(idx==(int)i){
    // out=(*pVec)[i]; break; } ++i; }`（ORIG +0xac..+0x0fa）。原越界防御
    // （idx>=0 && idx<size()）是多余语义，ORIG 无此判断。
    if (it != pMap->end())
    {
        // [b28d-28] p167 实测：ORIG 调 `ptEv`（operator->），非 `deEv`（operator*）。
        std::vector<std::string>* pVec = &it->second;
        unsigned int i = 0;
        while (pVec->size() > i)
        {
            if (i == (unsigned int)idx)
            {
                out = (*pVec)[i];
                break;
            }
            ++i;
        }
    }
    return out;
