// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CTradeSpace/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include "GameRand.h"
// ============================================================================
// df_game_r 还原 —— CTradeSpace（交易空间，G2-4 批次）
// 参照 docs/class_func_reports/CTradeSpace.md + ORIG 弱符号反汇编。
// 布局（ORIG CTradeSpace::CTradeSpace 0x082a416c / SetIDX 0x082a41f4 推导）：
//   +0x00 Mutex（0x18）          +0x18 int m_index
//   +0x1c char m_bTrading        +0x1d char m_bLocked
//   +0x20 CUser* m_pTraders[2]   +0x28 int m_nState[2]
//   +0x30 TradeSlot m_slots[2][27]（每槽 0x48，每行 0x798）
//   +0xf60 int m_nMoney[2]        +0xf68 int m_fieldF68[2]（生物转移计数）
//   （总尺寸 0xf70）
// 本 TU 只做 CTradeSpace 自身（含 TradeSlot 构造/Reset），跨类调用以真实符号
// 声明（头文件已有者直接使用；缺失者用 asm-label extern）。
// ============================================================================

#include "CUser.h"
#include "CInventory.h"
#include "CDataManager.h"
#include "InterfacePacketBuf.h"
// 跨类符号：CUser/cUserHistoryLog(CUser.h)、CSecu_ProtectionField(CInventory.h)、
// Mutex(GameTypes.h)、CMailBoxHelper/G_CGameManager(CGameManager.h)、
// DB_AvatarChangeOwner(DB_AvatarChangeOwner.h)。
#include "CGameManager.h"
#include "DB_AvatarChangeOwner.h"
#include "CTradeSpace.h"   // 类/布局唯一声明点（CGameManager.h 亦 include）
#include "RDARScriptStringManager.h"   // RDARScriptStringManager/g_scriptStringManager_（CleanFAsm 新建）
#include "HistoryLog.h"            // HistoryLog::WriteTradeUp/Down（proceed_trade）
#include "CAuctionAveragePrice.h"  // CAuctionAveragePrice（proceed_trade 均价）
#include "CStreamGuard.h"          // CStreamGuard（零金币日志流）
#include "StreamPool.h"            // StreamPool::Acquire
#include "MsgQueueMgr.h"           // MsgQueueMgr::put
#include "GlobalData.h"            // s_stream_pool / s_msgq_mgr
#include "W6_HackPunishDeps.h"     // CAutoPunishRuleHackTypeMgr（checkTotalTradeRuleOver）
#include "SigTypes_E.h"            // SIG_SAVE_GOLD_ZERO_TRADE_LOG
#include "GameWorld.h"             // G_GameWorld/send_all（remove_item 广播）

extern Inven_Item g_emptySlot;   // ORIG 全局 0x943ddc0（Inven_Item.cpp 定义）

// DB_InsertCeraItemHistory（定义 DB_U3.cpp；本 TU 仅调用 makeRequest）
class DB_InsertCeraItemHistory
{
public:
    static int makeRequest(int itemIdx, unsigned int senderCharacNo,
                           unsigned int receiverCharacNo, char flag);
};

// ---- 交易状态枚举（ORIG ENUM_TRADESTATE；mangling 编码类型名 15ENUM_TRADESTATE）----
enum ENUM_TRADESTATE
{
    ENUM_TRADESTATE_0 = 0,  // 解除锁定
    ENUM_TRADESTATE_1 = 1,  // 锁定
    ENUM_TRADESTATE_2 = 2,
    ENUM_TRADESTATE_3 = 3,
    ENUM_TRADESTATE_4 = 4
};

// ---- set_traders @ 0x085295f0 ----
#include "CTradeSpace/set_traders.inc"
}

// SendChangeTradeState（本 TU 实现，ORIG 0x852f0a0）
#include "CTradeSpace/SendChangeTradeState.inc"
}

// get_rand_int（CItemList.h 已有 C++ 链接声明）
#include "CTradeSpace/get_rand_int.inc"



#include "CTradeSpace/TradeSlot.inc"
}

#include "CTradeSpace/Reset.inc"
}


// ---- 构造 / 析构（ORIG 0x082a416c / 0x082a41e0）----
// ORIG：m_bTrading(+0x1c) 直接初始化（在数组成员构造循环之前清零，
// 声明顺序 m_bTrading 先于 m_slotsData）；m_bLocked(+0x1d) 在 ctor 体末尾赋值。
#include "CTradeSpace/CTradeSpace.inc"
}
#include "CTradeSpace/dtor_CTradeSpace.inc"
}

// ---- 槽位辅助（P49：-O0 下 static inline 也产真实符号 → 宏）----
// 基于真实二维数组成员索引（m_slotsData[2][27]，行步长 0x798、槽步长 0x48）。
#define TradeSlotAt(slots, trader, idx) (&((slots)[trader][idx]))

// ---- 弱符号访问器族（ORIG 0x082a41f4 起）----
#include "CTradeSpace/SetIDX.inc"
}

#include "CTradeSpace/lock.inc"
}

#include "CTradeSpace/unlock.inc"
}

#include "CTradeSpace/IsLocked.inc"
}

#include "CTradeSpace/_SetLock.inc"
}

#include "CTradeSpace/is_empty.inc"
}


#include "CTradeSpace/get_index.inc"
}

// ---- reset @ 0x085297f2 ----
#include "CTradeSpace/reset.inc"
}

// ---- cancel_trade_by_dis @ 0x0852f086 ----
#include "CTradeSpace/cancel_trade_by_dis.inc"
}

#include "CTradeSpace/GetOppositeUser.inc"
}

// ---- _CheckTrade4SameAvatar @ 0x08529d48 ----
#include "CTradeSpace/_CheckTrade4SameAvatar.inc"
}

// ---- check_item_exist @ 0x08529a18 ----
#include "CTradeSpace/check_item_exist.inc"
}

#include "CTradeSpace/get_empty_itemslot.inc"
}
#include "CTradeSpace/check_trade_possibility.inc"
}

// ---- _IsTradable @ 0x08529dce ----
#include "CTradeSpace/_IsTradable.inc"
}

// ---- _SaveAvatarItemOwnerChange @ 0x0853084a ----
#include "CTradeSpace/_SaveAvatarItemOwnerChange.inc"
}

// ---- add_item @ 0x08529aa2 ----
#include "CTradeSpace/add_item.inc"
}

// ---- cancel_trade @ 0x0852effa ----
#include "CTradeSpace/cancel_trade.inc"
}

// ---- send_lose_item_for_china @ 0x0852e3a8 ----
// [b28d-solo-CTradeSpace verdict=IDENTICAL/AE ndiff=0（O110/N110）]
//   修法：①if 判空改正向包裹（ORIG je 到尾部 return 0 岛，P222/P285）；
//   ②msg(0xff)/title(29) 先/后声明使 4.4 槽位与 ORIG 同址（msg -0x134 4 对齐
//   → 简单 rep stos；title -0x35 非 4 对齐 → 对齐剥离慢路，P312）；
//   ③findString/strlen/getCurCharacNo/GetServerGroup 全部内联实参（P0/P86），
//   消 5 个具名局部槽（帧 0x16c→0x15c）。
#include "CTradeSpace/send_lose_item_for_china.inc"
}

// ============================================================================
// G4 交易域补齐：checkTrade / checkCancelTrade / change_trade_state /
// cancel_trade_for_china / proceed_trade / regist_item / remove_item
// （真实语义；跨类调用以 ORIG 符号为准；槽位访问统一走文件顶部 TradeSlotAt）
// ============================================================================

// ---- checkTrade @ 0x08530210 ----
// 残差豁免（O=425/N=424）：SetInventoryMemory 实参 lea 装载位差 1 条。
// 校验交易可完成（ORIG 0x8530210，O=425）：双方各造 CInventory 副本
//（setCopy 直取 characR()->m_inven，+0xda 成员地址，无 InvenR getter），
// 模拟把 trader 登记的物品/金钱插入**对方** copy[1-trader]；
// 金钱分支 gain_money 返回值 != 提交额即失败；物品三分支
//（item.m_field1==8 头像 / m_field08==7 宠物 / else 背包），失败按
// field1==8 双 trace（0xba7/0xbab）。bool 返回（PRETTY @0x8c98444）。
// [b28d-solo-CTradeSpace verdict=NEAR ndiff=16（O425/N425）]
//   修法：局部全头部声明照 ORIG 槽序（P212）：count(-0x30) ret(-0x2c)
//   trader(-0x28) s1(-0x24) got(-0x20) s2(-0x1c)，两循环变量物理独立
//   （4.4 复用槽少一槽）；trader 单初始化（`int trader=0; for(;…)`）。
//   109→16。残差 16 = InventoryMemory 两局部栈地址差 1/2 字节
//   （ORIG -0xd0ff/-0x1923e vs 我方 -0xd100/-0x19240）。Main 2026-09-09 裁决：
//   不得删 InventoryMemory::m_tailC13f（ORIG 全量 110 处 CUser+0x858d4 反证
//   sizeof=0xC140），本残差按 N 封顶（4.1 相邻同型局部 1B 布局差）。
#include "CTradeSpace/checkTrade.inc"
}

// ---- checkCancelTrade @ 0x0852dbb8 ----
// 残差豁免（O=533/N=528）：ORIG 空 else 的 nop;jmp 对齐岛（+0x756 等，
// nop-strip 后消）+ Set 实参 lea 位差。
// 校验取消交易可行（ORIG 0x852dbb8，O=533）：双方各造一个 CInventory 副本
//（SetInventoryMemory 挂独立 InventoryMemory + setCopy(getCurCharacInvenRefR)），
// 模拟把各交易者自己登记的物品按 m_field08（0=背包/1=头像/7=宠物/else=装备）
// 与金钱插回其自身副本；任一插入失败 → 该分支专属 trace 并返回 false。
// 形态要点（逐点对 ORIG 汇编）：
//   - 双 mem 独立对象（ctor→Set 交替，+0xdc..0x120 序列），非 char[]+memset
//   - count(local_34) 跨外圈共享；money 分支 ++count 位于 && 短路内
//   - item 循环条件：槽非空在前、owner 判空在后（+0x23f-0x253）
//   - 四分支失败 trace 各独立字符串（0x6af/0x6b9/0x6c3/0x6d8），
//     characNo 先于 trace 构造求值（链式，P37）
// §6.2 巨件核销：P167 35/35 [0缺0多]；gap 5→4；真修：P191 trader 双初始化
//   （int trader=0; + for(trader=0;…) 复现 ORIG +0x189/+0x190 双 movl，O533/N529）。
//   残差 4 条归因 N 类（详见 docs/asm_translation_patterns.md 负例表）：
//   [dead-store] +0x253 gain_money 返回值 mov %eax,-0x20 死存，4.4 对未读
//     局部恒不占槽（技巧31 负结论），源码不可复现 +1；
//   [跨版本空体岛] +0x756/+0x759 两条失败路径各挂 nop;jmp;nop 中继
//     （负例表「空 else nop;jmp 岛 vs jump-threading」行），4.4 恒直达自增 +3。
// [b28d-solo-CTradeSpace verdict=DIFF ndiff=17（O531/N530）]
//   修法：局部全头部声明照 ORIG 槽序（P212）：count(-0x30) trader(-0x2c)
//   ret(-0x28) s1(-0x24) got(-0x20) s2(-0x1c)；补 gain_money 返回值死写入
//   got（P171 照抄）；两循环变量独立；trader 双初始化（P191）。173→17。
//   残差：①InventoryMemory 栈地址差 1/2 字节（同 checkTrade，Main 裁决 N）；
//   ②尾部一条 jmp 中继（N121/epilogue-threading）。p167 47/47 CLEAR。
// [b28f-SweepCTSpace verdict=NEAR ndiff=186（O533/N533, gap 0）——推翻旧 N 判词]
//   真修：P223 双岛形。ORIG +0x283/+0x29c 两条守卫各自 `je` 分落独立 nop 岛
//   （+0x756 nop / +0x757 jmp / +0x759 nop），4.4 对 `if(A && B)` 恒合并共享
//   出口 ⇒ 改写「负条件 goto + 尾标签串」：
//     if (m_addInfo == 0) goto L_s2_skip1; if (m_pTraders[trader] == 0) goto
//     L_s2_skip2; { body } L_s2_skip1:; L_s2_skip2:;
//   O530→533（=ORIG）、DIFF→NEAR（四数 near+1/diff-1）、ndiff 187→186，
//   逐 hunk 净差全 0（残差仅 InventoryMemory 栈地址 -0xd12f↔-0xd130 等
//   1~2 字节布局差，Main 已裁决 N）。p167 47/47 CLEAR。
#include "CTradeSpace/checkCancelTrade.inc"
}

// ---- cancel_trade_for_china @ 0x0852e544 ----
// 取消交易（ORIG 0x803 条指令完整转写）：金钱退回各自背包，注册物品按
// m_field08 分类插回（1=头像 / 7=宠物 / 0=背包 / 其它=装备），失败走邮件补偿；
// 构建取消回执包（header 0x10，count 占位 get_index+回填 put_short(idx,done)）
// 发给非触发方交易者。
// 形态（P157/P160）：owner 判空、getCurCharacR、槽字段、insert receiver 全部
//   每用点重读（ORIG +0x30c-0x367 条件三连每次全链寻址）；packet 视图内联
//   lea；无 Inven_Item item 栈拷贝（ORIG struct 实参直接从槽 push）。
//   返回值 bool（ORIG __PRETTY_FUNCTION__@+0x181 串为 bool，P159/P27）。
// 残差豁免（O=803/N=814，canon 级 ~11，跨版本结构性）：
//   [setcc-fold] +0x336 gCharacR!=0 判定：ORIG `test;sete;test;jne` 物化岛，
//     4.4 恒直判 je；+0x367 addInfo!=0 反向（ORIG 直判、4.4 生 mov$1/$0 岛）。
//   [jump-threading] 类型四分支物理序：ORIG inv(+0x3d0)→avatar(+0x4c2)→
//     creature(+0x669)→equip(+0x776)（4.1 选 ==0 case fall-through），4.4 对
//     同一源码布局 avatar 直落；等长块序差（diff 锚定重排）。
//   [N18b] put_short(count) 前 count 槽值：ORIG 预载 ecx 后 `mov %ecx,E`
//     （+0x241/+0x2ea），4.4 即时 `mov S,%eax; mov %eax,E` +1×2。
// §6.2 巨件核销：P167 47/47 [0缺0多]；gap −11→−11（O803/N814）；
//   残差归因 N 类（详见 docs/asm_translation_patterns.md 负例表）：上述
//   [setcc-fold]/[jump-threading 块序重排 @@-337,28+329,189/-403,157+556,25]/[N18b]
//   结构性等价残差，P167 调用点 multiset 全等、语义逐分支对照完整，N 封顶。
// [b28d-solo-CTradeSpace verdict=DIFF ndiff=660（O798/N814, gap -16）]
//   N 封顶（§6.2 三件套）：p167 47/47 CLEAR；gap -16（我方多发的 setcc 物化/
//   块序重排）；槽位集 N82 平移（PacketGuard -0x58↔-0x60）。残差归因：
//   [setcc-fold]/[jump-threading 类型四分支物理序]/[N18b put_short 前 count
//   预载]/[N82 全帧槽号错位]。语义逐分支对照完整。
#include "CTradeSpace/cancel_trade_for_china.inc"
}

// ---- change_trade_state @ 0x0852f134 ----
// 残差豁免（O=1096/N=1098，+2）：bool hack 汇聚岛与 4.4 折叠的 ±差异、
// [jump-threading] case2 cancel 成功/失败块序。float/double 阈值块按
// ORIG Ghidra 精度（第一块 float 0.7+0.01*r1，第二块 double 0.9）。
// [b28d-solo-CTradeSpace verdict=DIFF ndiff=824（O1088/N1086, gap 2）]
//   N 封顶（§6.2 三件套）：p167 112/112 CLEAR；gap 2；槽位集 N82 全帧平移
//   （PacketGuard -0xc0↔-0xd0、标量组 -0x120/-0x11c/-0x118 无对应）。
//   残差归因：N82（4.1 不按 lifetime 复用槽 vs 4.4 复用）+ [jump-threading]
//   case2 成功/失败块序 + bool 汇聚岛折叠。语义逐分支对照完整。
// [b28e-SweepTradeSpace verdict=DIFF ndiff=813（O1095/N1091, gap 4）]
//   真修：P0 删 `unsigned char oppErr` 具名局部——ORIG +0x2c0 为
//   `call GetOppositeErr; movzbl %al,%eax`（无槽往返）⇒ 实参内联；
//   ndiff 824→813。残余 813 归因：PacketGuard 槽差 0x0c（ORIG -0xc0 vs
//   我方 -0xcc）+ target/opp/me 槽序（ORIG -0x74/-0x70/-0x6c vs 我方
//   -0x78/-0x7c/-0x80；4.4 按首次使用序，P212 声明序不可翻）+ needCnt/
//   emptyCnt 数组槽差 0x10（-0xec vs -0xfc）+ case2/case3 块序（N82/P272）。
// [b28f-SweepCTradeSpace verdict=DIFF ndiff=1190（O1095/N1095, gap 0）]
//   真修四处（推翻 b28d「虚调用非真 bug」判词）：
//   ① **P282 真 bug：虚调用槽 0x14→0x10**。ORIG +0xa29/+0xaf9 为 `add $0x10;
//      call *`；用 ORIG 自身派生类 vtable 实证槽语义：_ZTV14CStackableItem
//      @0x08c96ea0 word[6]=0x08514a92=CStackableItem::IsAvatarItem（vptr+0x10）、
//      word[7]=0x08514a9c=IsCreatureItem（vptr+0x14）⇒ 原码调 **IsAvatarItem**，
//      旧重建误调 IsCreatureItem（运行时选中错误虚函数，hack 检测语义错）。
//   ② P222 负极性：ORIG +0x5f1 `call isEquipableItemType; xor $1; test; je`
//      ⇒ 原码把否定写在条件里（非装备走 check_item_exist 支）。
//   ③ P0 表达式源：ORIG 各处按 `m_pTraders[opp]->…` 逐点重算索引
//      （+0x5ef 段 `mov -0x70(%ebp),%edx; add $0x8,%edx; mov (%eax,%edx,4)`），
//      非读 target 具名槽。
//   ④ P222 嵌套 if：ORIG +0x8cb/+0x92b 三条件各自 `xor $1; test; je` 落共享
//      出口，4.4 对 `&&` 链折叠首条 xor ⇒ 拆嵌套 if（O1092→1095）。
//   残余归因：全帧槽图恒定平移 0x0c（PacketGuard -0xc0↔-0xcc、target
//   -0x74↔-0x80、needCnt -0xec↔-0xf8；4.1 首槽基 -0x4 vs 4.4 -0xc 属
//   N126 版本常数 + case3 两循环槽复用差，声明序反转实测无效=首次使用序）
//   + case3 块序（difflib 对齐漂移，N127）。p167 112/112 CLEAR（仅 C1/C2、
//   D1/D2 同址别名差，P93 豁免）。
#include "CTradeSpace/change_trade_state.inc"
}

// 执行交易（ORIG 0x0853087a 完整转写）：防 hack 金币/头像上报 → 双方金币/物品
// 互换 → 零金币日志 → 物品转移（头像/生物/普通三分支，共享 ret<0 失败出口）→
// 发包 → 更新列表 → 收尾日志。失败物品不回补（ORIG 直接 reset 槽位返回 0）。
// 形态（P157/P160，O=1606）：第二循环全程不缓存 m_pTraders/m_slotsData 指针，
//   每用点重读成员（+0x8d6/+0x91d 等）；仅 ORIG 自身缓存处保留具名局部：
//   -0x60/-0x5c src/dst（第一循环）、-0x3c item ptr（均价块）、
//   -0x34 slot ptr（头像成功块）、-0x2c characNo/-0x28 size（生物块）、
//   -0x4c/-0x50 from/to（零金币块）。
//   ret 共享槽 -0xac 初值 0（ORIG +0x0de）；三分支各写，共享 +0x11ce 负值检查。
//   ChangeItemLimitCount 记在**发送方** trader 的 historyLog（ORIG +0x1090）。
//   TradeComplete 参数为 tradeLogMoney[trader]/[1-trader]（ORIG +0x1613/+0x1639
//   直取 user+0x8e980，GetGoldControl 头内联 W@0x084ed008）。
// 残差豁免（O=1606/N=1619，canon 级 ~13，跨版本结构性）：
//   [N18a] 头像成功块 _Save/DB 参数 ts+4/ts+0x10 双读：ORIG 4.1 单段
//     `mov S,%edx; mov disp(%edx)`（+0xb0e/+0xb50），4.4 恒两跳物化 +1×2。
//   [N76] m_fieldF68[1-trader]+=1：ORIG SAVE_EXPR 折叠单索引复用（+0xed0-
//     +0xef0 8 条），4.4 双索引重算 13 条（+= 展开策略版本差，源码不可控）。
//   [N7/寄存器] 尾 TradeComplete money 槽基址 -0xa4 vs -0x88 与 ebx/edx 互换
//     （.o 帧槽序，AE 归一吸收）。
// [b28d-solo-CTradeSpace verdict=DIFF ndiff=884（O1606/N1614, gap -8）]
//   真修：①p167 差集 MISSING operator* ×2 / EXTRA operator-> ×2 ⇒ `it->first`/
//   `it->second` 改 `(*it).first`/`(*it).second`（P167/P256）；②GetGoldControl
//   头内 inline 在 4.4 -O0 不内联 ⇒ 改具名成员 `m_pTraders[t]->m_goldControl`
//   复现 ORIG 内联形（P119 反向）。p167 现 140/140 CLEAR。901→884。
//   N 封顶：gap -8、N82 槽号平移、bool 汇聚岛。
#include "CTradeSpace/proceed_trade.inc"
}

// ---- regist_item @ 0x08529e7e ----
// 残差豁免（O=2021/N=2021 等长；canon 级残差为跨版本结构）：
//   [tail-merge] slotIdx==-1 四分支：ORIG je 共享尾块(+0x14bf)，4.4 各分支
//     内联 mov$-1/jmp 岛（×4）；[setcc-fold] +0x640 m_addInfo2<b 的
//     setl/test 物化岛 vs 4.4 直判；[jump-threading] type0 回滚 ==/> 双
//     reset 落块序与 4.4 相反（等长）。
// 注册物品/金钱到交易槽（ORIG 0x08529e7e，2021 条完整转写）：
//   type==0 && a==0 → 金钱（use_money 扣钱 → m_nMoney[trader] += b）；
//   type==0 其余    → 背包物品；type==1 → 头像；type==3 → 装备栏；
//   type==7         → 宠物。取物校验 → delete_item 扣除 → add_item 入槽 →
//   checkTrade/checkCancelTrade 成功发包、失败回滚归还。
// §6.2 巨件核销：P167 227/227 [0缺0多]；gap 0→0（O2021/N2021 等长）；
//   残差归因 N 类（详见 docs/asm_translation_patterns.md 负例表）：
//   [tail-merge]/[setcc-fold]/[jump-threading] + [N82 全帧槽号错位]
//   （ndiff 796 与语义完成度反向；槽集/调用点 multiset 全等，语义逐分支对照
//   完整），N 封顶。
// [b28d-solo-CTradeSpace verdict=DIFF ndiff=794（O2021/N2019, gap 2）]
//   修法：IsLocked bool 化（P292）。N 封顶（§6.2 三件套）：p167 227/227 CLEAR；
//   gap 2；槽位集 N82 平移（-0xd4/-0xc4/-0xb4/-0xa4 ↔ -0xd0/-0xc0/-0xb0/-0xa0）。
//   残差归因：[tail-merge slotIdx==-1 四分支]/[setcc-fold m_addInfo2<b]/
//   [jump-threading type0 回滚块序]/[N82 全帧槽号错位]。
// [b28e-SweepTradeSpace verdict=DIFF ndiff=318（O2021/N2020, gap 1）]
//   真修两处（推翻旧 N82 判词）：
//   ① P331 局部声明/赋值序：`CUser* opp = 0; int other; int trader;`——ORIG 槽
//      opp(-0x64，+0x0e 预置零)/other(-0x60)/trader(-0x5c)，两分支 store 序
//      opp→other→trader；
//   ② P221/P227 四处具名 `cMyTrace tr(…); tr(…)` 改全表达式临时链
//      （0x147/0x1c8/0x1fc/0x281）⇒ cMyTrace 对象不再占深槽，槽图整体与 ORIG 对齐。
//   ndiff 794→318、gap 6→1；p167 227/227 CLEAR。残余 318 归因：短路物化极性
//   （ORIG `call;test %al,%al;je` vs 我方 `xor $1;…`，P214①/P222）、全局 receiver
//   装载序（P87/P329）、slotIdx 槽差（-0x58 vs -0x50）、块序重排（P272/N82）。
// [b28f-SweepCTSpace verdict=DIFF ndiff=1280（O2021/N2019, gap 2）]
//   真修四处：
//   ① P214① use_money 正向包裹：ORIG +0x1e9 `call; test %al,%al; je <远处 -5
//      块>`（正极性无 xor）⇒ 原码 `if (use_money(...)) { 成功路径 } return -5;`；
//   ② P325 共享尾块：ORIG 四处 `add_item` 后 `cmpl $-1; je <+0x2109 共享尾块
//      mov -0x58,%ebx>`（4.1 把 `return -1` 折叠为 `return slotIdx`）⇒ 源码
//      改为 `if (slotIdx == -1) goto L_slotN;` + 成功路径 `goto L_slotN;` +
//      尾部 `L_slotN: return slotIdx;`（我方实测生成 `je <共享块>` + 槽 -0x58
//      与 ORIG 同址）；
//   ③ 旧「虚调用非真 bug」判词作废（同 change_trade_state ①）。
//   ④ P198③ 双槽置 0：ORIG case7 tick 段 +0x1f16 `movl $0x0,-0x24` 后
//      `call GetDateTimeTick` 回写 ⇒ 原码「声明=0 + 独立赋值」两句式。
//      实测 `int tick = 0; tick = OS_API::GetDateTimeTick();` +1 条归位。
//      （反例：case1/case3 的 tick 改实参内联实测劣化 1280→2226，已回退。）
//   残余：type0 回滚 ==/> 双 reset 落块序（ORIG ==b 跳 +0xb95 尾块、我方内联；
//   嵌套 if 实测零效）+ 短路链 receiver 装载序（P87/P329）+ cMyTrace 实参
//   get_add_info 跨 call 保活落槽（ORIG %ebx）+ N82 全帧槽号平移。
//   p167 227/227 CLEAR（C1/C2、D1/D2 同址别名差按 P93 豁免）。
#include "CTradeSpace/regist_item.inc"
}
// ============================================================================
// ============================================================================
// remove_item @ 0x0852bfcc —— ORIG 完整转写（O=1767）
// 形态（P157/P160）：ORIG 全程不缓存槽指针，每个字段访问都重做
//   trader*0x798 + trade_slot_no*0x48 + 0x30 两级寻址；本函数用
//   m_slotsData[trader][trade_slot_no] 表达式逐点展开，包视图
//   ((InterfacePacketBuf*)&packet) 内联 lea（ORIG 每 call 前重 lea -0x80）。
// 语义：按 type 分派移除交易槽（0=金币/物品、1=头像、3=装备、7=生物），
// 物品放回 user 背包（插入失败返回负值，不回滚槽位）。
// 参数：trade_slot_no / type / target_slot(pos) / count(num)。
//   - 金币包为 if/else 两 put_int 调用点（ORIG +0x286/0x296/0x2ab）
//   - 广播判定 get_state() 调用两次（ORIG +0x14f1/+0x1501，P48 无 CSE）
//   - case3 取背包实体走成员 Inven_Item GetInvenSlot(int,int) const（sret，
//     ORIG +0x109c _ZNK10CInventory12GetInvenSlotEii）
//   - case0 部分扣减：条件与 newAddInfo 直读槽 m_item.m_addInfo2（ORIG
//     +0xc44/+0xc9c 单 mov），非 get_add_info() 调用
//   - case 1/7 移除后 TradeSlot::Reset()；case 0/3 手动逐字段清槽
// 残差豁免（O=1767/N≈1771，canon 级剩余，跨版本结构性）：
//   [setcc-fold] 3 处 char/bool 返回值 if 判定：4.4 生 test;setne;test;je
//     （ORIG +0x17 IsLocked、+0x481/+0x837 check_empty_count 直判 test;je），
//     无法以合法 C++ 抑制（P22/N 类同族）。
//   [N18b] slotItemType 与 copy.m_item.m_field1 比较：ORIG +0x410-0x41f 两段
//     物化（mov mem,%edx; movzbl; cmp reg,reg），4.4 恒折 cmp MEM,reg。
//   [N76] newAddInfo 减法：ORIG +0xc49-0xc4f store-then-sub mem，4.4 折
//     reg-sub。语义一致。
// §6.2 巨件核销：P167 160/160 [0缺0多]；gap 0→0（O1767/N1767 等长）；
//   残差归因 N 类（详见 docs/asm_translation_patterns.md 负例表）：
//   [setcc-fold]/[N18b]/[N76] + [N82 全帧槽号错位]（ndiff 650 与语义完成度
//   反向；槽集/调用点 multiset 全等，语义逐分支对照完整），N 封顶。
// ============================================================================
// [b28d-solo-CTradeSpace verdict=DIFF ndiff=648（O1767/N1765, gap 2）]
//   修法：IsLocked 返回类型 char→bool（P292，ORIG +0x17 `call;test %al,%al;je`
//   无 setne）——650→648 并连带 regist_item/proceed_trade 大降。
//   N 封顶（§6.2 三件套）：p167 160/160 CLEAR；gap 2；槽位集差 1 槽
//   （ORIG -0x74 vs 我方 -0x70，N82）。残差归因：[setcc-fold check_empty_count]/
//   [N18b slotItemType 比较]/[N76 newAddInfo store-then-sub]/[N82 全帧槽号错位]。
// [b28e-SweepTradeSpace verdict=AE ndiff=0（O1767/N1767, gap 0）——推翻旧 N 判词]
//   真修四处：
//   ① P331 局部声明/赋值序：otherUser(-0x44 最深)/other(-0x40)/trader(-0x3c)
//      按槽深序声明，两分支 store 序 otherUser→other→trader（ORIG +0x40/+0x58/
//      +0x5b/+0x62）；
//   ② P227 全表达式临时链：0x4b8 处具名 `cMyTrace tr(…); tr(…)` 两语句改
//      链式（ORIG +0x4b8 前 `mov -S,%ebx` 先求 vararg 并跨 ctor 保活）；
//   ③ P198① 两语句 newAddInfo：`int v=X; v-=count;` 复现 ORIG store-then-sub
//      mem（旧判 N76 翻案）；
//   ④ P319 `(unsigned int)slotItemType != copy.m_item.m_field1` 复现 ORIG
//      两段物化 `mov -S,%edx; movzbl -S2,%eax; cmp %eax,%edx`（旧判 N18b 翻案）。
//   p167 160/160 CLEAR；ndiff 648→0、gap 2→0。
#include "CTradeSpace/remove_item.inc"
}
