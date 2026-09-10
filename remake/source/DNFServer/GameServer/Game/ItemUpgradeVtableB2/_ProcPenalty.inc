int CItemUpgrade::_ProcPenalty(CUser* user, Inven_Item* item,
                               const CItem* itemScript, unsigned short slot,
                               const upgrade_table_t& table,
                               const upgrade_info_t* info, PacketGuard* pg,
                               Inven_Item* ticketCopy, char ticketFlag)
{
    // [b28d-solo-ItemUpgradeVtableB2] P222/P194：ORIG 0x08546bdb 先测
    // `ticketCopy->m_addInfo != 0`（正极性）再进票分支、`==0` 走 else
    // （0x08546bfd 表查询）；负极性写法令两臂布局互换。票分支内
    // `cmpb ticketFlag; jne` ⇒ if/else 独立赋值（非三目）。
    int penaltyType;
    if (ticketCopy->m_addInfo != 0)
    {
        if (ticketFlag == 0)
        {
            penaltyType = 1;
        }
        else
        {
            penaltyType = 2;
        }
    }
    else
    {
        penaltyType = table.GetPenaltyType(item->GetUpgrade(),
                                           (ENUM_RARITY)itemScript->get_rarity(),
                                           info);
        if (GlobalData::s_pEvent_UpGrade_Jar->isFortune())
        {
            penaltyType = 2;
        }
    }
    pg->put_byte((int)(char)penaltyType);

    if (penaltyType == 1)
    {
        pg->put_byte((int)item->GetUpgrade());
        pg->put_short((int)slot);
        return 0;
    }

    if (penaltyType == 2)
    {
        if (GlobalData::s_pEvent_UpGrade_Jar->isFortune())
        {
            // [2026-09-10 SweepCEquipItem2] P0：ORIG 0x08546cbb..0x08546cc7 为
            // `mov info,%eax; add $0x28,%eax; mov %eax,-0x3c(%ebp); mov -0x3c,%eax;
            // movl $0x1,(%eax)` ⇒ 原码有具名指针局部（槽 -0x3c）；直接写
            // const_cast 链会被 4.4 折成单条 `movl $0x1,0x28(%eax)`（少 3 条）。
            upgrade_info_t* probSlot = const_cast<upgrade_info_t*>(info);
            probSlot->m_prob20[2] = 1;  // info +0x28
        }
        // [b28d-solo-ItemUpgradeVtableB2] P167/P48：ORIG 0x08546cd9 比较用
        // GetUpgrade（值留 %eax 无槽）、0x08546d22/0x08546d4a 两个 SetUpgrade
        // 分支**各自重新调用** GetUpgrade，且 else 支重读 info->m_prob20[2]
        // ⇒ drop/up 均无具名局部，全部内联表达式（此前缓存 up/drop 少 2 个
        // GetUpgrade 调用点，p167 MISSING -2）。
        // ORIG 0x08546ce1..0x08546cf5：`cmp; jge L1; cmpb ticketFlag;
        // jns L2; L1: mov $1,%eax; jmp; L2: mov $0,%eax; test %al` ⇒ 源码是
        // `A || B` 短路表达式直接初始化 bool（4.1 生 jge/jns 双跳 + mov 岛）。
        bool toZero = (info->m_prob20[2] >= (int)item->GetUpgrade()) ||
                      (ticketFlag < 0);
        if (toZero)
        {
            item->SetUpgrade(0);
        }
        else if (ticketFlag > 0)
        {
            item->SetUpgrade(
                (unsigned char)((int)item->GetUpgrade() - (int)ticketFlag));
        }
        else
        {
            item->SetUpgrade((unsigned char)((int)item->GetUpgrade() -
                                             info->m_prob20[2]));
        }
        pg->put_byte((int)item->GetUpgrade());
        pg->put_short((int)slot);
        return 0;
    }

    // ---- penaltyType == 3：破坏 ----
    // [2026-09-10 SweepCEquipItem2] 真 bug 修（缺条件守卫）：ORIG 0x08546da1
    // `cmpl $0x3,-0x40(%ebp); jne 0854754b`（直接返回 0），Ghidra C 同证
    // `else if (local_44 == 3) { …整个破坏流程… }`。旧译文在
    // penaltyType==2 分支 return 后**无条件**执行破坏流程 ⇒
    // penaltyType∉{1,2,3}（如 GetPenaltyType 返回 0）时仍会 delete_item/返料。
    if (penaltyType == 3)
    {
        if (!item->m_amp.hasAbility())
        {
            // 保护券链：0x28be7e → 0x1f72 → 0x20ba，末位 0x292091ac 优先覆盖。
            // [2026-09-10 SweepCEquipItem2] 三目嵌套形改 ORIG 形（P198① 预清
            // 两句式 + if/else-if 链）：ORIG 0x08546dc4/0x08546dcb/0x08546dd2 三条
            // `movl $0xffffffff` 预置 protectSlot/slotA/slotB，0x08546e15 预置 slotC，
            // 0x08546e62 预置 slotD；选择段为 0x08546e3a..0x08546e62 的 je/赋值/jmp
            // 链（Ghidra C 同形：`if (local_30==-1){ if(local_2c==-1){…} else … } else …`）。
            int protectSlot = -1;
            int slotA = -1;
            int slotB = -1;
            slotA = ((const CUserCharacInfo*)user)
                        ->getCurCharacInvenR()
                        ->check_item_exist(0x28be7e);
            slotB = ((const CUserCharacInfo*)user)
                        ->getCurCharacInvenR()
                        ->check_item_exist(0x1f72);
            int slotC = -1;
            slotC = ((const CUserCharacInfo*)user)
                        ->getCurCharacInvenR()
                        ->check_item_exist(0x20ba);
            if (slotA == -1)
            {
                if (slotB == -1)
                {
                    if (slotC != -1)
                    {
                        protectSlot = slotC;
                    }
                }
                else
                {
                    protectSlot = slotB;
                }
            }
            else
            {
                protectSlot = slotA;
            }
            int slotD = -1;
            slotD = ((const CUserCharacInfo*)user)
                        ->getCurCharacInvenR()
                        ->check_item_exist(0x292091ac);
            if (slotD != -1)
            {
                protectSlot = slotD;
            }
            if (protectSlot != -1)
            {
                // [b28d-solo-ItemUpgradeVtableB2] P167/P183：ORIG 0x08546ea3
                // （GetInvenSlot）与 0x08546f10（delete_item）各自重调
                // getCurCharacInvenW，无具名 invenW 缓存（缓存少 1 个调用点）。
                Inven_Item used = ((CUserCharacInfo*)user)
                                      ->getCurCharacInvenW()
                                      ->GetInvenSlot(INVEN_TYPE_EQUIP, protectSlot);
                if (used.m_addInfo != 0x28be7e && used.m_addInfo != 0x1f72 &&
                    used.m_addInfo != 0x20ba && used.m_addInfo != 0x292091ac)
                {
                    return 0x11;
                }
                if (((CUserCharacInfo*)user)
                        ->getCurCharacInvenW()
                        ->delete_item(INVEN_TYPE_EQUIP, protectSlot, 1,
                                      eItemDelReason_3, true) != 1)
                {
                    return 0x11;
                }
                user->m_historyLog.apply_Item(used.m_addInfo, item->m_addInfo,
                                              eApplyItemReason_0);
                user->SendUpdateItem((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0,
                                     protectSlot);
                item->SetUpgrade(0);
                ((CUserCharacInfo*)user)->getCurCharacInvenW()->update_item(
                    INVEN_TYPE_EQUIP, slot, *item);
                user->SendUpdateItem((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0,
                                     (int)slot);
                return 0xd8;
            }
        }

        CInventory* invenW = ((CUserCharacInfo*)user)->getCurCharacInvenW();
        if (invenW->delete_item(INVEN_TYPE_EQUIP, slot, 1, eItemDelReason_0xd,
                                true) != 1)
        {
            return 0x11;
        }
        // [2026-09-10 SweepCEquipItem2] T1 直证：ORIG 取 GetUpgrade 后
        // `movzbl %al,%eax; mov %eax,-0x38`（32 位槽 + 零扩展）⇒ 原码是
        // 32 位局部再掩码（uchar 局部恒 `mov %al,-N`，槽宽 1 且少 1 条）。
        const unsigned int savedUp = static_cast<unsigned int>(
            static_cast<unsigned char>(item->GetUpgrade()));
        item->SetUpgrade(0);
        pg->put_byte(0);
        pg->put_short((int)slot);

        DisJointResult result;
        if (G_CDataManager()->m_disJoint->GetResult(item->m_addInfo, result) != 0)
        {
            pg->put_byte(0);
            return 0;
        }
        item->m_addInfo = 0;

        std::vector<std::pair<int, int> > items;
        items.reserve(6);
        for (int i = 0; i <= 4; ++i)
        {
        // [2026-09-10 SweepCEquipItem2] 真 bug 修（数组下标差 1）：ORIG 0x085471ab
        // `mov -0x1c(%ebp),%eax; add $0x4,%eax; mov -0xa0(%ebp,%eax,4),%eax`
        // （result 基址 -0xa4）⇒ 条件读 **m_f[i+5]**（= m_b[i]），与下方
        // make_pair 第 1 参 `lea 0x4(%eax)`（0x085471dc，同样 &m_f[i+5]）自洽；
        // 旧译文写 m_f[i+4] 既漏掉末元素 m_f[9]，又误纳入 m_f[4]。
        if (result.m_f[i + 5] != 0)
        {
                items.push_back(std::make_pair(result.m_f[i + 5], result.m_f[i]));
        }
            }
        _getAdditionalDisjointItem(savedUp, itemScript, items, table);
        if (item->m_amp.hasAbility())
        {
            // 表区子对象走**真实成员调用**（0x0854b382
            // CItemUpgradeTable::getUpgradeInfoTable）；inline 视图代返会丢该 call
            // （P167 差集实证 missing 1）。
            _getAdditionalDisjointItem(savedUp, itemScript, items,
                                       *getUpgradeInfoTable());
        }

        std::vector<int> slots;
        // ORIG 尾参 a/b 为 .rodata 空串 0x08c9c3a0（邮件标题/正文查找键）。
        static const char kEmptyKey[1] = "";
        // [b28d-solo-ItemUpgradeVtableB2] P167：ORIG 0x08547295 在
        // insert_event_items 前**重调** getCurCharacInvenW（不复用上面的 invenW）。
        const int rc = ((CUserCharacInfo*)user)->getCurCharacInvenW()
                           ->insert_event_items(items, slots, (eMoneyAddReason)7,
                                                eItemAddReason_0x13, false,
                                                kEmptyKey, kEmptyKey);
        pg->put_byte(rc);

        // insert_event_items 输出三元组 {slot, itemIdx, addInfo2/count}。
        // 【batch-16 P175 反向铁律】ORIG 0x085472xx 起走 **begin/end 迭代器**
        // （vector::begin、normal_iterator<int const*>(int*) 转换 ctor、每轮重取
        // end()、operator!=、operator++ ×3、operator*），我们写成 size()+operator[]
        // 索引形会丢这 4 个迭代器调用并多出 3×ix 与 1×size（P167 差集实测
        // missing 23 / extra 13）。三元组取数用 `*(it + n)` 形复现 3 次 operator++。
        // [b28d-solo-ItemUpgradeVtableB2] P167/P28：ORIG 每轮调 **ppEi ×3**
        // （MISSING -3 `normal_iterator::ppEi`、EXTRA +3 `pl`）⇒ 源码是三次
        // 后置 `*it++` 取三元组，不是 `it += 3` + `*(it+n)`。
        for (std::vector<int>::const_iterator it = slots.begin();
             it != slots.end(); )
        {
            const int outSlot = *it++;
            const int outIdx = *it++;
            const int outCnt = *it++;
            pg->put_short(outSlot);
            pg->put_item_idx((unsigned long)outIdx);
            pg->put_int(outCnt);

            CItem* outScript = G_CDataManager()->find_item(outIdx);
            if (outScript != 0)
            {
                Inven_Item tmp;
                tmp.m_addInfo = outIdx;
                tmp.set_add_info(outCnt);
                outScript->make_item(tmp);
                GetInstanceCubeStatistic()->collectCubeStatistics(
                    outScript, outCnt, user, (CUBE_STATISTIC_FIELD)1);
                int price = 0;
                // ORIG 0x085474xx 直读 Inven_Item+0x07 低 16 位（`movzwl`/`movswl`），
                // 不走 get_add_info() 调用。
                G_Store()->GetSellItemPrice(tmp, outScript,
                                        (short)tmp.m_addInfo2, false,
                                            price);
                GetInstanceValueStatistic()->AddValueStatistic(
                    (VALUE_STATISTIC_FIELD)9, user, (unsigned int)price);
            }
        }
    }

    return 0;
