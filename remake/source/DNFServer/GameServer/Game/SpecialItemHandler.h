#ifndef GAME_SPECIALITEMHANDLER_H
#define GAME_SPECIALITEMHANDLER_H

// ---------------------------------------------------------------------------
// df_game_r —— SpecialItemHandler 权威声明（唯一）。
// ORIG 全局类（mangled _ZN18SpecialItemHandler…，非 WongWork 前缀）。
// 归并自 GlobalData.cpp / CCeraShop_Special.cpp 的两处局部声明。
// 方法地址见 docs/class_func_reports/SpecialItemHandler/README.md。
//
// 布局（ctor 0x0827bcec 转写）：
//   +0x00  WongWork::CCeraShop* m_owner
//   +0x04  14 组 {respond 函数指针, int offset} 的派发表（execute 0x0827be0a 消费）
//   sizeof = 4 + 14*8 = 0x74
// 被 WongWork::CCeraShop 以 +0x00 指针持有（m_handler）；GlobalData.cpp 的
// CCeraShop ctor new SpecialItemHandler(this) 触发本 ctor 填表。
// ---------------------------------------------------------------------------

#include <cstddef>

class STSpecailItem_Param;  // 完整定义在 CCeraShop.h（ORIG W ctor 0x08234ec2）
class STSpecailItem_Result;

namespace WongWork
{
class CCeraShop;
}

class SpecialItemHandler
{
public:
    SpecialItemHandler(WongWork::CCeraShop* owner);  // 0x0827bcec
    ~SpecialItemHandler() {}   // ORIG D1/D2 W 0x08328284：空体（push/pop/ret）
    bool execute(int type, const STSpecailItem_Param& param,
                 STSpecailItem_Result& result);      // 0x0827be0a

private:
    // respond 子处理器（dispatch 表目标；各自 ORIG 地址见 README）。
    void respond_special_item_cargo(int type, const STSpecailItem_Param& param,
                                    STSpecailItem_Result& result);                       // 0x0827be8a
    void respond_special_item_cargo_abs(int type, const STSpecailItem_Param& param,
                                        STSpecailItem_Result& result);                    // 0x0827bee6
    void respond_special_item_inventory_upgrade(int type, const STSpecailItem_Param& param,
                                                STSpecailItem_Result& result);            // 0x0827bf90
    void respond_special_item_charac_slot_extend(int type, const STSpecailItem_Param& param,
                                                 STSpecailItem_Result& result);           // 0x0827c0c2
    void respond_special_item_package(int type, const STSpecailItem_Param& param,
                                      STSpecailItem_Result& result);                     // 0x0827c204
    void respond_special_item_booster(int type, const STSpecailItem_Param& param,
                                      STSpecailItem_Result& result);                     // 0x0827ca4a
    void respond_special_item_booster_selection(int type, const STSpecailItem_Param& param,
                                                STSpecailItem_Result& result);            // 0x0827ccfc
    void respond_special_item_gift_ticket(int type, const STSpecailItem_Param& param,
                                          STSpecailItem_Result& result);                 // 0x0827cdb6
    void respond_special_item_skill_confirm(int type, const STSpecailItem_Param& param,
                                            STSpecailItem_Result& result);               // 0x0827cdd0
    void respond_special_item_skill_tree_style(int type, const STSpecailItem_Param& param,
                                               STSpecailItem_Result& result);            // 0x0827ce7e
    void respond_special_item_premium(int type, const STSpecailItem_Param& param,
                                      STSpecailItem_Result& result);                     // 0x0827d3dc
    void respond_special_item_cerapoint_giftcard(int type, const STSpecailItem_Param& param,
                                                 STSpecailItem_Result& result);          // 0x0827de4e

    // ORIG fn 为普通函数指针（4 字节，ctor 直接存 respond_* 地址；成员指针 8 字节
    // 会使 Entry=12、sizeof=0xac，与 ORIG 0x74 不符）→ 用显式 this 的自由函数形态。
    typedef void (*RespondFn)(SpecialItemHandler* self, int type,
                              const STSpecailItem_Param& param,
                              STSpecailItem_Result& result);
    struct Entry
    {
        RespondFn fn;      // +0
        int offset;        // +4
    };
    WongWork::CCeraShop* m_owner;  // +0x00
    Entry m_entries[14];           // +0x04（14 * 8 = 0x70）
};

#endif  // GAME_SPECIALITEMHANDLER_H
