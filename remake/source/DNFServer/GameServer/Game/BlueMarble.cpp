// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 BlueMarble/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r BlueMarble（G2-3 PvP 链，大富翁房间，池化 300）还原（2026-08-17）。
// 逐函数对照 docs/class_func_reports/BlueMarble.md 与 ORIG 反汇编实现；
// 目标：编译产物与 ORIG 逐操作数一致（AE 口径）。
// 依赖子对象（BlueMarbleScriptManager/BlueMarbleUserInfo/CMTRand/...）为
// PvP_deps 桩；本 TU 以 asm-label extern 发跨类调用。只写本文件。
#include <cstdio>
#include <cstring>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "BlueMarble.h"
#include "CDataManager.h"
#include "CInventory.h"          // 权威 CInventory（原本地最小声明与 CInventory.h:418 重定义冲突）
#include "CEnvironment.h"
#include "LogManager.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "PacketGuard.h"   // 权威 PacketGuard（0xc）
#include "GameWorld.h"            // G_GameWorld / GameWorld::gotoBlueMarble/outFromBlueMarble（§9）
#include "CPrivateStoreMgr.h"     // private_store::GetInstancePrivateStoreMgr/IsBusyPrivateStore（§9）
#include "RDARScriptStringManager.h"  // g_scriptStringManager_/findString（§9）
#include "CGameManager.h"         // WongWork::CMailBoxHelper::ReqDBSendNewSystemMail + G_CGameManager（§9）
#include "PacketTypes.h"          // 权威 MSG_MAILBOX_SEND（+0x11/+0x39/+0x3d 具名字段）

// 2026-09-05 裸偏移收尾：原本地 shadow struct BM_ScriptFields（把 m_pScript 当
// 未建模字节区直读 +0x30..+0x4c）删除 —— 该区间即权威
// BlueMarbleScriptManager::m_info（+0x30，BlueMarbleInfoScript），逐槽改用
// m_pScript->m_info.<字段>（证据：getRandomItem 0x080d7690 `mov 0x4c(%eax)`、
// setDiceNumber 0x080d6f88 `mov 0x48/0x44(%eax)`、checkUserConditionEnterBlueMarble
// 0x080d8d02 `mov 0x30/0x34/0x38/0x3c(%eax)`，均 4 字节读）；private 槽位经
// BlueMarbleInfoScript 的 `friend class BlueMarble` 授权（ORIG 无 getter 符号）。

// ---- CGameManager / G_CGameManager：经 #include "CGameManager.h"（§9） ----

// ============================================================================
// BlueMarble 实现
// ============================================================================

// [b28d-solo-BlueMarble AE] seed 循环修复：`volatile unsigned long seedValue`
// 强制 4.4 先求值实参再算 receiver，逐条复现 ORIG 0x080d8b22 段
// `mov -0x20(%ebp),%eax; …; lea (%ecx,%edx,1),%edx`（P316 volatile 求值序；
// 非 volatile 恒 receiver 先 + `add`）。旧 N(P75/N4) 判词（P226 语句表达式/
// 指针形/具名实参无效）作废；104=104 gap 0。ORIG 0x080d8a5c
#include "BlueMarble/BlueMarble.inc"
}

#include "BlueMarble/dtor_BlueMarble.inc"
}

#include "BlueMarble/getMemoryPoolIndex.inc"
}

#include "BlueMarble/setMemoryPoolIndex.inc"
}

#include "BlueMarble/getState.inc"
}

#include "BlueMarble/changeState.inc"
}

#include "BlueMarble/getWinner.inc"
}

#include "BlueMarble/getWaitUserCount.inc"
}

#include "BlueMarble/getUserSlot.inc"
}

#include "BlueMarble/getUserState.inc"
}

#include "BlueMarble/setUserState.inc"
}

// [b28d-solo-BlueMarble AE] 关键修复：`int zone` 具名局部改内联右值实参
// `mm.insert(std::make_pair(m_users[i].getBoardZone(), i))`——ORIG zone 槽
// -0x1c 是 make_pair<int,int&> 右值形参的匿名临时物化（非具名局部），4.4 据此
// 把临时区整体对齐 ORIG（zone -0x1c / sret -0x24 / pair -0x2c / insert 临时
// -0x30；179=179 gap 0）；并删除原 P190 float framePad（内联后自然对齐，
// pad 反致整体深移 4）。具名 zone 三形（块内/头部/先声明后赋值）恒落 -0x54
// ⇒ 原 N93/N82附 判词作废。P234 去 `std::move(zone)`（ORIG 无 out-of-line
// move call）保留。ORIG 0x080d7afc
#include "BlueMarble/setUserGrade.inc"
}

#include "BlueMarble/getUserDeadCount.inc"
}

#include "BlueMarble/setUserDeadCount.inc"
}

#include "BlueMarble/getMaxBoardSize.inc"
}

// b28c-1 AE: P223 尾标签后 0 差。
#include "BlueMarble/setMaxBoardSize.inc"
}

#include "BlueMarble/getDiceNumber.inc"
}

#include "BlueMarble/setDiceNumber.inc"
}

#include "BlueMarble/getBlueMarbleMap.inc"
}

#include "BlueMarble/setBlueMarbleMap.inc"
}

#include "BlueMarble/getMovePosition.inc"
}

#include "BlueMarble/setMovePosition.inc"
}

#include "BlueMarble/getVisitZone.inc"
}

#include "BlueMarble/setVisitZone.inc"
}

#include "BlueMarble/getDungeonIndex.inc"
}

#include "BlueMarble/setDungeonIndex.inc"
}

#include "BlueMarble/getItemIndex.inc"
}

#include "BlueMarble/setItemIndex.inc"
}

#include "BlueMarble/getExpBuff.inc"
}

#include "BlueMarble/setExpBuff.inc"
}

#include "BlueMarble/getGoldBuff.inc"
}

#include "BlueMarble/setGoldBuff.inc"
}

#include "BlueMarble/getDropBuff.inc"
}

#include "BlueMarble/setDropBuff.inc"
}

// [b28d-solo-BlueMarble AE] P251/P261+P305：原 if/goto 链使 4.4 每次重载形参槽
// （多 2 载入 + 3 nop 岛，53/58）；改 `switch((int)type)`（窄枚举必须 (int) 化，
// 否则 case 1/2 被 -fstrict-enums 静默删）后 type 单次载入复用 eax，53=53 ndiff 0。
// ORIG 0x080d990e
#include "BlueMarble/getBuff.inc"
}

// b28c-1 AE: P218 下标 int 化（unsigned i→int i）后 0 差。
#include "BlueMarble/getRandomItem.inc"
}

// [b28d-solo-BlueMarble AE] 三处修复：①m_char4 char→bool（BlueMarbleTileScript.h，
// 已 R2 广播）+ 裸 `if(!tile->m_char4)` 复现 ORIG 0x080d74a5 `movzbl; xor $1;
// test; je`（P292/P300）；②type 分派 if 链改 `switch((int)type)`（窄枚举必须
// (int) 化，P251/P261/P305）复现单次载入 + cmp$7/$8/$2 链；③去掉 `int r` 局部
// （ORIG randInt 结果直入 %ebx、无槽，P0）后帧顶槽基同时对齐。214=214 ndiff 0。
// ORIG 0x080d73dc
#include "BlueMarble/getRandomDungeon.inc"
}

#include "BlueMarble/checkJoin.inc"
}

#include "BlueMarble/join.inc"
}

#include "BlueMarble/leaveUser.inc"
}

#include "BlueMarble/setScript.inc"
}

#include "BlueMarble/setUserInfo.inc"
}

#include "BlueMarble/resetBlueMarble.inc"
}

#include "BlueMarble/resetBuffInfo.inc"
}

#include "BlueMarble/resetIndexInfo.inc"
}

// [b28d-solo-BlueMarble AE + 真语义 bug] moveSpace 守卫原写反（==1 入主流程，
// ORIG 是 ==1 返回 0x17）；改 `(getMoveSpace()-1)==0` early-return 同时复现
// ORIG `sete; test; je <主流程>` 落点，175=175 ndiff 0。ORIG 0x080d8d02
#include "BlueMarble/checkUserConditionEnterBlueMarble.inc"
}

// [b28d-solo-BlueMarble NEAR(26)/N82附] 三处结构修复：①1024B 数组声明移入 if 块
// （ORIG rep stos 在 user==0 判空之后，0x080d9c28-3e）；②memoryPoolIndex/
// characNo/m_id 加纯 register 提示复现 ORIG edi/esi/ebx 跨调用保活
// （P231/P248 三条件；push edi/esi/ebx 齐备，无 asm 绑定）；③空 else{return;}
// 复现尾 `jmp <epilogue>; nop` 岛（P285）。137=137 gap 0。
// 余 26 条 = 4.4 标量/聚合分区（N82附）：buf -0x430 vs ORIG -0x424、三标量
// -0x28/-0x24/-0x20 vs ORIG -0x434/-0x430/-0x42c、log -0x30 vs -0x24。
// 本轮再实测两形均不翻：标量移入 if 块（buf 之后）、三标量包局部聚合 struct
// （劣化 DIFF，深槽多 -0x43c/-0x44c）；连同旧「块内提头/标量 register/全头部
// 声明」共六形 ⇒ N82附/P112 封顶。
// ORIG 0x080d9c12
#include "BlueMarble/checkBlueMarbleUserConfirm.inc"
}

#include "BlueMarble/enterBlueMarble.inc"
}

#include "BlueMarble/checkStartBlueMarble.inc"
}

#include "BlueMarble/startBlueMarble.inc"
}

// b28c-1 N: 第三条件 je/jne 极性（4.4 块序折叠；三目/表达式/具名 bool 三形实测均不可翻）。
// [b28d-32 AE + 真语义 bug: 旧实现两 return 值写反（全真返回 false）；取反条件形复现 ORIG 三条件 jle/jne/je + bVar1 双岛 | ORIG 0x080d6ed8]
#include "BlueMarble/checkThrowDice.inc"
}

#include "BlueMarble/throwDice.inc"
}

#include "BlueMarble/userThrowDice.inc"
}

// [b28d-32 AE] ORIG 0x080d9c08 尾岛 `jmp +0x1bf; nop`：P223 fC 条件 goto 前置
// （`if(!checkThrowDice) goto DONE; body; DONE:;`）复现 128=128 逐条全等；
// 旧 body 尾 goto 恒被 4.4 jump-thread（少 jmp+nop 2 条）。
#include "BlueMarble/movePosition.inc"
}

#include "BlueMarble/checkMoveFirstZone.inc"
}

// b28c-1 AE: P0/N96 清绑 + P223 双岛（0x080d737c/0x080d737f）后 0 差。
#include "BlueMarble/tileType.inc"
}

#include "BlueMarble/intoBoardZone.inc"
}

#include "BlueMarble/checkLastZoneDungeonClear.inc"
}

#include "BlueMarble/enterDungeon.inc"
}

#include "BlueMarble/exitDungeon.inc"
}

#include "BlueMarble/clearDungeon.inc"
}

// b28c-1 AE: P0/N96 清绑 + P223 三岛（0x080d96e7/ea/ed）后 0 差。
#include "BlueMarble/giveupDungeon.inc"
}

// b28c-1 AE: P223 三岛 + P194 反形裸短路后 0 差。
#include "BlueMarble/giveBonusItem.inc"
}

// [b28d-solo-BlueMarble AE] P223 尾岛：NEXT0 岛尾必须写 `continue;`——`goto NEXT1`
// 会被 4.4 jump-thread（NEXT0 fall-through 即 NEXT2→NEXT1），少 ORIG 0x080d7e6d
// 第二条 `jmp +0x92`；continue 形保留该 jmp，双 nop 岛齐全，53=53 ndiff 0。
// ORIG 0x080d7dde
#include "BlueMarble/resultReward.inc"
}

#include "BlueMarble/finishBlueMarble.inc"
}

// [b28d-solo-BlueMarble AE] P251/P261：if-else 链（带 -0xc grade 槽 + 逐次重载
// 比较）改 `switch (getGrade())` 后 4.4 复现 ORIG 决策树（cmp1/je、cmp1/jg、
// test/je、cmp2、cmp3）与 case 块序 0,1,2,3，104=104 ndiff 0。ORIG 0x080d7e8e
#include "BlueMarble/setResultItem.inc"
}

// [b28d-solo-BlueMarble NEAR(N7/N109)] P0 去缓存/register 化 + P235 union 视角后
// 177=177 gap 0。修法：addInfo/msg268/msg269/mailRet 四个具名局部内联
// （ORIG 分别直入 %ebx、直作 strncpy 实参、`test;sete;test;je` 无槽比较）、
// serverGroup/len 纯 register 复现 callee-saved 保活（P231/P248）、characNo
// 内联实参、MSG 载荷改 char[0x145]+union{raw,MSG_MAILBOX_SEND msg} 视角
// （P235；完整类型成员直访问使三字段 store 折叠为 -0x236/-0x232 绝对 disp）。
// 本轮修复 ②：strncpy dst 改 `local_273.raw + 0x11`（数组退化 + 常量偏移）
// 逐条复现 ORIG 0x080d8273 `lea -0x26f; add $0x11`；具名结构成员
// `msg.m_name11` 恒折 `lea 0x11(%eax)` ⇒ 原 N102/N4 判词作废（DIFF→NEAR）。
// 余 N7/N109：%esi/%ebx 角色互换（ORIG serverGroup→ebx、len→esi；4.4 按首次
// 使用恒 serverGroup→esi；声明序交换、声明/赋值分离两形实测均不翻）。
// ORIG 0x080d800e
#include "BlueMarble/insertItem.inc"
}

#include "BlueMarble/setLeaveUserBlueMarbleState.inc"
}

#include "BlueMarble/setLeaveUserSlot.inc"
}

#include "BlueMarble/sendBlueMarbleDiceInfo.inc"
}

#include "BlueMarble/sendBlueMarbleRoomInfo.inc"
}

#include "BlueMarble/sendBoardZoneActionInfo.inc"
}

// b28c-1 AE: P223 尾空标签岛（0x080d87f7 jmp+nop）后 0 差。
#include "BlueMarble/sendFinishBlueMarble.inc"
}

#include "BlueMarble/sendStartBlueMarble.inc"
}

#include "BlueMarble/sendTileGiveUpInfo.inc"
}

#include "BlueMarble/sendUpdateItemInfo.inc"
}

#include "BlueMarble/sendToBlueMarble.inc"
}
