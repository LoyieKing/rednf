// ---- CChattingEmoticonList::ReCalc（报告 ReCalc.md，ORIG T 0x080e5880）：
//      ① 遍历 config 24 行：行 m_nCount 能 find_emoticon 命中且 isCommonEmoticon
//         （vtable+0x14）→ 行拷入 rows 并记 used；② config.clear()；
//      ③ rows 各行回填到 config 中 m_nCount==0 的空行（[GetStartArrayIndex,
//         GetEndArrayIndex] 区间内第一个空位，整行 0x16 字节拷贝）；
//      ④ 遍历 m_map：GetIndex 未在 used 且 isCommonEmoticon → 同样区间内
//         空行写入 m_nCount。 ----
// 残余 DIFF 归因（o=298 n=296，无符号界/setbe 已对）：主体为 N7/N18a/N22
// 族——同址参数装载的 eax↔edx 指派与 spill 序（如 `mov (%eax),%edx` vs
// `mov (%eax),%eax` 后接不同中转），4.4.7 分配器固定反向，穷举表达式形
// 不可翻（范式库 N7/N18a/N22 + setcc 装载组合）。
// [b28d-solo-CDataManager AE 298=298] 本轮翻正（o=298 n=296→298）：
//   ① 循环尾测 ORIG `mov -0x20,%eax; cmp $0x17,%eax; setbe` 两段形 ⇒ 局部改
//      int + 显式 `(unsigned int)i <= 0x17`（P310，unsigned 局部恒折内存直比）；
//   ② 尾块 `mov $0x1,%ebx … mov %ebx,%eax` ⇒ 返回类型 void→int（P159；头
//      CDataManager.h 同步，mangled 不含返回类型，唯一调用点忽略返回值）。
// p167 29/29 CLEAR。
int CChattingEmoticonList::ReCalc(CChattingEmoticonConfig& config,
                                  const CUser& user)
{
    std::vector<CChattingEmoticonConfigRow> rows;
    std::set<int> used;

    // ORIG 0x080e58?? `mov -0x20; cmp $0x17; setbe` = 无符号比较（范式库
    // P2 反例：signed setle 异形）。P310：局部 int + 显式 (unsigned) 比较
    // 复现 ORIG 的「装载 + 寄存器比较」两段尾测（unsigned 局部恒折内存直比）。
    for (int i = 0; (unsigned int)i <= 0x17; ++i)
    {
        ChattingEmoticonBase* p = find_emoticon(config.m_Row[i].m_nCount);
        if (p == 0)
            continue;
        if (!p->isCommonEmoticon(user))
            continue;
        rows.push_back(config.m_Row[i]);
        used.insert(config.m_Row[i].m_nCount);
    }
    config.clear();

    for (unsigned int r = 0; r < rows.size(); ++r)
    {
        ChattingEmoticonBase* p = find_emoticon(rows[r].m_nCount);
        if (p == 0)
            continue;
        for (int i = p->GetStartArrayIndex(); i <= p->GetEndArrayIndex(); ++i)
        {
            if (config.m_Row[i].m_nCount == 0)
            {
                config.m_Row[i] = rows[r];
                break;
            }
        }
    }

    for (std::map<int, ChattingEmoticonBase*>::iterator it = m_map.begin();
         it != m_map.end(); ++it)
    {
        if (used.find(it->second->GetIndex()) != used.end())
            continue;
        if (!it->second->isCommonEmoticon(user))
            continue;
        for (int i = it->second->GetStartArrayIndex();
             i <= it->second->GetEndArrayIndex(); ++i)
        {
            if (config.m_Row[i].m_nCount == 0)
            {
                config.m_Row[i].m_nCount = (short)it->second->GetIndex();
                break;
            }
        }
    }
    return 1;
}