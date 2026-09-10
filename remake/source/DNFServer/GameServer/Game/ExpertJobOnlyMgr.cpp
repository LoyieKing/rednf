// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 ExpertJobOnlyMgr/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// ExpertJobOnlyMgr.cpp — expert_job OnlyOrig 清零批（EJMgrSide 批次）
//
// 主符号（ORIG nm -S / objdump 核对，逐份报告完整读后忠实转写）：
//   expert_job::CAlchemist
//     UseDelilah                    T 0x085d18ae  UseRevivalItem       T 0x085d1694
//     GetAssaultPrevent(CUser*,b*)  T 0x085d137c  OnUseStackableItem   T 0x085d1712
//     GetQuestItemDropRate          T 0x085d12ee  CheckUsableStackable T 0x085d18e2
//   expert_job::CDisjointer
//     OnEnterDisjointStore          T 0x085d309a  OnRepairDisjointMachine T 0x085d35ce
//     OnUpgradeDisjointMachine      T 0x085d37e8
//   expert_job::CDollController 生命周期 C1/C2 0x085d45b4、D1/D2 0x085d45d0、
//     D0 0x085d4600（ORIG vtable 四业务槽全 __cxa_pure_virtual → 抽象类，
//     ctor 仅置 vptr、dtor 仅置 vptr 后链基类析构；由类定义自然生成）
//   expert_job::CExpertJobMgr
//     OnLeaveUser T 0x0849e214  OnRequestPeer  T 0x0849e256
//     OnResponsePeer T 0x0849e362  IsAvailableRequestPeer T 0x0849e55e
//     OnEnchantByBead T 0x0849ed1a  OnGiveup  T 0x0849e60c
//   expert_job::CCharacExpertJob
//     SendExpertJobInfo             T 0x0832b454
//     set_disjoint_machine_grade    W 0x85d3e9a【objdump】
//   expert_job::CDisjointMachine::include(village_object::IObject const&) const
//                                   W 0x85d3de6【objdump】
//
// SKIP（已有唯一 owner）：CExpertJobMgr::AcquireExpertJob → ExpertJobLink4.cpp
//
// 阻塞依赖（见 yield 清单；调用点以 BLOCKED 注释精确保留）：
//   OnEnchantByBead: CStackableItem::GetMonsterCardId W 0x849f530 /
//     InstanceRentalSystem::CheckRentableItem(int,unsigned) T 0x82702dc /
//     GetInstanceExpertJobStatistic T 0x860f940 + increaseItemCount T 0x860f9d4 /
//     cUserHistoryLog::EnchantItem(int,int,int,bool,const char*,const char*)
//   OnUseStackableItem/CheckUsableStackableItem: CStackableItem::get_sub_type
//     W 0x0822c9d2（头缺声明）
// ============================================================================

#include "CExpertJob.h"

#include "CBattle_Field.h"     // CBattle_Field::SetAssignNPCByPotion（UseDelilah）
#include "CDataManager.h"      // G_CDataManager / GetExpertJobScript / find_item
#include "CParty.h"            // GetParty/get_user（+0xb24 内嵌战场区）
#include "CPrivateStoreMgr.h"  // private_store 单例 / IsBusyPrivateStore
#include "CStackableItem.h"    // m_int2d0（APC 药水持续秒）
#include "CSystemTime.h"       // CSystemTime::getCurSec
#include "InstanceRentalSystem.h"  // CheckRentableItem（OnEnchantByBead 租赁双查）
#include "GameTypes.h"         // cMyTrace
#include "GlobalData.h"        // s_systemTime_
#include "PacketGuard.h"
#include "SkillSlot.h"      // SkillSlot::delete_skill
#include "GameWorld.h"      // G_GameWorld / send_all

// CExpertJobList.cpp 定义的专家炼金信息表（B 0x094f7778，8 字节；
// [1] 即 ORIG 高半区 _DAT_094f777c）。
extern int g_alchemistInfo[2];

namespace expert_job
{

namespace
{

// 自身扩展数据槽（charac_expand 槽 0）快捷读取。b28d-solo：P49 必须用**宏**——
// -O0 下 static inline helper 会发射真实 call 符号（ORIG 无该符号，属发明符号），
// 且每个使用点须独立重发 GetCharacExpandData 调用（P48，ORIG 逐处 call 实证）。
#define GetSelfExpand(user) \
    reinterpret_cast<CCharacExpertJob*>((user)->GetCharacExpandData( \
        (ENUM_CHARAC_EXPAND_TYPE)0))

}  // namespace

// ===========================================================================
// expert_job::CAlchemist
// ===========================================================================

#include "ExpertJobOnlyMgr/CDollController.inc"
}

// ---- UseDelilah（报告 UseDelilah.md，ORIG T 0x085d18ae sz 0x33）----
#include "ExpertJobOnlyMgr/UseDelilah.inc"
}

// ---- UseRevivalItem（报告 UseRevivalItem.md，ORIG T 0x085d1694 sz 0x7d）----
// b28d-solo-ExpertJobOnlyMgr：AE（near 1→0，47 件 identical/AE）。
// ORIG 0x085d1694 多实参 call 首值 callee-saved 角色 = %esi（GetRevivalTime）/
// %ebx（GetUID）；4.4.7 对裸内联实参恒反向（首值 %ebx）。修法 = P231 裸
// `register int` 局部（非 asm 绑定，P0 合规）：两值以 register 提示升格
// callee-saved 且不落槽，分配偏好翻正为 ORIG 的 esi/ebx 序。
#include "ExpertJobOnlyMgr/UseRevivalItem.inc"
}

// ---- GetQuestItemDropRate（报告，ORIG T 0x085d12ee sz 0x71）----
#include "ExpertJobOnlyMgr/GetQuestItemDropRate.inc"
}

// ---- CheckUsableStackableItem（报告，ORIG T 0x085d18e2 sz 0x96）----
// [IG1 2026-09-10 S2 授权] bool 化：ORIG 调用点 xor $1/test/je 直证返回 bool
// （P248 档①/P292；返回类型不入 mangled、不改布局；定义侧机器码/尺寸 0x97 实测不变）。
#include "ExpertJobOnlyMgr/CheckUsableStackableItem.inc"
}

// ---- GetAssaultPrevent（报告，ORIG T 0x085d137c sz 0xa7）----
// b28c-1 AE: P0/N96 清绑 + P223 双岛（0x085d140a/0x085d140d）后 0 差。
#include "ExpertJobOnlyMgr/GetAssaultPrevent.inc"
}

// ---- OnUseStackableItem（报告，ORIG T 0x085d1712 sz 0x19b）----
// b28c-1 N7: 虚调用 callee %eax↔%edx 互换。
#include "ExpertJobOnlyMgr/OnUseStackableItem.inc"
}

}  // namespace expert_job

// ===========================================================================
// expert_job::CDisjointer（分解术士三接口）
// ===========================================================================

namespace expert_job
{

// ---- OnEnterDisjointStore（报告，ORIG T 0x085d309a sz 0x101）----
// 包 0xf5：byte(1) byte(等级+1) int(修理费) int(耐久)。
#include "ExpertJobOnlyMgr/OnEnterDisjointStore.inc"
}

// ---- OnRepairDisjointMachine（报告，ORIG T 0x085d35ce sz 0x218）----
// 分支 A：现价修理；分支 B（钱不够）：GetDisjointMachineRepair 以全部现金
// 换算可修值并回写现价。新耐久 <1 → 错误 0x16。
#include "ExpertJobOnlyMgr/OnRepairDisjointMachine.inc"
}

// ---- OnUpgradeDisjointMachine（报告，ORIG T 0x085d37e8 sz 0x2e2）----
// b28d-solo-ExpertJobOnlyMgr：N 封顶（块序 N93附/N121 类）+ 真语义修复。
// 修复：①P49 宏化 GetSelfExpand（原 static inline 发射发明符号
//   _ZN10expert_job12_GLOBAL__N_113GetSelfExpandEP5CUser，ORIG 无）⇒ 每使用点
//   独立重发 GetCharacExpandData 调用（P48）；②**真 bug** put_int(grade + 1)
//   （ORIG 0x085d3a2e `mov -0x18(%ebp),%eax; add $0x1,%eax`，原实现漏 +1）；
//   ③keyByte unsigned char + find 直传其地址（无 cast 临时）+ 两次独立
//   GetEtcScript() + 非 const iterator（P48/P96/P270）；④upgradeCost<0||==0
//   双独立守卫（P133①）；⑤比较方向 `it->second > level`（ORIG 先算 RHS）。
// 残差：it==end 的 0x13 错误块物理位置（ORIG 0x085d38ee 紧跟 0xe 块后，4.4 恒
// 置于函数尾 fall-through；202 vs 201，差 1 条 jmp）。else-if/正向包裹两形互试
// ⇒ 正向包裹最优（identical 20 vs 19）。p167 调用点 38/38 CLEAR、指令集合与
// 槽位集全等 ⇒ 语义完成、块序 N 封顶。
#include "ExpertJobOnlyMgr/OnUpgradeDisjointMachine.inc"
}

}  // namespace expert_job

// ===========================================================================
// expert_job::CDollController —— 抽象派生（ORIG vtable @0x8cc08e0：4 业务槽
// 全 __cxa_pure_virtual + D1/D0）。ctor 仅基类构造+vptr 置位，dtor 链基类析构，
// 由类定义自然生成；完整类布局建议见 decl 文件。
// ===========================================================================

namespace expert_job
{

#include "ExpertJobOnlyMgr/dtor_CDollController.inc"
}

}  // namespace expert_job

// ===========================================================================
// expert_job::CExpertJobMgr
// ===========================================================================

namespace expert_job
{

// ---- OnLeaveUser（报告，ORIG T 0x0849e214 sz 0x41）----
// b28c-1 N7: 虚调用 callee %ecx↔%edx 互换。
#include "ExpertJobOnlyMgr/OnLeaveUser.inc"
}

// ---- IsAvailableRequestPeer（报告，ORIG T 0x0849e55e sz 0xad）----
#include "ExpertJobOnlyMgr/IsAvailableRequestPeer.inc"
}

// ---- OnRequestPeer（报告，ORIG T 0x0849e256 sz 0x10b）----
// b28c-1 N-88: 槽比零 setne 物化（具名 bool 实测 2→11 回退）。
#include "ExpertJobOnlyMgr/OnRequestPeer.inc"
}

// ---- OnResponsePeer（报告，ORIG T 0x0849e362 sz 0x1fb）----
// b28d-solo-ExpertJobOnlyMgr：AE（DIFF→AE，48 件 identical/AE）。
// 两处修法：①**真语义 bug**——首包 put_header 前多一次 packet.clear()，
// ORIG 全函数仅 0x0849e4ce 一次 clear（首包 Send 之后）⇒ 删除（P171 照抄）；
// ②P277 派生 cast 前置到调用点
//（`static_cast<CEnchanter*>(enchanter)->SetPeerRequestType(type)`）复现 ORIG
// 0x0849e43b 段 this-first 装载（`mov -0xc,%eax; mov 0x20,%edx; …`）；
// 虚调用 OnStartMission 保持基类 CExpertJob* 形（槽 0）。
#include "ExpertJobOnlyMgr/OnResponsePeer.inc"
}

// ---- OnEnchantByBead（报告，ORIG T 0x0849ed1a sz 0x474）----
// b28d-solo-ExpertJobOnlyMgr：N 封顶（N82附/N115/N7）+ 真语义修复。
// 修复：①**真 bug**——统计调用漏第二次 `masterItem->get_index()`
//   （ORIG 0x0849f076 为第二次独立调用，P48；p167 修前 MISSING 1 → 修后 46/46
//   CLEAR）；②m_fieldd 写入时机提前到发包前（ORIG 0x0849f288）且 EnchantItem
//   第 3 参**重读** ringRef->m_fieldd（无 replacedCardIdx 具名局部，P0/P48）；
//   ③cMyTrace 改 `__PRETTY_FUNCTION__` + 全表达式临时链（P118/P139/P227）。
// 残差：帧 0x60 vs ORIG 0x6c（4.4 带 call 帧恒 16k 量化，N115/P190②不可凑）+
// 缺 push %edi（ORIG 用 %edi 存 &user->m_historyLog，4.4 用 %esi）⇒ 全帧槽号
// 整体差 0x10；callee-saved 角色互换（N7/N82附）。指令集合/调用点全等。
#include "ExpertJobOnlyMgr/OnEnchantByBead.inc"
}

// ---- OnGiveup（报告，ORIG T 0x0849e60c sz 0x70d）----
// b28d-solo-ExpertJobOnlyMgr：N 封顶（N82附/P75 求值序 + 帧槽差）+ 真结构还原。
// 还原：①删除 TU-local SendGiveupError helper（**发明符号**）⇒ 6 处
//   `throw (ENUM_ERROR)<code>` + `catch (ENUM_ERROR err){ SendCmdErrorPacket
//   (0xf3,(uchar)err); }`（P6/P229；p167 修前 MISSING __cxa_throw×6/
//   __cxa_begin_catch/__cxa_end_catch/__cxa_allocate_exception×6/GetCurCharac
//   ExpertJobType/getCurCharacQuestR/vector<pair>::operator[]/… + EXTRA
//   SendGiveupError×5 → 修后 95/95 CLEAR）；②补 jobType==0 独立守卫（ORIG
//   0x0849e6d0，第二次独立调用取 GetExpertJobScript 实参）；③get_quest_info
//   receiver 改 getCurCharacQuestR（ORIG 实证，非 QuestW）；④m_growSkillVec
//   每轮**两次** operator[] + 两次 getCurCharacSkillW（P48）；⑤删具名 inven
//   局部（P0，getCurCharacInvenW() 结果不落槽）；⑥两循环改 P283 goto 尾标签形
//   （ORIG begin/end 后无条件 jmp 底部条件块，for 恒条件置顶）。
// 残差：4.4 实参右→左求值序 vs ORIG 左→右（skillIdx 先于 get_charac_job，
// P75/N82附）、帧 0x50 vs ORIG 0x60、PacketGuard 槽 -0x34 vs -0x30；
// 指令数 460 vs 458，调用点 95/95 CLEAR，语义完成。
#include "ExpertJobOnlyMgr/OnGiveup.inc"
}

}  // namespace expert_job

// ===========================================================================
// expert_job::CCharacExpertJob 补充
// ===========================================================================

namespace expert_job
{

// ---- SendExpertJobInfo（报告，ORIG T 0x0832b454 sz 0x201）----
// b28d-solo-ExpertJobOnlyMgr：N 封顶（P197④/N-88 族，语义等价）。
// ORIG 0x0832b572 `cmpl $0,-0xc; sete %al; test %al,%al; jne <continue 岛>` 系
// 4.1.2 槽比零 setcc 物化；4.4.7 对 if/while 条件恒折直跳 `je`（-3 条）。
// 本波实测旋钮：①P194 具名 bool（落槽 -0x9 ⇒ 全帧重排，劣化）；
// ②P225 while+break（产镜像 setne/je 且少 1 岛，劣化）；③P226 语句表达式
//（回退基线 je）；④内联比较 = 双调用真 bug（P197④ 禁）。其余逐条全等：
// switch(1,2,3,4) 决策树、三 continue 岛（nop;jmp;nop;jmp;nop）、槽位集
//（-0x18/-0x28/-0x14/-0xc/-0x10）与全部调用点。唯一残差 = 该 3 条 setcc 物化。
#include "ExpertJobOnlyMgr/SendExpertJobInfo.inc"
}

// ---- set_disjoint_machine_grade【objdump】----
// ORIG W 0x85d3e9a sz 0x1b：call charac_expand::CData::alter 后写 +0x0c。
#include "ExpertJobOnlyMgr/set_disjoint_machine_grade.inc"
}

}  // namespace expert_job

// ===========================================================================
// expert_job::CDisjointMachine::include【objdump】
// ORIG W 0x85d3de6 sz 0x20：this+0x04 与 other+0x04 两矩形转调
// collision_detection::Rectangle::include（W 0x85cb492）。
// ===========================================================================

namespace expert_job
{

#include "ExpertJobOnlyMgr/include.inc"
}

}  // namespace expert_job
