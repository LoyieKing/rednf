// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 LC_Misc/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— LinkClosureC 批次散件 producer（杂项组）。
// 全部函数逐报告/逐指令转写；报告缺失处已用 ORIG objdump 逐指令核实并标注。
// 覆盖符号：
//   Global::CBossTowerFunc::DieMob         T 0x081468aa
//   Secu_HackLogCheck::DieMob              T 0x0827a81c
//   PvPDieUserRevive(int,int,uint,int)     T 0x08636555
//   GetItemIdByAddInfo(Inven_Item*,int,int) W 0x084e91a4
//   Sanicova::CPad::{incFailCnt,resetFailCnt,getPassword,checkAnswer}
//                                          W 0x082652d8/0x082652f2/0x08265326 / T 0x08599712
//   StrikerScript::checkSkillIndex         T 0x08a9e868
//   CEnvironment::getProcessSequence       W 0x08234516
#include "BossTowerFunc.h"

 
 #include <cstring>
 #include <cstdio>
 

#include "CEnvironment.h"       // getProcessSequence 声明
 #include "CUser.h"              // CPad/Secu_HackLogCheck 声明（权威）
#include "CGameManager.h"       // G_CGameManager / GetPvp
#include "Secu_HackLogCheckByParty.h"  // MSG_MONSTER_DIE 完整定义
 #include "GlobalData.h"         // s_pcryptRijndael_MousePassword_
#include "Inven_Item.h"
#include "PvP_Room.h"           // CheckTimerKey/HandleTimerUserRevive
#include "RefPvpGrade.h"
#include "StrikerScript.h"
#include "CSystemTime.h"        // getCurTickCount（SetMonsterInfo/init224/225）
#include "WarField.h"           // （CGameManager/TIMER_MESSAGE 依赖链）

// encryptAES_ —— ORIG T 0x0846c398（MouseRegisterEJ.cpp 定义）。
char* encryptAES_(CRijndael* method, char* src, char* dst);

// ---------------------------------------------------------------------------
// ORIG 0x081468aa（报告 Global__CBossTowerFunc/DieMob.md）：
// b28c-L8 判词 [N(N7)]：fresh 复验 35=35、p167 调用点差集 0/0、sub $0x44 +
// push/pop %ebx 全等。唯一残差 = ORIG 0x081468db `mov (%eax),%ebx`（虚槽 +0x24
// handleKillMonster 目标留 callee-saved %ebx）、实参装载走 %edx/%eax/%ecx；4.4 对间接
// call 目标恒留 %eax 且装载序整体前移一格（%ecx/%edx/%ebx）。P226 语句表达式升格只对
// 「call 结果值」有效、对 call 目标寄存器无效；-march=i486 对 N7 亦无效（Main 口径）
// ⇒ N7 封顶（与 lane12 Secu_MonsterCheck 族同判）。
// getBossTower() 非空 → 虚槽 +0x24（handleKillMonster）(user,mobSeq,param,msg,0)；
// 恒返回 1。
#include "LC_Misc/DieMob.inc"
                                   unsigned short param,
                                   const MSG_MONSTER_DIE& msg, int* out)
{
    WongWork::CBossTower* tower = user->getBossTower();
    if (tower != 0)
        tower->handleKillMonster(user, mobSeq, param, msg, 0);  // ORIG 第5实参=$0
    return 1;
}

// ---------------------------------------------------------------------------
// ORIG 0x0827a81c（报告 Secu_HackLogCheck/DieMob.md）：
// msg->m_fieldF(+0x0f short) != 0xffff 时：SetMonsterInfo(msg)；m_flag223/224/225
// 非 0 者分别以 Check223/224/225 结果刷新。
// [b28c-27] N18a：ORIG 8429xx `call Check223; mov 0x8(%ebp),%edx; mov %al,0x38(%edx)`
// 为 call 返回 char 单字节直存；4.4 恒经 `mov %eax,%edx` 截断中转（+2 条/处）。
// 删 cast、字段 unsigned 化、bool 化均实测不可翻（P168a③b 边界），语义等价。
#include "LC_Misc/DieMob_1.inc"
{
    // [P1T4 #284-303] ORIG 0x0827a81c 首支 `cmp $0xffff,%ax; je 0x0827a895`
    // （je 直指共享 leave），第三支 body（0x0827a892 `mov %al,0x3a(%edx)`，3 字节）
    // 直落 0x0827a895 epilogue、**无** `jmp <leave>`+`nop` 中继岛 ⇒ 原码是**包裹形**
    // `if (msg->m_fieldF != -1) { 全体 }`。旧早退形 `if (f == -1) return;` 在 4.4 下
    // 第三支尾恒多 `jmp +0x1; nop`（实测 42 条 vs ORIG 41 条）。
    // 探针 /tmp/dm_probe.cpp：d1（早退平铺）=42 条含岛；d3（包裹形）=41 条逐条命中
    // ORIG（`je` 直指 leave、body 直落）。语义等价：m_fieldF==-1 时函数直接返回。
    if (msg->m_fieldF != -1)
    {
        SetMonsterInfo(msg);

        // b28d-36 判词（档① callee 侧旋钮，待 Main 代落 CUser.h）：ORIG
        // 0x0827a84x 为 `call Check223; mov 0x8(%ebp),%edx; mov %al,0x38(%edx)`
        // （LHS-first 无中转），要求 callee 返回类型与 uchar 字段**同型**。
        // 探针 /tmp/d36_probe_dm.cpp：t2（返回 unsigned char）逐条命中，t1/t3
        // （返回 char，含调用点 cast 形）恒多 `mov %eax,%edx` 中转（P211 档③）。
        // Check223 尾部 ORIG `movzbl -0x11(%ebp),%eax` 亦证返回 uchar。
        // ⇒ 正解 = CUser.h Check223/224/225 返回 char→unsigned char。
        // [P1T4 #284-303 复核] 已落实：CUser.h:1834-1836 现为 `unsigned char`，
        // 本件 Check223/224/225 消费点无中转（本函数 fdiff blocks=0）。
        if (m_flag223 != 0)
            m_flag223 = Check223();
        if (m_flag224 != 0)
            m_flag224 = Check224();
        // [修订·P1T4] 旧判「P223 边界：epilogue-threading 尾岛 4.4 不可消」已推翻：
        // 真因是首支早退形；改包裹形后第三支 `je 0x0827a895` 与 ORIG 同形，尾岛自然
        // 消失（旧三形试验①否定早返②共享尾标签③空 else 的负结论作废）。
        if (m_flag225 != 0)
            m_flag225 = Check225();
    }
}

// ---------------------------------------------------------------------------
// ORIG 0x08636555（报告缺失；objdump 0x08636555..0x086365d8 逐指令核实）：
//   key = p4 + 0x33
//   room = G_CGameManager()->GetPvp(p1, (CUser*)0, key)
//   room == 0 → return 0
//   if (!room->CheckTimerKey((TIMER_MESSAGE)p4, p2)) return 0
//   room->HandleTimerUserRevive(p4)
//   return 1
// （p3 在 ORIG 中未被读取。）
#include "LC_Misc/PvPDieUserRevive.inc"
{
    (void)p3;
    // P134/P24：p4+0x33 两次均作内联实参（首用跨 G_CGameManager call 由
    // callee-saved 缓存，二次用点重物化），勿具名局部（会多槽丢 push ebx）。
    PvP_Room* room = G_CGameManager()->GetPvp(p1, static_cast<CUser*>(0), p4 + 0x33);
    if (room == 0)
        return 0;
    if (!room->CheckTimerKey((TIMER_MESSAGE)(p4 + 0x33), p2))
        return 0;
    room->HandleTimerUserRevive(p4);
    return 1;
}

// ---------------------------------------------------------------------------
// ORIG W 0x084e91a4（报告缺失；objdump 逐指令核实）：
// for i in 0..count-1: Inven_Item& it = items[i]（stride 0x3d）；
//   it.m_addInfo2(+0x07) == addInfo 且 != 0 → return it.m_addInfo(+0x02)；
// 未命中 return 0。（mangled 参数为非 const 指针 _Z18GetItemIdByAddInfoP10Inven_Itemii。）
#include "LC_Misc/GetItemIdByAddInfo.inc"
{
    for (int i = 0; i < count; ++i)
    {
        // P183：ORIG 每次访问重发 imul $0x3d 全链（无 it 指针槽，P0）。
        if (items[i].m_addInfo2 == addInfo && items[i].m_addInfo2 != 0)
            return items[i].m_addInfo;
    }
    return 0;
}

// ---------------------------------------------------------------------------
// ORIG W 0x0844d54c / 0x0844d55a（objdump：mov 0x134/0x138(%eax); ret）。
// ORIG W 0x082652d8（objdump：+0x38 自增返回新值）。
#include "LC_Misc/incFailCnt.inc"
{
    m_failCnt38 += 1;
    return m_failCnt38;
}

// ORIG W 0x082652f2：写 +0x38 = 0。
#include "LC_Misc/resetFailCnt.inc"
{
    m_failCnt38 = 0;
}

// ORIG W 0x08265326：返回 m_pad（密文首址，非 const）。
#include "LC_Misc/getPassword.inc"
{
    return m_pad;
}

// ORIG T 0x08599712（报告 Sanicova__CPad/checkAnswer.md）：
// 清零 36 字节缓冲 ×2 → strncpy(answer, 8) → encryptAES_ → 与 m_pad 0x24 字节
// strncmp（ORIG call 0x0807e8c0 = strncmp@plt；密文二进制可能含 NUL，
// strncmp 遇 NUL 提前停止——ORIG 语义如此，不得改为 memcmp）。
#include "LC_Misc/checkAnswer.inc"
{
    // b28d-36（修法，P214③/P253）：ORIG 0x08599712 对两缓冲各发内联
    // `mov $0x9,%edx; rep stos`（9 dword=36B）；源码 memset 使 4.4 恒发
    // call memset（N78 触发条件）⇒ 改 `= {0}` 显式零初始化即逐条复现。
    char plain[36] = {0};
    char cipher[36] = {0};

    strncpy(plain, answer, 8);
    encryptAES_(GlobalData::s_pcryptRijndael_MousePassword_, plain, cipher);
    return strncmp(m_pad, cipher, 0x24) == 0;
}

// encryptAES_ —— ORIG T 0x0846c398（MouseRegisterEJ.cpp 定义）。
char* encryptAES_(CRijndael* method, char* src, char* dst);

// ---------------------------------------------------------------------------
// ORIG 0x08a9e868（报告 StrikerScript/checkSkillIndex.md）：
// pass0 → getStrikerSkillVector(first,second)；pass1 → (first,0)；
// 向量为空指针返回 0；命中 m_field00 == skillIndex 返回 1。
#include "LC_Misc/checkSkillIndex.inc"
                                    int skillIndex)
{
    // b28d-36（修法，P212/P191）：ORIG 0x08a9e8xx 函数头连发 `movl $0x0,-0x10;
    // movl $0x0,-0xc` ⇒ vec/pass 均**全头部声明**且各带零初始化（vec 深 -0x10、
    // pass 浅 -0xc，4.1/4.4 全头部同为「先声明深」）；旧形把 vec 放 for 块内
    // 使其占 -0xc、pass 反占 -0x10（槽互换 + 帧 0x48→0x38）。
    std::vector<StrikerSkill>* vec = 0;
    int pass = 0;
    // b28d-solo-LC_Misc 判词 [AE]（修法，P25/P28；旧 b28d-36 N82/P112 帧差判词
    // 作废）：ORIG 尾部 `cmpl $0x1,-0xc; setle %al; test %al,%al; jne` + 入口
    // `jmp <cond>` = 4.1 while 形（for 恒 `cmpl; jle` 直跳、无 setcc 物化），且
    // 增量段调 `ppEi`（后置 `it++` 带独立 sret 临时槽，P28 增量侧推论；原 `++it`
    // 走 `ppEv` 且是 ORIG 无的符号）。原 for 头 `pass = 0` 另生一条重复
    // `movl $0,-0xc`。改 while + it++ 后帧 0x38→0x48、形参 spill 槽与迭代器
    // 槽组同 ORIG（-0x2c/-0x30、-0x1c/-0x18/-0x14），p167 7=7 CLEAR。
    while (pass <= 1)
    {
        if (pass == 0)
            vec = getStrikerSkillVector(firstGrowType, secondGrowType);
        else
            vec = getStrikerSkillVector(firstGrowType, 0);
        if (vec == 0)
            return false;

        for (std::vector<StrikerSkill>::iterator it = vec->begin();
             it != vec->end(); it++)
        {
            if (it->m_field00 == skillIndex)
                return true;
        }
        ++pass;
    }
    return false;
}

// ---------------------------------------------------------------------------
// ORIG W 0x08234516（objdump：mov 0x1ac(%eax),%eax; ret）：
// m_serverEnvir(+0x198) + 0x14 = m_processSequence。
#include "LC_Misc/getProcessSequence.inc"
{
    return m_serverEnvir.m_processSequence;
}

// ---------------------------------------------------------------------------
// ORIG T 0x08a9e7e0 sz 0x88（getStrikerSkillVector.md 全文已读）：
// m_skillMap(+0x78).find((int)(unsigned short)first)；命中返回 it->second
// （vector<StrikerSkill>*），未命中返回 0。ORIG 汇编中 second 形参仅被零扩展
// 存放（-0xe 槽），从未参与 find key——忠实保留未用语义。
#include "LC_Misc/getStrikerSkillVector.inc"
                                                                short second)
{
    // b28d-36（修法）：ORIG 0x08a9e7f4 段把 key 槽（int @-0x10）用**两次 16 位
    // store** 填成 `低半=first / 高半=second`：
    //   movzwl -0x1c,%eax; mov %ax,-0x10 ; movzwl -0x20,%eax; mov %ax,-0xe
    // 然后 find 传 &key（int）。旧形 `ushort uw + int key=(int)uw` 使 4.4 多出
    // uw 槽并 32 位直存 key（`mov %eax,-0x14`，+3 条、全帧浅 4）。
    // 用 union 的 short 视图逐半字写入即逐条复现（P197① union 拆半同族；
    // union 为函数局部 POD，零符号发射）。
    union StrikerSkillKey
    {
        int key;
        struct { unsigned short lo, hi; } parts;
    };
    StrikerSkillKey k;
    k.parts.lo = static_cast<unsigned short>(first);
    k.parts.hi = static_cast<unsigned short>(second);
    std::map<int, std::vector<StrikerSkill>*>::iterator it =
        m_skillMap.find(k.key);
    if (it != m_skillMap.end())
        return it->second;
    return 0;
}

// ===========================================================================
// Secu_MonsterCheck 9 函数族剩余 7 个（SetUserPtr/Reset/Secu_HackLogCheck::Init
// 已在 UserCoreDepsB2.cpp；DieMob 在本文件）。报告 Secu_MonsterCheck/
// {SetMonsterInfo,init223,init224,init225,Check223,Check224,Check225}.md 全文已读。
// ===========================================================================

// ORIG T 0x0827abfa sz 0x80（SetMonsterInfo.md）：
#include "LC_Misc/SetMonsterInfo.inc"
{
    m_curKillCount = msg->m_mobSeq;      // +0x28 ← msg+0x0d
    m_curX = msg->m_killPairA24;         // +0x2a ← msg+0xa24
    m_curY = msg->m_killPairA26;         // +0x2c ← msg+0xa26
    m_setTick = GlobalData::s_systemTime_.getCurTickCount();   // +0x30
    init223(msg);
    init224(msg);
    init225(msg);
}

// ORIG T 0x0827b288 sz 0x5c（init223.md）：active 未置 1 时记录首杀快照。
#include "LC_Misc/init223.inc"
{
    // b28c-lane12 P0 清绑：删 `register unsigned char asm("eax")` 绑定与
    // register 关键字（C28L20A 反例：裸 register 强制 callee-saved 占用破帧）。
    // b28c-L20 复验定稿：Main 已代落 CUser.h:1731 `bool m_active223`（档②字段类型旋钮），
    // 清绑后裸 `!m_active223` 即复现 ORIG @0x0827b294 `movzbl 0x4(%eax),%eax;
    // xor $0x1,%eax; test %al,%al; je` ⇒ fresh 27/27 ndiff=0（AE，原 N 判词作废）。
    if (!m_active223)
    {
        m_field08 = msg->m_mobSeq;       // +0x08 ← msg+0x0d
        m_field0a = msg->m_killPairA24;  // +0x0a ← msg+0xa24
        m_field0c = msg->m_killPairA26;  // +0x0c ← msg+0xa26
        m_field0e = 0;
        m_field10 = 0;
        m_active223 = 1;
    }
}

// ORIG T 0x0827b2e4 sz 0x50（init224.md）
#include "LC_Misc/init224.inc"
{
    // X2 形在此**不适用**（实测 ndiff 3→5 变差，已回退）：本函数体内有
    // `GlobalData::s_systemTime_.getCurTickCount()` 调用，4.4 会为跨 call 存活
    // 的 `act` 额外插一条 spill `mov`，反而多出一条。init223 无 call 故能闭合。
    // ⇒ X2 第二前提：**判断与 body 之间不得有 call**（否则 %eax 绑定值需 spill）。
    // CUser.h 已落 bool；bool+`!field` 复现 ORIG `xor $1;test %al`（P22），
    // 且从内存重取无跨 getCurTickCount call spill。
    if (!m_active224)
    {
        m_field12 = msg->m_mobSeq;       // +0x12 ← msg+0x0d
        m_field14 = GlobalData::s_systemTime_.getCurTickCount();
        m_field18 = 0;
        m_field1a = 0;
        m_active224 = 1;
    }
}

// ORIG T 0x0827b334 sz 0x50（init225.md）
#include "LC_Misc/init225.inc"
{
    // X2 形不适用，同 init224（body 内有 getCurTickCount 调用 ⇒ 多一条 spill mov）。
    // CUser.h 已落 bool；bool+`!field` 复现 ORIG `xor $1;test %al`（P22）。
    if (!m_active225)
    {
        m_field1c = msg->m_mobSeq;       // +0x1c ← msg+0x0d
        m_field20 = GlobalData::s_systemTime_.getCurTickCount();
        m_field24 = 0;
        m_field26 = 0;
        m_active225 = 1;
    }
}

// ORIG T 0x0827ad98 sz 0x1e0（Check223.md）：击杀数快照一致 → 1；
// 坐标/击杀差值阈值判定 → 计数超限上报 0xdf 并清状态返 0，否则重置快照返 1。
// [b28c-lane12 判词·DIFF(N)，Main 分派件] 接手后三处修正 + 残差定性：
//  ① P196 符号性鉴别：ORIG @0x0827ae60/0x0827aec6 的 `cmp $0xa,%ax; jbe` /
//     `cmp $0x4,%ax; jbe` 为 **16 位无符号**域，4.4 把 ushort 提升 int 后有符号
//     ⇒ 比较改写 unsigned 域后 jbe 族回归。值域 0..N 不越 0x7fff ⇒ 非真语义 bug。
//  ② P222/P47 块序翻正：ORIG @0x0827ae09 `jg +0xa1`（dx>0 跳远处 setne 块）⇒
//     外层改否定比较 `if (dx <= 0)`，内层 `-dx>0xc7`/`dy<=0` 同为 jg 跳走。
//  ③ 三出口嵌套三目汇合（无具名 bool）：令 4.4 三出口 setle/setne 留 %al 并统一
//     test/je，消除具名 `bool moveOk` 的 -0x9 槽物化与 4 字节帧漂（P226 应用例）。
//  残差 2 行（136=136）：`if (m_field0e > 4) m_field10 = 1;` 的 ORIG `jbe 尾`
//  vs 4.4 镜像 `ja body`；三形互试（>4 body / `<=4 goto 尾空标签` / BM8 外层
//  标签）4.4 恒折 ja ⇒ P222 镜像边界 + P223 空块岛被 jump-thread 吞，封顶。
#include "LC_Misc/Check223.inc"
{
    if (m_field08 == m_curKillCount)
        return 1;
    char result = 1;
    // b28c Main（采纳 C28L7 spec）：m_active223 已 bool 化，此处须**裸测**——
    // `!= 0` 使 4.4 物化 `cmpl $0,-off; setne %al; mov %al,-off2`（ORIG 为
    // `cmpl $0,-0x10; jg` 直分支），并连带局部槽整体平移 4（P219/P22 同族）。
    if (m_active223)
    {
        int dx = static_cast<int>(static_cast<unsigned short>(m_field0a)) -
                 static_cast<int>(static_cast<unsigned short>(m_curX));
        int dy = static_cast<int>(static_cast<unsigned short>(m_field0c)) -
                 static_cast<int>(static_cast<unsigned short>(m_curY));
        // P222/P47 块序翻正 + 三目汇合：ORIG @0x0827ae09..0x0827ae40 三出口
        // 各自 `setle/setne %al` 到**寄存器**再统一 `test %al,%al; je`
        // （dx>0 → 远处 setne 块，内层 `-dx>0xc7`/`dy<=0` 同为 jg 跳走）。
        // 具名 bool moveOk 会被 4.4 落槽（-0x9 槽 + cmpb/ja + 帧漂 4），
        // 故把三出口写成嵌套三目直接作 if 条件，令值留 %al。
        if (dx <= 0 ? (-dx > 0xc7 || dy <= 0 ? (-dy <= 0x95) : (dy != 0))
                    : (dx != 0))
        {
            m_field0e += 1;
            // P196 符号性鉴别：ORIG @0x0827ae60/0x0827aec6 为 `cmp $0xa,%ax;
            // jbe` / `cmp $0x4,%ax; jbe`（**16 位无符号**域），4.4 把 ushort
            // 提升 int 后恒产有符号 `jle`。值域 0..N 不越 0x7fff ⇒ 非真语义
            // bug，但为对齐 ORIG 需把比较留在 unsigned 域。
            // b28d-36（P222 外层翻正）：ORIG 0x0827ae60 `cmp $0xa,%ax;
            // jbe +0x127` ⇒ **else 块（>10，含上报/清零）物理在前、then 块
            // （<=10 的 >4 分支）在 +0x127**。旧写 `if (<=10) {…} else {…}`
            // 使 4.4 把 then 放前（镜像）。外层条件反写 `if (>10) {else-body}
            // else {then}` 后 4.4 直产 `jbe +0x127` 与 ORIG 逐条一致。
            if ((unsigned int)m_field0e > 0xa)
            {
                m_field10 = 3;
                if (m_user != 0)
                {
                    m_user->getHackAnalyzer()->addServerHackCnt(
                        m_user, static_cast<WongWork::ENUM_HACKTYPE>(0xdf),
                        3, 0, 0);
                }
                result = 0;
            }
            else
            {
                if ((unsigned int)m_field0e > 4)
                    m_field10 = 1;
            }
        }
        else
        {
            if (m_field10 == 1 && m_user != 0)
            {
                m_user->getHackAnalyzer()->addServerHackCnt(
                    m_user, static_cast<WongWork::ENUM_HACKTYPE>(0xdf),
                    1, 0, 0);
            }
            m_field08 = m_curKillCount;
            m_field0a = m_curX;
            m_field0c = m_curY;
            m_field0e = 0;
            m_field10 = 0;
        }
    }
    return result;
}

// ORIG T 0x0827af78 sz 0x187（Check224.md）：击杀间隔 1000ms 判定，上报 0xe0。
#include "LC_Misc/Check224.inc"
{
    if (m_field12 == m_curKillCount)
        return 1;
    char result = 1;
    // CUser.h 已落 bool；bare `if(field)` 复现 ORIG `movzbl;test %al`（8 位，
    // `!=0` 对 bool 会生 movzbl+movzbl+testl 异形）。
    if (m_active224)
    {
        int dt = m_setTick - m_field14;
        if (dt <= 0x3e8)
        {
            m_field18 += 1;
            // ORIG 块排布：两 cmp 直跳 else（P133 合并短路 fall-through =
            // v=1 块），v=3 在 else-if；反向嵌套 if 形会把 v=1 块挤到尾部。
            if (m_field18 > 4 && m_field18 <= 0xa)
            {
                m_field1a = 1;
            }
            else if (m_field18 > 0xa)
            {
                m_field1a = 3;
                if (m_user != 0)
                {
                    m_user->getHackAnalyzer()->addServerHackCnt(
                        m_user, static_cast<WongWork::ENUM_HACKTYPE>(0xe0),
                        3, 0, 0);
                }
                result = 0;
            }
        }
        else
        {
            if (m_field1a == 1 && m_user != 0)
            {
                m_user->getHackAnalyzer()->addServerHackCnt(
                    m_user, static_cast<WongWork::ENUM_HACKTYPE>(0xe0),
                    1, 0, 0);
            }
            m_field18 = 0;
            m_field1a = 0;
        }
        m_field12 = m_curKillCount;
        m_field14 = m_setTick;
    }
    return result;
}

// ORIG T 0x0827b100 sz 0x187（Check225.md）：击杀间隔 2000ms 判定，上报 0xe1。
#include "LC_Misc/Check225.inc"
{
    if (m_field1c == m_curKillCount)
        return 1;
    char result = 1;
    // CUser.h 已落 bool；bare `if(field)` 复现 ORIG 8 位 test。
    if (m_active225)
    {
        int dt = m_setTick - m_field20;
        if (dt <= 1999)
        {
            m_field24 += 1;
            // 同 Check224：ORIG v=1 块在 fall-through，v=3 在 else-if。
            if (m_field24 > 4 && m_field24 <= 0xa)
            {
                m_field26 = 1;
            }
            else if (m_field24 > 0xa)
            {
                m_field26 = 3;
                if (m_user != 0)
                {
                    m_user->getHackAnalyzer()->addServerHackCnt(
                        m_user, static_cast<WongWork::ENUM_HACKTYPE>(0xe1),
                        3, 0, 0);
                }
                result = 0;
            }
        }
        else
        {
            if (m_field26 == 1 && m_user != 0)
            {
                m_user->getHackAnalyzer()->addServerHackCnt(
                    m_user, static_cast<WongWork::ENUM_HACKTYPE>(0xe1),
                    1, 0, 0);
            }
            m_field24 = 0;
            m_field26 = 0;
        }
        m_field1c = m_curKillCount;
        m_field20 = m_setTick;
    }
    return result;
}
