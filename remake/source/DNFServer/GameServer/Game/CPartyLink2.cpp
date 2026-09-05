// ============================================================================
// CPartyLink2.cpp —— CParty/CDataManager 域链接期符号恢复（2026-08-25，LinkPartyData2）
//
// 覆盖 5 个 ORIG 符号（全部经 docs/class_func_reports/ 对应报告
// ## 汇编 + ## 反编译 C 完整读取后忠实转写）：
//   CParty::kill_monster            T 0x085a27e8（kill_monster.md，0x1381 字节）
//   CParty::SendSecretShopItemList  T 0x085b9ecc（SendSecretShopItemList.md）
//   CParty::setBestClearTime        T 0x085be178（setBestClearTime.md）
//   CBattle_Field::convertAPCRoleType2MonsterRoleType
//                                   T 0x0830a812（objdump 转写；CBattle_Field.h:385
//                                    已声明成员，完整类型为 CParty 子对象视图，
//                                    以 ORIG mangled 提供定义）
//   CAICharacter::getPvPWinPoint    T 0x0834a240（getPvPWinPoint.md；CAICharacter
//                                    完整类为 CharacterAIA2.cpp TU 局部，无权威头，
//                                    以 ORIG mangled 提供定义，
//                                    +0x1c/+0x20 布局经 set.md 实证）
//
// 本 TU 不编辑 CParty.cpp/CParty.h/CBattle_Field.*，全部经既有权威头/视图访问。
// clear_reward(int) 与 set_clear_reward_exp_parameter 由 CClearRewardBatch2 批次负责，
// 不在本域。
// ============================================================================

#include <algorithm>
#include <string.h>

#include "QuickParty.h"       // 必须先于 CDungeon.h/CParty.h（守卫惯例，与 CParty.cpp 同）
#include "CParty.h"
#include "CDataManager.h"     // m_rankSystem / m_aiCharacterList / CAICharacterList
#include "CUser.h"            // CUser/CUserCharacInfo/CHackAnalyzer/cUserHistoryLog
#include "CDungeon.h"         // CDungeon::IsEnterEachMap / get_exp_weight / get_index
#include "CMap.h"             // CMap 完整类型（GetCurrentMap->m_f0）
#include "WarField.h"         // map_monster 完整定义
#include "CBattle_Field.h"    // CBattle_Field 完整类型（bf 子对象视图）
#include "CBattle_Field_shared.h"  // STKillMonsterData
#include "BattleData.h"       // BattleData 完整类型（bd 子对象视图）
#include "CExpandEquipslot.h"  // GetCharacExpandData 返回类型完整化（上行 CData）
#include "ActiveStaticInfo.h" // ActiveStaticInfo::getHpMax
#include "GameRand.h"         // get_rand_int
#include "LogManager.h"       // cMyTrace
#include "GlobalData.h"       // s_event_manager
#include "CGuildServerProxy.h"// CEventManager::GetRepeatEvent / CEventBase
#include "ServiceRestrictManager.h"  // ServiceRestrictManager / RestrictCategory
#include "Arad_DataManager.h" // ARAD::Singleton<T> 权威模板
#include "BlueMarble.h"       // BlueMarble::getBuff
#include "PvPTypes.h"         // BlueMarbleBuffType::T
#include "BestClearTime.h"    // BestClearTime::setBestClearTime
#include "CSecretShop.h"      // secretshop::CSecretShop::SendSecretShopItemList
#include "SECRET_SHOP_DATA.h" // secretshop::SALE_INFO
#include "HistoryLog.h"       // HistoryLog::WriteKillMob
#include "Secu_HackLogCheckByParty.h"  // Secu_HackLogCheckByParty::DieMob / MSG_MONSTER_DIE
#include "InterfacePacketBuf.h"
#include "GameEnums.h"        // ENUM_DUNGEON_CLEAR_CONDITION_TYPE

namespace
{

// 子对象视图（与 CParty.cpp / CPartyDepsD2.cpp 同惯例）
inline CBattle_Field& bf(CParty* p)
{
    return *reinterpret_cast<CBattle_Field*>(&p->m_padBattleField1);
}
inline BattleData& bd(CParty* p)
{
    return p->m_battleData;
}
inline CTraceMobDieHack& traceHack(CParty* p)
{
    return *reinterpret_cast<CTraceMobDieHack*>(&p->m_pad210);
}

// fistpll（round-to-nearest）等价转写（同 PartyDepsD2.cpp 惯例）
#define LINK2_ROUND(x) ((int)((x) + 0.5f))

}  // namespace

// ============================================================================
// CAICharacter::getPvPWinPoint() const —— ORIG T 0x0834a240（getPvPWinPoint.md）。
// CAICharacter 完整类为 CharacterAIA2.cpp TU 局部，无法从外部补成员定义；此处以
// ORIG mangled 符号提供定义：+0x1c=最小胜点，+0x20=最大胜点；等则返回该值，
// 否则 min + get_rand_int(max - min + 1)。
// ============================================================================
int CAICharacterGetPvPWinPointShim(void const* self)
    asm("_ZNK12CAICharacter14getPvPWinPointEv");
int CAICharacterGetPvPWinPointShim(void const* self)
{
    int low = *reinterpret_cast<int const*>(reinterpret_cast<char const*>(self) + 0x1c);
    int high = *reinterpret_cast<int const*>(reinterpret_cast<char const*>(self) + 0x20);
    if (low == high)
        return low;
    return low + get_rand_int(high - low + 1);
}

// ============================================================================
// CBattle_Field::convertAPCRoleType2MonsterRoleType(char) const —— ORIG T 0x0830a812。
// 5→0（普通） 6→1（Boss） 7→2（精英） 8→3（特殊），其余原样返回。
// 声明在 CBattle_Field.h:385（成员）；CParty 以 bf 子对象视图调用，符号按 ORIG
// mangled 提供（this 为 CBattle_Field*）。
// ============================================================================
char CBattleFieldConvertAPCRoleType2MonsterRoleTypeShim(
    void const* /*self*/, char role) asm("_ZNK13CBattle_Field34convertAPCRoleType2MonsterRoleTypeEc");
char CBattleFieldConvertAPCRoleType2MonsterRoleTypeShim(
    void const* /*self*/, char role)
{
    switch (role)
    {
    case 5: return 0;
    case 6: return 1;
    case 7: return 2;
    case 8: return 3;
    default: return role;
    }
}

// ============================================================================
// CParty::setBestClearTime —— ORIG T 0x085be178（setBestClearTime.md）。
// 遍历 4 个成员槽：checkValidUser(i)==1 时 GetCharacExpandData(user, 0x12)
// 得 BestClearTime*，调 BestClearTime::setBestClearTime(dungeonIdx, diff, time)。
// ============================================================================
void CParty::setBestClearTime(int dungeonIdx, int difficulty, int time)
{
    for (int i = 0; i < 4; ++i)
    {
        if (checkValidUser(i))
        {
            charac_expand::CData* data = m_member[i].m_pUser->GetCharacExpandData(
                (ENUM_CHARAC_EXPAND_TYPE)0x12);
            BestClearTime* best = static_cast<BestClearTime*>(data);
            best->setBestClearTime(dungeonIdx, difficulty, time);
        }
    }
}

// ============================================================================
// CParty::SendSecretShopItemList —— ORIG T 0x085b9ecc（SendSecretShopItemList.md）。
// pos = GetUserPosition(user)；0<=pos<4 时经 GlobalData::s_secret_shop 把
// this+0x2b0+pos*0x1c 处的 secretshop::SALE_INFO vector 发送给 user。
// ============================================================================
void CParty::SendSecretShopItemList(CUser* user)
{
    int pos = GetUserPosition(user);
    if (pos < 0 || pos >= 4)
        return;

    std::vector<secretshop::SALE_INFO>* shopItems =
        reinterpret_cast<std::vector<secretshop::SALE_INFO>*>(
            reinterpret_cast<char*>(this) + 0x2b0 + pos * 0x1c);
    GlobalData::s_secret_shop->SendSecretShopItemList(user, *shopItems);
}

// ============================================================================
// CPartyExpBonusEvent::getBonusRate 内联体（ORIG W 0x85bfe1a）—— kill_monster 主循环
// 内联依赖。CPartyExpBonusEvent 无权威头；此处按 ORIG 汇编语义以内部 helper 提供：
//   经 vtable 槽 +0x34（IsEventing）判活（ORIG 传参 0），
//   活 && memberCount>1 && !helpAbuse → (float)m_value(+0xc) / 100.0f，否则 0.0f。
//   注意：ORIG 无 event==NULL 防护（GetRepeatEvent(0x18) 必然非空），此处加防御
//   避免解引用空指针，正路径语义不变。
// ============================================================================
namespace
{
float partyExpBonusRate(void* event, int memberCount, bool helpAbuse)
{
    if (event == 0)
        return 0.0f;
    typedef bool (*IsEventingFn)(void*);
    void** vtable = *reinterpret_cast<void***>(event);
    IsEventingFn isEventing = reinterpret_cast<IsEventingFn>(vtable[0x34 / 4]);
    if (!isEventing(event))
        return 0.0f;
    if (memberCount <= 1 || helpAbuse)
        return 0.0f;
    int value = *reinterpret_cast<int*>(reinterpret_cast<char*>(event) + 0xc);
    return (float)value / 100.0f;
}
}  // namespace

// ============================================================================
// CParty::kill_monster —— ORIG T 0x085a27e8（kill_monster.md，0x1381 字节）。
// 关键字段映射（汇编逐指令核对）：
//   STKillMonsterData：uid=monsterIdx, mapIdx, dieReason=memberCount,
//     +8=avgMoneyPercent, +0xc=minStdDropRate, +0x10/+0x11=die[0xa22/0xa23],
//     +0x14=set_random_reward(DROPPROB)->result.m_rate14
//   奖励分表：G_CDataManager()->m_serverParameter.m_expGroup368[memberCount-1]
//   击杀计数：BattleData(+0x32c).m_killedMonster0/1/2、m_field48、m_field4c
//   金卡掉落上限：G_CDataManager()->m_serverParameter.m_4968（dm+0x49d0），
//     当前地图 m_f0（CMap+0xf0）非 0 则覆盖；命中上限时 m_field48 不增。
//   维度判定：dungeon+0x89e / +0x89f（Cheat 维度标志）与 party short +0xc4c。
// ============================================================================
int CParty::kill_monster(CUser* user, int monsterIdx, unsigned short mapIdx,
                         MSG_MONSTER_DIE const& dieInfo, int* param)
{
    CBattle_Field& field = bf(this);
    int memberCount = get_member_count();

    // ORIG：地图怪物分区进入时记录当前击杀者席位到 BF。
    if (m_dungeon != 0 && m_dungeon->IsEnterEachMap())
        field.SetCurSeatNo(get_party_seatno(user));

    int avgMoneyPercent = 0;
    int sumMoneyPercent = 0;
    int validCount = 0;
    int minStdDropRate = 100;

    for (int i = 0; i < 4; ++i)
    {
        if (!_checkValidUser(i))
            continue;
        CUser* member = m_member[i].m_pUser;

        int moneyPercent = 0;
        int limit = G_CDataManager()->GetMoneyLimitPerLevel(
            member->get_charac_level(), member->get_acc_name());
        if (limit < 1)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x1414, 5);
            trace("WarField::HandleMonsterKill limitMoney(%d) is under 0", limit);
        }
        else
        {
            moneyPercent = (member->getCurCharacMoney() * 100) / limit;
            sumMoneyPercent += moneyPercent;
            ++validCount;
        }

        if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
                RestrictCategory::Enum_3, 1))
            minStdDropRate = std::min(minStdDropRate, member->getStdDropRate());
    }
    if (validCount != 0)
        avgMoneyPercent = sumMoneyPercent / validCount;

    // ---- STKillMonsterData 载荷 + 随机掉落概率 buff ----
    STKillMonsterData data;
    memset(&data, 0, sizeof(data));
    data.m_monsterUid = (unsigned short)monsterIdx;
    data.m_mapIndex = mapIdx;
    data.m_dieReason = (char)memberCount;
    data.m_field08 = avgMoneyPercent;
    data.m_field0c = minStdDropRate;
    data.m_flag10 = (char)(dieInfo.m_fieldA22 != 0);
    data.m_flag11 = (char)(dieInfo.m_fieldA23 != 0);

    QuickParty::RewardData_Param randParam(this);
    QuickParty::RewardData_Result randResult;
    set_random_reward(QuickParty::RANDOM_BUFF_INCREASE_DROPPROB, randParam, randResult);
    data.m_field14 = randResult.m_rate14;

    PacketGuard packet;
    map_monster mob;
    unsigned int reward = 0;
    if (!field.kill_monster(packet, mob, reward, data))
    {
        // ORIG 失败分支：非 Boss 房且（自动创建或单人）时上报 hack 0xcf。
        if (!checkBossRoom())
        {
            if (IsAutoCreated() == false && get_member_count() != 1)
            {
            }
            else
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, (WongWork::ENUM_HACKTYPE)0xcf, 1, 0, 0);
            }
        }
        return 0;
    }

    if (param != 0)
        *param = (int)reward;

    // ---- 击杀合法化 hack 检测（0x25e：地图存活计数异常；0x25f：HP 上限异常） ----
    if (mob.m_field20 == 0 && !field.IsClearDungeon())
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x25e, 1, 0, 0);
    if (mob.m_field24 < mob.m_active.getHpMax() && !field.IsClearDungeon())
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x25f, 1, 0, 0);

    // ---- 成员击杀回调 + 击杀角色统计 ----
    map_monster mobCopy(mob);
    OnKilledMonster(user, (int)reward, mobCopy, dieInfo);

    char roleType = CBattleFieldConvertAPCRoleType2MonsterRoleTypeShim(
        &field, mob.m_roleType);
    if (roleType == 1)
    {
        ++bd(this).m_killedMonster1;
    }
    else if (roleType == 3)
    {
        ++bd(this).m_killedMonster2;
    }
    else
    {
        ++bd(this).m_killedMonster0;
        ++bd(this).m_field4c;
        int expLimit = G_CDataManager()->m_serverParameter.m_4968;
        CMap* curMap = field.GetCurrentMap();
        if (curMap != 0 && curMap->m_f0 != 0)
            expLimit = curMap->m_f0;
        if (expLimit == 0 || bd(this).m_field4c <= expLimit)
            ++bd(this).m_field48;
    }

    // ---- 击杀追踪器（m_manager 存在且全网格清时 traceEnd + 全员人证） ----
    if (m_manager != 0)
    {
        traceHack(this).setMobKiller((int)mapIdx);
        if (field.check_grid_clear())
        {
            CUser* users[4];
            users[0] = 0; users[1] = 0; users[2] = 0; users[3] = 0;
            users[0] = m_manager;
            traceHack(this).traceEnd(m_manager, users);
            for (int i = 0; i < 4; ++i)
            {
                if (_checkValidUser(i))
                    m_member[i].m_pUser->reqHumanCertify4ClearMap(true);
            }
        }
    }

    // ---- 副本清图追踪 ----
    if (roleType == 3)
        m_dungeonClearTracer.IncrementKilledBossCount();
    m_dungeonClearTracer.IncrementKilledMonsterCount();

    // ---- 任务怪物登记（存活成员；live 标志 1；非 PvP 房） ----
    for (int i = 0; i < 4; ++i)
    {
        if (!_checkValidUser(i))
            continue;
        if (m_battleData.m_arr54[i] != 1 || m_partyType == 1)
            continue;
        int questEnemyType = 1;
        if (mob.m_roleType > 4 && mob.m_roleType <= 8)
            questEnemyType = 2;
        m_member[i].m_pUser->CheckQuestMonster(
            field.get_dungeon_index(), (int)reward,
            (ENUM_QUEST_ENEMY_TYPE)questEnemyType);
    }

    // ---- 击杀者金币历史（m_field796f8 为 _IO_FILE* 位模式） ----
    for (int i = 0; i < 4; ++i)
    {
        if (_checkValidUser(i) && m_member[i].m_pUser == user)
        {
            HistoryLog::WriteKillMob(
                reinterpret_cast<FILE*>(user->m_field796f8), (int)reward);
        }
    }

    // ---- 总经验计算（成员数倍率 + 队伍经验加成事件） ----
    float expRate;
    if (memberCount < 1)
        expRate = 1.0f;
    else
        expRate = G_CDataManager()->m_serverParameter.m_expGroup368[memberCount - 1];

    expRate += partyExpBonusRate(
        GlobalData::s_event_manager->GetRepeatEvent(0x18),
        memberCount, isHelpAbuseParty());

    float expWeight = (m_dungeon != 0) ? m_dungeon->get_exp_weight() : 1.0f;

    unsigned int totalExp = getMonsterTotalExp(
        reward, roleType, field.get_dungeon_diff(), expWeight, expRate,
        mob.isNamedMonster());

    // ---- 击杀历史日志（KillMonster） ----
    user->m_historyLog.KillMonster(
        (int)reward,
        dieInfo.m_killValue1, dieInfo.m_killValue2,
        dieInfo.m_killPairA24, dieInfo.m_killPairA26, dieInfo,
        mob.m_roleType, mob.m_roleType == 5,
        user->getCurCharacExp(), (int)totalExp,
        (eExpAddReason)1);

    _getMemberLevelGap();

    // ---- PvP 胜点（角色类型 4+，怪物掉率 100/200） ----
    int winPoint = 0;
    if (mob.m_roleType > 4 && G_CDataManager()->m_aiCharacterList != 0)
    {
        CAICharacter* aiChar = G_CDataManager()->m_aiCharacterList->get(reward);
        if (aiChar != 0 &&
            (mob.m_dropCnt == 100 || mob.m_dropCnt == 200))
        {
            winPoint = CAICharacterGetPvPWinPointShim(aiChar);
        }
    }

    // ---- 逐成员经验分配（die[0xa22]==0 时执行） ----
    for (int i = 0; i < 4 && dieInfo.m_fieldA22 == 0; ++i)
    {
        if (!_checkValidUser(i) || m_partyType == 1)
            continue;
        CUser* member = m_member[i].m_pUser;

        // 未进入地图且非维度绕行 → 0 经验包
        if (m_battleData.m_arr58[i] != 1 && m_dungeon != 0 &&
            m_dungeon->m_bloodDungeonType == 0)   // +0x89f
        {
            member->SendZeroExpPacket();
            continue;
        }

        unsigned int exp = 0;
        // 维度未启用进入限制的完整单人份额计算
        if (m_dungeon != 0 &&
            (m_dungeon->m_89e == 0 &&                     // +0x89e
             *(unsigned short*)((char*)this + 0xc4c) == 0))
        {
            if (m_dungeon->get_index() == 100)
            {
                unsigned int mobReward = 0;
                G_CDataManager()->get_mob_reward(member->get_charac_level(), mobReward);
                unsigned int total = getMonsterTotalExp(
                    mobReward, roleType,
                    *reinterpret_cast<int*>(&this->m_fieldCC4),   // +0xcc4（char 首字节起 4 字节块）
                    m_dungeon->get_exp_weight(), expRate, false);
                exp = total / (unsigned int)memberCount;
            }
            else
            {
                float penalty = G_CDataManager()->BaseExpPenalty(
                    member->get_charac_level(), (int)(unsigned char)mob.m_level);
                int memberPenalty = MemberPenaltyRate();
                exp = (unsigned int)LINK2_ROUND(
                    (float)totalExp * penalty * (float)memberPenalty /
                    (float)memberCount);
            }
        }
        else
        {
            unsigned int mobReward = 0;
            G_CDataManager()->get_mob_reward(member->get_charac_level(), mobReward);
            unsigned int total = getMonsterTotalExp(
                mobReward, roleType, field.get_dungeon_diff(),
                m_dungeon->get_exp_weight(), expRate, false);
            exp = total / (unsigned int)memberCount;
        }

        // ---- 随机 buff（经验类） ----
        QuickParty::RewardData_Param expParam(this);
        QuickParty::RewardData_Result expResult;
        set_random_reward(QuickParty::RANDOM_BUFF_INCREASE_EXP, expParam, expResult);
        exp = (unsigned int)expResult.m_value00;

        if (winPoint != 0)
            member->gainWinPoint(winPoint, (eWPAddReason)2);

        STGainedEachExpData expData;
        expData.m_exp = (int)exp;
        expData.m_expPartyDivisor = expRate;
        exp = sumGainedEachExp(member, expData);

        // 帮助滥用比例（维度均未启用）
        if (m_dungeon != 0 &&
            m_dungeon->m_89e != 1 &&                    // +0x89e
            *(unsigned short*)((char*)this + 0xc4c) == 0)
        {
            member->calcHelpAbuseRatio(exp, isHelpAbuseParty(member));
        }

        // 维度绕行：跳过蓝宝石倍率与经验发放
        if (m_dungeon != 0 && m_dungeon->m_bloodDungeonType != 0)
            continue;                                    // +0x89f

        // ---- 蓝宝石/迷宫倍率 ----
        if (member->getMoveSpace() == (UserSpace::T)1)
        {
            BlueMarble* marble = (BlueMarble*)member->getBlueMarble();
            if (marble != 0)
            {
                int buff = marble->getBuff(member, (BlueMarbleBuffType::T)0);
                if (buff > 1)
                    exp = (unsigned int)(buff * (int)exp);
            }
        }

        int gainedSp = 0;
        int gainedSfp = 0;
        if (member->gain_exp_sp(
                (int)exp, gainedSp, gainedSfp,
                (CUser::eExpAddReason)1, (int)reward, false))
        {
            member->SendNotiPacket((CUser::eSendTarget)2,
                                   (ENUM_NOTIPACKET)2, 1);
        }
    }

    // ---- 怪物疲劳统计（非命名怪且非 Boss/特殊） ----
    // m_mapHitList2[m_mapHitCount]（+0x64c*4+4）累加经验类值；
    // m_mapHitList3[m_mapHitCount]（+0x678*4+0xc）累加耗时类值。
    if (!mob.isNamedMonster() && roleType != 1 && roleType != 3)
    {
        ++m_field1ab8;
        m_mapHitList3[m_mapHitCount] += (int)dieInfo.m_killValue1;
        m_mapHitList2[m_mapHitCount] += (int)dieInfo.m_killValue2;
    }
    if (dieInfo.m_killValue1 > 9 && dieInfo.m_killValue1 % 10 == 0)
        m_field1aa0 = 1;

    // ---- 安全检测上报 + 广播 ----
    reinterpret_cast<Secu_HackLogCheckByParty*>(
        reinterpret_cast<char*>(this) + 0x1abc)->DieMob(&dieInfo, &mob);

    send_to_party(packet);

    // ---- Boss 击杀广播（header 0x32，单字节房间号） ----
    if (roleType == 1)
    {
        PacketGuard bossPacket;
        InterfacePacketBuf* buf = (InterfacePacketBuf*)&bossPacket;
        buf->clear();
        buf->put_header(0, 0x32);
        buf->put_int(*(int*)((char*)this + 0xccc));
        buf->finalize(true);
        send_to_party(bossPacket);
    }

    prepare_dungeon_clear();

    // ---- 清图条件判定（roleType→条件类型） ----
    int clearConditionType;
    if (mob.m_roleType == 3)
        clearConditionType = 4;
    else if (mob.m_roleType < 3 || (unsigned int)(mob.m_roleType - 5) > 3)
        clearConditionType = 2;
    else
        clearConditionType = 3;

    if (field.ClearCondition((ENUM_DUNGEON_CLEAR_CONDITION_TYPE)clearConditionType,
                             (int)reward))
    {
        ClearDungeon();
    }
    return 1;
}
