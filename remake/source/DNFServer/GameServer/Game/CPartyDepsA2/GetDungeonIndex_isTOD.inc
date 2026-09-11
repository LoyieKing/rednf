// ============================================================================
// GetDungeonIndex_isTOD —— ORIG T 0x085bacc0（报告 CParty/GetDungeonIndex_isTOD.md）
// [b28d-solo-CPartyDepsA2 判词·AE] 照抄写后未读 bool 局部（P171/P184）+ 删
// CDungeon* target 具名局部（P0）+ 删显式 cast（P211 LHS-first store）+
// cMyTrace/cMyTraceNoop 全表达式临时链（P221）+ Get_DungeonIndexByLayer 返回
// 类型 unsigned→int（.h 已 R2 广播）。
// ============================================================================
int CParty::GetDungeonIndex_isTOD(int& dungeonIdx, int& apcIdx)
{
    // [b28d-solo-CPartyDepsA2] ORIG 0x085bacca 函数头 `movb $0x0,-0x19`、
    // 0x085bad2a 命中后 `movb $0x1,-0x19`（Ghidra local_1d 只写不读）⇒ 原码
    // 保留了写后未读的 bool 局部（P171/P184 照抄，勿删）。
    bool todFlag = false;
    TowerOfDespairMgr* tod =
        reinterpret_cast<TowerOfDespairMgr*>(GlobalData::s_pTowerOfDespairMgr);
    if (tod == 0)
    {
        // [b28d-solo-CPartyDepsA2] ORIG 0x085bace1 的 cMyTraceNoop 槽在 -0x2d
        // （cMyTrace -0x2c 之前，先声明更深）；具名对象 4.4 反产 -0x1a ⇒
        // P221 全表达式临时链翻 sibling 块槽向。
        cMyTraceNoop()("Error : DungeonIndex : %d", dungeonIdx);
        return 0x13;
    }

    if (tod->GetBaseLayerIndex() == dungeonIdx)
    {
        todFlag = true;
        CUser* manager = m_manager;   // ORIG this+0x74
        if (manager == 0)
        {
            // [b28d-solo-CPartyDepsA2] ORIG 0x085bad55 cMyTrace 槽 -0x2c、
            // cMyTraceNoop -0x2d、todayLayer -0x30 均在 msg 之后深侧；
            // 4.4 把具名对象反放浅侧 ⇒ P221 全表达式临时链（两 trace 同形）。
            cMyTrace(__PRETTY_FUNCTION__, 0x41bb, 5)("TOD no Dungeon Manager!");
            return 0x13;
        }

        TOD_UserState* state = reinterpret_cast<TOD_UserState*>(
            manager->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xd));
        TOD_Layer todayLayer = state->getTodayEnterLayer();

        // [b28d-solo-CPartyDepsA2] ORIG 0x085badf6 段 `mov 0xc(%ebp),%edx;
        // mov %eax,(%edx)`（LHS-first 直存）⇒ 原码无显式 cast（P211：显式
        // static_cast<int> 使 RHS 异型 ⇒ 4.4 反产 RHS-first + edx 中转）。
        dungeonIdx = tod->Get_DungeonIndexByLayer(todayLayer.GetLayer());

        // [b28d-solo-CPartyDepsA2] ORIG 0x085badc5 段：find_dungeon 结果经
        // %eax 直传 CheckEnterable 第三实参，无 CDungeon* target 具名局部
        // （P0 槽-局部律：我们的 -0x10 槽即该多出的局部）。
        int err = tod->CheckEnterable(manager,
                                      G_CDataManager()->find_dungeon(dungeonIdx));
        if (err != 0)
        {
            return err;
        }

        apcIdx = tod->Get_APCIndexByLayer(todayLayer.GetLayer());

        char msg[256];
        sprintf(msg, "Enter Layer : %d",
                static_cast<int>(todayLayer.GetLayer()));
        tod->SendMessageToCUser(manager, msg);
    }
    return 0;
}