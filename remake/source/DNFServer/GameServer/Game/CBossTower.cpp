// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CBossTower/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ WongWork::CBossTower 实现（CBossTowerRetry 批次）。
// 全部函数按 docs/class_func_reports/WongWork__CBossTower/ 对应 md 的
// 汇编 + 反编译 C 转写。裸偏移已字段化：
//   基类段（CBossStage，protected）：+0x9e8→m_startTick、+0x9ec→m_endTick、
//     +0x9f0→m_dungeon、+0xa00/+0xa14→m_stageData.m_monsterMap(_M_node_count)、
//     +0xae0→m_stageCur、+0xae4→m_stageMax、+0xae8→m_difficulty；
//   CBossTower 自有段：+0xaf4→m_towerStartTick、+0xaf8→m_towerEndTick、
//     +0xafc→m_prepareFinishFlag、+0xb00→m_dungeonLog；
//   跨类：party+0xcd8→CParty::m_battleField.m_bloodState、user+0x79700→CUser::m_historyLog、
//     manager+0x796cc→CUser::m_gmBossDifficulty。
// getIdx 0x082aa728 读 +0xaf0；setIdx 0x082aa716 写 +0xaf0。
// 虚表 _ZTVN8WongWork10CBossTowerE@0x08b70260 共 11 槽（vptr 相对偏移：
// +0x08=_checkMemberDie、+0x0c=checkAllMemberReady、+0x14=handleLeaveUser）。
#include "CBossTower.h"

#include <algorithm>   // std::min（generateRewardItem 上限链）
#include <cstring>

#include "CParty.h"     // get_user/get_member_count/getMemberNames/m_battleField.m_bloodState(+0xcd8)
#include "CUser.h"      // SendCmdErrorPacket/m_historyLog(+0x79700)/m_gmBossDifficulty
#include "QuickParty.h" // 先于 CDungeon.h：QUICKPARTY_H 已定义则跳过其镜像块
#include "CDungeon.h"   // GetDungeonName/getRandomTowerMaxStage/get_min_level 等
#include "CInventory.h" // gain_money/insertItemIntoInventory/MakeItemPacket/
                        // GetInvenTypeFromItemSpace/eMoneyAddReason/eItemAddReason
#include "CEquipItem.h" // get_endurance（InsertRewardItems 邮件分支）
#include "CGenUniqueNo.h"            // WongWork::CGenUniqueNo::genIPGNo
#include "CMailBoxHelper.h"          // ReqDBSendNewSystemMail/ReqDBSendNewAvatarMail
#include "RDARScriptStringManager.h" // g_scriptStringManager_（邮件文案）

#include "GlobalData.h"      // s_systemTime_
#include "CSystemTime.h"     // getCurTickCount / getCurSec
#include "TimerQueue.h"      // G_TimerQueue / TimerEntry::OBJ_TYPE / TIMER_MESSAGE
#include "CGameManager.h"    // G_CGameManager()->returnBossTower
#include "cHistoryTrace.h"   // 权威声明（原局部最小声明已删除）
#include "PacketGuard.h"        // PacketGuard（回包构造）
#include "CDungeonEntranceLog.h" // GetInstanceDungeonEntranceLog / IncrementDungeonEntrance
#include "CDataManager.h"       // find_dungeon/find_item/get_mob_reward/GetTowerScript
#include "GameTypes.h"          // cMyTrace


namespace WongWork
{
// [b28f-SweepCBossTower P327] ctor/dtor 定义迁自 CDeathTower.cpp（ORIG 归属证据：
//   ORIG `_GLOBAL__I_` 键名——本 TU = WongWork::CBossTower::CBossTower()@0x0814557d，
//   CDeathTower.cpp TU = CDeathTower::CPacketHandler::makePickupItemHeader@0x0846991d；
//   ORIG ctor 0x081429b4 / dtor 0x08142a74 位于本 TU 区段，紧邻 _onTimerProcess）。
// 具名字段访问依据（ORIG 报告 WongWork__CBossTower/CBossTower.md @0x81429b4 /
// ~CBossTower.md @0x8142a74）：
//   +0xaf0 = CBossTower::m_idx（movl 清 0）
//   +8     = CBossPlay::m_field8（movl 清 0）
//   +0xd   = CBossPlay::m_memberAlive[4]（ORIG memset(0xd,0,4) → 保持 memset 形态）
//   +0x9f0 = CBossStage::m_dungeon（ORIG movl $0x0,0x9f0(%eax) 单条 dword 直存）
//   +4     = CBossPlay::m_party（movl 清 0）
//   +0xb00 = CBossTower::m_dungeonLog（CBossDungeonEntranceLog，0x18，编译器构造/析构）
// 继承：CBossTower : CBossStage : CBossPlay（隐式基类构造/析构，编译器处理 vptr）。
#include "CBossTower/CBossTower.inc"
{
    CBossStage::reset();
    m_idx = 0;
    m_field8 = 0;
    memset(m_memberAlive, 0, 4);
    m_dungeon = 0;   // ORIG 0x081429b4 证据 movl $0x0,0x9f0(%eax)：单条 dword 直存，非 memset call
    m_party = 0;
    // m_dungeonLog（CBossDungeonEntranceLog）由编译器在进入函数体前按权威
    // ctor（ORIG 0x081456c2：map C1 + reset）构造，此处不再 memset。
}

#include "CBossTower/dtor_CBossTower.inc"
{
    CBossStage::reset();
    m_idx = 0;
    m_field8 = 0;
    memset(m_memberAlive, 0, 4);
    m_dungeon = 0;   // ORIG 0x08142a74 证据 movl $0x0,0x9f0(%eax)：单条 dword 直存
    m_party = 0;
    // m_dungeonLog 由编译器在函数体结束后按权威 dtor（ORIG 0x08145704：
    // reset + map D1）析构，此处不再 memset。
}

#include "CBossTower/getIdx.inc"
{
    return m_idx;   // ORIG 0x082aa728：return *(int*)(this+0xaf0)
}

#include "CBossTower/setIdx.inc"
{
    m_idx = idx;    // ORIG 0x082aa716：*(int*)(this+0xaf0)=idx
}

// ============================================================================
// 虚函数实现（WongWork::CBossTower）
// ============================================================================

// ORIG T 0x08143a62：
//   user==0 → 0；CBossPlay::handleDieUser 非 1 → 0；虚调 _checkMemberDie()
//   （ORIG call *(vptr+8)）为真时 onFinishStage()；m_prepareFinishFlag(+0xafc)
//   != 1 时 _onPrepareFinishTower(false)；返回 1。
// [b28d-54 N] ORIG 0x08143a62：CBossPlay::handleDieUser 保持 int（bool 化实测致
//   callee epilogue %ebx 合并回归，CBossPlay.h:61 在册）⇒ 调用点恒 test/sete
//   （P197④/N18b）；m_prepareFinishFlag bool 化已翻正（本波）、`call *%edx` vs
//   %eax 为 N7。语义等价。
// [b28d-solo-CBossTower] AE（41:41，Main 代落 bool 化后翻正）：CBossPlay.h:61
//   handleDieUser int→bool（ORIG 调用点 0x08143a62 `call; xor $1,%eax; test
//   %al,%al; je` 无 setne ⇒ bool，P292）+ 字段 bool 化 + 裸 if(!f)。
#include "CBossTower/handleDieUser.inc"
{
    if (user == 0)
        return 0;
    if (!CBossPlay::handleDieUser(user))
        return 0;
    if (_checkMemberDie())
    {
        onFinishStage();
        // b28d-54 翻正：ORIG 0x08143ab6 `movzbl 0xafc(%eax),%eax; xor $1,%eax;
        // test %al,%al; je` ⇒ 字段 bool 化（CBossTower.h）+ 裸 `if(!f)`（P248 档②）。
        if (!m_prepareFinishFlag)
            _onPrepareFinishTower(false);
    }
    return 1;
}

// ORIG T 0x08143ae2：
//   user==0 → 0；CBossPlay::handleLeaveUser 为假 → 0；
//   记 LeaveDungeon 历史（dungeon=m_dungeon(+0x9f0)、stage=m_difficulty(+0xae8)、
//   成员名表、partyType 低字节）；trace "member_count : %d"；
//   无成员 → _destroy 并返回 0；（party 空或全员死）→ onFinishStage +
//   _onPrepareFinishTower(false)；否则按人数 switch 写 m_difficulty(+0xae8)
//   （1..4 → 0..3）；返回 1。
// [b27 DL3R 复核判词 N 封顶] 177:176。已翻案两处（diff 行 135→129）：
//   ① CBossPlay::handleLeaveUser 返回类型 int→bool（ORIG 本函数 0x08143b04
//     `call; test %al,%al; je` 单字节直判 ⇒ callee bool/uchar；callee 行数
//     128:131 int/bool 实测无差，零成本）；头段 `==0`/`!f()` 在 4.4 恒产 xor
//     物形，改正向包裹形（body 入 if 块 + 尾共享 `return 0` 岛）后 xor 消失、
//     je 直达尾岛 fall-through 与 ORIG 同形。② switch 前 `int members =
//     get_member_count()` 具名局部复现 ORIG `mov %eax,-0x1c; mov -0x1c,%eax`
//     槽往返（P0）。残余均为在册 N：成员名表清零展开 1B peel 段+基址 -0x11d
//     奇数槽（N82/P95 帧槽基差）；finishNow 布尔物化 ORIG 落 eax 常量岛无槽
//     vs 4.4 bool 局部必占槽（N82/N18b 族）；`call *%eax` vs ORIG `*%edx`
//     （N7）；m_difficulty 读 ORIG eax→ebx 两步 vs 4.4 直落 ebx（N18a）；
//     histTrace/partyType 槽号 ±1 平移（N82）。
// [b28d-54 N] ORIG 0x08143ae2：成员名表清零展开段槽基差（N82/P95：ORIG
//   -0x11d/0xff vs 4.4 -0x11a/0xfb）+ `call *%eax` vs ORIG *%edx（N7）+
//   finishNow 布尔物化落槽（P197④）；控制流/调用序列全等。
// [b28d-solo-CBossTower] AE（177:177，本波翻正）：①memberNames `""`→`{0}`
//   复现 ORIG 对齐剥离 + rep stos 慢路（P312）；②P226 语句表达式 finishNow；
//   ③CBossStage.h m_difficulty 匿名 union 双视角（P235/P254）+ 实参走
//   m_difficultyE（enum→int 转换 ⇒ 两段 `mov 0xae8(%eax),%eax; mov %eax,%ebx`）。
#include "CBossTower/handleLeaveUser.inc"
{
    if (user == 0)
        return 0;
    // [b27 DL3R 形探针 f1] ORIG 0x08143b04 `call; test %al,%al; je <尾部共享
    // mov$0 岛>`（无 xor、je 直达 fall-through 岛）。4.4 对 `==0`/`!f()` 均产
    // xor 物形 ⇒ 改正向包裹：body 落 if 内，`return 0` 收敛为块后岛。
    if (CBossPlay::handleLeaveUser(user))
    {
    // [b28d-solo-CBossTower] P312 试形：槽 -0x11d（非 4 对齐）⇒ `{0}` 应发
    // ORIG 的对齐剥离 + rep stos 慢路（`lea;mov $0xff,%ebx;…and $1`）。
    char memberNames[255] = {0};              // ebp-0x11d（ORIG 对齐剥离慢路）

    const char partyTypeByte =
        static_cast<char>(m_party->m_battleField.m_bloodState);   // ORIG 读 +0xcd8 低字节
    m_party->getMemberNames(memberNames);

    // ORIG 0x08143bb0 段 m_difficulty 读为两段（`mov 0xae8(%eax),%eax;
    // mov %eax,%ebx`）⇒ enum 视角 → int 形参（P265 反向）。
    user->m_historyLog.LeaveDungeon(m_dungeon->GetDungeonName(), m_difficultyE,
                                    memberNames,
                                    static_cast<int>(partyTypeByte));

    cHistoryTrace histTrace;
    histTrace("member_count : %d", m_party->get_member_count());

    if (m_party == 0 || m_party->get_member_count() < 1)
        {
        _destroy();
        return 0;
        }

    // ORIG 0x08143c1a..0x08143c70：finishNow 物化无槽（mov$0/jmp + mov$1 岛）
    // ⇒ P226 语句表达式包条件（具名 bool 恒槽化 +2 条）。
    if (({ (m_party == 0) ||
           (m_party->get_member_count() > 0 && _checkMemberDie()); }))
        {
        onFinishStage();
        _onPrepareFinishTower(false);
        }
    else
        {
            // ORIG 0x08143cbc `call; mov %eax,-0x1c; mov -0x1c,%eax; cmp $2`：
            // 槽往返 ⇒ 原码具名局部（P0），switch 条件不直调。
            int members = m_party->get_member_count();
            switch (members)
        {
        case 1:
            m_difficulty = 0;
            break;
        case 2:
            m_difficulty = 1;
            break;
        case 3:
            m_difficulty = 2;
            break;
        case 4:
            m_difficulty = 3;
            break;
        default:
            break;
    }
        }
    return 1;
    }
        return 0;
}

// ORIG T 0x08143996：
//   CBossStage::handleMoveMap 为假 → 0；否则对 4 个槽位中存在且存活的成员
//   调 m_dungeonLog(+0xb00).IncrementBossStageStart(user, (uchar)m_stageCur,
//   (uchar)m_difficulty)；返回 1。
// [b28d-54 N] ORIG 0x08143996：被调 int 返回 ⇒ 调用点 test/sete（P197④/N18b）；
//   je/jne 极性 + 单源尾 nop 岛（P223 负例，4.4 恒合并）。语义等价。
// [b28d-solo-CBossTower 真语义 bug 修正 + AE] 旧重建写作 `if (f()==0) goto BODY;`
//   ——成功/失败反向（handleMoveMap 失败时执行了成功逻辑）；Ghidra C 明确
//   `if (cVar2 == 1) { 循环; return 1; } return 0;`。改回正向守卫 `if(!f()) return 0;`。
//   Main 代落 CBossStage.h:42 handleMoveMap int→bool 后 xor 物化复现（P292），
//   isMemberAlive 头内 inline bool 化消循环体 setne ⇒ 58:58 AE。
#include "CBossTower/handleMoveMap.inc"
{
    // ORIG 0x081439ae `call; xor $1,%eax; test %al,%al; je <body>` + 失败路径
    // `mov $0,%eax; jmp end` ⇒ 原码 `if (!f()) return 0;`（P168a①裸形）。
    if (!CBossStage::handleMoveMap(mapIndex))
        return 0;

    for (int i = 0; i < 4; ++i)
    {
        CUser* member = m_party->get_user(i);
        if (member != 0 && isMemberAlive(i))
        {
            m_dungeonLog.IncrementBossStageStart(
                *member,
                static_cast<unsigned char>(m_stageCur),
                static_cast<unsigned char>(m_difficulty));
        }
    }
    return 1;
}

// ORIG T 0x08143e5e：转发 CBossStage::handleDropItem，结果 ==1 → 1 否则 0。
#include "CBossTower/handleDropItem.inc"
{
    bool ret = CBossStage::handleDropItem(user, a, b, c, d, e);
    // ORIG 0x08143ea3 movzbl+xor$1+test/je：bool 槽取反物化（P158 形）。
    if (!ret)
        return 0;
    return 1;
}

// ORIG T 0x08143ebc：
//   CBossStage::handleKillMonster 为假 → 0；怪物清空检查：ORIG 对 this+0xa00
//   的 std::map<int,map_monster> 取 empty()（_M_node_count@map+0x14 即
//   this+0xa14），为空时 onFinishStage()；且 (uint)m_stageMax(+0xae4) <=
//   (uint)m_stageCur(+0xae0) 时 _onPrepareFinishTower(true)；返回 1。
// [N 封顶] 残差三处：① 调用点 CBossStage::handleKillMonster 后 ORIG `mov
// %al,-0x9;movzbl;xor$1;test/je` bool 槽物化，4.4 对 int callee 恒
// `test%eax;sete;test`（P197-④/N18b）；bool 化被调已实测致其大函数帧
// 0x3bc→0x3cc 全槽漂移回归，不可行；② ORIG 0x8143f38 `mov 0xae0(%eax),%edx`
// 直落 vs 4.4 两步（N18a）；③ ORIG 帧 0x44 vs 0x34（bool 槽差，随①）。
// [b28d-54 N] ORIG 0x08143ebc：CBossStage::handleKillMonster 保持 int（bool 化
//   实测致其大 body 全帧槽漂移）⇒ 调用点 test/setne（P197④）。本波已翻正
//   m_stageCur 同型化单段装载 + bool killed 槽形；余 setne 为 N。
// [b28d-solo-CBossTower] AE（52:52，Main 代落 bool 化后翻正）：CBossStage.h:45
//   handleKillMonster int→bool（ORIG 0x08143f03 `call; mov %al,-0x9` 无 setne
//   截断 ⇒ bool）+ killed bool 槽 + m_stageCur 同型化。
#include "CBossTower/handleKillMonster.inc"
                                  const MSG_MONSTER_DIE& die, int* d)
{
    // b28d-54 翻正：ORIG 0x08143f03 `mov %al,-0x9; movzbl -0x9,%eax; xor $1,%eax;
    // test %al,%al; je` ⇒ 局部是 bool（P248 档②；char 恒 `cmpb $0,-0x9; jne`）。
    // 仅改局部类型，被调 CBossStage::handleKillMonster 保持 int（历史实测 bool 化
    // 其大 body 致全帧槽漂移回归）。
    bool killed = (bool)CBossStage::handleKillMonster(user, a, b, die, d);
    if (!killed)
        return 0;
    if (m_stageData.m_monsterMap.empty())
    {
        onFinishStage();
        // b28d-54：m_stageCur 已同型化为 unsigned（CBossStage.h）⇒ 去 cast 后
        // 4.4 复现 ORIG 0x08143f38 单段 `mov 0xae0(%eax),%edx`（P211 档③）。
        if (m_stageCur >= m_stageMax)
        {
            _onPrepareFinishTower(true);
        }
    }
    return 1;
}

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
#include "CBossTower/handlePickupItem.inc"
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

// ============================================================================
// 生命周期方法
// ============================================================================

// ORIG T 0x081440b2：
//   m_endTick(+0x9ec) = CSystemTime::getCurTickCount(s_systemTime_)；
//   playSec = (m_endTick - m_startTick(+0x9e8)) / 1000（ORIG imul 0x10624dd3
//   有符号除法）；对槽位 0..3：user=get_user(i)，user!=0 且 checkValidUser(i)==1
//   且 isMemberAlive(i) 时 m_dungeonLog.IncrementBossStageEnd(*user,
//   (uchar)m_stageCur, playSec, (uchar)m_difficulty)；恒返 1。
// [b28d-54 N] ORIG 0x081440b2：帧槽整体平移 4（N82/N95）+ bool 槽物化（P197④）；
//   控制流/调用序列全等。
// [b28d-solo-CBossTower] AE（85:85，本波翻正）：CBossPlay.h 头内 inline
//   `isMemberAlive` unsigned char→bool（P292 调用点判据：ORIG 0x0814414d
//   `call; test %al,%al; je` 无 setne ⇒ bool；body 单 movzbl 直返不变）
//   + 去具名 invalid（裸短路条件）⇒ setne 两段消除、DIFF→AE。三 TU
//   （CBossTower/CBossPlay/CBossStage）fresh 复验零回归。
#include "CBossTower/onFinishStage.inc"
{
    int playSec = 0;
    m_endTick = GlobalData::s_systemTime_.getCurTickCount();
    playSec = (m_endTick - m_startTick) / 1000;

    for (int i = 0; i <= 3; ++i)
    {
        CUser* member = m_party->get_user(i);
        // ORIG 0x0814414d..0x08144159：短路物化岛无 bool 槽；isMemberAlive
        // 独立 `test %al,%al;je` ⇒ 裸条件两段（P0/P226）。
        if (member == 0 || m_party->checkValidUser(i) != 1)
            continue;
        // P47/P59：ORIG 为 `call; test %al,%al; je <循环尾>`（正向包裹、直跳）
        // ⇒ 用 if(f){body}，勿用 if(!f)continue（uchar callee 恒多 sete+test）。
        if (isMemberAlive(i))
        {
            m_dungeonLog.IncrementBossStageEnd(
                *member,
                static_cast<unsigned char>(m_stageCur),
                static_cast<unsigned int>(playSec),
                static_cast<unsigned char>(m_difficulty));
        }
    }
    return 1;
}

// ORIG T 0x08142d24：
//   m_prepareFinishFlag(+0xafc)=flag；m_towerEndTick(+0xaf8)=getCurTickCount()；
//   playSec=(m_towerEndTick-m_towerStartTick(+0xaf4))/1000；成员名表 0xff 清零后
//   party->getMemberNames(names)；槽位 0..3 有效成员：
//     未完成加载（m_memberField[i]==0，ORIG this+(i+0x274)*4+8=+0x9d8 区）时
//     hack addServerHackCnt(0x13a,1,0,0)；
//     m_historyLog.DungeonClearInfo(flag, playSec)。
//   循环后 G_TimerQueue()->InsertTimer(OBJ_TYPE 7, m_idx, msg 0xa3, 5s,
//   key 0xa3, 0xa3)。
// [b28d-54 N] ORIG 0x08142d24：槽平移 4 + 成员名表 0xff 清零展开段（N82/N78）。
// [b28d-solo-CBossTower] AE：memberNames `= ""`→`= {0}` 复现 ORIG 单段
//   `lea;mov $0x3f,%ecx;rep stos` + 去具名 invalid（裸短路条件）⇒ DIFF→AE。
#include "CBossTower/_onPrepareFinishTower.inc"
{
    m_prepareFinishFlag = flag;

    int playSec = 0;
    m_towerEndTick = GlobalData::s_systemTime_.getCurTickCount();
    playSec = (m_towerEndTick - m_towerStartTick) / 1000;

    char memberNames[255] = {0};              // ebp-0x113（ORIG 单段 rep stos）
    m_party->getMemberNames(memberNames);

    for (int i = 0; i <= 3; ++i)
    {
        CUser* member = m_party->get_user(i);
        // ORIG 0x08142e4f..0x08142e5b：短路物化岛无 bool 槽 ⇒ 裸条件（P0/P226）。
        if (member == 0 || m_party->checkValidUser(i) != 1)
            continue;

        if (m_memberField[i] == 0)
        {
            member->getHackAnalyzer()->addServerHackCnt(
                member, (WongWork::ENUM_HACKTYPE)0x13a, 1, 0, 0);
        }

        member->m_historyLog.DungeonClearInfo(flag, playSec);
    }

    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)7, m_idx,
                                (TIMER_MESSAGE)0xa3, 5, 0xa3, 0xa3);
}

// ORIG T 0x08142b36：
//   m_dungeonLog.BossTowerLogDBUpdate() → m_dungeonLog.reset() →
//   G_CGameManager()->returnBossTower(this)。
#include "CBossTower/_destroy.inc"
{
    m_dungeonLog.BossTowerLogDBUpdate();
    m_dungeonLog.reset();
    G_CGameManager()->returnBossTower(this);
}

// ----------------------------------------------------------------------------
// ORIG T 0x08142f32 _ZN8WongWork10CBossTower13InitBossTowerEP6CPartyi：
// m_dungeon(+0x9f0) = find_dungeon(dungeonIdx)；party/dungeon 判空 → 0；
// m_party(+0x04) = party；m_stageMax(+0xae4) = getRandomTowerMaxStage()；
// m_prepareFinishFlag(+0xafc) = 0；返回 1。
// [batch-19 N18a 封顶 ndiff=5] 唯一结构残差 = ORIG +0x57 三段冗余 regmove
// （`call`→`mov %eax,%edx`→`mov 0x8(%ebp),%eax`→`mov %edx,0xae4(%eax)`）；
// 同函数 +0x1a 的 m_dungeon 落槽处 ORIG 只 3 条（`mov this,%edx;mov %eax,`
// `0x9f0(%edx)`）⇒ 4.1 在同一 store 模板上两种分配，源码侧无法定向。
// 实测否决：① `register int` 无约束 → ndiff 11（4.4 选 %ebx ⇒ 多
// push/pop + `add $0x14,%esp` 帧形全变）；② `register int asm("edx")` →
// ndiff 5（复现 `mov %eax,%edx` 但反出 `mov %edx,%eax; mov this,%edx`
// 三步，净 0 且违 P78）。维持直赋值。
// ----------------------------------------------------------------------------
#include "CBossTower/InitBossTower.inc"
{
    m_dungeon = G_CDataManager()->find_dungeon(dungeonIdx);
    if (party == 0 || m_dungeon == 0)
        return 0;

    m_party = party;
    m_stageMax = m_dungeon->getRandomTowerMaxStage();
    m_prepareFinishFlag = 0;
    return 1;
}

// ----------------------------------------------------------------------------
// ORIG T 0x08142fa6 _ZN8WongWork10CBossTower23checkStartGameConditionERhb：
//   dungeon 空/未开放 → trace "boss Tower dungeon is not open"(0xdb,5) +
//     包(1,0x10)/byte 0/int 0x15 → send_to_party → 0；
//   逐槽位：有效成员 charac_level < get_min_level → trace(0xf0,5) +
//     包 int 0xe → send → 0；
//   partyType(+0xcd8)==1 → 1；否则 CheckEnterAncientDungeon 失败 → 包 int 0x11 +
//     byte(flag) → send → 0；useItems 时 UseAncientDungeonItems 失败 →
//     包 int 1 → send → 0；通过 → 1。
// ----------------------------------------------------------------------------
// [b28d-54 N] ORIG 0x08142fa6：大件帧/槽整体平移（N82）+ 实参装载序（P23）；
//   控制流与调用序列全等（341 vs 340）。
// [b28f-SweepCBossTower] NEAR（340:340）。bool 化已落地（ORIG 调用点
//   0x081435b8/0x081441e6 `xor $1`）；残余仅 itemParams 与首个 cMyTrace 的
//   槽互换——ORIG itemParams=-0x44（深）/trace=-0x34（浅）vs 我们
//   trace=-0x44/itemParams=-0x34（N82附：4.4 聚合对象优先占低槽、标量分区
//   排布；声明序反转试形无效）。items -0x168、各分支 PacketGuard -0x50/
//   -0x5c/-0x68/-0x74、-0x16c(useItems spill) 与 ORIG 全等。
// [L5Free49to60 2026-09-11] **NEAR→AE（340:340，bcmp NDIFF 归零）**：同 P221 形——
//   首个 cMyTrace 由具名对象改全表达式临时链 `cMyTrace(...)(...)`，4.4 的聚合对象
//   低槽占用消失，itemParams 回深槽 -0x44、trace 回浅槽 -0x34，与 ORIG 逐槽全等。
#include "CBossTower/checkStartGameCondition.inc"
{
    Inven_Item items[4];                       // ebp-0x168 起 4*0x3d，整体清零 0xf4
    memset(items, 0, 0xf4);
    int itemParams[4];                         // ebp-0x44 起 0x10 字节清零
    memset(itemParams, 0, sizeof(itemParams));

    if (m_dungeon == 0 || !m_dungeon->IsOpenScheduleDungeon())
    {
        // [L5Free49to60 P221 试形] ORIG 该 cMyTrace 对象占浅槽 -0x34..-0x24（4.1
        // 互斥块后声明占浅），4.4 具名对象恒占低槽 -0x44（与 itemParams 互换）
        // ⇒ 改全表达式临时链复现 ORIG 槽方向（P37/P221）。
        cMyTrace(__PRETTY_FUNCTION__, 0xdb, 5)("boss Tower dungeon is not open");
        PacketGuard packet;
        packet.clear();              // ORIG 0x0814309a（本分支 C1 后显式 clear）
        packet.put_header(1, 0x10);
        packet.put_byte(0);
        packet.put_int(0x15);
        packet.finalize(true);
        m_party->send_to_party(packet);
        return 0;
    }

    for (int i = 0; i <= 3; ++i)
    {
        CUser* member = m_party->get_user(i);
        // ORIG 0x081431b6..+0x1e3：正向包裹（P214①）——`member != 0 &&
        // checkValidUser(i)` 产成功物化岛 `mov$1;jmp/mov$0 + test %al; je
        // <循环尾>`；`|| !f()` 的 continue 形在 4.4 多一条 `xor $1`。
        if (member != 0 && m_party->checkValidUser(i))
        {
            if (member->get_charac_level() < m_dungeon->get_min_level())
            {
                // P227 全表达式临时链：ORIG 0x081431e9..+0x209 在 ctor call 前
                // 右起求值 varargs（charac_level→%ebx、min_level→%esi 跨 ctor 保活）。
                cMyTrace(__PRETTY_FUNCTION__, 0xf0, 5)(
                    "dungeon level check charac_level : %d, dungeon_min_level : %d",
                    member->get_charac_level(), m_dungeon->get_min_level());
                PacketGuard packet;
                packet.clear();      // ORIG 0x08143229（本分支 C1 后显式 clear）
                packet.put_header(1, 0x10);
                packet.put_byte(0);
                packet.put_int(0xe);
                packet.finalize(true);
                m_party->send_to_party(packet);
                return 0;
            }
        }
    }

    if (m_party->m_battleField.m_bloodState == 1)
    {
        return 1;
    }

    // ORIG 0x08143328/0x0814342f：两处调用结果共用 -0x11 具名 bool 槽
    // （`mov %al,-0x11; movzbl; xor $1; test; je`）⇒ 源码是同一个 `bool ok`
    // 局部两次赋值（P158 形），第二次用 if(useItems) 包裹而非 `&&` 短路。
    bool ok = m_party->CheckEnterAncientDungeon(m_dungeon, flag, items, itemParams);
    if (!ok)
    {
        PacketGuard packet;
        packet.put_header(1, 0x10);
        packet.put_byte(0);
        packet.put_int(0x11);
        packet.put_byte(static_cast<unsigned char>(flag));
        packet.finalize(true);
        m_party->send_to_party(packet);
        return 0;
    }

    if (useItems)
    {
        ok = m_party->UseAncientDungeonItems(m_dungeon, items, itemParams);
        if (!ok)
        {
            PacketGuard packet;
            packet.put_header(1, 0x10);
            packet.put_byte(0);
            packet.put_int(1);
            packet.finalize(true);
            m_party->send_to_party(packet);
            return 0;
        }
    }
    return 1;
}

// ----------------------------------------------------------------------------
// ORIG T 0x081434f4 _ZN8WongWork10CBossTower15handleStartGameEP6CPartyic17ENUM_DUNGEON_TYPE：
// dungeonType==1 或 partyType(+0xcd8)==1 → 0；m_expParamA(+0xaec)=dungeonType、
// m_difficulty(+0xae8)=(char)b；reset 链 + m_dungeonLog.reset + m_stageCur=1；
// InitBossTower / checkStartGameCondition 失败 → 0；GM 管理者字节替换广播字节；
// RandomConsistMap 失败 → 0；InitPlayUser(idx 参数, m_idx)；
// m_towerStartTick(+0xaf4)=getCurTickCount、m_towerEndTick(+0xaf8)=0；成员名表
// 清零后逐槽位 IncrementBossStageStart + IncrementDungeonEntrance +
// EnterDungeon 日志；SetEPLPState(1)、gen_timer_key(0x2d)、
// SetSelectedEPLPCmd(-1)；包(0,0x1e1){towerIdx, m_stageMax(short), partyType(byte),
// announceByte(byte)} 广播；SendMapInfoPacket；InsertTimer(OBJ 7, m_idx, msg 0xa5,
// 单人 90s 否则 120s, key 0xa5, 0xa5)；返回 1。
// ----------------------------------------------------------------------------
// [b28d-54 N + P0 清绑] ORIG 0x081434f4：本波清除 3 处 `register asm("eax")` 绑定
//   （N96 违禁）→ 自然 `if(!f()) return 0;`；残余帧 0x15c→0x16c + 全帧槽平移（N82）
//   + int callee 调用点 sete 物化（P168a 定论，N18b）。调用序列全等（301 vs 302）。
// [b28d-solo-CBossTower] AE（302:302，本波翻正）：①IncrementDungeonEntrance 块序
//   反写（ORIG je→(towerIdx,1)）+ timeoutSec 两段式（P198③）+ 调用点 `!f()`
//   （本类 bool 化）；②CBossStage.h m_expParamA 匿名 union 双视角（P235/P211：
//   ORIG 0x08143531 LHS-first ⇒ 字段与 ENUM_DUNGEON_TYPE 同型）+ 赋值走
//   m_expParamAE；③m_difficulty/m_bloodState 同法（CBattle_Field.h:473 union，
//   EnterDungeon 实参走 m_bloodStateE ⇒ 两段 `mov 0xcd8(%eax),%eax; mov %eax,%esi`）。
//   CBossStage.h 补 ENUM_DUNGEON_TYPE guard 同体声明（CParty.h 约定）。
#include "CBossTower/handleStartGame.inc"
                                 ENUM_DUNGEON_TYPE dungeonType)
{
    if (dungeonType == static_cast<ENUM_DUNGEON_TYPE>(1))
        return 0;
    if (party->m_battleField.m_bloodState == 1)
        return 0;

    // P211 档③：ORIG 0x08143531 为 LHS-first ⇒ 字段与 RHS 同型（enum 视角
    // m_expParamAE，与形参 ENUM_DUNGEON_TYPE 同型；int 视角 + cast 两向实测
    // 均 RHS-first，4.4 看穿 cast）。
    m_expParamAE = dungeonType;
    m_difficulty = static_cast<int>(b);        // movsbl

    CBossPlay::reset();
    CBossStage::reset();
    m_dungeonLog.reset();
    m_stageCur = 1;

    // [b28d-54 P0 清绑] 原 X1 形用 `register asm("eax")` 绑定复现 ORIG 的
    // `call; xor $1; test; je`——违 N96/P0（全域禁 register asm）。按 P168a
    // 定论改自然 `if(!f()) return 0;`（int callee 调用点无法产 xor，属 N 封顶）。
    if (!InitBossTower(party, towerIdx))
        return 0;
    unsigned char gmFlag = 0;
    if (!checkStartGameCondition(gmFlag, true))
        return 0;
    // ORIG：manager 存 -0x28 复用；GM 字节直写回 b 的字节槽（-0x13c）。
    // 嵌套 if（非 &&）以复现 ORIG 双 je 落点形，避免 4.4 物化布尔。
    CUser* manager = party->getManager();
    if (manager->isGMUser())
    {
        if (manager->m_gmBossDifficulty != 0)
        {
            b = manager->m_gmBossDifficulty;   // GM 观战标记替换广播字节
            manager->m_gmBossDifficulty = 0;
        }
    }
    if (!RandomConsistMap(*manager))
        return 0;
    InitPlayUser(towerIdx, m_idx);
    m_towerStartTick = GlobalData::s_systemTime_.getCurTickCount();
    m_towerEndTick = 0;

    char memberNames[256] = {0};             // ebp-0x138（内联展开清零 0x100）
    m_party->_getMemberNames(memberNames);

    for (int i = 0; i <= 3; ++i)
    {
        CUser* member = m_party->get_user(i);
        // ORIG 0x081436f8..0x08143704：短路物化岛无 bool 槽 ⇒ 裸条件（P0/P226）。
        if (member == 0 || m_party->checkValidUser(i) != 1)
            continue;

        m_dungeonLog.IncrementBossStageStart(
            *member,
            static_cast<unsigned char>(m_stageCur),
            static_cast<unsigned char>(m_difficulty));

        // ORIG 0x0814376b..0x0814378f：`je <towerIdx,1 块>` + fall-through
        // (towerIdx,0) ⇒ 反写条件（!=1 → 0；else → 1）复现块序。
        if (m_party->m_battleField.m_bloodState != 1)
            GetInstanceDungeonEntranceLog()->IncrementDungeonEntrance(towerIdx, 0);
        else
            GetInstanceDungeonEntranceLog()->IncrementDungeonEntrance(towerIdx, 1);

        // ORIG 0x0814377x 段 m_bloodState 读为两段（`mov 0xcd8(%eax),%eax;
        // mov %eax,%esi`）⇒ enum 视角 → int 形参（P254/P265 反向）。
        member->m_historyLog.EnterDungeon(m_dungeon->GetDungeonName(),
                                          m_difficultyE, memberNames,
                                          m_party->m_battleField.m_bloodStateE);
    }

    m_party->SetEPLPState(1);
    m_party->gen_timer_key((TIMER_MESSAGE)0x2d);
    m_party->SetSelectedEPLPCmd(-1);

    PacketGuard packet;
    packet.put_header(0, 0x1e1);
    packet.put_int(towerIdx);
    packet.put_short(m_stageMax);
    packet.put_byte(m_party->m_battleField.m_bloodState);
    packet.put_byte(static_cast<int>(b));    // announceByte 即 b 槽（GM 可改写）
    packet.finalize(true);
    m_party->send_to_party(packet);

    SendMapInfoPacket();

    // ORIG 0x08143909..0x0814391b：movl $0,-0x24 预清 + sete/test/je 双赋值
    // ⇒ 声明=0 + if/else 两赋值（P198③），勿用三目（4.4 产 eax 中转）。
    int timeoutSec = 0;
    if (m_party->get_member_count() == 1)
        timeoutSec = 0x5a;
    else
        timeoutSec = 0x78;
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)7, m_idx,
                                (TIMER_MESSAGE)0xa5, timeoutSec, 0xa5, 0xa5);
    return 1;
}

// ----------------------------------------------------------------------------
// ORIG T 0x081454b8 _ZN8WongWork10CBossTower13handleUseItemEP5CUsermh：
// GetMemberSlotNo==-1 → 0；itemId==0xbdd → m_useItemCount[slot]++（+0x9c8 区）；
// param==2 → m_memberField[slot]++（+0x9d8）；返回 1。
// ----------------------------------------------------------------------------
#include "CBossTower/handleUseItem.inc"
{
    const int slot = static_cast<int>(m_party->GetMemberSlotNo(user));
    if (slot == -1)
        return 0;

    if (itemId == 0xbdd)
    {
        ++m_useItemCount[slot];
    }
    if (param == 2)
    {
        ++m_memberField[slot];
    }
    return 1;
}

// ----------------------------------------------------------------------------
// ORIG T 0x08143f6a _ZN8WongWork10CBossTower19handleFinishLoadingEP5CUser：
// slot 无效或 checkValidUser 为假 → 0；setMemberReady(slot,true) +
// SendCmdOkPacket(0x28)；虚调 checkAllMemberReady(get_member_count())（vptr+0xc）
// 为真时广播包(0,0x1e)/int 0/finalize/send；返回 1。
// ----------------------------------------------------------------------------
// [b28d-54 N] ORIG 0x08143f6a：帧 0x20→0x30 + 槽平移（N82）。
// [b28d-solo-CBossTower] AE（94:94，Main 代落 bool 化后翻正）：CBossPlay.h:60
//   checkAllMemberReady int→bool（ORIG 调用点 0x08143fd5 `call *%ebx; test %al,%al;
//   je` 无 setne ⇒ bool）+ party/ready 具名局部已去 + 裸短路条件化。
#include "CBossTower/handleFinishLoading.inc"
{
    const int slot = static_cast<int>(m_party->GetMemberSlotNo(user));
    // ORIG 0x08143f86..0x08143fb5：短路物化岛无 bool 槽（mov$1/jmp/mov$0 +
    // test %al,%al;je）⇒ 裸条件内联（P0/P226）。
    if (slot != -1 && m_party->checkValidUser(slot) != 0)
    {
        setMemberReady(slot, true);
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x28);
        if (checkAllMemberReady(m_party->get_member_count()))
        {
            PacketGuard packet;
            packet.put_header(0, 0x1e);
            packet.put_int(0);
            packet.finalize(true);
            m_party->send_to_party(packet);
        }
        return 1;
    }
    return 0;
}

// ----------------------------------------------------------------------------
// ORIG T 0x08142cde _ZN8WongWork10CBossTower15_onTimerProcessE13TIMER_MESSAGEj：
// msg 0xa3 → onClearBossTower()；0xa4 → onReturnVillage()；0xa5 →
// onReadyTimerCheck()；其余忽略。第二参数未使用。
// [batch-19 N 封顶 ndiff=3] 残余 = 末级比较极性：ORIG `cmp $0xa3,%eax;`
// `jne exit` + 体 fall-through，4.4 恒 `je body; jmp exit`（switch 尾 case
// 与空 default 汇入同一出口时 4.4 不 reverse 最后比较）。实测否决三形：
// ① 去 default → 3（同）；② if-else 链 → 11（4.4 折 `cmpl $imm,mem` 丢
// `mov 0xc(%ebp),%eax` 三级 + jne 正向）；③ `register` 形参 + if-else →
// 20（cmp 用寄存器但 body 全 fall-through，块序反）。switch+default 形最优。
// ----------------------------------------------------------------------------
// [b28d-54 N 复核] ORIG 0x08142cde：末级比较极性（ORIG `cmp $0xa3; jne exit` +
//   体 fall-through vs 4.4 switch 恒 `je body; jmp exit`）——4.4 switch 布局固有差；
//   本波复核 switch/default 形仍最优（ndiff 3），语义等价。
// [b28d-solo-CBossTower] N：23:24。4.4 switch 末级比较恒 `je body;jmp exit`，
//   4.1 为 `jne exit`+body fall-through（P285/P46 布局固有）；去 default/if-else/
//   register 三形历史实测均劣，维持 switch。
#include "CBossTower/_onTimerProcess.inc"
{
    (void)a;   // ORIG 忽略第二参数
    // [N 封顶 ndiff=3] ORIG 0x08142cde 末级比较 `cmp $0xa3; jne exit`+Ready 体
    // fall-through；4.4 对 switch 恒 `je body;jmp end` 不反转末比较。
    // batch-19 实测否决三形（去 default / if-else 链 / register 形参）；
    // 本轮补测 goto 链两形（register 局部落 %ebx+push/pop 反劣；普通形参
    // 4.4 折 `cmpl $imm,mem` 丢 `mov 0xc(%ebp),%eax`）同不通，维持 switch。
    switch (msg)
    {
    // ORIG 0x08142ce4..+0x1e：比较序 0xa4→0xa5→0xa3（4.1 决策树），
    // 叶块物理序 0xa3(+0x1e)/0xa4(+0x2b)/0xa5(+0x38) = case 书写序（P46/P324）。
    case (TIMER_MESSAGE)0xa3:
        onClearBossTower();
        break;
    case (TIMER_MESSAGE)0xa4:
        onReturnVillage();
        break;
    case (TIMER_MESSAGE)0xa5:
        onReadyTimerCheck();
        break;
    default:
        break;
    }
}

// ----------------------------------------------------------------------------
// ORIG T 0x08142b72 _ZN8WongWork10CBossTower17handleEPLPCommandEi：
// bool 门（party 非空且 GetEPLPState()==2 才放行）；cmd(unsigned)<=2 时成员名表
// 清零、逐有效成员记 LeaveDungeon(level 固定 0) 并 ReturnToVillage；随后
// _destroy，返回 1；门未放行返回 0。
// ----------------------------------------------------------------------------
// [b28d-54 N] ORIG 0x08142b72：bool 物化块 + 帧 +0x10（P197④/N82）。
// [b28d-solo-CBossTower] P226 否定式语句表达式
//   `if (({ (m_party==0) || (GetEPLPState()!=2); })) return 0;` 一次翻正物化岛/
//   极性/尾块。memberNames 改 `{0}` 复现单段 rep stos。
// [b28f-SweepCBossTower] AE（105:105，偏移全对齐）：范围检查改连续 case
//   switch（P286/P320）复现 ORIG 0x08142bb3 `mov 0xc(%ebp),%eax; add
//   $0x0,%eax; cmp $2,%eax; ja <_destroy>` 的「形参载入 + 寄存器比较 + ja
//   落点」；残余仅 4.1 特有 int→unsigned no-op `add $0x0,%eax`（4.4 恒消除，
//   P41 反向），由 clean 口径归一。
#include "CBossTower/handleEPLPCommand.inc"
{
    // ORIG 0x08142b7c..0x08142bb3：skip = (m_party==0) || (GetEPLPState()!=2)
    // 物化岛 mov$1/jmp/mov$0 + test %al,%al;je 无槽 ⇒ P226 否定式语句表达式。
    if (({ (m_party == 0) || (m_party->GetEPLPState() != 2); }))
        return 0;

    {
        // ORIG 0x08142bb3 `mov 0xc(%ebp),%eax; add $0x0,%eax; cmp $2,%eax; ja`
        // （int→unsigned 转换的 no-op add 为 4.1 特有；4.4 消除该条，P41 反向）
        // ⇒ 用连续 case switch 复现「形参载入 + 寄存器比较 + ja 落 _destroy」
        // （P286/P320），残余仅 1 条 no-op add。
        switch (cmd)
        {
        case 0:
        case 1:
        case 2:
            {
            char memberNames[255] = {0};          // ebp-0x110（ORIG 单段 rep stos）

            const char partyTypeByte =
                static_cast<char>(m_party->m_battleField.m_bloodState);   // 低字节 movsbl
            m_party->getMemberNames(memberNames);

            for (int i = 0; i <= 3; ++i)
            {
                CUser* member = m_party->get_user(i);
                if (member == 0 || m_party->checkValidUser(i) != 1)
                    continue;
                member->m_historyLog.LeaveDungeon(
                    m_dungeon->GetDungeonName(), 0, memberNames,
                    static_cast<int>(partyTypeByte));
            }
            m_party->ReturnToVillage();
            }
            break;
        default:
            break;
        }

        _destroy();
        return 1;
    }
    return 0;
}

// ----------------------------------------------------------------------------
// ORIG T 0x0814432a _ZN8WongWork10CBossTower16SendRewardPacketEv：
// 逐有效成员发奖励预告包(0,0x1e3)：put_int(m_rewardExp[i])；对全部 4 个奖励块
// （无论有效性）：put_byte(m_memberReward[j].m_count) 后逐件
// put_int(item.m_addInfo)+put_int(item.get_add_info())；finalize 后 Send。
// 恒返 1。
// ----------------------------------------------------------------------------
// [b28d-54 N] ORIG 0x0814432a：帧 0x30→0x40 + 槽平移（N82）+ 寄存器角色（N7）。
// [b28d-solo-CBossTower] AE：去 `Inven_Item& reward` 具名引用（P157/P183 每字段
//   重算 imul 寻址）+ invalid 裸短路条件 ⇒ DIFF→AE。
#include "CBossTower/SendRewardPacket.inc"
{
    for (int i = 0; i <= 3; ++i)
    {
        CUser* member = m_party->get_user(i);
        // ORIG 0x08144378..0x08144384：短路物化岛无 bool 槽 ⇒ 裸条件（P0/P226）。
        if (member == 0 || m_party->checkValidUser(i) != 1)
            continue;

        PacketGuard packet;
        packet.put_header(0, 0x1e3);
        packet.put_int(m_rewardExp[i]);              // this+(i+4)*4+8 = +0x18
        for (int j = 0; j <= 3; ++j)
        {
            packet.put_byte(m_memberReward[j].m_count);   // +j*0x268+0x28
            for (int k = 0; k < m_memberReward[j].m_count; ++k)
            {
                // P157/P183：ORIG 每字段重算完整 imul 寻址，无具名引用缓存。
                packet.put_int(m_memberReward[j].m_items[k].m_addInfo);
                packet.put_int(m_memberReward[j].m_items[k].get_add_info());
            }
        }
        packet.finalize(true);
        member->Send(packet);
    }
    return 1;
}

// ----------------------------------------------------------------------------
// ORIG T 0x081441d4 _ZN8WongWork10CBossTower14SendELPLPacketEv：
// 非 partyType==1 时逐有效成员 checkStartGameCondition(flag,false)，任一失败
// ok=false 提前跳出；随后无条件发包(0,0x1e4){ok}给全队；恒返 1。
// ----------------------------------------------------------------------------
// [b28d-54 N] ORIG 0x081441d4：帧 0x30→0x40 + 槽平移（N82）+ 寄存器角色（N7）。
// [b28d-solo-CBossTower] AE：删 party 具名局部（P0 逐点重读 m_party）+ invalid
//   裸短路条件 + checkStartGameCondition bool 化（本类，ORIG 调用点 0x081441e6
//   `xor $1`）⇒ DIFF→AE。
#include "CBossTower/SendELPLPacket.inc"
{
    bool ok = true;                            // ebp-0x11
    if (m_party->m_battleField.m_bloodState != 1)
    {
        for (int i = 0; i <= 3; ++i)
        {
            CUser* member = m_party->get_user(i);
            // ORIG 0x0814420e..0x08144221：短路物化岛无 bool 槽
            // （mov$1/jmp/mov$0 + test %al,%al;jne 循环尾）⇒ 裸条件内联（P0/P226）。
            if (member == 0 || m_party->checkValidUser(i) != 1)
                continue;

            unsigned char requireFlag = 0;       // ebp-0x21
            if (!checkStartGameCondition(requireFlag, false))
            {
                ok = false;
                break;
            }
        }
    }

    PacketGuard packet;
    packet.put_header(0, 0x1e4);
    packet.put_byte(ok ? 1 : 0);
    packet.finalize(true);
    m_party->send_to_party(packet);
    return 1;
}

// ----------------------------------------------------------------------------
// ORIG T 0x081446a2 _ZN8WongWork10CBossTower15InsertRewardExpEv：
// 逐有效成员：bonusExp/extraFlag 局部清零后 gain_exp_sp(m_rewardExp[i], ...,
// eExpAddReason 2, 0, false)；成功时 SendNotiPacket(eSendTarget 2,
// ENUM_NOTIPACKET 2, 1)。恒返 1。
// [batch-19 AE] 4→0：两处 P0 形订正——
//  ① ORIG +0x53 起 `xor$1;test%al,%al;jne +0xe2` 直测 %eax（choose 无槽）⇒
//    成员有效性判定必为 `if (member==0 || checkValidUser(i)!=1) continue;`
//    内联形；写 `bool invalid = ...;` 具名 bool 局部 ⇒ 4.4 多
//    `mov %al,-0xa; cmpb $0,-0xa` 两段（+2/−1）。
//  ② ORIG +0x60 `movb $0x0,-0x9(%ebp)` 在 gain_exp_sp call **之前** ⇒ 原码
//    `bool gained = false;` 后接 `gained = member->gain_exp_sp(...)` 两句；
//    合写成 `bool gained = call();` 丢预清行（−1）。
// ----------------------------------------------------------------------------
#include "CBossTower/InsertRewardExp.inc"
{
    for (int i = 0; i <= 3; ++i)
    {
        CUser* member = m_party->get_user(i);
        if ((member == 0) || (m_party->checkValidUser(i) != 1))
            continue;

        int bonusExp = 0;                        // ebp-0x18
        int extraFlag = 0;                       // ebp-0x1c
        bool gained = false;
        gained = member->gain_exp_sp(
            m_rewardExp[i], bonusExp, extraFlag,
            (CUser::eExpAddReason)2, 0, false);
        if (gained)
        {
            member->SendNotiPacket((CUser::eSendTarget)2,
                                   (ENUM_NOTIPACKET)2, 1);
        }
    }
    return 1;
}

// ----------------------------------------------------------------------------
// ORIG T 0x081447a0 _ZN8WongWork10CBossTower17InsertRewardItemsEv：
// 逐有效成员、逐件 m_memberReward[i].m_items[j]（上限 m_count）：
//   空（m_addInfo==-1）跳过；m_addInfo==0 → 金币：get_add_info() 金额经
//   gain_money(reason 5,true,0) 入账，不足部分 SendMoneyFullReason(0,金额,实得)，
//   slotResult=0；否则 insertItemIntoInventory(item, reason 0xb, true, true)。
//   成功（>=0）发包(0,0xe){itemSpace,short 1,MakeItemPacket}+Send；
//   失败（<0）邮件补发：uniqueNo[0x18]/sender[0x1e]/title[0x100] 清零，
//   find_item(m_addInfo) 后取脚本文案 chn_game_server_msg_21(sender)/22(title)，
//   IsAvatarItem()（vptr+0x10）为真 → 计算 usable period 到期秒
//   （0 或 0x16d 不限）、genIPGNo(TYPE 3,characNo) 代理名、get_endurance，
//   ReqDBSendNewAvatarMail；否则 ReqDBSendNewSystemMail(0xf, 本服组)。
// 恒返 1。
// ----------------------------------------------------------------------------
// [b28d-54 N] ORIG 0x081447a0：帧/槽整体平移（N82）+ std::min 模板槽（N86）；
//   调用序列全等（354 vs 366）。
// [b28d-solo-CBossTower] N：366:354（ndiff 298→264）。本波修：invalid 裸短路
//   条件、reward 具名指针（P171 死判空 + Ghidra C local_38 照抄）、agency/
//   endurance 内联（P0/P183：Ghidra C 中为寄存器伪变量 pcVar8/cVar2、无槽往返）。
//   残余 = 全帧槽号平移（我们浅 4：i@-0x3c vs ORIG -0x40；P112/N98 4.4 槽按
//   首次使用+帧尺寸分配，声明序不可翻）+ local_2c 类位置差（N82）。
#include "CBossTower/InsertRewardItems.inc"
{
    for (int i = 0; i <= 3; ++i)
    {
        CUser* member = m_party->get_user(i);
        // ORIG 0x081447d8..0x081447e4：短路物化岛无 bool 槽 ⇒ 裸条件（P0/P226）。
        if (member == 0 || m_party->checkValidUser(i) != 1)
            continue;

        for (int j = 0; j < m_memberReward[i].m_count; ++j)
        {
            // ORIG 0x081447f9 具名指针槽 -0x34 + P171 死指针判空（Ghidra C
            // `local_38 != 0 && *(int*)(...+0x2e) != -1`）。
            Inven_Item* reward = &m_memberReward[i].m_items[j];
            if (reward == 0 || reward->m_addInfo == -1)
                continue;

            int slotResult = -1;
            if (reward->m_addInfo == 0)
            {
                // 金币奖励：m_addInfo2 为金额（ORIG 三次 get_add_info 各用点重取，P48）
                const int gained = member->getCurCharacInvenW()->gain_money(
                    reward->get_add_info(), (eMoneyAddReason)5, true, 0);
                if (gained < reward->get_add_info())
                {
                    member->SendMoneyFullReason(
                        (ENUM_MONEY_FULL_REASON)0,
                        static_cast<unsigned long>(reward->get_add_info()),
                        static_cast<unsigned long>(gained));
                }
                slotResult = 0;
            }
            else
            {
                // ORIG else 分支重取 getCurCharacInvenW（与金币分支各一次，P48）
                slotResult = member->getCurCharacInvenW()->insertItemIntoInventory(
                    *reward, (eItemAddReason)0xb, true, true);
            }

            if (slotResult >= 0)
            {
                PacketGuard packet;
                packet.put_header(0, 0xe);
                packet.put_byte(reward->GetItemSpace());
                packet.put_short(1);
                member->getCurCharacInvenR()->MakeItemPacket(
                    (INVEN_TYPE)GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)reward->GetItemSpace()),
                    slotResult, packet);
                packet.finalize(true);
                member->Send(packet);
            }
            else
            {
                // 背包满 → 邮件补发
                // ORIG 0x08144a73..+0x2da：邮件分支入口两条 int 预清
                // （-0x28 写后未读死局部 P171 照抄；-0x24 expireSec P198③ 两段式）。
                int mailDeadLocal = 0;             // ebp-0x28（ORIG 只写不读）
                int expireSec = 0;                 // ebp-0x24
                char uniqueNo[0x18] = {0};         // ebp-0x64（ORIG .rodata 清零模板）

                CItem* itemData =
                    G_CDataManager()->find_item(reward->m_addInfo);
                if (itemData == 0)
                    continue;

                char title[256];                   // ebp-0x182 起 0x100 清零
                memset(title, 0, sizeof(title));
                char senderName[30];               // ebp-0x82 起 0x1e 清零
                memset(senderName, 0, sizeof(senderName));

                strncpy(senderName,
                        g_scriptStringManager_.findString(
                            4, "chn_game_server_msg_21", 0),
                        0x1d);
                strncpy(title,
                        g_scriptStringManager_.findString(
                            4, "chn_game_server_msg_22", 0),
                        0xff);

                // ORIG 0x08144b82 第一次 IsAvatarItem：仅决定是否换算到期秒
                // （P48：同一 getter 在两臂各出现 ⇒ 源码两次独立调用，勿缓存）。
                if (itemData->IsAvatarItem())
                {
                    // 有效期换算到期秒（0 或 0x16d=365 视为无限）
                    const int period = itemData->getUsablePeriod();
                    // ORIG 0x08144ba6..+0x42f：两个 je 同落 0 块（+0x431），
                    // 计算块 fall-through ⇒ 正向包裹（计算入 then、0 归 else 尾岛）。
                    if (period != 0 && period != 0x16d)
                        expireSec =
                            GlobalData::s_systemTime_.getCurSec() +
                            period * 0x15180;      // 天 → 秒
                    else
                        expireSec = 0;             // ORIG 0x08144bd1 movl $0,-0x24
                }

                // ORIG 0x08144bd8 第二次 IsAvatarItem：决定邮件类型。
                if (itemData->IsAvatarItem())
                {
                    // [b28d-solo-CBossTower] P0/P183：Ghidra C 中 genIPGNo 结果
                    // （pcVar8）与 get_endurance 结果（cVar2）均为寄存器伪变量、
                    // 非具名局部槽 ⇒ 内联进实参（右起求值序 genIPGNo 先于
                    // get_endurance，与 ORIG 一致）。
                    WongWork::CMailBoxHelper::ReqDBSendNewAvatarMail(
                        member, member->getCurCharacNo(), reward->m_addInfo,
                        expireSec,
                        static_cast<char>(
                            static_cast<CEquipItem*>(itemData)->get_endurance()),
                        0,
                        WongWork::CGenUniqueNo::genIPGNo(
                            WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE_3,
                            static_cast<unsigned int>(member->getCurCharacNo()),
                            uniqueNo),
                        title, static_cast<int>(strlen(title)));
                }
                else
                {
                    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                        senderName, *reward, 0,
                        static_cast<unsigned int>(member->getCurCharacNo()),
                        title, static_cast<int>(strlen(title)), 0xf,
                        member->GetServerGroup(), false, false);
                }
            }
        }
    }
    return 1;
}

// ----------------------------------------------------------------------------
// ORIG T 0x08144508 _ZN8WongWork10CBossTower16onClearBossTowerEv：
// partyType==1 → 仅发包(0,0xfe)；否则 generateRewardExp/generateRewardItem/
// SendRewardPacket/InsertRewardExp/InsertRewardItems 任一非 1 → 0；
// SendELPLPacket()==1 时 SetEPLPState(2) 并 InsertTimer(OBJ 7, m_idx, msg 0xa4,
// 20s, key 0xa4, 0xa4)。返回结果标志。
// ----------------------------------------------------------------------------
// [b28d-54 N] ORIG 0x08144508：槽平移 4 + bool 物化（N82/P197④）。
// [b28d-solo-CBossTower] AE：删 party 局部 + 6 处 `!=1` 改 `!f()`（本类
//   generateRewardExp/generateRewardItem/SendRewardPacket/InsertRewardExp/
//   InsertRewardItems/SendELPLPacket bool 化，ORIG 调用点全 xor）⇒ DIFF→AE。
#include "CBossTower/onClearBossTower.inc"
{
    if (m_party->m_battleField.m_bloodState == 1)
    {
        PacketGuard packet;
        packet.put_header(0, 0xfe);
        packet.finalize(true);
        m_party->send_to_party(packet);
    }
    else
    {
        if (!generateRewardExp())
            return 0;
        if (!generateRewardItem())
            return 0;
        if (!SendRewardPacket())
            return 0;
        if (!InsertRewardExp())
            return 0;
        if (!InsertRewardItems())
            return 0;
    }

    if (!SendELPLPacket())
        return 0;

    m_party->SetEPLPState(2);
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)7, m_idx,
                                (TIMER_MESSAGE)0xa4, 0x14, 0xa4, 0xa4);
    return 1;
}

// ----------------------------------------------------------------------------
// ORIG T 0x08144d54 _ZN8WongWork10CBossTower17generateRewardExpEv：
// 逐有效成员：get_mob_reward(charac_level) 得基础奖励经验后直调基类
// CBossPlay::generateRewardExp(i, m_stageCur, mobReward, m_dungeon)（ORIG
// call 08149e20 非虚）。恒返 1。
// ----------------------------------------------------------------------------
// [b28d-54 N] ORIG 0x08144d54：CBossPlay::generateRewardExp 保持 int ⇒ 缺 ORIG
//   `call; xor $1; test %al; jmp` 死物化（P197④ + P225 dead 变体）；callee
//   bool 化跨 TU（CBossPlay.h 虚槽）不可行。invalid 局部已去、调用序列全等。
// [b28d-solo-CBossTower] AE（66:66，本波翻正）：Main 代落 CBossPlay.h:57
//   generateRewardExp int→bool 后，循环体尾改 P225 dead 变体
//   `while (!f()) break;` 复现 ORIG 0x08144e10 `call; xor $1; test %al,%al; jmp`。
#include "CBossTower/generateRewardExp.inc"
{
    for (int i = 0; i <= 3; ++i)
    {
        CUser* member = m_party->get_user(i);
        // b28d-54 翻正：ORIG 0x08144d7e 段 `test %al,%al; jne` 直测（无 bool 槽）
        // ⇒ 去具名 invalid 局部，条件内联（P0：无槽往返 ⇒ 无具名局部）。
        if (member == 0 || m_party->checkValidUser(i) != 1)
            continue;

        unsigned int mobReward = 0;              // ebp-0x14
        G_CDataManager()->get_mob_reward(member->get_charac_level(), mobReward);
        // [b28d-solo-CBossTower] P225 dead 变体：ORIG 0x08144e10 `call;
        // xor $1,%eax; test %al,%al; jmp`（bool callee 取反物化后无条件跳，
        // test 无消费者）⇒ 源形 `while (!f()) break;`（callee 已由 Main
        // 代落为 bool）。
        while (!CBossPlay::generateRewardExp(
                   i, static_cast<unsigned int>(m_stageCur), mobReward, m_dungeon))
            break;
    }
    return 1;
}

// ----------------------------------------------------------------------------
// ORIG T 0x08144e36 _ZN8WongWork10CBossTower18generateRewardItemEv：
// 逐有效成员：层数奖励上限 = min(10, min(GetTowerScript 槽值,
// getTowerMaxClearRewardItemNum()))，其中 GetTowerScript 返回 it->second+4
// （&stDeathTower_t::m_rateA[0]），ORIG 读 ret+4+(stage+200)*4（stage>=1 落在
// m_cntB 区）；层数参数 = min(10, m_stageCur)；直调基类
// CBossPlay::generateRewardItem(i, itemCount, level, difficulty, 层数参数,
// m_dungeon)（ORIG call 08149914 非虚）。恒返 1。
// ----------------------------------------------------------------------------
// [b28d-54 N] ORIG 0x08144e36：实参/局部装载序（P23/P75）+ bool 死物化（P197④）；
//   invalid 局部已去、调用序列全等。
// [b28d-solo-CBossTower] generated bool 局部 + P225 dead 变体
//   `while (!generated) break;`（Main 代落 callee bool 后复现
//   `mov %al,-0x19; movzbl; xor $1; test; jmp`）+ 实参走 m_difficultyE（union
//   双视角，单段 `mov 0xae8(%eax),%ebx`）+ stageParam 内联。
// [b28f-SweepCBossTower] AE（116:116）：旧残余「itemCount 预清位置 vs 槽号
//   互换」用「scriptLimit **裸声明前置** + 赋值后置」解开——4.4 按声明序
//   「先声明浅」（P309）⇒ scriptLimit=-0x3c（浅）/itemCount=-0x40（深）与 ORIG
//   全等，且 itemCount 的 `movl $0,-0x40` 落回 GetTowerScript 之前
//   （P198③ 两段式镜像；旧「声明前置」形把两槽号互换，实测更劣）。
#include "CBossTower/generateRewardItem.inc"
{
    for (int i = 0; i <= 3; ++i)
    {
        CUser* member = m_party->get_user(i);
        // b28d-54 翻正：同 generateRewardExp（ORIG 直测形，无 bool 槽）。
        if (member == 0 || m_party->checkValidUser(i) != 1)
            continue;

        // [b28d-solo-CBossTower] P331：ORIG 0x08144e91 `movl $0x0,-0x40` 在
        //   GetTowerScript **之前**、itemCount 占深槽 -0x40、scriptLimit -0x3c
        //   ⇒ 同尺寸标量区 4.4 按「先声明→更深槽」排布 ⇒ itemCount 声明前置。
        // 4.4 按「先声明浅」分区（P309）⇒ 槽号对齐需 scriptLimit 声明在前、
        //   itemCount 在后；而 ORIG 的预清 `movl $0,-0x40` 在 GetTowerScript 之前
        //   ⇒ itemCount 用「裸声明分离」不可行（声明序即槽序），改 scriptLimit
        //   裸声明前置、赋值后置（P198③ 两段式的镜像）。
        int scriptLimit;                  // ebp-0x3c（4.4 先声明浅）
        int itemCount = 0;   // ORIG 0x08144e91 循环头预清（P198③ 两段式）

        stDeathTower_t* towerScript = G_CDataManager()->GetTowerScript(
            static_cast<unsigned int>(m_dungeon->get_index()));
        // ORIG 0x08144e36 +0x09d..+0x0a3：`add $0xc8,%edx`（0xc8*4=0x320）后
        // `mov 0x4(%eax,%edx,4),%eax` ⇒ 绝对槽 = towerScript + 4 + 0x320 + 4*stage
        // = stDeathTower_t::m_rewardCardNum[stage]（该数组 +0x324，int 读宽一致）。
        scriptLimit = towerScript->m_rewardCardNum[m_stageCur];

        itemCount = std::min(m_dungeon->getTowerMaxClearRewardItemNum(),
                             scriptLimit);
        itemCount = std::min(10, itemCount);

        // P0/P183：stageParam 无槽往返（ORIG `mov (%eax),%eax; mov %eax,%esi`）
        // ⇒ 内联实参，勿具名 const 局部。
        // ORIG 0x08144fb8 `call; mov %al,-0x1d; movzbl; xor $1; test; jmp`
        // （Ghidra C local_1d = 返回值 bool 局部，结果未用）⇒ 接收返回值。
        // ORIG 0x08144fc9 `mov 0xae8(%eax),%ebx` 单段 ⇒ 实参走 enum 视角
        // m_difficultyE（与形参 ENUM_DUNGEON_DIFFICULTY 同型；int 视角 +
        // 显式 cast 恒产 eax→ebx 两段，探针 /tmp/ctb_probe/p4.o f1/f2）。
        bool generated = CBossPlay::generateRewardItem(
            i, itemCount,
            static_cast<unsigned int>(member->get_charac_level()),
            m_difficultyE,
            static_cast<unsigned int>(std::min(10, (int)m_stageCur)),
            m_dungeon);
        // [b28d-solo-CBossTower] P225 dead 变体：ORIG 0x08144fb8 `mov %al,-0x19;
        // movzbl -0x19,%eax; xor $1,%eax; test %al,%al; jmp`（具名 bool 槽 +
        // 取反物化后无条件跳）⇒ `while (!generated) break;` 取代 `(void)generated`。
        while (!generated)
            break;
    }
    return 1;
}

// ----------------------------------------------------------------------------
// ORIG T 0x0814530c _ZN8WongWork10CBossTower19onChargeRequireItemEP5CUser：
// user==0 → 0；partyType==1 → 1；m_dungeon->m_entryRequiredItem.m_enableFlag(+0x7fc) 非零时：
// find_item(m_entryRequiredItem.m_itemIdx(+0x7f4)) 失败 → 0；make_item 构造物品后按 ORIG
// 用 dungeon 字段覆写 item.m_addInfo(+0x7f4)/m_addInfo2(+0x7f8)，入包
// （reason 4,true,true），slot>=0 时 SendUpdateItemList(target 1, space 0,
// slot)。末尾恒返 1。
// ----------------------------------------------------------------------------
#include "CBossTower/onChargeRequireItem.inc"
{
    if (user == 0)
        return 0;
    if (m_party->m_battleField.m_bloodState == 1)
        return 1;

    if (m_dungeon->m_entryRequiredItem.m_enableFlag != 0)
    {
        CItem* itemData = 0;                        // ORIG 0x8145316 `movl $0x0,-0x10` 先于 item ctor（P191/P198③ 双 init）
        Inven_Item item;                               // ebp-0x4d

        itemData = G_CDataManager()->find_item(
            m_dungeon->m_entryRequiredItem.m_itemIdx);
        if (itemData == 0)
            return 0;

        itemData->make_item(item);                     // 虚槽 vptr+0x08
        item.m_addInfo2 = m_dungeon->m_entryRequiredItem.m_itemCount;  // item+0x07
        item.m_addInfo = m_dungeon->m_entryRequiredItem.m_itemIdx;   // item+0x02（ORIG 0x8145341 先 +0x07 后 +0x02）

        int slot = -1;                                 // ORIG 0x8145337 `movl $0xffffffff,-0xc` 预置

        slot = user->getCurCharacInvenW()->insertItemIntoInventory(
            item, (eItemAddReason)4, true, true);
        if (slot >= 0)
        {
            user->SendUpdateItemList(CUser::eSendTarget_1,
                                     (ENUM_ITEMSPACE)0, slot);
        }
    }
    return 1;
}

// ----------------------------------------------------------------------------
// ORIG T 0x0814512a _ZN8WongWork10CBossTower17onReadyTimerCheckEv：
// 虚调 checkAllMemberReady(get_member_count())（vptr+0x0c）为真 → 0；
// party 空或无成员 → _destroy 返回 0；逐槽位有效且未就绪
// （m_memberReady[i]!=1，基类 +0x11 数组）的成员：onChargeRequireItem +
// 虚调 handleLeaveUser(member)（vptr+0x14）；仍有成员 → 广播包(0,0x1e)/int 0，
// 否则 _destroy；返回 1。
// ----------------------------------------------------------------------------
// [b28d-54 N] ORIG 0x0814512a：被调 int 返回 setne（P197④）+ 槽平移（N82）。
// [b28d-solo-CBossTower] AE（145:145，Main 代落 bool 化后翻正）：CBossPlay.h:60
//   checkAllMemberReady int→bool + invalid 裸条件化 + m_memberReady 检查保留。
#include "CBossTower/onReadyTimerCheck.inc"
{
    if (checkAllMemberReady(m_party->get_member_count()))
        return 0;

    if (m_party == 0 || m_party->get_member_count() < 1)
    {
        _destroy();
        return 0;
    }

    for (int i = 0; i <= 3; ++i)
    {
        CUser* member = m_party->get_user(i);
        // ORIG 0x081451eb..0x081451f7：短路物化岛无 bool 槽 ⇒ 裸条件（P0/P226）。
        if (member == 0 || m_party->checkValidUser(i) != 1)
            continue;
        if (m_memberReady[i] != 1)
        {
            onChargeRequireItem(member);
            handleLeaveUser(member);
        }
    }

    if (m_party->get_member_count() > 0)
    {
        PacketGuard packet;
        packet.put_header(0, 0x1e);
        packet.put_int(0);
        packet.finalize(true);
        m_party->send_to_party(packet);
    }
    else
    {
        _destroy();
    }
    return 1;
}

// ----------------------------------------------------------------------------
// ORIG T 0x08144fca _ZN8WongWork10CBossTower15onReturnVillageEv：
// party 存在且 GetEPLPState()==2 时：成员名表 0xff 清零、逐有效成员记
// LeaveDungeon(dungeon, m_difficulty, names, partyType 低字节)，ReturnToVillage
// 后 _destroy。恒返 1。
// ----------------------------------------------------------------------------
// [b28d-54 N] ORIG 0x08144fca：尾岛/极性 + 大数组清零槽基差（N78/N82）。
// [b28d-solo-CBossTower] m_difficulty 两段已由 union 双视角 + m_difficultyE
//   翻正（`mov 0xae8(%eax),%eax; mov %eax,%ebx`）。
// [b28f-SweepCBossTower N 复核] ORIG 0x08144fde 为无条件 `jmp +0x150`
//   （字节 e9 37 01 00 00，objdump 实证；Ghidra C 亦只给 `return 1`），且
//   全二进制对 `test %eax,%eax` 紧跟 `jmp` 的形仅 5 处（onReturnVillage /
//   CCreatureItemConverter::SetEggItem / CTradeSpace::_IsTradable /
//   CParty::check_allmember_loading / ExportMapScript）⇒ 系 4.1 前端残留
//   死块（同 CTradeSpace 0x08529e54 `and $1;test;jmp` 判例），body 运行期
//   不可达；4.1.2 探针（/tmp/retprobe{,2}.cpp，空 then/嵌套 if/if-else/goto
//   五形）与 4.4 均产不出该形 ⇒ N 封顶。重建保留真实业务语义（body 可达），
//   残余 = 我们多 `je <第二条件>` + 两条 receiver 重载（ORIG 复用 eax=m_party）
//   vs ORIG 单条 `jmp`（104:103）。
// [L5Free49to60 2026-09-11 复核] fresh 103 vs 104 不变；bcmp 唯一差异 = ORIG 头部
//   无条件 `jmp`（死块前端残留）vs 我方 `je <第二条件>; mov 0x8(%ebp),%eax` 两条
//   ⇒ 维持 N（不为凑形态引入伪死块）。
#include "CBossTower/onReturnVillage.inc"
{
    if (m_party != 0 && m_party->GetEPLPState() == 2)
    {
        char memberNames[255] = {0};           // ebp-0x120（ORIG 0x08144fd9 单段
                                               // rep stos 0x3f dwords）

        const char partyTypeByte =
            static_cast<char>(m_party->m_battleField.m_bloodState);   // 低字节 movsbl
        m_party->getMemberNames(memberNames);

        for (int i = 0; i <= 3; ++i)
        {
            CUser* member = m_party->get_user(i);
            // ORIG 0x08145094..0x081450a0：短路物化岛无 bool 槽 ⇒ 裸条件（P0/P226）。
            if (member == 0 || m_party->checkValidUser(i) != 1)
                continue;
            member->m_historyLog.LeaveDungeon(m_dungeon->GetDungeonName(),
                                              m_difficultyE, memberNames,
                                              static_cast<int>(partyTypeByte));
        }
        m_party->ReturnToVillage();
        _destroy();
    }
    return 1;
}

}  // namespace WongWork
