bool CDBMsgSender::SendMail(Inven_Item* item, int characNo, int slot, int reason,
                            int mapSize)
{
    // ORIG 0x833fe95：头部 m_stream 判空，buf 无二次判空。
    if (m_stream == 0)
        return false;
    SIG_CREATURE_ITEM* buf = GetInBuffer<SIG_CREATURE_ITEM>();
    memset(buf, 0, 0x74);
    // ORIG 0x833febb：lea 临时 this（无 compat 槽）。
    // 【asm("eax") 绑定违规清零】同 RecvMail 回裸形（compat 局部形 N82 反劣）；
    // 另 +0x2b 处 ORIG `xor$1;test;je` 常量岛在 4.4 折成 bool^1 直落形。
    buf->m_itemView.setCopy(*item);   // [b28d-solo P235] 具名视角出 lea（N107 边界）
    buf->m_addInfo2 = item->m_addInfo2;               // ← item+0x07
    buf->m_reason = (unsigned char)reason;            // +0x51
    buf->m_characNo = characNo;                       // +0x48
    buf->m_hdr |= 0x100;                              // or $0x1,%dh
    buf->m_field53 = 1;                               // +0x53
    buf->m_field64 = 3;                               // +0x64
    buf->m_mapSize = mapSize;                         // +0x68
    return true;
}
