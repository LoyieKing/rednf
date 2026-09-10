bool CItemList::load_item_files()
{
    // ORIG 控制流：装备表循环内失败立即 return false；path 为空或表尾则
    // 转入可堆叠表循环；可堆叠表尾/空 path 时 DisplayMaxLength 后成功返回。
    STStackableScript stackScript;
    CEquipItem* pEquip = 0;
    CStackableItem* pStack = 0;
    const char* path = 0;
    std::map<int, std::string>::iterator it;

    sync_script::GetInstanceSyncScript()->truncate_dnf_item_info_table();

    it = sync_script::g_stEquipmentScriptFileList.begin();
    while (it != sync_script::g_stEquipmentScriptFileList.end())
    {
        STEquipmentScript eqScript;
        path = it->second.c_str();
        if (!path)
        {
            break;
        }
        if (!ImportEquipmentScript(&eqScript, path, false, 0, it->first))
        {
            LogManager::logFormat(1, "item.cpp",
                __PRETTY_FUNCTION__, 0x8e8,
                "ImportEquipmentScript() fail file - %s", path);
            return false;
        }
        // ORIG 0x8511843 `call GetInstanceSyncScript; mov %eax,(%esp)` 无槽往返
        // ⇒ 原码为 receiver 内联实参（P0/P183 禁具名指针缓存）。
        if (!sync_script::GetInstanceSyncScript()
                 ->insert_equiptment_item_to_db(&eqScript, 0, 0, 0))
        {
            if (eqScript.m_field1b4 != 0x16 && eqScript.m_field1b4 != 0x17 &&
                eqScript.m_field1b4 != 0x18 && eqScript.m_field1b4 != 0x19)
            {
                printf("insert_equiptment_item_to_db() fail file - %s", path);
            }
        }
        pEquip = new CEquipItem;
        if (pEquip == 0)
        {
            LogManager::logFormat(1, "item.cpp",
                __PRETTY_FUNCTION__, 0x903,
                "new CEquipItem error");
            return false;
        }
        pEquip->set_item(eqScript);
        if (!pEquip->verify())
        {
            LogManager::logFormat(1, "item.cpp",
                __PRETTY_FUNCTION__, 0x90a,
                "Equip item verify fail. [%s]", path);
            return false;
        }
        insert_item(pEquip->get_grade(), pEquip->get_index(), pEquip);
        ((ExtreamDungeon::CCompound_ExtreamDun_Item*)
            GlobalData::s_pCompoundBindCube)->check_N_addItem(*pEquip);
        // [b28d-20] P28 增量侧：p167 差集 ORIG 调 `_Rb_tree_iterator::ppEi`
        //   （后置，带 sret 临时槽）×2，我方 `++it` 落 EXTRA `ppEv` ⇒ 源写 it++。
        it++;
    }

    it = sync_script::g_stStackableScriptFileList.begin();
    while (it != sync_script::g_stStackableScriptFileList.end())
    {
        path = it->second.c_str();
        if (!path)
        {
            // [b28d-20] P167：ORIG 全函数仅 1 个 DisplayMaxLength 调用点
            //   （我方原写 2 处 ⇒ EXTRA +1）；改 break 落共享尾部。
            break;
        }
        if (!ImportStackableScript(&stackScript, path, false, it->first))
        {
            LogManager::logFormat(1, "item.cpp",
                __PRETTY_FUNCTION__, 0x926,
                "importStackableScript fail file - %s", path);
            return false;
        }
        if (GlobalInstance<Arad_GiftItem_Set>::inst()->open(&stackScript))
        {
            LogManager::logFormat(1, "item.cpp",
                __PRETTY_FUNCTION__, 0x931,
                "[ERROR] Arad GiftItem Setting!! [item_idx:%d][levelt:%d]",
                stackScript.m_field0, stackScript.m_field5c);
            return false;
        }
        if (!sync_script::GetInstanceSyncScript()
                 ->insert_stackable_item_to_db(&stackScript))
        {
            printf("insert_stackable_item_to_db() fail file - %s", path);
        }
        pStack = new CStackableItem;
        if (pStack == 0)
        {
            LogManager::logFormat(1, "item.cpp",
                __PRETTY_FUNCTION__, 0x941,
                "ptemp_stack_data is null");
            return false;
        }
        pStack->set_item(stackScript);
        if (!pStack->verify())
        {
            LogManager::logFormat(1, "item.cpp",
                __PRETTY_FUNCTION__, 0x948,
                "Stackable item verify fail. [%s]", path);
            return false;
        }
        // [b28d-solo] P225 推广：ORIG 0x8511xxx 两条件各发
        //   `cmp; sete/setne %al; test %al,%al; je` ⇒ 源码为嵌套 if 形
        //   （合并 && 被 4.4 折成直跳，无 setcc）。
        if (pStack->GetItemType() == 7)
        {
            if (pStack->m_lotto.m_items.size() !=
                pStack->m_lotto.m_counts.size())
            {
                // [L13-b14 P227] ORIG 在 cMyTrace C1 call **之前**右起求 varargs 并
                // 跨 ctor 保活 callee-saved；两语句形（具名 trace）4.4 只在 ctor
                // 后求实参 ⇒ 少 push/帧偏小。改全表达式临时链复现。
                cMyTrace(__PRETTY_FUNCTION__, 0x94f, 5)(
                    "Stackable item set_item fail (file [%s])", path);
                return false;
            }
        }
        if (pStack->GetItemType() == 0xd)
        {
            if (pStack->m_lotto.m_items.size() !=
                pStack->m_lotto.m_counts.size())
            {
                // [L13-b14 P227] 同上（ORIG 0x8511xxx 段）。
                cMyTrace(__PRETTY_FUNCTION__, 0x957, 5)(
                    "Stackable item set_item fail 2 (file [%s])", path);
                return false;
            }
        }
        insert_item(pStack->get_grade(), pStack->get_index(), pStack);
        it++;   // [b28d-20] P28 同上一处（ORIG ppEi）
    }
    sync_script::DisplayMaxLength();
    return true;
