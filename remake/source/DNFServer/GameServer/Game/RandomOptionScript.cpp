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
int importRandomOptionScript(RandomOptionScript& script, const char* path)
{
    if (!loadRDARScriptFile("", path))
    {
        return 0x11;
    }

    std::string line;
    bool ok = false;
    while (ScanType(line, true))
    {
        int v;

        if (line == "[random application]")
        {
            RandomApplicationKey key;
            for (;;)
            {
                key.m_field0 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    break;
                }
                key.m_field1 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    return 0x27;
                }
                RandomApplicationValue val;
                val.m_field0 = ScanInt(&ok);
                if (!ok)
                {
                    return 0x2b;
                }
                val.m_field4 = ScanInt(&ok);
                if (!ok)
                {
                    return 0x2f;
                }
                if (!script.m_map0.insert(std::make_pair(key, val)).second)
                {
                    LogManager::logFormat(0, "../RDARScriptRandomOption.cpp",
                                          __FUNCTION__, 0x33,
                        "importRandomOptionScript - random_application_ duplicate key error");
                    return 0x34;
                }
            }
        }
        else if (line == "[level limit]")
        {
            RandomApplicationKey key;
            for (;;)
            {
                key.m_field0 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    break;
                }
                key.m_field1 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    return 0x44;
                }
                LevelLimitDecisionValue val;
                val.m_field0 = ScanInt(&ok);
                if (!ok)
                {
                    return 0x48;
                }
                val.m_field4 = ScanInt(&ok);
                if (!ok)
                {
                    return 0x4c;
                }
                if (!script.m_map18.insert(std::make_pair(key, val)).second)
                {
                    LogManager::logFormat(0, "../RDARScriptRandomOption.cpp",
                                          __FUNCTION__, 0x50,
                        "importRandomOptionScript - level_limit_ duplicate key error");
                    return 0x51;
                }
            }
        }
        else if (line == "[part type]")
        {
            // ORIG 每行三字段：partType itemGroupName weight；循环回 0x8a70e28
            // 重新读 partType。weight 读失败 → return 0x6e（非跳出）。
            std::string name;
            int partType = 0;
            PartSelectonField field;   // ORIG -0x428/-0x424：两 int 槽由扫描直填
            for (;;)
            {
                partType = ScanInt(&ok);
                if (!ok)
                {
                    break;   // 段首字段失败 → 回主循环（ORIG esi=1）
                }
                name.clear();
                if (!ScanStr(&name))
                {
                    return 0x64;
                }
                std::map<std::string, int>::const_iterator git =
                    getItemGroupNameTable().find(name);
                if (git == getItemGroupNameTable().end())
                {
                    return 0x68;
                }
                // ORIG 262-267：git->second 与 ScanInt 结果**直接落 field 的两个 int 槽**
                // （node 侧 -0x428/-0x424），无 itemGroup/weight 中转量。
                field.m_partType = git->second;
                field.m_weight = ScanInt(&ok);
                if (!ok)
                {
                    return 0x6e;
                }
                std::map<int, PartSelectonValue>::iterator mit =
                    script.m_map30.find(partType);
                if (mit != script.m_map30.end())
                {
                    // ORIG 297-325 四段 pt 链：①pt → m_vec10.push_back(field)
                    // ②pt → m_fieldC += field.m_weight（node+0x10 就地累加）
                    // ③pt → **field.m_weight = m_fieldC（累计值回写 field 权重槽）**
                    // ④pt → m_vec0.push_back(field)（故第二表存的是累计权重）
                    mit->second.m_vec10.push_back(field);
                    mit->second.m_fieldC += field.m_weight;
                    field.m_weight = mit->second.m_fieldC;   // ORIG +0x5ac 回写
                    mit->second.m_vec0.push_back(field);
                }
                else
                {
                    PartSelectonValue val;
                    val.m_vec10.push_back(field);
                    val.m_vec0.push_back(field);
                    val.m_fieldC = field.m_weight;
                    // ORIG 此分支 insert 不检查 duplicate。
                    script.m_map30.insert(std::make_pair(partType, val));
                }
            }
        }
        else if (line == "[base item]")
        {
            RandomApplicationKey key;
            BaseItemSelectionValue val;
            int itemCount = 0;
            // [b28d-solo-RandomOptionScript] ORIG 444 段内 v 预清零（-0x50）
            int v = 0;
            for (;;)
            {
                key.m_field0 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    break;
                }
                key.m_field1 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    return 0x94;
                }
                itemCount = ScanInt(&ok);
                if (!ok)
                {
                    return 0x98;
                }
                val.clear();
                for (int i = 0; i < itemCount; ++i)
                {
                    v = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0x9f;
                    }
                    // [b28d-solo-RandomOptionScript] ORIG 493-500：赋值表达式返回引用 ⇒
                    // `v = (val.m_fieldC += v);` 后 push 的是回写后的 v（-0x50），
                    // 非内存直加 + 重读 m_fieldC（我方原形多 add mem 少 4 条装载）。
                    v = val.m_fieldC += v;
                    val.m_vec0.push_back((unsigned int)v);
                }
                if (!script.m_map48.insert(std::make_pair(key, val)).second)
                {
                    LogManager::logFormat(0, "../RDARScriptRandomOption.cpp",
                                          __FUNCTION__, 0xa8,
                        "importRandomOptionScript - base_item_selection_ duplicate key error");
                    return 0xa9;
                }
            }
        }
        else if (line == "[option type]")
        {
            RandomApplicationKey key;
            RandomOptionGive give;
            int optionType = 0;
            // [b28d-solo-RandomOptionScript] ORIG 608/609 段内 v(-0x48)/dead(-0x44) 预清零
            int v = 0;
            int dead = 0;
            for (;;)
            {
                key.m_field0 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    break;
                }
                optionType = ScanInt(&ok);
                if (!ok)
                {
                    return 0xbc;
                }
                key.m_field1 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    return 0xc0;
                }
                give.clear();
                for (int i = 0; i <= 2; ++i)
                {
                    v = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0xc7;
                    }
                    give.m_c += v;
                    // ORIG 0x8a71575：m_c 仅累加，vector 推入的是本次输入原值
                    // （-0x334 = -0x48 的副本），与 [base item] 推累计和不同。
                    give.m_vec0.push_back((unsigned int)v);
                }
                std::map<RandomApplicationKey,
                         std::map<int, RandomOptionGive> >::iterator it =
                    script.m_map90.find(key);
                if (it != script.m_map90.end())
                {
                    if (!it->second.insert(std::make_pair(optionType, give)).second)
                    {
                        LogManager::logFormat(0, "../RDARScriptRandomOption.cpp",
                                              __FUNCTION__, 0xd3,
                            "importRandomOptionScript - option_type_ duplicate key error");
                        return 0xd4;
                    }
                }
                else
                {
                    std::map<int, RandomOptionGive> newMap;
                    newMap.insert(std::make_pair(optionType, give));
                    script.m_map90.insert(std::make_pair(key, newMap));
                }
            }
        }
        else if (line == "[quantity ratio]")
        {
            for (;;)
            {
                QuantityRatio ratio;
                ratio.m_field0 = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ratio.m_weight4 = static_cast<unsigned int>(ScanInt(&ok));
                if (!ok)
                {
                    return 0xea;
                }
                script.m_veca8.push_back(ratio);
            }
        }
        else if (line == "[option quantity]")
        {
            OptionQuantityValue val;         // ORIG -0x4a8（m_minA/m_maxB 逐 word 直存）
            OptionQuantityKeyView keyView;   // ORIG -0x4a4（逐 byte 直存，无移位或链）
            for (;;)
            {
                keyView.m_bytes.m_b0 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    break;
                }
                keyView.m_bytes.m_b1 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    return 0xfb;
                }
                keyView.m_bytes.m_b2 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    return 0xff;
                }
                keyView.m_bytes.m_b3 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    return 0x103;
                }
                val.m_minA = static_cast<short>(ScanInt(&ok));
                if (!ok)
                {
                    return 0x107;
                }
                val.m_maxB = static_cast<short>(ScanInt(&ok));
                if (!ok)
                {
                    return 0x10b;
                }
                if (!script.m_mapb4.insert(std::make_pair(keyView.m_key, val)).second)
                {
                    LogManager::logFormat(0, "../RDARScriptRandomOption.cpp",
                                          __FUNCTION__, 0x10f,
                        "importRandomOptionScript - option_quantity_ duplicate key error");
                    return 0x110;
                }
            }
        }
        else if (line == "[choose option group]")
        {
            OptionGroupChoiceValue choice;
            std::string name;
            // [b28d-solo-RandomOptionScript] ORIG 1040 段内 v 预清零（-0x4d4）
            int v = 0;
            int groupCount = 0;
            // [b28d-solo-RandomOptionScript] P191 扩展（S287 实证）：ORIG -0x38 段级
            // 共用循环变量，两循环各 for(i=0) + 声明处双初始化（ORIG 1039/1150/1174）。
            int i = 0;
            OptionGroupChoiceKeyView keyView;   // ORIG -0x4b0：逐字节/双字直存
            for (;;)
            {
                keyView.m_bytes.m_b0 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    break;
                }
                keyView.m_bytes.m_b1 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    return 0x125;
                }
                name.clear();
                if (!ScanStr(&name))
                {
                    return 0x129;
                }
                std::map<std::string, int>::const_iterator git =
                    getItemGroupNameTable().find(name);
                if (git == getItemGroupNameTable().end())
                {
                    return 0x12d;
                }
                keyView.m_bytes.m_b2 = static_cast<unsigned char>(git->second);
                // ORIG +0x1289：ScanInt 结果整 dword 直存键 +4（无中间量）
                keyView.m_bytes.m_hi = ScanInt(&ok);
                if (!ok)
                {
                    return 0x133;
                }
                keyView.m_bytes.m_b3 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    return 0x137;
                }
                groupCount = ScanInt(&ok);   // ORIG +0x12e8：直存计数槽
                if (!ok)
                {
                    return 0x13b;
                }
                choice.clear();
                for (i = 0; i < groupCount; ++i)
                {
                    v = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0x142;
                    }
                    choice.m_vec0.push_back(v);
                }
                for (i = 0; i < groupCount; ++i)
                {
                    v = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0x14b;
                    }
                    choice.m_vecC.push_back((unsigned int)v);
                    choice.m_field18 += v;
                }
                // 键已按 ORIG 序逐段写入（b0/b1/b2/hi/b3），此处仅取 u64 视图
                if (!script.m_mapcc.insert(std::make_pair(keyView.m_key, choice)).second)
                {
                    LogManager::logFormat(0, "../RDARScriptRandomOption.cpp",
                                          __FUNCTION__, 0x153,
                        "importRandomOptionScript - option_group_choice_ duplicate key error");
                    return 0x154;
                }
            }
        }
        else if (line == "[modified option selection]")
        {
            OptionGroupChoiceValue choice;
            std::string name;
            // [b28d-solo-RandomOptionScript] ORIG 1318 段内 v 预清零（-0x504）
            int v = 0;
            int groupCount = 0;
            // [b28d-solo-RandomOptionScript] P191 扩展：ORIG -0x30 段级共用循环变量
            // （ORIG 1317/1428/1452 三处清零 = 声明 + 两 for init）。
            int i = 0;
            OptionGroupChoiceKeyView keyView;   // ORIG -0x4e0：逐字节/双字直存
            for (;;)
            {
                keyView.m_bytes.m_b0 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    break;
                }
                keyView.m_bytes.m_b1 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    return 0x16a;
                }
                name.clear();
                if (!ScanStr(&name))
                {
                    return 0x16e;
                }
                std::map<std::string, int>::const_iterator git =
                    getItemGroupNameTable().find(name);
                if (git == getItemGroupNameTable().end())
                {
                    return 0x172;
                }
                keyView.m_bytes.m_b2 = static_cast<unsigned char>(git->second);
                keyView.m_bytes.m_hi = ScanInt(&ok);   // ORIG +0x1358：dword 直存键 +4
                if (!ok)
                {
                    return 0x178;
                }
                keyView.m_bytes.m_b3 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    return 0x17c;
                }
                groupCount = ScanInt(&ok);             // ORIG +0x1380：直存计数槽
                if (!ok)
                {
                    return 0x180;
                }
                choice.clear();
                for (i = 0; i < groupCount; ++i)
                {
                    v = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0x187;
                    }
                    choice.m_vec0.push_back(v);
                }
                for (i = 0; i < groupCount; ++i)
                {
                    v = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0x190;
                    }
                    choice.m_vecC.push_back((unsigned int)v);
                    choice.m_field18 += v;
                }
                // 键已按 ORIG 序（b0/b1/b2/hi/b3）逐段写入，取 u64 视图入表
                if (!script.m_map130.insert(std::make_pair(keyView.m_key, choice)).second)
                {
                    LogManager::logFormat(0, "../RDARScriptRandomOption.cpp",
                                          __FUNCTION__, 0x198,
                        "importRandomOptionScript - modified_option_group_choice_ duplicate key error");
                    return 0x199;
                }
            }
        }
        else if (line == "[modified option type]")
        {
            RandomApplicationKey key;
            RandomOptionGive give;
            int optionType = 0;
            // [b28d-solo-RandomOptionScript] ORIG 1596/1597 段内 v(-0x2c)/dead(-0x28) 预清零
            int v = 0;
            int dead = 0;
            for (;;)
            {
                key.m_field0 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    break;
                }
                optionType = ScanInt(&ok);
                if (!ok)
                {
                    return 0x1ac;
                }
                key.m_field1 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    return 0x1b0;
                }
                give.clear();
                for (int i = 0; i <= 2; ++i)
                {
                    v = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0x1b7;
                    }
                    give.m_c += v;
                    // ORIG 0x08a7259a：m_c 仅累加，vector 推入的是本次输入原值
                    // （-0x1b4 = -0x2c 的副本），与 [base item] 推累计和不同。
                    give.m_vec0.push_back((unsigned int)v);
                }
                std::map<RandomApplicationKey,
                         std::map<int, RandomOptionGive> >::iterator it =
                    script.m_map118.find(key);
                if (it != script.m_map118.end())
                {
                    if (!it->second.insert(std::make_pair(optionType, give)).second)
                    {
                        LogManager::logFormat(0, "../RDARScriptRandomOption.cpp",
                                              __FUNCTION__, 0x1c3,
                            "importRandomOptionScript - modified_option_type_ duplicate key error");
                        return 0x1c4;
                    }
                }
                else
                {
                    std::map<int, RandomOptionGive> newMap;
                    newMap.insert(std::make_pair(optionType, give));
                    script.m_map118.insert(std::make_pair(key, newMap));
                }
            }
        }
        else if (line == "[option modification]")
        {
            for (;;)
            {
                OPTION_MODIFICATION_COST cost;
                cost.m_level = static_cast<short>(ScanInt(&ok));   // ORIG：word 直存 +0x00
                if (!ok)
                {
                    break;
                }
                cost.m_cost2 = ScanInt(&ok);
                if (!ok)
                {
                    return 0x1da;
                }
                cost.m_cost3 = ScanInt(&ok);
                if (!ok)
                {
                    return 0x1de;
                }
                script.m_costVec10c.push_back(cost);
            }
        }
        else if (line == "[option value ratio]")
        {
            // ORIG 0x8a72a15/~0x8a72ac1：ratio 对象（+0 scaleRange0/+4 scaleBase4/+8 weight8）
            // 于**循环外**构造一次（ORIG 1923 C1Ev，回跳点 1926 跳过构造）；行内首 float
            // 直接 fstps +4（-0x550）、次 float 直存 +0（-0x554）、int 直存 +8（-0x54c），
            // 无 base/range/v 中转量。
            OptionValueRatio ratio;
            for (;;)
            {
                ratio.m_scaleBase4 = ScanFloat(&ok);   // +0x04（行内首 float）
                if (!ok)
                {
                    break;
                }
                ratio.m_scaleRange0 = ScanFloat(&ok);  // +0x00（行内次 float）
                if (!ok)
                {
                    break;
                }
                ratio.m_weight8 = (unsigned int)ScanInt(&ok);   // +0x08
                if (!ok)
                {
                    return 0x1f5;
                }
                script.m_ratioE4.m_c += ratio.m_weight8;
                script.m_ratioE4.m_vec0.push_back(ratio);
            }
        }
        else if (line == "[option group]")
        {
            OptionGroupValue value;
            int type = ScanInt(&ok);
            if (!ok)
            {
                return 0x203;
            }
            for (;;)
            {
                // ORIG 1939-1975：两次 ScanInt 结果**直接落进 g 的字段槽**
                // （-0x56c = g.m_type0、-0x568 = g.m_weight04），界检读回 g.m_type0，
                // 累计项取 g.m_weight4 —— 全程无 weight/v 中转量。
                OptionGroup g;
                g.m_type0 = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                if (g.m_type0 <= 0 || g.m_type0 > 0xff)
                {
                    return 0x20c;
                }
                g.m_weight4 = ScanInt(&ok);
                if (!ok)
                {
                    return 0x210;
                }
                value.m_vec0.push_back(g);
                value.m_fieldC += g.m_weight4;
            }
            if (!script.m_map160.insert(std::make_pair(type, value)).second)
            {
                LogManager::logFormat(0, "../RDARScriptRandomOption.cpp",
                                      __FUNCTION__, 0x218,
                    "importRandomOptionScript - option_group_ duplicate key error");
                return 0x219;
            }
        }
        else if (line == "[postfix]")
        {
            // [b28d-solo-RandomOptionScript] ORIG 2115 循环外 count 预清零（-0x20），
            // 2116 再 ScanInt 赋值（P198③ 两段式；直存计数槽，无 v 中转）。
            int count = 0;
            count = ScanInt(&ok);
            if (!ok)
            {
                return 0;
            }
            std::string str;
            for (;;)
            {
                int grade = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                std::vector<std::string> vec;
                for (int i = 0; i < count; ++i)
                {
                    str.clear();
                    if (!ScanStr(&str))
                    {
                        return 0;
                    }
                    vec.push_back(str);
                }
                script.m_prefixNormal[grade] = vec;
                vec.clear();
                for (int i = 0; i < count; ++i)
                {
                    str.clear();
                    if (!ScanStr(&str))
                    {
                        return 0;
                    }
                    vec.push_back(str);
                }
                script.m_prefixPremium[grade] = vec;
            }
        }
        else if (line == "[choose postfix]")
        {
            // [b28d-solo-RandomOptionScript] ORIG 2254-2256：循环外声明=0
            // （-0x14/-0x10/-0xc 三槽），循环内仅赋值（P198③ 两段式）。
            int minVal = 0;
            int maxVal = 0;
            int v = 0;
            for (;;)
            {
                minVal = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                maxVal = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                v = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                // ORIG 0x8a7301f：ChoosePrefix 临时对象按右值 push_back（EOS0_ 移动重载）。
                script.m_choosePrefix1d8.push_back(ChoosePrefix(minVal, maxVal, v));
            }
        }
        else if (line == "[break seal cost]")
        {
            // [b28d-solo-RandomOptionScript] ORIG 2308 段入口 cost 预清零（-0x58c），
            // 循环内赋值（回跳点 2312 跳过清零）。
            int cost = 0;
            for (;;)
            {
                // ORIG 2251-2287：cost 槽先清 0，随后三次 ScanInt 结果**直接落键对象**
                // （byte0 = k1 → -0x588；**ushort@+2 = k2 → -0x586**；byte1 = k3 →
                // -0x587），末次直存 cost(-0x58c) —— 无 k1/k2/k3 中转量、无移位或链。
                BreakSealCostKeyView keyView;
                keyView.m_bytes.m_b0 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    break;
                }
                keyView.m_bytes.m_w2 = static_cast<unsigned short>(ScanInt(&ok));
                if (!ok)
                {
                    return 0x28f;
                }
                keyView.m_bytes.m_b1 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    return 0x293;
                }
                cost = ScanInt(&ok);
                if (!ok)
                {
                    return 0x297;
                }
                if (!script.m_map178.insert(std::make_pair(keyView.m_key, cost)).second)
                {
                    LogManager::logFormat(0, "../RDARScriptRandomOption.cpp",
                                          __FUNCTION__, 0x29b,
                        "importRandomOptionScript - break_seal_cost_ duplicate key error");
                    return 0x29c;
                }
            }
        }
        else if (line == "[option text]")
        {
            std::string str;
            for (;;)
            {
                v = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                str.clear();
                if (!ScanStr(&str))
                {
                    return 0x2ab;
                }
                // ORIG：读到的文本行不写入任何成员（丢弃）。
            }
        }
        else if (line == "[different weight]")
        {
            for (;;)
            {
                // ORIG 2389/2399：两次 ScanInt 结果直接落 short 键的两字节槽
                DifferentWeightKeyView keyView;
                keyView.m_bytes.m_b0 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    break;
                }
                keyView.m_bytes.m_b1 = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    return 0x2ba;
                }
                int v2 = ScanInt(&ok);
                if (!ok)
                {
                    return 0x2be;
                }
                int v3 = ScanInt(&ok);
                if (!ok)
                {
                    return 0x2c2;
                }
                if (v3 <= v2)
                {
                    return 0x2c5;
                }
                if (v3 > 0x64 || v3 < 0)
                {
                    return 0x2c8;
                }
                if (v2 > 0x64 || v2 < 0)
                {
                    return 0x2cb;
                }
                DifferentWeightValue val;
                val.m_field0 = v2;
                val.m_field4 = v3;
                if (!script.m_mapf4.insert(std::make_pair(keyView.m_key, val)).second)
                {
                    LogManager::logFormat(0, "../RDARScriptRandomOption.cpp",
                                          __FUNCTION__, 0x2cf,
                        "importRandomOptionScript - different_weight_ duplicate key error");
                    return 0x2d0;
                }
            }
        }
        else if (line == "[changed postfix]")
        {
            std::string str;
            ScanStr(&str);
            script.m_str1e4 = str;
        }
        else if (line == "[postfix grade modification]")
        {
            unsigned char key = 0;
            for (;;)
            {
                key = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    break;
                }
                v = ScanInt(&ok);
                if (!ok)
                {
                    return 0x2eb;
                }
                // ORIG 0x08a73522：此行 ScanInt 结果仅消费、不入 ResetValue（丢弃）。
                int minVal = ScanInt(&ok);
                if (!ok)
                {
                    return 0x2ef;
                }
                int maxVal = ScanInt(&ok);
                if (!ok)
                {
                    return 0x2f3;
                }
                int weight = ScanInt(&ok);
                if (!ok)
                {
                    return 0x2f7;
                }
                ResetValue rv;
                rv.m_min = minVal;       // +0x00
                rv.m_max = maxVal;       // +0x04
                rv.m_weight8 = weight;   // +0x08
                std::map<unsigned char, ResetValues>::iterator it =
                    script.m_map1e8.find(key);
                if (it != script.m_map1e8.end())
                {
                    it->second.m_count += weight;
                    it->second.m_values.push_back(rv);
                }
                else
                {
                    ResetValues rvs;
                    rvs.m_count = weight;   // ORIG 0x08a73678：not-found 直接置本行 weight
                    rvs.m_values.push_back(rv);
                    script.m_map1e8.insert(std::make_pair(key, rvs));
                }
            }
        }
        else if (line == "[unable to modify postfix grade]")
        {
            // [b28d-solo-RandomOptionScript] ORIG 2682/2685 两段式：独立 unsigned long
            // key 循环外预清零（-0x5c8），循环内直存 ScanInt 结果后按引用 insert。
            unsigned long key = 0;
            for (;;)
            {
                key = (unsigned long)ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                script.m_set200.insert(key);
            }
        }
        else if (line == "[gradeless postfix]")
        {
            // [b28d-solo-RandomOptionScript] ORIG 2709/2712 两段式（-0x5cc 预清零）
            unsigned long key2 = 0;
            for (;;)
            {
                key2 = (unsigned long)ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                script.m_set218.insert(key2);
            }
        }
    }
    return 0;
}

void OPTION_VALUE_RATIO::clear()
{
    m_vec0.clear();
    m_c = 0;
}

RandomOptionGive::RandomOptionGive()
{
    clear();
}

RandomOptionGive::RandomOptionGive(const RandomOptionGive& other)
    : m_vec0(other.m_vec0)
{
    m_c = other.m_c;
}

RandomOptionGive::~RandomOptionGive()
{
}

void RandomOptionGive::clear()
{
    m_vec0.clear();
    m_c = 0;
}

RandomOptionScript::RandomOptionScript()
{
    clear();
}

RandomOptionScript::~RandomOptionScript()
{
}

void RandomOptionScript::clear()
{
    m_map0.clear();
    m_map18.clear();
    m_map30.clear();
    m_map48.clear();
    m_baseItems60.clear();
    m_map78.clear();
    m_map90.clear();
    m_veca8.clear();
    m_mapb4.clear();
    m_mapcc.clear();
    m_ratioE4.clear();
    m_mapf4.clear();
    m_costVec10c.clear();
    m_map118.clear();
    m_map130.clear();
    // ORIG 0x08372648/0x083726a2：+0x130 clear 后、+0x160 clear 前，
    // 依序 set(&+0x90,&+0xcc) / set(&+0x118,&+0x130)。
    m_providerNormal148.set(&m_map90, &m_mapcc);
    m_providerModified154.set(&m_map118, &m_map130);
    m_map160.clear();
    m_map178.clear();
    m_map190.clear();
    m_prefixNormal.clear();
    m_prefixPremium.clear();
    // ORIG 0x083838c4 起：+0x1d8 vector clear 后直接 +0x1e8 map clear，
    // 中间的 m_str1e4（ORIG C1/D1 实证 +0x1e4 string）不参与 clear()。
    m_choosePrefix1d8.clear();
    m_map1e8.clear();
    m_set200.clear();
    m_set218.clear();
}

// ORIG 0x08a73a80（非 const，_ZN mangled；循环前缓存 vector 指针，
// 命中存 result 后 break 汇合返回——对齐 ORIG -O0 布局）。
int RandomOptionScript::getPrefixGrade(int level)
{
    int result = 0;
    std::vector<ChoosePrefix>& vec = m_choosePrefix1d8;
    for (unsigned int i = 0; i < vec.size(); ++i)
    {
        if (level >= vec[i].m_min && level < vec[i].m_max)
        {
            result = vec[i].m_grade - 1;
            break;
        }
    }
    return result;
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
std::string RandomOptionScript::getPrefix(int prefix, int grade, bool flag)
{
    std::string out;
    // ORIG 0x8a73956 逐条：①第三参数先拷入本地 byte 槽（`mov %al,-0x3c(%ebp)`），
    // 此后两次以该槽作 `cmpb $0` 判；②表指针**先无条件取 m_prefixNormal(+0x1a8)**
    // 存 -0x28，再在 flag 真时用 m_prefixPremium(+0x1c0) 覆盖（非三目选择形）；
    // ③getPrefixGrade 结果入槽后条件 +3；④find/end/pt 全走非 const iterator。
    // [b28d-28] 该 byte 槽是 4.4 对 bool 形参的自动截断 spill；再写具名
    // `bool premium = flag;` 会多一对 movzbl+movb 且把 it/end 槽顶深 4（P244 配套）。
    std::map<int, std::vector<std::string> >* pMap = &m_prefixNormal;
    if (flag)
    {
        pMap = &m_prefixPremium;
    }
    int idx = getPrefixGrade(grade);
    if (flag)
    {
        idx += 3;
    }
    std::map<int, std::vector<std::string> >::iterator it = pMap->find(prefix);
    // [b28d-28] ORIG 0x08a73956 实为「具名 vector 指针 + unsigned 下标 while
    // 扫描」：`pVec=&(*it).second; i=0; while(pVec->size()>i){ if(idx==(int)i){
    // out=(*pVec)[i]; break; } ++i; }`（ORIG +0xac..+0x0fa）。原越界防御
    // （idx>=0 && idx<size()）是多余语义，ORIG 无此判断。
    if (it != pMap->end())
    {
        // [b28d-28] p167 实测：ORIG 调 `ptEv`（operator->），非 `deEv`（operator*）。
        std::vector<std::string>* pVec = &it->second;
        unsigned int i = 0;
        while (pVec->size() > i)
        {
            if (i == (unsigned int)idx)
            {
                out = (*pVec)[i];
                break;
            }
            ++i;
        }
    }
    return out;
}

int RandomOptionScript::getOptionModificationCost(short level, ENUM_RARITY rarity) const
{
    for (std::vector<OPTION_MODIFICATION_COST>::const_iterator it =
             m_costVec10c.begin();
         it != m_costVec10c.end(); ++it)
    {
        if (level <= (*it).m_level)   // ORIG 用 operator*（de）
        {
            if (rarity == RARITY_RARE)
            {
                return (*it).m_cost2;
            }
            if (rarity == RARITY_UNIQUE)
            {
                return (*it).m_cost3;
            }
        }
    }
    return -1;
}
