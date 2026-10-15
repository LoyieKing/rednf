// b28d-65：真差异已修（Move_Item 实参来源改形参 type/space，见函数内注释）；残余
//   N93（slotItem1/slotItem2 两 Inven_Item 槽向相反，类对象直接初始化不可翻）+
//   4.4 短路/比较不物化（ORIG 0x08499c8a `mov$1/mov$0+test`、0x08499ce8 setne+test 岛）
//   + 对齐 nop ⇒ 封顶。
// [P8S1 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 DIFF 系 objdump 无 -r 的假残差
int CExpandEquipslot::Change_Equip(CUser* pUser, int type, int slot,
                                   int space, int expandSlot)
{
    if (pUser == 0)
    {
        LogManager::logFormat(
            1, "ExpandEquipslot.cpp",
            __PRETTY_FUNCTION__,
            0xd8, "Fail Change_Equip : pUser is NULL");
        return 1;
    }
    if (pUser->get_state() < 3)
    {
        LogManager::logFormat(
            1, "ExpandEquipslot.cpp",
            __PRETTY_FUNCTION__,
            0xde, "Fail Change_Equip : pUser is not valid state");
        return 0x13;
    }
    CInventory* invenW = pUser->getCurCharacInvenW();
    switch (type)
    {
    case 0:
    {
        const Inven_Item* srcItem =
            pUser->getCurCharacInvenR()->GetInvenRef(1, slot);
        CItem* item = G_CDataManager()->find_item(srcItem->m_addInfo);
        switch (space)
        {
        case 0x12:
        {
            if (invenW->CheckValidSlot(1, slot) != 1)
            {
                return 1;
            }
            if (invenW->CheckValidSlot(5, expandSlot) != 1)
            {
                return 1;
            }
            if (pUser->getCurChannelEquipslotSwitch() == 0)
            {
                if (item != 0 &&
                    !item->IsEnableWorld((ENUM_WORLD_TYPE)1))
                {
                    return 1;
                }
            }
            Inven_Item slotItem1 = invenW->GetInvenSlot(1, slot);
            if (slotItem1.m_seal != 0)
            {
                int secuRet = GlobalData::s_pSecuProtectionField->Check(pUser, (SECURITY_PROTCTION)0x16);
                if (secuRet != 0)
                {
                    return secuRet;
                }
            }
            if (pUser != 0)
            {
                // b28d-65 [P226/P214①]：ORIG 0x08499c64..0x08499ca2 为短路 && 的
                // `je L_false ×2 / mov $1 / jmp / L_false: mov $0 / test %al; je`
                // 零槽物化岛；4.4 对 `if (A && B)` 恒 jump-thread 成直跳。
                // 语句表达式包住整个条件即复现（探针 t25/p2_ee.o 逐条同形，
                // sret 缓冲仍落浅槽 -0x59）。
                if (({ pUser->CheckItemLock(1, slot) &&
                       invenW->GetInvenSlot(1, slot).m_seal != 0; }))
                {
                    return 0xd5;
                }
            }
            int moveRet = 0;
            // b28d-65 真差异修正：ORIG 0x08499cb0 段压栈为 (pUser,type,slot,space,expandSlot-10)
            // —— 实参来源是形参 type/space（分支保证其值为 0/0x12），非字面量（P97/P171 照抄来源）。
            moveRet = Move_Item(pUser, type, slot, space, expandSlot - 10);
            // b28d-65 [P225]：ORIG 0x08499ce4 段 `cmpl $0,-0x10; setne %al;
            // test %al,%al; je`（4.1 对 int 条件物的物化）；4.4 对 if 恒折
            // `cmpl;je`。while+return 形复现 setne 物化（探针 t22/p2_ee.o）。
            while (moveRet != 0)
            {
                return moveRet;
            }
            alter();
            return 0;
        }
        default:
            return 1;
        }
    }
    default:
        return 1;
    }
}