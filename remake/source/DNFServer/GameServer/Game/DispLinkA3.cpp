#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DispLinkA3/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "Dispatchers.h"

#include "CUser.h"            // get_state / getSecurityCard / getPad / getWebAddress /
                              //   get_acc_id；CUserCharacInfo 继承链
#include "CUserCharacInfo.h"  // getCurCharacInvenR / getCurCharacInvenW / getCurCharacNo
#include "CInventory.h"       // GetInvenTypeFromItemSpace / GetInvenRef / GetCreatureMgrW；
                              //   user_creature::CCreatureMgr::{GetCreatureIndex,ItemEvolute}
#include "CDataManager.h"     // G_CDataManager / CDataManager::find_item
#include "CItem.h"            // CItem::is_stackable
#include "CStackableItem.h"   // CStackableItem::m_int360（+0x360 基怪 id）
#include "GameTypes.h"        // cMyTrace
#include "cHistoryTrace.h"    // cHistoryTrace

// ---- DB 取消计数请求类（ORIG 符号 _ZN25DB_PassPadUpdateCancelCnt11makeRequestEjjPKc /
//      _ZN30DB_SecurityCardUpdateCancelCnt11makeRequestEjj；定义在 DB_AccountCargoSync.cpp
//      @0x0842403a / 0x0842c112）。注：DB_AccountCargoSync.h 中当前声明为非 static 成员，
//      而 ORIG 调用方（本函数与 CUser.cpp:7187/7192）均为静态调用形态；符号一致可链接，
//      头文件声明需后续统一为 static（已报告，不在本 TU 修改）。
class DB_SecurityCardUpdateCancelCnt
{
public:
    static void makeRequest(unsigned int accId, unsigned int cancelCnt);
};

class DB_PassPadUpdateCancelCnt
{
public:
    // ORIG T 0x0842403a（mangled EjjPc 非 const）。
    static void makeRequest(unsigned int accId, unsigned int cancelCnt,
                            char* webAddress);
};

#include "DispLinkA3/check_error.inc"


#include "DispLinkA3/check_error_1.inc"


#include "DispLinkA3/process.inc"


#include "DispLinkA3/process_1.inc"


#include "DispLinkA3/Dispatcher_SDC_ActiveStatus_Damage_Check.inc"




#include "DispLinkA3/check_error_2.inc"

