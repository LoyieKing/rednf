// ORIG 0x084e05c2 T dispatch_sig —— 村庄攻击奖励结束：组 0x1ec 包全网广播、清理复仇副本与角色状态。
// 报告 docs/class_func_reports/Inter_VillageAttackedRewardServer/dispatch_sig.md 全文转写。
// [b28d-lane2] verdict=DIFF ndiff=119（ORIG 0x084e05c2）：槽序/帧差(N82/P212/P221)+混合残差；gap+7。
// [b28d-solo-InterDispatcher] verdict=DIFF（ORIG 0x84e05c2）：本轮补 P253 memset(10) 显式形 + P28 后置 it++（p167 CLEAR）；余 N82/P112 帧槽平移。
int Inter_VillageAttackedRewardServer::dispatch_sig(CUser* /*user*/, char* /*data*/, int /*len*/)
{
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
        return 0;
    // ORIG 读 env+0x1a8 = Server_Envir(+0x198)+0x10 = m_serverEnvir.m_serverType
    if (G_CEnvironment()->m_serverEnvir.m_serverType == 1 ||
        G_CEnvironment()->m_serverEnvir.m_serverType == 2)
        return 0;

    // ORIG `call memset@plt`（Ghidra C `memset(local_26,0,10)`）⇒ 显式 memset 形（P253/P214③）。
    char local_26[10];
    memset(local_26, 0, 10);
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x1ec);

    const int local_14 =
        GlobalData::s_villageMonsterMgr->GetRewardType();   // ORIG _ZN16village_attacked18CVillageMonsterMgr13GetRewardTypeEv
    // [P5·2026-09-11] P251/P261 真 switch：ORIG 084e072x `mov -0x10(%ebp),%eax; cmp $0x1,%eax;
    //   je …; cmp $0x2,%eax; je …; jmp …`（**单次加载 + 寄存器 cmp 链**）；if/else-if 令 4.4
    //   产 `cmpl $0x1,-0x10(%ebp); jne …`（内存比较、逐次重读，无加载）。探针
    //   /tmp/P5/sw.cpp 四形实测：f3（本 switch 形）逐条产 `mov -0xc(%ebp),%eax; cmp $0x1,%eax;
    //   je; cmp $0x2,%eax; je; jmp` = ORIG 同形（f1/f2/f4 均产内存比较形）。
    switch (local_14)
    {
    case 1:
        local_26[0] = '\0';
        break;
    case 2:
        local_26[0] = '\x01';
        break;
    }
    local_26[1] = 2;
    packet.put_binary(local_26, 10);
    packet.finalize(true);

    GlobalData::s_revengeDungeonMgr->CloseRevengeDungeon();
    GlobalData::s_revengeDungeonMgr->SetCloseTime(0);

    for (std::map<unsigned short, CUser*>::iterator it =
             G_GameWorld()->m_UserInWorld.begin();
         it != G_GameWorld()->m_UserInWorld.end(); it++)   // P28：ORIG 增量段 ppEi ⇒ 后置++
    {
        CUser* local_10 = it->second;
        if (local_10 != 0 && local_10->get_state() > 2)
        {
            local_10->SetCurCharacRevengeData('\0', '\0');
            local_10->VillageAttack_NotDBUpdate();
            local_10->Send(packet);
        }
    }

    GlobalData::s_villageMonsterMgr->OnEndRewardVillageMonster();
    return 0;
}