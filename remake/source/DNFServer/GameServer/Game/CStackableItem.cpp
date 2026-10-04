// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CStackableItem/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
// [PoolA 2026-09-11 fresh 复核 · 清单 7 件（routed 11:55，本 TU 四数 431/900/3/6）]：
//   · C1Ev（ORIG 0x08515762 W，size 0x3b1）：**DIFF** 292/293 ndiff2。唯一残差 =
//     vptr 写时机：ORIG `call <AradItemInfo::C1>`（this+0x148）**之后**才
//     `mov 0x8(%ebp),%eax; movl $<vtable>,(%eax)`；4.4 在成员构造前先写 vptr
//     ⇒ P242/N109 布局-发射序族（须把 AradItemInfo 建模为首/次基类），
//     维持「Main 裁决本轮不做，spec 存档」。
//   · check_change_job_type（ORIG 0x0850eba2）：**NEAR** 29/29 ndiff3，帧 0x10 vs
//     0x28 + 2 char 参数槽（-0x4/-0x8 ↔ -0xc/-0x10）⇒ N115（4.4 -O0 恒预留 16B
//     对齐 outgoing；callee CItem::check_job_type 定义在 CItem.cpp = 跨 TU，
//     P210 不可用）⇒ 维持。
//   · GetCoolTimeOption（ORIG 0x086946d2 W）/ GetEffectItemOption（0x086946f4 W）：
//     **DIFF** 10/11 ndiff1（各 1 条 `mov 0xc(%ebp),%edx` this 重载）。ORIG
//     `movzwl 0x2d8(%edx),%ecx`（值进 %ecx 保留 %edx=this 供第二读）；4.4 覆盖
//     %edx 后重载 ⇒ N7（P230 记载 4.4.6 后端族）；前人多形探针（ptr-arith 视图/
//     3B 子对象整拷/逗号/aggregate/const 引用）在册 ⇒ 维持。
//   · get_limit_cube_item（ORIG 0x0850fbe4）：**NEAR** 118/119 ndiff7，残差 =
//     it/range 槽互换（ORIG it -0x3c / range -0x38；我方 it -0x38 / range -0x44）
//     + end() 调用块位置 ⇒ N95/N93（W13A 两形探针实测均劣化，在册）⇒ 维持。
//   · createStackableMultiBoxLotteryInfo（ORIG 0x089bafd1）：**DIFF** 667/667
//     ndiff2，唯一残差 = `if (!(it == c->end()))` 的 `xor $1,%eax` 物化 vs ORIG
//     `test %al,%al; je; jmp`（等长替换）⇒ 跨版本 `if(!bool)` lowering（五形探针
//     f_sw/f_goto/f_else/f_while/f_neg 在册）⇒ 维持 N。
//   · set_item（ORIG 0x0850d81e，size 0x10bf）：**本波真改善**。基 1060/1066 ndiff8
//     → 现 **1060/1063 gap −3 ndiff4**（四数零回退）。修法 = N 族旋钮矩阵探针
//     （/tmp/lane_poola/probe1.cpp g2/g3/g4 定量）结论落地：store 装载序的**唯一**旋钮
//     是「源/目标符号性同异型」（异型 ⇒ RHS-first + `mov %eax,%edx` 中转 = ORIG 形；
//     同型 ⇒ LHS-first 单段）。据此改本 TU 私有镜像 STStackableScript 5 个字段：
//     m_field17c char→unsigned char（目标 AradItemInfo::m_field4 uchar 同型，
//     消 `mov %eax,%edx`）、m_field1e8 int→StackableAction::Type（目标 m_int39c enum
//     同型）、m_field358 int→unsigned int（目标 StatChangeInfo::sec int 异型，
//     生 ORIG 的 `mov 0x358(%eax),%eax; mov %eax,%edx`）、m_field47e char→unsigned char、
//     m_field474 int→unsigned int（callee 形参 set_lottery_use_cost(unsigned,unsigned)
//     同型，消中转）。全部同宽 4B/1B、逐字段唯一消费点在 set_item、零共享头改动。
//     残差 2 处（判 N）：①`lea -0x64(%ebp),%esi` 提前物化（4.1 RA 调度，P183/P226
//     族）；②m_avatar365 段（+0x365/+0x367）4.1 双基址缓存 vs 4.4 逐语句重载。
//   ⇒ 7 件中 1 件（set_item）本波实质收敛；其余 6 件与既有判词一致（仅登记）。

#include "CStackableItem.h"

#include "PvPTypes.h"   // 权威 CMTRand 完整类型（new CMTRand(seed) 需要；原局部 stub 已删除）
#include "AradRabbitBox.h"  // +0x19c 兔盒元素类型（W6_ARAD_ItemBonus 收敛）

#include <algorithm>
#include <string>
#include <string.h>
#include <time.h>

#include "../../ServerCommon/DNFFunctionLib.h"


#include "CStackableItem/CStackableItem.inc"


#include "CStackableItem/dtor_CStackableItem.inc"


#include "CStackableItem/tag2Type.inc"


#include "CStackableItem/tag2EndType.inc"


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

#include "CStackableItem/BoosterSelectInfo.inc"
{
    // [b28c-27] AE：ORIG 0x8513bb8 先写 m_field18 再 vector::clear（源码语句序
    // 必须同序；原序 clear→field18 使 landing pad 布局差 2 条）。
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
    m_field18 = '\0';
    m_nameVec.clear();
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

class CChattingEmoticonList;
class CDataManager
{
public:
    void set_AuctionRegFreeCeraItem(unsigned int idx);
    void set_lottery_use_cost(unsigned int cost, unsigned int flag);
    char m_pad[0xc];
    class CItemList* m_itemList;   // +0x0c
    char m_pad10[0x18];
    CChattingEmoticonList* m_emoticonList;  // +0x28
};
extern CDataManager* G_CDataManager();

#include "CSystemTime.h"

#include "GlobalData.h"   // [R18scanPH] GlobalData::s_systemTime_（make_item 时间基准；原硬编码 0x941f714 已废）

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

#include "CStackableItem/GetEquipmentTypeStringToEnum.inc"


#include "CStackableItem/getStackableActionType.inc"


#include "CStackableItem/getStackableActionUsablePlace.inc"


#include "CStackableItem/getStackableActionUsableServerType.inc"


#include "CStackableItem/createRecipeInfo.inc"
                      std::vector<std::pair<int, int> >* extra);
#include "CStackableItem/createLotteryInfo.inc"
#include "CStackableItem/createStackableLotteryInfo.inc"
#include "CStackableItem/createStackableMultiBoxLotteryInfo.inc"
    MultiBoxLotteryInfo* info,
    std::vector<std::vector<int> >* a, std::vector<int>* b,
    std::vector<int>* c);
#include "CStackableItem/createGlobalEffectInfo.inc"
                           std::vector<int>* source);


#include "CStackableItem/createRecipeInfo_1.inc"

#include "CStackableItem/LotteryInfo.inc"

#include "CStackableItem/dtor_LotteryInfo.inc"
#include "CStackableItem/clear.inc"

#include "CStackableItem/createLotteryInfo_1.inc"


#include "CStackableItem/createStackableMultiBoxLotteryInfo_1.inc"


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
    // [PoolA 2026-09-11 P248档③ · 探针 /tmp/lane_poola/probe1.cpp g8 实证]
    // ORIG set_item 该 store 为单段 `movzbl 0x17c(%eax),%edx; mov 0x8(%ebp),%eax;
    // mov %dl,0x14c(%eax)` ⇒ 源与 AradItemInfo::m_field4(unsigned char) **同型**；
    // 我方原 char(signed) 与之异型 ⇒ 4.4 多 `mov %eax,%edx` 中转。改 unsigned char
    // （同宽 1B、本 TU 唯一消费点 = 1671 行，零布局/符号影响）。
    unsigned char m_field17c;            // +0x17c
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
    // [PoolA 2026-09-11 P248档③] ORIG 该 store 为单段 `mov 0x1e8(%eax),%edx;
    // mov 0x8(%ebp),%eax; mov %edx,0x39c(%eax)` ⇒ 与目标 m_int39c(enum) **同型**；
    // 原 int 致 4.4 多 `mov %eax,%edx` 中转。改 StackableAction::Type（同宽 4B）。
    StackableAction::Type m_field1e8;    // +0x1e8
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
    // [PoolA 2026-09-11 P248档③ · 探针 g2/g3 实证：符号性异型 ⇒ `mov mem,%eax;
    // mov %eax,%edx` 中转 = ORIG 形] ORIG 0x0850d81e 段 `mov 0x358(%eax),%eax;
    // mov %eax,%edx; mov 0x8(%ebp),%eax; mov %edx,0x2dc(%eax)` ⇒ 源与目标
    // StatChangeInfo::sec(int) **异型**；改 unsigned int（同宽 4B，唯一消费点 1606）。
    unsigned int m_field358;             // +0x358
    char m_pad35c[4];                    // +0x35c
    // [R19ty T-2b 2026-09-13] char→bool：ORIG set_item 0x0850df79 `movzbl 0x360(%eax),%edx;
    //   mov %dl,0x2e0(%eax)` 单段直存（无 test/setne 归一化）⇒ 源与目标 CStackableItem::
    //   StatChangeInfo::m_flagLo 同为 bool（P136）。1B 宽不变，后续 pad/偏移不变。
    bool m_field360;                     // +0x360
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
    // [PoolA 2026-09-11 P248档③] ORIG 0x0850d81e 段 `mov 0x474(%eax),%esi`（单段直装
    // callee-saved）⇒ 源与 callee 形参 `set_lottery_use_cost(unsigned int, unsigned int)`
    // **同型**；原 int 异型致 4.4 多 `mov %eax,%esi` 中转。改 unsigned int（同宽 4B）。
    unsigned int m_field474;             // +0x474
    int m_field478;                      // +0x478
    unsigned short m_field47c;           // +0x47c
    // [PoolA 2026-09-11 P248档③] ORIG 该 store 为单段 `movzbl 0x47e(%edx),%edx;
    // mov %dl,0x367(%eax)` ⇒ 与目标 m_avatar365.m_field2(unsigned char) **同型**；
    // 原 char(signed) 致 4.4 多 `mov %eax,%edx` 中转。改 unsigned char（同宽 1B）。
    unsigned char m_field47e;            // +0x47e
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


#include "CStackableItem/check_able_To_use.inc"


#include "CStackableItem/check_change_job_type.inc"


#include "CStackableItem/getBoosterItem.inc"


#include "CStackableItem/getUsableItemType.inc"


#include "CStackableItem/get_1st_material_count.inc"


#include "CStackableItem/get_2nd_material_index.inc"


#include "CStackableItem/get_2nd_material_count.inc"


#include "CStackableItem/get_amplify_reinforce_count.inc"


#include "CStackableItem/get_limit_cube_item.inc"


#include "CStackableItem/is_included_material_for_limit_cube.inc"


#include "CStackableItem/quickslot_priority.inc"


#include "CStackableItem/verify.inc"


#include "CStackableItem/verifyUsableServerType.inc"


#include "CStackableItem/verifyStackableAction.inc"


// ===================== make_item（ORIG 0x850f8b8） =====================

#include "CStackableItem/make_item.inc"


// ===================== get_lotto_item（ORIG 0x850ebfc） =====================

#include "CStackableItem/get_lotto_item.inc"


// ===================== verifyUsablePlace（ORIG 0x850e9f4） =====================

#include "CStackableItem/verifyUsablePlace.inc"


// ===================== set_item（ORIG 0x850d81e） =====================

// [b28d-solo-CStackableItem] TU 局部 `struct CItemView` + `(CItemView*)this` cast 反模式
// 已消除：+0x8c/+0xd0/+0xfc/+0x100 四个偏移在权威头 CItem.h 落地为具名成员 + 匿名 union
// 双视角（int m_increaseStatusType；union{bool m_boolD0; char m_charD0}；
// union{float m_floatFC; int m_intFC}；union{float m_float100; int m_int100}），
// 底部 static_assert 逐偏移锁定（sizeof 0x148 不变）。视图 cast 只做偏移“翻译”，
// 与 ORIG 的 plain `mov %edx,0xfc(%eax)` 直存同形；改成具名成员后访问形态不变。

#include "CStackableItem/set_item.inc"


// ===================== get_multibox_default_item（ORIG 0x850f136） =====================

#include "CStackableItem/get_multibox_default_item.inc"


// ===================== get_multibox_default_item_arad（ORIG 0x850f38a） =====================

#include "CStackableItem/get_multibox_default_item_arad.inc"


// ===================== get_multibox_lottery_item（ORIG 0x850edda） =====================

#include "CStackableItem/get_multibox_lottery_item.inc"


// ===================== 简单虚函数 / 访问器（ORIG weak） =====================

#include "CStackableItem/GetItemType.inc"


#include "CStackableItem/IsAvatarItem.inc"


#include "CStackableItem/IsCreatureItem.inc"


#include "CStackableItem/IsClearAvatar.inc"


#include "CStackableItem/IsEnchantalbe.inc"


#include "CStackableItem/isEnableAvatarFunc.inc"


#include "CStackableItem/GetGuildPowerWarPoint.inc"


#include "CStackableItem/GetUserPowerWarPoint.inc"


#include "CStackableItem/IsImpossibleGift.inc"


#include "CStackableItem/IsRandomOptionUnseal.inc"


#include "CStackableItem/set_parent_booster_item_id.inc"


#include "CStackableItem/get_parent_booster_item_id.inc"


#include "CStackableItem/getTradeLimitCount.inc"


// ===================== 简单访问器 / 状态（ORIG weak） =====================

#include "CStackableItem/getStackableLimit.inc"


#include "CStackableItem/getGlobalEffectData.inc"


#include "CStackableItem/IsExpAffect.inc"


#include "CStackableItem/GetCoolTimeOption.inc"


#include "CStackableItem/GetEffectItemOption.inc"


#include "CStackableItem/GetStatChangeInfo.inc"


#include "CStackableItem/GetExpAffectRatio.inc"


#include "CStackableItem/getJewelTargetSocket.inc"


#include "CStackableItem/get_sub_type.inc"


#include "CStackableItem/IsUse2ndUpgradeTableRecipe.inc"


#include "CStackableItem/IsDeletableItem.inc"


#include "CStackableItem/IsSharedEffectItem.inc"


#include "CStackableItem/GetEmancipate.inc"


#include "CStackableItem/is_random_amplify_reinforce_ticket.inc"

