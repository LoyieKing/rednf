// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 DB_UpdateCeraShopPurchaseCount/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— DB_UpdateCeraShopPurchaseCount（塞拉商城购买计数 DB 请求）。
// 归属批次：OrigCeraCount（仅 ORIG 4 符号：dispatch/_select/_update/_insert；
// makeRequest 为 LinkE2.JpnExImpl 批次既有实现）。
// ORIG 符号（df_game_r 80）：
//   T 0x0844b0cc _ZN30DB_UpdateCeraShopPurchaseCount8dispatchEiiP6Stream
//   T 0x0844b450 _ZN30DB_UpdateCeraShopPurchaseCount20_selectPurchaseCountEjRt
//   T 0x0844b680 _ZN30DB_UpdateCeraShopPurchaseCount20_updatePurchaseCountEjt
//   T 0x0844b808 _ZN30DB_UpdateCeraShopPurchaseCount20_insertPurchaseCountEj
//   T 0x0844b31c _ZN30DB_UpdateCeraShopPurchaseCount11makeRequestEjmii（既有）
// 语义：dispatch 读流 SIG（m_id + purchase_cnt）→ select 计数 → 无则 insert(1) /
// 有则 update(+1) → 组 0x2d9 回发 DISPATCH_Q。DB 异常统一 throw const char*。
// vtable @0x8c5e020 = [offtop][typeinfo][dispatch][~D1][~D0]。
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
    static void makeRequest(unsigned int accId, unsigned long itemUid,
                            int count, int param4);

private:
    bool _selectPurchaseCount(unsigned int mId, unsigned short& out);
    void _updatePurchaseCount(unsigned int mId, unsigned short count);
    void _insertPurchaseCount(unsigned int mId);
};

// b28d-21（D21）：SIG_CERASHOP_PURCHASE_COUNT 已按 ORIG 逐字段具名化
//（SigTypes_A.h，sizeof 0x10 不变），原 CeraShopCntView 视图与裸偏移全部删除。

// ============================================================================
// b21 NEAR：逐指令语义与 ORIG 0x0844b0cc 全等；残余差异仅
//   ① m_cnt 自增 N-86b 同算术多 codegen（lea $0x1 vs add $0x1，结果全同）
//   ② cMyTrace/sig 局部栈槽偏移（lea -0x28 vs -0x40 帧布局产品）
//   ③ 数据地址/call 目标【链接期解析，AE 归一】。语义完成，byte 层 N 封顶。
//   【b28d-solo 复核】① 已对齐（lea 0x1(%eax),%edx 逐条同形）；② 已由
//   cMyTrace 临时链翻正（见下方 b28d-solo 判词）；③ 仅余 AE 归一项。
// DB_UpdateCeraShopPurchaseCount::dispatch @ 0x0844b0cc
// 逐指令转写：GetOutBuffer<SIG_CERASHOP_PURCHASE_COUNT> → null 则 trace(0xbbac)
//   "ptrData is null." 返 false；_selectPurchaseCount(m_id, &purchase_cnt) 失败
//   trace(0xbbb2) "select query fail." 返 false；purchase_cnt==0 →
//   cnt=1 + _insertPurchaseCount(m_id)，否则 cnt+1 + _updatePurchaseCount(m_id,cnt)；
//   共用出口：Acquire("DBThread.cpp",0xbbc1) → CStreamGuard(s,true) → *guard<<0x2d9
//   → *guard<<slot → guard->GetInBuffer<SIG>()（null 则 trace(0xbbc9) 返 false）
//   → 16B 逐字拷贝 → MsgQueueMgr::put(DISPATCH_Q=1, guard) → 返 true。
// ============================================================================
// b28d-solo-DB_UpdateCeraShopPurchaseCount: AE（NEAR→AE，ndiff 13→0）—
//   修法：3 个 cMyTrace 具名对象改全表达式临时链
//   `cMyTrace(f,l,v)("...")`（P221/P37）⇒ 4.4 互斥/嵌套块对象槽序翻正为
//   ORIG 深→浅（trace -0x40/-0x30/-0x20、guard -0x48），槽号逐条一致。
//   154=154 insns，p167 CLEAR(22=22)，compare_tu clean=ae（残差仅 .o 重定位
//   数据地址/call 目标，P264 口径）。ORIG 0x0844b0cc。
#include "DB_UpdateCeraShopPurchaseCount/dispatch.inc"
}

// ============================================================================
// DB_UpdateCeraShopPurchaseCount::_selectPurchaseCount @ 0x0844b450
// 逐指令转写：GetDBHandle(9,0)（null→throw "handle_null"）→
//   set_query(" seLect purchase_cnt from event_purchase_cnt where m_id = %s ",
//   NumberToString(mId,0))（失败 throw "set_query"）→ exec(true)（失败 throw
//   "exec"）→ get_n_rows()==1 时 fetch()（失败 throw "fetch"）+
//   get_ushort(0,out)（失败 throw "get_ushort"）；否则 out=0；返 true。
//   catch(const char* e) → trace(0xbc0b) "select purchase count (error:%s)" → false。
// ============================================================================
#include "DB_UpdateCeraShopPurchaseCount/_selectPurchaseCount.inc"
}

// ============================================================================
// DB_UpdateCeraShopPurchaseCount::_updatePurchaseCount @ 0x0844b680
// 逐指令转写：GetDBHandle(9,0)（null→throw "handle_null"）→ set_query(
//   " upDate event_purchase_cnt set purchase_cnt=%d where m_id = %s ",
//   count, NumberToString(mId,0))（失败 throw "set_query"）→ exec(true)
//   （失败 throw "exec"）。catch(const char* e) → trace(0xbc22)
//   "update purchase count (error:%s)"。void 返回。
// ============================================================================
#include "DB_UpdateCeraShopPurchaseCount/_updatePurchaseCount.inc"
}

// ============================================================================
// DB_UpdateCeraShopPurchaseCount::_insertPurchaseCount @ 0x0844b808
// 逐指令转写：GetDBHandle(9,0)（null→throw "handle_null"）→ set_query(
//   " inSert into event_purchase_cnt(m_id, purchase_cnt) values(%s, 1) ",
//   NumberToString(mId,0))（失败 throw "set_query"）→ exec(true)
//   （失败 throw "exec"）。catch(const char* e) → trace(0xbc39)
//   "update purchase count (error:%s)"。void 返回。
// ============================================================================
#include "DB_UpdateCeraShopPurchaseCount/_insertPurchaseCount.inc"
}

// ============================================================================
// DB_UpdateCeraShopPurchaseCount::makeRequest @ 0x0844b31c
// 逐指令转写：Acquire("DBThread.cpp",0xbbd6) → CStreamGuard(s,true) →
//   *guard<<0x2d9 → *guard<<param4 → guard->GetInBuffer<SIG>()（null 则
//   trace(0xbbde) "ptrData is null" 返）→ sig+0xc=count；+0=accId；+4(8B)=itemUid；
//   +8(2B)=0 → MsgQueueMgr::put(DB_Q=2, guard)。
// ============================================================================
// b28d-solo-DB_UpdateCeraShopPurchaseCount: AE（NEAR→AE，ndiff 9→0）—
//   修法：if 块内 cMyTrace 具名对象改全表达式临时链
//   `cMyTrace(f,l,v)("...")`（P221/P37）⇒ trace 槽 -0x1c（ORIG）对齐，
//   guard 保持 -0x24；槽号与 ORIG 逐条一致。82=82 insns，p167 CLEAR(14=14)，
//   compare_tu clean=ae（残差仅 .o 重定位数据地址/call 目标，P264 口径）。
//   ORIG 0x0844b31c。
#include "DB_UpdateCeraShopPurchaseCount/makeRequest.inc"
}

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
}
