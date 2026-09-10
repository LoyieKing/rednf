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
#include "DNFFunctionLib.h"         // DNFFLib::get_rand_int（ORIG 0x08108b06）

// ---- 本 TU 局部原型（定义分属 GameUtils.cpp/CItemList 域，签名以 ORIG 为准）----
int getDailyInitializeTime(int hour, int min, int sec);   // ORIG T 0x86b1f5a
void __GetCurrentDNFDate(tm& out);                        // ORIG T 0x86b1fc0

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

bool CActionPointManager::_IsGMOn;                                // ORIG B 0x093ff07c
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
    int ret = uncompress(reinterpret_cast<unsigned char*>(out), &raw,
                         reinterpret_cast<const unsigned char*>(in + 4),
                         static_cast<unsigned long>(inLen));
    outLen = static_cast<unsigned int>(raw);
    if (ret != 0)
        return false;
    return true;
}

// ----------------------------------------------------------------------------
//   bound = len + len/5 + 12（ORIG imul $0x66666667+sar 序列 = gcc 除 5 magic；
//            即 zlib compressBound 的 1.2 倍形式，非 1.1 倍）；
//   zerr = compress2(out+4,&bound,in,len,-1)；失败返回 false；
//   成功但 *outLen（入参容量）< bound → 返回 false（ORIG 防御性容量检查）；
//   成功：out 头 dword = len & 0x3fffffff（ORIG and 截断 30 位），outLen = bound+4。
// ----------------------------------------------------------------------------
// [b28d-27] DIFF N82（已按 P0 修）：ORIG 0x086b201f；补回 out/inLen/bound/dst4/ret
// 五个具名局部（ORIG 有槽往返）后 ndiff 35→14，残差为跨版本槽号分配（N82），
// [b28d-solo-APSystemOnlyOrig] 判词：**AE**（check_tu routed；66=66；残差仅 <A>/<T> 归一）。
// p167 CLEAR（2/2），语义闭合。
bool compress_zip(char* out, unsigned int& outLen, const char* in, unsigned int inLen)
{
    // [b28d-27] ORIG 0x086b201f 逐条：形参 out/inLen 各复制进具名局部（-0x10/-0x14，
    // P0 槽往返证据），bound 为 **int**（imul 0x66666667+sar 有符号除 5，槽 -0x1c），
    // dst4 具名指针（-0xc），compress2 返回值具名 ret（-0x18），末段 bound 先写回
    // 再赋 outLen（两步）。
    // [b28d-solo-APSystemOnlyOrig] 声明序 = ORIG 槽位映射（P309）：bound -0x1c、
    // ret -0x18、len -0x14、dst -0x10、dst4 -0xc；赋值语句序保持 ORIG 指令序。
    unsigned int bound;
    int ret;
    int len;
    char* dst;
    char* dst4;
    dst = out;
    len = (int)inLen;
    if (len <= 0)
    {
        // [b28d-solo-APSystemOnlyOrig] ORIG 0x086b2037 立即数 = 0x8cf5ab8
        // "error; compress failed:unexpected message size"（旧写 "d%c%02d"
        // 是相邻字面量 0x8cf5ab0，日志内容错）。
        LogManager::logFormat(
            1, "Utility.cpp",
            __PRETTY_FUNCTION__, 0x1c6,
            "error; compress failed:unexpected message size");
        return false;
    }
    // [b28d-solo-APSystemOnlyOrig] bound 为 unsigned（ORIG 尾段
    // `mov -0x1c,%edx; mov 0xc,%eax; mov %edx,(%eax)` 单段直落；int 版多
    // `mov %eax,%edx` 中转）。除法仍按 int 有符号 magic 0x66666667。
    bound = static_cast<unsigned int>(len + len / 5 + 12);
    // dst4 声明 char*、调用点再转 unsigned char*：ORIG 0x086b209e 在压参前把
    // arg0 预装 %eax（call 点转换触发提前物化）；直接传 unsigned char* 时 4.4
    // 把 arg0 排到最后装载。
    dst4 = dst + 4;
    ret = compress2(reinterpret_cast<unsigned char*>(dst4),
                    reinterpret_cast<unsigned long*>(&bound),
                    reinterpret_cast<const unsigned char*>(in),
                    static_cast<unsigned long>(len), -1);
    // [b28d-solo-APSystemOnlyOrig] ORIG 0x086b20c3：单条负向守卫
    // `ret != 0 || outLen < bound` → jne/jae 双跳同落 fail（P214① 正极性源形），
    // 两个独立 if-return 会翻成 fail 直落布局。
    if (ret != 0 || outLen < bound)
        return false;
    *reinterpret_cast<unsigned int*>(dst) =
        static_cast<unsigned int>(len) & 0x3fffffff;  // ORIG and 截断 30 位
    bound += 4;
    outLen = bound;
    return true;
}

namespace APSystem
{

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
    return SendResult(a, b, *sig, ok);
}

// ParseActionClearState —— 报告全文转写。ORIG T 0x08123f50。this 未使用。
// [b28d-27] DIFF N82：ORIG 0x08123f50；p167 CLEAR（10/10），残差为帧/槽分配，
// [b28d-solo-APSystemOnlyOrig] 判词：**AE**（check_tu routed；142=142；三独立列指针 col1/col2/col3 + day 字节局部 -0x9 还原后全等）。
// 语义闭合。
bool DB_LoadActionPoint::ParseActionClearState(char* text, _ActionPoint* table,
                                               unsigned int count)
{
    if (text == 0 || table == 0)
    {
        return false;
    }
    // [b28d-solo-APSystemOnlyOrig] ORIG 0x08123f6d..0x08124145 逐槽还原：
    // saveOuter -0x28、line -0x24（先 0 后赋值两句，P198①）、row -0x20、
    // saveInner -0x2c、**三个独立列指针** col1 -0x1c / col2 -0x18 / col3 -0x14
    // （ORIG 各自 movl $0 预清，复用单一 col 会少 2 个槽+2 条预清）、slot -0x10。
    char* saveOuter = 0;
    char* line = 0;
    line = strtok_r(text, "|", &saveOuter);
    unsigned int row = 0;
    while (line != 0 && row < count)
    {
        char* saveInner = 0;
        char* col1 = 0;
        col1 = strtok_r(line, ",", &saveInner);
        char* col2 = 0;
        char* col3 = 0;
        if (col1 != 0)
        {
            table[row].m_actionNo = static_cast<short>(atoi(col1));   // *(short*)+0
            col2 = strtok_r(0, ",", &saveInner);
        }
        if (col2 != 0)
        {
            table[row].m_groupIndex = atoi(col2);                    // *(int*)+2
            // P0：Ghidra C 无 groupChk 中间局部 —— IsValidActionGroupIndex 参数直接读字段
            if (!APSystem::CActionPointEtcParameter::IsValidActionGroupIndex(
                    (_ActionGroupIndex)table[row].m_groupIndex))
            {
                return false;
            }
            col3 = strtok_r(0, ",", &saveInner);
        }
        for (int slot = 0; col3 != 0 && slot <= 4; ++slot)
        {
            // [b28d-solo-APSystemOnlyOrig] ORIG 0x081240a8：atoi 结果先落字节
            // 局部 -0x9（`mov %al,-0x9; movzbl -0x9,%ecx; mov %cl,6(%edx,%eax,1)`）
            // 再算 LHS 地址 ⇒ 无需 callee-saved 跨 atoi 保活（去 push esi）。
            unsigned char day = static_cast<unsigned char>(atoi(col3));
            table[row].m_days[slot] = day;
            col3 = strtok_r(0, ",", &saveInner);
        }
        line = strtok_r(0, "|", &saveOuter);
        ++row;
    }
    return true;
}

// Select —— 报告全文转写。ORIG T 0x08123b9e。
// [b28d-27] DIFF N82/N89：ORIG 0x08123b9e；p167 CLEAR（21/21），残差为 SQL/STL
// [b28d-solo-APSystemOnlyOrig] 判词：**gap -3→0**（本轮还原：列号具名局部 `col++` 后置自增、5 次 get_ubyte 复用同一 flag 槽 -0x39、col 后死局部 -0x14、apSum 直传 `&sig.m_apSum`、blobLen unsigned、尾部 `if(!Parse)return false; return true;`；245=245）。残差 = N93附（nRows `jne false岛` vs 4.4 `je+mov$0+jmp`）。p167 CLEAR 21/21。
// 实例与帧/槽分配，语义闭合。
bool DB_LoadActionPoint::Select(_SIG_LOAD_ACTION_POINT& sig)
{
    // 伪 helper 归一（P49-P50）：ORIG 直接调 DBMgr::GetDBHandle，勿经 inline 中继
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    int dailyHour =
        G_CEnvironment()->m_serverEnvir.m_fatigueTime;   // +0x37c
    // [b28d-solo-APSystemOnlyOrig] ORIG：getDailyInitializeTime 结果直落 -0x2c
    // 并作为 localtime_r 实参（无 tt 副本；旧版多 `mov -0x20,%eax; mov %eax,-0x2c`）。
    time_t dailyTime = getDailyInitializeTime(dailyHour, 0, 0);
    struct tm tmc;
    localtime_r(&dailyTime, &tmc);
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
    // [b28d-solo-APSystemOnlyOrig] ORIG 0x08123c88..0x08123c8f：行数落具名局部
    // -0x1c 后 `cmpl $0x1,-0x1c; jne`（无 setne 物化）。
    int nRows = db->get_n_rows();
    if (nRows != 1)
    {
        return false;
    }
    if (!db->fetch())
    {
        return false;
    }

    // [b28d-solo-APSystemOnlyOrig] ORIG 0x08123cf0..0x08123e37：列号是具名局部
    // -0x18（初值 0，`mov S,%eax; addl $0x1,S` 后置自增），5 次 get_ubyte 复用
    // **同一 flag 槽** -0x39（仅首次 `movb $0x0` 初始化），apSum 直接传
    // `&sig.m_apSum`（ORIG `lea 0x4(%eax),%edx`，无 apSumTmp 临时）。
    unsigned int col = 0;
    // [b28d-solo-APSystemOnlyOrig] ORIG 0x08123cb8：col 之后另有
    // `movl $0x0,-0x14` 死局部（全函数无读点，P171 照抄）。
    int deadSlot = 0;
    (void)deadSlot;
    // packed 字段不能直接绑定到 unsigned int&（GCC 报错，且 reinterpret_cast
    // 自 &sig.m_apSum 仍被追踪）⇒ 经 char* 基址 + offsetof 转写；codegen 仍为
    // ORIG 的 `lea 0x4(%eax),%edx`。
    if (!db->get_uint(col++, *reinterpret_cast<unsigned int*>(
                                 reinterpret_cast<char*>(&sig) + 4)))
    {
        return false;
    }
    unsigned char flag = 0;
    if (!db->get_ubyte(col++, flag)) return false;
    sig.m_isRewardMedal = (flag != 0);
    if (!db->get_ubyte(col++, flag)) return false;
    sig.m_isRewardItem[0] = (flag != 0);
    if (!db->get_ubyte(col++, flag)) return false;
    sig.m_isRewardItem[1] = (flag != 0);
    if (!db->get_ubyte(col++, flag)) return false;
    sig.m_isRewardItem[2] = (flag != 0);
    if (!db->get_ubyte(col++, flag)) return false;
    sig.m_isRewardItem[3] = (flag != 0);

    // [b28d-solo-APSystemOnlyOrig] ORIG 0x08123e3a：`cmpl $0x7ff,-0x10; jbe`
    // （无符号比较）⇒ 局部类型 unsigned。
    unsigned int blobLen = db->get_binary_length(col);   // 第 7 列 ap_clear_state；>0x7ff 拒绝
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
    // [b28d-solo-APSystemOnlyOrig] ORIG 0x08123f2f..0x08123f4e：`call; xor $1;
    // test; je ok; mov $0,%eax; jmp ret; ok: mov $1,%eax; leave; ret`
    // ⇒ 源码是 `if (!Parse...) return false; return true;`（直返 call 会丢物化岛）。
    if (!ParseActionClearState(g_szStrAP_CLEAR_STATE, sig.m_actionPoints, 300))
    {
        return false;
    }
    return true;
}

// SendResult —— 报告全文转写。ORIG T 0x08124156。
bool DB_LoadActionPoint::SendResult(int a, int b, _SIG_LOAD_ACTION_POINT& sig, bool ok)
{
    if (ok)
    {
        // P0：ORIG 无 Stream* 中间局部 —— Acquire 返回值直传 CStreamGuard ctor
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "localchina/localchina_ActionPointSystem_Dispatcher.cpp", 0xb1), true);
        guard.operator*()->operator<<(a);
        guard.operator*()->operator<<(b);
        guard.operator->()->put_binary(&sig, 0xcf1);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
    else
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "localchina/localchina_ActionPointSystem_Dispatcher.cpp", 0xbc), true);
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
// [b28d-27] DIFF（p167 MISSING 4）：ORIG 0x081246d6；ORIG 多 std::string 临时
// [b28d-solo-APSystemOnlyOrig] 判词：**AE**（check_tu routed；137=137；P157 去 rec 引用缓存 + groupIndex vararg 走 unsigned 两段中转后全等）。
// 构造/析构（_ZNSsC1Ev/D1Ev×2）+ _Unwind_Resume，我方缺临时对象与 EH 清理路径。
void DB_UpdateActionPoint::MakeActionClearStateString(std::string& out,
                                                      const _ActionPoint* table,
                                                      unsigned int count)
{
    out.clear();
    // [b28d-27] ORIG 0x081246d6：out.clear() 后立即 `_ZNSsC1Ev`（具名 std::string
    // 局部），函数尾 D1Ev + _Unwind_Resume（EH 清理）；该局部在 ORIG 中未被赋值
    // （死局部，P171 照抄）。
    std::string tmp;
    (void)tmp;
    // [b28d-solo-APSystemOnlyOrig] ORIG 0x08124704..0x081247d9：**无**行指针/
    // groupChk 具名局部（ORIG 每个字段各自重算 row*0xb 全链，共 7 次，P183
    // 地址重物化铁律）；缓存 rec/groupChk 每处省 6 条 ⇒ 97 vs ORIG 137。
    for (unsigned int row = 0; row < count; ++row)
    {
        if (!APSystem::CActionPointEtcParameter::IsValidActionGroupIndex(
                (_ActionGroupIndex)table[row].m_groupIndex))
        {
            continue;
        }
        char buf[128];
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d|",
                static_cast<int>(table[row].m_actionNo),
                static_cast<unsigned int>(table[row].m_groupIndex),
                static_cast<int>(static_cast<signed char>(table[row].m_days[0])),
                static_cast<int>(static_cast<signed char>(table[row].m_days[1])),
                static_cast<int>(static_cast<signed char>(table[row].m_days[2])),
                static_cast<int>(static_cast<signed char>(table[row].m_days[3])),
                static_cast<int>(static_cast<signed char>(table[row].m_days[4])));
        out += buf;
    }
}

// Update —— 报告全文转写。ORIG T 0x08124386。
// [b28d-27] DIFF N82/N89：ORIG 0x08124386；p167 CLEAR（22/22），残差为 SQL/STL
// [b28d-solo-APSystemOnlyOrig] 判词：**gap 0**（本轮还原 ORIG 0x08124472..d7 指令序：zipLen=0x400 → `cmp $0x7ff; jbe` → memset → compress_zip；旧版把检查放 compress 后、memset 放 zipLen 前 ⇒ 整段错位）。残差 = 实参装载时刻（ORIG 提前装 sig.m_apSum 到 %ebx）+ 寄存器分配（N7/N82）。p167 CLEAR 22/22。
// 实例与帧/槽分配，语义闭合。
bool DB_UpdateActionPoint::Update(bool bExpireDate, const _SIG_LOAD_ACTION_POINT& sig)
{
    // 伪 helper 归一：ORIG 直接调 DBMgr::GetDBHandle
    // [b28d-solo-APSystemOnlyOrig] 残差 = 全帧槽基浅 4（db -0x24↔-0x20、
    // now -0x3c↔-0x38、dateTm -0x68↔-0x64、date -0x30↔-0x2c）+ 帧 0xbc↔0xac；
    // P190 double framePad 实测反劣（-0x6c→-0x7c、diff 268→280）⇒ N82/N115 封顶。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    struct tm dateTm;
    if (bExpireDate)   // localTime(now-0x15180)
    {
        // [b28d-solo-APSystemOnlyOrig] ORIG 0x081243ca..0x081243e5：单个
        // time_t 局部 -0x38，`now = now - 0x15180` 为 load/sub/store 三形
        // （地址被 localtime_r 取用，故非内存 add）；旧版 int now + time_t tt
        // 双局部多 4 条 + 帧 +0x10。
        time_t now = GlobalData::s_systemTime_.getCurSec();
        now = now - 0x15180;
        localtime_r(&now, &dateTm);
    }
    else
    {
        __GetCurrentDNFDate(dateTm);
    }
    // [b28d-solo-APSystemOnlyOrig] ORIG 0x081243f7：memset 长度 0xc（date 槽
    // -0x2c 起 12 字节；旧版 11 字节短 1）。
    char date[ConstOCC_DATELength + 1];
    memset(date, 0, sizeof(date));
    sprintf(date, "%04d-%02d-%02d", dateTm.tm_year + 1900, dateTm.tm_mon + 1,
            dateTm.tm_mday);

    std::string clearState;
    MakeActionClearStateString(clearState, sig.m_actionPoints, 300);

    // [b28d-solo-APSystemOnlyOrig] ORIG 0x08124472..0x081244d7 指令序 =
    // `zipLen=0x400` → `cmp $0x7ff; jbe`（容量检查）→ memset → compress_zip；
    // 旧版把检查放在 compress 之后、memset 放在 zipLen 之前 ⇒ 整段错位。
    unsigned int zipLen = 0x400;
    if (zipLen > 0x7ff)
        return false;
    memset(g_szZipAP_CLEAR_STATE, 0, ConstMaxZipAP_CLEAR_STATE);
    if (!compress_zip(g_szZipAP_CLEAR_STATE, zipLen, clearState.c_str(),
                      static_cast<int>(clearState.size())))
    {
        return false;
    }

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
    // [b28c-4] ORIG 0x0812487a：空体保留 sub $0x10 帧（未使用局部）。
    char framePad[0x10];
    (void)framePad;
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
    if (Select(*sig))
    {
        return SendResult(a, b, *sig);
    }
    return false;
}

// Select —— 报告全文转写。ORIG T 0x08124910。
// [b28d-27] DIFF N82/N89：ORIG 0x08124910；p167 CLEAR（18/18），残差为 SQL/STL
// [b28d-solo-APSystemOnlyOrig] 判词：**gap -4→-1**（本轮还原：medalCount=size() 先于 minAp=99999、strToday/strYesterday 16 字节、get_uint 直传 &sig.m_apSum；165 vs 166）。残差 = N93附（nRows `jle` vs 4.4 `jg+mov$0+jmp`）。p167 CLEAR 18/18。
// 实例与帧/槽分配，语义闭合。
bool DB_LoadRewardMedal::Select(_SIG_LOAD_REWARD_MEDAL& sig)
{
    // 伪 helper 归一：ORIG 直接调 DBMgr::GetDBHandle
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    int dailyHour =
        G_CEnvironment()->m_serverEnvir.m_fatigueTime;   // +0x37c
    time_t dailyTime = getDailyInitializeTime(dailyHour, 0, 0);
    time_t yesterdaySec = dailyTime - 0x93a80;

    struct tm tmToday;
    struct tm tmYesterday;
    // P0：Ghidra C 无 today/prev 中间局部 —— localtime_r 直接取 dailyTime 槽地址
    localtime_r(&dailyTime, &tmToday);
    localtime_r(&yesterdaySec, &tmYesterday);
    // [b28d-solo-APSystemOnlyOrig] ORIG 0x08124910：两个 sprintf 目标各 16 字节
    // （Ghidra local_40[16]/local_50[16]；旧版 11 字节短 5）。
    char strToday[16];
    char strYesterday[16];
    sprintf(strToday, "%04d-%02d-%02d", tmToday.tm_year + 1900, tmToday.tm_mon + 1,
            tmToday.tm_mday);
    sprintf(strYesterday, "%04d-%02d-%02d", tmYesterday.tm_year + 1900,
            tmYesterday.tm_mon + 1, tmYesterday.tm_mday);

    const std::vector<_MedalRewardItem>& medals =
        G_CDataManager()->GetActionPointEtcParameter(CActionPointManager::_IsGMOn != 0)->m_medalRewards;

    // [b28d-solo-APSystemOnlyOrig] ORIG 0x08124910：size() 先于 99999 初始化
    // （Ghidra local_1c=size、local_18=99999）。
    std::size_t medalCount = medals.size();
    unsigned int minAp = 99999;
    for (unsigned int i = 0; i < medalCount; ++i)
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
    // P0：Ghidra C 具名局部 local_14 = get_n_rows()
    int nRows = db->get_n_rows();
    if (nRows < 1)
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
    // [b28d-solo-APSystemOnlyOrig] ORIG 0x08124b4d：`mov 0xc(%ebp),%eax;
    // add $0x4,%eax` 直传 &sig.m_apSum（packed 字段经 char* 基址转写）。
    if (!db->get_uint(1, *reinterpret_cast<unsigned int*>(
                             reinterpret_cast<char*>(&sig) + 4)))
    {
        return false;
    }
    return true;
}

// SendResult —— 报告全文转写。ORIG T 0x08124b8e。
bool DB_LoadRewardMedal::SendResult(int a, int b, _SIG_LOAD_REWARD_MEDAL& sig)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           "localchina/localchina_ActionPointSystem_Dispatcher.cpp", 0x1c0),
                       true);
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
    // [b28c-4] ORIG 0x08124c70：空体保留 sub $0x10 帧（未使用局部）。
    char framePad[0x10];
    (void)framePad;
}

// Update —— 报告汇编+反编译转写。ORIG T 0x08124cb2。
bool DB_UpdateRewardMedal::Update(_SIG_UPDATE_REWARD_MEDAL& sig)
{
    // ORIG 0x08124cb8..+0x1e：GetDBHandle(s_db_mgr,2,0) 直接内联（非经辅助函
    // 数），db 存 -0x10 槽。ap_db_handle() 中继会退化为外联 call 差出调用点。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    db->set_query("update charac_action_point set is_reward_medal=%u \t\t\t\t\twhere charac_no = %u\t\t\t\t\t\tand is_reward_medal = %u \t\t\t\t\tand occ_date = '%s'",
                  1, sig.m_characNo, 0, sig.m_occDate);
    // ORIG 0x08124d21..+0x72：exec 返回 bool 先落 -0x9 槽再 movzbl 回传。
    bool ret = db->exec(true);
    return ret;
}

// dispatch —— 报告全文转写。ORIG T 0x08124c78。
bool DB_UpdateRewardMedal::dispatch(int a, int b, Stream* stream)
{
    (void)a;
    (void)b;
    _SIG_UPDATE_REWARD_MEDAL* sig =
        stream->GetOutBuffer<_SIG_UPDATE_REWARD_MEDAL>();
    // ORIG: sig==0 → return 0 岛；else Update 后 return 1。
    // `return sig!=0` 会差成末段 setne 物化（P198②）。
    if (sig == 0)
        return 0;
    Update(*sig);
    return 1;
}

DB_UpdateRewardMedal::DB_UpdateRewardMedal() {}
DB_UpdateRewardMedal::~DB_UpdateRewardMedal() {}

// ============================================================================
// CActionPointManager
// ============================================================================

// Set —— ORIG T 0x08121b0c objdump：prev ← m_sig.m_apSum 后复制完整 packed 包。
void CActionPointManager::Set(_SIG_LOAD_ACTION_POINT& sig)
{
    m_prevApSum = m_sig.m_apSum;
    memcpy(&m_sig, &sig, sizeof(m_sig));
}

// GM_Reset —— 报告汇编+反编译全文转写。ORIG T 0x08121d12。
// [b28d-27] DIFF N82：ORIG 0x08121d12；p167 CLEAR（1/1），残差为循环体数组寻址
// [b28d-solo-APSystemOnlyOrig] 判词：**N18a 封顶**（唯一残差 = ORIG `mov %edx,%ecx; neg %ecx` vs 4.4 `neg %edx; mov %edx,%ecx` 1 对；p167 CLEAR 1/1；槽位 -0x14/-0x10/-0xc 与帧 0x28 全等）。
// （两步 add vs 4 步 imul 链）+ 帧差，语义闭合。
// [b28d-round1-L24] **DIFF→AE**（55=55）：唯一残差 `mov %edx,%ecx; neg %ecx`
// （ORIG 0x08121d9d/0x08121d9f）靠**提升到 unsigned int 再取负**复现 ——
// `(unsigned char)(-(unsigned int)(signed char)x)`（探针 /tmp/probe_neg.cpp f6
// 逐条同形；f1..f5 恒产 `neg %edx; mov %edx,%ecx`，int 中介亦无效）。
void CActionPointManager::GM_Reset()
{
    m_flag00 = 1;                             // *(this)=1
    memset(&m_sig.m_isRewardItem[0], 0, sizeof(m_sig.m_isRewardItem));   // ORIG 4 字节
    m_sig.m_isRewardMedal = 0;                // this+0x10 = sig+8
    m_sig.m_apSum = 0;                        // *(uint*)(this+0xc)
    m_prevApSum = 0;                          // *(uint*)(this+4)
    for (int row = 0; row <= 299; ++row)
    {
        // [b28d-solo-APSystemOnlyOrig] ORIG 0x08121d74：每外层轮把行基址
        // &m_sig.m_actionPoints[row]（=this+0x15+row*0xb）缓存进具名指针槽
        // -0x10，内层以 0x6(%edx,%eax,1) 访问；不缓存则 4.4 折成
        // 0x1b(%ecx,%eax,1) 全地址重算（多 6 条 + push esi/ebx 帧差）。
        _ActionPoint* point = &m_sig.m_actionPoints[row];
        for (int slot = 0; slot <= 4; ++slot)
        {
            if (static_cast<signed char>(point->m_days[slot]) > 0)
            {
                point->m_days[slot] = static_cast<unsigned char>(
                    -(unsigned int)static_cast<signed char>(point->m_days[slot]));
            }
        }
    }
}

// ClearActionOne —— 报告全文转写。ORIG T 0x08121f1c。
// [b28d-solo-APSystemOnlyOrig] ORIG 0x08121f1c：`GetActionPointEtcParameter(_IsGMOn)`
// （uchar 直传 bool 形参，ORIG 只 movzbl×2）->m_actionPointRefs 直读 +0x18，
// **非 const** find/end（_ZNSt3map…4findERS7_/3endEv）+ iterator→const_iterator
// 转换（C1ERKSt17_Rb_tree_iterator），end 侧表达式整段重写（P48 两次
// G_CDataManager/GetActionPointEtcParameter 调用）；旧版经 getActionPointRefs()
// [b28d-solo-APSystemOnlyOrig] 判词：**N18a 封顶**（唯一残差 = `mov;neg` 序 1 对；p167 CLEAR；非 const map find/end + `&action.m_actionNo` 直接取址 + 循环外缓存 size 还原后 145=145）。
// [b28d-round1-L24] 旧 N18a 判词作废：**DIFF→AE**（145=145）—— 同 GM_Reset，
// 取负写法提升到 `-(unsigned int)(signed char)x` 即复现 ORIG 0x0812207x
// `mov %edx,%ecx; neg %ecx`（探针 /tmp/probe_neg.cpp f6）。
// const getter ⇒ const 版 find/end（p167 MISSING 5）。
bool CActionPointManager::ClearActionOne(_ActionPoint& action, int itemIdx)
{
    if (itemIdx != 0)
    {
        std::map<unsigned short, const CActionPointEx*>::const_iterator it =
            G_CDataManager()
                ->GetActionPointEtcParameter(CActionPointManager::_IsGMOn)
                ->m_actionPointRefs.find(*reinterpret_cast<const unsigned short*>(&action.m_actionNo));
        if (it == G_CDataManager()
                      ->GetActionPointEtcParameter(CActionPointManager::_IsGMOn)
                      ->m_actionPointRefs.end())
        {
            return false;
        }
        const CActionPointEx* data = it->second;
        if (data == 0)
        {
            return false;
        }
        // [b28d-solo-APSystemOnlyOrig] ORIG 0x08122007：size() 循环外缓存一次
        // （-0x14），i 为 -0x10；逐次 `data->m_itemList[i]`（P157 不缓存引用）。
        bool found = false;
        std::size_t count = data->m_itemList.size();
        for (unsigned int i = 0; i < count; ++i)
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
    // [b28d-solo-APSystemOnlyOrig] ORIG 0x0812206e：**两个独立 if**（零槽即
    // return false，非 break），day 不落槽（P0 表达式直读）；累加用**写回后的
    // 成员**重读（movzbl 0x6(...)+movsbl），非局部副本。
    for (int slot = 0; slot <= 4; ++slot)
    {
        if (action.m_days[slot] == 0)
        {
            return false;
        }
        if (static_cast<signed char>(action.m_days[slot]) < 0)
        {
            // ORIG 先按字节取负，再以有符号 char 累加（含 0x80 情形）。
            m_prevApSum = m_sig.m_apSum;
            action.m_days[slot] = static_cast<unsigned char>(
                -(unsigned int)static_cast<signed char>(action.m_days[slot]));
            m_sig.m_apSum += static_cast<signed char>(action.m_days[slot]);
            m_flag00 = 1;
            return true;
        }
    }
    return false;
}

// ClearAction —— 报告全文转写。ORIG T 0x08121dcc。
// [b28d-27] DIFF N82：ORIG 0x08121dcc；p167 CLEAR（3/3），残差为帧/槽分配，
// [b28d-solo-APSystemOnlyOrig] 判词：**N18b/P265 封顶**（残差 = ①字段 vs enum 形参比较 4.4 恒载寄存器 1 条；②enum 实参 int→unsigned-enum 转换两段中转 ×2；p167 CLEAR 3/3；帧 0x34/槽 -0xd/-0xc/汇聚变量全等）。
// 语义闭合。
bool CActionPointManager::ClearAction(CUser& user, _ActionGroupIndex group,
                                      int itemIdx, ENUM_PACKETCLASS packetClass)
{
    // [b28d-solo-APSystemOnlyOrig] ORIG 0x08121dd3：bool 汇聚变量 -0xd（初值 0，
    // 成功置 1，尾部 movzbl 返回）+ 行号 -0xc；**无 groupOut 具名局部**（ORIG 每
    // 次调用点重算地址并直装 ecx）。早返 `return true` 会改块布局并多槽。
    bool result = false;
    for (int row = 0; row <= 299; ++row)
    {
        if (m_sig.m_actionPoints[row].m_groupIndex == 0)
        {
            break;
        }
        if (m_sig.m_actionPoints[row].m_groupIndex != (int)group)
        {
            continue;
        }
        if (ClearActionOne(m_sig.m_actionPoints[row], itemIdx))
        {
            result = true;
            CUserProc::SendActionClear(user, m_sig.m_actionPoints[row].m_actionNo,
                                       (_ActionGroupIndex)m_sig.m_actionPoints[row].m_groupIndex,
                                       true, packetClass);
            break;
        }
        if (packetClass == 1)
        {
            CUserProc::SendActionClear(user, m_sig.m_actionPoints[row].m_actionNo,
                                       (_ActionGroupIndex)m_sig.m_actionPoints[row].m_groupIndex,
                                       false,
                                       static_cast<ENUM_PACKETCLASS>(1));
        }
    }
    return result;
}

// SetTodayAction —— 报告汇编+反编译全文转写。ORIG T 0x0812161c。
// [b28d-solo-APSystemOnlyOrig] ORIG 0x0812161c（Ghidra C + objdump 逐段）：
// ①time_t 单局部 -0x4c（旧版 now/tt 双局部）；②`it != table.end()`（ne，非 eq）
// 且 end 走具名临时 -0x4c 槽；③`list` 为具名指针（ptEv 结果）；④candidates 为
// **空 vector + 独立建表循环 `push_back(&(*list)[pass])`（prvalue ⇒ OS3_ 右值重载）**
// ——旧版预尺寸 C1EjRKS3_RKS4_ 缺 C1Ev/push_back(&&)；
// ⑤过滤循环用 `candidates[i]=0` 标记 + goto next（每字段重调 operator[]，共 11 次）；
// ⑥group 判定链顺序 0x1d/0x1e/0x1f/**0x2d**/0x20（P46 case 序）；
// [b28d-solo-APSystemOnlyOrig] 判词：**gap 10→2**（本轮还原：标准 `for(;row<300 && it!=end; it++)` 循环旋转布局（4.1 init 后 jmp 底部条件）、P223 fA/fH goto 空标签岛补回 3 处 nop+jmp 岛、lo/hi 表达式直读；370 vs 368）。残差 = N18b（weekday 比较 2 条 mov 中转）+ N107（memcpy dest `add $0x15` vs `lea`）。p167 CLEAR 35/35。
// ⑦循环尾 `it++`（ppEi 后置，P28 增量侧）。
void CActionPointManager::SetTodayAction(int level, unsigned int characNo,
                                         ENUM_EXPERT_JOB_TYPE jobType,
                                         const std::map<unsigned short,
                                                        std::vector<CActionPointEx> >& table)
{
    Reset();
    m_sig.m_characNo = characNo;

    time_t now = GlobalData::s_systemTime_.getCurSec();
    struct tm tmc;
    localtime_r(&now, &tmc);
    int weekday = tmc.tm_wday;

    std::map<unsigned short, std::vector<CActionPointEx> >::const_iterator it =
        table.begin();
    int row = 0;
    // [b28d-solo-APSystemOnlyOrig] ORIG 0x0812167a..0x08121686：init 后
    // 无条件 `jmp` 到底部条件块（4.1 for 循环旋转布局）⇒ 源码必须写成
    // 标准 for(; cond; it++) 形（body 尾 ++row、cond 失败后 m_flag00=1），
    // 而非 `for(;;){ bool cond; ...; if(!cond) return; }`（4.4 条件置顶）。
    for (; row < 300 && it != table.end(); it++)
    {
        const std::vector<CActionPointEx>* list = &it->second;
        int total = static_cast<int>(list->size());
        // [b28d-solo-APSystemOnlyOrig] ORIG 0x081216a5..0x08121a8f：`je 0x475`
        // 落到 nop 岛、then 块尾 `jmp 0x476` 跳过岛（4.1 空 else 占位）⇒
        // P223 复现形 `if (total==0) goto after_body; { body } after_body:;`
        // （4.4 探针 /tmp/ap_probe.cpp fA 精确产 `je <nop>; body; jmp; nop`）。
        if (total == 0) goto after_body;
        {
            bool useWeekday = false;
            bool useLevelBand = false;
            if ((*list)[0].m_weekday != 7)
            {
                useWeekday = true;
            }
            if ((*list)[0].m_record.m_levelMin != 0)
            {
                useLevelBand = true;
            }
            std::vector<const CActionPointEx*> candidates;
            for (unsigned int pass = 0; static_cast<int>(pass) < total; ++pass)
            {
                candidates.push_back(&(*list)[pass]);
            }
            int remain = 0;
            for (unsigned int i = 0; static_cast<int>(i) < total; ++i)
            {
                // [b28d-solo-APSystemOnlyOrig] ORIG 0x08121772..0x0812198e：
                // 空 then 占位 nop 岛（`jne 0x371`→nop→`jmp 0x375`）⇒ P223
                // fH 形（探针 /tmp/ap_probe2.cpp fH 产 `je L; body; jmp L+1;
                // L: nop`）。
                if (candidates[i] == 0) goto next_candidate;
                {
                    if (useWeekday)
                    {
                        if (_gmWeekday.m_gmOn != 0)
                        {
                            weekday = _gmWeekday.m_weekday;
                        }
                        if (candidates[i]->m_weekday != weekday)
                        {
                            candidates[i] = 0;
                            goto next_candidate;
                        }
                    }
                    if (useLevelBand)
                    {
                        // [b28d-solo-APSystemOnlyOrig] ORIG 0x081217e0..0x0812181a：
                        // 两次独立 `(*list)[i]` 直读（P183 地址重物化），lo/hi
                        // **无栈槽**（P0）⇒ 表达式内联，勿声明具名局部。
                        if (level < static_cast<int>(
                                        (*list)[i].m_record.m_levelMin) ||
                            static_cast<int>(
                                (*list)[i].m_record.m_levelMax) < level)
                        {
                            candidates[i] = 0;
                            goto next_candidate;
                        }
                    }
                    {
                        int group = candidates[i]->m_record.m_groupIndex;    // +0x02
                        if (group == 0x1d || group == 0x1e || group == 0x1f ||
                            group == 0x2d || group == 0x20)
                        {
                            if (group == 0x1d && jobType != ENUM_EXPERT_JOB_TYPE_2)
                            {
                                candidates[i] = 0;
                            }
                            else if (group == 0x1e && jobType != ENUM_EXPERT_JOB_TYPE_4)
                            {
                                candidates[i] = 0;
                            }
                            else if (group == 0x1f && jobType != ENUM_EXPERT_JOB_TYPE_1)
                            {
                                candidates[i] = 0;
                            }
                            else if (group == 0x20 && jobType != ENUM_EXPERT_JOB_TYPE_3)
                            {
                                candidates[i] = 0;
                            }
                            else if (group == 0x2d && jobType != ENUM_EXPERT_JOB_TYPE_3)
                            {
                                candidates[i] = 0;
                            }
                            if (candidates[i] == 0)
                            {
                                goto next_candidate;
                            }
                        }
                        if (group == 0x22 && jobType != ENUM_EXPERT_JOB_TYPE_0)
                        {
                            candidates[i] = 0;
                        }
                        else
                        {
                            ++remain;
                        }
                    }
                }
            next_candidate:;
            }

            if (remain == 0)
            {
                --row;
            }
            else
            {
                int pick = DNFFLib::get_rand_int(remain);
                int seen = 0;
                const CActionPointEx* chosen = 0;
                for (unsigned int k = 0; static_cast<int>(k) < total; ++k)
                {
                    // [b28d-solo-APSystemOnlyOrig] ORIG 0x081219ec..0x08121a1e：
                    // 同 fH 形（`sete; jne <nop 岛>; then; jmp; ++seen`）。
                    if (candidates[k] == 0) goto next_k;
                    {
                        if (seen == pick)
                        {
                            chosen = candidates[k];
                            break;
                        }
                        else
                        {
                            ++seen;
                        }
                    }
                next_k:;
                }
                if (chosen == 0)
                {
                    --row;
                }
                else
                {
                    memcpy(&m_sig.m_actionPoints[row],
                           reinterpret_cast<const void*>(chosen),
                           sizeof(m_sig.m_actionPoints[row]));
                }
            }
        }
    after_body:;
        ++row;
    }
    m_flag00 = 1;
}

// GetTodayRewardAndSendMail —— 报告全文转写。ORIG T 0x08122104。
// [b28d-solo-APSystemOnlyOrig] ORIG 0x08122104：size() 循环外缓存（-0x1c），
// 逐字段 `todays[i].xxx` **每次重调** const operator[]（P157 禁引用缓存；旧版
// `const _TodayRewardItem& entry=todays[i]` 只 1 次调用 ⇒ p167 MISSING 3）；
// `entry` 是**循环外声明的具名指针**（-0x18 初值 0，命中即 break），循环后
// 虚调用槽核对（Main 2026-09-09 要求）：ORIG 0x08122263 `call *%edx` 槽 = vptr+0x8
// （`add $0x8,%eax`）；我方 0x203b 同槽 ⇒ make_item 目标一致（仅 %eax vs %edx = N7）。
// [b28d-solo-APSystemOnlyOrig] 判词：**N82 + 档②封顶**（残差 = 槽基浅 4 + N93附 两处 `je` vs 4.4 `jne+mov$0+jmp` + N7 虚调目标寄存器 + P75 实参装载序 + P254 字段中转；m_isRewardItem uchar `cmp $1` vs ORIG bool `xor $1`（档②候选：探针 /tmp/ap_probe6.cpp gS3 显示需 bool 字段 + `if(!f)` 才出 xor，共享头改动未落）；p167 CLEAR 18/18）。
// `if(entry==0)return false`（死检查照抄 P171），`m_flag00=1` 在该检查**之后**。
bool CActionPointManager::GetTodayRewardAndSendMail(
    CUser& user, _SIG_LOAD_ACTION_POINT::_TodayRewardItem reward)
{
    const std::vector<APSystem::_TodayRewardItem>& todays =
        G_CDataManager()->GetActionPointEtcParameter(CActionPointManager::_IsGMOn != 0)->m_todayRewards;

    std::size_t count = todays.size();
    const APSystem::_TodayRewardItem* entry = 0;
    // [b28d-solo-APSystemOnlyOrig] P212：for-init 的 i 提至函数头（全头部声明），
    // 4.4 才按「先声明深」分配（-0x14，ORIG 形）；块内声明会占 -0xc。
    unsigned int i;
    for (i = 0; i < count; ++i)
    {
        // [b28d-solo-APSystemOnlyOrig] ORIG 0x08122168..0x081221be：三子句
        // `&&` 短路（jne/jb/ja 同落 mov$0 岛 + test;je，P214① 正极性形）；
        // 拆成 if/continue 会产 setne 物化 + 比较序反转（每轮 +3 条）。
        if (reward.m_itemIdx == todays[i].m_itemIdx &&
            !(m_sig.m_apSum < todays[i].m_minAp) &&
            !(m_sig.m_apSum > todays[i].m_maxAp))
        {
            if (m_sig.m_isRewardItem[reward.m_itemIdx] == 1)  // this+0x11+idx
            {
                continue;
            }
            m_sig.m_isRewardItem[reward.m_itemIdx] = 1;
            entry = &todays[i];
            break;
        }
    }
    if (entry == 0)
    {
        return false;
    }
    m_flag00 = 1;   // LAB 入口 *this = 1（find_item 之前）

    CItem* itemDef = G_CDataManager()->find_item(entry->m_rewardItemIdx);
    Inven_Item item;
    if (itemDef == 0)
    {
        return false;
    }
    itemDef->make_item(item);
    // ORIG：count ushort 先零扩展，再完整写入 packed Inven_Item +0x07。
    item.m_addInfo2 = static_cast<int>(entry->m_count);
    item.m_addInfo = entry->m_rewardItemIdx;

    const char* title =
        g_scriptStringManager_.findString(4, "chn_game_server_msg_82", 0);
    char titleBuf[30];
    strncpy(titleBuf, title, 0x1d);
    const char* msg =
        g_scriptStringManager_.findString(4, "chn_game_server_msg_83", 0);
    char msgBuf[256];
    strncpy(msgBuf, msg, 0xff);

    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        titleBuf, item, 0, user.get_charac_no(-1), msgBuf, strlen(msgBuf),
        8, user.GetServerGroup(), false, false);
    return true;
}

// ============================================================================
// Inter_LoadRewardMedal::SendMail —— 报告全文转写。ORIG T 0x08124ea0。
// 声明位于 InterDispatcher.h（类内 public）。
// ============================================================================
// [b28d-solo-APSystemOnlyOrig] ORIG 0x08124ea0：size() 缓存 -0x1c、hit -0x18
// （初值 0）、i -0x14；两个条件各逐字段重调 const operator[]（P157；旧版
// 虚调用槽核对（Main 2026-09-09 要求）：ORIG 0x08125015 `call *%edx` 槽 = vptr+0x8
// （`add $0x8,%eax`）；我方 0x22df 同槽 ⇒ make_item 目标一致（仅 %eax vs %edx = N7）。
// [b28d-solo-APSystemOnlyOrig] 判词：**N82 封顶**（残差 = 槽基浅 4 + N93附 两处 return 块序 + N7 虚调目标寄存器 + P254 字段 eax→ebx 中转 + P75 实参装载序（`movl $0x8,S` 提前）；171 vs 175）。p167 CLEAR 20/20。
// `const _MedalRewardItem& m = medals[i]` 只 1 次 ⇒ p167 MISSING 5）。
bool Inter_LoadRewardMedal::SendMail(CUser& user, unsigned int medalCount)
{
    const std::vector<_MedalRewardItem>& medals =
        G_CDataManager()->GetActionPointEtcParameter(CActionPointManager::_IsGMOn != 0)->m_medalRewards;

    std::size_t count = medals.size();
    const _MedalRewardItem* hit = 0;
    // [b28d-solo-APSystemOnlyOrig] P212：同 GetTodayRewardAndSendMail ——
    // i 提至函数头，4.4 按「先声明深」落在 -0x14（ORIG 形）。
    unsigned int i;
    for (i = 0; i < count; ++i)
    {
        if (!(medalCount < medals[i].m_field00) && !(medals[i].m_field02 < medalCount))
        {
            hit = &medals[i];
            break;
        }
        if (!(medalCount < medals[i].m_field00) && medals[i].m_field00 == 100)
        {
            hit = &medals[i];
            break;
        }
    }
    if (hit == 0)
    {
        return false;
    }

    CItem* itemDef = G_CDataManager()->find_item(hit->m_field04);
    Inven_Item item;
    if (itemDef == 0)
    {
        return false;
    }
    itemDef->make_item(item);
    // ORIG：count ushort 先零扩展，再完整写入 packed Inven_Item +0x07。
    item.m_addInfo2 = static_cast<int>(hit->m_field08);
    item.m_addInfo = hit->m_field04;

    const char* title =
        g_scriptStringManager_.findString(4, "chn_game_server_msg_82", 0);
    char titleBuf[30];
    strncpy(titleBuf, title, 0x1d);
    const char* msg =
        g_scriptStringManager_.findString(4, "chn_game_server_msg_83", 0);
    char msgBuf[256];
    strncpy(msgBuf, msg, 0xff);

    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        titleBuf, item, 0, user.get_charac_no(-1), msgBuf, strlen(msgBuf), 8,
        user.GetServerGroup(), false, false);
    return true;
}

}  // namespace APSystem
