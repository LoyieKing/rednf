// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08671102：N82/N89 封顶——p167 差集未清零
// （memset/vector begin/end 族），gap -5。
// （memset/vector begin/end 族，见 /tmp/b28d_solo_CUserOnlyOrig_result.tsv）；
// 残余为帧槽/模板内联跨版本差（N82/N89 族），语义完成。
bool CUser::_ProcCompoundCore(
    // [Q2P342ScanB 2026-09-11] 形参拼写改用 GameTypes.h 的 typedef（同底层类型 ⇒
    // mangled/ABI 零变化），使 `__PRETTY_FUNCTION__` 打印 ORIG 的裸 typedef 名
    // （ORIG 串 `_ZZN5CUser17_ProcCompoundCore…E19__PRETTY_FUNCTION__` @0x8cf2bc0
    //  size=0xb6 = `bool CUser::_ProcCompoundCore(const IntPairVector&, IntPairVector&,
    //  IntVector&, int, bool, PacketGuard*, bool&, RequestItemCountType, stEquipCondition,
    //  CStackableItem*, Inven_Item&)`，pstr.py 实读）。
    const IntPairVector& mats,
    IntPairVector& outputs,
    IntVector& deletedSlots,
    int mainMatSlotNo,
    bool cubeMode,
    PacketGuard* pkt,
    bool& resultFlag,
    RequestItemCountType mulCount,
    stEquipCondition cond,
    CStackableItem* recipe,
    Inven_Item& keptOptionSrc)
{
    // ORIG 0x08671120..0x08671145 入口连续三次构造，声明序即 ORIG 构造序：
    //   ctor(ebp-0x8c) vector<int>  ← deletedSet
    //   ctor(ebp-0x98) vector<int>  ← insertedSlots
    //   ctor(ebp-0x9c) __gnu_cxx::__normal_iterator<int*, vector<int> >  ← it
    // 第三个是 ORIG 真实存在的迭代器默认构造（Ghidra: local_a0），两个 3 元组
    // 循环共用同一个迭代器变量，故必须在函数作用域声明、循环内仅赋值。
    std::vector<int> deletedSet;      // ebp-0x8c（delete_event_items 输出，3 int/项）
    std::vector<int> insertedSlots;   // ebp-0x98（insert_event_items 输出，3 int/项）
    std::vector<int>::iterator it;    // ebp-0x9c（ORIG __normal_iterator C1Ev ×1）

    // ORIG 入口未把 getCurCharacInvenW() 落到栈局部：接收者直接内联求值
    // （ORIG 0x0867114a 先 movzbl cubeMode→%ebx，再 call getCurCharacInvenW）。
    const int delCount = getCurCharacInvenW()->delete_event_items(
        mats, deletedSet, resultFlag,
        (eMoneySubReason)0xc, (eItemDelReason)8, cubeMode);

    if (delCount == 0)
    {
        // ORIG 0x08671199 起先求 mats.size()→%esi、getCurCharacNo()→%ebx，
        // 之后才 call cMyTrace::cMyTrace —— 即 operator() 作用于**临时**
        // cMyTrace 对象（具名局部会先构造后求参，顺序与 ORIG 相反）。
        cMyTrace(
            __PRETTY_FUNCTION__,   // [Q2P342ScanB] 原手写串 → prett（发射 ORIG 同符号
                                   // _ZZN5CUser17_ProcCompoundCore…E19__PRETTY_FUNCTION__ size=0xb6）
            0x5c8b, 5)(
            "CUser::compound_item, getCurCharacInvenR()->delete_event_items , size does not match! %d %d !=%d",
            getCurCharacNo(), delCount, (int)mats.size());
        return false;    // ORIG ebx=0 → 0x08671e69
    }

    if (cubeMode)        // ebp+0x1c（ORIG cmpb 0 检查 @0x08671202）
    {
        pkt->put_byte(delCount);
    }
    else
    {
        pkt->put_byte((int)mulCount + delCount);   // movzwl mulCount + delCount
        if (!getCurCharacInvenW()->delete_item(
                (INVEN_TYPE)1 /*INVEN_TYPE_EQUIP*/, mainMatSlotNo,
                (int)mulCount, (eItemDelReason)8, true))
        {
            cMyTrace(
                __PRETTY_FUNCTION__,   // [Q2P342ScanB] 同上
                0x5c9a, 5)(
                "CUser::compound_item, getCurCharacInvenW()->delete_item failed , %d %d",
                getCurCharacNo(), mainMatSlotNo);
            return false;
        }
        pkt->put_byte(0);
        pkt->put_short(mainMatSlotNo);
        pkt->put_int((int)mulCount);
    }

    // 删除结果回包：deletedSet 三元组逐项 put_byte/put_short/put_int
    if (delCount > 0)
    {
        for (it = deletedSet.begin(); it != deletedSet.end();)
        {
            const int v0 = *it++;
            const int v1 = *it++;
            const int v2 = *it++;
            pkt->put_byte(v0);
            pkt->put_short(v1);
            pkt->put_int(v2);
        }
    }

    // recipe+0x1a8 = m_recipe174(+0x174).m_field34
    const bool succeed = isSuccessCompound(recipe->m_recipe174.m_field34);

    if (!succeed)
    {
        recipe->m_recipe174.changeOutput(outputs);
        if (outputs.empty())
        {
            pkt->put_byte(0);
            return true;    // ORIG ebx=1 @0x086714a3
        }
    }

    const int insCount = getCurCharacInvenW()->insert_event_items(
        outputs, insertedSlots,
        (eMoneyAddReason)7, (eItemAddReason)9, false, "", "");
    pkt->put_byte(insCount);

    if (insCount != (int)outputs.size())
    {
        cMyTrace(
            __PRETTY_FUNCTION__,   // [Q2P342ScanB] 同上
            0x5d73, 5)(
            "CUser::compound_item, getCurCharacInvenR()->insert_event_items failed %d",
            getCurCharacNo());
        return false;
    }

    // insertedSlots 三元组 [0/结果码 r, slotNo, count/itemIdx]：每轮消费 3 int
    for (it = insertedSlots.begin(); it != insertedSlots.end();)
    {
        const int slotRef = *it++;    // +0x453 deref → local_a4（push_back/put_short 用）
        // ORIG 0x08671102 区内无 _ZN10Inven_ItemC1Ev 调用点，仅 1×GetInvenSlot：
        // 按值返回直接 sret 拷贝初始化（NRVO），不存在"先默认构造再赋值"。
        Inven_Item got = getCurCharacInvenR()->GetInvenSlot((int)INVEN_TYPE_EQUIP, slotRef);

        if (got.m_itemType == 1)        // ORIG movzbl got+0x01 cmp 1（ITEM_TYPE_EQUIP）
        {
            CItem* script = G_CDataManager()->find_item(got.m_addInfo);
            if (script != 0)
            {
                if ((char)cond.m_upgrade == 1)    // ebp+0x2c（cond+0）
                {
                    if (!succeed)
                    {
                        if (recipe->m_recipe174.m_field3c == 1)   // recipe+0x1b0
                            copyItemOption(got, keptOptionSrc);
                    }
                    else
                    {
                        if (recipe->m_recipe174.m_field3c == 1)
                        {
                            copyItemOption(got, keptOptionSrc);
                        }
                        else
                        {
                            // vtable+0xc 虚调用（CItem::GetItemType）结果作尾参
                            setRecipeEqiupmentOption(got, cond, slotRef, recipe,
                                                     script->GetItemType());
                        }
                        if (recipe->m_recipe174.m_field38 != 0)   // recipe+0x1ac
                        {
                            compoundSystemMessage(got);
                            compoundGuildMessage(got);
                        }
                    }
                    getCurCharacInvenW()->update_item((INVEN_TYPE)1, slotRef, got);
                }
                else
                {
                    // ORIG: rarity==5 || GetItemType()==0xb || recipe->GetNeedLevel()==0
                    //       → doMake=false；否则 true
                    // [SubD D1 2026-09-11 负结论·已回滚] 内联 `!(rarity==5||type==0xb||needLv==0)`
                    // 实测 **850→850 中性**（4.4 仍产槽形）⇒ 回滚；本件 gap -6 属 N82 族。
                    const bool doMake = !(script->get_rarity() == 5 ||
                                          script->GetItemType() == 0xb ||
                                          script->GetNeedLevel() == 0);
                    if (doMake)
                    {
                        // GetCharacExpandData(10) 返回 CItemMakingSkill*（charac_expand::CData 派生）
                        CItemMakingSkill* skill =
                            (CItemMakingSkill*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)10);
                        const int level = skill->GetItemMakingSkillLevel(recipe, this);

                        const stItemMakingSkill* table =
                            (const stItemMakingSkill*)G_CDataManager()->GetItemMakingSkill();
                        std::map<int, stUpgradeWithCompound>::const_iterator ti =
                            table->m_map0.find(level);
                        // [b28d-solo-CUserOnlyOrig P167 真形] ORIG 0x08671833/0x0867183b
                        // 在 find 之后**再调** G_CDataManager()->GetItemMakingSkill()
                        // 取 .end()（p167 MISSING G_CDataManager+GetItemMakingSkill 各 1）；
                        // 旧形复用局部 table 少这两个调用点。
                        if (ti == ((const stItemMakingSkill*)
                                       G_CDataManager()->GetItemMakingSkill())->
                                       m_map0.end())
                        {
                            // [K7 2026-09-11 §6.5 PRETTY 串盲区] ORIG 0x08671887 压入
                            // 0x8cf2bc0 = `_ZZN5CUser17_ProcCompoundCore…E19__PRETTY_FUNCTION__`
                            // （size 0xb6，pstr.py 实读 `bool CUser::_ProcCompoundCore(const
                            // IntPairVector&, IntPairVector&, IntVector&, int, bool,
                            // PacketGuard*, bool&, RequestItemCountType, stEquipCondition,
                            // CStackableItem*, Inven_Item&)`）。
                            // [Q2P342ScanB 2026-09-11 复核] 旧注「prett 在此展开为模板形 ⇒
                            // 必须写字面量」**已推翻**：本函数非模板、形参改 typedef 拼写
                            // 后 prett 逐字节同 ORIG（本 TU 4 处统一改用 prett）。
                            LogManager::logFormat(1, "user.cpp",
                                __PRETTY_FUNCTION__,
                                                  0x5d0c,
                                                  "item_making_skill_level error : %d",
                                                  level);
                            return false;
                        }

                        int upgrade = 0;
                        if (recipe->m_recipe174.m_field25 == 0)   // recipe+0x199
                        {
                            // iter+0xc = ti->second.m_field08（概率分子，分母固定 10000）
                            // iter+0x04 = m_field00（基值）；iter+0x10 = m_rangeExp0C（/1000）
                            // iter+0x14 = m_field10（minVal 基值）；iter+0x20 = m_rangeExp1C（/1000）
                            if ((unsigned int)get_rand_int(10000)
                                < (unsigned int)ti->second.m_field08)
                            {
                                upgrade = ti->second.m_field00
                                        + get_rand_int(ti->second.m_rangeExp0C) / 1000;
                            }
                            else
                            {
                                upgrade = ti->second.m_field10
                                        + get_rand_int(ti->second.m_rangeExp1C) / 1000;
                            }
                        }
                        if (upgrade < 0 || upgrade > 0x1f)
                            upgrade = 0;

                        got.SetUpgrade((unsigned char)upgrade);
                        if (got.GetUpgrade() != 0)
                            getCurCharacInvenW()->update_item((INVEN_TYPE)1, slotRef, got);

                        if (recipe->GetItemType() == 3)   // recipe vtable+0xc
                        {
                            // ORIG 0x08671ab6/0x08671abb：mov 0x5e0(%eax),%ebx
                            // 其中 eax=G_CDataManager()；绝对 +0x5e0 =
                            // m_serverParameter(+0x68) + m_noticeLegacyWithUpgrade(+0x578)
                            // 写入：importServerParameterScript 标签 [notice legacy with upgrade]
                            const int capUpgrade =
                                G_CDataManager()->m_serverParameter.m_noticeLegacyWithUpgrade;
                            if (capUpgrade <= (int)got.GetUpgrade())
                            {
                                PacketGuard pg;
                                pg.put_header(0, 0x56);
                                pg.put_byte(7);
                                pg.put_byte(1);
                                pg.put_short((int)(get_unique_id() & 0xffff));
                                pg.put_int(got.m_addInfo);
                                pg.put_byte((int)got.GetUpgrade());
                                pg.finalize(true);
                                G_GameWorld()->send_all(pg);
                            }
                        }
                    }
                }
            }
        }

        deletedSlots.push_back(slotRef);
        pkt->put_byte(0);
        pkt->put_short(slotRef);

        const int v1 = *it++;    // put_int#1 数据（三元组第 2 元素）
        const int v2 = *it++;    // put_int#2 数据（三元组第 3 元素）
        pkt->put_int(v1);
        pkt->put_int(v2);

        pkt->put_byte((int)got.GetUpgrade());
        pkt->put_short((int)got.m_endurance);                 // got+0x0b（endurance，movzwl -0xe6）
        pkt->put_byte((int)got.m_amp.getAbilityType());    // stAmplifyOption_t@got+0x11
        pkt->put_short((int)got.m_amp.getAbilityValue());
        pkt->put_byte((int)(unsigned char)got.m_seal != 0 ? 1 : 0);  // movzbl got+0x00 setne
        pkt->put_packet(got);

        if (cubeMode)
        {
            GetInstanceCubeStatistic()->collectCubeStatistics(v1, v2, this,
                                                              (CUBE_STATISTIC_FIELD)3);
        }
    }

    return true;
}