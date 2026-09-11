// ORIG T 0x084602c4：header(0,0x91)；int(v1)；后接 4 组 [byte cnt；cnt×{int
// m_addInfo(Inven_Item+0x02); int get_add_info()}]，Inven_Item 步长 0x3d 遍历；
// finalize(true)。
void CDeathTower::CPacketHandler::makeDisplayReward(int v1,
                                                    int cnt1, const Inven_Item* items1,
                                                    int cnt2, const Inven_Item* items2,
                                                    int cnt3, const Inven_Item* items3,
                                                    int cnt4, const Inven_Item* items4)
{
    put_header(0, 0x91);
    put_int(v1);

    put_byte(cnt1);
    for (int i = 0; i < cnt1; ++i)
    {
        put_int(items1[i].m_addInfo);
        put_int(items1[i].get_add_info());
    }
    put_byte(cnt2);
    for (int i = 0; i < cnt2; ++i)
    {
        put_int(items2[i].m_addInfo);
        put_int(items2[i].get_add_info());
    }
    put_byte(cnt3);
    for (int i = 0; i < cnt3; ++i)
    {
        put_int(items3[i].m_addInfo);
        put_int(items3[i].get_add_info());
    }
    put_byte(cnt4);
    for (int i = 0; i < cnt4; ++i)
    {
        put_int(items4[i].m_addInfo);
        put_int(items4[i].get_add_info());
    }
    finalize(true);
}