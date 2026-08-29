// df_game_r 还原 —— Arad_MileageProcess（全局类；ORIG nm 实证）。
// ORIG 符号：
//   W 0x0817a2ce C1(ItemVendingMachine*)（ctor：*(this)=vm；无 vtable）
//   W 0x0817a2dc D1（空体；非虚析构）
//   T 0x0819e220 BuySuccess(CUser*, i, i, i, i, i, i, RSt6vector<Inven_Item>)
//   T 0x0819dc9c GiveMileage(CUser*, i, i)
//   T 0x0819dd4e BuyMileageItem(CUser*, i, m, t, i, l, i, c, c, i, i, i, PKc, RN..param_t)
//   T 0x0819e50e BuyFail(CUser*, s, i, i, i)
// 布局：+0x00 ItemVendingMachine*（同 Arad_IPGProcess）。
// 声明属主：HdrCreate1 生成；实现：Arad_IPGProcess.cpp（LinkE2.JpnExImpl 批次）。
#ifndef DNF_GAME_ARAD_MILEAGEPROCESS_H_
#define DNF_GAME_ARAD_MILEAGEPROCESS_H_

#include <vector>

#include "CCeraShop.h"  // WongWork::CCeraShop::stCeraShopItemParam_t 形参

class CUser;
class Inven_Item;
class ItemVendingMachine;

class Arad_MileageProcess
{
public:
    Arad_MileageProcess(ItemVendingMachine* vm);  // ORIG W 0x0817a2ce（C1=C2 同址）
    ~Arad_MileageProcess();                       // ORIG W 0x0817a2dc（空体；非虚）

    void BuySuccess(CUser* user, int a2, int a3, int a4, int a5, int a6, int a7,
                    std::vector<Inven_Item>& items);                             // 0x0819e220
    void GiveMileage(CUser* user, int ipgNo, int amount);                        // 0x0819dc9c
    int BuyMileageItem(CUser* user, int param2, unsigned long itemIdx,
                       unsigned short stack, int count, long seq, int param7,
                       char c1, char c2, int price, int a11, int a12,
                       char const* str,
                       WongWork::CCeraShop::stCeraShopItemParam_t& param);       // 0x0819dd4e
    void BuyFail(CUser* user, short errCode, int a3, int a4, int a5);            // 0x0819e50e

private:
    ItemVendingMachine* m_vm;  // +0x00（ORIG ctor 唯一成员写入）
};

#endif  // DNF_GAME_ARAD_MILEAGEPROCESS_H_
