int Dispatcher_UseLimitCube::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    if (user->get_state() != 3)
    {
        return 0;
    }

    stUseLimitCube& st = *reinterpret_cast<stUseLimitCube*>(&param);
    MSG_USE_LIMIT_CUBE& m = *reinterpret_cast<MSG_USE_LIMIT_CUBE*>(&msg);

    // ORIG movzwl+cwtl（process.md 0x39/0x53）＝ short 读后符号扩展入 int。
    int slotA = m.m_slotA;
    unsigned int itemIdx = m.m_itemIdx;
    int slotB = m.m_slotB;

    // ---- 槽物品读取（INVEN_TYPE_EQUIP=1）----
    Inven_Item invA = user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)1, slotA);
    Inven_Item invB = user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)1, slotB);

    // cube = find_item(invB.m_addInfo)，source = find_item(invA.m_addInfo)
    CItem* cube = G_CDataManager()->find_item(invB.m_addInfo);
    CItem* source = G_CDataManager()->find_item(invA.m_addInfo);

    // ---- 物品锁检查 ----
    int lockSpace = GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)0);
    if (user->CheckItemLock(lockSpace, slotA))
    {
        st.m_4 = 0x1;
        return 0;
    }

    // ---- 校验 ----
    int err = check_error(slotA, itemIdx, slotB, invA, invB, cube);
    if (err != 0)
    {
        st.m_4 = (unsigned char)err;
        return 0;
    }

    // ---- 结果物品（pair<int,int>）----
    std::pair<int, int> limitPair;
    ((CStackableItem*)cube)->get_limit_cube_item(limitPair);
    if (limitPair.first == 0 && limitPair.second == 0)
    {
        st.m_4 = 0x11;
        return 0;
    }
    st.m_8 = limitPair.first;
    st.m_c = limitPair.second;

    // ---- 材料计数 ----
    int count1 = ((CStackableItem*)cube)->get_1st_material_count(itemIdx);
    int mat2Idx = ((CStackableItem*)cube)->get_2nd_material_index();
    int mat2Count = ((CStackableItem*)cube)->get_2nd_material_count();

    // ---- 材料表 ----
    std::vector<std::pair<int, int> > materials;
    materials.push_back(std::make_pair((int)itemIdx, count1));
    materials.push_back(std::make_pair(invB.m_addInfo, 1));
    materials.push_back(std::make_pair(mat2Idx, mat2Count));

    if (user->getCurCharacInvenR()->CheckNeedItemList(materials) == 0)
    {
        st.m_4 = 0x11;
        return 0;
    }

    // ---- 临时库存模拟删除（tryDeleteEventItems）----
    {
        CInventory tempInv;
        InventoryMemory tempMem;
        tempInv.SetInventoryMemory(&tempMem);
        tempInv.setCopy(*user->getCurCharacInvenR());
        if (tempInv.tryDeleteEventItems(materials) == 0)
        {
            st.m_4 = 0x11;
            return 0;
        }
    }

    // ---- 源信息复制（source 非 stackable 时）----
    // ORIG：local_35 = invA（整体 Inven_Item 拷贝，含 m_amp/m_attr/m_addInfo2/m_fieldd）；
    //        随后仅 SetReSealCount(invA.GetReSealCount()&0xff) + set_add_info(invA.get_add_info())。
    Inven_Item out;
    out.reset();
    unsigned char resealCount = 0;
    int copyAddInfo = 0;
    if (!source->is_stackable())
    {
        out = invA;
        resealCount = invA.GetReSealCount();
        copyAddInfo = invA.get_add_info();
    }

    // ---- 三次删除 + 更新元组 ----
    std::vector<int> updateSlots;  // 每 3 个 int 一组 {0, slot, count}

    // ① source（材料）
    if (user->getCurCharacInvenW()->delete_item((INVEN_TYPE)1, slotA, count1,
                                                (eItemDelReason)8, true) != 0)
    {
        updateSlots.push_back(0);
        updateSlots.push_back(slotA);
        updateSlots.push_back(count1);
    }
    // ② cube（立方体）
    if (user->getCurCharacInvenW()->delete_item((INVEN_TYPE)1, slotB, 1,
                                                (eItemDelReason)8, true) != 0)
    {
        updateSlots.push_back(0);
        updateSlots.push_back(slotB);
        updateSlots.push_back(1);
    }
    // ③ 第二材料
    int mat2Slot = user->getCurCharacInvenR()->check_item_exist(mat2Idx);
    if (mat2Slot < 0)
    {
        st.m_4 = 0x11;
        return 0;
    }
    if (user->getCurCharacInvenW()->delete_item((INVEN_TYPE)1, mat2Slot, mat2Count,
                                                (eItemDelReason)8, true) != 0)
    {
        updateSlots.push_back(0);
        updateSlots.push_back(mat2Slot);
        updateSlots.push_back(mat2Count);
    }

    // ---- 对已删除槽逐组 SendUpdateItem ----
    // [轴B 回滚·N] 表内行 jg/setb(81d4433) 为**假阳性**：ORIG 该点是
    // `cmpl $0x9,-0x20(%ebp); jg`（迭代器循环的计数上限），与我方此处的
    // `size_t i + 2 < updateSlots.size()` 循环尾测**上下文不同构**。
    // 试改 int 后符号性虽对齐（setb→setl），但 matched 144→144、
    // gap −96 不变、TU 四数 131/0/3/34 不变 ⇒ 无收益，按判据回滚。
    for (size_t i = 0; i + 2 < updateSlots.size(); i += 3)
    {
        int slot = updateSlots[i + 1];
        user->SendUpdateItem(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0, slot);
    }

    // ---- 结果物品生成 ----
    Inven_Item resultItem;
    resultItem.reset();
    CItem* resultDef = G_CDataManager()->find_item(limitPair.first);  // 结果对象索引
    if (resultDef == 0)
    {
        st.m_4 = 0x11;
        return 0;
    }
    resultDef->make_item(resultItem);
    if (!resultDef->is_stackable())
    {
        resultItem = out;
        resultItem.SetReSealCount(resealCount);
        resultItem.set_add_info(copyAddInfo);
    }
    else
    {
        resultItem.m_fieldd = limitPair.second;
    }

    // ---- 插入背包 ----
    int newSlot = user->getCurCharacInvenW()->insertItemIntoInventory(
        resultItem, (eItemAddReason)9, true, true);
    if (newSlot < 0)
    {
        st.m_4 = 0x11;
        return 0;
    }
    st.m_4 = 0x0;

    // ---- wrong-result 日志 ----
    int cubeIdx = invB.m_addInfo;
    int resultIdx = limitPair.first;
    if (cubeIdx == 0x1f06)
    {
        if (resultIdx <= 0x671d || resultIdx > 0x6749)
        {
            LogManager::logFormat(1, "PacketDispatcher_Impl_1.cpp",
                                  __PRETTY_FUNCTION__,
                                  0x344b,
                                  "LIMIT CUBE : WRONG RESULT - cube_item(%u), result_item(%u)",
                                  (unsigned int)cubeIdx, (unsigned int)resultIdx);
        }
    }
    else if (cubeIdx == 0x1f11)
    {
        if (resultIdx <= 0x6749 || resultIdx > 0x6775)
        {
            LogManager::logFormat(1, "PacketDispatcher_Impl_1.cpp",
                                  __PRETTY_FUNCTION__,
                                  0x3452,
                                  "LIMIT CUBE : WRONG RESULT - cube_item(%u), result_item(%u)",
                                  (unsigned int)cubeIdx, (unsigned int)resultIdx);
        }
    }

    // ---- 更新结果槽 ----
    user->SendUpdateItem(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0, newSlot);

    return 0;
