// ============================================================================
// process —— ORIG 0x081d3d38（handler2：dispatch_template 校验后调用）
// ============================================================================
// [b28d-28] DIFF | ORIG 0x081d3d38 | P167 MISSING 3×deEv/3×ppEv + 3×pair ctor
// 变体 + getCurCharacInvenRefR（迭代器重载 + 取值路径缺口）+ N82 帧
// （0xcac0 vs 0xcaf0）；未闭合缺口。
// [P6S5 2026-09-11 本波处理]
//  ① **已落地（判据⑦ 接收者错）**：ORIG 0x081d409b `call 81803ca
//     <_ZNK15CUserCharacInfo21getCurCharacInvenRefREv>` 与 0x80da27e 的
//     `getCurCharacInvenREv` **不同地址（非同址别名）** ⇒ setCopy 源用 RefR 重载；
//     改后 p167fix 该 TU 的 `process` 段 **getCurCharacInven* 条目全 CLEAR**，
//     TU 四数 46/90/0/2/35 **不变**（零 codegen 影响）。
//  ② **负结论·回滚（codegen 轴）**：三条材料 push_back 若改 `std::make_pair(...)`
//     → 显式 `std::pair<int,int>(...)`（模板实参逐条对齐 ORIG：0x081d3fa3
//     `pair<unsigned&,int&>` / 0x081d3fde `pair<unsigned long&,int>`（`lea
//     -0x136(%ebp),%eax; add $0x2,%eax` + `movl $0x1,-0x70(%ebp)`）/ 0x081d4012
//     `pair<int&,int&>`），则 p167 的 3×pair-ctor UNRESOLVED + 2×make_pair EXTRA
//     虽消，但 TU 四数 ae 90→**87**（丢 3 个原先 AE 的 make_pair 实例符号）且
//     process 自身 gap −7→**+3**（ndiff 115→134）⇒ 按「四数不得劣化」回滚。
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
    // ORIG 0x081d3e32：slotA 先入 %ebx、space **无具名槽**（P329 实参不提炼局部）
    //   `mov -0x84(%ebp),%ebx; movl $0x0,(%esp); call GetInvenTypeFromItemSpace;
    //    mov %ebx,0x8(%esp); mov %eax,0x4(%esp)`。
    if (user->CheckItemLock(GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)0), slotA))
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
        // [P6S5 · 判据⑦ 接收者] ORIG 0x081d409b `call 81803ca
        // <_ZNK15CUserCharacInfo21getCurCharacInvenRefREv>`（**与 0x80da27e 的
        // getCurCharacInvenR 非别名、不同地址**）⇒ 本处（setCopy 源）原码用 RefR 重载。
        tempInv.setCopy(*user->getCurCharacInvenRefR());
        if (tempInv.tryDeleteEventItems(materials) == 0)
        {
            st.m_4 = 0x11;
            return 0;
        }
    }

    // ---- 源信息复制（source 非 stackable 时）----
    // ORIG 0x081d40e2 四槽清零（-0x31/-0x30/-0x2c/-0x28）+ 0x081d410d 分支逐字段取：
    //   +0x00(1B) / GetReSealCount() / +0x0d(int) / get_add_info() 共 4 项——**无**
    //   Inven_Item 局部、**无** out=invA 整体拷贝（旧形整体拷贝多 16 条 mov）。
    bool outField0 = false;        // → resultItem+0x00（ORIG movb $0x0,-0x31）
    int resealCount = 0;           // ORIG movl $0x0,-0x30（GetReSealCount 零扩展入 int 槽）
    int outFieldD = 0;             // → resultItem+0x0d（ORIG movl $0x0,-0x2c）
    int copyAddInfo = 0;           // ORIG movl $0x0,-0x28
    if (!source->is_stackable())
    {
        outField0 = invA.m_field0;             // ORIG 0x081d410d movzbl -0xf9
        resealCount = invA.GetReSealCount();   // ORIG 0x081d4125 movzbl %al（uchar→int 零扩展）
        outFieldD = invA.m_fieldd;             // ORIG 0x081d412b mov -0xec(%ebp)（invA+0x0d）
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
    // ORIG 0x081d4290 `mov %ax,-0x22(%ebp)` / 0x081d4294 `cmpw $0x0,-0x22(%ebp)`
    //   / 0x081d42b2+0x081d4309 `movswl -0x22(...)` ⇒ 槽宽 16 位 = short（P9/P211 类型轴）。
    short mat2Slot = (short)user->getCurCharacInvenR()->check_item_exist(mat2Idx);
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
    // ORIG 0x081d433d-0x081d4443：**迭代器**循环（begin/end/operator!=/operator*
    //   /operator++ 调用全可见，it 槽 -0xbc、end 临时 -0x4c/-0x50）+ 计数上限
    //   10（0x081d442f `cmpl $0x9,-0x20(%ebp)` / 0x081d4433 `jg`）——上限是
    //   **语义缺口**（旧形索引循环 `i+2 < size()` 无上限）。循环体每组读 3 个
    //   int（-0x14/-0x10/-0xc），只把前两个传给 SendUpdateItem(1, space, slot)。
    std::vector<int>::const_iterator it = updateSlots.begin();
    int sentCount = 0;
    while (it != updateSlots.end() && sentCount < 10)
    {
        int space = *it;      // ORIG 0x081d4376 operator* → -0x14
        ++it;
        int slot = *it;       // ORIG 0x081d4397 operator* → -0x10
        ++it;
        int restCount = *it;  // ORIG 0x081d43b8 operator* → -0xc（读出后未消费）
        ++it;
        (void)restCount;
        user->SendUpdateItem(CUser::eSendTarget_1, (ENUM_ITEMSPACE)space, slot);
        ++sentCount;          // ORIG 0x081d43fa addl $0x1,-0x20
    }

    // ---- 结果物品生成 ----
    Inven_Item resultItem;                 // ctor 内已 reset（ORIG 0x081d4452 只有 1 次 call）
    resultItem.m_addInfo = limitPair.first;    // ORIG 0x081d4457 直写 +0x02，其后重读（P48）
    CItem* resultDef = G_CDataManager()->find_item(resultItem.m_addInfo);  // 结果对象索引
    if (resultDef == 0)
    {
        st.m_4 = 0x11;
        return 0;
    }
    resultDef->make_item(resultItem);
    if (resultDef->is_stackable())
    {
        resultItem.m_addInfo2 = limitPair.second;  // +0x07 数量（ORIG 0x081d44c1 直写 -0x16c）
    }
    else
    {
        resultItem.m_field0 = outField0;                    // +0x00
        resultItem.SetReSealCount((unsigned char)resealCount);
        resultItem.m_fieldd = outFieldD;                    // +0x0d
        resultItem.set_add_info(copyAddInfo);
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
}