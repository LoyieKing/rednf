// ============================================================================
// CBossPlay.cpp — WongWork::CBossPlay 基类实现（ORIG 还原）
//
// 9 个 ORIG 方法 + 生命周期，按
// docs/class_func_reports/WongWork__CBossPlay/*.md 逐字段还原。
// 注：generateRewardExp / generateRewardItem 依赖的若干共享类型尚未建模
//   （stClearRewardExpParameter_t/stClearRewardBonusExp_t、stGenerateResult_t 的
//   vector 视图、CPCRoomBurningEvent、CItemGenerator），此处以 ORIG 报告给出的
//   精确布局/原始虚表调用建模最小视图，避免改动共享头。
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


// Ghidra ROUND 记号：fistpl 舍入到最近整数。
#define ROUND(x) ((long long)((x) + 0.5f))

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

// stRewardItem_t ctor（ORIG W 0x081519b6）：构造 10 个 Inven_Item
// （ORIG 为编译器对 Inven_Item m_items[10] 数组成员的自动构造，body 为空）。
CBossPlay::stRewardItem_t::stRewardItem_t()
{
}

// Ctor（ORIG W 0x081519ec）：置 vptr（编译器自动），构造 4 个 stRewardItem_t 块
// （编译器在进入函数体前自动调用 stRewardItem_t ctor），再调 reset()。
CBossPlay::CBossPlay()
{
    reset();
}

// Dtor（ORIG W 0x08151a3a）：置 vptr 后调 reset()。
CBossPlay::~CBossPlay()
{
    reset();
}

// ============================================================================
// reset（ORIG 0x08149120）
// ============================================================================
void CBossPlay::reset()
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

// ============================================================================
// InitPlayUser（ORIG 0x0814926e）
// ============================================================================
void CBossPlay::InitPlayUser(int questCond, int towerIdx)
{
    for (int i = 0; i <= 3; ++i)
    {
        CUser* user = m_party->get_user(i);
        bool invalid = false;
        if (user == 0)
            invalid = true;
        else if (m_party->checkValidUser(i) != 1)
            invalid = true;
        if (!invalid)
        {
            CUser* u = m_party->get_user(i);
            u->setBossTowerIndex((short)towerIdx);
            // ORIG 连续两次 call CUser::getCurCharacQuestW（0x081492f1 与
            // 0x0814930d），结果不入局部变量：第一次 `add $0x75d8,%eax`
            // + call _Quest_Authen_Data::reset ⇒ 具名成员 UserQuest::m_authenData
            //（CUser.h 权威布局 +0x75d8）；第二次直接 set_authen_data。
            u->getCurCharacQuestW()->m_authenData.reset();
            u->getCurCharacQuestW()->set_authen_data(
                (QUEST_CONDITION)0, questCond, 0xffffffff);
            u->set_state((ch_state)0xc);
            setMemberAlive(i, true);
        }
    }
}

// ============================================================================
// _checkMemberDie（ORIG 0x08149382）
// ============================================================================
bool CBossPlay::_checkMemberDie()
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
int CBossPlay::checkAllMemberReady(int count)
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
int CBossPlay::getLiveMemberCount()
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
// [b21 N·P197-④] 语义已闭合：找成员循环 `checkValidUser(i) && get_user(i)==user`
//   的取反判定——ORIG(4.1) 发 `sete %al; test; je`，4.4-O0 对 call 结果直比恒发
//   `mov $1/mov $0 + test/jne`（P197-④：源形不可复现，禁改内联比较=双调用真 bug）；
//   循环头 `movl $0,-0xc` 双写为 4.1 代码生 quirk（int i=0）。返回路径 ORIG 在各
//   return 点直发 `mov $0x0/%1,%eax`+共享尾，4.4 以 %ebx 合并 epilogue（N82/寄存器分配）。
//   while(true){if(3<i)return 1;...break;++i;} 形态 + 时间比较改 `GetRevivalTime() >
//   getCurSec()` 已对齐 4.1 求值序/setg 方向；逐调用逐分支语义逐一相等，封 N。
// ============================================================================
int CBossPlay::handleDieUser(CUser* user)
{
    int i = 0;
    while (true)
    {
        if (3 < i)
            return 1;
        if (m_party->checkValidUser(i) && m_party->get_user(i) == user)
            break;
        ++i;
    }
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
bool CBossPlay::handleLeaveUser(CUser* user)
{
    int i = 0;
    for (i = 0; i <= 3; ++i)
    {
        if (m_party->checkValidUser(i) && m_party->get_user(i) == user)
        {
            if (m_memberField[i] == 0 && user->get_state() == 0xc)
            {
                user->getHackAnalyzer()->addServerHackCnt(user, (ENUM_HACKTYPE)0x13a, 1, 0, 0);
            }
            m_party->leave_user(user, (ENUM_PARTY_INFO_TYPE)2);
            setMemberAlive(i, false);
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
// ============================================================================
int CBossPlay::generateRewardExp(int memberIdx, unsigned int a, unsigned int b,
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
        stDeathTower_t* tower =
            G_CDataManager()->GetTowerScript(dungeon->get_index());
        clearExp = (unsigned int)(long long)(
            (double)(long long)((unsigned int)(a * b)) *
            tower->m_rewardExpWeight[1 + a]);
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
    int burningRate = 0;
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
// ============================================================================
int CBossPlay::generateRewardItem(int memberIdx, int itemCount, unsigned int a,
                                  ENUM_DUNGEON_DIFFICULTY diff, unsigned int b,
                                  const CDungeon* dungeon)
{
    m_memberReward[memberIdx].m_count = itemCount;
    int rare = 0;
    int rare2 = 0;
    float rare3 = 0.0f;
    unsigned int dungeonIdx = dungeon->get_index();
    CDataManager* dm = G_CDataManager();
    stDeathTower_t* tower = dm->GetTowerScript(dungeonIdx);
    // 掉宝概率三档累计表 / 金币费率（ORIG 0x08149971/0x08149992/0x081499b9
    // 分别 `mov 0xc88(%eax)`、`mov 0xc8c(%eax)`、`mov 0xc94(%eax)`，
    // 全为 4 字节读 ⇒ m_rewardItemProb[0]/[1]（int）、m_rewardGoldWeight（float））。
    rare = tower->m_rewardItemProb[0];
    dungeonIdx = dungeon->get_index();
    dm = G_CDataManager();
    tower = dm->GetTowerScript(dungeonIdx);
    rare2 = tower->m_rewardItemProb[1];
    dungeonIdx = dungeon->get_index();
    dm = G_CDataManager();
    tower = dm->GetTowerScript(dungeonIdx);
    rare3 = tower->m_rewardGoldWeight;
    CItemGeneratorMgr* genMgr = G_CDataManager()->getItemGenerator();
    CItemGenerator* gen = genMgr->getGenerator(CItemGeneratorMgr::eGenerateType_2);

    for (int i = 0; i < itemCount; ++i)
    {
        int randVal = get_rand_int(1000);
        stGenerateRefData_t ref;
        stGenerateResult_t result;

        if ((unsigned int)(rare * b) < (unsigned int)randVal ||
            (unsigned int)(rare * b - randVal) == 0)
        {
            if (randVal <= (unsigned int)(rare2 * b) && (unsigned int)(rare2 * b - randVal) != 0)
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
                gen->generateSpecificItem(ref, result);
                if (!result.m_dropItems.empty())
                {
                    Inven_Item& dst = m_memberReward[memberIdx].m_items[i];
                    dst.setCopy(result.m_dropItems.front());
                    // ORIG +0x3f2：按首项 idx 查 CItemList，命中后按
                    // itemType(+0x01)/rarity/grade 扣幸运点（::CLuckPoint）。
                    CItem* item = dm->find_item(dst.m_addInfo);   // +0x02 = item idx
                    if (item != 0)
                    {
                        ::CLuckPoint::UseLuckPoint(
                            m_party->get_user(memberIdx),
                            dst.m_field1, item->get_rarity(), item->get_grade());
                    }
                }
            }
        }
        else
        {
            // 随机掉落分支（ORIG +0x0f2 起，ref 基址同上 -0x124）：
            //   movb -0xef ⇒ +0x35=difficulty；movb -0xf0 ⇒ +0x34=a；
            //   movb -0xee ⇒ +0x36=member_count；movl $0x1 -0xec ⇒ +0x38=1；
            //   4×movw $0x1 ⇒ +0x3c/+0x3e/+0x40/+0x42 各 =1
            //   （+0x40/+0x42 合成指针位型 0x10001，随机路径不解引用）；
            // 虚槽 +0x8 = generateRandomItem。
            ref.m_field35 = (char)(unsigned char)diff;
            ref.m_field34 = (char)a;
            ref.m_rateSlot =
                (char)((m_party != 0) ? m_party->get_member_count() : 0);
            ref.m_rankBonusIndex = 1;
            ref.m_goldBase = 1;
            ref.m_goldFactor = 1;
            ref.m_field40 = 1;
            ref.m_field42 = 1;
            gen->generateRandomItem(ref, result);
            Inven_Item& dst = m_memberReward[memberIdx].m_items[i];
            dst.setCopy(result.m_dropItems.front());
            int addInfo = dst.get_add_info();
            dst.set_add_info((int)ROUND((float)addInfo * rare3));
        }
    }
    return 1;
}

}  // namespace WongWork