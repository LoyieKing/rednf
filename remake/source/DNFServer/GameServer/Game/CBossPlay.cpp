// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CBossPlay/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "CBossPlay.h"

#include <cmath>

#include "CParty.h"
#include "CUser.h"
#include "CDungeon.h"
#include "CDataManager.h"
#include "PacketGuard.h"
#include "GlobalData.h"
#include "CSystemTime.h"
#include "GameWorld.h"
#include "CGuildServerProxy.h"   // CEventBase / CEventManager（GetRepeatEvent）
#include "CServerEvent.h"
#include "CEnvironment.h"
#include "CBurningFatigueEvent.h"
#include "CExpDoubleEvent.h"
#include "CPCRoomBurningEvent.h"
#include "LogManager.h"
#include "CItemGeneratorMgr.h"
#include "CBattle_Field_shared.h"
#include "GameRand.h"   // get_rand_int

// stClearRewardExpParameter_t / stClearRewardBonusExp_t 权威定义已迁至
// CParty.h（FixBossRewardExpDeps 域），此处不再本地定义。


namespace WongWork
{

namespace
{

const float kPercentDivisor = 100.0f;   // _DAT_08b7507c

// CItemGenerator 虚调用已由权威头 CItemGenerator.h 提供（generateRandomItem/
// generateSpecificItem 虚槽 +0x08/+0x0c），不再使用原始虚表跳转。
// CEventBase 判活改经真实虚函数 IsEventing(CUser*)（vtable+0x34）；
// CPCRoomBurningEvent::GetExpFactor 改真实类调用（权威头 CPCRoomBurningEvent.h）。
}  // namespace

// ============================================================================
// 生命周期
// ============================================================================

// ============================================================================
// reset（ORIG 0x08149120）
// ============================================================================
#include "CBossPlay/reset.inc"

// stRewardItem_t ctor（ORIG W 0x081519b6）：构造 10 个 Inven_Item
// （ORIG 为编译器对 Inven_Item m_items[10] 数组成员的自动构造，body 为空）。
#include "CBossPlay/stRewardItem_t.inc"

// Ctor（ORIG W 0x081519ec）：置 vptr（编译器自动），构造 4 个 stRewardItem_t 块
// （编译器在进入函数体前自动调用 stRewardItem_t ctor），再调 reset()。
#include "CBossPlay/CBossPlay.inc"

// Dtor（ORIG W 0x08151a3a）：置 vptr 后调 reset()。
#include "CBossPlay/dtor_CBossPlay.inc"

// ============================================================================
// InitPlayUser（ORIG 0x0814926e）
// ============================================================================
#include "CBossPlay/InitPlayUser.inc"

// ============================================================================
// _checkMemberDie（ORIG 0x08149382）
// ============================================================================
#include "CBossPlay/_checkMemberDie.inc"

// ============================================================================
// checkAllMemberReady（ORIG 0x081493fe）
// ============================================================================
#include "CBossPlay/checkAllMemberReady.inc"

// ============================================================================
// getLiveMemberCount（ORIG 0x08149466）
// ============================================================================
#include "CBossPlay/getLiveMemberCount.inc"

// ============================================================================
// handleDieUser（ORIG 0x081494a4）
// [b28d-46 判词·N/P197④+P168a③b+N82] 168 vs 172，p167 CLEAR（29/29）。
// for(i=0;i<=3;++i)+continue 形复现双初始化（P191）与尾部 setle 物化（P51）。
// [b21 N·P197-④] 语义已闭合：找成员循环 `checkValidUser(i) && get_user(i)==user`
//   的取反判定——ORIG(4.1) 发 `sete %al; test; je`，4.4-O0 对 call 结果直比恒发
//   `mov $1/mov $0 + test/jne`（P197-④：源形不可复现，禁改内联比较=双调用真 bug）；
//   循环头 `movl $0,-0xc` 双写为 4.1 代码生 quirk（int i=0）。返回路径 ORIG 在各
//   return 点直发 `mov $0x0/%1,%eax`+共享尾，4.4 以 %ebx 合并 epilogue（N82/寄存器分配）。
//   while(true){if(3<i)return 1;...break;++i;} 形态 + 时间比较改 `GetRevivalTime() >
//   getCurSec()` 已对齐 4.1 求值序/setg 方向；逐调用逐分支语义逐一相等，封 N。
// ============================================================================
#include "CBossPlay/handleDieUser.inc"

// ============================================================================
// handleLeaveUser（ORIG 0x08149724）
// [N18b 封顶] ORIG 0x08149771 对 get_user(i)==user（call 结果 vs 形参比较）生成
// `cmp 0xc(%ebp),%eax; sete %al; test` 物化；4.4 恒折直 cmp+jne 双岛，无源码写法
// 可产 sete（N18b 判据），其余 128 条逐条同形。
// [b27 复证] N93附 尾块序亦不适用（此处非 return 块而是 && 汇聚物化；
// 探针 44 再证 4.4 双岛恒 jne+mov$1/mov$0）。维持封顶。
// [b27 DL3R] 返回类型 int→bool（ORIG 调用点 CBossTower 0x08143b04 `test %al`
// 单字节直判实证）：callee 行数 128:131 两型实测完全一致（零成本），受益方为
// 调用点 xor 消形。
// ============================================================================
#include "CBossPlay/handleLeaveUser.inc"

// ============================================================================
// generateRewardExp（ORIG 0x08149e20）
// [b28d-46 判词·N/N82] 244 vs 267，p167 CLEAR（32/32）。已修：baseExp 具名 float
// （P252 复现 fstps -0xf0 槽往返）、tower 内联表达式（P0/P183 无槽直用 %eax）。
// 残差 = 帧 0x13c vs 0x174 全帧槽号平移（N82）＋ `1+a` 下标 add vs disp 折叠（P30/N4）⇒ 封顶。
// ============================================================================
#include "CBossPlay/generateRewardExp.inc"

// ============================================================================
// generateRewardItem（ORIG 0x08149914）
// [b28d-46 判词·N(死 ebx 物化) + 3 真 bug] 327 vs 324（gap 3）、p167 44:44 CLEAR
// （仅 C1/D1↔C2/D2 同址别名，P93）、帧 0x13c 两侧全等。本轮修：
// ① 真 bug·虚槽互换：ORIG vtable `_ZTVN8WongWork15CItemGeneratorE` @0x08c9a788
//    word[4]=0x0853a400 generateRandomItem（vptr+0x08）、word[5]=0x0853a406
//    generateSpecificItem（vptr+0x0c）；ORIG 段 A（0x08149a87 `add $0xc`）与
//    段 B（0x08149c03 `add $0x8`）各调其一，原实现两分支调用名互换。
// ② 真 bug·舍入：ORIG 0x08149b18 为 `fildl; fmuls; 设截断 CW; fistpl`
//    （= (int)(float) 截断），原 ROUND(+0.5f) 宏（fadd 0.5 + fistpll）错误。
// ③ 形态：外层条件原 `< || ==` 双算 imul 且块序反 ⇒ 单条件 + 随机块 fall-through
//    （P328⑧/P272）；内层同理改 `rare2*b > randVal`。
// ④ P0/P52/P248：rare/rare2/randVal 同型化 unsigned（消 edx 中转）、rare 三段链式
//    （消 dungeonIdx/dm/tower 具名槽）、ref/result 先于 get_rand_int 构造、
//    dst 去具名引用（逐点重算地址）、特定分支补 `Inven_Item rewardItem = *begin()`
//    值拷贝（ORIG 0x08149c9b..0x08149d25 共 17 条 mov）、随机分支独立 randResult
//    对象（-0x9c 自带 ctor/dtor）、删 m_party 判空（P140/P171）、
//    addInfo 回写同槽（ORIG fildl/fistpl 同址 -0x12c）。
// 残差 gap 3 = ORIG 段 A/B 公共出口 `mov $0x1,%ebx` / item==0 路径 `mov $0x0,%ebx`
//  + 循环尾 `test %ebx,%ebx`（**无消费者死物化**：4.1 条件值保留；ebx 全程无栈槽
//  ⇒ 非具名局部，4.4 无对应源形，同 P197④/跨版本族）⇒ 语义闭合，封 N。
// ============================================================================
#include "CBossPlay/generateRewardItem.inc"

}  // namespace WongWork