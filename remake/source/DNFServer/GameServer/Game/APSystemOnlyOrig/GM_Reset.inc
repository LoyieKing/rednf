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
