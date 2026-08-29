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

// stRewardItem_t ctor（ORIG W 0x081519b6）：构造 10 个 Inven_Item。
CBossPlay::stRewardItem_t::stRewardItem_t()
{
    for (int i = 0; i < 10; ++i)
        m_items[i] = Inven_Item();
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
            // ORIG：*(int*)(block + j*0x3d + 0x06) = 0xffffffff（Inven_Item+0x02 槽位标记）
            *(int*)((char*)&m_memberReward[i].m_items[j] + 2) = 0xffffffff;
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
            UserQuest* quest = u->getCurCharacQuestW();
            _Quest_Authen_Data* auth = (_Quest_Authen_Data*)((char*)quest + 0x75d8);
            auth->reset();
            quest->set_authen_data((QUEST_CONDITION)0, questCond, 0xffffffff);
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
    unsigned int i;
    for (i = 0; i < 4; ++i)
    {
        if (m_party->checkValidUser(i) == 0 || !isMemberAlive(i))
            break;
    }
    return i == 4;
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
// ============================================================================
int CBossPlay::handleDieUser(CUser* user)
{
    int i;
    for (i = 0; i <= 3; ++i)
    {
        if (m_party->checkValidUser(i) && m_party->get_user(i) == user)
            break;
    }
    if (3 < i)
        return 1;
    if (isMemberAlive(i))
    {
        setMemberAlive(i, false);

        PacketGuard packet1;
        packet1.put_header(0, 0x20);
        packet1.put_short(user->get_unique_id() & 0xffff);
        packet1.put_byte(0);
        packet1.finalize(true);
        m_party->send_to_party(packet1);

        if (GlobalData::s_systemTime_.getCurSec() < user->GetRevivalTime())
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
// ============================================================================
int CBossPlay::handleLeaveUser(CUser* user)
{
    int i;
    for (i = 0; i <= 3; ++i)
    {
        if (m_party->checkValidUser(i) && m_party->get_user(i) == user)
        {
            if (m_memberField[i] == 0 && user->get_state() == 0xc)
            {
                CHackAnalyzer* hack = user->getHackAnalyzer();
                hack->addServerHackCnt(user, (ENUM_HACKTYPE)0x13a, 1, 0, 0);
            }
            m_party->leave_user(user, (ENUM_PARTY_INFO_TYPE)2);
            setMemberAlive(i, false);
        }
    }

    GameWorld* world = G_GameWorld();
    world->out_from_dungeon(user);

    PacketGuard packet;
    packet.put_header(0, 2);
    packet.put_byte(0);
    packet.put_short(1);
    user->make_basic_info((char*)&packet, '\0');
    packet.finalize(true);
    world->send_all(packet, user);
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
            (double)(long long)((unsigned long long)a * b) *
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
    // 渠道组字节：ORIG movzbl G_CEnvironment()+0x378 低字节
    //             = Server_Envir::m_serverGroup(+0x1e0)（set_clear_reward_exp_parameter/
    //             generateRewardExp 两份报告一致引用）
    unsigned char channelType = (unsigned char)env->get_server_group();
    float channelRate =
        chScript->getBonusRate(channelType, channelNo, dungeon->get_index());

    float pcrBurning = 0.0f;
    float expDouble = 0.0f;
    int burningRate = 0;
    float premiumRate = 0.0f;

    CEventBase* ev8 = GlobalData::s_event_manager->GetRepeatEvent(8);
    if (ev8 != 0 && ev8->IsEventing(user))
        // ORIG：整型 /100（imul 0x51eb851f 形态）后才转 float
        pcrBurning = (float)(((CPCRoomBurningEvent*)ev8)->GetExpFactor() / 100);

    CEventBase* ev3 = GlobalData::s_event_manager->GetRepeatEvent(3);
    if (ev3 != 0 && ev3->IsEventing(0))
        expDouble = (float)((CExpDoubleEvent*)ev3)->GetExpFactor() / kPercentDivisor;

    CEventBase* evF = GlobalData::s_event_manager->GetRepeatEvent(0xf);
    if (evF != 0 && evF->IsEventing(0))
        burningRate =
            ((CBurningFatigueEvent*)evF)->getBonusExpRate(user->getCurCharacUsedFatigue());

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
    rare = *(int*)((char*)tower + 0xc88);
    dungeonIdx = dungeon->get_index();
    dm = G_CDataManager();
    tower = dm->GetTowerScript(dungeonIdx);
    rare2 = *(int*)((char*)tower + 0xc8c);
    dungeonIdx = dungeon->get_index();
    dm = G_CDataManager();
    tower = dm->GetTowerScript(dungeonIdx);
    rare3 = *(float*)((char*)tower + 0xc94);
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
                // 特定掉落分支（ORIG +0x2a0）：ref blob
                //   +0x30=difficulty、+0x31=a、+0x34=float 1.0f、+0x38=0、
                //   +0x3c=dungeon；虚槽 +0xc = generateSpecificItem。
                *(char*)((char*)&ref + 0x30) = (char)(unsigned char)diff;
                *(char*)((char*)&ref + 0x31) = (char)a;
                *(float*)((char*)&ref + 0x34) = 1.0f;
                *(int*)((char*)&ref + 0x38) = 0;
                *(CDungeon**)((char*)&ref + 0x3c) = const_cast<CDungeon*>(dungeon);
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
            // 随机掉落分支（ORIG +0x0f2）：ref blob
            //   +0x30=difficulty、+0x31=a、+0x32=member_count、+0x34=1、
            //   +0x38/+0x3c=0x10001；虚槽 +0x8 = generateRandomItem。
            *(char*)((char*)&ref + 0x30) = (char)(unsigned char)diff;
            *(char*)((char*)&ref + 0x31) = (char)a;
            *(char*)((char*)&ref + 0x32) =
                (char)((m_party != 0) ? m_party->get_member_count() : 0);
            *(int*)((char*)&ref + 0x34) = 1;
            *(int*)((char*)&ref + 0x38) = 0x10001;
            *(int*)((char*)&ref + 0x3c) = 0x10001;
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