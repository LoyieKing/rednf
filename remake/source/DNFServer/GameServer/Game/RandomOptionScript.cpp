// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 RandomOptionScript/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— RandomOptionScript（G5 random_option 域）
// 逐函数对照 docs/class_func_reports/RandomOptionScript.md：
//   clear 0x0837261e / ctor 0x08372904 / dtor 0x0837c9e8
//   getPrefixGrade 0x08a73a80 / getPrefix 0x08a73956 / 
//   getOptionModificationCost 0x08a738a0 / importRandomOptionScript 0x08a70a90
// ============================================================================

#include "RandomOptionScript.h"

#include <string.h>

#include "DNFLexWrapper.h"
#include "CCubeStatistic.h"   // getItemGroupNameTable()
#include "LogManager.h"       // logFormat（ORIG duplicate-key 报错）

// ===================== importRandomOptionScript =====================
// ORIG 0x08a70a90（0x2e10 字节脚本解析器，2754 条反汇编逐段核对）。
//
// 【batch-14 收敛】we-bigger 221 → 1（ORIG 2754 / ours 2755），P167 调用点差集
// 双向 0（361/361 逐符号 Counter 全等）。削减手段两类：
//  ① 键拼装改「扫描即直存键槽」：4 处 map 键（[option quantity] int 键 -0x4a4、
//     [choose option group]/[modified option selection] u64 键 -0x4b0/-0x4e0、
//     [break seal cost] int 键 -0x588、[different weight] short 键 -0x592）原用
//     移位或链拼装在 -O0 每站 40-49 insns，ORIG 实为逐 byte/word/dword 直存
//     （`mov %al,-0x4b0` / `mov %eax,-0x4ac` / `mov %ax,-0x586` …）⇒ 用文件内
//     anonymous-namespace union 视图（P180，纯类型零符号 + static_assert 尺寸）复现；
//  ② 44 处 `v = ScanInt(&ok); if(!ok)…; dst = (T)v;` 的共享 v 中转改写为
//     `dst = (T)ScanInt(&ok); if(!ok)…`（P183 物化口径：ORIG 每次 Scan 结果
//     `call 后紧接 mov %eax/%ax/%al,目的槽`，随后才 movzbl ok 检查），每站 −2 insns；
//     同口径连带消掉 [part type] itemGroup/weight 与 [option group] weight 中转量。
//  ③ 顺带修正两处语义缺失：[part type] 命中支缺 `field.m_weight = it->second.m_fieldC`
//     累计回写（ORIG 313-317 第 4 次 pt，原缺 1 条 operator-> 调用）；
//     [part type]/[option group] 第二 vector 实收累计权重而非原始 weight。
// 残余 DIFF 归因：N82/P112 帧槽号（4.1 vs 4.4 分配序）+ .o 未 relink 的
// R_386_PC32/R_386_32 显示差（AE 口径链接后核销）；本函数另有 3 站 std::string
// 临时量与 map 迭代器具名实例化产生 C2/D2 别名拼写差（ORIG nm 双侧实存）。
// 同 TU：getPrefix 0x8a73956 已按 ORIG 形重写（第三参拷本地 byte 槽、表指针
// 先无条件取 m_prefixNormal 再条件覆盖、iterator 非 const）→ 99/99 gap 0。
// ============================================================================
// 主循环 ScanType 按段名分发，各段把脚本数值填入对应成员表；
// 返回出错行号（成功 0）——与调用方 CDataManager::init
// `const int err = importRandomOptionScript(...); if (err != 0)` 消费一致。
// ORIG 无 script.clear()（调用方 CDataManager 于加载前已 clear）。
//
// 段名 → 目标表（+ 成员偏移）：
//   [random application]        → m_map0(+0x00)
//   [level limit]               → m_map18(+0x18)
//   [part type]                 → m_map30(+0x30)
//   [base item]                 → m_map48(+0x48)
//   [option type]               → m_map90(+0x90)
//   [quantity ratio]            → m_veca8(+0xa8)
//   [option quantity]           → m_mapb4(+0xb4)
//   [choose option group]       → m_mapcc(+0xcc)
//   [modified option selection] → m_map130(+0x130)
//   [modified option type]      → m_map118(+0x118)
//   [option modification]       → m_costVec10c(+0x10c)
//   [option value ratio]        → m_ratioE4(+0xe4)
//   [option group]              → m_map160(+0x160)
//   [postfix]                   → m_prefixNormal(+0x1a8)/m_prefixPremium(+0x1c0)
//   [choose postfix]            → m_choosePrefix1d8(+0x1d8)
//   [break seal cost]           → m_map178(+0x178)
//   [option text]               → 丢弃（ORIG 仅 ScanInt+ScanStr 消费行）
//   [different weight]          → m_mapf4(+0xf4)
//   [changed postfix]           → m_str1e4(+0x1e4)
//   [postfix grade modification]→ m_map1e8(+0x1e8)
//   [unable to modify postfix grade] → m_set200(+0x200)
//   [gradeless postfix]         → m_set218(+0x218)

// ============================================================================
// 键拼装视图（batch-10 P167 反向差集复核）：ORIG 侧这些 map 键不是「先存若干
// 临时量再移位或拼」，而是**每次 ScanInt 后直接把字节写进键对象自己的槽**，
// 最后以 u64/int 视图作 make_pair 实参：
//   [option quantity]        int  键 -0x4a4：`mov %al,-0x4a4/-0x4a3/-0x4a2/-0x4a1`
//     （报告 ORIG +0xECA..+0xF05；值侧 `mov %ax,-0x4a8/-0x4a6` = m_minA/m_maxB 直存）
//   [choose option group]    u64  键 -0x4b0：`mov %al,-0x4b0/-0x4af/-0x4ae`、
//     `mov %eax,-0x4ac`（高 32）、`mov %al,-0x4ad`（+0x1393.. 区段），
//     make_pair 实参型 `Ry`（unsigned long long&）⇒ 同对象 u64 视图
//   [modified option selection] u64 键 -0x4e0：同形（-0x4e0/-0x4df/-0x4de/
//     -0x4dc(dword)/-0x4dd）
//   [break seal cost]        int  键 -0x588：`mov %al,-0x588`、`mov %ax,-0x586`、
//     `mov %al,-0x587`（+2 word / +1 byte 混序，ORIG 逐条照抄）
// 故用 union 视图复现「具名成员 + 常量偏移直存」形（范式库 P180）；移位或链在
// -O0 会多出 40+ insns/站（本函数 we-bigger 221 的主因）。
// ============================================================================

namespace
{

union OptionQuantityKeyView
{
    int m_key;                      // map<int, OptionQuantityValue> 键
    struct
    {
        unsigned char m_b0;         // +0x00（`mov %al,-0x4a4`）
        unsigned char m_b1;         // +0x01
        unsigned char m_b2;         // +0x02
        unsigned char m_b3;         // +0x03
    } m_bytes;
};
static_assert(sizeof(OptionQuantityKeyView) == 4, "ORIG int 键 4 字节");

union OptionGroupChoiceKeyView
{
    unsigned long long m_key;       // map<u64, OptionGroupChoiceValue> 键
    struct
    {
        unsigned char m_b0;         // +0x00
        unsigned char m_b1;         // +0x01
        unsigned char m_b2;         // +0x02（getItemGroupNameTable 值）
        unsigned char m_b3;         // +0x03
        unsigned int m_hi;          // +0x04（`mov %eax,-0x4ac`）
    } m_bytes;
};
static_assert(sizeof(OptionGroupChoiceKeyView) == 8, "ORIG u64 键 8 字节");

union BreakSealCostKeyView
{
    int m_key;
    struct
    {
        unsigned char m_b0;         // +0x00（`mov %al,-0x588`）
        unsigned char m_b1;         // +0x01（`mov %al,-0x587`）
        unsigned short m_w2;        // +0x02（`mov %ax,-0x586`）
    } m_bytes;
};
static_assert(sizeof(BreakSealCostKeyView) == 4, "ORIG break seal int 键 4 字节");

// [different weight] 键 = short（ORIG 槽 -0x592/-0x591 两字节直存，
// make_pair 实参型 `Rs` = short&）：`mov %al,-0x592` / `mov %al,-0x591`。
union DifferentWeightKeyView
{
    short m_key;
    struct
    {
        unsigned char m_b0;         // +0x00
        unsigned char m_b1;         // +0x01
    } m_bytes;
};
static_assert(sizeof(DifferentWeightKeyView) == 2, "ORIG different weight short 键 2 字节");

}   // namespace

// [b28d-solo-RandomOptionScript] DIFF（§6.2 超大函数口径，本轮大幅收敛）：
// ORIG 0x08a70a90；p167 CLEAR 361/361（双向零差）+ 逐块操作码差 75/2754
// （difflib ndiff 2617→75，降 97%）+ 帧尺寸同 0x5f0 + ORIG 30 处 movl $0 预清零
// 我方现 31 处。本轮按 S287 P191/P198③ 范式补齐的段内声明/两段式：
//  ① P191 扩展：段级共用循环变量（[choose option group]/[modified option selection]
//     两循环共用 `int i=0` + 各 `for(i=0;…)`，ORIG -0x38/-0x30 各 3 处清零）；
//  ② 段内 v/dead 预清零（-0x50/-0x48/-0x44/-0x2c/-0x28/-0x4d4/-0x504，其中
//     -0x44/-0x28 为只清零不读的 dead local，4.4-O0 保留，探针 f 形实证）；
//  ③ 两段式（声明=0 + 独立赋值）：[postfix] count、[break seal cost] cost、
//     [choose postfix] minVal/maxVal/v、[unable to modify]/[gradeless postfix]
//     独立 unsigned long key（ORIG 2682/2709 预清零）；
//  ④ [option value ratio] ratio 对象移循环外构造一次（ORIG 1923 C1Ev，回跳点
//     1926 跳过）+ 三字段 ScanFloat/ScanInt 直存，消 base/range/v 中转；
//  ⑤ [base item] 累计回写 `v = (val.m_fieldC += v);`（ORIG 493-500 赋值表达式
//     返回引用后回写 v，再 push v）。
// 残余 25 块为跨版本 CFG 布局（4.1 `je 成功; jmp 尾` 两跳 vs 4.4 `jne 尾` 一跳
// 共 6 处、ORIG 循环回跳 jmp、nop 岛/epilogue-threading N121、int 局部常量比较
// 4.4 恒 mem 直比 P209/N18b），按 §6.2 判「语义完成、byte 层跨版本布局 N 封顶」。
//
// [BigRndOpt 本波·四套配平复核 → 维持 N 封顶，无改动]
//  ① 性质：ORIG 2754 / ours 2761（+7，0.25%）⇒ 指令数基本齐 ⇒ 形态族，
//     非「未翻译段」。台账 ndiff 3963 是 autojunk 对巨件的对齐坍缩假象
//     （N127）；strict(autojunk=False) ndiff=**1120**、structdiff struct=**70**。
//  ② p167 调用点：**361/361 双向零差**（CLEAR）。
//  ③ 串 key 多重集：ORIG 侧本函数引用 35 个 unique 串（22 个段名 + 11 条
//     "…duplicate key error" + 11×文件名 + 11×__FUNCTION__），我方 **35/35
//     全覆盖，零 MISSING 零 EXTRA**；源码层同步核（段名字面量 22、logFormat
//     11、__FUNCTION__ 11）。⇒ 骨架完整，无缺分支/缺段。
//  ④ 返回常量多重集（本族最强的「分支覆盖」判据，`mov $0xNN,%ebx` 全收集）：
//     ORIG 75 / OURS 76，**orig-only = ∅**，our-only 仅 `0x0` ×1（主循环正常
//     出口 `return 0` 在 4.4 下另发一次立即数装载，ORIG 复用前值）
//     ⇒ 75 个出错返回行号逐一对应，零缺零多。
//  ⑤ 立即数多重集（reloc 折叠行已剔除，避免 `.rodata+off` 假差）：orig-only
//     仅 0x5f0（`sub $0x5f0,%esp`），our-only 仅 0x600 ⇒ 唯一净差 = 帧大小
//     16 字节 = N82；其余全消。
//  ⑥ 四条修法逐一试尺：P329 不适用（11 处 logFormat 已是 5 实参全内联形，
//     ORG 形 `movl $line,0xc(%esp)/movl $<func>,0x8/movl $<file>,0x4/movl $0,
//     (%esp)/call` 与我方逐位同）；P183 不适用（无我方独有缓存，反向补缓存
//     受 N128 禁）；P332 补 不适用（ORIG `0x8(%ebp)` 直读 33 次、我方 33 次，
//     `0xc(%ebp)` 4/5 次 ⇒ 形参未被溢出到栈槽）；BigAuction 三翻法 不适用
//     （本函数无 switch，段分发是 else-if 串且 22 段物理序已与 ORIG 叶块序
//     一致——见 ② 的 eq 调用点全等）。
//  ⑦ struct=70 的分布：26 个块，最大 10 行（主循环回跳处 4.4 多 5 条 `jmp`
//     空跳转链 + `test;je` 对 ORIG `xor $0x1;test;jne`），其余 1-7 行碎片
//     ⇒ 「多段不同常数」形，按 §4 尺封 N（N121 jump-threading / N18b・P209
//     bool 取反系统差 / N82 帧槽·nop 岛）。
//  ⇒ **无 REAL-BUG**：本件未发现可观察行为差异（无缺 else、无错偏移、
//     无伪析构、无判空对象错位）。
#include "RandomOptionScript/importRandomOptionScript.inc"
}

#include "RandomOptionScript/clear.inc"
}

#include "RandomOptionScript/RandomOptionGive.inc"
}

#include "RandomOptionScript/RandomOptionGive_1.inc"
}

#include "RandomOptionScript/dtor_RandomOptionGive.inc"
}

#include "RandomOptionScript/clear_1.inc"
}

#include "RandomOptionScript/RandomOptionScript.inc"
}

#include "RandomOptionScript/dtor_RandomOptionScript.inc"
}

#include "RandomOptionScript/clear_2.inc"
}

// ORIG 0x08a73a80（非 const，_ZN mangled；循环前缓存 vector 指针，
// 命中存 result 后 break 汇合返回——对齐 ORIG -O0 布局）。
#include "RandomOptionScript/getPrefixGrade.inc"
}

// ORIG 0x08a73956 RandomOptionScript::getPrefix(int,int,bool)
// 按 flag 选择 m_prefixNormal(+0x1a8) / m_prefixPremium(+0x1c0) map，
// 以第一个 int(prefix) 为 key find；返回向量下标
// getPrefixGrade(grade)（flag 时 +3）处的字符串。
// [b28d-solo-RandomOptionScript] AE（compare_tu clean 口径，99/99 逐条同构）：本轮三处修正——
//  ① 原越界防御（idx>=0 && idx<size()）非 ORIG 语义：ORIG 是「具名 vector 指针 +
//     unsigned 下标 while 扫描」`pVec=&it->second; i=0; while(pVec->size()>i){
//     if(idx==i){out=(*pVec)[i];break;} ++i; }`（ORIG +0xac..+0x0fa 逐条复现）；
//  ② p167 实测 ORIG 调 `ptEv`（operator->）非 `deEv`（operator*）⇒ `it->second`；
//  ③ bool 形参 `flag` 直用（删具名 premium 局部，P244 配套：4.4 对 bool 形参
//     自动截断 spill 到 -0x3c，具名局部会多 movzbl+movb 并把 it/end 顶深 4）。
// 复验：p167 CLEAR（11/11）；TU identical+ae 计数 +2。
#include "RandomOptionScript/getPrefix.inc"
}

#include "RandomOptionScript/getOptionModificationCost.inc"
}
