#ifndef GAME_BLUEMARBLE_H_
#define GAME_BLUEMARBLE_H_

// ============================================================================
// df_game_r BlueMarble（G2-3 PvP 链，大富翁房间，池化 300）
// 布局依据 ORIG 构造（0x080d8a5c）/析构（0x080d8bae）推导：
//   +0x000 int m_memoryPoolIndex（-1）+0x004 std::vector<BlueMarbleUserInfo>
//   +0x010 int m_state              （getState 0x080d6dc8 `mov 0x10(%eax)` /
//                                     changeState 0x080d9202
//                                     `mov %edx,0x10(%eax)` 实证同一槽；ORIG 无
//                                     m_curUserCount 语义）
//                                    +0x014 CMTRand* m_pRands（new[4]）
//   +0x018 int m_diceNumber         （getDiceNumber 0x080d6fc4 `mov 0x18(%eax)`）
//   +0x01c int m_blueMarbleMap      （getBlueMarbleMap 0x080d6e7c `mov 0x1c(%eax)`）
//   +0x020 std::set<int>（0x18）     +0x038 BlueMarbleScriptManager* m_pScript
//   +0x03c int m_maxBoardSize       （getMaxBoardSize 0x080d6ecc/setMaxBoardSize
//                                     0x080d6e88 `mov %edx,0x3c(%eax)`）
//                                    （总尺寸 0x40）
// 方法签名以 ORIG mangled 名为准（/tmp/bluemarble_syms.txt 与报告）。
// ============================================================================

#include <set>
#include <vector>

#include "PvPTypes.h"   // PvP/BlueMarble 子对象权威声明（原 PvP_deps.h 已并入）
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
    // 原 int；ORIG 0x080d9202 尾 pop;ret 无 %eax 置值 ⇒ void（mangled 无返回类型）
    void changeState(BlueMarbleState::T state);
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
    bool getVisitZone(int seat) const;
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
    bool checkJoin();
    int join(CUser* user);
    int leaveUser(CUser* user);
    int setScript(BlueMarbleScriptManager* script);
    int setUserInfo(BlueMarbleScriptManager* script);
    int resetBlueMarble();
    int resetBuffInfo(int seat);
    int resetIndexInfo(int seat);
    int checkUserConditionEnterBlueMarble(CUser* user);
    void checkBlueMarbleUserConfirm(CUser* user);
    int enterBlueMarble(CUser* user);
    bool checkStartBlueMarble();
    int startBlueMarble();
    bool checkThrowDice(int seat);
    int throwDice(int seat);
    int userThrowDice(CUser* user);
    int movePosition(CUser* user, int pos);
    bool checkMoveFirstZone(int seat, BlueMarbleTileScript::Type type);
    int tileType(int seat);
    int intoBoardZone(int zone);
    bool checkLastZoneDungeonClear(int seat);
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
    BlueMarbleState::T m_state;                // +0x010（getState/changeState 权威；
                                               //   ORIG changeState 0x080d9205 this 先载直存
                                               //   ⇒ 与形参同型，技巧5）
    CMTRand* m_pRands;                         // +0x014
    int m_diceNumber;                          // +0x018（getDiceNumber/setDiceNumber）
    int m_blueMarbleMap;                       // +0x01c（getBlueMarbleMap/setBlueMarbleMap）
    std::set<int> m_set;                       // +0x020
    BlueMarbleScriptManager* m_pScript;        // +0x038
    int m_maxBoardSize;                        // +0x03c（getMaxBoardSize/setMaxBoardSize）
};

// 布局自检（字段重命名仅改标识符；ORIG 偏移实证见上方类注释）。
// 注：成员为 private，GCC 4.4.7 下 offsetof 私有成员是硬错误（非警告），
// 故仅断言总尺寸；逐槽偏移由上方注释的 ORIG 读/写指令实证 + 本 TU 编译产物
// 与 ORIG 反汇编比对口径保证。
static_assert(sizeof(BlueMarble) == 0x40, "BlueMarble size drift");

#endif  // GAME_BLUEMARBLE_H_
