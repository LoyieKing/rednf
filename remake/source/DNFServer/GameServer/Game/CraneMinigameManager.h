#ifndef DNF_GAME_CRANE_MINIGAME_MANAGER_H_
#define DNF_GAME_CRANE_MINIGAME_MANAGER_H_

#include <vector>
#include <utility>
#include <stddef.h>   // offsetof（下方布局断言用）


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

// 布局断言 —— 证据：ORIG importCraneMinigameScript 0x088e3924 逐条 disp：
// `movl $0x14,(%esp); call _Znwj`（sizeof 0x14）、`mov %ax,(%ebx)`（+0 word）、
// `mov %eax,0x4(%ebx)`（+4 dword）、`mov %ax,0x8(%ebx)`（+8 word）、
// `fstps 0xc(%ebx)`（+0xc float）、`mov %eax,0x10(%ebx)`（+0x10 dword）。
// 实测（4.4.7 -m32 模板参数回显）：sizeof=20、偏移 0/4/8/12/16。偏移断言走
// POD 镜像（仓内 CDataManager.h/EquipmentParameterInfo.h 同法：带用户 ctor 的类
// 直接 offsetof 会触发 GCC「invalid access to non-static data member of NULL
// object」警告，镜像成员类型/顺序逐条照抄，零语义）。
struct CraneItemInfoLayoutMirror
{
    unsigned short m_id;
    unsigned short m_pad02;
    int m_itemIdx;
    unsigned short m_cnt;
    unsigned short m_pad0a;
    float m_viewRatio;
    int m_pickRatio;
};
static_assert(sizeof(CraneItemInfoLayoutMirror) == 0x14
                  && sizeof(CraneItemInfoLayoutMirror) == sizeof(CraneItemInfo),
              "CraneItemInfo size drift (ORIG new 0x14)");
static_assert(offsetof(CraneItemInfoLayoutMirror, m_id) == 0x00, "m_id +0x00 (mov %ax,(%ebx))");
static_assert(offsetof(CraneItemInfoLayoutMirror, m_itemIdx) == 0x04, "m_itemIdx +0x04 (mov %eax,0x4(%ebx))");
static_assert(offsetof(CraneItemInfoLayoutMirror, m_cnt) == 0x08, "m_cnt +0x08 (mov %ax,0x8(%ebx))");
static_assert(offsetof(CraneItemInfoLayoutMirror, m_viewRatio) == 0x0c, "m_viewRatio +0x0c (fstps 0xc(%ebx))");
static_assert(offsetof(CraneItemInfoLayoutMirror, m_pickRatio) == 0x10, "m_pickRatio +0x10 (mov %eax,0x10(%ebx))");

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

// 布局断言 —— 证据：ORIG importCraneMinigameScript 0x088e3924 逐条 disp：
// `mov %dx,(%eax)`（+0 word m_viewCnt）、`add $0x4,%eax` → vector::size /
// push_back（+4 m_items）、`mov %eax,0x10(%edx)`（+0x10 dword m_material）、
// `mov %dx,0x14(%eax)`（+0x14 word m_field14）、
// `lea 0x18(%eax); call importActionMailScript`（+0x18 m_sendMail）、
// `lea 0x5c(%eax); call vector<pair<int,int>>::push_back`（+0x5c m_needMaterials）。
// Script/Manager 含非 Trivial 成员（vector / Action_SendMail），无法用 POD 镜像
// 复刻偏移 ⇒ 只断言 sizeof（不触发 offsetof 警告）；偏移 0/4/16/20/24/92 与
// sizeof=104(0x68) 已由 4.4.7 -m32 模板参数回显实测，成员偏移另由上方逐成员
// ORIG disp 注释 + 本 TU importCraneMinigameScript 的具名字段访问锁定。
static_assert(sizeof(CraneMinigameScript) == 0x68,
              "CraneMinigameScript size drift (+0x5c vector 尾 0x68)");

// ORIG C1 0x080ebc32 仅调 CraneMinigameScriptC1（构造 m_items/m_sendMail/
// m_needMaterials）+ m_selectIdx=0；D1 仅调 CraneMinigameScriptD1。故
// Manager 与 Script 同一对象布局，Manager 公开继承 Script（成员全部来自
// 基类），仅新增 m_selectIdx(+0x68)。
class CraneMinigameManager : public CraneMinigameScript
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
    // 校验 m_material(+0x10) 与 slot 物品 m_addInfo 一致、(short)+0x14 低 16 位 ≤
    // item.m_addInfo2 后 delete_item(1,slot,count,0x34,true)。
    // 返回 0x11=材料不符、0x16=数量/删除失败、0=成功。
    int startUseMaterial(CUser* user, int slot);

private:
    unsigned char m_selectIdx;                     // +0x68（Script 无此成员）
};

// Manager 实测 sizeof=0x6c（+0x68 m_selectIdx 后补齐）；SendCraneItemNeedMaterial
// 报告 0x080ebf2e `put_byte(this+0x68)` 实证偏移。
static_assert(sizeof(CraneMinigameManager) == 0x6c,
              "CraneMinigameManager size drift (+0x68 m_selectIdx 后补齐)");

#endif  // DNF_GAME_CRANE_MINIGAME_MANAGER_H_
