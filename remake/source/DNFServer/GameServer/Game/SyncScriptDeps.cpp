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
int getMbcsBufferSize()        // ORIG T 0x08adea92
{
    return 0x1000;
}
int getUnicodeBufferSize()     // ORIG T 0x08adea9c
{
    return 0x4000;
}
int getTCharBufferSize()       // ORIG T 0x08adeaa6
{
    return 0x1000;
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
void toUnicode(const char* src, std::wstring& dst)     // ORIG 0x08adf32f
{
    const int curindex = gUnicodeBufferIndex;
    const std::size_t nRet = mbstowcs(gUnicodeBuffer[curindex], src, 0x1000);
    gUnicodeBuffer[curindex][nRet] = 0;
    dst = gUnicodeBuffer[curindex];
    gUnicodeBufferIndex = (gUnicodeBufferIndex + 1) % 10;
}
void toUnicode(const std::string& src, std::wstring& dst)  // ORIG 0x08adf25c
{
    const int curindex = gUnicodeBufferIndex;
    const std::size_t nRet = mbstowcs(gUnicodeBuffer[curindex], src.c_str(), 0x1000);
    gUnicodeBuffer[curindex][nRet] = 0;
    dst = gUnicodeBuffer[curindex];
    gUnicodeBufferIndex = (gUnicodeBufferIndex + 1) % 10;
}

// ORIG 0x08adf024：toMbcs(char const*, string&) = sDst = pszSrc。
void toMbcs(const char* pszSrc, std::string& sDst)
{
    sDst = pszSrc;
}

// ORIG 0x08adef49：toMbcs(string const&, string&) = sDst = sSrc。
void toMbcs(const std::string& sSrc, std::string& sDst)
{
    sDst = sSrc;
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
void toMbcs(const wchar_t* pszSrc, std::string& sDst)
{
    int curIndex = gMbcsBufferIndex;
    size_t nRet = wcstombs(gMbcsBuffer[curIndex], pszSrc, 0x1000);
    gMbcsBuffer[curIndex][nRet] = 0;
    sDst = gMbcsBuffer[curIndex];
    gMbcsBufferIndex = (gMbcsBufferIndex + 1) % 10;
}

// ORIG 0x08adef63：toMbcs(wstring const&, string&)：wcstombs 后 sDst = buf。
// [b28d-L8] AE | 同上（TLS 组布局对齐，ORIG 0x08adef63）
void toMbcs(const std::wstring& sSrc, std::string& sDst)
{
    int curIndex = gMbcsBufferIndex;
    size_t nRet = wcstombs(gMbcsBuffer[curIndex], sSrc.c_str(), 0x1000);
    gMbcsBuffer[curIndex][nRet] = 0;
    sDst = gMbcsBuffer[curIndex];
    gMbcsBufferIndex = (gMbcsBufferIndex + 1) % 10;
}

// ORIG 0x08aded01：toTString(char const*, string&) = sDst = pszSrc。
void toTString(const char* pszSrc, std::string& sDst)
{
    sDst = pszSrc;
}

// ORIG 0x08adec26：toTString(string const&, string&) = sDst = sSrc。
void toTString(const std::string& sSrc, std::string& sDst)
{
    sDst = sSrc;
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
void toTString(const wchar_t* pszSrc, std::string& sDst)
{
    int curIndex = gTCharBufferIndex;
    size_t nRet = wcstombs(gTCharBuffer[curIndex], pszSrc, 0x1000);
    gTCharBuffer[curIndex][nRet] = 0;
    sDst = gTCharBuffer[curIndex];
    gTCharBufferIndex = (gTCharBufferIndex + 1) % 10;
}

// ORIG 0x08adec40：toTString(wstring const&, string&)：wcstombs 后 sDst = buf。
// [b28d-L8] AE | 同上（TLS 组布局对齐，ORIG 0x08adec40）
void toTString(const std::wstring& sSrc, std::string& sDst)
{
    int curIndex = gTCharBufferIndex;
    size_t nRet = wcstombs(gTCharBuffer[curIndex], sSrc.c_str(), 0x1000);
    gTCharBuffer[curIndex][nRet] = 0;
    sDst = gTCharBuffer[curIndex];
    gTCharBufferIndex = (gTCharBufferIndex + 1) % 10;
}

// ORIG 0x088be57e getCharacterJob(const char*)：
// 按 ORIG 0x93f4d40 作业名表（"[swordman]"…"[creator mage]"）顺序 strcmp，
// 命中返回下标 0..10，未命中返回 11。
int getCharacterJob(const char* name)
{
    static const char* const kJobTable[11] = {
        "[swordman]",        // 0
        "[fighter]",         // 1
        "[gunner]",          // 2
        "[mage]",            // 3
        "[priest]",          // 4
        "[at gunner]",       // 5
        "[thief]",           // 6
        "[at fighter]",      // 7
        "[at mage]",         // 8
        "[demonic swordman]",// 9
        "[creator mage]"     // 10
    };
    for (int i = 0; i <= 0xa; ++i)
    {
        if (strcmp(name, kJobTable[i]) == 0)
        {
            return i;
        }
    }
    return 0xb;
}

// ORIG 0x088be5c5 checkValiedJobName(const char*)：遍历 0x93f4d40 作业名表
// strcmp，命中返回 1；与 getCharacterJob 共用同一张表。
int checkValiedJobName(const char* name)
{
    static const char* const kJobTable[12] = {
        "[swordman]",        // 0
        "[fighter]",         // 1
        "[gunner]",          // 2
        "[mage]",            // 3
        "[priest]",          // 4
        "[at gunner]",       // 5
        "[thief]",           // 6
        "[at fighter]",      // 7
        "[at mage]",         // 8
        "[demonic swordman]",// 9
        "[creator mage]",    // 10
        "[all]"              // 11
    };
    for (int i = 0; i <= 0xb; ++i)
    {
        if (strcmp(name, kJobTable[i]) == 0)
        {
            return 1;
        }
    }
    return 0;
}

// ==================== APSystem::CSyncScript desc ====================

// ORIG 0x0812268c W：bGM=false→m_actionPoint1（Chn_ActionPointSystem.etc），
// true→m_actionPoint2（_GM.etc）。见 CDataManager/init.md 0x6079 上下文。
APSystem::CActionPointEtcParameter* CDataManager::GetActionPointEtcParameter(bool bGM)
{
    return bGM ? &m_actionPoint2 : &m_actionPoint1;
}

const std::map<unsigned short, const APSystem::CActionPointEx*>&
APSystem::CActionPointEtcParameter::getActionPointRefs() const
{
    return m_actionPointRefs;
}

// ORIG 0x0812262a：GetDBHandle(2,0) → "trUncate table charac_action_point_desc"。

bool APSystem::CSyncScript::TruncateDescTable()
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)2, (ENUM_SERVER_GROUP)0);
    db->set_query("trUncate table charac_action_point_desc");
    // ORIG 0x08122676：exec 结果 `xor $1,%eax; test %al,%al; je L; mov $0,%eax; jmp end;
    // L: mov $1,%eax`（b28c-L4 探针 f1 逐条全等）。`return !db->exec(true);` 被 4.4 折叠成
    // xor;leave;ret（少 4 条分支），须写成早期返回形保留分支（P101/P84）。
    if (!db->exec(true))
        return false;
    return true;
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

struct ActionPointExDescView
{
    unsigned short m_actionIndex;   // +0x00
    char m_pad02[2];
    int m_groupIndex;               // +0x02
    char m_pad06[0x1e];
    std::string m_name;             // +0x24
};

}  // namespace

// [b28d-L8] N82 | 帧差 0x4fc vs 0x4f0 + 槽平移（ORIG 0x0812268c）
bool APSystem::CSyncScript::InsertDescTable()
{
    CDataManager* mgr = G_CDataManager();
    const std::map<unsigned short, const APSystem::CActionPointEx*>& refs =
        mgr->GetActionPointEtcParameter(false)->getActionPointRefs();

    std::string query =
        "inSert into charac_action_point_desc "
        "(action_index, action_group_index, action_group_name) values ";

    const size_t total = refs.size();
    size_t row = 0;
    bool ok = true;
    for (std::map<unsigned short, const APSystem::CActionPointEx*>::const_iterator it =
             refs.begin(); it != refs.end(); ++it, ++row)
    {
        const ActionPointExDescView* ex =
            reinterpret_cast<const ActionPointExDescView*>(it->second);
        if (ex == 0)
        {
            continue;
        }

        char idx[68];
        char grp[68];
        char name[1024];
        sprintf(idx, "%u", (unsigned int)ex->m_actionIndex);
        sprintf(grp, "%u", (unsigned int)ex->m_groupIndex);
        memset(name, 0, sizeof(name));
        if (!CodePage::script2Database(
                const_cast<char*>(ex->m_name.c_str()), name))
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x409, 5);
            trace("Error!!! %s->%s [%s][%s][%d]",
                  CodePage::script(), CodePage::database(),
                  ex->m_name.c_str(),
                  "static bool APSystem::CSyncScript::InsertDescTable()",
                  0x409);
            ok = false;
            break;
        }

        std::string tuple = "(";
        tuple += idx;
        tuple += ", ";
        tuple += grp;
        tuple += ", ";
        tuple += "'";
        tuple += name;
        tuple += "'";
        tuple += ")";
        if (row + 1 < total)
        {
            tuple += ",";
        }
        query += tuple;
    }

    if (!ok)
    {
        return false;
    }

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)2, (ENUM_SERVER_GROUP)0);
    db->set_query(query.c_str());
    return db->exec(true) == 1;
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
bool SyncScript::truncateItemDescTable()
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)3, (ENUM_SERVER_GROUP)0);
    db->set_query("trUncate table charac_advance_altar_item_desc");
    if (!db->exec(true))
        return false;
    return true;
}

// ORIG 0x08134132：见头注。type==1→+0x10 vec；type==2→+0x1c vec；type==0→+0x04 vec；
// 其它 type 直接返回 false。逐项 CodePage::script2Database 转换 desc 后 sprintf 元组，
// 项间追加 ", "；任一项转换失败走 cMyTrace 错误日志并返回 false。
// [b28d-L8] N82 | 帧差 0x85c vs 0x55c（大局部数组）+ 块序（ORIG 0x08xxxxxx）
bool SyncScript::getItemValueStirng(int ridableId, const BuyShopData& data,
                                    AdvanceAltarShopType::T type, std::string& out)
{
    const std::vector<BuyUpgradeData>* vec = 0;
    switch (type)
    {
    case AdvanceAltarShopType::T_1: vec = &data.m_vecT1; break;
    case AdvanceAltarShopType::T_2: vec = &data.m_vecT2; break;
    case AdvanceAltarShopType::T_0: vec = &data.m_vecT0; break;
    default: return false;
    }

    char tuple[0x400];
    const int total = (int)vec->size();
    int i = 0;
    for (std::vector<BuyUpgradeData>::const_iterator it = vec->begin();
         it != vec->end(); ++it, ++i)
    {
        std::string s;                 // ORIG 循环体内多余局部（汇编构造/析构实证）
        char conv[0x100] = {0};
        if (!CodePage::script2Database(
                const_cast<char*>(it->m_strDesc.c_str()), conv))
        {
            cMyTrace trace(
                "static bool advancealtar::SyncScript::getItemValueStirng(int, const advancealtar::BuyShopData&, advancealtar::AdvanceAltarShopType::T, std::tstring&)",
                0xd36, 5);
            trace("Error!!! %s->%s [%s][%s][%d]",
                  CodePage::script(), CodePage::database(),
                  it->m_strDesc.c_str(),
                  "static bool advancealtar::SyncScript::getItemValueStirng(int, const advancealtar::BuyShopData&, advancealtar::AdvanceAltarShopType::T, std::tstring&)",
                  0xd36);
            return false;
        }
        sprintf(tuple, "(%d,%d,%d,'%s')", ridableId, (int)type,
                it->m_nItemId, conv);
        out += tuple;
        if (i < total - 1)
        {
            out += ", ";
        }
    }
    return true;
}

// ORIG 0x08133ea6：遍历 G_CDataManager()+0x631c 的 map<int,BuyShopData>
// （advancealtar::AdvanceAltarShopParameter m_altarShop 的 +0 表），对每个
// ridable_id 依次生成 type 0/1/2 的 values 元组（项间以 "," 分隔），拼装
// "inSert into charac_advance_altar_item_desc (ridable_id, item_type,
// item_id, item_desc) values " 后 INSERT；任一行失败返回 0，全部成功返回 1。
// [b28d-L8] N82 | 块序（string C1/双检）+ 槽平移（ORIG 0x08xxxxxx）
bool SyncScript::insertItemDescTable()
{
    CDataManager* dm = G_CDataManager();
    if (dm == 0)
    {
        return false;
    }

    // ORIG：数据源位于 CDataManager+0x631c = m_altarShop（AdvanceAltarShopParameter）
    // 的 +0 成员 m_buyShopMap（map<int,BuyShopData>，importAdvanceAltarShopParameter
    // 填充）。若该表未填充，循环体不会执行，返回 true（与 ORIG 空表行为一致）。
    typedef std::map<int, BuyShopData> BuyShopDataMap;
    BuyShopDataMap* shopMap = &dm->m_altarShop.m_buyShopMap;

    std::string query;
    std::string values;
    for (BuyShopDataMap::const_iterator it = shopMap->begin();
         it != shopMap->end(); ++it)
    {
        query =
            "inSert into charac_advance_altar_item_desc \t\t\t\t  "
            "(ridable_id, item_type, item_id, item_desc) values ";
        values.clear();
        if (getItemValueStirng(it->first, it->second,
                               AdvanceAltarShopType::T_0, values) == 0)
        {
            return false;
        }
        values += ",";
        if (getItemValueStirng(it->first, it->second,
                               AdvanceAltarShopType::T_1, values) == 0)
        {
            return false;
        }
        values += ",";
        if (getItemValueStirng(it->first, it->second,
                               AdvanceAltarShopType::T_2, values) == 0)
        {
            return false;
        }
        query += values;

        MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
            (ENUM_DB_HANDLE_IDX)3, (ENUM_SERVER_GROUP)0);
        db->set_query(query.c_str());
        if (db->exec(true) != 1)
        {
            return false;
        }
    }
    return true;
}
}  // namespace advancealtar
