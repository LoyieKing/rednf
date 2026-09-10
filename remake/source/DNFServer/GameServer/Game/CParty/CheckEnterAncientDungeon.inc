bool CParty::CheckEnterAncientDungeon(CDungeon const* dungeon, unsigned char& flag,
                                      Inven_Item* items, int* param)
{
    // b28d-49 判词：N 封顶（N82 帧/槽）。P167 CLEAR。@0x859e528
    if (dungeon == 0)
        return 1;
    if (dungeon->isTowerOfDespairDungeon())
        return 1;
    if ((unsigned char)dungeon->m_entryRestrictFlag != 0)   // ORIG dungeon+0x85c（set_dungeon 自
                                              //   STDungeonScript+0x88 复制的事件副本标志）
    {
        if (CheckEnterEventDungeon(dungeon, flag, items, param) != 1)
            return 0;
        return 1;
    }
    if (dungeon->m_entryRequiredItem.m_itemIdx == -1)
        return 1;
    if (dungeon->m_entryRequiredItem.m_enableFlag != 1)
        return 1;
    if (m_battleField.m_bloodState == 1)
        return 1;
    // b28d-solo-CParty：ORIG 0x859e6e4 处两条连续 `movl $0x0,-0xc(%ebp)` =
    // P191 双初始化形（`int i=0;` + `for(i=0;…)` 两条都写）；且 get_index()
    // 结果不落槽（`call; cmp $0x5dc,%eax; sete` 寄存器直比，P197④）⇒ 内联。
    int i = 0;
    for (i = 0; i <= 3; ++i)
    {
        if (_checkValidUser(i) != 1)
            continue;
        if (dungeon->get_index() == 0x5dc)
        {
            // P15/P38：ORIG 0x859e63b 先把 &param[i] 算入 %ebx，store 后立即
            // `mov (%ebx),%eax` 回读 ⇒ 赋值嵌入条件（SAVE_EXPR 地址复用），
            // 拆两句会重算地址三次。
            if ((param[i] = m_member[i].m_pUser->getCurCharacInvenR()
                     ->check_vilmark_dungeon_item_exist(
                         dungeon->m_entryRequiredItem.m_itemIdx)) == -1)
            {
                flag = (unsigned char)i;
                return 0;
            }
        }
        else
        {
            if ((param[i] = m_member[i].m_pUser->getCurCharacInvenR()
                     ->check_item_exist(dungeon->m_entryRequiredItem.m_itemIdx)) == -1)
            {
                flag = (unsigned char)i;
                return 0;
            }
        }
        // ORIG 0x859e700/0x859e7b3：赋值目标 &items[i] 与随后的 m_addInfo2 读取
        // 各自独立重物化（imul $0x3d）⇒ 无具名引用槽（P183/P0 表达式直写）。
        items[i] =
            m_member[i].m_pUser->getCurCharacInvenR()->GetInvenSlot(1, param[i]);
        if (items[i].m_addInfo2 < dungeon->m_entryRequiredItem.m_itemCount)
        {
            flag = (unsigned char)i;
            return 0;
        }
    }
    return 1;
