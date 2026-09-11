// ORIG _ZN5CUser32SendEventJarOutputItemStatisticsERKSt6vectorI10Inven_ItemSaIS1_EE
//   @ 0x0868a782 (T)
// vec 长==2 时：元素加信息(+2 addInfo)==0x28be7b 即罐子；另一元素为产出道具。
// 组 Packet_Event_UpgradeJar_Output_Statistic_GTS(0x106e/0x12) 经 statistic
// proxy(GET_ENUM_SERVER_GROUP_NONE) 下发。罐子 id 为 0x28be7b，剩余逻辑：
//   local_e = 罐子在 vec 中的下标（0 或 1）；m_itemCode = vec[1-local_e].addInfo；
//   m_itemCount = vec[local_e].+7（m_addInfo2 罐子数量）。
// b28d-lane5 N封顶: P168a/P248 setcc 物化方向; N82/P112 槽号分配 | ORIG 0x0868a782 | _ZN5CUser32SendEventJarOutputItemStatisticsERKSt6vectorI10Inven_ItemSaIS1_EE
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
}