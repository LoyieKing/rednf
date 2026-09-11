// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CUserPremium/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ CUserPremium 实现（独立 TU）。
// 声明以 CUser.h 为准；ORIG 字段已全部具名化（m_entries / m_additionalBase /
// m_additionalInfo 等，见 CUser.h 的 WongWork::CUserPremium），本 TU 不再使用
// 裸偏移访问（参照 docs/class_func_reports/WongWork__CUserPremium）。
// [SweepCUserPremium 2026-09-09] 本波终判汇总（check_tu_routed 现编现测）：
//   TU 四数 478(169+309)/near1/diff6 → 479(169+310)/near0/diff6（near→ae，
//   零回退）；dual_rank --tu CUserPremium.cpp 候选 4 件全部逐件核对。
//   真缺口修正（ORIG 直证）：
//   ① importPremiumList `[term]`/`[restrict]` 分支**内循环**：ORIG 0x08906e77/
//      0x08906ebc 成功写字段后 `jmp` 回分支头继续 ScanInt，失败才 `jmp 08906e3c`
//      回 ScanType（Ghidra C 段 `while(1){v=0;v=ScanInt(&ok);if(!ok)break;
//      写字段}` 同证）；旧单次形把「[term] v1 v2」拆成两次 ScanType，丢后续值。
//   ② importPremiumList 死局部/前置置零照抄（P171/P232/P198①/P212）：-0x58
//      （0x08906221）、-0x3c 与 cnt/itemId（0x089067f7..805）、slot/cnt/itemId
//      （0x089068cb..d9）；cur/i 声明提头、apply-date 的 v 提前声明 ⇒ 全帧槽
//      集合与 ORIG 逐槽相等（40/40，含 -0x6c/-0x68/-0x64/-0x60/-0x5c…-0x18）；
//      gap 14→2、fn_diff ndiff 827→592。
//   ③ GetReturnItemRate：P326 右值临时形 `it != const_iterator(map.end())`
//      ⇒ NEAR→AE（42/42、ndiff 0；旧 N93「两迭代器槽互换无源码解」作废）。
//   ④ 其余 DIFF 件复测维持 N 封顶（p167 全 CLEAR；jcc 逐位一致）：
//      _Additioal_info::add gap12 / subtract gap22（N-86b/N18a 寄存器舞；
//      subtract 0x4c/0x4e 加法特例已忠实保留）、LoadPremiumInfo gap-2
//      （N82/N95；补 key 具名局部实测帧 0x7e3c→0x7e4c、ndiff 312→407，已回退）、
//      setPremiumState gap1（N107/N7）、static_init（N123 噪音）。
//   全 TU 硬证据：ORIG 本 TU 581 函数 **0 处 `call *`**（虚槽判据 N/A）；
//   调用多重集差仅 static_init（N123）；EH/异常规格（_Unwind_Resume/__cxa_*）
//   零差异；jcc 批量比对 582 函数仅 importPremiumList 1 处 X 族布局差
//   （jne↔je，P168a/P292 封顶）。无 .h 改动。
#include "CUser.h"
#include "GlobalData.h"
#include "DNFLexWrapper.h"   // loadRDARScriptFile/ScanType/ScanInt/ScanStr（ORIG 真实符号）

#include <string>
#include <vector>

#include <map>
#include <string.h>
#include <time.h>


// ORIG 全局 g_map_premium_item_data（0x948b198 B，C 符号名）：map<unsigned long,
// STPremiumItemData>。由 SetPremiumItemData（0x86af70a）写入，
// respond_special_item_premium（CCeraShop_Special.cpp）查询。全局命名空间定义。
std::map<unsigned long, STPremiumItemData> g_map_premium_item_data;

// ===========================================================================
// PremiumList_New.etc 解析链（ORIG：importPremiumList 0x08905d95、
// stPremiumTable_t ctor/dtor 0x86b0898/0x86b0910、clean 0x891ae68、
// premium_script_parsing::STPremiumItemData ctor 0x891ae4c）。
// 类型布局逐偏移取自 ORIG 反汇编；全局容器 g_vecPremiumOverEquipableInfo /
// g_vecPremiumReturnItemInfo（B 0x0948b180/0x0948b18c，ReCalcAdvantage 按
// index 消费）。LoadPremiumInfo(0x86aeca4) 为其唯一工程内调用方。
// ===========================================================================
namespace premium_script_parsing
{

struct STPremiumItemData
{
    unsigned char m_termFlag;      // +0x00 [term] 值截断（[item] 块内）
    unsigned char m_restrictFlag;  // +0x01 [restrict] 值截断
    unsigned short m_pad02;        // +0x02 ctor 不清零（ORIG 仅写 b0/b1/key）
    unsigned int m_key;            // +0x04 [item] 首参（目标物品 idx）

    STPremiumItemData()            // ORIG W 0x891ae4c
    {
        m_termFlag = 0;
        m_restrictFlag = 0;
        m_key = 0;
    }
};  // sizeof 8

}  // namespace premium_script_parsing

// [b28d-solo-CUserPremium] P254：ReCalcAdvantage 0x086ae5xx 读 m_slots[j]
// 为两段 `mov 0x4(%eax,%edx,4),%eax; mov %eax,%ebx`（4.1 对 enum 字段保留
// eax→目标中转）；int 字段 4.4 恒折单段 ⇒ m_slots 声明为 enum（4 字节，
// 值域 0..0x15，布局/sizeof 不变）。
enum ENUM_OVER_EQUIP_SLOT
{
    OVER_EQUIP_SLOT_HAT_AVATAR = 0x00,
    OVER_EQUIP_SLOT_MAGIC_STONE = 0x15,
    OVER_EQUIP_SLOT_MAX = 0x16
};

struct SPremiumOverEquipableInfo
{
    int m_count;        // +0x00 组内标签数（ScanInt 失败前已写入）
    ENUM_OVER_EQUIP_SLOT m_slots[26];  // +0x04 标签分类值（[hat avatar]=0 .. [magic stone]=0x15）
};  // memset 0x6c；仅前 22 槽被写入
typedef char spremiumoverequipableinfo_size_0x6c
    [(sizeof(SPremiumOverEquipableInfo) == 0x6c) ? 1 : -1];

struct SPremiumReturnItemInfo
{
    int m_count;        // +0x00 组内 int 数
    int m_items[10];    // +0x04..+0x2b
};  // memset 0x2c
typedef char spremiumreturniteminfo_size_0x2c
    [(sizeof(SPremiumReturnItemInfo) == 0x2c) ? 1 : -1];

struct stPremiumTable_t
{
    struct Pair
    {
        int first;
        int second;
    };

    stPremiumTable_t();   // ORIG W 0x86b0898（仅三个含资源成员子构造）
    ~stPremiumTable_t();  // ORIG W 0x86b0910
    void clean();         // ORIG W 0x891ae68

    int m_kind;                        // +0x000 [attr]: [bonus]=1 / [term]=2
    int m_checkTerm;                   // +0x004 [term]
    int m_expireTime;                  // +0x008 [expire time]
    Pair m_advantageItems[5];          // +0x00c [items] (item,count)*n（无上界检查）
    unsigned short m_fatigueRate;      // +0x034 [fatigue]
    unsigned short m_pad036;
    int m_coin;                        // +0x038 [coin]
    int m_expRate;                     // +0x03c [exp]
    unsigned char m_unlimitFatigue;    // +0x040 [unlimit fatigue]（v==1）
    unsigned short m_overSkillLevel;   // +0x042 [over skill]
    unsigned short m_cerashopDiscountRate;  // +0x044 [cerashop discount rate]
    unsigned short m_pad046;
    int m_bonusExp;                    // +0x048 [bonus exp]（失败→整体 return 0）
    int m_questItemDropRate;           // +0x04c [quest item drop rate]（同上怪癖）
    std::vector<unsigned int> m_independentDropRates;  // +0x050
    int m_useTargetServer;             // +0x05c [apply target server]：self=1/all=0
    unsigned char m_goldBonus;         // +0x060 [gold bonus]：[true]=1/[false]=0
    unsigned char m_pad061[3];
    int m_overEquipCount;              // +0x064 [over equipinfo]，>5→0x1591
    Pair m_overEquipPairs[5];          // +0x068
    int m_returnItemCount;             // +0x090 [return iteminfo]，>3→0x15a6
    Pair m_returnItemPairs[3];         // +0x094
    int m_applyDayMask[7];             // +0x0ac [apply date]（clean 置 -1）
    int m_hpMax;                       // +0x0c8 [HP MAX]
    int m_mpMax;                       // +0x0cc [MP MAX]
    unsigned short m_physicalAttack;   // +0x0d0
    unsigned short m_physicalDefense;  // +0x0d2
    unsigned short m_magicalAttack;    // +0x0d4
    unsigned short m_magicalDefense;   // +0x0d6
    unsigned short m_fireResistance;   // +0x0d8
    unsigned short m_waterResistance;  // +0x0da
    unsigned short m_darkResistance;   // +0x0dc
    unsigned short m_lightResistance;  // +0x0de
    unsigned short m_rawE0[17];        // +0x0e0..+0x100 解析器不写，加载器照抄栈值
    unsigned short m_pad102;
    int m_inventoryLimit;              // +0x104 [inventory limit]
    unsigned short m_hpRegenSpeed;     // +0x108
    unsigned short m_mpRegenSpeed;     // +0x10a
    unsigned short m_moveSpeed;        // +0x10c
    unsigned short m_attackSpeed;      // +0x10e
    unsigned short m_castSpeed;        // +0x110
    unsigned short m_hitRecovery;      // +0x112
    unsigned short m_jumpPower;        // +0x114
    unsigned short m_pad116;
    int m_field118;                    // +0x118 解析器不写，加载器照抄栈值
    std::string m_name;                // +0x11c [name]
    unsigned int m_targetItemKey;      // +0x120 [target item]（失败→return 0 怪癖）
    std::vector<premium_script_parsing::STPremiumItemData> m_premiumItems;  // +0x124
};  // 总计 0x130
typedef char stpremiumtable_size_0x130[(sizeof(stPremiumTable_t) == 0x130) ? 1 : -1];

#include "CUserPremium/stPremiumTable_t.inc"

#include "CUserPremium/dtor_stPremiumTable_t.inc"

#include "CUserPremium/clean.inc"


// ORIG 全局容器（变量名不 mangle，符号与 B 0x0948b180/0x0948b18c 一致）
std::vector<SPremiumOverEquipableInfo> g_vecPremiumOverEquipableInfo;
std::vector<SPremiumReturnItemInfo> g_vecPremiumReturnItemInfo;

#include "CUserPremium/importPremiumList.inc"


namespace WongWork
{

#include "CUserPremium/getOverSkillLevel.inc"


#include "CUserPremium/GetOverEquipableLevel.inc"


#include "CUserPremium/GetAdvantageExpRate.inc"


#include "CUserPremium/GetAdvantageFatigueRate.inc"


#include "CUserPremium/GetAdvPremiumCount.inc"


#include "CUserPremium/GetTotalPremiumCount.inc"


#include "CUserPremium/InitPremium.inc"


#include "CUserPremium/AddPremium.inc"


#include "CUserPremium/RemovePremium.inc"


#include "CUserPremium/startPremium.inc"


#include "CUserPremium/ReCalcAdvantage.inc"


#include "CUserPremium/setPremiumState.inc"


#include "CUserPremium/_CheckApply.inc"


#include "CUserPremium/SetPremiumItemData.inc"


#include "CUserPremium/IsRestrictedPremium.inc"


#include "CUserPremium/RecalcAdditionalInfo.inc"

#include "CUserPremium/GetAdditionalInfo.inc"



#include "CUserPremium/CheckPremiumTimeout.inc"


#include "CUserPremium/GetPremiumInfo.inc"


#include "CUserPremium/GetReturnItemRate.inc"


#include "CUserPremium/CheckPremium.inc"


#include "CUserPremium/IsNeedCheckEveryday.inc"


#include "CUserPremium/GetAddExpBonus.inc"


#include "CUserPremium/GetAdvantageItem.inc"


#include "CUserPremium/LoadPremiumInfo.inc"


} // namespace WongWork

// ---- _Additioal_info 字段算术（ORIG df_game_r 逐指令转写；全局作用域类）----
// operator+=（ORIG W _ZN15_Additioal_infopLERKS_ @0x08693cea）：仅转发 add，
//   无引用返回（ORIG 函数尾未装载 this 到 eax）。
//   P210 定义前置：置于 add 定义之前，4.4 对未见面定义的 callee 用保守
//   outgoing 帧（复现 ORIG 4.1 帧 0x18 vs 继承 add 小帧的 0x8）。
void _Additioal_info::operator+=(const _Additioal_info& other)
{
    add(other);
}

#include "CUserPremium/add.inc"


#include "CUserPremium/subtract.inc"


#include "CUserPremium/GetAddIndependentDropRate.inc"
