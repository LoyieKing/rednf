// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CUserMissingA2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// CUserMissingA2.cpp —— CUser 未定义方法补齐（独占 TU；不改 CUser.cpp/CMake）。
//
// 逐份完整读取报告（## 汇编 + ## 反编译 C）后转写：
//   1. CUser::GetCharacHotKeyType            docs/.../CUser/GetCharacHotKeyType.md
//                                            ORIG T 0x08692ef2 _ZN5CUser19GetCharacHotKeyTypeEi
//   2. CUser::CheckLoadHotKeyCashedCharacType .../CUser/CheckLoadHotKeyCashedCharacType.md
//                                            ORIG T 0x08692e9c _ZN5CUser31CheckLoadHotKeyCashedCharacTypeEii
//   3. CUser::CalcMercenaryAvatarBonus       .../CUser/CalcMercenaryAvatarBonus.md
//                                            ORIG T 0x0868ea68 _ZNK5CUser24CalcMercenaryAvatarBonusEv
//   4. CUser::setAvatarTypeBonus             .../CUser/setAvatarTypeBonus.md
//                                            ORIG T 0x0868ebaa _ZN5CUser18setAvatarTypeBonusEc
//   5. UserMercenaryInfoMgr::FindIndexByCharacNo（4 的依赖，ORIG T 0x081b42ae）
//   6. UserMercenaryInfoMgr::CheckCharacIndex    （4 的依赖，ORIG T 0x081b4262）
//   7. CHARAC_LOAD_MERCENARY::Reset          .../CHARAC_LOAD_MERCENARY/Reset.md
//                                            ORIG T 0x081b410a（布局自证）
//   8.（已迁回 CFatigueBatteryHandle.cpp 本类 TU；此处不再定义）
//                                            .../CFatigueBatteryHandle/ActiveFatigueBattery_EndLevel.md
//                                            ORIG T 0x084a81f4
//   9. DB_LoadCharacHotKeyOption::makeRequest .../DB_LoadCharacHotKeyOption/makeRequest.md
//                                            ORIG T 0x0842f710
//
// eventDeliveryBuyItem（ORIG T 0x0866463c）已由 CUserDepsB.cpp 定义，本 TU 不重复。
//
// ---------------------------------------------------------------------------
// [b28d-solo-CUserMissingA2②] 本波（b28d_tu 12 件清单）收口：
//   TU 三数 279/3/9（identical+ae=279, near=3, diff=9 含 _Z41 豁免），
//   missing_in_game=43；基线 278/3/10 ⇒ ae +1、near/diff 不升。
//   AE  : CUser::calc_quest_stat（上波）；CHARAC_LOAD_MERCENARY::
//         GetCompetitionPeriodTerm（本波 DIFF→AE：P226 裸语句表达式
//         `({ getCurSec(); }) - m_field28` 复现 ORIG 分子段装载序 6 条形）。
//   NEAR: CUser::GetETC（N93：ORIG end=-0x14/find=-0x10，4.4 反向；P309 全头部
//         声明+先声明后赋值形触发 iterator C1Ev ×2 反而 DIFF，封顶）；
//         CUser::processAwakenReward（N7：4.4.7 间接 call 目标恒 %eax）；
//         CUser::CalcMercenaryAvatarBonus（N7，p167 8/8 CLEAR）。
//   DIFF(N 封顶):
//     CUser::updateBingoData        N18a 反向（ORIG `mov esi,[eax]` 单段直落
//                                   callee-saved；P226 语句表达式/纯 register 均不翻）
//     CUser::SaveMercenaryData      N101 imul vs shl/add（下标形与 ptr+row 指针
//                                   加法形 4.4 均强度归约）+ N82 槽序（row 头部
//                                   声明/预清均不改 -0x24 归属）
//     CUser::SetEquipment           N7 寄存器分配（4.4 跨 operator[] 保活 base+索引
//                                   push %esi；去 cnt 局部/分 cast 形均劣化）
//     CUser::_fnStatQuestClear      N(形参自增 4.4 恒折 `add [ebp+0xc],4`，ORIG
//                                   lea+store) + N82 槽序 + N89（ORIG 内层 for
//                                   初始化重复发射 begin+C1IPiE，p167 22/24）
//     CHARAC_LOAD_MERCENARY::Competition   N82 槽序 + N18a（16 位比较已用 (short)
//                                   收窄复现 `cmp dx,ax; jg`；余 `mov edx,eax`
//                                   中转与 lea-vs-add）p167 CLEAR 30/30
//     CUser::processFatigueCharac   N115 outgoing 预留（帧 0x190 vs 0x1ac、多 push
//                                   %edi）+ N91/N104 EH cleanup 块序（p167 CLEAR 29/29）
//     CHARAC_LOAD_MERCENARY::Reward        N82/N104（帧 0x8c vs 0x9c；isFinal 已按
//                                   ORIG 0x081b3dca 预清提函数头）p167 CLEAR 49/49
//     CHARAC_LOAD_MERCENARY::GetRewardItem 本波补齐两处真缺口：item 分支
//                                   `if (!group.m_itemList.empty())` 正向包裹
//                                   （ORIG 0x081b3786 const empty 调用）+
//                                   `if ((int)dropLists.size() <= it->second.first)
//                                   return false;` 早返形（ORIG 0x081b3636 setle）
//                                   ⇒ p167 50/50 CLEAR；余 return-false 岛(N111)
//  真语义 bug（上波已修）: GetRewardItem m_itemList pair 字段互换。
//  本波头改动（Main 代落，R2/S2）：CUser.h:3037 getUserQuestRewardItem +static
//   （ORIG 调用点压 2 实参无 this）；SigTypes_C.h:1029 CHARAC_LOAD_MERCENARY::
//   m_field28 int→unsigned（P211 档③ RHS-first+中转，探针 /tmp/probe_m28.cpp）。
//   P0/N96 绑定: 本文件 0 处 register asm（grep 复核）。
//
// ---------------------------------------------------------------------------
// [b28d-solo-CUserMissingA2③] 本轮重核（dual_rank 两件，逐件重读报告 + 段级比对）：
//   CUser::_fnStatQuestClear      gap 16→3。真缺口：ORIG 内层 end 只求一次
//     （0x08664fb6），每轮外层迭代只重求内层 begin（0x0866505a）；原实现把
//     begin+end 都放体内，静态少 13 条。修法：end 提出循环外、内层 for 只重求
//     begin（语义等价）。余 3 条 = 三处 context 形参自增（P322②/N：4.4 恒折
//     `addl $0x4,0xc(%ebp)`，ORIG 为 `lea edx,[eax+4]; mov [ebp+0xc],edx`）。
//     p167 24/24（C1↔C2 同址别名，P262）。
//   CHARAC_LOAD_MERCENARY::GetRewardItem  gap 6→1。真差异：dropDice 误建为函数级
//     static（ORIG 0x081b3648 落栈槽 -0x2c，且 nm 六个 .bss 局部符号不含它）
//     ⇒ 改普通局部 int；groupSum/groupIdx 槽序翻正（ORIG -0x40/-0x3c）；补
//     groupIdx 重复清零与 mob/item 分支显式 return，复现 ORIG 四个 return-false
//     岛。余 1 条 jmp 中继 + 尾部对齐 nop（§6.1/N112）。p167 50/50（CMTRand
//     C1↔C2 同址别名，P262）。
//
// ---------------------------------------------------------------------------
// [Lane14C·UserMissingA2] 本轮（Main 落 446 路由后复核；进入时 110/170/3/9
//   ⇒ 收口 110/173/1/8，含 _Z41 豁免 ⇒ 真 diff 7 件）：
//   · **CUser::updateBingoData DIFF→AE（真类型缺口）**：ORIG 0x0869145a
//     `mov (%eax),%esi` 单段直落 callee-saved；字段 `BingoData::m_eventId` 为
//     unsigned int 而 makeRequest 第 3 形参为 int（mangled Eijib）⇒ 4.4 对异型
//     实参插转换中转（`mov (%eax),%eax; mov %eax,%esi`）。探针 p2(异型→两段)/
//     p3(形参 unsigned→单段)/p6(同型 int 左值视图→单段) 三向实证 ⇒ 以 int 左值
//     视图读取（P266 同族旋钮；sizeof/偏移/mangled 零变化）。
//   · **CUser::processFatigueCharac 168→163 条（真语义修复）**：ORIG 0x08692689
//     `cmpl $0,-0x14; je <+0x21f 共享清理出口>` = `if (def != 0) { … }` 正向包裹
//     （P222/P257）；原「if (def == 0) return;」平铺形使 4.4 生成反向 jne + 独立
//     出口块，**且 def==0 时仍执行 snprintf/findString**（ORIG 直接清理返回）。
//     余差 = N115 outgoing 预留 16B + itemIndex 与 Inven_Item 对象重叠槽
//     （ORIG -0x5f=item+2，4.1 栈复用；4.4 独立分配）+ expire→%edi 保活。
//   · N 复核（新增探针，均为跨版本/后端结构性，语义等价）：
//     - GetETC（NEAR 维持，N93）：/tmp/l14p 以外的 exp6 逗号声明形、exp2 先声明
//       后赋值形（触发 iterator C1Ev + sret 临时 → DIFF）实测均不翻；ORIG end=
//       -0x14/find=-0x10 与 4.4 恒反且调用序锁死声明序 ⇒ 封顶。
//     - SetEquipment（N7 维持）：`int* volatile cnt` 强制重载探针实测无效（4.4
//       仍缓存 base 到 %ebx、*cnt 到 %esi；ORIG 只缓存索引 %ebx）⇒ 封顶。
//     - Competition（N）：16 位比较四形探针实测——(short)a<=(short)b ⇒ 6 条+jg；
//       ushort<=ushort ⇒ 5 条+ja；交换操作数 ⇒ 5 条+jl；`*reinterpret_cast<short*>`
//       视图 ⇒ 6 条+jg（多 add）。ORIG=5 条+jg（movzwl 直落 %edx）不可兼得 ⇒ 维持
//       原 `(short)a <= m_level`（有符号语义优先，6 条+jg）。
//     - GetRewardItem（N82/N112）：fresh 复核槽**集合全同**（-12…-104 各 26 项），
//       差异为同一变量槽号互换（range ORIG -0x60 vs 我方 -0x50 等 14 块）+ 1 条
//       jmp 中继/尾 nop。
//     - Reward（N82）：我方槽集合多 5 项（ORIG 16 / 我方 21），帧 0x9c vs 0x8c；
//       p167 仅 cMyTrace C1↔C2 / STL C1↔C2 同址别名（P262）。
//     - SaveMercenaryData（N101/N82）：探针 p7 实证 4.4 对常数乘法恒强度归约
//       shl/add（`row*0x54` 六条 vs ORIG 单条 `imul $0x54`；变量乘数可出 imul 但
//       新增槽使帧 0x3c→0x40 反劣）⇒ 维持下标形。
//     - _fnStatQuestClear（N1/N107）：context 自增十形探针（f0-f5/f8/f9/fA/fB）
//       全折 `add DWORD PTR [ebp+0xc],0x4` 或劣化；ORIG `mov/lea/mov` 三连不可造，
//       余 3 条/函数。p167 24/24（P262）。
// ============================================================================

#include <string.h>  // memset

#include "CUser.h"                  // CUser/UserMercenaryInfoMgr/CHARAC_LOAD_MERCENARY/_Charac_info
#include "CInventory.h"             // CInventory::GetInvenRef(int,int) const、INVEN_TYPE_INVEN=0
#include "CDataManager.h"           // G_CDataManager / find_item(int) const
#include "CEquipItem.h"             // CEquipItem 完整类型（dynamic_cast 目标 + IsAvatarItem 槽 4）
#include "CFatigueBatteryHandle.h"  // 本类声明
#include "GameTypes.h"              // cMyTrace(name, line, flag) + operator()
#include "GlobalData.h"             // s_stream_pool / s_msgq_mgr
#include "StreamPool.h"             // StreamPool::Acquire(char const*, int)
#include "W8LT_DBTask.h"            // DB_UpdateBingoEvent 权威声明（makeRequest/dispatch）
#include "MsgQueueMgr.h"            // MsgQueueMgr::put(QUEUE_IDX, CStreamGuard&)
#include "SigTypes.h"               // SIG_REQ_LOAD_CHARAC_HOTKEY_OPTION
#include "DB_LoadCharacHotKeyOption.h"
#include "CItem.h"                // CItem::get_rarity（Reward 尾部）
#include "CItemList.h"            // CItemList::create_item / ENUM_ITEM_CREATE_TYPE
#include "CMailBoxHelper.h"       // WongWork::CMailBoxHelper::ReqDBSendNewSystemMail
#include "CSystemTime.h"          // CSystemTime::getCurSec
#include "DB_MercenaryLogInfo.h"  // DB_MercenaryLogInfo::makeRequest
#include "PacketTypes.h"          // competition_reward_result 完整定义
#include "PvPTypes.h"             // CMTRand（probArea/probabilityRewardItem 局部 static）
#include "RDARScriptStringManager.h"  // g_scriptStringManager_ / findString
#include <stdio.h>                // sprintf / strncpy / strlen
#include "PacketGuard.h"           // PacketGuard（SendUnlimitChallengePacket）
#include "InterfacePacketBuf.h"    // put_header / put_byte / finalize
#include "CEventScriptMng.h"       // EventClassify::CEventScriptMng::process_level_up_reward
#include "Quest.h"               // Quest / RewardItemInfo 完整类型

// ----------------------------------------------------------------------------
// ORIG W 模板（本 TU 消费；符号 0x081b48da/<int> 0x081b4a66 / 0x081b495d）：
// 遍历累加 first 概率，dice<=累计 命中返回 second；未命中回退首元素 second。
// ----------------------------------------------------------------------------
template <typename T>
T FindRelativeProb(const std::vector<std::pair<int, T> >& vec, int dice)
{
    int sum = 0;
    for (size_t i = 0; i < vec.size(); ++i)
    {
        sum += vec[i].first;
        if (dice <= sum)
            return vec[i].second;
    }
    return vec[0].second;
}

// ----------------------------------------------------------------------------
// ORIG W 0x081b495d：取 first<=level 中 first 最大的元素，返回其 second。
// ----------------------------------------------------------------------------
template <typename T>
T Find_max_value_lower_N(int level,
                         const std::vector<std::pair<int, T> >& vec)
{
    int bestIdx = 0;
    int bestFirst = 0;
    for (size_t i = 0; i < vec.size(); ++i)
    {
        // [b28d-27] ORIG 0x081b495d：两条件各自 setcc 物化（P225 推广，`&&`
        // 合并 4.4 恒折 jg/jl 直跳）⇒ 拆嵌套 if。
        if (vec[i].first <= level)
        {
            if (vec[i].first >= bestFirst)
            {
                bestFirst = vec[i].first;
                bestIdx = i;
            }
        }
    }
    return vec[bestIdx].second;
}

// ----------------------------------------------------------------------------
// ORIG b 0x09407e80（.bss 全局 string，internal linkage；初值推断为空串，
// 运行期默认构造。GetRewardItem/Reward 用作兜底邮政消息）。
// ----------------------------------------------------------------------------
namespace MercenaryHardCodingSettingValue
{
    static std::string MERCENARY_BASE_POSTAL_MESSAGE;
    // ORIG r 0x08bb8754 = 0x2710 (10000)，_ZN31MercenaryHardCodingSettingValueL19ITEM_DROP_MAX_VALUEE
    static const int ITEM_DROP_MAX_VALUE = 10000;
}

// ----------------------------------------------------------------------------
// 11. ORIG T 0x081b2b34 sz 0x8f。m_vec40 同型检查；再判
// level >= m_areaLevel（ushort 符号扩展后与 level 比较）。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/CheckCompetitionAreaIndex.inc"
}

// ----------------------------------------------------------------------------
// 2. ORIG T 0x08692e9c sz 0x55。
// cashedCharacJob==-1 → 1（首次加载放行）；否则两次调 GetCharacHotKeyType 比较，
// 相同→0、不同→1（类型变化才允许重新加载热键）。
// [P210] 本函数定义前置到 callee GetCharacHotKeyType 定义之前：4.4 对 TU 内
// 已见定义的 callee 精确继承 outgoing（帧塌缩 0x14→0x8），前置后恢复 ORIG
// 保守帧 sub $0x14（b28 P210 派生附注同法）。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/CheckLoadHotKeyCashedCharacType.inc"
}

// ----------------------------------------------------------------------------
// 1. ORIG T 0x08692ef2 sz 0x17。
// cmpl $0xa,param; jne→0；相等→1。返回值 eax 全宽（头文件声明 int，调用点
// LoadCharacTypeHotKey 以 mov %eax 全宽取值后截 byte；语义 characJob==10）。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/GetCharacHotKeyType.inc"
}

// ----------------------------------------------------------------------------
// 3. ORIG T 0x0868ea68 sz 0x141。雇佣兵 avatar 等级加成 = 全身 avatar 装备的最低
// 品阶：初值 10；getCurCharacR/getCurCharacInvenR 为空→0；否则 i∈[0,7] 遍历
// GetInvenRef(INVEN, i)：空引用/find_item 失败/dynamic_cast<CEquipItem*> 失败/
// IsAvatarItem()==0（vtable+0x10 槽 4，两处 dtor 后第 5 槽）任一即返 0；
// 否则取 min(get_grade)；循环后 >3 归零。
// ----------------------------------------------------------------------------
// [b28d-27] DIFF N82：ORIG 0x0868ea68；p167 CLEAR（8/8），残差为帧/槽号与
// callee-saved 分配（4.1 保守帧 vs 4.4 紧凑帧），语义闭合。
// [b28d-solo-CUserMissingA2] 修复⇒NEAR：去 `CItem* item` 具名局部（dynamic_cast
// 直调 find_item 结果，ORIG 0x0868eb0f 无 item 槽）+ P222 正向包裹 equip 判定 +
// 尾部 `if (3 < best) return 0;` ⇒ 槽位集与 ORIG 全等（best/-0x1c、inven/-0x18、
// i/-0x14、ref/-0x10、equip/-0xc）。残差仅 N7（虚调用目标 4.4.7 恒 %eax vs ORIG
// %edx）+ .o 重定位伪差；p167 CLEAR 8/8。
#include "CUserMissingA2/CalcMercenaryAvatarBonus.inc"
}

// ----------------------------------------------------------------------------
// 4. ORIG T 0x0868ebaa sz 0xb1。
// bonus∈[0,3] 且 getCurCharacR() 非空时：characNo=*getCurCharacR()（+0 dword）→
// m_mercenaryMgr.FindIndexByCharacNo → !=-1 且 CheckCharacIndex((uchar)idx) →
// vector 元素 +0x32（m_avatarBonus）写 bonus。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/setAvatarTypeBonus.inc"
}

// ----------------------------------------------------------------------------
// 5. ORIG T 0x081b42ae sz 0x51。按 m_characs[i].m_characNo（首 dword 无符号比较）
// 线性查找；未命中返回 0xffffffff。循环条件 seta（size()>idx 无符号大于）。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/FindIndexByCharacNo.inc"
}

// ----------------------------------------------------------------------------
// 6. ORIG T 0x081b4262 sz 0x4b。empty→1；size>=index（jae 无符号，含等）→1；
// size<index→0（汇编实证：empty 分支经 eax=0→je→返回 1，仅 size<index
// 落到返回 0）。ORIG 边界 index<=size 与调用点 FindIndexByCharacNo
// 命中下标恒 <size 相容。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/CheckCharacIndex.inc"
}

// ORIG T 0x081b45ce：未找到项返回等待；命中项以 GetCompetitionState()==0 判等待。
#include "CUserMissingA2/IsWaitingStateCharac.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x081b4198：线性查找 characNo 相同项；命中且 level 不同则更新
// level 并 alter()，返回 0；命中 level 相同也返回 0；未命中返回 1。
// 循环条件 seta（size()>i 无符号），参数首 dword 每次迭代载入 ebx（P36）。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/CheckValidAppend.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x081b44c0：user 空→0；FindIndexByCharacNo + CheckCharacIndex 收窄
// 校验；有效则 m_characs[idx].Reward(user, reward) 直传返回值（无规范化）。
// [b28d-27] 返回类型 int→bool（P9）：ORIG 尾 `call Reward; leave; ret` 直传单字节，
// 无 movzbl 规范化；CUser.h:791 已由 Main 代落（mangled 不含返回类型，ABI 零影响）。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/ReturnCharac.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x081b4536：无效下标→0；有效项 Competition(user,modeA,modeB) 成功→1。
// modeB/modeA 先 movsbl 入 esi/ebx 跨 operator[] 存活（P52）。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/CompetitionCharac.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x0868e692：转发 UserMercenaryInfoMgr::ReturnCharac（receiver
// lea this+0x6ef6c）。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/Return_Mercenary_Charac.inc"
}

// ORIG T 0x081b2e86：返回 1=竞技中、0=未报名、2=已结束且已报名。
#include "CUserMissingA2/GetCompetitionState.inc"
}

// ----------------------------------------------------------------------------
// 6b. ORIG C1 W 0x08694c08 sz 0x86（报告缺失，objdump 取证）：与 Reset 字段
// 集一致但顺序不同 —— 标量在前（+0/+4/+0x24/+0x28/+0x2c 清零、
// +0x30/0x31=0xff、+0x32=0xa），两个 memset 在后（+6,0x1e / +0x34,0x20）。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/CHARAC_LOAD_MERCENARY.inc"
}

// ----------------------------------------------------------------------------
// 7. ORIG T 0x081b410a sz 0x85。字段清零/初始化顺序逐条对应汇编：
// +0 dword=0、+4 word=0、memset(+6,0,0x1e)、memset(+0x34,0,0x20)、
// +0x24 byte=0、+0x28 dword=0、+0x2c dword=0、+0x30=0xff、+0x31=0xff、+0x32=0xa。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/Reset.inc"
}

// ----------------------------------------------------------------------------
// 9. ORIG T 0x0842f710 sz 0xe3。DB 请求入队：
// Acquire("DBThread.cpp",0x749a) → CStreamGuard(stream,true) → opcode 800(0x320)
// → uid(int) → GetInBuffer<SIG_REQ_LOAD_CHARAC_HOTKEY_OPTION> 写 +0 accId(uint)、
// +4 hotkeyType(int) → put(DB_Q=2, guard)。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/makeRequest.inc"
}

// ============================================================================
// CHARAC_LOAD_MERCENARY 竞技域 15 函数（ORIG 报告：docs/class_func_reports/
// CHARAC_LOAD_MERCENARY/*.md；alter/save_complete 报告缺失按 objdump 取证）。
// ============================================================================

// ----------------------------------------------------------------------------
// 10. ORIG T 0x081b2bc4 sz 0x59。G_CDataManager()->m_vec14：
// empty→0；!empty 且 size>=(signed)periodIdx→1；否则 0。movsbl 符号扩展。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/CheckCompetitionPeriodIndex.inc"
}

// ----------------------------------------------------------------------------
// 12. ORIG T 0x081b2c94 sz 0x75。m_vec20 表按 m_avatarBonus 查 float。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/GetAvatarBonus.inc"
}

// ----------------------------------------------------------------------------
// 13. ORIG T 0x081b2c1e sz 0x75。m_vec14 表按 m_field31 查 float。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/GetPeriodBonus.inc"
}

// ----------------------------------------------------------------------------
// 14. ORIG T 0x081b2d6a sz 0x11b。base=Find_max_value_lower_N(...).second；
// rate=(int)(term*base*period*avatar)（fistpl 截断）；GM 打印调试；
// rate<1 归 -1。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/CalcItemDropRate.inc"
}

// ----------------------------------------------------------------------------
// 15. ORIG T 0x081b2d0a sz 0x5f。dice<0 或 dice>m_field3c（0xa918）→0；
// 否则 FindRelativeProb<float>(m_vec30, dice)。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/GetRewardCritical.inc"
}

// ----------------------------------------------------------------------------
// 16. ORIG T 0x081b2ed2 sz 0x5f。period 有效时返回
// vec14[periodIdx].first * m_field0（每期秒数）。
// ----------------------------------------------------------------------------
// [b28d-27] DIFF N82附/P52：ORIG 0x081b2ed2；ORIG push esi + sub 0x20（值跨 call
// 保活 callee-saved），4.4 延迟装载 + 帧 0x24；p167 CLEAR（4/4），语义闭合。
// [b28d-solo-CUserMissingA2] 修复：整数乘操作数交换为 `m_field0 * vec[..].first`
// （P243 同族）⇒ 32/32 与 ORIG 逐条同形（m_field0 先入 %esi 跨 call 保活 +
// push esi/sub $0x20 帧），转 AE。
#include "CUserMissingA2/GetCompetitionPeriodSecond.inc"
}

// ----------------------------------------------------------------------------
// 17. ORIG T 0x081b2f32 sz 0x113。m_field31 期次检查 + 表越界→0；
// term=(getCurSec()-m_field28)/m_field0；term<期次表值→out=true；
// 否则 term=表值（else 分支二次 operator[] 照 ORIG）。
// ----------------------------------------------------------------------------
// [b28d-solo-CUserMissingA2②] **AE**（本波 DIFF→AE）：ORIG 0x081b2f32。
// 关键 = P226 裸语句表达式：分子段 ORIG 0x081b2fa5 先求 getCurSec（call→eax→
// `mov %eax,%edx`）再读 m_field28（`mov 0x28(%eax),%eax`），4.4 对裸
// `getCurSec() - m_field28` 恒先读内存（`mov 0x28(%edx),%edx; sub %edx,%eax`
// 单段）；改 `({ GlobalData::s_systemTime_.getCurSec(); }) - m_field28` 后逐条
// 复现 ORIG 6 条形。p167 CLEAR 8/8。
#include "CUserMissingA2/GetCompetitionPeriodTerm.inc"
}

// ----------------------------------------------------------------------------
// 18. ORIG T 0x081b4190 sz 0x7。空函数（仅 prologue/epilogue）。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/SendMessageToCUser.inc"
}

// ----------------------------------------------------------------------------
// 19. ORIG W 0x081b47c8 sz 0xc：movb $1,0x24(%eax)（竞技报名后置位）。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/alter.inc"
}

// ----------------------------------------------------------------------------
// 20. ORIG W 0x08694c8e sz 0xc：movb $0,0x24(%eax)（结算后清除）。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/save_complete.inc"
}

// ----------------------------------------------------------------------------
// 21. ORIG T 0x081b3046 sz 0x22f。金奖励：
// base=Find_max_value_lower_N(...).first；gold=(int)(term*base*period*avatar+0.01)；
// criticalDice=randInt(m_field3c)；critical=GetRewardCritical；
// GM 打印；result.m_0=(int)(term*base*period*critical+0.01)；
// nCritical=(int)(gold*critical+0.01)；result.m_4=nCritical-result.m_0。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/GetRewardGold.inc"
}

// ----------------------------------------------------------------------------
// 22. ORIG T 0x081b3276 sz 0x59d。物品奖励（长流程）：
// 重置 item、msg 置兜底邮政消息；area 校验；m_areaLevel==0 归零 m_field30；
// itemDropDice=randInt(10000)（GM 用户强制 0）；>=dropRate 失败；
// groupDice=randInt(m_groupIdSum)；GM 打印；按组累加选组（越界回绕 0）；
// msg=组名；mobList 非空走 mob 分支（map24 查独立掉落表），否则 item 分支
// （m_itemList 累加）；命中写 item+create_item+result.m_8=1。
// ----------------------------------------------------------------------------
// [b28d-27] DIFF（cv 旋钮待修）：ORIG 0x081b3276；p167 差集显示 ORIG 用 const
// 迭代器/const operator[]（_ZNKSt6vector...ixEj ×4、_ZNKSt17_Rb_tree_iterator...ptEv ×3、
// _ZNKSt6vector...emptyEv），我方用非 const 版（+4/+3/+2）⇒ N89/P96 封顶。
// [b28d-solo-CUserMissingA2] 修复（真语义 bug）：m_itemList 的 pair 语义为
// (概率, 道具id)——`sum2 += .first`、命中 `item.m_addInfo = .second`
// （ORIG 0x081b376c `mov (%eax),%eax` / 0x081b3790 `mov 0x4(%eax),%eax` 实证；
// 原实现两处互换 ⇒ 概率累加错、道具 id 取错）。另：pArea 表达式内联（消 -0x48
// 具名槽）、`itemDropDice >= dropRate` 比较方向翻正（ORIG `cmp dropRate,%eax; jl`）。
// [b28d-solo-CUserMissingA2②] 本波补齐两处真缺口 ⇒ **p167 50/50 CLEAR**：
// ① item 分支正向 empty 包裹 `if (!group.m_itemList.empty())`（ORIG 0x081b3786
//    `mov eax,[ebp-0x38]; add eax,0x14; call _ZNKSt6vectorISt4pairIiiESaIS1_EE5emptyEv;
//    xor $1; test; je <return-0>` 实证——原实现直接进 for 少一次 const empty）；
// ② dropLists 越界守卫改早返形 `if ((int)dropLists.size() <= it->second.first)
//    return false;`（ORIG 0x081b3636 `call size; mov ebx,eax; call ptEv; mov
//    eax,[eax+0x4]; cmp ebx,eax; setle; test; je <主流程>` 实证）。
// [b28d-solo-CUserMissingA2③] 本轮重核（dual_rank gap 6）⇒ 修真差异 + gap 6→1：
//   ① **dropDice 误建为函数级 static**：ORIG 0x081b3648 `mov %eax,-0x2c(%ebp)` /
//      0x081b3673 `mov -0x2c(%ebp),%eax` 是栈槽，且 nm 的 6 个 .bss 局部符号
//      （itemDropDice/groupDice/mobDice/itemDice/buffer/probabilityRewardItem）**不含**
//      dropDice ⇒ 原码是普通局部 int（原实现多写一个 .bss 符号且读写形态不同）。
//   ② groupSum/groupIdx 声明序翻正（ORIG 0x081b346e 先清 -0x40=groupSum、后清
//      -0x3c=groupIdx；原实现槽互换）。
//   ③ 复现 ORIG 形态：groupIdx 重复清零（0x081b347c）、mob/item 两分支在循环后补
//      显式 `return false` 使 4.4 分出独立 return-false 岛（ORIG 0x081b36f6/0x081b3700
//      /0x081b37fa/0x081b3803 四岛；原实现共享单一出口，少 5 条）。
//   残差 = 1 条 `jmp` 中继岛 + 尾部对齐 `nop`（§6.1 豁免）⇒ N112 封顶；
//   p167 50/50（MISSING/EXTRA 仅 CMTRand C1↔C2 同址别名，P262）。
#include "CUserMissingA2/GetRewardItem.inc"
}

// ----------------------------------------------------------------------------
// 23. ORIG T 0x081b3b68 sz 0x5a2。结算主流程：GetCompetitionState 非 0 才继续；
// 局部 static emptyItem/mailTitle/serverMessage/strMessage；try{GetRewardGold+
// GetRewardItem}catch 复位+alter+cMyTrace 返回 1；gold<1 走 MercenaryReturn
// 清理；否则发系统邮件（ReqDBSendNewSystemMail）+ 日志 + DB 记录。
// ----------------------------------------------------------------------------
// [b28d-27] DIFF N82/N89：ORIG 0x081b3b68；p167 CLEAR（49/49），残差为 STL 实例
// 形态与帧/槽分配，语义闭合。
// [b28d-solo-CUserMissingA2] 修复：`bool isFinal2` 去预清（ORIG 无 movb $0，
// 直读 movzbl -0x3a）+ `emptyItem.m_addInfo` 两处独立读取（P48 无 CSE，消
// itemIdx 具名槽）。p167 CLEAR 49/49；残差为槽序（4.4 首次使用序）+ STL 实例
// 形态 ⇒ N82/N89 封顶。
#include "CUserMissingA2/Reward.inc"
}

// ----------------------------------------------------------------------------
// 24. ORIG T 0x081b3814 sz 0x353。竞技报名：Reward 预结算+状态/期次/地区校验；
// area m_worldMapIdx==0xffff 时随机选可报名地区（probArea + 有效地区表）；
// GetCompetitionPeriodSecond 非 0 → alter + 记录时间/期次/地区 + DB 日志。
// ----------------------------------------------------------------------------
// [b28d-27] DIFF N82/N86：ORIG 0x081b3814；p167 CLEAR（30/30），残差为容器迭代器
// 临时槽 + 帧分配，语义闭合。
// [b28d-solo-CUserMissingA2] 修复：`if (!CheckCompetitionAreaIndex(...)) areaIdx=0;
// else areaIdx=idx;` 否定形复现 ORIG 0x081b3a4a `xor $1,%eax` 物化 + pArea 表达式
// 内联（消具名槽）⇒ 全段 diff 行 254→200。p167 CLEAR 30/30；残差帧 0x8c vs 0x7c +
// 槽序（N82/N93）封顶。
#include "CUserMissingA2/Competition.inc"
}

// ============================================================================
// Wave15 批：CUser 仅ORIG 强符号补齐（逐报告核对；真实语义/签名）
// ============================================================================

// ----------------------------------------------------------------------------
// ORIG W 0x08697474 _ZN5CUser6GetETCEs：map 取键值后删除。
// 汇编：end()→find(key)→eq 返 0；op[](key) 取 int；erase(key)；返 val。
// ----------------------------------------------------------------------------
// [b28d-27] NEAR N93：ORIG 0x08697474；ORIG end()槽 -0x14 / find()槽 -0x10（4.1
// 先声明占深槽），4.4 恒反向（先声明浅槽）；交换声明序会改调用序（ORIG end 先、
// find 后）⇒ 封顶。p167 CLEAR（5/5），语义闭合。
// [b28d-solo-CUserMissingA2] 复验：p167 CLEAR 5/5。两形实测——①交换声明序（it
// 先/e 后）⇒ NEAR→DIFF（调用序反）；②it 包内层块作用域 ⇒ 槽序仍反。ORIG
// end=-0x14 / find=-0x10（4.1 先声明深），4.4 恒反向 ⇒ N93 封顶。
#include "CUserMissingA2/GetETC.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x0868dd44 _ZN5CUser17get_SoloPlay_modeEv：
// GetGameOptionRef()→get_etc_option(0x1b) short→int 符号扩展返回。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/get_SoloPlay_mode.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x0868a4b6 _ZN5CUser15set_MailMsgDataEP16MSG_MAILBOX_SEND：
// movb $0,0x8e3ec（mail 标志清 0）+ memcpy(this+0x8e101, msg, 0x145)。
// 0x8e101 起块视图沿用 m_pad8e101（0x2eb=0x8e3ec-0x8e101，先例见 CUserOnlyOrig）。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/set_MailMsgData.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x08686f58 _ZN5CUser26SendUnlimitChallengePacketEv：
// PacketGuard：header(0,0xa4) + byte(1) + finalize(true) + Send。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/SendUnlimitChallengePacket.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x08688d26 _ZN5CUser16GetItemLimitInfoEi：
// begin/end 双迭代器（end 缓存）；元素 +0x15==itemIdx 返回元素指针；否则 0。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/GetItemLimitInfo.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x08688c70 _ZN5CUser17IsLimitItemUsableEi：
// 命中条目按 isUsable() 判定（真返 1/假返 0）；无条目视为可用返 1。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/IsLimitItemUsable.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x0867925a _ZN5CUser20processGrowTypeEventEhh21eChangeGrowTypeReason：
// reason==1 忽略；转 _onChangeGrowType；CEventScriptMng::process_level_up_reward
// (this,true)；p2==0 时 processLevelUpEventReward(true)。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/processGrowTypeEvent.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x0868e6c0 _ZN5CUser28Competition_Mercenary_CharacEjcc：
// 转发 m_mercenaryMgr.CompetitionCharac(this, slot, modeA, modeB)。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/Competition_Mercenary_Charac.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x086914cc _ZN5CUser15updateBingoDataEv：
// m_needSave 置位时 DB_UpdateBingoEvent::makeRequest(GetUID(), acc_id,
// m_eventId, m_isWin)（ORIG 签名 Eijib），随后 m_needSave 清 0。
// getBingoData() 每次独立调用（P48 无 CSE）。
// ----------------------------------------------------------------------------
// [b28d-27] DIFF N18a 反向：ORIG 0x086914cc；ORIG `mov (%eax),%esi` 单段直落
// callee-saved，4.4 恒经 %eax 两段；旧探针族（具名/内联/register）均不翻 ⇒ 封顶。
// p167 CLEAR（7/7），语义闭合。
// [b28d-solo-CUserMissingA2] 复验 44/43：P226 裸语句表达式、P231 纯 register、
// 删 `static_cast<int>` 三形实测均不翻（register 反使 isWin 装载序劣化），
// ORIG `mov (%eax),%esi` 单段直落 callee-saved 不可造 ⇒ N18a 反向封顶；
// p167 CLEAR 7/7。
#include "CUserMissingA2/updateBingoData.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x08651de6 _ZN5CUser17UpdateBloodRecordEv：
// 遍历当前角色 _Charac_info.m_bloodBestRecords（+0xecc），每项经 stream 发
// SIG_UPDATE_BLOOD_DUNGEON_BEST_RECORD（sig+0=characNo、+4=m_time、+8=m_round、
// +0xc=key）到 LOGDB 队列；末尾 setBloodBestRecordUpdate(false)。
// ----------------------------------------------------------------------------
// [b28d-27] DIFF（p167 MISSING 3）：ORIG 0x08651de6。
// [b28d-solo-CUserMissingA2] 已修复为 **AE**（127/127；p167 24/24 CLEAR）：
// [b28d-solo-CUserMissingA2] 迭代器族归位：ORIG 0x08651de6 用**非 const**
// `_Rb_tree_iterator`（p167：MISSING 非 const begin/end/ptEv/ppEv/ne，EXTRA 我方
// const 版全族）⇒ 原码 cur 为非 const 指针；本函数只读遍历，用 const_cast 取回
// 非 const 视图（不改共享头，避免全仓重编译）。循环条件形 = `it != map.end()`
// （ORIG end 调用点在循环底部，无 end 具名缓存）。
// [b28d-solo-CUserMissingA2] store 序：ORIG 4 处 store 全为 RHS-first + disp 直存
// （`mov RHS,%edx; mov sig,%eax; mov %edx,disp(%eax)`）⇒ 原码是**具名视图结构成员
// 赋值**（裸 `*(unsigned int*)((char*)sig+K)` 产 lea+ebx 跨 call 缓存，DIFF）；
// 字段类型须与 RHS 精确同型（probe /tmp/b28d_solo/probe_view4.cpp fK 逐条实证）。
struct BloodBestRecordSigView
{
    unsigned int m_characNo;   // +0x00（getCurCharacNo 返回 unsigned int，同型免中转）
    unsigned int m_time;       // +0x04（stBloodBestRecord::m_time）
    int m_round;               // +0x08（stBloodBestRecord::m_round）
    int m_key;                 // +0x0c（pair::first）
};

#include "CUserMissingA2/UpdateBloodRecord.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x0865da4e _ZN5CUser12SetEquipmentEiPviS0_：
// for(i=0..9) 检查装备槽 i 过期（m_addInfo!=0 且 get_add_info()>=1 且
// GetDateTimeTick()>addInfo）；过期则把 m_characList[slot].m_characNo 记入
// outData 表并 reset 该槽、计数+1；循环后对 m_characList[slot]+0xda 的
// CInventory 调 SetInvenData(0, invenData, size)。
// ----------------------------------------------------------------------------
// [b28d-27] DIFF N82：ORIG 0x0865da4e；p167 CLEAR（7/7），残差为帧/槽分配，
// 语义闭合。
// [b28d-solo-CUserMissingA2] 修复：P226 条件语句表达式复现 `je skip`×3 + `mov
// $1/%eax … mov $0/%eax; test` 物化岛（消 expired bool 槽）；P311 具名视图
// OutDataView/OutTailView 复现 `mov %edx,0x4(%eax,%ebx,4)` /
// `mov %cl,0x5a4(%edx,%eax,1)` 单条 scaled/disp store（消 shl+add 地址链）；
// 第三条件操作数交换为 `get_add_info() < GetDateTimeTick()` 复现 ORIG 求值序。
// 残差仅 N7 寄存器分配：4.4 跨 operator[] 保活基址+索引（push %esi；ORIG 只保活
// 索引 %ebx、基址重载）；p167 CLEAR 7/7。
#include "CUserMissingA2/SetEquipment.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x086925c4 _ZN5CUser20processFatigueCharacEi：
// 按职业/已用疲劳匹配疲劳礼物；命中则取脚本文案、构造 Inven_Item、snprintf
// 疲劳消息，itemIndex!=0 时经 CMailBoxHelper 发系统邮件。
// ----------------------------------------------------------------------------
// [b28d-27] DIFF N82：ORIG 0x086925c4；p167 CLEAR（29/29），残差为帧/槽与
// 寄存器分配，语义闭合。
// [b28d-solo-CUserMissingA2] 修复：变参位内联 getCurCharacUsedFatigue()（ORIG
// 0x086926d8 `movzwl %ax,%ebx` 无局部槽）。残差为跨版本槽复用：ORIG 的 itemIndex
// 槽与 item.m_addInfo(-0x5f) 重叠（4.1 允许，4.4 独立分配）+ 帧 0x1bc vs 0x190
// ⇒ N82 封顶；p167 CLEAR 29/29。
#include "CUserMissingA2/processFatigueCharac.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x086792d6 _ZN5CUser19processAwakenRewardEc：
// switch(stage) 设定邮件正文/标题脚本文案（jump table），命中后构造
// Inven_Item(0x3c0, sealCount=1) 并 ReqDBSendNewSystemMail；default 直接返回。
// ----------------------------------------------------------------------------
// [b28d-27] DIFF N7+P52：ORIG 0x086792d6；虚调用 callee 寄存器 %eax↔%edx 互换
// （4.4 间接 call 目标恒 %eax）+ 值先物化 callee-saved 缺失；p167 CLEAR（32/32），
// 语义闭合。
// [b28d-solo-CUserMissingA2] 修复 P290：rewardItemId int→unsigned ⇒ find_item
// 实参升格 %ebx 跨 G_CDataManager call 保活（ORIG 0x08679609 对齐）；残余仅 N7
// （本 TU 路由 4.4.7 ⇒ 间接 call 目标恒 %eax），201/201。
#include "CUserMissingA2/processAwakenReward.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x0868e36a _ZN5CUser17SaveMercenaryDataEv：
// m_mercenaryMgr.m_characs 空则返 1；否则 stream 写 0x2b3+GetUID，
// SIG_MERCENARY_DB_DATA 清零后逐项（+0x24 脏标记）memcpy 0x54 到
// db+4+row*0x54 并 save_complete()；入 LOGDB 队列，返回 1。
// ----------------------------------------------------------------------------
// [b28d-27] DIFF（p167 MISSING 4）：ORIG 0x0868e36a；ORIG 多 4 条调用（STL 族），
// 我方实现语义等价但实例/形式不同（N89 类）+ 帧差。
// [b28d-solo-CUserMissingA2] 复验 136/132：p167 20/20 CLEAR；残余为槽序
// （4.4 首次使用序 vs 4.1）+ `imul $0x54` 强度归约（4.4 恒 shl/add 链）+
// `*db` store 序 ⇒ N82/N101 封顶。
// [b28d-solo-CUserMissingA2] p167 归位：ORIG 0x0868e36a 调 CStreamGuard::operator*
// ×2 + operator-> ×1（代理链）、operator[] ×3（P157 逐次重读，非引用缓存）、
// 结构整体赋值 `rep movsl 0x15`（非 memcpy）；row 为每次从 *db 读的具名局部。
struct MercenaryDbHeadView
{
    unsigned int m_count;   // +0x00（SIG_MERCENARY_DB_DATA 首 dword = 行数）
};

#include "CUserMissingA2/SaveMercenaryData.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x0868f726 _ZN5CUser20updatePcRoomPlayTimeEv：
// 取事件脚本管理器 get_event_entity(0x5c) 为 CPcRoomPlayTime；verify 通过
// （==0）时 collectPcRoomPlayTime() 并 DB_SavePcRoomPlayTimeEvent::makeRequest
// (accId, totalPlayTime, rewardCnt)。
// ----------------------------------------------------------------------------
class DB_SavePcRoomPlayTimeEvent
{
public:
    static void makeRequest(unsigned int accId, unsigned int totalTime,
                            unsigned char rewardCnt);
};

#include "CUserMissingA2/updatePcRoomPlayTime.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x08664e8e _ZN5CUser17_fnStatQuestClearEjPc（enumQuestClear 静态回调）：
// ctx=[this, elixirIds*, outList*]；find_quest 命中且任务种类（+0xb8）为
// 0/0x19 时，对奖励表×elixirIds 双层遍历，匹配项 add_matched_list 到 outList。
// ----------------------------------------------------------------------------
// [b28d-27] DIFF（p167 MISSING 8 族）：ORIG 0x08664e8e；ORIG 用迭代器遍历
// （begin/end/C1Ev/ptEv/deEv/ppEv/ne 共 14 次），我方索引遍历 ⇒ 符号族不同
// （P175/N89 类），语义等价。
// [b28d-solo-CUserMissingA2] 复验：const_iterator（rewards 指针 const 化）+ P50
// 先声明后赋值 + 缓存 end + 4 迭代器全头部声明。p167 差 2 = ORIG 非 const
// `vector<int>::begin()` + `C1IPiE` 转换构造实例未发射（N89 实例差异，语义等价：
// 我方同点位有 begin/C1IPiE 调用）；残差为 4.1 内层 for-init 旋转到外层体末尾、
// 4.4 恒置开头 ⇒ N164/N82 封顶。
// [b28d-solo-CUserMissingA2②] 本波：getUserQuestRewardItem 已 static（Main 代落
// CUser.h:3037）⇒ 调用点压 2 实参（[esp]=q/[esp+4]=user），与 ORIG 0x08664ecf
// 一致；rewards 补两句式预清（ORIG 0x08664f07 `mov [ebp-0xc],0x0`）。p167 22/24：
// MISSING = ORIG 额外一次 `vector<int>::begin()`+`C1IPiE`（ORIG 0x08664fe8 段内层
// for-init 重复发射，4.4 恒单次，语义等价）。context 自增：`context += 4` /
// `context = context + 4` / `&context[4]` / `(char*)((int*)context+1)` 四形 4.4
// 均折 `add DWORD PTR [ebp+0xc],0x4`（P322② 同族、形参无 volatile 位点），ORIG
// `lea edx,[eax+4]; mov [ebp+0xc],edx` 不可造 ⇒ N 封顶。
// [b28d-solo-CUserMissingA2③] 本轮重核（dual_rank gap 16）⇒ 修真缺口 + gap 16→3：
// ORIG 初始化段（0x08664f53/0x08664f6e/0x08664f89/0x08664fb6）求外层 begin/end 与
// 内层 begin/end 各一次，此后**每轮外层迭代只重求内层 begin**（0x0866505a 段，位于
// ++ri 之前；内层 end 不再重求）⇒ 原实现把内层 begin+end 都写在体内，静态少 13 条。
// [④ 2026-09-09] 进一步复现 ORIG 块序：内层条件在体外（0x08665044）+ `ppEv` 在体内
// 末尾（0x08665039），begin 重求位于**内层循环之后、外层 ++ri 之前** ⇒ 源形改为
// `while (ii != ie) { …; ++ii; } ii = elixirIds->begin();`（原 for-init 形把 begin
// 重求放外层体开头，difflib 报整块移动）。改后行级 diff 循环段逐行对齐，仅剩 call
// 地址（.o 未重定位）差异。
// 残差 3 条 = 三处 context 形参自增（4.1 `mov;lea;mov;mov` 5 条/处 vs 4.4
// `mov;mov;mov;add mem` 4 条/处）；/tmp/probe_ctx{,2,3,4,5,6,7,8,9,10}.cpp 十组探针
// （4.1.2 与 4.4.7 双编译）穷举 `context++`/`+=4`/`=c+4`/`&c[4]`/`(int)c+4`/`*p++`/
// 引用 cast/逗号形，两侧均不出 `lea 0x4(%eax),%edx; mov %edx,0xc(%ebp)` 写回形
// ⇒ 跨版本 add-vs-lea 结构性差异（N1/N107 族），语义等价（三处均 +4 推进）。
// p167 24/24（MISSING/EXTRA 仅 `__normal_iterator` C1↔C2 同址别名，P262）。
#include "CUserMissingA2/_fnStatQuestClear.inc"
}

// ----------------------------------------------------------------------------
// ORIG _ZL14elixir_item_id（df_game_r .rodata 8×int；值 1039..1046 实证）。
// 精灵药水道具 id 表，calc_quest_stat 的匹配键集。
// ----------------------------------------------------------------------------
namespace
{
const int elixir_item_id[8] = { 1039, 1040, 1041, 1042, 1043, 1044, 1045, 1046 };
}

// ----------------------------------------------------------------------------
// ORIG T 0x08664c50 _ZN5CUser15calc_quest_statER15_Additioal_info：
// 精灵药水表→count_specific_items 统计持有；UserQuest+4 的 CQuestClear 经
// enumQuestClear(_fnStatQuestClear, this, ids, rewards) 收集可完成奖励；
// complete_reward_list 按持有补齐；逐项 apply_elixir_item 到 _Additioal_info。
// ----------------------------------------------------------------------------
// [b28d-27] DIFF N82：ORIG 0x08664c50；p167 CLEAR（18/18），残差为帧/槽分配，
// 语义闭合。
// [b28d-solo-CUserMissingA2] 修复⇒AE：qc 表达式内联进 enumQuestClear 实参
// （ORIG 0x08664c9e `lea 0x4(%eax),%edx`，无具名局部）⇒ 消 -0x14 槽、全帧槽号
// 与 ORIG 对齐，DIFF→AE（p167 CLEAR 18/18）。
#include "CUserMissingA2/calc_quest_stat.inc"
}
