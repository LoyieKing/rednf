// [b28d-solo-CEquipItem] ORIG 0x0851098a（182/184）：仅剩两处结构性残差，N 封顶。
//   ① 位掩码段（0x08510b7f 段）ORIG 为单次读 `mov 0x234(%eax),%eax`（cmp 与 shl 共用
//      %eax）：4.1 对 `&&` 两侧同一 this->m_itemType 的两次读复用为一次；4.4 -O0 内联形
//      恒发第二次 `mov 0x8(%ebp),%eax; mov 0x234(%eax),%eax`（+2 条），register 局部形
//      换成 `mov %eax,%ebx`+`mov %ebx,%eax` 两条中转（净差同）⇒ 跨版本同表达式字段读
//      CSE 差异，封顶（探针 /tmp/p1/t.cpp f_inline/f_reg）。
//   ② G_CEnvironment 段（0x08510ba3 段）ORIG 值先入 %eax、&item+0x15 入 %edx；4.4 恒
//      互换（助记符序列全等仅寄存器角色互换 = N7；本 TU 已路由 4.4.6 后端仍存）。
// [b2c 复核·维持 N18a/N7] 槽归一化后全函数**仅剩 2 组**残差（其余逐条全等）：
//   ① ORIG#158 插入 2 条：我方位掩码段（ORIG 0x08510b88 `cmp $0x19` 前）多发
//      `mov 0x8(%ebp),%eax; mov 0x234(%eax),%eax`（第二次成员载入，同 checkItemUpgrade
//       Possible 的 4.1 RTL-CSE vs 4.4 -O0 差异 = N18a）；
//   ② ORIG#167 起 5 条：ORIG `mov 0x198(%eax),%eax; mov 0xc(%ebp),%edx; add $0x15,%edx;
//      mov %eax,0x8(%esp); mov %edx,0x4(%esp)`（m_gcNo→%eax、&item+0x15→%edx）vs 我方
//      %eax/%edx 角色互换（纯寄存器分配 = N7，源码形不可控；P211/P248 类型旋钮对
//      该段不适用：实参是 env 成员与地址常量 +0x15）。
void CEquipItem::make_item(Inven_Item& item) const
{
    if (IsAvatarItem())
    {
        item.m_itemType = 8;
    }
    else
    {
        item.m_itemType = 1;
    }
    if (m_itemType == 0x16)
    {
        item.m_itemType = 5;
    }
    else if (m_itemType == 0x17 || m_itemType == 0x18 || m_itemType == 0x19)
    {
        item.m_itemType = 6;
    }
    item.set_add_info(get_rand_int(0));
    item.ResetItemAttr();
    item.m_endurance = (unsigned short)m_randomTable.m_endurance;
    if (item.m_itemType == 8)
    {
        if (m_avatarSelectIdx > 0 && m_avatarSelectIdx <
            (int)((std::vector<stAvatarTypeSelect_t>*)getAvatarTypeSelect())->size())
        {
            item.m_endurance = (unsigned short)m_avatarSelectIdx;
        }
    }
    if (m_attachType == 3)
    {
        item.m_seal = 1;
    }
    if (PeriodAfterUnsealing() == 0)
    {
        int t = 0;
        if (getUsablePeriod() != 0)
        {
            t = GlobalData::s_systemTime_.getCurSec() +
                getUsablePeriod() * 0x15180;
        }
        else if (getExpirationDate() != 0)
        {
            t = getExpirationDate();
        }
        if (t != 0 && m_itemType != 10)
        {
            item.m_endurance = (unsigned short)((t - 0x44a54a80) / 0x15180);
        }
    }
    item.m_extendInfo = 0;
    // ORIG（make_item）：(`inven` + 0x15) 处 0x11 字节打包全局唯一标识，位于
    // Inven_Item 的 m_amp(+0x11).m_pad4 起始（即 &item + 0x15），跨 ampen/pad 与
    // m_random 的交叠打包区。保留地址形式（打包交叠区非独立具名成员），锚定到
    // 具名子成员 m_amp.m_pad4 使偏移可审计。
    ((itemGloballyUniqueIdentifier_t*)(char*)&item.m_amp.m_pad4[0])->reset();
    if ((unsigned int)m_itemType < 0x1a &&
        ((1 << m_itemType) & 0x3bffc00U) != 0)
    {
        g_ItemGloballyUniqueIdentifierGenerator.generate(
            (itemGloballyUniqueIdentifier_t*)(char*)&item.m_amp.m_pad4[0],
            G_CEnvironment()->m_serverEnvir.m_gcNo);
    }
    item.m_amp.reset();
}