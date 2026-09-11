// ORIG _ZN13user_creature12CCreatureMgr22SetCreatureItemAddInfoEii T 0x0833b620
// （SetCreatureItemAddInfo.md）：背包区 3 槽物存在且 idx(+0x02) 非空 →
// set_add_info(value) 返 1；否则 0。
bool CCreatureMgr::SetCreatureItemAddInfo(int slot, int value)
{
    // ORIG 0x833b620+7：entry `movl $0,-0xc`（item 两段式 P197③）；refW 结果
    // 无槽往返（P0）⇒ 嵌套实参直传 GetInvenRef 首参。
    Inven_Item* item = 0;
    item = m_user->getCurCharacInvenRefW()->GetInvenRef(3, slot);
    if (item == 0)
        return false;
    if (item->m_addInfo == 0)   // +0x02
        return false;
    item->set_add_info(value);
    return true;
}