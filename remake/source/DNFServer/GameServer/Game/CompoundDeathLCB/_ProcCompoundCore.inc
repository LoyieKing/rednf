int CCompoundAvatar::_ProcCompoundCore(CUser* user, tagCompoundInfo& info,
                                       tagCompoundAvatarTable* table)
{
    // 【b28d-solo-CompoundDeathLCB 判词】DIFF 封顶（§6.2）：ORIG 653 / ours 636、
    // p167 CLEAR(61/61)、无缺失调用。残余 = 块序/极性（`if (m_fieldD6==0 ||
    // m_type==4)` ORIG then 在后、4.4 在前）+ 间接虚调用 N7 + 帧 0xcc vs 0xc0
    //（ORIG 多 callee-saved %edi）+ 权重抽样段物理展开差异（N82/N164 族）。
    CInventory* inv = user->getCurCharacInvenW();
    const tagCompoundAvatar* rec =
        table->getCompoundAvatarInfo(info.m_field04);
    const int subType = CI_SubType(info.m_scriptA);
    int resultIdx = 0;                                         // -0x48

    if (info.m_fieldD6 == 0 || info.m_type == 4)
    {
        if (info.m_type == 4)
        {
            const int r = get_rand_int(
                rec->m_cube.m_field0c[subType]);               // +0x218 权重计数
            resultIdx = getItemIndex(rec->m_cube.m_vecs[subType], r);  // +0x240
        }
        else
        {
            resultIdx = info.m_fieldD0;                        // 直接选定结果
        }
    }
    else
    {
        // 输出权重池空则失败。
        if (rec->m_b[subType] == 0 || rec->m_optsA[subType].empty())
            return 1;

        int weight = 0;                                        // -0x38
        if (info.m_scriptA->get_grade() == 1 &&
            info.m_scriptB->get_grade() == 1)
        {
            weight = rec->m_d[subType];                        // +0x16c
            if (m_rand->randInt(
                    static_cast<unsigned long>(10000)) < 5000) // setbe 0x1387
            {
                const int r = m_rand->randInt(
                    static_cast<unsigned long>(weight));
                resultIdx = getItemIndex(rec->m_optsC[subType], r);  // +0x194
            }
            if (resultIdx < 1)
                weight = rec->m_a[subType];                    // +0x004 兜底权重
        }
        else
        {
            weight = rec->m_e[subType];                        // +0x2e0
        }

        if (resultIdx < 1)
        {
            // 服务端全局修正（datamgr+0x4a54 加成 / +0x4a58 千分比衰减；
            // CDataManager 域具名化后改为成员访问）。
            if (info.m_type == 1)
            {
                // datamgr+0x4a54 = m_serverParameter(+0x68)+0x49ec
                //（importServerParameterScript "[upper bind cube rate bonus]" 写点）；
                // ORIG 0x08333170 `mov 0x4a54(%eax),%eax` = 4 字节读 → int 成员。
                weight += G_CDataManager()->m_serverParameter.m_49ec;
            }
            if (info.m_type == 3)
            {
                // datamgr+0x4a58 = m_serverParameter+0x49f0
                //（"[lower bind cube rate bonus]"；ORIG 0x0833318c `mov 0x4a58(%eax),%eax`）。
                weight -= (G_CDataManager()->m_serverParameter.m_49f0 * weight) /
                          10000;
            }
            // [轴 B 2026-09-10 保留] ORIG 0x083331d1 `cmp -0x38(%ebp),%eax; setl %al`
            // （**有符号序**，槽 -0x38 = weight；上下文同构：`call randInt(const&)`
            // 后即刻比较，与 ORIG 0x083331cc/0x083331d1 逐条对应）⇒ 原码比较为有符号；
            // 旧文 `unsigned long < int` 按 C++ 规则转无符号 ⇒ 4.4 产 `setb`。
            // 加 `(int)` 复现有符号序（randInt(10000) ∈ [0,9999]，值域内无分叉）。
            // 实测：our 636→635；汇编 diff 段 992→988 行（minus 364→361）⇒ ndiff 严格改善；
            // TU 四数 32/50/1/3/8 逐值不变（不劣）⇒ 保留。
            // 同件另一行（`_ProcCompoundCore` ORIG 0x08333157 `cmpl $0x0,-0x48; jg`）
            // 经地址自校为**表内配对假阳性**：我方 0x1280 同为 `jg`（有符号）⇒ 不改。
            if ((int)m_rand->randInt(static_cast<unsigned long>(10000)) < weight)
            {
                const int r = get_rand_int(rec->m_b[subType]);
                resultIdx = getItemIndex(rec->m_optsA[subType], r);  // +0x7c
            }
        }
    }

    if (resultIdx == 0)
    {
        const int r = get_rand_int(rec->m_c[subType]);         // +0x054 权重计数
        resultIdx = getItemIndex(rec->m_optsB[subType], r);    // +0xf4
    }

    if (resultIdx != 0 && G_CDataManager()->find_item(resultIdx) != 0)
    {
        // ---- 成功路径：删两件材料 ----
        CItem* item = G_CDataManager()->find_item(resultIdx);
        if (!item->IsCreatureItem())          // ORIG vtable+0x10（IsCreatureItem）
            return 1;
        // ORIG 0x083330ec：槽位由目标 item 的 avatar option size 随机重选，
        // 且 >0x63(99) 时清零（覆盖客户端传入 slot）。
        info.m_slot = static_cast<unsigned short>(get_rand_int(
            reinterpret_cast<CEquipItem*>(item)->getAvatarOptionSize()));
        if (info.m_slot > 99)
            info.m_slot = 0;
        if (inv->delete_item((INVEN_TYPE)2,
                             static_cast<int>(info.m_field08), 1,
                             static_cast<eItemDelReason>(3), true) == 1 &&
            inv->delete_item((INVEN_TYPE)2,
                             static_cast<int>(info.m_field48), 1,
                             static_cast<eItemDelReason>(3), true) == 1)
        {
            bool bFoundAll = false;                            // -0x8d
            // 【batch-16 物理展开】ORIG 0x0833321x..0x083332bx 是 4 个 if-else 分支
            // **各自完整调用一次** delete_event_items + vector::size + 比较 + 独立
            // `return 0x16` 出口（调用点差集实测：提公共 materials 指针后单调用会
            // 少 3 个 delete_event_items 与 2 个 size）。type4 的 size 比较对象
            // 仍是 tailC（ORIG 0x083332b7 读 +0x2d0）【怪癖保留】。
            if (info.m_type == 0)
            {
                const int deleted = inv->delete_event_items(
                    rec->m_tailA, info.m_results, bFoundAll,
                    static_cast<eMoneySubReason>(0x12),
                    static_cast<eItemDelReason>(0x12), false);
                if (deleted != static_cast<int>(rec->m_tailA.size()))
                    return 0x16;
            }
            else if (info.m_type == 1)
            {
                const int deleted = inv->delete_event_items(
                    rec->m_tailB, info.m_results, bFoundAll,
                    static_cast<eMoneySubReason>(0x12),
                    static_cast<eItemDelReason>(0x12), false);
                if (deleted != static_cast<int>(rec->m_tailB.size()))
                    return 0x16;
            }
            else if (info.m_type == 3)
            {
                const int deleted = inv->delete_event_items(
                    rec->m_tailC, info.m_results, bFoundAll,
                    static_cast<eMoneySubReason>(0x12),
                    static_cast<eItemDelReason>(0x12), false);
                if (deleted != static_cast<int>(rec->m_tailC.size()))
                    return 0x16;
            }
            else if (info.m_type == 4)
            {
                const int deleted = inv->delete_event_items(
                    rec->m_cube.m_vec, info.m_results, bFoundAll,
                    static_cast<eMoneySubReason>(0x12),
                    static_cast<eItemDelReason>(0x12), false);
                if (deleted != static_cast<int>(rec->m_tailC.size()))
                    return 0x16;
            }

            const int curSec = GlobalData::s_systemTime_.getCurSec();
            // 两段 remain 计算各自重取 invenR → GetAvatarItemMgrR（ORIG
            // 0x0833330a / 0x08333345，不缓存 mgr）。
            const int remainA =
                ((CUserCharacInfo*)user)
                    ->getCurCharacInvenR()
                    ->GetAvatarItemMgrR()
                    ->GetRemainDate(static_cast<int>(info.m_materialA.m_addInfo2),
                                    curSec);
            const int remainB =
                ((CUserCharacInfo*)user)
                    ->getCurCharacInvenR()
                    ->GetAvatarItemMgrR()
                    ->GetRemainDate(static_cast<int>(info.m_materialB.m_addInfo2),
                                    curSec);

            // 注销并广播移除事件。
            inv->GetAvatarItemMgrW()->UnRegistItem(
                static_cast<int>(info.m_materialA.m_addInfo2));
            inv->SendAvatarEvent(0x36,
                                 static_cast<int>(info.m_materialA.m_addInfo2),
                                 1, static_cast<int>(info.m_field08) + 10, 0);
            info.m_results.push_back(1);                       // 结果三元组①
            info.m_results.push_back(static_cast<int>(info.m_field08));
            info.m_results.push_back(1);
            inv->GetAvatarItemMgrW()->UnRegistItem(
                static_cast<int>(info.m_materialB.m_addInfo2));
            inv->SendAvatarEvent(0x36,
                                 static_cast<int>(info.m_materialB.m_addInfo2),
                                 1, static_cast<int>(info.m_field48) + 10, 0);
            info.m_results.push_back(1);                       // 结果三元组②
            info.m_results.push_back(static_cast<int>(info.m_field48));
            info.m_results.push_back(1);

            char ipgNoBuf[0x14];                               // -0xa5
            _generateIPGNo(user, curSec, ipgNoBuf);

            // AddAvatarItem 走**重取**的 invenW（ORIG 0x083333xx getCurCharacInvenW）。
            info.m_resultSlot = static_cast<unsigned short>(
                user->getCurCharacInvenW()->AddAvatarItem(
                    resultIdx, remainA + remainB, false,
                                   static_cast<char>(info.m_slot),
                                   static_cast<char>(-1), ipgNoBuf,
                                   static_cast<eAvatarItemAddReason>(6), true,
                                   0));
            if (static_cast<short>(info.m_resultSlot) == -1)
            {
                cMyTrace trace(
                    "ENUM_ERROR WongWork::CCompoundAvatar::_ProcCompoundCore(CUser*, "
                    "WongWork::CCompoundAvatar::stCompoundAvatarInfo&, "
                    "stCompoundAvatarTable*)",
                    0x361, 5);
                trace("[%s][%d] AddAvatarItem Fail",
                      "ENUM_ERROR WongWork::CCompoundAvatar::_ProcCompoundCore(CUser*, "
                      "WongWork::CCompoundAvatar::stCompoundAvatarInfo&, "
                      "stCompoundAvatarTable*)",
                      0x361);
                return 0x11;
            }
            DB_UpdateAvatarHistory::makeRequest(
                static_cast<unsigned int>(user->getCurCharacNo()),
                static_cast<int>(info.m_materialA.m_addInfo2), ipgNoBuf);
            DB_UpdateAvatarHistory::makeRequest(
                static_cast<unsigned int>(user->getCurCharacNo()),
                static_cast<int>(info.m_materialB.m_addInfo2), ipgNoBuf);
            return 0;
        }
        return 0x16;                                           // delete_item 失败
    }
    return 1;                                                  // 结果不可实例化
