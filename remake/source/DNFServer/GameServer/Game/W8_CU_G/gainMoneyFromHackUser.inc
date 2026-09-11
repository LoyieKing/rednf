// ORIG 0x0867fbba：已 loading 时累加并按规则判定惩罚入库；否则置 loading 标志
// 并记首条未加载流水（DB_AutoPunishedSecondUserData::makeRequest(accId,gold,uid,who)）。
// b28d-solo-W8_CU_G: DIFF/N7 封顶（100/101）— 本波修 5 处：
// ①真 ABI bug：两个 DB 请求类局部声明非静态（多压临时对象 this，实参整体右移 4B）
//   ⇒ 改 `static void`（权威声明 W7LinkDB.cpp:28/44、CUserOnlyOrig.cpp:93；ORIG
//   调用点 0x0867fc32/0x0867fcfb 的 (%esp) 为第 1 实参 get_acc_id()，P88）；
// ②真语义 bug：末尾 DB_InsertAutoPunishSecondUser::makeRequest 第 3 参原写 totalNow，
//   ORIG 0x0867fce6 读 -0x10/-0xc = 形参 gold ⇒ 改传 gold；
// ③isLoadingHackGold uchar→bool（P292，Main 代落 CUser.h:2695/CUser.cpp:5017）⇒
//   首条件 `xor $1; test; je` 复现 0x0867fbe5；
// ④去 totalNow/cntNow/punish8 具名局部（ORIG 全程 ebx:esi/eax 寄存器直传，P0）⇒
//   帧 0x54→0x30，spill 槽 -0x10/-0xc/-0x18/-0x14 与 ORIG 全等；
// ⑤`gold + totalGold` 交换书写序复现 ORIG 0x0867fc3f 先载 totalGold 入 eax:edx。
// 余 1 条：this=全局指针 g_autoPunishRuleHackTypeMgr 装载 ORIG `mov g,%edx` 直落
// vs 4.4 `mov %eax,%edx; mov g,%eax`（N7 寄存器角色互换）；b28d-W8b 复测去
// `static_cast<unsigned int>`（隐式 int→unsigned 实参旋钮）同 101 ⇒ 封 N。
void CUser::gainMoneyFromHackUser(unsigned int who, unsigned long long gold,
                                  unsigned int cnt, unsigned long long totalGold)
{
    if (!isLoadingHackGold())
    {
        setLoadHackGold(true);
        DB_AutoPunishedSecondUserData::makeRequest(get_acc_id(), gold, GetUID(), who);
        return;
    }

    // b28d-solo-W8_CU_G：ORIG 0x0867fc3f 先载 totalGold(-0x18/-0x14) 入 eax:edx、
    // 后载 gold 入 ecx:ebx ⇒ 4.4 先求右操作数，写 `gold + totalGold`（加法交换律）。
    incTotalHackUserTradeGold(gold + totalGold);
    incTotalHackUserTradeCnt(cnt);

    // b28d-solo-W8_CU_G：ORIG 0x0867fc7f/0x0867fc8e 两 getter 结果全程寄存器
    //（ebx:esi / eax，无槽往返，P0）⇒ 内联实参，勿声明 totalNow/cntNow 具名局部。
    if (GlobalData::g_autoPunishRuleHackTypeMgr->checkTotalTradeRuleOver(
            static_cast<unsigned int>(getTotalHackUserTradeCnt()),
            getTotalHackUserTradeGold()))
    {
        SetTradePunishType(8);
    }
    // b28d-solo-W8_CU_G 真语义修正：ORIG 0x0867fce6 末尾 makeRequest 第 3 参读
    // -0x10/-0xc（形参 gold 槽），**非** getTotalHackUserTradeGold() 结果
    //（该值仅用于 checkTotalTradeRuleOver，ebx:esi 之后即弃）。原码传 totalNow
    // 属参数传错，已改正为 gold。
    DB_InsertAutoPunishSecondUser::makeRequest(
        get_acc_id(), who, gold, isTradePunishType(8));
}