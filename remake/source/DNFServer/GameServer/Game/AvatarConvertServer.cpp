// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 AvatarConvertServer/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r Game/ AvatarConvertServer + ARAD::SCRIPT::AvatarConvert 实现。
// 逐指令转写依据 docs/class_func_reports/ARAD__SCRIPT__AvatarConvert/*.md 与
// docs/class_func_reports/AvatarConvertServer/{AvatarConvertServer,~AvatarConvertServer,
// Test,Load,Destroy}.md（ORIG 地址见各函数注释）。
#include <cstring>
#include <new>

#include <cstdio>
#include <string>

#include "AvatarConvertServer.h"
#include "GameTypes.h"      // cMyTrace
// ----------------------------------------------------------------------------
// [b28d-E03 2026-09-10 本波复核] 四数 102/182/1/5 不变（4 件维持封顶）：
//   · StatisticsEmblemCreate 115/112：P231 纯 register 试形（itemId 升格 callee-saved）
//     实测**无效已回退**；残差 = 实参升格时刻 P75 + N94/N4 索引分解 + N22。
//   · CheckCondition 293/292：N18a 两段装 callee-saved（`mov 0x4(%eax),%eax;
//     mov %eax,%ebx` 4.4 恒折单段）。
//   · Convert 610/604：N18a/N82（p167 CLEAR）。
//   · ARAD::SCRIPT::AvatarConvert::loadScript NEAR：N7 寄存器角色（路由 446 后端已启用）。
//   · `_ZNSt20__uninitialized_copyILb0EE…move_iterator…` 实例：N89 库头族。
// ----------------------------------------------------------------------------
#include "DNFLexWrapper.h"  // loadRDARScriptFile/unloadRDARScriptFile/ScanType/
                            // ScanInt(bool*)/ScanTypeOrInt（真实符号声明）

// ---- 布局断言（ORIG 反汇编实证；std::map=0x18 与 ORIG 一致）----
typedef char avatar_convert_size_assert[sizeof(ARAD::SCRIPT::AvatarConvert) == 0x18 ? 1 : -1];
typedef char avatar_convert_getinfo_size_assert[sizeof(ARAD::SCRIPT::AvatarConvert::GetInfo) == 0x8 ? 1 : -1];
typedef char avatar_convert_result_size_assert[sizeof(ARAD::SCRIPT::AvatarConvert::ConvertResult) == 0x24 ? 1 : -1];
typedef char avatar_convert_server_size_assert[sizeof(AvatarConvertServer) == 0x1c ? 1 : -1];

namespace ARAD
{
namespace SCRIPT
{

// ============================================================================
// GetInfo —— POD 聚合；trivial D1/D0 弱符号由编译器在 vector 销毁路径产生
// （ORIG W 0x088b3148）。
//
// ConvertResult —— ORIG W ctor 0x088b1f42 / dtor 0x088b1f8e。ORIG loadScript 先对
// 0x24 字节 memset(0) 再调用本 ctor，ctor 内仅默认构造两个 vector——标量不在此
// 清零，与 ORIG 一致。
// ============================================================================
// ============================================================================
// AvatarConvert —— ORIG ctor 0x088b1b0e：map ctor。dtor 0x088b1b22：仅 map dtor
// （不 delete 条目——释放须经 clearScript，ORIG 如此）。
// ============================================================================
#include "AvatarConvertServer/AvatarConvert.inc"
}

#include "AvatarConvertServer/dtor_AvatarConvert.inc"
}

// ============================================================================
// loadScript —— ORIG T 0x88b1b36。"Etc/EmblemRenewal/" + "avatar_convert_jpn.etc"。
// 每条记录：TYPE 头 → grade=ScanInt、flag=(ScanInt==1) → 循环 ScanTypeOrInt 取
// 数值键 + 2×short（push GetInfo{key,idx,cnt}，累计 cnt 入 m_cumWeights）→
// 因 TYPE 退出时查重插入 m_convertMap（total=m_cumWeights.back()），因 EOF/非法
// 退出则整条丢弃。重复 grade：printf("%s duplicate grade:%d") 后返回 -2
// （ORIG 此路径不释放 cr——忠实保留）。
// 返回 0=成功 / -1=打开失败 / -2=重复 grade。
// ============================================================================
// [b28d-solo-AvatarConvertServer] NEAR（ORIG 0x088b1b36，190 vs 190，助记符序列全等，
// p167 CLEAR 25/25）。修法：P299/P318 value-init（删 ConvertResult 用户 ctor/dtor ⇒
// `new ConvertResult()` 前置零初始化，消原 operator new+memset+placement new 的 N83）+
// P164 循环头形（for(;;)+if(!ScanType) break，复现 4.1 入口直落条件头）+
// P0 去 key 具名局部（info.m_key 直收 ScanTypeOrInt 返回值）+ P220 正极性 find==end +
// field20 具名临时（复现 ORIG -0x1c 比较前落槽）。
// 残差：槽号整体浅 4（块内 endIt 的 end() sret -0x48 vs ORIG -0x34；试函数头先声明后
// 赋值反而劣化 195 条 ⇒ 回退）⇒ N82/P112 槽分配族，语义完成。
#include "AvatarConvertServer/loadScript.inc"
}

// ============================================================================
// clearScript —— ORIG T 0x88b1dc2：begin/end 各取一次，遍历 delete ConvertResult*
// （delete 表达式自带判空分支，与 ORIG 的 test→D1→operator delete 形态一致）
// → map.clear()。
// ============================================================================
#include "AvatarConvertServer/clearScript.inc"
}

// getConvertResult —— ORIG T 0x88b1e48：先默认构造迭代器再赋值 find 结果；
// 命中返回指针，未命中 0。
#include "AvatarConvertServer/getConvertResult.inc"
}

}  // namespace SCRIPT
}  // namespace ARAD

// ============================================================================
// AvatarConvertServer —— ORIG C1 0x081921b6 / D1 0x081921ec。基类口径同
// EmblemCompoundServer.cpp 注释（Singleton W 0x0819380a 未建模）。
// ============================================================================
#include "AvatarConvertServer/AvatarConvertServer.inc"
}

#include "AvatarConvertServer/dtor_AvatarConvertServer.inc"
}

// Load —— ORIG T 0x081930ba：cMyTrace(pretty,0x37f,9,true,false) +
// operator()("\t- Loading Avatar Convert script - ")；loadScript() 非0 → 0。
#include "AvatarConvertServer/Load.inc"
}

// Destroy —— ORIG T 0x0819312c：return 1（基类槽1 void 约定）。
#include "AvatarConvertServer/Destroy.inc"
}

// Test —— ORIG T 0x0819220c：空（非虚成员）。
#include "AvatarConvertServer/Test.inc"
}

// ============================================================================
// LinkClosureB.AradServers 追加段：
//   StatisticsAvatarDisjoint ORIG T 0x08193136（StatisticsAvatarDisjoint.md 转写）
//   StatisticsEmblemCreate  ORIG T 0x081931dc（StatisticsEmblemCreate.md + 汇编，
//     包布局 Packet_Emblem_Create_Statistic：PacketHeader(0x17a4,0x36)+count@0xa
//     +grades@0xe[5]+addInfo@0x22[5]；ORIG packet 类 C1 W @0x08110bdc 归
//     PacketTypes 域，此处按字节布局组装，不重复建模。）
//   CheckCondition          ORIG T 0x08192212（CheckCondition.md 全文转写）
//   Convert                 ORIG T 0x081926de（Convert.md 全文转写）
// ============================================================================

#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CInventory.h"
#include "Inven_Item.h"
#include "CAvatarItemMgr.h"
#include "CDataManager.h"       // G_CDataManager / find_item / ENUM_CMDPACKET
#include "LogManager.h"
#include "CStatisticServerProxy.h"
#include "CServerProxyMgr.h"
#include "GlobalData.h"
#include "AvatarRechargeServer.h"

#include "MsgQueueMgr.h"
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MoreTypes.h"          // arad::SigAradAvatarConvert 完整定义
#include "Packet_Avater_Disjoint_Statistic.h"  // 统计包权威布局（ORIG W C1 0x08110bae）
#include "Packet_Emblem_Create_Statistic.h"    // 统计包权威布局（ORIG W C1 0x08110bdc）
// DB_UpdateAvatarHistory —— ORIG makeRequest T @0x0844536a。
// 权威定义在 CompoundDeathLCB.cpp（LinkClosureB.CompoundDeath 批），
// 此处仅经 DB_UpdateAvatarHistory.h 引用，避免多重定义。
#include "DB_UpdateAvatarHistory.h"

    // [b28d-solo-AvatarConvertServer] AE（ORIG 0x08193136，49/49，p167 CLEAR）。
    // N7 间接 call 目标寄存器（ORIG 0x0819319b `mov (%eax),%edx; call *%edx`）在
    // 4.4.7 下恒 %eax；Main 已按本泳道双后端读数把本 TU 路由到 446 后端
    // （dnf_game_cxx.sh「仅换 446 后端」表），逐操作数一致。P282 已核虚槽 +0xc。
#include "AvatarConvertServer/StatisticsAvatarDisjoint.inc"
}

// [b28d-solo-AvatarConvertServer] DIFF 封顶（ORIG 0x081931dc，115 vs 114，p167 CLEAR）。
// 修法：权威 Packet_Emblem_Create_Statistic C1 W 0x08110bdc（消原 char[0x36]+memset+
// 字节写）+ m_grades/m_values 具名数组（P311/P95）+ 语句位 `it++` 复现 ppEi（探针实证，
// N113 仅适用 for 增量段）+ int i 复现 cmpl $5/jg + 代理内联（消 TU-local helper）。
// 残差（语义完成、byte 层 N 封顶）：① find_item 实参求值序——ORIG 0x0819327b
// `mov -0x14(%ebp),%ebx; call G_CDataManager; mov %ebx,0x4(%esp)`，4.4 恒 receiver 先
// （P75/N82附）② `mov -0x18(%ebp),%ebx; call size; cmp %eax,%ebx`（N22）③ m_values 的
// disp/索引分解 -0x58+4(i+8) vs -0x50+4(i+6)（N94/N4 族）。
#include "AvatarConvertServer/StatisticsEmblemCreate.inc"
}

// [b28d-solo-AvatarConvertServer] DIFF 封顶（ORIG 0x08192212，293 vs 292，p167 CLEAR 35/35）。
// 真语义 bug：成功条件极性整段反置——ORIG 0x08192617 `movzbl 0x20; xor $1; test; je` +
// 0x08192639 `cmp; jb` ⇒ 失败条件 = `!m_field20 && size >= selectNo`；原实现写成
// `m_field20 || size <= selectNo`（条件与返回值双向错）。修法：invR/mgr 内联（P52/P0，
// 消帧 0x54→0x44）+ m_slot 三处使用点 `(unsigned short)` 两段零扩展 + 删 `(int)` 冗余 cast。
// 残差：1 条 N18a——ORIG 0x081923d0 `mov 0x4(%eax),%eax; mov %eax,%ebx`（两段装 callee-saved），
// 4.4 恒折单段 ⇒ N 封顶。
#include "AvatarConvertServer/CheckCondition.inc"
}

// [b28d-solo-AvatarConvertServer] DIFF 封顶（ORIG 0x081926de，610 vs 601，p167 CLEAR
// 85/85，§6.2 超大函数三件套口径）。真语义 bug 3 处：
//   ① `sig->m_resultCount` 取错字段——ORIG 0x081928a5 取 GetInfo+0x4（数量），
//      原实现取 +0x6（权重）；字段名已按 ORIG 语义改 m_count@+0x4 / m_weight@+0x6。
//   ② newItem 漏写 m_addInfo/m_addInfo2——ORIG 0x0819292d 写 +0x2、0x0819294e 写 +0x7
//      （且 m_addInfo2 在 stackable 校验之后赋值）。
//   ③ StatisticsEmblemCreate 实参应复用 items——ORIG 0x0819301d `lea -0x98(%ebp)`
//      （原实现多建 created vector：p167 EXTRA vector<pair> C1/D1/push_back）。
// 修法：invW 内联 5 处（p167 MISSING getCurCharacInvenW×4）+ oldRef 走 const 版
// getCurCharacInvenR/GetInvenRef（MISSING const 两符号）+ GetInfo 逐字段 operator[]（P157，
// MISSING ixEj）+ make_pair 左值实参 + MoreTypes m_resultItemId→unsigned long（MISSING
// make_pairIRmRsE）+ 去 count 局部。
// 残差：N82 槽分配（帧 0x14c vs 0x140，全帧浅移 4）+ N18a（sig->m_itemId 两段装 ebx）+
// edi 保活（cMyTrace 段 ORIG `movzwl %ax,%edi; mov %edi,0xc(%esp)`）+ P168a③b（ORIG
// is_stackable `test %al,%al; je` 无 xor，4.4 恒 xor 物化）⇒ 语义完成、byte 层 N 封顶。
#include "AvatarConvertServer/Convert.inc"
}
