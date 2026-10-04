// bool CUser::_check_level_up(int exp, int& sp, int& sfp, eExpAddReason, int)
//
// ORIG 参数语义（报告反编译 param_2..param_5 + 汇编实证）：
//   expAdd    — 本次待结算经验增量；
//   sp/sfp    — 升级时按 GetSpAtLevelUp(next)/>49 返还的输出引用；
//   reason    — eExpAddReason（第 4 参，0x18(%ebp)）；ORIG 两处
//               `cmpl $0x0,0x18(%ebp)`（+0x3e6 升级路径 / +0x5d9 未升级路径）
//               ⇒ 非 0 时向 XNuclear::CHades::ExpUp 上报经验（原码即用它作标志）；
//   hadesFlag — 第 5 参（0x1c(%ebp)）；ORIG 全函数 0 次引用，未读取。
// [SweepCUserCheckLevel 2026-09-09] 逐条对齐结论（check_tu_routed fresh .o 实证）：
//   ① 真 bug-1：循环为 while 前置条件（ORIG +0x087 je 落点 +0x600 条件块）；
//      原 for(;;)+尾部 break 至少多执行一次循环体（可观察差异）。已改 while。
//   ② 真 bug-2：Hades 上报标志用第 4 参 eExpAddReason（0x18(%ebp)，ORIG 两处
//      cmpl $0x0,0x18(%ebp)），第 5 参 0x1c(%ebp) ORIG 全函数 0 次引用。已改。
//   ③ P0 具名局部双向修正：补 goldBonus（ORIG -0x1c 槽往返）；删 inven/proxy/
//      open 三个多余具名局部（ORIG 结果寄存器直传，无槽）。
//   ④ P221 扩展：4 个 cMyTrace 改全表达式临时链 ⇒ 槽 -0x70/-0x60/-0x50/-0x40
//      与 ORIG 逐槽一致（具名形 4.4 恒反向浅→深）。
//   ⑤ bottom/top 声明提至函数头 ⇒ 槽 -0x2c/-0x28 与 ORIG 一致。
//   ⑥ if/else 正极性翻转（达线{升级}else{未升级}）⇒ 复现 ORIG 尾部未升级块
//      布局 + setge 条件码。
//   ⑦ 结果：帧 push edi/esi/ebx + sub $0xac、全部具名局部槽号、442 条指令与
//      ORIG 全等；check_tu_routed 判本函数 = AE（strict 差仅数据符号地址，N132）。
//   ⑧ 调用点 73=73；p167 仅 C1/C2 同址别名伪差（nm 实证同址，P93 豁免）⇒ CLEAR。
//   ⑨ 虚槽核对：ORIG 区间 `call *` = 0 处（全直接调用），无虚槽可错。
//   ⑩ jcc 逐位交叉验证 32 处：条件码/落点语义全等。
//   剩余真缺口 = 0。
// [R13fixE2 E轴 prett] ORIG 0x08cf2f00 = "…(int, int&, int&, eExpAddReason, int)"
// ⇒ 定义处用全局限定 `::eExpAddReason`（同类型；gain_exp_sp 同法先例）。
bool CUser::_check_level_up(int expAdd, int& sp, int& sfp,
                            ::eExpAddReason reason, int hadesFlag)
{
    (void)hadesFlag;   // ORIG 全函数不引用 0x1c(%ebp)（第 5 参）

    // ORIG 0x08662b01：先算 nextLevel（-0x30）再初始化 leveledUp（-0x21）⇒
    // 原码声明序 = nextLevel 在前（P0/P28 槽-局部律）。
    int nextLevel = get_charac_level() + 1;                   // -0x30 目标级
    int bottom;                                               // -0x2c 经验下界
    int top;                                                  // -0x28 经验上界
    bool leveledUp = false;                                   // -0x21 标志位

    if (getCurCharacR() == 0)                                 // +0x02c 空角色守卫
    {
        // ORIG trace 槽序 -0x70/-0x60/-0x50/-0x40（深→浅）⇒ 全表达式临时链
        // （P221 扩展：具名形 4.4 恒浅→深，临时链复现 ORIG 深→浅）。
        cMyTrace(__PRETTY_FUNCTION__, 0x401b, 5)(
            "CUser::_check_level_up m_selected is NULL");
        return false;
    }
    if (get_charac_level() == 0)                              // +0x08d 零级守卫
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x4022, 5)(
            "CUser::_check_level_up m_selected->m_level is 0");
        return false;
    }

    // ORIG +0x087 的 je 落点是 +0x600（循环条件块，物理位于函数尾部）：
    // 首次进入前先判条件 ⇒ 原码是 while 前置条件，而非 for(;;)+尾部 break
    // （后者至少多执行一次循环体 = 真语义差异）。
    while (expAdd > 0 && nextLevel <= 0x55)                   // 循环头 +0x0cd
    {
        // 经验区间：bottom=当前级经验下界，top=目标级经验上界。
        bottom = G_CDataManager()->get_level_exp(get_charac_level());  // -0x2c
        top = G_CDataManager()->get_level_exp(nextLevel);              // -0x28

        // ORIG：区间不一致条件对 get_charac_exp 各重调一次，trace 内再次重调
        // （共 3 次，缓存违反 P183/P48）。
        if (get_charac_exp() < bottom || get_charac_exp() > top)   // +0x110 区间不一致
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x403b, 5)(
                "%s is Level(%d) and Exp(%d) Inconsistency(bottom(%d), top(%d))",
                getCurCharacName(), get_charac_level(), get_charac_exp(), bottom, top);
            setCurCharacExp(bottom);                          // +0x1bd 钳回下界
        }

        if (top - bottom <= 0)                                // +0x1ca 表异常出口
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x4046, 5)(
                "CUser::_check_level_up, LEVEL:%d TOP:%d BOTTOM:%d",
                nextLevel, top, bottom);
            return false;
        }

        // ORIG +0x232 用正极性 `setge` + je 跳到尾部的未升级块（+0x5c7），
        // 升级块直落 +0x243 ⇒ 原码写 if(达线){升级}else{未升级}（P214①/P222）。
        if (get_charac_exp() + expAdd >= top)                 // +0x232 达升级线
        {
            sp += (int)G_CDataManager()->GetSpAtLevelUp(nextLevel);   // SP 返还
            if (nextLevel > 0x31)                             // >49 级 SFP+1
                sfp += 1;

            int overflow = top - get_charac_exp();            // -0x20 溢出经验
            ++nextLevel;                                      // -0x30++
            incCurCharacLevel();                              // +0x298
            expAdd += get_charac_exp() - top;                 // 剩余增量回写

            // >=10 级且未跳过新手引导 → 置跳过标记。
            if (get_charac_level() >= 10 && GetTutorialSkipable() == 0)
                UpdateTutorialSkipable();                     // +0x3b6d

            setCurCharacExp(top);                             // +0x2f5 经验写回 top
            _update_charac_stat(true);                        // 重算角色属性
            leveledUp = true;                                 // +0x30d

            // 升级日志（FILE* 句柄存于 m_historyLogFp，int 位宽与 ORIG 一致）。
            HistoryLog::WriteLevelUp(reinterpret_cast<FILE*>(m_historyLogFp),
                                     getCurCharacExp());

            // 金币加成：GetGoldBonus(level)!=0 时入账并刷新物品列表。
            // ORIG +0x34c 把结果落 -0x1c 槽再 cmpl ⇒ 原码有具名局部（P0）；
            // get_charac_level 在条件与入账各重调一次，禁缓存（P183/P48）。
            int goldBonus = m_premium.GetGoldBonus(get_charac_level());
            if (goldBonus != 0)                               // +0x358 判定
            {
                // ORIG +0x382：getCurCharacInvenW 结果 %eax 直传 this，
                // 无栈槽往返 ⇒ 原码是链式单表达式（P0/P183/P52）。
                getCurCharacInvenW()->gain_money(
                    m_premium.GetGoldBonus(get_charac_level()),  // +0x3a6
                    static_cast<eMoneyAddReason>(0xd), true, 0);
                SendUpdateItemList(eSendTarget_1, ENUM_ITEMSPACE_0, 0);
            }
            m_premium.RecalcAdditionalInfo(this);             // +0x3e1

            if (reason != 0)                                  // +0x3e6（0x18(%ebp)，上报溢出量）
                ((XNuclear::CHades*)getHades())->ExpUp((unsigned int)overflow);

            m_historyLog.LevelUp(get_charac_level(), eLevelUpReason_0);

            if (get_charac_level() > 0x3b)                    // >59 级维度进出刷新
                DimensionInoutUpdate(true, true);

            // 满 85 级且非 GM → 向 monitor 广播满级公告（包长 0x28）。
            if (get_charac_level() == 0x55 &&
                !GlobalData::s_pGMAccounts_->isGM(get_acc_id()))
            {
                Packet_Monitor_Max_Level_BroadCast pkt;       // +0x4ab ctor
                strncpy(pkt.m_name, getCurCharacName(), 0x1e);  // 名字 @+0xa
                // ORIG +0x4d8：GetServerProxy 结果 %eax 直传 SendPacket this，
                // 无槽往返；pkt 地址缓存 %ebx 跨两次调用（P0/P183/P52）。
                GlobalData::s_monitor_proxy_mgr->GetServerProxy(
                    GetServerGroup())->SendPacket(
                        reinterpret_cast<char*>(&pkt), 0x28);
            }

            // 达到地下城最低进图等级 → 按 ServerParameter 开关广播进出消息。
            if (get_charac_level() ==
                G_GameWorld()->getDungeonMinimumRequiredLevel(0x2aff))
            {
                // ORIG +0x53a：isDungeonOpen 结果留在 %al 直测（无槽往返）
                // ⇒ 原码无具名 bool 局部（P0）；真假两分支各自重调 G_GameWorld
                // 并独立 send_user_dungeon_inout_message（禁缓存，P183/P48）。
                if (G_CDataManager()->m_serverParameter.isDungeonOpen(0x2aff))
                    G_GameWorld()->send_user_dungeon_inout_message(this, 0x2aff,
                                                                   true);
                else
                    G_GameWorld()->send_user_dungeon_inout_message(this, 0x2aff,
                                                                   false);
            }

            makeGuildLevelUpMessage(get_charac_level());      // 公会升级公告
            _onLevelUp();                                     // ORIG T 0x866311a
        }
        else                                                  // 未升级路径 +0x5c7
        {
            addCurCharacExp(expAdd);                          // +0x5d4
            if (reason != 0)                                  // +0x5dd（0x18(%ebp)）
                ((XNuclear::CHades*)getHades())->ExpUp((unsigned int)expAdd);
            expAdd = 0;                                       // +0x5f9
        }

    }
    return leveledUp;
}