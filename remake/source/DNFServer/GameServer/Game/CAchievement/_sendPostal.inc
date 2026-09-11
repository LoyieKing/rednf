// [b28d-solo-CAchievement N7/N109 封顶] 三保活值 callee-saved 角色互换
//（ORIG GetServerGroup→edi、subject.c_str→esi、getCurCharacNo→ebx vs 4.4
//  →esi/→ebx/→edi；74 条长度/助记符全等，无虚调用，实参求值序相同）。
// P248 三档类型旋钮不适用（无 store 序/类型差）；本 TU 已路由 4.4.6 后端仍不翻。
// [L5cBattleMonAch 2026-09-11 复测] 75/76 保持 NEAR；逐条核对仅上述三处寄存器
// 角色差（其余含 0x24/0x20/0x18/0x14/0x10/0xc/0x8 压参序、字符串实参来源全等），
// 无可行源码旋钮 ⇒ N7/N109 封顶。
// ORIG 0x0828c152。
void CAchievement::_sendPostal(int itemIdx, int addInfo, std::string sender,
                               std::string subject)
{
    Inven_Item item;
    CItem* pItem = G_CDataManager()->find_item(itemIdx);
    if (pItem == 0)
    {
        return;
    }
    item.m_addInfo = itemIdx;
    pItem->make_item(item);
    item.set_add_info(addInfo);
    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        sender.c_str(), item, 0, (unsigned int)m_user->getCurCharacNo(),
        subject.c_str(), (int)subject.size(), 0, m_user->GetServerGroup(),
        false, false);
}