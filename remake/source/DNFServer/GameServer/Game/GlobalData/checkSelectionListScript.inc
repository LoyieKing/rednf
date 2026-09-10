bool RentalMachineDimension::checkSelectionListScript(
    const std::vector<std::vector<EventRewardItemInfo> >& selectionList)
{
    // ORIG 0x0826f14e 段（p167 MISSING 迭代器族）：两层均为 const_iterator
    // 遍历（begin/end/operator*/operator++/operator!=），非下标形（P175 反例）。
    for (std::vector<std::vector<EventRewardItemInfo> >::const_iterator
             sit = selectionList.begin();
         sit != selectionList.end(); ++sit)
    {
        const std::vector<EventRewardItemInfo>& items = *sit;
        for (std::vector<EventRewardItemInfo>::const_iterator iit = items.begin();
             iit != items.end(); ++iit)
        {
            // ORIG 0x0826f232 同形：`call operator*; mov %eax,-0x14` 引用绑定。
            const EventRewardItemInfo& info = *iit;
            CItem* item = G_CDataManager()->find_item((int)info.m_field0);
            if (item == 0)
            {
                std::cerr << "[RentalDimensionSystem::Ready()] Can`t find item["
                          << (unsigned)info.m_field0 << "]" << std::endl;
                return 0;
            }
            if (item->IsImpossibleAmplify() != 1)
            {
                std::cerr << "[RentalDimensionSystem::Ready()] Impossible "
                             "contents(amplify upgrade) is not checked. ["
                          << (unsigned)info.m_field0 << "]" << std::endl;
                return 0;
            }
            if (item->IsImpossibleCharacCargo() != 1)
            {
                std::cerr << "[RentalDimensionSystem::Ready()] Impossible "
                             "contents(charac cargo) is not checked. ["
                          << (unsigned)info.m_field0 << "]" << std::endl;
                return 0;
            }
            if (item->IsImpossibleDisjoint() != 1)
            {
                std::cerr << "[RentalDimensionSystem::Ready()] Impossible "
                             "contents(disjoint) is not checked. ["
                          << (unsigned)info.m_field0 << "]" << std::endl;
                return 0;
            }
            if (item->IsImpossibleGift() != 1)
            {
                std::cerr << "[RentalDimensionSystem::Ready()] Impossible "
                             "contents(gift) is not checked. ["
                          << (unsigned)info.m_field0 << "]" << std::endl;
                return 0;
            }
            if (item->IsImpossibleUpgrade() != 1)
            {
                std::cerr << "[RentalDimensionSystem::Ready()] Impossible "
                             "contents(upgrade) is not checked. ["
                          << (unsigned)info.m_field0 << "]" << std::endl;
                return 0;
            }
            if (item->GetAttachType() != 2)
            {
                std::cerr << "[RentalDimensionSystem::Ready()] AtttachType is "
                             "not trade delete. ["
                          << (unsigned)info.m_field0 << "]" << std::endl;
                return 0;
            }
            m_impl->m_set.insert((unsigned)info.m_field0);
        }
    }
    return 1;
