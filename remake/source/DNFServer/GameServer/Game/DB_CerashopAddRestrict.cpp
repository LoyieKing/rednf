// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 DB_CerashopAddRestrict/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
//   docs/class_func_reports/DB_LoadCerashopAddRestrictType/{makeRequest,dispatch}.md
//   docs/class_func_reports/DB_SaveCerashopAddRestrictType/{makeRequest,dispatch}.md
// 语义：makeRequest 向 DB 队列发商城限购请求（包体 CStreamGuard）；
// dispatch 在 DBThread 侧读流执行 SQL（LOAD 组包回投 DISPATCH_Q）。
// 两类继承 DBDispatcher（ORIG vtable 0x8b2db30/0x8b2db48，虚槽 2 = dispatch）。
// ============================================================================
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "SigTypes.h"
#include "CUser.h"
#include "DBDispatcher.h"
#include "DBMgr.h"
#include "MySQL.h"
#include "Stream.h"
#include "CEnvironment.h"   // ENUM_DB_HANDLE_IDX / ENUM_SERVER_GROUP

// ORIG T @ 0x080e1ff6（mangled _ZN30DB_LoadCerashopAddRestrictType11makeRequestEP5CUser）
// dispatch @ 0x080e1aca（_ZN30DB_LoadCerashopAddRestrictType8dispatchEiiP6Stream）
class DB_LoadCerashopAddRestrictType : public DBDispatcher
{
public:
    DB_LoadCerashopAddRestrictType() {}   // ORIG C1 0x844ff82（sz 0x4）
    virtual bool dispatch(int a, int b, Stream* stream);   // ORIG T 0x080e1aca
    static void makeRequest(CUser* user);
};

// ORIG T @ 0x080e17ba
//（mangled _ZN30DB_SaveCerashopAddRestrictType11makeRequestEP5CUserRN19CerashopAddRestrict11DBInputDataE）
// dispatch @ 0x080e14f6（_ZN30DB_SaveCerashopAddRestrictType8dispatchEiiP6Stream）
class DB_SaveCerashopAddRestrictType : public DBDispatcher
{
public:
    DB_SaveCerashopAddRestrictType() {}   // ORIG C1 0x844ff66（sz 0x4）
    virtual bool dispatch(int a, int b, Stream* stream);   // ORIG T 0x080e14f6
    static void makeRequest(CUser* user, CerashopAddRestrict::DBInputData& data);
};
// ============================================================================
// DB_LoadCerashopAddRestrictType::makeRequest @ 0x080e1ff6
// 包体：0x341 | GetUID() | get_acc_id() | getCurCharacNo()
// ============================================================================
#include "DB_CerashopAddRestrict/makeRequest.inc"
}

// ============================================================================
// DB_SaveCerashopAddRestrictType::makeRequest @ 0x080e17ba
// 按 data.m_pInfo->m_type（op）分派：
//   op==0（登记）：包体 0x340 | -1 | m_type | m_ipgNo | head[+4] | head[+8](long)
//                  | m_pInfo[+0x10](long) | get_acc_id()
//   op==1（删除）：同上，末字段为 getCurCharacNo()
// 注：ORIG DBInputData 的第 0 个字段（slot +0x00，DBInputData::m_result）实际存
// head 记录（paramDaily*）指针。m_result 建模为 int，本 TU 将其 reinterpret_cast 为
// paramDaily* 解释 m_count(+4)/m_nextDate(+8)，保持 ABI 与 ORIG 字节布局一致。
// ============================================================================
#include "DB_CerashopAddRestrict/makeRequest_1.inc"
}

// ============================================================================
// DB_SaveCerashopAddRestrictType::dispatch @ 0x080e14f6（报告 dispatch.md 全文转写）
// 读流：type | id | ipgNo | nextDate(long) | endDate(long)；
// type==0（账号）：再读 m_id；UPDATE，affected==0 时 INSERT。
// type==1（角色）：再读 characNo；同构。失败返回 false，成功 true。
// ============================================================================
#include "DB_CerashopAddRestrict/dispatch.inc"
}

// ============================================================================
// DB_LoadCerashopAddRestrictType::dispatch @ 0x080e1aca（报告 dispatch.md 全文转写）
// 读流：m_id | characNo；账号表（handle 1）查询 limit 10，n_rows>0 时组包
// 0x341 | param2 | SIG（count + type=0 + 每行 3 uint 自 +8 步长 0xc）投 DISPATCH_Q；
// 角色表（handle 2）同构（type=1，行号 0x1da）。失败 false，成功 true。
// ============================================================================
#include "DB_CerashopAddRestrict/dispatch_1.inc"
}