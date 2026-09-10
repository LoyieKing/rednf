// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 Gm_List/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r：Gm_List 独立编译单元。
// ---- b28d-61 判词汇总（本 TU 2 件非 identical，全 N 封顶）----
// SendGMList_To_UpperServer 0x08299d54（P167 MISSING+EXTRA：Packet_Sync_GM_List
//   构造/发送块差异，N80 变参求值序）、LoadGmListFromDb 0x08299a10
//   （P167 CLEAR 38/38；N78/N89 迭代器+局部初始化 lowering）。
// ORIG SendGMList_To_UpperServer @0x8299d54：
//   serverGroup 0=监视服务器、1=公会服务器；每 20 个 GM 一包（Packet_Sync_GM_List，
//   header 0x1f4d/0x70），m_pageNo 递增；末批不足 20 也发送。
#include "CGuildServerProxy.h"   // Gm_List / WongWork::CGMAccounts 权威声明

#include <list>

#include "CGuildServerProxy.h"      // WongWork::CGMAccounts / stGMInfo_t / proxies
#include "CMonitorServerProxy.h"
#include "CServerProxyMgr.h"
#include "CEnvironment.h"
#include "GlobalData.h"
#include "Packet_Sync_GM_List.h"
#include "DBMgr.h"
#include "MySQL.h"
#include "CInventory.h"   // NumberToString

// 类声明权威点在 CGuildServerProxy.h；此处仅实现。
#include "Gm_List/SendGMList_To_UpperServer.inc"
}


// ---- 生命周期 / DB 装载 ----

// ORIG 0x08299912：m_db = DBMgr::GetDBHandle((ENUM_DB_HANDLE_IDX)6, (ENUM_SERVER_GROUP)0)
// 后立即 LoadGmListFromDb()。
#include "Gm_List/Gm_List.inc"
}

// ORIG 0x08299948：D1 为 deleting 形态（cmpl $0,0x8(%ebp); je;
// call D1(自身递归); call _ZdlPv）——源码为 `delete this` 展开。
#include "Gm_List/dtor_Gm_List.inc"
#include "Gm_List/LoadGmList.inc"
}

// ORIG 0x08109510。
#include "Gm_List/clearGmList.inc"
}

// ORIG 0x0829996c：查询 "seLect m_id, level from gm_manifest"；失败 trace 返回；
// 成功先 clearGmList，逐行 fetch 取 m_id/level 并 LoadGmList，累计 m_id 日志串；
// 若 get_channel_no()==1 再 SendGMList_To_UpperServer(0) 同步监视服务器。
// [A28 round5 定稿] 本轮修复（ORIG 0x0829996c 逐段对齐）：
//   ① 返回类型 bool（三处 `mov $0x0,%ebx; jmp` + 尾 `movzbl -0x11(%ebp),%ebx`）；
//   ② P212 全头部声明（ORIG 序言四条 `movb/movl $0x0,-0x11/-0x10/-0x74/-0x78`）；
//   ③ 删 accounts 具名指针（P0：ORIG `mov &s_pGMAccounts_,%eax; mov %eax,(%esp)`）；
//   ④ fetch 结果丢弃 + 同一 ok 复用（P102；ORIG 无 `!fetch()||` 短路段）；
//   ⑤ P221/P227 五处 cMyTrace 改全表达式临时链（具名对象 4.4 复用同槽致帧浅 0x30，
//      同族 CGM_Manager::LoadGmListFromDb 0x08299530 实证）；
//   ⑥ idLog.append 提到 GM M_ID trace 之前（ORIG 0x08299af1 段 < 0x08299bd2）；
//   ⑦ 删旧 `ok = true;`（ORIG 无该 store）；⑧ 发送用 GlobalData::gmList（非隐式 this）。
//   残余 gap=5：三处 `mov $0x0/$0x1,%esi`（4.4 EH 状态寄存器，ORIG 无）+ 两处
//   NumberToString 结果 %ebx(ORIG)/%esi(4.4) ⇒ 判 N104（EH 尾块相对序，ORIG 正常
//   路径尾块在后/4.4 就地内联）+ N7（callee-saved 角色）封顶。
//   p167 调用点差集：仅 Packet_Sync_GM_List C1↔C2 同址别名（P93 良性）。
#include "Gm_List/LoadGmListFromDb.inc"
}
