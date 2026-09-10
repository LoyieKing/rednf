// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 GmCmds_E/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// GmCmds_E.cpp — game_master GM 命令批次 E（11 类）。
// 对照 docs/class_func_reports/game_master__<Class>/：
//   CMobileRestrictCommand / CMonsterDropTest / NpcLimitBuyItem /
//   COneADayItemShop / CPcroomPremiumCmd / CPcroomRewardReset / CPickup /
//   CPowerWarStraightVictory / CRandomObjectTest / CRecipeSuccess / CRefund
//
// b28d-solo-GmCmds_E（2026-09-09 单文件泳道收口）：清单 9 件最终
//   ①AE/identical：makeGoldItemDropList、parse、getVendingMachineInfo（原状）
//     + CMonsterDropTest::execute（本次 DIFF→AE：fname 255 + 表头/循环 switch）；
//   ②NEAR：makeGeneralItemDropList（key 槽提到函数头，ratio 0.9108→0.9682）；
//   ③N 封顶 DIFF：processKillMonster（T1/T2 直证 unsigned base + (int)(long long)
//     截断，282=280；余 N85 x87 比较 + N82/N115 帧）、makeUpgradeDropList
//     （N98/N82 槽序反向）、passedMapMonsterKill（N7 后端寄存器角色）、
//     CRandomObjectTest::execute（P75 memset 实参装载序，手册「待解」项）。
//   TU 级：identical=123 ae=251 near=1 diff=6（基线 123/250/1/7，diff −1、ae +1）；
//   无 .h 改动；无子子代理。
//
// b28d-solo-GmCmds_E-2（2026-09-09 第二轮：dual_rank 3 件 + 全 TU jcc 交叉验证）
//   ①processKillMonster gap 2→0（280→282=ORIG 282）：
//     - 删 `float baseF` 具名局部（P0：ORIG getDropRate 返回值直接 fnstcw/
//       fistpll 转 int64，无 fstps 槽往返）⇒ 消 2 条 fstps/flds；
//     - 两处 `if(checkValidUser(i)){body}` → `if(!checkValidUser(i)) continue;`
//       （P168a：ORIG 0x084adf66 `xor $1;test;jne <循环尾>`）⇒ 全函数 jcc
//       助记符序列与 ORIG 逐位一致（MNE-DIFF 1→0）。余 N85 x87 比较 lowering
//       （ORIG fucompp/fnstsw/test$0x45/sete vs 4.4 fucomip/fstp/seta，4 条）
//       + N82/N115 帧（ORIG push edi/esi/ebx+sub 0x9c vs sub 0x98）。
//   ②makeGeneralItemDropList NEAR→**AE**（ae 252→253、near 1→0）：
//     删具名 `int key`，改 `out.find(it->m_item.m_addInfo)` 内联实参
//     （P236：ORIG find 实参临时槽 -0x50 比 it(-0x58) 浅；具名 key 被 4.4
//     排到 -0x54 且 end 临时 -0x50，与 ORIG 互换）。
//   ③makeUpgradeDropList ndiff 555→427（grade/up/k2 内联进 find 实参，P236
//     同族）：仍 DIFF，余 N86（ORIG find 返回经 sret 临时再拷贝 -2 mov，
//     4.4 copy-elision 无源码解）+ N98 槽序 + EH 析构路径（p167 EXTRA）。
//   ④全 TU jcc 逐位交叉验证：MNE-DIFF=0（条件码序列全对齐）；TGT-DIFF 7 件
//     均为指令数/编码差导致的相对偏移差（5 件游戏域 + N123 static_init +
//     N97 _M_incr）。逐 call * 虚槽核对：本 TU 两侧虚调用站点 = 0（无 call *）。
//   ⑤CRandomObjectTest::execute / passedMapMonsterKill / _M_incr 复验：
//     - CRandomObjectTest::execute gap 2 = ORIG 中部对齐 nop(§6.1) + memset
//       首参 lea 先行/size 经 edx 中转（P75 跨版本，手册「待解」项，三形复测）；
//     - passedMapMonsterKill gap -3 = N7 寄存器角色（ORIG sret=%edx/实参=%eax
//       vs 4.4 先 `mov %eax,%edx`），路由已 4.4.6 仍不翻，禁改路由 ⇒ 封顶；
//     - _M_incr = N97（4.1 mov/sar/shr/lea vs 4.4 lea/test/cmovs）。
//   TU 级（fresh check_tu_routed）：identical=126 ae=253 near=0 diff=6
//   （本轮起点 126/252/1/6 ⇒ ae +1、near −1、零回退）；无 .h 改动；无子子代理。
//
// b28d-loopaudit（Main 高优先级系统性排查：脚本循环「goto 退出标签」假 AE 模式）
//   本 TU 无 `goto`（仅 for/while/switch + break/continue）；按「jcc/jmp 目标
//   **指令下标**」逐位核对全部 469 符号（非字节偏移，避免 AE 归一化掩盖）：
//   - 7 件目标下标不一致：5 件游戏域 + N123 static_init + N97 _M_incr；
//   - 5 件游戏域逐条展开核对，落点差异全部语义等价，无「循环退出 vs 回边」错位：
//     · processKillMonster #12/#16：ORIG 落点含 callee-saved 载入
//       （`movzbl -0x35(%ebp),%ebx` / `mov -0x34(%ebp),%edi`）vs 我方栈直读
//       ⇒ N82/P332 装载时机差；
//     · CMonsterDropTest::parse #0/#4：je/jb 落点相差一个对齐 nop（§6.1）；
//     · CRandomObjectTest::execute #8：memset 实参装载序（P75，已知 gap2）；
//     · makeUpgradeDropList / passedMapMonsterKill：落点全对应（仅指令数平移）。
//   - 回边完整性：processKillMonster(2)/passedMapMonsterKill(3)/
//     CRandomObjectTest::execute(1)/singleMapMonsterKill(2)/CMonsterDropTest::
//     execute(4) 的 `jne <循环条件块>` 回边数量与落点均与 ORIG 一一对应，
//     无「continue/break 落到函数出口」形态；
//   - AE/IDENTICAL 循环密集件落点 DIFF=0：singleMapMonsterKill/
//     makeGoldItemDropList/makeFileGoldContents/CMonsterDropTest::execute/
//     getVendingMachineInfo/getVendingMachineItem/processVendingMachineItemLottery。
//   ⇒ 本 TU 无该 bug 模式，无需修复。
//
// 头文件 GameMasterCmd.h 本批禁止编辑：+0x08 通用槽在头里为占位
// std::string（gcc4.4 旧 ABI sizeof=4）。本批多数类的 ORIG 真实语义是
// int（atoi/sscanf 解析结果），经 str_as_int / str_as_int2 别名访问；
// CPowerWarStraightVictory 的 +0x08 真实语义是 short（仅写 2 字节），
// 经 str_as_short 访问；CMobileRestrictCommand 的 +0x0c 为 uint（无占位名），
// 从 &m_str0+4 别名访问（sizeof=0x10 与 vptr+m_parser+8 一致）。
//
// 辅助函数带参者（头内占位声明为无参）以 G 批次同款 asm 标签自由函数承载
// ORIG 成员符号（i386 成员 ABI = cdecl + this 首参压栈，extern 兼容）。
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <list>
#include "GameWorld.h"
#include "LimitNpcBuyItemRequestInfo.h"
#include <vector>
#include <string>
#include <algorithm>

#include "GameMasterCmd.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CParty.h"
#include "WarField.h"
#include "CBattle_Field.h"
#include "CDataManager.h"
#include "CGlobalEffectManager.h"
#include "DBUpdatePcroomDailyReward.h"  // DBUpdatePcroomDailyReward::MSG（MoreTypes.h 占位已迁入类）
#include "GlobalData.h"
#include "PvPTypes.h"
#include "Inven_Item.h"
#include "CItem.h"
#include "CHandlePremium.h"
#include "CSystemTime.h"
#include "GameTypes.h"
#include "CMonitorServerProxy.h"
#include "CServerProxyMgr.h"
#include "StreamPool.h"
#include "Stream.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "TaiwanInternal.h"
#include "MoreTypes.h"
#include "CInventory.h"
#include "InterSelectPcroomDailyReward.h"

// ---------------------------------------------------------------------------
// CBoosterTest 辅助静态成员（SendChatMsg/makeFileContents/printClient）已由
// GameMasterCmd.h 声明为 game_master::CBoosterTest 的 static 成员，调用点直接
// 真实成员调用（ORIG T 0x084aeac6/0x084af504/0x084aebf2；定义归 CBoosterTest 批）。
// ---------------------------------------------------------------------------
// ORIG T 0x081090fa _Z14StringToNumberPKc —— 全局工具：s ? strtoul(s,0,10) : 0。
// 项目尚无定义点；weak 定义避免与他批冲突（若他处强定义则以彼为准）。
__attribute__((weak)) int StringToNumber(const char* s);
__attribute__((weak)) int StringToNumber(const char* s)
{
    if (s == 0)
    {
        return 0;
    }
    return static_cast<int>(strtoul(s, 0, 10));
}

namespace game_master
{

namespace
{

// 注：历史版本以 std::string 占位 +0x08 槽并按 ORIG int/short 语义别名访问
//（gcc4.4 旧 ABI string sizeof=4）；该批占位字段已全部具名化
//（GameMasterCmd.h：m_duration/m_param1/m_param2/m_result/m_straight/
// m_mode/m_count/m_restrictSwitch 等），本命名空间仅保留空壳。

} // namespace

// ===========================================================================
// CMobileRestrictCommand（sizeof=0x10）
//   +0x08 string m_str0（目标限制域："mobile"/"character"）
//   +0x0c uint 限制开关（StringToNumber(args[2])!=0；无头内占位名，别名访问）
// ===========================================================================

// ORIG ctor 0x082a89b8：基类 ctor → 自身 vptr；string@+0x08 默认构造。

// ORIG parse 0x084b3bdc：parser 非空时 m_str0=args[1]；+0xc=StringToNumber(args[2])!=0。
#include "GmCmds_E/parse.inc"
}

// ORIG execute 0x084b3c60：m_str0=="mobile" 时改 garena auth data +0x2c 字节并打日志；
// =="character" 分支仅做比较（结果丢弃，空体）。
// b28d-solo-GmCmds_E：P227 全表达式临时链——ORIG 在 cMyTrace ctor 之前把
// yes/no 三元物化进 %ebx 并跨 ctor 保活（`mov $yes,%ebx` 在 C1 前），具名
// yesNo 局部会被 4.4 槽化；改临时链 + 三元内联复现。
#include "GmCmds_E/execute.inc"
}

// ===========================================================================
// CMonsterDropTest（sizeof=0x10）
//   +0x08 int mode（0=single / 1=passed-map 保存 / 2=passed 统计 / 3=gold 统计）
//   +0x0c int count（统计次数，0 视作 10000；上限 mode2=10000 其余=100000）
// ===========================================================================

// ORIG ctor 0x082a861e：基类 ctor → 自身 vptr；string@+0x08 默认构造。

// ORIG clear 0x084b4096：两 int 槽清零。
#include "GmCmds_E/clear.inc"
}

// ORIG parse 0x084ace8a：clear；sscanf(args,"%d%d",&mode,&count)；随后按发起者
// party 校验：无 party 提示；mode==1 清空 passed 列表后压入当前图并开启
// 地图保存；mode∈{2,3} 关闭地图保存。
// b28d-solo-GmCmds_E：P0/P251——ORIG 无 mode/passed/bf/cur 局部槽（m_mode 每次
// 重读成员 `cmp $1` 决策树；getPassedMapInfoList 两次独立调用；GetCurrentMapInfo
// 结果跨 call 保活 %ebx），故改 switch(m_mode) + 全表达式内联。
#include "GmCmds_E/parse_1.inc"
}

// ===========================================================================
// CMonsterDropTest 辅助成员（真签名见 GameMasterCmd.h；i386 成员 ABI =
//   cdecl + this 首参压栈，与 ORIG mangled 逐字节同形，转换零 ABI 险）。
// ===========================================================================

// ORIG checkCommandExecuteError(CParty*) 0x084ad82a：返回错误文案指针或 0。
// b28d-solo-GmCmds_E：P0 槽律——ORIG 无 mode/count/bf/idx 局部槽（m_mode 每次
// 重读成员、m_count 先载寄存器再 unsigned 比、get_dungeon_index 结果跨 call
// 保活 %ebx），故全部内联为表达式；原具名局部 mode/count/bf/idx 删除。
#include "GmCmds_E/checkCommandExecuteError.inc"
}

// ORIG processDungeonDrop(CParty*, map&, map2&) 0x084ad8dc：按 mode 分派击杀模拟。
// ORIG 决策树 `test;je→S;sub$2;cmp$1;ja→end;jmp→P` 为 switch case 0 +
// case 2..3 range 形（P46/P163 族），case 升序书写复现块序。
#include "GmCmds_E/processDungeonDrop.inc"
}

// ORIG processKillMonster(CParty*, MapInfo&, map_monster&, vector<bool>&, int, list&)
// 0x084add20：全局效果 1/4 放大掉落次数（向零截断换算，min/max 夹取）；
// 统计队员 premium 0x4f 数并取加成；hell 组计入全灭标记；最终转发
// CBattle_Field::_MakeDropItems。
// b28d-solo-GmCmds_E：P167 调用点差集 CLEAR（21/21）；282=280（ratio 0.43→
// 0.50 段）。已修：base 类型按 T1/T2 直证改 `unsigned int`（ORIG 0x084add59
// 段 `fistp QWORD [ebp-0x68]` + `mov %eax,-0x2c; mov $0,%edx` 零扩展 +
// `fild QWORD` ⇒ unsigned 语义），加成截断按 T1 写 `(int)(long long)expr`
// （ORIG 第二次 `fistp QWORD` 后取低 32 位）。
// b28d-solo-GmCmds_E-2：282=282（gap 0）——删 `float baseF` 具名局部
// （P0：ORIG getDropRate 返回值直接 fnstcw/fistpll，无 fstps 槽往返）+
// 两处 `if(checkValidUser(i)){body}` 改 `if(!checkValidUser(i)) continue;`
// （P168a：ORIG 0x084adf66 `xor $1;test;jne <循环尾>`）⇒ 全函数 jcc 助记符
// 序列与 ORIG 逐位一致（MNE-DIFF 1→0）。残差为 x87 比较 lowering
// （ORIG `fucompp/fnstsw/test $0x45/sete` vs 4.4 `fucomip/fstp/seta`，N85；
// 需 -march=i486 的 TU 级旗标，路由表禁改）与帧/槽差（N82/N115，ORIG
// push edi/esi/ebx + sub 0x9c vs 0x98；P190 double framePad 实测 0x98→0xa8
// 反劣已回退），语义完成、byte 层封顶。
// [round1-L29 2026-09-10 复核] 判据① CLEAR（38 vs 38 分支，目标无错位）。残差 = ORIG
//   `push edi/esi/ebx + sub $0x9c` vs 我方 `sub $0x98`（callee-saved 3 个差异；P332：源级
//   不可诱导，本函数已无多余具名对象可删）⇒ N（ORIG 0x084add20）。
#include "GmCmds_E/processKillMonster.inc"
}

// ORIG singleMapMonsterKill 0x084ad93a：当前地图全部 m_dropCnt==100 怪 × count 次。
#include "GmCmds_E/singleMapMonsterKill.inc"
}

// ORIG passedMapMonsterKill 0x084adaf8：passed 列表每张图同样处理。
// b28d-solo-GmCmds_E：对齐 singleMapMonsterKill——双 bool 槽形、passed 指针
// 不落槽（getPassedMapInfoList 内联实参）、`m_dropCnt != 100` continue 形。
// p167 CLEAR（27/27），162=165（ratio 0.9541）。残差 = begin/end 调用点
// sret↔this 寄存器角色互换（4.4.7 恒 `mov %edx,%eax?`+sret=%eax/this=%edx，
// ORIG sret=%edx/this=%eax；N7/P230 后端敏感族）——本 TU 不在 dnf_game_cxx.sh
// 的 4.4.6 路由表内（禁改路由表）⇒ N 封顶。
// [round1-L29 2026-09-10 复核] 判据① CLEAR（38 vs 38）。我方 163 / ORIG 161 ⇒ 帧/push 族
//   （P332）⇒ N（ORIG 0x084ae7xx）。
#include "GmCmds_E/passedMapMonsterKill.inc"
}

// ORIG getDungeonDropResult(list&, map&, map2&) 0x084ae0e2：按 mode 归账。
// b28d-solo-GmCmds_E：P251——ORIG 为无栈槽的 `cmp $2;je / cmp $3;je /
// test;jne` 决策链（m_mode 值恒在 %eax），原码是 switch(m_mode) 而非 if 链。
#include "GmCmds_E/getDungeonDropResult.inc"
}

// ORIG makeGeneralItemDropList 0x084ae734：普通物品按 itemId 计数；
// itemId==0（金币类）把 Inven_Item.m_addInfo2 累计到键 0。
// b28d-solo-GmCmds_E-2：157=157 **AE**（本轮 near 1→0、ae +1）——P0/P48 条件与
// insert 处重新读成员 + P222 正向包裹 + **find 实参内联**
// （`out.find(it->m_item.m_addInfo)`，P236：ORIG find 实参临时槽 -0x50 比
//  it(-0x58) 浅；具名 `int key` 被 4.4 排到 -0x54 且 end 临时 -0x50 与 ORIG
//  互换）。make_pair 值类别仍保留左值形（ORIG `IiRi` vs 4.4 `Iii`，指令序列
//  identical）。
#include "GmCmds_E/makeGeneralItemDropList.inc"
}

// ORIG makeGoldItemDropList 0x084ae944：itemId==0 的金币掉落按金额分组计数。
// b28d-solo-GmCmds_E：92=92 AE（P222 正向包裹形，check_tu_routed 判 AE）。
#include "GmCmds_E/makeGoldItemDropList.inc"
}

// ORIG makeUpgradeDropList 0x084ae14c：非堆叠掉落按 grade 分桶；
// 桶内记录 [强化等级]=次数 与 [特殊类型+1000]=次数（特殊类型由
// m_addInfo2 映射：0x5aa7→1 / 0xfa6→2 / 0x6a23→3 / 0x5df0→4 / 0x7270→5）。
// b28d-solo-GmCmds_E：P167 CLEAR（58/58）——make_pair 实例修正
// （make_pair<uchar,int>、make_pair<int,map&>）+ get_grade 二次调用复现；
// switch(v) case 升序复现决策树。
// b28d-solo-GmCmds_E-2：376=375；grade/up/k2 改内联进 find 实参
// （P236 同族：ORIG 的 find 实参临时槽 -0xf0/-0xe8 比 git(-0xfc)/it(-0xf8) 浅）
// ⇒ ndiff 555→427、it 槽 -0xec→-0xf0（ORIG -0xf8）。仍 DIFF，残差：
// ① N86——ORIG find 返回经 sret 临时再拷贝到具名迭代器（-2 mov），4.4
//    copy-elision 直构，无源码解；② N98 槽序（4.4 按首次使用序）+ N82；
// ③ EH 析构路径差（p167 EXTRA pair/map D1；正常路径构造/析构次数一致）。
// 语义完成、byte 层封顶。
// [round1-L29 2026-09-10 复核] 判据① CLEAR（86 vs 86）。残差 = ORIG `push esi/ebx +
//   sub $0x130` vs 我方（P332 帧/push 族）⇒ N（ORIG 0x084ae14c）。
#include "GmCmds_E/makeUpgradeDropList.inc"
}

// ORIG makeFileGoldContents 0x084ad6d6：gold 统计追加到文件缓冲；容量不足时提示。
// （ORIG static：makeRequest.md 反编译注释 __PRETTY_FUNCTION__ 无 this，
//   报告实证 arg0=CUser*。）
// b28d-solo-GmCmds_E：89=89 AE（P214③ `char line[1024]={0}` 复现头部 rep stos；
// 删 used/addLen 具名局部改内联 strlen 相加）。原 memset(line,0,0x100) 少清 768B 已修。
#include "GmCmds_E/makeFileGoldContents.inc"
}

// ORIG execute 0x084acf98：主流程（校验 → 分派击杀 → 分模式落盘 → 升级分表输出）。
// b28d-solo-GmCmds_E：AE 达成（identical=123/ae=251 中本件计入 ae；p167 CLEAR
// 59/59）。本次三项直证修正：
// ① fname 改 255 字节（ORIG 4.1 清零展开 `mov $0xff,%ebx` + 起点 -0x197 非 4
//    对齐 ⇒ P312 慢路；原 256 使 fileBuf 起点 -0x178b4 漂 5）；
// ② 表头改 `switch(m_mode)` 且 case 2/0 共用 0x44 块（ORIG 0x084ad116 段
//    `cmp $2; je 0x84ad128(0x44 块)`、`cmp $3; je 0x84ad14c`、`test;jne done`
//    ⇒ 单次成员读 + case 2 与 case 0 同块；原 if 链重读成员且 mode==2 漏表头）；
// ③ while 内分派改 `switch(m_mode)`，case 0/2/3 书写序复现叶块物理序
//    （ORIG 0x084ad178/0x84ad17d 目标 0x84ad1c9/0x84ad207/0x84ad243）。
// 残留仅重定位显示（A vs 段内偏移）与 `@plt` 后缀，compare_tu AE 口径已归零。
#include "GmCmds_E/execute_1.inc"
}

// ===========================================================================
// NpcLimitBuyItem（sizeof=0x8）
// ===========================================================================

// ORIG ctor 0x082a885a：基类 ctor → 自身 vptr，无数据成员。

// ORIG parse 0x084b1f22：空实现。
#include "GmCmds_E/parse_2.inc"
}

// ORIG execute 0x084b1f28：向监控代理发送 LimitNpcBuyItemRequestInfo
//（type 0x27d8 / len 10）请求限购信息重载。
#include "GmCmds_E/execute_2.inc"
}

// ===========================================================================
// COneADayItemShop（sizeof=0x8）
// ===========================================================================

// ORIG ctor 0x082a868e：基类 ctor → 自身 vptr，无数据成员。

// ORIG execute 0x084b0610：G_GameWorld()->testResetOneADayItemShop()。
#include "GmCmds_E/execute_3.inc"
}

// ===========================================================================
// CPcroomPremiumCmd（sizeof=0xc）
//   +0x08 int 时长（秒；atoi(args)，占位槽别名访问）
// ===========================================================================

// ORIG ctor 0x082a822e：基类 ctor → 自身 vptr；string@+0x08 默认构造。

// ORIG parse 0x084a9db0：trim(args)；+0x08 = atoi(args)。
#include "GmCmds_E/parse_3.inc"
}

// ORIG execute 0x084a9de8：以当前系统秒为起点设置 PC 房 premium 区间
// [now, now+dur]，交 WongWork::CHandlePremium::handleSetUserPCRoom 生效。
#include "GmCmds_E/execute_4.inc"
}

// ===========================================================================
// CPcroomRewardReset（sizeof=0x8）
// ===========================================================================

// ORIG ctor 0x082a89e2：基类 ctor → 自身 vptr，无数据成员。

// ORIG parse 0x084b3d12：仅读 m_parser 判存（mov/test/ret），无副作用。
// 4.1 保留 dead test 无分支；探针实证 `if (p) return;` 形在 4.4 复现
// load/test/pop-ret 同形（/tmp/bl5/probe2.cpp k2）。
#include "GmCmds_E/parse_4.inc"
}

// ORIG execute 0x084b3d20：置 CUser+0x8ec32=4（每日奖励待更新态）、
// 发 DB 更新请求并回发查询包，最后日志 acc id。
#include "GmCmds_E/execute_5.inc"
}

// ===========================================================================
// CPickup（sizeof=0x10）
//   +0x08 int 参数一（atoi(args[0..space))）
//   +0x0c int 参数二（atoi(args[space+1..])；无头内占位名，别名访问）
//   注：ORIG execute 为空函数（0x084ac966 直接 ret）。
// ===========================================================================

// ORIG ctor 0x082a85e6：基类 ctor → 自身 vptr；string@+0x08 默认构造。

// ORIG parse 0x084ac84c：trim；find(" ") 切两段分别 atoi 入 +0x08/+0x0c。
// b28d-solo-GmCmds_E：86=86 AE——`size_t pos` 改 `int pos`（P218 同族：signed
// 索引局部使 4.4 复现 ORIG 的 `mov -0xc,%edx` 先载 + `add $1,%eax; mov %eax,%edx`
// 两段形与 ecx/edx 角色）；原 P75/N18a 判词作废。
#include "GmCmds_E/parse_5.inc"
}

// ORIG execute 0x084ac966：空实现。
#include "GmCmds_E/execute_6.inc"
}

// ===========================================================================
// CPowerWarStraightVictory（sizeof=0xc）
//   +0x08 short 连胜数（atoi(args) 截断为 16 位写入）
// ===========================================================================

// ORIG ctor 0x082a83c4：基类 ctor → 自身 vptr；string@+0x08 默认构造。

// ORIG parse 0x084ab202：trim(args)；*(short*)(this+8) = (short)atoi(args)。
#include "GmCmds_E/parse_6.inc"
}

// ORIG execute 0x084ab23e：有队伍设队伍连胜，否则设个人（CUserCharacInfo）连胜。
#include "GmCmds_E/execute_7.inc"
}

// ===========================================================================
// CRandomObjectTest（sizeof=0x10）
//   +0x08 int mode（1=售货机统计，其余无动作）
//   +0x0c int count（统计次数，0 视作 10000；合法域 [0,100000]）
// ===========================================================================

// ORIG ctor 0x082a8656：基类 ctor → 自身 vptr；string@+0x08 默认构造。

// ORIG clear 0x084b40ca：两 int 槽清零。
#include "GmCmds_E/clear_1.inc"
}

// ORIG parse 0x084b0032：clear；sscanf(args,"%d%d")。
#include "GmCmds_E/parse_7.inc"
}
// ===========================================================================
// CRandomObjectTest 辅助成员（真签名见 GameMasterCmd.h；i386 成员 ABI =
//   cdecl + this 首参压栈，与 ORIG mangled 逐字节同形，转换零 ABI 险）。
// ===========================================================================

// ORIG checkCommandExecuteError() 0x084b02be：count ∉ [0,100000] 报错。
#include "GmCmds_E/checkCommandExecuteError_1.inc"
}

// ORIG getVendingMachineInfo 0x084b0444：找 vending 表首个 m_id==1 条目中
// 首个 m_groupNum==1 的 info；找不到返回 0。直调 CDataManager::getVendingMachineTable。
// b28d-solo-GmCmds_E：ORIG 为标准 for 循环（begin 后 `jmp cond`，尾部 ppEv
// 回跳，命中 `sete;test;jne` 物化岛）；原 while(true)+break 形改 for 复现。
// p167 补修（P96 反向）：ORIG 0x084b0452/0x084b0467 调 const 版
// `_ZNKSt6vector...5beginEv/3endEv` + `PK18stVendingMachine_t` const_iterator
// （operator->/operator*/ppEv/ne 全 const 族）⇒ table 改 `const vector*`、
// 两循环迭代器改 const_iterator（返回值 const_cast 为零指令编译期操作）。
#include "GmCmds_E/getVendingMachineInfo.inc"
}

// ORIG getVendingMachineItem(map&) 0x084b02e6：以 time 种子 Mersenne 随机
// 模拟 count 次抽取，统计 itemId 出现次数。
// b28d-solo-GmCmds_E：96=96 AE——删 key 具名局部（find 实参为 unsigned 成员经
// 转换临时、insert 处重新读成员并 `(int)` cast 成 make_pair<int,int> 右值实参）+
// P222 正向包裹 + CMTRand 局部 aligned(4)（真类含 unsigned long 成员）+
// 空 else{return;}（P285 保留循环出口 jmp+nop 岛）。原 N95 判词作废。
#include "GmCmds_E/getVendingMachineItem.inc"
}

// ORIG processVendingMachineItemLottery(CMTRand&, info const*, Inven_Item&)
// 0x084b0550：在 [0, m_giveCount-1] 掷点，扫 m_vecGive 取首个累计权重
// 大于掷点的输出，把 itemId/m_count 写入物品 +0x02/+0x07。
// b28d-solo-GmCmds_E：62=62 AE——stVendingMachineOutput_t::m_itemId 改
// unsigned int（CDataManager.h，P211 字段侧同型化，探针 /tmp/probe_fields.cpp）
// 后 `item.m_addInfo = it->m_itemId` 复现 ORIG 单段 `mov (%eax),%edx`；
// 原 N18a/N89 判词作废。
#include "GmCmds_E/processVendingMachineItemLottery.inc"
}

// ORIG execute 0x084b0082：校验后按 mode==1 跑售货机统计并分片落盘。
// b28d-solo-GmCmds_E：P167 CLEAR（18/18）；162=160，ratio 0.9317。本次修正：
// ① fname 改 255 字节 + 声明序提前到 fileBuf 之前（ORIG 4.1 清零展开
//    `lea -0x153; mov $0xff,%ebx` 在前、fileBuf `call memset` 在后；改后
//    fname 起点 -0x153 非 4 对齐触发 P312 慢路，fileBuf 起点 -0x1746b 精确对齐）；
// ② 全函数槽位/帧已逐条对齐（唯一残差 = ORIG memset 首参 `lea` 先行 + size 经
//    `mov $0x17318,%edx` 中转，4.4 恒右到左直接压立即数；`&fileBuf[0]`/字面量/
//    sizeof 三形实测同形 ⇒ 跨版本 P75/手册「待解」项封顶）。
#include "GmCmds_E/execute_8.inc"
}

// ===========================================================================
// CRecipeSuccess（sizeof=0xc）
//   +0x08 int 强制成功率档位（0=无 / 1=success / 2=fail）
// ===========================================================================

// ORIG ctor 0x082a8972：基类 ctor → 自身 vptr；string@+0x08 默认构造。

// ORIG parse 0x084b399a：parser 非空时按 args[1] 文本映射 1/2/0。
#include "GmCmds_E/parse_8.inc"
}

// ORIG execute 0x084b3a34：GetUser()->recipeForceProbability(slot)。
#include "GmCmds_E/execute_9.inc"
}

// ===========================================================================
// CRefund（sizeof=0x10）
//   +0x08 int 参数一 / +0x0c int 参数二（ORIG execute 为空函数，参数仅解析）
// ===========================================================================

// ORIG ctor 0x082a8602：基类 ctor → 自身 vptr；string@+0x08 默认构造。

// ORIG parse 0x084ac96c：trim；find(" ") 切两段分别 atoi 入 +0x08/+0x0c。
// b28d-solo-GmCmds_E：86=86 AE——与 CPickup::parse 同修（`size_t pos`→`int pos`，
// P218 同族 signed 索引局部旋钮）；原 P75/N18a 判词作废。
#include "GmCmds_E/parse_9.inc"
}

// ORIG execute 0x084aca86：空实现。
#include "GmCmds_E/execute_10.inc"
}

} // namespace game_master