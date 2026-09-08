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
#include "CInventory.h"          // 权威 CInventory（原本地最小声明与 CInventory.h:418 重定义冲突）
#include "CEnvironment.h"
#include "LogManager.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "PacketGuard.h"   // 权威 PacketGuard（0xc）
#include "GameWorld.h"            // G_GameWorld / GameWorld::gotoBlueMarble/outFromBlueMarble（§9）
#include "CPrivateStoreMgr.h"     // private_store::GetInstancePrivateStoreMgr/IsBusyPrivateStore（§9）
#include "RDARScriptStringManager.h"  // g_scriptStringManager_/findString（§9）
#include "CGameManager.h"         // WongWork::CMailBoxHelper::ReqDBSendNewSystemMail + G_CGameManager（§9）
#include "PacketTypes.h"          // 权威 MSG_MAILBOX_SEND（+0x11/+0x39/+0x3d 具名字段）

// 2026-09-05 裸偏移收尾：原本地 shadow struct BM_ScriptFields（把 m_pScript 当
// 未建模字节区直读 +0x30..+0x4c）删除 —— 该区间即权威
// BlueMarbleScriptManager::m_info（+0x30，BlueMarbleInfoScript），逐槽改用
// m_pScript->m_info.<字段>（证据：getRandomItem 0x080d7690 `mov 0x4c(%eax)`、
// setDiceNumber 0x080d6f88 `mov 0x48/0x44(%eax)`、checkUserConditionEnterBlueMarble
// 0x080d8d02 `mov 0x30/0x34/0x38/0x3c(%eax)`，均 4 字节读）；private 槽位经
// BlueMarbleInfoScript 的 `friend class BlueMarble` 授权（ORIG 无 getter 符号）。

// ---- CGameManager / G_CGameManager：经 #include "CGameManager.h"（§9） ----

// ============================================================================
// BlueMarble 实现
// ============================================================================

// b28c-1 N: P75 实参装载序（ORIG 先载 seedValue 再算 m_pRands[i]，4.4 恒 receiver 先）。
BlueMarble::BlueMarble()
    : m_memoryPoolIndex(-1), m_state(BlueMarbleState::T_0), m_diceNumber(0), m_blueMarbleMap(0),
      m_pScript(0), m_maxBoardSize(0)
{
    m_pRands = new CMTRand[4];
    unsigned long seedValue = (unsigned long)time((time_t*)0);
    for (int i = 0; i <= 3; ++i)
    {
        // b28c-1：ORIG 0x080d8a5c 先载 seedValue 再算 m_pRands[i]（P75 实参
        //  装载序跨版本差；静态调用形不可表达），保持自然成员调用形。
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
    return m_state;
}

void BlueMarble::changeState(BlueMarbleState::T state)
// 原 int；ORIG 0x080d9202 尾 pop;ret 无 %eax 置值 ⇒ void（mangled 无返回类型）
{
    m_state = state;
}

int BlueMarble::getWinner() const
{
    for (int i = 0; i < m_pScript->m_info.m_playerMax; ++i)
    {
        if (m_users[i].getUser() == 0)
        {
            continue;
        }
        if (m_users[i].getGrade() == 0)
        {
            return i;
        }
    }
    return -1;
}

int BlueMarble::getWaitUserCount() const
{
    int count = 0;
    for (int i = 0; i < m_pScript->m_info.m_playerMax; ++i)
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
    for (int i = 0; i < m_pScript->m_info.m_playerMax; ++i)
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

// b28c-1 N: 清绑（P0/N96）后余 5 条 = 4.4 未内联 std::move（ORIG 4.1 内联）；
//  去 move 则指令序列与 ORIG 全等但块内 zone/pair 槽方向差 14 行（N93/N82附）。
int BlueMarble::setUserGrade(int grade)
{
    std::multimap<int, int, std::greater<int> > mm;
    m_users[grade].setGrade(BlueMarbleUserGrade::T_0);
    for (int i = 0; i < m_pScript->m_info.m_playerMax; ++i)
    {
        // ORIG 0x080d7b5d/0x080d7b87：==0 物化 sete %al 后 test;jne 出口（body fall，
        // 范式23 goto）。b28c-1：P0/N96 清绑，P168a② 具名 bool + 正向 goto；
        // 声明前置（P198① 两句式）避免 goto 跨初始化。
        if (m_users[i].getUser() == 0)
            goto NEXT0;
        if (m_users[i].getGrade() == 0)
            goto NEXT0;
        {
            // ORIG 0x080d7bb0..：zone 槽 -0x1c；make_pair 经 move（4.1 内联）。
            int zone = m_users[i].getBoardZone();
            mm.insert(std::make_pair(std::move(zone), i));
        }
    NEXT0:
        ;
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
    return m_maxBoardSize;
}

// b28c-1 AE: P223 尾标签后 0 差。
int BlueMarble::setMaxBoardSize()
{
    BlueMarbleMapScript* map = m_pScript->getMap(getBlueMarbleMap());
    // ORIG 0x080d6e88：以 getMap() 返回值为 this 直调 vector::size（m_tiles 在
    // BlueMarbleMapScript +0x00，地址等价）⇒ 具名 map->m_tiles.size()。
    // b28c-1：P223 空块 nop 中继岛（ORIG je 跳过 body 落 nop、body 尾 jmp 汇合）
    //   ⇒ goto 尾空标签形，等价语义。
    if (map == 0) goto done;
    m_maxBoardSize = (int)map->m_tiles.size();
done:;
}

int BlueMarble::getDiceNumber() const
{
    return m_diceNumber;
}

int BlueMarble::setDiceNumber()
{
    m_diceNumber = (unsigned int)m_pRands[0].randInt() %
                    (unsigned int)m_pScript->m_info.m_diceMax +
                m_pScript->m_info.m_diceMin;
}

int BlueMarble::getBlueMarbleMap() const
{
    return m_blueMarbleMap;
}

int BlueMarble::setBlueMarbleMap()
{
    m_blueMarbleMap = (unsigned int)m_pRands[1].randInt() %
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

bool BlueMarble::getVisitZone(int seat) const
{
    std::set<int>::const_iterator it = m_set.find(seat);
    if (it == m_set.end())
    {
        return 0;
    }
    return 1;
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

// b28c-1 N: P209 4.4 每次重载形参槽 vs ORIG 载入一次复用（switch/裸形均劣化）。
int BlueMarble::getBuff(CUser* user, BlueMarbleBuffType::T type)
{
    if (user == 0)
    {
        return 0;
    }
    int seat = getUserSlot(user);
    if (seat == -1)
    {
        LogManager::logFormat(1, "BlueMarble.cpp", __PRETTY_FUNCTION__,
                              0x4c7, "BlueMarble::getBuff slot(%d)", seat);
        return 0;
    }
    // 语义修正 + 块序：ORIG 0x080d997c `cmp$1; je→getGoldBuff@999e`、
    // 0x080d9984 `cmp$2; je→getDropBuff@99b2`、0x080d9988 `test; jne→mov$0@99c6`、
    // fall type==0→getExpBuff@9997（case 对应：0→exp、1→gold、2→drop；
    // 范式23 goto 钉 je-岛序，exp body 为 fall-through 首块）。
    // b28c-1：ORIG 0x080d997c `mov 0x10(%ebp),%eax; cmp $1,%eax` 载入复用
    //  ⇒ P209 unsigned 比较恒载寄存器形（switch 形实测劣化 5）。
    if ((unsigned int)type == 1)
        goto L_GOLD;
    if ((unsigned int)type == 2)
        goto L_DROP;
    if ((unsigned int)type != 0)
        goto L_DEF;
    return getExpBuff(seat);
L_GOLD:
    return getGoldBuff(seat);
L_DROP:
    return getDropBuff(seat);
L_DEF:
    return 0;
}

// b28c-1 AE: P218 下标 int 化（unsigned i→int i）后 0 差。
int BlueMarble::getRandomItem() const
{
    unsigned long range = (unsigned long)m_pScript->m_info.m_itemWeightTotal;
    int r = m_pRands[3].randInt(range);
    // P0：ORIG 无 items 局部槽——每次使用重算 this（0x80d87xx `mov 0x8,%edx;
    // mov 0x38,%edx; add $0x98,%edx`，循环内两处）。
    int count = (int)m_pScript->m_info.m_randomItemList.size();
    int acc = 0;
    // b28c-1：P218 下标 int 声明触发 RHS 值先（ORIG 0x080d7690 先载 i 再算
    //  this 链；unsigned i 恒 this 先）。
    int i = 0;
    while (i < count)
    {
        acc += m_pScript->m_info.m_randomItemList[i].m_weight;
        if (acc >= r)
        {
            return m_pScript->m_info.m_randomItemList[i].m_itemIndex;
        }
        ++i;
    }
    return 0;
}

int BlueMarble::getRandomDungeon(int seat, BlueMarbleTileScript::Type type) const
{
    std::vector<int> local;
    int level = m_users[seat].getUser()->get_charac_level();
    // ORIG 0x080d741d-0x080d748a：getBlueMarbleMap()+getMap 链在判空与
    // getTile 两处**各取一次**（无 map 局部缓存，P160/P172）；判空为
    // 4.1 sete 双物化（test;je 直连不可复现，残差记录）。
    if (m_pScript->getMap(getBlueMarbleMap()) == 0)
    {
        return 0;
    }
    BlueMarbleTileScript* tile = m_pScript->getMap(getBlueMarbleMap())->getTile(
        (unsigned int)m_users[seat].getBoardZone());
    if (tile == 0)
    {
        return 0;
    }
    if (tile->m_char4 != 1)   // ORIG 0x080d74a5 `movzbl 0x4(%eax),%eax`（char 读）
    {
        return 0;
    }
    // ORIG 0x080d74ba-：type 三分支各自 sret 临时槽（-0x38/-0x2c/-0x20）
    // 直写 `local = getXXXIndex(level)`（每分支一对 op=+D1），无跨分支
    // 共享 tmp——旧 tmp 块形与 move 中转已废。
    if (type == (BlueMarbleTileScript::Type)7)
    {
        local = m_pScript->m_info.getUniqueDungeonIndex(level);
    }
    else if (type == (BlueMarbleTileScript::Type)8 ||
             type == (BlueMarbleTileScript::Type)2)
    {
        local = m_pScript->m_info.getBossDungeonIndex(level);
    }
    else
    {
        local = m_pScript->m_info.getRandomDungeonIndex(level);
    }
    if (local.size() == 0)
    {
        return 0;
    }
    // 保持具名形：inline 直调实测反增 1（4.4 在取模表达式中仍需暂存槽）。
    int r = m_pRands[2].randInt();
    unsigned int idx = (unsigned int)r % (unsigned int)local.size();
    if (idx >= local.size())
    {
        return 0;
    }
    return local[idx];
}

bool BlueMarble::checkJoin()
{
    if (getState() != 0)
    {
        return 0;
    }
    if (getWaitUserCount() >= m_pScript->m_info.m_playerMax)
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
    for (int i = 0; i < m_pScript->m_info.m_playerMax; ++i)
    {
        if (m_users[i].getUser() == 0)
        {
            m_users[i].setUser(user);
            m_users[i].getUser()->setBlueMarbleIndex((short)getMemoryPoolIndex());
            m_users[i].getUser()->setMoveSpace((UserSpace::T)1);
            break;
        }
    }
    G_GameWorld()->gotoBlueMarble(user);
    sendBlueMarbleRoomInfo();
    return 0;
}

void BlueMarble::leaveUser(CUser* user)
{
    if (user == 0)
    {
        LogManager::logFormat(1, "BlueMarble.cpp", __PRETTY_FUNCTION__, 0x3f9,
                              "BlueMarble::leaveUser user is NULL");
    }
    else
    {
        int seat = getUserSlot(user);
        if (seat == -1)
        {
            LogManager::logFormat(1, "BlueMarble.cpp", __PRETTY_FUNCTION__, 0x400,
                                  "BlueMarble::leaveUser slot(%d)", seat);
        }
        else
        {
            G_GameWorld()->outFromBlueMarble(user);
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
    m_diceNumber = 0;
    m_blueMarbleMap = 0;
    m_set.clear();
    m_maxBoardSize = 0;
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
    for (int i = 0; i < m_pScript->m_info.m_playerMax; ++i)
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
    // ORIG +0x95 `cmp $3; setne %al; test; je <体>` / +0xcd `cmp $1; sete %al;
    // test; je <体>`：guard 物化形（P194 反形），真→fall 0x17 岛，假→je 入体。
    // b28c-1：P0/N96 清绑（P168a② 具名 bool）。
    if (user->get_state() != 3)
    {
        return 0x17;
    }
    if (user->checkInBlueMarble())
    {
        return 0x17;
    }
    // ORIG +0xcd `cmp $1; sete %al; test; je <体>`：==1 物化，je 入体、
    // fall 0x17 岛——goto 形唯一复现（P194）。b28c-1：P0/N96 清绑。
    if (user->getMoveSpace() == (UserSpace::T)1)
        goto L_MOVE_OK;
    return 0x17;
L_MOVE_OK:
    // P48 去缓存：ORIG +0x0e7/+0x0ff 对上下界各独立重调 get_charac_level
    //（两次调用，不缓存进 level 局部）；第二条件 call 在前（P134）。
    if (user->get_charac_level() < m_pScript->m_info.m_enterLevel ||
        user->get_charac_level() > m_pScript->m_info.m_enterMaxLevel)
    {
        return 0x17;
    }
    if ((int)user->getBlueMarbleEnterCount() >=
        m_pScript->m_info.m_enterCount)
    {
        return 0x17;
    }
    if (user->getCurCharacVill() == 7)
    {
        return 0x17;
    }
    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
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
    if (user->isCompetitionMercenary())
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

void BlueMarble::checkBlueMarbleUserConfirm(CUser* user)
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
        for (i = 0; i < m_pScript->m_info.m_playerMax; ++i)
        {
            if (m_users[i].getUser() == user)
            {
                userBlueMarbleIndex = (int)(short)m_users[i].getUser()->getBlueMarbleIndex();
                int memoryPoolIndex = getMemoryPoolIndex();
                moveSpace = m_users[i].getUser()->getMoveSpace();
                userState = m_users[i].getUser()->get_state();
                int characNo = m_users[i].getUser()->get_charac_no(-1);
                char* m_id = NumberToString(m_users[i].getUser()->get_acc_id(), 0);
                CMyFileLog log(__PRETTY_FUNCTION__,
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
    if (getWaitUserCount() == m_pScript->m_info.m_playerMax)
    {
        changeState((BlueMarbleState::T)1);
    }
    return 0;
}

bool BlueMarble::checkStartBlueMarble()
{
    if (getState() != 1)
    {
        return 0;
    }
    if (getWaitUserCount() < m_pScript->m_info.m_playerMax)
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
        for (int i = 0; i < m_pScript->m_info.m_playerMax; ++i)
        {
            if (m_users[i].getUser() == 0)
            {
                continue;
            }
            m_users[i].setUserState((BlueMarbleUserState::T)1);
            m_users[i].getUser()->increaseBlueMarbleEnterCount();
            m_users[i].getUser()->sendBlueMarbleEnterCount();
        }
        sendStartBlueMarble();
    }
}

// b28c-1 N: 第三条件 je/jne 极性（4.4 块序折叠；三目/表达式/具名 bool 三形实测均不可翻）。
bool BlueMarble::checkThrowDice(int seat)
{
    // ORIG 0x080d6ed8：bVar1 物化全程 %eax（0x080d6f3f/6f46 mov$1/$0,%eax），
    // return !bVar1 走 `test al; je; mov$0/$1` 双岛（0x80d6f4b..）。
    // b28c-1：P0/N96 清绑；P56 if-return 分支形（ORIG `test al; je; mov$0/$1`）。
    // b28c-1：P56 if-return 分支形（三目/表达式形实测劣化）；ORIG 尾
    //  `test al; je; mov$0/$1` 双岛仅第三条件 je/jne 极性差（N：4.4 块序折叠）。
    if (m_users[seat].getBoardZone() < getMaxBoardSize() &&
        m_users[seat].getUserState() == (BlueMarbleUserState::T)1 &&
        getState() == 2)
    {
        return false;
    }
    return true;
}

int BlueMarble::throwDice(int seat)
{
    setDiceNumber();
    sendBlueMarbleDiceInfo(seat);
}

void BlueMarble::userThrowDice(CUser* user)
{
    if (user == 0) goto done;
    {
        int seat = getUserSlot(user);
        if (seat == -1)
        {
            LogManager::logFormat(1, "BlueMarble.cpp", __PRETTY_FUNCTION__, 0x446,
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

// b28c-1 N: ORIG 0x080d9c08 EH cleanup 尾 jmp 岛（4.4 析构内联无岛）。
void BlueMarble::movePosition(CUser* user, int pos)
{
    int seat = getUserSlot(user);
    if (seat == -1)
    {
        LogManager::logFormat(1, "BlueMarble.cpp", __PRETTY_FUNCTION__, 0x4f4,
                              "BlueMarble::movePosition slot(%d)", seat);
    }
    // ORIG 0x80d9ab9 xor $0x1,%eax; test; jne 出口（=false 走体）——用已验证的
    // `(int)!boolcall()` 物化形（handleStageCommand 0x84661a3 同构）。
    else if ((int)!checkThrowDice(seat))
    {
        ;
    }
    else
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
        // b28c-1：P223 尾岛（ORIG 0x080d9c08 jmp+nop 汇合形）。
        goto DONE;
    }
DONE:
    ;
}

bool BlueMarble::checkMoveFirstZone(int seat, BlueMarbleTileScript::Type type)
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

// b28c-1 AE: P0/N96 清绑 + P223 双岛（0x080d737c/0x080d737f）后 0 差。
int BlueMarble::tileType(int seat)
{
    int zone = m_users[seat].getBoardZone();
    // P0+范式22：ORIG 0x080d70f2 `test;sete %al` 物化 + `test al; jne` 出口
    //（body fall）。zone 槽 -0x10（0x080d70d2）。
    // b28c-1：P0/N96 清绑（P168a② 具名 bool + 正向 goto）。
    // ORIG 0x080d70f7：test al; jne 出口 —— 正向 goto（范式23）阻 4.4 xor 取反。
    if (m_pScript->getMap(getBlueMarbleMap()) == 0)
        goto T_OUT;
    {
        BlueMarbleTileScript* tile = m_pScript->getMap(getBlueMarbleMap())->getTile(
            (unsigned int)zone);
        // b28c-1：ORIG 双岛（0x080d737c mapNull 岛 / 0x080d737f tile==0 岛）
        //   ⇒ P223 多岛形：第二个条件也用 goto 到相邻标签。
        if (tile == 0)
            goto T_OUT2;
        switch (tile->m_type)   // ORIG 0x080d713b `mov (%eax),%eax`（int 读）
        {
            case 2:
            case 8:
                setDungeonIndex(seat,
                                getRandomDungeon(seat, (BlueMarbleTileScript::Type)2));
                break;
            case 3:
                setGoldBuff(seat, tile->m_value);
                break;
            case 4:
                setExpBuff(seat, tile->m_value);
                break;
            case 5:
                setDropBuff(seat, tile->m_value);
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
                setMovePosition(seat, tile->m_value);
                if (checkMoveFirstZone(seat, (BlueMarbleTileScript::Type)9))
                {
                    resetBuffInfo(seat);
                }
                intoBoardZone(seat);
                break;
            case 10:
                setVisitZone(zone);
                setMovePosition(seat, tile->m_value - getMovePosition(seat));
                if (checkMoveFirstZone(seat, (BlueMarbleTileScript::Type)10))
                {
                    resetBuffInfo(seat);
                }
                intoBoardZone(seat);
                break;
            case 11:
            case 12:
            case 13:
                setDungeonIndex(seat, tile->m_value);
                break;
        default:
            break;
        }
    }
T_OUT2:
    ;
T_OUT:
    ;
}

int BlueMarble::intoBoardZone(int zone)
{
    setDungeonIndex(zone, getRandomDungeon(zone, (BlueMarbleTileScript::Type)14));
    tileType(zone);
    setVisitZone(m_users[zone].getBoardZone());
}

bool BlueMarble::checkLastZoneDungeonClear(int seat)
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

void BlueMarble::enterDungeon(CUser* user)
{
    if (user == 0) goto done;
    {
        int seat = getUserSlot(user);
        if (seat == -1)
        {
            LogManager::logFormat(1, "BlueMarble.cpp", __PRETTY_FUNCTION__, 0x45c,
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

void BlueMarble::exitDungeon(CUser* user)
{
    if (user == 0) goto done;
    {
        int seat = getUserSlot(user);
        if (seat == -1)
        {
            LogManager::logFormat(1, "BlueMarble.cpp", __PRETTY_FUNCTION__, 0x49d,
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

void BlueMarble::clearDungeon(CUser* user)
{
    if (user == 0) goto done;
    {
        int seat = getUserSlot(user);
        if (seat == -1)
        {
            LogManager::logFormat(1, "BlueMarble.cpp", __PRETTY_FUNCTION__, 0x48a,
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

// b28c-1 AE: P0/N96 清绑 + P223 三岛（0x080d96e7/ea/ed）后 0 差。
void BlueMarble::giveupDungeon(CUser* user)
{
    // b28c-1：P223 多岛形（ORIG 三岛 0x080d96e7/ea/ed：user==0、mapNull、tile==0
    //  各 goto 到相邻尾标签），P0/N96 清绑后以裸条件 + goto 复现。
    if (user == 0) goto DONE1;
    {
        int seat = getUserSlot(user);
        if (seat == -1)
        {
            LogManager::logFormat(1, "BlueMarble.cpp", __PRETTY_FUNCTION__, 0x46b,
                                  "BlueMarble::giveupDungeon slot(%d)", seat);
        }
        else
        {
            if (m_pScript->getMap(getBlueMarbleMap()) == 0)
                goto DONE2;
            {
                BlueMarbleTileScript* tile = m_pScript->getMap(getBlueMarbleMap())->getTile(
                    (unsigned int)m_users[seat].getBoardZone());
                if (tile == 0)
                    goto DONE3;
                {
                    if (checkMoveFirstZone(seat,
                                           (BlueMarbleTileScript::Type)tile->m_type))
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
DONE1:
    ;
DONE2:
    ;
DONE3:
    ;
}

// b28c-1 AE: P223 三岛 + P194 反形裸短路后 0 差。
void BlueMarble::giveBonusItem(CUser* user)
{
    // b28c-1：P223 多岛形（ORIG 三岛 0x080d9905/908/90b：user==0、state 条件假、
    //  m_addInfo==0 各落相邻尾标签）。
    if (user == 0) goto DONE1;
    {
        int seat = getUserSlot(user);
        if (seat == -1)
        {
            LogManager::logFormat(1, "BlueMarble.cpp", __PRETTY_FUNCTION__, 0x4ac,
                                  "BlueMarble::giveBonusItem slot(%d)", seat);
        }
        else
        {
            // ORIG 0x080d9876..9900：bad=(state!=2||userstate!=1) 物化 mov$1/$0 后
            // test al; jne 岛2（P194 X1 反形，裸短路形）。
            if (getState() != 2 || getUserState(seat) != 1)
                goto DONE2;
            Inven_Item item;
            item.m_addInfo = getItemIndex(seat);
            item.m_addInfo2 = 1;
            if (item.m_addInfo == 0)
                goto DONE3;
            {
                insertItem(seat, item, (eItemAddReason)0x41);
                resetIndexInfo(seat);
            }
            goto DONE3;
        }
    }
DONE1:
    ;
DONE2:
    ;
DONE3:
    ;
}

// b28c-1 N: ORIG 0x080d7e6c 死岛（nop+jmp）被 4.4 jump-thread 合并（4.1 goto 岛产物）。
int BlueMarble::resultReward()
{
    Inven_Item item;
    for (int i = 0; i < m_pScript->m_info.m_playerMax; ++i)
    {
        if (m_users[i].getUser() == 0)
        {
            // b28c-1：ORIG continue 目标 = 岛1（0x080d7e6c nop+jmp），P223 多岛形。
            goto NEXT0;
        }
        item.reset();
        setResultItem(i, item);
        if (item.m_addInfo != 0)
        {
            insertItem(i, item, (eItemAddReason)0x42);
            // ORIG 0x80d7e6a：insertItem 后显式 jmp 循环尾块@80d7e70（4.1 岛形，
            // 范式24 goto 落点岛阻 4.4 fall-through 合并）。
            goto NEXT1;
        }
        // b28c-1：P223 多岛形（ORIG 0x80d7e6c nop+jmp 岛 + 0x80d7e6f nop 岛）
        //  ⇒ 条件假分支同样 goto 到相邻标签。
        goto NEXT2;
    NEXT0:
        ;
    NEXT3:
        ;
    NEXT2:
        ;
    NEXT1:
        ;
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
    // ORIG 0x080d7eb5-：每处 `(*rewards)[g]` 均从 this+0x38+0xa4 重新算
    // 基址再 call vector::operator[]（4 组 5 条装载链，无 rewards 指针
    // 局部缓存——旧缓存形每位省 2 条即 we-short 主因，P172 去缓存）。
    int grade = m_users[seat].getGrade();
    if (grade == 1)
    {
        item.m_addInfo = m_pScript->m_info.m_rewardList[1].m_a;
        item.m_addInfo2 = m_pScript->m_info.m_rewardList[1].m_b;
    }
    else
    {
        if (grade > 1)
        {
            if (grade == 2)
            {
                item.m_addInfo = m_pScript->m_info.m_rewardList[2].m_a;
                item.m_addInfo2 = m_pScript->m_info.m_rewardList[2].m_b;
            }
            else if (grade == 3)
            {
                item.m_addInfo = m_pScript->m_info.m_rewardList[3].m_a;
                item.m_addInfo2 = m_pScript->m_info.m_rewardList[3].m_b;
            }
        }
        else if (grade == 0)
        {
            item.m_addInfo = m_pScript->m_info.m_rewardList[0].m_a;
            item.m_addInfo2 = m_pScript->m_info.m_rewardList[0].m_b;
        }
    }
}

int BlueMarble::insertItem(int seat, Inven_Item& item, eItemAddReason reason)
{
    // ORIG 0x080d800e：`mov 0x7(%eax),%ebx`（4 字节读）= Inven_Item::m_addInfo2。
    int addInfo = item.m_addInfo2;
    G_CDataManager()->m_itemList->create_item(ENUM_ITEM_CREATE_TYPE_0, item,
                                              addInfo);
    // ORIG 0x080d80xx：链式直调（getCurCharacInvenW 返回值 %edx 直接作
    // this 压栈，无 inven 局部槽——旧缓存形多 1 条 reload）。
    int ret = ((CInventory*)m_users[seat].getUser()->getCurCharacInvenW())
                  ->insertItemIntoInventory(item, reason, true, false);
    if (ret < 0)
    {
        char local_2e[30];
        char local_12e[256];
        const char* msg268 = g_scriptStringManager_.findString(
            4, "game_server_msg_268", 0);
        strncpy(local_2e, msg268, 0x1d);
        const char* msg269 = g_scriptStringManager_.findString(
            4, "game_server_msg_269", 0);
        strncpy(local_12e, msg269, 0xff);
        ENUM_SERVER_GROUP serverGroup = m_users[seat].getUser()->GetServerGroup();
        size_t len = strlen(local_12e);
        int characNo = m_users[seat].getUser()->get_charac_no(-1);
        int mailRet = WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            local_2e, item, 0, (unsigned)characNo, local_12e, (int)len, 0xf,
            serverGroup, false, false);
        if (mailRet == 0)
        {
            // ORIG 栈槽 local_273（0x145）= MSG_MAILBOX_SEND 载荷（权威布局
            // PacketTypes.h：m_name11@+0x11 / m_itemId39@+0x39 / m_extra3d@+0x3d；
            // 写指令 `mov %eax,-0x236/-0x232(%ebp)` 均 4 字节 ⇒ 字段宽一致）。
            // 保留 char 数组载体（ORIG 该槽未 4 字节对齐：-0x273(%ebp)），且
            // 不引入额外局部指针变量（O0 下会多占栈槽、改变指令数）：逐处
            // reinterpret_cast 直读；不新增对象（MSG_BASE ctor 会多出 call）。
            char local_273[0x145];
            memset(local_273, 0, 0x145);
            strncpy(reinterpret_cast<MSG_MAILBOX_SEND*>(local_273)->m_name11,
                    local_2e, 0x1d);                   // +0x11 收件人角色名
            reinterpret_cast<MSG_MAILBOX_SEND*>(local_273)->m_itemId39 =
                static_cast<unsigned long>(item.m_addInfo);   // +0x39 附件物品 id
            reinterpret_cast<MSG_MAILBOX_SEND*>(local_273)->m_extra3d =
                item.m_addInfo2;                       // +0x3d 附件 add_info
            m_users[seat].getUser()->m_historyLog.SendMail(
                reinterpret_cast<MSG_MAILBOX_SEND*>(local_273), 0);
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
        for (int i = 0; i < m_pScript->m_info.m_playerMax; ++i)
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

void BlueMarble::setLeaveUserSlot(int seat)
{
    if (m_users[seat].getUser() == 0)
    {
        m_users[seat].resetBlueMarbleUserInfo();
        LogManager::logFormat(1, "BlueMarble.cpp", __PRETTY_FUNCTION__, 0x2c4,
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
    for (int i = 0; i < m_pScript->m_info.m_playerMax; ++i)
    {
        if (m_users[i].getUser() == 0)
        {
            continue;
        }
        packet.put_byte(i);
        packet.put_short((unsigned short)m_users[i].getUser()->get_unique_id());
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

// b28c-1 AE: P223 尾空标签岛（0x080d87f7 jmp+nop）后 0 差。
int BlueMarble::sendFinishBlueMarble() const
{
    int winner = getWinner();
    // b28c-1：P223 尾空标签岛（ORIG 0x080d87f7 `jmp +0xa8; nop` 汇合形）
    //   ⇒ 条件反极性 goto + 内层复合块（避免 goto 跨 packet 初始化声明）。
    if (winner == -1) goto done;
    {
        PacketGuard packet;
        packet.put_header(0, 0x1b1);
        packet.put_byte(winner);
        packet.finalize(true);
        sendToBlueMarble(packet);
    }
done:;
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
    // ORIG 0x080d7exx：getCurCharacInvenR 结果免槽直入 this（%eax 跨 3 条 mov 保活，
    // P0 右值内联 + 范式37）。
    ((CInventory*)m_users[seat].getUser()->getCurCharacInvenR())
        ->MakeItemPacket(INVEN_TYPE_EQUIP, idx, packet);
    packet.finalize(true);
    m_users[seat].getUser()->Send(packet);
}

void BlueMarble::sendToBlueMarble(PacketGuard& guard) const
{
    for (int i = 0; i < m_pScript->m_info.m_playerMax; ++i)
    {
        if (m_users[i].getUser() == 0)
        {
            continue;
        }
        m_users[i].getUser()->Send(guard);
    }
}
