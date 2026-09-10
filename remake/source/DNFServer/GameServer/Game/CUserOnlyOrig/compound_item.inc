int CUser::compound_item(int itemIdx, bool useRecipe2ndCheck,
                         unsigned short count, int subCmd)
{
    int recipeKeyOut = 0;                                          // local_80
    CStackableItem* recipe =                                       // local_5c（ORIG -0x58 存 CItem* 返回，直接当 CStackableItem 布局访问）
        static_cast<CStackableItem*>(_GetRecipeItem(
            (unsigned long)itemIdx, useRecipe2ndCheck, recipeKeyOut));
    if (recipe == 0)
        return 0x11;

    const RecipeInfo& recipeInfo = recipe->m_recipe174;            // +0x174
    if (is_recipe_items_avatar_type(recipeInfo.m_vecC))            // +0x180（ORIG 汇编）
    {
        return process_recipe_avatar_items(itemIdx, useRecipe2ndCheck,
                                           count, subCmd, recipe);
    }
    if (!_CheckCompoundSkill(recipe, useRecipe2ndCheck))           // xor1/test → !result
        return 0x16;

    int blankSlots[11];                                            // local_124
    memset(blankSlots, 0, sizeof(blankSlots));
    int needSlots[11];                                             // local_150
    memset(needSlots, 0, sizeof(needSlots));
    int keepSlots[11];                                             // local_17c
    memset(keepSlots, 0, sizeof(keepSlots));

    int freeQuick = getCurCharacInvenR()->QuickEmptyCount();       // local_58

    std::vector<std::pair<int,int> > delList(recipeInfo.m_vec0);   // local_8c ← +0x174
    // [b28d-PoolCUOO2 真语义修复 2026-09-09] ORIG 0x08672369-0x086723c0：非空时插入的是
    // **recipe+0x19c = recipeInfo.m_vec28**（const begin/end + insert<IPK 模板>），
    // 旧「自我倍增」形（delList.begin/end 非 const）系误读，p167 const be/en MISSING 实证。
    if (!delList.empty())
        delList.insert(delList.end(), recipeInfo.m_vec28.begin(), recipeInfo.m_vec28.end());
    std::vector<std::pair<int,int> > delListScaled(delList);       // local_98
    if (subCmd == 0xf2)
        CalculateExpertJobMaterials(delListScaled);
    std::vector<std::pair<int,int> > resultMulList(recipeInfo.m_vecC);  // local_a4 ← +0x180

    _CalcItemVectorMultiply(delListScaled, (int)count);
    _CalcItemVectorMultiply(resultMulList, (int)count);
    if (_IsCompoundResult2MoreEquip(resultMulList))
        return 0x17;
    if (!getCurCharacInvenR()->CheckNeedItemList(delListScaled))   // ORIG 以 local_98 校验
        return 0x15;

    stEquipCondition condObj;                                      // local_ac
    Inven_Item condItem;                                           // local_1b9
    const int checkRet = getCurCharacInvenR()->CheckEquipmentItemCondition(
        delListScaled, condObj, condItem);                         // ORIG 以 local_98 校验
    if (checkRet != 0)
        return checkRet;

    getCurCharacInvenR()->CalBlankSlot(blankSlots, delListScaled);
    if ((!useRecipe2ndCheck) && (recipeKeyOut == 1))
        blankSlots[2] = blankSlots[2] + 1;
    getCurCharacInvenR()->CalNeedSlot(needSlots, delList);

    freeQuick = freeQuick + blankSlots[0];
    for (int si = 1; si < 0xb; ++si)
    {
        if (keepSlots[si] + blankSlots[si] < needSlots[si])
        {
            freeQuick = freeQuick + blankSlots[si] +
                        (keepSlots[si] - needSlots[si]);
            if (freeQuick < 0)
                return 4;
        }
    }

    STItemScript::SEXPERTJOB::stExpertCompoundResultVariation variation;  // local_b4
    // [b28d-PoolCUOO2 修法] ORIG 本段 receiver 是**值版** getCurCharacInvenR()（InvenR
    // 7/RefR 1 直方图实证；RefR 唯一落点=下方 setCopy），且 ExpertJobType ORIG 调 ×2
    // 不缓存（第二点在下面 increaseItemCount）⇒ 去 invRef/expertType 具名缓存。
    const int expertExp = GetCurCharacExpertJobExp();
    const int expertLevel = GetCurExpertJobLevel(expertExp);
    const_cast<CInventory*>(getCurCharacInvenR())->GetExpertJobCompoundResultVariation(
        (ENUM_EXPERT_JOB_TYPE)GetCurCharacExpertJobType(), expertLevel, variation);

    char limitStackHit = 0;                                        // local_49
    int failCode = 0;                                              // unaff_EBX
    bool shadowOk = false;                                         // bVar2
    CInventory shadowInv;                                          // local_9b0
    InventoryMemory shadowMem;                                     // local_caef
    shadowInv.SetInventoryMemory(&shadowMem);
    shadowInv.setCopy(*const_cast<CInventory*>(getCurCharacInvenRefR()));
    if (shadowInv.tryDeleteEventItems(delListScaled))
    {
        if (useRecipe2ndCheck)
        {
            shadowOk = true;
        }
        else if (shadowInv.delete_item((INVEN_TYPE)1, itemIdx,
                                       (int)count, (eItemDelReason)8, false))
        {
            shadowOk = true;
        }
        else
        {
            failCode = 0x11;
        }

        if (shadowOk)
        {
            if (variation.m_field4 <= 0.0f)                        // local_b0
            {
                shadowOk = (shadowInv.tryInsertEventItems(resultMulList) != 0);
                if (!shadowOk)
                    failCode = 4;
            }
            else
            {
                std::vector<std::pair<int,int> > scaled(resultMulList);  // local_dc
                for (std::vector<std::pair<int,int> >::iterator it = scaled.begin();
                     it != scaled.end(); ++it)
                {
                    CItem* def = G_CDataManager()->find_item(it->first);  // local_40（ORIG Ghidra 视图 CStackableItem*）
                    if (def != 0)
                    {
                        if (def->is_stackable())
                        {
                            if (static_cast<CStackableItem*>(def)
                                    ->getStackableLimit() == 1)
                                limitStackHit = 1;
                        }
                        else
                        {
                            limitStackHit = 1;
                        }
                    }
                    if (limitStackHit != 1)
                    {
                        // ORIG 0x86727a7..0x86727f9：
                        // pair.second = (int)ROUND(field4 / 100.0f) * pair.second
                        it->second = static_cast<int>(variation.m_field4 / 100.0f + 0.5f)
                                     * it->second;
                    }
                }
                if (shadowInv.tryInsertEventItems(scaled) == 0)
                    failCode = (limitStackHit == 0) ? 100 : 4;
                else
                    shadowOk = true;
            }
        }
    }
    else
    {
        failCode = 0x11;
    }

    if (!shadowOk)
        return failCode;

    char bSendEquip = 0;                                           // local_b5
    PacketGuard packet;                                            // local_c4
    packet.put_header(1, subCmd);
    packet.put_byte(1);
    if (recipeInfo.m_field25 != 0)                                 // recipe+0x199
        {
            // [b28d-PoolCUOO2 真语义修复] ORIG 0x086729xx：size 与遍历对象均为
            // **recipeInfo.m_vecC（recipe+0x180 const）**——发包用配方**原始**材料表，
            // 非倍增后的 resultMulList。const begin/end + 同型 ne + ++ri（ppEv）。
            packet.put_byte((int)recipeInfo.m_vecC.size());
            for (std::vector<std::pair<int,int> >::const_iterator ri = recipeInfo.m_vecC.begin();
                 ri != recipeInfo.m_vecC.end(); ++ri)
    {
            packet.put_int(ri->first);
            packet.put_int(ri->second);
        }

        int perExp = 0;                                            // local_ec
        int successCnt = 0;                                        // local_3c
        int expGain = 0;                                           // local_38
        int bonusCnt = 0;                                          // local_34
        const float ratio = variation.m_field4 / 100.0f;           // local_30
        for (int ci = 0; ci < (int)count; ++ci)
        {
            if (expert_job::CExpertJob::IsSuccess(this, recipe, perExp))
            {
                expGain = expGain + perExp;
                const int dice = get_rand_int(100);
                if (((float)dice < variation.m_field0) && (limitStackHit != 1))
                    bonusCnt = bonusCnt + 1;
                successCnt = successCnt + 1;
            }
        }
        if (successCnt < 1)
        {
            resultMulList.clear();
        }
        else
        {
            expert_job::CExpertJob::IncreaseExpertJobExp(
                this, GetCurCharacExpertJobExp() + expGain);
            // ORIG 此循环 ppEi（后置）⇒ `it++`。
            for (std::vector<std::pair<int,int> >::iterator it = resultMulList.begin();
                 it != resultMulList.end(); it++)
            {
                const int perCount = it->second / (int)count;      // local_28
                it->second = (it->second / (int)count) * successCnt;
                if (0 < bonusCnt)
                {
                    // ORIG：ROUND(perCount * ratio - perCount) * bonusCnt
                    const int extra =
                        static_cast<int>((float)perCount * ratio - (float)perCount + 0.5f)
                        * bonusCnt;
                    it->second = it->second + extra;
                }
                CExpertJobStatistic* stat = GetInstanceExpertJobStatistic();
                stat->increaseItemCount((ENUM_EXPERT_JOB_TYPE)GetCurCharacExpertJobType(),
                                        (unsigned long)it->first, it->second);
            }
        }
        packet.put_int(successCnt);
        packet.put_int((int)count - successCnt);
        if (recipeInfo.m_field25 != 0)
            packet.put_byte(bonusCnt);
    }

    std::vector<int> updatedSlots;                                 // local_d0
    updatedSlots.clear();
    if (!_ProcCompoundCore(delListScaled, resultMulList, updatedSlots,
                           itemIdx, useRecipe2ndCheck, &packet,
                           *(bool*)&bSendEquip, count, condObj, recipe,
                           condItem))
        return 0x11;

    packet.finalize(true);
    Send(packet);
    if (!updatedSlots.empty())
    {
        // ORIG int 循环 ppEi ⇒ `ui++`。
        for (std::vector<int>::const_iterator ui = updatedSlots.begin();
             ui != updatedSlots.end(); ui++)
        {
            const int slot = *ui;                                  // local_20
            if (-1 < slot)
            {
                const Inven_Item it = getCurCharacInvenR()->GetInvenSlot(1, slot);
                if (it.m_field1 == 1)                              // local_1f5：equip 类型
                    SendUpdateItem((eSendTarget)1, (ENUM_ITEMSPACE)0, slot);
            }
        }
    }
    if (bSendEquip != '\0')
        send_equip(0x16);
    _onCompoundItem(recipe->get_index());
    if (recipe->GetNeedLevel() != 0)
    {
        CItemMakingSkill* skill =
            reinterpret_cast<CItemMakingSkill*>(
                GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)10));
        skill->GainItemMakingSkillPoint(recipe, this);
    }
    HistoryLog::WriteComposition((FILE*)m_field796f8, recipe->get_index());
    return 0;
