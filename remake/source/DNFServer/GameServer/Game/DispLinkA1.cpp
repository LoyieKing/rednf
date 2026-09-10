// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 DispLinkA1/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// DispLinkA1.cpp — Dispatcher 链接核销批次 A1（独占 TU）
//
// 逐符号忠实转写，报告依据 docs/class_func_reports/：
//   1. Dispatcher_Avatar_Disjoint_Random_Reward::read        0x081de9da (read.md)
//   2. Dispatcher_Avatar_Disjoint_Random_Reward::send        0x081dec56 (send.md)
//   3. Dispatcher_CallGuildCreateRight::send_packet_guildcreateright 0x081ffbde
//   4. Dispatcher_CallGuildInvite::send_packet_guild_invite  0x08207852
//   5. Dispatcher_CallGuildLevelUp::send_packet_guild_levelup 0x08200126
//   6. Dispatcher_Client_Spec_Statistic::check_error         0x081ce75a
//   7. Dispatcher_Client_Spec_Statistic::read                0x081ce5fe
//   8. Dispatcher_CompatibilityIndex::check_error            0x081e23e0
//   9. Dispatcher_Compound_ExtreamItem::_read_packet         0x08225806
//  10. Dispatcher_CreateDisjointStore::check_error           0x081cf8d4
//  11. Dispatcher_DisJointAvatar::SendCreateEmblemInfo       0x08218a36
//  12. Dispatcher_DisJointAvatar::SendDisjointAvatarInfo     0x08218996
//  13. Dispatcher_DisJointAvatar::_getResultItems (const)    0x0821873c
// ============================================================================

#include "Dispatchers.h"

#include <utility>
#include <vector>

#include "Inven_Item.h"
#include "CItem.h"
#include "CStackableItem.h"         // stStackableBoosterItemInfo_t / getBoosterItem
#include "CDataManager.h"           // G_CDataManager / find_item / m_disJoint (+0x1c)
#include "DisJoint.h"               // DisJoint（m_disJoint 指向对象）
#include "CInventory.h"             // MakeItemPacket / GetInvenSlot
#include "CUser.h"                  // Send / get_state / GetParty
#include "CUserCharacInfo.h"        // getCurCharacInvenR / getCurCharacR / GetCurCharacExpertJob*
#include "PacketGuard.h"            // PacketGuard（含 PacketHeader 传递可见）
#include "CompoundEmblemDeps.h"     // LineFunc
#include "CStatisticServerProxy.h"  // SendPacket(char*, int)
#include "CServerProxyMgr.h"        // CServerProxyMgr<T>::GetServerProxy
#include "GlobalData.h"             // GlobalData::s_statistic_proxy_mgr
// [b28c-32] P104 合并：匿名 namespace 内的 Packet_Emblem_Create_Statistic /
// Packet_Avater_Disjoint_Statistic 镜像类删除，统一 include 权威头
// （Avater 局部版仅 0x17 字节 ⇒ 帧 0x34 vs ORIG 0x44、对象落 -0x23 vs -0x34）。
#include "Packet_Emblem_Create_Statistic.h"
#include "Packet_Avater_Disjoint_Statistic.h"

// ============================================================================
// 1. Dispatcher_Avatar_Disjoint_Random_Reward::read  ORIG 0x081de9da
//    LineFunc(0x5041, pretty, 0, 0)
// ============================================================================
#include "DispLinkA1/read.inc"
}

// ============================================================================
// 2. Dispatcher_Avatar_Disjoint_Random_Reward::send  ORIG 0x081dec56
//    拷贝 stAvatarDisjointRandomReward（含 vector 拷贝），按结果码分支发包。
// [b28d-solo-DispLinkA1] verdict=AE（compare_tu clean；p167 25/25 CLEAR）。
//   修法：P96 迭代器改非 const iterator；P13+ORIG 多一次 size() 并以非零包裹
//   循环（setne 物化）；P28 it++（ORIG operator++(int)）。
// ============================================================================
#include "DispLinkA1/send.inc"
}

// ============================================================================
// 3. Dispatcher_CallGuildCreateRight::send_packet_guildcreateright
//    ORIG 0x081ffbde：clear + header(0,0xe) + byte(0) + short(1) +
//    MakeItemPacket(INVEN 1, slot 0) + finalize + Send
// ============================================================================
#include "DispLinkA1/send_packet_guildcreateright.inc"
}

// ============================================================================
// 4. Dispatcher_CallGuildInvite::send_packet_guild_invite  ORIG 0x08207852
//    header(1,0x9a)；inviteType==0 → byte(1)，否则 byte(0)+byte(inviteType)
// ============================================================================
#include "DispLinkA1/send_packet_guild_invite.inc"
}

// ============================================================================
// 5. Dispatcher_CallGuildLevelUp::send_packet_guild_levelup  ORIG 0x08200126
//    同 4 型：header(1,0x4c)
// [b28d-solo-DispLinkA1] verdict=IDENTICAL（58=58，纯重定位标签差）。
// ============================================================================
#include "DispLinkA1/send_packet_guild_levelup.inc"
}

// ============================================================================
// 6. Dispatcher_Client_Spec_Statistic::check_error  ORIG 0x081ce75a
// ============================================================================
#include "DispLinkA1/check_error.inc"
}

// ============================================================================
// 7. Dispatcher_Client_Spec_Statistic::read  ORIG 0x081ce5fe
//    msg+0xd/+0xe/+0xf 三字节，+0xf 为条数；条目步长 0xc：
//    +0x12 byte、+0x16 dword、+0x1a dword（ORIG 裸偏移直读 MSG 体）。
//    b28c-34 判词（identical）：ORIG 寻址为 (p + i*0xc) + K（先 add 基址、后 add $K，
//    0x081ce65e..0x081ce6a5）。4.4 前端会把纯指针形 `p + i*0xc + K` 折叠成
//    `p + (i*0xc + K)`（先 add 常量、后 add 基址，后两条再多 mov %ecx/lea）；
//    经 unsigned long 中转 `(unsigned long)(p + i*0xc) + K` 阻止该折叠，逐条复现 ORIG。
// ============================================================================
#include "DispLinkA1/read_1.inc"
}

// ============================================================================
// 8. Dispatcher_CompatibilityIndex::check_error  ORIG 0x081e23e0（纯 return 0）
// ============================================================================
#include "DispLinkA1/check_error_1.inc"
}

// ============================================================================
// 9. Dispatcher_Compound_ExtreamItem::_read_packet  ORIG 0x08225806
// [b28d-solo-DispLinkA1] verdict=AE（172=172，ndiff=0；p167 12/12 CLEAR）。
//   修法：P0 删具名 slot 局部（ORIG 值留 %edi 跨 GetInvenSlot）；P211 档③
//   字段侧旋钮——m_itemRefs 改 4 字节整型存储（Dispatchers.h），复现 ORIG
//   RHS-first + `mov %eax,%edx` 中转（const CItem*/const void* 两形实测均同型）。
// ============================================================================
#include "DispLinkA1/_read_packet.inc"
}

// ============================================================================
// 10. Dispatcher_CreateDisjointStore::check_error  ORIG 0x081cf8d4
// ============================================================================
#include "DispLinkA1/check_error_2.inc"
}

// ============================================================================
// 11. Dispatcher_DisJointAvatar::SendCreateEmblemInfo  ORIG 0x08218a36
//     条数<6 才发；每对 (itemIdx, value)：find_item 失败即整体放弃；
//     slots[i+1]=素材 grade、slots[i+6]=附加值；末写 slots[0]=条数。
// [b28d-solo-DispLinkA1] verdict=DIFF N 封顶（98 vs 97，ndiff=13；p167 14/14 CLEAR）。
//   已修：P28 it++（operator++(int)）、P167 内联 GetServerProxy（删匿名 helper）、
//   P0 具名 itemIdx/value 槽、P13 end 缓存、P196/P218 负条件 setne/jne、
//   P239 begin→end→count=0 发射序、P311 具名数组成员视图（m_grades 已归正）。
//   残余 N：①N7 实参先升格 %ebx 跨 receiver call（P226 语句表达式/裸形均不翻，
//   4.4 恒先 call 后装载）；②N94 m_values 常量拆分（ORIG add $8+disp -0x58，
//   4.4 lea 0x4+disp -0x48，地址等价）；③N18b 尾比较（ORIG mov count,%ebx;
//   call size; cmp %eax,%ebx，4.4 恒 cmp mem,%eax 内存直比）。
// ============================================================================
#include "DispLinkA1/SendCreateEmblemInfo.inc"
}

// ============================================================================
// 12. Dispatcher_DisJointAvatar::SendDisjointAvatarInfo  ORIG 0x08218996
// ============================================================================
// [b28c-32] verdict=NEAR：P104 合并镜像类为权威头（0x17→0x28）；余 N7（47=47）。
#include "DispLinkA1/SendDisjointAvatarInfo.inc"
}

// ============================================================================
// 13. Dispatcher_DisJointAvatar::_getResultItems (const)  ORIG 0x0821873c
//     DisJoint 表选素材：jewelTypeIndex==-1 → [grade][type] 区（+0x444 起），
//     否则按 job 遍历定位（+0xd4 起）；素材须为 stackable(0x10)，收集
//     getBoosterItem 条目 (m_itemIdx, m_addInfo)。
// [b28d-solo-DispLinkA1] verdict=DIFF N 封顶（186 vs 190，ndiff=52；p167 23/23 CLEAR）。
//   已修：P315/P167 字段 m_itemIdx int→unsigned int（CStackableItem.h）⇒
//   make_pair<unsigned int const&,unsigned short const&> 与
//   pair<int,int>::pair<unsigned,unsigned short> 实例、2 次 iterator→const_iterator
//   转换构造、ne<const*,const*> 全部归位；P47/P222 if/else 反转（je 落表查找块）；
//   P0 去 dj/grade/type 具名槽；P231 纯 register dj（非 asm 绑定）消槽。
//   残余 N：4.4 算术 lowering/寄存器角色差（N86/N87/N7 族）——ORIG dj→%esi 且
//   job 先于 get_grade 装载、grade*5 用 edx 累加、`add $0x34; mov 0x4(%esi,%eax,4)`
//   单条 scaled（成员数组视图缺 +4 表建模）；4.4 恒 dj→%ebx、job 后装、指针下标
//   `[i+K]` 折 shl+lea。P226/裸 register/基址+4 三形实测均不翻。
// ============================================================================
#include "DispLinkA1/_getResultItems.inc"
}
