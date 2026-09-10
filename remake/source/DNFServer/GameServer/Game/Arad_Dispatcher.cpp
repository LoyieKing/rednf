// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 Arad_Dispatcher/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— ARAD::Arad_Dispatcher / Arad_Dispatcher_Manager /
// Arad_Dispatcher_BuyItemCheck_Impl + ARAD::DATABASE::ITEM_QUERY 查询。
// 依据 docs/class_func_reports/ARAD__Arad_Dispatcher{,_Manager}/、
// ARAD__Arad_Dispatcher_BuyItemCheck_Impl/、ARAD__DATABASE__ITEM_QUERY/ 报告逐函数转写。
// C++98 / -m32 / O0 基线。
// ============================================================================

#include "Arad_Dispatcher.h"

#include <string.h>
#include <stdio.h>
#include <time.h>

#include "GlobalData.h"        // s_db_mgr / s_stream_pool / s_msgq_mgr / s_pCeraShop / s_pItemVendingMachine
#include "Stream.h"            // Stream operator>> / GetOutBuffer<T>
#include "StreamPool.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "DBMgr.h"             // DBMgr 完整类型（GlobalData::s_db_mgr->GetDBHandle）
#include <stddef.h>
#include "MySQL.h"
#include "LogManager.h"
#include "SigTypes.h"          // MSG_BUY_CERASHOP_ITEM（0x3f84）
#include "MoreTypes.h"         // ARAD::INTERNAL_PACKET::PURCHASE_COUNT_CHECK
#include "CCeraShop.h"         // WongWork::CCeraShop::ProcessError
#include "ItemVendingMachine.h"
#include "CUser.h"

// 布局断言：ORIG Manager ctor 0x0818fdd4 两组各 0x354 指针数组，size = 0x1AA0。
typedef char AradDispatcherManagerSizeAssert[
    sizeof(ARAD::Arad_Dispatcher_Manager) == 0x354 * 4 * 2 ? 1 : -1];

// b28c-L26 [判词 N·跨版本 new-expression landing-pad 抑制，模板实例变体]
// 实测在 Manager ctor 定义**之前**放成员函数显式实例化
// （`template void GlobalInstance<ARAD::Arad_Dispatcher_Manager>::create();`）
// 仍产 24 条形（无 cleanup 岛）——4.4 对模板成员的代码生成延至 TU 末尾，
// 届时 ctor 体恒可见 ⇒ 判 nothrow 抑 landing pad。P210/BM11「定义前置」翻案
// 仅适用**非模板** caller（open()/NexonCash C1）；对 GlobalInstance<T>::create()
// 这类头模板成员无源码解（禁改头/禁删 ctor）。ORIG 37 条 = 双检 + new(0x1aa0)
// + ctor call + `mov %ebx,%eax; mov %eax,m_p` + 正常路径逐条等价，差额 13 条
// 全落在 ctor 抛异常的 delete+_Unwind_Resume 清理岛及其 push %edi/%esi/帧 0x1c。
// 已回退该无效实例化声明，保持纯自然形。
// [b28d-23 N·N100/模板] 复测维持：模板成员 create 的 landing pad 抑制无源码解
// （P210 定义前置仅适用非模板 caller）⇒ N。ORIG 0x0818e956

// ---------------------------------------------------------------------------
// 本 TU 实例化：GlobalInstance<ARAD::Arad_Dispatcher_Manager>::create()  @0x0818e956
// （报告 docs/class_func_reports/StrikerScript/create.md 属撞名——其 `## 反编译 C` 段为
// StrikerScript::create@0x8a9dc70，与本符号无关；verdict 以 nm 精确切片的 ORIG 汇编为权威，
// §6.3）。
// [b23判词·跨版本 N（new-expression landing-pad 抑制）+ AE 假阳性] 双检（两次
// `mov m_p,%eax; test; jne`）、`movl $0x1aa0` → new → Manager ctor → `mov %ebx,%eax;
// mov %eax,m_p` → `jmp` + `nop` 岛与 ORIG 正常路径逐条相等；37−24=13 条差额全部落在
// ctor 抛异常的清理岛（operator delete + _Unwind_Resume）及其连带的 push %edi/%esi 与
// 帧 0x1c/0x14（根因同 open()，见其判词：基类 ctor/dtor 本 TU 可见空定义 ⇒ 4.4 推断
// nothrow 抑制 landing pad，/tmp 探针 A/B/C/D 实证）。另有 3 处 m_p 数据地址与
// 2 处 call 目标显示为 .o 重定位占位（§6.3 假阳性），relink 后转 IDENTICAL_AE。
// ---------------------------------------------------------------------------

namespace ARAD
{

#include "Arad_Dispatcher/db_dispatch.inc"
}

// ============================================================================
// Arad_Dispatcher_Manager::open —— ORIG 0x0818fe38。
// [b28 BM11] P210 定义前置翻案（推翻 b23「landing-pad 抑制不可复现」判词，与
// NexonCash C1 同法）：open 定义前置于 BuyItemCheck_Impl ctor 定义之前 ⇒ Impl
// ctor 本 TU 仅原型可见 ⇒ 4.4 对 new-expression 保守生成 EH cleanup
// （delete+Unwind_Resume 岛 + push %edi/%esi + 帧 0x2c/槽 -0x1c，ORIG 全形）。
// ============================================================================
#include "Arad_Dispatcher/open.inc"
}

// ============================================================================
// Arad_Dispatcher_BuyItemCheck_Impl —— ORIG C1 @0x0818e9cc：基类码 0x1fa。
// P210：本 ctor 定义必须先于基类 Arad_Dispatcher(int) ——基类 ctor 本 TU 可见时
// 4.4 按其后继帧精确继承 outgoing（帧 0x18→0x8，与 ORIG 0x18 反）。
// ============================================================================
#include "Arad_Dispatcher/Arad_Dispatcher_BuyItemCheck_Impl.inc"
}

// ============================================================================
// Arad_Dispatcher —— ORIG C1 @0x0818edd0：vptr 由编译器写；m_code = code（+0x04）。
// ============================================================================
#include "Arad_Dispatcher/Arad_Dispatcher.inc"
}

// ORIG D1 @0x0818e900：仅写基类 vptr；D0 条件 operator delete(this)。
#include "Arad_Dispatcher/dtor_Arad_Dispatcher.inc"
}

#include "Arad_Dispatcher/inter_dispatch.inc"
}

#include "Arad_Dispatcher/register_dispatcher.inc"
}

#include "Arad_Dispatcher/unregister_dispatcher.inc"
}

// ============================================================================
// Arad_Dispatcher_Manager —— ORIG C1 @0x0818fdd4：两个 0x354 项指针数组清零。
// ============================================================================
#include "Arad_Dispatcher/Arad_Dispatcher_Manager.inc"
}

#include "Arad_Dispatcher/dtor_Arad_Dispatcher_Manager.inc"
}

#include "Arad_Dispatcher/register_dispatcher_1.inc"
}

#include "Arad_Dispatcher/unregister_dispatcher_1.inc"
}

#include "Arad_Dispatcher/inter_dispatch_1.inc"
}

#include "Arad_Dispatcher/db_dispatch_1.inc"
}

#include "Arad_Dispatcher/dtor_Arad_Dispatcher_BuyItemCheck_Impl.inc"
}

#include "Arad_Dispatcher/inter_dispatch_2.inc"
}

// [b29-R28 复核 · N104/N121 封顶] ORIG 0x0818eb0c（158 条）：p167 = 纯 **P93 同址
// 别名伪差**（MISSING `CStreamGuardD2Ev`×2 / `CStreamGuardC2EP6Streamb`×1 vs EXTRA
// `D1Ev`×2 / `C1EP6Streamb`×1，同址多符号 objdump 选名差异）⇒ 调用点多集 CLEAN。
// 指令差仅两处**块序**：①ORIG 正常路径尾 `guard.~D1()` 共享块在函数末（`jmp +0x254`），
// 4.4 把该析构就地内联在成功路径（尾部只留 EH unwind 块）②ORIG `mov $0x0,%ebx`
// 就地发射两次（0x0818ecf5/0x0818ed3e），4.4 cross-jump 合并为一份共享块。
// 二者 = 4.4 block-merge/尾块相对序（N104/N121 族，逐条内容全等）⇒ N 封顶，语义等价。
// 裸 register ret 保留（P248 三条件：跨 call 保活+4.4 不自升+ORIG 用 %ebx）。
#include "Arad_Dispatcher/db_dispatch_2.inc"
}

// ============================================================================
// ARAD::GetQuarterOfYear —— ORIG T 0x0819e8d0。
// time/localtime 后 quarter=tm_mon/3+1（0x55555556 除魔数）、year=(tm_year+1900)%2000
// （0x10624dd3/sar7 除魔数 = /2000 + imul 0x7d0 取余），sprintf("%02d%d", year, quarter)。
// ============================================================================
// [b29 P0/P329] ORIG 0x0819e8d0：`time(&t)` 取址传参（非 time(0)）；year/quarter
// 无栈槽往返（quarter 留 %ebx: `lea 0x1(%eax),%ebx`，year 中间值留 %esi）⇒ 原码
// 无具名局部、sprintf 实参内联（右→左：quarter 先求，tm_year 只读一次由 CSE 保证）。
#include "Arad_Dispatcher/GetQuarterOfYear.inc"
}

#include "Arad_Dispatcher/arad_sp_buy_item_check.inc"
}


#include "Arad_Dispatcher/dispathc_code.inc"
}

}  // namespace ARAD

