// CParty::CheckEnterEventDungeon @ 0x859ee66 —— 事件地下城条件（必备/奖励材料）。
// b28d-solo-CParty: N 封顶 —— N82 帧 0x60/0x70 + 守卫极性；259 vs 276
bool CParty::CheckEnterEventDungeon(CDungeon const* dungeon, unsigned char& flag,
                                    Inven_Item* items, int* param)
{
    // b28d-49 判词：N 封顶（N82 帧/槽）。P167 CLEAR。@0x859ee66
    // P133/P225：ORIG 0x859ee94..0x859eedc 为两层「条件失败即 return 1」，
    // 每个失败分支各自 `mov $1,%eax; jmp epilog`，不是 && 合并短路。
    if (!((dungeon->m_entryRequiredItem2.m_itemIdx != -1) ||
          (dungeon->m_entryRequiredItem.m_itemIdx != -1)))
        return 1;
    if (!((dungeon->m_entryRequiredItem2.m_itemIdx == -1) ||
          (dungeon->m_entryRequiredItem2.m_enableFlag == 1) ||
          (dungeon->m_entryRequiredItem.m_itemIdx == -1) ||
          (dungeon->m_entryRequiredItem.m_enableFlag == 1)))
        return 1;
    int i = 0;
    for (i = 0; i <= 3; ++i)
    {
        if (_checkValidUser(i) != 1)
            continue;
        char local_d = 1;
        // [G7-sub] ORIG 0x0859ef10..0x0859ef51：`shl $2,%eax; mov %eax,%ebx;
        // add 0x18(%ebp),%ebx` + `mov %eax,(%ebx)` + `mov (%ebx),%eax` —— &param[i]
        // 全程在 %ebx（callee-saved），**无指针槽** ⇒ 原码无具名指针（P183/P0）。
        // 旧码 `int* pi = &param[i]` 落槽 -0x14（+1 store，每次解引用 +1 load，帧 +4）。
        param[i] = m_member[i].m_pUser->getCurCharacInvenR()
                  ->check_item_exist(dungeon->m_entryRequiredItem2.m_itemIdx);
        if (param[i] == -1)
        {
            flag = (unsigned char)i;
            local_d = 0;
        }
        else
        {
            // ORIG 0x0859ef61..0x0859f011：&items[i] 亦全程在 %ebx
            //（`imul $0x3d; mov %eax,%ebx; add 0x14(%ebp),%ebx`），逐字段拷贝 16 条
            // 直写 `mov %eax,N(%ebx)` ⇒ 无具名指针（P0）。
            items[i] = m_member[i].m_pUser->getCurCharacInvenR()
                       ->GetInvenSlot(1, param[i]);
            if (items[i].m_addInfo2 < dungeon->m_entryRequiredItem2.m_itemCount)
            {
                flag = (unsigned char)i;
                local_d = 0;
            }
        }
        if (!local_d)
        {
            local_d = 1;
            param[i] = m_member[i].m_pUser->getCurCharacInvenR()
                      ->check_item_exist(dungeon->m_entryRequiredItem.m_itemIdx);
            if (param[i] == -1)
            {
                flag = (unsigned char)i;
                local_d = 0;
            }
            else
            {
                items[i] = m_member[i].m_pUser->getCurCharacInvenR()
                           ->GetInvenSlot(1, param[i]);
                if (items[i].m_addInfo2 < dungeon->m_entryRequiredItem.m_itemCount)
                {
                    flag = (unsigned char)i;
                    local_d = 0;
                }
            }
        }
        if (!local_d)
            return 0;
    }
    return 1;
}