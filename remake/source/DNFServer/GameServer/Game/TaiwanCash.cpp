// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 TaiwanCash/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— Taiwan 计费子系统实现（声明见 TaiwanCash.h）。
// 逐函数依据 docs/class_func_reports/Taiwan__TaiwanCash/{TaiwanCash,purchase_08173a00,
// 及 df_game_r objdump 实测（BillTransaction::init 0x08175540、increaseRequest
// 0x0817365a、CIPGMgr::getBillingCash 0x0817a2ee、CheckBillingErrorCode 0x08146669）。
// C++98 / -m32 / O0 基线。
// ============================================================================

#include "TaiwanCash.h"
#include <cstdio>
#include <cstring>

#include "CEnvironment.h"  // G_CEnvironment / STDBConnInfo / DB_HANDLE_E（InitBillingCash）
#include "LogManager.h"    // Initialize 失败路径 logFormat（ORIG IPGMgr.cpp 0x98 行文案）
#include "DNFFunctionLib.h"  // NumberToString(ull,int)（billing trace 直调）

// ---- InitBillingCash —— ORIG T 0x0814657c：
//   定义位置：置于 TaiwanCash::TaiwanCash 定义之前（P210 三态③：4.4 只见声明
//   ⇒ new-expression 保守布 EH landing pad；ORIG 0x81465b7..0x81465cc 有
//   `jmp` + operator delete landing 段实证）。
//   type = *(uchar*)(G_CEnvironment()+0x378)（= Server_Envir.m_serverGroup 低字节，
//   CEnvironment.h 布局 +0x198+0x1e0 实证）——ORIG 中保持 %esi 寄存器、无栈槽 ⇒
//   P0 判为右值内联，禁具名局部；G_CEnvironment() 亦临时直用（各次仅寄存器）。
//   new TaiwanCash(type, 1) 存入出参；以 m_dbConnInfo[DB_HANDLE_E] 的连接信息
//   init(host, port, db, user, pass)。
//   ORIG 反汇编尾部：出参判空走 0xd0 → 恒返回 1；init 结果按 0xd5/0xe0/0xe5
//   三态全部走 eax 寄存器（无栈槽 ⇒ P0：ghidra 的 bVar2 非具名局部），
//   即 `!(cash==0 || init==1)` 单表达式返回，勿拆 if 注入 bool 局部。----
// [b28d-47] verdict=DIFF(routed) [N100 + 真语义修正] ORIG 0x0814657c orig/ours=76/61
//   修：真语义 bug——ORIG 返回 true ⇔ (cash==0 || init 返回 1)（双层取反
//   bVar2=!(A||B); return !bVar2）；旧单层 !(A||B) 在 cash==0/init 成功时返回 false，
//   与 ORIG 相反。已改嵌套 !(!(...))（4.4 折叠单层但语义正确、无 bVar2 槽、槽位
//   -0x20/-0x1c 与 ORIG 对齐）。残余：new-expression landing pad 抑制（ORIG 有
//   `jmp`+_ZdlPv+_Unwind_Resume 10 条，4.4 判 ctor nothrow 不布 cleanup ⇒ N100）。
#include "TaiwanCash/InitBillingCash.inc"
}

namespace Taiwan
{

// ---- PurchaseQuery::PurchaseQuery —— ORIG 0x0817454c。----
// N76 封顶：uid 写入块 4.4 恒 LHS(this/ecx) 先物化、ORIG 4.1 恒 RHS(槽回读)先，
// 3 行时序差不可翻（exp/probe 见 GiftQuery 同判）。帧 0x28 已由 init 类内 inline 复原。
#include "TaiwanCash/PurchaseQuery.inc"
}

// ---- setQuery —— ORIG 0x081745aa。----
#include "TaiwanCash/setQuery.inc"
}
// ---- execQuery —— ORIG 0x081745f2。----
#include "TaiwanCash/execQuery.inc"
}

// ---- getResult —— ORIG 0x0817460e：任一步失败抛 int 错误码
//（0x187=set_query / 0x18a=exec / 0x18d=fetch / 400=get_ulonglong / 0x193=get_int）。
// ORIG 含 catch：LogManager 记录异常后返回 0（行号 0x199）。----
#include "TaiwanCash/getResult.inc"
}

// ---- TaiwanCash::TaiwanCash —— ORIG 0x08173608：MySQL@+8 自动构造、Mutex@+0x42094
// 自动构造，随后 +4=0、+0=type、+2=field2。----
#include "TaiwanCash/TaiwanCash.inc"
}

// ---- init —— ORIG T 0x08173686：m_mysql.init()（返回值忽略）后
// m_mysql.open(host, port, db, user, pass)。----
#include "TaiwanCash/init.inc"
}

// ---- increaseRequest —— ORIG 0x0817365a：++*(int*)(this+4)。----
#include "TaiwanCash/increaseRequest.inc"
}

// ---- checkResultCode —— ORIG 0x0817366e。----
#include "TaiwanCash/checkResultCode.inc"
}

// ---- purchase —— ORIG 0x08173a00：
//   PurchaseQuery(account, charac, price, pay, uid)
//   → processBilling 失败返回 4；
//   → checkResultCode(m_resultCode) 失败返回 m_resultCode；
//   → 成功把 query 内部事务（+0x18..0x23）拷给出参 trans 并 trace 后返回 0。----
#include "TaiwanCash/purchase.inc"
}

}  // namespace Taiwan

// explicit instantiation → ORIG W 符号 _ZN6Taiwan10TaiwanCash14processBillingINS_13PurchaseQueryEEEbRT_
template bool Taiwan::TaiwanCash::processBilling<Taiwan::PurchaseQuery>(
    Taiwan::PurchaseQuery&);

// ============================================================================
// WongWork::IPG::CIPGMgr —— 权威布局见 TaiwanCash.h；逐函数 ORIG 转写。
// 依据：docs/class_func_reports/WongWork__IPG__CIPGMgr/{Initialize,Finalize}.md
// 及 objdump 实测（ctor 0x08101108、dtor 0x08101222、GetIPGHelper 0x081053c6）。
// ============================================================================

// ---- CIPGMgr::CIPGMgr —— ORIG W 0x08101108：
//   *(this+0)=0；两个 queue<SIPGData*>（临时 deque move 构造，等价默认构造）
//   落 +0xc/+0x34；DynamicPool<SIPGData>(1000)@+0x5c；helper@+0xb8；
//   Mutex@+0xbc/+0xd4 自动构造。billingCash(+0xec) 不在此初始化（ORIG 同）。----
namespace WongWork { namespace IPG {

#include "TaiwanCash/CIPGMgr.inc"
}

// ---- CIPGMgr::~CIPGMgr —— ORIG W 0x08101222：逆序析构 mutex/pool/queue（成员自动）。----
#include "TaiwanCash/dtor_CIPGMgr.inc"
}

// ---- CIPGMgr::Initialize —— ORIG T 0x08104494：
//   workers 已存在 → -1；count!=0 时 new CIPGWorker[count]，逐个
//   SetIPGMgr(this)+ThreadInterface::begin()；写 param2@+8、count@+4；
//   InitBillingCash(billingCash) 失败 → logFormat("IPGMgr.cpp",0x98) 后 -1。----
// [b28d-47] verdict=DIFF(routed) [N82] ORIG 0x08104494 orig/ours=116/121
//   p167 CLEAR（8/8）⇒ 语义完成；残差为槽/寄存器分配跨版本差（new[] 展开 + EH 段）。
#include "TaiwanCash/Initialize.inc"
}

#include "TaiwanCash/getBillingCash.inc"
}
 }}

// DynamicPool<SIPGData>::CNode 特化 ctor（ORIG W 0x081021d6，声明见 TaiwanCash.h
// 全局作用域）：基类 value-init（POD 聚合清零）→ 4.4 -O0 发射与 ORIG 同形的
// `mov $0x0,%eax; mov $0xb7,%edx; rep stos`（探针 /tmp/bm3/p20 N1）；随后两个
// 成员各带 this 重读的函数体赋值（ORIG `mov 0x8(%ebp),%eax; movl $0x8f21,0x2dc`
// `/ movb $0,0x2e0` 形，P29 函数体序）。
#include "TaiwanCash/CNode.inc"
}

// ---- CheckBillingErrorCode —— ORIG 0x08146669：code==0 → true。----
#include "TaiwanCash/CheckBillingErrorCode.inc"
}

// ============================================================================
// Wave5 仅ORIG批 —— TaiwanCash 新增 9 方法（追加块，不改上方既有实现）。
// 依据 docs/class_func_reports/Taiwan__TaiwanCash/{getBalance,getBalance_0817381a,
// checkEntrance,gift,cancel,charge,confirm,purchase,ceraPoint}.md 汇编逐条转写。
// Query 完整类型来自 TaiwanQueries.h（TwQuery 批次）；各 processBilling<X>
// 显式实例化 → ORIG 独立 W 符号 _ZN6Taiwan10TaiwanCash14processBillingINS_IXXXEEEbRT_。
// NumberToString(y,i) @0x08109098 以 snprintf 等价替代【推断：等价日志面，
// 沿用本 TU purchase 既有模式】。所有 SQL/存储过程文本在各 Query 归属批内。
// ============================================================================

#include "TaiwanQueries.h"

namespace Taiwan
{

// ---- getBalance(const char*, int&) —— ORIG T 0x08173766 ----
#include "TaiwanCash/getBalance.inc"
}

// ---- getBalance(const char*, int&, int&) —— ORIG T 0x0817381a ----
#include "TaiwanCash/getBalance_1.inc"
}

// ---- checkEntrance(const char*, char&) —— ORIG T 0x081736ce ----
#include "TaiwanCash/checkEntrance.inc"
}

// ---- gift —— ORIG T 0x08173b20 ----
#include "TaiwanCash/gift.inc"
}

// ---- cancel(const BillTransaction&) —— ORIG T 0x08173d1c ----
#include "TaiwanCash/cancel.inc"
}

// ---- charge —— ORIG T 0x08173df0 ----
#include "TaiwanCash/charge.inc"
}

// ---- confirm(const BillTransaction&) —— ORIG T 0x08173c48 ----
#include "TaiwanCash/confirm.inc"
}

// ---- purchase(6 参重载) —— ORIG T 0x081738e6 ----
#include "TaiwanCash/purchase_1.inc"
}

// ---- ceraPoint —— ORIG T 0x08173f0c ----
#include "TaiwanCash/ceraPoint.inc"
}

}  // namespace Taiwan

// ---- processBilling<X> 显式实例化 → ORIG W 符号 ----
template bool Taiwan::TaiwanCash::processBilling<Taiwan::BalanceQuery>(
    Taiwan::BalanceQuery&);
template bool Taiwan::TaiwanCash::processBilling<Taiwan::BalancePointQuery>(
    Taiwan::BalancePointQuery&);
template bool Taiwan::TaiwanCash::processBilling<Taiwan::EntranceQuery>(
    Taiwan::EntranceQuery&);
template bool Taiwan::TaiwanCash::processBilling<Taiwan::GiftQuery>(
    Taiwan::GiftQuery&);
template bool Taiwan::TaiwanCash::processBilling<Taiwan::CancelQuery>(
    Taiwan::CancelQuery&);
template bool Taiwan::TaiwanCash::processBilling<Taiwan::ChargeQuery>(
    Taiwan::ChargeQuery&);
template bool Taiwan::TaiwanCash::processBilling<Taiwan::ConfirmQuery>(
    Taiwan::ConfirmQuery&);
template bool Taiwan::TaiwanCash::processBilling<Taiwan::CeraPointQuery>(
    Taiwan::CeraPointQuery&);
