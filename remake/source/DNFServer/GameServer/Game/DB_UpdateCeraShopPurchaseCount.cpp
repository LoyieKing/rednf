// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DB_UpdateCeraShopPurchaseCount/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "GlobalData.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "CStreamGuard.h"
#include "SigTypes.h"  // SIG_CERASHOP_PURCHASE_COUNT（0x10 POD，SigTypes_A.h）
#include "GameTypes.h"  // cMyTrace
#include "DBMgr.h"
#include "MySQL.h"
#include "SkillSlot.h"  // NumberToString
#include "DBDispatcher.h"

#include <string.h>

class DB_UpdateCeraShopPurchaseCount : public DBDispatcher
{
public:
    DB_UpdateCeraShopPurchaseCount() {}  // ORIG W C1 0x844fbe6（0x4 基类 ctor 形态）
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
    // [Q3PrettSweepA 2026-09-11] 拼写对齐 ORIG prett（同底层，mangled 不变）：
    // `static void DB_UpdateCeraShopPurchaseCount::makeRequest(memberIdentificationNumber_t,
    // itemIndexOnlyServer_t, int, int)` size 0x77；另三件同理（memberIdentificationNumber_t）。
    static void makeRequest(memberIdentificationNumber_t accId, itemIndexOnlyServer_t itemUid,
                            int count, int param4);

private:
    bool _selectPurchaseCount(memberIdentificationNumber_t mId, unsigned short& out);
    void _updatePurchaseCount(memberIdentificationNumber_t mId, unsigned short count);
    void _insertPurchaseCount(memberIdentificationNumber_t mId);
};

// b28d-21（D21）：SIG_CERASHOP_PURCHASE_COUNT 已按 ORIG 逐字段具名化
//（SigTypes_A.h，sizeof 0x10 不变），原 CeraShopCntView 视图与裸偏移全部删除。

#include "DB_UpdateCeraShopPurchaseCount/dispatch.inc"


#include "DB_UpdateCeraShopPurchaseCount/_selectPurchaseCount.inc"


#include "DB_UpdateCeraShopPurchaseCount/_updatePurchaseCount.inc"


#include "DB_UpdateCeraShopPurchaseCount/_insertPurchaseCount.inc"


#include "DB_UpdateCeraShopPurchaseCount/makeRequest.inc"


// ============================================================================
// DB_SavePuUserCheck::makeRequest —— ORIG T 0x0844c098
// _ZN18DB_SavePuUserCheck11makeRequestEj（LinkE2.JpnExImpl 批次追加：
// ItemVendingMachine::BillingRecord 调用链 producer）。
// 逐指令转写：Acquire("DBThread.cpp", 0xbcf4) → guard(s,true) → << 0x2e2 →
//   << -1 → << accId → MsgQueueMgr::put(QUEUE_IDX 2, guard)。
// ============================================================================
class DB_SavePuUserCheck
{
public:
    DB_SavePuUserCheck() {}  // ORIG W C1 0x844fc1e（0x4 基类 ctor 形态）
    static void makeRequest(unsigned int accId);
};

#include "DB_UpdateCeraShopPurchaseCount/makeRequest_1.inc"

