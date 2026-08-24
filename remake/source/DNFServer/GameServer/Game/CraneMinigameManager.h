#ifndef DNF_GAME_CRANE_MINIGAME_MANAGER_H_
#define DNF_GAME_CRANE_MINIGAME_MANAGER_H_

#include <vector>
#include <utility>

#include "InGameEventScripts.h"   // ::Action_SendMail 唯一权威定义（全局作用域，
                                  // ORIG _ZN15Action_SendMail*；EventLevelupA2 批次）
// ORIG：CraneMinigameManager 与 CraneMinigameScript 为同一对象（init 0x080ebc6e
// 以 (CraneMinigameScript*)this 调用 importCraneMinigameScript），布局一致：
//   +0x10 int、+0x18 Action_SendMail、+0x5c vector<pair<int,int>>、+0x68 byte。

class CraneItemInfo
{
public:
    CraneItemInfo() {}                       // ORIG C1 0x88e41f6 W（空构造）

    unsigned short m_id;                     // +0x00（= 入表前 m_items.size()）
    unsigned short m_pad02;                  // +0x02
    int m_itemIdx;                           // +0x04（[item] ScanInt）
    unsigned short m_cnt;                    // +0x08（[cnt] ScanInt）
    unsigned short m_pad0a;                  // +0x0a
    float m_viewRatio;                       // +0x0c（[viewRatio] ScanFloat）
    int m_pickRatio;                         // +0x10（[pickRatio] ScanInt）
};                                           // sizeof = 0x14

// Action_SendMail（全局作用域，0x44 字节）内嵌于 +0x18..+0x5b；
// 权威布局/成员见 InGameEventScripts.h（本头不再重复定义）。

class CraneMinigameScript
{
public:
    unsigned short m_viewCnt;                // +0x00（[viewCnt]）
    unsigned short m_pad02;                  // +0x02
    std::vector<CraneItemInfo*> m_items;     // +0x04
    int m_material;                          // +0x10（[material] 首个 int）
    short m_field14;                         // +0x14（[material] 第二个 short）
    unsigned char m_pad16[2];                // +0x16
    Action_SendMail m_sendMail;              // +0x18（ORIG 内嵌对象）
    std::vector<std::pair<int, int> > m_needMaterials;  // +0x5c
};

class CraneMinigameManager
{
public:
    CraneMinigameManager();
    ~CraneMinigameManager();
    bool init();                      // ORIG 0x080ebc6e 返回 1 成功 / 0 失败
    void updateCraneItemNeedMaterial(int count);   // ORIG 0x080ebebc

private:
    char m_pad[0x10];                              // +0x00
    int m_field10;                                 // +0x10
    char m_pad14[4];                               // +0x14
    Action_SendMail* m_sendMail;                   // +0x18（ORIG 内嵌对象；指针建模，推断）
    char m_pad1c[0x40];                            // +0x1c..0x5b
    std::vector<std::pair<int, int> > m_materials; // +0x5c
    unsigned char m_selectIdx;                     // +0x68
};

#endif  // DNF_GAME_CRANE_MINIGAME_MANAGER_H_
