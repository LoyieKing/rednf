// ORIG T 0x086435ca const：
//   pData+0xe←this+0x16 / pData+0xc←this+0x14 / pData+4←this+0xc /
//   pData+8←this+0x10 / pData+0x10←this+0x18 / pData+0x14←this+0x1c
//   （ORIG 不回写 sig+0 ← this+8）。
void TOD_UserState::getData(char* pData) const
{
    SIG_REQUEST_TOD_USER_STATE& out =
        *reinterpret_cast<SIG_REQUEST_TOD_USER_STATE*>(pData);
    // [b28c-lane17] ORIG 0x086435ca 发射序 = +0x16→out+0xe、+0x14→out+0xc、
    // +0xc→out+4、+0x10→out+8、+0x18→out+0x10、+0x1c→out+0x14（-O0 逐句发射，
    // 书写序即机器码序）；原按偏移升序书写导致前两组语句块整体错位。
    out.m_lastEnterLayer = m_data.m_lastEnterLayer;      // src +0x16 → out +0x0e
    out.m_todayLayer = m_data.m_todayLayer;              // src +0x14 → out +0x0c
    out.m_firstEnterTime = m_data.m_firstEnterTime;      // src +0x0c → out +0x04
    out.m_enterCount = m_data.m_enterCount;              // src +0x10 → out +0x08
    out.m_lastEnterTime = m_data.m_lastEnterTime;        // +0x10
    out.m_lastClearTime = m_data.m_lastClearTime;        // +0x14
}