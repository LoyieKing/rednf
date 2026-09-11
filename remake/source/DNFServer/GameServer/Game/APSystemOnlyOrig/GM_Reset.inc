// GM_Reset —— 报告汇编+反编译全文转写。ORIG T 0x08121d12。
// [b28d-27] DIFF N82：ORIG 0x08121d12；p167 CLEAR（1/1），残差为循环体数组寻址
// [b28d-solo-APSystemOnlyOrig] 判词：**N18a 封顶**（唯一残差 = ORIG `mov %edx,%ecx; neg %ecx` vs 4.4 `neg %edx; mov %edx,%ecx` 1 对；p167 CLEAR 1/1；槽位 -0x14/-0x10/-0xc 与帧 0x28 全等）。
// （两步 add vs 4 步 imul 链）+ 帧差，语义闭合。
// [b28d-round1-L24] **DIFF→AE**（55=55）：唯一残差 `mov %edx,%ecx; neg %ecx`
// （ORIG 0x08121d9d/0x08121d9f）靠**提升到 unsigned int 再取负**复现 ——
// `(unsigned char)(-(unsigned int)(signed char)x)`（探针 /tmp/probe_neg.cpp f6
// 逐条同形；f1..f5 恒产 `neg %edx; mov %edx,%ecx`，int 中介亦无效）。
void CActionPointManager::GM_Reset()
{
    m_flag00 = 1;                             // *(this)=1
    memset(&m_sig.m_isRewardItem[0], 0, sizeof(m_sig.m_isRewardItem));   // ORIG 4 字节
    m_sig.m_isRewardMedal = 0;                // this+0x10 = sig+8
    m_sig.m_apSum = 0;                        // *(uint*)(this+0xc)
    m_prevApSum = 0;                          // *(uint*)(this+4)
    for (int row = 0; row <= 299; ++row)
    {
        // [b28d-solo-APSystemOnlyOrig] ORIG 0x08121d74：每外层轮把行基址
        // &m_sig.m_actionPoints[row]（=this+0x15+row*0xb）缓存进具名指针槽
        // -0x10，内层以 0x6(%edx,%eax,1) 访问；不缓存则 4.4 折成
        // 0x1b(%ecx,%eax,1) 全地址重算（多 6 条 + push esi/ebx 帧差）。
        _ActionPoint* point = &m_sig.m_actionPoints[row];
        for (int slot = 0; slot <= 4; ++slot)
        {
            if (static_cast<signed char>(point->m_days[slot]) > 0)
            {
                point->m_days[slot] = static_cast<unsigned char>(
                    -(unsigned int)static_cast<signed char>(point->m_days[slot]));
            }
        }
    }
}