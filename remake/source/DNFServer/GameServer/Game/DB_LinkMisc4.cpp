// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DB_LinkMisc4/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— LinkMisc4 批次 DB 请求 TU（4 个 makeRequest）。
// 逐函数对照 docs/class_func_reports/{DB_EventGrowthWeaponReset,
// DB_EventUpdateGrowthWeaponTimepiece,DB_ReqChangeCharacName,
// DB_TestTradeRestrict}/makeRequest.md。语义：向 DB 队列（QUEUE_IDX = 2 = DB_Q）
// 发一条 CStreamGuard 请求；表头值、字段写入顺序、Acquire 行号常量均取自报告。
// 声明唯一来源为各自同名头文件。
#include <cstring>

#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "CEnvironment.h"
#include "SigTypes.h"
#include "DB_EventGrowthWeaponReset.h"
#include "DB_EventUpdateGrowthWeaponTimepiece.h"
#include "DB_ReqChangeCharacName.h"
#include "DB_TestTradeRestrict.h"
#include "DBDispatcher.h"
#include "DBMgr.h"
#include "MySQL.h"
#include <map>

#include "DB_LinkMisc4/makeRequest.inc"


#include "DB_LinkMisc4/makeRequest_1.inc"


#include "DB_LinkMisc4/makeRequest_2.inc"


#include "DB_LinkMisc4/makeRequest_3.inc"


#include "DB_LinkMisc4/dispatch.inc"


#include "DB_LinkMisc4/dispatch_1.inc"


#include "DB_LinkMisc4/dispatch_2.inc"


#include "DB_LinkMisc4/dispatch_3.inc"
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return false;

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    SIG_CHANGE_CHARAC_NAME* sig = stream->GetOutBuffer<SIG_CHANGE_CHARAC_NAME>();

    char name_buf[60] = { '\0' };   // 转义缓冲区（ORIG rep stos 清零 60 字节）
    db->escape_string(name_buf, sig->m_name);

    // env+0x68 = m_dbConnInfo：ORIG 直访成员（经 G_CEnvironment()+0x68 做
    // map::operator[] this，无 accessor call），故禁 get_db_conn_info() accessor。
    typedef std::map<ENUM_DB_HANDLE_IDX, STDBConnInfo> DbConnMap;
    ENUM_DB_HANDLE_IDX connIdx = DB_HANDLE_2;   // ORIG 栈上局部初始值 2
    int ret = _hasSameCharacName(
        db,
        G_CEnvironment()->m_dbConnInfo[connIdx].m_db,
        name_buf);
    if (ret != 0)
    {
        _sendResult(sig, slot, ret);
        return true;
    }

    db->set_query("upDate charac_info set charac_name='%s' where charac_no=%d and delete_flag=0",
                  name_buf, sig->m_characNo);
    if (!(db->exec(true) && db->getAffectedRowCount() != 0))
    {
        _sendResult(sig, slot, 2);
        return true;
    }

    MySQL* guildDb = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_8, E_SERVER_GROUP_NONE);
    // ORIG movzbl 0x2c(%eax)（8 位读）；m_arg2c 为 uint 成员，用 P82 尾部视图折叠为单 disp。
    struct ChangeCharacNameByteView
    {
        char m_pad[0x2c];
        unsigned char m_serverId;
    };
    guildDb->set_query("seLect guild_id from guild_member where charac_no=%d and member_flag=1 "
                       "and server_id=%d",
                       sig->m_characNo,
                       reinterpret_cast<ChangeCharacNameByteView*>(sig)->m_serverId);
    sig->m_arg28 = 0;
    if (!(guildDb->exec(true) && guildDb->fetch()))
        sig->m_arg28 = 0;
    if (guildDb->get_n_rows() == 0)
        sig->m_arg28 = 0;
    else if (!guildDb->get_uint(0, sig->m_arg28))
        sig->m_arg28 = 0;
    _sendResult(sig, slot, 0);
    return true;
}

#include "DB_LinkMisc4/_hasSameCharacName.inc"


#include "DB_LinkMisc4/_sendResult.inc"

