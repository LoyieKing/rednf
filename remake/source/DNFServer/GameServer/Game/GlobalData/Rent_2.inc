// ---- Dimension::Rent —— T 0x0826ec22（Rent.md 全文转写）---------------------
// [b28d-solo-GlobalData] 归零 176=176：m_bEquip 分支条件取反+两臂交换复现
// ORIG else-first 布局（`test;je <ProcessInsertItem>`，P47/P222）；p167 CLEAR 21/21 | ORIG 0x0826ec22
// [R4lead 2026-09-12 · P174] 返回类型 int→ENUM_ERROR：ORIG prett 实串
// `virtual ENUM_ERROR RentalMachineDimension::Rent(CUser&, const InstanceRentalSystem::
// RentInput&, InstanceRentalSystem::RentOutput&)` @0x8bf9f60（size 0x83）直证。
ENUM_ERROR RentalMachineDimension::Rent(CUser& user,
                                        const InstanceRentalSystem::RentInput& input,
                                        InstanceRentalSystem::RentOutput& output)
{
    if (!pc_room::CheckRentableEnvironmentAtChannelType(user))
    {
        return (ENUM_ERROR)7;   // ENUM_ERROR 7
    }
    if (!checkRentalLevel(user.get_charac_level()))
    {
        return (ENUM_ERROR)14;  // ENUM_ERROR 0xe（14）
    }
    if (!checkRentalGrowJob((int)(signed char)user.getCurCharFirstGrowType()))
    {
        return (ENUM_ERROR)254;   // ENUM_ERROR 0xfe（254）
    }
    int err = CheckupAllRentalItems(user, input);
    if (err != 0)
    {
        return (ENUM_ERROR)err;
    }

    pc_room::DeleteAllRentalItemInSameSectionForNewRent(user, output.m_deleted,
                                                        2);
    // ORIG 0x0826f3xx：const_iterator 三段循环（begin/end/ne/deEv/ppEv，
    // p167 MISSING 实证），非 size()/operator[] 下标形。
    for (std::vector<InstanceRentalSystem::RentInput::RentInElem>::const_iterator
             it = input.m_elems.begin();
         it != input.m_elems.end(); ++it)
    {
        const InstanceRentalSystem::RentInElem& elem = *it;
        EventRewardItemInfo info;
        if (!GetRentItemInfo(user, elem, info))
        {
            LogManager::logFormat(
                1, "RentalMachineDimension.cpp",
                __PRETTY_FUNCTION__,
                0x43,
                "Rent error. GetDimensionRentItemInfo return false. "
                "scriptSectionIndex(%d), scriptSeqIndex(%d)",
                (int)(signed char)elem.m_sectionIdx,
                (int)(signed char)elem.m_itemIdx);
            return (ENUM_ERROR)1;   // ENUM_ERROR 1
        }
        // ORIG 0x0826ed9f `test %al,%al; je <ProcessInsertItem>`（equip 直落）
        // ⇒ 条件取反 + 两臂交换复现 4.1 的 else-first 块布局（P47/P222 族）。
        if (elem.m_bEquip != 0)
        {
            pc_room::ProcessEquipItem(user, elem, info, output, 0x3e);
        }
        else
        {
            pc_room::ProcessInsertItem(user, elem, info, output, 0x3e);
        }
        if (input.m_logFlag != 0)
        {
            log(user, info.m_field0);
        }
    }
    return (ENUM_ERROR)0;   // 0=成功
}