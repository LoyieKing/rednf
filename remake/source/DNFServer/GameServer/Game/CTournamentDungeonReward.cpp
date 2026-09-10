// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CTournamentDungeonReward/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include "CTournamentDungeonReward.h"
#include <cstring>
#include <iostream>            // P338：ORIG 本 TU 含 std::ios_base::Init __ioinit
                               //（static_init 0x082854e9 有 InitC1+__cxa_atexit，
                               //  GI 0x08285529 sub $0x18）⇒ 缺此 include 则本 TU
                               //  不发射 GI/__static_init，missing_in_game 多 2。

#include "CDungeon.h"          // TournamentRewardRate/TournamentRewardItemRate 权威定义
#include "CBattle_Field_shared.h"   // WongWork::stGenerateRefData_t/stGenerateResult_t
#include "CDataManager.h"
#include "CItemList.h"
#include "PacketGuard.h"
#include "InterfacePacketBuf.h"
#include "GameRand.h"          // get_rand_int

// ORIG T 0x08284750 (size 0x1f)：外联强符号定义，须先于本 TU 全部 caller
// （ctor 0x8284770 / ResetReward 0x8284820）——P210② 帧继承（4.4 按已见
// callee 定义精确继承 outgoing ⇒ ResetReward sub $0x14 与 ORIG 一致）。
#include "CTournamentDungeonReward/Clear.inc"
}

// ORIG ctor 0x8284770：写虚表 0x8c0cbf0；D1 0x82847dc；ResetReward 0x8284820。
// 本 TU 仅提供 CBattle_Field 构造/析构链与 ResetReward 所需符号（ORIG 0x8284820）。
// 其余成员（MakeSelectPacket/_MakeRewardPacket/MakeRewardPacket 等）在头为 G2
// 占位布局时未声明成员，故不在此定义，属后续批次。
#include "CTournamentDungeonReward/CTournamentDungeonReward.inc"
}

#include "CTournamentDungeonReward/dtor_CTournamentDungeonReward.inc"
#include "CTournamentDungeonReward/ResetReward.inc"
}

// ORIG 0x828502e：type∈{0,1}、maxItems<3 校验后，按 cardNo 在
// m_rewards[type][i].m_flag08 中匹配卡片号：
//   m_id==0  → items[0] 置 add_info=m_field04（固定物品），items[1] 空；
//   m_id!=-1 → items[1].m_addInfo=m_id 后经 CItemList::create_item 生成，
//              items[0] 空；m_id==-1 → 双空返回 0（未命中）。
#include "CDataManager.h"
#include "CItemList.h"   // create_item（G_CDataManager()->getItemList()）

#include "CTournamentDungeonReward/GetItemFromCard.inc"
}

// ============================================================================
// ORIG 0x8284a24：两张卡槽各按 rate 掷点决定奖励（未过首档则 m_id=0）。
// [b28d-N82] fresh 复验 118/130：残差 = 4.4 多 push %edi + 帧 0x30→0x3c
// （callee-saved 需求/帧策略差）+ 二维索引寄存器（N18a）⇒ 语义等价封顶。
// [b28d-solo-CTournamentDungeonReward] DIFF 118/121：P272 德摩根块序（else 体直落、
// 双跳同落 then）+ `pick+1` 初始化物化 + operator[] 引用缓存（ORIG 槽 -0xc）三项修后
// 130→121；残 = 寄存器角色互换（card↔slotIdx，N7/N18a 族）；p167 4=4 CLEAR。
// [SweepCTournament 2026-09-09] 复核 118/121：残 = N18a/N7 索引中转×3；p167 4=4
// CLEAR；jcc（je/jl/jg/seta/setle）与 ORIG 逐位全等 ⇒ N 封顶。
// ============================================================================
#include "CTournamentDungeonReward/DecisionRewardType.inc"
}

// ============================================================================
// ORIG 0x8284890：round=victory?round:round-1（夹取 0..4）；查 rates 表命中
// 后掷卡槽 0；终局轮（round==4 且胜利）再以表项 5 掷卡槽 1。
// ============================================================================
#include "CTournamentDungeonReward/MakeReward.inc"
}

// ============================================================================
// ORIG 0x8284b8e：空奖励槽（m_id==0）以生成结果首个掉落物的 add_info2
// （金币额）乘金币率写入 m_field04。
// [b28d-N82] fresh 复验 91/94：同 DecisionRewardType（多 push %edi、帧 0x30→0x3c、
// 循环槽 -0x10/-0xc → -0x20/-0x1c）+ N18a 索引差 ⇒ 语义等价封顶。
// [b28d-solo-CTournamentDungeonReward] DIFF 91/95：修 empty()→begin()!=end()（P197④
// 反向：4.4 empty 是 out-of-line call+bool 物化岛）与 get_add_info()→m_addInfo2
// 直读（ORIG `mov 0x7(%eax),%eax`）后调用点 4=4 与 ORIG 全等；残 = N18a + push %edi/
// N115 帧 0x10；p167 4=4 CLEAR。
// [SweepCTournament 2026-09-09] 复核 91/95：残 = N115 多 push %edi + N18a 中转×2；
// p167 4=4 CLEAR；jcc（jne/je/setle）与 ORIG 逐位全等 ⇒ N 封顶。
// ============================================================================
#include "CTournamentDungeonReward/SetRewardGold.inc"
}

// ORIG 0x82852e4：单卡型奖励包体（byte2 + 每卡槽 int/int/short0）
// [b28d-N18a] fresh 复验 65/66：残 1 行 = 二维索引中间寄存器 + 块尾 nop 差。
// [b28d-solo-CTournamentDungeonReward] DIFF 65/66：残 = N18a×2 + ORIG 空 then 占位
// nop 岛（§6.1 剥离）；p167 4=4 CLEAR。
#include "CTournamentDungeonReward/_MakeRewardPacket.inc"
}

// ORIG 0x8284caa：卡型 0 必发；flag 时补卡型 1，否则 byte0 占位
#include "CTournamentDungeonReward/MakeRewardPacket.inc"
}
// ============================================================================
// ORIG 0x08284ee4（报告 SelectCard.md）：cardA/cardB ∈ [0,1]、slotNo ∈ [0,3]
// 校验后，m_rewards[(int)cardA][(int)cardB].m_flag08（movzbl 零扩展读）== 0xff（未选）
// 才写入 slotNo 低字节并返回 1；否则返回 0。
// [b28d-N18a] fresh 复验 66/68：残差 = 二维索引中间寄存器（movsbl 装载序 +
// `mov %eax,%edx` 中转，P30/N18a）。
// [b28d-solo-CTournamentDungeonReward] DIFF 66/68：残 = N18a×2（读/写各一）；
// p167 0=0 CLEAR。
// [SweepCTournament 2026-09-09] 复核 66/68；jcc 逐位核对（js/jle×3 + cmp $0xff
// /jne）与 ORIG 全等 ⇒ N 封顶。
// ============================================================================
#include "CTournamentDungeonReward/SelectCard.inc"
}
// ============================================================================
// ORIG 0x08285284（报告 CTournamentDungeonReward/isMemberSelectCard.md）：
// 遍历该卡型两槽，m_flag08（char 符号扩展）== cardNo 即 1。
// [b28c-16 定论 N18a] 残差仅二维寻址中间结果寄存器：ORIG `lea 0x0(,%eax,4),%ecx`
// 直落 ecx（type 在 edx/slot 在 ecx），4.4 恒 slot*12→eax 后 `mov %eax,%edx`
// 多 1 条并改寄存器名（type→ecx/slot→eax）。探针 /tmp/l16p/p8.cpp 两形不可择。
// [b28d-N18a] fresh 复验 34/35：维持 N18a。
// [b28d-solo-CTournamentDungeonReward] DIFF 34/35：残 = N18a×1；p167 0=0 CLEAR。
// [SweepCTournament 2026-09-09] 复核 34/35；jcc（jne/setle）与 ORIG 全等 ⇒ N。
// ============================================================================
#include "CTournamentDungeonReward/isMemberSelectCard.inc"
}

// ============================================================================
// ORIG 0x082853b8（报告 _ForceSelectCard.md）：type>1 直接返回；
// 对两槽：m_flag08==0xff（未选）时遍历 members，凡 !isMemberSelectCard(type,*it)
// 者写入 m_flag08（低字节）。持续遍历不提前退出（ORIG 形态）。
// [b28d-N82] fresh 复验 95/96：残差 = 多 push %edi + 帧 0x20→0x2c + 循环槽平移。
// [b28d-solo-CTournamentDungeonReward] DIFF 95/101：修 `++it`→`it++`（P28 增量侧：
// ORIG 增量段 `operator++(it,0)` + 独立 sret 临时 -0x10）；修后结构/槽序与 ORIG
// 全等，残 = N18a×1 + push %edi/N115 帧 0x10；p167 7=7 CLEAR。
// [SweepCTournament 2026-09-09] 复核 96/101：残 = N115 push %edi + N18a 中转×3；
// p167 7=7 CLEAR；jcc（jg/jne/je/setle）与 ORIG 逐位全等 ⇒ N 封顶。
// ============================================================================
#include "CTournamentDungeonReward/_ForceSelectCard.inc"
}

// ============================================================================
// ORIG 0x0828523c（报告 ForceSelectCard.md）：_ForceSelectCard(0)；victory 时
// 再 _ForceSelectCard(1)。
// ============================================================================
#include "CTournamentDungeonReward/ForceSelectCard.inc"
}

// ============================================================================
// ORIG 0x08284d08（报告 isCardSelectComplete.md）：members 空 → 0。
// type==2（双卡型）：2x2 全部槽的 m_flag08（char 引用，std::find<char> 实证
// _ZSt4findI...cET_S8_S8_RKT0_）都必须出现在 members 中；否则查该行两槽。
// 全命中 → 1。
// [b28d-N82] fresh 复验 137/139：残差 = 帧/槽平移 + find 块尾 jmp 岛（N104）。
// [b28d-solo-CTournamentDungeonReward] DIFF 137/139：修具名 found 迭代器后求值序
// 与槽位（end -0x2c / begin -0x28 / find -0x30 / end -0x24；else 支 -0x20/-0x1c/
// -0x34/-0x18）与 ORIG 逐槽全等；残 = N18a×2；p167 11=11 CLEAR。
// [SweepCTournament 2026-09-09] 复核 137/139：残 = N18a 中转×2；p167 11=11 CLEAR
//（empty/find/end/begin/eq 全命中）⇒ N 封顶。
// ============================================================================
#include "CTournamentDungeonReward/isCardSelectComplete.inc"
}

// ============================================================================
// ORIG 0x08284fa4（报告 MakeSelectPacket.md）：两轮 put_byte(2)；内轮对两槽
// put_byte((char)m_flag08 符号扩展)。
// [b28c-16 定论 N18a] 同上（二维寻址中间寄存器单段直落不可造）。
// [b28d-N18a] fresh 复验 44/45：维持 N18a。
// [b28d-solo-CTournamentDungeonReward] DIFF 44/45：残 = N18a×1；p167 2=2 CLEAR。
// [SweepCTournament 2026-09-09] 复核 44/45；put_byte 调用点 2=2 全等 ⇒ N。
// ============================================================================
#include "CTournamentDungeonReward/MakeSelectPacket.inc"
}
