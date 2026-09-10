// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 TowerAPCLeafC3/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// TowerAPCLeafC3.cpp —— TowerOfDespair APC 发送链（TowerAPCLeafC3 批次）
//
// 逐指令依据 docs/class_func_reports/（## 汇编 + ## 反编译 C 已完整读取）：
//   TowerOfDespair_APC_Mgr/Send_APC_Info.md   ORIG T 0x085feee4（本批次目标）
//   SendingAPC_Info/Send_APC_Info.md          ORIG T 0x085febca
//   SendingAPC_Info/Set_APC_Info.md           ORIG T 0x085fe906
//   TowerOfDespairMgr/SendMessageToCUser.md   ORIG T 0x08644366（静态）
// 辅助 W 内联符号与 ORIG nm 一致：
//   TOD_Layer::GetLayer            0x0822ec40 W（头文件 inline）
//   TOD_Layer::operator<           0x085ff114 W（头文件 inline；map key 序）
//   SendingAPC_Info::get_user_charac_no      0x085ff12c W（头文件 inline）
//   cMyTraceNoop C1/cl             0x0822ad5a/0x0822ad60 W（既有独立 TU 提供）
// ============================================================================

#include <stdio.h>
#include <string.h>

#include "TowerOfDespairMgr.h"
#include "GameTypes.h"       // cMyTrace(char const*, int, int) / operator()(char const*, ...)
#include "cMyTraceNoop.h"    // cMyTraceNoop
#include "PacketGuard.h"
#include "CUser.h"           // Send(PacketGuard&) / isGMUser / getCurCharacNo

// ---------------------------------------------------------------------------
// SendingAPC_Info::Set_APC_Info —— ORIG T 0x085fe906 [0x085fe906, 0x085febc9]
// clear → m_userCharacNo=info[+0] → put_header(0,0x15a) → put_byte(layer)
// → (put_int(len)+put_str(name@+4)) → put_byte(lv/job/grow_type @+24/+28/+2c)
// → (put_int(len)+put_str(guild@+30)) → put_int(pvp_grade@+48)
// → 循环 i=0..21：put_int(*(int*)(info + 0x4c + i*4))（能力表 22 项）
// → (put_int(len)+put_str(creature_name@+a4)) → put_int(creature_id@+b4)
// → finalize(true) → cMyTrace 日志。
// ---------------------------------------------------------------------------
// SendingAPC_Info::SendingAPC_Info —— ORIG T 0x085fe860 [0x085fe860,0x085fe8c9]
// 报告 SendingAPC_Info/SendingAPC_Info.md：构造 +0x04 PacketBuf → clear →
// memset(m_apcInfo@+0x17338, 0, 0xbc)。
#include "TowerAPCLeafC3/SendingAPC_Info.inc"
}

// SendingAPC_Info::~SendingAPC_Info —— ORIG T 0x085fe8ca [0x085fe8ca, 0x085fe8df]
// 仅析构 +0x04 内嵌 PacketBuf（m_buf），成员自动析构等价。
#include "TowerAPCLeafC3/dtor_SendingAPC_Info.inc"
}

// [b28d-32 N(P75): 1 条装载序/寄存器差（202/201） | ORIG 0x085fe906]
#include "TowerAPCLeafC3/Set_APC_Info.inc"
}
// ---------------------------------------------------------------------------
#include "TowerAPCLeafC3/Send_APC_Info.inc"
}

// ---------------------------------------------------------------------------
// TowerOfDespairMgr::SendMessageToCUser —— ORIG T 0x08644366
//                                        [0x08644366, 0x0864446f]（静态成员，
//                                        asm 无 this 槽，user@+8/msg@+0xc）
// isGMUser 命中才发包：PacketGuard() → put_header(0,0xc) → byte/short/byte 0
// → put_int(strlen(msg)) → put_str(msg, strlen(msg))（ORIG 调用两次 strlen）
// → finalize(true) → Send。
// ---------------------------------------------------------------------------
#include "TowerAPCLeafC3/SendMessageToCUser.inc"
}

// ---------------------------------------------------------------------------
// TowerOfDespair_APC_Mgr::Send_APC_Info —— ORIG T 0x085feee4
//                                          [0x085feee4, 0x085ff01f]（本批次目标）
// find(layer)；未命中 → cMyTrace("...Send_APC_Info(const TOD_Layer&, CUser*)",
// 0xa1, 5)("TOD:invalid layer %d", GetLayer)；
// 命中 → it->second.Send_APC_Info(layer,user) → get_user_charac_no()
// → cMyTraceNoop 构造后输出
// "TOD:sending [%d] toUser, layer [%d] : clone : [%d]"(getCurCharacNo,layer,no)。
// 注：运行期 mgr 对象由 CtorProducers_1.cpp 的 new 创建（空 map 内存映像与
// 真实 map<TOD_Layer,SendingAPC_Info> 一致）；填充方 Set_APC_Info 后续批次
// 接入时须按本真实容器模型实现。
// ---------------------------------------------------------------------------
// [b28d-32 N(P75): 装载序/寄存器差（90/87） | ORIG 0x085feee4]
#include "TowerAPCLeafC3/Send_APC_Info_1.inc"
}
