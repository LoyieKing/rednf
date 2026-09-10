int CUser::process_recipe_avatar_items(int mainSlot, bool param2,
                                       unsigned short count, int cmdHeader,
                                       CStackableItem const* recipe)
{
    // 0x0868c4f4+0x01f: recipe == NULL -> 0x11
    if (recipe == NULL) {
        return 0x11;
    }
    // +0x023: param2 != 0 -> 0x13
    if (param2) {
        return 0x13;
    }

    // +0x042: 拷贝 recipe+0x174 -> materials（CStackableItem::m_recipe174.m_vec0）
    // +0x06e: 拷贝 recipe+0x180 -> outputs（m_recipe174.m_vecC）
    std::vector<std::pair<int, int> > materials(recipe->m_recipe174.m_vec0);
    std::vector<std::pair<int, int> > outputs(recipe->m_recipe174.m_vecC);
    // +0x076: outputs.size() > 1 -> 0x11
    if (outputs.size() > 1) {
        return 0x11;
    }

    // +0x0a1: vector<int>（槽位更新三元组，元素按 [space, slot, count] 排布）
    std::vector<int> updatedSlots;
    // +0x0af: 显式 clear（ORIG 空构造后即 clear，忠实保留）
    updatedSlots.clear();

    // +0x0bd..+0x105: 栈上副本库存
    //   CInventory::CInventory() 0x0822d52c
    //   InventoryMemory::InventoryMemory() 0x0822d3b8
    //   CInventory::SetInventoryMemory 0x0822d4f6
    //   CUserCharacInfo::getCurCharacInvenRefR 0x081803ca
    //   CInventory::setCopy 0x084fa314
    CInventory inv;
    InventoryMemory invMem;
    inv.SetInventoryMemory(&invMem);
    // getCurCharacInvenRefR 的头签名是 const Inven_Item*，但 ORIG 将返回值按
    // const CInventory& 传给 setCopy（同一 m_inven 子对象，两种视图同址）。
    inv.setCopy(*(const CInventory*)getCurCharacInvenRefR());

    // [DualCUOO 2026-09-09 双判据 gap] ORIG 0x0868c5f9 `movzbl -0xca7c(%ebp),%eax`
    // （param2 溢出槽）+ `xor $1; test; je 0868c6b7` ⇒ 源码此处另有 `if (!param2)`
    // 包住 delete_item 与三个 push_back（param2 到此处恒 0，行为等价）。
    if (!param2) {
    // +0x14a: inv.delete_item(1 /*INVEN_TYPE_EQUIP*/, mainSlot, count, 8, false)
    // +0x15d: 失败 -> 0x11（eItemDelReason 枚举缺成员 8，按 ORIG 立即数强转）
    if (!inv.delete_item(INVEN_TYPE_EQUIP, mainSlot,
                         (int)(unsigned int)count, (eItemDelReason)8, false)) {
        return 0x11;
    }
    // +0x167..+0x1b5: push_back(0 /*equip space*/, mainSlot, count)
    updatedSlots.push_back(0);
    updatedSlots.push_back(mainSlot);
    updatedSlots.push_back((int)(unsigned int)count);
    }

    // +0x1cf..+0x439: 材料循环（materials.size() 次）
    {
        int matIdx = 0;
        while ((int)materials.size() > matIdx) {
            // [b28d-PoolCUOO2 修法 2026-09-09 P78] ORIG 无引用缓存、**每读点重调**
            // `vector<pair>::operator[]`（ixEj 6/6 vs 旧 2/6）⇒ 删 const& mat，逐点索引。
            // +0x1ed: G_CDataManager() 0x080cc19b
            // +0x1f9: CDataManager::find_item(int) 0x0835fa32
            CDataManager* dataMgr = G_CDataManager();
            CItem* item = dataMgr->find_item(materials[matIdx].first);

            if (item != NULL) {
                // +0x210: 虚调用 *(vtable+0x10) -> CItem::IsAvatarItem()
                if (!item->IsAvatarItem()) {
                    // 普通材料：在副本库存中删除事件物品
                    // +0x319: inv.get_inven_slot_no(mat.first) 0x0850cd62
                    int slot = inv.get_inven_slot_no(materials[matIdx].first);
                    if (slot == -1) {
                        return 0xd5;
                    }
                    // +0x381: inv.tryDeleteEventItems(one) 0x085058da
                    std::vector<std::pair<int, int> > one;
                    one.push_back(materials[matIdx]);
                    if (!inv.tryDeleteEventItems(one)) {
                        // +0x394: 失败 -> 0x11（就地退出，不做产出）
                        return 0x11;
                    }
                    // +0x3a3..+0x3ef: push_back(0, slot, mat.second)
                    updatedSlots.push_back(0);
                    updatedSlots.push_back(slot);
                    updatedSlots.push_back(materials[matIdx].second);
                } else {
                    // avatar 材料
                    // +0x249: inv.get_avatar_slot_no(mat.first) 0x0850cd1c
                    int slot = inv.get_avatar_slot_no(materials[matIdx].first);
                    if (slot == -1) {
                        return 0xd5;
                    }
                    // +0x281: CheckItemLock(2 /*INVEN_TYPE_AVATAR*/, slot)
                    if (CheckItemLock(INVEN_TYPE_AVATAR, slot)) {
                        return 0xd5;
                    }
                    // +0x2a8..+0x2f5: push_back(1 /*avatar space*/, slot, 1)
                    updatedSlots.push_back(1);
                    updatedSlots.push_back(slot);
                    updatedSlots.push_back(1);
                }
            }
            ++matIdx;
        }
    }

    // +0x471..+0x489: 取产出物品（注意：ORIG 未判 outputs.size() 是否为 0，
    // 直接读 outputs[0]；size>1 已被拒，size==0 属 ORIG 原始 UB，忠实转写）
    CDataManager* dataMgr = G_CDataManager();
    CItem* outItem = dataMgr->find_item(outputs[0].first);
    // +0x489: outItem == NULL -> 0x11
    if (outItem == NULL) {
        return 0x11;
    }

    // +0x4a2: PacketGuard::PacketGuard() 0x0858dd4c
    PacketGuard guard;
    // +0x4ad: getCurCharacInvenW() 0x080da28e
    CInventory* invW = getCurCharacInvenW();

    // +0x4cd..+0x6eb: 按三元组在真实库存中删除
    // [b28d-PoolCUOO2 修法 2026-09-09] ORIG 本循环=**非 const** iterator（deEv IPi 3 +
    // ppEi IPi 3 + ne<IPi> 同型 1）⇒ `*it++` 后置形（临时结果被 deref 消费，N113
    // 折叠不适用），比较两侧同为非 const。
    {
        std::vector<int>::iterator it = updatedSlots.begin();
        while (it != updatedSlots.end()) {
            int space = *it++;
            int slot = *it++;
            int delCount = *it++;

            if (space == 0) {
                // +0x578: getCurCharacInvenW() 0x080da28e（ORIG 分支内重取）
                CInventory* invW2 = getCurCharacInvenW();
                // +0x5ac: delete_item(1 /*EQUIP*/, slot, delCount, 8, true)
                if (!invW2->delete_item(INVEN_TYPE_EQUIP, slot, delCount,
                                        (eItemDelReason)8, true)) {
                    // +0x5c9: trace（ORIG 字符串原样）
                    // [FL5 2026-09-10 负结论·已回滚] P37/P227 全表达式链在本件
                    // ndiff 1008→1012 反劣（ours 742→743）⇒ 维持两语句具名形；勿再试。
                    cMyTrace trace(
                        "int CUser::process_recipe_avatar_items(int, bool, "
                        "RequestItemCountType, int, const CStackableItem*)",
                        0xa8c1, 5);
                    trace("CUser::process_recipe_avatar_items, "
                          "getCurCharacInvenW()->delete_item failed , %d %d",
                          getCurCharacNo(), slot);
                    return 0x11;
                }
            } else { // space == 1（ORIG 仅判 ==1 / !=0 两路，此为 !=0 路）
                // +0x62b: getCurCharacInvenR() 0x080da27e
                // +0x64c: GetInvenSlot(2, slot) 0x084fb918 —— 读出当前 avatar 物品
                const CInventory* invR = getCurCharacInvenR();
                Inven_Item curItem = invR->GetInvenSlot(INVEN_TYPE_AVATAR, slot);
                // +0x681: invW->delete_item(2, slot, 1, 3, true)
                if (!invW->delete_item(INVEN_TYPE_AVATAR, slot, 1,
                                       eItemDelReason_3, true)) {
                    return 0x16;
                }
                // +0x6a3: mgr->UnRegistItem(curItem.m_addInfo2) 0x082f9402
                //   （Ghidra 反编译漏掉该 int 实参，汇编 0x6a8 以 -0x12e=Inven_Item+0x07 传入）
                WongWork::CAvatarItemMgr* mgr = invW->GetAvatarItemMgrW();
                mgr->UnRegistItem(curItem.m_addInfo2);
                // +0x6e6: invW->SendAvatarEvent(0x36, addInfo2, 1, slot+10, 0)
                invW->SendAvatarEvent(0x36, curItem.m_addInfo2, 1,
                                      slot + 10, 0);
            }
        }
    }

    // +0x720..+0x76b: 生成 IPG 编号（缓冲 0x18 字节清零）
    char ipgBuf[0x18];
    for (int k = 0; k < 0x18; ++k) {
        ipgBuf[k] = 0;
    }
    // +0x766: CGenUniqueNo::genIPGNo(7, getCurCharacNo(), ipgBuf) 0x084b7750
    //   （ORIG 第 1 参立即数 7，枚举现有成员只到 6，按 ORIG 强转）
    char* ipgNo = WongWork::CGenUniqueNo::genIPGNo(
        (WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE)7,
        (unsigned int)getCurCharacNo(), ipgBuf);

    // +0x773: CItem::get_index(outItem) 0x08110c48
    int outIdx = outItem->get_index();
    // +0x78d: CItem::getUsablePeriod(outItem) 0x08110c60
    int usablePeriod = outItem->getUsablePeriod();

    // +0x7d1: invW->AddAvatarItem(outIdx, usablePeriod, false, 0, -1,
    //                            ipgNo, 2, false, 0) 0x08509b9e
    //   （eAvatarItemAddReason 缺成员 2，按 ORIG 立即数强转）
    // [b28d-PoolCUOO2 修法] ORIG 0x0868cc81（+0x78d）此点**重调** getCurCharacInvenW()
    // （全函数 InvenW×3：init 0x868c9a1 / space0 分支 0x868ca72 / 本点；循环 else 支
    // 三处用走 init 槽无 call）⇒ 不用 invW 缓存。
    int newSlot = getCurCharacInvenW()->AddAvatarItem(
        outIdx, usablePeriod, false, 0, (char)-1, ipgNo,
        (eAvatarItemAddReason)2, false, 0);
    // +0x7e9: newSlot == -1 -> 0x4
    if (newSlot == -1) {
        return 0x4;
    }

    // +0x810: getCurCharacInvenR() + GetInvenSlot(2, newSlot) —— 读回新 avatar
    Inven_Item newItem =
        getCurCharacInvenR()->GetInvenSlot(INVEN_TYPE_AVATAR, newSlot);

    // +0x835..+0x84b: put_byte(1); put_byte((int)updatedSlots.size() / 3)
    //   （ORIG 用 0xaaaaaaab 魔数除 3）
    guard.put_header(1, cmdHeader);     // +0x9fa 区段前：put_header(1, param_4)
    guard.put_byte(1);
    guard.put_byte((int)(updatedSlots.size() / 3));

    // +0x9d4..+0x99e: 三元组逐个 put_byte(space)/put_short(slot)/put_int(count)
    // [b28d-PoolCUOO2 修法 P326/P167] ORIG 本循环=const_iterator（deEv IPKi 3 +
    // ppEi IPKi 3 + **同型** ne<IPKi> 1 + 转换 ctor 2）⇒ init 一处转换 + 比较点
    // `const_iterator(updatedSlots.end())` 第二处转换（end 侧不缓存具名）。
    {
        std::vector<int>::const_iterator it = updatedSlots.begin();
        while (it != std::vector<int>::const_iterator(updatedSlots.end())) {
            int space = *it++;
            int slot = *it++;
            int cnt = *it++;
            guard.put_byte(space);
            guard.put_short(slot);
            guard.put_int(cnt);
        }
    }

    // +0x9f7..+0xa2f: 产出物品字段
    guard.put_byte(1);
    guard.put_byte(1);
    guard.put_short(newSlot);
    guard.put_int(newItem.m_addInfo);    // Inven_Item+0x02（item idx）
    guard.put_int(newItem.m_addInfo2);   // Inven_Item+0x07
    // +0xa2f: Inven_Item::GetUpgrade() 0x080f506c -> put_byte
    guard.put_byte((int)newItem.GetUpgrade() & 0xff);
    // +0xa5c: put_short(newItem.m_fieldb)（Inven_Item+0x0b endurance）
    guard.put_short((int)newItem.m_fieldb);
    // +0xa6e: stAmplifyOption_t::getAbilityType() 0x08150732 -> put_byte
    guard.put_byte((int)newItem.m_amp.getAbilityType() & 0xff);
    // +0xa94: stAmplifyOption_t::getAbilityValue() 0x08150772 -> put_short
    guard.put_short((int)newItem.m_amp.getAbilityValue() & 0xffff);
    // +0xaba: put_byte(newItem.m_field0 != 0 ? 1 : 0)
    guard.put_byte(newItem.m_field0 != 0 ? 1 : 0);
    // +0xaea: put_packet(newItem) 0x0815098e
    guard.put_packet(newItem);
    // +0xb13: finalize(true) 0x080cb958
    guard.finalize(true);

    // +0xb28: Send(guard) 0x086485ba
    Send(guard);
    // +0xb4a: SendUpdateItemList(eSendTarget_1, (ENUM_ITEMSPACE)1, newSlot)
    //   0x0867c65a（ENUM_ITEMSPACE 枚举缺成员 1，按 ORIG 强转）
    SendUpdateItemList(eSendTarget_1, (ENUM_ITEMSPACE)1, newSlot);

    // +0xb4f: HistoryLog::WriteComposition(*(FILE**)(this+0x796f8),
    //                                      recipe->get_index()) 0x084ba174
    //   （m_field796f8 为 int 存文件句柄，与 ORIG *(int*)(this+0x796f8) 一致）
    HistoryLog::WriteComposition((FILE*)(unsigned)m_field796f8,
                                 recipe->get_index());

    // +0xb6f: return 0
    return 0;
