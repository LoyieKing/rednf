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
#include "TimerQueue.h"  // TIMER_MESSAGE（_onTimerProcess ORIG 参数类型）
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
    std::map<int, map_item>& stageItemMap() { return m_stageData.m_itemMap; }

    // ---- 虚函数 override ----
    virtual bool handleDieUser(CUser* user);      // 0x08143a62
    virtual bool handleLeaveUser(CUser* user);    // 0x08143ae2（随基类 bool 化同步；ORIG 尾 mov$0/mov$1 常量直返无规范化）
    virtual bool handleMoveMap(unsigned short* mapIndex); // 0x08143996
    virtual bool handleDropItem(CUser* user, int a, int b, char c, int d, int e);  // 随基类 CBossStage bool 化同步（ORIG 尾 mov$0/mov$1 常量无规范化）  // 0x08143e5e
    virtual bool handleKillMonster(CUser* user, unsigned short a, unsigned short b,
                                  const MSG_MONSTER_DIE& die, int* d);  // 0x08143ebc
    // 新增虚（char 按值，与 CBossStage 的 char& 版本不同）0x08143d2c
    // [Q6HeaderTypeAxis 2026-09-11 类型轴] ORIG prett 0x08b701a0 size 0x4d =
    //   `virtual bool WongWork::CBossTower::handlePickupItem(CUser*, int, bool, char)`
    //   （REF 见 ORIG 0x08143dec `movl $0x8b701a0,0x4(%esp)`，P8S2 实读）。
    //   我方原 `virtual int` ⇒ prett 串多 4 字节（"int " vs "bool "），故此前只能
    //   手写串；本项与基类 CBossStage::handlePickupItem(char&) 参数不同（char 按值
    //   vs char&）⇒ 非 override，无 conflicting return type 风险；返回类型不入
    //   mangling，vtable 槽位/符号/sizeof 零变化。
    virtual bool handlePickupItem(CUser* user, int a, bool b, char c);

    // ---- 其余非虚方法（全部于 CBossTower.cpp 按对应 ORIG 报告转写实现）----
    // b28d-solo-CBossTower P248 档①：ORIG 调用点全部 `call; xor $0x1,%eax;
    // test %al,%al; je`（无 setcc 中转）⇒ callee 返回 bool（int 恒多
    // `test %eax,%eax; setne %al` 两条）。返回类型不入 mangled，零 ABI 风险。
    bool InitBossTower(CParty* party, int towerIdx);       // 0x08142f32（调用点 0x081435aa xor）
    bool InsertRewardExp();                                // 0x081446a2（调用点 0x0814460c xor）
    bool InsertRewardItems();                              // 0x081447a0（调用点 0x08144625 xor）
    bool SendELPLPacket();                                 // 0x081441d4（调用点 0x0814463e xor）
    bool SendRewardPacket();                               // 0x0814432a（调用点 0x081445f0 xor）
    void _destroy();                                       // 0x08142b36
    void _onPrepareFinishTower(bool flag);                 // 0x08142d24
    void _onTimerProcess(TIMER_MESSAGE msg, unsigned int a);  // 0x08142cde（mangled E13TIMER_MESSAGEj）
    bool checkStartGameCondition(unsigned char& a, bool b);  // 0x08142fa6（调用点 0x081435b8/0x081441e6 xor）
    bool generateRewardExp();                              // 0x08144d54（调用点 0x081445b8 xor；非虚，隐藏基类重载）
    bool generateRewardItem();                             // 0x08144e36（调用点 0x081445d4 xor；非虚，隐藏基类重载）
    int handleEPLPCommand(int cmd);                        // 0x08142b72（ORIG eax 0/1）
    int handleFinishLoading(CUser* user);                 // 0x08143f6a（ORIG 返回 0/1）
    int handleUseItem(CUser* user, unsigned long a, unsigned char b);    // 0x081454b8（ORIG 返回 0/1）
    int onChargeRequireItem(CUser* user);                  // 0x0814530c（ORIG eax 0/1）
    int onClearBossTower();                                // 0x08144508（ORIG eax 0/1）
    int onFinishStage();                                   // 0x081440b2（ORIG 恒返 1）
    char handleStartGame(CParty* party, int a, char b, ENUM_DUNGEON_TYPE dungeonType);  // 0x081434f4（ORIG mangled ...ic17ENUM_DUNGEON_TYPE；SelectDungeon 读 %al 返回值）
    int onReadyTimerCheck();                               // 0x0814512a（ORIG eax 0/1）
    int onReturnVillage();                                 // 0x08144fca（ORIG 恒返 1）

private:
    int m_idx;                      // +0xaf0（getIdx/setIdx）
    int m_towerStartTick;           // +0xaf4（handleStartGame 开局时刻；
                                    //   _onPrepareFinishTower 结算时长起点）
    int m_towerEndTick;             // +0xaf8（handleStartGame 清零；
                                    //   _onPrepareFinishTower 置当前 tick）
    // b28d-54：char→bool。ORIG handleDieUser 0x08143ab6 `movzbl 0xafc(%eax),%eax;
    //   xor $1,%eax; test %al,%al; je`（P248 档②：字段 bool 化 + 裸 `if(!f)` 才产
    //   xor 物化；char 恒 `cmp $1`）。宽度 1、sizeof 0xb18、偏移全不变。
    bool m_prepareFinishFlag;       // +0xafc（_onPrepareFinishTower(bool) 写入）
    CBossDungeonEntranceLog m_dungeonLog;  // +0xb00..+0xb18（0x18）
    // 总尺寸 0xb18
};

// 布局断言（C++98 兼容）：CBossTower 总尺寸 0xb18
// （隐式验证 m_dungeonLog 的 std::map 在目标工具链为 0x18 字节）。
typedef char CBossTower_size_assert_0xb18[(sizeof(WongWork::CBossTower) == 0xb18) ? 1 : -1];

}  // namespace WongWork

#endif  // DNF_CBOSSTOWER_H