// ============================================================================
// CBossPlay.h — WongWork::CBossPlay 权威基类（ORIG 还原）
//
// ORIG 证据（df_game_r, elf32-i386）：
//   - 布局：vptr@+0x00；总尺寸 0x9e8（CBossStage 从 +0x9e8 起扩展）。
//   - 虚表 _ZTVN8WongWork9CBossPlayE @ 0x08b75080，共 6 个虚函数槽（[2]..[7]）：
//       [2] generateRewardItem   [3] generateRewardExp
//       [4] _checkMemberDie      [5] checkAllMemberReady
//       [6] handleDieUser        [7] handleLeaveUser
//     析构函数为非虚（CBossTower 由 StaticPool 管理，不用 delete 释放）。
//   - Ctor（W 0x081519ec）：置 vptr=CBossPlay vtable，构造 4 个 stRewardItem_t 块，
//     再调 reset()。Dtor（W 0x08151a3a）：置 vptr 后调 reset()。
//   - reset（0x08149120）逐字段清零，见 CBossPlay.cpp。
//
// 由 FixCBossPlay 建立；CBossStage : CBossPlay、CBossTower : CBossStage。
// ============================================================================
#ifndef DNF_CBOSSPLAY_H
#define DNF_CBOSSPLAY_H

#include "Inven_Item.h"

class CUser;
class CParty;
class CDungeon;

// ENUM_DUNGEON_DIFFICULTY（CBossPlay 虚函数参数类型）。守卫与 CBattle_Field_shared.h
// 相同（DNF_ENUM_ENUM_DUNGEON_DIFFICULTY_DEFINED），此处本地定义避免早拉入
// CBattle_Field_shared.h 的 CMap 视图（与 CMap.h 互斥）。
#ifndef DNF_ENUM_ENUM_DUNGEON_DIFFICULTY_DEFINED
#define DNF_ENUM_ENUM_DUNGEON_DIFFICULTY_DEFINED
enum ENUM_DUNGEON_DIFFICULTY { ENUM_DUNGEON_DIFFICULTY_0 = 0 };
#endif

namespace WongWork
{

class CBossPlay
{
public:
    // 成员奖励块（0x268 字节；每成员一个，共 4 个）。ctor ORIG W 0x081519b6
    // （构造 10 个 Inven_Item）。
    struct stRewardItem_t
    {
        stRewardItem_t();               // ORIG W 0x081519b6
        int m_count;                    // +0x00（generateRewardItem 置为 itemCount）
        Inven_Item m_items[10];         // +0x04（10 * 0x3d = 0x262）
        char m_pad[0x268 - 0x04 - 10 * 0x3d];  // +0x266..+0x268（2 字节）
    };                                  // sizeof 0x268

    CBossPlay();                        // ORIG W 0x081519ec
    ~CBossPlay();                       // ORIG W 0x08151a3a（非虚）

    // ---- 虚函数（虚表槽顺序必须保持：[2]..[7]）----
    virtual bool generateRewardItem(int memberIdx, int itemCount, unsigned int a,
                                   ENUM_DUNGEON_DIFFICULTY diff, unsigned int b,
                                   const CDungeon* dungeon);  // ORIG 0x08149914
    virtual bool generateRewardExp(int memberIdx, unsigned int a, unsigned int b,
                                  const CDungeon* dungeon);   // ORIG 0x08149e20
    virtual bool _checkMemberDie();                            // ORIG 0x08149382
    virtual bool checkAllMemberReady(int);                     // ORIG 0x081493fe
    virtual bool handleDieUser(CUser* user);                   // ORIG 0x081494a4
    virtual bool handleLeaveUser(CUser* user);                 // ORIG 0x08149724（[b27 DL3R] ORIG 调用点 CBossTower 0x08143b04 为 `call; test %al,%al; je` 单字节直判 ⇒ callee 返回 bool/uchar；mangled 不含返回类型，ABI 安全）

    // ---- 非虚成员 ----
    void InitPlayUser(int questCond, int towerIdx);   // ORIG 0x0814926e
    int getLiveMemberCount();                         // ORIG 0x08149466

    // reset（ORIG 0x08149120）：清零全部字段（含 4 个 stRewardItem_t 块与两个 int 数组）。
    void reset();

    // ---- 弱（inline）helper（ORIG W）----
    // 0x08145682：单 movzbl 直返（bool 字段直返与 uchar 返回同形）。b28d-solo-CBossTower：
    // uchar→bool（P292 调用点判据——ORIG 0x0814414d/0x081439f5 `call; test %al,%al; je`
    // 无 setne ⇒ callee bool；uchar 恒多 setne 两段）。符号尺寸/body 不变。
    bool isMemberAlive(int idx) { return m_memberAlive[idx]; }
    void setMemberAlive(int idx, bool alive) { m_memberAlive[idx] = alive ; } // 0x0814a7e4
    void setMemberReady(int idx, bool ready) { m_memberReady[idx] = ready ; } // 0x08145692

    CParty* getParty() const { return m_party; }

protected:  // 派生类 CBossTower::handleUseItem 写 m_useItemCount/m_memberField（ORIG 同址访问）
    CParty* m_party;             // +0x04
    int m_field8;                // +0x08
    char m_allMemberReady;       // +0x0c
    bool m_memberAlive[4];       // +0x0d（ORIG setMemberAlive 0x0814a7e4 this先+movzbl回读序，
                                 //   探针实证 bool 数组+bool 形参直赋唯一匹配；isMemberAlive
                                 //   movzbl 零扩展直返同形）
    bool m_memberReady[4];       // +0x11（setMemberReady 0x08145692 同 setMemberAlive 实证）
    int m_rewardExp[4];          // +0x18（generateRewardExp 逐成员写入，16 字节）
    stRewardItem_t m_memberReward[4];  // +0x28（4 * 0x268 = 0x9a0）
    int m_useItemCount[4];       // +0x9c8（handleUseItem 0xbdd 道具逐槽计数；reset 清零）
    int m_memberField[4];        // +0x9d8（handleLeaveUser 读取 ==0 判断）
    // 总尺寸 0x9e8
};

}  // namespace WongWork

#endif  // DNF_CBOSSPLAY_H