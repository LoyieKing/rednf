void CUser::SendEventJarOutputItemStatistics(
    const std::vector<Inven_Item>& items)
{
    // b28d-GrpD 按 ORIG 0x0868a782 直证改写（4 处）：
    //  ① ORIG +0x07 `lea -0x1c(%ebp),%eax; call Packet_..._GTSC1Ev` 是首条语句
    //     ⇒ pkt 具名对象声明必须在 size 检查**之前**（我方旧版声明在中段）。
    //  ② ORIG +0x47 / +0x71 两处罐子判定均为 `cmp $0x28be7b,%eax; sete %al;
    //     test %al,%al; je <另一支>` ⇒ 源码是 `==` 正向两分支；我方旧 `!=`
    //     写法产 `setne/je` 反极性（P59/P222 分支布局反写）。
    //  ③ ORIG +0x96 `mov 0x7(%eax),%eax; mov %eax,-0xe(%ebp)`（pkt+0xe=m_itemCount）
    //     先于 +0xbd 的 `mov %eax,-0x12(%ebp)`（pkt+0xa=m_itemCode），且全程
    //     无 itemCode/itemCount 具名槽 ⇒ 直接赋成员、m_itemCount 先写（P183/P329）。
    //  ④ ORIG +0xd3 GetServerProxy 后**无判空**，eax 直接作 this 调 SendPacket
    //     （+0xd8 `movl $0x12,0x8(%esp)`）⇒ 删我方发明的防御判空（判据①）。
    Packet_Event_UpgradeJar_Output_Statistic_GTS pkt;
    if ((int)items.size() != 2)
        return;
    unsigned short jarIdx = 0;
    if (items[0].m_addInfo == 0x28be7b)
    {
        jarIdx = 0;
    }
    else
    {
        if (items[1].m_addInfo == 0x28be7b)
            jarIdx = 1;
        else
            return;
    }
    pkt.m_itemCount = items[(int)jarIdx].m_addInfo2;
    pkt.m_itemCode = items[1 - (int)jarIdx].m_addInfo;
    GlobalData::s_statistic_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0)
        ->SendPacket((char*)&pkt, 0x12);
