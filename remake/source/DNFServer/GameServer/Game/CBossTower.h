// ============================================================================
// CBossTower.h — WongWork::CBossTower（CBossStage 派生）
//
// ORIG 证据（df_game_r, elf32-i386）：
//   - CBossTower : CBossStage（0xaf0 基类）；CBossTower 自身尺寸 0xb18。
//   - Ctor（0x081429b4）：调 CBossStage ctor、置 CBossTower vtable、构造
//     CBossDungeonEntranceLog@+0xb00、CBossStage::reset、+0xaf0=0、+0x08=0、
//     memset +0x0d,0x4、+0x9f0=0、+0x04=0。
//   - Dtor（0x08142a74）：置 vtable、CBossStage::reset、同上清零、析构
//     CBossDungeonEntranceLog@+0xb00、CBossStage dtor。
//   - getIdx（0x082aa728）读 +0xaf0；setIdx（0x082aa716）写 +0xaf0。
//   - 虚表：11 槽 = CBossPlay 6 + CBossStage 4 + 新增 handlePickupItem(char)；
//     override handleDieUser/handleLeaveUser/handleMoveMap/handleDropItem/
//     handleKillMonster。
//
// 由 FixCBossPlay 建立；ctor/dtor 实现在 CDeathTower.cpp，getIdx/setIdx 在
// CBossTower.cpp。
// ============================================================================
#ifndef DNF_CBOSSTOWER_H
#define DNF_CBOSSTOWER_H

#include "CBossDungeonEntranceLog.h"

#include "CBossStage.h"
class CUser;
struct MSG_MONSTER_DIE;

// ORIG 全局枚举（mangled 17ENUM_DUNGEON_TYPE；权威定义 CParty.h，同守卫幂等
// 复制——handleStartGame 声明须产生与 ORIG 一致的参数类型符号）。
#ifndef DNF_ENUM_ENUM_DUNGEON_TYPE_DEFINED
#define DNF_ENUM_ENUM_DUNGEON_TYPE_DEFINED
enum ENUM_DUNGEON_TYPE
{
    ENUM_DUNGEON_TYPE_0 = 0
};
#endif

namespace WongWork
{

class CBossTower : public CBossStage
{
public:
    CBossTower();               // ORIG 0x081429b4
    ~CBossTower();              // ORIG 0x08142a74

    int getIdx();               // ORIG 0x082aa728：return +0xaf0
    void setIdx(int idx);       // ORIG 0x082aa716：写 +0xaf0

    // ---- 虚函数 override ----
    virtual int handleDieUser(CUser* user);       // 0x08143a62
    virtual int handleLeaveUser(CUser* user);     // 0x08143ae2
    virtual int handleMoveMap(unsigned short* mapIndex);  // 0x08143996
    virtual int handleDropItem(CUser* user, int a, int b, char c, int d, int e);  // 0x08143e5e
    virtual int handleKillMonster(CUser* user, unsigned short a, unsigned short b,
                                  const MSG_MONSTER_DIE& die, int* d);  // 0x08143ebc
    // 新增虚（char 按值，与 CBossStage 的 char& 版本不同）0x08143d2c
    virtual int handlePickupItem(CUser* user, int a, bool b, char c);

    // ---- 其余方法（声明；实现见 CDeathTower.cpp / 后续批次）----
    int InitBossTower(CParty* party, int towerIdx);        // 0x08142f32（ORIG eax 0/1）
    void InsertRewardExp();                                // 0x081446a2
    void InsertRewardItems();                              // 0x081447a0
    void SendELPLPacket();                                 // 0x081441d4
    void SendRewardPacket();                               // 0x0814432a
    void _destroy();                                       // 0x08142b36
    void _onPrepareFinishTower(bool flag);                 // 0x08142d24
    void _onTimerProcess(int timerMsg, unsigned int a);    // 0x08142cde
    int checkStartGameCondition(unsigned char& a, bool b);// 0x08142fa6（ORIG eax 0/1）
    void generateRewardExp();                              // 0x08144d54
    void generateRewardItem();                             // 0x08144e36
    void handleEPLPCommand(int cmd);                       // 0x08142b72
    int handleFinishLoading(CUser* user);                 // 0x08143f6a（ORIG 返回 0/1）
    int handleUseItem(CUser* user, unsigned long a, unsigned char b);    // 0x081454b8（ORIG 返回 0/1）
    void onChargeRequireItem(CUser* user);                 // 0x0814530c
    void onClearBossTower();                               // 0x08144508
    int onFinishStage();                                   // 0x081440b2（ORIG 恒返 1）
    char handleStartGame(CParty* party, int a, char b, ENUM_DUNGEON_TYPE dungeonType);  // 0x081434f4（ORIG mangled ...ic17ENUM_DUNGEON_TYPE；SelectDungeon 读 %al 返回值）
    void onReadyTimerCheck();                              // 0x0814512a
    void onReturnVillage();                                // 0x08144fca

private:
    int m_idx;                      // +0xaf0（getIdx/setIdx）
    char m_padAF4[0xafc - 0xaf4];   // +0xaf4..+0xafc
    char m_prepareFinishFlag;       // +0xafc（_onPrepareFinishTower(bool) 写入）
    CBossDungeonEntranceLog m_dungeonLog;  // +0xb00..+0xb18（0x18）
    // 总尺寸 0xb18
};

// 布局断言（C++98 兼容）：CBossTower 总尺寸 0xb18
// （隐式验证 m_dungeonLog 的 std::map 在目标工具链为 0x18 字节）。
typedef char CBossTower_size_assert_0xb18[(sizeof(WongWork::CBossTower) == 0xb18) ? 1 : -1];

}  // namespace WongWork

#endif  // DNF_CBOSSTOWER_H