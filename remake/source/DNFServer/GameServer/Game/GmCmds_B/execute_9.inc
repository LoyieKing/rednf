// ORIG execute 0x084aafb0：逐 avatar 槽 [0,0x68] 删除非空物品并 SendAvatarEvent，
// 末尾 send_itemspace(AVATAR)。
void CClearAvatarInvenCmd::execute()
{
    CUser* user = GetUser();
    // [b23S3C] P48 去缓存：ORIG getCurCharacInvenR ×2（+0x28/+0x58）、
    // getCurCharacInvenW ×2（+0x78/+0x10f）各用点独立求值，无 invenR/invenW 槽；
    // addInfo2 预清槽 -0x10（+0x1c movl $0x0）⇒ 循环外 `int addInfo2 = 0;`；
    // NumberToString 结果直落 0x14(%esp) 实参槽（无 acc 局部，P0 寄存器直传）。
    int addInfo2 = 0;
    for (int slot = 0; slot <= 0x68; ++slot)
    {
        if (user->getCurCharacInvenR()->m_pAvatarSlot[slot].m_addInfo != 0)
        {
            addInfo2 = user->getCurCharacInvenR()
                           ->m_pAvatarSlot[slot].m_addInfo2;
            if (!user->getCurCharacInvenW()
                     ->delete_item(INVEN_TYPE_AVATAR, slot, 1,
                                   eItemDelReason_3, true))
            {
                LogManager::logFormat(
                    1, "GameMaster.cpp",
                    __PRETTY_FUNCTION__, 0x64d,
                    "CClearAvatarInvenCmd::execute, pUser(%s)->getCurCharacInvenW()->delete_item return FAILED",
                    NumberToString(user->get_acc_id(), 0));
                return;
            }
            user->getCurCharacInvenW()->SendAvatarEvent(0x36, addInfo2, 1,
                                                        slot + 10, 0);
        }
    }
    // ORIG xor$1 形需 send_itemspace 返 bool（Main 已代落 CUser.h:2843）。
    if (user->send_itemspace(1) != 1)   // ENUM_ITEMSPACE_AVATAR=1
    {
        LogManager::logFormat(
            1, "GameMaster.cpp",
            __PRETTY_FUNCTION__, 0x656,
            "CClearAvatarInvenCmd::execute, pUser(%s)->send_itemspace( ENUM_ITEMSPACE_AVATAR ) return FAILED",
            NumberToString(user->get_acc_id(), 0));
    }
}