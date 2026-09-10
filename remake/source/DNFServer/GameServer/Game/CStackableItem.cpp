// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CStackableItem/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include "GameRand.h"
// df_game_r CStackableItem 还原（I1 批次，2026-08-17）。
// 函数形态以 docs/class_func_reports/CStackableItem.md + ORIG weak 反汇编为准
// （AE 口径：调用目标归一；字段偏移/常量逐条一致）。
// 依赖的辅助类（CMTRand/CSystemTime/CDataManager 等）由 GameStubs.cpp 提供桩；
// STStackableScript 内联完整布局为 I1 工作布局（脚本表批次细化）。
//
// [b28d-solo S422_CStackableItem 收口判词 2026-09-09] 本 TU 9 件终态：
//  · identical/AE：get_multibox_default_item（stDefaultItem::m_prob unsigned 化，
//    AE 153=153，P321-S1 一次旋钮；原 N18b 判词作废）。
// [b28d-poolcera-PoolCera 2026-09-09 追加修复] createStackableMultiBoxLotteryInfo
//   p167 EXTRA（D1×2+_Unwind_Resume×1）根因翻案 ⇒ 真修复非 N91：ORIG 0x085219d1
//   实例化 `_Destroy_aux<true>::__destroy<stDefaultItem*>`（trivial 析构实证）而
//   LotteryUnit 为 aux<false> ⇒ ORIG stDefaultItem **无用户声明析构**；我方
//   MultiBoxLotteryInfo.h `~stDefaultItem(){}` 为 user-provided 非平凡形 ⇒ 循环体
//   局部 item 发射 D1 岛 + push_back landing pad。删声明+定义（Main 批准，
//   同 LotteryDefaultArad 先例 P12/P71）后 p167 106/106 全 CLEAR、668→649 行
//   （ORIG 652，gap 3）、W D1 仍以 size=5 平凡形自然发射（ORIG 0x089d48b2 同寸）。
//   剩余 = N95/N115 族槽号整体平移 + mn 差 {mov×3, je×1}vs{jne×1}，判 DIFF(N)。
//  · N 封顶（p167 CLEAR 或结构性跨版本 codegen）：
//    - set_item：p167 CLEAR 84/84；N115 帧 0x26c vs 0x27c、槽深 4、ORIG `lea -0x64,%esi`
//      地址缓存 vs 我们直用 %edx（P183/P226 族）。
//    - createRecipeInfo：[b28d-PoolSmall **P326 命中归零**] 旧「N93/N95 it-end 槽方向
//      无源码解」判词作废：第二个迭代器**不具名**、把右值临时内联进比较
//      （`if (it == source->end())`）⇒ it 落深槽 -0x48、begin 返回值临时落 -0x44，
//      与 ORIG 4.1 逐槽全等 ⇒ 227=227 **NDIFF 0**、p167 33=33 CLEAR，
//      TU near 4→3 / diff 7→6。
//    - get_limit_cube_item：N95；range 槽 ORIG -0x38 vs 我们 -0x44（取地址具名标量
//      在 4.4 恒落临时区之后）。
//    - check_change_job_type：N115 帧 0x10 vs 0x28 + 2 char 参数槽平移。
//    - GetCoolTimeOption/GetEffectItemOption：N7 寄存器角色（4.1 值进 %ecx 保留
//      %edx=this 供第二读，4.4 覆盖 %edx 后重载 0xc(%ebp)）；P230 记载 4.4.6 后端可修。
//    - C1Ev：布局建模（AradItemInfo 须为次基类），Main 裁决本轮不做，spec 存档。
//  TU 三数（fresh）：identical+ae 1327 / near 4 / diff 7（起点 1326 / 4 / 8）。
//  [b28d-poolcera] stDefaultItem 平凡化 + framePad 删 + 形B 后（fresh）：
//  identical+ae 1331 / near 3 / diff 6（_M_insert_aux DIFF→identical、
//  new_allocator destroy NEAR→AE、MultiBox ndiff 359→15 仍 DIFF）。
//  [b28d-solo-CStackableItem 2026-09-09 二轮] createStackableMultiBoxLotteryInfo
//  **gap 3→0**（fresh 652=652）：stDefaultItemInfo 三字段 int→unsigned int
//  （MultiBoxLotteryInfo.h，Main 批准 + all 广播，同宽 4B ⇒ sizeof/偏移/mangled
//  零变化）。四数零回退：identical+ae 1331 / near 3 / diff 6 / missing 271
//  （MultiBoxLotteryInfo.cpp 113/0/1、GmCmds_F.cpp 5/18/1/1 亦零回退）。
//  余 1 处 `!eq` xor 物化 vs ORIG je+jmp（跨版本 lowering，N）——详见函数头判词。

#include "CStackableItem.h"

#include "PvPTypes.h"   // 权威 CMTRand 完整类型（new CMTRand(seed) 需要；原局部 stub 已删除）
#include "AradRabbitBox.h"  // +0x19c 兔盒元素类型（W6_ARAD_ItemBonus 收敛）

#include <algorithm>
#include <string>
#include <string.h>
#include <time.h>

#include "../../ServerCommon/DNFFunctionLib.h"


// 最小构造/析构（ORIG 逐成员构造属后续精修批次；保证可链接）
// b28c-L8 判词 [N(布局建模)]：fresh 复验 _ZN14CStackableItemC1Ev 292=292，唯一残差
// 4 行 = 本类 vptr 写入时机。ORIG 0x08515763 构造序 = `call _ZN5CItemC1Ev`(0x85152d4)
// → `add $0x148; call _ZN12AradItemInfoC1Ev`(0x8515618) → **然后**才
// `movl $0x8c96ea8,(%eax)` 写本类 vptr → RecipeInfo(+0x174) / LotteryInfo(+0x1c0) / …
// ⇒ AradItemInfo 必为**次基类**而非成员（CItem sizeof=0x148 权威 ⇒ 次基类恰落 +0x148，
// 全部偏移/sizeof 不变；AradItemInfo 无虚函数 ⇒ 不引入第二 vptr）。成员模型下 4.4
// 恒在首成员 ctor 之前写 vptr，源码不可翻。同法先例：C28L9 AvatarRechargeServer.h、
// C28L16 AvatarFixedHiddenOptionServer.h。本波不改 CStackableItem.h（跨
// C28L17/18/21 三方写权 CItemList.cpp + Arad_ItemBonus.cpp 消费面），spec 已广播交
// Main 统一批次；改动清单=头继承表 + CStackableItem.cpp 12 处 + Arad_ItemBonus.cpp:431
// + CItemList.cpp:1986 去 `m_arad148.` 前缀。
// [b28d-N(布局建模)｜待专项批次] fresh 复验 292=292 维持 N：唯一残差 = 本类 vptr
// 写入时机（AradItemInfo 须为次基类，跨 CItemList.cpp/Arad_ItemBonus.cpp 消费面）。
// Main 2026-09-09 裁决：**本轮不做**（收益仅本件 4 行残差，代价为全域构造/析构语义
// 变更 + 14 处消费点 + 多基类 RTTI/EH 风险），spec 存档待专项批次：
//   头改 `class CStackableItem : public CItem, public AradItemInfo`，删 m_arad148 成员，
//   去 CStackableItem.cpp 12 处 `m_arad148.` 前缀 + CItemList.cpp:1986 + Arad_ItemBonus.cpp:431；
//   CItem sizeof=0x148 权威 ⇒ 全部偏移/sizeof 不变、AradItemInfo 无虚函数 ⇒ 不引入第二 vptr。
#include "CStackableItem/CStackableItem.inc"
}

#include "CStackableItem/dtor_CStackableItem.inc"
}

// ORIG T 0x089c2832：if-return 链 + strcmp（每分支 `mov $N,%eax; jmp` 统一出口，
// 尾项默认 `mov $0x9,%eax` 直落；P17/P47 形态）。字符串映射（.rodata 0x8e19e71..）：
//   "[avatar]"→0 "[special avatar]"→1 "[cera]"→2 "[creature]"→3 "[equipment]"→4
//   "[stackable]"→5 "[etc]"→6 "[special creature]"→7 "[emblem]"→8，未知→9。
#include "CStackableItem/tag2Type.inc"
}

// ORIG T 0x089c295a：同 tag2Type 形态；结束标签 "[/xxx]" 映射同值表
// （.rodata 0x8e19ec6..）："[/avatar]"→0 .. "[/emblem]"→8，未知→9。
#include "CStackableItem/tag2EndType.inc"
}

// ORIG 0x8515128：逐字段复制（3 int + vector<string> + char）
BoosterSelectInfo& BoosterSelectInfo::operator=(
    const BoosterSelectInfo& other)
{
    m_field0 = other.m_field0;
    m_field4 = other.m_field4;
    m_field8 = other.m_field8;
    m_nameVec = other.m_nameVec;
    m_field18 = other.m_field18;
    return *this;
}

// ORIG 0x8513b82：vector<string> 默认构造 + 三 int movl 清零 + char movb 清零
// （P10/P33：m_field18 必须 char）。
#include "CStackableItem/BoosterSelectInfo.inc"
}

#include "CStackableItem/get_rand_int.inc"

enum ENUM_ITEM_CREATE_TYPE
{
    ENUM_ITEM_CREATE_TYPE_0 = 0,
    ENUM_ITEM_CREATE_TYPE_1 = 1
};

class CItemList
{
public:
    void create_item(ENUM_ITEM_CREATE_TYPE type, Inven_Item& item, int param);
};

class CEnvironment;
extern CEnvironment* G_CEnvironment();

struct CEnvironmentView
{
    char m_pad[0x1a8];
    int m_serverType;   // +0x1a8
};

class CDataManager
{
public:
    void set_AuctionRegFreeCeraItem(unsigned int idx);
    void set_lottery_use_cost(unsigned int cost, unsigned int flag);
    char m_pad[0xc];
    class CItemList* m_itemList;   // +0x0c
};
extern CDataManager* G_CDataManager();

struct CDataManagerView
{
    char m_pad[0x28];
    int m_field28;   // +0x28
};

#include "CSystemTime.h"

#include "ItemGloballyUniqueIdentifierGenerator.h"  // itemGloballyUniqueIdentifier_t 权威定义


class GameWorld;
extern GameWorld* G_GameWorld();

class GameWorld
{
public:
    bool IsPVPChannel() const;
};

class CUserCharacInfo;
class CUser
{
public:
    int get_state();
    CUserCharacInfo* get_charac_info();
};

class CUserCharacInfo
{
public:
    char getCurCharFirstGrowType() const;
};

// ORIG 0x8a86d28：26 项字符串→枚举映射，默认 0x16
#include "CStackableItem/GetEquipmentTypeStringToEnum.inc"
}

// ===================== StackableAction 字符串→枚举映射（ORIG T） =====================
// getStackableActionType @0x089aeb9b：14 项字符串→Type 映射，默认 0（if-return 链，
// 每项 call eq/test/je/mov+jmp 统一出口，与 ORIG 同形）。
#include "CStackableItem/getStackableActionType.inc"
}

// getStackableActionUsablePlace @0x089aed67：5 项字符串→UsablePlace，默认 1
#include "CStackableItem/getStackableActionUsablePlace.inc"
}

// getStackableActionUsableServerType @0x089aee0a：3 项字符串→UsableServerType，默认 1
#include "CStackableItem/getStackableActionUsableServerType.inc"
}

#include "CStackableItem/createRecipeInfo.inc"
#include "CStackableItem/createLotteryInfo.inc"
#include "CStackableItem/createStackableLotteryInfo.inc"
#include "CStackableItem/createStackableMultiBoxLotteryInfo.inc"
#include "CStackableItem/createGlobalEffectInfo.inc"
#include "CStackableItem/createRecipeInfo_1.inc"
}
// ============================================================================
// LotteryInfo 生命周期 + 解析辅助（自 GameStubs.cpp 迁移，保持 ORIG 语义）：
//   LotteryInfo C1/D1/clear（ORIG 0x83706d4 / 0x837069c）
//   createLotteryInfo（ORIG 0x89babc5）
//   createStackableMultiBoxLotteryInfo（ORIG 0x89bafd1；已实现，见下）
// ============================================================================
// b28c-L8 代落 C28L25（lane25 件）：ORIG C1 0x083706d4 仅构造两 vector 成员后
// call clear()，无 +0x00/+0x10 标量清零 ⇒ 标量不入初始化列表（清零由 clear 承担）。
#include "CStackableItem/LotteryInfo.inc"
}
#include "CStackableItem/dtor_LotteryInfo.inc"
#include "CStackableItem/clear.inc"
}
// ORIG 0x89babc5：src 相邻元素配对填充 m_items，m_counts 每对补 1；奇数时 break
// 【N90】差值主体=libstdc++4.1 iterator/find 非 inline sret 外联（ORIG 8 insns/步 vs 4.4 inline 4）；豁免不逐形追。
// [b28d-N90] fresh 复验 97/85：维持 N90（同上，迭代器外联 + 帧差）。
#include "CStackableItem/createLotteryInfo_1.inc"
}

// ORIG 0x89bafd1：解析 multibox 彩票脚本。
//   a (vector<vector<int>>)：每元素是一个 LotteryUnit 的原始 int 流
//     [count, defaultIdx, defaultNum, defaultFlag, (itemIdx,weight,count,flag)xN]
//     —— m_items.push_back({itemIdx,weight})，m_bonus.push_back({count,flag})
//   b (vector<int>)：field8=b[0]，fieldc=b[1]
//   c (vector<int>)：m_defaultCount/Idx/Num/Flag + m_items(stDefaultItem) 流
// 【N90】差值主体=libstdc++4.1 iterator/find 非 inline sret 外联（ORIG 8 insns/步 vs 4.4 inline 4）；豁免不逐形追。
// [b28d-solo-CStackableItem] p167 收口（本波核心修法，ORIG 调用集 106 逐符号对齐）：
// ①c 部分 ORIG 是 5 个**裸早返** `if (it == c->end()) return;`（0x089bb694/0x089bb6fe/
//   0x089bb768/0x089bb7d2/0x089bb83c 各落独立 nop 中继岛 0xa51/0xa54/0xa57/0xa5a/0xa64，
//   P223 岛形），原「嵌套 if (it != c->end())」写法恒产 ne×5（p167 EXTRA ne×5 /
//   MISSING eq×6）⇒ 改写后 ne/eq 全部归位。
// ②a 内层循环与 c 循环均为**底部旋转**（a：0x089bb45a eq 检查 + 0x089bb493 ne 条件同在
//   循环尾；c：body 0x089bb87d / 条件 0x089bb9e2）⇒ 源码 do-while 形。
// ③真语义 bug 修正：a 内层第 4 个边界检查在 **push_back 之后**（ORIG 0x089bb45a 的
//   `eq; jne 0x4ab` 落在 m_units.push_back 之前）——原写法在 push 前 break 会丢弃
//   「恰好用完最后一个元素」的那一组；c 循环体内同样只有 3 个中间检查（m_flag 后无检查）。
// ④p167 终态（b28d-poolcera 更新）：**106/106 全 CLEAR**。原 EXTRA（D1×2 +
//   _Unwind_Resume×1）根因 = 我方 MultiBoxLotteryInfo.h 给用户空析构（非平凡）；
//   ORIG `_Destroy_aux<true>` 实证 stDefaultItem 析构平凡 ⇒ 删声明+定义后 D1 岛与
//   landing pad 自然消失（非 N91 codegen 差，判词翻案）。行数 668→649（ORIG 652，
//   gap 3）；⑤framePad 删除（P232 回推形随 D1 岛消失而反向，保留=整体深 4；删后
//   ndiff 1187→15）+ 第5检查翻形 B `if (!(it == c->end())) { do-while }`（je 极性
//   归位，探针 F/G/cmp 形不可再翻）。终残差 = 3 段 `mov (%eax),%eax; mov %eax,%edx`
//   两段装载被 4.4 折单段（N18a，P248 三档类型旋钮已封）+ `!eq` 的 xor $1 物化
//   vs ORIG je+jmp 双跳（N18a 折叠族，cargo_abs 先例）。ndiff 359→15。
// [b28d-solo-CStackableItem 2026-09-09 二轮] **gap 3→0（fresh 652=652）**，旧判词
// 「3 段两段装载 N18a 封顶」**作废**：真因 = stDefaultItemInfo 三字段
// m_defaultCount/m_defaultIdx/m_defaultNum 建模成 int（应为 unsigned int，P254/
// P211 档③同异型侧）。直证（同函数、同一 RHS `*it` int、唯字段类型不同）：
//   · 本类字段 store = `mov (%eax),%eax; mov %eax,%edx; mov 0x8(%ebp),%eax;
//     mov %edx,0x1c(%eax)`（0x089bb6af / 0x089bb719 / 0x089bb783，+0x1c/+0x20/+0x24）
//     —— 带 eax→edx 中转 = 与 int 异型；
//   · 同函数 MultiBoxLotteryInfo::m_field8/m_fieldc（源码 int）store =
//     `mov (%eax),%edx; mov 0x8(%ebp),%eax; mov %edx,0x8(%eax)`（0x089bb5c7 /
//     0x089bb62f）—— 3 条无中转 = 同型。
// 旁证：get_multibox_default_item 0x0850f371 读 m_defaultCount 后 `cmp i; seta`
// （无符号 above，循环变量 unsigned int）且读取单段（unsigned 字段读形）。
// 探针（446 后端与路由一致）：/tmp/probe_mb.cpp（int 字段=3 条 / unsigned=4 条带中转）、
// /tmp/probe_mb2.cpp（unsigned 字段读 + unsigned 循环上界与 ORIG 同形零回退）。
// 头改动 MultiBoxLotteryInfo.h 三字段 int→unsigned int（同宽 4B，sizeof/偏移/mangled
// 零变化），Main 批准 + all 广播；消费 TU 复编：CStackableItem 1331/3/6（零回退）、
// MultiBoxLotteryInfo 113/0/1（零回退）、GmCmds_F 5/18/1/1（零回退）。
// 终残差 = 1 处 `!eq` 的 xor $1 物化 vs ORIG `test; je body; jmp tail`（等长替换，
// 652=652）：跨版本 `if(!bool)` lowering 差（4.1 块交换 / 4.4 xor 物化），探针
// /tmp/probe_mb3.cpp f_sw/f_goto/f_else/f_while/f_neg 五形均不可造 ⇒ 判 N（P292/N18a 族）。
#include "CStackableItem/createStackableMultiBoxLotteryInfo_1.inc"
}

class CChattingEmoticonList
{
public:
    void* find_emoticon(int idx);
};

class STStackableScript
{
public:
    STStackableScript();
    ~STStackableScript();
    int m_field0;                        // +0x00
    int m_field4;                        // +0x04
    int m_field8;                        // +0x08
    int m_fieldc;                        // +0x0c
    int m_field10;                       // +0x10
    int m_field14;                       // +0x14
    int m_field18;                       // +0x18
    int m_field1c;                       // +0x1c
    int m_field20;                       // +0x20
    int m_field24;                       // +0x24
    int m_field28;                       // +0x28
    int m_field2c;                       // +0x2c
    char m_field30;                      // +0x30
    bool m_field31;                      // +0x31
    char m_pad32[6];                     // +0x32
    bool m_usable[0xb];                  // +0x38
    char m_pad43[0x19];                  // +0x43
    int m_field5c;                       // +0x5c
    int m_field60;                       // +0x60
    int m_field64;                       // +0x64
    int m_field68;                       // +0x68
    std::string m_str6c;                 // +0x6c
    char m_pad70[0x18];                  // +0x70
    std::vector<int> m_vec88;            // +0x88
    std::vector<std::string> m_vec94;    // +0x94
    char m_pada0[0xc];                   // +0xa0
    std::string m_strac;                 // +0xac
    char m_padb0[0x14];                  // +0xb0
    int m_fieldc4;                       // +0xc4
    int m_fieldc8;                       // +0xc8
    int m_fieldcc;                       // +0xcc
    bool m_fieldd0;                      // +0xd0
    char m_padd1[7];                     // +0xd1
    bool m_fieldd8;                      // +0xd8
    bool m_fieldd9;                      // +0xd9
    bool m_fieldda;                      // +0xda
    char m_paddb[1];                     // +0xdb
    std::set<int> m_setdc;               // +0xdc
    int m_fieldf4;                       // +0xf4
    int m_fieldf8;                       // +0xf8
    char m_fieldfc;                      // +0xfc
    char m_padfd[3];                     // +0xfd
    int m_field100;                      // +0x100
    char m_field104;                     // +0x104
    STSpecialMonsterDrop m_specialDrop;  // +0x105
    char m_field10e;                     // +0x10e
    bool m_field10f;                     // +0x10f
    std::vector<std::pair<int, int> > m_vec110;  // +0x110
    std::set<STItemScript::ENUM_ITEM_CATEGORY> m_set11c;  // +0x11c
    char m_pad134[0x1c];                 // +0x134
    int m_field150;                      // +0x150
    int m_field154;                      // +0x154
    std::vector<STItemScript::SEXPERTJOB::SUSABLE> m_vec158;  // +0x158
    DyeInfo m_dye164;                    // +0x164
    char m_pad174[4];                    // +0x174
    int m_field178;                      // +0x178
    char m_field17c;                     // +0x17c
    char m_pad17d[3];                    // +0x17d
    std::string m_str180;                // +0x180
    std::string m_str184;                // +0x184
    std::string m_str188;                // +0x188
    int m_field18c;                      // +0x18c
    std::vector<int> m_vec190;           // +0x190
    std::vector<CRabbitBox> m_vec19c;    // +0x19c（ORIG vector<CRabbitBox>，secret_shop_event_item 实证）
    char m_pad1a8[4];                    // +0x1a8
    int m_field1ac;                      // +0x1ac
    int m_field1b0;                      // +0x1b0
    int m_field1b4;                      // +0x1b4
    char m_pad1b8;                       // +0x1b8
    unsigned char m_field1b9;            // +0x1b9
    unsigned char m_field1ba;            // +0x1ba
    unsigned char m_field1bb;            // +0x1bb
    unsigned char m_field1bc;            // +0x1bc
    unsigned char m_field1bd;            // +0x1bd
    unsigned char m_field1be;            // +0x1be
    char m_field1bf;                     // +0x1bf
    bool m_field1c0;                     // +0x1c0
    char m_pad1c1[3];                    // +0x1c1
    int m_field1c4;                      // +0x1c4
    char m_pad1c8[8];                    // +0x1c8
    char m_field1d0;                     // +0x1d0
    char m_pad1d1[0xf];                  // +0x1d1
    int m_field1e0;                      // +0x1e0
    int m_field1e4;                      // +0x1e4
    int m_field1e8;                      // +0x1e8
    std::vector<int> m_vec1ec;           // +0x1ec
    std::vector<StackableAction::UsablePlace> m_vec1f8;  // +0x1f8
    char m_pad204;                       // +0x204
    char m_field205;                     // +0x205
    char m_pad206[0xe];                  // +0x206
    std::vector<StackableAction::UsableServerType> m_vec214;  // +0x214
    int m_field220;                      // +0x220
    std::vector<int> m_vec224;           // +0x224
    char m_pad230[4];                    // +0x230
    std::vector<std::vector<int> > m_vec234;  // +0x234
    char m_pad240[4];                    // +0x240
    std::vector<std::vector<int> > m_vec244;  // +0x244
    std::vector<int> m_vec250;           // +0x250
    std::vector<int> m_vec25c;           // +0x25c
    std::vector<STStackablePackageData> m_vec268;  // +0x268（ORIG vector<STStackablePackageData>）
    std::vector<std::vector<STStackablePackageData> > m_vec274;  // +0x274
    stStackableBooster_t m_booster;      // +0x280
    std::vector<STStackablePackageData> m_vec2a4;  // +0x2a4（ORIG vector<STStackablePackageData>）
    char m_pad2b0[0x7c];                 // +0x2b0
    char m_field32c;                     // +0x32c
    char m_pad32d[0x2b];                 // +0x32d
    int m_field358;                      // +0x358
    char m_pad35c[4];                    // +0x35c
    char m_field360;                     // +0x360
    char m_pad361[3];                    // +0x361
    // ORIG set_item 0x0850dcf4 `mov 0x364(%eax),%edx; mov %edx,0x28c(%eax)`
    // 4 字节直读 ⇒ 该字段为 int（ushort 版恒产 movzwl 双段）。pad 同步缩短保持
    // +0x380 后续偏移不变。
    int m_field364;                      // +0x364
    char m_pad368[0x18];                 // +0x368
    unsigned short m_field380;           // +0x380
    char m_pad382[2];                    // +0x382
    char m_pad384[0x10];                 // +0x384
    unsigned short m_field394;           // +0x394
    bool m_field396;                     // +0x396
    char m_pad397;                       // +0x397
    unsigned short m_field398;           // +0x398
    char m_field39a;                     // +0x39a
    bool m_field39b;                     // +0x39b
    int m_field39c;                      // +0x39c
    char m_field3a0;                     // +0x3a0
    // [b28d-solo-CStackableItem] ORIG set_item 0x0850e???: `movzbl 0x3a1(%eax),%edx;
    // mov %dl,0x344(%eax)`（无 bool 归一化）+ 目标 CItem::m_char344 为 bool
    // ⇒ 源与目标同为 bool 字节直传（P136）；声明 char 会让 4.4 插 test/setne（+2 条）。
    // 该字段在本 TU 只被 set_item 读取（STStackableScript.cpp grep 0 命中），改型零风险。
    bool m_field3a1;                     // +0x3a1
    char m_pad3a2[2];                    // +0x3a2
    std::map<int, int> m_map3a4;         // +0x3a4
    char m_pad3bc[4];                    // +0x3bc
    std::vector<std::pair<int, int> > m_vec3c0;  // +0x3c0
    int m_field3cc;                      // +0x3cc
    char m_field3d0;                     // +0x3d0
    bool m_field3d1;                     // +0x3d1
    char m_pad3d2[2];                    // +0x3d2
    std::vector<std::pair<int, int> > m_vec3d4;  // +0x3d4
    int m_field3e0;                      // +0x3e0
    int m_field3e4;                      // +0x3e4
    std::vector<stLimitCubeResult> m_vec3e8;     // +0x3e8
    std::map<STEnchantSystemMapKey, STEnchantSystemMapData> m_map3f4;  // +0x3f4
    char m_field40c;                     // +0x40c
    char m_fields40d[2];                 // +0x40d
    char m_pad40f[9];                    // +0x40f
    int m_field418;                      // +0x418
    BoosterSelectInfo m_boosterSelect;   // +0x41c
    // [b28d-P248档③] ORIG set_item 0x0850e1e2 store 侧 `movzbl 0x438(%eax),%edx;
    // mov %dl,0x320(%eax)`（bool→bool 直存，无 test/setne 归一化）⇒ 本字段同型化为
    // bool，与 CStackableItem.h m_char320 bool 配对（宽度 1B、偏移/布局零变化）。
    bool m_field438;                     // +0x438
    char m_pad439[3];                    // +0x439
    int m_field43c;                      // +0x43c
    int m_field440;                      // +0x440
    int m_field444;                      // +0x444
    int m_field448;                      // +0x448
    int m_field44c;                      // +0x44c
    int m_field450;                      // +0x450
    int m_field454;                      // +0x454
    int m_field458;                      // +0x458
    int m_field45c;                      // +0x45c
    int m_field460;                      // +0x460
    int m_field464;                      // +0x464
    int m_field468;                      // +0x468
    int m_field46c;                      // +0x46c
    char m_pad470[4];                    // +0x470
    int m_field474;                      // +0x474
    int m_field478;                      // +0x478
    unsigned short m_field47c;           // +0x47c
    char m_field47e;                     // +0x47e
    char m_pad47f;                       // +0x47f
    std::set<int> m_set480;              // +0x480
    std::set<int> m_set498;              // +0x498
    bool m_field4b0;                     // +0x4b0
    char m_pad4b1[7];                    // +0x4b1
    int m_field4b8;                      // +0x4b8
    char m_pad4bc[0x30];                 // +0x4bc
    int m_field4ec;                      // +0x4ec
    char m_pad4f0[0x18];                 // +0x4f0
    int m_field508;                      // +0x508
    std::set<int> m_set50c;              // +0x50c
};

// ===================== 报告函数（CStackableItem.md） =====================

#include "CStackableItem/IsAvatarOptionChangeItem.inc"
}

#include "CStackableItem/check_able_To_use.inc"
}

// 【N90】差值主体=libstdc++4.1 iterator/find 非 inline sret 外联（ORIG 8 insns/步 vs 4.4 inline 4）；豁免不逐形追。
// P182/N82 封顶（b26 Lane27）：29 条指令全等，仅帧 0x28 vs 0x10（4.4 outgoing/
// slack 多 24B）连带 2 char 槽平移（-0xc/-0x10 vs -0x4/-0x8）；帧策略跨版本差。
// b28c-L8 fresh 复验：44 行口径 29=29、NDIFF=4，判词维持 N82（callee
// _ZNK5CItem14check_job_typeEc 定义在 CItem.cpp 0x0850d728 = 跨 TU ⇒ P210 定义前置不可用；
// P190 double framePad 反向撑帧亦不可（本例是 4.4 多撑，需减帧））。
// [b28d-N115/N82] fresh 29=29 维持 N：帧 0x28 vs 0x10 + 2 char 参数槽平移（-0xc/-0x10
// vs -0x4/-0x8）。根因 = N115：4.1 参数直写帧底（outgoing 仅实参 4B），4.4 -O0 恒预留
// 16B 对齐 outgoing 区（帧 +0x18）。P210 定义前置不可用（callee CItem::check_job_type
// 定义在 CItem.cpp = 跨 TU，迁定义违 R1）；P190 double 撑帧方向反（本例需减帧）。
#include "CStackableItem/check_change_job_type.inc"
}

// [b28d-solo-CStackableItem] AE 101=101。修法（新范式，P218/P310 扩展）：局部 `i`
// 声明 unsigned int + **裸比较** `while (it->m_count > i)` ⇒ 4.4 产 ORIG 形
// `mov m_count,%eax; cmp i,%eax; seta`。此前 `int i + (unsigned) 双 cast` 恒产
// `mov i,%ebx; cmp m_count,%ebx; setb`（+1 条 mov，且方向反）；反写 `m_count > i`
// 带 cast 形劣化 +2（`mov %eax,%edx; mov i,%eax; cmp`）。外层/内层 end() 按 P13 缓存。
#include "CStackableItem/getBoosterItem.inc"
}

#include "CStackableItem/getUsableItemType.inc"
}

#include "CStackableItem/get_1st_material_count.inc"
}

// ORIG 0x822c9e0（弱符号 W，14 字节）：mov 0x29c(%eax),%eax —— 第 2 材料项索引
#include "CStackableItem/get_2nd_material_index.inc"
}

// ORIG 0x822c9ee（弱符号 W，14 字节）：mov 0x2a0(%eax),%eax —— 第 2 材料项数量
#include "CStackableItem/get_2nd_material_count.inc"
}

#include "CStackableItem/get_amplify_reinforce_count.inc"
}

// [b28d-solo-CStackableItem] NEAR 118=118。已修：`sum` 局部 unsigned 化（新范式
// P218/P310 扩展）⇒ 比较翻正为 ORIG `mov rand,%eax; cmp sum,%eax; ja`（此前 int sum
// 恒产 `mov sum,%eax; cmp rand,%eax; jb`）。残余 N95 封顶：range 槽 ORIG -0x38
// （在 begin/end/pp 临时槽之后），4.4 恒 -0x44（临时槽之前）；把 range 声明移到循环后
// 实测反向（range -0x48/it -0x44）；本波补测「移到 total 之后」= 零效（range 仍 -0x44，
// 4.4 按首次使用序把取地址具名标量排到匿名临时区之后）⇒ 槽分配方向不可择，判 N95/N93。
// ORIG 0x850fbe4 两循环复用同一 const_iterator 槽 -0x48（第二循环 `it = begin()`
// 重新赋值），循环体先 operator*（deEv）取引用地址入槽（-0x10/-0xc），增量段 ppEi。
#include "CStackableItem/get_limit_cube_item.inc"
}

#include "CStackableItem/is_included_material_for_limit_cube.inc"
}

#include "CStackableItem/quickslot_priority.inc"
}

#include "CStackableItem/verify.inc"
}

#include "CStackableItem/verifyUsableServerType.inc"
}

// ORIG 0x0822ca58 (W)：return m_int39c == type
#include "CStackableItem/verifyStackableAction.inc"
}

// ===================== make_item（ORIG 0x850f8b8） =====================

#include "CStackableItem/make_item.inc"
}

// ===================== get_lotto_item（ORIG 0x850ebfc） =====================

// [b28d-solo-CStackableItem] ORIG 0x850ebfc：pairIt 先声明（默认构造 C1Ev，P50）后
// `pairIt = begin()`；循环体 `item = std::pair<int,int>(*pairIt)` 走 aSEOS0_（P135），
// 之后一律读 item.first/second（-0x44/-0x40）；countIt 为 const_iterator 转换构造。
#include "CStackableItem/get_lotto_item.inc"
}

// ===================== verifyUsablePlace（ORIG 0x850e9f4） =====================

// [b28d-solo-CStackableItem] ORIG 0x850e9f4：place 为 UsablePlace 类型（find 实例
// `…EEEES3_ET_S9_S9_RKT0_` 的 RKT0_=UsablePlace，int 版 mangled 为 `…EEEiE`）；
// 比较为单表达式 `find(...) == m_usablePlaces.end()`（end() 求值两次：-0x18 供
// 比较、-0x10 供 find），迭代器为非 const iterator（eq 实例 `eqIPN…`）。
#include "CStackableItem/verifyUsablePlace.inc"
}

// ===================== set_item（ORIG 0x850d81e） =====================

// [b28d-solo-CStackableItem] TU 局部 `struct CItemView` + `(CItemView*)this` cast 反模式
// 已消除：+0x8c/+0xd0/+0xfc/+0x100 四个偏移在权威头 CItem.h 落地为具名成员 + 匿名 union
// 双视角（int m_increaseStatusType；union{bool m_boolD0; char m_charD0}；
// union{float m_floatFC; int m_intFC}；union{float m_float100; int m_int100}），
// 底部 static_assert 逐偏移锁定（sizeof 0x148 不变）。视图 cast 只做偏移“翻译”，
// 与 ORIG 的 plain `mov %edx,0xfc(%eax)` 直存同形；改成具名成员后访问形态不变。

// [b28d-solo-CStackableItem] fresh 复验 1060/1073（起点 1059/1100）：已修——
// ①CItemView 三处偏移整体少 0x10（m_charD0@0xc0→0xd0、m_intFC 0xec→0xfc、
//   m_int100 0xf0→0x100）真语义 bug；②删 ORIG 全函数无写入的
//   `m_boolF8 = script.m_field31`（P171）；③删 scriptText/strncpy 冗余缓冲，
//   script2Database 直传 m_strac.c_str()；④cMyTrace 改全表达式临时链（P227）；
//   ⑤镜像 STStackableScript 13 字段按 ORIG `movzbl` 单段形改型；⑥m_field364→int。
// [b28d-N] fresh 1059/1071（差 12 条，逐行 diff 323 行、槽号归一后匹配 76 行）判 N 封顶：
// 残余族 = ①槽整体深 4（ORIG -0x64/-0x258 vs 我们 -0x68/-0x25c；前置死局部 `int
// framePad` 实测反向变 -0x6c，N95 槽基不可上移）；②ORIG 把 `lea -0x64,%esi` 地址缓存进
// callee-saved 而我们直用 %edx（P183/P226 地址物化族）；③装载序/中转（N76/P211）；
// ④`m_vec94[i]` lea-vs-add（N107）；⑤比较方向 seta-vs-setb（N18b）。
// 注：函数内 `__asm__ __volatile__` rep stosl 清零块（ORIG 0x7d dwords=500B）沿用既有
// 批次做法，非 P0 register 绑定；本波未改动（如 Main 要求可换 P312 `char buf[500]={0}`
// + 前置死局部形重测）。
// [b28d-solo-CStackableItem Lane34 视图 cast 波 2026-09-09] gap 12→**6**（1060/1066）：
//  ①4 处 `(CItemView*)this` 写点改权威具名成员（m_charD0/m_intFC/m_int100/
//    m_increaseStatusType，见本 TU 上一段注释 + CItem.h static_assert），删除 TU 局部
//    CItemView；②镜像 STStackableScript::m_field3a1 char→bool（ORIG `movzbl 0x3a1;
//    mov %dl,0x344` 无 bool 归一化 ⇒ 两侧同为 bool 字节直传，P136；本 TU 唯一读者）
//    ⇒ 消 `test/setne` 2 条；③emo 段 `EmoticonFn fn=...;fn(...)` 改直接调用表达式
//    （ORIG 函数指针在 %edx 无槽往返，P0）⇒ -2；④`*reinterpret_cast<unsigned short*>
//    (&m_avatar365.m_field0)` 改 CStackableItem.h 新增 16 位具名视角 m_jobType16
//    （union 同址零布局 + static_assert；ORIG `mov %cx,0x365(%eax)` disp 直存）⇒ -1；
//  ⑤`for (int i=1; i<m_vec94.size(); ++i)` 改 `for (unsigned i=1; size()>i; ++i)`
//    （ORIG 尾判 `cmp -0x28(%ebp),%eax; seta`，操作数序按 size() 在左）⇒ -1。
//  残 6 条 = 纯跨版本装载/寄存器族（无结构缺口）：①地址缓存（ORIG `lea -0x64,%esi`
//  callee-saved vs 4.4 重发 lea，P183/P226）；②`mov mem,%eax; mov %eax,%edx` 中转
//  ×4（N18a/N92 反向）；③m_field474 载 %eax 再中转 %esi（P87）；④m_stat2dc 段
//  ORIG 两段 vs 4.4 单段（-1）。判定：**判据①（jcctgt 目标下标逐位）0 mismatch**；
//  p167 调用点 84=84 CLEAR（仅 C1/C2 符号别名 P93）；TU 四数 431/900/3/6 零回退。
#include "CStackableItem/set_item.inc"
}

// ===================== get_multibox_default_item（ORIG 0x850f136） =====================

// [b28d-solo-CStackableItem] **AE-EQUAL 153=153**（本波修复，原 N18b 判词作废）。
// 修法 = 字段侧异型化（P274/P321-S1 一次旋钮）：stDefaultItem::m_prob int→unsigned int
// （MultiBoxLotteryInfo.h:33，Main 代落 + R2 广播；同宽 4B、偏移 +0x08/sizeof 0x10/
// mangled 零变化）。ORIG 0x0850f1bd 累加段 = `mov 0x8(%eax),%edx; mov -0x1c(%ebp),%eax;
// lea (%edx,%eax,1),%eax; mov %eax,-0x1c(%ebp)`（读-加-写四段）：4.4 对「int 字段 +
// int 累加器」恒折 `mov mem,%eax; add %eax,sum`（N18b），对「unsigned 字段 + int 累加器」
// 逐条复现（探针 /tmp/probe_sum2.cpp a1；int 侧 a5/a6 均折）。sum 保持 int ⇒ 后续
// `sum > rand` 仍为 ORIG 有符号 `jle`。volatile 化实测槽漂移(-0x1c→-0x28)+比较物化，已回退。
// 本件 `flag = stDefaultItemInfo::m_defaultFlag != 0` 的 test/setne 物化已匹配
// （该字段保持 char，见 get_multibox_lottery_item 判词的字段分型说明）。
// ORIG 0x850f136：rand/sum/itemIdx/count/flag 声明在函数头（-0x20/-0x1c/-0x18/-0x14/-0xd），
// 循环体内先 `it = begin()`（-0x2c）再 get_rand_int(1000000)；内层 while 每次调 end()、
// 增量 `it++`（ppEi）。
#include "CStackableItem/get_multibox_default_item.inc"
}

// ===================== get_multibox_default_item_arad（ORIG 0x850f38a） =====================

// [b28d-solo-CStackableItem] ORIG 0x850f38a：**static 成员**（首参 0x8(%ebp)、无 this，
// P245/P281）；静态表 push_back 走右值重载 push_backEOS1_（临时对象直传，P270）；
// 选择循环 rand<=sum、operator*、it++（ppEi），pair.second 直写 0。
#include "CStackableItem/get_multibox_default_item_arad.inc"
}

// ===================== get_multibox_lottery_item（ORIG 0x850edda） =====================

// [b28d-solo-CStackableItem] AE 214=214。修法（P300/P248 档②）：本函数 `flag` 局部
// char→bool + **LotteryUnit::m_defaultFlag** 字段 char→bool（MultiBoxLotteryInfo.h，
// 已 R2 广播）⇒ `flag = unit.m_defaultFlag;` 与 `p.second = flag;` 均产 ORIG 直存
// `movzbl -0x5c; mov %al`（此前 char 版 4.4 归一化 `cmpb/setne` 多 1 条）。
// 注意**同名字段分属两类、真型相反**：stDefaultItemInfo::m_defaultFlag 保持 char
// （其消费点 get_multibox_default_item / createStackableMultiBoxLotteryInfo 要求
// char 形）——字段级分型后三件同时满足（见各自判词）。
// ORIG 0x850edda：rand/sum/itemIdx/count/flag 声明在函数头（-0x24/-0x20/-0x1c/-0x18/-0xd）；
// 外层 while 每次调 end()、增量 unitIt++（ppEi）；unit 拷贝构造落 -0x68；内层双迭代器
// itemIt(-0x40)/bonusIt(-0x44) 均 ppEi，字段访问走 operator*（deEv）。
#include "CStackableItem/get_multibox_lottery_item.inc"
}

// ===================== 简单虚函数 / 访问器（ORIG weak） =====================

#include "CStackableItem/GetItemType.inc"
}

#include "CStackableItem/IsAvatarItem.inc"
}

#include "CStackableItem/IsCreatureItem.inc"
}

#include "CStackableItem/IsClearAvatar.inc"
}

#include "CStackableItem/IsEnchantalbe.inc"
}

#include "CStackableItem/isEnableAvatarFunc.inc"
}

#include "CStackableItem/GetGuildPowerWarPoint.inc"
}

#include "CStackableItem/GetUserPowerWarPoint.inc"
}

#include "CStackableItem/IsImpossibleGift.inc"
}

#include "CStackableItem/IsRandomOptionUnseal.inc"
}

#include "CStackableItem/set_parent_booster_item_id.inc"
}

#include "CStackableItem/get_parent_booster_item_id.inc"
}

#include "CStackableItem/getTradeLimitCount.inc"
}

// ===================== 简单访问器 / 状态（ORIG weak） =====================

// ORIG 0x822c9fc：返回 this+0x28c
#include "CStackableItem/getStackableLimit.inc"
}

// ORIG 0x822ca0a：返回 this+0x280（引用→拷贝构造源）
#include "CStackableItem/getGlobalEffectData.inc"
}

// ORIG 0x8694736：movzbl 0x2e7(%eax); ret —— 直接返回 bool 字段（无 test/setne 归一）。
#include "CStackableItem/IsExpAffect.inc"
}

// +0x2d8 为 CountinuousItemOption 子对象（m_option2d8：3 独立字节）。
#include "CStackableItem/GetCoolTimeOption.inc"
}

// ORIG 0x086946f4 (W 非 const)：读 +0x2e4(+0x2e6)，effect 时长(秒)/标志
// +0x2e4 为 CountinuousItemOption 子对象（m_option2e4：3 独立字节）。
// 【N90】差值主体=libstdc++4.1 iterator/find 非 inline sret 外联（ORIG 8 insns/步 vs 4.4 inline 4）；豁免不逐形追。
#include "CStackableItem/GetEffectItemOption.inc"
}

// ORIG 0x08694716 (W 非 const)：无 ctor 调用，直接 8 字节拷贝
//   mov 0x2e0(%eax),%edx; mov 0x2dc(%eax),%eax; mov %eax,(%ecx); mov %edx,0x4(%ecx)
// +0x2dc 为 StatChangeInfo 子对象（m_stat2dc：int + 低两字节 flag）。
// 调用方(_processEffectItem)按 ORIG 只测 flag 的低 16 位。
#include "CStackableItem/GetStatChangeInfo.inc"
}

// ORIG 0x08694746 (W 非 const)：flds +0x2e8 —— 返回经验加成比例(float)
#include "CStackableItem/GetExpAffectRatio.inc"
}

// ORIG 0x822ca28：movzwl 0x2b4(%eax) —— 返回珍宝目标槽位（16 位，单次零扩展）
#include "CStackableItem/getJewelTargetSocket.inc"
}

// ORIG W 0x822c9d2 _ZNK14CStackableItem12get_sub_typeEv：
// mov 0x2bc(%eax) —— 返回 m_int2bc（sub_type）
#include "CStackableItem/get_sub_type.inc"
}

// ORIG 0x0869475e W _ZNK14CStackableItem26IsUse2ndUpgradeTableRecipeEv：
// movzbl 0xd0(%eax) —— 返回 CItem 基类 +0xd0 字节（m_boolD0）
#include "CStackableItem/IsUse2ndUpgradeTableRecipe.inc"
}

// ORIG 0x822ca48 (W)：movzbl 0x321(%eax),%eax —— 直接返回 bool 字段（无 test/setne）。
#include "CStackableItem/IsDeletableItem.inc"
}

// ORIG _ZN14CStackableItem18IsSharedEffectItemEv W 0x0822ca18：movzbl 0x2ec(%eax),%eax —— 返回 m_char2ec 字节（共享特效物品标记）
#include "CStackableItem/IsSharedEffectItem.inc"
}

// ORIG _ZN14CStackableItem13GetEmancipateEv W 0x0854b4e8：movzwl 0x2ee(%eax),%eax —— 返回 m_short2ee（解绑值）
#include "CStackableItem/GetEmancipate.inc"
}

// ============================================================================
// CStackableItem::is_random_amplify_reinforce_ticket @ 0x854b518
//   返回 +0x344 bool（movzbl 0x344(%eax); ret，无 test/setne 归一）。
// ============================================================================
#include "CStackableItem/is_random_amplify_reinforce_ticket.inc"
}
