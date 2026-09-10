// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CPartyOnlyOrig/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// CPartyOnlyOrig.cpp —— CParty「仅 ORIG」批次（Wave5 OnlyOrig_APSEJ 分域，
// worklist /tmp/only_orig/CParty.txt 47 符号，2026-08-27）
//
// 全部函数逐份完整读取 docs/class_func_reports/CParty/<Func>.md（## 汇编 +
// ## 反编译 C）转写；无独立报告的叶子依赖按 ORIG nm/objdump 转写并标注。
//
// thunk 解析（报告汇编中被 strip 的 W 内联符号，经 df_game_r nm 核实）：
//   0x080da37c CUser::get_unique_id() const
//   0x080da27e CUserCharacInfo::getCurCharacInvenR() const
//   0x080da2b8 CUserCharacInfo::get_charac_level() const
//   0x080da2da CUserCharacInfo::IsCurCharacGhost()
//   0x08110dc2 CUser::GetPCRoomNo()
//   0x0819a8a6 CUser::getCurCharacQuestR() const
//   0x0822d0d8 CBattle_Field::get_dungeon_index()
//   0x080f981c CBattle_Field::get_dungeon_diff()
//   0x085bf3d8 CPartyResultRecvFlag::SetRecvFlag(int)
//   0x085bf406 CPartyResultRecvFlag::IsRecv(int) const
//   0x085bf6e0 CParty::reset_recv_flag()
// 依赖唯一 owner（链接期解析，本 TU 不重复定义）：
//   CBattle_Field::{kill_blood_monster,DestroyPassiveObject} —— CBattle_Field.h
//     已声明（BF 域批次落地定义）；CUser::Send(PacketGuard&)（CUser 域批次）；
//     CUser::CheckLimitMiniumAge(int,int&)（PartyUserLink 批次）。
// ============================================================================
#include "CParty.h"

#include "CDungeon.h"              // m_entryRequiredItem/get_dimension_possible/
                                   //   getLimitCoinDiff/m_87c/m_898
#include "CBattle_Field.h"         // BF 视图转发（先例：CParty.cpp checkBossRoom）
#include "CDataManager.h"          // G_CDataManager()/find_dungeon/get_dimensionInout
#include "CInventory.h"            // check_item_exist/GetInvenSlot
#include "SECRET_SHOP_DATA.h"      // SECRET_SHOP_DATA::m_info（GetUserSecretShop 具名视图）
#include "PacketGuard.h"            // PacketGuard（InterfacePacketBuf 子类）
#include "TimerQueue.h"             // G_TimerQueue/InsertTimer/gen_timer_key 支持
#include "CGuildServerProxy.h"     // CEventBase/CEventManager::GetRepeatEvent→IsEventing
#include "Quest.h"                 // m_index/m_triggerType/m_triggerSubType
// HistoryLog::WriteUseCoin 以本 TU 最小视图接入（先例 WarRoom.cpp：权威
// HistoryLog.h 经 CGameManager.h 的 include 链与本 TU 冲突；mangled 一致）。
class HistoryLog
{
public:
    static void WriteUseCoin(_IO_FILE* file, const char* reason);
};
#include "cMyTraceNoop.h"
#include "DataManagerInitDepends.h" // TOD_UserState/TOD_Layer

namespace
{

// +0xb14 BattleData 逐成员已用硬币数组具名成员视图。
inline int* memberUsedCoin(CParty* party)
{
    return party->m_battleData.m_arr7e8;
}

// +0x1dc CPartyResultRecvFlag 接收标志区视图。ORIG 三处 receiver 取址
// （0x085b3f58 / 0x085b3f85 / 0x085a464）均为 `mov this,%eax;
// lea 0x1dc(%eax),%edx`；4.4 对「视图 cast + 具名成员」产同形 lea，对
// `reinterpret_cast<CPartyResultRecvFlag*>(&m_pad1dc)` 取址恒产 add（N107）
// ⇒ 以视图具名成员表达 receiver（探针 /tmp/b28d_s187/p2.cpp m1..m3）。
struct CPartyRecvFlagView
{
    unsigned char m_pad1dc[0x1dc];
    CPartyResultRecvFlag m_recvFlag;   // +0x1dc
};

}  // namespace

// ============================================================================
// P2P 连接管理区
// ============================================================================

// ORIG T 0x0859a48c _ZN6CParty10ConnectP2PEP5CUserPci（ConnectP2P.md）。
// 队伍状态必须为 1（入队/出发态），否则抛 ENUM_ERROR(0x13)；
// 目标成员 IP 缓冲（cMember+0x0e 起 4 字节）复制 min(port,4) 字节并把
// cMember+0x0c 置 1；全员连接完成后清坏 P2P 成员。ip 参数仅作 memcpy 源
// （ORIG 未读其长度，端口参数被 min 截断使用【忠实转写】）。
#include "CPartyOnlyOrig/ConnectP2P.inc"
}

// ORIG T 0x0859a664（_ClearConnectP2PAbsolute.md）：除 slot 外全员中与该槽
// P2P 不通的逐个以 ENUM_PARTY_INFO_TYPE 2 离队；恒返回 1。
#include "CPartyOnlyOrig/_ClearConnectP2PAbsolute.inc"
}

// ORIG T 0x0859a6dc（_ClearConnectP2PRelative.md）：仅扫描槽位 2..3，
// 与 param 的 P2P 点数相等者离队；恒返回 1。
#include "CPartyOnlyOrig/_ClearConnectP2PRelative.inc"
}

// ORIG T 0x0859a7a2（_GetConnectP2PMaxMinPoint.md）：max=0/min=4 初值后，
// 对有效成员取 P2P 点数并收拢到 [min,max]。
#include "CPartyOnlyOrig/_GetConnectP2PMaxMinPoint.inc"
}

// ORIG T 0x0859a830（ClearBadP2PUser.md）：绝对清理两个全通槽位，再迭代
// 相对点数差于最差的相对清理。
#include "CPartyOnlyOrig/ClearBadP2PUser.inc"
}

// ORIG T 0x0859cea2（send_party_ipinfo.md）：header(0,0xb) 广播各成员
// uniqueId/内外网 IP/端口/账号/NAT 类型/MTU 与角色耐力字节，末尾
// finalize(true) 后 send_to_party。
// P157/P48：ORIG 每个使用点都重新走 m_member[i].m_pUser 取用户链（从不
// 缓存局部），getCurCharacR 在 then/else 臂各调用一次；uid/port 低 16 位
// 截断是 caller 侧 movzwl（用 static_cast<unsigned short>，非 & 0xffff）。
#include "CPartyOnlyOrig/send_party_ipinfo.inc"
}

// ORIG T 0x0859d258（send_invite_pvp.md）/ 0x0859d388（send_invite_warroom.md）：
// header(0,7)+短地图索引+邀请类型（pvp=2/warroom=3）+发起者 uid 低 16 位。
#include "CPartyOnlyOrig/send_invite_pvp.inc"
}

#include "CPartyOnlyOrig/send_invite_warroom.inc"
}

// ORIG T 0x085ba5d6（send_to_alive_member.md）：存活（user state==5 且
// +0x380 liveFlag 非 0）的成员直发。
#include "CPartyOnlyOrig/send_to_alive_member.inc"
}

// ============================================================================
// 成员查询 / 访问器
// ============================================================================

// ORIG T 0x085b66fa（isMyMember.md）：任一有效成员 uid 低 16 位命中即 1。
#include "CPartyOnlyOrig/isMyMember.inc"
}

// ORIG T 0x085b357c（GetCoinCount.md）：INT_MAX - 该成员 BattleData 已用硬币。
#include "CPartyOnlyOrig/GetCoinCount.inc"
}

// ORIG T 0x0859a1ae（get_total_level.md）：有效成员角色等级合计。
#include "CPartyOnlyOrig/get_total_level.inc"
}

// ORIG T 0x085b603e（getMemberMinLevel.md）：std::min 形态最小等级；
// 无有效成员时保持初值 INT_MAX。
#include "CPartyOnlyOrig/getMemberMinLevel.inc"
}

// ORIG T 0x085b687c（isSamePCRoomParty.md）：首个成员 PCBang 号缓存后逐一
// 相同；任一为 0 或不一致即 false，全部一致 true。
#include "CPartyOnlyOrig/isSamePCRoomParty.inc"
}

// ORIG T 0x085b8fa6（IsThereGhostInParty.md）。
#include "CPartyOnlyOrig/IsThereGhostInParty.inc"
}

// ORIG T 0x085b900c（getMemberPassParty.md）：param 为带新人入队的用户；
// 排除其师徒上端与本尊成员后取队伍最高等级。无人可排除返回 0；
// 最高级与新人 dungeon 前等级差 >6 时返回 1（不可加入），否则 0。
// b28d-solo-CPartyOnlyOrig 判词：**AE** @0x085b900c（clean 口径 ndiff=1，余为
// R_386_PC32 重定位显示差）。修法：level/maxLevel 声明序翻转（4.4 按首次使用
// 序，N93 标量例外）+ 删 `level = 0`（ORIG 只有 -0x14/-0xc 两条清零）+ 两比较
// 合并为单个 `||` 短路（复现 ORIG `mov $1/jmp/mov $0/test` 常量岛）+ 尾段按
// Ghidra C 的 `&&` 物化 + 重复零判形。
#include "CPartyOnlyOrig/getMemberPassParty.inc"
}

// ORIG T 0x085b9e80（GetUserSecretShop.md）：+0x2a8 + slot*0x1c 起的逐成员
// secret 商店数据视图；slot 出界返回空。
// b28d-solo-CPartyOnlyOrig 判词：DIFF→AE。修法：CParty.h +0x2a8 落
// POD 具名数组成员 `m_secretShopInfo[4][0x1c]`（R2 已广播，sizeof/偏移不变）
// ⇒ `&this->m_secretShopInfo[slot]` 复现 ORIG 的 28x 强度归约链与
// `lea 0x2a0(%edx),%eax; add this; add $8` 常量拆分；cast 取址形恒多
// `mov this,%ecx` 中转（探针 /tmp/b28d_s187/p11/p14/p18）。
#include "CPartyOnlyOrig/GetUserSecretShop.inc"
}

// ============================================================================
// 队伍状态 / 组队生命周期
// ============================================================================

// ORIG T 0x085ba680（switchPostion.md）：整个 cMember 槽（0x18 字节含
// m_pUser）对调。
#include "CPartyOnlyOrig/switchPostion.inc"
}

// ORIG T 0x0859b2b6（join_user.md）：空指针日志 -1；满员且 state==2 特判
// -2；找到空槽写入成员/userUid、回写 PartyIndex、清连击胜场并返回 1。
#include "CPartyOnlyOrig/join_user.inc"
}

// ORIG T 0x0859b404（change_manager_for_quick_party.md）：manager 本人以
// PARTY_INFO_TYPE 无关路径摘出（SetPartyIndex(-1) 并清槽），再选首个有效
// 成员为新 host/manager；找不到则记录 no manager 日志返回 0。
#include "CPartyOnlyOrig/change_manager_for_quick_party.inc"
}

// ORIG T 0x0859b52c（delete_party_for_quick_party.md）：末人解散入池销毁；
// manager 引换手；普通成员直接清槽；查无此人返回 1。
#include "CPartyOnlyOrig/delete_party_for_quick_party.inc"
}

// ORIG T 0x0859d656（CheckMemberArea.md）：以首个有效成员区域为基准逐一比较。
#include "CPartyOnlyOrig/CheckMemberArea.inc"
}

// ORIG T 0x085ba762（setMonsterMoveHack.md）：对全体成员记 0xd3 服务端反外挂计数。
#include "CPartyOnlyOrig/setMonsterMoveHack.inc"
}

// ============================================================================
// 结果接收 / 任务 / 副本入场券
// ============================================================================

// ORIG T 0x085b3f34（set_recv_party_packet_flag.md）：+0x1dc 结果接收标志区。
// b28d-solo-CPartyOnlyOrig 判词：DIFF→AE。修法：+0x1dc 视图具名成员
// receiver（`CPartyRecvFlagView::m_recvFlag`）复现 ORIG `lea 0x1dc(%eax),%edx`
// —— 4.4 对取址 cast 恒 add（N107），对视图具名成员产 lea。
#include "CPartyOnlyOrig/set_recv_party_packet_flag.inc"
}

// ORIG T 0x085b3f72（recv_party_packet_all.md）：全员均已回包才返回 true。
// b28d-solo-CPartyOnlyOrig 判词：DIFF→AE（+0x1dc 视图具名 receiver，
// 同 set_recv_party_packet_flag 修法）。
// b28d-45 recv_party_packet_all 0x085b3f72：已修 P248 档①（CPartyResultRecvFlag::
// IsRecv uchar→bool，0x085b3fc0 xor 段对齐）。
#include "CPartyOnlyOrig/recv_party_packet_all.inc"
}

// ORIG T 0x085b3fd8（CheckRecvFromAllMember.md）：登记本人标志并轮询全员。
#include "CPartyOnlyOrig/CheckRecvFromAllMember.inc"
}

// ORIG T 0x0859a34e（TimeoutCompleteAfterAssault.md）：+0x118 守卫非 0 时
// 复位并返回 0；为 0 时按未回包成员逐个踢出（trace 后 leave_user type2），
// 返回 1（ORIG 主路径不写 +0x118；调用方 CAssaultMgr 两处均 test %al 消费返回值）。
// b28d-solo-CPartyOnlyOrig 判词：DIFF→AE（+0x1dc 视图具名 receiver）。
// b28d-45 TimeoutCompleteAfterAssault 0x0859a34e：IsRecv bool 化（P248 档①）+
// 正向 if 包裹（P214①）后 85:85。
#include "CPartyOnlyOrig/TimeoutCompleteAfterAssault.inc"
}

// ORIG T 0x085a82d2（IsClearedQuest.md）：任一有效成员未清任务即 false。
// b28d-45 IsClearedQuest 0x085a82d2：FIX→AE —— Main 已落 QuestClear bool 链
// （STClearedQuest::m_cleared / CQuestClear::isClearedQuest / CUser.h:1270），
// 调用点改裸取反 `if(!isClearQuest(...))` 后 40:40 槽位集全等、xor 段逐条对齐；
// 余为 R_386_32 重定位显示差（链接后即 ORIG 绝对地址）。
#include "CPartyOnlyOrig/IsClearedQuest.inc"
}

// ORIG T 0x085b65a2（HasNoQuestGoWithApcInParty.md）：状态 5/10 的成员若未在
// 做与 APC 同行任务（triggerType==3 且 triggerSubType==0 判定配对）则 false。
// b28d-solo-CPartyOnlyOrig 判词：DIFF（N 封顶）@0x085b65a2。修法已落：删
// userQuest 局部槽（内联 getCurCharacQuestR()->IsDoingQuest）对齐 ORIG 直接
// 压参形；外层条件改正向 `==5 || ==10`。残余 N 封顶：4.4 对 `||` 第二条件后
// 恒物化 `mov $1/$0; test %al,%al` 岛，4.1 为 `je body / jne continue` 直跳
//（探针 f1..f6 + 本轮 p19 f1..f5：正/负极性、嵌套 if、goto、switch 五形全产
// 物化岛）。语义等价。
#include "CPartyOnlyOrig/HasNoQuestGoWithApcInParty.inc"
}

// ORIG T 0x0859e7fe（CheckEnterAncientDungeon_0859e7fe.md，单参别名版）：
// dungeon 未开组队入口时做 ancient 入场券检查；否则按 4 参事件版判定。
// 残差归因（N 类）：ORIG 将 itemIdx 读入 %esi 跨 getCurCharacInvenR 调用
// 缓存（4.4 恒为 call 后现场重读，P75 outgoing-arg 装载时刻族）、i 槽 -0xc
// 与 items 基 -0x111（4.1 帧内 4B 交错，P112）、循环头双 movl $0 初始化
// 复制（4.1 tail-duplicator 产物）均无法由合法源码复现。
// b28d-solo-CPartyOnlyOrig 判词：DIFF（部分修复，残余 N 封顶）@0x0859e7fe。
// 本轮修法：①`while (slots[i] == -1) { return 0; }` 复现 ORIG 0x0859e97c
// `cmp $-1; sete %al; test %al,%al; je`（4.4 的 `if` 直形恒折 `jne`，探针
// p22 g6）；②`int i = 0;` + `for (i = 0; ...)` 双初始化复现 ORIG 双
// `movl $0,-0xc`（P191）。残余 N 封顶：ORIG 第二处 `slots[i]` 读复用 %ebx
// （i 缓存）vs 4.4 恒重读 `-0xc(%ebp)`（+1 条，N7/N18a 寄存器分配族；register
// 提示反致槽漂移 -0x1c→-0x28，已回退）。**报告撞名警示：
// docs/class_func_reports/CParty/CheckEnterAncientDungeon.md 是 4 参重载
// 0x0859e528 的内容，本件 ORIG 基线取自二进制 nm 切片 0x0859e7fe..0x0859eac2。**
// 修法已落：`!= 1` 取反形复现 ORIG `xor $1; test; je`（P194；CheckEnterEventDungeon
// 返回 bool）；Main 代落 RequiredItem::m_enableFlag char→bool 后
// `!m_enableFlag` 段与 ORIG xor 形对齐。
#include "CPartyOnlyOrig/CheckEnterAncientDungeon.inc"
}

// ORIG T 0x085be56c（checkLimitUsedCoinCondition.md）：party/dungeon 双层
// 硬币限额校验；超限返回 0x16、找不到副本返回 0x13、通过返回 0。
// 形参直写回 0xc(%ebp)（ORIG +0x34 `mov %eax,0xc(%ebp)`：find_dungeon 结果
// 直接覆写形参槽，无本地别名副本）；diff 以内联实参入 getLimitCoinDiff。
// b28d-solo-CPartyOnlyOrig 判词：**AE** @0x085be56c（clean 口径 ndiff=0，102:102）。
// 修法：P225 拆嵌套 if 复现 4.1 每条件 `setne/setge %al; test %al,%al; je`
// 物化（合并 `&&` 在 4.4 恒折 jl/jne 直跳，探针 p1..p4）+ Main 代落
// CDungeon::getLimitCoinDiff int→bool（ORIG `test %al`）+ 本 TU
// CheckHellCoinLimint int→bool（ORIG `call; xor $1; test %al`）。
#include "CPartyOnlyOrig/checkLimitUsedCoinCondition.inc"
}

// ORIG T 0x085be4ae（checkUnLimitUsedCoinCondition.md）：type 0x5e 重复事件
// 处于进行中且副本开启维度次数限制时豁免硬币上限。
#include "CPartyOnlyOrig/checkUnLimitUsedCoinCondition.inc"
}

// ORIG T 0x085a834a（CheckHellCoinLimint.md）：hell 场景且 +0xcd4 开关打开、
// 未清完全部 hell 组时，对照已用硬币数判断是否越过本副本 hell 限额。
// b28d-45 CheckHellCoinLimint 0x085a834a：FIX+N —— Main 已落
// IsKilledAllHellGruoups bool 链（调用点裸取反后 xor 段对齐）；IsEnableHellDungeon
// 按 Main 口径维持 uchar（其体依赖被指字段 uchar，5 个 ORIG 调用点无判别力）⇒
// 本件 4.4 恒多 `setne` 归一段，N 封顶（29:31，语义等价）。
#include "CPartyOnlyOrig/CheckHellCoinLimint.inc"
}

// ORIG U 0x0859a210（is_limit_minimum_age.md）：任一成员不满足年龄下限即 0
// （param 输出违规成员索引由 CUser::CheckLimitMiniumAge 内部填写）。
// [b28c-L7][SIGFIX:CParty::is_limit_minimum_age] int→bool（判词见 CParty.h:671）
#include "CPartyOnlyOrig/is_limit_minimum_age.inc"
}

// ORIG U 0x0859a286（is_limit_minimum_level.md）：等级不足者以 uid 回填 param。
// [b28c-L7][SIGFIX:CParty::is_limit_minimum_level] int→bool（同族，ORIG 出口
// `mov $0x0/$0x1,%eax`；mangled 不变）
#include "CPartyOnlyOrig/is_limit_minimum_level.inc"
}

// ORIG T 0x085bcab6（CheckMemberLastTryAssaultTime.md）：任一成员 600 秒内
// 有混沌击杀时间记录即 1（禁止立即组队袭击）。
#include "CPartyOnlyOrig/CheckMemberLastTryAssaultTime.inc"
}

// ORIG T 0x0859bfe2（increase_dimension_inout_count.md）：副本允许维度进出时，
// 各维度 inout 槽 (dm 上限, 成员现值) 同加 delta 并截断到 dm 上限。
#include "CPartyOnlyOrig/increase_dimension_inout_count.inc"
}

// ORIG T 0x085a2320（GetExpPenaltyPercent.md）：等级差惩罚百分比表。
#include "CPartyOnlyOrig/GetExpPenaltyPercent.inc"
}

// ============================================================================
// 副本流程 / 结算
// ============================================================================

// ORIG T 0x085a2246（RestartDungeon.md）：复位战场后以原 idx/diff 重开局。
#include "CPartyOnlyOrig/RestartDungeon.inc"
}

// ORIG T 0x085a7786（isMonsterDrop_item.md）：场上存在该掉落且数量非 0 才算 0。
// b28d-solo-CPartyOnlyOrig 判词：DIFF→AE。修法：map_item::m_count uchar→bool
// （WarField.h，R2 已广播）+ 裸取反 `!item.m_count` ⇒ 复现 ORIG
// `movzbl -0x60; xor $1,%eax; test %al,%al; je`（P248 档②；uchar 字段下
// `(m_count^1)!=0`/`!=1` 七写法探针全折 `cmpb $1`）。
#include "CPartyOnlyOrig/isMonsterDrop_item.inc"
}

// [N-类·帧布局] CParty::VoteLackUser（本 TU 声明对应定义在 CPartyDepsD2）
// 剩 31 全为槽文本差（4.4 帧底保留区致 i/temp/iterator 槽整体深 4B）+
// ebx/esi 角色互换（P78 禁追），语义已双证正确，勿再投预算。

// ORIG T 0x085a3b6a（kill_blood_monster.md）：血轮击杀转发战场层。
#include "CPartyOnlyOrig/kill_blood_monster.inc"
}

// ORIG T 0x085b6540（DestroyPassiveObject.md）：摧毁被动物件并在触发清除条件
// 时结算整场。
// b28d-solo-CPartyOnlyOrig 判词：DIFF→修复 @0x085b6540（gap 1→0）。4.4 对普通
// bool 局部条件恒折 `cmpb $0,-0x9(%ebp); je`，ORIG 4.1 为 `movzbl -0x9(%ebp),%eax;
// test %al,%al; je`（P322② volatile 强占形）。探针 /tmp/probe_hnq/dp3.cpp v9
// （`volatile bool destroyed` + `if (destroyed)` + `return (unsigned char)destroyed`）
// 逐条复现 ORIG 29 条（含尾部 movzbl 返回）；普通 bool（v11）仍产 cmpb。
// 旧判词「uchar/bool/int 局部 × 五种条件形全产 cmpb」（探针 d1..d5/p23 f1..f5）
// 对 volatile 形不成立，已作废。
#include "CPartyOnlyOrig/DestroyPassiveObject.inc"
}

// ORIG T 0x085b9f28（SetRidableObjectState.md）：坐骑状态转发战场层（返回值穿透）。
#include "CPartyOnlyOrig/SetRidableObjectState.inc"
}

// ============================================================================
// TOD / 血轮复活
// ============================================================================

// ORIG T 0x085ba906（OnTODRewardTimer.md）：0x2c 报文发送 TOD 结算奖励包，
// EPLP 置 2、注册 0x15 层推进定时器后回到 state1，并注册 8 秒后的 0x2c 重投递。
#include "CPartyOnlyOrig/OnTODRewardTimer.inc"
}

// ORIG T 0x085a22a8（SelectEPLPCommand.md）：EPLP 选令：置状态 3 并按指定秒数
// 注册 0x14 定时器。
#include "CPartyOnlyOrig/SelectEPLPCommand.inc"
}

// ORIG T 0x085baa06（_Send_TOD_ClearReward.md）：0x15b 包——管理者的 layer
// 扩展数据最后进入层 + 最多 9 组奖励清单（不足补 -1/0）+ 特殊奖励索引/数量。
// b28d-solo-CPartyOnlyOrig 判词：DIFF（N 封顶）@0x085baa06。修法已落：具名
// userState 指针落槽（P0 槽往返）+ 匿名 sret 全表达式链（P278）+
// `m_battleData.m_field0` 具名成员访问 ⇒ packet 槽 -0x24 与 ORIG 对齐。
// 残余 N 封顶：4.1 `mov 0x330(%eax),%edx` 冗余装载（N18a）与首循环前
// `call size; cmp $9` 死比较（4.1 tail-duplicator 产物）。语义等价。
#include "CPartyOnlyOrig/_Send_TOD_ClearReward.inc"
}

// ORIG T 0x085b27c8（ReviveUser.md）：state2/EPLP 下按 uid 找死亡成员；
// 免费复活条件不满足时扣操作者 BattleData 硬币（写使用日志），随后原地复活。
#include "CPartyOnlyOrig/ReviveUser.inc"
}

// RidableObject::clear 定义置于本 TU（与 CDungeon.cpp 中的 C1 调用者分离）：
// 4.4 unit-at-a-time 对 TU 内可见 callee 塌缩 outgoing 帧（C1 sub $0x4），
// ORIG 4.1 恒发 sub $0x18（探针 /tmp/nl4p/p6.cpp vs p7.cpp 同族实证）。
#include "CPartyOnlyOrig/clear.inc"
}
