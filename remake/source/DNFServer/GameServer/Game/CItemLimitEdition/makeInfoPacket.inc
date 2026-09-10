void CItemLimitEdition::makeInfoPacket(PacketGuard& packet) const
{
    CILE_PACKET_BUF(packet)->put_int(m_ipgNo);                       // +0x00
    CILE_PACKET_BUF(packet)->put_item_idx(m_itemIdx);                // +0x04（ORIG 单段 ⇒ 隐式转换）
    CILE_PACKET_BUF(packet)->put_int(m_relatedItemCnt);              // +0x08
    CILE_PACKET_BUF(packet)->put_byte((int)m_avatarTypeSelect);      // +0x0c
    CILE_PACKET_BUF(packet)->put_int(m_priceCera);                   // +0x10
    CILE_PACKET_BUF(packet)->put_int(m_priceGold);                   // +0x14
    CILE_PACKET_BUF(packet)->put_int(m_curSellNum);                  // +0x48
    CILE_PACKET_BUF(packet)->put_int(m_totalSellNum);                // +0x1c
    CILE_PACKET_BUF(packet)->put_int(m_featuredIdx);                 // +0x20
    // ORIG 0x085131bb..0x0851320b：售罄剩余时间逐分支各 fresh put_int（now>=sellEnd
    // 则 put_int(0)，否则 put_int(sellEnd - now)）；非合并后单 put_int。
    // [P66/P243 求值序] ORIG 先 `mov 0x28(%eax),%ebx`（字段，跨 getCurSec call 保活）
    // 再 `cmp %eax,%ebx; setle` —— 左操作数是 m_sellEndTime；反写（getCurSec() >= …）
    // 4.4 产 call 先、字段后载 + setge 极性 ⇒ 按 ORIG 书写方向。
    if (m_sellEndTime <= GlobalData::s_systemTime_.getCurSec())
        CILE_PACKET_BUF(packet)->put_int(0);                         // ORIG 85131e4
    else
        CILE_PACKET_BUF(packet)->put_int(m_sellEndTime - GlobalData::s_systemTime_.getCurSec()); // ORIG 851320b
    CILE_PACKET_BUF(packet)->put_int(m_field2c);                     // +0x2c
    CILE_PACKET_BUF(packet)->put_byte(m_condCharacJob);              // +0x30
    CILE_PACKET_BUF(packet)->put_short((int)m_condCharacLevelBegin); // +0x34
    CILE_PACKET_BUF(packet)->put_short((int)m_condCharacLevelEnd);   // +0x36
    CILE_PACKET_BUF(packet)->put_int(m_condAccCreateTimeBegin);      // +0x38
    CILE_PACKET_BUF(packet)->put_int(m_condAccCreateTimeEnd);        // +0x3c
    CILE_PACKET_BUF(packet)->put_int(m_condCharacCreateTimeBegin);   // +0x40
    CILE_PACKET_BUF(packet)->put_int(m_condCharacCreateTimeEnd);     // +0x44
