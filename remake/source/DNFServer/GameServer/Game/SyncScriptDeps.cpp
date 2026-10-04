// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 SyncScriptDeps/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
#include "APSystemTypes.h"

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

#include "SyncScriptDeps/getUnicodeBufferSize.inc"

#include "SyncScriptDeps/getTCharBufferSize.inc"


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

#include "SyncScriptDeps/toUnicode_1.inc"


#include "SyncScriptDeps/toMbcs.inc"


#include "SyncScriptDeps/toMbcs_1.inc"


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

#include "SyncScriptDeps/toMbcs_2.inc"


#include "SyncScriptDeps/toMbcs_3.inc"


#include "SyncScriptDeps/toTString.inc"


#include "SyncScriptDeps/toTString_1.inc"


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

#include "SyncScriptDeps/toTString_2.inc"


#include "SyncScriptDeps/toTString_3.inc"


#include "SyncScriptDeps/getCharacterJob.inc"


#include "SyncScriptDeps/checkValiedJobName.inc"


// ==================== APSystem::CSyncScript desc ====================

#include "SyncScriptDeps/GetActionPointEtcParameter.inc"


// ORIG 0x0812262a：GetDBHandle(2,0) → "trUncate table charac_action_point_desc"。

#include "SyncScriptDeps/TruncateDescTable.inc"


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

#include "SyncScriptDeps/InsertDescTable.inc"


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

#include "SyncScriptDeps/truncateItemDescTable.inc"


#include "SyncScriptDeps/getItemValueStirng.inc"


#include "SyncScriptDeps/insertItemDescTable.inc"
{
    // ORIG 0x08133eae..0x08133eb8：G_CDataManager 结果不落槽、**无判空**，
    // `add $0x631c,%eax` 后单槽 -0x18 ⇒ 链式取址（dm 具名槽/判空早退是发明）。
    // 且 size/begin/end 全部 `_ZNKSt3map…`（const 形，0x08133ed7/0x08133eec/
    // 0x081340b4）⇒ shopMap 必须 **const 指针**（非 const 版会多发
    // iterator 转换临时槽 -0x34 与 C2 调用）。
    typedef std::map<int, BuyShopData> BuyShopDataMap;
    const BuyShopDataMap* shopMap = &G_CDataManager()->m_altarShop.m_buyShopMap;

    // 槽布局证据（同 InsertDescTable 配方）：-0x24 query（循环内 ssaSE 重赋值
    // 0x08133f00）、-0x28 **第二未用 string**（0x08133ec6 C1，仅 0x081340f2
    // 尾析构，无任何读写点）、-0x2c it、-0x20 end 临时、-0x30 循环体 values。
    std::string query;
    std::string legacyValues;
    const int total = (int)shopMap->size();   // 0x08133ed7 size→-0x14，ORIG 亦无后续读取
    BuyShopDataMap::const_iterator it = shopMap->begin();
    int row = 0;
    for (; it != shopMap->end(); it++, ++row)
    {
        query =
            "inSert into charac_advance_altar_item_desc \t\t\t\t  "
            "(ridable_id, item_type, item_id, item_desc) values ";
        std::string values;                    // 0x08133f13 循环体 C1（非 clear）
        // ORIG 首实参取 `mov 0x4(%eax)`（pair+4 = BuyShopData 首字 m_nField0
        // 的**值**），非 map key（0x08133f37/0x08133f81/0x08133fcb）；
        // 次实参 = `lea 0x4(%eax)` = &it->second。
        // **返回值不检查**：ORIG 三次 call 后直接 pLE(",")（0x08133f50→0x08133f55
        // 之间无 test/je；失败处理在 getItemValueStirng 内部），旧版判 false
        // 早退属发明，已删。
        getItemValueStirng(it->second.m_nField0, it->second,
                           AdvanceAltarShopType::T_0, values);
        values += ",";
        getItemValueStirng(it->second.m_nField0, it->second,
                           AdvanceAltarShopType::T_1, values);
        values += ",";
        getItemValueStirng(it->second.m_nField0, it->second,
                           AdvanceAltarShopType::T_2, values);
        query += values;

        MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
            (ENUM_DB_HANDLE_IDX)3, (ENUM_SERVER_GROUP)0);
        db->set_query(query.c_str());
        // 0x08134048 xor/test/je 早返回形（P101/P84 先例）。
        if (!db->exec(true))
            return false;
    }
    return true;
}
}  // namespace advancealtar
