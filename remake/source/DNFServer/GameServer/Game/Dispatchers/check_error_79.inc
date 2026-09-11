// ==== Dispatcher_AvatarOptionChange::check_error (ORIG 0x081e0a40) ====
// 校验链：user/状态门 → Secu 保护域(0x2a) → 消耗品槽一致性/数量 → 消耗品为可堆叠的
// avatar option change 道具 → 目标槽物品一致性 → 目标为非堆叠 avatar 装备 → 锁定检查
// → job/type/grade 条件或限定集合 → option 值合法性。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x081e0a40 orig/ours=289/298 ndiff-blocks=29
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: sub    $0xc4,%esp | mov    &_ZN10GlobalData22s_pSecuProtectionFieldE,%eax | mov    %eax,-0x24(%ebp)
int Dispatcher_AvatarOptionChange::check_error(CUser* user, MSG_BASE& msg)
{
    // [DispLaneB 2026-09-11] P0/P183：ORIG 本函数无 msg 缓存槽 —— 0x10(%ebp) 形参直读
    //   7 次（如 0x081e0aa6 段 `mov 0x10(%ebp),%eax; movzwl 0x..(%eax),%eax`），
    //   我方原引用局部多占 -0x28 并抬帧 0x10（0xd4 vs ORIG 0xc4）⇒ 改编译期内联式，
    //   每个 m.xxx 在使用点经形参直读（语义不变、零新增符号）。
    if (user == 0)
        return -1;
    if (user->get_state() <= 2)
        return -1;

    int secuErr = GlobalData::s_pSecuProtectionField->Check(user,
                                                            (SECURITY_PROTCTION)0x2a);
    if (secuErr != 0)
        return secuErr;
    // [g05 试形 F 负结论 2026-09-11] 改用引用局部 `AocMsgView& m = *(AocMsgView*)&msg;`
    //   （同 TU process_312/read_313 先例）⇒ **劣化**：帧 0xc4→0xd4（引用局部多占 0x10）、
    //   ours 296→298、gap −7→−9 ⇒ 已回滚维持宏（编译期内联式；历史 DispLaneB 同结论）。
#define m (*reinterpret_cast<AocMsgView*>(&msg))
    // [G1-Dispatchers] P48：ORIG 两处 GetInvenSlot 各自重取 `getCurCharacInvenR()`
    // （p167 ORIG-only ×1；我方原用 invR 缓存 ⇒ 少一次调用）。故不设具名 invR。
    Inven_Item consumeSlot =
        user->getCurCharacInvenR()->GetInvenSlot(INVEN_TYPE_EQUIP, m.m_consumeSlot);
    if (consumeSlot.m_addInfo != m.m_consumeItemIdx)
        return 0x11;
    if (consumeSlot.m_addInfo2 <= 0)
        return 0x16;

    CStackableItem* consume =
        static_cast<CStackableItem*>(G_CDataManager()->find_item(consumeSlot.m_addInfo));
    if (consume == 0)
        return 0x11;
    if (!consume->is_stackable())
        return 0x11;
    if (!consume->IsAvatarOptionChangeItem())
        return 0x11;

    Inven_Item avatarSlot =
        user->getCurCharacInvenR()->GetInvenSlot(INVEN_TYPE_AVATAR, m.m_avatarSlot);
    if (avatarSlot.m_addInfo != m.m_equipItemIdx)
        return 0x11;

    CItem* equip = G_CDataManager()->find_item(m.m_equipItemIdx);
    if (equip == 0)
        return 0x11;
    if (equip->is_stackable())  // 目标必须是非堆叠装备（ORIG 判断取反）
        return 0x11;
    if (!equip->IsAvatarItem())  // ORIG vtable+0x10 无参虚调用（IsAvatarItem 槽）
        return 0x11;
    if (user->CheckItemLock(INVEN_TYPE_AVATAR, m.m_avatarSlot))
        return 0xd5;

    // 消耗品尾部三字节选项条件区：+0x365 job、+0x366 type、+0x367 grade
    // （AvatarOptionChangeType 3 字节视图：低 ushort = job/type，高字节 = grade）
    // [Lane1Dispatchers 2026-09-10 · Main「视图 cast 反模式」修法] 原
    //   `*reinterpret_cast<const unsigned short*>(&consume->m_avatar365.m_field0)`
    //   取址 cast 在 4.4 恒产 `lea 0x365(%eax),%edx; movzwl (%edx)`；改用
    //   Lane34 已落盘的 union 16 位视角 `m_jobType16`（同址零布局）⇒ 单条 disp 读。
    // [L7g5 2026-09-11 试·已回滚] 改成 ORIG 的具名 byte 指针形
    //   `const unsigned char* tail = (const char*)consume + 0x365; tail[0..2]`
    //   ⇒ 实测劣化：298→**305**（+7）。原因：ORIG 的 -0xc 指针槽形在 4.4 下
    //   使 `tail[i]` 每点重算 `mov -0xc(%ebp),%eax; movzbl i(%eax)`，且 `empty()`
    //   的 `lea 0x368(%eax)` 退化为 add；净增 7 条 ⇒ 维持 ushort 单条 disp 读。
    const unsigned short tail365 = consume->m_avatar365.m_jobType16;
    const signed char optJob = (signed char)(tail365 & 0xff);
    const signed char optType = (signed char)((tail365 >> 8) & 0xff);
    const signed char optGrade = (signed char)consume->m_avatar365.m_field2;

    if (consume->m_set368.empty())
    {
        if ((equip->check_job_type((char)optJob) != 1) && optJob != 0xb)
            return 0xfe;
        if (equip->GetItemType() != (int)optType && optType != 0x1a)
            return 0xfd;
        if (equip->get_grade() != (int)optGrade && optGrade != 0)
            return 0xfc;
    }
    else
    {
        // ORIG 调 const set::find（_ZNKSt3setIiSt4lessIiESaIiEE4findERKi）→ 经 const 引用。
        const std::set<int>& set368 = consume->m_set368;
        if (set368.find(avatarSlot.m_addInfo) == set368.end())
        {
            return 0xff;
        }
    }

    // ORIG m_set380 同样走 const find。
    const std::set<int>& set380 = consume->m_set380;
    if (!set380.empty() &&
        set380.find((int)(signed char)m.m_option) != set380.end())
    {
        return 0xfb;
    }

    if (!static_cast<const CEquipItem*>(equip)->IsAvatarOptionValid(m.m_option))
        return 0xfa;

    return 0;
}