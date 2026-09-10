void CSecretShopStatistic::SendSecretShopStatistic()
{
    // [b28d-solo-CSecretShop] 真语义 bug 修正 + P0/P197④/P83：
    // ① ORIG 0x085fd2a6 `mov %eax,-0xfbc`（packet+0xe=m_shopIdx）与 0x085fd2c3
    //    `mov %eax,-0xfc0`（packet+0xa=m_count）证明原码逐 pos 写这两个包字段
    //    （旧实现漏写）；② `empty()` 改 `size()`（ORIG call size，P197④）；
    // ③ memcpy 改结构整体赋值（ORIG 5×mov 逐字段，P83/N84）；
    // ④ proxy 无具名局部（ORIG call 结果直作 this，P0）；count 声明在循环外
    //    （ORIG -0x10 函数头 + 循环内两次清零）。
    Packet_Secret_Shop_Statistic packet;
    int count = 0;
    for (int pos = 0; pos < 3; ++pos) {
        packet.m_shopIdx = pos;
        packet.m_count = (int)m_pos[pos].size();
        if (packet.m_count == 0)
            continue;
        count = 0;
        memset(packet.m_items, 0, sizeof(packet.m_items));
        for (std::map<int, SECRET_SHOP_STATISTIC_DATA>::iterator it =
                 m_pos[pos].begin();
             it != m_pos[pos].end(); ++it, ++count) {
            packet.m_items[count] = it->second;
        }
        GlobalData::s_statistic_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0)
            ->SendPacket((char*)&packet, 0xfb2);
    }
