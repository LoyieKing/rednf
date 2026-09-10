// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CBossPlay/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "CBossPlay.h"

#include <cmath>

#include "CParty.h"
#include "CUser.h"
#include "CDungeon.h"
#include "CDataManager.h"
#include "PacketGuard.h"
#include "GlobalData.h"
#include "CSystemTime.h"
#include "GameWorld.h"
#include "CGuildServerProxy.h"   // CEventBase / CEventManager（GetRepeatEvent）
#include "CServerEvent.h"
#include "CEnvironment.h"
#include "CBurningFatigueEvent.h"
#include "CExpDoubleEvent.h"
#include "CPCRoomBurningEvent.h"
#include "LogManager.h"
#include "CItemGeneratorMgr.h"
#include "CBattle_Field_shared.h"
#include "GameRand.h"   // get_rand_int

// stClearRewardExpParameter_t / stClearRewardBonusExp_t 权威定义已迁至
// CParty.h（FixBossRewardExpDeps 域），此处不再本地定义。


namespace WongWork
{

namespace
{

const float kPercentDivisor = 100.0f;   // _DAT_08b7507c

// CItemGenerator 虚调用已由权威头 CItemGenerator.h 提供（generateRandomItem/
// generateSpecificItem 虚槽 +0x08/+0x0c），不再使用原始虚表跳转。
// CEventBase 判活改经真实虚函数 IsEventing(CUser*)（vtable+0x34）；
// CPCRoomBurningEvent::GetExpFactor 改真实类调用（权威头 CPCRoomBurningEvent.h）。
}  // namespace

// ============================================================================
// 生命周期
// ============================================================================

// ============================================================================
// reset（ORIG 0x08149120）
// ============================================================================
#include "CBossPlay/reset.inc"
{
    m_party = 0;
    m_field8 = 0;
    m_allMemberReady = 0;
    memset(m_memberAlive, 0, sizeof(m_memberAlive));
    memset(m_memberReady, 0, sizeof(m_memberReady));
    memset(m_rewardExp, 0, sizeof(m_rewardExp));
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 10; ++j)
        {
            m_memberReward[i].m_count = 0;
            m_memberReward[i].m_items[j].reset();
            // ORIG 0x08149202 `movl $0xffffffff,0xe(%eax)`（eax=block+j*0x3d+0x20，
            // 即 &m_items[j]+0x02）⇒ 整字写 Inven_Item::m_addInfo(item idx) = -1。
            m_memberReward[i].m_items[j].m_addInfo = -1;
        }
    }
    memset(m_useItemCount, 0, sizeof(m_useItemCount));
    memset(m_memberField, 0, sizeof(m_memberField));
}

// stRewardItem_t ctor（ORIG W 0x081519b6）：构造 10 个 Inven_Item
// （ORIG 为编译器对 Inven_Item m_items[10] 数组成员的自动构造，body 为空）。
#include "CBossPlay/stRewardItem_t.inc"
{
}

// Ctor（ORIG W 0x081519ec）：置 vptr（编译器自动），构造 4 个 stRewardItem_t 块
// （编译器在进入函数体前自动调用 stRewardItem_t ctor），再调 reset()。
#include "CBossPlay/CBossPlay.inc"
{
    reset();
}

// Dtor（ORIG W 0x08151a3a）：置 vptr 后调 reset()。
#include "CBossPlay/dtor_CBossPlay.inc"
{
    reset();
}

// ============================================================================
// InitPlayUser（ORIG 0x0814926e）
// ============================================================================
#include "CBossPlay/InitPlayUser.inc"
{
    for (int i = 0; i <= 3; ++i)
    {
        CUser* user = m_party->get_user(i);
        // [b28d-46 实测] ORIG 单条短路 or 守卫（Ghidra bVar1 汇聚）：
        // `cmpl $0,-0xc; je T` + `call; xor $1; test; je F` + `T:mov $1/jmp;
        //  F:mov $0; test %al; jne skip` ⇒ 合并 `||` 形。
        if (user == 0 || !m_party->checkValidUser(i))
            continue;
        {
            // [b28d-46 P52/P200] ORIG receiver 是**内联** get_user 调用结果
            // （0x081492e5 call 后 eax 直作 this），towerIdx 短整截断先物化
            // `movswl %ax,%ebx` 并跨该 receiver call 保活 ⇒ 禁具名 receiver 局部。
            m_party->get_user(i)->setBossTowerIndex(towerIdx);
            // ORIG 连续两次 call CUser::getCurCharacQuestW（0x081492f1 与
            // 0x0814930d），结果不入局部变量：第一次 `add $0x75d8,%eax`
            // + call _Quest_Authen_Data::reset ⇒ 具名成员 UserQuest::m_authenData
            //（CUser.h 权威布局 +0x75d8）；第二次直接 set_authen_data。
            user->getCurCharacQuestW()->m_authenData.reset();
            user->getCurCharacQuestW()->set_authen_data(
                (QUEST_CONDITION)0, questCond, 0xffffffff);
            user->set_state((ch_state)0xc);
            setMemberAlive(i, true);
        }
    }
}

// ============================================================================
// _checkMemberDie（ORIG 0x08149382）
// ============================================================================
#include "CBossPlay/_checkMemberDie.inc"
{
    // ORIG 0x08149396 双 init（P191 配套）；条件链 test/je 直落 mov$1/mov$0
    // 岛（无 xor 物化 → 正向 && 短路）；尾块 i==4 双岛 return（P56）。
    unsigned int i = 0;
    for (i = 0; i < 4; ++i)
    {
        if (m_party->checkValidUser(i) && isMemberAlive(i))
            break;
    }
    if (i == 4)
        return true;
    return false;
}

// ============================================================================
// checkAllMemberReady（ORIG 0x081493fe）
// ============================================================================
#include "CBossPlay/checkAllMemberReady.inc"
{
    if (m_allMemberReady)
        return 1;
    int ready = 0;
    for (int i = 0; i <= 3; ++i)
    {
        if (m_memberReady[i])
        {
            ++ready;
            if (ready == count)
            {
                m_allMemberReady = 1;
                return 1;
            }
        }
    }
    return 0;
}

// ============================================================================
// getLiveMemberCount（ORIG 0x08149466）
// ============================================================================
#include "CBossPlay/getLiveMemberCount.inc"
{
    int count = 0;
    for (int i = 0; i <= 3; ++i)
    {
        if (m_memberAlive[i])
            ++count;
    }
    return count;
}

// ============================================================================
// handleDieUser（ORIG 0x081494a4）
// [b28d-46 判词·N/P197④+P168a③b+N82] 168 vs 172，p167 CLEAR（29/29）。
// for(i=0;i<=3;++i)+continue 形复现双初始化（P191）与尾部 setle 物化（P51）。
// [b21 N·P197-④] 语义已闭合：找成员循环 `checkValidUser(i) && get_user(i)==user`
//   的取反判定——ORIG(4.1) 发 `sete %al; test; je`，4.4-O0 对 call 结果直比恒发
//   `mov $1/mov $0 + test/jne`（P197-④：源形不可复现，禁改内联比较=双调用真 bug）；
//   循环头 `movl $0,-0xc` 双写为 4.1 代码生 quirk（int i=0）。返回路径 ORIG 在各
//   return 点直发 `mov $0x0/%1,%eax`+共享尾，4.4 以 %ebx 合并 epilogue（N82/寄存器分配）。
//   while(true){if(3<i)return 1;...break;++i;} 形态 + 时间比较改 `GetRevivalTime() >
//   getCurSec()` 已对齐 4.1 求值序/setg 方向；逐调用逐分支语义逐一相等，封 N。
// ============================================================================
#include "CBossPlay/handleDieUser.inc"
{
    // [b28d-46 实测] ORIG 双 `movl $0,-0xc`（P191）+ 尾部 `cmpl $3; setle; test;
    // jne`（P51）+ 两独立守卫各 `je INC`（P133）⇒ for + continue 形。
    int i = 0;
    for (i = 0; i <= 3; ++i)
    {
        if (!m_party->checkValidUser(i))
            continue;
        if (!(m_party->get_user(i) == user))
            continue;
        if (isMemberAlive(i))
        {
        setMemberAlive(i, false);

        PacketGuard packet1;
        packet1.put_header(0, 0x20);
        packet1.put_short(user->get_unique_id() & 0xffff);
        packet1.put_byte(0);
        packet1.finalize(true);
        m_party->send_to_party(packet1);

        if (user->GetRevivalTime() > GlobalData::s_systemTime_.getCurSec())
        {
            user->SetRevivalTime(0);
            user->SetUseAPCPotionTime(0);
            setMemberAlive(i, true);

            PacketGuard packet2;
            packet2.put_header(0, 0x20);
            packet2.put_short(user->get_unique_id() & 0xffff);
            packet2.put_byte(2);
            packet2.finalize(true);
            m_party->send_to_party(packet2);
        }
        return 1;
        }
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x2b, 0x12);
        return 0;
    }
    return 1;
}

// ============================================================================
// handleLeaveUser（ORIG 0x08149724）
// [N18b 封顶] ORIG 0x08149771 对 get_user(i)==user（call 结果 vs 形参比较）生成
// `cmp 0xc(%ebp),%eax; sete %al; test` 物化；4.4 恒折直 cmp+jne 双岛，无源码写法
// 可产 sete（N18b 判据），其余 128 条逐条同形。
// [b27 复证] N93附 尾块序亦不适用（此处非 return 块而是 && 汇聚物化；
// 探针 44 再证 4.4 双岛恒 jne+mov$1/mov$0）。维持封顶。
// [b27 DL3R] 返回类型 int→bool（ORIG 调用点 CBossTower 0x08143b04 `test %al`
// 单字节直判实证）：callee 行数 128:131 两型实测完全一致（零成本），受益方为
// 调用点 xor 消形。
// ============================================================================
#include "CBossPlay/handleLeaveUser.inc"
{
    int i = 0;
    for (i = 0; i <= 3; ++i)
    {
        // P133：ORIG 两守卫独立 jne（checkValidUser 直跳 + get_user==user
        // sete 岛），合并 && 生 bool 展开形。拆两条 + 直比（同型件
        // CDeathTower::handleLeaveUser 清绑后 AE 先例）。
        if (m_party->checkValidUser(i))
        {
        if (m_party->get_user(i) == user)
        {
            if (m_memberField[i] == 0 && user->get_state() == 0xc)
            {
                user->getHackAnalyzer()->addServerHackCnt(user, (ENUM_HACKTYPE)0x13a, 1, 0, 0);
            }
            m_party->leave_user(user, (ENUM_PARTY_INFO_TYPE)2);
            setMemberAlive(i, false);
        }
        }
    }

    G_GameWorld()->out_from_dungeon(user);

    PacketGuard packet;
    packet.put_header(0, 2);
    packet.put_byte(0);
    packet.put_short(1);
    user->make_basic_info((char*)&packet, '\0');
    packet.finalize(true);
    G_GameWorld()->send_all(packet, user);
    return 1;
}

// ============================================================================
// generateRewardExp（ORIG 0x08149e20）
// [b28d-46 判词·N/N82] 244 vs 267，p167 CLEAR（32/32）。已修：baseExp 具名 float
// （P252 复现 fstps -0xf0 槽往返）、tower 内联表达式（P0/P183 无槽直用 %eax）。
// 残差 = 帧 0x13c vs 0x174 全帧槽号平移（N82）＋ `1+a` 下标 add vs disp 折叠（P30/N4）⇒ 封顶。
// ============================================================================
#include "CBossPlay/generateRewardExp.inc"
                                 const CDungeon* dungeon)
{
    CUser* user = m_party->get_user(memberIdx);
    if (user == 0)
        return 0;

    // 基础经验 = round((float)(a*b) × 死亡之塔费率)。GetTowerScript 返回结构体基址，
    // ORIG flds 0x4(%eax,%edx,4) 读 m_rewardExpWeight[1+a]（[reward exp weight] 表，
    // 脚本下标从 1 起）；x87 fistpll 就近舍入，直接 (long long) 强转等价。
    unsigned int clearExp;
    {
        // [b28d-46 P252 实测] ORIG 先 fildll(a*b)→fstps 具名 float 槽 -0xf0，再取
        // tower 并 fmuls mem；具名 float 局部复现该槽往返。
        float baseExp = (float)(long long)((unsigned int)(a * b));
        // ORIG tower 无槽往返（GetTowerScript 结果直用 %eax）⇒ 内联表达式（P0/P183）。
        clearExp = (unsigned int)(long long)(
            baseExp * G_CDataManager()->GetTowerScript(
                dungeon->get_index())->m_rewardExpWeight[1 + a]);
    }

    stClearRewardBonusExp_t bonus;
    stClearRewardExpParameter_t param;
    float avatarBonus = (float)m_party->getAvatarBonus();
    float creatureBonus = (float)m_party->getCreatureBonus(user);

    float serverRate = (float)CServerEvent::GetExpRate() / kPercentDivisor;

    CDataManager* dm2 = G_CDataManager();
    channel_script_t* chScript = (channel_script_t*)dm2->GetChannelScript();
    CEnvironment* env = G_CEnvironment();
    unsigned int channelNo = (unsigned int)env->get_channel_no();
    // 渠道组字节：ORIG 重取 G_CEnvironment() 并内联读 +0x378（= Server_Envir
    //             m_serverGroup，+0x198+0x1e0），不 call get_server_group()。
    unsigned char channelType =
        (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup;
    float channelRate =
        chScript->getBonusRate(channelType, channelNo, dungeon->get_index());

    float pcrBurning = 0.0f;
    float expDouble = 0.0f;
    // [L4-r3 判据⑫] widthscan: clr_l orig=0 / our=1 ⇒ ORIG 无任何 int 具名局部的
    // `=0` 初始化（我方多 1 条 `movl $0,-N`）⇒ 去初始化器（赋值点在使用前）。
    int burningRate;
    float premiumRate = 0.0f;

    CEventBase* ev8 = GlobalData::s_event_manager->GetRepeatEvent(8);
    if (ev8->IsEventing(user))
        // ORIG：整型 /100（imul 0x51eb851f 形态）后才转 float
        pcrBurning = (float)(((CPCRoomBurningEvent*)
            GlobalData::s_event_manager->GetRepeatEvent(8))->GetExpFactor() / 100);

    CEventBase* ev3 = GlobalData::s_event_manager->GetRepeatEvent(3);
    if (ev3->IsEventing(0))
        expDouble = (float)((CExpDoubleEvent*)
            GlobalData::s_event_manager->GetRepeatEvent(3))->GetExpFactor() / kPercentDivisor;

    CEventBase* evF = GlobalData::s_event_manager->GetRepeatEvent(0xf);
    if (evF->IsEventing(0))
        burningRate =
            ((CBurningFatigueEvent*)
                GlobalData::s_event_manager->GetRepeatEvent(0xf))->getBonusExpRate(user->getCurCharacUsedFatigue());

    if (user->IsHavePremiumAdvantage())
    {
        WongWork::CUserPremium* pre = user->GetPremiumInfo();
        premiumRate = (float)pre->GetAdvantageExpRate() / kPercentDivisor;
    }

    unsigned int total = m_party->getClearRewardBonusExp(user, clearExp, param, bonus);

    if (0x10c8e0 < total)
    {
        LogManager::logFormat(1, "localglobal/global_PlayUser.cpp",
            __PRETTY_FUNCTION__,
            0x159, "BOSS_LOG : EROOR BOSSTOWER CLEAR REWARD EXP(%u)\n", total);
        LogManager::logFormat(1, "localglobal/global_PlayUser.cpp",
            __PRETTY_FUNCTION__,
            0x163, "BOSS_LOG : BOSSTOWER EXP PARAMETER : %f, %f, %f, %f, %f, %f, %f, %d\n",
            (double)avatarBonus, (double)creatureBonus, (double)pcrBurning, (double)expDouble,
            (double)channelRate, (double)premiumRate, (double)serverRate, burningRate);
        total = 0x10c8e0;
    }
    m_rewardExp[memberIdx] = (int)total;
    return 1;
}

// ============================================================================
// generateRewardItem（ORIG 0x08149914）
// [b28d-46 判词·N(死 ebx 物化) + 3 真 bug] 327 vs 324（gap 3）、p167 44:44 CLEAR
// （仅 C1/D1↔C2/D2 同址别名，P93）、帧 0x13c 两侧全等。本轮修：
// ① 真 bug·虚槽互换：ORIG vtable `_ZTVN8WongWork15CItemGeneratorE` @0x08c9a788
//    word[4]=0x0853a400 generateRandomItem（vptr+0x08）、word[5]=0x0853a406
//    generateSpecificItem（vptr+0x0c）；ORIG 段 A（0x08149a87 `add $0xc`）与
//    段 B（0x08149c03 `add $0x8`）各调其一，原实现两分支调用名互换。
// ② 真 bug·舍入：ORIG 0x08149b18 为 `fildl; fmuls; 设截断 CW; fistpl`
//    （= (int)(float) 截断），原 ROUND(+0.5f) 宏（fadd 0.5 + fistpll）错误。
// ③ 形态：外层条件原 `< || ==` 双算 imul 且块序反 ⇒ 单条件 + 随机块 fall-through
//    （P328⑧/P272）；内层同理改 `rare2*b > randVal`。
// ④ P0/P52/P248：rare/rare2/randVal 同型化 unsigned（消 edx 中转）、rare 三段链式
//    （消 dungeonIdx/dm/tower 具名槽）、ref/result 先于 get_rand_int 构造、
//    dst 去具名引用（逐点重算地址）、特定分支补 `Inven_Item rewardItem = *begin()`
//    值拷贝（ORIG 0x08149c9b..0x08149d25 共 17 条 mov）、随机分支独立 randResult
//    对象（-0x9c 自带 ctor/dtor）、删 m_party 判空（P140/P171）、
//    addInfo 回写同槽（ORIG fildl/fistpl 同址 -0x12c）。
// 残差 gap 3 = ORIG 段 A/B 公共出口 `mov $0x1,%ebx` / item==0 路径 `mov $0x0,%ebx`
//  + 循环尾 `test %ebx,%ebx`（**无消费者死物化**：4.1 条件值保留；ebx 全程无栈槽
//  ⇒ 非具名局部，4.4 无对应源形，同 P197④/跨版本族）⇒ 语义闭合，封 N。
// ============================================================================
#include "CBossPlay/generateRewardItem.inc"
                                  ENUM_DUNGEON_DIFFICULTY diff, unsigned int b,
                                  const CDungeon* dungeon)
{
    m_memberReward[memberIdx].m_count = itemCount;
    // [b28d-46 P248 档③] ORIG 比较段 `mov rare,%eax; imul b,%eax; cmp randVal,%eax`
    // 无 edx 中转 ⇒ rare/rare2 与 b 同型（unsigned int）。
    unsigned int rare = 0;
    unsigned int rare2 = 0;
    float rare3 = 0.0f;
    // 掉宝概率三档累计表 / 金币费率（ORIG 0x08149971/0x08149992/0x081499b9
    // 分别 `mov 0xc88(%eax)`、`mov 0xc8c(%eax)`、`mov 0xc94(%eax)`，
    // 全为 4 字节读 ⇒ m_rewardItemProb[0]/[1]（int）、m_rewardGoldWeight（float））。
    // [b28d-46 P0/P52] ORIG 无 dungeonIdx/dm/tower 具名局部：get_index 结果留
    // %ebx 跨 G_CDataManager call 保活（`mov %eax,%ebx` 无栈往返）⇒ 逐条链式。
    rare = G_CDataManager()->GetTowerScript(dungeon->get_index())->m_rewardItemProb[0];
    rare2 = G_CDataManager()->GetTowerScript(dungeon->get_index())->m_rewardItemProb[1];
    rare3 = G_CDataManager()->GetTowerScript(dungeon->get_index())->m_rewardGoldWeight;
    for (int i = 0; i < itemCount; ++i)
    {
        // [b28d-46 P0] ORIG 构造序：ref ctor → result ctor → get_rand_int
        // （0x081499ce/0x081499e2 在 0x081499ee 之前）⇒ 标量声明置于对象之后。
        stGenerateRefData_t ref;
        stGenerateResult_t result;
        unsigned int randVal = (unsigned int)get_rand_int(1000);

        // [b28d-46 P328⑧] ORIG 单次 imul + `cmp -0x20(%ebp),%eax; jbe`（无符号
        // `rare*b <= randVal`）；原 `< || ==` 写法双算 imul 且块序反。
        // [b28d-46 P272] ORIG 块序：随机块 fall-through、`jbe` 落特定判断块
        // ⇒ 源码须写成 `if (rare*b > randVal) {随机} else if (rare2*b > randVal)
        // {特定}`；嵌套 `<=` 形 4.4 反产 `ja 随机块`（块序相反）。
        if ((unsigned int)(rare * b) > (unsigned int)randVal)
        {
            // 随机掉落分支（ORIG +0x0f2 起，ref 基址同上 -0x124）：
            //   movb -0xef ⇒ +0x35=difficulty；movb -0xf0 ⇒ +0x34=a；
            //   movb -0xee ⇒ +0x36=member_count；movl $0x1 -0xec ⇒ +0x38=1；
            //   4×movw $0x1 ⇒ +0x3c/+0x3e/+0x40/+0x42 各 =1
            //   （+0x40/+0x42 合成指针位型 0x10001，随机路径不解引用）；
            // 虚槽 +0x8 = generateRandomItem。
            ref.m_field35 = (char)(unsigned char)diff;
            ref.m_field34 = (char)a;
            // [b28d-46 P140/P171] ORIG 无 m_party 判空（0x08149a18 直接
            // `mov 0x4(%eax),%eax; mov %eax,(%esp); call get_member_count`）。
            ref.m_rateSlot = (char)m_party->get_member_count();
            ref.m_rankBonusIndex = 1;
            ref.m_goldBase = 1;
            ref.m_goldFactor = 1;
            ref.m_field40 = 1;
            ref.m_field42 = 1;
            // [b28d-46] ORIG 随机分支自带独立 result 对象（-0x9c，ctor 0x08149a63 /
            // dtor 0x08149b7f 均在分支内），与循环级 result（-0x6c）并存。
            // [b28d-46 真 bug] ORIG vtable（_ZTVN8WongWork15CItemGeneratorE @0x08c9a788）
            // word[4]=0x0853a400 generateRandomItem（vptr+0x08）、word[5]=0x0853a406
            // generateSpecificItem（vptr+0x0c）；本段 ORIG 0x08149a87 `add $0xc`
            // ⇒ 调 generateSpecificItem（原实现调 generateRandomItem 为虚槽取错）。
            stGenerateResult_t randResult;
            G_CDataManager()->getItemGenerator()->getGenerator(
                CItemGeneratorMgr::eGenerateType_2)->generateSpecificItem(ref, randResult);
            m_memberReward[memberIdx].m_items[i].setCopy(
                *randResult.m_dropItems.begin());
            int addInfo = m_memberReward[memberIdx].m_items[i].get_add_info();
            // [b28d-46 真 bug] ORIG 0x08149b18 段为 `fildl; fmuls; 设截断 CW;
            // fistpl`（无 fadd 0.5、无 fistpll）⇒ 原码是 `(int)(float)` 截断，
            // 原 ROUND(+0.5f) 宏为舍入错误（少截断多 flds/faddp 两条）。
            // [b28d-46 P0] ORIG `fistpl -0x12c` 与 `fildl -0x12c` 同槽 ⇒ 结果
            // 回写 addInfo 局部再传参（非匿名临时槽）。
            addInfo = (int)((float)addInfo * rare3);
            m_memberReward[memberIdx].m_items[i].set_add_info(addInfo);
        }
        // ORIG 内层同样单次 imul：`mov rare2,%eax; imul b; cmp randVal; jbe 跳过`
        // ⇒ 单条件 `rare2*b > randVal` 才进特定掉落（特定块 fall-through）。
        else if ((unsigned int)(rare2 * b) > (unsigned int)randVal)
        {
            // 特定掉落分支（ORIG +0x2a0 起，ref 基址 = 本帧 -0x124，
            // ctor `lea -0x124(%ebp),%eax` 实证）：
            //   movb -0xef ⇒ +0x35=difficulty；movb -0xf0 ⇒ +0x34=a；
            //   mov $0x3f800000/movl -0xec ⇒ +0x38=float 1.0f；
            //   movl -0xe4 ⇒ +0x40=dungeon；movl $0x0 -0xe8 ⇒ +0x3c=0；
            // 虚槽 +0xc = generateSpecificItem。
            ref.m_field35 = (char)(unsigned char)diff;
            ref.m_field34 = (char)a;
            ref.m_rankBonusRate = 1.0f;
            ref.m_dungeon = const_cast<CDungeon*>(dungeon);
            ref.m_field3c = 0;
            // [b28d-46 真 bug] ORIG 0x08149c03 `add $0x8` ⇒ vptr+0x08 =
            // generateRandomItem（原实现调 generateSpecificItem 为虚槽取错）。
            G_CDataManager()->getItemGenerator()->getGenerator(
                CItemGeneratorMgr::eGenerateType_2)->generateRandomItem(ref, result);
            if (!result.m_dropItems.empty())
            {
                // [b28d-46 P167] ORIG 调 vector::begin + iterator::operator*
                // （各 3 处）⇒ 迭代器解引用形。
                m_memberReward[memberIdx].m_items[i].setCopy(
                    *result.m_dropItems.begin());
                // [b28d-46 P0] ORIG 第二次 begin()+operator* 后把 *begin() 值拷贝
                // 到栈临时（0x08149c9b..0x08149d25 共 17 条 mov，0x3d 字节），
                // 再自临时读 +0x02/+0x01 ⇒ 具名 Inven_Item 局部（非引用）。
                Inven_Item rewardItem = *result.m_dropItems.begin();
                // ORIG +0x3f2：按首项 idx 查 CItemList，命中后按
                // itemType(+0x01)/rarity/grade 扣幸运点（::CLuckPoint）。
                // [b28d-46 真 bug/P167] ORIG 0x08149d42 调 CItemList::find_item
                // （receiver = G_CDataManager()+0xc = m_itemList），非
                // CDataManager::find_item；原实现调错类（EXTRA find_item）。
                CItem* item = G_CDataManager()->m_itemList->find_item(
                    rewardItem.m_addInfo);   // +0x02 = item idx
                if (item != 0)
                {
                    ::CLuckPoint::UseLuckPoint(
                        m_party->get_user(memberIdx),
                        rewardItem.m_field1, item->get_rarity(), item->get_grade());
                }
            }
        }
    }
    return 1;
}

}  // namespace WongWork