// ==== Dispatcher_PcRoomRentItem::giveDimensionItems (ORIG 0x081e583e) ====
// 注：ORIG 为类成员（见文件头宿主声明补充说明）。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081e583e orig/ours=78/74 ndiff-blocks=14
//   帧差 sub $3c vs $38 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %edi | push   %esi | push   %ebx
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
    // ORIG 0x081e5892(W 0x08110c94)/0x081e58a0(W 0x080fdf20)/0x081e58aa：growType→%edi、
    // job→%ebx 跨 receiver call 保活（callee-saved，**无**具名槽）；ORIG 0x081e58c3 调
    // rewardData->getRewardGiveItems(selectionIdx, job, growType)（P183 全表达式内联），
    // 非空则逐项组装 RentInElem 入 input.m_elems（m_type=1 GiveList 标记）
    std::vector<EventRewardItemInfo>* items =
        (&G_CDataManager()->m_dimensionAct.m_rewardData)
            ->getRewardGiveItems(selectionIdx, user->get_charac_job(),
                                 static_cast<int>(user->getCurCharFirstGrowType()));
    // [G6-giveDim] ORIG 0x081e5920 循环退出点 `jmp 0x081e592c`（epilogue）为独立尾岛
    //   ⇒ 循环后须存在可达标签（否则 4.4 直接落入 epilogue 少 1 条 jmp）。
    if (items == 0)
    {
        goto done;
    }
    for (int i = 0; i < static_cast<int>(items->size()); ++i)
    {
        InstanceRentalSystem::RentInput::RentInElem elem;
        elem.m_sectionIdx = static_cast<unsigned char>(selectionIdx);
        elem.m_itemIdx = static_cast<unsigned char>(i);
        elem.m_type = 1;                             // GiveList 路径标记
        input.m_elems.push_back(elem);
    }
done:
    return;
}