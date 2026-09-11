// ---- PcRoom::Rent —— T 0x082704e6（Rent.md 全文转写）------------------------
// [b28d-solo-GlobalData] 归零 169=169：①真语义 bug——首守卫 ORIG 为
// `!IsPcRoomUser && !m_field11`（两条 je 同落继续块，旧写 `|| m_field11==1` 逻辑相反）；
// ②RentInput::m_field11 uchar→bool（P248 档②）+ 嵌套 if（P133①）；
// ③m_bEquip 取反+两臂交换（P47/P222）| ORIG 0x082704e6
int RentalMachinePcRoom::Rent(CUser& user,
                              const InstanceRentalSystem::RentInput& input,
                              InstanceRentalSystem::RentOutput& output)
{
    // ORIG 0x082704f4..0x08270513（真语义 bug 修正）：`call IsPcRoomUser; xor $1;
    // test; je <继续>` + 内层 `movzbl 0x11; xor $1; test; je <继续>`；两条 je 同落
    // 继续块 ⇒ 原码是 `if (!IsPcRoomUser(user) && !input.m_field11) return 7;`
    // （旧写 `|| m_field11 == 1` 逻辑相反）。
    // P133①/P225 推广：ORIG 两条守卫各自 `je <继续>`（非汇聚物化）⇒ 嵌套 if
    // （外层 !A、内层 !B → return 7），4.4 才逐条复现 `xor $1; test; je`。
    if (!pc_room::IsPcRoomUser(user))
    {
        if (!input.m_field11)
        {
            return 7;   // ENUM_ERROR 7
        }
    }
    if (!pc_room::CheckRentableEnvironment(user))
    {
        return 7;   // ENUM_ERROR 7
    }
    int err = CheckupAllRentalItems(user, input);
    if (err != 0)
    {
        return err;
    }

    pc_room::DeleteAllRentalItemInSameSectionForNewRent(user, output.m_deleted,
                                                        1);
    // ORIG 0x082704e6：const_iterator 三段循环（p167 MISSING 实证）。
    for (std::vector<InstanceRentalSystem::RentInput::RentInElem>::const_iterator
             it = input.m_elems.begin();
         it != input.m_elems.end(); ++it)
    {
        const InstanceRentalSystem::RentInElem& elem = *it;
        EventRewardItemInfo info;
        if (!pc_room::GetRentItemInfo(user, (char)elem.m_sectionIdx,
                                      (char)elem.m_itemIdx, info, 1))
        {
            LogManager::logFormat(
                1, "RentalSystem.cpp",
                __PRETTY_FUNCTION__,
                0xf8,
                "Rent error. GetRentItemInfo return false. "
                "scriptSectionIndex(%d), scriptSeqIndex(%d)",
                (int)(signed char)elem.m_sectionIdx,
                (int)(signed char)elem.m_itemIdx);
            return 1;   // ENUM_ERROR 1
        }
        // ORIG `test %al,%al; je <ProcessInsertItem>`（equip 直落）⇒ 条件取反 +
        // 两臂交换复现 4.1 else-first 布局（P47/P222，同 Dimension::Rent）。
        if (elem.m_bEquip != 0)
        {
            pc_room::ProcessEquipItem(user, elem, info, output, 0x3d);
        }
        else
        {
            pc_room::ProcessInsertItem(user, elem, info, output, 0x3d);
        }
        if (input.m_logFlag != 0)
        {
            log(user, info.m_field0);
        }
    }
    return 0;   // 0=成功
}