// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 ScriptLeafA2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <utility>

#include "ScriptLeafA2/getChangeStatusType.inc"
#include "ScriptLeafA2/ConvertWeekdayStringToInt.inc"
#include "ScriptLeafA2/getItemRarity.inc"
#include "ScriptLeafA2/getCharacterJobToEnumValue.inc"

#include "CDataManager.h"        // stNPCCommonData_t / RandomOptionItem_Regenerator /
                                 // eventReward / StageMapList / APSystem 域权威声明
#include "APSystemTypes.h"       // CActionPointEx / _MedalRewardItem 权威布局
#include "StrikerScript.h"       // StrikerScript 及子类型（ContentsType 等）
#include "CItemAmplifier.h"      // upgrade_table_t / upgrade_info_t 权威布局
#include "CItem.h"               // CItem::GetMedalPrice 声明与 m_field14
#include "DNFLexWrapper.h"       // loadRDARScriptFile / ScanType / ScanInt / ScanFloat / ScanStr
#include "CUserProc.h"          // APSystem::_ActionGroupIndex 完整定义
#include "CEnvironment.h"        // less_TCHAR / SpinLock
#include "LogManager.h"          // LogManager::logFormat

// ORIG B 全局（定义 InitScriptProducers_5.cpp；同布局视图访问：
// STScriptFileList +0x00 即 std::map<int,std::string>，sizeof 0x18）。
namespace
{
class StageScriptFileListView
{
public:
    std::map<int, std::string> m_fileMap;   // +0x00
};
}  // namespace
extern StageScriptFileListView g_stageMapScriptFileList;    // B 0x09494e20
extern char g_npcScriptBaseDirectory[0x100];                // B 0x094fe680

// ============================================================================
// convertTabTypeStringToEnum —— ORIG T 0x08aa7d98
// 报告 _lt_global_gt_/convertTabTypeStringToEnum.md 全文转写。
// 函数级静态 __lookup(map<char const*,int,less_TCHAR>)/__init_lock(SpinLock)/
// __inited 三件套：首调登记 general/specific/despair/event → 0x11/0x14/0x17/0x1a；
// 之后查表命中返回 0..3，未命中返回 4。
// ============================================================================
#include "ScriptLeafA2/convertTabTypeStringToEnum.inc"
{
    // [b28d-solo-ScriptLeafA2] 结构按 ORIG 0x08aa7d98 还原（报告 Ghidra C 全文）：
    //   ret 局部 + 合并 switch——FIND 命中值（0x11/0x14/0x17/0x1a）与初始化哨兵
    //   0 共用同一 switch（case 0 与 0x11 共块，P163 的"补未列 case 0"同族）；
    //   各 case 的 `!__inited` 分支插入、else 返回 N；未命中 ret=-1 落 switch 外。
    //   已修：①insert 实参改 map::value_type 直构（P315）→ p167 CLEAR（29=29）；
    //   ②迭代器改 `!= end`（ORIG operator!=）；③__inited 改 bool + `!` 极性复现
    //   ORIG `movzbl;xor $1;test;je`（P168a/N122）。帧 sub $0x8c 与 ORIG 全等。
    //   本波（fresh）再修三处极性/布局（P292/P222/P59 族，ORIG 0x08aa7d98）：
    //   ① 头段 ORIG 为 **正极性直测** `movzbl __inited; test; je <lock 段>`
    //      ⇒ 源码写 `if (__inited) {find} else {lock}`（then=find 落 fall-through，
    //      lock 段在后），消去 4.4 对 `!` 的 `xor $1` 并翻正块序；
    //   ② 尾段 ORIG 为 `movzbl; xor $1; test; je <nop;mov $4>` ⇒ 源码写
    //      `if (!__inited) {leave; __inited=1; continue;} return 4;`（leave 段
    //      fall-through、return 4 在 else 后）；
    //   ③ `int ret = 0;` → `int ret;`（ORIG 函数头无 `movl $0,-0x1c` 初始化，
    //      仅 else 段赋值 ret=0，P197③ 两段式）。
    static std::map<const char*, int, less_TCHAR> __lookup;
    static SpinLock __init_lock;
    static bool __inited = false;

    int ret;
    for (;;)
    {
        // ① 正极性：`if (__inited)` 直测（4.4 对 bool 局部 `!` 恒 xor，N122）
        if (__inited)
        {
            std::map<const char*, int, less_TCHAR>::const_iterator it =
                __lookup.find(text);
            // ORIG 0x08aa7ea7 段调 operator!=（非 ==，p167 EXTRA ==/MISSING !=）。
            ret = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                continue;
            }
            ret = 0;
        }

        switch (ret)
        {
        case 0:
        case 0x11:
            // p167：ORIG 直接调 value_type::C1<char const(&)[N], int>（无 make_pair
            // 中转），按 P315 用 map::value_type 直接构造。极性写 `!__inited`
            // 复现 ORIG 0x08aa7fc0 段 `movzbl;xor $1;test;je`（P168a/N122）。
            if (!__inited)
            {
                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type(
                    "general", 0x11));
            }
            else
            {
                return 0;   // general
            }
        case 0x14:
            if (!__inited)
            {
                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type(
                    "specific", 0x14));
            }
            else
            {
                return 1;   // specific
            }
        case 0x17:
            if (!__inited)
            {
                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type(
                    "despair", 0x17));
            }
            else
            {
                return 2;   // despair
            }
        case 0x1a:
            if (!__inited)
            {
                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type(
                    "event", 0x1a));
            }
            else
            {
                return 3;   // event
            }
        }

        // ② 尾段固定形：`!__inited` ⇒ leave+置位+回跳落 fall-through，
        //    `return 4` 走 else 后的尾岛（ORIG 0x08aa80d9 段 `xor $1; test;
        //    je <nop;mov $4>`）。
        if (!__inited)
        {
            __init_lock.leave();
            __inited = 1;
            continue;
        }
        break;      // 未命中：落到函数尾的单一 return（4.4 尾块 fall-through）
    }
    return 4;
}


// ============================================================================
// stNPCCommonData_t::clear —— ORIG W 0x089fae9e（objdump 全文核实）：
// 仅清标量——循环 i=0..9 写 favorExp[i]（movw $0,0x2(%eax,%edx,2)，cmpl $0x9/setle），
// 再按 +0x00→+0x34→+0x30→+0x32 顺序清四个标量；无任何容器 clear。
// ============================================================================
#include "ScriptLeafA2/clear.inc"
{
    for (int i = 0; i <= 9; ++i)
    {
        m_favorExp[i] = 0;
    }
    m_maxFavorExp = 0;
    m_maxFavorLevelNpc = 0;
    m_favorSystemBeginQuest = 0;
    m_favorSystemBeginLevel = 0;
}

// ============================================================================
// loadNPCCommonData —— ORIG T 0x089fafc5
// 报告 _lt_global_gt_/loadNPCCommonData.md 全文转写。
// 返回：0x3c=脚本打开失败；其余路径一律 0（解析失败与干净 EOF 同值，ORIG 如此）。
// ============================================================================
#include "ScriptLeafA2/loadNPCCommonData.inc"
{
    // [b28d-solo-ScriptLeafA2] 本件修复：
    //   ① CDataManager.h stOneBuff_t::m_isNotPlus int→bool（P248②/P32）复现
    //      ORIG 0x089fb390/0x089fb396 的 `movb $0/$1,-0x50(%ebp)`；
    //   ② 同结构补 +0x09..+0x0b 显式 pad，使 new_allocator::construct 对 +0x08
    //      恢复 ORIG 0x8a01d24 的 dword 块拷贝（bool 单独拷贝退化为 movzbl+movb）；
    //   ③ make_pair 实例改右值版（P61/P81 + P234 static_cast<T&&>）复现 ORIG
    //      0x089fb40d 的 make_pair<int,vector>(int&&,vector&&)。
    //   p167：MISSING 0（CLEAR）。残余 EXTRA = stOneBuff_t::~stOneBuff_t() ×2
    //   （4.4 对含非平凡析构的局部对象恒布 EH cleanup，ORIG 4.1 消除空体析构调用，
    //   N91 族跨版本 EH 块；头内空体实验无效已回退）⇒ N 封顶，语义等价。
    if (!loadRDARScriptFile(g_npcScriptBaseDirectory, path))
    {
        return 0x3c;
    }

    std::string token;
    int idx = 0;
    bool ok = false;

    out.clear();

    for (;;)
    {
        if (!ScanType(token, true))
        {
            break;
        }

        if (token == "[npc favor exp]")
        {
            for (;;)
            {
                idx = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                out.m_favorExp[idx] = static_cast<unsigned short>(ScanInt(&ok));
                if (!ok)
                {
                    return 0;
                }
                out.m_maxFavorExp =
                    std::max(out.m_maxFavorExp, out.m_favorExp[idx]);
            }
        }
        else if (token == "[favorable npc list]")
        {
            for (;;)
            {
                idx = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                out.m_favorableNpcList.push_back(idx);
            }
        }
        else if (token == "[illustration variable npc list]")
        {
            for (;;)
            {
                idx = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                out.m_illustrationVariableNpcList.push_back(idx);
            }
        }
        else if (token == "[max favor level npc]")
        {
            out.m_maxFavorLevelNpc = static_cast<unsigned short>(ScanInt(&ok));
            if (!ok)
            {
                break;
            }
        }
        else if (token == "[favor system begin quest]")
        {
            out.m_favorSystemBeginQuest = static_cast<unsigned short>(ScanInt(&ok));
            if (!ok)
            {
                break;
            }
        }
        else if (token == "[favor system begin level]")
        {
            out.m_favorSystemBeginLevel = static_cast<unsigned short>(ScanInt(&ok));
            if (!ok)
            {
                break;
            }
        }
        else if (token == "[favor level reward npc]")
        {
            for (;;)
            {
                int npcId = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                out.m_favorLevelRewardNpcList.push_back(npcId);
            }
        }
        else if (token == "[favor duration buff list]")
        {
            for (;;)
            {
                std::vector<stNPCCommonData_t::stOneBuff_t> buffs;
                // [本波实测] 槽序反转律在本块**无效**：把 `int first;` 提前到
                // buffs 之前重编后，first 仍落 -0x1c（ORIG -0x48），帧/计数零变化
                // ⇒ 块内标量+类对象混合区归 N98（4.4 按首次使用序）域，声明序不可翻。
                int first = ScanInt(&ok);
                if (!ok)
                {
                    break;                          // 回主循环（buffs 析构）
                }
                int count = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                for (int i = 0; i < count; ++i)
                {
                    std::string s;
                    if (!ScanStr(&s))
                    {
                        return 0;
                    }
                    int discardedType = getChangeStatusType(s);   // ORIG 存局部后弃用
                    int value = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0;
                    }
                    stNPCCommonData_t::stOneBuff_t buff;
                    buff.m_changeStatusType = getChangeStatusType(s);   // 第二次入元素
                    buff.m_value = value;
                    if (!ScanStr(&s))
                    {
                        return 0;
                    }
                    if (s == "+")
                    {
                        buff.m_isNotPlus = 0;
                    }
                    else
                    {
                        buff.m_isNotPlus = 1;
                    }
                    buffs.push_back(buff);
                }
                // [b28d-solo-ScriptLeafA2] ORIG 0x089fb40d 调
                // make_pair<int, vector<stOneBuff_t> >(int&&, vector&&)（p167 MISSING
                // 左值引用版 `<int&,vector&>`）；按 P61/P81 实参左值性决定实例名，
                // 用 P234 的 static_cast<T&&>（纯语言构造、零 call）传右值即复现。
                out.m_favorDurationBuffList.push_back(
                    std::make_pair(
                        static_cast<int>(first),
                        static_cast<
                            std::vector<stNPCCommonData_t::stOneBuff_t>&&>(buffs)));
            }
        }
    }
    return 0;
}

// ---- ORIG 全局：g_RegenerationRandomOptionScriptBaseDirectory
//      （B 0x095005c0 size 0x100，CU=RDARScriptRegeneration_ROI.cpp，与本函数同 TU）----
// char[0x100] 零初始化脚本目录缓冲（init 流程填充）。ORIG 全二进制零指令、零数据
// 引用（二进制扫描唯一命中在 .symtab 自身）⇒ 死数据；补定义对齐符号账
// （REAL_GAP_SYMBOL），不新增消费点（importRegenerationRandomOptionScript 的路径
// 参数仍按 ORIG 0x08a7e610 实参字面量 "Etc/RandomOption/..." 形式）。
// 位置说明（P327）：本 TU 首符号保持 convertTabTypeStringToEnum（ORIG 另一 TU 的
// GI 键名），故不把本变量前移；ORIG 的
// `_GLOBAL__I_g_RegenerationRandomOptionScriptBaseDirectory`（0x08a7ea66）与本 TU
// 键名互斥，属结构性 REORDER_ONLY。
char g_RegenerationRandomOptionScriptBaseDirectory[0x100];

// ============================================================================
// importRegenerationRandomOptionScript —— ORIG T 0x08a7e610
// 报告 _lt_global_gt_/importRegenerationRandomOptionScript.md 全文转写。
// 返回：0=成功/文件打开失败（ORIG 同值）；0x1d=[choose part] 首扫描失败；
//       0x5d=[regeneration price] 组重复。
// ============================================================================
#include "ScriptLeafA2/importRegenerationRandomOptionScript.inc"
                                         const char* path)
{
    // [b28d-solo-ScriptLeafA2] 判词 = N（槽序/帧差，P28/N82 族；p167 CLEAR 47=47）。
    //   已修：①4 处 token 比较改「字面量在左」复现 operator==(char const*, string)
    //   （P196 族，原 4×EXTRA 反向）；②[choose part] 去 entry 引用缓存、循环内
    //   重写 reg.m_map0[part].m_vec8（P157/P183，补回 1×operator[]）。
    //   [本波实测] 循环头改 `for(;;){ if(!ScanType(...)) break; }` 复现 ORIG
    //   0x08a7e673 的 `xor $1; test; jne`：**实测劣化**（我方 raw 300→301、
    //   OURS-only 增 setl/je、ORIG-only jl 丢失）⇒ 已回滚。ORIG 的 jne 远跳
    //   （exit 在函数尾 0x3ba）+ 4.4 恒 `je <body>` 短跳，方向不可选择 ⇒ N。
    //   残余 = 局部槽基差（ORIG token@-0x2c/s@-0x30/ok@-0x31/part@-0x38 与我方
    //   -0x24/-0x28/-0x29/-0x30 整体浅 8，帧同 0x60 ⇒ N126 版本首槽基址常数）；
    if (!loadRDARScriptFile("", path))
    {
        return 0;
    }

    std::string token;
    std::string s;
    bool ok = false;
    int part = 0;

    while (ScanType(token, true))
    {
        // [b28d-solo-ScriptLeafA2] 4 处 token 比较按 ORIG 写成「字面量在左」
        // （p167：ORIG 调 operator==(char const*, string const&)，我方原为
        // (string const&, char const*) 反向 4 处 EXTRA/MISSING，P196 族）。
        if ("[choose part]" == token)
        {
            part = ScanInt(&ok);
            if (!ok)
            {
                return 0x1d;
            }
            ScanStr(&s);
            // ORIG 循环内重新 operator[]（p167 MISSING 1×operator[]；P157/P183
            // 容器成员禁缓存引用），故不设 entry 引用。
            reg.m_map0[part].m_name4 = s;
            while (ScanStr(&s))
            {
                reg.m_map0[part].m_vec8.push_back(s);
            }
        }
        else if ("[adjust quantity ratio]" == token)
        {
            part = ScanInt(&ok);            // ORIG 不校验 ok
            ScanStr(&s);
            reg.m_map0[part].m_name4 = s;
            for (int i = 0; i < 3; ++i)
            {
                reg.m_vec30.push_back(ScanInt(&ok));
            }
        }
        else if ("[adjust value ratio]" == token)
        {
            part = ScanInt(&ok);            // ORIG 不校验 ok
            ScanStr(&s);
            reg.m_map0[part].m_name4 = s;
            _str_option_value_ratio ratio(0.0f, 0.0f, 0);  // ORIG 无默认 ctor 符号
            ratio.m_scaleRange0 = 0.0f;
            ratio.m_scaleBase4 = 0.0f;
            ratio.m_weight8 = 0;
            while (ScanFloat(&ratio.m_scaleRange0))
            {
                ScanFloat(&ratio.m_scaleBase4);
                ratio.m_weight8 = static_cast<unsigned int>(
                    ScanInt(reinterpret_cast<int*>(&ratio.m_weight8)));
                reg.m_vec3c.push_back(ratio);
                reg.m_field48 += ratio.m_weight8;
            }
        }
        else if ("[regeneration price]" == token)
        {
            int limit = ScanInt(&ok);       // ORIG 不校验 ok
            for (;;)
            {
                int group;
                if (!ScanInt(&group))
                {
                    break;
                }
                if (reg.m_map18.find(group) != reg.m_map18.end())
                {
                    printf("random option item script error %u, group : %d\n",
                           0x5c, group);
                    return 0x5d;
                }
                std::map<int, _str_cost>& costs = reg.m_map18[group];
                int key;
                do
                {
                    ScanInt(&key);          // 三连 ScanInt(int*)，均不校验（ORIG 同）
                    int a;
                    int b;
                    ScanInt(&a);
                    ScanInt(&b);
                    _str_cost cost(a, b);
                    costs[key] = cost;
                } while (key < limit);
            }
        }
    }
    return 0;
}

// ============================================================================
// StrikerScript::importScript —— ORIG T 0x08a9ddb2
// 报告 StrikerScript/importScript.md 全文转写。成功置 m_loaded=1 并返回 true。
// ============================================================================
#include "ScriptLeafA2/importScript.inc"
{
    // [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N82）→ 试=①n1_v 复核 497/498（gap −1）；
    //   ②prett（q3_gap2 修正判据）③strall 均 CLEAR；④ORIG 0x08a9e1cd 段
    //   `mov 0x8(%ebp),%eax; lea 0x78(%eax),%edx; lea -0x5c(%ebp),%eax;
    //   mov %eax,0x4(%esp); mov %edx,(%esp); call find`（先算对象地址、lea 形）vs 我方
    //   `lea -0x5c(%ebp),%eax; …; mov 0x8(%ebp),%edx; add $0x78,%edx`（先算实参、add 形）
    //   = 求值序 + 地址计算形，4.4 前端固定，源码不可控（具名 int/int& 中转旧实测 +2）。
    //   ⇒ 维持 N82。
    // [b28d-solo-ScriptLeafA2] 判词 = N（帧/槽形差，N82；p167 CLEAR 87=87）。
    //   已修：19 处 token 比较改「字面量在左」（P196 族）+ m_skillMap.find 比较改
    //   operator==（反向包裹，P222）。
    //   本波：①引入函数级 int num（ORIG -0x48）供 6 个分支复用——[tag mode]
    //   （0x08a9de49 死写入照抄，P171）、[tag cost time]（0x08a9de70/79 两段中转）、
    //   [striker start time]（0x08a9def9/df02）、[striker dungeon impossible]
    //   （0x08a9e035/53 while(ScanInt(&num))）、[striker skill]（0x08a9e10d）、
    //   [striker skill macro]（0x08a9e2e8）；②[striker skill] 还原 5 个具名 int
    //   局部 a..e（ORIG -0x30..-0x20，0x08a9e129..0x08a9e198 先取值后逐字段拷进
    //   skill，帧差 0x10 的来源）。
    //   读数（本波 fresh）：orig 490 / our 487（gap 21→10→3），ndiff 60→57。
    //   残余 = ①ORIG-only {xor:4, je:3} vs OURS {jne:3}：ORIG 0x08a9e040
    //   `xor $1; test %al,%al; je <body>` 为 4.1 对 `while(ScanInt(&num))`/`if(!ok)`
    //   的跨版本 bool 归一化（4.4 恒折 `test;jne` 顶掉 xor）⇒ Main 已定 N 族
    //   （P292/P168a③，勿恋战）；②OURS {movzwl:2, shl:1, or:1} = [striker skill]
    //   key 打包 `(ushort)a | (uint)(ushort)b<<16` 的零扩展链（ORIG 用形参槽直读）。
    //   语义等价（p167 CLEAR 87=87）。
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    std::string token;
    std::string s;
    bool ok = false;
    // ORIG 0x08a9dde5 段：`movb $0x0,-0x41(%ebp)`（ok 预清）后紧跟
    // `movl $0x0,-0x48(%ebp)`（num 预清）⇒ 两句式带初值（P198①）。
    int num = 0;                                    // ORIG -0x48：多分支复用（P171 死写入照抄）

    // [b28d-L4a 2026-09-11] ORIG 0x08a9de09 段为 **top-tested** 形：
    // `+0x051 jmp +0x57 / +0x054 jmp +0x57`（双空块入口）→ +0x57 cond →
    // `xor $1; test %al; jne +0x6be`（**失败跳 END**）→ body → 尾部 `jmp +0x57`
    // ⇒ 原码是 `for (;;) { if (!ScanType(...)) break; ... }`，非 `while(...)`
    // （我方旧式产 bottom-tested `jmp cond / body / cond: jne body`）。
    for (;;)
    {
        if (!ScanType(token, true))
        {
            break;
        }
        if ("[tag mode]" == token)
        {
            num = ScanInt(&ok);                     // 仅存局部、后续不读（ORIG 0x08a9de49 死写入）
        }
        else if ("[tag cost time]" == token)
        {
            num = ScanInt(&ok);                     // ORIG 0x08a9de70/0x08a9de79 两段中转
            m_tagCostTime = num;
        }
        else if ("[tag character setting]" == token)
        {
            m_tagCharacterSetting = ScanInt(&ok);
        }
        else if ("[striker combo]" == token)
        {
            m_strikerCombo = ScanInt(&ok);
        }
        else if ("[striker start time]" == token)
        {
            num = ScanInt(&ok);                     // ORIG 0x08a9def9/0x08a9df02 两段中转
            m_strikerStartTime = num;
        }
        else if ("[striker skill data up]" == token)
        {
            importSkillDataUpVector();
        }
        else if ("[striker skill attack penalty]" == token)
        {
            importSkillAttackPenalty();
        }
        else if ("[striker skill cooltime penalty]" == token)
        {
            importSkillCoolTimePenalty();
        }
        else if ("[striker level penalty]" == token)
        {
            importParentLevelPenlaty();
        }
        else if ("[striker module impossible]" == token)
        {
            m_moduleImpossible.clear();
            // ORIG 0x08a9dfc0 段同形：`xor $1; test; je <body>` + fall-through
            // `nop; jmp +0x57`（失败 break 回外层 cond）⇒ top-tested。
            for (;;)
            {
                if (!ScanStr(&s))
                {
                    break;
                }
                m_moduleImpossible.push_back(
                    static_cast<ContentsType::T>(ContentsType::getType(s.c_str())));
            }
        }
        else if ("[striker dungeon impossible]" == token)
        {
            m_dungeonImpossible.clear();
            // ORIG 0x08a9e035/0x08a9e053 段同形（失败 break 回外层 cond）。
            for (;;)
            {
                if (!ScanInt(&num))                 // ORIG 复用 -0x48
                {
                    break;
                }
                m_dungeonImpossible.push_back(num);
            }
        }
        else if ("[striker end time]" == token)
        {
            m_strikerEndTime = ScanInt(static_cast<bool*>(0));
        }
        else if ("[position]" == token)
        {
            m_positionType = ScanInt(static_cast<bool*>(0));   // +0xac
            m_field38 = ScanInt(static_cast<bool*>(0));        // +0x38
            m_field3c = ScanInt(static_cast<bool*>(0));        // +0x3c
        }
        else if ("[striker skill]" == token)
        {
            for (;;)
            {
                StrikerSkill skill;
                if (!ScanInt(&num))                 // ORIG 复用 -0x48（0x08a9e10d）
                {
                    break;                              // 扫描失败退出该段
                }
                // ORIG 0x08a9e129..0x08a9e198：5 个具名 int 局部（-0x30..-0x20）
                // 先取值，再逐字段拷进 skill（帧差 0x10 的来源）。
                int a = num;                        // -0x30（0x08a9e129）
                int b = ScanInt(static_cast<bool*>(0));      // -0x2c
                int c = ScanInt(static_cast<bool*>(0));      // -0x28
                int d = ScanInt(static_cast<bool*>(0));      // -0x24
                int e = ScanInt(static_cast<bool*>(0));      // -0x20
                ScanStr(&s);
                skill.m_field00 = c;                // -0x58（0x08a9e176）
                skill.m_field04 = d;                // -0x54
                skill.m_field08 = e;                // -0x50
                skill.m_name = s;                   // -0x4c（0x08a9e198 operator=）
                // map 键 = 两个截断 ushort 打包（栈 -0x5c/-0x5a 对）
                // ORIG 0x08a9e19d..0x08a9e1b4 逐条：`mov -0x30,%eax; mov %ax,-0x5c`
                // （+0x00 写 2 字节）→ `mov -0x2c,%eax; mov %ax,-0x5a`（+0x02 写 2 字节）
                // → `lea -0x5c(%ebp),%edx; mov %edx,0x8(%esp)` 传该 4 字节对象**地址**给
                // find（无 32 位读）⇒ 原码 = 4 字节具名结构（两 ushort 成员，成员必相邻）
                // + 以 int 引用传参。（旧位移/或打包在 4.4 多出 movzwl/shl/or 链 +3 条；
                // 两个独立 ushort 局部实测 4.4 不保证相邻 ⇒ 槽布局错。）
                struct StrikerSkillKey
                {
                    unsigned short lo;      // +0x00
                    unsigned short hi;      // +0x02
                };
                StrikerSkillKey keyParts;
                keyParts.lo = static_cast<unsigned short>(a);
                keyParts.hi = static_cast<unsigned short>(b);
                // [b28d-L4a 2026-09-11] 负结论：具名 `int& keyRef = *(int*)&keyParts`
                // 实测 4.4 为引用发额外槽（our 499，+2）⇒ 保持直接 (int*)&keyParts
                // 表达式形（our 497 = ORIG 497）。
                std::map<int, std::vector<StrikerSkill>*>::iterator it =
                    m_skillMap.find(*reinterpret_cast<int*>(&keyParts));
                // p167：ORIG 调 _Rb_tree_iterator::operator==（非 !=）⇒ 反向包裹。
                if (it == m_skillMap.end())
                {
                    std::vector<StrikerSkill>* vec = new std::vector<StrikerSkill>;
                    vec->push_back(skill);
                    m_skillMap[*reinterpret_cast<int*>(&keyParts)] = vec;
                }
                else
                {
                    it->second->push_back(skill);
                }
            }
        }
        else if ("[striker skill macro]" == token)
        {
            m_macroCommands.clear();
            for (;;)
            {
                if (!ScanInt(&num))                 // ORIG 复用 -0x48（0x08a9e2e8）
                {
                    break;
                }
                StrikerSkillAddCommand cmd;
                cmd.m_field00 = num;
                cmd.m_field04 = ScanInt(static_cast<bool*>(0));
                cmd.setType(ScanInt(static_cast<bool*>(0)));
                m_macroCommands.push_back(cmd);
            }
        }
        else if ("[common cool time]" == token)
        {
            m_commonCoolTime = ScanInt(static_cast<bool*>(0));
        }
        else if ("[direction type]" == token)
        {
            m_direction.setType(ScanInt(static_cast<bool*>(0)));
        }
        else if ("[contents revision]" == token)
        {
            m_contentsRevision.clear();
            for (;;)
            {
                if (!ScanStr(&s))
                {
                    break;
                }
                ContentsRevision rev;
                rev.m_type = ContentsType::getType(s.c_str());
                rev.m_field04 = ScanInt(static_cast<bool*>(0));
                rev.m_field08 = ScanInt(static_cast<bool*>(0));
                rev.m_field0c = ScanInt(static_cast<bool*>(0));
                m_contentsRevision.push_back(rev);
            }
        }
        else if ("[upgrade revision]" == token)
        {
            m_upgradeRevision = ScanInt(static_cast<bool*>(0));
        }
    }

    m_loaded = 1;
    return true;
}

// ============================================================================
// StageMapList::compare —— ORIG T 0x08364848（比较 +0 uniqueId）。
// ============================================================================
#include "ScriptLeafA2/compare.inc"
{
    return a.m_uniqueId < b.m_uniqueId;
}

// ---- stagemap 域镜像（布局取自 loadList 报告汇编偏移；类型 ORIG 真实存在，
//      完整建模归 stagemap 域批次）----
namespace stagemap
{

struct StageData
{
    int m_field0;                            // +0x00
    int m_field4;                            // +0x04 (ORIG StageData stride 0x10)
    int m_field8;                            // +0x08
    int m_fieldC;                            // +0x0c
};

struct DistrictData
{
    int m_field0;                            // +0x00
    std::vector<StageData> m_vec4;           // +0x04 (ORIG StageData vector)
    std::string m_appendImage;              // +0x10 (ORIG std::string size 4)
    int m_field14;                           // +0x14
};

struct AreaData
{
    int m_field0;                            // +0x00
    char m_pad04[0x1c];                      // +0x04
    std::vector<DistrictData> m_vec20;       // +0x20
    int m_field2c;                           // +0x2c
};

class StageMapScript
{
public:
    StageMapScript();                        // ORIG W 0x08373488
    ~StageMapScript();                       // ORIG W 0x0837f782
    void clear();                            // ORIG W 0x08373428

    char m_pad00[0x14];                      // +0x00（未还原字段区）
    std::vector<AreaData> m_areaVec;         // +0x14
};

}  // namespace stagemap

// ORIG T 0x088a9f28（定义待 stagemap 脚本域批次；本 TU 仅声明引用）
#include "ScriptLeafA2/importStageMapScript.inc"

// ============================================================================
// StageMapList::loadList —— ORIG T 0x0836485c
// 报告 StageMapList/loadList.md 全文转写。
// ============================================================================
#include "ScriptLeafA2/loadList.inc"
{
    // [b28d-solo-ScriptLeafA2] 判词 = N（§6.2 口径，残余帧/槽差；p167 CLEAR 55=55）。
    //   已修：三层遍历由索引 size()/operator[] 改迭代器（begin/end/operator!=/
    //   operator->/operator++(int)，P28 后置 ++）+ ORIG 的容器引用缓存（Ghidra C
    //   `local_20 = local_20 + 0x20`）+ 尾循环条件/体内各调 summaries.size()
    //   （ORIG 三处调用点）⇒ 本件 +18 AE。
    //   [本波实测·三形均未翻正，判 N] ORIG 0x08364875 `movl $0x0,-0x28(%ebp)`
    //   （预清）→ 0x083648b3 `mov %eax,-0x28`（= it->second.c_str()）→ 0x083648b6
    //   `cmpl $0x0,-0x28; je`（判空 break）⇒ 原码是函数级具名 `const char* name`。
    //   实测三形：(a) 循环内 `const char* name = c_str()`（现状）gap 3 / ndiff 359；
    //   (b) 补 `int ok = 0;` 两句式 gap 1 / ndiff 57；(c) 函数级
    //   `const char* name = 0;`+循环内赋值 gap 2 / ndiff 360。三形均为 DIFF（分类
    //   不变），且 (c) 反而劣化 ⇒ 帧差（我方 0xe0 vs ORIG 0xf0，判据⑪ 整帧平移，
    //   N126）非声明序可修；恢复语义最干净形。ORIG 0x0836485c，p167 CLEAR 55=55。
    // [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N126）→ 试=④P339/P232：按 ORIG 帧 0xf0
    //   （我方 0xe0）补函数级 `int pad;` ⇒ 帧对齐 0xf0，但 name 槽仍差 4（ORIG -0x28
    //   vs 我方 -0x24）且 4.4 对未初始化 int 恒发 `movl $0x0,-0x24`（+1 条）⇒ 槽号不可控，
    //   已回滚（356/356 gap 0 与原状同）。⇒ 维持 N126。
    std::vector<_StageMapSummary> summaries;
    // [b28d-L4a 2026-09-11] ORIG 0x08364875 `movl $0x0,-0x28(%ebp)`（summaries
    // ctor 之后、begin() 之前）⇒ 函数级具名 `name` 带初值 0，循环内 `mov %eax,-0x28`
    // 覆写（形 (c) 复原）。
    const char* name = 0;

    for (std::map<int, std::string>::iterator it =
             g_stageMapScriptFileList.m_fileMap.begin();
         it != g_stageMapScriptFileList.m_fileMap.end(); it++)
    {
        name = it->second.c_str();
        if (name == 0)
        {
            break;
        }

        stagemap::StageMapScript script;
        if (!importStageMapScript(&script, name))
        {
            printf("Stage Map Script Error: %s\n", name);
            return false;
        }

        // p167：ORIG 三层均为迭代器遍历（begin/end/operator!=/operator->/
        // operator++(int)），非索引 + size()/operator[]；后置 ++ 由 P28 增量侧
        // 推论（ORIG 增量段调 ppEi）。
        for (std::vector<stagemap::AreaData>::iterator ai =
                 script.m_areaVec.begin();
             ai != script.m_areaVec.end(); ai++)
        {
            int districtId = 0;
            // ORIG 缓存容器引用（Ghidra C：`local_20 = local_20 + 0x20` 即 &ai->m_vec20
            // 一次取址；p167 显示我方 begin/end 各多一次 operator->）。
            std::vector<stagemap::DistrictData>& districts = ai->m_vec20;
            for (std::vector<stagemap::DistrictData>::iterator di =
                     districts.begin();
                 di != districts.end(); di++)
            {
                int stageIdx = 0;
                std::vector<stagemap::StageData>& stages = di->m_vec4;
                for (std::vector<stagemap::StageData>::iterator si =
                         stages.begin();
                     si != stages.end(); si++)
                {
                    _StageMapSummary sum;
                    // [b28d-L4a 2026-09-11] 真语义修复（字段错位轴③）：ORIG
                    // 0x083649ad `movb $0x0,-0xa0(%ebp)` = sum+0xc（1 字节），
                    // 0x08364b09 `movb $0x1,-0xb0(%ebp)` = sum+0xc（幕区收尾）；
                    // ORIG 构造段对 +0x08（int m_nextId，尾循环 0x08364cec 起
                    // `mov 0x4(%eax),%eax; mov %eax,0x8(%ebx)` 写入）**只靠 C1
                    // memset 清零**。我方旧式 `sum.m_nextId = 0/1`（movl 写 +8）
                    // 使 +0xc 的 bool 永远为 0 ⇒ getNextStage 0x08364e58 读该字节
                    // 时 ORIG=1 / 我方=0，行为分叉。
                    sum.m_flags = 0;
                    sum.m_stageId = si->m_field0;
                    sum.m_uniqueId =
                        makeStageMapUniqueId(ai->m_field0, di->m_field0,
                                             stageIdx);
                    summaries.push_back(sum);
                    ++stageIdx;
                }
                districtId = di->m_field0;
            }
            // ORIG 0x08364ad6 `movl $0x0,-0x18(%ebp)`（stageIdx2 预清）**先于**
            // 0x08364add `addl $0x1,-0x20(%ebp)`（districtId += 1）⇒ 语句序复原。
            int stageIdx2 = 0;
            districtId += 1;
            if (0 < ai->m_field2c)
            {
                _StageMapSummary sum;
                sum.m_flags = 1;      // ORIG 0x08364b09 `movb $0x1,-0xb0(%ebp)`
                sum.m_stageId = ai->m_field2c;
                sum.m_uniqueId =
                    makeStageMapUniqueId(ai->m_field0, districtId, stageIdx2);
                summaries.push_back(sum);
            }
        }
    }

    std::sort(summaries.begin(), summaries.end(), compare);

    // ORIG 0x08365… 尾循环条件每轮调 size()，体内另调一次（p167 MISSING size×1；
    // 报告 Ghidra C 556-591 三处 size() 调用点）。
    for (size_t i = 0; i < summaries.size(); ++i)
    {
        if (i == 0)
        {
            // ORIG 0x08364cb4 `mov -0xc(%ebp),%eax; mov %eax,0x4(%esp)`（传**变量 i**
            // 而非立即数 0）⇒ 原码 `summaries[i]`（i==0 域内等价）。
            m_firstStageId = summaries[i].m_stageId;
        }
        if (summaries.size() - 1 != i)
        {
            summaries[i].m_nextId = summaries[i + 1].m_stageId;
        }
        m_map.insert(std::make_pair(summaries[i].m_stageId, summaries[i]));
    }
    return true;
}

// ============================================================================
// APSystem::CActionPointEtcParameter::importActionPointSystemEtcScript
// ORIG T 0x08894996（静态成员）。报告
// APSystem__CActionPointEtcParameter/importActionPointSystemEtcScript.md 全文转写。
// 返回 false=成功/文件失败；错误路径亦为 false（调用方仅判零，语义兼容；
// ORIG 各行级错误码见注释）。
// ============================================================================
namespace APSystem
{

// [b28d-P330 int] ORIG 返回 = **错误行号 int**（callee 尾 `mov %ebx,%eax; ret`，
// ebx 写点含 `mov $0x57..$0xfa,%ebx` 14 处行号常量、0 引用 ×2；caller 4B 全
// store 无 movzbl）。旧 bool 声明把行号截成 0/1 ⇒ trace "%d line" 日志值错。
#include "ScriptLeafA2/importActionPointSystemEtcScript.inc"
    const char* path, CActionPointEtcParameter& out)
{
    // [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N91）→ 试=④P342（triviality 轴）：
    //   `APSystemTypes.h:48` 用户空析构使 `APSystem::_TodayRewardItem` 非平凡 ⇒ 4.4 对
    //   栈对象 `today` 布 EH 清理（p167 EXTRA `_TodayRewardItemD*Ev`×2）；删声明+定义后
    //   812→**792**（ORIG 794，gap −18→**+2**），D1/D2 仍由 odr-use 发射（nm W 定义）。
    //   本 TU 由 Q8OldNAgain 同轮落地核账（659/1087/0/13/319 → 661/1087/0/13/318）。
    //   余 +2 = ORIG 0x088949af/0x08894a2f 两个**死局部** `movl $0x0,-0x40/-0x38`（各仅
    //   1 次写、无使用，P232）；补同样 pad 可达 794=794 但帧 0x160→0x170 槽整体平移
    //   ⇒ 未采用。⇒ 部分推翻（N91 项已消），残余归 N82/N126。
    // [b28d-solo-ScriptLeafA2] 判词 = N（残余迭代器计数/EH 清理差；p167 110=112）。
    //   已修：①EOF 段改 const_iterator + 后置 ++ + const vector&（P96/P28）；
    //   ②itemList 缓冲改 c_str() 视图（p167 原 operator[] EXTRA/c_str MISSING）；
    //   ③vector<int>::push_back 改具名 int 左值（ORIG const& 版）；④find 比较改
    //   operator==（反向包裹，P222）。残余 EXTRA = _TodayRewardItem::~_TodayRewardItem
    //   ×2（4.4 对含非平凡析构局部恒布 EH 清理，ORIG 消除，N91 族）+ const_iterator
    //   operator->/operator[] const 各 1 计数差；语义等价。
    // [轴 B 2026-09-10 · a_S->U 清单件 = **N（表内配对假阳性）**] 清单行
    //   `orig_addr=0x08894bc1 setle` 与我方 `seta` 系按出现序号交叉配对。按地址自校：
    //   ORIG 0x08894bc1 = `addl $0x1,-0x20(%ebp); cmpl $0x4,-0x20(%ebp); setle`
    //   （days 循环尾测，有符号）↔ 我方 0x1d73 同构
    //   `addl $0x1,-0x1c(%ebp); cmpl $0x4,-0x1c(%ebp); setle`（已一致）；
    //   我方 seta（0x1b3c `… call size(); cmp -0x3c(%ebp),%eax; seta`）对应
    //   ORIG 0x08895564 `addl $0x1,-0xc(%ebp); mov -0xc(%ebp),%eax;
    //   cmp -0x10(%ebp),%eax; setb` —— 两侧同为**无符号序**（仅比较方向不同）⇒ 本就一致，
    //   **禁改**（P196）。实测该件 794/811（gap −17）不变，未动一行源码。
    std::string token;
    bool ok = false;

    out.ClearData();
    if (!loadRDARScriptFile("", path))
    {
        return 0;   // ORIG +0x04c 文件缺失静默返回 0
    }

    for (;;)
    {
        if (!ScanType(token, true))
        {
            // [b28d-L4a 2026-09-11] ORIG 0x08894a0b `jne 0x08895481 (+0xaeb)`：
            // 扫描失败直接跳**函数尾部**的 EOF 段（非就地展开）⇒ 原码是
            // `for (;;) { if (!ScanType(...)) break; ... }`，EOF 登记段位于循环之后。
            break;
        }

        if (token == "[action point list]")
        {
            for (;;)
            {
                CActionPointEx ex;
                int v1 = ScanInt(&ok);
                if (!ok)
                {
                    break;                              // 回主循环
                }
                ex.m_ap.m_actionNo = static_cast<unsigned short>(v1);

                int v2 = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ex.m_field2c = static_cast<unsigned char>(v2);

                int key = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ex.m_mapKey = static_cast<unsigned short>(key);    // +0x0c map 键

                int v4 = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }

                ex.m_ap.m_groupIndex = v4;                        // +0x02 int（组索引）
                if (!IsValidActionGroupIndex((_ActionGroupIndex)v4))
                {
                    return 0x62;                        // ORIG 错误码 0x62
                }

                int v5 = ScanInt(&ok);
                if (!ok)
                {
                    return 0x65;                        // 0x65
                }
                ex.m_ap.m_days[2] = static_cast<unsigned char>(v5);   // +0x08（后被覆写）

                for (int i = 0; i <= 4; ++i)            // days 区 B+0x06..+0x0a
                {
                    int day = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0x6d;                    // 0x6d
                    }
                    if (day != 0)
                    {
                        ex.m_ap.m_days[i] =
                            static_cast<unsigned char>(-static_cast<char>(day));
                    }
                }

                std::string itemList;
                if (!ScanStr(&itemList))
                {
                    return 0x76;                        // 0x76
                }
                {
                    // strtok_r 按 "," 切分 atoi 入 vector<int>（B+0x14）
                    char* context = 0;
                    // p167：ORIG 调 string::c_str()（我方原为 operator[]）。
                    char* buf = const_cast<char*>(itemList.c_str());
                    for (char* tok = strtok_r(buf, ",", &context); tok != 0;
                         tok = strtok_r(0, ",", &context))
                    {
                        // ORIG 调 push_back(int const&)（具名 int 局部），非右值版。
                        int value = atoi(tok);
                        ex.m_itemList.push_back(value);
                    }
                }

                std::string weekdayStr;
                if (!ScanStr(&weekdayStr))
                {
                    return 0x9c;                        // 0x9c
                }
                unsigned int weekday = 0;
                if (!ConvertWeekdayStringToInt(weekdayStr, weekday))
                {
                    weekday = 7;
                }
                ex.m_weekday = static_cast<int>(weekday);

                int b9 = ScanInt(&ok);
                if (!ok)
                {
                    return 0xa4;                        // 0xa4
                }
                ex.m_ap.m_days[3] = static_cast<unsigned char>(b9);   // +0x09（覆盖 days[3]）

                int b10 = ScanInt(&ok);
                if (!ok)
                {
                    return 0xa7;                        // 0xa7
                }
                ex.m_levelMax = static_cast<unsigned char>(b10);   // +0x10

                if (!ScanStr(&ex.m_str24))
                {
                    return 0xaa;                        // 0xaa
                }
                if (!ScanStr(&ex.m_str28))
                {
                    return 0xad;                        // 0xad
                }

                std::map<unsigned short, std::vector<CActionPointEx> >::iterator fit =
                    out.m_actionPoints.find(ex.m_mapKey);
                // p167：ORIG 调 _Rb_tree_iterator::operator==（非 !=）⇒ 反向包裹。
                if (fit == out.m_actionPoints.end())
                {
                    std::vector<CActionPointEx> vec;
                    vec.push_back(ex);
                    out.m_actionPoints.insert(
                        std::make_pair(
                        ex.m_mapKey, vec));
                }
                else
                {
                    fit->second.push_back(ex);
                }
            }
        }
        else if (token == "[action point user message]")
        {
            for (;;)
            {
                int msgId = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                std::string msg;
                if (!ScanStr(&msg))
                {
                    return 0xc7;                        // ORIG 错误码 199(0xc7)
                }
                out.m_rewardMessages.insert(std::make_pair(msgId, msg));
            }
        }
        else if (token == "[action point system message]")
        {
            for (;;)
            {
                int msgId = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                std::string msg;
                if (!ScanStr(&msg))
                {
                    return 0xd5;                        // ORIG 错误码 0xd5
                }
                out.m_rewardMessages2.insert(std::make_pair(msgId, msg));
            }
        }
        else if (token == "[action point medal reward]")
        {
            for (;;)
            {
                _MedalRewardItem medal;
                medal.m_field00 = static_cast<unsigned short>(ScanInt(&ok));
                if (!ok)
                {
                    break;
                }
                medal.m_field02 = static_cast<unsigned short>(ScanInt(&ok));
                medal.m_field04 = ScanInt(&ok);
                medal.m_field08 = static_cast<unsigned short>(ScanInt(&ok));
                out.m_medalRewards.push_back(medal);
            }
        }
        else if (token == "[action point today reward]")
        {
            for (;;)
            {
                APSystem::_TodayRewardItem today;   // Wave5：表行=顶层类(0xe)
                int raw = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                today.m_itemIdx = raw - 1;
                if (!IsValidTodayRewardItem(*reinterpret_cast<const _SIG_LOAD_ACTION_POINT::_TodayRewardItem*>(&today)))  // 嵌套形参按值4B,首int同位(顶层0xe表行,ORIG import 五列中间态见 ScriptLeafA2 批)
                {
                    return 0xfa;                        // ORIG 错误码 0xfa
                }
                ScanInt(&ok);                           // 四个丢弃列（ORIG 存栈即弃）
                ScanInt(&ok);
                ScanInt(&ok);
                ScanInt(&ok);
                out.m_todayRewards.push_back(today);
            }
        }
    }

    // EOF：把各组元素指针登记进 +0x18 引用表（ORIG 0x08895481 尾段循环）
    // p167：ORIG 用 const_iterator（转换构造 ×2）+ operator++(int) +
    // operator->/operator[] const ⇒ 按 P96/P28 改写。
    for (std::map<unsigned short, std::vector<CActionPointEx> >::const_iterator
             it = out.m_actionPoints.begin();
         it != out.m_actionPoints.end(); it++)
    {
        const std::vector<CActionPointEx>& vec = it->second;
        for (size_t i = 0; i < vec.size(); ++i)
        {
            out.m_actionPointRefs.insert(
                std::make_pair(it->first,
                               static_cast<const CActionPointEx*>(&vec[i])));
        }
    }
    // ↑ [P330] ORIG EOF 正常出口：ebx=0
    return 0;
}

}  // namespace APSystem

// ============================================================================
// eventReward::ImportRewardScript —— ORIG T 0x080ef3a2
// 报告 eventReward/ImportRewardScript.md 全文转写。
// 令牌派发表为函数级静态 __lookup/__init_lock/__inited 三件套
// （[/reward]=0x8f [job type]=0x93 [sub type]=0xa3 [option]=0xaf
//   [equipment item]=0xb9 [stackable item]=0xd8 [/sub type]=0xfc
//   [/job type]=0x102 [/option]=0x107；未知 -1 忽略）。
// 返回：1=[/reward] 收尾或自然 EOF 且最后 ok=true；0=文件/解析失败
// （自然 EOF 返回最后一次 ScanInt 的 ok 态，ORIG ebx=local_18d 同语义）。
// ============================================================================
#include "ScriptLeafA2/ImportRewardScript.inc"
{
    // [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N168a/N18a/N82/N122）→ 试=②prett（按
    //   q3_gap2「Ndx != UND」修正判据复核）③strall 均 CLEAR；④12 处 ORIG
    //   `movzbl <__inited>; xor $0x1,%eax; test %al,%al; je` vs 我方 `movzbl; test; je`
    //   —— 4.1 物化 xor、4.4 对**静态 char** 的 `!` 恒反相 jcc（否定形 4.4 产 test;jne
    //   且 +1 条，A16 复核旧实测）⇒ 跨版本框标差不可造；余为共体块（P333 状态机 9 项
    //   insert 顺序已对齐）。⇒ 维持 N168a/N122。
    // [b28d-solo-ScriptLeafA2] 结构按 ORIG 0x080ef3a2 还原为 **P333 状态机形**：
    //   每 case 块头 `if (__inited) {业务; break;} else {insert; fallthrough}`，
    //   9 项插完后共同尾部 `if(!__inited){leave; __inited=1; goto retry;}` 回跳重查
    //   （ORIG 0x080f01d5/0x080f01e3）；static 三件套声明在循环体内 retry 标签之后
    //   （P177：每轮穿 guard，ORIG 0x080ef438 每轮执行 guard 检查）。
    //   业务块逐块按报告 ## 汇编 转写，含 3 处此前漏掉的返回值检查：
    //   ①[job type] ScanStr 失败 → return 0（0x080ef79a）；②[sub type] 首 ScanStr
    //   失败 → return 0（0x080ef8d9）；③[option] ScanStr 成功才调 getOptionType
    //   （0x080efa47 失败直跳析构）。[equipment item]/[stackable item] 行扫描失败
    //   一律回主循环（0x080efb6b/0x080efb92/0x080efe03/0x080efe2a/0x080efe51 →
    //   0x080f01d5），非 return 0；[stackable item] 的 `curItem.m_count = 1` 在
    //   循环体内每轮执行（0x080efdb3）。key 三标量为 signed char 直落（0x080ef7be）。
    //   读数（本波 fresh）：gap 30→18（ORIG 885 vs 我方 867）；p167 **122=122**
    //   （EXTRA/MISSING 仅剩 C1↔C2/D1↔D2 对称差；itemDataD1Ev ×6 已由 itemData
    //   删用户空析构消除）。残余 18 条 = 每 call 段 ≤2 条的槽/寄存器形差
    //   （N82/N18a）；判**语义完成、byte 层封顶**。
    if (!loadRDARScriptFile(dir, file))
    {
        return 0;
    }

    std::string token;                              // -0x188
    bool ok = false;                                // -0x189
    std::map<int, std::vector<itemData> > subMap;   // -0x1a4
    rewardKey key;                                  // -0x1a9
    int deadPad = 0;                                // -0x20（ORIG 死局部，P232）
    itemData curItem;                               // -0x1b8
    (void)deadPad;

    // [b28d-L4a 2026-09-11] ORIG 0x080ef430 `xor $1; test %al,%al; jne 0x080f0201
    // (+0xe5f)`：扫描失败直接跳**函数尾部**（非就地）⇒ 原码为 top-tested
    // `for (;;) { if (!ScanType(...)) break; ... }`（与 importScript/APEtc 同形）。
    for (;;)
    {
        if (!ScanType(token, true))
        {
            break;                                  // EOF → 尾段 return ok?1:0
        }
    retry:
        static std::map<const char*, int, less_TCHAR> __lookup;
        static SpinLock __init_lock;
        static char __inited = 0;
        int id;

        if (__inited)
        {
            std::map<const char*, int, less_TCHAR>::const_iterator it =
                __lookup.find(token.c_str());
            id = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                goto retry;
            }
            id = 0;                                 // 首遍登记态
        }

        switch (id)
        {
        case 0:         // 首轮哨兵（与 [/reward] 共体）
        case 0x8f:      // [/reward]
            // [A16 复核] ORIG 0x080ef6b8 段 `movzbl __inited; xor $1; test %al; je`
            // 不可复现：把本处改写为否定形 `if(!__inited){insert}else{return 1;}`
            // 实测 4.4 产 `test;jne`（**无 xor**）且 +1 条 ⇒ 回退。4.4 对静态
            // bool/char 的 `!` 恒折 `test;jne`（N122 仅覆盖具名局部），
            // 与 4.1 的 xor 物化互斥 ⇒ 12 处检查点差记 N168a/N122 封顶。
            if (__inited)
            {
                return 1;
            }
            __lookup.insert(
                std::map<const char*, int, less_TCHAR>::value_type("[/reward]", 0x8f));
            // fallthrough
        case 0x93:      // [job type]
            if (__inited)
            {
                std::string tok2;
                if (!ScanStr(&tok2))
                {
                    return 0;
                }
                key.m_job = static_cast<signed char>(getCharacterJobToEnumValue(tok2));
                key.m_a = static_cast<signed char>(ScanInt(&ok));
                if (!ok)
                {
                    return 0;
                }
                key.m_b = static_cast<signed char>(ScanInt(&ok));
                if (!ok)
                {
                    return 0;
                }
                break;
            }
            __lookup.insert(
                std::map<const char*, int, less_TCHAR>::value_type("[job type]", 0x93));
            // fallthrough
        case 0xa3:      // [sub type]
            if (__inited)
            {
                std::string tok2;
                if (!ScanStr(&tok2))
                {
                    return 0;
                }
                key.m_sub.m_type = static_cast<signed char>(getSubKeyType(tok2));
                std::string tok3;
                if (!ScanStr(&tok3))
                {
                    return 0;
                }
                key.m_sub.m_action = static_cast<signed char>(getSubKeyAction(tok3));
                break;
            }
            __lookup.insert(
                std::map<const char*, int, less_TCHAR>::value_type("[sub type]", 0xa3));
            // fallthrough
        case 0xaf:      // [option]
            if (__inited)
            {
                std::string tok2;
                if (ScanStr(&tok2))
                {
                    getOptionType(tok2, curItem);
                }
                break;
            }
            __lookup.insert(
                std::map<const char*, int, less_TCHAR>::value_type("[option]", 0xaf));
            // fallthrough
        case 0xb9:      // [equipment item]
            if (__inited)
            {
                defaultOption(key.m_sub.m_action, curItem);
                for (;;)
                {
                    int findKey = 0;
                    itemData item = curItem;        // 栈副本
                    findKey = ScanInt(&ok);         // ORIG 键为独立局部 int（不占 itemData）
                    if (!ok)
                    {
                        break;                      // 回主循环
                    }
                    item.m_itemIdx = ScanInt(&ok);
                    if (!ok)
                    {
                        break;                      // 回主循环（非 return 0）
                    }
                    std::map<int, std::vector<itemData> >::iterator fit =
                        subMap.find(findKey);
                    if (fit != subMap.end())
                    {
                        fit->second.push_back(item);
                    }
                    else
                    {
                        std::vector<itemData> vec;
                        vec.push_back(item);
                        subMap.insert(std::make_pair(findKey, vec));
                    }
                }
                break;
            }
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type(
                "[equipment item]", 0xb9));
            // fallthrough
        case 0xd8:      // [stackable item]
            if (__inited)
            {
                defaultOption(key.m_sub.m_action, curItem);
                for (;;)
                {
                    int findKey = 0;
                    curItem.m_count = 1;            // 每轮重置（ORIG 0x080efdb3）
                    itemData item = curItem;
                    findKey = ScanInt(&ok);         // ORIG 键为独立局部 int
                    if (!ok)
                    {
                        break;                      // 回主循环
                    }
                    item.m_itemIdx = ScanInt(&ok);
                    if (!ok)
                    {
                        break;                      // 回主循环（非 return 0）
                    }
                    item.m_count = ScanInt(&ok);
                    if (!ok)
                    {
                        break;                      // 回主循环（非 return 0）
                    }
                    std::map<int, std::vector<itemData> >::iterator fit =
                        subMap.find(findKey);
                    if (fit != subMap.end())
                    {
                        fit->second.push_back(item);
                    }
                    else
                    {
                        std::vector<itemData> vec;
                        vec.push_back(item);
                        subMap.insert(std::make_pair(findKey, vec));
                    }
                }
                break;
            }
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type(
                "[stackable item]", 0xd8));
            // fallthrough
        case 0xfc:      // [/sub type]
            if (__inited)
            {
                m_map0.insert(std::make_pair(key, subMap));
                key.m_sub.clear();                  // rewardSubKey::clear 仅清子键
                subMap.clear();
                break;
            }
            __lookup.insert(
                std::map<const char*, int, less_TCHAR>::value_type("[/sub type]", 0xfc));
            // fallthrough
        case 0x102:     // [/job type]
            if (__inited)
            {
                key.clear();                        // rewardKey::clear
                subMap.clear();
                break;
            }
            __lookup.insert(
                std::map<const char*, int, less_TCHAR>::value_type("[/job type]", 0x102));
            // fallthrough
        case 0x107:     // [/option]
            if (__inited)
            {
                curItem.clear();                    // itemData::clear（W 0x080f1162，call 实证）
                break;
            }
            __lookup.insert(
                std::map<const char*, int, less_TCHAR>::value_type("[/option]", 0x107));
            break;

        default:        // 未知令牌忽略
            break;
        }

        if (!__inited)
        {
            __init_lock.leave();
            __inited = 1;
            goto retry;
        }
    }
    // 自然 EOF：返回最后一次 ok 态（ORIG ebx=local_18d）
    return ok ? 1 : 0;
}

// ============================================================================
// importUpgradeTable —— ORIG T 0x08900c26
// 报告 _lt_global_gt_/importUpgradeTable.md 全文转写。
// 返回 1=成功（含 EOF）；0=失败（文件打开失败/关键扫描失败/键重复/异常捕获）。
//
// 【batch-15/16 验收标注（§6.2 口径）】orig 1119 / ours 1166（gap -132 → -47）。
//   ① 调用点差集（P167）**已归零**：direct 138 ↔ 136，missing 14 / extra 12 全部
//      是 C2/D2 ↔ C1/D1 同体构造/析构符号名对称差 + 1 个 _Unwind_Resume，
//      无任何真实调用点缺口；indirect 0/0。
//   ② 段边界切片法（difflib 在 N82 帧号重排下不可信）逐 token 段核对，修掉两处
//      真语义偏差：[cost weight by upgrade level] 的取数通道（ORIG 用
//      ScanInt(bool*) 返回值 + 循环头 ok 复位，非 ScanInt(int*) 出参形）、
//      [amplification const] 循环入口缺 `ok = true` 复位（orig local_cb）；
//      并删除匿名 namespace 的 inline helper（-O0 逐点发 call，见 N-I 延伸）。
//   ③ 余 -47 集中在三处已定位的 4.1 专属形：0x089003fa 的守卫 bool 汇聚岛
//      （实测命名 bool 反而 +12，已回退）、0x08900d5a 的双段 catch
//      （begin_catch/end_catch + 清理后 rethrow 的 EH 展开表形）、
//      0x0890124x 的成功出口 jmp/nop 中继 ⇒ N3/N82/setcc-fold/跨版本 EH
//      命中 ⇒ 判**语义完成、byte 层封顶**。
//   ④ 三处「看起来更像 ORIG 但实测更远」的改写已回退并在原地留数值记录
//      （get_upgrade_info 逐行重发 gain 471、dword 下标视图 326、
//        命名 bool 汇聚岛 1177），避免后续批次重试。
// ============================================================================
// upgrade_info_t 的表行写入使用**真实字段下标直访**（batch-15 N-I 延伸：
// 匿名 namespace 的 inline helper 在 -O0 下同样逐点发 call，不豁免）；偏移→字段
// 映射按 upgrade_info_t 已建分段具名成员（m_prob00[8]/m_prob20[3]/+0x2c..34 三项/
// m_w38[5]/m_w4c[4]/m_w5c[4]，合计恰 0x6c）。

// [P248 档①] 返回 bool（全部 return 为常量 0/1 ⇒ 机器码与 int 版逐条相同）。
#include "ScriptLeafA2/importUpgradeTable.inc"
{
    // [A16 round5 修复 + 判词] 本件含一处**真语义修**：12 处把 int 视角
    //   `m_prob00[k] / m_w38[4] / m_w4c[j] / m_w5c[j]` 改成同址 float 视角
    //   （m_f00[0,1,5,2,6,3]/m_f38[4]/m_f4c[1]/m_f5c[0]/m_f5c[1]/m_f4c[2]/
    //   m_f5c[2]/m_f4c[3]），因 ORIG 该 12 处为 `fildl … fstps`（浮点存储）：
    //   int 视角下 4.4 恒折 fnstcw/movzwl/fldcw×2/fistpl/mov 六条往返（12 处
    //   共 -95 条），且**运行期把 float 位模式当整数截断**（下游 CItemAmplifier.cpp
    //   0x0854b282 段以 m_f00/m_f4c 视图读取）⇒ 原为真 bug。
    //   fresh 读数：改前 fistpl 12/fnstcw 12/fldcw 24/fstps 8，改后 fildl 2/fstps 20
    //   = ORIG 完全一致；gap 1167→1072（ORIG 1119：+48 → -47）。
    //   残余 47 条 = 帧/槽分区：ORIG `sub $0x190` vs 我方 `$0x160`（试补
    //   char[0x30] 死局部可让帧回 0x190，但槽号不动、gap 与四数零变化 ⇒ 已回退）
    //   + 槽号体系差（ORIG ok@-0xc6 vs 我方 -0x95）⇒ N82/N126 版本常数封顶；
    //   p167 仅别名伪差（C1/C2·D1/D2 同址，P93）+ _Unwind_Resume ×1（我方多一处
    //   EH landing）。语义等价（token 比较方向/表插入/错误路径均与 ORIG 对齐）。
    // [轴 B 2026-09-10 · a_S->U 清单件 = **N（表内配对假阳性）**] 清单行
    //   `orig_addr=0x08901016 setle` 与我方 `setbe` 系按出现序号交叉配对。按地址自校：
    //   ORIG 0x08901016 = `addl $0x1,-0x74(%ebp); cmpl $0x4,-0x74(%ebp); setle`
    //   （有符号；字段循环尾测）↔ 我方 0x390b 同构 setle（已一致）；
    //   我方唯一 setbe（0x3895 `call vector<upgrade_info_t>::size(); cmp -0x58(%ebp),%eax;
    //   setbe`）对应 ORIG 0x08901039 `call size(); cmp -0x7c(%ebp),%eax; jbe`
    //   —— 两侧同为**无符号序**且上下文同构 ⇒ 本就一致，**禁改**（P196）。
    //   实测该件 1119/1074（gap 45）不变，未动一行源码。
    if (!loadRDARScriptFile("", path))
    {
        return 0;
    }

    try
    {
        std::string token;
        std::string rarityName;
        bool ok = false;

        for (;;)
        {
            if (!ScanType(token, true))
            {
                return 1;                               // EOF → 成功
            }

            if (token == "[table]")
            {
                table->m_infos.clear();
                for (;;)
                {
                    upgrade_info_t info;
                    info.m_f00[0] =
                        static_cast<float>(ScanInt(&ok));
                    if (!ok)
                    {
                        break;
                    }
                    // [A16 round5 真语义修] 报告 0x08900d13 实证：本行在首字段
                    // 检查之后即清 +0x10 = `mov $0x0,%eax; mov %eax,-0x154`（float
                    // 视角两段形 ⇒ 0.0f），不是 [table] 段末尾的 int 单条 movl。
                    info.m_f00[4] = 0.0f;
                    info.m_f00[1] = ScanFloat(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    info.m_f00[5] = ScanFloat(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    info.m_f00[2] = ScanFloat(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    info.m_f00[6] = ScanFloat(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    info.m_f00[3] = ScanFloat(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    // [A16 round5 真语义修] 报告 0x08900de1 `mov $0x0,%eax; mov
                    // %eax,-0x148`（+0x1c = 0.0f 两段形）⇒ m_f00[7] 是常量 0，
                    // **不消费** ScanInt。原写 `m_prob00[7] = ScanInt(&ok)` 使其后
                    // 5 个整数整体错位一槽（m_prob20[2]/m_recipeItem 取值错、
                    // m_meterialCount 恒 0）—— 真 bug。
                    info.m_f00[7] = 0.0f;
                    info.m_prob20[0] = ScanInt(&ok);
                    info.m_prob20[1] = ScanInt(&ok);
                    info.m_prob20[2] = ScanInt(&ok);
                    info.m_recipeItem = ScanInt(&ok);
                    // 报告 0x08900e45 (→+0x30) 为第 5 个连续 ScanInt，0x08900e50
                    // (→+0x34) 才是 `movl $0x0`（m_discountBase = 0，int 单条）。
                    info.m_meterialCount = ScanInt(&ok);
                    info.m_discountBase = 0;
                    info.m_f38[4] =
                        static_cast<float>(ScanInt(&ok));
                    if (!ok)
                    {
                        break;
                    }
                    info.m_f5c[0] = 0.0f;
                    info.m_f4c[1] = ScanFloat(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    info.m_f5c[1] = ScanFloat(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    info.m_f4c[2] = ScanFloat(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    info.m_f5c[2] = ScanFloat(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    info.m_f4c[3] = ScanFloat(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    table->m_infos.push_back(info);
                }
            }
            else if (token == "[amplification const]")
            {
                size_t index = 0;
                ok = true;                    // ORIG local_cb = true（入循环前置真）
                for (;;)
                {
                    // ORIG 条件本体是 `ok == false || size() <= index` 的合并短路
                    // （0x08901020 起 + bool 汇聚岛）。ok 复位为 true 的语义已补（下方）；
                    // 汇聚岛在 4.4 下改命名 bool 反而多 12 insn（实测 1165→1177），
                    // 故保留直判形，岛形差记 N3/setcc-fold。
                    if (index >= table->m_infos.size())
                    {
                        break;
                    }
                    upgrade_info_t& info = table->m_infos[index];
                    ++index;
                    for (int i = 0; i <= 4; ++i)
                    {
                        if (i == 0)
                        {
                            info.m_w38[i] = 0;
                        }
                        else
                        {
                            info.m_w38[i] = ScanInt(&ok);
                        }
                        if (!ok)
                        {
                            break;
                        }
                    }
                }
                if (!ScanType(token, true))
                {
                    LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                                          __FUNCTION__, 0xb3a,
                                          "ERROR [amplification const] count %d",
                                          static_cast<int>(index));
                }
            }
            else if (token == "[cost weights by rarity]")
            {
                for (int i = 0; i <= 5; ++i)
                {
                    UPG_TBL_F32(table, i + 0x1b) = ScanFloat(&ok);
                    if (!ok)
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptEtcParameter.cpp", __FUNCTION__,
                            0xb48, "importUpgradeTable - invalid cost rarity table");
                    }
                }
            }
            else if (token == "[stat weights by rarity]")
            {
                for (int i = 0; i <= 5; ++i)
                {
                    UPG_TBL_F32(table, i + 0x20) = ScanFloat(&ok);
                    if (!ok)
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptEtcParameter.cpp", __FUNCTION__,
                            0xb5a, "importUpgradeTable - invalid stat rarity table");
                    }
                }
            }
            else if (token == "[type]")
            {
                for (int i = 10; i <= 0x15; ++i)
                {
                    UPG_TBL_F32(table, i + 0x4) = ScanFloat(&ok);
                    if (!ok)
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptEtcParameter.cpp", __FUNCTION__,
                            0xb6c, "importUpgradeTable - invalid type table");
                    }
                }
            }
            else if (token == "[level]")
            {
                for (int i = 0; i <= 3; ++i)
                {
                    UPG_TBL_F32(table, i) = ScanFloat(&ok);
                    if (!ok)
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptEtcParameter.cpp", __FUNCTION__,
                            0xb7e, "importUpgradeTable - invalid level table");
                    }
                }
            }
            else if (token == "[cost]")
            {
                for (int i = 0; i <= 0x63; ++i)
                {
                    UPG_TBL_I32(table, i + 0x26) = ScanInt(&ok);
                    if (!ok)
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptEtcParameter.cpp", __FUNCTION__,
                            0xb90, "importUpgradeTable - invalid gold table");
                    }
                }
            }
            else if (token == "[notice]")
            {
                UPG_TBL_I32(table, 0x8a) = ScanInt(&ok);
                if (!ok)
                {
                    LogManager::logFormat(
                        0, "../RDARScriptEtcParameter.cpp", __FUNCTION__,
                        0xb9e, "importUpgradeTable - invalid notice tag");
                }
            }
            else if (token == "[repair cost rate by upgrade level]")
            {
                for (int i = 0; i <= 0x63; ++i)
                {
                    UPG_TBL_F32(table, i + 0x9b) = ScanFloat(&ok);
                    if (!ok)
                    {
                        if (i != 0x63)
                        {
                            LogManager::logFormat(
                                0, "../RDARScriptEtcParameter.cpp",
                                __FUNCTION__, 0xbac,
                                "ERROR : [repair cost rate by upgrade level] size %d / %d",
                                i, 100);
                        }
                        break;
                    }
                }
            }
            else if (token == "[destroy level by rarity]")
            {
                while (ScanStr(&rarityName))
                {
                    int level = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0;
                    }
                    int rarity = getItemRarity(rarityName.c_str());
                    if (rarity == 6)
                    {
                        return 0;
                    }
                    UPG_TBL_I32(table, rarity + 0xfe) = level;
                }
            }
            else if (token == "[max upgrade level by rarity]")
            {
                while (ScanStr(&rarityName))
                {
                    int level = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0;
                    }
                    int rarity = getItemRarity(rarityName.c_str());
                    if (rarity == 6)
                    {
                        return 0;
                    }
                    UPG_TBL_I32(table, rarity + 0x104) = level;
                }
            }
            else if (token == "[disjoint]")
            {
                for (;;)
                {
                    if (!ScanType(token, true))
                    {
                        throw static_cast<unsigned int>(0xbdc);
                    }
                    if (token == "[/disjoint]")
                    {
                        break;
                    }
                    if (token == "[disjoint bonus item]")
                    {
                        UPG_TBL_I32(table, 0x8b) = ScanInt(&ok);
                        if (!ok)
                        {
                            break;
                        }
                    }
                    else if (token == "[equip level const]")
                    {
                        UPG_TBL_I32(table, 0x8c) = ScanInt(&ok);
                        if (!ok)
                        {
                            throw static_cast<unsigned int>(0xbea);
                        }
                    }
                    else if (token == "[upgrade failed bonus weight by rarity]")
                    {
                        while (ScanStr(&rarityName))
                        {
                            float weight = ScanFloat(&ok);
                            if (!ok)
                            {
                                throw static_cast<unsigned int>(0xbf4);
                            }
                            int rarity = getItemRarity(rarityName.c_str());
                            if (rarity == 6)
                            {
                                throw static_cast<unsigned int>(0xbf7);
                            }
                            UPG_TBL_F32(table, rarity + 0x8d) = weight;
                        }
                    }
                    else if (token == "[upgrade const for bonus item count]")
                    {
                        UPG_TBL_I32(table, 0x93) = ScanInt(&ok);
                        if (!ok)
                        {
                            throw static_cast<unsigned int>(0xbff);
                        }
                    }
                    else if (token == "[correction grade by rarity]")
                    {
                        while (ScanStr(&rarityName))
                        {
                            int grade = ScanInt(&ok);
                            if (!ok)
                            {
                                throw static_cast<unsigned int>(0xc09);
                            }
                            int rarity = getItemRarity(rarityName.c_str());
                            if (rarity == 6)
                            {
                                throw static_cast<unsigned int>(0xc0c);
                            }
                            UPG_TBL_I32(table, rarity + 0x94) = grade;
                        }
                    }
                }
            }
            else if (token == "[destruction result]")
            {
                for (int t = 10; t <= 0x15; ++t)
                {
                    UPG_TBL_F32(table, t + 0x10b) = ScanFloat(&ok);
                    if (!ok)
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptEtcParameter.cpp", __FUNCTION__,
                            0xc26, "importUpgradeTable - invalid type table");
                    }
                }
                int destroyIdx = ScanInt(&ok);
                if (!ok)
                {
                    return 1;                           // ORIG jmp EOF 成功出口
                }
                table->m_destruction.m_field58 = destroyIdx;
                for (;;)
                {
                    STResultItemCnt cnt;
                    cnt.m_a = static_cast<unsigned short>(ScanInt(&ok));
                    if (!ok)
                    {
                        break;
                    }
                    cnt.m_b = static_cast<unsigned short>(ScanInt(&ok));
                    if (!ok)
                    {
                        break;
                    }
                    cnt.m_c = static_cast<unsigned short>(ScanInt(&ok));
                    if (!ok)
                    {
                        break;
                    }
                    table->m_destruction.m_vec.push_back(cnt);
                }
            }
            else if (token == "[cost weight by upgrade level]")
            {
                for (;;)
                {
                    // ORIG 0x08901e81 起：每轮循环头先 `local_d9 = false` 复位 ok，
                    // 数值走 **ScanInt(bool*)/ScanFloat(bool*) 的返回值通道**
                    // （Ghidra `local_e0 = (float)ScanInt(&local_d9)`、
                    // `lVar8 = (longdouble)ScanFloat(&local_d9)`），ok 仅作
                    // 「!= true → 退出/返回 0」判定。此前写成 ScanInt(int*) 的
                    // 出参通道（P170：mangled 参数类型须 nm 实查），使本函数
                    // 引用了 ORIG 未调用的 _Z7ScanIntPi。
                    ok = false;
                    int level = ScanInt(&ok);
                    if (ok != true)
                    {
                        break;
                    }
                    float weight = ScanFloat(&ok);
                    if (ok != true)
                    {
                        return 0;
                    }
                    std::pair<std::map<int, float>::iterator, bool> ins =
                        table->m_floatMap.insert(std::make_pair(level, weight));
                    if (!ins.second)
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptEtcParameter.cpp", __FUNCTION__,
                            0xc54,
                            "[cost weight by upgrade level] key is duplicated]");
                        return 0;
                    }
                }
            }
            else if (token == "[upgrade machine frame base time]")
            {
                if (!ScanInt(&table->m_zero4b4[0]))
                {
                    return 0;
                }
                if (!ScanInt(&table->m_zero4b4[1]))
                {
                    return 0;
                }
                if (!ScanInt(&table->m_zero4b4[2]))
                {
                    return 0;
                }
                if (!ScanInt(&table->m_zero4b4[3]))
                {
                    return 0;
                }
            }
            else if (token == "[upgrade machine frame rate]")
            {
                for (;;)
                {
                    int key;
                    if (!ScanInt(&key))
                    {
                        break;
                    }
                    upgrade_table_t::MachineFrameInfo info;
                    if (!ScanFloat(&info.m_f00))
                    {
                        return 0;
                    }
                    if (!ScanFloat(&info.m_f04))
                    {
                        return 0;
                    }
                    if (!ScanFloat(&info.m_f08))
                    {
                        return 0;
                    }
                    if (!ScanFloat(&info.m_f0c))
                    {
                        return 0;
                    }
                    if (!ScanInt(&info.m_i10))
                    {
                        return 0;
                    }
                    // ORIG 0x08901e35：直接就地构造 value_type
                    // （_ZNSt4pairIKiN15upgrade_table_t16MachineFrameInfoEEC1IRiRS2_EEOT_OT0_
                    //  = pair<const int,MachineFrameInfo>::pair<int&,MachineFrameInfo&>）
                    // 后 _Hashtable::insert(value_type const&)，无 make_pair 转换临时。
                    // （上面 m_floatMap 那处 ORIG 确为 make_pair+转换，保持不动。）
                    table->m_machineFrames.insert(
                        std::unordered_map<int, upgrade_table_t::MachineFrameInfo>::value_type(
                            key, info));
                }
            }
        }
    }
    catch (unsigned int errLine)
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              __FUNCTION__, 0xc14, "Error: (%s):%d",
                              "importUpgradeTable", static_cast<int>(errLine));
        return 0;
    }
}

// ============================================================================
// stLevelUpRewardItemScript::clear —— ORIG W 0x083711d6
// 【报告缺失】无专属函数报告；已先读调用者报告
// _lt_global_gt_/ImportLevelUpRewardItemScript.md 后按 nm 地址 objdump 转写：
//   83711dc mov 0x8(%ebp),%eax ; push eax
//   83711e2 call std::map<LevelUpRewardItemTableKey,LevelUpRewardItemData>::clear
// 即 m_map0.clear()。
// ============================================================================
#include "ScriptLeafA2/clear_1.inc"
{
    m_map0.clear();
}

// ============================================================================
// seriaRoomDecoAniEventScript::clear —— ORIG W 0x083732a4
// 【报告缺失】无专属函数报告；已先读同类报告
// _lt_global_gt_/importSeriaRoomAnimationDecoEvent.md 与 ORIG ctor 0x083731ca
// （objdump）确认五成员布局后转写：
//   vector@+0x00 clear / map@+0x0c clear / map@+0x24 clear /
//   map@+0x40 clear / string@+0x3c clear。
// ============================================================================
#include "ScriptLeafA2/clear_2.inc"
{
    m_rewardList.clear();
    m_itemMap.clear();
    m_aniList.clear();
    m_finishAniMap.clear();
    m_finishSound.clear();
}

// ============================================================================
// CItem::GetMedalPrice —— ORIG W 0x0822c862
// 【报告缺失】无专属函数报告；已先读调用者报告
// DisPatcher_RentAvatar/dispatch_sig.md 与 DisPatcher_BuyItem/get_data.md
// （与 GetCashPrice/get_price 并列的价格访问器）后按 nm 地址 objdump 转写：
//   mov 0x8(%ebp),%eax ; mov 0x14(%eax),%eax ; ret —— 返回 this+0x14 int。
// ============================================================================
#include "ScriptLeafA2/GetMedalPrice.inc"
{
    return m_field14;   // +0x14
}
