// §6.2 巨件核销：P167 98/98 [0缺0多]；gap 811→811/818；残差归因 N82/P112
// （diff 形态计数对称无缺类，详见 docs/asm_translation_patterns.md 负例表）。
// [b28d-29] N82：帧 0x2bc vs 0x2dc + 槽平移（-0x26c vs -0x28c），调用点全等  (ORIG 0x081f92ca insn 811->818)
// 无真修条目。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 544（ORIG 807 / ours 815），残差=setcc-fold bool 物化岛/P211/N18a 中转/store 序/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
int DisPatcher_DisJointItem::_disjoint(CUser* user, int slot, int type,

                                       ENUM_CMDPACKET cmd, CUser* targetUser,
                                       unsigned short region)
{
    // 081f9301 _check_disjointable_user_state(user, cmd, (char)type, (short)slot)
    if (!_check_disjointable_user_state(user, cmd,
                                        static_cast<char>(type),
                                        static_cast<short>(slot)))
    {
        return 0;
    }

    PacketGuard packet;                              // local_74（try 块外构造）
    try
    {
        // +0x337 put_header(1, cmd)
        packet.put_header(1, cmd);

        // +0x042 GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)type)（W 0x080f7845，定义 CTitleBook.cpp）
        const int invenType =
            GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)static_cast<ENUM_ITEMSPACE>(type));
        expert_job::CExpertJob* expertJob = 0;       // local_44
        if (targetUser != 0)
        {
            expertJob = targetUser->GetCurCharacExpertJob();   // 0x0822f8d4
            if (expertJob == 0)
            {
                LogManager::logFormat(1, "PacketDispatcher_Impl_1.cpp",
                                      __PRETTY_FUNCTION__, 0x87b7,
                                      "expert job null point");
                throw (ENUM_ERROR)0x13;
            }
            if (expertJob->GetType() != 3)            // W 0x08234796，非虚
            {
                LogManager::logFormat(1, "PacketDispatcher_Impl_1.cpp",
                                      __PRETTY_FUNCTION__, 0x87bd,
                                      "expert job permission error");
                throw (ENUM_ERROR)0x13;
            }
            expertJob->OnStartMission(targetUser, user);  // vptr slot0 虚调用
        }

        // +0x18b CUser::CheckItemLock(int,int) const
        if (user->CheckItemLock(invenType, slot))
        {
            throw (ENUM_ERROR)0xd5;
        }

        Inven_Item invenItem;                          // local_fd
        CItem* item = 0;                               // local_78
        if (region != 0xffff)
        {
            GameWorld* gw = G_GameWorld();
            if (gw->IsPVPChannel())
            {
                throw (ENUM_ERROR)0x60;
            }
        }
        // +0x263 _check_disjointable_item_state（C++ 名定义于 ExpertJobLink4.cpp）
        _check_disjointable_item_state(user, slot, invenType, invenItem, &item,
                                       targetUser, region);
        // +0x286 _check_disjoint_need_slot（C++ 名定义于 ExpertJobLink4.cpp）
        _check_disjoint_need_slot(
            user, item, invenItem,
            reinterpret_cast<expert_job::CDisjointer*>(expertJob));

        DisJointResult result;                         // local_c0，ctor 清零 10×int
        const int srcItemIdx = invenItem.m_addInfo;    // local_fb = item+0x02
        int ret = 0;                                   // local_40
        if (targetUser == 0)
        {
            DisJoint* dj = G_CDataManager()->m_disJoint;   // +0x1c 已具名
            ret = dj->GetResult(srcItemIdx, result);
        }
        else
        {
            ret = static_cast<expert_job::CDisjointer*>(expertJob)
                      ->get_disjoint_result(item, &invenItem, result, user);
        }
        if (0 < ret)
        {
            throw (ENUM_ERROR)ret;
        }
        if (region != 0xffff)
        {
            CInventory* invenW = user->getCurCharacInvenW();
            if (invenW->use_item((int)region, 1, 1) != 0)
            {
                throw (ENUM_ERROR)0x16;
            }
            user->SendUpdateItem((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0,
                                 (int)region);
        }

        CItem* foundItems[6];                          // local_90[0..5]
        foundItems[0] = 0;                             // local_90[0]=0（+0x41a）
        int sellPrice0 = 0;                            // 源物品卖价出参（GetSellItemPrice 第 5 参）
        Inven_Item outItems[5];                        // local_26b..，步长 0x3d
        for (int i = 0; i < 5; ++i)
        {
            if (result.m_f[i + 1] == 0)                // -0xb8+(i+1)*4 = result+4*(i+1)
            {
                continue;
            }
            foundItems[i + 1] = G_CDataManager()->find_item(result.m_f[i + 1]);
            if (foundItems[i + 1] == 0)
            {
                continue;
            }
            outItems[i].m_addInfo = result.m_f[i + 1];   // +0x2 ← m_f[i+1]
            outItems[i].set_add_info(result.m_f[i]);     // +0x7 ← m_f[i]
            foundItems[i + 1]->make_item(outItems[i]);   // vptr+0x8 槽
            const int slotResult =
                ((const CInventory*)user->getCurCharacInvenR())
                    ->tryInsertItemIntoInventory(outItems[i]);
            if (slotResult == -1)
            {
                throw (ENUM_ERROR)4;
            }
        }
        // 删除源物品（INVEN_TYPE, slot, 1, eItemDelReason 9, 1）
        const bool deleted = user->getCurCharacInvenW()->delete_item(
            (INVEN_TYPE)invenType, slot, 1, (eItemDelReason)9, true);
        if (!deleted)
        {
            throw (ENUM_ERROR)ret;
        }
        // 源物品卖价统计（sellPrice 用 foundItems[0] 槽位承接，ORIG local_90[0]）
        foundItems[0] = 0;
        G_Store()->GetSellItemPrice(invenItem, item, (short)invenItem.m_addInfo2,
                                    false, sellPrice0);
        GetInstanceValueStatistic()->AddValueStatistic(
            (VALUE_STATISTIC_FIELD)0x13, user, (unsigned int)sellPrice0);
        if (targetUser == 0)
        {
            APSystem::CUserProc::ClearActionAndSendtoUser(
                user, (APSystem::_ActionGroupIndex)7, 0, (ENUM_PACKETCLASS)0);
        }
        else if (user != 0)
        {
            if (user->get_charac_no(-1) == targetUser->get_charac_no(-1))
            {
                APSystem::CUserProc::ClearActionAndSendtoUser(
                    user, (APSystem::_ActionGroupIndex)0x20, 0,
                    (ENUM_PACKETCLASS)0);
            }
            else
            {
                APSystem::CUserProc::ClearActionAndSendtoUser(
                    targetUser, (APSystem::_ActionGroupIndex)0x2d, 0,
                    (ENUM_PACKETCLASS)0);
            }
        }
        Inven_Item unusedPacketTmp;  // ORIG +0x765 构造 -0x136 处 Inven_Item，后续未使用
        packet.put_byte(1);
        packet.put_short(slot);
        packet.put_byte(type);
        int countIndex = packet.get_index();           // local_94
        packet.put_byte(0);
        int outCount = 0;                              // local_34
        for (int i = 0; i < 5; ++i)
        {
            if (foundItems[i + 1] == 0)
            {
                continue;
            }
            if (foundItems[i + 1]->is_stackable() &&
                999 < outItems[i].get_add_info())
            {
                cMyTrace trace(__PRETTY_FUNCTION__, 0x8867, 5);
                trace("DisPatcher_DisJointItem::dispatch_sig, out_item , ITEM #%d, %d",
                      (int)outItems[i].m_addInfo, (int)outItems[i].get_add_info());
            }
            const int newSlot =
                user->getCurCharacInvenW()->insertItemIntoInventory(
                    outItems[i], (eItemAddReason)10, true, false);
            if (newSlot >= 0)
            {
                const Inven_Item* ref =
                    user->getCurCharacInvenR()->GetInvenRef(1, newSlot);
                if (ref != 0 && ref->m_addInfo != (int)outItems[i].m_addInfo)
                {
                    cMyTrace trace2(__PRETTY_FUNCTION__, 0x888a, 0);
                    trace2("ENUM_CMDPACKET_DISJOINT_ITEM error item_id mismatch %d, %d",
                           (int)ref->m_addInfo, (int)outItems[i].m_addInfo);
                }
                packet.put_short(newSlot);
                packet.put_int((int)outItems[i].m_addInfo);
                packet.put_int((int)outItems[i].m_addInfo2);
                // [DpcMisc-P5 负结论 2026-09-11，已回滚] 试过把本处改写为合并正极性
                // `if (targetUser != 0 && targetUser->GetCurCharacExpertJob() != 0)
                //  { collect(FIELD=2); } else { collect(FIELD=0); }`
                // （依据 ORIG 0x081f9c9x 段两个 `je` 同落 FIELD=0 共享尾、FIELD=2 走
                // fall-through）。父泳道 fresh 核账：our **818→822（+4 劣化）**⇒ 4.4.6
                // 对该合并形的物化（`&&` 短路 + setne 汇聚）反而更长；ORIG 的块序由
                // 4.1 的独立条件发射产生，非源码可择。恢复到 `if/else if` 原形。
                if (targetUser == 0)
                {
                    GetInstanceCubeStatistic()->collectCubeStatistics(
                        (int)outItems[i].m_addInfo, (int)outItems[i].m_addInfo2,
                        user, (CUBE_STATISTIC_FIELD)0);
                }
                else if (targetUser->GetCurCharacExpertJob() != 0)
                {
                    GetInstanceCubeStatistic()->collectCubeStatistics(
                        (int)outItems[i].m_addInfo, (int)outItems[i].m_addInfo2,
                        user, (CUBE_STATISTIC_FIELD)2);
                }
                int itemPrice = 0;
                G_Store()->GetSellItemPrice(outItems[i], foundItems[i + 1],
                                            (short)outItems[i].m_addInfo2,
                                            false, itemPrice);
                GetInstanceValueStatistic()->AddValueStatistic(
                    (VALUE_STATISTIC_FIELD)8, user, (unsigned int)itemPrice);
                ++outCount;
            }
        }
        packet.put_byte(countIndex, outCount);
        if (targetUser != 0)
        {
            static_cast<expert_job::CDisjointer*>(expertJob)
                ->complete_disjoint(user, packet);
        }
        packet.finalize(true);
        user->Send(packet);
        HistoryLog::WriteDisJoint(
            reinterpret_cast<FILE*>(user->m_field796f8), invenItem.m_addInfo);
    }
    catch (ENUM_ERROR e)
    {
        packet.put_byte(0);
        packet.put_byte((int)e);
        packet.finalize(true);
        user->Send(packet);
    }
    return 0;
}

// [DpcMisc-D1 2026-09-11 负结论·残差已定位但非单旋钮可修] 本件 our 818 / ORIG 811（+7）。
// 助记符直方图（归一去地址/槽号/reloc 伪影后）：`add $IMM` ORIG 6 → OUR 12（多 6）、
// `mov %R,-MEM` 9→13、`mov -MEM,%R` 55→58、`mov $IMM,-MEM` 14→10、`je` 24→20、
// `jne` 6→9；`mov $IMM,OFF(%R)` 类差值（48/66）经核对为 .o reloc 未解析伪影
// （ORIG `$"str"`+`$&sym` 12+6=18 条 ↔ OUR 18 条）**不计**。
// 根因 = 帧/局部变量集合不同导致的连锁寻址差：
// · 帧 ORIG `add $0x2bc,%esp` vs OUR `add $0x2dc,%esp`（+0x20）。
// · 槽集比对（slots.py）无单一 delta 可对齐：only-ORIG {-53,-68,-112,-116,-120,-136,
//   -140,-144,-184,-188,-242,-247,-249,-310,-615,-620}；only-OUR {-56,-65,-72,-76,-80,
//   -96,-152,-168,-208,-262,-267,-269,-330,-635,-652} ⇒ 布局是分布式错位。
// · foundItems 索引形：ORIG 0x081f96f6/0x081f970e/0x081f9748 `mov -0xb8(%ebp,%eax,4)`
//   且 0x081f96f0 `add $0x4,%eax`（锚点 base=-0xb8 + 索引 i+4）；我方 `-0xd0(%ebp,%eax,4)`
//   + `add $0x1,%eax`（索引 i+1）。两者在各自帧内**语义等价**（anchored base 不同），
//   随帧布局连锁，非源码索引字面量错（源码 `foundItems[i+1]` 与 OUR 自洽）。
// ⇒ 需先逐变量对齐局部（P28/P331 声明序=槽序）再谈索引锚点；本轮不盲改（改动会连带
//   数十条寻址指令）。
// p167 实测：ORIG calls=98 / OUR calls=98；差集**全部**为 C1/C2·D1/D2 同址别名
// （MISSING C2 ×3 D2 ×2 ... ↔ EXTRA C1 ×3 D1 ×2 ...：Inven_Item/cMyTrace/PacketGuard/
// DisJointResult）⇒ 判据库第 8 条不计，无语义缺失。
