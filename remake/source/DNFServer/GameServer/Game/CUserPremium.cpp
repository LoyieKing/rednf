// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CUserPremium/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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
}

// ORIG 全局容器（变量名不 mangle，符号与 B 0x0948b180/0x0948b18c 一致）
std::vector<SPremiumOverEquipableInfo> g_vecPremiumOverEquipableInfo;
std::vector<SPremiumReturnItemInfo> g_vecPremiumReturnItemInfo;

// [b28d-solo-CUserPremium] 终判（b28d 复测，routed 口径）：P167 CLEAR（145/145），
// 语义完成。本波按 ORIG 直证改写的源形（全部经 clean/AE ndiff 前后对比）：
//  ①[target item]/[item] 两分支各自的局部 bool（ORIG -0x7a/-0x7b 两独立槽，
//    旧函数头单例 itemOk 只产一槽）——P0；
//  ②[target item] 补 `int v = 0; v = ScanInt(...)` 两句式（ORIG -0x14 预清）；
//  ③[independent drop rate] `for (int rate = 0; ; rate = 0)` + `push_back(rate)`
//    （ORIG -0x40 int 槽 + -0x70 转换临时 ⇒ int→unsigned int 右值选中 EOj）；
//  ④[items]/[item]-inner 按 ORIG 分支头三连置零/两句式声明改写（P198①）；
//  ⑤equipCase 去初始化、cur 移到 clean 循环后声明、补 ORIG -0x6c 死局部（P171/P232）。
// 残差（ndiff 923→712，本文 TU 口径）全为 4.1↔4.4 栈槽分配序族：ORIG 头段
// token/value/ok 在 -0x74/-0x78/-0x79 而 4.4 给 -0x6c/-0x70/-0x71（全帧 8 字节
// 平移）、cur/equipCase/i 相对序相反，以及各 `cur->m_X = ScanInt()` 点的值/地址
// 寄存器角色互换（N7）⇒ N93/N98/N82 封顶，语义与调用点差集均已闭合。
#include "CUserPremium/importPremiumList.inc"
}

namespace WongWork
{

// ---------------------------------------------------------------------------
// g_SPremiumInfo 权威元素类型见 SPremiumInfo.h（stride 0x120、0x6a 元素）；
// 本 TU 经 g_SPremiumInfo[type] 具名字段访问。
// importPremiumList 终判（b28d-solo-CUserPremium）：P167 CLEAR（145/145）
// + 上列 gap ⇒ 语义完成；残余全为帧-槽族 N82/N95/P212（见函数头判词）。
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// CUserPremium 字段（ORIG 偏移 → CUser.h 具名成员）
//   0x000 m_entries[0x6a]  0x848 m_normalCount  0x84c m_restrictedCount
//   0x850 outFatigueRate  0x854 outPremiumCount  0x858 outExpRate
//   0x85c overEquipFlag   0x85e overSkillLevel   0x860 field860   0x862 field862
//   0x864 m_overEquipableLevel[0x58]  0x8bc m_field8bc   0x8bd m_additionalBase   0x90f m_additionalInfo
//   0x964 m_returnItemMap（ORIG 为 std::map<int,int>；真实成员，C1/D1 自动管理）
// ---------------------------------------------------------------------------
#include "CUserPremium/getOverSkillLevel.inc"
}

#include "CUserPremium/GetOverEquipableLevel.inc"
}

#include "CUserPremium/GetAdvantageExpRate.inc"
}

#include "CUserPremium/GetAdvantageFatigueRate.inc"
}

#include "CUserPremium/GetAdvPremiumCount.inc"
}

// ORIG W 0x0863bd5c：*(int*)(this+0x848) + *(int*)(this+0x84c)
#include "CUserPremium/GetTotalPremiumCount.inc"
}

#include "CUserPremium/InitPremium.inc"
}

#include "CUserPremium/AddPremium.inc"
}

// [b28d-solo-CUserPremium] P309 + 早返尾形（b28d 复测）：ORIG 0x086adba6 两处
// std::max 临时槽对为四个独立槽（if 臂 dec/zero=-0x18/-0x14、else 臂
// dec/zero=-0x10/-0xc）⇒ 四标量提函数头且声明序 zero2(-0xc)/dec2(-0x10)/
// zero1(-0x14)/dec1(-0x18)，4.4「头部声明先声明浅」精确复现四槽。
// 尾形：ORIG 两臂各自 `jmp <leave>`（0x86adc2f/0x86adc64）+ je 落 nop 岛
// （0x86adc66）⇒ 源形必须是「if 臂 return + else 臂落体外 return」两处裸早返
// （探针 /tmp/b28d_solo/p3.o f4：`if(kind==1){…;return;} …;return;` 产
// `jne 臂2; 臂1; jmp exit; 臂2; jmp exit; nop; exit` 与 ORIG 逐条同形）；
// if/else 形 4.4 令末臂直落（缺第二 jmp）⇒ 已由本形取代。
// 终判（b28d-solo-CUserPremium，routed 口径）：IDENTICAL 59/59（ndiff 0）、
// P167 CLEAR（2/2）。
#include "CUserPremium/RemovePremium.inc"
}

#include "CUserPremium/startPremium.inc"
}

// [b28d-solo-CUserPremium] 修法（P167/P0/P211/P214①/P254）：
// ①插入路径改 make_pair<int&,int&> + pair<const int,int> 右值转换构造、
//   else 用 `it->second +=`（ORIG 单次 ptEv）⇒ 调用点差集清零（23/23 CLEAR）；
// ②overEquip body 的 lhs 索引 + max 第一实参各重算 vec[]（ORIG 3 调用点）；
// ③去 int now 中转（nowT 单槽）；④+0x85e/+0x850 由 Main 代落 CUser.h 两字段
//   short→unsigned short（ORIG 0x0812cdb4/0x0860939e 两处 movzwl 证据；探针
//   /tmp/p_u16b.cpp f6 vs f7/f8：目标 short ⇒ `movzwl→%eax; mov→%edx` 两段，
//   目标 unsigned short ⇒ `movzwl→%edx` 单段）；⑤m_slots enum 化复现两段
//   中转（P254）。
// 终判（b28d 复测）：IDENTICAL 482/482（ndiff 0，gap 0，槽位集全等），
// P167 CLEAR（23/23）——旧判词「余 3 处 N18a/N-86b 封顶」已由④推翻。
#include "CUserPremium/ReCalcAdvantage.inc"
}

// [b28d-solo-CUserPremium] N107/N7 封顶（b28d 复测；旧注释地址 0x0830ed48
// 系误录，权威地址 nm：0x086b084c）。ORIG 14 条：前两条 `mov 0xc(%ebp),%edx;
// mov 0x8(%ebp),%ecx`（type 先载/this 后载），尾段 `lea (%ecx,%eax,1),%eax;
// lea 0x10(%eax),%edx; mov 0x10(%ebp),%eax; mov %eax,(%edx)`。
// 指针算术形（下方实现）13 条：4.4 折 `lea (%ecx,%eax,1),%edx` + disp store
// `mov %eax,0x10(%edx)`，且 this 先载。
// 本波穷举读数：①`m_entries[(int)type]` 下标形 16 条（push %ebx）；
// ②`*(&m_entries[(int)type].m_state)` 取址形 16 条（push %ebx + add $0x10）；
// ③`((int*)(m_entries+(int)type))[4]` 整型数组视图 15 条（add $0x10 + this
// 中转）——均劣于指针算术形。两段 lea（N107：4.4 对成员常量偏移恒折
// disp/add）+ 形参装载序（4.4 this-first vs 4.1 type-first）双封顶；
// P167 CLEAR、gap 1/14 ⇒ 语义等价。
// 本波复测：clean/AE ndiff=7、14 vs 13（唯一缺口 = ORIG `lea 0x10(%eax),%edx;
// mov %eax,(%edx)` 两段取址 vs 4.4 折 disp），N107 六形探针结论不变 ⇒ 维持封顶。
#include "CUserPremium/setPremiumState.inc"
}

// [b28d-solo-CUserPremium] P225 修法：ORIG 0x086ae834 的 mask[0]==-1 分支是
// `mov $1,%eax; jmp end`（return true 直返，非 ok=true 落槽）；循环为
// while+break 形（体首 mask==-1 je 出口、尾测 `cmpl $6; setle; test; jne`），
// 旧 for&& 形使 4.4 折成 `jg` 直跳（缺 setle 物化）。
#include "CUserPremium/_CheckApply.inc"
}

// [b28d-solo-CUserPremium] IDENTICAL/AE（routed 口径 check_tu_routed 复验）：
// ORIG 0x086af70a 逐条对齐（52/52，ndiff=0）。P167 唯一差 = make_pair 模板
// 实参 IRKm vs IRm ⇒ 定义处形参加 top-level const（P315：const 左值实参令
// make_pair 推导 IRKm；top-level const 不入函数类型/mangled，声明无需同步）。
// 终判（b28d-solo-CUserPremium）：IDENTICAL 52/52、P167 CLEAR（6/6）。
#include "CUserPremium/SetPremiumItemData.inc"
}

#include "CUserPremium/IsRestrictedPremium.inc"
}

// [b28d-solo-CUserPremium] 修法（P235/P211）：ORIG 0x086ae8c6 读侧单段
// `mov (%eax),%edx; shr $0x5,%edx`；改 unsigned int 同型视图
// （m_maxHP/m_maxMP/m_moveSpeed）直读后 315=315 ndiff=0（旧 int 视图 +
// static_cast 使 4.4 多发 `mov %eax,%edx`，3 处 ×7 条）。P167 CLEAR（3/3）。
#include "CUserPremium/RecalcAdditionalInfo.inc"
}
// ORIG W 0x8694a88 GetAdditionalInfo() const：sret ← 隐式拷贝 ctor 的
// 内联块移（对齐检查 + rep movsl + word/byte 尾，0x52 字节，无 clear 调用
// ⇒ ORIG 源即 return 成员表达式，非 局部+memcpy——后者多 out ctor 与 call）。
#include "CUserPremium/GetAdditionalInfo.inc"
}


#include "CUserPremium/CheckPremiumTimeout.inc"
}

#include "CUserPremium/GetPremiumInfo.inc"
}

// [b28d-solo-CUserPremium] N93 封顶（b28d 复测）：ORIG 0x086ae7a8；find 结果槽
// -0x10 / end -0xc 与 4.4 相反（同块两直接初始化迭代器，4.1 声明序低→高、
// 4.4 相反）。交换声明序 ⇒ 调用序反；提头声明 ⇒ 默认 C1+sret 拷贝（N86）；
// P293 单表达式形 ⇒ end 先于 find（ORIG 调用序反）。P167 CLEAR（5/5）、
// 42=42 gap0 ⇒ 语义等价，封 N93。
// 本波复测：clean/AE ndiff=10（唯二残差 = find/end 两迭代器槽互换，
// ORIG -0x10/-0xc vs 4.4 -0xc/-0x10；交换声明序会反转调用序，探针族已证）
// ⇒ N93/N98 维持。
#include "CUserPremium/GetReturnItemRate.inc"
}

#include "CUserPremium/CheckPremium.inc"
}

// ORIG W 0x086b086c：返回 +0x862 字节（任一 premium 定义需每日检查时为 1）。
#include "CUserPremium/IsNeedCheckEveryday.inc"
}

// ---------------------------------------------------------------------------
// ORIG W 0x085bf940 _ZNK8WongWork12CUserPremium14GetAddExpBonusEi：
// type∈[0,0x69] 时返回 g_SPremiumInfo[type].m_field44（表基 0x09483a84，
// 步长 0x120），否则 0。逐指令转写。
// ---------------------------------------------------------------------------
#include "CUserPremium/GetAddExpBonus.inc"
}

// [b28d-solo-CUserPremium] 修法（P225）：ORIG 0x086adf7c 循环为「尾测 setle
// 物化 + 体首 first==0 出口」形；for&& 合并条件使 4.4 折 jg/bool 物化，
// 改 while+break 后 132=132 ndiff=0。P167 CLEAR（7/7）。
#include "CUserPremium/GetAdvantageItem.inc"
}

// ---------------------------------------------------------------------------
// 终判（b28d-solo-CUserPremium，b28d 复测）：P167 CLEAR（20/20），语义完成。
// 本波按 ORIG 直证改写（clean/AE ndiff 466→56）：
//  ①`size() != 0` 正向判 + 非空分支直落/空分支置后（ORIG 0x86af571
//    `test;setne;test;je <空分支>`；旧 `==0` 产 sete 且块序相反）；
//  ②for 初始化处直构 it/itEnd 并缓存 end()（ORIG 0x86af5a2/0x86af5ce 各一次、
//    it=-0x44/end=-0x48；旧形每轮重调 end() 且槽错位）；
//  ③两处 SetPremiumItemData 调用点改 bool 裸 `!`（ORIG 0x86af61d/0x86af679
//    `call;xor $1;test %al;je`；返回类型经 Main 代落 CUser.h int→bool，T3 直证）；
//  ④外层循环变量 i 提函数头（P212，复现 ORIG i=-0x34）。
// 残差 56 行 = ORIG `sub $0x7d50/$0x7d10 + disp 0x10` vs 4.4 `sub $0x7d40/
// $0x7d00 + disp 0` 的常量折叠基准差（N1/N4 族，等效地址）+ 键值局部取舍：
// ORIG 0x86af54c 确有 key 栈槽往返（P0），但 4.4 为它分配栈槽会触发整帧
// 0x7e3c→0x7e4c / table 基址 -0x7e28→-0x7e2c 平移（探针 /tmp/b28d_solo：
// 带槽 466、内联实参 56、纯 register 52）⇒ 取内联实参形（语义一致，仅少
// store/load 对）⇒ N82/N95/N1 封顶。
// ORIG T 0x086aeca4 WongWork::CUserPremium::LoadPremiumInfo(char const*)。
// importPremiumList 失败→0x15c；逐记录拷贝进 g_SPremiumInfo[i]（0x130→0x120
// 字段重排，+0xc8 起写穿 _Additioal_info 子对象区）；SetPremiumItemData
// 失败：非空 vector 分支→0x1bc、空分支（key=m_targetItemKey，flag 取
// m_checkTerm 低字节）→0x1c4。全部成功返回 0（CDataManager::init 以
// "LoadPremiumInfo Fail(Line: %d)" 打印该行码）。
// ---------------------------------------------------------------------------
// [b28d-solo-CUserPremium] 修法（P167/P96/P50）：ORIG 0x086aeca4 调非 const
// 迭代器族（ptEv/ppEv/ne(N,N)）且无 `iterator::C1Ev`（P167 EXTRA×1）⇒ 非
// const iterator 且 for 初始化处直构；改后差集清零（20/20 CLEAR）。
// 终判（b28d）：同上——删 key 局部后 gap -3/758；残余帧差与槽基差（N82/N95/
// P232）封顶。
#include "CUserPremium/LoadPremiumInfo.inc"
}

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

// add（ORIG W _ZN15_Additioal_info3addERKS_ @0x08146300）：int@+0x00/+0x04 32 位加；
//   u16@+0x08..+0x0e 显式加；循环 i=0..3 加 u16[i] @ (i+8)*2（即 m_field10[4]）；
//   循环 j=0..0x10 加 u16[j] @ 0x18+2j（即 m_field18[17]）；
//   收尾依次 +0x3a(int)、+0x3e、+0x40、+0x42(int)、+0x46、+0x48、+0x4a、+0x4c、+0x4e(int)。
//   u16 均为零扩展后 mod 2^16 运算（movzwl/add/mov），unsigned short 成员天然良构。
// [b21 N-86b] 语义已闭合：ORIG(4.1) 对 16 位和用 `lea (%edx,%eax,1)`+`mov %eax,%edx`
//   寄存器舞（0x3e/0x40/0x4a/0x4c 四 union 字段），4.4 恒发 `add %eax,%edx` 直形——
//   同算术收形源码不可择（N-86b probe 实证恒内存直形）；循环体寄存器分配同理。
//   逐字段语义逐一相等，封 N。
// [b28d-solo-CUserPremium] N-86b/N7 封顶（b28d 复测）：ORIG 0x08146300 对
// 16 位加发 `movzwl; mov %eax,%edx; movzwl; lea (%edx,%eax,1); mov %eax,%edx`
// 寄存器舞 + %edx 索引中转；4.4 恒折 `movzwl/movzwl/add/mov %dx` 直形且用
// %ecx 索引（探针 /tmp/probe_add.cpp 六形 + probe_add2.cpp 五形全败）。
// P167 CLEAR（0/0）、逐字段语义相等 ⇒ 封顶。
// 本波复测（探针 /tmp/b28d_solo/p1.o c1..c8 + p2.o v1..v5，共 13 形：
// `x+=y` / `x=x+y` / 显式 (unsigned short)/(int)/(unsigned) cast / 数组元素
// / 具名临时 / bitfield 位置）——4.4 恒 `movzx edx; movzx eax; add edx,eax`，
// ORIG 的 `movzx eax; mov edx,eax; …; lea eax,[edx+eax]; mov edx,eax` 舞
// 无任何源形可择 ⇒ N-86b 维持；clean/AE ndiff 116→52（gap 12 条全为四 union
// 字段 + 两循环体的收形差），语义逐字段相等。
#include "CUserPremium/add.inc"
}

// subtract（ORIG W _ZN15_Additioal_info8subtractERKS_ @0x08693d04）：与 add 同构、
//   逐字段相减——唯 ORIG 对 m_field4c 与 m_field4e 两字段仍执行加法
//   （lea (%edx,%eax,1) 与 add %eax,%edx 指令实证），忠实保留该行为。
// [b21 N-86b] 语义已闭合：与 add 同族——ORIG(4.1) 16 位差经
//   `mov %eax,%edx`+`lea/sub` 寄存器中途（含 mov %ebx,%eax 回转），4.4 恒发直接
//   `sub %ax,%cx`/`add %eax,%edx` 形；0x4c/0x4e 加法特例已忠实保留。逐字段语义
//   逐一相等，封 N。
// [b28d-solo-CUserPremium] N-86b/N7 封顶（b28d 复测）：同 add——ORIG 16 位差
// 经 lea/mov 寄存器舞 + %edx 索引中转，4.4 恒直接 sub/add + %ecx；0x4c/0x4e
// 两字段加法特例已忠实保留。P167 CLEAR（0/0）⇒ 语义等价，封顶。
// 本波复测：同 add 探针族（13 形）不可择；clean/AE ndiff 121→62，语义等价。
#include "CUserPremium/subtract.inc"
}

// ORIG W 0x0830ed48 _ZN8WongWork12CUserPremium25GetAddIndependentDropRateEij
// 二参（unsigned，1..4）选 m_independentDropRates 列；一参（int，0..0x69）选
// g_SPremiumInfo 行；越界返回 0。
// [b28d-solo-CUserPremium] P133/P258 修法：ORIG 0x0830ed48 的两组越界检查是
// **短路或**形（`cmp 0; je bad; cmp 4; jbe ok` 与 `cmp 0; js bad; cmp 0x69;
// jle ok`，两失败块各自独立 mov$0）；旧独立早返形使 4.4 对 level<0 产 jns 反跳。
#include "CUserPremium/GetAddIndependentDropRate.inc"
}