bool RentalMachinePcRoom::checkAllRentalItemScript()
{
    // ORIG 0x082708cc 段（p167 实证）：外层 map 的 begin()/end() 各自独立重调
    // G_CDataManager()（P157/P48）；内两层为 const_iterator 遍历
    // （begin/end/operator*/operator++/operator!=），非 size()/operator[] 下标形
    // （P175 反例）；`(*it).second` 代 `it->second`（P167 deEv 非 ptEv）。
    for (std::map<int,
                  std::vector<std::vector<EventRewardItemInfo> > >::iterator
             outer = G_CDataManager()->m_pcBangRentar.m_mapC.begin();
         outer != G_CDataManager()->m_pcBangRentar.m_mapC.end(); ++outer)
    {
        const std::vector<std::vector<EventRewardItemInfo> >& sections =
            (*outer).second;
        for (std::vector<std::vector<EventRewardItemInfo> >::const_iterator
                 sit = sections.begin();
             sit != sections.end(); ++sit)
        {
            const std::vector<EventRewardItemInfo>& items = *sit;
            for (std::vector<EventRewardItemInfo>::const_iterator
                     iit = items.begin();
                 iit != items.end(); ++iit)
            {
                // ORIG 0x082709b6 `call operator*; mov %eax,-0x14` ⇒ 原码是引用
                // 绑定 `const EventRewardItemInfo& info = *iit;`（-0x14 存对象
                // 地址），6 个使用点各自重解引用（SPAN O=8/U=7 实证）。
                const EventRewardItemInfo& info = *iit;
                CItem* item = G_CDataManager()->find_item((int)info.m_field0);
                if (item == 0)
                {
                    std::cerr << "[RentalSystem::Ready()] Can`t find item["
                              << (unsigned)info.m_field0 << "]" << std::endl;
                    return false;
                }
                if (item->IsImpossibleAmplify() != 1)     // vt+0x34 槽13
                {
                    std::cerr << "[RentalSystem::Ready()] Impossible contents("
                                 "amplify upgrade) is not checked. ["
                              << (unsigned)info.m_field0 << "]" << std::endl;
                    return false;
                }
                if (item->IsImpossibleCharacCargo() != 1)  // vt+0x40 槽16
                {
                    std::cerr << "[RentalSystem::Ready()] Impossible contents("
                                 "charac cargo) is not checked. ["
                              << (unsigned)info.m_field0 << "]" << std::endl;
                    return false;
                }
                if (item->IsImpossibleDisjoint() != 1)     // vt+0x38 槽14
                {
                    std::cerr << "[RentalSystem::Ready()] Impossible contents("
                                 "disjoint) is not checked. ["
                              << (unsigned)info.m_field0 << "]" << std::endl;
                    return false;
                }
                if (item->IsImpossibleGift() != 1)         // vt+0x3c 槽15
                {
                    std::cerr << "[RentalSystem::Ready()] Impossible contents("
                                 "gift) is not checked. ["
                              << (unsigned)info.m_field0 << "]" << std::endl;
                    return false;
                }
                if (item->IsImpossibleUpgrade() != 1)      // vt+0x30 槽12
                {
                    std::cerr << "[RentalSystem::Ready()] Impossible contents("
                                 "upgrade) is not checked. ["
                              << (unsigned)info.m_field0 << "]" << std::endl;
                    return false;
                }
                if (item->GetAttachType() != 2)
                {
                    std::cerr << "[RentalSystem::Ready()] AtttachType is not "
                                 "trade delete. ["
                              << (unsigned)info.m_field0 << "]" << std::endl;
                    return false;
                }
                m_impl->m_set.insert((unsigned)info.m_field0);
            }
        }
    }
    return true;
