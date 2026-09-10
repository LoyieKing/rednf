void Dispatcher_PcRoomRentItem::giveDimensionItems(CUser* user,
                                                   InstanceRentalSystem::RentInput& input)
{
    if (user == 0 || input.m_machineKey != 2)            // 仅 dimension(machineKey==2)
    {
        return;
    }
    int selectionIdx = 0;
    if (!pc_room::GetSelectionIndex(*user, selectionIdx, 2))
    {
        return;
    }
    int growType = static_cast<int>(user->getCurCharFirstGrowType());   // W 0x08110c94
    int job = user->get_charac_job();                                    // W 0x080fdf20

    // ORIG 0x081e58c3: rewardData->getRewardGiveItems(selectionIdx, job, growType)，
    // 非空则逐项组装 RentInElem 入 input.m_elems（m_type=1 GiveList 标记）
    DimensionActivationRewardData* rewardData =
        reinterpret_cast<DimensionActivationRewardData*>(
            reinterpret_cast<char*>(G_CDataManager()) + 0x4b34);
    std::vector<EventRewardItemInfo>* items =
        rewardData->getRewardGiveItems(selectionIdx, job, growType);
    if (items != 0)
    {
        for (int i = 0; i < static_cast<int>(items->size()); ++i)
        {
            InstanceRentalSystem::RentInput::RentInElem elem;
            elem.m_sectionIdx = static_cast<unsigned char>(selectionIdx);
            elem.m_itemIdx = static_cast<unsigned char>(i);
            elem.m_type = 1;                             // GiveList 路径标记
            input.m_elems.push_back(elem);
        }
    }
