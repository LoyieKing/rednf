// ----------------------------------------------------------------------------
// 6. ORIG T 0x0865eed2 sz 0x30e。space 分派：
//   1 头像域（INVEN_TYPE_AVATAR=2）：频道不可用装备校验失败→msg_146 通知返 1；
//     isCompetitionMercenary→返 0xb3；ChangeEquip(AVATAR,slot,dst)==0 时回写
//     setAvatarTypeBonus(CalcMercenaryAvatarBonus)；>0 直接返；否则
//     AvatarRechargeServer::CalcDurabilityChangeEquip(this,dstSlot)。
//   2 交易域：CheckInTrade()→返 0x13（否则落尾部）。
//   0 装备域（INVEN_TYPE_EQUIP=1）：同款不可用装备校验；
//     getCurChannelEquipslotSwitch()==1 且 dstSlot!=0xb 时 find_item(m_addInfo)
//     非空且 !IsEnableWorld(WORLD_1) → 返 1；ChangeEquip(EQUIP,...)>0 直接返。
//   其它 space 返 1。
// 尾部：0<=dstSlot<0xc 时 m_characterView.enableSaveCharacView()；
// send_equip(dstSlot)；返 0。
// ----------------------------------------------------------------------------
// [2026-09-09 AE 归零] check_tu_routed fresh ⇒ AE（orig=203 our=203，
// fn_diff_tu 0 差异行，帧 sub $0x34 与槽 -0x1c/-0x18/-0x14/-0x10/-0xc 全等），
// p167 调用点差集 CLEAR（32/32），jcc 逐位交叉验证一致（30 条 jcc 的
// 助记符+目标指令索引全等）。四处源形修正（均有 ORIG 直证）：
//   ① if/else 链 → switch(space)：ORIG `mov 0xc(%ebp),%eax; cmp $1,%eax;
//      je; cmp $2,%eax; je; test %eax,%eax; jne` 是 switch 决策链
//      （4.4 if 链恒折内存直比 cmpl $imm,mem，P251/P286）；case 叶块
//      物理序=书写序（P324）⇒ case 0（fall-through 体）→ 2 → 1 → default；
//   ② `int ret = 0;`（ORIG 头部 movl $0,-0x18）；
//   ③ equipslot 守卫合并短路 `a==1 && b!=0xb` → 嵌套 if：ORIG 为
//      `cmp $1,%al; sete %al; test %al,%al; je X; cmpl $0xb; je X`
//      （两守卫同落 X + 首条件 sete 物化 = 嵌套 if 形，P133/P225；
//      合并 && 产短路直跳 jne/je 反形）；
//   ④ find_item 实参内联：ORIG GetInvenRef 结果无槽往返
//      （`mov 0x2(%eax),%eax; mov %eax,%ebx; call G_CDataManager;
//      mov %ebx,0x4(%esp); call find_item`，P0/P183/P52），
//      声明中间 `const Inven_Item* item` 会多占一槽并把帧撑到 0x44。
// 旧判词「N82/N95 帧 0x34 vs 0x44 封顶」已作废。
int CUser::change_equip(int space, short slot, int dstSlot)
{
    int ret = 0;
    // ORIG 决策链 mov 0xc(%ebp),%eax / cmp $1 / cmp $2 / test ⇒ switch；
    // case 叶块物理序 = 书写序（P324）：0 → 2 → 1，default 收尾。
    switch (space)
    {
    case 0:
        if (G_GameWorld()->IsCheckUnusableEquipmentChannel())
        {
            const Inven_Item* item =
                getCurCharacInvenR()->GetInvenRef(1, slot);
            if (!online_preliminary::COnlinePreliminary::CheckUnusableEquipment(
                    item))
            {
                SendNotiPacketMessage(
                    g_scriptStringManager_.findString(4, "game_server_msg_146",
                                                      (bool*)0),
                    (ENUM_MESSAGE_TYPE)0);
                return 1;
            }
        }
        // ORIG 两守卫各自 je 同一出口 + 首条件 sete 物化 ⇒ 嵌套 if
        //（合并 && 会退化成短路直跳形，P133/P225）。
        if (getCurChannelEquipslotSwitch() == 1)
        {
            if (dstSlot != 0xb)
            {
                // ORIG: call GetInvenRef → mov 0x2(%eax),%eax → %ebx →
                // call G_CDataManager → call find_item（item 无槽往返，
                // P0/P183：实参表达式内联，勿声明中间指针局部）。
                CItem* itemInfo = G_CDataManager()->find_item(
                    getCurCharacInvenR()->GetInvenRef(1, slot)->m_addInfo);
                if (!(itemInfo == 0 ||
                      itemInfo->IsEnableWorld((ENUM_WORLD_TYPE)1)))
                    return 1;
            }
        }

        ret = getCurCharacInvenW()->ChangeEquip(INVEN_TYPE_EQUIP, slot,
                                                dstSlot);
        if (ret > 0)
            return ret;
        break;

    case 2:
        if (CheckInTrade())
            return 0x13;
        break;

    case 1:
        if (G_GameWorld()->IsCheckUnusableEquipmentChannel())
        {
            const Inven_Item* item = getCurCharacInvenR()->GetInvenRef(2, slot);
            if (!online_preliminary::COnlinePreliminary::CheckUnusableEquipment(
                    item))
            {
                SendNotiPacketMessage(
                    g_scriptStringManager_.findString(4, "game_server_msg_146",
                                                      (bool*)0),
                    (ENUM_MESSAGE_TYPE)0);
                return 1;
            }
        }
        if (isCompetitionMercenary())
            return 0xb3;

        ret = getCurCharacInvenW()->ChangeEquip(INVEN_TYPE_AVATAR, slot,
                                                dstSlot);
        if (ret == 0)
            setAvatarTypeBonus(
                static_cast<char>(CalcMercenaryAvatarBonus()));  // movsbl %al
        if (ret > 0)
            return ret;

        ARAD::Singleton<AvatarRechargeServer>::Get()->CalcDurabilityChangeEquip(
            this, dstSlot);
        break;

    default:
        return 1;
    }

    if (dstSlot >= 0 && dstSlot < 12)
        m_characterView.enableSaveCharacView();
    send_equip(dstSlot);
    return 0;
}