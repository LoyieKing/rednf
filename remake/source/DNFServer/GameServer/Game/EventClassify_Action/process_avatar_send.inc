// ============================================================================
// process_avatar_send（ORIG 0x0810d70c）
// genIPGNo(type7) 取 IPG 号 → AddAvatarItem(idx, usablePeriod,0,0,-1,ipg,2,0,0)
// 成功后按槽内物品与当前时间回填剩余日期到 item +0x07 并清 upgrade 位。
// ============================================================================
// [b28d-solo-EventClassify_Action] ORIG 0x0810d70c，79=79 AE。
//   本件修法：①ipg/usablePeriod 去具名局部改内联实参（P0/P200）⇒ 复现 ORIG
//     esi(ipg)/ebx(usablePeriod)/edi(item.m_addInfo) callee-saved 独占；
//   ②GetRemainDate 的 addInfo2 实参改匿名嵌套 sret
//     `GetRemainDate(GetInvenSlot(2,slot).m_addInfo2, now)`（P278）⇒ ipgBuf@-0x75、
//     sret@-0x5d 槽序与帧 0x9c 与 ORIG 全等（具名 slotItem 时两者互换、NEAR）。
int CEventActionMng::process_avatar_send(CUser* user, Inven_Item& item,
                                         CItem const* citem)
{
    char ipgBuf[24];
    int slot = -1;
    slot = user->getCurCharacInvenW()->AddAvatarItem(
        item.m_addInfo, citem->getUsablePeriod(),
        /*b*/ false, /*c*/ 0, /*d*/ (char)-1,
        WongWork::CGenUniqueNo::genIPGNo(
            (WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE)7,
            (unsigned int)user->getCurCharacNo(), ipgBuf),
        (eAvatarItemAddReason)2, /*e*/ false, /*f*/ 0);

    if (slot != -1)
    {
        int now = GlobalData::s_systemTime_.getCurSec();
        item.m_addInfo2 =                                   // +0x07
            user->getCurCharacInvenR()->GetAvatarItemMgrR()->GetRemainDate(
                user->getCurCharacInvenR()->GetInvenSlot(2, slot).m_addInfo2,
                now);
        item.SetUpgrade(0);
    }
    return slot;
}