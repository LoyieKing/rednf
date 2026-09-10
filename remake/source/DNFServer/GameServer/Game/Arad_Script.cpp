// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 Arad_Script/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include "Arad_Script.h"

// ============================================================================
// [b28d-64 判词汇总 · 泳道64] 本 TU 4 件（ORIG 地址见各报告 /tmp/b28d_lane_64_result.tsv）：
//   AradJumping_Script::importScript 0x089... 92/102 N封顶：P167 CLEAR 39/39；
//     残余 = cMyTrace 字符串重定位 + nop 岛落点 + %esi/%ebx 实参保活序（N80/N7）。
//   Arad_Script::Load(PKc)           418/421 N封顶：P167 CLEAR 39/39；槽序/寄存器
//     （本 TU 路由 -O2 + gnu++0x，ORIG 为 -O0）。
//   Arad_Script::Load(PKc, S1)       397/400 N封顶：P167 CLEAR 31/31；同上。
//   ARAD::SCRIPT::importAradEventScript 340/360 N封顶：P167 EXTRA
//     SEventPeriodData/SRewardPeriodData::~D1 ×2（4.4 对 vector 元素/临时多发析构 + EH）；
//     MISSING 空 ⇒ 语义完成、N91 封顶。
// ============================================================================

#include <cstdio>
#include <cstdlib>
#include <cstring>

// [moved] include 区统一收敛到文件头（中部 include 在 lsd44 链下会污染后续标准头）

#include "Arad_Script/Arad_Script.inc"
}

#include "Arad_Script/Arad_Script_1.inc"
}

#include "Arad_Script/GetRowNum.inc"
}

#include "Arad_Script/dtor_Arad_Script.inc"
}

// [b28d-solo-Arad_Script] Create verdict=AE（fresh .o 66/66 ndiff=0）：三处形全等——
// ①memset size 实参两段 `mov 0x10(%eax),%eax; mov %eax,%edx` 由 `int bufBytes_`
// 的 int→size_t 转换复现（探针 fb/fk）；②store 段 `shl;mov` 由 RHS `* 0x80u`
// 与 int 字段异型复现（探针 gb）；③new[]/map clear 段原形。连带 ExpandCol/ExpandRow
// 同时 AE（RHS 异型 `unsigned<<7` / 同型 `*128` 两向开关，探针 ga/gd）。
#include "Arad_Script/Create.inc"
}

#include "Arad_Script/ExpandCol.inc"
}

#include "Arad_Script/ExpandRow.inc"
}

#include "Arad_Script/make_file_path.inc"
}

#include "Arad_Script/GetIdx4Col.inc"
}

#include "Arad_Script/GetIdx4Row.inc"
}

#include "Arad_Script/GetInt.inc"
}

#include "Arad_Script/GetInt_1.inc"
}

#include "Arad_Script/GetInt_2.inc"
}

#include "Arad_Script/GetInt_3.inc"
}

#include "Arad_Script/GetFloat.inc"
}

#include "Arad_Script/GetFloat_1.inc"
}

#include "Arad_Script/GetFloat_2.inc"
}

#include "Arad_Script/GetFloat_3.inc"
}

#include "Arad_Script/GetDouble.inc"
}

#include "Arad_Script/GetDouble_1.inc"
}

#include "Arad_Script/GetDouble_2.inc"
}

#include "Arad_Script/GetDouble_3.inc"
}

#include "Arad_Script/GetString.inc"
}

#include "Arad_Script/GetString_1.inc"
}

#include "Arad_Script/GetString_2.inc"
}

#include "Arad_Script/GetString_3.inc"
}

#include "Arad_Script/SetInt.inc"
}

#include "Arad_Script/SetInt_1.inc"
}

#include "Arad_Script/SetInt_2.inc"
}

#include "Arad_Script/SetInt_3.inc"
}

#include "Arad_Script/SetString.inc"
}

#include "Arad_Script/SetString_1.inc"
}

#include "Arad_Script/SetString_2.inc"
}

#include "Arad_Script/SetString_3.inc"
}

// ---------------------------------------------------------------------------
// Arad_Script::Load(dir, file) —— ORIG _ZN11Arad_Script4LoadEPKcS1_ @ 0x0817b950
// 报告 docs/class_func_reports/Arad_Script/Load.md（汇编 + Ghidra C 全文转写）。
// 语义：内存流定位 "[%s]"(file) 节标记 → 列名行解析（'/' 分段写入 colIndex_，
// 末尾统一 ++usedCols_ 并按需 ExpandCol）→ 数据块行解析（首 token 写 rowIndex_，
// 其余 strcpy 进 cells_，行满 usedCols_ 后跳行尾，按需 ExpandRow）。
// EOF(-1) 在节/列行段直接失败；数据段空 token 视作 EOF。失败路径不 fclose
// （本重载无文件句柄）。
// 【残差豁免·跨版本 N 类】ORIG 397 / ours 400（gap 24→3）。语义与调用序逐段对齐；
// 剩余差异均为 gcc4.1↔4.4 结构性产物：①do-while/while 回边 `je exit;jmp top` vs
// `jmp top;nop;jne top`（负例表"空 else nop;jmp 岛 vs jump-threading"，ORIG
// 0x0817bc7d/0x0817ba70）；②key2 写回 `mov %eax,%edx` 缓存 ref（4.1 regmove 方向，
// N18a，ORIG 0x0817bbf4）；③cells_ 寻址 `lea (%ecx,%eax,1)` vs `add mem,%eax`
// （N4 折叠，ORIG 0x0817bdbc）；④string/allocator 临时栈槽偏移 -0x54/-0x4d… vs
// -0x44/-0x3d…（NEAR 级槽序）；⑤EH landing-pad 块序（"EH 双份 dtor epilogue vs
// merge"）。源码层无合法写法翻转以上各向。
// ---------------------------------------------------------------------------
// [b28d-solo-Arad_Script] Load(PKc,PKc) verdict=N 封顶（fresh .o 387/387 gap=0,
// p167 CLEAR 31/31，ndiff 196 中 139 条为块内临时对象槽号整体深 0x10：4.1 把循环内
// allocator/string 对放在 -0x4d/-0x54 贴近 colBuf，4.4 放在 -0x3d/-0x44 贴近 ebp
// —— 4.1 声明序 vs 4.4 首次使用序的槽方向差，N82/N93 族（P212 全头部声明法不适用
// 于依赖运行期实参构造的块内对象）；其余为 4.1 空块 nop;jmp 岛/块序（N104/N112/N121）
// 与 `mov %eax,%ecx; lea` vs `add`（N18a/P30）。语义完成、byte 层 N 封顶。
#include "Arad_Script/Load.inc"
}

// [b28d-solo-Arad_Script] Load(PKc) verdict=N 封顶（fresh .o 408/408 gap=0,
// p167 CLEAR 39/39）。残余同双参版：块内 allocator/string 临时槽整体差 0x10
// （N82/N93 槽方向）、4.1 空块 nop;jmp 岛与块序（N104/N112/N121）、
// `mov %eax,%ecx; lea (%ecx,%eax,1)` vs `add`（N18a/P30；rowIdx 为 int 且需与 -1
// 比较，无法用 P211 同型化翻 add-vs-lea）。语义完成、byte 层 N 封顶。
#include "Arad_Script/Load_1.inc"
}

// ============================================================================
// W6_ARAD.ScriptPods —— ORID df_game_r 的 ARAD::SCRIPT 域残余真实现清零批。
//
// 覆盖 ORIG 符号（nm -S 实证，全部弱/T）：
//   _ZN4ARAD6SCRIPT16SEventPeriodDataC1Eijj    W 0x088b4368（【objdump】三字段直存）
//   _ZN4ARAD6SCRIPT16SEventPeriodDataD1Ev      W 0x088b558a（5B 空体【objdump】）
//   _ZN4ARAD6SCRIPT16SEventPeriodDataeqERKS1_  W 0x088b4388（仅比 +0x00【objdump】）
//   _ZN4ARAD6SCRIPT17SRewardPeriodDataD1Ev     W 0x088b5764（5B 空体【objdump】）
//   _ZN4ARAD6SCRIPT17SRewardPeriodDataeqERKS1_ W 0x088b43da（仅比 +0x00【objdump】）
//   _ZN4ARAD6SCRIPT22_AradJumpingRewardItemD1Ev W 0x088b597e（5B 空体【objdump】）
//   _ZN4ARAD6SCRIPT21AradJumpingScriptDataC1Ev W 0x0837170a / D1Ev W 0x0837171e
//   _ZN4ARAD6SCRIPT18AradJumping_ScriptC1Ev    W 0x08371732 / D1Ev W 0x08371746
//   _ZN4ARAD6SCRIPT18AradJumping_Script17getRDARScriptDataEv W 0x088b43ee
//   _ZN4ARAD6SCRIPT18AradJumping_Script12importScriptEPcRS1_ T 0x088b4128
//   _ZN4ARAD6SCRIPT8readTimeEPs                T 0x088b3970（报告 ARAD__SCRIPT/readTime.md）
//   _ZN4ARAD6SCRIPT10uint32TimeEPs             T 0x088b3b4b（报告 ARAD__SCRIPT/uint32Time.md）
//   _ZN4ARAD6SCRIPT21importAradEventScriptE... T 0x088b3bd8（报告 importAradEventScript.md）
// getCurRewardItem(T 0x088b424a) 已由 AradPeriodB.cpp 批次提供，此处不重复定义。
//
// Ctor/Dtor 族说明：ORIG AradJumpingScriptData C1 仅调用 vector<_AradJumpingRewardItem>
// 默认构造；AradJumping_Script C1/D1 直接转发基类 —— 空体定义 + vector 成员即忠实对应。
// ============================================================================
#include <algorithm>
#include "CDataManager.h"               // ARAD::SCRIPT::AradJumping*（m_aradJumping 权威段）
#include <ctime>

#include "DNFLexWrapper.h"               // loadRDARScriptFile / ScanType / ScanInt(bool*)
#include "LogManager.h"                  // LogManager::logFormat(int, file, func, line, fmt, ...)
#include "Arad_EventPeriodDataManager.h" // SEventPeriodData / SRewardPeriodData / readTime 声明

namespace ARAD {
namespace SCRIPT {

namespace {
char const* const kRDAREtcParamFile = "../localjapan/Arad_RDARScriptEtcParameter.cpp";
}

// ---- SEventPeriodData -------------------------------------------------------
// 【objdump】C1：m_eventCode/m_beginTime/m_endTime 三字段直存（无其它副作用）。
#include "Arad_Script/SEventPeriodData.inc"
}

bool SEventPeriodData::operator==(SEventPeriodData const& other)
{
    return m_eventCode == other.m_eventCode;   // 【objdump】仅比 +0x00
}

// ---- readTime ---------------------------------------------------------------
// 报告 ARAD__SCRIPT/readTime.md 全文转写：连读 4 个 short；任一 ScanInt(bool*)
// 失败抛出该字段名常量，catch 后 logFormat(line=0x27) 并返回 false。
#include "Arad_Script/readTime.inc"
}

bool SRewardPeriodData::operator==(SRewardPeriodData const& other)
{
    return m_eventCode == other.m_eventCode;   // 【objdump】仅比 +0x00
}

// ---- uint32Time -------------------------------------------------------------
// 报告 ARAD__SCRIPT/uint32Time.md 全文转写：
//   time(NULL) → localtime → 改 y(-1900)/m(-1)/d/h（min/sec=0）→ mktime，
//   返回值沿用 mktime 返回（ORIG eax 直通）。年/月加载后 cwtl/movswl 符号扩展。
unsigned int uint32Time(short* t)
{
    time_t now = ::time(0);
    tm* lt = ::localtime(&now);
    lt->tm_year = (int)*t + -1900;       // lea -0x76c(%eax)
    lt->tm_mon  = (int)t[1] + -1;        // lea -0x1(%eax)
    lt->tm_mday = (int)t[2];             // movswl
    lt->tm_hour = (int)t[3];             // movswl
    lt->tm_min  = 0;
    lt->tm_sec  = 0;
    return (unsigned int)::mktime(lt);
}

}  // namespace SCRIPT

// ============================================================================
// AradJumping 族生命周期（见 CDataManager.h 中声明段注释的 ORIG 地址清单）。
// ============================================================================
namespace SCRIPT {

// [b28d-solo] _AradJumpingRewardItem 无用户 dtor（P317/P12）：ORIG D1/D2 W 0x088b597e
// 由 new_allocator<T>::destroy/construct 使用点发射为 W；显式空体定义会使类型
// non-trivial 并让 importScript 多发 D1 call + EH landing（ORIG 无）。

#include "Arad_Script/AradJumpingScriptData.inc"
}

#include "Arad_Script/dtor_AradJumpingScriptData.inc"
}

#include "Arad_Script/AradJumping_Script.inc"
}

#include "Arad_Script/dtor_AradJumping_Script.inc"
}

#include "Arad_Script/getRDARScriptData.inc"
}

// [b28d-solo-Arad_Script] importScript verdict=N 封顶（fresh .o 90/90 gap=0,
// p167 CLEAR 12/12, ndiff 4）。修复：①删 `_AradJumpingRewardItem` 用户空 dtor
// （P317：ORIG 局部 item 无 D1 call，D1/D2 仍由 allocator::destroy 使用点发射 W）；
// ②外层 while 改 goto 顶测消 loop rotation（P283 反向，ndiff 39→21）；③内层
// `while(a&&b&&c)` 物化岛改 for+`if(!ok)break;`（ndiff 21→4）。残余 4 条 =
// ORIG `mov items,%eax; lea &item,%edx`（this 先求值保活）vs 4.4 实参先装载
// （N7/P75；`(*items).`/语句表达式/static_cast receiver 三形实测均 4 条不变）。
#include "Arad_Script/importScript.inc"
}
// [b28d-solo-Arad_Script] importAradEventScript verdict=N 封顶（fresh .o 336/336
// gap=0，p167 CLEAR 57/57）。修复：①删 SEventPeriodData/SRewardPeriodData 用户空
// dtor（P317，消 4 处 EXTRA D1 call，identical +4/missing -4）；②外层 while 改
// goto 顶测消 loop rotation（P283 反向，gap -22→0）。残余（归一化槽号后 35 条）=
// 槽号差 8（N82/N93）、SRewardPeriodData 5 实参装载序/中转寄存器差（N7/P75）、
// 尾部 ebx 返回值物化位置（N104）。语义完成、byte 层 N 封顶。
#include "Arad_Script/importAradEventScript.inc"
}

}  // namespace SCRIPT
}  // namespace ARAD
