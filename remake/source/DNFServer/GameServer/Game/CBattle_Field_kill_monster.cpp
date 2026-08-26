// ============================================================================
// CBattle_Field_kill_monster.cpp —— CBattle_Field::kill_monster 完整转写。
//
// ORIG T 0x0830bc78（0x1913 字节）：
//   _ZN13CBattle_Field12kill_monsterER11PacketGuardR11map_monsterRjRK17STKillMonsterData
//   CBattle_Field::kill_monster(PacketGuard&, map_monster&, unsigned int&,
//                               STKillMonsterData const&)
// 依据 docs/class_func_reports/CBattle_Field/kill_monster.md（objdump -d 全量
// 汇编 + Ghidra 反编译 C 双重对照，逐段忠实转写；__PRETTY_FUNCTION__ 取自
// .r 区 0x08c23400 字面量）。
//
// 外部真实符号（ORIG T/W，本项目尚未还原，本 TU 仅声明原型供链接解析；
// 函数体转写属独立批次，清单见 /tmp/df_batches/BFKillMonster.md）：
//   CKillMonsterInfo::{Check,Register}{BossKill,APCBossKill,NamedKill}
//     0x0854d184 / 0x0854d418 / 0x0854d1e8 / 0x0854d48c / 0x0854d1b6 / 0x0854d452
//   CHellPartyItemStatistic::SendHellPartyItemStatistic  0x0860fcc0
//   GetInstanceHellPartyItemStatistic                    0x0860fcb6
//   CDataManager::getDropRate                            0x0830e6d2(W)
//   CUser::getAddIndependentDropRateFromPremium          0x0830edaa(W)
//
// 同 TU 依赖的 CBattle_Field 成员（权威头已有纯声明，定义在其它 TU）：
//   KillHellPartyGroupMonsterCnt / checkKilledSpawnBloodMonsters /
//   send_elevator_time_packet / _MakeDropItems / _MakeNotiPacketDieMonster /
//   convertAPCRoleType2MonsterRoleType（定义见 CPartyLink2.cpp）
// ============================================================================

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <list>
#include <map>
#include <utility>
#include <vector>

#include "CBattle_Field.h"
#include "CGlobalEffectManager.h"   // isAffectedEffect / getEffectValue
#include "CSystemTime.h"            // CSystemTime::getCurTickCount
#include "GlobalData.h"             // s_globalEffectManager_ / s_systemTime_
#include "LogManager.h"             // LogManager::logFormat
// ORIG __PRETTY_FUNCTION__（.r 区字面量；重建签名 unsigned int& 与 uint& 文本差异，
// 故按 ORIG 字符串硬编码，供 LogManager/cMyTrace 使用）。
#define BFKM_ROUND(v) ((int)((v) + 0.5f))


namespace
{

// 故按 ORIG 字符串硬编码，供 LogManager/cMyTrace 使用）。
char const s_bfkm_pretty[] =
    "bool CBattle_Field::kill_monster(PacketGuard&, map_monster&, uint&, "
    "const STKillMonsterData&)";


// ---- ORIG 真实符号原型（本 TU 仅声明，不提供定义）----

// CDataManager::getDropRate() const —— W 0x0830e6d2
float bfkmGetDropRateShim(CDataManager const* /*self*/)
    asm("_ZNK12CDataManager11getDropRateEv");

// CUser::getAddIndependentDropRateFromPremium(ENUM_PREMIUM_TYPE, unsigned int)
// —— W 0x0830edaa（转发 WongWork::CUserPremium::GetAddIndependentDropRate）
int bfkmGetAddIndependentDropRateFromPremiumShim(
    CUser* /*self*/, ENUM_PREMIUM_TYPE /*type*/, unsigned int /*count*/)
    asm("_ZN5CUser36getAddIndependentDropRateFromPremiumE17ENUM_PREMIUM_TYPEj");

// CKillMonsterInfo 六个击杀登记谓词（charac_expand 类型 0xc 扩展数据）
bool bfkmCheckBossKillShim(void* /*self*/, unsigned int /*mobIdx*/,
                           unsigned char /*diff*/)
    asm("_ZN16CKillMonsterInfo13CheckBossKillEjh");
bool bfkmRegisterBossKillShim(void* /*self*/, unsigned int /*mobIdx*/,
                              unsigned char /*diff*/)
    asm("_ZN16CKillMonsterInfo16RegisterBossKillEjh");
bool bfkmCheckAPCBossKillShim(void* /*self*/, unsigned int /*mobIdx*/,
                              unsigned char /*diff*/)
    asm("_ZN16CKillMonsterInfo16CheckAPCBossKillEjh");
bool bfkmRegisterAPCBossKillShim(void* /*self*/, unsigned int /*mobIdx*/,
                                 unsigned char /*diff*/)
    asm("_ZN16CKillMonsterInfo19RegisterAPCBossKillEjh");
bool bfkmCheckNamedKillShim(void* /*self*/, unsigned int /*mobIdx*/,
                            unsigned char /*diff*/)
    asm("_ZN16CKillMonsterInfo14CheckNamedKillEjh");
bool bfkmRegisterNamedKillShim(void* /*self*/, unsigned int /*mobIdx*/,
                               unsigned char /*diff*/)
    asm("_ZN16CKillMonsterInfo17RegisterNamedKillEjh");

// 地狱派对物品统计单例（ORIG 函数内 static instance 0x0947334c）
void* bfkmGetInstanceHellPartyItemStatisticShim()
    asm("_Z33GetInstanceHellPartyItemStatisticv");
bool bfkmSendHellPartyItemStatisticShim(void* /*self*/, bool /*random*/,
                                        int /*dungeonIdx*/, int /*diff*/,
                                        int /*dieReason*/, char /*scriptIdx*/,
                                        int* /*statistic[6]*/)
    asm("_ZN23CHellPartyItemStatistic26SendHellPartyItemStatisticEbiiicPi");

}  // namespace

// ================= kill_monster 主函数 =================

bool CBattle_Field::kill_monster(PacketGuard& packet, map_monster& mob,
                                 unsigned int& reward,
                                 const STKillMonsterData& data)
{
    // ---- STKillMonsterData 载荷展开（ORIG +0x000c..+0x006b 标量提取） ----
    const int uid = (unsigned short)data.m_monsterUid;   // -0xe0（zwl）
    const short mapIndex = data.m_mapIndex;              // -0xa2
    const int dieReason = (signed char)data.m_dieReason; // -0xa0
    const int avgMoneyPercent = data.m_field08;          // -0x9c
    int moneyValue = data.m_field0c;                     // -0xe4（效果缩放对象）
    const bool flag10 = data.m_flag10 != 0;              // -0x96
    const bool flag11 = data.m_flag11 != 0;              // -0x95

    MapInfo* mapInfo = GetCurrentMapInfo();              // -0x94

    // ---- 怪物查找（ORIG +0x007c：m_monsterMap.find(uid)，未命中再查 m_mapA8） ----
    std::map<int, map_monster>::iterator it = mapInfo->m_monsterMap.find(uid);
    if (it == mapInfo->m_monsterMap.end())
    {
        // ORIG 求值序：find 先于 end（+0x00e2..+0x0146）
        std::map<int, int>::iterator traceIt = mapInfo->m_mapA8.find(uid);
        if (traceIt == mapInfo->m_mapA8.end())
        {
            const unsigned int characNo =
                m_party ? (unsigned int)m_party->getManager()->getCurCharacNo()
                        : 0u;
            // ORIG 此处重新调用 GetCurrentMapInfo 取 map_index（+0x01a9）
            LogManager::logFormat(
                1, "battle_field.cpp", s_bfkm_pretty, 0x1ba7,
                "MOVE_MAP_LOG : kill_monster monster_map_trace_.find() fail ERROR "
                "monster_uid(%d), grid(%d,%d) map_index(%d) charac_no (%u)",
                uid, m_curX, m_curY, GetCurrentMapInfo()->m_mapIndex, characNo);
        }
        return false;
    }

    if (flag11)
        it->second.m_bloodRoundFlag = 1;                 // pair+0x34f
    map_monster* mobPtr = &it->second;                   // -0x90

    // ---- 锦标赛副本：移除本轮 multimap 条目 + 轮次推进/疲劳发放（+0x0214..） ----
    if (m_pDungeon != NULL && m_pDungeon->isTournamentDungeon())
    {
        const int round = GetTournamentCurRound();       // -0x110
        // ORIG +0x0261：此处重新调用 GetCurrentMapInfo 取 m_tournamentMob
        std::multimap<int, map_monster>& tourn =
            GetCurrentMapInfo()->m_tournamentMob;

        // ORIG +0x028e..+0x02b8：先 lower_bound，后 upper_bound
        std::multimap<int, map_monster>::iterator ti = tourn.lower_bound(round);
        std::multimap<int, map_monster>::iterator ubEnd = tourn.upper_bound(round);
        while (ti != ubEnd)
        {
            if (ti->second.m_instId == mobPtr->m_instId)
            {
                tourn.erase(ti);
                break;
            }
            ti++;   // ORIG 后缀 operator++(int)（_ZNSt17_Rb_tree_iterator...ppEi）
        }

        // ORIG +0x0345：find 先于 end
        std::multimap<int, map_monster>::iterator remain = tourn.find(round);
        if (remain == tourn.end())
        {
            if (round != 4)
            {
                const int fatigue = m_pDungeon->getTournamentRoundFatigue();
                for (int i = 0; i <= 3; ++i)
                {
                    if (!m_party->checkValidUser(i))
                        continue;
                    m_party->GetMember()[i].m_pUser->FatigueUp(fatigue);
                    m_party->GetMember()[i].m_pUser->SendFatigue();
                }
                SetTournamentCurRound(round + 1);
            }
        }
    }

    // ---- 地狱派对组击杀计数（+0x0478） ----
    bool killedAllGroups = false;                        // -0xe9
    if (mobPtr->m_hellPartyGroupIdx != 0)
        KillHellPartyGroupMonsterCnt(*mapInfo, *mobPtr, killedAllGroups);

    // ---- roleType 数据源校验（+0x04be..+0x06cb） ----
    if ((signed char)mobPtr->m_roleType <= 3)
    {
        if (G_CDataManager()->find_monster(mobPtr->m_mobIndex) == NULL)
        {
            if (mobPtr->m_hellPartyGroupIdx != 0)
            {
                if (CAICharacterList::get(G_CDataManager()->m_aiCharacterList,
                                          (unsigned int)mobPtr->m_mobIndex) == NULL)
                {
                    LogManager::logFormat(
                        1, "battle_field.cpp", s_bfkm_pretty, 0x1bed,
                        "G_CDataManager()->find_monster(%u) fail",
                        (unsigned int)mobPtr->m_mobIndex);
                    return false;
                }
            }
            else
            {
                LogManager::logFormat(
                    1, "battle_field.cpp", s_bfkm_pretty, 0x1bf3,
                    "G_CDataManager()->find_monster(%u) fail",
                    (unsigned int)mobPtr->m_mobIndex);
                return false;
            }
        }
    }
    else if ((signed char)mobPtr->m_roleType <= 8)
    {
        if (CAICharacterList::get(G_CDataManager()->m_aiCharacterList,
                                  (unsigned int)mobPtr->m_mobIndex) == NULL)
        {
            LogManager::logFormat(
                1, "battle_field.cpp", s_bfkm_pretty, 0x1bfd,
                "G_CDataManager()->aiCharacterList_->get(%u) fail",
                (unsigned int)mobPtr->m_mobIndex);
            return false;
        }
    }
    else
    {
        const unsigned int characNo =
            m_party ? (unsigned int)m_party->getManager()->getCurCharacNo() : 0u;
        LogManager::logFormat(
            1, "battle_field.cpp", s_bfkm_pretty, 0x1c03,
            "MOVE_MAP_LOG : kill_monster role_type(%d) ERROR monster_uid(%d), "
            "grid(%d,%d) charac_no (%u)",
            (int)(signed char)mobPtr->m_roleType, uid, m_curX, m_curY, characNo);
        return false;
    }

    // ---- 输出怪物快照（ORIG map_monster::operator= 0x08151d3a） ----
    mob = *mobPtr;

    // ---- 首杀登记（CKillMonsterInfo）+ 组队成员同步（+0x06e1..+0x0b13） ----
    bool firstBossKill = false;                          // -0x8a
    bool firstNamedKill = false;                         // -0x89
    const unsigned int mobIdx = (unsigned int)mob.m_mobIndex;

    if (m_party->IsSinglePlay())
    {
        // ORIG +0x0704：单人分支独立调用 getManager 取扩展数据宿主
        CUser* manager = m_party->getManager();
        void* killInfo =
            manager->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xc);
        const int diff = get_dungeon_diff();
        if (mob.m_roleType == 3)
        {
            if (!bfkmCheckBossKillShim(killInfo, mobIdx, (unsigned char)diff))
            {
                if (bfkmRegisterBossKillShim(killInfo, mobIdx,
                                             (unsigned char)get_dungeon_diff()))
                    firstBossKill = true;
            }
        }
        else if (mob.m_roleType == 8)
        {
            if (!bfkmCheckAPCBossKillShim(killInfo, mobIdx,
                                          (unsigned char)get_dungeon_diff()))
            {
                if (bfkmRegisterAPCBossKillShim(killInfo, mobIdx,
                                                (unsigned char)get_dungeon_diff()))
                    firstBossKill = true;
            }
        }
        else if (mob.isNamedMonster())
        {
            if (!bfkmCheckNamedKillShim(killInfo, mobIdx,
                                        (unsigned char)get_dungeon_diff()))
            {
                if (bfkmRegisterNamedKillShim(killInfo, mobIdx,
                                              (unsigned char)get_dungeon_diff()))
                    firstNamedKill = true;
            }
        }
    }
    else
    {
        // ORIG +0x07ab：组队分支独立调用 getManager 取扩展数据宿主
        CUser* manager = m_party->getManager();
        void* killInfo =
            manager->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xc);
        const int diff = get_dungeon_diff();
        if (mob.m_roleType == 3)
        {
            if (bfkmCheckBossKillShim(killInfo, mobIdx, (unsigned char)diff))
            {
                m_party->MemberRegisterKillMonster(
                    mob, (unsigned char)get_dungeon_diff());
            }
            else if (bfkmRegisterBossKillShim(killInfo, mobIdx,
                                              (unsigned char)get_dungeon_diff()))
            {
                firstBossKill = true;
                m_party->MemberRegisterKillMonster(
                    mob, (unsigned char)get_dungeon_diff());
            }
        }
        else if (mob.m_roleType == 8)
        {
            if (bfkmCheckAPCBossKillShim(killInfo, mobIdx,
                                         (unsigned char)get_dungeon_diff()))
            {
                m_party->MemberRegisterKillMonster(
                    mob, (unsigned char)get_dungeon_diff());
            }
            else if (bfkmRegisterAPCBossKillShim(
                         killInfo, mobIdx, (unsigned char)get_dungeon_diff()))
            {
                firstBossKill = true;
                m_party->MemberRegisterKillMonster(
                    mob, (unsigned char)get_dungeon_diff());
            }
        }
        else if (mob.isNamedMonster())
        {
            if (bfkmCheckNamedKillShim(killInfo, mobIdx,
                                       (unsigned char)get_dungeon_diff()))
            {
                m_party->MemberRegisterKillMonster(
                    mob, (unsigned char)get_dungeon_diff());
            }
            else if (bfkmRegisterNamedKillShim(
                         killInfo, mobIdx, (unsigned char)get_dungeon_diff()))
            {
                firstNamedKill = true;
                m_party->MemberRegisterKillMonster(
                    mob, (unsigned char)get_dungeon_diff());
            }
        }
    }


    // ---- 首杀日志（+0x0b29 cMyTrace 块） ----
    if (firstBossKill || firstNamedKill)
    {
        // ORIG +0x0b29..+0x0bf5：先逐项求值保存，再构造 cMyTrace 并输出
        const int traceDiff = get_dungeon_diff();
        const int traceDungeonIdx = get_dungeon_index();
        const int traceMobType = (int)mob.m_roleType;
        const int traceMobIdx = mob.m_mobIndex;
        const int singleMode = m_party->IsSinglePlay() ? 1 : 0;
        const unsigned int characNo =
            m_party ? (unsigned int)m_party->getManager()->getCurCharacNo() : 0u;
        cMyTrace trace(s_bfkm_pretty, 0x1c67, 0);
        trace("First Monster KILL : charac_no(%d), single_mode(%d), mob_index(%d), "
              "mob_type(%d), dungeon_index(%d), dungeon_diff(%d)",
              (int)characNo, singleMode, traceMobIdx, traceMobType,
              traceDungeonIdx, traceDiff);
    }

    // ---- 掉落生成准备（+0x0bfd..） ----
    std::list<map_item> itemList;                        // -0xf4
    std::vector<bool> dropFlags;                         // -0x108
    dropFlags.push_back(firstBossKill);
    dropFlags.push_back(firstNamedKill);

    if (mobPtr->m_dropCnt == 100 && !flag10 && m_bloodState != 1)
    {
        // 全局效果 1：掉落金额缩放（+0x0c9e..+0x0dfa）
        if (GlobalData::s_globalEffectManager_->isAffectedEffect(1))
        {
            const float effectValue =
                GlobalData::s_globalEffectManager_->getEffectValue(1);
            const int dropRateRounded = BFKM_ROUND(bfkmGetDropRateShim(G_CDataManager()));
            if ((float)dropRateRounded < effectValue)
            {
                const int delta = BFKM_ROUND(
                    (effectValue - (float)dropRateRounded) * (float)moneyValue);
                moneyValue += delta;
                const int cap = BFKM_ROUND((float)moneyValue * effectValue);
                moneyValue = std::max(0, std::min(moneyValue, cap));
            }
        }
        // 全局效果 4：同构缩放（+0x0dfa..+0x0f73）
        if (GlobalData::s_globalEffectManager_->isAffectedEffect(4))
        {
            const float effectValue =
                GlobalData::s_globalEffectManager_->getEffectValue(4);
            const int dropRateRounded = BFKM_ROUND(bfkmGetDropRateShim(G_CDataManager()));
            if ((float)dropRateRounded < effectValue)
            {
                const int delta = BFKM_ROUND(
                    (effectValue - (float)dropRateRounded) * (float)moneyValue);
                moneyValue += delta;
                const int cap = BFKM_ROUND((float)moneyValue * effectValue);
                moneyValue = std::max(0, std::min(moneyValue, cap));
            }
        }

        // 会员（premium 0x4f）独立掉率加成（+0x0f73..+0x106a）
        int premiumCount = 0;                            // -0x59
        int premiumAddRate = 0;                          // -0x58
        for (int i = 0; i <= 3; ++i)
        {
            if (m_party->checkValidUser(i))
            {
                if (m_party->GetMember()[i].m_pUser->isAffectedPremium(
                        (ENUM_PREMIUM_TYPE)0x4f))
                    ++premiumCount;
            }
        }
        if (premiumCount != 0 && premiumCount <= 4)
        {
            for (int i = 0; i <= 3; ++i)
            {
                if (!m_party->checkValidUser(i))
                    continue;
                premiumAddRate = bfkmGetAddIndependentDropRateFromPremiumShim(
                    m_party->GetMember()[i].m_pUser, (ENUM_PREMIUM_TYPE)0x4f,
                    (unsigned int)premiumCount);
                break;
            }
        }

        _MakeDropItems(itemList, *mapInfo, *mobPtr, (unsigned short)mapIndex,
                       dieReason, avgMoneyPercent, moneyValue, killedAllGroups,
                       dropFlags, data.m_field14, premiumAddRate);
    }

    // ---- APC 击杀计数（+0x1109：convert==1 且 rand()%100>0） ----
    if (convertAPCRoleType2MonsterRoleType(mobPtr->m_roleType) == 1 &&
        rand() % 100 > 0)
        ++m_apcKillCount;

    // ---- 死亡通知包（+0x1155） ----
    _MakeNotiPacketDieMonster(packet, itemList, uid, (unsigned short)mapIndex,
                              *mobPtr, mob);

    // ---- 怪物经验/金钱奖励表查询（+0x119d） ----
    G_CDataManager()->get_mob_reward((int)(unsigned char)mobPtr->m_level, reward);

    // ---- 地狱派对统计与奖励缩放（+0x11bb..+0x142d） ----
    if (mobPtr->m_hellPartyGroupIdx != 0)
    {
        const bool doStatistic =
            m_pDungeon != NULL && itemList.size() != 0 && killedAllGroups;
        if (doStatistic)
        {
            for (std::list<map_item>::iterator li = itemList.begin();
                 li != itemList.end(); li++)   // ORIG 后缀 operator++(ppEi)
            {
                const map_item& dropped = *li;
                CItem* item =
                    G_CDataManager()->find_item(dropped.m_item.m_addInfo);
                if (item != NULL && dropped.m_item.m_field1 == 1)
                {
                    const int rarity = item->get_rarity();
                    if (rarity < 6 && rarity >= 0)
                    {
                        ++m_hellItemStatistic[rarity];
                        m_hellPartyDropFlag = 1;
                    }
                }
            }
            if (m_hellPartyDropFlag != 0 && m_killedAllHellGroups != 0)
            {
                const bool sent = bfkmSendHellPartyItemStatisticShim(
                    bfkmGetInstanceHellPartyItemStatisticShim(),
                    m_hellPartyRandom != 0,
                    m_pDungeon->get_index(), get_dungeon_diff(), dieReason,
                    m_hellPartyDifficulty, &m_hellItemStatistic[0]);
                if (!sent)
                {
                    LogManager::logFormat(
                        1, "battle_field.cpp", s_bfkm_pretty, 0x1d04,
                        "HELLPARTY_LOG : DUNGEON INDEX (%d) : HELLPARTY "
                        "STATISTIC ERROR!!\n",
                        m_pDungeon->get_index());
                }
            }
        }

        // 奖励按地狱派对脚本比例缩放（+0x13f8：short 表 [scriptIdx+0x38]）
        const short* hellValues = (const short*)
            G_CDataManager()->get_hellparty_script_values();
        const int scriptSlot = (int)(signed char)mobPtr->m_hellPartyScriptIdx +
                               0x38;
        reward = (unsigned int)((int)reward * (int)hellValues[scriptSlot]);
    }

    // ---- Boss 房进入条件击杀计数（+0x1436；ORIG 不判空 m_pDungeon，忠实保留） ----
    if (m_pDungeon->m_bossRoom90c.m_0 == 1)
    {
        for (std::vector<stConditionEnterBossRoomStatus_t>::iterator ci =
                 m_bossRoomConditions.begin();
             ci != m_bossRoomConditions.end(); ++ci)
        {
            if (ci->m_monsterIdx == mobPtr->m_mobIndex)
                ++ci->m_curKillCount;
        }
    }

    // ---- 特定怪 0xdd8c：电梯状态切换（+0x14ec） ----
    if (mobPtr->m_mobIndex == 0xdd8c)
    {
        if ((signed char)m_elevatorTimer <= 3)
            m_elevatorState = 1;
        else
            m_elevatorState = 2;
        send_elevator_time_packet(m_elevatorState);
    }

    // ---- 从当前地图移除该怪（+0x1536） ----
    mapInfo->m_monsterMap.erase(it);

    // ---- 血轮副本：击杀后生成检查（+0x1565） ----
    if ((signed char)m_pDungeon->m_bloodDungeonType > 0)
        checkKilledSpawnBloodMonsters((int)mapInfo->m_monsterMap.size());

    // ---- 击杀节奏反外挂（killChecker1：技能材料，1 秒窗，hack 0xd0）（+0x1585） ----
    stMapMonsterKillChecker_t* checker1 = &mapInfo->m_killChecker1;
    if (checker1->getUseSkillMaterialTime() == 0 && dieReason == 1 &&
        !IsClearDungeon())
    {
        const unsigned int now = GlobalData::s_systemTime_.getCurTickCount();
        if (checker1->dieMonster(now, 1000) == 3)
        {
            checker1->initMonsterInfo();
            for (int i = 0; i <= 3; ++i)
            {
                if (!m_party->checkValidUser(i))
                    continue;
                CUser* user = m_party->get_user(i);
                if (user == NULL)
                    continue;
                user->getHackAnalyzer()->addServerHackCnt(
                    user, (WongWork::ENUM_HACKTYPE)0xd0, 1, 0, 0);
                break;
            }
        }
    }

    // ---- killChecker2：3 秒窗，hack 0xd7（+0x169a） ----
    stMapMonsterKillChecker_t* checker2 = &mapInfo->m_killChecker2;
    if (checker2->getUseSkillMaterialTime() == 0 && dieReason == 1 &&
        !IsClearDungeon())
    {
        const unsigned int now = GlobalData::s_systemTime_.getCurTickCount();
        if (checker2->dieMonster(now, 3000) == 3)
        {
            checker2->initMonsterInfo();
            for (int i = 0; i <= 3; ++i)
            {
                if (!m_party->checkValidUser(i))
                    continue;
                CUser* user = m_party->get_user(i);
                if (user == NULL)
                    continue;
                user->getHackAnalyzer()->addServerHackCnt(
                    user, (WongWork::ENUM_HACKTYPE)0xd7, 1, 0, 0);
                break;
            }
        }
    }

    // ---- 长期连续击杀检测（仅 checker1，700ms 窗，hack 0xde；无 break）（+0x17af） ----
    const unsigned int lastNow = GlobalData::s_systemTime_.getCurTickCount();
    if ((unsigned int)checker1->checkLongTermMobDie(lastNow, 700) > 4)
    {
        checker1->setUncontinuallyMobDieCnt(0);
        for (int i = 0; i <= 3; ++i)
        {
            if (!m_party->checkValidUser(i))
                continue;
            CUser* user = m_party->get_user(i);
            if (user == NULL)
                continue;
            user->getHackAnalyzer()->addServerHackCnt(
                user, (WongWork::ENUM_HACKTYPE)0xde, 1, 0, 0);
        }
    }
    checker1->setLastMobDieTime(lastNow);

    return true;
}
