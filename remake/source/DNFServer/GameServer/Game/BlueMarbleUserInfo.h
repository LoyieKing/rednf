#ifndef GAME_BLUEMARBLE_USERINFO_H_
#define GAME_BLUEMARBLE_USERINFO_H_

// ============================================================================
// df_game_r BlueMarbleUserInfo（G2-4 大富翁子对象，尺寸 0x28）
// 布局依据 ORIG 构造（0x080d6a90）/报告 BlueMarbleUserInfo.md：
//   +0x000 CUser* m_user            +0x004 BlueMarbleUserState::T m_userState
//   +0x008 int m_boardZone          +0x00c BlueMarbleUserGrade::T m_grade
//   +0x010 BuffInfo m_buff（0xc）    +0x01c TileIndexInfo m_tileIndex（0x8）
//   +0x024 BlueMarbleScriptManager* m_pScript
// BuffInfo/TileIndexInfo 构造/复位经 asm-label extern 调用（权威 TU 另行交付），
// 本头仅给出字段布局（不声明构造函数，避免自动构造顺序与 ORIG 不符），
// 保证 0x28 尺寸与复制语义（平凡拷贝）与 ORIG 一致。
// ============================================================================

#include "CUser.h"

class BlueMarbleScriptManager;

namespace BlueMarbleUserState
{
enum T
{
    T_0 = 0
};
}

namespace BlueMarbleUserGrade
{
enum T
{
    T_0 = 0
};
}

// ---- BuffInfo（0xc）/ TileIndexInfo（0x8，内嵌于 BlueMarbleUserInfo） ----
class BuffInfo
{
public:
    int m_exp;   // +0x00
    int m_gold;  // +0x04
    int m_drop;  // +0x08
};

class TileIndexInfo
{
public:
    int m_dungeon;         // +0x00
    unsigned int m_item;   // +0x04
};

class BlueMarbleUserInfo
{
public:
    BlueMarbleUserInfo();
    ~BlueMarbleUserInfo();

    void setUser(CUser* user);
    CUser* getUser() const;

    void setScript(BlueMarbleScriptManager* script);

    void setUserState(BlueMarbleUserState::T state);
    BlueMarbleUserState::T getUserState() const;

    void setBoardZone(int zone, int pos);
    int getBoardZone() const;

    void setGrade(BlueMarbleUserGrade::T grade);
    int getGrade() const;

    void setDungeon(int dungeon);
    int getDungeon() const;

    void setItem(unsigned int itemIdx);
    int getItem() const;

    void setExp(int exp);
    int getExp() const;

    void setGold(int gold);
    int getGold() const;

    void setDrop(int drop);
    int getDrop() const;

    void resetBlueMarbleUserInfo();
    void resetBuff();
    void resetIndex();

private:
    CUser* m_user;                        // +0x00
    BlueMarbleUserState::T m_userState;   // +0x04
    int m_boardZone;                      // +0x08
    BlueMarbleUserGrade::T m_grade;       // +0x0c
    BuffInfo m_buff;                      // +0x10
    TileIndexInfo m_tileIndex;            // +0x1c
    BlueMarbleScriptManager* m_pScript;   // +0x24
};

#endif  // GAME_BLUEMARBLE_USERINFO_H_
