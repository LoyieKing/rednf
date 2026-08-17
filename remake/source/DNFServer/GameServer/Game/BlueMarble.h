#ifndef GAME_BLUEMARBLE_H_
#define GAME_BLUEMARBLE_H_

// ============================================================================
// df_game_r BlueMarble（G2-3 PvP 链，大富翁房间，池化 300）
// 布局依据 ORIG 构造（0x080d8a5c）/析构（0x080d8bae）推导：
//   +0x000 int m_memoryPoolIndex（-1）+0x004 std::vector<BlueMarbleUserInfo>
//   +0x010 int m_curUserCount        +0x014 CMTRand* m_pRands（new[4]）
//   +0x018 int m_field18             +0x01c int m_field1c
//   +0x020 std::set<int>（0x18）     +0x038 BlueMarbleScriptManager* m_pScript
//   +0x03c int m_field3c             （总尺寸 0x40）
// 方法签名以 ORIG mangled 名为准（/tmp/bluemarble_syms.txt 与报告）。
// ============================================================================

#include <set>
#include <vector>

#include "PvP_deps.h"
#include "InterfacePacketBuf.h"

class BlueMarble
{
public:
    BlueMarble();
    ~BlueMarble();

    // ---- 基础访问器 ----
    int getMemoryPoolIndex() const;
    void setMemoryPoolIndex(int idx);
    int getState() const;
    int changeState(BlueMarbleState::T state);
    int getWinner() const;
    int getWaitUserCount() const;
    int getUserSlot(CUser* user) const;
    int getUserState(int seat) const;
    void setUserState(int seat, BlueMarbleUserState::T state);
    int setUserGrade(int grade);
    int getUserDeadCount(int seat) const;
    void setUserDeadCount(int count);
    int getMaxBoardSize() const;
    int setMaxBoardSize();
    int getDiceNumber() const;
    int setDiceNumber();
    int getBlueMarbleMap() const;
    int setBlueMarbleMap();
    int getMovePosition(int seat) const;
    int setMovePosition(int seat, int pos);
    int getVisitZone(int seat) const;
    int setVisitZone(int zone);
    int getDungeonIndex(int seat) const;
    int setDungeonIndex(int seat, int dungeon);
    int getItemIndex(int seat) const;
    int setItemIndex(int seat, int itemIdx);
    int getExpBuff(int seat) const;
    int setExpBuff(int seat, int exp);
    int getGoldBuff(int seat) const;
    int setGoldBuff(int seat, int gold);
    int getDropBuff(int seat) const;
    int setDropBuff(int seat, int drop);
    int getBuff(CUser* user, BlueMarbleBuffType::T type);
    int getRandomItem() const;
    int getRandomDungeon(int seat, BlueMarbleTileScript::Type type) const;

    // ---- 房间流程 ----
    int checkJoin();
    int join(CUser* user);
    int leaveUser(CUser* user);
    int setScript(BlueMarbleScriptManager* script);
    int setUserInfo(BlueMarbleScriptManager* script);
    int resetBlueMarble();
    int resetBuffInfo(int seat);
    int resetIndexInfo(int seat);
    int checkUserConditionEnterBlueMarble(CUser* user);
    int checkBlueMarbleUserConfirm(CUser* user);
    int enterBlueMarble(CUser* user);
    int checkStartBlueMarble();
    int startBlueMarble();
    int checkThrowDice(int seat);
    int throwDice(int seat);
    int userThrowDice(CUser* user);
    int movePosition(CUser* user, int pos);
    int checkMoveFirstZone(int seat, BlueMarbleTileScript::Type type);
    int tileType(int seat);
    int intoBoardZone(int zone);
    int checkLastZoneDungeonClear(int seat);
    int enterDungeon(CUser* user);
    int exitDungeon(CUser* user);
    int clearDungeon(CUser* user);
    int giveupDungeon(CUser* user);
    int giveBonusItem(CUser* user);
    int resultReward();
    int finishBlueMarble(int seat);
    int setResultItem(int seat, Inven_Item& item);
    int insertItem(int seat, Inven_Item& item, eItemAddReason reason);
    int setLeaveUserBlueMarbleState();
    int setLeaveUserSlot(int seat);
    int sendBlueMarbleDiceInfo(int seat) const;
    int sendBlueMarbleRoomInfo() const;
    int sendBoardZoneActionInfo(int seat, int zone, unsigned int pos) const;
    int sendFinishBlueMarble() const;
    int sendStartBlueMarble() const;
    int sendTileGiveUpInfo(int seat) const;
    int sendUpdateItemInfo(int seat, int idx) const;
    void sendToBlueMarble(PacketGuard& guard) const;

private:
    int m_memoryPoolIndex;                     // +0x000
    std::vector<BlueMarbleUserInfo> m_users;   // +0x004
    int m_curUserCount;                        // +0x010
    CMTRand* m_pRands;                         // +0x014
    int m_field18;                             // +0x018
    int m_field1c;                             // +0x01c
    std::set<int> m_set;                       // +0x020
    BlueMarbleScriptManager* m_pScript;        // +0x038
    int m_field3c;                             // +0x03c
};

#endif  // GAME_BLUEMARBLE_H_
