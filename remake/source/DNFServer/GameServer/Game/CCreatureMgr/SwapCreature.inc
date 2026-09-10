// [2026-09-11 收口判词 · P75/N-G 实参装载调度] ORIG 0x08339420（189 条）= 我方 189 条；
//   p167 仅 D1/D2 同址别名（P93 豁免）。残余 = 4 处调用点实参装载方向互反：
//   ① 0x8339613 SetSlotNo：ORIG this-first `mov -0x10(%ebp),%eax; mov 0x1c(%ebp),%edx;
//      mov %edx,0x4(%esp); mov %eax,(%esp)` vs 4.4 arg-first
//      （`mov 0x1c,%eax; mov %eax,0x4(%esp); mov -0x10,%eax; …`）；
//   ② 0x83395aa EquipCreatureItem：ORIG 逐个 load-store，4.4 多一次 `mov -0xc(%ebp),%edx`
//      预载；③ 0x833962b 同 ①；④ 0x83395e1 同 ②。
//   已试形：P277 具名派生指针/static_cast 同型（156→158 劣化，已回滚）；
//   显式限定调用 `CCreature::SetNoCharge(p,b)` 非法（成员调用需对象，编译错）⇒
//   判跨版本实参装载调度（P75/N-G），非源码可择 ⇒ NEAR 封顶（助记符序列全等）。
bool CCreatureMgr::SwapCreature(Inven_Item* item1, int slot1, int type1,
                                Inven_Item* item2, int slot2, int type2)
{
    CCreatureItem* left = 0;
    CCreatureItem* right = 0;

    if (item1->m_field1 == Inven_Item::ITEM_TYPE_CREATURE_EQUIP ||
        item2->m_field1 == Inven_Item::ITEM_TYPE_CREATURE_EQUIP)
    {
        left = FindCreatureItem(item1->get_add_info());
        right = FindCreatureItem(item2->get_add_info());

        if (!IsCreatureEquipmentScope(slot1))
        {
            // P200：ORIG 0x83394xx `movzwl %ax,%ebx` 直落 callee-saved（无槽往返）
            // ⇒ 短整表达式作普通实参直传，禁具名中间局部。
            // P227/P87：ORIG 在 ctor 前 `movzwl %ax,%ebx` 求值 uid 并跨 ctor
            // 保活 ⇒ 全表达式临时链。
            cMyTrace("bool user_creature::CCreatureMgr::SwapCreature("
                     "Inven_Item*, int, int, Inven_Item*, int, int)",
                     0x9a5, 5)(
                "IsCreatureEquipmentScope() uid(%d) slot_left(%d)\n",
                static_cast<unsigned short>(m_user->get_unique_id()), slot1);
            return false;
        }
        if (!IsCreatureEquipmentScope(slot2))
        {
            cMyTrace("bool user_creature::CCreatureMgr::SwapCreature("
                     "Inven_Item*, int, int, Inven_Item*, int, int)",
                     0x9ab, 5)(
                "IsCreatureEquipmentScope() uid(%d) slot_right(%d)\n",
                static_cast<unsigned short>(m_user->get_unique_id()), slot2);
            return false;
        }

        // 目标侧为 0x16 号槽且来源计数 0 → 视作穿戴/脱下链
        // （FindCreatureItem 返回 CCreatureItem*；ORIG 形参为 CCreature*，镜像同址转换）
        if (slot1 == 0x16 && type1 == 0)
        {
            if (!EquipCreatureItem(reinterpret_cast<CCreature*>(left),
                                   reinterpret_cast<CCreature*>(right)))
                return false;
        }
        else if (slot2 == 0x16 && type2 == 0)
        {
            if (!EquipCreatureItem(reinterpret_cast<CCreature*>(right),
                                   reinterpret_cast<CCreature*>(left)))
                return false;
        }
    }

    // 来源侧计数为 0 时槽号 +0xd8（ORIG add $0xd8 实证）
    if (type2 == 0)
        slot2 += 0xd8;
    if (type1 == 0)
        slot1 += 0xd8;

    // [P277 试形失败] static_cast 同类型形不改变装载序（仍实参先压，189 条不变）⇒
    // 回滚；残余 = ORIG 0x8339613/0x833962b this-first 装载序（P75/N-G 跨版本）。
    if (left != 0)
        left->SetSlotNo(slot2);
    if (right != 0)
        right->SetSlotNo(slot1);

    CDBMsgSender sender(static_cast<ENUM_INTERNALPACKET>(0x8c), m_user);
    // ORIG 0x8339679 `xor $1;test;je <成功>` + ebx 汇聚（mov $0/$1 → mov %ebx,%eax）
    // ⇒ 内联判定 + 早返形（P63/P194），禁具名 ok 局部。
    if (!sender.SwapCreatureItem(m_user, item1, slot1, item2, slot2))
        return false;
    sender.Send();
    return true;
}
