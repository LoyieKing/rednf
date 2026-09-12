// df_game_r 还原 —— Arad_IPGProcess（全局类，非 ARAD:: 命名空间；ORIG nm 实证）。
// ORIG 符号（nm -S /home/wangyilei/下载/df_game_r）：
//   W 0x0817a2ba _ZN15Arad_IPGProcessC1EP18ItemVendingMachine   （ctor：*(this)=vm）
//   W 0x0817a2c8 _ZN15Arad_IPGProcessD1Ev                        （空体；无 D0 → 非虚析构，无 vtable）
//   T 0x0819c2b4 CashShopModeAvatar(CUser*, m, i, i, c, c, PKc, Ri, j, i, c)
//   T 0x0819c434 CashShopModeCoin(CUser*, t, j, i, c)
//   T 0x0819c4a0 CashShopModeItem(CUser*, m, t, l, Ri(RInven_Item), RN..param_t, Ri, j, i, c)
//   T 0x0819c670 CashShopModeCreature(CUser*, m, t, Ri, Ri, j, i, c)
//   T 0x0819c862 CashShopModeVendingItem(CUser*, m, t, l, RInven_Item, RN..param_t, Ri, i, c)
// 布局：+0x00 ItemVendingMachine*（ctor 逐指令：mov 0xc(%ebp),%edx; mov %edx,(%eax)）。
// 各 CashShop* 方法内 ORIG 把 *(this)（即 vm，其 +0x00 为内嵌 WongWork::CCeraShop）
// 按 CCeraShop* 传给 CCeraShop::_process* 家族（地址等价，类型双关为 ORIG 原样）。
// 声明属主：HdrCreate1 生成；实现：Arad_IPGProcess.cpp（LinkE2.JpnExImpl 批次）。
#ifndef DNF_GAME_ARAD_IPGPROCESS_H_
#define DNF_GAME_ARAD_IPGPROCESS_H_

#include <vector>

#include "CCeraShop.h"  // WongWork::CCeraShop 完整定义（stCeraShopItemParam_t 形参）
#include "GameTypes.h"  // itemIndexOnlyServer_t（P7 类型轴：CashShopModeVendingItem 第 2 参拼写）

class CUser;
class Inven_Item;
class ItemVendingMachine;

class Arad_IPGProcess
{
public:
    Arad_IPGProcess(ItemVendingMachine* vm) throw();   // ORIG W 0x0817a2ba（C1=C2 同址）
    ~Arad_IPGProcess();                        // ORIG W 0x0817a2c8（D1=D2 同址，空体；非虚）

    void CashShopModeCoin(CUser* user, unsigned short param2, unsigned int param3,
                          int param4, char buyFlag);                                    // 0x0819c434
    void CashShopModeItem(CUser* user, unsigned long itemIdx, unsigned short stack,
                          int count, long seq, std::vector<Inven_Item>& out,
                          WongWork::CCeraShop::stCeraShopItemParam_t& param,
                          int& restrictOut, unsigned int accId, int characNo,
                          char buyFlag);                                                // 0x0819c4a0
    void CashShopModeAvatar(CUser* user, unsigned long itemIdx, int a3, int a4,
                            char c1, char c2, char const* str, int& out,
                            unsigned int accId, int a10, char buyFlag);                 // 0x0819c2b4
    void CashShopModeCreature(CUser* user, unsigned long itemIdx, unsigned short stack,
                              int& outCodeA, int& outCodeB, unsigned int accId,
                              int characNo, char buyFlag);                              // 0x0819c670
    // [P7HeaderAxis 2026-09-11] 第 2 参拼写 itemIndexOnlyServer_t（ORIG prett 直证 size 0xe5
    // `void Arad_IPGProcess::CashShopModeVendingItem(CUser*, itemIndexOnlyServer_t, short unsigned int, …)`
    // vs 我方 0xe1；mangled 展开为 `m`=unsigned long ⇒ 同底层类型、零 ABI）。定义侧见 .cpp 聚合器（P6AuditOldN）。
    void CashShopModeVendingItem(CUser* user, itemIndexOnlyServer_t itemIdx, unsigned short stack,
                                 int count, long seq, std::vector<Inven_Item>& out,
                                 WongWork::CCeraShop::stCeraShopItemParam_t& param,
                                 int& restrictOut, int characNo, char buyFlag);         // 0x0819c862

private:
    ItemVendingMachine* m_vm;  // +0x00（ORIG ctor 唯一成员写入）
};

#endif  // DNF_GAME_ARAD_IPGPROCESS_H_
