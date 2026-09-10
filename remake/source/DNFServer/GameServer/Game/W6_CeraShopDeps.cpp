// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 W6_CeraShopDeps/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— WongWork::CCeraShop 12 个 ORIG-only 符号批次
//   （Wave6 / W6_CeraShopDeps 独占 TU；报告目录 docs/class_func_reports/WongWork__CCeraShop/）
// 符号清单（nm -S 核实于 df_game_r；所有反汇编/反编译依据逐一读取）：
//   CallSpecialItemHandler(int,STSpecailItem_Param const&,STSpecailItem_Result&)
//                                                       0x08325854 (0x2a)
//   LogError(stErrorLogInput_t const&) const            0x0832033a (0x13f)
//   _ProcessIPGError(CUser*,SIPGData*,SIPGData*,hijiic) 0x083237f6 (0x182)
//   _processCoinGift(CUser*,ushort,uint,int)            0x0832619c (0x195)
//   _processCoinGift(CUser*,ushort,uint,int,int)        0x08326332 (0x1a4)
//   _processItemGift(CUser*,Inven_Item,uint,int)        0x083264d6 (0x112)
//   _processAvatarGift(CUser*,i,i,c,c,PKc,j,i)          0x08326036 (0x166)
//   _processCreatureGift(CUser*,Inven_Item,uint,int,int,int)
//                                                       0x083265e8 (0x151)
//   ProcessBuyRequest(CUser*,MSG_BUY_CERASHOP_ITEM const&)
//                                                       0x0832047a (0x18b3)
//   ProcessIPGUserData(CUser*,SIPGData*,SIPGData*,Stream*)
//                                                       0x8321d2e (0x1ac7)
//   _CheckValidCount(CCeraShopGoods const**)            0x08323af4 (0x435)
//       → 定义位于 CCeraShop_Special.cpp；本批次修正其二级指针签名以匹配
//         ORIG mangled EPPK14CCeraShopGoods（原 PKPK… 不产 ORIG 符号）。
//   Is2010ChuseokPacket(unsigned int)                   0x083280dc W
//       → 同上；参数类型 ulong→uint（ORIG mangled …Ej）。
// ============================================================================

#include "CCeraShop.h"
#include "GlobalData.h"
#include "SigTypes.h"
#include "SpecialItemHandler.h"
#include "CStreamGuard.h"
#include "CCeraShopBonusItemEvent.h"   // 权威声明（P104 ODR 收敛，2026-08-29 EventManagerWave）
#include "Stream.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "InterfacePacketBuf.h"
#include "PacketTypes.h"
#include "CDataManager.h"
#include "CItemList.h"
#include "CItemLimitEdition.h"
#include "CEquipItem.h"
#include "CUser.h"
#include "DB_UpdateGiftCount.h"     // DB_UpdateGiftCount / DB_UpdateBreakAwayAccruedCera 权威声明
#include "CInventory.h"
#include "CMailBoxHelper.h"
#include "GameWorld.h"
#include "CGuildServerProxy.h"   // CEventManager / CEventBase 权威声明
#include "CEventMgr.h"
#include "CGenUniqueNo.h"
#include "LogManager.h"
#include "CCirculationStatistic.h"
#include "RDARScriptStringManager.h"
#include "TaiwanCash.h"          // WongWork::IPG::CIPGHelper 权威声明
#include "CSystemTime.h"
#include "CEnvironment.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <algorithm>

#include "BreakAwayPreventSystem.h"   // break_away_prevent::CBreakAwayPreventSystem 权威声明


// ORIG T @0x0831ce5e [updateUsedCera.md]：仅转发 DB_UpdateBreakAwayAccruedCera::
// makeRequest；不访问任何成员字段，调用视图本地布局即 ABI 安全。
#include "W6_CeraShopDeps/updateUsedCera.inc"
}

// DB_UpdateGiftCount/DB_UpdateBreakAwayAccruedCera 权威声明（含 makeRequest/dispatch
// 与生命周期实现）见 DB_UpdateGiftCount.h（本批次新建 TU，已登记 CMake note）。

// WongWork::CWebEvent 权威声明现位于 GlobalData.h（ReqSaveEventItem ORIG T 0x086c1910）。

namespace WongWork
{

namespace
{
const int kBuyEntryStride = 0x1cf;   // MSG 条目区步长（ProcessBuyRequest imul 实证）
}  // namespace

// SIPGData 为 char 镜像（TaiwanCash.h）；按偏移读形。P49：-O0 下 inline helper
// 会产真实 call 符号（仅NEW 发明符号违规），统一用宏直发 ORIG 的内联寻址形。
// AtoiAt/ReadIntAt 语义字段化由后续 IPG 批次收口【推断】。
#define AtoiAt(data, off) atoi(reinterpret_cast<const char*>(data) + (off))
// [b28d-S278 P319] 整型加法+回 cast：复现 4.1 `add $K,%eax; mov %eax,%reg`
// 两步形（指针算术 4.4 恒折单条 `lea K(%eax),%reg`；探针 /tmp/lg_probe4.cpp fA/fB）。
#define BufAt(data, off) ((char*)((unsigned int)(data) + (off)))

// ---------------------------------------------------------------------------
// CallSpecialItemHandler —— ORIG T 0x08325854 (0x2a)
// 薄转发：m_handler->execute(type,param,result)。
// ---------------------------------------------------------------------------
#include "W6_CeraShopDeps/CallSpecialItemHandler.inc"
}

// ---------------------------------------------------------------------------
// LogError —— ORIG T 0x0832033a (0x13f)
// StreamPool 取流 → guard.operator*() 两次分别 operator<<(0x32e)(int 形参) 与
// operator<<(input[0])(unsigned int 形参，ORIG 0x080e2292) → SIG ctor
// (0x08328110 清零) + 7 字段按 ORIG 写入序（+0/+4/+0x14/+0x10/+8/+0xc/+0x18）→
// guard.operator->()->put_binary(&sig,0x1c)（P103 链保留）→ put(DB_Q) → 返回 1
// （P159 尾部 mov $1 跨 D1 保活）。
// ---------------------------------------------------------------------------
// [b28d-solo-W6_CeraShopDeps] verdict=AE（check_tu_routed DIFF→AE；修法=P248 档③
// 字段同型化：stErrorLogInput_t::m_field0 与 SIG_CERASHOP_ERROR_LOG::m_field0
// int→unsigned int，消 4.4 的 `mov (%eax),%eax; mov %eax,%ebx` 两段，复现 ORIG
// 0x08320394 单段直落 callee-saved。p167 CLEAR）。
#include "W6_CeraShopDeps/LogError.inc"
}

// ---------------------------------------------------------------------------
// _ProcessIPGError —— ORIG T 0x083237f6 (0x182)
// 【§6.2】P167 差集归零；AtoiAt/ReadIntAt/BufAt 已宏化（P49），109 vs 105
// 残余为寻址折叠/寄存器形态（N7/N18b 族）。
// resData->status(+0x1c)==2 且 atoi(resData+0x173)==1 时走 helper 取消链；
// 否则 logFormat "IPG ERROR (Goods No: %d)(%s)"；末尾 user!=0 转 ProcessError。
// ---------------------------------------------------------------------------
// [b28d-solo-W6_CeraShopDeps] verdict=DIFF-N 封顶（orig 109 / our 110，gap=1；
// p167 CLEAR 3/3）。本轮修法：P59/P222 分支极性翻转（ORIG 0x08323837 `je <user==0
// 块>`，then 为 9 参重载 slot7、else 为 2 参重载 slot9 + return 直跳 epilogue）、
// P235 具名成员访问（resData->m_type 消 ReadIntAt 宏的 add+mov）、P319 整型加法
// BufAt 复现 add+mov 并使虚指针回落栈槽（与 ORIG `call *-0x28(%ebp)` 同形）、
// P223 负条件 goto 尾空标签复现 4.1 if-join 岛（`call ProcessError; jmp; nop`）。
// 残余唯一 1 条：logFormat 0x177 实参 4.4 恒插 `mov %eax,%edx` 中转
// （int 形参/const char* cast 两形实测不消）⇒ 语义等价，byte 层 N 封顶。
#include "W6_CeraShopDeps/_ProcessIPGError.inc"
}

// gift 族（coinGift×2/itemGift/avatarGift/creatureGift）ORIG 均内联
// memset 0x100（rep stos 慢路）+ getCurCharacName/findString/sprintf 三调用，
// 本批删除 FormatGiftMessage/GiftHistoryLog helper（P49/§4.1 发明符号），逐
// 函数内联复刻 ⇒ P167 差集归零；coinGift 族余 gap≈N78 rep 段（28 vs 5 insn）。
// ---------------------------------------------------------------------------
// _processCoinGift —— ORIG T 0x0832619c (0x195)：四参重载
// 代币物品 idx=1、count=currencyType 构造 Inven_Item → 赠送邮件 + 统计日志。
// ---------------------------------------------------------------------------
// [b28d-solo-W6_CeraShopDeps] verdict=AE（DIFF→AE）。修法：①P28 声明序——item 先、
// msgBuf 后（ORIG 0x0832619c rep stos 在 ResetItemAttr call +0x041 之后 +0x046）；
// ②真 bug：SendCeraShopGiftStatisticLog 第 3/4 实参应取 item.m_addInfo(-0x43) /
// item.m_addInfo2(-0x3e)，原写常量 1L/currencyType（数据源取错，P92 族）。
#include "W6_CeraShopDeps/_processCoinGift.inc"
}

// ---------------------------------------------------------------------------
// _processCoinGift —— ORIG T 0x08326332 (0x1a4)：五参重载（count 选用赠品种类）
// kindFlag：count==0xec→4、count==0xca→5、否则 0。
// ---------------------------------------------------------------------------
// [b28d-solo-W6_CeraShopDeps] verdict=AE（DIFF→AE）。修法同四参重载：P28 声明序
// （ORIG 0x08326332 rep stos 在 ResetItemAttr 之后）+ 统计日志第 3/4 实参改
// item.m_addInfo(-0x54)/item.m_addInfo2(-0x4f)（原写 count/currencyType）。
#include "W6_CeraShopDeps/_processCoinGift_1.inc"
}

// ---------------------------------------------------------------------------
// _processItemGift —— ORIG T 0x083264d6 (0x112)
// 入参为按值 Inven_Item（调用方已完成拷贝）；giftKind 作邮件参数与统计额。
// ---------------------------------------------------------------------------
#include "W6_CeraShopDeps/_processItemGift.inc"
}

// ---------------------------------------------------------------------------
// _processAvatarGift —— ORIG T 0x08326036 (0x166)
// deadline = now + periodDays*86400（periodDays∈{0,0x16d} 保持 0）→ avatar 赠送邮件。
// ---------------------------------------------------------------------------
#include "W6_CeraShopDeps/_processAvatarGift.inc"
}

// ---------------------------------------------------------------------------
// _processCreatureGift —— ORIG T 0x083265e8 (0x151)
// 宠物赠送：ITEM_TYPE_CREATURE_EQUIP(5) 走 ReqDBSendNewCreatureMail，
// 其余走 ReqDBSendNewMailCashShop。
// ---------------------------------------------------------------------------
#include "W6_CeraShopDeps/_processCreatureGift.inc"
}

// ---------------------------------------------------------------------------
// ProcessBuyRequest —— ORIG T 0x0832047a (0x18b3)
// MSG 条目逐个 FindGoods → 非 GM 走 sticker/gift 禁售检查 → GM/常规路径查
// GetRelatedItem/PVP 禁购清单/_CheckBuyingCondition(0x8323f86) →
// _CheckValidCount 通过后重置 UsedCera 并逐条折扣计价、64 位余额扣减
// （std::max<long long>(0LL,·) 夹紧）→ 组装 stCeraShopIPGRequest_t 经
// CStreamGuard 发往 s_pIPGHelper 的 IPGOutputSimulation(+0x18)/IPGOutput
// (+0x14)（ceraPrice==0 分支）。控制流、错误码、cMyTrace 行号与字符串均照
// 报告 /tmp/pbr_report.md（ORIG 汇编 + Ghidra C）逐块转写；MSG 访问一律
// 经 SigTypes_A.h 具名字段视图（条目步长 0x1cf，P161 组件视图）。
// 说明【忠实度局限】：0x8323f86 六参校验在 ORIG 以 6 数据槽装载
// （goodsNo / itemIdx:stackCount 组合 long / expiry / byte37 / byte38sext），
// 对齐现存 CCeraShop_Special.cpp 实现体的头声明，末参 e 以 0 补位。
// 【§6.2 验收标注】orig=1551 our=1562；P167 调用点差集归零（仅余 N78 三处
// rep stos→memset call 差：deadPad/priceStr/serialBuf 清零区）；其余 |gap|
// 为 N82 帧/槽号与 4.1/4.4 条件物化（sete/setne、块排布）残余 ⇒ 语义完成、
// byte 层 N 封顶，勿再耗轮次。
// ---------------------------------------------------------------------------
// [b28c-L7][SIGFIX:WongWork::CCeraShop::ProcessBuyRequest] int→bool（判词见
// CCeraShop.h:153）；体内两处 return 值实测仅 1（无错误码语义）。
// [b28d-solo-W6_CeraShopDeps] verdict=DIFF-N82 封顶（orig 1551 / our 1588，
// gap=37=2.4%；p167 CLEAR 127/127）。本轮修法：P0/P227 全表达式临时链
// （NumberToString 结果无槽、itemCount 在 cMyTrace ctor 前求值）、P214③
// goodsList/relatedItems 数组零初始化复现 ORIG 两段 rep stos、P16 早返形
// （firstGoods==0 错误块直落 + jmp 尾）。残余：帧 0x43c vs ORIG 0x40c 及全帧
// 槽号平移（N82：ORIG 给 12 个 cMyTrace 各占独立 0x10 槽，4.4 复用同槽）
// + N7 间接调用目标寄存器 + 跨版本寄存器/寻址形态 ⇒ §6.2 三件套判语义完成、
// byte 层 N 封顶。
#include "W6_CeraShopDeps/ProcessBuyRequest.inc"
}

// ---------------------------------------------------------------------------
// ProcessIPGUserData —— ORIG T 0x8321d2e (0x1ac7)
// 【§6.2 验收标注】orig=1564 our≈1634；P167 调用点差集归零（含本批补全：
// _ProcessIPGError 双调用点、creature 双 expiration/usable、get_acc_id 现场、
// ChildrensDay period 双调用、GetRepeatEvent 双调用、make_pair<ulong&,uchar&>
// 转换形、const_iterator 显式转换对、m_itemList 成员直读、opt push_back 直引）。
// we-bigger 残余为 4.4 EH/cleanup 物化与 N82 槽号形态。
// IPG 结果回包处理：resData->status(+0x1c) 4=回填 cera、0=状态包、2=业务结算
// （avatar/coin/item/item-special/creature/additional 六路分派）→ 成功路径
// 扣款与响应包 0x43/0x35/0xe 广播 + ChildrensDay 补发 + Bonus 概率奖励。
// ---------------------------------------------------------------------------
// [b28d-solo-W6_CeraShopDeps] verdict=DIFF-N82 封顶（orig 1564 / our 1627，
// gap=63=4.0%；p167 CLEAR 192/192）。本轮修法：删 bSend 标志局部（ORIG 无该槽：
// type==4/0 处理完直落尾部 send 块 08321ea3/08321f0d jmp 08323791，type==2 由
// success 的 bool 归一化决定 08323755，user==0 早返 08321d5b je 083237dc）、
// P235 具名 m_type 消 add+mov。残余：帧 0x31c vs ORIG 0x2cc + 全帧槽号平移（N82，
// difflib 大面积窗口错位）+ 4.4 EH cleanup 物化（N91/N104 族）⇒ §6.2 三件套判
// 语义完成、byte 层 N 封顶。
#include "W6_CeraShopDeps/ProcessIPGUserData.inc"
}

}  // namespace WongWork

// ---------------------------------------------------------------------------
// ORIG W 0x08179fb4 _ZNK5CItem21isCerashopPackageItemEv：逐指令转写。
// 【翻案 b25c】旧 N 标注（setcc-fold）过时：单 return 短路链（P45）探针
// /tmp/probe_short.cpp q2 定论 4.4 与 ORIG 逐字同构（cmp;je 共享真岛 +
// cmp;jne + mov$1/mov$0），if-return 链才折 setcc。26/26 全等。
// 【b28c-36 复验】fresh ndiff=16 / NEAR（26=26 指令数）：残差 2 处全为
// **N7 间接 call 目标寄存器**——ORIG `mov (%eax),%edx; mov this,%eax; call *%edx`
// （目标 %edx/this %eax），4.4.7 恒 `mov (%eax),%eax; mov this,%edx; call *%eax`
// （目标 %eax/this %edx）。P230 补：4.4.6 后端间接 call 目标用 %edx（与 ORIG 同形），
// 本波禁自切路由，待 Main 三态后端 sweep 后按 N7 敏感族复测。源码形无解。
// 经 vtable 槽 +0x0c 虚方法（子类型索引，与 CI_SubType 先例同型）连续两次
// 调用，返回值 ==0xe 或 ==0xf 判定礼包类目 → true，否则 false。
// ---------------------------------------------------------------------------
#include "W6_CeraShopDeps/isCerashopPackageItem.inc"
}
