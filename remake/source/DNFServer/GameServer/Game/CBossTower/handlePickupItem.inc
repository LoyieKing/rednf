// ORIG T 0x08143d2c（新增虚槽：char 按值版，与 CBossStage 的 char& 版并存）：
//   错误路径条件 = (m_party == 0) || IsRoutingState()（负极性短路物化岛
//   mov$1/jmp/mov$0 + test；m_party==0 时 `je` 直达 mov$1 岛、**不**调
//   IsRoutingState）→ SendCmdErrorPacket(0x2e,0xd8)，返回 0；经由
//   CBossStage::handlePickupItem(char& out_rarity) 拾取失败 → 0；
//   server_rarity != client 参数时 trace（ORIG __PRETTY_FUNCTION__ 文本，
//   600,5）；user 非空且 rarity==3 时 m_dungeonLog.IncrementBossPickUpItem
//   (*user, (uchar)m_stageCur, (uchar)m_difficulty)；返回 1。
// [b28f-SweepCBossTower] ①**真语义 bug**：旧条件 `m_party != 0 &&
//   IsRoutingState()` 把 m_party==0 归入正常路径（ORIG 0x08143d46
//   `je 0x08143d5a(mov $1)` ⇒ m_party==0 走错误路径）⇒ 改
//   `m_party == 0 || m_party->IsRoutingState()`。
//   ②CBossStage.h:54 + CBossStage.cpp:179 handlePickupItem int→bool（P248 档①：
//   ORIG 0x08143dbb `call; mov %al,-0x9` 无 test/setne ⇒ 单字节返回；mangled
//   不含返回类型，vtable 槽序/sizeof 零影响；CBossStage.cpp 四数改前/改后
//   596/2/7/134 全等）⇒ 调用点 test/setne 2 条消除，DIFF→NEAR（82:82）。
//   残余 = serverRarity 槽 -0xa vs ORIG -0x1d（N82附：4.4 聚合对象优先占低槽、
//   标量分区排布——trace 对象占 -0x1c..-0xb 之后；声明序反转试形无效）。
// [L5Free49to60 2026-09-11] **NEAR→AE（82:82，gap 0，bcmp NDIFF 15→0）**：P221 全表达式
//   临时链 `cMyTrace(...)(...)` 替换具名 trace 对象后复现 ORIG 槽方向（trace 浅、
//   serverRarity 深至 -0x1d）——证明该残差是「具名类对象 vs 临时链」槽方向旋钮，
//   非同族 N82附 不可控项；凡 cMyTrace 与标量/数组互斥槽序的 NEAR 件可重试此形。
bool CBossTower::handlePickupItem(CUser* user, int a, bool b, char c)
// [P8S2 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
// [P8S2 2026-09-11 负结论·P139] ORIG prett=0x4d `virtual bool WongWork::CBossTower::handlePickupItem(CUser*, int, bool, char)`（0x08b701a0，REF 见 ORIG 0x08143dec `movl $0x8b701a0,0x4(%esp)`）VS 我方手写串保留：CBossTower.h:59 声明为 `virtual int` ⇒ 换 __PRETTY_FUNCTION__ 会发射 `virtual int …`（size 不符）；修法=CBossTower.h 返回类型 int→bool（mangled 不变），属跨 TU 头改动 ⇒ 只报告不改（本泳道无 .h 写权）。
// [Q6HeaderTypeAxis 2026-09-11 已落地] 上述负结论的修法已完成：CBossTower.h:59
//   返回类型 int→bool（Q6，头轴）+ 本行定义同步；CBossTower.cpp 内 cMyTrace 的
//   手写串同步换回 `__PRETTY_FUNCTION__` ⇒ 发射 `_ZZN8WongWork10CBossTower16
//   handlePickupItemEP5CUseribcE19__PRETTY_FUNCTION__`（size 0x4d 与 ORIG 对齐）。
{
    // [b28d-solo-CBossTower 真语义 bug 修正] ORIG 0x08143d3e..0x08143d68：
    //   m_party==0 → `je` 直达 0x08143d5a `mov $1,%eax`（**不**调 IsRoutingState）；
    //   m_party!=0 时 `call IsRoutingState; test %al,%al; je <mov $0 岛>`；
    //   物化岛 mov$1/jmp/mov$0 + `test %al,%al; je <正常路径>` ⇒ 错误路径条件 =
    //   `(m_party==0) || IsRoutingState()`（负极性短路，P214①）。旧重建写
    //   `m_party != 0 && IsRoutingState()` —— m_party==0 时误入正常路径（真 bug）。
    if (m_party == 0 || m_party->IsRoutingState())
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x2e), 0xd8);
        return 0;
    }

    // ORIG 0x08143d8f `movb $0x0,-0x1d` / 0x08143dbb `mov %al,-0x9`（**无**
    // test/setne ⇒ callee 单字节返回，P248 档①）。ORIG 槽序：serverRarity
    // -0x1d（深）在前、picked -0x9（浅）在后（4.1 先声明更深）。
    bool picked;             // ORIG -0x9 槽
    char serverRarity = 0;   // ebp-0x1d
    picked = CBossStage::handlePickupItem(user, a, b, serverRarity);
    if (!picked)
        return 0;

    if (serverRarity != c)
    {
        // [L5Free49to60 P221 试形] 具名 cMyTrace 对象在 4.4 占低槽（-0x1c 区），
        // ORIG 该对象在 -0x1c..-0xb、serverRarity 深至 -0x1d ⇒ 改全表达式临时链
        // 复现 ORIG 槽方向（P37/P221）。
        // [Q6HeaderTypeAxis 2026-09-11] 手写串 → __PRETTY_FUNCTION__（头/定义已 bool 化，
        //   prett 产物逐字节 = ORIG `virtual bool WongWork::CBossTower::handlePickupItem
        //   (CUser*, int, bool, char)`，size 0x4d @ORIG 0x08b701a0）。
        cMyTrace(__PRETTY_FUNCTION__, 600, 5)("server_item_rarity != client_item_rarity");
    }

    if (user != 0 && c == 3)
    {
        m_dungeonLog.IncrementBossPickUpItem(
            *user,
            static_cast<unsigned char>(m_stageCur),
            static_cast<unsigned char>(m_difficulty));
    }
    return 1;
}