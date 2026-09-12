// ORIG 0x084e05c2 T dispatch_sig —— 村庄攻击奖励结束：组 0x1ec 包全网广播、清理复仇副本与角色状态。
// 报告 docs/class_func_reports/Inter_VillageAttackedRewardServer/dispatch_sig.md 全文转写。
// [b28d-lane2] verdict=DIFF ndiff=119（ORIG 0x084e05c2）：槽序/帧差(N82/P212/P221)+混合残差；gap+7。
// [b28d-solo-InterDispatcher] verdict=DIFF（ORIG 0x84e05c2）：本轮补 P253 memset(10) 显式形 + P28 后置 it++（p167 CLEAR）；余 N82/P112 帧槽平移。
// [SemA17 2026-09-12] verdict=DIFF **nd .o 口径 9→2**、ORIG/NEW=156/157（gap −1）、
//   p167 MISSING 空；TU 四数（InterDispatcher.cpp）改前=改后 identical=135 ae=1400
//   near=4 diff=82 missing=703、逐件 verdict 零差。本件两处落地：
//   1 **真语义差修正**：载荷两个 int 字段（+0x02/+0x06）ORIG 显式再清零
//     （0x084e0698/0x084e069f 实证）⇒ `char[10]` 改 pack(1) 具名视图
//     `stVillageRewardReq_t{m_type,m_sub,m_value,m_extra}`（sizeof=10）并补
//     `m_value=0; m_extra=0;`（裸 char[] 无法折叠出单条 disp 清零，P19/P80/P82）。
//   2 **P222 正向嵌套包裹**：`a!=0 && a->get_state()>2` → `if(a!=0){if(a->get_state()>2){}}`
//     ⇒ 4.4 由 `jle+mov $1/jmp/mov $0` 转为 ORIG 的 `setg %al; test %al,%al; je`
//     （探针 probe/p_cond.cpp f5 与 ORIG 逐条同形；f1=`&&` 反形、f2/f3=拆分守卫反形）。
//   残余 2 行 = `put_binary((char*)&local_26,10)` 实参装载序（ORIG 先 lea 后常量，
//   4.4 恒反序）⇒ P75/N23 跨版本调度，封顶。
int Inter_VillageAttackedRewardServer::dispatch_sig(CUser* /*user*/, char* /*data*/, int /*len*/)
{
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
        return 0;
    // ORIG 读 env+0x1a8 = Server_Envir(+0x198)+0x10 = m_serverEnvir.m_serverType
    if (G_CEnvironment()->m_serverEnvir.m_serverType == 1 ||
        G_CEnvironment()->m_serverEnvir.m_serverType == 2)
        return 0;

    // ORIG `call memset@plt`（Ghidra C `memset(local_26,0,10)`）⇒ 显式 memset 形（P253/P214③）。
    // [SemA17 2026-09-12 · P19/P80 线格式具名视图] ORIG 0x084e0698/0x084e069f 在
    //   `movb $0x2,-0x21` 之后**另有** `movl $0x0,-0x20(%ebp)` + `movl $0x0,-0x1c(%ebp)`
    //   ——即 10 字节载荷的两个 int 字段（+0x02/+0x06）在 memset 之后被显式再清零。
    //   裸 `char[10]` 无法让 4.4 折叠出单条 disp 清零（会生成 `lea+movl $0`两段），
    //   故载荷改为 pack(1) 具名视图：`m_value/m_extra` 直写即产 ORIG 两条单 disp store。
    //   载荷语义：m_type(+0)/m_sub(+1)=2/m_value(+2)/m_extra(+6)，共 10 字节（size 由
    //   ORIG `put_binary(...,0xa)` 与 memset 长度 0xa 双证）。
    struct stVillageRewardReq_t
    {
        char m_type;   // +0x00
        char m_sub;    // +0x01
        int m_value;   // +0x02
        int m_extra;   // +0x06
    } __attribute__((packed));
    stVillageRewardReq_t local_26;
    memset(&local_26, 0, 10);
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
        local_26.m_type = '\0';
        break;
    case 2:
        local_26.m_type = '\x01';
        break;
    }
    local_26.m_sub = 2;
    local_26.m_value = 0;   // ORIG 0x084e0698 显式再清零（memset 后幂等）
    local_26.m_extra = 0;   // ORIG 0x084e069f 显式再清零（memset 后幂等）
    packet.put_binary((char*)&local_26, 10);
    packet.finalize(true);

    GlobalData::s_revengeDungeonMgr->CloseRevengeDungeon();
    GlobalData::s_revengeDungeonMgr->SetCloseTime(0);

    for (std::map<unsigned short, CUser*>::iterator it =
             G_GameWorld()->m_UserInWorld.begin();
         it != G_GameWorld()->m_UserInWorld.end(); it++)   // P28：ORIG 增量段 ppEi ⇒ 后置++
    {
        CUser* local_10 = it->second;
        // [SemA17 2026-09-12 · P222 正向嵌套包裹] ORIG 0x084e077x 段为
        //   `mov -0xc(%ebp),%eax; cmpl $0x0,-0xc(%ebp); je <增量>; …call get_state;
        //    cmp $0x2,%eax; setg %al; test %al,%al; je <增量>`
        //   ——`&&` 单条件在 4.4 产 `jle + mov $1/jmp/mov $0` 两臂 0/1 物化（非 setcc）；
        //   改**嵌套正向 if** 后 4.4 直出 `setg %al; test %al,%al; je`（探针三形实测：
        //   /tmp/lane_diff/SemA17/probe/p_cond.cpp f5 与 ORIG 逐条同形；f1=`&&` 反形）。
        //   语义逐字等价（两条件仍为短路合取、求值序不变）。
        if (local_10 != 0)
        {
            if (local_10->get_state() > 2)
            {
                local_10->SetCurCharacRevengeData('\0', '\0');
                local_10->VillageAttack_NotDBUpdate();
                local_10->Send(packet);
            }
        }
    }

    GlobalData::s_villageMonsterMgr->OnEndRewardVillageMonster();
    return 0;
}