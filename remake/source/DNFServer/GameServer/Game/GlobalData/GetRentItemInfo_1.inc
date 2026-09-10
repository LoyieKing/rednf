bool RentalMachineDimension::GetRentItemInfo(
    CUser& user, const InstanceRentalSystem::RentInElem& inElem,
    EventRewardItemInfo& out)
{
    int selectionIdx = 0;
    if (!pc_room::GetSelectionIndex(user, selectionIdx, 2))
    {
        return false;
    }
    char job = 0;
    if (!pc_room::GetCharacterJob(user, job))
    {
        return false;
    }

    const int packageKey =
        makeFairPVPEquipmentPackageKey((int)(signed char)job, selectionIdx);

    // [b28d-PoolGlobalData] 二压：ORIG 0x0826f8ba 无 `list=0` 初始化死槽写、
    // 无 growType 槽（0x0826f8d0 `movsbl %al,%edx` 直落实参位）、else 支第二实参
    // 为**常量 2**（0x0826f8f2 `movl $0x2,0x4(%esp)` 直证，非重读 m_type）、
    // m_type 比较为字节形（0x0826f8c1 `cmp $0x1,%al`）。
    std::vector<std::vector<EventRewardItemInfo> >* list;
    if (inElem.m_type == 1)
    {
        list = (std::vector<std::vector<EventRewardItemInfo> >*)
            GetRewardItemGiveList((int)(signed char)job,
                                  (int)(signed char)user.getCurCharFirstGrowType());
    }
    else
    {
        // ORIG 0x0826f900：pc_room::GetRewardItemSelectionList(packageKey, 2)。
        list = pc_room::GetRewardItemSelectionList(packageKey, 2);
    }
    if (list == 0)
    {
        return false;
    }

    const int sz0 = (int)list->size();
    if ((int)(signed char)inElem.m_sectionIdx < 0)
    {
        goto L_ret0;
    }
    if ((int)(signed char)inElem.m_sectionIdx < sz0)
    {
        goto L_pass0;
    }
L_ret0:
    return false;
L_pass0:
    std::vector<EventRewardItemInfo>* inner =
        &list->at((unsigned int)(int)(signed char)inElem.m_sectionIdx);
    {
        const int sz1 = (int)inner->size();
        if ((int)(signed char)inElem.m_itemIdx < 0)
        {
            goto L_ret1;
        }
        if ((int)(signed char)inElem.m_itemIdx < sz1)
        {
            goto L_pass1;
        }
    L_ret1:
        return false;
    L_pass1: ;
    }
    out = (*inner)[(unsigned int)(int)(signed char)inElem.m_itemIdx];
    return true;
