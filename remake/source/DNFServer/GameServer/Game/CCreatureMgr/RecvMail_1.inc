bool CDBMsgSender::RecvMail(Inven_Item* item, int characNo, int slot,
                            int reason, int mapSize)
{
    // ORIG 0x833ff3d：头部 m_stream 判空（mov (%eax);test;jne），buf 无二次判空。
    if (m_stream == 0)
        return false;
    SIG_CREATURE_ITEM* buf = GetInBuffer<SIG_CREATURE_ITEM>();
    memset(buf, 0, 0x74);
    // ORIG 0x833ff71：lea 0x4(%eax),%edx 临时 this；arg2 经 %eax（0xc 形参晚装）。
    // 【asm("eax") 绑定违规清零】batch-19 曾以 register asm 换 ndiff 8→4，
    // 硬约束禁 asm 绑定已移除；compat 局部形探针使 buf 槽 -0xc→-0x10 全帧
    // 错位（N82）反劣；裸形残差 = P30 lea/add 装载序 + N82 槽号 3 处。
    // [b28d-solo-CCreatureMgr P235] cast 形改具名视角成员调用（完整类型）：
    // ORIG `lea 0x4(%eax),%edx`（N107 add-vs-lea 六 cast 形恒 add）。
    buf->m_itemView.setCopy(*item);
    buf->m_addInfo2 = item->m_addInfo2;               // ← item+0x07
    buf->m_reason = (unsigned char)reason;            // +0x51
    buf->m_characNo = characNo;                       // +0x48
    buf->m_hdr |= 0x130;
    buf->m_field53 = 0;
    buf->m_slot = slot;                               // +0x2c
    buf->m_field64 = 4;
    buf->m_mapSize = mapSize;                         // +0x68
    return true;
}
