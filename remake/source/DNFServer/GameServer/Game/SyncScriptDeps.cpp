// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 SyncScriptDeps/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r Game/ —— sync_script.cpp 缺失依赖符号最小定义（2026-08-21）。
// sync_script.cpp 的多数外部依赖（ScanType/ScanInt/ScanStr/std::string*/loadRDARScriptFile/
// GetEscapeString/codePage）已由 DNFLexWrapperHelpers.cpp/DNFFunctionLib.cpp 等已入
// CMake TU 提供；本 TU 仅补齐 ORIG 中仍缺的少量符号（签名匹配 ORIG，符号 T）：
//   toMbcs(const char*)      ORIG 0x08adee98（恒等返回 src）
//   getCharacterJob(const char*)  ORIG 0x088be57e（作业名表查找）
//   APSystem::CSyncScript::InsertDescTable/TruncateDescTable
//   advancealtar::SyncScript::insertItemDescTable/truncateItemDescTable

#include <string.h>

#include <string>
#include <cstdio>
#include <cstring>
#include <map>
#include <stdlib.h>  // wcstombs（12 个宽字符转换重载的 TLS 环形缓冲）

#include "advancealtar.h"   // AdvanceAltarData/_CharacAdvanceAltarDbData（CUser.h 依赖）
#include "STExpReward.h"    // STExpReward 叶子头（CUser.h 按值成员依赖；先于
                            // CDataManager.h 的 CItemGeneratorMgr→CUser.h 重入环）

#include "CDataManager.h"
#include "DBMgr.h"
#include "DNFFunctionLib.h"  // CodePage::script2Database/script/database
#include "GameTypes.h"       // cMyTrace
#include "SyncScriptDeps.h"

// ==================== 全局工具 ====================
// ==================== 全局工具 ====================

// ORIG 0x08adee98 toMbcs(const char*)：仅返回 src（无需转换时恒等）。
const char* toMbcs(const char* src)
{
    return src;
}

// ORIG 0x08adedd3 toMbcs(string const&)：c_str()。
const char* toMbcs(const std::string& sSrc)
{
    return sSrc.c_str();
}

// ORIG 0x08adeab0 toTString(const std::string&)：恒等返回内部缓冲的 const char*（TCHAR*）。
const char* toTString(const std::string& src)
{
    return src.c_str();
}

// ORIG 0x08adeb75 toTString(char const*)：恒等返回 src。
const char* toTString(const char* pszSrc)
{
    return pszSrc;
}

// ==================== 宽字符转换重载（TLS 环形缓冲） ====================

// ==================== 宽字符转换重载（TLS 环形缓冲） ====================
// ORIG 汇编（0x08adeea0/0x08adeac3 等）以 %gs 访问 10×0x1000 TLS 环形缓冲 +
// wcstombs + 尾 0 + (idx+1)%10 递增；与 Library/Include/Core/UnicodeConvert.cpp
// 的语义一致。UnicodeConvert.cpp 仅编入 auction/point 服务，game CMake 未收录，
// 故在本属主 TU 用同名 static __thread 缓冲迁移（TU 局部，无跨 TU ODR 冲突；
// getMbcsBuffer 等其余 Unicode 辅助符号仍归 Unicode TU，二者缓冲不共享，
// 若未来将 UnicodeConvert.cpp 纳入 game 需合并此处缓冲）。
// [b28d-lane8] TLS 布局对齐（P190 类比；无访问点、无外部符号，仅占 .tbss）：
// ORIG 中 toMbcs/toTString 属含 gUnicodeBuffer 的 TU，其 LE 偏移
//   gMbcsBuffer=-0x3c028（0x08adeea0 段 `lea -0x3c028(%eax)`）、
//   gTCharBuffer=-0xa020（0x08adeb7d 段）。
// 本 TU 组须复现同组内相对布局：lead 4B 使 gMbcsBuffer 8 对齐，
// gUnicodeBuffer[10][0x1000]+index 补足 0x28004 间距，tail 0x18B 补齐尾距。
// [b28d-Main·检查点13] lead 4→8：与 tail 0x14 组合 ⇒ 本组变量偏移 +4、tls_size +8
// （TLS 段按 8 对齐，tail 单独 +4 实测 tpoff 移 −8）⇒ tpoff = offset − tls_size 净 −4，
// 对齐 ORIG（gMbcsBuffer −0x32028 / gUnicodeBuffer −0xa024 / gTCharBuffer −0x20）。
static __thread char gTlsPadLead[8] __attribute__((unused));
static __thread char gMbcsBuffer[10][0x1000];
static __thread int gMbcsBufferIndex;
static __thread wchar_t gUnicodeBuffer[10][0x1000] __attribute__((unused));
static __thread int gUnicodeBufferIndex __attribute__((unused));
static __thread char gTCharBuffer[10][0x1000];
static __thread int gTCharBufferIndex;
// [b28d-Main·检查点13] tail 0x10→0x14：tpoff = offset_block − tls_size，本组变量
// 偏移不变而 tls_size +4 ⇒ 本组全部 tpoff −4。实测本组三变量曾整体比 ORIG 高 4
// （gMbcsBuffer −0x32024 vs ORIG −0x32028；gUnicodeBuffer −0xa020 vs −0xa024；
// gTCharBuffer −0x1c vs −0x20），组内间距与 ORIG 全等 ⇒ 仅基址差 4。
static __thread char gTlsPadTail[0x14] __attribute__((unused));

// [b28d-Main R3 迁移·检查点13] getMbcsBuffer/getUnicodeBuffer/getTCharBuffer + 3 size
// 自 StringConvert.cpp 迁入（ORIG 0x08adea35/0x08adea54/0x08adea73 与本 TLS 组同 TU）。
char* getMbcsBuffer()          // ORIG T 0x08adea35
{
    return gMbcsBuffer[gMbcsBufferIndex];
}
wchar_t* getUnicodeBuffer()    // ORIG T 0x08adea54
{
    return gUnicodeBuffer[gUnicodeBufferIndex];
}
char* getTCharBuffer()         // ORIG T 0x08adea73
{
    return gTCharBuffer[gTCharBufferIndex];
}
#include "SyncScriptDeps/getMbcsBufferSize.inc"
}
#include "SyncScriptDeps/getUnicodeBufferSize.inc"
}
#include "SyncScriptDeps/getTCharBufferSize.inc"
}

// [b28d-Main R3 迁移·检查点13] 4 个 TLS 版 toUnicode 重载自 StringConvert.cpp 迁入。
const wchar_t* toUnicode(const char* src)              // ORIG 0x08adf1b3
{
    const int curindex = gUnicodeBufferIndex;
    const std::size_t nRet = mbstowcs(gUnicodeBuffer[curindex], src, 0x1000);
    gUnicodeBuffer[curindex][nRet] = 0;
    gUnicodeBufferIndex = (gUnicodeBufferIndex + 1) % 10;
    return gUnicodeBuffer[curindex];
}
const wchar_t* toUnicode(const std::string& src)       // ORIG 0x08adf0f6
{
    const int curindex = gUnicodeBufferIndex;
    const std::size_t nRet = mbstowcs(gUnicodeBuffer[curindex], src.c_str(), 0x1000);
    gUnicodeBuffer[curindex][nRet] = 0;
    gUnicodeBufferIndex = (gUnicodeBufferIndex + 1) % 10;
    return gUnicodeBuffer[curindex];
}
#include "SyncScriptDeps/toUnicode.inc"
}
#include "SyncScriptDeps/toUnicode_1.inc"
}

// ORIG 0x08adf024：toMbcs(char const*, string&) = sDst = pszSrc。
#include "SyncScriptDeps/toMbcs.inc"
}

// ORIG 0x08adef49：toMbcs(string const&, string&) = sDst = sSrc。
#include "SyncScriptDeps/toMbcs_1.inc"
}

// ORIG 0x08adeea0：toMbcs(wchar_t const*)：wcstombs 到 TLS Mbcs 缓冲。
// [b28d-L8] AE | 修法：本 TU TLS 组按 ORIG 布局补 lead4B+gUnicodeBuffer[10][0x1000]+tail0x10（gMbcsBuffer LE=-0x3c028、gTCharBuffer LE=-0xa020，ORIG 0x08adeea0）；待 Main relink 复验
const char* toMbcs(const wchar_t* pszSrc)
{
    int curIndex = gMbcsBufferIndex;
    size_t nRet = wcstombs(gMbcsBuffer[curIndex], pszSrc, 0x1000);
    gMbcsBuffer[curIndex][nRet] = 0;
    gMbcsBufferIndex = (gMbcsBufferIndex + 1) % 10;
    return gMbcsBuffer[curIndex];
}

// ORIG 0x08adede6：toMbcs(wstring const&)：wcstombs 到 TLS Mbcs 缓冲。
// [b28d-L8] AE | 同上（TLS 组布局对齐，ORIG 0x08adede6）
const char* toMbcs(const std::wstring& sSrc)
{
    int curIndex = gMbcsBufferIndex;
    size_t nRet = wcstombs(gMbcsBuffer[curIndex], sSrc.c_str(), 0x1000);
    gMbcsBuffer[curIndex][nRet] = 0;
    gMbcsBufferIndex = (gMbcsBufferIndex + 1) % 10;
    return gMbcsBuffer[curIndex];
}

// ORIG 0x08adf03e：toMbcs(wchar_t const*, string&)：wcstombs 后 sDst = buf。
// [b28d-L8] AE | 同上（TLS 组布局对齐，ORIG 0x08adf03e）
#include "SyncScriptDeps/toMbcs_2.inc"
}

// ORIG 0x08adef63：toMbcs(wstring const&, string&)：wcstombs 后 sDst = buf。
// [b28d-L8] AE | 同上（TLS 组布局对齐，ORIG 0x08adef63）
#include "SyncScriptDeps/toMbcs_3.inc"
}

// ORIG 0x08aded01：toTString(char const*, string&) = sDst = pszSrc。
#include "SyncScriptDeps/toTString.inc"
}

// ORIG 0x08adec26：toTString(string const&, string&) = sDst = sSrc。
#include "SyncScriptDeps/toTString_1.inc"
}

// ORIG 0x08adeb7d：toTString(wchar_t const*)：wcstombs 到 TLS TChar 缓冲。
// [b28d-L8] AE | 同上（TLS 组布局对齐，ORIG 0x08adeb7d）
const char* toTString(const wchar_t* pszSrc)
{
    int curIndex = gTCharBufferIndex;
    size_t nRet = wcstombs(gTCharBuffer[curIndex], pszSrc, 0x1000);
    gTCharBuffer[curIndex][nRet] = 0;
    gTCharBufferIndex = (gTCharBufferIndex + 1) % 10;
    return gTCharBuffer[curIndex];
}

// ORIG 0x08adeac3：toTString(wstring const&)：wcstombs 到 TLS TChar 缓冲。
// [b28d-L8] AE | 同上（TLS 组布局对齐，ORIG 0x08adeac3）
const char* toTString(const std::wstring& sSrc)
{
    int curIndex = gTCharBufferIndex;
    size_t nRet = wcstombs(gTCharBuffer[curIndex], sSrc.c_str(), 0x1000);
    gTCharBuffer[curIndex][nRet] = 0;
    gTCharBufferIndex = (gTCharBufferIndex + 1) % 10;
    return gTCharBuffer[curIndex];
}

// ORIG 0x08aded1b：toTString(wchar_t const*, string&)：wcstombs 后 sDst = buf。
// [b28d-L8] AE | 同上（TLS 组布局对齐，ORIG 0x08aded1b）
#include "SyncScriptDeps/toTString_2.inc"
}

// ORIG 0x08adec40：toTString(wstring const&, string&)：wcstombs 后 sDst = buf。
// [b28d-L8] AE | 同上（TLS 组布局对齐，ORIG 0x08adec40）
#include "SyncScriptDeps/toTString_3.inc"
}

// ORIG 0x088be57e getCharacterJob(const char*)：
// 按 ORIG 0x93f4d40 作业名表（"[swordman]"…"[creator mage]"）顺序 strcmp，
// 命中返回下标 0..10，未命中返回 11。
#include "SyncScriptDeps/getCharacterJob.inc"
}

// ORIG 0x088be5c5 checkValiedJobName(const char*)：遍历 0x93f4d40 作业名表
// strcmp，命中返回 1；与 getCharacterJob 共用同一张表。
#include "SyncScriptDeps/checkValiedJobName.inc"
}

// ==================== APSystem::CSyncScript desc ====================

// ORIG 0x0812268c W：bGM=false→m_actionPoint1（Chn_ActionPointSystem.etc），
// true→m_actionPoint2（_GM.etc）。见 CDataManager/init.md 0x6079 上下文。
#include "SyncScriptDeps/GetActionPointEtcParameter.inc"
}

// ORIG 0x0812262a：GetDBHandle(2,0) → "trUncate table charac_action_point_desc"。

#include "SyncScriptDeps/TruncateDescTable.inc"
}

// ORIG 0x0812268c（249 insn）：G_CDataManager()->GetActionPointEtcParameter(false)
// 取 +0x18 m_actionPointRefs（map<unsigned short, const CActionPointEx*>），
// 拼批量 INSERT：query = "inSert into charac_action_point_desc (action_index,
// action_group_index, action_group_name) values " + 逐项
// "(idx, grp, 'name')"（非末项追加 ","），最后 GetDBHandle(2,0) →
// set_query → exec(true)。元素布局：+0x00 ushort actionIndex、+0x02 int
// groupIndex、+0x24 std::string name；script2Database 转换失败→cMyTrace
// 错误日志并返回 false。CActionPointEx 权威布局尚未落地（APSystemTypes.h
// 为空占位，改其大小会破坏 CDataManager ABI），此处用本地视图结构按 ORIG
// 偏移具名访问 [推断自 InsertDescTable.md 汇编]。
namespace
{

struct __attribute__((packed)) ActionPointExDescView
{
    // packed：ORIG 机器码实证 m_groupIndex 走非对齐直读 `mov 0x2(%eax)`
    // （0x08122757）、name 走 `add $0x24`（0x0812278d）；常规对齐布局会把
    // int 推到 +0x04、string 推到 +0x28 ⇒ 机器码残差。
    unsigned short m_actionIndex;   // +0x00
    int m_groupIndex;               // +0x02
    char m_pad06[0x1e];             // +0x06
    std::string m_name;             // +0x24
};

}  // namespace

// [b28d-L8→b28d-SymRemnant P326 翻案] N82 作废，本件 **AE(248/248 全行一致)**。
// 胜出配方（逐条机器码证据，非沿用旧封顶）：
//  ① G_CDataManager 结果不落槽（0x0812269d 直传参）→ 链式表达式；
//  ② 视图 struct 必须 packed：groupIndex ORIG 非对齐直读 `mov 0x2(%eax)`
//     (0x08122757)、name `add $0x24`(0x0812278d)——对齐版会推到 +0x4/+0x28；
//  ③ query 两步 `std::string query; query = "…";`（C1 后 ssaSEPKc 0x081226d7）；
//  ④ ORIG 0x081226be 构造**第二个未用 string**(槽 -0x4c)、尾部 0x08122a0d 先于
//     query 析构 ⇒ 原码遗留声明（legacyRow），它决定深层槽链与帧 0x4fc；
//  ⑤ 槽序配方：声明序 refs→query→legacyRow→total→**db(裸声明)**→**it(出环
//     具名)**→row ⇒ total -0x28 / db -0x24 / row -0x20 / it -0x50；
//  ⑥ row=0 须在 beginEv 之后发射 ⇒ 迭代器初始化提出 for-init（`for(;cond;)`）；
//  ⑦ 循环步进 **postfix `it++`**（0x08122dfa `_ZNSt23_Rb_tree_const_iterator…ppEi`）；
//  ⑧ 逗号判形 `row < total - 1`（写 `total-1 > row` 被 4.4 规范化成 add/cmp/jae）；
//  ⑨ name 缓冲用 `char name[1024] = { 0 };`（0x08122774 常量 rep stosl；
//     显式 memset 退化成 call memset@plt）；idx/grp/name 声明**交错**在
//     各自 sprintf 前（0x08122746/-0x98、0x08122766/-0xdc、0x08122774/-0x4dc）；
//  ⑩ 错误分支 = tuple 声明后、单语句临时 `cMyTrace(…)(…)`（三串实参先求值
//     %edi/%esi/%ebx 0x081227b1..）+ **return false**（非 ok+break：ORIG 无
//     ok 槽，%esi 是共享清理尾路径标志）；
//  ⑪ 收尾 `if (!db->exec(true)) return false; return true;`（0x081229e3 xor/test/
//     je 形，P101/P84 先例；`== 1` 会被折叠）。
// 并行试过无效的形：具名 mgr 槽；memset；ok+break；`total-1>row`；`++it`；
// size_t total/row；row 声明先于 total（db/total 槽互换）；db 提前 `=0` 初始化
// （多发一条 movl）。P326 的“第二迭代器右值临时”形**不适用**本件：ORIG
// begin/end 皆 `_ZNKSt3map…`（const 直出），无 const_iterator 转换临时。
#include "SyncScriptDeps/InsertDescTable.inc"
}

// ==================== advancealtar::SyncScript desc ====================

#include <cstdio>
#include <cstring>
#include <map>
#include <vector>

#include "CDataManager.h"
#include "DNFFunctionLib.h"  // CodePage::script2Database

namespace advancealtar
{
// BuyUpgradeData / BuyShopData 权威定义在 CDataManager.h（namespace advancealtar）。

// 注：SyncScript 类声明见 SyncScriptDeps.h；getItemValueStirng 为其 static 内部
// 辅助（非 ORIG 独立符号，仅本 TU 使用）。

// ORIG 0x08133e44：GetDBHandle(3,0) → "trUncate table charac_advance_altar_item_desc"。
#include "SyncScriptDeps/truncateItemDescTable.inc"
}

// ORIG 0x08134132：见头注。type==1→+0x10 vec；type==2→+0x1c vec；type==0→+0x04 vec；
// 其它 type 直接返回 false。逐项 CodePage::script2Database 转换 desc 后 sprintf 元组，
// 项间追加 ", "；任一项转换失败走 cMyTrace 错误日志并返回 false。
// [b28d-L8→b28d-SymRemnant P326 重试] 旧判 N82「帧差 0x85c vs 0x55c」已破：
// 帧差唯一来源 = **conv 是 char[0x400]（0x300 差额）**，非 0x100（ORIG 清零
// rep stosl 0x100 dwords @0x081341c6 实证）；tuple 声明须在循环体 conv 之后
// （-0x844 更深）；i=0 在 beginEv 后 ⇒ 迭代器出环具名 + postfix it++；trace
// 单语句临时。**残留 9 行** = switch lowering 末节点方向：ORIG `test; jne fail`
// +T0 fallthrough，4.4.7 对同语义 switch **必发** `test; je T0`+fail fallthrough
// （/tmp/swtest 最小探针实测），if-链形又退化为 mem 直比 `cmpl $N,0x10(%ebp)`
// （ORIG 为单 `mov 0x10,%eax` + 三连寄存器比较 = switch lowering 特征）⇒
// 两形各有不可替代半，封底 9 行。
#include "SyncScriptDeps/getItemValueStirng.inc"
}

// ORIG 0x08133ea6：遍历 G_CDataManager()+0x631c 的 map<int,BuyShopData>
// （advancealtar::AdvanceAltarShopParameter m_altarShop 的 +0 表），对每个
// ridable_id 依次生成 type 0/1/2 的 values 元组（项间以 "," 分隔），拼装
// "inSert into charac_advance_altar_item_desc (ridable_id, item_type,
// item_id, item_desc) values " 后 INSERT；任一行失败返回 0，全部成功返回 1。
// [b28d-L8] N82 | 块序（string C1/双检）+ 槽平移（ORIG 0x08xxxxxx）
#include "SyncScriptDeps/insertItemDescTable.inc"
}
}  // namespace advancealtar
