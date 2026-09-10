// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CompoundDeathLCB/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// CompoundDeathLCB.cpp — WongWork::CCompoundAvatar 合成核心四方法 +
//                        WongWork::CDeathTowerRanking 记录 packet（LinkClosureB）
//
// 全部实现逐段转写自 docs/class_func_reports/{WongWork__CCompoundAvatar,
// WongWork__CDeathTowerRanking}/ 报告汇编+反编译 C：
//   WongWork::CCompoundAvatar
//     _MakeCompoundInfo            T 0x08331efe（sret 返回 tagCompoundInfo）
//     _CheckCondition              T 0x0833260c（const）
//     _ProcCompoundCore            T 0x08332ee8
//     _SendDisjointAvatarStatistic T 0x08333ecc
//     _generateIPGNo               T 0x08333df6（const；闭链 helper，报告同目录）
//   WongWork::CDeathTowerRanking
//     makeMyBestRecordPacket       T 0x08468b8e
//     makeTopRankerRecordPacket    T 0x08468c68
//     _makeRecordPacket            T 0x08468cb6（前两者共用的序列化 helper）
// 同 TU 附加直接依赖 producer（报告完整、当前全源无定义）：
//   getItemIndex(const std::vector<std::pair<int,int>>&, int) T 0x08332e4e
//   Packet_Avater_Disjoint_Statistic C1 W 0x08110bae（内嵌 PacketHeader W 0x08110b70；
//     权威布局已归口 Packet_Avater_Disjoint_Statistic.h，本 TU 仅 include）
//   DB_UpdateAvatarHistory::makeRequest(unsigned int,int,char*) T 0x0844536a
// ============================================================================

#include "CCompoundAvatar.h"
#include "CDeathTowerRanking.h"
#include "CInventory.h"         // CInventory 完整类型 + OS_API::snprintf
#include "PacketGuard.h"        // PacketGuard 完整类型（CDeathTowerRanking.h 仅前向）
#include "CAvatarItemMgr.h"      // IsTempKey/GetRemainDate/GetExpireDate/UnRegistItem
#include "CUser.h"               // CUserCharacInfo getter/getCurCharacInven*
#include "CEnvironment.h"        // G_CEnvironment()/m_serverEnvir（_generateIPGNo 通道号）
#include "GlobalData.h"          // s_systemTime_/s_statistic_proxy_mgr/s_stream_pool/s_msgq_mgr
#include "CSystemTime.h"
#include "MsgQueueMgr.h"        // MsgQueueMgr::put / QUEUE_IDX
#include "CServerProxyMgr.h"     // CServerProxyMgr<CStatisticServerProxy> 完整模板
#include "CStatisticServerProxy.h"
#include "CStreamGuard.h"
#include "GameTypes.h"          // cMyTrace
#include "CDataManager.h"        // G_CDataManager()/find_item
#include "PvPTypes.h"            // CMTRand（m_rand 完整类型）
#include "GameRand.h"            // get_rand_int
#include "DB_UpdateAvatarHistory.h"
#include "Packet_Avater_Disjoint_Statistic.h"  // 统计包权威布局（本 TU 原重复定义已迁除）
#include <cstring>
#include <ctime>
#include <utility>
#include <vector>


// ---------------------------------------------------------------------------
// getItemIndex —— ORIG T 0x08332e4e _Z12getItemIndexRKSt6vectorISt4pairIiiESaIS1_EEi。
// 全局函数；唯一定义已在 HeroMissionLink4.cpp:257（签名/mangled 与 ORIG 一致），
// 此处仅声明复用，避免重复符号。语义：返回向量中第一个 second > bound 的
// 元素 first；遍历完仍无 → 0。
// ---------------------------------------------------------------------------
#include "CompoundDeathLCB/getItemIndex.inc"
#include "CompoundDeathLCB/makeRequest.inc"
}

namespace WongWork {

// ---------------------------------------------------------------------------
// ORIG CItem 虚表槽直调 helper。
// ORIG CItem vtable(.rodata 0x08c96da0) 槽序实证：D1/D2、D0、纯虚区；
//   +0x0c=子类型索引（派生 _ZNK10CEquipItem12get_sub_typeEv）、
//   +0x10=IsAvatarItem(bool)、+0x24=getUsableItemType(int)(bool)。
// 当前 CItem.h 虚函数声明序与该权威槽序的审计差异尚未收敛（AuditCItemVtable
// 域负责）；为保证本 TU 与 ORIG 虚分派逐指令等价，经槽偏移显式调用，
// 全局虚表收敛后应替换回语义虚调用。
//
// 必须是**宏**而非 static helper：ORIG 的虚取址序列
//   mov (obj),%eax ; add $slot,%eax ; mov (%eax),%edx ; mov obj,%eax ;
//   mov %eax,(%esp) ; call *%edx
// （7 insn/处）位于调用者帧内；提成 helper 后调用者只剩 2 insn，本 TU
// _CheckCondition 实测 10 处虚调用 ⇒ ~50 insn 缺口（P183 同类物化口径，
// P167 差集 ORIG indirect=10 / ours=0 实证）。
// ---------------------------------------------------------------------------
#define CI_VCALL0(ret, slot, obj)                                        \
    reinterpret_cast<ret (*)(const void*)>(                              \
        *reinterpret_cast<const int*>(                                   \
            *reinterpret_cast<const void* const*>(obj) + (slot)))(obj)
#define CI_VCALL1(ret, slot, obj, a1)                                    \
    reinterpret_cast<ret (*)(const void*, int)>(                         \
        *reinterpret_cast<const int*>(                                   \
            *reinterpret_cast<const void* const*>(obj) + (slot)))(obj, (a1))

#define CI_SubType(item)       CI_VCALL0(int, 0x0c, item)
#define CI_IsAvatarItem(item)  CI_VCALL0(bool, 0x10, item)
#define CI_UsableType1(item)   CI_VCALL1(bool, 0x24, item, 1)

// ============================ _MakeCompoundInfo ============================

// ORIG 0x08331efe（sret 返回 tagCompoundInfo）。步骤：
//   1. tagCompoundInfo 构造（0x08334344）+ clear()（0x0833424a）；
//   2. 回填 msg 定长字段（MSG_COMPOUND_AVATAR.h 权威布局，具名字段读取）；
//   3. **三次独立** getCurCharacInvenR → GetInvenSlot(1|2, (short)msg+0xd/0xf/0x15)
//      → 临时槽（-0x88）→ 15×movl + 1×movb 逐字段拷进 info 的
//      m_avatar(+0x92)/m_materialA(+0x0a)/m_materialB(+0x4a)；
//   4. find_item 两脚本指针（各自重取 G_CDataManager）；
//   5. 双脚本有效且 msg 两 itemIdx 与槽内 addInfo 一致时，job 0..10 匹配循环
//      定 info.m_field00；再按表长遍历 rec，对 msg->m_type 选中的 **那条**
//      tail 向量在 case 分支内各自完整展开一遍 begin/end/!=/++/-> 循环
//      （ORIG 物理展开 4 组，非提公共指针后单循环 —— N-C/P165），
//      命中 first==m_avatar.addInfo 则 found=idx 并终止双层。
//   6. 落 info.m_field04(+0x6cd)/m_type(+0x6d9)。
//
// 【batch-12 验收标注（§6.2 超大函数口径）】o=523 n=537（gap 114 → 14）。
//   ① 调用点差集（P167）：ORIG direct 40 ↔ ours direct 40，**归零**（残
//      missing=2/extra=2 仅为 tagCompoundInfo C2/D2 ↔ 4.4 发射 C1/D1 的
//      同体符号名对称差）；
//   ② 补全的三形：
//      a) `info.x = inv->GetInvenSlot(...)` 折叠 sret 目标 ⇒ 改「Inven_Item
//         临时 + 赋值」两步形，恢复 3×（getCurCharacInvenR + 15×movl+1×movb）；
//      b) 遍历提公共 `opts` 指针 ⇒ 只发 1 组迭代器调用，ORIG 是 4 组物理展开
//         （begin/end/operator!=/operator++/operator-> 各 ×4）⇒ case 内各自成组，
//         且迭代器**声明即初始化**（ORIG 该区无 normal_iterator 默认构造）；
//      c) found 经 `cand` 变量在 while 体内传递（ORIG 用条件里的 `uVar8 = idx`
//         逗号项），避免第二次 operator!= 调用；
//      d) job 匹配循环改「相等判定 + 命名 bool bothUsable」以复现 4.1 的
//         `mov$1/mov$0/test` 汇聚岛（三条独立 continue 少 6 insn）。
//   ③ 余 14 insn 逐块核对为：每字段拷贝前 ORIG 重发 `mov %ebx,%eax`（this 重载，
//      4.4 寄存器缓存不重发，6 处）、job 段 `jmp/nop` 中继岛、末尾对齐 nop
//      （§6.1 豁免）⇒ 判**语义完成、byte 层 N76/N82/N18a 封顶**，勿再耗轮次。
// 【b28d-solo-CompoundDeathLCB 定案】达成 **AE**：523/523 逐条全等（clean
//   519=519，仅 .o 重定位标签伪差，P264）。本轮修法：①三步 GetInvenSlot 临时改
//   `*&expr` 匿名临时 ⇒ 4.4 复用同一槽（帧 0x11c→0x8c=ORIG，槽位集全等）；
//   ②`m_tables[f00]` 每迭代重取（P183 禁缓存）；③循环内 `found=idx; break;` 直赋
//   （删 cand×4）；④job 段 `!=` 否定早返 + P226 语句表达式（消 bothUsable 槽）；
//   ⑤switch→if-else 链（P17）；⑥每分支尾各一份 `if(found)break`（ORIG 各一份）；
//   ⑦删冗余 (int) cast（P41/P254）＋索引 `static_cast<unsigned int>` 触发 4.4
//   中转（P211 异型化，消最后 2 条）。
#include "CompoundDeathLCB/_MakeCompoundInfo.inc"
}

// ============================== _CheckCondition =============================

// ORIG 0x0833260c（const）。错误码：1 材料异常/条目缺失、0x11 通用条件失败、
// 0x13 有效期类别不一致、0x16 缺事件材料、0x17 组合约束失败、0x18 剩余天数过短；
// 0 通过。
//
// 【batch-11 验收标注（§6.2 口径）】orig 611 / ours 616（gap 5）。
//   ① 调用点差集：ORIG direct 52 + indirect 10 ↔ ours direct 52 + indirect 10，
//      **归零**（仅 __normal_iterator C2×2 ↔ 4.4 发射 C1×2 的同体符号名对称差）；
//   ② 逐块补齐：CI_* 由 static helper 改宏内联（虚取址 7 insn 回到本帧，
//      indirect 0→10）、补 ORIG 0x08332792 的 `m_field04 == 0 → 0x17` 漏判、
//      getCurCharacInvenR ×7 / GetAvatarItemMgrR ×3 / get_grade ×6 /
//      get_charac_job ×2 全部改为逐点重取、迭代器改「默认构造 + 分支内
//      begin/end + 循环后 ==」形、opts 下标不再命名局部（gap 156 → 5）；
//   ③ 残余 5 insn 为 4.1 的 `cmp $imm; sete; test` 布尔物化岛与统一出口
//      jmp/nop 布局（N3/N76/setcc-fold 族）⇒ 判**语义完成、byte 层封顶**。
// 物化口径（P183/P167 实证）：ORIG 对每个判据都重发完整取数链 ——
//   getCurCharacInvenR ×7（0x08332c27/0c6a/0cad/0cee + 日期段 0x0723/0x0752/0x0781/0x07ab）
//   GetAvatarItemMgrR ×3、get_grade ×6、get_charac_job ×2、
//   两个 vector 迭代器各显式构造（normal_iterator C2 ×2）+ begin/end ×2 + 循环后 ==。
// 重建端禁缓存（一处缓存即整条链消失，本函数 gap 156 的主体即此）。
#include "CompoundDeathLCB/_CheckCondition.inc"
}

// ============================== _ProcCompoundCore ===========================

// ORIG 0x08332ee8。结果选择（按 rec 内三组权重池抽样）→ 删除两件材料与事件
// 材料 → 注销两 uid 并 push 结果三元组 → AddAvatarItem 入包 → 回填 DB 历史。
//
// 【batch-16 验收标注（§6.2 口径）】orig 653 / ours 635（gap 77 → 18）。
//   ① 调用点差集 **归零**：direct 61 ↔ 61、indirect 2 ↔ 2（残 missing=1/extra=1
//      仅 cMyTrace C2 ↔ C1 同体符号名对称差）；
//   ② 补齐的真实缺口（P167 差集定位）：
//      a) 事件材料整单删除由「switch 提公共 materials 指针 + 单调用」改为
//         4 个 if-else 分支**各自完整展开**（ORIG 每支独立 delete_event_items +
//         vector::size + 独立 0x16 出口）⇒ 恢复 delete_event_items ×3、size ×2；
//         type4 的 size 比较对象仍是 tailC(+0x2d0)【ORIG 怪癖，已保留】；
//      b) 两段 GetRemainDate 各自重取 getCurCharacInvenR → GetAvatarItemMgrR
//         （不缓存 mgr）；c) AddAvatarItem 走重取的 getCurCharacInvenW。
//   ③ 余 18 为 N82 帧号重排下的块序对齐伪影（difflib 显示 -46/-32/-5 与总量
//      -18 自相矛盾，而调用点差集已归零）⇒ 判语义完成、byte 层封顶。
#include "CompoundDeathLCB/_ProcCompoundCore.inc"
}

// =========================== _SendDisjointAvatarStatistic ===================

// ORIG 0x08333ecc。统计服上报本次分离的两件头像分级；注意 B 组 subType 也读
// scriptA 的 vtbl+0xc（ORIG 如此）。
// 【b28d-solo-CompoundDeathLCB 判词】NEAR（79/79 逐条全等；帧 `push %ebx;
//   sub $0x54`、全部槽号、`lea -0x3c(%ebp),%ebx` 与 ORIG 全等）。修法：
//   ①两组分级复用同一 gradeIdx 槽（P160，两个独立局部会各占一槽 +4 帧）；
//   ②proxy 具名局部改单语句链式调用（P52，消槽且 &packet 升格 ebx）。
//   残余 = CI_SubType 虚调用目标寄存器 N7 ×2（4.4.7 恒 %eax / ORIG %edx）。
#include "CompoundDeathLCB/_SendDisjointAvatarStatistic.inc"
}

// ================================ _generateIPGNo ============================

// ORIG T 0x08333df6（const）。"C%02d%02d%02d%02d%010d" 五元组。
#include "CompoundDeathLCB/_generateIPGNo.inc"
}

}  // namespace WongWork

// ======================== CDeathTowerRanking packets ========================

namespace WongWork {

// ORIG T 0x08468cb6。每名成员：strlen 名字(int)+名字串、两个槽位 byte
// （stBestRecord_t +0x80/+0x84 即 m_pad7c[4+i]/m_pad7c[8+i]）、层号 short、
// 通关耗时 int、名次 int。
// 【batch-12 标注】o=89 n=88（gap 10 → 1）：strlen 两次独立调用已补齐（P48）；余 1
//   为 ORIG 尾 `call leave ret` 前的 mov + 对齐 nop（§6.1 豁免）⇒ N 封顶。
#include "CompoundDeathLCB/_makeRecordPacket.inc"
}

// ORIG T 0x08468b8e。查 best 表：命中 put_byte(1)+纪录；否则 put_byte(0)。
// 【切片转写】0x08468b9e `call _getBestRecordTable`（不内联算址）；find/end 结果
// 各经 _Hashtable_iterator → _Hashtable_const_iterator **转换构造 2 次**
// （0x08468bc5/0x08468bfb），比较用 const_iterator::operator==、取值用
// const_iterator::operator-> ⇒ 必须声明 const_iterator 并写 `==`（iterator +
// `!=` 会走 operator!= 并省掉两次转换 ctor，实测差 5 call / 9 insn）。
// 【batch-12 标注】o=63 n=61（gap 9 → 2）：调用点差集 10/10 归零（残差仅
//   _Hashtable_const_iterator 转换 ctor 的 C2↔C1 符号名）；余 2 = 尾部 nop/装载
//   步序（§6.1/N82）⇒ N 封顶。
#include "CompoundDeathLCB/makeMyBestRecordPacket.inc"
}

// ORIG T 0x08468c68。该塔 5 个榜首槽顺序序列化（基址 tower*0x390，stride 0x98）。
#include "CompoundDeathLCB/makeTopRankerRecordPacket.inc"
}

}  // namespace WongWork
