// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 APSystemOnlyOrig/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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

#include "APSystemOnlyOrig/getDailyInitializeTime.inc"
#include "APSystemOnlyOrig/__GetCurrentDNFDate.inc"

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

#include "APSystemOnlyOrig/uncompress_zip.inc"


#include "APSystemOnlyOrig/compress_zip.inc"


namespace APSystem
{

// ============================================================================
// POD 生命周期（objdump 转写）
// ============================================================================

#include "APSystemOnlyOrig/_MedalRewardItem.inc"


#include "APSystemOnlyOrig/_TodayRewardItem.inc"

// [Q8 2026-09-11 P342] `dtor__TodayRewardItem.inc` 已删（用户空析构删除；
// 隐式析构由 ScriptLeafA2 的 odr-use 链发射，nm 实测 D1/D2 仍在）。

#include "APSystemOnlyOrig/_SIG_LOAD_ACTION_POINT.inc"


// ============================================================================
// DB_LoadActionPoint
// ============================================================================

#include "APSystemOnlyOrig/dispatch.inc"


#include "APSystemOnlyOrig/ParseActionClearState.inc"


#include "APSystemOnlyOrig/Select.inc"
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
    // [round5-C05 2026-09-10] ORIG 0x08123c8f `cmpl $0x1,-0x1c; jne 08123f49`
    // ⇒ nRows 的失败出口是**函数尾部独立块** `mov $0,%eax`+（落 leave），而
    // 其余早返都是块内 `mov $0,%eax; jmp <epilogue>`。探针 /tmp/tail_probe.cpp
    // fA/fB 实证：平铺 `if(nRows!=1)return false;` 恒产 `je <cont>; mov $0;
    // jmp`（旧 DIFF 根因），把后续主体嵌套进 `if (nRows == 1) { …return true; }`
    // 并在其后 `return false;` 才产 ORIG 的 `jne <尾部 fail 块>` 形。
    if (nRows == 1)
    {
        if (!db->fetch())
        {
            return false;
        }

        // [b28d-solo-APSystemOnlyOrig] ORIG 0x08123cf0..0x08123e37：列号是具名局部
        // -0x18（初值 0，`mov S,%eax; addl $0x1,S` 后置自增），5 次 get_ubyte 复用
        // **同一 flag 槽** -0x39（仅首次 `movb $0x0` 初始化），apSum 直接传
        // `&sig.m_apSum`（ORIG `lea 0x4(%eax),%edx`，无 apSumTmp 临时）。
        // [round5-C05 2026-09-10] T1–T4 对 col 无直证（仅作实参，无符号性消费指令）
        // ⇒ 按 S1 一次性旋钮试 int：ORIG 0x08123e69 段为「先载 blobLen→%eax→
        // 立即 store 0xc(%esp)，再载 col」，我方 unsigned 形 4.4 预计算
        // （`mov blobLen,%edx; mov col,%eax` 后才 store）⇒ 探针 /tmp/arg_probe.cpp
        // t1(unsigned)/t3(int) 逐条实证该差异由实参类型决定；int 形与 ORIG 全等。
        int col = 0;
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
        // test; je ok; mov $0,%eax; jmp ret; ok: mov $1,%eax; jmp ret; fail: mov
        // $0,%eax`（ok 路径的 jmp 跳过尾部 fail 块）。
        if (!ParseActionClearState(g_szStrAP_CLEAR_STATE, sig.m_actionPoints, 300))
        {
            return false;
        }
        return true;
    }
    return false;
}

#include "APSystemOnlyOrig/SendResult.inc"


#include "APSystemOnlyOrig/DB_LoadActionPoint.inc"
#include "APSystemOnlyOrig/dtor_DB_LoadActionPoint.inc"

// ============================================================================
// DB_UpdateActionPoint
// ============================================================================

#include "APSystemOnlyOrig/MakeActionClearStateString.inc"


#include "APSystemOnlyOrig/Update.inc"
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

#include "APSystemOnlyOrig/dispatch_1.inc"


#include "APSystemOnlyOrig/DB_UpdateActionPoint.inc"
#include "APSystemOnlyOrig/dtor_DB_UpdateActionPoint.inc"

// ============================================================================
// DB_LoadRewardMedal
// ============================================================================

#include "APSystemOnlyOrig/makeRequest.inc"


#include "APSystemOnlyOrig/dispatch_2.inc"


#include "APSystemOnlyOrig/Select_1.inc"
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
    // [round5-C05 2026-09-10] S1 一次性旋钮：ORIG 0x08124a5a `cmp -0x14(%ebp),%eax;
    // setl` 为**内存直比**（无转换中转）⇒ RHS 是普通 int 局部（T1–T4 对 minAp
    // 无直证：仅赋值/比较/实参）。unsigned 形 4.4 多一条 `mov -0x14(%ebp),%eax`
    // 并把字段值挪到 %edx（167 vs 166 唯一残差）。
    int minAp = 99999;
    for (unsigned int i = 0; i < medalCount; ++i)
    {
        if (static_cast<int>(medals[i].m_field00) < minAp)
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
    // [round5-C05 2026-09-10] ORIG 0x08124aff `cmpl $0x0,-0x10; jle 08124b7f`
    // ⇒ nRows 失败出口 = 函数尾部独立块（+0x26f `mov $0,%eax` 落 leave），
    // 同 DB_LoadActionPoint::Select（探针 /tmp/tail_probe.cpp fA/fB）。
    if (nRows >= 1)
    {
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
    return false;
}

#include "APSystemOnlyOrig/SendResult_1.inc"


#include "APSystemOnlyOrig/DB_LoadRewardMedal.inc"
#include "APSystemOnlyOrig/dtor_DB_LoadRewardMedal.inc"

// ============================================================================
// DB_UpdateRewardMedal
// ============================================================================

#include "APSystemOnlyOrig/makeRequest_1.inc"


#include "APSystemOnlyOrig/Update_1.inc"


#include "APSystemOnlyOrig/dispatch_3.inc"


#include "APSystemOnlyOrig/DB_UpdateRewardMedal.inc"
#include "APSystemOnlyOrig/dtor_DB_UpdateRewardMedal.inc"

// ============================================================================
// CActionPointManager
// ============================================================================

#include "APSystemOnlyOrig/Set.inc"


#include "APSystemOnlyOrig/GM_Reset.inc"


#include "APSystemOnlyOrig/ClearActionOne.inc"


#include "APSystemOnlyOrig/ClearAction.inc"


#include "APSystemOnlyOrig/SetTodayAction.inc"
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
            // [FragB·PoolI-sync] CActionPointEx 头改造：m_record 已移除（真实 _ActionPoint
            //   m_ap@+0x00 + 具名派生 m_levelMin@+0x0f/m_levelMax@+0x10/m_groupIndex@+0x02）。
            if ((*list)[0].m_levelMin != 0)
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
                        // [FragB·PoolI-sync] 同步新头成员名（原 m_record.m_levelMin/Max）。
                        if (level < static_cast<int>(
                                        (*list)[i].m_levelMin) ||
                            static_cast<int>(
                                (*list)[i].m_levelMax) < level)
                        {
                            candidates[i] = 0;
                            goto next_candidate;
                        }
                    }
                    {
                        int group = candidates[i]->m_ap.m_groupIndex;    // +0x02（_ActionPoint 内）
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

#include "APSystemOnlyOrig/GetTodayRewardAndSendMail.inc"


#include "APSystemOnlyOrig/SendMail.inc"


}  // namespace APSystem
