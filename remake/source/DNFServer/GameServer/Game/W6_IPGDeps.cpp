// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 W6_IPGDeps/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// W6_IPGDeps.cpp —— WongWork::IPG 域 only_orig 清零批次（Wave6_WongWorkAssault）
//
// 目标符号（31）：CIPGHelper 全族 17 + stIPGStatus_t C1/C2 +
//   CIPGMgr 12（CIPGNetwork::ConnectTo / CIPGWorker::{dispatch,ProcIPGSync,
//   ProcIPGSyncForChina,ProcIPGSyncForTaiwan} / AllocIPGData / FreeIPGData /
//   PutIPGRequest / _GetIPGRequest / getIPGStatus / Finalize；
//   GetIPGHelper 由 TaiwanCash.h 类内 inline 定义产生 W 符号）。
//
// 报告：docs/class_func_reports/WongWork__IPG__{CIPGHelper,CIPGMgr,
//   CIPGMgr__CIPGWorker,CIPGMgr__CIPGWorker__CIPGNetwork}/。
// 无独立报告者（GetIPGMgr/CIPGMgr::getIPGStatus/stIPGStatus_t ctor）以
// df_game_r objdump 转写，地址见各函数注释。
// SIPGData 域内字段已在权威头 TaiwanCash.h 具名字段化（m_pad[0x2dc] 拆解 +
//   六个命令族 union 视图，逐字段附 ORIG 汇编宽度/偏移证据）；本 TU 一律
//   成员访问，无裸偏移访存。仅保留两处已注明豁免：put_binary(req/res, 0x2dc)
//   的整块线格式序列化、dispatch 里按 ORIG 原样行为的指针类型强转实参。
// ============================================================================

#include "CUser.h"
#include "CUserCharacInfo.h"

#include "GameTypes.h"        // Guard<Mutex> / Mutex / DynamicPool
#include "TaiwanCash.h"       // WongWork::IPG 权威类
#include "LogManager.h"
#include "GlobalData.h"       // s_stream_pool / s_msgq_mgr / s_server_stop
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "CStreamGuard.h"
#include "CEnvironment.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// ORIG T 0x0858c4e0（_ZN7OS_API5SleepEi，nm/objdump 实测）：真实定义已由
// LinkFinalDeps.cpp 落地（usleep 包装）；此处声明与 CInventory.h 权威一致。
namespace OS_API
{
#include "W6_IPGDeps/Sleep.inc"
#include "W6_IPGDeps/snprintf.inc"
}

// ORIG T（_Z14NumberToStringyi）：64 位版 NumberToString；32 位版声明在
// CInventory.h。定义属 NumberToString 权威 TU，此处仅声明。
// _SetIPGData 的 get_acc_id()（unsigned int）须选 u32 版（ORIG _Z14NumberToStringji），
// 故两重载都声明；tranNo 为 ull 仍走 64 位版。
char* NumberToString(unsigned long long value, int radix);
char* NumberToString(unsigned int value, int radix);


namespace WongWork { namespace IPG {

// ORIG T 0x080ffae0 (0x2a)。
#include "W6_IPGDeps/getIPGStatus.inc"
}

// ---------------------------------------------------------------------------
// stIPGStatus_t —— ORIG W 0x080ee3de (0x22)：依次清零 +0/+4/+8。
// ---------------------------------------------------------------------------
#include "W6_IPGDeps/stIPGStatus_t.inc"
}

// ---------------------------------------------------------------------------
// CIPGMgr —— 数据池与请求队列
// ---------------------------------------------------------------------------

// ORIG T 0x08104600 (0x6c)。
// [b28d-solo-W6_IPGDeps] verdict=NEAR/N7 封顶（447 口径 43=43，残差仅寄存器角色）。
// 修法（三处真修复）：
// ① 循环体 ORIG 0x08104618 无 i*0x10 缩放（`mov this; mov (%eax),%eax; mov (%eax),%edx;
//    call *%edx`）⇒ 原码对首元素 `m_workers->stop()`（非 m_workers[i]），照抄 P171；
// ② ORIG 0x08104648 段单次 D0 调用、无数组 cookie/元素循环 ⇒ 原码 `delete m_workers`
//    （非 delete[]），照抄；③ ThreadInterface.h 虚函数声明序修正（stop 移到虚析构前，
//    vtable 槽0=stop 与 ORIG _ZTV13ThreadInterface@0x8ce6b90 逐槽一致）；
//    ④ 空 else{return;} 复现 ORIG 0x08104667 尾 `jmp;nop` 岛（P285）。
// 残差：间接 call 目标寄存器 ORIG %edx vs 4.4.7 %eax（N7，P230：后端敏感族）。
// **双后端读数**：4.4.7 = NEAR 43=43；4.4.6 后端（-B cc1plus446bin/，头集仍 4.4.7）
// = AE（本 TU 整体 near 3→0 / ae 155→158，零回归）⇒ 已上报 Main 申请将
// W6_IPGDeps.cpp 纳入 dnf_game_cxx.sh 的 446h47 路由表（路由表禁自改）。
// 语义/调用点/帧全等。
#include "W6_IPGDeps/Finalize.inc"
}

// ORIG T 0x0810466c (0x83)：Guard(allocMutex) → pool.Acquire → memset 0x2dc。
#include "W6_IPGDeps/AllocIPGData.inc"
}

// ORIG T 0x081046f0 (0x63)：Guard(allocMutex) → pool.Free。
#include "W6_IPGDeps/FreeIPGData.inc"
}

// ORIG T 0x08104754 (0x95)：queued 且 workerCount!=0 → Guard(reqMutex) 入队，
// 返回 0；否则同步调 workers[0].ProcIPGSync 并透传其结果。
// [b28d-solo-W6_IPGDeps] verdict=AE（52=52）。修法：把 `Guard<Mutex> guard` 包进
// 内层复合块、`return 0` 移到 Guard 作用域外 ⇒ 4.4 复现 ORIG 0x081047c7 在
// Guard dtor 之后物化 `mov $0x0,%eax`（原形提前到 %ebx 缓存）。其余（Guard EH 段、
// Proc 尾）逐条同形。
#include "W6_IPGDeps/PutIPGRequest.inc"
}

// ORIG T 0x081047ea (0x9a)：Guard(reqMutex)；空队→0；front 为 0 → 返回 0 不弹；
// 否则 pop 并返回原 front。
#include "W6_IPGDeps/_GetIPGRequest.inc"
}

// ORIG W 0x081010ae (0x45)：objdump 转写（无独立报告）——
// status+0=pool.Remain、+4=pool.Size、+8=reqQueue.size。
// [b28d-solo-W6_IPGDeps] 修法：字段改 int（P211/P248 档③）+ 删三个冗余 cast——
// 前 2 store 与 Remain/Size() 的 int 返回同型 ⇒ LHS-first；第 3 store 与
// queue::size() 的 size_t 异型 ⇒ RHS-first+edx 中转。三 store 装载序与 ORIG 同形。
#include "W6_IPGDeps/getIPGStatus_1.inc"
}

// ---------------------------------------------------------------------------
// CIPGMgr::CIPGWorker —— 线程体与同步处理链
// ---------------------------------------------------------------------------

// ORIG T 0x08105258 (0x1d)：tail-call ProcIPGSyncForTaiwan。
#include "W6_IPGDeps/ProcIPGSync.inc"
}

// ORIG T 0x081048b4 (0x12)：local -0x4 = 0 后返回（恒返空结果）。
#include "W6_IPGDeps/ProcIPGSyncForChina.inc"
}

// ORIG T 0x08105276 (0xcb)：主循环 —— 取请求→同步处理→接口结果回调（vptr+0x34 =
// IIPGHelperInterface slot11 IPGResult，CIPGHelper vtable 0x08b420a0 槽序实证）→
// 释放临时结果与请求；s_server_stop 置位退出。
// b28d-Main 终裁（P282）：ORIG 0x081052dc 段 `mov (%eax),%edx; add $0x34,%edx;
// mov (%edx),%ecx` = vptr+0x34 = slot13 = IIPGHelperInterface::IPGResult
// （TaiwanCash.h:244；vptr+0x2c=slot11 才是 IPGQuery）⇒ 调用目标为
// IPGResult(req, res)（压栈序 arg1=req@0x4/arg2=res@0x8），非 IPGQuery。已上报 Main 裁定。
// [b28d-solo-W6_IPGDeps] verdict=NEAR/N7 封顶（447 口径 58=58，残差仅寄存器角色）。
// 修法：① 内联
// `m_mgr->GetIPGHelper()->IPGResult(req,res)`（ORIG helper 指针无槽往返，P0/P52，
// 消 -0xc 具名槽）；② req==0 判定改 `while(req==0){Sleep;goto next_iteration;}`
// 复现 ORIG `cmpl $0,-0x10; sete %al; test %al,%al; je <Proc>` 的 ==0 物化
// （P225 while 形；4.4 对 if 恒折 jne 直跳）。残差=虚调用目标寄存器 ORIG %ecx/
// 实参 %edx vs 4.4.7 %edx/%ecx（N7）。双后端读数同 Finalize：4.4.6 后端下 AE。
#include "W6_IPGDeps/dispatch.inc"
}

// ---------------------------------------------------------------------------
// ProcIPGSyncForTaiwan —— ORIG T 0x081048c6 (0x992)。switch(data->m_type)：
// 0=charge、2=purchase、3=cancel、4=getBalance、5=confirm、6=ceraPoint、
// 其余（含 1）记日志。cash 取 m_mgr->m_billingCash（ORIG 6 处均直接解引用
// m_mgr(+0xc)→+0xec，无 getBillingCash call）。载荷按命令族视图访问
// （m_cmd.m_input / m_output / m_cancel / m_point / m_query，见 TaiwanCash.h）。
// §6.2 巨件核销（b25）：P167 修前 50/56 [多6 getBillingCash]→修后 50/50
// [0缺0多]；gap 0→0（orig 601 / our 601 指令数相等）；修法：6 处
// getBillingCash() 调用改 m_mgr->m_billingCash 直接成员访问（ORIG 本函数
// 内联解引用 +0xec，访问器不产生 call；TaiwanCash.cpp 的 out-of-line 定义
// 保留供 0x08175f94 等真实 call 点使用）。残差为 N82 槽序/ndiff 951 字节层
// 排布（详见 docs/asm_translation_patterns.md 负例表）。
// ---------------------------------------------------------------------------
// [b28d-solo-W6_IPGDeps] verdict=DIFF/N4+N7+N107 封顶（§6.2 三件套）。
// 本轮修法（四组，全部有 ORIG 证据）：
//  ① **case 块物理顺序重排**：ORIG jump table @0x8b42290 七项目标地址排序 =
//     case 4/2/5/3/0/6/1，P46「叶块排放顺序 = case 书写顺序」⇒ 源码按 4,2,5,3,0,6 书写；
//  ② **删 11 个视图指针局部**（rq/in/cash×2/out/ro/cv×2/pt/rv，P183/P134）——
//     ORIG 各点 `mov 0xc(%ebp),%eax; add $0xNN` 重算，指针局部使帧 0xdc（ORIG 0x9c）；
//  ③ **case 2 pay/price 内联实参**（P0：ORIG 两 atoi 结果只在 %ebx/%eax 跨第二 atoi
//     保活、无槽往返 ⇒ 原码非具名局部）；
//  ④ **case 3/5 tran 改整体赋值** `tran = data->m_cmd.m_cancel.m_tran;`（P247：ORIG
//     单基址 0x1f0/0x1f4/0x1f8 三 dword 连续拷贝）。
// 三件套读数（447 与 446 双后端一致）：① gap=16（601 vs 585，逐点 = 下述 add-vs-lea
// 每处少 1 条）；② p167 调用点差集 CLEAR（50/50，0 缺 0 多，/tmp/tu_W6_IPGDeps.o
// 复验）；③ 帧 0x9c 与 ORIG 全等、槽集差 1（多 0x68 = case 0 tran 与 case 6 缓存
// 复用边界的 4B 槽），语义逐块比对完成（Ghidra C 段 622-812 逐 case 核对）。
// 残差 = 4.1 `add $K,%eax; mov %eax,%reg` vs 4.4 `lea K(%eax),%reg`（N4/P30/N107，
// 探针 /tmp/addprobe{,2,3}.cpp 证实 4.4 在"地址+另一变参并存"时恒 lea）
// + 间接调用/实参寄存器角色（N7）。语义完成、byte 层 N 封顶。
#include "W6_IPGDeps/ProcIPGSyncForTaiwan.inc"
}

// ---------------------------------------------------------------------------
// CIPGMgr::CIPGWorker::CIPGNetwork::ConnectTo —— ORIG T 0x081043e0 (0xb4)：
// socket(AF_INET,SOCK_STREAM,0) → connect(inet_addr/htons)，失败码 -1/-2。
// socket/connect/inet_addr/htons 经 PLT（同 ORIG），加 <sys/socket.h 等 include 见尾注。
// ---------------------------------------------------------------------------
} }  // namespace WongWork::IPG

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace WongWork { namespace IPG {

// [b28d-solo-W6_IPGDeps] verdict=AE（48=48，原判 N(P75) 作废）。唯一残差原为
// connect 三实参求值序（ORIG arg3(len)→arg2(&addr)→arg1(m_sock) 严格右到左；
// 4.4.7 裸形恒 arg2→arg1→arg3）**且** ORIG 的 len 占浅槽 -0xc、addr 占深槽 -0x1c。
// 复现形（探针 /tmp/ct_probe6.cpp g15，与 ORIG 逐条同形）：
//   ① len 声明**前置**于 `sockaddr_in addr;` 之前（P309 头部声明方向 ⇒ 占浅槽）；
//   ② volatile 限定（P316 ⇒ volatile 读先于非 volatile 实参求值）；
//   ③ 赋值语句 `addrLen = sizeof(addr);` 留在原位（避免重复初始化）。
// 语义/调用点/帧一致；报告 ConnectTo.md 的 Ghidra C `local_10 = 0x10;` 对应一致。
#include "W6_IPGDeps/ConnectTo.inc"
}


// ---------------------------------------------------------------------------
// CIPGHelper —— 报告 docs/class_func_reports/WongWork__IPG__CIPGHelper/
// ---------------------------------------------------------------------------

// ORIG T 0x08100b3e (0x153)。
// [b28d-solo-W6_IPGDeps] verdict=AE（92=92，原判 N(N81/N102) 作废）。
// 唯一残差原为 `m_ipgIP[0]` 读：ORIG 0x08100ba7 两步 `add $0x2b8,%eax;
// movzbl (%eax),%eax`，4.4 对数组元素直读恒折单条 disp。复现形 =
// `((unsigned char*)&G_CEnvironment()->m_serverEnvir.m_ipgIP)[0]`（取数组成员
// 地址 + 转 uchar* + 下标，P311 同族；探针 /tmp/init_probe2.cpp f1 折单条
// vs f15/f18 出两步且零槽）。仍为具名成员访问，非裸偏移；报告 Initialize.md
// 的 Ghidra C `*(char *)(iVar1 + 0x2b8) == '\0'` 语义一致。P167 CLEAR（9/9）。
#include "W6_IPGDeps/Initialize.inc"
}

// ORIG T 0x08100c92 (0x3f)：mgr->Finalize → ~CIPGMgr → delete → 清指针。
#include "W6_IPGDeps/Finalize_1.inc"
}

// ORIG W 0x081010a2 (0xb)：objdump 转写 —— return *(this+4)。
#include "W6_IPGDeps/GetIPGMgr.inc"
}

// ORIG T 0x08100a2e (0x10f)。
// [b28d-solo-W6_IPGDeps] verdict=AE（74=74）。修法（三处）：① 删 accStr 具名局部，
// NumberToString 结果内联作 snprintf 第 4 变参（ORIG 0x08100a56 无槽往返，P0/P134）；
// ② SIPGData::m_uid unsigned→int ⇒ ORIG 0x08100b22 `call GetUID; mov %eax,%edx;
// mov -0xc(%ebp),%eax; mov %edx,0x20(%eax)` 的 RHS-first+中转（P211 档③/BL2）；
// ③ m_notifyFlag unsigned char→bool + 源 `(unsigned char)flag` ⇒ ORIG 0x08100b31
// LHS-first（P300）。P167 CLEAR（9/9）。
#include "W6_IPGDeps/_SetIPGData.inc"
}

// ORIG T 0x081007e8 (0x76)。
// b28d-60 verdict=AE（IDENTICAL）：m_type→enum（P254）+ m_dataTableA→int（P265）
// 两处类型旋钮修正后 37=37 逐条全等。
#include "W6_IPGDeps/IPGRequest.inc"
}

// ORIG T 0x08100790 (0x57)。
#include "W6_IPGDeps/IPGQuery.inc"
}

// ORIG T 0x080ffb0a (0x19a)。
#include "W6_IPGDeps/IPGInput.inc"
}

// ORIG T 0x080ffca4 (0x203)。
// [b28d-solo-W6_IPGDeps] verdict=AE（121=121）。修法：① 删 in 具名局部，全字段走
// `data->m_cmd.m_input.*` 成员链（ORIG 各点 add/lea $0xdd/0xe8/… 重算，P0/P183）；
// ② SIPGDataInputView::m_price unsigned→int（P211 档③）⇒ ORIG 0x080ffdxx
// `mov 0x14(%ebp),%edx; mov -0xc(%ebp),%eax; mov %edx,0x98(%eax)` RHS-first 3 条形。
// P167 CLEAR（18/18）。
#include "W6_IPGDeps/IPGInput_1.inc"
}

// ORIG T 0x080ffea8 (0x118)。
#include "W6_IPGDeps/IPGInputCancel.inc"
}

// ORIG T 0x080fffc0 (0x135)。
#include "W6_IPGDeps/IPGInputPoint.inc"
}

// ORIG T 0x0810022e (0x11f)。
#include "W6_IPGDeps/IPGOutput.inc"
}

// ORIG T 0x081000f6 (0x137)。
// [b28d-solo-W6_IPGDeps] verdict=AE（76=76）。修法：删 `SIPGDataPointView* pt`
// 具名局部，全部改 `data->m_cmd.m_point.*` 成员链（ORIG 0x081000f6 各字段
// `add $0x8c/0x94/0xac/0xb0/0xb1/0xb2` 直接重算，无槽往返，P0/P183）。
// P167 CLEAR（11/11）。
#include "W6_IPGDeps/IPGOutputPoint.inc"
}

// ORIG T 0x0810034e (0x18f)。返回值恒为请求临时块地址（调用前已释放）——ORIG 如此。
// [b28d-solo-W6_IPGDeps] verdict=NEAR/N7 封顶（447 口径 108=108，残差仅寄存器角色）。
// 修法：删 out/rv 具名局部，全字段走 `req->m_cmd.m_output.*` /
// `res->m_cmd.m_output.*` 成员链（ORIG 各点 add $0x69/0x71/0x7d/0x91/0xf5/0x173
// 重算，P0/P183）。残差=虚调用目标寄存器 ORIG %edx vs 4.4.7 %eax（N7）。
// 双后端读数同 Finalize：4.4.6 后端下 AE。P167 CLEAR（11/11）。
#include "W6_IPGDeps/IPGOutputSimulation.inc"
}

// ORIG T 0x081004de (0x138)。
// [b28d-solo-W6_IPGDeps] verdict=AE（77=77）。修法：① 删 cv 具名局部，全字段走
// `data->m_cmd.m_cancel.*` 成员链（ORIG 各点 add $0x69/0x7e/… 重算，P0/P183）；
// ② tran 拷贝改匿名 union 视图整体赋值 `data->m_cmd.m_cancel.m_tran = *tran`
// （TaiwanCash.h P246 双视角）⇒ 复现 ORIG 尾部 tran 指针单载 + 3 dword 连续拷贝。
#include "W6_IPGDeps/IPGOutputCancel.inc"
}

// ORIG T 0x08100616 (0x179)：Alloc 响应块 → 拷 req/res 字段 → 投递（无前置保留字段）。
// [b28d-solo-W6_IPGDeps] verdict=AE（97=97）。修法：① 删 dv/rv/sv 具名局部，req/res
// 各点成员链重算（P0/P183）；② tran 拷贝改 union 视图整体赋值
// `d->m_cmd.m_cancel.m_tran = res->m_cmd.m_output.m_tran`（P246）⇒ ORIG 尾部
// res 指针单载 + 3 dword 连续拷贝。P167 CLEAR（9/9）。
#include "W6_IPGDeps/IPGOutputCancel_1.inc"
}

// ORIG T 0x08100cd2 (0x138)。
// [b28d-solo-W6_IPGDeps] verdict=AE（77=77）。修法：① 删 cv 具名局部，全字段走
// `data->m_cmd.m_cancel.*` 成员链（P0/P183）；② tran 拷贝改匿名 union 视图整体赋值
// `data->m_cmd.m_cancel.m_tran = *tran`（P246）⇒ ORIG 尾部单载 + 3 dword 连续拷贝。
// P167 CLEAR（8/8）。
#include "W6_IPGDeps/IPGOutputConfirm.inc"
}

// ORIG T 0x08100e0a (0x194)：前置写保留字段后按 Cancel 同款拷 req/res → 投递。
// [b28d-solo-W6_IPGDeps] verdict=AE（103=103）。修法：① 删 dv/rv/sv 具名局部，
// req/res 各点成员链重算（P0/P183）；② tran 拷贝改 union 视图整体赋值（P246）
// ⇒ ORIG 尾部 res 指针单载 + 3 dword 连续拷贝。P167 CLEAR（9/9）。
#include "W6_IPGDeps/IPGOutputConfirm_1.inc"
}

// ORIG T 0x0810085e (0x1d0)：结果回传流 —— Acquire→header 0x48/err -1→按有无
// 响应数据组织 0/1 分支 put_binary(req[,res])→回调数据或回收。
// [b28d-solo-W6_IPGDeps] verdict=AE（120=120）。修法：① 删 stream 具名局部，
// `Acquire(...)` 内联作 CStreamGuard ctor 实参（ORIG 0x0810086e 无槽往返，P259，
// guard 槽回 -0x10）；② 两处 if/else 按 ORIG 块序翻转写（P47/P59）：res!=0 块直落
// + res==0 块 je 落后（0x08100985）、notifyFlag!=0 块直落 + Free 块 je 落后
// （0x081009eb）。P167 CLEAR（22/22）。
#include "W6_IPGDeps/IPGResult.inc"
}

} }  // namespace WongWork::IPG
