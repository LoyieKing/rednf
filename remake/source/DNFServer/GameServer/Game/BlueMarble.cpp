// df_game_r BlueMarble（G2-3 PvP 链，大富翁房间，池化 300）还原（2026-08-17）。
// 逐函数对照 docs/class_func_reports/BlueMarble.md 与 ORIG 反汇编实现；
// 目标：编译产物与 ORIG 逐操作数一致（AE 口径）。
// 依赖子对象（BlueMarbleScriptManager/BlueMarbleUserInfo/CMTRand/...）为
// PvP_deps 桩；本 TU 以 asm-label extern 发跨类调用。只写本文件。
#include <cstdio>
#include <cstring>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "BlueMarble.h"
#include "CDataManager.h"
#include "CEnvironment.h"
#include "LogManager.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

// ============================================================================
// PacketGuard（ORIG 0xc 字节：InterfacePacketBuf + int + char；
//      构造/析构由 GameStubs 提供单例定义）
// ============================================================================
class PacketGuard : public InterfacePacketBuf
{
public:
    PacketGuard();
    ~PacketGuard();

private:
    int m_nField4;
    char m_cField8;
};

// ---- BlueMarbleScriptManager +0x30 起字段布局（成员访问还原 ORIG 直接位移寻址） ----
struct BM_ScriptFields
{
    int m_field0[12];   // +0x00..+0x2c
    int m_maxUser;      // +0x30
    int m_minLevel;     // +0x34
    int m_maxLevel;     // +0x38
    int m_enterLimit;   // +0x3c
    int m_field40;      // +0x40
    int m_diceMin;      // +0x44
    int m_diceRange;    // +0x48
    int m_field4c;      // +0x4c
};

// ---- CInventory 最小声明（仅本 TU 需要的两个方法；CInventory.cpp 提供定义） ----
class CInventory
{
public:
    int insertItemIntoInventory(Inven_Item item, eItemAddReason reason,
                                bool bLog, bool b);
    void MakeItemPacket(int invenType, int slot, PacketGuard& packet) const;
};

// ---- CGameManager 最小声明（CheckOutParty / 单例；CGameManager.cpp 提供定义） ----
class CGameManager
{
public:
    void CheckOutParty(CUser* user, bool flag);
};
extern CGameManager* G_CGameManager();

// ============================================================================
// 跨类 / 子对象方法（asm-label extern；PvP_deps.cpp / GameStubs 提供链接桩）
// ============================================================================
// BlueMarbleInfoScript 三个方法：mangled 名是 Ei（ORIG 声明怪癖），
// 实际调用传 3 参数：(vector<int>& out, BlueMarbleInfoScript*（m_pScript+0x30）, int zone)
extern "C" void sub_BMInfoScript_getUniqueDungeonIndex(void* out, void* script, int zone)
    asm("_ZN20BlueMarbleInfoScript21getUniqueDungeonIndexEi");
extern "C" void sub_BMInfoScript_getBossDungeonIndex(void* out, void* script, int zone)
    asm("_ZN20BlueMarbleInfoScript19getBossDungeonIndexEi");
extern "C" void sub_BMInfoScript_getRandomDungeonIndex(void* out, void* script, int zone)
    asm("_ZN20BlueMarbleInfoScript21getRandomDungeonIndexEi");

extern "C" void* sub_G_GameWorld() asm("_Z11G_GameWorldv");
extern "C" void sub_GameWorld_gotoBlueMarble(void* world, void* user)
    asm("_ZN9GameWorld14gotoBlueMarbleEP5CUser");
extern "C" void sub_GameWorld_outFromBlueMarble(void* world, void* user)
    asm("_ZN9GameWorld17outFromBlueMarbleEP5CUser");

extern "C" unsigned char sub_CUser_isCompetitionMercenary(void* user)
    asm("_ZNK5CUser22isCompetitionMercenaryEv");

extern "C" void* sub_GetInstancePrivateStoreMgr()
    asm("_ZN13private_store26GetInstancePrivateStoreMgrEv");
extern "C" bool sub_CPrivateStoreMgr_IsBusyPrivateStore(void* mgr, void* user)
    asm("_ZN13private_store16CPrivateStoreMgr18IsBusyPrivateStoreEP5CUser");

extern "C" char* sub_RDARScriptStringManager_findString(void* mgr, int idx,
                                                        const char* key, bool* out)
    asm("_ZNK23RDARScriptStringManager10findStringEiPKcPb");
extern "C" int sub_CMailBoxHelper_ReqDBSendNewSystemMail(
    const char* title, const void* item, unsigned b, unsigned c, const char* msg,
    int d, int e, int group, bool f, bool g)
    asm("_ZN8WongWork14CMailBoxHelper22ReqDBSendNewSystemMailEPKcRK10Inven_ItemjjS2_ij17ENUM_SERVER_GROUPbb");
extern "C" void sub_cUserHistoryLog_SendMail(void* self, void* mail, unsigned v)
    asm("_ZN15cUserHistoryLog8SendMailEP16MSG_MAILBOX_SENDj");

// ============================================================================
// BlueMarble 实现
// ============================================================================

BlueMarble::BlueMarble()
    : m_memoryPoolIndex(-1), m_curUserCount(0), m_field18(0), m_field1c(0),
      m_pScript(0), m_field3c(0)
{
    m_pRands = new CMTRand[4];
    unsigned long seedValue = (unsigned long)time((time_t*)0);
    for (int i = 0; i <= 3; ++i)
    {
        m_pRands[i].seed(seedValue);
    }
    m_users.clear();
    m_set.clear();
}

BlueMarble::~BlueMarble()
{
    if (m_pRands)
    {
        delete[] m_pRands;
    }
    m_pRands = 0;
}

int BlueMarble::getMemoryPoolIndex() const
{
    return m_memoryPoolIndex;
}

void BlueMarble::setMemoryPoolIndex(int idx)
{
    m_memoryPoolIndex = idx;
}

int BlueMarble::getState() const
{
    return m_curUserCount;
}

int BlueMarble::changeState(BlueMarbleState::T state)
{
    m_curUserCount = state;
}

int BlueMarble::getWinner() const
{
    int i = 0;
    while (i < ((BM_ScriptFields*)m_pScript)->m_maxUser)
    {
        if (m_users[i].getUser() != 0)
        {
            if (m_users[i].getGrade() == 0)
            {
                return i;
            }
        }
        ++i;
    }
    return -1;
}

int BlueMarble::getWaitUserCount() const
{
    int count = 0;
    for (int i = 0; i < ((BM_ScriptFields*)m_pScript)->m_maxUser; ++i)
    {
        if (m_users[i].getUser() != 0)
        {
            ++count;
        }
    }
    return count;
}

int BlueMarble::getUserSlot(CUser* user) const
{
    if (user == 0)
    {
        return -1;
    }
    for (int i = 0; i < ((BM_ScriptFields*)m_pScript)->m_maxUser; ++i)
    {
        if (m_users[i].getUser() == user)
        {
            return i;
        }
    }
    return -1;
}

int BlueMarble::getUserState(int seat) const
{
    return m_users[seat].getUserState();
}

void BlueMarble::setUserState(int seat, BlueMarbleUserState::T state)
{
    m_users[seat].setUserState(state);
}

int BlueMarble::setUserGrade(int grade)
{
    std::multimap<int, int, std::greater<int> > mm;
    m_users[grade].setGrade(BlueMarbleUserGrade::T_0);
    for (int i = 0; i < ((BM_ScriptFields*)m_pScript)->m_maxUser; ++i)
    {
        if (m_users[i].getUser() != 0)
        {
            if (m_users[i].getGrade() != 0)
            {
                int zone = m_users[i].getBoardZone();
                mm.insert(std::make_pair(std::move(zone), i));
            }
        }
    }
    std::multimap<int, int, std::greater<int> >::iterator it = mm.begin();
    int curGrade = 0;
    int nextGrade = 1;
    int lastZone = 0;
    while (it != mm.end())
    {
        if (it->first == lastZone)
        {
            lastZone = it->first;
            m_users[it->second].setGrade((BlueMarbleUserGrade::T)curGrade);
        }
        else
        {
            lastZone = it->first;
            m_users[it->second].setGrade((BlueMarbleUserGrade::T)nextGrade);
            curGrade = nextGrade;
            ++nextGrade;
        }
        ++it;
    }
}

int BlueMarble::getUserDeadCount(int seat) const
{
    return m_users[seat].getUser()->getBlueMarbleUserDeadCount();
}

void BlueMarble::setUserDeadCount(int count)
{
    m_users[count].getUser()->increaseBlueMarbleUserDeadCount();
}

int BlueMarble::getMaxBoardSize() const
{
    return m_field3c;
}

int BlueMarble::setMaxBoardSize()
{
    BlueMarbleMapScript* map = m_pScript->getMap(getBlueMarbleMap());
    if (map != 0)
    {
        m_field3c = (int)((std::vector<BlueMarbleTileScript*>*)map)->size();
    }
}

int BlueMarble::getDiceNumber() const
{
    return m_field18;
}

int BlueMarble::setDiceNumber()
{
    m_field18 = (unsigned int)m_pRands[0].randInt() %
                    (unsigned int)((BM_ScriptFields*)m_pScript)->m_diceRange +
                ((BM_ScriptFields*)m_pScript)->m_diceMin;
}

int BlueMarble::getBlueMarbleMap() const
{
    return m_field1c;
}

int BlueMarble::setBlueMarbleMap()
{
    m_field1c = (unsigned int)m_pRands[1].randInt() %
                    (unsigned int)m_pScript->getMapTotalCount() +
                1;
}

int BlueMarble::getMovePosition(int seat) const
{
    return m_users[seat].getBoardZone();
}

int BlueMarble::setMovePosition(int seat, int pos)
{
    m_users[seat].setBoardZone(pos, getMaxBoardSize());
}

int BlueMarble::getVisitZone(int seat) const
{
    std::set<int>::const_iterator it = m_set.find(seat);
    if (it == m_set.end())
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int BlueMarble::setVisitZone(int zone)
{
    std::set<int>::const_iterator it = m_set.find(zone);
    if (it == m_set.end())
    {
        m_set.insert(zone);
    }
}

int BlueMarble::getDungeonIndex(int seat) const
{
    return m_users[seat].getDungeon();
}

int BlueMarble::setDungeonIndex(int seat, int dungeon)
{
    m_users[seat].setDungeon(dungeon);
}

int BlueMarble::getItemIndex(int seat) const
{
    return m_users[seat].getItem();
}

int BlueMarble::setItemIndex(int seat, int itemIdx)
{
    m_users[seat].setItem((unsigned int)itemIdx);
}

int BlueMarble::getExpBuff(int seat) const
{
    return m_users[seat].getExp();
}

int BlueMarble::setExpBuff(int seat, int exp)
{
    m_users[seat].setExp(exp);
}

int BlueMarble::getGoldBuff(int seat) const
{
    return m_users[seat].getGold();
}

int BlueMarble::setGoldBuff(int seat, int gold)
{
    m_users[seat].setGold(gold);
}

int BlueMarble::getDropBuff(int seat) const
{
    return m_users[seat].getDrop();
}

int BlueMarble::setDropBuff(int seat, int drop)
{
    m_users[seat].setDrop(drop);
}

int BlueMarble::getBuff(CUser* user, BlueMarbleBuffType::T type)
{
    if (user == 0)
    {
        return 0;
    }
    int seat = getUserSlot(user);
    if (seat == -1)
    {
        LogManager::logFormat(1, "BlueMarble.cpp",
                              "int BlueMarble::getBuff(CUser*, BlueMarbleBuffType::T)",
                              0x4c7, "BlueMarble::getBuff slot(%d)", seat);
        return 0;
    }
    if (type == (BlueMarbleBuffType::T)1)
    {
        return getGoldBuff(seat);
    }
    else if (type == (BlueMarbleBuffType::T)2)
    {
        return getDropBuff(seat);
    }
    else if (type == (BlueMarbleBuffType::T)0)
    {
        return getExpBuff(seat);
    }
    else
    {
        return 0;
    }
}

int BlueMarble::getRandomItem() const
{
    unsigned long range = (unsigned long)((BM_ScriptFields*)m_pScript)->m_field4c;
    int r = m_pRands[3].randInt(range);
    std::vector<BlueMarbleRandomItemInfo>* items =
        (std::vector<BlueMarbleRandomItemInfo>*)((char*)m_pScript + 0x98);
    int count = (int)items->size();
    int acc = 0;
    unsigned int i = 0;
    while ((int)i < count)
    {
        acc += (*items)[i].m_weight;
        if (acc >= r)
        {
            return (*items)[i].m_itemIndex;
        }
        ++i;
    }
    return 0;
}

int BlueMarble::getRandomDungeon(int seat, BlueMarbleTileScript::Type type) const
{
    std::vector<int> local;
    int level = m_users[seat].getUser()->get_charac_level();
    BlueMarbleMapScript* map = m_pScript->getMap(getBlueMarbleMap());
    if (map == 0)
    {
        return 0;
    }
    int tile = (int)m_pScript->getMap(getBlueMarbleMap())->getTile(
        (unsigned int)m_users[seat].getBoardZone());
    if (tile == 0)
    {
        return 0;
    }
    if (*(char*)(tile + 4) != 1)
    {
        return 0;
    }
    {
        std::vector<int> tmp;
        if (type == (BlueMarbleTileScript::Type)7)
        {
            sub_BMInfoScript_getUniqueDungeonIndex(&tmp, (char*)m_pScript + 0x30, level);
        }
        else if (type == (BlueMarbleTileScript::Type)8 ||
                 type == (BlueMarbleTileScript::Type)2)
        {
            sub_BMInfoScript_getBossDungeonIndex(&tmp, (char*)m_pScript + 0x30, level);
        }
        else
        {
            sub_BMInfoScript_getRandomDungeonIndex(&tmp, (char*)m_pScript + 0x30, level);
        }
        local = std::move(tmp);
    }
    if (local.size() == 0)
    {
        return 0;
    }
    int r = m_pRands[2].randInt();
    unsigned int idx = (unsigned int)r % (unsigned int)local.size();
    if (idx >= local.size())
    {
        return 0;
    }
    return local[idx];
}

int BlueMarble::checkJoin()
{
    if (getState() != 0)
    {
        return 0;
    }
    if (getWaitUserCount() >= ((BM_ScriptFields*)m_pScript)->m_maxUser)
    {
        return 0;
    }
    return 1;
}

int BlueMarble::join(CUser* user)
{
    if (user == 0)
    {
        return 0x17;
    }
    for (int i = 0; i < ((BM_ScriptFields*)m_pScript)->m_maxUser; ++i)
    {
        if (m_users[i].getUser() == 0)
        {
            m_users[i].setUser(user);
            short idx = (short)getMemoryPoolIndex();
            m_users[i].getUser()->setBlueMarbleIndex(idx);
            m_users[i].getUser()->setMoveSpace((UserSpace::T)1);
            break;
        }
    }
    sub_GameWorld_gotoBlueMarble(sub_G_GameWorld(), user);
    sendBlueMarbleRoomInfo();
    return 0;
}

int BlueMarble::leaveUser(CUser* user)
{
    if (user == 0)
    {
        LogManager::logFormat(1, "BlueMarble.cpp",
                              "void BlueMarble::leaveUser(CUser*)", 0x3f9,
                              "BlueMarble::leaveUser user is NULL");
    }
    else
    {
        int seat = getUserSlot(user);
        if (seat == -1)
        {
            LogManager::logFormat(1, "BlueMarble.cpp",
                                  "void BlueMarble::leaveUser(CUser*)", 0x400,
                                  "BlueMarble::leaveUser slot(%d)", seat);
        }
        else
        {
            sub_GameWorld_outFromBlueMarble(sub_G_GameWorld(), user);
            setLeaveUserSlot(seat);
            setLeaveUserBlueMarbleState();
            sendBlueMarbleRoomInfo();
        }
    }
}

int BlueMarble::setScript(BlueMarbleScriptManager* script)
{
    m_pScript = script;
}

int BlueMarble::setUserInfo(BlueMarbleScriptManager* script)
{
    BlueMarbleUserInfo info;
    info.setScript(script);
    m_users.push_back(info);
}

int BlueMarble::resetBlueMarble()
{
    m_field18 = 0;
    m_field1c = 0;
    m_set.clear();
    m_field3c = 0;
}

int BlueMarble::resetBuffInfo(int seat)
{
    m_users[seat].resetBuff();
}

int BlueMarble::resetIndexInfo(int seat)
{
    m_users[seat].resetIndex();
}

int BlueMarble::checkUserConditionEnterBlueMarble(CUser* user)
{
    if (user == 0)
    {
        return 0x17;
    }
    int count = 0;
    for (int i = 0; i < ((BM_ScriptFields*)m_pScript)->m_maxUser; ++i)
    {
        if (m_users[i].getUser() == user)
        {
            ++count;
        }
    }
    if (count > 0)
    {
        leaveUser(user);
        return 0x12;
    }
    if (user->get_state() == 3)
    {
        if (user->checkInBlueMarble())
        {
            return 0x17;
        }
        if (user->getMoveSpace() != (UserSpace::T)1)
        {
            int level = user->get_charac_level();
            if (level < ((BM_ScriptFields*)m_pScript)->m_minLevel ||
                level > ((BM_ScriptFields*)m_pScript)->m_maxLevel)
            {
                return 0x17;
            }
            if ((int)user->getBlueMarbleEnterCount() >=
                ((BM_ScriptFields*)m_pScript)->m_enterLimit)
            {
                return 0x17;
            }
            if (user->getCurCharacVill() == 7)
            {
                return 0x17;
            }
            if (sub_CPrivateStoreMgr_IsBusyPrivateStore(
                    sub_GetInstancePrivateStoreMgr(), user))
            {
                return 0x17;
            }
            if (user->IsBusyDisjointStore())
            {
                return 0x17;
            }
            if (user->IsCurCharacGhost())
            {
                return 0x17;
            }
            if (user->CheckInTrade())
            {
                return 0x17;
            }
            if (sub_CUser_isCompetitionMercenary(user))
            {
                return 0x17;
            }
            if (user->CheckInParty())
            {
                CParty* party = (CParty*)user->GetParty();
                if (party != 0)
                {
                    party->send_invite_bluemarble(user->get_unique_id() & 0xffff);
                    G_CGameManager()->CheckOutParty(user, false);
                }
            }
            return 0;
        }
        return 0x17;
    }
    return 0x17;
}

int BlueMarble::checkBlueMarbleUserConfirm(CUser* user)
{
    int userBlueMarbleIndex;
    UserSpace::T moveSpace;
    int userState;
    char local_428[1024] = {0};
    int i;
    if (user != 0)
    {
        sprintf(local_428, "./log/%s/BlueMarbleConfirmInfo",
                G_CEnvironment()->get_file_name());
        for (i = 0; i < ((BM_ScriptFields*)m_pScript)->m_maxUser; ++i)
        {
            if (m_users[i].getUser() == user)
            {
                userBlueMarbleIndex = (int)(short)m_users[i].getUser()->getBlueMarbleIndex();
                int memoryPoolIndex = getMemoryPoolIndex();
                moveSpace = m_users[i].getUser()->getMoveSpace();
                userState = m_users[i].getUser()->get_state();
                int characNo = m_users[i].getUser()->get_charac_no(-1);
                char* m_id = NumberToString(m_users[i].getUser()->get_acc_id(), 0);
                CMyFileLog log("void BlueMarble::checkBlueMarbleUserConfirm(CUser*)",
                               0x514);
                log(local_428,
                    "checkBlueMarbleUserConfirm() m_id(%s) charac_no(%u)userState(%d) userSpace(%d) blueMarble_Index(%d) user_bluemarble_index(%d) slot_index(%d)",
                    m_id, characNo, userState, moveSpace, memoryPoolIndex,
                    userBlueMarbleIndex, i);
            }
        }
    }
}

int BlueMarble::enterBlueMarble(CUser* user)
{
    int result = 0;
    if (user == 0)
    {
        return 0x17;
    }
    if (!checkJoin())
    {
        return 0x17;
    }
    result = checkUserConditionEnterBlueMarble(user);
    if (result > 0)
    {
        return result;
    }
    result = join(user);
    if (result > 0)
    {
        return result;
    }
    if (getWaitUserCount() == ((BM_ScriptFields*)m_pScript)->m_maxUser)
    {
        changeState((BlueMarbleState::T)1);
    }
    return 0;
}

int BlueMarble::checkStartBlueMarble()
{
    if (getState() != 1)
    {
        return 0;
    }
    if (getWaitUserCount() < ((BM_ScriptFields*)m_pScript)->m_maxUser)
    {
        changeState(BlueMarbleState::T_0);
        return 0;
    }
    return 1;
}

int BlueMarble::startBlueMarble()
{
    if (checkStartBlueMarble())
    {
        resetBlueMarble();
        changeState((BlueMarbleState::T)2);
        setBlueMarbleMap();
        setMaxBoardSize();
        for (int i = 0; i < ((BM_ScriptFields*)m_pScript)->m_maxUser; ++i)
        {
            if (m_users[i].getUser() != 0)
            {
                m_users[i].setUserState((BlueMarbleUserState::T)1);
                m_users[i].getUser()->increaseBlueMarbleEnterCount();
                m_users[i].getUser()->sendBlueMarbleEnterCount();
            }
        }
        sendStartBlueMarble();
    }
}

int BlueMarble::checkThrowDice(int seat)
{
    bool b;
    if (m_users[seat].getBoardZone() < getMaxBoardSize() &&
        m_users[seat].getUserState() == (BlueMarbleUserState::T)1 &&
        getState() == 2)
    {
        b = false;
    }
    else
    {
        b = true;
    }
    return !b;
}

int BlueMarble::throwDice(int seat)
{
    setDiceNumber();
    sendBlueMarbleDiceInfo(seat);
}

int BlueMarble::userThrowDice(CUser* user)
{
    if (user == 0) goto done;
    {
        int seat = getUserSlot(user);
        if (seat == -1)
        {
            LogManager::logFormat(1, "BlueMarble.cpp",
                                  "void BlueMarble::userThrowDice(CUser*)", 0x446,
                                  "BlueMarble::userThrowDice slot(%d)", seat);
        }
        else if (checkThrowDice(seat))
        {
            resetIndexInfo(seat);
            throwDice(seat);
            setMovePosition(seat, getDiceNumber());
            intoBoardZone(seat);
            sendBoardZoneActionInfo(seat, getDungeonIndex(seat),
                                    (unsigned int)getItemIndex(seat));
        }
    }
done:
    ;
}

int BlueMarble::movePosition(CUser* user, int pos)
{
    int seat = getUserSlot(user);
    if (seat == -1)
    {
        LogManager::logFormat(1, "BlueMarble.cpp",
                              "void BlueMarble::movePosition(CUser*, int)", 0x4f4,
                              "BlueMarble::movePosition slot(%d)", seat);
    }
    else if (checkThrowDice(seat))
    {
        PacketGuard packet;
        packet.put_header(0, 0x1af);
        packet.put_byte(seat);
        packet.put_byte(pos - getMovePosition(seat));
        packet.finalize(true);
        sendToBlueMarble(packet);
        resetIndexInfo(seat);
        setMovePosition(seat, pos - getMovePosition(seat));
        intoBoardZone(seat);
        sendBoardZoneActionInfo(seat, getDungeonIndex(seat),
                                (unsigned int)getItemIndex(seat));
    }
}

int BlueMarble::checkMoveFirstZone(int seat, BlueMarbleTileScript::Type type)
{
    switch (type)
    {
    case (BlueMarbleTileScript::Type)11:
    case (BlueMarbleTileScript::Type)12:
    case (BlueMarbleTileScript::Type)13:
        return 0;
    case (BlueMarbleTileScript::Type)9:
    case (BlueMarbleTileScript::Type)10:
        if (getMovePosition(seat) != 1)
        {
            return 0;
        }
        break;
    default:
        break;
    }
    return 1;
}

int BlueMarble::tileType(int seat)
{
    int zone = m_users[seat].getBoardZone();
    BlueMarbleMapScript* map = m_pScript->getMap(getBlueMarbleMap());
    if (map != 0)
    {
        int* tile = (int*)m_pScript->getMap(getBlueMarbleMap())->getTile(
            (unsigned int)zone);
        if (tile != 0)
        {
            switch (tile[0])
            {
            case 2:
            case 8:
                setDungeonIndex(seat,
                                getRandomDungeon(seat, (BlueMarbleTileScript::Type)2));
                break;
            case 3:
                setGoldBuff(seat, tile[2]);
                break;
            case 4:
                setExpBuff(seat, tile[2]);
                break;
            case 5:
                setDropBuff(seat, tile[2]);
                break;
            case 6:
                setItemIndex(seat, getRandomItem());
                break;
            case 7:
                if (!getVisitZone(zone))
                {
                    setDungeonIndex(seat,
                                    getRandomDungeon(seat,
                                                     (BlueMarbleTileScript::Type)7));
                }
                break;
            case 9:
                setVisitZone(zone);
                setMovePosition(seat, tile[2]);
                if (checkMoveFirstZone(seat, (BlueMarbleTileScript::Type)9))
                {
                    resetBuffInfo(seat);
                }
                intoBoardZone(seat);
                break;
            case 10:
                setVisitZone(zone);
                setMovePosition(seat, tile[2] - getMovePosition(seat));
                if (checkMoveFirstZone(seat, (BlueMarbleTileScript::Type)10))
                {
                    resetBuffInfo(seat);
                }
                intoBoardZone(seat);
                break;
            case 11:
            case 12:
            case 13:
                setDungeonIndex(seat, tile[2]);
                break;
            default:
                break;
            }
        }
    }
}

int BlueMarble::intoBoardZone(int zone)
{
    setDungeonIndex(zone, getRandomDungeon(zone, (BlueMarbleTileScript::Type)14));
    tileType(zone);
    setVisitZone(m_users[zone].getBoardZone());
}

int BlueMarble::checkLastZoneDungeonClear(int seat)
{
    if (getState() == 2)
    {
        if (m_users[seat].getBoardZone() == getMaxBoardSize())
        {
            return 1;
        }
    }
    return 0;
}

int BlueMarble::enterDungeon(CUser* user)
{
    if (user == 0) goto done;
    {
        int seat = getUserSlot(user);
        if (seat == -1)
        {
            LogManager::logFormat(1, "BlueMarble.cpp",
                                  "void BlueMarble::enterDungeon(CUser*)", 0x45c,
                                  "BlueMarble::enterDungeon slot(%d)", seat);
        }
        else
        {
            setUserState(seat, (BlueMarbleUserState::T)2);
        }
    }
done:
    ;
}

int BlueMarble::exitDungeon(CUser* user)
{
    if (user == 0) goto done;
    {
        int seat = getUserSlot(user);
        if (seat == -1)
        {
            LogManager::logFormat(1, "BlueMarble.cpp",
                                  "void BlueMarble::exitDungeon(CUser*)", 0x49d,
                                  "BlueMarble::exitDungeon slot(%d)", seat);
        }
        else
        {
            setUserState(seat, (BlueMarbleUserState::T)1);
        }
    }
done:
    ;
}

int BlueMarble::clearDungeon(CUser* user)
{
    if (user == 0) goto done;
    {
        int seat = getUserSlot(user);
        if (seat == -1)
        {
            LogManager::logFormat(1, "BlueMarble.cpp",
                                  "void BlueMarble::clearDungeon(CUser*)", 0x48a,
                                  "BlueMarble::clearDungeon slot(%d)", seat);
        }
        else if (checkLastZoneDungeonClear(seat))
        {
            finishBlueMarble(seat);
            resultReward();
        }
    }
done:
    ;
}

int BlueMarble::giveupDungeon(CUser* user)
{
    if (user == 0) goto done;
    {
        int seat = getUserSlot(user);
        if (seat == -1)
        {
            LogManager::logFormat(1, "BlueMarble.cpp",
                                  "void BlueMarble::giveupDungeon(CUser*)", 0x46b,
                                  "BlueMarble::giveupDungeon slot(%d)", seat);
        }
        else
        {
            BlueMarbleMapScript* map = m_pScript->getMap(getBlueMarbleMap());
            if (map != 0)
            {
                int* tile = (int*)m_pScript->getMap(getBlueMarbleMap())->getTile(
                    (unsigned int)m_users[seat].getBoardZone());
                if (tile != 0)
                {
                    if (checkMoveFirstZone(seat,
                                           (BlueMarbleTileScript::Type)tile[0]))
                    {
                        setMovePosition(seat, 1 - getMovePosition(seat));
                        resetBuffInfo(seat);
                        sendTileGiveUpInfo(seat);
                    }
                    setUserState(seat, (BlueMarbleUserState::T)1);
                }
            }
        }
    }
done:
    ;
}

int BlueMarble::giveBonusItem(CUser* user)
{
    if (user == 0) goto done;
    {
        int seat = getUserSlot(user);
        if (seat == -1)
        {
            LogManager::logFormat(1, "BlueMarble.cpp",
                                  "void BlueMarble::giveBonusItem(CUser*)", 0x4ac,
                                  "BlueMarble::giveBonusItem slot(%d)", seat);
        }
        else if (getState() == 2 && getUserState(seat) == 1)
        {
            Inven_Item item;
            item.m_addInfo = getItemIndex(seat);
            item.m_addInfo2 = 1;
            if (item.m_addInfo != 0)
            {
                insertItem(seat, item, (eItemAddReason)0x41);
                resetIndexInfo(seat);
            }
        }
    }
done:
    ;
}

int BlueMarble::resultReward()
{
    Inven_Item item;
    for (int i = 0; i < ((BM_ScriptFields*)m_pScript)->m_maxUser; ++i)
    {
        if (m_users[i].getUser() != 0)
        {
            item.reset();
            setResultItem(i, item);
            if (item.m_addInfo != 0)
            {
                insertItem(i, item, (eItemAddReason)0x42);
            }
        }
    }
}

int BlueMarble::finishBlueMarble(int seat)
{
    setUserGrade(seat);
    changeState((BlueMarbleState::T)3);
    sendFinishBlueMarble();
}

int BlueMarble::setResultItem(int seat, Inven_Item& item)
{
    int grade = m_users[seat].getGrade();
    std::vector<BlueMarbleRewardInfo>* rewards =
        (std::vector<BlueMarbleRewardInfo>*)((char*)m_pScript + 0xa4);
    if (grade == 1)
    {
        item.m_addInfo = (*rewards)[1].m_a;
        item.m_addInfo2 = (*rewards)[1].m_b;
    }
    else
    {
        if (grade > 1)
        {
            if (grade == 2)
            {
                item.m_addInfo = (*rewards)[2].m_a;
                item.m_addInfo2 = (*rewards)[2].m_b;
            }
            else if (grade == 3)
            {
                item.m_addInfo = (*rewards)[3].m_a;
                item.m_addInfo2 = (*rewards)[3].m_b;
            }
        }
        else if (grade == 0)
        {
            item.m_addInfo = (*rewards)[0].m_a;
            item.m_addInfo2 = (*rewards)[0].m_b;
        }
    }
}

int BlueMarble::insertItem(int seat, Inven_Item& item, eItemAddReason reason)
{
    int addInfo = *(int*)((char*)&item + 7);
    G_CDataManager()->m_itemList->create_item(ENUM_ITEM_CREATE_TYPE_0, item,
                                              addInfo);
    CInventory* inven =
        (CInventory*)m_users[seat].getUser()->getCurCharacInvenW();
    int ret = inven->insertItemIntoInventory(item, reason, true, false);
    if (ret < 0)
    {
        char local_2e[30];
        char local_12e[256];
        char* msg268 = sub_RDARScriptStringManager_findString(
            (void*)0x0949b140, 4, "game_server_msg_268", 0);
        strncpy(local_2e, msg268, 0x1d);
        char* msg269 = sub_RDARScriptStringManager_findString(
            (void*)0x0949b140, 4, "game_server_msg_269", 0);
        strncpy(local_12e, msg269, 0xff);
        int serverGroup = (int)m_users[seat].getUser()->GetServerGroup();
        size_t len = strlen(local_12e);
        int characNo = m_users[seat].getUser()->get_charac_no(-1);
        int mailRet = sub_CMailBoxHelper_ReqDBSendNewSystemMail(
            local_2e, &item, 0, (unsigned)characNo, local_12e, (int)len, 0xf,
            (int)(intptr_t)serverGroup, false, false);
        if (mailRet == 0)
        {
            char local_273[0x145];
            memset(local_273, 0, 0x145);
            strncpy(local_273 + 0x11, local_2e, 0x1d);
            *(int*)(local_273 + 0x39) = *(int*)((char*)&item + 2);
            *(int*)(local_273 + 0x3d) = *(int*)((char*)&item + 7);
            sub_cUserHistoryLog_SendMail((char*)m_users[seat].getUser() + 0x79700,
                                         local_273, 0);
        }
    }
    else
    {
        sendUpdateItemInfo(seat, ret);
    }
}

int BlueMarble::setLeaveUserBlueMarbleState()
{
    if (getWaitUserCount() == 0)
    {
        for (int i = 0; i < ((BM_ScriptFields*)m_pScript)->m_maxUser; ++i)
        {
            m_users[i].resetBlueMarbleUserInfo();
        }
        changeState(BlueMarbleState::T_0);
        resetBlueMarble();
    }
    else
    {
        if (getState() == 1)
        {
            changeState(BlueMarbleState::T_0);
        }
    }
}

int BlueMarble::setLeaveUserSlot(int seat)
{
    if (m_users[seat].getUser() == 0)
    {
        m_users[seat].resetBlueMarbleUserInfo();
        LogManager::logFormat(1, "BlueMarble.cpp",
                              "void BlueMarble::setLeaveUserSlot(int)", 0x2c4,
                              "BlueMarble::setLeaveUserSlot slot(%d)", seat);
    }
    else
    {
        m_users[seat].getUser()->setBlueMarbleIndex(-1);
        m_users[seat].getUser()->setMoveSpace((UserSpace::T)0);
        m_users[seat].getUser()->resetBlueMarbleUserDeadCount();
        m_users[seat].resetBlueMarbleUserInfo();
    }
}

int BlueMarble::sendBlueMarbleDiceInfo(int seat) const
{
    PacketGuard packet;
    packet.put_header(0, 0x1af);
    packet.put_byte(seat);
    packet.put_byte((int)(char)getDiceNumber());
    packet.finalize(true);
    sendToBlueMarble(packet);
}

int BlueMarble::sendBlueMarbleRoomInfo() const
{
    PacketGuard packet;
    packet.put_header(0, 0x1ae);
    packet.put_short(getMemoryPoolIndex());
    packet.put_byte(getWaitUserCount());
    for (int i = 0; i < ((BM_ScriptFields*)m_pScript)->m_maxUser; ++i)
    {
        if (m_users[i].getUser() != 0)
        {
            packet.put_byte(i);
            packet.put_short(m_users[i].getUser()->get_unique_id() & 0xffff);
        }
    }
    packet.finalize(true);
    sendToBlueMarble(packet);
}

int BlueMarble::sendBoardZoneActionInfo(int seat, int zone,
                                        unsigned int pos) const
{
    PacketGuard packet;
    packet.put_header(0, 0x1b2);
    packet.put_short(zone);
    packet.put_int((int)pos);
    packet.finalize(true);
    m_users[seat].getUser()->Send(packet);
}

int BlueMarble::sendFinishBlueMarble() const
{
    int winner = getWinner();
    if (winner != -1)
    {
        PacketGuard packet;
        packet.put_header(0, 0x1b1);
        packet.put_byte(winner);
        packet.finalize(true);
        sendToBlueMarble(packet);
    }
}

int BlueMarble::sendStartBlueMarble() const
{
    PacketGuard packet;
    packet.put_header(0, 0x1b0);
    packet.put_byte((int)(char)getBlueMarbleMap());
    packet.finalize(true);
    sendToBlueMarble(packet);
}

int BlueMarble::sendTileGiveUpInfo(int seat) const
{
    PacketGuard packet;
    packet.put_header(0, 0x1b3);
    packet.put_byte(seat);
    packet.finalize(true);
    sendToBlueMarble(packet);
}

int BlueMarble::sendUpdateItemInfo(int seat, int idx) const
{
    PacketGuard packet;
    packet.put_header(0, 0xe);
    packet.put_byte(0);
    packet.put_short(1);
    CInventory* inven =
        (CInventory*)m_users[seat].getUser()->getCurCharacInvenR();
    inven->MakeItemPacket(1, idx, packet);
    packet.finalize(true);
    m_users[seat].getUser()->Send(packet);
}

void BlueMarble::sendToBlueMarble(PacketGuard& guard) const
{
    for (int i = 0; i < ((BM_ScriptFields*)m_pScript)->m_maxUser; ++i)
    {
        if (m_users[i].getUser() != 0)
        {
            m_users[i].getUser()->Send(guard);
        }
    }
}
