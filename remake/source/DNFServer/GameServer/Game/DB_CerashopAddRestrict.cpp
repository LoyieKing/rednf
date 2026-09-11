#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DB_CerashopAddRestrict/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
#include "DB_CerashopAddRestrict/makeRequest.inc"


#include "DB_CerashopAddRestrict/makeRequest_1.inc"


#include "DB_CerashopAddRestrict/dispatch.inc"


#include "DB_CerashopAddRestrict/dispatch_1.inc"
