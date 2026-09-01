// ============================================================================
// APSystemOnlyOrig.cpp —— Wave5「仅ORIG」APSystem 批（独占 TU；CMake 由 Main
// 统一登记）。
//
// worklist：/tmp/only_orig/APSystem.txt（49 mangled，全部逐符号 DONE）。
// 报告目录：docs/class_func_reports/APSystem__{DB_LoadActionPoint,
//   DB_LoadRewardMedal, DB_UpdateActionPoint, DB_UpdateRewardMedal,
//   CActionPointManager, Inter_LoadRewardMedal}/（各 md 的 ## 汇编 + ## 反编译 C
//   全文已读后转写）。无独立报告者按 objdump 逐指令转写并标注。
//
// 额外实现（Main 授权，唯一属主=本 TU）：
//   * T 0x086b2102 uncompress_zip / T 0x086b201f compress_zip（zlib 包装，
//     objdump 全文转写）
//   * static APSystem::CActionPointManager::_IsGMOn / _gmWeekday 定义
//   * POD 生命周期：_MedalRewardItem / _TodayRewardItem / _SIG_LOAD_ACTION_POINT
// DBDispatcher 本体：唯一定义在 DB_Login.cpp（bool dispatch 版，AltarDB 定稿）；
// 本 TU 仅经 Game/DBDispatcher.h 声明使用。
// ============================================================================

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "CUser.h"                    // 必须先于 MoreTypes.h：避免其 namespace SIG_GIVE_GROWTH_CREATURE_EVENT 抢占 CUser.h 的全局 struct 前置
#include "APSystemTypes.h"            // _SIG_LOAD_ACTION_POINT/_ActionPoint/zip 声明
#include "DB_AccountCargoSync.h"      // APSystem::DB_UpdateActionPoint 权威声明
#include "APSystemDbDispatchers.h"    // 本批 DB 类权威声明(内含 MoreTypes.h)
#include "CUserProc.h"                // CUserProc::SendActionClear / _ActionGroupIndex
#include "GameEnums.h"                // ENUM_PACKETCLASS
#include "MySQL.h"
#include "DBMgr.h"
#include "InterDispatcher.h"         // APSystem::Inter_LoadRewardMedal 类声明
#include "GlobalData.h"               // s_db_mgr/s_stream_pool/s_msgq_mgr/s_systemTime_
#include "Stream.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "CStreamGuard.h"
#include "CEnvironment.h"             // G_CEnvironment / ENUM_DB_HANDLE_IDX / ENUM_SERVER_GROUP
#include "RDARScriptStringManager.h"  // g_scriptStringManager_/findString
#include "CMailBoxHelper.h"           // ReqDBSendNewSystemMail
#include "Inven_Item.h"
#include "LogManager.h"               // compress_zip 错误分支
#include "CDataManager.h"             // G_CDataManager/find_item/GetActionPointEtcParameter

// ---- 本 TU 局部原型（定义分属 GameUtils.cpp/CItemList 域，签名以 ORIG 为准）----
int getDailyInitializeTime(int hour, int min, int sec);   // ORIG T 0x86b1f5a
void __GetCurrentDNFDate(tm& out);                        // ORIG T 0x86b1fc0
int get_rand_int(int range);                              // CItemList 域全局

namespace APSystem
{

// ---- 全局压缩缓冲（Select/Update 使用；ORIG 数据段同名 B 数组语义等价）----
char g_szZipAP_CLEAR_STATE[0x800];
char g_szStrAP_CLEAR_STATE[0x800];

// ---- ORIG 域常量（nm：_ZN8APSystemL…E，r 段；值自 ORIG rodata 读取）----
// gcc 4.4：namespace 内 static const 未使用也发射（内部链接，名含 L）。
static const int ConstMaxZipAP_CLEAR_STATE = 0x800;    // 0x08b5e0ec (2048)
static const int ConstMaxStrAP_CLEAR_STATE = 0x800;    // 0x08b5e0f0 (2048)
// 指针型分隔符常量（ORIG 值 = 指向 rodata 串；本侧指向等价字符串字面量）：
static const char* ConstDelimterPoint = ",";           // 0x08b5e0fc → ",\0vector::_M_insert_aux" 区
static const char* ConstDelimterAction = "|";          // 0x08b5e0f8 → 0x7c('|')
static const char* ConstDelimterCondition = ",";       // 0x08af2e94 → 0x2c(',')
static const int ConstClientClearAction[5] = {2, 5, 0x29, 0x2b, 0x2c};  // 0x08af2e9c (0x14 字节)

// ORIG 域常量（nm：_ZN8APSystemL19ConstOCC_DATELengthE，r 段；"YYYY-MM-DD" 含 NUL）
static const int ConstOCC_DATELength = 11;

unsigned char CActionPointManager::_IsGMOn;                       // ORIG B 0x093ff07c
CActionPointManager::GmWeekdayCtl CActionPointManager::_gmWeekday;  // ORIG D 0x0937008c

}  // namespace APSystem

// ============================================================================
// zlib 包装（本批唯一属主；objdump 全文转写）
// ============================================================================

extern "C" int compress2(unsigned char* dest, unsigned long* destLen,
                         const unsigned char* source, unsigned long sourceLen,
                         int level);
extern "C" int uncompress(unsigned char* dest, unsigned long* destLen,
                          const unsigned char* source, unsigned long sourceLen);

// ----------------------------------------------------------------------------
// uncompress_zip —— ORIG T 0x086b2102（range [0x86b2102,0x86b216d]）：
//   if (inLen==0) { *outLen=0; return true; }
//   raw = *(uint*)in & 0x3fffffff;
//   ret = uncompress(out,&raw,in+4,inLen);
//   *outLen = raw(无条件写回);  return ret==0;
// ----------------------------------------------------------------------------
bool uncompress_zip(char* out, unsigned int& outLen, const char* in, unsigned int inLen)
{
    if (inLen == 0)
    {
        outLen = 0;
        return true;
    }
    unsigned long raw =
        static_cast<unsigned long>(*reinterpret_cast<const unsigned int*>(in)) &
        0x3fffffff;
    if (uncompress(reinterpret_cast<unsigned char*>(out), &raw,
                   reinterpret_cast<const unsigned char*>(in + 4),
                   static_cast<unsigned long>(inLen)) != 0)
    {
        outLen = static_cast<unsigned int>(raw);
        return false;
    }
    outLen = static_cast<unsigned int>(raw);
    return true;
}

// ----------------------------------------------------------------------------
//   bound = len + len/5 + 12（ORIG imul $0x66666667+sar 序列 = gcc 除 5 magic；
//            即 zlib compressBound 的 1.2 倍形式，非 1.1 倍）；
//   zerr = compress2(out+4,&bound,in,len,-1)；失败返回 false；
//   成功但 *outLen（入参容量）< bound → 返回 false（ORIG 防御性容量检查）；
//   成功：out 头 dword = len & 0x3fffffff（ORIG and 截断 30 位），outLen = bound+4。
// ----------------------------------------------------------------------------
bool compress_zip(char* out, unsigned int& outLen, const char* in, unsigned int inLen)
{
    // ORIG 以带符号语义处理 inLen（cmpl+jg 有符号比较、imul 0x66666667+sar 有符号除 5）
    if ((int)inLen <= 0)
    {
        LogManager::logFormat(
            1, "Utility.cpp",
            __PRETTY_FUNCTION__, 0x1c6,
            "d%c%02d");
        return false;
    }
    unsigned long bound =
        static_cast<unsigned long>((int)inLen + (int)inLen / 5 + 12);
    if (compress2(reinterpret_cast<unsigned char*>(out + 4), &bound,
                  reinterpret_cast<const unsigned char*>(in),
                  static_cast<unsigned long>(inLen), -1) != 0)
    {
        return false;
    }
    if (outLen < bound)                        // ORIG：*destCap < size → 失败
        return false;
    *reinterpret_cast<unsigned int*>(out) =
        static_cast<unsigned int>(inLen) & 0x3fffffff;   // ORIG and 截断 30 位
    outLen = static_cast<unsigned int>(bound + 4);
    return true;
}

namespace APSystem
{

inline MySQL* ap_db_handle()   // 两类 Select/Update 均取 GetDBHandle(s_db_mgr,2,0)
{
    return GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
}

// ============================================================================
// POD 生命周期（objdump 转写）
// ============================================================================

// _MedalRewardItem —— W 0x08895712 C1/C2 同体：memset(this,0,0xa)（尾 pad 不清）
_MedalRewardItem::_MedalRewardItem()
{
    memset(this, 0, 0xa);
}

// _TodayRewardItem（顶层）—— W 0x08895736 C1/C2 同体：memset(this,0,0xe)；D1/D2 空体
_TodayRewardItem::_TodayRewardItem()
{
    memset(this, 0, sizeof(*this));   // ORIG 常量 0xe
}
_TodayRewardItem::~_TodayRewardItem() {}

// _SIG_LOAD_ACTION_POINT —— W 0x0848f5d8 C1/C2 同体（0x51 字节）：
//   esi 循环 [300..1] 逐元素调 _ActionPointC1Ev，随后 memset(this,0,0xcf1)。
//   数组成员 _ActionPoint m_actionPoints[300] 由编译器自动逐元素构造
//   （与 ORIG 同形）；旧实现额外显式 D1+placement new 造成双重构造，已删。
_SIG_LOAD_ACTION_POINT::_SIG_LOAD_ACTION_POINT()
{
    memset(this, 0, sizeof(*this));                // 终态与 ORIG 全零一致
}

// ============================================================================
// DB_LoadActionPoint
// ============================================================================

// dispatch —— 报告 dispatch.md 全文转写。ORIG T 0x08123b10。
bool DB_LoadActionPoint::dispatch(int a, int b, Stream* stream)
{
    if (!DBDispatcher::dispatch(a, b, stream))
    {
        return false;
    }
    _SIG_LOAD_ACTION_POINT* sig = stream->GetOutBuffer<_SIG_LOAD_ACTION_POINT>();
    if (sig == 0)
    {
        return false;
    }
    bool ok = Select(*sig);
    SendResult(a, b, *sig, ok);
    return true;
}

// ParseActionClearState —— 报告全文转写。ORIG T 0x08123f50。this 未使用。
bool DB_LoadActionPoint::ParseActionClearState(char* text, _ActionPoint* table,
                                               unsigned int count)
{
    if (text == 0 || table == 0)
    {
        return false;
    }
    char* saveOuter = 0;
    char* line = strtok_r(text, "|", &saveOuter);
    unsigned int row = 0;
    while (line != 0 && row < count)
    {
        char* saveInner = 0;
        char* col = strtok_r(line, ",", &saveInner);
        if (col != 0)
        {
            table[row].m_actionNo = static_cast<short>(atoi(col));   // *(short*)+0
            col = strtok_r(0, ",", &saveInner);
        }
        if (col != 0)
        {
            table[row].m_groupIndex = atoi(col);                     // *(int*)+2
            _ActionGroupIndex groupChk;
            groupChk.m_value = table[row].m_groupIndex;
            if (!APSystem::CActionPointEtcParameter::IsValidActionGroupIndex(groupChk))
            {
                return false;
            }
            col = strtok_r(0, ",", &saveInner);
        }
        for (int slot = 0; col != 0 && slot <= 4; ++slot)
        {
            table[row].m_days[slot] = static_cast<unsigned char>(atoi(col));
            col = strtok_r(0, ",", &saveInner);
        }
        line = strtok_r(0, "|", &saveOuter);
        ++row;
    }
    return true;
}

// Select —— 报告全文转写。ORIG T 0x08123b9e。
bool DB_LoadActionPoint::Select(_SIG_LOAD_ACTION_POINT& sig)
{
    MySQL* db = ap_db_handle();
    int dailyHour =
        G_CEnvironment()->m_serverEnvir.m_fatigueTime;   // +0x37c
    int dailyTime = getDailyInitializeTime(dailyHour, 0, 0);
    time_t tt = dailyTime;
    struct tm tmc;
    localtime_r(&tt, &tmc);
    char date[16];
    sprintf(date, "%04d-%02d-%02d", tmc.tm_year + 1900,
            tmc.tm_mon + 1, tmc.tm_mday);

    db->set_query("select ap_sum, is_reward_medal, is_reward_item_1, "
                  "is_reward_item_2, is_reward_item_3, is_reward_item_4, "
                  "ap_clear_state  from charac_action_point  where charac_no = %u "
                  " and occ_date = '%s' ",
                  sig.m_characNo, date);
    if (!db->exec(true))
    {
        return false;
    }
    if (db->get_n_rows() != 1)
    {
        return false;
    }
    if (!db->fetch())
    {
        return false;
    }

    {
        unsigned int apSumTmp = 0;               // packed 成员不可绑引用，经临时转写
        if (!db->get_uint(0, apSumTmp))          // col0 → sig+4
        {
            return false;
        }
        sig.m_apSum = apSumTmp;
    }
    int col = 1;
    for (int i = 0; i < 5; ++i)             // 五个 ubyte → sig+8..+0xc
    {
        unsigned char flag = 0;
        if (!db->get_ubyte(col++, flag))
        {
            return false;
        }
        *(&sig.m_isRewardMedal + i) = (flag != 0) ? 1 : 0;
    }

    int blobLen = db->get_binary_length(col);   // >0x7ff 拒绝（cmpl $0x7ff;jbe 继续）
    if (!(blobLen <= 0x7ff))
    {
        return false;
    }
    memset(g_szZipAP_CLEAR_STATE, 0, ConstMaxZipAP_CLEAR_STATE);
    if (!db->get_binary(col, g_szZipAP_CLEAR_STATE, blobLen))
    {
        return false;
    }
    unsigned int raw = *reinterpret_cast<unsigned int*>(g_szZipAP_CLEAR_STATE) &
                       0x3fffffff;
    if (!(raw <= 0x7ff))
    {
        return false;
    }
    memset(g_szStrAP_CLEAR_STATE, 0, ConstMaxStrAP_CLEAR_STATE);
    unsigned int outLen = raw;
    if (!uncompress_zip(g_szStrAP_CLEAR_STATE, outLen, g_szZipAP_CLEAR_STATE,
                        static_cast<unsigned int>(blobLen)))
    {
        return false;
    }
    return ParseActionClearState(g_szStrAP_CLEAR_STATE, sig.m_actionPoints, 300);
}

// SendResult —— 报告全文转写。ORIG T 0x08124156。
int DB_LoadActionPoint::SendResult(int a, int b, _SIG_LOAD_ACTION_POINT& sig, bool ok)
{
    if (ok)
    {
        Stream* s = GlobalData::s_stream_pool->Acquire(
            "localchina/localchina_ActionPointSystem_Dispatcher.cpp", 0xb1);
        CStreamGuard guard(s, true);
        guard.operator*()->operator<<(a);
        guard.operator*()->operator<<(b);
        guard.operator->()->put_binary(&sig, 0xcf1);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
    else
    {
        Stream* s = GlobalData::s_stream_pool->Acquire(
            "localchina/localchina_ActionPointSystem_Dispatcher.cpp", 0xbc);
        CStreamGuard guard(s, true);
        guard.operator*()->operator<<(0x12);
        guard.operator*()->operator<<(b);
        guard.operator->()->put_binary(&sig, 0xcf1);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
    return 1;
}

// ctor/dtor —— objdump 0x0844dd7a / 0x0812544c 转写（基类链+置 vptr，无数据成员）
DB_LoadActionPoint::DB_LoadActionPoint() {}
DB_LoadActionPoint::~DB_LoadActionPoint() {}

// ============================================================================
// DB_UpdateActionPoint
// ============================================================================

// MakeActionClearStateString —— 报告全文转写。ORIG T 0x081246d6。
void DB_UpdateActionPoint::MakeActionClearStateString(std::string& out,
                                                      const _ActionPoint* table,
                                                      unsigned int count)
{
    out.clear();
    for (unsigned int row = 0; row < count; ++row)
    {
        const _ActionPoint& rec = table[row];
        _ActionGroupIndex groupChk;
        groupChk.m_value = rec.m_groupIndex;
        if (!APSystem::CActionPointEtcParameter::IsValidActionGroupIndex(groupChk))
        {
            continue;
        }
        char buf[128];
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d|",
                static_cast<int>(rec.m_actionNo),
                rec.m_groupIndex,
                static_cast<int>(static_cast<signed char>(rec.m_days[0])),
                static_cast<int>(static_cast<signed char>(rec.m_days[1])),
                static_cast<int>(static_cast<signed char>(rec.m_days[2])),
                static_cast<int>(static_cast<signed char>(rec.m_days[3])),
                static_cast<int>(static_cast<signed char>(rec.m_days[4])));
        out += buf;
    }
}

// Update —— 报告全文转写。ORIG T 0x08124386。
bool DB_UpdateActionPoint::Update(bool bExpireDate, const _SIG_LOAD_ACTION_POINT& sig)
{
    MySQL* db = ap_db_handle();
    struct tm dateTm;
    if (bExpireDate)   // localTime(now-0x15180)
    {
        int now = GlobalData::s_systemTime_.getCurSec();
        now -= 0x15180;
        time_t tt = now;
        localtime_r(&tt, &dateTm);
    }
    else
    {
        __GetCurrentDNFDate(dateTm);
    }
        char date[ConstOCC_DATELength];
    memset(date, 0, sizeof(date));
    sprintf(date, "%04d-%02d-%02d", dateTm.tm_year + 1900, dateTm.tm_mon + 1,
            dateTm.tm_mday);

    std::string clearState;
    MakeActionClearStateString(clearState, sig.m_actionPoints, 300);

    memset(g_szZipAP_CLEAR_STATE, 0, ConstMaxZipAP_CLEAR_STATE);
    unsigned int zipLen = 0x400;
    if (!compress_zip(g_szZipAP_CLEAR_STATE, zipLen, clearState.c_str(),
                      static_cast<int>(clearState.size())))
    {
        return false;
    }
    // ORIG：压缩结果仍超 0x7ff 字节 → 更新失败（防御性检查，ORIG 实证）
    if (zipLen > 0x7ff)
        return false;

    char* blobStr = db->blob_to_str(0, g_szZipAP_CLEAR_STATE, zipLen);
    db->set_query("update charac_action_point set ap_sum=%d, is_reward_medal=%u, \t\t\t\t\tis_reward_item_1=%u, is_reward_item_2=%u, is_reward_item_3=%u, is_reward_item_4=%u,\t\t\t\t\t\tap_clear_state='%s'\t\t\t\t\t\twhere occ_date='%s'\t\t\t\t\t\tand charac_no=%u",
                  sig.m_apSum, sig.m_isRewardMedal, sig.m_isRewardItem[0],
                  sig.m_isRewardItem[1], sig.m_isRewardItem[2],
                  sig.m_isRewardItem[3], blobStr, date, sig.m_characNo);
    bool execOk = db->exec(true);
    if (!execOk || db->getAffectedRowCount() == 0)
    {
        blobStr = db->blob_to_str(0, g_szZipAP_CLEAR_STATE, zipLen);
        db->set_query("insert into charac_action_point\t\t\t\t\t\t\t(occ_date, charac_no, ap_sum, is_reward_medal, \t\t\t\t\t\t\tis_reward_item_1, is_reward_item_2, is_reward_item_3, is_reward_item_4,\t\t\t\t\t\t\t\tap_clear_state)\t\t\t\t\t\t\t\tvalues \t\t\t\t\t\t\t('%s', %u, %u, %u, %u, %u, %u, %u, '%s')",
                      date, sig.m_characNo, sig.m_apSum, sig.m_isRewardMedal,
                      sig.m_isRewardItem[0], sig.m_isRewardItem[1],
                      sig.m_isRewardItem[2], sig.m_isRewardItem[3], blobStr);
        execOk = db->exec(true);
    }
    return execOk;
}

// dispatch —— 报告全文转写。ORIG T 0x0812432a。
bool DB_UpdateActionPoint::dispatch(int a, int b, Stream* stream)
{
    (void)a;
    (void)b;
    bool expire = false;
    (*stream) >> expire;
    _SIG_LOAD_ACTION_POINT* sig = stream->GetOutBuffer<_SIG_LOAD_ACTION_POINT>();
    if (sig == 0)
    {
        return false;
    }
    Update(expire, *sig);
    return true;
}

DB_UpdateActionPoint::DB_UpdateActionPoint() {}
DB_UpdateActionPoint::~DB_UpdateActionPoint() {}

// ============================================================================
// DB_LoadRewardMedal
// ============================================================================

// makeRequest —— ORIG T 0x0812487a 空体（报告全文转写）
void DB_LoadRewardMedal::makeRequest(int, unsigned int)
{
}

// dispatch —— 报告全文转写。ORIG T 0x08124882。
bool DB_LoadRewardMedal::dispatch(int a, int b, Stream* stream)
{
    if (!DBDispatcher::dispatch(a, b, stream))
    {
        return false;
    }
    _SIG_LOAD_REWARD_MEDAL* sig = stream->GetOutBuffer<_SIG_LOAD_REWARD_MEDAL>();
    if (sig == 0)
    {
        return false;
    }
    if (!Select(*sig))
    {
        return false;
    }
    SendResult(a, b, *sig);
    return true;
}

// Select —— 报告全文转写。ORIG T 0x08124910。
bool DB_LoadRewardMedal::Select(_SIG_LOAD_REWARD_MEDAL& sig)
{
    MySQL* db = ap_db_handle();
    int dailyHour =
        G_CEnvironment()->m_serverEnvir.m_fatigueTime;   // +0x37c
    int dailyTime = getDailyInitializeTime(dailyHour, 0, 0);
    int yesterdaySec = dailyTime - 0x93a80;

    time_t today = dailyTime;
    time_t prev = yesterdaySec;
    struct tm tmToday;
    struct tm tmYesterday;
    localtime_r(&today, &tmToday);
    localtime_r(&prev, &tmYesterday);
    char strToday[ConstOCC_DATELength];
    char strYesterday[ConstOCC_DATELength];
    sprintf(strToday, "%04d-%02d-%02d", tmToday.tm_year + 1900, tmToday.tm_mon + 1,
            tmToday.tm_mday);
    sprintf(strYesterday, "%04d-%02d-%02d", tmYesterday.tm_year + 1900,
            tmYesterday.tm_mon + 1, tmYesterday.tm_mday);

    const std::vector<_MedalRewardItem>& medals =
        G_CDataManager()->GetActionPointEtcParameter(CActionPointManager::_IsGMOn != 0)->m_medalRewards;

    unsigned int minAp = 99999;
    for (unsigned int i = 0; i < medals.size(); ++i)
    {
        if (static_cast<int>(medals[i].m_field00) < static_cast<int>(minAp))
        {
            minAp = medals[i].m_field00;
        }
    }

    db->set_query("select occ_date, ap_sum  \t\t\t\t\tfrom charac_action_point\t\t\t\t\t\twhere charac_no = %u\t\t\t\t\t\tand is_reward_medal = %u \t\t\t\t\tand occ_date < '%s'\t\t\t\t\t\tand occ_date >= '%s'\t\t\t\t\t\tand ap_sum >= %d",
                  sig.m_characNo, 0, strToday, strYesterday,
                  static_cast<int>(minAp));
    if (!db->exec(true))
    {
        return false;
    }
    if (db->get_n_rows() < 1)
    {
        return false;
    }
    if (!db->fetch())
    {
        return false;
    }
    if (!db->get_str(0, sig.m_occDate, 0xb))
    {
        return false;
    }
    {
        unsigned int apSumTmp = 0;               // packed 同上
        if (!db->get_uint(1, apSumTmp))
        {
            return false;
        }
        sig.m_apSum = apSumTmp;
    }
    return true;
}

// SendResult —— 报告全文转写。ORIG T 0x08124b8e。
int DB_LoadRewardMedal::SendResult(int a, int b, _SIG_LOAD_REWARD_MEDAL& sig)
{
    Stream* s = GlobalData::s_stream_pool->Acquire(
        "localchina/localchina_ActionPointSystem_Dispatcher.cpp", 0x1c0);
    CStreamGuard guard(s, true);
    guard.operator*()->operator<<(a);
    guard.operator*()->operator<<(b);
    guard.operator->()->put_binary(&sig, 0x13);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

DB_LoadRewardMedal::DB_LoadRewardMedal() {}
DB_LoadRewardMedal::~DB_LoadRewardMedal() {}

// ============================================================================
// DB_UpdateRewardMedal
// ============================================================================

// makeRequest —— ORIG T 0x08124c70 空体（报告全文转写）
void DB_UpdateRewardMedal::makeRequest(int, const _SIG_LOAD_REWARD_MEDAL&)
{
}

// Update —— 报告汇编+反编译转写。ORIG T 0x08124cb2。
bool DB_UpdateRewardMedal::Update(_SIG_UPDATE_REWARD_MEDAL& sig)
{
    MySQL* db = ap_db_handle();
    db->set_query("update charac_action_point set is_reward_medal=%u \t\t\t\t\twhere charac_no = %u\t\t\t\t\t\tand is_reward_medal = %u \t\t\t\t\tand occ_date = '%s'",
                  1, sig.m_characNo, 0, sig.m_occDate);
    return db->exec(true);
}

// dispatch —— 报告全文转写。ORIG T 0x08124c78。
bool DB_UpdateRewardMedal::dispatch(int a, int b, Stream* stream)
{
    (void)a;
    (void)b;
    _SIG_UPDATE_REWARD_MEDAL* sig =
        stream->GetOutBuffer<_SIG_UPDATE_REWARD_MEDAL>();
    if (sig != 0)
    {
        Update(*sig);
    }
    return sig != 0;
}

DB_UpdateRewardMedal::DB_UpdateRewardMedal() {}
DB_UpdateRewardMedal::~DB_UpdateRewardMedal() {}

// ============================================================================
// CActionPointManager
// ============================================================================

// Set —— ORIG T 0x08121b0c objdump：prev ← m_sig.m_apSum 后 memcpy(this+8,sig,0xcf1)
void CActionPointManager::Set(_SIG_LOAD_ACTION_POINT& sig)
{
    m_prevApSum = m_sig.m_apSum;
    memcpy(&m_sig, &sig, 0xcf1);
}

// GM_Reset —— 报告汇编+反编译全文转写。ORIG T 0x08121d12。
void CActionPointManager::GM_Reset()
{
    m_flag00 = 1;                             // *(this)=1
    memset(&m_sig.m_isRewardItem[0], 0, 4);   // memset(this+0x11,0,4)
    m_sig.m_isRewardMedal = 0;                // this+0x10 = sig+8
    m_sig.m_apSum = 0;                        // *(uint*)(this+0xc)
    m_prevApSum = 0;                          // *(uint*)(this+4)
    for (int row = 0; row <= 299; ++row)
    {
        for (int slot = 0; slot <= 4; ++slot)
        {
            signed char& day =
                *reinterpret_cast<signed char*>(&m_sig.m_actionPoints[row].m_days[slot]);
            if (day > 0)
            {
                day = static_cast<signed char>(-day);
            }
        }
    }
}

// ClearActionOne —— 报告全文转写。ORIG T 0x08121f1c。
bool CActionPointManager::ClearActionOne(_ActionPoint& action, int itemIdx)
{
    if (itemIdx != 0)
    {
        const std::map<unsigned short, const CActionPointEx*>& refs =
            G_CDataManager()->GetActionPointEtcParameter(CActionPointManager::_IsGMOn != 0)
                ->getActionPointRefs();
        std::map<unsigned short, const CActionPointEx*>::const_iterator it =
            refs.find(action.m_actionNo);
        if (it == refs.end())
        {
            return false;
        }
        const CActionPointEx* data = it->second;
        if (data == 0)
        {
            return false;
        }
        bool found = false;
        for (unsigned int i = 0; i < data->m_itemList.size(); ++i)
        {
            if (data->m_itemList[i] == itemIdx)
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            return false;
        }
    }
    for (int slot = 0; slot <= 4; ++slot)
    {
        signed char& day = *reinterpret_cast<signed char*>(&action.m_days[slot]);
        if (day == 0)
        {
            continue;
        }
        if (day > 0)
        {
            return false;   // 正值日：跳过（反编译在 day!=0 且非负时继续下一槽）
        }
        // day < 0：恢复一天行动点并置完成标志
        m_prevApSum = m_sig.m_apSum;
        day = static_cast<signed char>(-day);
        m_sig.m_apSum += day;
        m_flag00 = 1;
        return true;
    }
    return false;
}

// ClearAction —— 报告全文转写。ORIG T 0x08121dcc。
void CActionPointManager::ClearAction(CUser& user, _ActionGroupIndex group, int itemIdx,
                                      ENUM_PACKETCLASS packetClass)
{
    for (int row = 0; row <= 299; ++row)
    {
        if (m_sig.m_actionPoints[row].m_groupIndex == 0)
        {
            break;
        }
        if (m_sig.m_actionPoints[row].m_groupIndex != group.m_value)
        {
            continue;
        }
        _ActionGroupIndex groupOut;
        groupOut.m_value = m_sig.m_actionPoints[row].m_groupIndex;
        if (ClearActionOne(m_sig.m_actionPoints[row], itemIdx))
        {
            CUserProc::SendActionClear(user, m_sig.m_actionPoints[row].m_actionNo,
                                       groupOut, true, packetClass);
            return;
        }
        if (packetClass == 1)
        {
            CUserProc::SendActionClear(user, m_sig.m_actionPoints[row].m_actionNo,
                                       groupOut, false,
                                       static_cast<ENUM_PACKETCLASS>(1));
        }
    }
}

// SetTodayAction —— 报告汇编+反编译全文转写。ORIG T 0x0812161c。
void CActionPointManager::SetTodayAction(int level, unsigned int characNo,
                                         ENUM_EXPERT_JOB_TYPE jobType,
                                         const std::map<unsigned short,
                                                        std::vector<CActionPointEx> >& table)
{
    Reset();
    m_sig.m_characNo = characNo;

    int now = GlobalData::s_systemTime_.getCurSec();
    time_t tt = now;
    struct tm tmc;
    localtime_r(&tt, &tmc);
    int weekday = tmc.tm_wday;

    std::map<unsigned short, std::vector<CActionPointEx> >::const_iterator it =
        table.begin();
    for (int row = 0; row < 300;)
    {
        if (it == table.end())
        {
            break;
        }
        const std::vector<CActionPointEx>& list = it->second;
        int total = static_cast<int>(list.size());
        if (total != 0)
        {
            bool useWeekday = (list[0].m_weekday != 7);
            bool useLevelBand = (list[0].m_bytes02[0x0d] != 0);   // B+0x0f 单字节判定

            std::vector<const CActionPointEx*> candidates(
                total, static_cast<const CActionPointEx*>(0));
            int remain = 0;
            for (int pass = 0; pass < total; ++pass)
            {
                const CActionPointEx* data = &list[pass];
                if (useWeekday)
                {
                    if (_gmWeekday.m_gmOn != 0)
                    {
                        weekday = _gmWeekday.m_weekday;
                    }
                    if (data->m_weekday != weekday)
                    {
                        continue;
                    }
                }
                if (useLevelBand)
                {
                    unsigned char lo = data->m_bytes02[0x0d];   // B+0x0f
                    unsigned char hi = data->m_bytes02[0x0e];   // B+0x10
                    if (level < static_cast<int>(lo) ||
                        static_cast<int>(hi) < level)
                    {
                        continue;
                    }
                }
                int group =
                    *reinterpret_cast<const int*>(&data->m_bytes02[0]);  // B+0x02
                if (group == 0x1d || group == 0x1e || group == 0x1f ||
                    group == 0x20 || group == 0x2d)
                {
                    bool mismatch =
                        (group == 0x1d && jobType != ENUM_EXPERT_JOB_TYPE_2) ||
                        (group == 0x1e && jobType != ENUM_EXPERT_JOB_TYPE_4) ||
                        (group == 0x1f && jobType != ENUM_EXPERT_JOB_TYPE_1) ||
                        (group == 0x20 && jobType != ENUM_EXPERT_JOB_TYPE_3) ||
                        (group == 0x2d && jobType != ENUM_EXPERT_JOB_TYPE_3);
                    if (mismatch)
                    {
                        continue;
                    }
                }
                else if (group == 0x22 && jobType != ENUM_EXPERT_JOB_TYPE_0)
                {
                    continue;
                }
                candidates[pass] = data;
                ++remain;
            }

            if (remain == 0)
            {
                --row;   // 反编译口径：该键贡献 0 行后行计数回退再统一前进
            }
            else
            {
                int pick = get_rand_int(remain);
                const CActionPointEx* chosen = 0;
                int seen = 0;
                for (int i = 0; i < total; ++i)
                {
                    if (candidates[i] != 0)
                    {
                        if (seen == pick)
                        {
                            chosen = candidates[i];
                            break;
                        }
                        ++seen;
                    }
                }
                if (chosen == 0)
                {
                    --row;
                }
                else
                {
                    memcpy(&m_sig.m_actionPoints[row],
                           reinterpret_cast<const void*>(chosen), 0xb);
                }
            }
        }
        ++row;
        ++it;
    }
    m_flag00 = 1;
}

// GetTodayRewardAndSendMail —— 报告全文转写。ORIG T 0x08122104。
void CActionPointManager::GetTodayRewardAndSendMail(
    CUser& user, _SIG_LOAD_ACTION_POINT::_TodayRewardItem reward)
{
    const std::vector<APSystem::_TodayRewardItem>& todays =
        G_CDataManager()->GetActionPointEtcParameter(CActionPointManager::_IsGMOn != 0)->m_todayRewards;

    for (unsigned int i = 0; i < todays.size(); ++i)
    {
        const APSystem::_TodayRewardItem& entry = todays[i];
        if (reward.m_itemIdx != entry.m_itemIdx)
        {
            continue;
        }
        if (entry.m_minAp > m_sig.m_apSum || m_sig.m_apSum > entry.m_maxAp)
        {
            continue;
        }
        unsigned char& flag = m_sig.m_isRewardItem[reward.m_itemIdx];  // this+0x11+idx
        if (flag == 1)
        {
            continue;
        }
        flag = 1;
        m_flag00 = 1;   // LAB 入口 *this = 1（find_item 之前）

        CItem* itemDef = G_CDataManager()->find_item(entry.m_rewardItemIdx);
        Inven_Item item;
        if (itemDef == 0)
        {
            return;
        }
        itemDef->make_item(item);
        // asm：count word → Inven_Item+0x07；elem+4 双字 → Inven_Item+0x02
        *reinterpret_cast<unsigned short*>(&item.m_addInfo2) =
            entry.m_count;
        item.m_addInfo = entry.m_rewardItemIdx;

        const char* title =
            g_scriptStringManager_.findString(4, "chn_game_server_msg_82", 0);
        char titleBuf[30];
        strncpy(titleBuf, title, 0x1d);
        titleBuf[sizeof(titleBuf) - 1] = '\0';
        const char* msg =
            g_scriptStringManager_.findString(4, "chn_game_server_msg_83", 0);
        char msgBuf[256];
        strncpy(msgBuf, msg, 0xff);
        msgBuf[sizeof(msgBuf) - 1] = '\0';

        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            titleBuf, item, 0, user.get_charac_no(-1), msgBuf, strlen(msgBuf),
            8, user.GetServerGroup(), false, false);
        return;
    }
}

// ============================================================================
// Inter_LoadRewardMedal::SendMail —— 报告全文转写。ORIG T 0x08124ea0。
// 声明位于 InterDispatcher.h（类内 public）。
// ============================================================================
void Inter_LoadRewardMedal::SendMail(CUser& user, unsigned int medalCount)
{
    const std::vector<_MedalRewardItem>& medals =
        G_CDataManager()->GetActionPointEtcParameter(CActionPointManager::_IsGMOn != 0)->m_medalRewards;

    const _MedalRewardItem* hit = 0;
    for (unsigned int i = 0; i < medals.size(); ++i)
    {
        const _MedalRewardItem& m = medals[i];
        if (!(medalCount < m.m_field00) && !(m.m_field02 < medalCount))
        {
            hit = &m;
            break;
        }
        if (!(medalCount < m.m_field00) && m.m_field00 == 100)
        {
            hit = &m;
            break;
        }
    }
    if (hit == 0)
    {
        return;
    }

    CItem* itemDef = G_CDataManager()->find_item(hit->m_field04);
    Inven_Item item;
    if (itemDef == 0)
    {
        return;
    }
    itemDef->make_item(item);
    *reinterpret_cast<unsigned short*>(&item.m_addInfo2) =
        hit->m_field08;
    item.m_addInfo = hit->m_field04;

    const char* title =
        g_scriptStringManager_.findString(4, "chn_game_server_msg_82", 0);
    char titleBuf[30];
    strncpy(titleBuf, title, 0x1d);
    titleBuf[sizeof(titleBuf) - 1] = '\0';
    const char* msg =
        g_scriptStringManager_.findString(4, "chn_game_server_msg_83", 0);
    char msgBuf[256];
    strncpy(msgBuf, msg, 0xff);
    msgBuf[sizeof(msgBuf) - 1] = '\0';

    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        titleBuf, item, 0, user.get_charac_no(-1), msgBuf, strlen(msgBuf), 8,
        user.GetServerGroup(), false, false);
}

}  // namespace APSystem
