#include "GameRand.h"
// df_game_r 还原 —— WongWork::CDeathTower（死亡之塔）
// 基于 ORIG 反汇编逐函数实现语义。
#include "CGameManager.h"
#include "CUser.h"
#include "CParty.h"
#include "CItem.h"
#include "Inven_Item.h"
#include "CDungeon.h"
#include "TimerQueue.h"
#include "LogManager.h"
#include "CInventory.h"
#include "CCirculationStatistic.h"
#include "CDataManager.h"
#include "CMap.h"
#include "GameWorld.h"

class CAICharacter
{
public:
    unsigned char getLevel() const { return 1; }
    unsigned int getIdx() const { return 0; }
    int getAppearancePoint() const { return 0; }
    void deathTowerSpecifyItemDrop(std::vector<unsigned long>& dropIds) {}
    int getPvPWinPoint() const { return 0; }
};

class CMonster
{
public:
    long deathTowerSpecifyItemDrop() { return 0xfffffffe; }
};
CAICharacter* CAICharacterList::get(void* list, unsigned int idx) { return 0; }
// CAICharacterList::~CAICharacterList（自 GameStubs.cpp 迁移；空白析构，ORIG 底层批次）
CAICharacterList::~CAICharacterList() {}

// ---- stMapMonsterKillChecker_t（0x18 字节；来自 CBattle_Field_deps.h） ----
struct stMapMonsterKillChecker_t
{
    unsigned int m_useSkillMaterialTime;   // +0x00
    unsigned int m_lastMobDieTime;         // +0x04
    int m_uncontinuallyMobDieCnt;          // +0x08
    int m_dieCnt;                          // +0x0c
    int m_field10;                         // +0x10
    int m_field14;                         // +0x14

    void init();
    void initMonsterInfo();
    void setUseSkillMaterial(unsigned int tick);
    unsigned int getUseSkillMaterialTime() const;
    int getUseSkillMaterialCount() const;
    int dieMonster(unsigned int tick, unsigned int limit);
    void setLastMobDieTime(unsigned int tick);
    int checkLongTermMobDie(unsigned int tick, unsigned int limit);
    void setUncontinuallyMobDieCnt(unsigned int cnt);
};

unsigned int stMapMonsterKillChecker_t::getUseSkillMaterialTime() const { return m_useSkillMaterialTime; }

int stMapMonsterKillChecker_t::dieMonster(unsigned int tick, unsigned int limit)
{
    if (m_lastMobDieTime == 0 || tick - m_lastMobDieTime > limit)
    {
        m_uncontinuallyMobDieCnt = 0;
    }
    else
    {
        m_uncontinuallyMobDieCnt++;
    }
    m_lastMobDieTime = tick;
    m_dieCnt++;
    return m_uncontinuallyMobDieCnt;
}

void stMapMonsterKillChecker_t::initMonsterInfo()
{
    m_useSkillMaterialTime = 0;
    m_lastMobDieTime = 0;
    m_uncontinuallyMobDieCnt = 0;
    m_dieCnt = 0;
}


struct map_item
{
    int m_count;              // +0x00
    int m_itemIndex;          // +0x04
    unsigned short m_dropIndex;  // +0x08
    char m_pad0a[2];          // +0x0a
    int m_createTick;         // +0x0c
    Inven_Item m_item;        // +0x10（0x3d 字节）
    char m_pad4d[3];          // +0x4d
    int m_ownerId;            // +0x50

    map_item();
    ~map_item();
};

class TimerQueue;
TimerQueue* G_TimerQueue();

// ---- map_monster 最小定义（用于 CStage::checkClearStage） ----
struct map_monster
{
    int m_mobId;              // +0x00
    int m_instId;             // +0x04
    char m_roleType;          // +0x08
    char m_pad9[3];           // +0x09
    int m_mobIndex;           // +0x0c
    int m_level;              // +0x10
    char m_flag14;            // +0x14
    char m_flag13;            // +0x15
    char m_flag12;            // +0x16
    char m_flag11;            // +0x17
    int m_field18;            // +0x18
    char m_pad1c[4];          // +0x1c
    int m_field20;            // +0x20
    int m_field24;            // +0x24
    int m_damage;             // +0x28
    int m_dropCnt;            // +0x2c
    int m_dropRate;           // +0x30
    char m_flag34;            // +0x34
    char m_pad35[0x34c - 0x35];  // +0x35
};


extern "C" void* sub_GetInstanceDungeonEntranceLog() asm("_Z29GetInstanceDungeonEntranceLogv");

int GetInvenTypeFromItemSpace(int space) { return space; }

extern "C" void* sub_GetInstanceDungeonEntranceLog() asm("_Z29GetInstanceDungeonEntranceLogv");
extern "C" void sub_CDungeonEntranceLog_DecrementDungeonEntrance(void* log, int idx, bool flag) asm("_ZN19CDungeonEntranceLog24DecrementDungeonEntranceEib");
extern "C" void sub_CDungeonEntranceLog_IncrementDungeonEntrance(void* log, int idx, bool flag) asm("_ZN19CDungeonEntranceLog24IncrementDungeonEntranceEib");

extern "C" unsigned int sub_CDeathTowerRanking_makeDungeonIdx2TowerIdx(unsigned int dungeonIdx) asm("_ZN8WongWork18CDeathTowerRanking23makeDungeonIdx2TowerIdxEj");
extern "C" void* sub_CDeathTowerRanking_getRankTable(unsigned int memberCnt) asm("_ZN8WongWork18CDeathTowerRanking12getRankTableEj");
extern "C" void sub_CDeathTowerRanking_checkRenewMyRecord(void* rankTable, unsigned int towerIdx, unsigned int a, unsigned int b, unsigned int c) asm("_ZN8WongWork18CDeathTowerRanking18checkRenewMyRecordEjjjj");
extern "C" int sub_CDeathTowerRanking_checkRenewTopRecord(void* rankTable, unsigned int towerIdx, unsigned int a, unsigned int b) asm("_ZN8WongWork18CDeathTowerRanking19checkRenewTopRecordEjjj");
extern "C" int sub_CDeathTowerRanking_getVirtualRank(void* rankTable, unsigned int towerIdx, unsigned int stage, unsigned int playTime) asm("_ZN8WongWork18CDeathTowerRanking14getVirtualRankEjjj");
namespace WongWork {

extern "C" void sub_CBossStage_reset(void*)
    asm("_ZN8WongWork10CBossStage5resetEv");

CBossTower::CBossTower()
{
    sub_CBossStage_reset(this);
    memset((char*)this + 0xaf0, 0, 4);
    *(int*)((char*)this + 8) = 0;
    memset((char*)this + 0xd, 0, 4);
    memset((char*)this + 0x9f0, 0, 4);
    *(int*)((char*)this + 4) = 0;
    memset((char*)this + 0xb00, 0, 0x18);
}

CBossTower::~CBossTower()
{
    sub_CBossStage_reset(this);
    memset((char*)this + 0xaf0, 0, 4);
    *(int*)((char*)this + 8) = 0;
    memset((char*)this + 0xd, 0, 4);
    memset((char*)this + 0x9f0, 0, 4);
    *(int*)((char*)this + 4) = 0;
    memset((char*)this + 0xb00, 0, 0x18);
}


// CDeathTowerRanking 方法桩（待后续按 ORIG 实现）
unsigned int sub_CDeathTowerRanking_makeDungeonIdx2TowerIdx(unsigned int dungeonIdx) { return 0; }
void* sub_CDeathTowerRanking_getRankTable(unsigned int memberCnt) { return 0; }
void sub_CDeathTowerRanking_checkRenewMyRecord(void* rankTable, unsigned int towerIdx, unsigned int key, unsigned int clearTime, unsigned int playTime)
{
    // Simplified: hash_map lookup not yet implemented
    // Returns via side effect; actual logic requires CDeathTowerRanking layout
    (void)rankTable;
    (void)towerIdx;
    (void)key;
    (void)clearTime;
    (void)playTime;
}
int sub_CDeathTowerRanking_checkRenewTopRecord(void* rankTable, unsigned int towerIdx, unsigned int clearTime, unsigned int playTime)
{
    char* base = (char*)rankTable + towerIdx * 0x390;
    if (base[0x128] != 1)
    {
        return 0;
    }

    unsigned int* existingRecord = (unsigned int*)(base + 0x98);
    unsigned int existingClearTime = existingRecord[0];
    unsigned int existingPlayTime = existingRecord[1];

    if (clearTime < existingClearTime)
    {
        return 1;
    }
    else if (clearTime == existingClearTime && playTime < existingPlayTime)
    {
        return 1;
    }
    return 0;
}

int sub_CDeathTowerRanking_getVirtualRank(void* rankTable, unsigned int towerIdx, unsigned int stage, unsigned int playTime)
{
    char* base = (char*)rankTable;

    // Best record at this + 0x7058
    unsigned int* bestRecord = (unsigned int*)(base + 0x7058);
    unsigned int bestStage = bestRecord[0];
    unsigned int bestPlayTime = bestRecord[1];

    // New record is (stage, playTime)
    // Compare: better means higher stage, or same stage and lower playTime
    bool newIsBetter = false;
    bool newIsEqual = false;

    if (stage > bestStage)
    {
        newIsBetter = true;
    }
    else if (stage == bestStage && playTime < bestPlayTime)
    {
        newIsBetter = true;
    }
    else if (stage == bestStage && playTime == bestPlayTime)
    {
        newIsEqual = true;
    }

    if (newIsBetter)
    {
        return -1;
    }

    if (newIsEqual)
    {
        return *(int*)(base + 0x70ec);
    }

    // Compare with top record at this + towerIdx * 0x390 + 0x98
    unsigned int* topRecord = (unsigned int*)(base + towerIdx * 0x390 + 0x98);
    unsigned int topStage = topRecord[0];
    unsigned int topPlayTime = topRecord[1];

    if (stage > topStage || (stage == topStage && playTime < topPlayTime))
    {
        return 1;
    }

    // Simplified: return 0 for middle ranks (full map-based calculation requires CDeathTowerRanking layout)
    return 0;
}

CDeathTower::CDeathTower()
{
    memset(this, 0, sizeof(*this));
}

CDeathTower::~CDeathTower() {}

int CDeathTower::getIdx()
{
    return m_idx;
}

int CDeathTower::getTowerState()
{
    return m_towerState;
}

void CDeathTower::setIdx(int idx)
{
    m_idx = idx;
}

int CDeathTower::setTowerState(ENUM_TOWER_STATE state)
{
    if (state > (ENUM_TOWER_STATE)m_towerState)
    {
        m_towerState = (int)state;
        return 1;
    }
    return 0;
}

void CDeathTower::reset()
{
    m_dungeonMgr.reset();
    m_stage.reset();
    m_playData.reset();
    m_party = 0;
    m_towerState = 0;
}

void CDeathTower::_destroy()
{
    setTowerState((ENUM_TOWER_STATE)0);
    G_CGameManager()->returnDeathTower(this);
}

CDeathTower::CDungeonMgr* CDeathTower::getCDungeonMGr()
{
    return &m_dungeonMgr;
}

CDeathTower::CStage* CDeathTower::getCStage()
{
    return &m_stage;
}

void CDeathTower::attachParty(CParty* party)
{
    m_party = party;
    for (int i = 0; i < 4; ++i)
    {
        if (party->checkValidUser(i))
        {
            short idx = (short)m_idx;
            CUser* user = party->get_user(i);
            user->setDeathTowerIndex(idx);
        }
    }
    setTowerState((ENUM_TOWER_STATE)1);
}

int CDeathTower::genTimerKey(TIMER_MESSAGE msg)
{
    return _genTimerKey(msg);
}

int CDeathTower::_genTimerKey(TIMER_MESSAGE msg)
{
    int idx = (int)msg - 0x48;
    m_timerKeys[idx]++;
    return m_timerKeys[idx];
}

int CDeathTower::_checkTimerKey(TIMER_MESSAGE msg, unsigned int key)
{
    int idx = (int)msg - 0x48;
    return (m_timerKeys[idx] == (int)key) ? 1 : 0;
}

int CDeathTower::_checkMemberDie()
{
    for (int i = 0; i < 4; ++i)
    {
        if (m_party->checkValidUser(i))
        {
            if (m_playData.isMemberAlive(i))
            {
                return 0;
            }
        }
    }
    return 1;
}

int CDeathTower::_checkMemberReady()
{
    m_playData.resetMemberReady();
    int count = m_party->get_member_count();
    unsigned int timeout;
    if (count == 1)
    {
        timeout = 0x78;  // CONST_SINGLE_MAP_LOADING_WAIT_TIMEOUT
    }
    else
    {
        timeout = 0x5a;  // CONST_PARTY_MAP_LOADING_WAIT_TIMEOUT
    }
    int key = _genTimerKey((TIMER_MESSAGE)0x49);
    int idx = m_idx;
    G_TimerQueue()->InsertTimer(
        (TimerEntry::OBJ_TYPE)6, idx, (TIMER_MESSAGE)0x49, timeout, key, 0x49);
    return 0;
}

int CDeathTower::onLeaveUser(CUser* user)
{
    user->setDeathTowerIndex((short)-1);
    CInventory* inven = (CInventory*)user->getCurCharacInvenW();
    inven->RemoveFPItem();
    char memberNames[0x100];
    memset(memberNames, 0, sizeof(memberNames));

    if (m_party)
    {
        CDungeon* dungeon = m_dungeonMgr.getDungeon();
        if (dungeon)
        {
            int dungeonIdx = dungeon->get_index();
            void* entranceLog = sub_GetInstanceDungeonEntranceLog();
            sub_CDungeonEntranceLog_DecrementDungeonEntrance(entranceLog, dungeonIdx, false);
        }
    }

    if (m_party)
    {
        int cnt = m_party->get_member_count();
        if (cnt <= 0)
        {
            _destroy();
        }
    }
    return 1;
}

void CDeathTower::_beginTowerClearProcess(bool isLast)
{
    if (!(getTowerState() > 2))
        return;

    unsigned int memberCnt = m_playData.getStartMemberCnt();
    void* rankTable = sub_CDeathTowerRanking_getRankTable(memberCnt);

    _onClear(isLast);

    int partyState = m_party->get_party_type();
    if (partyState == 1)
    {
        setTowerState((ENUM_TOWER_STATE)5);
        m_party->SetEPLPState(2);

        int key = _genTimerKey((TIMER_MESSAGE)0x4d);
        int mgrIdx = m_idx;
        G_TimerQueue()->InsertTimer(
            (TimerEntry::OBJ_TYPE)6, mgrIdx, (TIMER_MESSAGE)0x4d, 0x3c, key, 0);

        PacketGuard pkt;
        pkt.put_header(0, 0xfe);
        pkt.finalize(true);
        m_party->send_to_party(pkt);
    }
    else
    {
        setTowerState((ENUM_TOWER_STATE)3);

        int key = _genTimerKey((TIMER_MESSAGE)0x4b);
        int mgrIdx = m_idx;
        G_TimerQueue()->InsertTimer(
            (TimerEntry::OBJ_TYPE)6, mgrIdx, (TIMER_MESSAGE)0x4b, 1, key, 0);
    }

    int curStage = m_stage.getCurrentStage();
    unsigned int playTime;
    if (isLast)
    {
        playTime = m_playData.getPlayTime();
    }
    else
    {
        playTime = m_playData.getLastPlayTime();
        int stageMinus1 = curStage - 1;
        int one = 1;
        curStage = std::max(stageMinus1, one);
    }

    for (int i = 0; i <= 3; ++i)
    {
        CUser* user = m_party->get_user(i);
        if (!user || !m_party->checkValidUser(i))
            continue;

        user->set_charac_party_bonus_exp(0);
        user->set_charac_member_bonus_exp(0);
        user->set_charac_fatigue_buf_bonus_exp(0);
        user->set_charac_seria_buf_bonus_exp(0);

        int characNo = user->getCurCharacNo();
        unsigned int dungeonIdx = m_dungeonMgr.getDungeon()->get_index();

        int result = _checkRenewMyRecord(dungeonIdx, memberCnt, characNo, curStage, playTime);
        int isNewRecord = (isLast && result == 1) ? 1 : 0;

        int topResult = _checkRenewTopRecord(dungeonIdx, memberCnt, characNo, playTime);
        if (isLast && topResult)
            isNewRecord += 2;

        unsigned int towerIdx = sub_CDeathTowerRanking_makeDungeonIdx2TowerIdx(dungeonIdx);
        int virtualRank = sub_CDeathTowerRanking_getVirtualRank(rankTable, towerIdx, curStage, playTime);

        // Construct result packet (simplified)
        PacketGuard pkt;
        pkt.put_header(0, 0x8f);
        pkt.put_byte(isNewRecord);
        pkt.put_int(virtualRank);
        pkt.put_int(curStage);
        pkt.put_int(playTime);
        pkt.finalize(true);
        user->Send(pkt);
    }
}
int CDeathTower::handleKillMonster(CUser* user, unsigned short monsterIdx, unsigned short a3, const MSG_MONSTER_DIE& msg, int* a5)
{
    if (m_stage.getState() != 1)
    {
        if (user)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x2a, 0x13);
        }
        return 0;
    }

    // Monster kill checker (skill material time)
    stMapMonsterKillChecker_t* checker = (stMapMonsterKillChecker_t*)&m_timerKeys[7];
    bool bSkillMaterial = false;
    if (checker->getUseSkillMaterialTime() == 0)
    {
        unsigned int curTick = GlobalData::s_systemTime_.getCurTickCount();
        int result = checker->dieMonster(curTick, 1000);
        if (result > 2)
        {
            bSkillMaterial = true;
        }
    }

    if (bSkillMaterial)
    {
        bool isPremium = false;
        if (user && user->get_charac_level() >= 0x1e)
        {
            isPremium = true;
        }
        else if (user && user->get_charac_level() >= 0x19)
        {
            CUserPremium* premium = user->GetPremiumInfo();
            if (premium)
            {
                std::vector<SUserPremiumInfo> premiumList;
                premium->GetPremiumInfoList(premiumList, 0x270f);
                for (size_t i = 0; i < premiumList.size(); i++)
                {
                    int type = premiumList[i].m_type;
                    if (type >= 0x17 && type <= 0x1b && premiumList[i].m_state == 1)
                    {
                        isPremium = true;
                        break;
                    }
                }
            }
        }

        if (isPremium)
        {
            checker->initMonsterInfo();
            CHackAnalyzer* hackAnalyzer = user->getHackAnalyzer();
            if (hackAnalyzer)
            {
                hackAnalyzer->addServerHackCnt(user, (WongWork::ENUM_HACK_TYPE)0xd6, 1, 0, 0);
            }
        }
    }

    // Kill monster in stage
    map_monster monster;
    int killResult = m_stage.killMonster(monsterIdx, monster);
    if (killResult == 0)
    {
        return 0;
    }

    int memberLevelGap = m_party->getMemberLevelGap();
    int winPoint = 0;
    std::vector<unsigned long> dropItemIds;

    if (monster.m_roleType < 5)
    {
        // Boss monster drop
        CMonster* monsterPtr = (CMonster*)G_CDataManager()->find_monster(monster.m_mobIndex);
        if (monsterPtr)
        {
            CDungeon* dungeon = m_dungeonMgr.getDungeon();
            if (dungeon && dungeon->hasTowerItemDrop())
            {
                long dropId = monsterPtr->deathTowerSpecifyItemDrop();
                if (dropId != 0xfffffffe)
                {
                    dropItemIds.push_back(dropId);
                }
            }
        }
    }
    else
    {
        // Normal monster drop
        CAICharacterList* aiCharList = G_CDataManager()->m_aiCharacterList;
        CAICharacter* aiChar = 0;
        if (aiCharList)
        {
            aiChar = CAICharacterList::get(aiCharList, monster.m_mobIndex);
        }
        if (aiChar)
        {
            CDungeon* dungeon = m_dungeonMgr.getDungeon();
            if (dungeon && dungeon->hasTowerItemDrop())
            {
                aiChar->deathTowerSpecifyItemDrop(dropItemIds);
            }
            if (monster.m_dropCnt == 100 || monster.m_dropCnt == 200)
            {
                winPoint = aiChar->getPvPWinPoint();
            }
        }
    }

    // Create items from drop ids
    std::vector<Inven_Item> rewardItems;
    for (size_t i = 0; i < dropItemIds.size(); i++)
    {
        int itemId = dropItemIds[i];
        CItem* item = G_CDataManager()->m_itemList->find_item(itemId);
        if (item)
        {
            Inven_Item invenItem;
            invenItem.m_addInfo = itemId;
            G_CDataManager()->m_itemList->create_item((ENUM_ITEM_CREATE_TYPE)1, invenItem, 1);
            int memberCnt = m_party->get_member_count();
            for (int j = 0; j < memberCnt; j++)
            {
                rewardItems.push_back(invenItem);
            }
        }
    }

    // Exp rewards for party members
    for (int i = 0; i < 4; i++)
    {
        CUser* partyUser = m_party->get_user(i);
        if (!partyUser || !m_party->checkValidUser(i))
        {
            continue;
        }

        if (m_party->get_party_type() != 1)
        {
            unsigned int mobExp = 0;
            int charLevel = partyUser->get_charac_level();
            G_CDataManager()->get_mob_reward(charLevel, mobExp);
            // m_party->getMonsterTotalExp(mobExp, false, 0, 1.0f, 1.0f, false);

            // RewardData_Param rewardParam(m_party);
            // RewardData_Result rewardResult;
            // rewardResult.m_exp = mobExp;
            // m_party->set_random_reward(0, rewardParam, 0, rewardResult);
            // mobExp = rewardResult.m_exp;

            if (monster.m_roleType == 5)
            {
                float* ratePtr = (float*)((char*)G_CDataManager() + 0x5198);
                mobExp = (unsigned int)(*ratePtr * mobExp);
            }

            float* ratePtr2 = (float*)((char*)G_CDataManager() + 0x5e1c);
            mobExp = (unsigned int)(*ratePtr2 * mobExp);

            // mobExp = m_party->sumGainedEachExp(partyUser, expData);

            if (winPoint != 0)
            {
                // CDeathTowerWinPointEvent* event = (CDeathTowerWinPointEvent*)CEventManager::GetRepeatEvent(GlobalData::s_event_manager, 0x27);
                // float bonusRate = event ? event->getBonusRate() : 0.0f;
                // if (bonusRate == 0.0f)
                // {
                //     partyUser->gainWinPoint(winPoint, 2);
                // }
                // else
                // {
                //     partyUser->gainWinPoint((int)(winPoint * bonusRate), 2);
                // }
            }

            // partyUser->get_charac_info()->calcHelpAbuseRatio(mobExp);

            int notiType = 0xb;
            if (monster.m_roleType == 5)
            {
                notiType = 0xc;
            }

            int expGain = 0;
            int spGain = 0;
            if (partyUser->gain_exp_sp(mobExp, expGain, spGain, (eExpAddReason)notiType, monster.m_mobIndex, false))
            {
                partyUser->SendNotiPacket((CUser::eSendTarget)2, (ENUM_NOTIPACKET)2, 1);
            }

            // History log
            // cUserHistoryLog* historyLog = (cUserHistoryLog*)((char*)partyUser + 0x79700);
            // historyLog->KillMonster(monster.m_mobIndex, 0, 0, 0, 0, &msg, monster.m_roleType, monster.m_roleType == 5, 0, mobExp, notiType);

            // Quest check
            // partyUser->CheckQuestMonster(dungeonIdx, monster.m_mobIndex, questType);
        }
    }

    // Drop items to stage and send packet
    std::list<map_item> dropList;
    for (size_t i = 0; i < rewardItems.size(); i++)
    {
        map_item item;
        item.m_count = 1;
        item.m_itemIndex = a3;
        item.m_createTick = GlobalData::s_systemTime_.getCurSec();
        item.m_item = rewardItems[i];
        m_stage.dropItem(item);
        dropList.push_back(item);
    }

    // Send drop item packet to party
    PacketGuard packet;
    packet.put_header(0, 0x26);
    packet.put_short(monsterIdx);
    int itemCountPos = packet.get_index();
    packet.put_byte(0);  // placeholder for count

    int itemCount = 0;
    for (std::list<map_item>::iterator it = dropList.begin(); it != dropList.end(); ++it)
    {
        map_item& item = *it;
        packet.put_short(item.m_itemIndex);
        packet.put_int(item.m_item.m_addInfo);
        unsigned char itemAttr = item.m_item.GetItemAttr();
        packet.put_byte(itemAttr);

        if (!item.m_item.isAvatarItemType())
        {
            packet.put_int(item.m_item.get_add_info());
        }
        else
        {
            CEquipItem* equipItem = (CEquipItem*)G_CDataManager()->find_item(item.m_item.m_addInfo);
            if (!equipItem)
            {
                packet.put_int(item.m_item.get_add_info());
            }
            else
            {
                // Avatar period: getAvatarPeriod not yet implemented, use add_info
                packet.put_int(item.m_item.get_add_info());
                stAvatarEmblemInfo_t emblemInfo;
                emblemInfo.init();
                equipItem->getAvatarSocket(item.m_item.m_fieldb, emblemInfo);
            }
        }

        packet.put_short(item.m_item.m_fieldb);
        unsigned char amplifyType = item.m_item.m_amp.getAbilityType();
        packet.put_byte(amplifyType);
        unsigned short amplifyValue = item.m_item.m_amp.getAbilityValue();
        packet.put_short(amplifyValue);
        packet.put_packet(item.m_item);

        if (!item.m_item.isAvatarItemType())
        {
            packet.put_byte(0);
        }
        else
        {
            packet.put_byte(1);
            packet.put_int(0x1e);
            stAvatarEmblemInfo_t emblemInfo;
            emblemInfo.init();
            packet.put_binary((char*)&emblemInfo, 0x1e);
        }

        packet.put_short(item.m_dropIndex);
        itemCount++;
    }

    packet.put_byte(0);
    packet.put_byte(0);
    packet.put_byte(0);
    packet.put_byte(itemCountPos, itemCount);
    packet.finalize(true);
    m_party->send_to_party(packet);

    // Check stage clear
    if (m_stage.checkClearStage())
    {
        _onFinishStage();
        int curStage = m_stage.getCurrentStage();
        int endStage = m_dungeonMgr.getEndStage();
        if (curStage == endStage)
        {
            _onPrepareFinishTower();
        }
    }

    return 1;
}
void CDeathTower::_onClear(bool isLast)
{
    unsigned int playTime;
    if (isLast)
    {
        playTime = m_playData.getPlayTime();
    }
    else
    {
        playTime = m_playData.getLastPlayTime();
    }

    for (int i = 0; i <= 3; ++i)
    {
        if (m_party->checkValidUser(i))
        {
            CUser* user = m_party->get_user(i);
            user->setChattingMessageCount(0);
            cUserHistoryLog* log = (cUserHistoryLog*)((char*)user + 0x79700);
            log->DungeonClearInfo((int)isLast, playTime / 1000);
        }
    }
}
void CDeathTower::_onFinishDeathTower()
{
    for (int i = 0; i <= 3; ++i)
    {
        if (m_party->checkValidUser(i))
        {
            CUser* user = m_party->get_user(i);
            onLeaveUser(user);
        }
    }
    m_party->ReturnToVillage();
    if (getTowerState() > 0)
    {
        _destroy();
    }
}

void CDeathTower::_onFinishStage()
{
    m_stage.onFinishStage();
    unsigned int clearTime = m_stage.getStageClearTime();
    m_playData.addPlayTime(clearTime);

    for (int i = 0; i <= 3; ++i)
    {
        if (m_party->checkValidUser(i))
        {
            CUser* user = m_party->get_user(i);
            if (clearTime <= 6000)
            {
                CHackAnalyzer* hack = user->getHackAnalyzer();
                hack->addServerHackCnt(user, (WongWork::ENUM_HACK_TYPE)0x19a, 1u, 0u, 0u);
            }
            int stageMapIdx = m_stage.getStageMapIndex();
            UserQuest* quest = user->getCurCharacQuestW();
            quest->set_authen_data((QUEST_CONDITION)1, stageMapIdx, 0);
        }
    }
}

void CDeathTower::_onPrepareFinishTower()
{
    int key = _genTimerKey((TIMER_MESSAGE)0x4a);
    int idx = m_idx;
    G_TimerQueue()->InsertTimer(
        (TimerEntry::OBJ_TYPE)6, idx, (TIMER_MESSAGE)0x4a, 5, key, 0x4a);
}

void CDeathTower::_onStartDeathTower()
{
    char memberNames[0x100];
    memset(memberNames, 0, sizeof(memberNames));
    m_party->getMemberNames(memberNames);

    for (int i = 0; i <= 3; ++i)
    {
        if (m_party->checkValidUser(i))
        {
            CUser* user = m_party->get_user(i);
            int state = m_party->get_party_type();
            CDungeon* dungeon = m_dungeonMgr.getDungeon();
            const char* dungeonName = dungeon->GetDungeonName();
            cUserHistoryLog* log = (cUserHistoryLog*)((char*)user + 0x79700);
            log->EnterDungeon(dungeonName, state);

            UserQuest* quest = user->getCurCharacQuestW();
            _Quest_Authen_Data* authData = (_Quest_Authen_Data*)((char*)quest + 0x75d8);
            authData->reset();

            int dungeonIdx = dungeon->get_index();
            quest->set_authen_data((QUEST_CONDITION)0, dungeonIdx, -1);
        }
    }
}

void CDeathTower::_onStartStage()
{
    m_stage.onStartStage();
}

// ---- CStage ----
void CDeathTower::CStage::reset()
{
    memset(this, 0, sizeof(*this));
}

void CDeathTower::CStage::onStartStage()
{
    m_startTick = GlobalData::s_systemTime_.getCurTickCount();
}

void CDeathTower::CStage::onFinishStage()
{
    m_finishTick = GlobalData::s_systemTime_.getCurTickCount();
}

int CDeathTower::CStage::getStageClearTime()
{
    return m_finishTick - m_startTick;
}

int CDeathTower::CStage::getStageMapIndex()
{
    return m_stageMapIndex;
}

int CDeathTower::CStage::getCurrentStage()
{
    return m_currentStage;
}
int CDeathTower::_pickupItemMoney(CUser* user, int memberCnt, const map_item& item, int* money1, int* money2)
{
    int money = item.m_item.get_add_info();
    int base = money / memberCnt;
    int remainder = money % memberCnt;
    int total = base + remainder;

    for (int i = 0; i < memberCnt; ++i)
    {
        CUser* member = m_party->get_user(i);
        if (!member || !m_party->checkValidUser(i))
            continue;

        int amount = (member == user) ? total : base;

        CInventory* invenW = (CInventory*)member->getCurCharacInvenW();
        money2[i] = invenW->gain_money(amount, (eMoneyAddReason)4, true, 1);
        money1[i] = amount;
    }

    return 0;
}
CUser* CDeathTower::_pickupItem(CUser* user, char* a2, int slot, const map_item& item, bool a5, char a6, const CItem* citem)
{
    (void)a2;
    (void)a5;
    (void)a6;

    if (slot <= 1)
    {
        return NULL;
    }

    // Determine if item should be routed
    bool isFromMap = (*(const char*)&item) != 0;
    bool routeFlag = !isFromMap;

    if (citem)
    {
        int rarity = citem->get_rarity();
        if (rarity > 3)
        {
            routeFlag = a6;
        }
        if (isFromMap && citem->isEpicRoutingItem())
        {
            routeFlag = true;
        }
    }

    if (isFromMap && citem && !citem->check_item_routing())
    {
        routeFlag = false;
    }

    if (!routeFlag)
    {
        return user;
    }

    // Routing: find next member
    int memberCnt = m_party->get_member_count();
    int routedTurn = m_playData.getLastRoutedTurn() + 1;
    if (routedTurn >= memberCnt)
    {
        routedTurn = 0;
    }

    for (int i = 0; i < memberCnt; ++i)
    {
        int idx = (routedTurn + i) % memberCnt;
        CUser* member = m_party->get_user(idx);
        if (!member || !m_party->checkValidUser(idx))
            continue;
        if (!m_playData.isMemberAlive(idx))
            continue;

        return member;
    }

    return NULL;
}
void CDeathTower::deathTowerCardStatistic(int count, CUser* user, const Inven_Item* items)
{
    unsigned int dungeonIdx = m_dungeonMgr.getDungeon()->get_index();
    bool isNotTowerOfDespair = (dungeonIdx != 0x2af8);
    int level = user->get_charac_level();

    for (int i = 0; i < count; ++i)
    {
        const Inven_Item* item = (const Inven_Item*)((const char*)items + i * 0x3d);
        int itemId = item->m_addInfo;
        if (itemId == 0)
        {
            int addInfo = item->get_add_info();
            GetInstanceValueStatistic()->AddValueStatistic(
                (VALUE_STATISTIC_FIELD)0xb, user, (unsigned int)addInfo);
        }
        else
        {
            CItem* citem = G_CDataManager()->find_item(itemId);
            if (citem)
            {
                int price = 0;
                Inven_Item itemCopy = *item;
                G_Store()->GetSellItemPrice(itemCopy, (const CItem*)citem, (short)count, false, price);
                GetInstanceValueStatistic()->AddValueStatistic(
                    (VALUE_STATISTIC_FIELD)0xc, user, (unsigned int)price);
            }
        }
    }
    (void)isNotTowerOfDespair;
    (void)level;
}
bool CDeathTower::_makeDropItem(CUser* user, char type, int slot, int count, map_item& item)
{
    Inven_Item invenItem;
    memset(&invenItem, 0, sizeof(invenItem));

    switch (type)
    {
    case 0: // money or equip
        if (count == 0)
        {
            // money item
            item.m_item.m_field1 = 0;
            *(char*)&item.m_item.m_amp = 2;
            item.m_item.set_add_info(0);
            item.m_item.ResetItemAttr();
        }
        else
        {
            CInventory* inven = (CInventory*)user->getCurCharacInvenR();
            inven->GetInvenSlotByRef(1, slot, invenItem);
            memcpy(&item.m_item, &invenItem, sizeof(Inven_Item));
        }
        return true;
    case 2: // cargo
    {
        const CCargo* cargo = user->getCurCharacCargoR();
        invenItem = cargo->get_cargo_slot(slot);
        memcpy(&item.m_item, &invenItem, sizeof(Inven_Item));
        return true;
    }
    case 3: // avatar
    {
        CInventory* inven = (CInventory*)user->getCurCharacInvenR();
        inven->GetInvenSlotByRef(2, slot, invenItem);
        memcpy(&item.m_item, &invenItem, sizeof(Inven_Item));
        return true;
    }
    case 7: // creature
    {
        CInventory* inven = (CInventory*)user->getCurCharacInvenR();
        inven->GetInvenSlotByRef(3, slot, invenItem);
        memcpy(&item.m_item, &invenItem, sizeof(Inven_Item));
        return true;
    }
    default:
        return false;
    }
}

int CDeathTower::_checkRenewMyRecord(unsigned int dungeonIdx, unsigned int memberCnt, unsigned int a, unsigned int b, unsigned int c)
{
    unsigned int towerIdx = sub_CDeathTowerRanking_makeDungeonIdx2TowerIdx(dungeonIdx);
    void* rankTable = sub_CDeathTowerRanking_getRankTable(memberCnt);
    sub_CDeathTowerRanking_checkRenewMyRecord(rankTable, towerIdx, a, b, c);
    return 0;
}

int CDeathTower::_checkRenewTopRecord(unsigned int dungeonIdx, unsigned int memberCnt, unsigned int a, unsigned int b)
{
    unsigned int towerIdx = sub_CDeathTowerRanking_makeDungeonIdx2TowerIdx(dungeonIdx);
    void* rankTable = sub_CDeathTowerRanking_getRankTable(memberCnt);
    return sub_CDeathTowerRanking_checkRenewTopRecord(rankTable, towerIdx, a, b);
}

int CDeathTower::_checkStartGameCondition(unsigned char& flag)
{
    Inven_Item items[4];
    int params[4];
    memset(items, 0, sizeof(items));
    memset(params, 0, sizeof(params));

    CDungeon* dungeon = m_dungeonMgr.getDungeon();
    if (m_party->CheckEnterAncientDungeon(dungeon, flag, items, params))
    {
        dungeon = m_dungeonMgr.getDungeon();
        if (m_party->UseAncientDungeonItems(dungeon, items, params))
        {
            return 0;
        }
        return 1;
    }
    return 0x11;
}
int CDeathTower::handleStartGame(int dungeonIdx, char flag, ENUM_DUNGEON_TYPE dungeonType)
{
    if (dungeonType == (ENUM_DUNGEON_TYPE)1)
    {
        throw 8;
    }

    m_dungeonMgr.reset();
    if (!m_dungeonMgr.initDungeonMgr(dungeonIdx))
    {
        throw 0x16;
    }

    unsigned char checkFlag = 0;
    int cond = _checkStartGameCondition(checkFlag);
    if (cond != 0)
    {
        throw cond;
    }

    m_stage.reset();
    m_stage.resetCurrentStage();
    CMap* map = m_dungeonMgr.getStageMap(m_stage.getCurrentStage());
    if (!map)
    {
        throw 0x15;
    }
    if (!m_stage.consistMap(&m_dungeonMgr, map, this))
    {
        throw 1;
    }

    m_playData.reset();
    m_playData.makeStartMemberInfo(m_party);

    for (int i = 0; i <= 3; ++i)
    {
        if (m_party->checkValidUser(i))
        {
            CUser* user = m_party->get_user(i);
            int minLevel = m_dungeonMgr.getDungeon()->get_min_level();
            if (user->get_charac_level() < minLevel)
            {
                throw 0xe;
            }
            user->set_state((ch_state)10);
            m_playData.setMemberAlive(i, true);
            int partyState = m_party->get_party_type();
            sub_CDungeonEntranceLog_IncrementDungeonEntrance(
                sub_GetInstanceDungeonEntranceLog(), dungeonIdx, partyState == 1);
        }
    }

    m_party->gen_timer_key((TIMER_MESSAGE)0x2d);
    m_party->SetSelectedEPLPCmd(-1);
    PacketGuard packet;
    packet.put_header(0, 0x8e);

    packet.put_int(dungeonIdx);
    packet.put_short(m_dungeonMgr.getEndStage());
    if (m_party == NULL)
    {
        packet.put_byte(0xb);
    }
    else
    {
        packet.put_byte(m_party->get_random_buff_type());
    }
    packet.finalize(true);
    m_party->send_to_party(packet);

    packet.clear();
    packet.put_header(0, 0x8f);
    m_stage.makeStagePacket(packet);
    packet.finalize(true);
    m_party->send_to_party(packet);

    _checkMemberReady();
    setTowerState((ENUM_TOWER_STATE)2);
    _onStartDeathTower();

    return 1;
}

// ---- CDungeonMgr ----
void CDeathTower::CDungeonMgr::reset()
{
    memset(this, 0, sizeof(*this));
}

bool CDeathTower::CDungeonMgr::initDungeonMgr(int dungeonIdx) { return true; }
CMap* CDeathTower::CDungeonMgr::getStageMap(int stage)
{
    CDungeon* dungeon = getDungeon();
    if (!dungeon) return NULL;
    int mapIdx = dungeon->getDeathTowerMapIndex(stage);
    return (CMap*)G_CDataManager()->find_map(mapIdx);
}

int CDeathTower::CDungeonMgr::getEndStage()
{
    CDungeon* dungeon = getDungeon();
    if (!dungeon) return 0;
    return dungeon->getDeathTowerMaxStage();
}

int CDeathTower::handleFinishLoading(CUser* user)
{
    int slot = m_party->GetMemberSlotNo(user);
    if (slot == -1 || !m_party->checkValidUser(slot))
    {
        return 0;
    }

    if (m_playData.checkEnterStartMap(slot))
    {
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x28);
    }
    m_playData.setMemberReady(slot, true);

    int memberCnt = m_party->get_member_count();
    if (m_playData.checkAllMemberReady(memberCnt))
    {
        PacketGuard packet;
        m_party->send_to_party(packet);
    }
    return 1;
}

int CDeathTower::handleDieUser(CUser* user)
{
    for (int i = 0; i <= 3; ++i)
    {
        if (m_party->checkValidUser(i) && m_party->get_user(i) == user)
        {
            if (!m_playData.isMemberAlive(i))
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0x2b, 0x12);
                return 0;
            }
            m_playData.setMemberAlive(i, false);

            PacketGuard packet;
            unsigned short uid = user->get_unique_id();
            // CPacketHandler::makeDieState(packet, uid)
            m_party->send_to_party(packet);

            if (_checkMemberDie())
            {
                _onFinishStage();
                _onPrepareFinishTower();
            }
            return 1;
        }
    }
    return 1;
}

int CDeathTower::handleDropItem(CUser* user, int a2, int a3, char a4, int a5, int a6)
{
    if (a6 < 0)
    {
        return 0;
    }

    map_item item;
    PacketGuard packet;

    if (_makeDropItem(user, a4, a5, a6, item))
    {
        int dropIdx = m_stage.dropItem(item);
        if (dropIdx == -1)
        {
            // makeDropItemError(packet, 0x32, 0x16, a4)
            user->Send(packet);
            return 0;
        }
    }
    return 1;
}

int CDeathTower::CStage::killMonster(int idx, map_monster& monster)
{
    std::map<int, map_monster>::iterator it = m_monsterMap.find(idx);
    if (it == m_monsterMap.end())
    {
        return 0;
    }
    monster = it->second;
    int field14 = (unsigned char)it->second.m_flag14;
    m_monsterMap.erase(it);
    return field14;
}

bool CDeathTower::CStage::peekItem(int idx, map_item& item)
{
    std::map<int, map_item>::iterator it = m_itemMap.find(idx);
    if (it == m_itemMap.end())
    {
        return false;
    }
    item = it->second;
    return true;
}

int CDeathTower::handleLeaveUser(CUser* user)
{
    for (int i = 0; i <= 3; ++i)
    {
        if (m_party->checkValidUser(i) && m_party->get_user(i) == user)
        {
            m_party->leave_user(user, (ENUM_PARTY_INFO_TYPE)2);
            m_playData.setMemberAlive(i, false);
        }
    }

    G_GameWorld()->out_from_dungeon(user);

    PacketGuard packet;
    packet.put_header(0, 2);
    packet.put_byte(0);
    packet.put_short(1);
    user->make_basic_info((char*)&packet, 0);
    packet.finalize(true);
    G_GameWorld()->send_all(packet, user);

    onLeaveUser(user);

    bool allDead = false;
    if (m_party->get_member_count() > 0)
    {
        allDead = _checkMemberDie() != 0;
    }
    if (allDead)
    {
        _onFinishStage();
        _onPrepareFinishTower();
    }
    return 1;
}

int CDeathTower::handleMoveMap()
{
    if (_checkMemberDie())
    {
        PacketGuard packet;
        // makeCommonError(packet, 0x30, 0x16)
        m_party->send_to_party(packet);
        return 0;
    }

    if (m_stage.getState() < 2)
    {
        PacketGuard packet;
        // makeCommonError(packet, 0x30, 0x13)
        m_party->send_to_party(packet);
        return 0;
    }

    unsigned int endStage = m_dungeonMgr.getEndStage();
    if (!m_stage.moveNextStage(endStage))
    {
        PacketGuard packet;
        // makeCommonError(packet, 0x30, 0x16)
        m_party->send_to_party(packet);
        return 0;
    }

    CMap* map = m_dungeonMgr.getStageMap(m_stage.getCurrentStage());
    if (!map)
    {
        throw 0x15;
    }

    m_stage.reset();
    if (!m_stage.consistMap(&m_dungeonMgr, map, this))
    {
        throw 1;
    }

    PacketGuard packet;
    packet.put_header(0, 0x8f);
    m_stage.makeStagePacket(packet);
    packet.finalize(true);
    m_party->send_to_party(packet);

    _checkMemberReady();
    return 1;
}

int CDeathTower::handlePickupItem(CUser* user, int idx, bool a3, char a4)
{
    map_item item;
    PacketGuard packet;

    if (!m_stage.peekItem(idx, item))
    {
        // makeCommonError(packet, 0x2e, a4 < 4 ? 0x15 : 0x51)
        user->Send(packet);
        return 0;
    }

    int memberCnt = m_party->get_member_count();
    if (memberCnt < 1)
    {
        // makeCommonError(packet, 0x2e, 0x15)
        user->Send(packet);
        return 0;
    }

    int itemId = *(int*)((char*)&item + 0x10);
    if (itemId == 0)
    {
        // Money item
        if (!m_stage.pickupItem(idx, item))
        {
            // makeCommonError(packet, 0x2e, 0x15)
            user->Send(packet);
            return 0;
        }
        int money1 = 0, money2 = 0;
        _pickupItemMoney(user, memberCnt, item, &money1, &money2);
        unsigned short uid = user->get_unique_id();
        // makePickupItemHeader(packet, uid, idx)
        // appendPickupMoney(packet, &money1, &money2, m_party)
    }
    else
    {
        CItem* citem = G_CDataManager()->find_item(itemId);
        if (!citem)
        {
            return 0;
        }

        int itemType = 0;
        if (citem->is_stackable())
        {
            if (citem->GetItemType() == 9)
            {
                itemType = 7;
            }
        }
        else
        {
            if (citem->IsCreatureItem())
            {
                itemType = 7;
            }
            else
            {
                if (citem->IsAvatarItem())
                {
                    itemType = 1;
                }
            }
        }

        char ipgNo[0x18] = {0};
        CUser* targetUser = _pickupItem(user, ipgNo, memberCnt, item, a3, a4, citem);
        if (!targetUser)
        {
            // makeCommonError(packet, 0x2e, 0x16)
            user->Send(packet);
            return 0;
        }

        int slot = -1;
        Inven_Item* invenItem = (Inven_Item*)&item;
        if (invenItem->isAvatarItemType())
        {
            int characNo = targetUser->getCurCharacNo();
            WongWork::CGenUniqueNo::genIPGNo(
                (WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE)5, characNo, ipgNo);
            slot = targetUser->addAvatarItem(itemId, 0, (char)item.m_count, ipgNo, (eAvatarItemAddReason)0x10);
        }
        else
        {
            CInventory* inven = (CInventory*)targetUser->getCurCharacInvenW();
            slot = inven->insertItemIntoInventory(*invenItem, (eItemAddReason)4, true, true);
        }

        if (slot == -1)
        {
            // makeCommonError(packet, 0x2e, 4)
            user->Send(packet);
            return 0;
        }

        if (citem->IsRandomOption())
        {
            // statistc_proxy::add(...)
        }

        if (!m_stage.pickupItem(idx, item))
        {
            // makeCommonError(packet, 0x2e, 0x15)
            user->Send(packet);
            return 0;
        }

        unsigned short uid = user->get_unique_id();
        // makePickupItemHeader(packet, uid, idx)
        unsigned short targetUid = targetUser->get_unique_id();
        // appendPickupItem(packet, ipgNo, targetUid, slot, itemType)
    }

    m_party->send_to_party(packet);
    return 1;
}

int CDeathTower::handleSelectEPLPCommand(CUser* user, int cmd)
{
    int slot = m_party->GetMemberSlotNo(user);
    if (slot == -1 || !m_party->checkValidUser(slot))
    {
        return 0;
    }

    m_party->SetEPLPState(3);
    int key = _genTimerKey((TIMER_MESSAGE)0x4e);
    G_TimerQueue()->InsertTimer(
        (TimerEntry::OBJ_TYPE)6, m_idx, (TIMER_MESSAGE)0x4e, cmd, key, 0x4e);
    return 1;
}
int CDeathTower::handleStageCommand(CUser* user, unsigned char cmd)
{
    if (m_party->getManager() != user)
    {
        return 0;
    }
    if (cmd == 1)
    {
        if (!m_stage.setState((ENUM_STAGE_STATE)1))
        {
            return 1;
        }
        if (_checkMemberDie())
        {
            return 1;
        }
        _onStartStage();
    }
    else if (cmd == 2)
    {
        if (!m_stage.setState((ENUM_STAGE_STATE)2))
        {
            return 1;
        }
    }
    else
    {
        return 0;
    }
    return 1;
}

int CDeathTower::CStage::getState() { return m_state; }
bool CDeathTower::CStage::checkClearStage()
{
    for (std::map<int, map_monster>::iterator it = m_monsterMap.begin();
         it != m_monsterMap.end(); ++it)
    {
        map_monster& mob = it->second;
        if (mob.m_dropCnt == 100 && mob.m_flag13 == 0)
        {
            return false;
        }
    }
    return true;
}


bool CDeathTower::CStage::setState(ENUM_STAGE_STATE state)
{
    if (state == 1)
    {
        if (m_state != 0)
        {
            return false;
        }
    }
    else if (state == 2)
    {
        if (m_state != 1)
        {
            return false;
        }
    }
    else
    {
        return false;
    }
    m_state = state;
    return true;
}
bool CDeathTower::CStage::moveNextStage(unsigned int endStage)
{
    if (checkClearStage() && (unsigned int)m_currentStage < endStage)
    {
        m_currentStage++;
        return true;
    }
    return false;
}


bool CDeathTower::CStage::pickupItem(int idx, map_item& item)
{
    std::map<int, map_item>::iterator it = m_itemMap.find(idx);
    if (it == m_itemMap.end())
    {
        return false;
    }
    item = it->second;
    m_itemMap.erase(it);
    return true;
}
int CDeathTower::CStage::dropItem(const map_item& item)
{
    int idx = m_dropItemCnt;
    m_dropItemCnt++;
    map_item localItem = item;
    localItem.m_itemIndex = idx;
    m_itemMap[idx] = localItem;
    return idx;
}
bool CDeathTower::CStage::consistMap(CDungeonMgr* dungeonMgr, CMap* map, CDeathTower* tower)
{
    m_stageMapIndex = map->get_index();

    std::vector<unsigned int> mobIdVec;
    CAICharacterList* aiList = G_CDataManager()->m_aiCharacterList;
    std::map<int, int> apMap;

    int mobIdx = 0;
    const std::vector<STAICharacterMapArrangeData>* aiChars = map->getAICharacters();

    if (!aiChars->empty())
    {
        for (size_t i = 0; i < aiChars->size(); i++)
        {
            const STAICharacterMapArrangeData& data = (*aiChars)[i];
            CAICharacter* aiChar = (CAICharacter*)aiList->get(aiList, data.m_type);
            if (!aiChar)
            {
                return false;
            }

            unsigned int aiIdx = aiChar->getIdx();
            mobIdVec.push_back(aiIdx);

            int ap = aiChar->getAppearancePoint();
            if (ap != 0)
            {
                std::map<int, int>::iterator it = apMap.find(ap);
                if (it != apMap.end())
                {
                    int val = it->second - 1;
                    it->second = std::max(val, 0);
                }
            }

            map_monster mob;
            memset(&mob, 0, sizeof(mob));
            mob.m_mobId = mobIdx;
            mob.m_instId = m_instIdCounter;
            m_instIdCounter++;
            mob.m_roleType = 5;
            mob.m_mobIndex = data.m_type;
            mob.m_level = aiChar->getLevel();
            mob.m_dropCnt = data.m_level;

            m_monsterMap[mobIdx] = mob;
            mobIdx++;
        }
    }

    // AP random monster handling (simplified)
    int apRandomPoint = map->getAPCRandomPoint();
    if (apRandomPoint != 0)
    {
        // TODO: full AP random monster logic
        mobIdx = 0x40;
    }

    return true;
}

int CDeathTower::handleUseStackable(CUser* user, ENUM_ITEMSPACE itemSpace, unsigned short slot)
{
    int invenType = GetInvenTypeFromItemSpace((int)itemSpace);
    CInventory* inven = (CInventory*)user->getCurCharacInvenR();
    Inven_Item* item = inven->GetInvenRef(invenType, slot);
    if (!item)
    {
        return 0;
    }

    CDungeon* dungeon = m_dungeonMgr.getDungeon();
    if (dungeon->limitOfStackableItemInTower())
    {
        unsigned int itemId = *(unsigned int*)((char*)item + 2);
        if (!((itemId < 0x1964 || itemId > 6999) && itemId != 0x18))
        {
            return 1;
        }
        if (itemId < 0x19)
        {
            if (itemId != 10)
            {
                return 0;
            }
        }
        else if (itemId != 0x33 && itemId != 0x28978a)
        {
            return 0;
        }
    }
    return 1;
}

int CDeathTower::onTimer(TIMER_MESSAGE msg, unsigned int key)
{
    if (!_checkTimerKey(msg, key))
    {
        return 0;
    }

    if (getTowerState() == 0)
    {
        return 0;
    }

    switch (msg)
    {
    case 0x49:
    {
        int memberCnt = m_party->get_member_count();
        if (memberCnt == 0)
        {
            _destroy();
            return 1;
        }

        bool someoneLeft = false;
        for (int i = 0; i < 4; ++i)
        {
            if (m_party->checkValidUser(i))
            {
                CUser* user = m_party->get_user(i);
                if (!m_playData.isMemberReady(i))
                {
                    PacketGuard packet;
                    user->Send(packet);
                    handleLeaveUser(user);
                    someoneLeft = true;
                }
            }
        }

        memberCnt = m_party->get_member_count();
        if (!someoneLeft || memberCnt < 1)
        {
            if (memberCnt == 0)
            {
                _destroy();
            }
        }
        else
        {
            for (int i = 0; i < 4; ++i)
            {
                m_playData.setMemberReady(i, true);
            }
            PacketGuard packet;
            m_party->send_to_party(packet);
        }
        break;
    }
    case 0x4b:
    {
        if (getTowerState() < 3)
        {
            return 0;
        }
        for (int i = 0; i < 4; ++i)
        {
            if (m_party->checkValidUser(i))
            {
                CUser* user = m_party->get_user(i);
                PacketGuard packet;
                Inven_Item* item3 = m_playData.getRewardItem(3);
                int cnt3 = m_playData.getRewardItemCount(3);
                Inven_Item* item2 = m_playData.getRewardItem(2);
                int cnt2 = m_playData.getRewardItemCount(2);
                Inven_Item* item1 = m_playData.getRewardItem(1);
                int cnt1 = m_playData.getRewardItemCount(1);
                Inven_Item* item0 = m_playData.getRewardItem(0);
                int cnt0 = m_playData.getRewardItemCount(0);
                int exp = m_playData.getRewardExp(i);
                Inven_Item* myItem = m_playData.getRewardItem(i);
                int myCnt = m_playData.getRewardItemCount(i);
                deathTowerCardStatistic(myCnt, user, myItem);
                user->Send(packet);
            }
        }
        setTowerState((ENUM_TOWER_STATE)4);
        int tkey = _genTimerKey((TIMER_MESSAGE)0x4c);
        G_TimerQueue()->InsertTimer(
            (TimerEntry::OBJ_TYPE)6, m_idx, (TIMER_MESSAGE)0x4c, 5, tkey, 0x4c);
        break;
    }
    case 0x4c:
    {
        if (getTowerState() < 4)
        {
            return 0;
        }
        bool allHaveItem = true;
        for (int i = 0; i < 4; ++i)
        {
            if (m_party->checkValidUser(i))
            {
                CUser* user = m_party->get_user(i);
                Inven_Item item;
                const CInventory* inven = (const CInventory*)user->getCurCharacInvenR();
                if (inven->GetInvenData(0x1057, item) < 0)
                {
                    allHaveItem = false;
                    break;
                }
            }
        }

        PacketGuard packet;
        m_party->send_to_party(packet);

        for (int i = 0; i < 4; ++i)
        {
            if (m_party->checkValidUser(i))
            {
                CUser* user = m_party->get_user(i);
                int rewardCnt = m_playData.getRewardItemCount(i);
                for (int j = 0; j < rewardCnt; ++j)
                {
                    Inven_Item* rewardItem = m_playData.getRewardItem(i);
                    int itemId = *(int*)((char*)rewardItem + j * 0x3d + 2);
                    if (itemId != -1)
                    {
                        int slot = -1;
                        if (itemId == 0)
                        {
                            int addInfo = rewardItem->get_add_info();
                            CInventory* inven = (CInventory*)user->getCurCharacInvenW();
                            int gained = inven->gain_money(addInfo, (eMoneyAddReason)5, true, 0);
                            if (gained < addInfo)
                            {
                                user->SendMoneyFullReason((ENUM_MONEY_FULL_REASON)0, addInfo, gained);
                            }
                            slot = 0;
                            CDungeon* dungeon = m_dungeonMgr.getDungeon();
                            int dungeonIdx = dungeon->get_index();
                            if (dungeonIdx == 11000)
                            {
                                GetInstanceCirculationStatistic()->AddCirculationStatistic((CIRCULATION_STATISTIC_FIELD)4, user, (unsigned int)gained);
                            }
                            else
                            {
                                GetInstanceCirculationStatistic()->AddCirculationStatistic((CIRCULATION_STATISTIC_FIELD)5, user, (unsigned int)gained);
                            }
                        }
                        else
                        {
                            CInventory* inven = (CInventory*)user->getCurCharacInvenW();
                            slot = inven->insertItemIntoInventory(*rewardItem, (eItemAddReason)0xb, true, true);
                        }

                        if (slot >= 0)
                        {
                            PacketGuard updatePacket;
                            user->Send(updatePacket);
                        }
                    }
                }

                int exp = m_playData.getRewardExp(i);
                user->calcHelpAbuseRatio(exp, false);
                int a = 0, b = 0;
                if (user->gain_exp_sp(exp, a, b, (eExpAddReason)2, 0, false))
                {
                    user->SendNotiPacket((CUser::eSendTarget)2, (ENUM_NOTIPACKET)2, 1);
                }
            }
        }

        setTowerState((ENUM_TOWER_STATE)5);
        m_party->SetEPLPState(2);
        int tkey = _genTimerKey((TIMER_MESSAGE)0x4d);
        G_TimerQueue()->InsertTimer(
            (TimerEntry::OBJ_TYPE)6, m_idx, (TIMER_MESSAGE)0x4d, 0x3c, tkey, 0x4d);
        break;
    }
    case 0x4d:
    {
        if (m_party->GetEPLPState() != 2)
        {
            return 0;
        }
        if (m_party->GetSelectedEPLPCmd() == 3)
        {
            return 0;
        }
        _onFinishDeathTower();
        break;
    }
    case 0x4e:
    {
        if (m_party->GetEPLPState() != 3)
        {
            return 0;
        }
        _onFinishDeathTower();
        break;
    }
    case 0x4a:
    {
        if (getTowerState() < 2)
        {
            return 0;
        }
        if (m_party->get_member_count() < 1)
        {
            _destroy();
            return 1;
        }
        if (_checkMemberDie())
        {
            _beginTowerClearProcess(false);
        }
        else
        {
            int curStage = m_stage.getCurrentStage();
            int endStage = m_dungeonMgr.getEndStage();
            if (curStage == endStage)
            {
                _beginTowerClearProcess(true);
            }
        }
        break;
    }
    default:
        break;
    }

    return 1;
}

Inven_Item* CDeathTower::CPlayData::getRewardItem(int idx)
{
    return (Inven_Item*)m_pad18 + idx;
}

int CDeathTower::CPlayData::getRewardItemCount(int idx)
{
    return m_rewardItemCount[idx];
}

int CDeathTower::CPlayData::getRewardExp(int idx)
{
    return m_rewardExp[idx];
}

int CDeathTower::CPlayData::getLastRoutedTurn()
{
    return m_lastRoutedTurn;
}

CDungeon* CDeathTower::CDungeonMgr::getDungeon() const
{
    return m_pDungeon;
}

void CDeathTower::CStage::resetCurrentStage()
{
    m_currentStage = 0;
}

void CDeathTower::CStage::makeStagePacket(PacketGuard& packet)
{
    packet.put_short(m_currentStage);
    packet.put_int(get_rand_int(0));
    packet.put_short(m_stageMapIndex);
    packet.put_byte(m_monsterMap.size());

    for (std::map<int, map_monster>::iterator it = m_monsterMap.begin();
         it != m_monsterMap.end(); ++it)
    {
        map_monster& mob = it->second;
        packet.put_int(mob.m_mobId);
        packet.put_short(mob.m_instId);
        packet.put_int(mob.m_mobIndex);
        packet.put_byte(mob.m_level);
        packet.put_byte(mob.m_roleType);
        packet.put_byte(mob.m_flag13);
        packet.put_byte(mob.m_flag14);
    }
}

void CDeathTower::CPlayData::reset()
{
    memset(this, 0, sizeof(*this));
}

int CDeathTower::CPlayData::getStartMemberCnt()
{
    return m_field0;
}

void CDeathTower::CPlayData::makeStartMemberInfo(CParty* party)
{
    for (int i = 0; i < 4; ++i)
    {
        m_memberAlive[i] = 0;
        m_memberReady[i] = 0;
    }
    m_allReady = 0;
}

void CDeathTower::CPlayData::resetMemberReady()
{
    m_allReady = 0;
    for (int i = 0; i < 4; ++i)
    {
        m_memberReady[i] = 0;
    }
}

void CDeathTower::CPlayData::setMemberAlive(int idx, bool alive)
{
    m_memberAlive[idx] = alive ? 1 : 0;
}

int CDeathTower::CPlayData::isMemberAlive(int idx)
{
    return m_memberAlive[idx];
}

void CDeathTower::CPlayData::setMemberReady(int idx, bool ready)
{
    m_memberReady[idx] = ready ? 1 : 0;
}

int CDeathTower::CPlayData::isMemberReady(int idx)
{
    return m_memberReady[idx];
}

bool CDeathTower::CPlayData::checkEnterStartMap(int idx)
{
    return m_memberAlive[idx] != 0;
}

bool CDeathTower::CPlayData::checkAllMemberReady(int memberCnt)
{
    for (int i = 0; i < memberCnt; ++i)
    {
        if (!m_memberReady[i])
        {
            return false;
        }
    }
    return true;
}

void CDeathTower::CPlayData::addPlayTime(unsigned int time)
{
    m_lastPlayTime = m_playTime;
    m_playTime += time;
}

unsigned int CDeathTower::CPlayData::getPlayTime()
{
    return m_playTime;
}

unsigned int CDeathTower::CPlayData::getLastPlayTime()
{
    return m_lastPlayTime;
}

} // namespace WongWork
