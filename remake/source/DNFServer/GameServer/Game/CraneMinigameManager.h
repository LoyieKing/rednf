#ifndef DNF_GAME_CRANE_MINIGAME_MANAGER_H_
#define DNF_GAME_CRANE_MINIGAME_MANAGER_H_

#include <vector>
#include <utility>


class CUser;   // SendCraneItemNeedMaterial(CUser*) 形参（Inter_LoadEtc 消费）
#include "InGameEventScripts.h"   // ::Action_SendMail 唯一权威定义（全局作用域，
                                  // ORIG _ZN15Action_SendMail*；EventLevelupA2 批次）
// ORIG：CraneMinigameManager 与 CraneMinigameScript 为同一对象（init 0x080ebc6e
// 以 (CraneMinigameScript*)this 调用 importCraneMinigameScript），布局一致：
//   +0x10 int、+0x18 Action_SendMail、+0x5c vector<pair<int,int>>、+0x68 byte。

class CraneItemInfo
{
public:
    CraneItemInfo() : m_id(0xffff), m_itemIdx(-1) {}  // ORIG C1 0x88e41f6 W：m_id=0xffff、+4=-1
    // ORIG W 0x088e3708：只拷 +0(word)/+4(int)/+8(word) 三字段（0xc 字节），
    // 不拷 m_viewRatio/m_pickRatio；需显式实现（隐式会拷全 0x14）。
    CraneItemInfo& operator=(const CraneItemInfo& other);

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
    CraneMinigameScript();        // ORIG C1 0x088e3736：仅默认构造 m_items(+0x04)、
                                  // m_sendMail(+0x18)、m_needMaterials(+0x5c)；
                                  // m_viewCnt/m_material/m_field14 故意不初始化。
                                  // 定义 CraneMinigameManager.cpp（外联产符号）。
    ~CraneMinigameScript();       // ORIG D1 0x088e37a8：遍历 m_items delete 非空元素、
                                  // 元素置 0 后 clear，再逆序析构成员。
                                  // 定义 CraneMinigameManager.cpp（外联产符号）。
    unsigned short m_viewCnt;                // +0x00（[viewCnt]）
    unsigned short m_pad02;                  // +0x02
    std::vector<CraneItemInfo*> m_items;     // +0x04
    int m_material;                          // +0x10（[material] 首个 int）
    short m_field14;                         // +0x14（[material] 第二个 short）
    unsigned char m_pad16[2];                // +0x16
    Action_SendMail m_sendMail;              // +0x18（ORIG 内嵌对象）
    std::vector<std::pair<int, int> > m_needMaterials;  // +0x5c
    // ORIG T 0x088e38d6（CraneMinigameScript/getItem.md）：size 校验后返回
    // m_items[idx]，越界返回 0。定义 EventBattleLink.cpp。
    CraneItemInfo* getItem(short idx);
};

class CraneMinigameManager
{
public:
    CraneMinigameManager();
    ~CraneMinigameManager();
    bool init();                      // ORIG 0x080ebc6e 返回 1 成功 / 0 失败
    void updateCraneItemNeedMaterial(int count);   // ORIG 0x080ebebc
    void SendCraneItemNeedMaterial(CUser* user);   // ORIG T（Inter_LoadEtc::dispatch_sig 消费；producer 归 Crane 域批次）

    // EventBattleLink 批次（ORIG nm 核实）：
    int getItemCnt(int slot);                      // ORIG T 0x080ebd44 _ZN20CraneMinigameManager10getItemCntEi
    bool isPickSuccess(short slot, int pickIdx);   // ORIG T 0x080ebce0 _ZN20CraneMinigameManager13isPickSuccessEsi
    int sendItem(CUser* user, int slot);           // ORIG T 0x080ebd7a _ZN20CraneMinigameManager8sendItemEP5CUseri
    // ORIG T 0x080ebe06（d_aa_body.cpp objdump 转写；定义 StubS3A_deps.cpp）：
    // 校验 m_field10 与 slot 物品 m_addInfo 一致、(short)m_pad14 低 16 位 ≤
    // item.m_addInfo2 后 delete_item(1,slot,count,0x34,true)。
    // 返回 0x11=材料不符、0x16=数量/删除失败、0=成功。
    int startUseMaterial(CUser* user, int slot);

    // NOTE（OffsetCleanup7）：ORIG 中 Manager 与 Script 为同一对象布局，
    // getItemCnt/isPickSuccess/sendItem 均以 (CraneMinigameScript*)this 调
    // getItem / 访问 +0x18 内嵌 Action_SendMail；本头 m_sendMail 原为指针
    // 建模（推断误差），已按 ORIG 改为内嵌对象（0x44 字节，布局不变）。

private:
    char m_pad[0x10];                              // +0x00
    int m_field10;                                 // +0x10
    char m_pad14[4];                               // +0x14
    Action_SendMail m_sendMail;                    // +0x18（ORIG 内嵌对象，0x44 字节）
    std::vector<std::pair<int, int> > m_materials; // +0x5c
    unsigned char m_selectIdx;                     // +0x68
};

#endif  // DNF_GAME_CRANE_MINIGAME_MANAGER_H_
