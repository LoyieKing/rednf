// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CGuildServerProxy/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include "LogManager.h"
#include "FunctionGuildMember.h"
#include "Packet_Monitor_Call_Guild_Members_ToChannel.h"  // ST_Guild_Mem_Info 完整布局
#include "PacketGuard.h"                                  // PacketGuard 完整类型（put_* 调用）
#include "CMyFileLog.h"
#include "CEnvironment.h"
// df_game_r 还原 —— CGuildServerProxy（公会服务器客户端代理）实现。
// 参照 docs/class_func_reports/CGuildServerProxy.md；单 TU 编译后经
// check_tu_game_orig.sh 与 df_game_r ORIG 对比（AE 口径）。
//
// [b28d-solo-CGuildServerProxy 泳道判词 2026-09-09] 清单 5 件全部 N 封顶，
//   TU 计数 identical+ae=169 / near=1 / diff=5（与基线同值，零劣化）；
//   p167 五符号全 CLEAR（0 MISSING / 0 EXTRA，语义调用点完整）。
//   本轮新范式复验（P309/P311/P316/P226/P227/P211 族）均为负结果，逐件记于函数头。
//
// [b28d-PoolDBLogin 池批·本轮] ≤30 小件 4 件复核（check_tu_routed.sh 现编现测）。
//   TU 三数 基线 identical=30 ae=139 near=1 diff=5 → 现在 identical=30 ae=139
//   near=1 diff=5（**零变差**；两轮 Main-P326 试验各造成 near 1→0 / diff 5→6，
//   均已回滚并 fresh 复测回基线）。修成 0 件，4 件维持封 N（solo_diff 行级读数）：
//    ChangeGuildNameInProxy NEAR ndiff=10（gap=0）：N98 纯槽号互换——end() sret 我方
//      -0xc / ORIG -0x10，pGuild 我方 -0x10 / ORIG -0xc，53=53 逐条同；P326 形实测
//      10→19、ours 53→58（ORIG 比较调点 = 非 const iterator::eq @0x0846ea88，无
//      转换临时槽）⇒ 不适用，读数并列于函数头。
//    SendGameServerInfo DIFF ndiff=6：N75/N107/N18a——ORIG 压栈前 `lea -0x27(%ebp);
//      lea 0xd(%eax),%edx` 两步保活 dst，4.4 恒右起求值且折 `lea+add`；前轮 13 形
//      + P311 元素取址形均不闭合 ⇒ S1 耗尽。
//    ModifyGuildInfoProxy DIFF ndiff=31：N107/N76——逐字段拷贝段 ORIG `lea 0x4(%eax),%edx`
//      物化 dst、src 装 %eax；4.4 恒 dst 留 %eax（+4 折进各 disp，故 62 vs 63 少 1 条）；
//      P326 形实测 31→42（+5 条）已回滚，ORIG 同为非 const eq @0x0846e9a8。
//    SendGuildExpLimit DIFF ndiff=3（gap=-1）：N94——ORIG `add $0x14aa,%edx;
//      mov 0x1c(%eax,%edx,8)` vs 4.4 `mov 0xa56c(%eax,%edx,8)`；0xa56c 非 8 倍数
//      ⇒ 拆常量的数组视角不可构造。
//   本文件 p167 四件全 CLEAR；无 `register ... asm("reg")`；无 .h 改动（R2 无申报项）。
//   [注] 旧判词记的静态初始化器符号名差异（`_GLOBAL__I__ZN12STGuildProxyC2Ev`）
//   已不复现：现编 .o 的静态初始化器符号名即为
//   `_GLOBAL__I__ZN17CGuildServerProxyC2ESsii`（与 ORIG 同名同体：`sub $0x18`
//   +压 0xffff/1+call SIAD，逐条 identical）；SIAD 体亦逐条对齐（含 __ioinit
//   C1 + __cxa_atexit，P338 的 `<iostream>` 已由 CUser.h 链带入）。
//
// [SweepCGuildServerProxy 2026-09-10 本轮] gap' 排行（dual_rank --tu）候选 3 件
//   （InitTcp gap'2 / ModifyGuildInfoProxy gap'1 / SendGuildExpLimit gap'1，miss=extra=0），
//   加 NEAR 1 + DIFF 2（SendGameServerInfo / ChangeGuildNameInProxy）共 5 件复核；
//   另加虚槽/jcc/调用目标/字符串/数据符号五路交叉核对。结论：
//   ① **修真 bug 1 处**：SendLogin 第二参书写类型应为 `memberIdentificationNumber_t`
//      （ORIG `__PRETTY_FUNCTION__` 串 0x08c69000 段实证；mangled `...EijiPchS0_` 不变）
//      —— 已改定义签名并在本 TU 就近 typedef（重复同型 typedef 合法）。修后 .rodata
//      串 = `void CGuildServerProxy::SendLogin(int, memberIdentificationNumber_t, int,
//      char*, unsigned char, char*)` 与 ORIG 逐字节同；TU 四数不变（零劣化）。
//   ② 虚槽核对（P193/P282）：本 TU 全部 3 处 `call *%reg` 反查 _ZTV ——
//      OnBuyItem vptr+0x28 = CEquipItem/CStackableItem::GetGuildPowerWarPoint（0x08514e04/
//      0x08514ade）、vptr+0x2c = GetUserPowerWarPoint（0x08514e12/0x08514aec）；
//      SendPowerWarPoint vptr+0x34 = CEventBase::IsEventing(CUser*)（0x080c84fa）——全对。
//   ③ jcc 逐位核对：5 件条件跳转助记符序列与 ORIG 全等（InitTcp jne/jns/je、
//      ModifyGuildInfoProxy je、ChangeGuildNameInProxy je、SendGuildExpLimit jne、
//      SendGameServerInfo 无 jcc）——无条件写反。
//   ④ 调用目标多重集（含 C1/C2·D1/D2 同址别名归一后）全等，仅 GI→SIAD 与 N123 撞名豁免。
//   ⑤ 字符串字面量多重集：除①外全等；数据符号引用多重集全等（仅 `__PRETTY_FUNCTION__`
//      的 `_ZZ...` 具名静态 vs 匿名 .rodata 属 P139 跨版本符号差）。
//   ⑥ 地址区间 0x0846d800-0x0846f260 / 0x084ca300-0x084ca700 内 ORIG 函数本 TU 全有
//      （无缺函数；`CUser::SetGuildInfoProxy` 属 CUser TU，非本件）。
//   ⑦ 3 件 gap 候选维持封 N（语义逐条复核正确）：InitTcp=N80（varargs 跨 ctor 保活；
//      P226/P227/P231/P212 已试，本轮未再劣化）；SendGuildExpLimit=N94（探针复证：
//      指针视图形 4.4 产 `add+shl+add` 6 条、成员数组视图形产 `lea 0x14aa+scaled`
//      仍非 ORIG `add $0x14aa,%edx`；构造视图须改 CDataManager.h，收益 1 条不抵风险）；
//      ModifyGuildInfoProxy=N107/N76（隐式逐块拷贝 dst/src 寄存器角色差，9+2 形已穷举）。
//   ⑧ 判据㉓ 绑定修复（Main 批准，R2 已广播）：STGuildProxy 两 ctor 由本 .cpp 外联
//      定义（我方 T）改为 CGuildServerProxy.h 内 inline 定义。ORIG 两符号均 **W**：
//      `_ZN12STGuildProxyC1Ev/C2Ev` @0x082344de(0x38)、`_ZN12STGuildProxyC1EPKchh/
//      C2EPKchh` @0x084ed19a(0x42)，地址分散在非本 TU 段（后者落在 InterDispatcher
//      段）⇒ 头内定义、使用点发射（P233）。sizeof(STGuildProxy)=0x1a、m_guildName@+0、
//      m_pad16@+0x16、m_level@+0x17、m_powerSide@+0x18、m_agitFlag@+0x19 与全部
//      mangled **零变化**。修后本 TU：C1Ev/C2Ev 绑定 T→W（16/16 AE 不变）；3 参 ctor
//      不再发射（ORIG 本 TU 段亦无副本，其 W 副本由 InterDispatcher.cpp 发射）；
//      P210 复核：LoadGuildInfoProxyFromDB 帧未变（156/156 AE），ChangeGuildNameInProxy
//      仍 NEAR、其余 5 件 verdict 不变；TU 四数 31/139/1/5 → 31/138/1/5（件数 -1
//      即移除本 TU 不该发射的 3 参 ctor 符号）；InterDispatcher.cpp 复编 1455→1457
//      （+2 AE，属主 SweepInterDispatcher-2 确认零回退，新增 2 符号为 4.4 的 C5E/D5Ev
//      local 别名结构性噪音）。头内新增 `#include <string.h>`（inline 体 memset/memcpy）。
//   本文件 p167 四件全 CLEAR；无 `register ... asm("reg")`。

#include "CGuildServerProxy.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "CUser.h"
#include "CDataManager.h"

#include "Packet_Add_Guild_Fund.h"
#include "Packet_Change_Char_Name.h"
#include "Packet_Change_Guild_Name.h"
#include "Packet_Game_Server_Regist.h"
#include "Packet_GM_PowerWar_End.h"
#include "Packet_GM_PowerWar_Start.h"
#include "Packet_Guild_Apply_Origial_Power_Side.h"
#include "Packet_Guild_Call_Guild_Invite.h"
#include "Packet_Guild_Change_Power_War_Point.h"
#include "Packet_Guild_Create_Guild_Agit.h"
#include "Packet_Guild_Delete_Guild_Agit.h"
#include "Packet_Guild_Exp_Limit.h"
#include "Packet_Guild_Increase_Decrease_Power_War_Point.h"
#include "Packet_Guild_Power_War_Process_Info.h"
#include "Packet_Guild_Reply_Guild_Invite_From_Invited.h"
#include "Packet_Guild_Request_Guild_Board_Delete.h"
#include "Packet_Guild_Request_Guild_Board_Open.h"
#include "Packet_Guild_Request_Guild_Board_Write.h"
#include "Packet_Guild_Request_Guild_Master_Delegate.h"
#include "Packet_Guild_Request_Guild_Secede.h"
#include "Packet_Guild_Upgrade_Guild_Agit.h"
#include "Packet_Guild_Write_Guild_Member_Memo.h"
#include "Packet_Monitor_Buy_Guild_Skill.h"
#include "Packet_Monitor_Call_Guild_All_Members.h"
#include "Packet_Monitor_Call_Guild_Info.h"
#include "Packet_Monitor_Call_Guild_Level_Up.h"
#include "Packet_Monitor_Call_Guild_Members.h"
#include "Packet_Monitor_Char_Changable_Info.h"
#include "Packet_Monitor_Increase_Guild_Exp.h"
#include "Packet_Monitor_Send_Guild_Mail.h"
#include "Packet_Monitor_UDP_HeartBeat.h"
#include "Packet_Monitor_UDP_Login.h"
#include "Packet_Monitor_UDP_Logout.h"
#include "Packet_Notice_Guild_War_End.h"
#include "Packet_Notice_Guild_War_Start.h"
#include "Packet_Notice_Power_War_CFG.h"
#include "Packet_Refresh_Guild_Info.h"
#include "Packet_Request_Guild_Create.h"
#include "Packet_Request_Join_Power.h"
#include "Packet_Request_Power_War_Start_Info.h"
#include "Packet_Request_Secede_Power.h"
#include "Packet_Web_Notify_Message_To_Guild.h"

#include "DNFFunctionLib.h"

// ORIG `__PRETTY_FUNCTION__` 串实证（0x0846db9f 引用、0x08c69000 段）：
// SendLogin 第二参书写为 memberIdentificationNumber_t（= unsigned int，mangled
// `...EijiPchS0_` 不变）。全仓同 typedef 亦见 InterSelectPcroomDailyReward.h /
// AuctionPacket.h，此处按 TU 需要就近声明（重复 typedef 同型合法）。
typedef unsigned int memberIdentificationNumber_t;

// （static 3 参 int&+ip+port）的调用以 m_tcp 自身地址作 int& 实参（ORIG
// 0846d9be 三槽 this+0x24/c_str()/m_tcpPort 实证）。
// ---- 生命周期 ----

// ---- STGuildProxy（ORIG 弱符号 0x082344de / 0x084ed19a）----

// ORIG 强符号（0x0846d818 T）：头文件仅声明，此处给出非内联定义。
// [B24D判词 N] 语义正确：string ip/udp/tcp 三参 → m_ip=ip + map C1 + CNetwork C1 +
//   CUdpHandler C1。diff=EH 清理 landing pad 我方多分离一 `jmp` 跳转（4.1 vs 4.4），
//   61/60 纯 lowering 表示差异，语义等价。
#include "CGuildServerProxy/CGuildServerProxy.inc"
}

#include "CGuildServerProxy/Init.inc"
}

#include "CGuildServerProxy/InitUdp.inc"
}

// [b28d-solo-CGuildServerProxy N80: 74/72] P225 while 形已复现 ORIG @0x0846d9c1
//   `cmpl $0x0,-0x1c(%ebp); setne %al; test %al,%al; je`（原 if 形 4.4 恒折
//   `cmpl;je` 少 2 条）。残余=4.1 在 cMyTrace ctor 前把 varargs 右起求值并
//   callee-saved 保活（strerror→ebx、errno→edi、ret→esi ⇒ push edi + sub $0x5c）；
//   4.4 只保活两个调用结果（strerror→ebx、errno→esi），槽变量 ret 恒在使用点
//   `mov -0xc(%ebp),%eax` ⇒ 缺 push edi、帧 -0x10、全槽基差 0x10。P226 语句表达式
//   `({ ret; })`、P227 全表达式临时链均实测零效；P231 裸 register 反使 ret 失槽
//   （ORIG 有槽 -0x1c，违 P0 判读）；b28d 复验追加：varargs 首参改
//   `(*(volatile int*)&ret)` 强制 volatile 读（74→76 劣化，仍缺 push edi）
//   ⇒ N80 封顶。b28d-solo2 复验：ret 声明移至函数头（P212/P309 头部声明律）实测
//   零效（ret 仍 -0xc、帧仍 0x50、缺 push edi）⇒ N80 定案 | ORIG 0x0846d96c
#include "CGuildServerProxy/InitTcp.inc"
}

#include "CGuildServerProxy/Disconnect.inc"
}

// ---- 收发 ----

#include "CGuildServerProxy/SendPacket.inc"
}

#include "CGuildServerProxy/SendTcpPacket.inc"
}

#include "CGuildServerProxy/OnTcpRecv.inc"
}

#include "CGuildServerProxy/OnTcpSend.inc"
}

#include "CGuildServerProxy/OnTcpError.inc"
}

#include "CGuildServerProxy/Parsing.inc"
}

// ---- Send*：登录/心跳/信息上报 ----

#include "CGuildServerProxy/SendCharLevelGrowType.inc"
}

#include "CGuildServerProxy/SendHeartBeat.inc"
}

#include "CGuildServerProxy/SendLogin.inc"
}

#include "CGuildServerProxy/SendLogout.inc"
}

#include "CGuildServerProxy/SendPacketCallGuildMembers.inc"
}

#include "CGuildServerProxy/SendPacketCallGuildAllMembers.inc"
}

#include "CGuildServerProxy/SendGuildLevelup.inc"
}

#include "CGuildServerProxy/SendCallGuildInfo.inc"
}

#include "CGuildServerProxy/SendPowerWarStart.inc"
}

#include "CGuildServerProxy/SendPowerWarEnd.inc"
}

#include "CGuildServerProxy/SendPowerWarProcessInfo.inc"
}

#include "CGuildServerProxy/SendGuildWarEnd.inc"
}

#include "CGuildServerProxy/SendGuildWarStart.inc"
}

#include "CGuildServerProxy/SendPowerWarCfg.inc"
}

#include "CGuildServerProxy/SendBuyGuildSkill.inc"
}

#include "CGuildServerProxy/SendGuildMail.inc"
}

#include "CGuildServerProxy/SendCallGuildInvite.inc"
}

#include "CGuildServerProxy/SendReplyGuildInvite.inc"
}

#include "CGuildServerProxy/SendRequestGuildSecede.inc"
}

#include "CGuildServerProxy/SendMessageToGuild.inc"
}

#include "CGuildServerProxy/SendMemoToGuild.inc"
}

#include "CGuildServerProxy/SendGuildMasterDelegate.inc"
}

#include "CGuildServerProxy/SendGuildCreate.inc"
}

// ORIG @846e387：param_1 → m_charNo(+0xe)、param_2 → m_guildKey(+0xa)
// （调用点 Dispatchers.cpp:7786 传 (curNo, guildkey)）。
#include "CGuildServerProxy/SendGuildNameChange.inc"
}

#include "CGuildServerProxy/SendChangeCharName.inc"
}

#include "CGuildServerProxy/SendReqestPowerWarStartInfo.inc"
}

#include "CGuildServerProxy/SendPowerWarPoint.inc"
}

#include "CGuildServerProxy/SendJoinPower.inc"
}

#include "CGuildServerProxy/SendSecedePower.inc"
}

// ---- 公会信息缓存 ----

#include "CGuildServerProxy/LoadGuildInfoProxyFromDB.inc"
}

// [b28d-solo-CGuildServerProxy N107/N7: 63/62] 本件已修两处真缺口：
//   ① 删具名 endIt 改内联 end()（ORIG find 落 -0x14、end 落 -0x10；具名 endIt 使
//      4.4 反排成 -0xc/-0x10，与同 TU ChangeGuildNameInProxy 的 ORIG 槽序对齐）；
//   ② 补 pGuild 死槽复现 ORIG @0x0846e9eb/0x0846e9fc 的两次 ptEv（第一次 +0x4 写
//      -0xc 后全程未读，P171 死写入照抄；单语句 `it->second = info;` 只发一次 ptEv）。
//   残余仅逐字段拷贝段：ORIG `lea 0x4(%eax),%edx`（dst 物化 %edx、src 装 %eax），
//   4.4 恒 dst 留 %eax、src 装 %edx 且 +4 折 disp（N107/N18a）。实测 9 形
//   （显式 operator-> / (*it) / *(&it->second) / RHS cast 族 / 具名 q）均不可翻。
//   b28d 复验追加：`*(volatile STGuildProxy*)&it->second`（编译失败：volatile 对象无
//   非 volatile operator=）、`register STGuildProxy* pDst=&it->second; *pDst=info;`
//   （63→65 劣化）均不可用 | ORIG 0x0846e95e
//   b28d-solo2 复验追加：`it->second.operator=(info);` 显式限定调用实测与 `=` 逐条
//   同形（4.4 仍 dst=eax+disp / src=edx），N107/N76 族封顶不变 | ORIG 0x0846e95e
//   b28d-PoolDBLogin 复验（Main P326｜N93 翻案形，一次性）：
//   `it == std::map<unsigned int, STGuildProxy>::const_iterator(m_guildInfoMap.end())`
//   实测 ndiff 31→42、ours 62→67（+5 条 = const_iterator 转换 ctor + 转换后比较调点）
//   ⇒ 本件 ORIG 比较调点是**非 const** `_ZNKSt17_Rb_tree_iteratorISt4pairIKj12STGuildProxyEEeqERKS4_`
//   @0x0846e9a8，仅两槽 -0x14(it)/-0x10(end)，无第三级转换临时 ⇒ P326 形结构性不适用，
//   已回滚（回滚后 fresh 复测 = 基线 63/62 ndiff 31）。
#include "CGuildServerProxy/ModifyGuildInfoProxy.inc"
}

// [b28d-solo-CGuildServerProxy N98: 53/53 NEAR] P316 翻案（DIFF→NEAR）：pGuild
//   声明为 `STGuildProxy* volatile` 后，4.4 把 memset/memcpy 的 dst 装载提前到压栈
//   之前（ORIG @0x0846ead7 `mov -0xc(%ebp),%eax` 先于 `movl $0x17,0x8(%esp)`），
//   memcpy src 自然落 %edx（ORIG `mov 0x10(%ebp),%edx` 同形）⇒ 求值序/寄存器全对齐。
//   残余仅槽号互换：4.4 给 volatile pGuild=-0x10、end() 内联临时=-0xc，ORIG 反之
//   （pGuild=-0xc、end 临时=-0x10）。已实测不闭合：①声明移至函数头最前（it 得
//   -0x14 ✓ 但 pGuild/end 仍互换）；②具名 endIt（it/endIt 互换）；③
//   `*(STGuildProxy* volatile*)&pGuild` 读形（+2 条）。均属 N98 槽分配族 ⇒ 封顶
//   | ORIG 0x0846ea3e。b28d-solo2 复验追加：④`end()==it` 操作数互换（P74补②）；
//   ⑤`*&m_guildInfoMap.end()`（P316 sret 折叠抑制）；⑥pGuild 声明移到 it 之后——
//   三形 end() sret 仍恒 -0xc（it -0x14 / pGuild -0x10 或反向），槽号互换不变 ⇒ 定案。
//   ⑦pGuild 移入 else 块内声明（P314 块级旋钮）⇒ pGuild -0x14 / it -0x10 更劣，已回滚。
//   b28d-PoolDBLogin 复验（Main P326｜N93 翻案形，一次性）：同形实测 ndiff 10→19、
//   ours 53→58、NEAR→DIFF（TU 三数 near 1→0 / diff 5→6 **变差**）⇒ 本件 ORIG 比较
//   调点同为**非 const** iterator::eq @0x0846ea88，三级槽 = -0x14(it) / -0x10(end) /
//   -0xc(pGuild 指针，非迭代器临时)，无转换临时可落 ⇒ P326 形不适用，已回滚；
//   N98 槽号互换维持封顶。
#include "CGuildServerProxy/ChangeGuildNameInProxy.inc"
}

#include "CGuildServerProxy/ChangeGuildAgitFlagInProxy.inc"
}

#include "CGuildServerProxy/GetGuildInfoProxy.inc"
}

#include "CGuildServerProxy/InsertGuildInfoProxy.inc"
}

// ---- 公会资金/经验/PowerWar/根据地/公告板 ----

#include "CGuildServerProxy/SendIncreaseGuildExp.inc"
}

#include "CGuildServerProxy/SendIncreaseDecreasePowerWarPoint.inc"
}

#include "CGuildServerProxy/SendCreateGuildAgit.inc"
}

#include "CGuildServerProxy/SendDeleteGuildAgit.inc"
}

#include "CGuildServerProxy/SendUpgradeGuildAgit.inc"
}

// [b28d-solo-CGuildServerProxy N94: 43/42] 唯一残差=循环体 scaled-index 大常量拆分：
//   ORIG @0x0846ee96 `mov -0xc(%ebp),%edx; add $0x14aa,%edx; mov 0x1c(%eax,%edx,8),%eax`
//   （C%8=4 拆成 add+disp8），4.4 恒折单条 `mov 0xa56c(%eax,%edx,8),%eax`
//   （N94 定论：4.4 仅当 C%8==0 才拆，源形不可择）⇒ 封顶 | ORIG 0x0846ee44
//   b28d-solo2 复验追加：`*(m_levelUpCost[i] + 1)` 指针形劣化为 shl+add+add+mov 6 条；
//   P311 二维视图需 X*C=0x14aa 且第二维长 ≥0x11，本布局 0xa56c 非 8 倍数不可构造 ⇒ 定案。
#include "CGuildServerProxy/SendGuildExpLimit.inc"
}

#include "CGuildServerProxy/SendApplyOriginalPowerSide.inc"
}

#include "CGuildServerProxy/OnBuyItem.inc"
}

#include "CGuildServerProxy/SendGuildBoardOpen.inc"
}

#include "CGuildServerProxy/SendGuildBoardWrite.inc"
}

#include "CGuildServerProxy/SendGuildBoardDelete.inc"
}
// [b28d-solo-CGuildServerProxy N75/N107/N18a: 36/36] ORIG @0x0846f130 在压 strncpy
//   常量前 `lea -0x27(%ebp),%eax; lea 0xd(%eax),%edx` 两步保活 dst；4.4 恒右起求值
//   （dst 装载落最后）且折 `lea+add`。P226 语句表达式（({ pkt.m_name; })）、具名指针
//   中间量、P231 裸 register（v28 探针：双 lea+顺序对，但改用 %ebx 且 push/pop 破帧）
//   等 13 形实测均不闭合 ⇒ 封顶 | ORIG 0x0846f0f2
//   b28d-solo2 复验追加：`strncpy(&pkt.m_name[0], name, 0x10)`（P311 数组元素取址）
//   与 `pkt.m_name` 逐条同形（4.4 仍右起求值 + add）⇒ P75 跨版本求值序封顶不变。
#include "CGuildServerProxy/SendGameServerInfo.inc"
}

#include "CGuildServerProxy/SendRefreshGuildInfo.inc"
}

#include "CGuildServerProxy/SendAddGuildFund.inc"
}

// ---- FunctionGuildMember（ORIG 静态成员，公会成员包构造/日志辅助）----

// ORIG 0x084ca37a T _ZN19FunctionGuildMember26MakePacketGuildMembersInfoER11PacketGuardiP17ST_Guild_Mem_Info
// 报告 docs/class_func_reports/FunctionGuildMember/MakePacketGuildMembersInfo.md：
// put_byte(count) → 每成员：name(0x04)/guildMemDBInfo(0x22) int+len、
// level、job/growthType/char 渠道/性别/黑名单/MG、pad 秒差。
#include "CGuildServerProxy/MakePacketGuildMembersInfo.inc"
}

// ORIG 0x084ca55a T _ZN19FunctionGuildMember23LogMakeGuildMembersInfoEjiP17ST_Guild_Mem_Info
// 报告 LogMakeGuildMembersInfo.md：拼成员文本 → ./log/<file>/GuildModi 文件日志。
#include "CGuildServerProxy/LogMakeGuildMembersInfo.inc"
}
