// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 GlobalData/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// [L8GlobalData · round1 · 2026-09-10] 本批 33 件（/tmp/round1_worklist.md 的
//   `## GlobalData.cpp` 小节）逐件重核 + 修复。开工基线 check_tu_routed（18:16 路由 .o）：
//   identical=899 ae=1791 near=11 diff=38 missing_in_game=847。
//   工具：/tmp/l8/an.py（section-aware 双侧反汇编 + 判据① jcc 目标下标逐位比对）、
//        /tmp/l8/jcc5.py（jcc delta 双侧）、/tmp/l8/slots.py（槽直方图 = 判据⑪）。
//
// 真修复 3 件（含判据①/⑨/⑫ 推翻旧判词的实测证据）：
//  ① UdpThread::_processUDPData（ORIG 0x0857c5a4）：
//     a. **P259**：3 处 `Stream* stream = s_stream_pool->Acquire(...)` 具名局部删除、
//        内联进 CStreamGuard ctor 实参 ⇒ 槽集合收敛为与 ORIG **逐值全等**
//        {-0x1c,-0x20,-0x28,-0x30,-0x38}；帧 0x5c→0x4c = ORIG。
//     b. **判据⑨（旧判词"结构已修"不完整）**：ORIG 0x0857c696 段是 `switch(*data)`
//        决策树（`movsbl %al,%eax; cmp $0x1;je +63; cmp $0x1;jg; test;je; jmp;
//        cmp $0x2;je; cmp $0x3;je`），我方旧 if/else 链产 `cmp $0x1,%al;jne` 二段形
//        ⇒ 改 switch；并按 **P324 叶块物理序=书写序**（ORIG 树后紧跟 case 0 体）
//        把 case0 写在 case1 前。改后单件 ndiff 91→58，残差**全部为 AE 域数据地址**
//        （6 块：packet_counter 的 GVS/atexit 立即数 + counters 地址）；267→262 条
//        （ORIG 266，差 4 为 4.1 侧地址立即数重复计数，非结构差）。
//  ② UdpThread::dispatch（ORIG 0x0857c9de）：
//     a. **FD_ZERO 展开形**：ORIG 4.1-glibc 为逐元素循环（0x0857ca44 `lea -0xc8;
//        mov %eax,-0x24; movl $0,-0x28; …; movl $0,(%eax,%edx,4); addl $1,-0x28;
//        cmpl $0x1f,-0x28; setbe`），我方新头集走 `rep stos` 完全无形 ⇒ 手写等价循环
//        （`unsigned fi` + `int* fdBits`）复现；148→136 条（ORIG 137）。
//     b. **循环变量拆分**：ORIG 两处 for 各用独立变量（-0x20/-0x1c 各 6 次），我方合并
//        单 `i`（-0x2c×12）⇒ 改块内独立 `for (int i …)`。
//     c. **FD_SET/FD_ISSET 实参 cast unsigned**：ORIG `shr $0x5`（逻辑移位）vs 我方
//        int 版 `lea 0x1f/test/cmovs` 三条修正。
//     残余：maxfd 槽 -0x34 vs ORIG -0x48（8B 整段位移、2 槽）+ ORIG 0x0857c9ea
//     `movl $0x1,-0x38` 死写（P171）⇒ 判据⑪ 属 (b) 整帧/分区差，声明重排 3 次实测不收敛
//     ⇒ N115/N126 封顶。
//  ③ ItemVendingMachine::BillingRecord（ORIG 0x0817991e）：
//     **判据⑫（widthscan）+ P321 T1 直证**：ORIG 0x08179bc0 段 `movsbl -0x9(%ebp),%eax`
//     后 `cmp $0x1/0x2/0x3/0x4`（1 字节符号扩展读），我方 int 版产 4 条
//     `cmpl $K,-0xc(%ebp)` 槽直比（widthscan：cmp_l orig=0 our=5、ext_movsbl orig=2 our=0）
//     ⇒ `int cent = atoi(centBuf);` 改 **char**（值域 0..9，语义等价）；改后我方该段
//     cmpl 5→1，与 ORIG 单段 movsbl 形一致。
//
// 判 N（本轮重核证据；判据① jcc 目标下标逐位 + 判据⑪ 槽直方图）：
//   · CDBEnviroment::parse：槽集合置换（only OURS {-0x3c,-0x40,-0x5c,-0x60} vs
//     only ORIG {-0x30,-0x34,-0x48,-0x4c}，整体 8B 位移）；声明序对调实验（dbType
//     先于 map）后 map 槽 -0x5c→-0x48 仍不匹配 ⇒ 已回滚，判 N（4.1 跨块分配）。
//   · break_away_prevent::loadDbData：槽集合**全等**、243=243 条，但 jcc 目标差
//     （ours i137 jmp d=67 vs ORIG i135 jmp d=95；i187 d=5 vs i185 d=36）+ 下标链
//     `shl/add` vs ORIG `add %eax,%eax`（P205/N1）+ 首条 `movl $0x0,-0x10` 死写 ⇒ N1/N75。
//   · RentalMachinePcRoom::log / RentalMachineDimension::log（×2）：槽相对序颠倒
//     （ORIG serverEnv -0x2c / flog -0x24 / serverInfo -0x19；我方 flog -0x28 /
//     serverInfo -0x1d / serverEnv -0x1c）+ 帧 0x4c vs 0x3c + callee-saved 角色轮换
//     （ORIG esi=charNo/ebx=accId/edi=pData，我方 edi/esi/ebx）⇒ N7+N82。
//   · checkAllRentalItemScript / checkSelectionListScript：槽集合全等、三变量计数
//     分布置换（-0xc×2/-0x10×13/-0x14×10 vs 我方 -0xc×13/-0x10×10/-0x14×3）⇒ N82。
//   · Secu_DataControl::UpdateSecurityServiceState：槽集合全等；残差=我方尾岛
//     `jmp <epilogue>; nop`（N121）+2 条 ⇒ N121。
//   · CHumanCertifier::init：ORIG 表 @0x936e2a0 直读（P185）= `0x0814af00+2i`
//     （**.text 地址**）⇒ 运行时填充表、静态不可恢复；ORIG 0x082a73ed 取表项后直接
//     `_ZNSsC1EPKcRKSaIcE`（无 null 检查）⇒ 我方安全占位 +12 条不可消 ⇒ N（数据缺失）。
//   · 其余件（QueryConnInfo/Init/InitTcp/DispatchThread/CSimpleSSO/proc_addFatigue/
//     generateImage/NetworkThread::dispatch/RecvPacket/GetRentItemInfo/BillingRecord/
//     AcceptThread::dispatch/ProcessBuyRequestEx/ProcessIPG_ResultOutput/
//     MonitorTcpThread/init_function_type/pc_room::DeleteAllRentalItem/
//     SendCheckChannelScript/CServerProxyMgr D1×3）：维持历轮判词
//     （N7/N18a/N78/N82/N115/N121/N126/N130）；本轮 jcc 扫描无新增真结构差
//     （全部差异为指令数差引起的 jcc delta 平移 + 槽位分布），未再耗轮次。
// 已完整阅读报告：docs/class_func_reports/UdpThread/_processUDPData.md、
//   UdpThread/dispatch.md、RentalMachinePcRoom/log.md（三者 ## 汇编 + ## 反编译 C 全读）、
//   CDBEnviroment/parse.md（同源生成物全文）。
// 结论：本 TU 剩余真缺口 = 0（未闭合件均为跨版本结构性 N 类；2 件已实证缩 gap 且零回退）。
// ============================================================================

#include "LogManager.h"
// df_game_r GlobalData（D1 组）还原。
// 函数形态/字符串/池尺寸均取自 ORIG 反汇编与 Ghidra C。
// 外部类（后续批次）仅在本 TU 以最小 stub 声明，符号留给 ORIG/后续 TU。
//
// ============================================================================
// [Lane7GlobalData 2026-09-10 · 判据①全 TU jcc/jmp 目标下标逐位比对]
// 基线 check_tu_routed：identical=899 ae=1785 near=10 diff=45
//    （FT_Done_Memory 等 10 件第三方占位 + _Z41__static_init/__tcf_0 结构性
//     豁免 ⇒ **真实件 33**）。
// 收口：identical=899 ae=**1787** near=**11** diff=**42**（ae +2 / near +1 /
//    diff −3，零回退；合计函数数 2684→2686 = 2 件 DIFF→AE）。
// 判据①执行器：/tmp/l7/jcc4.py（objdump 双侧按 nm 符号区间取指令，逐 jcc/jmp
//    算「目标下标 − 自身下标」delta 双侧比对；extent 取 min(nm size, 下一符号)
//    避免弱符号 size 失真的假数据）；ctx.py 打点位上下文。
// 逐件（mangled 简写 / 段类型 / 修法 / 读数）：
//  ① CDBConnector::Connect（DIFF→AE，ndiff 22→0）：
//     判据①命中 #2/#4 极性 je↔jne（ORIG 0x080ec792/0x080ec7a4/0x080ec7b4 为
//     **正极性守卫** `test;je T;cmp $5;jne F;test %al;jne <跳过体>`）+ #1/#5/#6
//     目标差 2。修 3 处：①`if (!(a==0||a==5)){body}` → `if (a==0||a==5) continue;`
//     （探针确认正极性守卫复现 ORIG 落点）；②cMyTrace 变参 arg4/arg5 反序修正
//     （ORIG 先算 arg5=m_db(+0x46=second+4+0x42) 后 arg4=m_user(+0x1c)；
//     +0x04 基址来自 pair::second 偏移，与 CEnvironment.h 布局一致，非错布局）；
//     ③删 arg3 冗余 `(int)` cast（P41：多一条 `mov %eax,%edi` 中转）。
//     已读报告：docs/class_func_reports/CDBConnector/Connect.md（汇编+反编译 C 全读）。
//  ② pc_room::CheckInventoryEmptyCount（DIFF→AE，ndiff 22→0）：
//     判据①命中 7 处极性（ORIG 0x082710b9 族 `xor $1;test;jne 0x082711e9`）。
//     修法：改**单个三元全表达式链** `(!c1||…||!c7) ? true : (GetRemainCapacity>=need)`
//     （探针 /tmp/l7/probe1.cpp D 形逐条同形；A 嵌套形反产 je、B/C/E 多一级物化）。
//     旧嵌套形另含**真语义 bug**（运行语义=任一 check 为真即 true，与 ORIG 相反，
//     与其自述判词矛盾）。已读报告：pc_room/CheckInventoryEmptyCount.md。
//  ③ DispatchThread::dispatch（DIFF→NEAR，ndiff 257→38）：
//     判据①命中首差 #7 `js/jle` vs ORIG `je/je`；实为 **switch case 映射真 bug**：
//     ORIG 0x0847e61d `cmp $1;je +0x35f; cmp $2;je +0x442; test;je +0x3d2;
//     jmp +0x4b0` ⇒ case1→SmallStreamPool(0xfff)、**case0→s_stream_pool(0x3fff,
//     "Default…")**、case2→BigStreamPool(0x3ffff)、**default 空体**；旧源码把
//     case0 并入 case1（错发 SmallStreamPool）且把 s_stream_pool 体挂 default
//     （错：任意其它 streamType 在我们这边会误 Free）。按 P324 叶块物理序
//     （1,0,2）+ default 空体改写。已读报告：DispatchThread/dispatch.md。
//  ④ UdpThread::_processUDPData（DIFF 结构已修，ndiff 大残差）
//     ①判据①命中 #4 `je` vs ORIG `jne` ⇒ 空 then + else 体形
//     （`if (dispatchPacket(data)) { } else { … }`，探针 /tmp/l7/probe3.cpp t2）：
//     4.4 得 `test %al,%al; jne <岛>` 与 ORIG 逐条同形（旧 `if(!f())` 多 1 条 xor）；
//     ②删 `uVar1/tracer` 具名局部（P0/P52：ORIG packetId 升格 %ebx 跨
//     CPacketTracerInstance call 保活 ⇒ 单语句链式实参）；③删 `char cVar2` 具名
//     局部、比较字面量改 int 形（ORIG 逐次重读 `*data`，P0/P86）。残余=4.4 把
//     `char` 提升比较折成 `cmpb` 而 ORIG `movzbl+movsbl+cmp`（T5：类型非排序旋钮）
//     + 槽位/迭代器族。已读报告：UdpThread/_processUDPData.md。
// 判 N（本轮新证据，勿再耗轮次）：
//  · N18a「两段装载」族（pc_room::ProcessEquipItem/ProcessInsertItem、
//    CheckupAllRentalItems×2：ORIG `mov mem,%eax; mov %eax,%esi/%ebx`，4.4 恒
//    折单段）。**新旋钮证据**：探针 /tmp/l7/probe5.cpp tG —— 局部/字段声明
//    `unsigned int` 且消费方形参 `int` 时 4.4 即产 ORIG 两段形；但 T2/T5 明文
//    「纯 load/store 宽度不能定 int/unsigned」「禁为装载序改类型、无直证直接记 N」
//    ⇒ 本泳道**不改共享头/局部类型**，登记旋钮供 Main 裁决（涉及
//    ServerParameterScript.h EventRewardItemInfo::m_field0）。
//  · N121（Secu_DataControl::UpdateSecurityServiceState ndiff=1）：我方多
//    `jmp <epilogue>; nop` 尾岛；ORIG 0x08270777 自 PacketGuard D1 call **直落**
//    epilogue。否定早返/共享尾标签/显式 return 三形不消（P285 反向）⇒ 封顶。
//  · N125（CmmChannelProxy::RecvPacket 清零循环 `pBits[i]=0`）：ORIG
//    0x0810649c `movl $0,(%eax,%edx,4)` scaled 直存 vs 4.4 恒 `shl+add`。
//  · CSimpleSSO::init：ORIG 0x08602826 `mov -0xc(%ebp),%ecx` 提前装载 +
//    `add %ecx,%eax` 寄存器形；4.4 恒折 `add -0xc(%ebp),%eax`（N18a 反向）；
//    二维视图/cast 两形探针（/tmp/l7/probe2.cpp）均反劣。
//  · FT_* 10 件 + _Z41__static_initialization_and_destruction_0ii + __tcf_0
//    + STL 模板 3 件（N130 C++11 右值重载族）：结构性豁免（第三方占位/N123/
//    libstdc++ 代际），不计真缺口。
//  · 旗标探针（P230 代数）：本 TU 现路由 = cc1plus 4.4.6 / -O0 / **无**
//    -march=i486（bash -x 实证）；我方含 30 条 `cmov`（MIN_EXPR 展开），
//    ORIG 对应位点为 `setbe/setg/setne` 分支形（P230 签名）。**实测**加
//    `-march=i486 -mtune=generic` 后 cmov 30→0 但 TU 四数**完全不变**
//    （899/1787/11/42）⇒ 该族函数的残差不止 cmov，改路由非本 TU 的解；
//    路由变更属 Main 域，本泳道不动 dnf_game_cxx.sh。
//  · [Main「视图 cast 反模式」修法 · 2026-09-10 · 跨 TU 批准] 本次共修 3 件
//    形差异（诊断源 = 分类器 /tmp/l7/classify_gd2.py + 逐条 asm 复核）：
//    ① `ItemVendingMachine::ProcessBuyRequestEx` 子项/选项写入 5 处：去掉
//       `*reinterpret_cast<T*>(&m_subEntries[k][0])=`（4.4 折 add+shl 普通 store）
//       → SigTypes_E.h 匿名 union 具名视角 `m_subItems[k].m_itemIdx/.m_value`、
//       `m_optItems[k].m_itemUid/.m_a/.m_b`（旧 2D 字节视图保留，消费点零破坏）。
//       实测 scaled 直存全部现形：`mov %edx,0x8(%eax,%ecx,8)` /
//       `mov %dl,0xc(%eax,%ecx,8)` / `mov %edx,0x4(...)` / `mov %dx,0x8(...)` /
//       `mov %dx,0xa(...)` = ORIG 0x08177edf/0x08177f0d/0x08177f87/0x08177fb4/
//       0x08177fe3 五处逐条同形（探针 /tmp/l7/probe7.cpp f1 vs f2）。
//       注：L3901-3902 的 `*(unsigned long*)&…[0]` **保留**——其 `unsigned long&`
//       类型是为选中 `make_pair<ulong&,uchar&>` 实例（P61/P135），非反模式。
//    ② `ItemVendingMachine::ProcessIPG_ResultOutput` bonus-rate 段：改单条嵌套
//       实参链 + 链置比较符左侧 ⇒ 我方 933..939 与 ORIG 894..900 逐条同形
//       （`cmp %eax,%ebx; setge`，insns 1179→1178，ORIG 1158）。
//    ③ InterDispatcher.cpp `Inter_ItemLimitEdition_Reply::dispatch_sig`（Main 批
//       准跨 TU）：目的侧同法迁移 ⇒ 752→745 insns（ORIG 711）、ndiff 705→676；
//       该 TU 四数 135/1352/5/129 迁移前后逐位相同（A/B 对照实证）。
//  · GlobalData::Init 全函数**唯一**助记符差 = 1 条（ORIG i2751 `jmp` vs 我方
//    `je`），且**帧与槽集合侧全等**（两侧 `sub $0x79c`、81 个 ebp 槽、最深
//    0x77c 一致）⇒ 该件 1478 行 ndiff 系 N127 对齐坍缩伪差，非真缺口。
//    该 1 条 = **4.1 保留不可达 else 的折常真分支**：ORIG 0x0829cd71 段
//    `call *%ebx; test %eax,%eax; setne %al; test %al,%al; jmp 0x829cdc0`
//    （无条件跳转越过 0x0829cd7a..0x0829cdbb 的 cMyTrace+`return 0` 死块），
//    4.4 同源形恒产 `je <then>`（死块被删/改为可达）。探针 /tmp/l7/probe8.cpp
//    8 形（`ok||true` / `ok&&true` / `f()||true` / `ok|1` / while+break /
//    `ok?true:true` / 逗号 `(ok,true)`）**均不能复现**「保留死 else + 无条件
//    jmp」⇒ 判 N（跨代 codegen；若该虚调用返回值恒非 0 则两侧可观察行为一致，
//    照抄 ORIG 需删整个失败分支，属改写语义，不做）。
//  · [只读分类 · 编译冻结窗口产出] 全 55 件非 identical 按「帧/槽集合/助记符
//    多重集」三轴分类（脚本口径：objdump 双侧 × nm 区间，无编译参与）：
//      A. **仅寄存器/顺序差**（d=0 且 slotΔ=0）：Connect / CheckInventoryEmptyCount
//         （compare_tu 口径已 ndiff=0）/ CRandomOptionItemHandle C1 / St3map::ix /
//         St4pair D1 ⇒ 前两件已闭合，余 3 件 N7/S 族。
//      B. **仅槽号差**（d=0, slotΔ>0）：initTournamentDB(14) / CDBEnviroment::parse(8)
//         / DispatchThread::dispatch(4) ⇒ N82 族（声明序/首用序）。
//      C. **近战**（d≤4 且 |Δinsn|≤2，共 14 件，下一窗口优先队列）：
//         Init(d=3) / QueryConnInfo(1) / CServerProxyMgr D1×3(1) / proc_addFatigue(1)
//         / InitTcp(2) / CheckupAllRentalItems×2(1,2) / PC·Dim log×2(1,帧 0x4c vs 0x3c)
//         / ProcessEquipItem·ProcessInsertItem(1, N18a tG 旋钮待 Main 裁决) /
//         CSimpleSSO::init(2) / Secu(3, N121 尾岛) / __tcf_0(3, 结构性)。
//      D. **真缺口候选**（助记符差 ≥5 且指令数差 ≥3）：MonitorTcpThread(1575/1836,
//         cmov 28× + EH 尾岛) / ProcessBuyRequestEx(1599/1606) / ProcessIPG_ResultOutput
//         (1158/1178) / UdpThread::dispatch(137/149) / _processUDPData(266/274) /
//         AcceptThread(518/522) / BillingRecord(324/327) / init_function_type(157/169) /
//         checkAllRentalItemScript(282/293) / checkSelectionListScript(252/262) /
//         GetRentItemInfo(111/117) / DeleteAllRentalItem(119/124) /
//         CHumanCertifier::init(64/76) / RecvPacket(171/176) / SendCheckChannelScript
//         (289/291) / generateImage(685/682) / STL 3 件（N130）。
//  · [C 类实测记录 · 2026-09-10] ① `log`×2：P248补四「register 声明序旋钮」实测
//    **反劣**（pData 提前 ⇒ 角色未转正，两件 NEAR→DIFF：TU near 11→9、diff 38→40）
//    ⇒ 已回滚（回滚后 899/1791/11/38 逐位复原），判词改为「4.4 callee-saved 归属
//    在多 register 局部场景**不随声明序轮转**」；其帧差 0x4c vs 0x3c（缺 0x10）与
//    槽位差并存，属 N7+N82 双族，勿再试。
//    ② `proc_addFatigue`：残差 = 具名 `lower`@-0x14 与 `std::max` 匿名常量临时
//    @-0x1c 的深浅互换（ORIG 临时更深）。该对跨「具名标量区 / 匿名临时区」两区
//    ⇒ P212 明示临时区不可翻（前批三形实测已反劣）⇒ 维持 N98/N212 封顶。
// ============================================================================
// ============================================================================
// [b28d-solo-GlobalData 泳道判词] 清单 /tmp/b28d_tu/GlobalData.tsv（44 件）。
// 开工基线 check_tu_routed：identical=832 ae=1637 near=9 diff=54；
// 收口：identical=832 ae=1639 near=8 diff=53（AE +2，无劣化）。
// 修复转 AE 两件（证据）：
//   ① CGM_Manager::LoadGmListFromDb 0x08299530 —— ORIG 三处 cMyTrace 各占独立
//      槽（-0x44/-0x34/-0x24，4.1 不复用），具名对象在 4.4 下复用同槽致全帧浅
//      0x30 ⇒ 改 P221 全表达式临时链 `cMyTrace(a,b,c)(fmt)` 后 AE。
//   ② ItemVendingMachine::_CheckEnableAvatarCondition 0x08176794 —— 首调
//      GetRelatedItem 无槽往返（P0 内联）、去 vec 具名指针（P183）、
//      `cmp -0x1c(%ebp),%al; jg` 8 位有符号比较（T1 直证 char）后 87=87 AE。
// 改善未闭合一件：
//   （原 PunishClient 残差已闭：[b28d-PoolGlobalData] R2 落地 CBusinessImpl.h
//     PunishClient 返回类型 void→int + `return 0`，0x080de3ee ndiff 19→0 AE，
//     TU diff 53→52。）
//   CheckInventoryEmptyCount 0x0827108c 103→96（P63 共享出口改嵌套正向 if；
//     残差=ORIG `xor $1;test;jne RET1` vs 4.4 恒 `je NEXT`，极性恒折 ⇒ N）。
// 其余件按 N 类封顶（逐函数头有证据判词）：N18b 比较方向（CServerProxyMgr D1 ×3，
// 模板体在 CServerProxyMgr.h，R1 禁改 .h）、N7 寄存器角色轮换、N82/N95 槽位分配、
// N80 cMyTrace varargs 求值序、N4/P30 add-vs-mem、
// N-数据缺失（CHumanCertifier 人证表内容不可还原）。
// ============================================================================
// [SweepCAutoPunish 2026-09-10] dual_rank 13 件（gap' 降序）逐件复核：
// 收口 identical=894 ae=**1784** near=8 diff=**47** missing=846
// （基线 894/1772/15/54/849 ⇒ ae +12、near −7、diff −7、missing −3，零回退）。
// 真修 4 件（逐件证据见函数头）：①CaptchaGen::~CaptchaGen gap16→**0**（AE）——
// FT_Done_Face/FT_Done_FreeType 定义迁 StubS3A_deps.cpp（P210②，Main 授权），
// 复现 ORIG 0x080de706 EH landing；②NetworkThread::dispatch gap6→**0**（AE）——
// P0 具名局部 + P198① 预清 + P225 while 物化 + P47 极性；③SendCheckChannelScript
// −4→+2——三处 P295 while 形消 sete 差异；④generateImage 680→**682**——P321 T1
// int 局部 + 删 cast + FT_Bitmap::width 类型 + P243 首项序 + P223 双岛。
// 其余 9 件维持 N（parse/InitTcp/DispatchThread/QueryConnInfo/CSimpleSSO/
// CheckupAllRentalItems×2/pc_room×2：gap 1~6，无 jcc/调用差，p167 CLEAR 仅
// C1/C2·D1/D2 同址别名，归 N7/N18a/N78/N82/N126）。
// ============================================================================
// [b28d-PoolGlobalData 二压 2026-09-09] 三数 基线 892/1780/8/53 →
// 收口 identical=893 ae=1781 near=8 diff=52（+1/+1/0/−1，missing_in_game
// 849→847，无劣化）。战果（逐件证据见函数头）：
//   ① CBusinessImpl::PunishClient 0x080de3ee ndiff 19→0（R2：
//      CBusinessImpl.h:34/48 void→int + return 0；ORIG 0x080de4b8/4c8/4d0
//      单出口返 0 实证，全仓无消费点）。
//   ② RentalInfo 伪 user 析构删除（InstanceRentalSystem.h，R2 已批）：
//      _Destroy_aux<ILb1E>@0823e5c7 实证 trivial ⇒ ProcessInsertItem
//      0x08271666 ndiff 133→3、ProcessEquipItem 0x082714c5 139→3（EXTRA
//      D1Ev×2+_Unwind_Resume×1 归零；D1/D2 W size 5 与 ORIG 同寸保留）；
//      同批语义修正 ri.m_field8/9/B=inElem[0]/[1]/[3]（旧 reward>>8/24 误推）。
//   ③ Dimension::GetRentItemInfo 0x0826f842 ndiff 96→2（sz 具名落槽+下标
//      重读+goto「ret 标签居中」形；残 2 = 4.4 块启发 `jmp` 中继，探针
//      pol_probe4/5 四形穷举后 N7 封顶）；else 支实参=常量 2、无 list=0 死
//      初始化（0x0826f8f2/0x0826f8ba 直证）。
//   P326（N93 双迭代器翻案）自查：proc_addFatigue 匿名 0 临时形实测反劣
//   （4→8 条）已回退，N93/N98 判不变；本池其余件无「双具名 const_iterator
//   比较」残差形（CBlackIPMonitor 三件已 AE）。
//   小件复核维持 N：D1×3（N18b 实测交换无效）、CRandomOptionItemHandle C1
//   （纯寄存器角色）、2×log（帧 0x4c vs 0x3c 无源形+esi/ebx/edi 轮换）、
//   CheckInventoryEmptyCount（xor$1;jne 极性，probe f1–f9 复核 4.4 无
//   「无往返+直分支」形）、CSimpleSSO::init（ndiff 已 3，N18a add 折叠）、
//   CheckupAllRentalItems ×2（各 3 条，N18a regmove 融合）、CaptchaGen D1
//   （N-EH 判词已于 2026-09-10 被 SweepCAutoPunish 推翻并修复：FT 占位定义迁
//    StubS3A_deps.cpp 后 AE 49=49）、InitTcp（N80/N82）；CHumanCertifier::init
//   N-数据缺失；Init/Fini/dispatch 族与 IPG/ vending 超大件维持原判词。
// ============================================================================
// ============================================================================
// [SweepGD 2026-09-10 round5 交付小结] 残余 33 件逐件处置（gap = 净指令数，去 nop 口径）
//  本波真修 2 件（合计 −255 条）+ 微修 1 件；其余 30 件逐件复核后维持/重判 N：
//  ① MonitorTcpThread::dispatch（ORIG 0x0857d95a）**gap 256→17**（our 1830→1592）四修：
//     a. FD_ZERO 手写旧 glibc 逐元素循环（ORIG +0x0f5/+0x123 `lea; mov %eax,-0x50;
//        movl $0,-0x54; …; movl $0,(%eax,%edx,4); cmpl $0x1f,-0x54; setbe`）——
//        用 `fd_set* fds + __FDS_BITS(fds)[fi]` 具名数组成员视角产 scaled 存
//        （旧 `int* fdBits[fi]` 恒折 shl+add，多 2 条/处）；
//     b. FD_SET/FD_ISSET 实参 (unsigned) cast ×26：ORIG `shr $0x5`+`and $0x1f` 无符号形，
//        我方 int 实参产 cmovs 有符号除链（sar$0x1f/shr$0x1b/add/sar$0x5 + %32 五条）；
//     c. 外层循环改 `while (!GlobalData::s_server_stop)`（ORIG +0x1a32 尾条件块
//        `movzbl; xor $0x1; test; jne body` + 函数头 jmp 直达）——原
//        `while(true){if(s_server_stop)return;}` 的函数头 4 个 jmp+nop 中继岛全消；
//     d. 去 7 条多余 `movl $0` 清零（ORIG 函数头仅 5 条：soError+lastReconnect×4；
//        裸声明 -O0 零指令，P328⑤）。
//     残余 = N126/N95 帧槽整体浅 8（fd_set@-0x180 vs ORIG -0x188，sub 同 0x21c）
//        + 4 处 setcc-fold 比较折叠（`cmp $0xffffffff; setne; test; je` vs 我方 `cmp; je`）
//        + 1 处 N121 尾岛。
//  ② CmmChannelProxy::RecvPacket（ORIG 0x0810645c）gap 4→3（净 175→174）：
//     FD_ZERO 循环改 `fd_set* fds = &readfds; __FDS_BITS(fds)[i] = 0;` ⇒ 复现 ORIG
//     0x08106496 段 `mov -0x20,%edx; mov -0x1c,%eax; movl $0,(%eax,%edx,4)` scaled 直存
//     （**旧判词 N125 作废**；`unsigned long* pBits` 版恒折 shl+add）。
//     残余 = FD_SET 单份 scratch（ORIG `shr $5`→%eax 直用+`mov %eax,%edx` 复制；具名 ix
//     174 vs 内联 176，取前者）记 N18a/N96。
//  ③ 其余 31 件（本轮逐件复核，ORIG/OURS 净条数）：
//     · gap 0（槽位/寄存器族 N82/N7/N18a/N18b/N93/N95/N126）：initTournamentDB 220/220 /
//       Init 3139/3139（N89 libstdc++ 全内联） / QueryConnInfo 169/169 / CDBEnviroment::parse
//       289/289 / NetworkThread::dispatch 204/204 / CRandomOptionItemHandle C1 69/69 /
//       DispatchThread::dispatch 325/325 / CServerProxyMgr D1×3 67/67（模板体在 .h） /
//       proc_addFatigue 73/73（N98/N212） / loadDbData 243/243 / RentalMachinePcRoom::log 58/58 /
//       RentalMachineDimension::log 58/58。
//     · gap 1–6：SendCheckChannelScript 289/290 / Secu_DataControl 150/151（N121 尾岛） /
//       GetRentItemInfo 111/113（N7 块启发） / AcceptThread 516/519（N82） /
//       BillingRecord 324/327（N1/N75） / RecvPacket 171/174 / checkAllRentalItemScript
//       282/286 / checkSelectionListScript 252/256 / DeleteAllRentalItem 118/123 /
//       ProcessBuyRequestEx 1598/1604（p167 仅 C1/C2·D1/D2 同址别名 P93，无真缺口） /
//       InitTcp 73/71 / _processUDPData 263/261 / CSimpleSSO::init 45/44 /
//       UdpThread::dispatch 136/135 / generateImage 682/679。
//     · gap ≥10：CHumanCertifier::init 64/74（ORIG 人证表 .text 运行时填充 0x0814af00+2i，
//         静态不可恢复 = 数据缺失 N） / StackableAction::Action::init_function_type 157/169
//         （boost::bind 展开 + arg<1> placeholder 构造跨版本差异 N） /
//       ProcessIPG_ResultOutput 1158/1177（p167 EXTRA 全为多发局部对象 ctor/dtor
//         `vector<Inven_Item>`×4/`PacketGuard`×4 + static guard EH `__cxa_guard_abort`×2/
//         `_Unwind_Resume`×2 ⇒ N82/N91 族）。
//  工具实测：`check_tu_routed.sh` **COMPILE OK**，四数 identical=899 ae=1791 near=11 diff=38
//  （missing_in_game=847）**零回退**；p167 除 C1/C2·D1/D2 同址别名（P93）与 N130 右值重载外
//  **零 MISSING**。
// ============================================================================
// [SweepGD 2026-09-10 round5b · C05 家族（①尾 fail 块 / ②int 局部比较 / ③多守卫合并）复查：
//  逐件核对 33 件，**命中并落地 1 件**，其余逐条给不适用理由（避免重复烧轮次）：
//  ✔ **pc_room::DeleteAllRentalItemInSameSectionForNewRent**（ORIG 0x08271807）gap **+5→−1**
//    （net 123→117，ORIG 118）：三修全部 ORIG 直证 —— ①删 `const int itemId` 具名槽
//    （0x08271874 `mov 0x4(%eax),%eax; mov %eax,%ebx` 值走 %ebx 无槽）→ 使用点内联重读；
//    ②删 `CInventory* invR` 具名槽（0x08271879..99 getCurCharacInvenR 结果直传、无 -0xc 槽）→
//    内联到调用表达式（P183）；③entry 写序改 ORIG `slot→bEquip→itemId`（0x08271902/909/910）
//    + m_bEquip 直存 `= isEquip`（movzbl+mov，P136）。残 −1 = ORIG 双条收尾
//    `lea -0x8(%ebp),%esp; add $0x0,%esp` vs 4.4 单条 add（N115/N126 结构性）。
//  ✗ 不适用清单（逐条理由）：
//    · 尾 fail 块形（C05①）：本 TU 失败出口多为 `xor $1; test; jne <函数尾>` 且与 4.4 产物
//      已同形（DeleteAllRentalItem/Secu 均已核），其余件（checkAll/checkSelection/GetRentItemInfo）
//      残差为槽分布（N82）非岛，改包裹无收益；
//    · int 局部比较（C05②）：本 TU 的 `cmp mem` 族（CServerProxyMgr D1×3）经 §6.2 判定为
//      **模板体在 CServerProxyMgr.h**（改头需跨泳道广播且 gap 已 0），且 Main 已确认该范式仅
//      对「**两个具名局部**比较」命中（常量/形参/跨调用返回值三类均负例）；
//    · 多守卫同目标合并（C05③/D08）：本 TU 无「多守卫 + 各自 jmp+nop 岛」件
//      （Secu 为单守卫、MonitorTcpThread 的岛已由 while 条件重构消除）；
//    · 本方全 TU 已按 A28 口径核过：我方多岛处（MonitorTcpThread 尾 1 处、RecvPacket 0 处）
//      实测无源码形可消，按 N121 封顶。
// ============================================================================

#include "GlobalData.h"
#include "PCRoomServerProxy.h"   // CPCRoomServerProxy 权威（原 TU 局部定义收敛，Fix14_I）
#include "Action.h"  // StackableAction::Action 定义（GlobalData.h 不再复制）
#include "OnlinePreliminary.h"   // online_preliminary 域权威声明（本 TU 持有 s_onlinePreliminaryTeamMgr 构造）
#include "RestrictGeolocation.h"   // RestrictGeolocation 权威（原局部镜像删除，实现迁 RestrictGeolocation.cpp）
#include "CDailyScheduleManager.h"  // _DailySchedule / CDailyScheduleManager 权威（原本地镜像删除，ODR 收敛）
#include "CCapchaDataManager.h"  // WongWork::CCapchaDataManager / MSG_UDP_CAPCHA_CERTIFY 权威（显式，防间接 include 漂移）
#include "Tick.h"                // Tick::nowAsInternalValue（CaptchaGen 播种源，对齐 ORIG）
#include "GameMasterCmd.h"        // game_master::CGameMasterMgr（占位骨架已删，实现在 GmCore/GmRegister）
#include "CItemList.h"             // get_rand_int
#include "CCeraShopBonusItemEvent.h"   // 权威声明（P104 ODR 收敛，2026-08-29 EventManagerWave）
#include "OS_API.h"                    // OS_API::snprintf（ORIG _ZN6OS_API8snprintfEPcjPKcz）

#include <iostream>
#include <new>
#include <stdio.h>
#include <memory>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <list>
#include <string>
#include <string>
#include <deque>
#include <math.h>
#include <dlfcn.h>
#include <set>
#include <vector>
#include <boost/function.hpp>
#include <boost/bind.hpp>

#include "TaiwanInternal.h"
#include "MoreTypes.h"
#include "CStreamGuard.h"
#include "CStatisticServerProxy.h"   // CServerProxyMgr<CStatisticServerProxy> 完整类型
#include "CGM_Manager.h"             // CGM_Manager 真实布局（ORIG 0x3c）
#include "SpecialItemHandler.h"      // ORIG 全局类 SpecialItemHandler 权威声明
#include "CSyncSlangFilter.h"
#include "AradServerStateMessage.h"  // ARAD::Arad_ServerStateManager 成员类型
#include "Arad_ServerStateManager.h"  // ARAD::Arad_ServerStateManager 权威定义（原局部镜像已删除）
#include "Arad_DataManager.h"  // ARAD::Singleton 唯一权威定义
#include "CInventory.h"              // CSecu_ProtectionField 权威定义（CInventory.h）
#include "BaseServerProxy.h"         // ENUM_SOCKET_STATE（CPCRoomServerProxy socketState）
#include "CMyFileLog.h"              // 租赁 log（RandomRentalA2 批次）
#include "CUser.h"    // StackableAction proc_* 需要 CUser 完整类型
#include "CSimpleSSO.h"  // WongWork::CSimpleSSO 权威定义（原局部类已收敛）
#include "CItem.h"    // StackableAction proc_* 需要 CItem 完整类型
#include "CCeraShop.h"        // WongWork::CCeraShop 权威声明
#include "SigTypes.h"         // MSG_BUY_CERASHOP_ITEM（BuyItem/ProcessBuyRequestEx 形参 const 引用）
#include "InstanceRentalSystem.h"  // InstanceRentalSystem 权威定义（§7 合并）
#include "CGlobalEffectManager.h" // CGlobalEffectManager 权威声明（本地占位已移除）
#include "CSystemTime.h"      // CSystemTime::getCurSec（IPG 请求 curSec）
#include "RefPvpGrade.h"      // RefPvpGrade 权威定义（ORIG 布局 0x14c）
#include "Teleport.h"        // Teleport/NPC_Teleport/User_Teleport 权威声明
#include "CBusinessImpl.h"   // IBusiness/CBusinessImpl 权威声明
#include "CHackLog_InvalidAreaMove.h"  // checkSeriaRoom（User_Teleport::check_rule）
#include "CFatigueBatteryHandle.h"  // CFatigueBatteryHandle 权威定义（ORIG ctor 0x84a7ff8 空体）
#include "sync_script.h"     // sync_script::CSyncScript 权威定义（GetInstanceSyncScript/SetEnable/IsEnable）
#include "TimerQueue.h"      // G_TimerQueue / TimerEntry（RegistNextTimer）
#include "CBlackIPMonitor.h"  // WongWork::CBlackIPMonitor 权威定义（ORIG ctor 0x82a6526）
#include "CTimeGate.h"        // CTimeGate : village_object::IObject 权威定义（ORIG ctor 0x8284512）
#include "CVillageObjectMgr.h" // village_object::{IObject,CVillageObjectMgr} 权威定义（本地镜像已移除）
#include "TaiwanCash.h"  // WongWork::IPG::{IIPGHelperInterface,CIPGMgr} 权威定义（局部 mirror 已删除）
#include "CountryCode.h"   // CountryCode 权威定义（原 TU-local struct 已删除，实现迁 CountryCode.cpp）

#include "CCommunityServerProxy.h"
#include "CSchoolServerProxy.h"

// StaticPool/expert_job::CEnchanter/CDisjointer 权威声明已迁 CExpertJob.h
//（含 CExpertJobFactory/CExpertJobMgr），本 TU 经 include 获取。
#include "CExpertJob.h"

#include "CEnvironment.h"
#include "GameTypes.h"
// PacketGuard 完整类型经 CInventory.h（权威定义点）；此处不再重复 include。
#include "CRijndael.h"
// expert_extraction 域权威声明（原局部类定义已删，ODR 收敛；实现 ExpertExtraction.cpp）
#include "CExtraction.h"
#include "CAlchemistExtraction.h"
#include "CEnchanterExtraction.h"
#include "CDollControllerExtraction.h"
#include "CExtractionMgr.h"
#include "PvPTypes.h"      // CMTRand 完整类型（CExtraction::m_rand）
namespace DNFFLib { int Binary2Hex(unsigned char const*, int, char*); }   // _ZN7DNFFLib10Binary2HexEPKhiPc，定义见 DNFFLib 权威 TU
#include "Cipher.h"
#include "BaseServerProxy.h"
#include "CHadesServerProxy.h"
#include "CaptchaGenData.h"
#include "CQueryCounter.h"
#include "ThreadInterface.h"
#include "DBMgr.h"
#include "MsgQueueMgr.h"
#include "ServiceRestrictManager.h"  // 权威定义（本地类已迁移）
#include "BingoEvent.h"
#include "CMonitorServerProxy.h"
#include "W6_HackPunishDeps.h"   // WongWork::CAutoPunishRule* 权威完整类定义
#include "CLogGameChannel.h"   // WongWork::CLogGameChannel 完整定义（s_logGameChannel 初始化）
#include "CServerProxyMgr.h"
#include "CItemAmplifier.h"
#include "CPowerManager.h"   // 权威 CPowerManager 类（替代下方本地 stub，解除 ODR）
#include "CItemLimitEdition.h"       // ProcessIPG_ResultOutput case 10（限购展示位）
#include "Arad_IPGProcess.h"         // static IPGProcess（ORIG W ctor 0x0817a2ba）
#include "Arad_MileageProcess.h"     // static mMileageProcess（ORIG W ctor 0x0817a2ce）
#include "HeroMissionEvent.h"        // setMissionValue（case 7 任务计数）
#include "PacketTracer.h"            // CPacketTracer::AddUdpLog / CPacketTracerInstance（UdpThread::_processUDPData）
#include "EncryptTool.h"             // GlobalInstance<EncryptTool> 显式实例化（ORIG W 0x848e1d3/0x848e1e5）
#include "InterDispatcher.h"         // InterDispatcher 权威类（GlobalInstance<InterDispatcher> 实例化）
#include "TimerDispatcher.h"         // TimerDispatcher 权威类（GlobalInstance<TimerDispatcher> 实例化）
#include "PacketDispatcher.h"       // PacketDispatcher / G_PacketDispatcher / pUserPacketCounter_
#include "GameServerDemonManager.h" // GameserverDemonManagerInstance / IsProhibitConnectState
#include "GameWorld.h"               // GameWorld 权威类（GlobalInstance<GameWorld> 实例化）
#include "CDataManager.h"            // CDataManager 权威类（GlobalInstance<CDataManager> 实例化）
#include "AuctionProxy.h"           // CAuctionServerProxy/CCeraAuctionServerProxy 权威类（MonitorTcpThread::dispatch）
#include "CGameManager.h"           // CGameManager/StaticPool<CUser,600> 完整类型（AcceptThread::dispatch）
#include "PacketGuard.h"            // PacketGuard 完整类型（AcceptThread::dispatch 栈对象）
#include "EpollHandler.h"          // EpollHandler 完整类型（NetworkThread::dispatch）
#include "W8LT_DBTask.h"          // DBTask 完整类型（DBThread::dispatch）
#include "MessageCounter.h"       // message_counter::{CTimeInterval,CMessageCounter}（DB/DispatchThread）
#include "UserPacketCounter.h"    // message_counter::CUserPacketCounter（DispatchThread::dispatch）
#include "OS_API.h"               // OS_API::{Exit,Sleep,uSleep}（线程 dispatch）
#include "PacketHeader.h"         // 权威 10 字节线格式头（UdpThread::_processUDPData packetId 视图）
#include "RandomOptionScript.h"   // RandomOptionScript::m_providerNormal148(+0x148) 权威成员（CRandomOptionItemHandle ctor）
#include "XorRand4.h"             // XorRand4 权威声明（定义 XorRand.cpp；本 TU 原重复 inline 副本已删）
#include "RandGen.h"              // RandGen::getFloat(XorRand4*,float,float)（ORIG 0x08ae02a4）

// ============================================================================
// 依赖类最小 stub（符号/布局属后续批次；本 TU 只用于还原 GlobalData 函数）
// ============================================================================

typedef std::map<ENUM_DB_HANDLE_IDX, ENUM_SERVER_GROUP> DBGroupMap;
typedef std::map<ENUM_DB_HANDLE_IDX, STDBConnInfo> DBInfoMap;

// ARAD::Arad_ServerStateManager 局部镜像已删除：权威定义在 Arad_ServerStateManager.h。

class PacketPool;

// ORIG W 0x0857f5d6（_Z13G_EpollHandlerv）：GlobalInstance<EpollHandler>::inst_ptr()。
// 与 CUser.cpp 同形态的本地声明（EpollHandler.h 权威头未收录，避免共享头改动）。
EpollHandler* G_EpollHandler();

// ORIG W：G_TimerDispatcher 0x0847e8a4 / G_InterDispatcher 0x0847e8b7；
// GlobalInstance 单例访问器，定义见本文件尾部（同 G_TimerQueue 形态）。
class TimerDispatcher;
class InterDispatcher;
TimerDispatcher* G_TimerDispatcher();
InterDispatcher* G_InterDispatcher();

class DBThread : public ThreadInterface
{
public:
    DBThread();
    void setLogDB(bool isLogDB);
    bool isLogDB();  // ORIG T 0x083ff77a：读 +0x0c
    virtual void dispatch(void* arg);          // ORIG T 0x083ff786（线程体：DB 事件循环）
private:
    // ORIG 布局（objdump 0x083ff5f2）：ThreadInterface 基类 0x00..0x0b
    // （vptr +0 / m_tid +4 / m_tidCopy +8）；+0x0c bool m_bLogDB；
    // +0x10 CQueryCounter（sizeof 0x3c → 总 0x4c）。
    bool m_bLogDB;                 // +0x0c
    CQueryCounter m_queryCounter;  // +0x10（sizeof 0x3c → 总 0x4c）
};

class DispatchThread : public ThreadInterface
{
public:
    DispatchThread();
    virtual void dispatch(void* arg);          // ORIG T 0x0847e2d2（线程体：主分发循环）
};

class NetworkThread : public ThreadInterface
{
public:
    NetworkThread();
    virtual void dispatch(void* arg);          // ORIG T 0x0857d5be（线程体：epoll 收包）
};

class UdpThread : public ThreadInterface
{
public:
    UdpThread();
    virtual void dispatch(void* arg);          // ORIG 0x0857c9de（线程体：bind + select/recv 循环）
private:
    int _bindUDPSocket(unsigned short port);   // ORIG 0x0857c3be
    void _processUDPData(char* data, int len); // ORIG 0x0857c5a4
};

class MonitorTcpThread : public ThreadInterface
{
public:
    MonitorTcpThread();
    virtual void dispatch(void* arg);          // ORIG T 0x0857d95a（线程体：代理 select 轮询）
};

template <class T> class CServerProxyMgr;
class AcceptThread : public ThreadInterface
{
public:
    AcceptThread();
    virtual void dispatch(void* arg);          // ORIG T 0x0857cc5a（线程体：listen/accept 循环）
private:
    // ORIG ctor 0x0857cbe8：置 +0x0c 字节为 0 → 基类 0xc 之后一个 bool，总 0x10。
    bool m_acceptFlag;  // +0x0c
};

class CDoubleConnCheckServerProxy
{
public:
    CDoubleConnCheckServerProxy(std::string ip, int port);
    bool Init();
private:
    char m_pad[0x10];
};



namespace secretshop
{
class CSecretShop
{
public:
    CSecretShop();
private:
    char m_pad[0x64];
};
}

class CmmChannelProxy
{
public:
    CmmChannelProxy(std::string ip, int port);
    bool Init();
    bool SendChannelInfo();   // _ZN15CmmChannelProxy15SendChannelInfoEv 0x0810619e
    bool ConnectBridgeServer();               // _ZN15CmmChannelProxy19ConnectBridgeServerEv 0x08106278
    int SendPacket(char* buf, int len);       // _ZN15CmmChannelProxy10SendPacketEPci 0x0810637c
    int RecvPacket();                         // _ZN15CmmChannelProxy10RecvPacketEv 0x0810645c
    bool SendCheckChannelScript();            // _ZN15CmmChannelProxy22SendCheckChannelScriptEv 0x081066ea
private:
    // ORIG 布局：+0x00 string、+0x04 int、+0x10 CUdpHandler、+0x1c 字段、
    // +0x20 buf[0x80000]（sizeof 0x80020）。
    std::string m_ip;       // +0x00
    int m_port;             // +0x04
    char m_pad8[8];         // +0x08
    CUdpHandler m_udp;      // +0x10
    int m_sock;             // +0x18 桥接 TCP fd（ORIG ConnectBridgeServer
                            // 0x08106287 `mov %eax,0x18(%edx)` 直写实证）
    int m_field1c;          // +0x1c
    char m_buf[0x80000];    // +0x20
};

// SendCheckChannelScript 依赖的系统头/外部符号（ORIG T：OS_API::Sleep
// 0x0858c4e0、DNFFLib::Make_Dir 0x08108ad7，定义见 DomainMiscDeps.cpp）。
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
namespace OS_API { void Sleep(int ms); }
namespace DNFFLib { void Make_Dir(char* dir); }
// send_check_channel_script 的函数作用域静态（ORIG b 符号 @0x094051c0/0x094055c0）
static char s_version[0x400];
static int s_linePos;

// 布局：char category@+0、char packetID@+1、unsigned int size@+2（setSize/
// getSize 均 dword 访问实证；setSize 0x08106bdb arg 先载 ⇒ ORIG 字段与 int
// 形参存在 signedness 差 ⇒ unsigned int，技巧5 反推）、负载自 +0x0b；
// packed 保证 +2 落位。
class tagPacketHeader
{
public:
    tagPacketHeader() {}
    void setCategory(int c) { m_category = (char)c; }   // 0x08106be6：mov %dl,(%eax)
    void setPacketID(int id) { m_packetID = (char)id; } // 0x08106bbc：mov %dl,0x1(%eax)
    void setSize(int s) { m_size = s; }                 // 0x08106bd8：mov %edx,0x2(%eax)
    // ORIG mangled _ZN15tagPacketHeader7getSizeEv（非 const，0x08106bcc：
    // mov 0x2(%eax),%eax）；类外定义保证 O0 下调用点出真实 call，与 ORIG 同形。
    int getSize();
private:
    char m_category;   // +0x00
    char m_packetID;   // +0x01
public:
    unsigned int m_size;  // +0x02（ORIG 0x08106bdb：`mov 0xc(%ebp),%edx` 先于
                          // this 载入 ⇒ arg→member 有 int→unsigned 转换）
    char m_pad6[5];    // +0x06..+0x0a（头总长 0xb）
} __attribute__((packed));

// GI 移位（SURPLUS cand1，ORIG GI 地址序最小）：本函数原位于 5334 行区，纯移位
// 上送至本 TU 首个强实体 tagPacketHeader::getSize 之前，使 _GLOBAL__I_ 后缀对齐
// ORIG _GLOBAL__I__ZN13CBusinessImpl18SendBuffertoClientEiiijPhs。语义零变化。
#include "GlobalData/SendBuffertoClient.inc"
}

#include "GlobalData/getSize.inc"

struct tagCS_GET_SCRIPT   // ORIG ctor 0x08106c48：cat=2/id=9/size=0xb
{
    tagPacketHeader m_hdr;   // +0x00
    tagCS_GET_SCRIPT()
    {
        m_hdr.setCategory(2);
        m_hdr.setPacketID(9);
        m_hdr.setSize(0xb);
    }
} __attribute__((packed));   // sizeof 0xb（无负载）

struct tagCS_CHECK_SCRIPT_VERSION   // ORIG ctor 0x08106c94：cat=1/id=5/size=0x1b
{
    tagPacketHeader m_hdr;   // +0x00
    char m_version[0x10];    // +0x0b（SendCheckChannelScript 写版本串）
    tagCS_CHECK_SCRIPT_VERSION()
    {
        m_hdr.setCategory(1);
        m_hdr.setPacketID(5);
        m_hdr.setSize(0x1b);
    }
} __attribute__((packed));   // sizeof 0x1b

// ORIG ctor 0x08106bfc（SendChannelInfo 栈上构造）：cat=1/id=2/size=0x33。
// 负载字段与宽度按 ORIG 0x0810619e 反汇编栈槽写序列（CmmChannelProxy/SendChannelInfo.md）：
// gc_no←mov %eax,-0x30(%ebp)（int）、group←-0x2c、channel_no←-0x28、max_user_num←-0x24、
// cur_user_num←-0x20、server_ip←strcpy(lea -0x3b; add $0x1f)、port←mov %eax,-0xc(%ebp)。
struct tagCS_UPDATE_CHANNEL_INFO
{
    tagPacketHeader m_hdr;      // +0x00（payload 起于 +0x0b）
    int m_gcNo;                 // +0x0b
    int m_group;                // +0x0f
    int m_channelNo;            // +0x13
    int m_maxUserNum;           // +0x17
    int m_curUserNum;           // +0x1b
    char m_serverIP[16];        // +0x1f
    int m_port;                 // +0x2f
    tagCS_UPDATE_CHANNEL_INFO()
    {
        m_hdr.setCategory(1);
        m_hdr.setPacketID(2);
        m_hdr.setSize(0x33);
    }
} __attribute__((packed));      // sizeof 0x33（= ORIG setSize 值）

// ORIG 0x810619e：构造栈上 tagCS_UPDATE_CHANNEL_INFO（header cat=1/id=2/size=0x33）
// 并经 m_udp.SendToServer 发往频道服务器。
#include "GlobalData/SendChannelInfo.inc"
}

// ORIG ConnectBridgeServer 0x08106278：socket/connect 桥接服务器并设非阻塞。
// [b28d-18 IDENTICAL_AE] 修：addrlen 改 int + (socklen_t) cast 翻实参装载序（P248 档③新实证，addrlen→%ecx 先载）；NDIFF 3→1（余尾 nop） | ORIG 0x08106278
#include "GlobalData/ConnectBridgeServer.inc"
}

// ORIG SendPacket 0x0810637c：write 发送；EAGAIN/EINTR/0 → 0（可重试），
// 其它错误 cMyTrace 后返回 -1。
#include "GlobalData/SendPacket.inc"
}

// ORIG RecvPacket 0x0810645c：select(300s) → SO_ERROR 探测（结果忽略）→
// Sleep(100ms) → read 追加到 m_buf 尾部并累加 m_field1c。
// ORIG 0x0810645c 全量转写：
//   ret(-0x2c)=0；tv(0x12c,0)@-0x54；额外 dead init(-0x28)=0（0x0810647d）；
//   fd_set@-0xdc 经指针槽(-0x1c)+计数槽(-0x20)手循环清 32 词
//   （0x081064a7 `cmpl $0x1f; setbe` i<=31 形）；FD_SET 宏产无符号
//   `shr $5`+`and $0x1f`+`shl` 位图形（0x081064b8）；select 结果 `jg body`
//   岛（0x0810651c，源码 `if (ret > 0) {...} return 0;`）；getsockopt 局部
//   error@-0x58/len@-0x5c；read 结果槽 -0x24；负判定 `shr $0x1f; test`
//   （0x081065ab，等价 `(unsigned)nread >> 31`）；errno 链 ==EAGAIN||==EINTR||
//   ==EAGAIN||==0（0x081065bb-5e3 四次独立取址，11 出现两次为原码忠实转写）。
// [b28d-18 N82] 残差=帧槽/寄存器（N82） | ORIG 0x0810645c
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（18/18）；残差=N82 槽位/寄存器 + N89 STL 展开。
#include "GlobalData/RecvPacket.inc"
}

// ORIG SendCheckChannelScript 0x081066ea（0x453 字节）：桥接后校验/拉取
// channel_info.etc 并落盘。stat 失败且非 ENOENT → 0；version 文件读写两次；
// CHECK_SCRIPT_VERSION(cat1/id5) 应答尾包写入 version；GET_SCRIPT(cat2/id9)
// 应答尾包为脚本体（fwrite size-0xb 字节）。
// [SweepCAutoPunish 2026-09-10] 285→**291**（ORIG 289，余 +2）：三处 `if(fp==0)`
// 改 P295 while 形，复现 ORIG 0x081067e4/0x081069d7/0x08106ae8 的
// `cmpl $0,-0x20; sete %al; test %al,%al; je`（探针 /tmp/gdprobe/p3.cpp f2/f3；
// if 形恒折 `jne`）。余 +2 = fgets 行剥离循环的 continue 岛布局（ORIG
// `jne <body>` 回边 vs 我方 `jne; jmp; nop`，P223 边界）+ 3 条装载序，
// 无 jcc 语义差、p167 CLEAR（仅 C1/C2 别名）。
#include "GlobalData/SendCheckChannelScript.inc"
}
// SpecialItemHandler（ORIG 全局类，mangled _ZN18SpecialItemHandler…，非 WongWork 前缀；
// T 0x827bcec：new(0x74) 后构造；对象 0x74 字节）。
// 权威声明见 SpecialItemHandler.h（dispatch 表 ctor 0x0827bcec / execute 0x0827be0a），
// 实现见 CCeraShop_Special.cpp。此处仅需其完整类型以支持 new/delete。

// CLogGameChannel 权威定义见 CLogGameChannel.h（§7 合并）。

// CAutoPunishRuleBlackIPMgr / CAutoPunishRuleHackTypeMgr / Hack*IPCounter_t /
// ctor/reset 成员函数定义）。

// WongWork::CSimpleSSO 权威定义已收敛到 CSimpleSSO.h（顶部 include）；
// 成员函数 ctor/dtor/init 在本 TU，其余方法在 WongWorkSSO_A.cpp。

// WongWork::CSecurityCardCenter 权威类定义已收敛到 GlobalData.h（W6_SecMiscDeps
// 批），ctor/init/其余方法定义在 W6_SecMiscDeps.cpp。

// CIPGHelper 保留本 TU（ORIG 方法集属 GlobalData 域）；基类 IIPGHelperInterface
// 与成员 CIPGMgr 用 TaiwanCash.h 权威定义（WongWork::IPG，include 见文件头部）。

namespace IPG
{

// [dedup] CIPGHelper 权威定义见 TaiwanCash.h（Wave6 IPG 16 槽接口重建）；
// 本 TU 仅保留 out-of-line 方法定义（Initialize/Finalize 等），成员布局以权威头为准。
}
// ItemVendingMachine 权威声明 + ARAD::PurcahseBonus / CeraShopPurcahseCountBonus
// 见 ItemVendingMachine.h；此处不再重复定义（避免 ODR）。
#include "ItemVendingMachine.h"

class Secu_DataControl
{
public:
    Secu_DataControl();
    void UpdateSecurityServiceState(CUser* user, unsigned char service,
                                    unsigned char flag, char* msg);  // ORIG T 0x08286e40
private:
    char m_pad[1];
};


#include "CLevelDungeonPlayStatistic.h"
#include "CDungeonExpStatistic.h"   // +0x18 子对象权威定义（map<int, stDungeonExp>）

class CStatisticMgr : public CLevelDungeonPlayStatistic
{
private:
    // ORIG ctor 0x82aa89e：CLevelDungeonPlayStatistic@+0（0x18，map<int,
    // stDungeonPlayInfo>）+ CDungeonExpStatistic@+0x18（0x18，map<int,
    // stDungeonExp>）。基类即 CLevelDungeonPlayStatistic（首基类同址）。
    // [b28d-BigGlobalInit] 无用户默认 ctor（ORIG nm：W _ZN13CStatisticMgrC1Ev
    // 0x45 为隐式合成）⇒ new (nothrow) CStatisticMgr() 走 value-init 前置清零
    // （P299/CWG434：用户 ctor 会抑制该清零，ORIG 0x829c5bc 有 rep stos 0xc dwords）。
    CDungeonExpStatistic m_dungeonExp;      // +0x18（权威定义见 CDungeonExpStatistic.h）
};

class CGM_Manager;  // 完整定义见 CGM_Manager.h（真实布局）

#include "BreakAwayPreventSystem.h"   // break_away_prevent 唯一权威声明点


// CExpertJobFactory/CExpertJobMgr 权威声明已迁 CExpertJob.h（含 StaticPool 双池），
// 本 TU 仅保留其成员函数定义（下方）。
// village_attacked::{Zone,CVillageMonsterArea,CVillageMonsterMgr} 权威定义见
// CVillageMonsterMgr.h（经 GameWorld.h→CUser.h→Area.h 链引入，原本地镜像已删除，
// ODR 收敛）；ctor/dtor 由 CVillageMonsterMgr.cpp 提供。

// RentalMachinePcRoomImpl（ORIG ctor 0x8271a80：set<unsigned>@0x104 构造 +
// memset 前 0x104；总 0x11c；D1 0x8271d50 仅析构 set）。嵌套于
// RentalMachinePcRoom（mangled _ZN19RentalMachinePcRoom23RentalMachinePcRoomImpl…，
// 与 ORIG 4 符号 C1/C2/D1/D2 一致）。
class RentalMachinePcRoom : public IRentalMachine
{
public:
    RentalMachinePcRoom();
    virtual ~RentalMachinePcRoom() {}

    // 槽2 Rent：ORIG T 0x082704e6（0x258 字节 pc_room 租赁事务链，本体待
    // pc_room 域批次 —— 报告 RentalMachinePcRoom/Rent.md）。声明占位保 vtable 序。
    virtual int Rent(CUser& user, const InstanceRentalSystem::RentInput& input,
                      InstanceRentalSystem::RentOutput& output);
    // 槽3 Ready：ORIG 0x0827073e = checkAllRentalItemScript() 尾调。
    virtual bool Ready();
    // 槽4 CheckRentableItem：ORIG 0x08270752 = m_impl->m_set.find(idx)!=end。
    virtual bool CheckRentableItem(unsigned int itemIdx);

private:
    struct RentalMachinePcRoomImpl
    {
        RentalMachinePcRoomImpl() { memset(m_data, 0, sizeof(m_data)); }
        ~RentalMachinePcRoomImpl() {}   // ORIG D1 @0x08271d50：仅析构 set
        char m_data[0x104];            // +0x00（日志路径等，未还原字段区）
        std::set<unsigned int> m_set;  // +0x104
    };  // 0x11c
    bool checkAllRentalItemScript();   // ORIG T 0x082708cc（本批实现）
    void initLoggingEnvironment();     // ORIG T 0x8270886
    int CheckupAllRentalItems(CUser& user,
                              const InstanceRentalSystem::RentInput& input); // T 0x08270d1e
    void log(CUser& user, int itemId);                                      // T 0x082707c0
    std::auto_ptr<RentalMachinePcRoomImpl> m_impl;  // +0x04（ORIG auto_ptr）
};
class RentalMachineDimension : public IRentalMachine
{
public:
    RentalMachineDimension();
    virtual ~RentalMachineDimension() {}

    // 槽2 Rent：ORIG T 0x0826ec22（与 PcRoom 同构，本体待 pc_room 域批次）。
    virtual int Rent(CUser& user, const InstanceRentalSystem::RentInput& input,
                      InstanceRentalSystem::RentOutput& output);
    // 槽3 Ready：ORIG 0x0826ee94 = checkAllRentalItemScript() 尾调。
    virtual bool Ready();
    // 槽4 CheckRentableItem：ORIG 0x0826eea8 = m_impl->m_set.find(idx)!=end。
    virtual bool CheckRentableItem(unsigned int itemIdx);

private:
    // [b28d-solo-GlobalData] Impl 必须**嵌套**（P76/P104）：ORIG nm 为
    // _ZN22RentalMachineDimension26RentalMachineDimensionImpl*（嵌套 mangling），
    // 全局平铺 struct 使 auto_ptr<Impl> 的 operator->/C1/D1 符号不匹配
    // （p167 MISSING ptEv×多件）。与 PcRoom 同构。
    struct RentalMachineDimensionImpl
    {
        RentalMachineDimensionImpl() { memset(m_data, 0, sizeof(m_data)); }
        ~RentalMachineDimensionImpl() {}   // ORIG D1 @0x0826fb0c：仅析构 set
        char m_data[0x104];            // +0x00
        std::set<unsigned int> m_set;  // +0x104
    };  // 0x11c
    bool checkAllRentalItemScript();   // ORIG T 0x0826f022（本批实现）
    bool checkSelectionListScript(
        const std::vector<std::vector<EventRewardItemInfo> >& list);         // T 0x0826f14e
    // 返回 bool：ORIG 调用处 0x0826f591 `xor $0x1,%eax; test %al,%al; je`
    // （P73/探针 v12：仅 bool 实参的 `!` 会折成 xor，int/char 为 test+sete）；
    // 本体返回点均为 `mov $0x0/$0x1,%eax`（0x0826f870/…）与 bool 一致。
    bool GetRentItemInfo(CUser& user,
                        const InstanceRentalSystem::RentInElem& elem,
                        EventRewardItemInfo& out);                           // T 0x0826f842
    int GetRewardItemGiveList(int job, int growType);                        // T 0x0826f814（透传）
    int CheckupAllRentalItems(CUser& user,
                              const InstanceRentalSystem::RentInput& input); // T 0x0826f52c
    bool checkRentalLevel(int level);                                        // T 0x0826f7e4
    bool checkRentalGrowJob(int growType);                                   // T 0x0826f7fc
    void log(CUser& user, int itemId);                                       // T 0x0826ef16
    void initLoggingEnvironment();                                           // ORIG T 0x826efdc
    // ORIG 0x0826eeb7：成员访问经 auto_ptr::operator-> 实调用（非内联展开）。
    std::auto_ptr<RentalMachineDimensionImpl> m_impl;  // +0x04（ORIG auto_ptr）
};


// _DailySchedule / CDailyScheduleManager 权威定义见 CDailyScheduleManager.h
// （include 引入；本 TU 的 C1/D1 定义保留于下方实现区）。

// online_preliminary 域（CTeamFactory/COnlinePreliminaryTeamMgr）权威声明已迁
// OnlinePreliminary.h（OnlyOrig_OnlinePvp 批）；本 TU 构造/析构/OnLoadTeamInfo
// 保留于下方原实现位置，经该头取完整类型。

// game_master 命令体系完整声明见 GameMasterCmd.h（GmCore/GmRegister 等 TU 实现）。


class DB_ReloadAutoPunishRuleBackIP
{
public:
    static void makeRequest();
};

class DB_ReloadAutoPunishRuleHackType
{
public:
    static void makeRequest();
};

class DB_LoadCleanPadPoint
{
public:
    static void makeRequest();
};

class DB_BlackIPMonitor
{
public:
    static void makeRequest();
};

void* CreateAntiServerObj(const char* path);

// ORIG 0x0822ad44 W：return *(char*)GetLightServerFlag()（GetLightServerFlag 见
// CDataManagerScripts.cpp，ORIG 0x822ad3a）。
bool* GetLightServerFlag();   // extern（CDataManagerScripts.cpp 定义）
#include "GlobalData/IsLightServer.inc"
}


namespace random_option
{
void* GetRandomOption();  // ORIG T 0x085f0eb0：&G_CDataManager()->+0x148

}

class CHumanCertifier
{
public:
    static void init();
    // ORIG B 0x93f5758：map<string, vector<int>> 人证类型表。
    static std::map<std::string, std::vector<int> > humanCertifyTable_;
};

class CDBEnviroment : public CEnvironment
{
public:
    CDBEnviroment();
    ~CDBEnviroment();
    bool parse(char* buf);   // ORIG T 0x0829d8e8（覆盖 CEnvironment::parse）
private:
    // ORIG 布局：CEnvironment(+0x3dc 止) 之后 +0x3e0 为 db_type 枚举（parse 写
    // 1..0xd/0x63/0x64/0x11；G_CEnvironment() 的 m_serverGroupMap 按此键归类）。
    ENUM_SERVER_GROUP m_dbType;   // +0x3e0
    char m_pad[4];                // +0x3e4（总 0x3e8）
};

// ---- Captcha ----
// ---- XorRand4：权威声明 XorRand4.h、定义 XorRand.cpp（ORIG 符号均为 T 强
// 符号 ⇒ 非 inline；本 TU 原重复 inline 副本已删，§4.4 单一声明点）。

class GreyImage
{
public:
    GreyImage();
    ~GreyImage();
    void init(int width, int height, unsigned char fill);
    void reset();
    int getWidth() const;
    int getHeight() const;
    void* getImage();
    void* getImage() const;
    unsigned char getAlpha(int x, int y);
    void setAlpha(int x, int y, unsigned char alpha);
    void setAlphaAsMax(int x, int y, unsigned char alpha);
    int sampleAlpha(int x, int y);
private:
    int m_width;          // +0x00
    int m_height;         // +0x04
    unsigned char* m_image;  // +0x08
};

// GreyImage 方法实现在 GameStubs.cpp（ORIG 中 GreyImage 独立 TU；GlobalData TU
// 仅保留声明，使 GenerateCaptchaData 的 EH 清理结构对齐 ORIG）。

// ---- drawLine（ORIG 0x080dea17，文件内静态）----
static void drawLine(GreyImage* img, int x1, int y1, int x2, int y2)
{
    if (x1 >= x2)
    {
        return;
    }
    int x = x1;
    int y = y1;
    int dx = x2 - x1;
    int dy = y2 - y1;
    int err = dx / 2;
    int step = 1;
    if (y2 < y1)
    {
        step = -1;
        dy = -dy;
    }
    while (x < x2)
    {
        if (x != x1 || y != y1)
        {
            img->setAlphaAsMax(x, y, 0xff);
        }
        ++x;
        err -= dy;
        if (err < 0)
        {
            y += step;
            err += dx;
        }
    }
}

class CaptchaGen
{
public:
    struct ImageOption
    {
        ImageOption();
        int m_field0;    // +0x00 噪点圆数量
        int m_field4;    // +0x04 噪点圆最大半径
        int m_field8;    // +0x08 干扰线数量
        float m_fieldc;  // +0x0c 旋角下限
        float m_field10; // +0x10 旋角上限
        float m_field14; // +0x14 缩放下限
        float m_field18; // +0x18 缩放上限
        int m_field1c;   // +0x1c 字符纵向偏移范围
        float m_field20; // +0x20 字符推进系数
        float m_field24; // +0x24 波形频率下限
        float m_field28; // +0x28 波形频率上限
        float m_field2c; // +0x2c 波形幅度下限
        float m_field30; // +0x30 波形幅度上限
    };

    CaptchaGen();
    ~CaptchaGen();
    bool addFont(const char* font, int size);
    void generateText(CountryCode code, int len, unsigned short* out);
    void generateImage(int width, int height, const unsigned short* text, int len,
                       const ImageOption& opt, GreyImage* image);
    // ---- generateText 字符集（ORIG 原表；类静态成员，定义见类后）----
    static const unsigned short kKoreanGeneral[978];   // VA 0x08b2bca0
    static const unsigned short kJapaneseHirakana[72]; // VA 0x08b2c460
    static const unsigned short kTaiwanAlphabet[26];   // VA 0x08b2c500
    // [b23S3C] 非 const：ORIG(4.1) -O0 对 rodata 常量仍出运行时 mov 读取再 div；
    // 4.4 见 const 初始化器即折成魔数乘（mul/imul 形）。探针 /tmp/r3probe/p4 实证非 const 复原 `mov addr;div`。
    // ORIG 0x080de87x：divisor 装载 `mov <A>,%edx; mov %edx,%ecx`（int 读入
    // edx 再转 unsigned 到 ecx）⇒ 声明 int + 使用处显式转 unsigned（P211 档③）。
    static int kKoreanGeneralCount;     // VA 0x08b2c444（978）
    static int kTaiwanAlphabetCount;    // VA 0x08b2c534（26）
    static int kJapaneseHirakanaCount;  // VA 0x08b2c4f0（72）
private:
    void* m_library;             // +0x00（ORIG FT_Library；本还原中为句柄占位）
    std::vector<void*> m_faces;  // +0x04
    XorRand4 m_rng;              // +0x10
};

// ---- CaptchaGen 字符集（ORIG .rodata；类静态成员定义，见 CaptchaGen.h）----
const unsigned short CaptchaGen::kKoreanGeneral[978] = {
    0xac00, 0xac01, 0xac04, 0xac07, 0xac08, 0xac10, 0xac11, 0xac12, 0xac13, 0xac15, 0xac16, 0xac19,
    0xac1a, 0xac1b, 0xac1c, 0xac1d, 0xac40, 0xac54, 0xac70, 0xac71, 0xac74, 0xac77, 0xac78, 0xac80,
    0xac81, 0xac83, 0xac89, 0xac8c, 0xaca8, 0xaca9, 0xacaa, 0xacac, 0xacb0, 0xacb9, 0xacbd, 0xacc1,
    0xacc4, 0xace0, 0xace1, 0xace4, 0xace7, 0xace8, 0xacf0, 0xacf1, 0xacf3, 0xacf5, 0xacfc, 0xad00,
    0xad11, 0xad1c, 0xad34, 0xad49, 0xad50, 0xad6c, 0xad6d, 0xad70, 0xad73, 0xad74, 0xad75, 0xad76,
    0xad7d, 0xad81, 0xad8c, 0xadc0, 0xadd3, 0xaddc, 0xade0, 0xade4, 0xadf8, 0xadf9, 0xadfc, 0xae00,
    0xae01, 0xae08, 0xae09, 0xae0b, 0xae0d, 0xae30, 0xae34, 0xae38, 0xae40, 0xae45, 0xae4a, 0xae4c,
    0xae4d, 0xae4e, 0xae50, 0xae54, 0xae5c, 0xae5d, 0xae61, 0xae65, 0xae68, 0xaebc, 0xaebe, 0xaecc,
    0xaecd, 0xaecf, 0xaed1, 0xaed8, 0xaef4, 0xaf2c, 0xaf2d, 0xaf34, 0xaf3c, 0xaf3d, 0xaf42, 0xaf43,
    0xaf49, 0xaf64, 0xafb8, 0xafbc, 0xafc0, 0xafc8, 0xb00c, 0xb044, 0xb048, 0xb04a, 0xb04c, 0xb053,
    0xb054, 0xb057, 0xb05d, 0xb07c, 0xb08c, 0xb098, 0xb099, 0xb09a, 0xb09c, 0xb0a0, 0xb0a1, 0xb0a8,
    0xb0a9, 0xb0ab, 0xb0ad, 0xb0ae, 0xb0af, 0xb0b1, 0xb0b3, 0xb0b4, 0xb0bc, 0xb0c4, 0xb0c7, 0xb0c9,
    0xb0d0, 0xb0e5, 0xb108, 0xb109, 0xb10c, 0xb110, 0xb113, 0xb118, 0xb123, 0xb124, 0xb125, 0xb137,
    0xb140, 0xb141, 0xb144, 0xb150, 0xb155, 0xb178, 0xb179, 0xb17c, 0xb180, 0xb188, 0xb18d, 0xb192,
    0xb193, 0xb194, 0xb1cc, 0xb1e8, 0xb204, 0xb208, 0xb215, 0xb258, 0xb274, 0xb284, 0xb290, 0xb291,
    0xb294, 0xb298, 0xb299, 0xb2a5, 0xb2a6, 0xb2ac, 0xb2c8, 0xb2d0, 0xb2d8, 0xb2e4, 0xb2e5, 0xb2e6,
    0xb2e8, 0xb2eb, 0xb2ec, 0xb2ed, 0xb2ee, 0xb2f4, 0xb2f5, 0xb2f7, 0xb2f9, 0xb2ff, 0xb300, 0xb301,
    0xb310, 0xb313, 0xb354, 0xb355, 0xb358, 0xb35c, 0xb35f, 0xb364, 0xb365, 0xb367, 0xb369, 0xb36e,
    0xb370, 0xb378, 0xb3c4, 0xb3c5, 0xb3c8, 0xb3cc, 0xb3d5, 0xb3d7, 0xb3d9, 0xb3fc, 0xb418, 0xb41c,
    0xb450, 0xb451, 0xb458, 0xb460, 0xb461, 0xb465, 0xb4a4, 0xb4b7, 0xb4dc, 0xb4dd, 0xb4e0, 0xb4e3,
    0xb4e4, 0xb4ec, 0xb4ed, 0xb4ef, 0xb4f1, 0xb514, 0xb529, 0xb52a, 0xb530, 0xb531, 0xb534, 0xb538,
    0xb540, 0xb545, 0xb54c, 0xb55c, 0xb5a0, 0xb5a1, 0xb5a4, 0xb5a8, 0xb5bb, 0xb5bc, 0xb610, 0xb611,
    0xb69c, 0xb6ab, 0xb6b1, 0xb6f0, 0xb728, 0xb729, 0xb72f, 0xb730, 0xb73b, 0xb744, 0xb77c, 0xb77d,
    0xb780, 0xb78c, 0xb78d, 0xb791, 0xb797, 0xb798, 0xb79c, 0xb7a8, 0xb7ab, 0xb7b5, 0xb7c9, 0xb7ec,
    0xb7ed, 0xb7f0, 0xb7f4, 0xb7fc, 0xb7fd, 0xb7ff, 0xb801, 0xb807, 0xb808, 0xb809, 0xb80c, 0xb824,
    0xb825, 0xb828, 0xb82c, 0xb835, 0xb839, 0xb840, 0xb85c, 0xb85d, 0xb860, 0xb86c, 0xb86d, 0xb86f,
    0xb8cc, 0xb8e8, 0xb8e9, 0xb8f9, 0xb8fb, 0xb904, 0xb958, 0xb959, 0xb960, 0xb96d, 0xb974, 0xb978,
    0xb97c, 0xb984, 0xb987, 0xb98e, 0xb9ac, 0xb9ad, 0xb9b0, 0xb9bc, 0xb9bd, 0xb9bf, 0xb9c1, 0xb9c8,
    0xb9c9, 0xb9cc, 0xb9ce, 0xb9d0, 0xb9d1, 0xb9d8, 0xb9d9, 0xb9db, 0xb9dd, 0xb9de, 0xb9e1, 0xb9e3,
    0xb9e4, 0xb9e5, 0xb9e8, 0xb9f5, 0xb9fa, 0xba38, 0xba39, 0xba3c, 0xba40, 0xba48, 0xba4b, 0xba4d,
    0xba4e, 0xba54, 0xba58, 0xba69, 0xba70, 0xba74, 0xba78, 0xba85, 0xba87, 0xbaa8, 0xbaa9, 0xbaac,
    0xbab0, 0xbab8, 0xbab9, 0xbabb, 0xbabd, 0xbb18, 0xbb34, 0xbb35, 0xbb36, 0xbb38, 0xbb3b, 0xbb3c,
    0xbb44, 0xbb47, 0xbb50, 0xbb58, 0xbb63, 0xbbc0, 0xbbf8, 0xbbfc, 0xbbff, 0xbc00, 0xbc09, 0xbc0c,
    0xbc0f, 0xbc11, 0xbc14, 0xbc15, 0xbc16, 0xbc18, 0xbc1b, 0xbc1c, 0xbc1d, 0xbc1f, 0xbc24, 0xbc25,
    0xbc29, 0xbc2d, 0xbc30, 0xbc31, 0xbc40, 0xbc43, 0xbc49, 0xbc84, 0xbc88, 0xbc8c, 0xbc94, 0xbc95,
    0xbc97, 0xbca0, 0xbca4, 0xbca8, 0xbcbc, 0xbcbd, 0xbcc0, 0xbcc4, 0xbccd, 0xbcd1, 0xbcd5, 0xbcf4,
    0xbcf5, 0xbcf6, 0xbcf8, 0xbcfc, 0xbd04, 0xbd07, 0xbd09, 0xbd48, 0xbd59, 0xbd80, 0xbd81, 0xbd84,
    0xbd88, 0xbd89, 0xbd90, 0xbd93, 0xbd95, 0xbd99, 0xbdf0, 0xbe0c, 0xbe10, 0xbe14, 0xbe44, 0xbe4c,
    0xbe54, 0xbe57, 0xbe5a, 0xbe5b, 0xbe60, 0xbe61, 0xbe68, 0xbe75, 0xbe7c, 0xbe8f, 0xbea8, 0xbed0,
    0xbed4, 0xbed7, 0xbf08, 0xbf09, 0xbf51, 0xbfcc, 0xbfd0, 0xc058, 0xc068, 0xc0ac, 0xc0ad, 0xc0b0,
    0xc0b4, 0xc0b6, 0xc0bc, 0xc0bf, 0xc0c1, 0xc0c8, 0xc0c9, 0xc0cc, 0xc0dd, 0xc0e4, 0xc11c, 0xc11d,
    0xc11e, 0xc120, 0xc124, 0xc12c, 0xc12d, 0xc12f, 0xc131, 0xc138, 0xc139, 0xc13c, 0xc148, 0xc14b,
    0xc154, 0xc158, 0xc168, 0xc18c, 0xc18d, 0xc190, 0xc194, 0xc19c, 0xc19f, 0xc1a1, 0xc1a5, 0xc1c4,
    0xc1e0, 0xc1fc, 0xc218, 0xc219, 0xc21c, 0xc21f, 0xc220, 0xc228, 0xc22b, 0xc22d, 0xc232, 0xc26c,
    0xc270, 0xc27d, 0xc288, 0xc2a4, 0xc2a8, 0xc2ac, 0xc2b4, 0xc2b5, 0xc2b7, 0xc2b9, 0xc2dc, 0xc2dd,
    0xc2e0, 0xc2e3, 0xc2e4, 0xc2eb, 0xc2ec, 0xc2ed, 0xc2ef, 0xc2f1, 0xc2f6, 0xc2f8, 0xc2f9, 0xc2fc,
    0xc300, 0xc30d, 0xc313, 0xc368, 0xc369, 0xc370, 0xc379, 0xc384, 0xc3d8, 0xc3df, 0xc464, 0xc4f0,
    0xc4f4, 0xc4f8, 0xc500, 0xc50c, 0xc528, 0xc529, 0xc52c, 0xc539, 0xc53b, 0xc544, 0xc545, 0xc548,
    0xc549, 0xc54a, 0xc54c, 0xc553, 0xc554, 0xc555, 0xc557, 0xc559, 0xc55e, 0xc560, 0xc561, 0xc568,
    0xc57c, 0xc57d, 0xc580, 0xc584, 0xc587, 0xc591, 0xc595, 0xc597, 0xc598, 0xc5b4, 0xc5b5, 0xc5b8,
    0xc5b9, 0xc5bb, 0xc5bc, 0xc5c4, 0xc5c5, 0xc5c6, 0xc5c7, 0xc5c9, 0xc5ca, 0xc5cc, 0xc5ce, 0xc5d0,
    0xc5d4, 0xc5d8, 0xc5ec, 0xc5ed, 0xc5f0, 0xc5f4, 0xc5f7, 0xc5fc, 0xc5fd, 0xc5ff, 0xc601, 0xc606,
    0xc608, 0xc61b, 0xc624, 0xc625, 0xc628, 0xc62c, 0xc62e, 0xc633, 0xc637, 0xc639, 0xc640, 0xc644,
    0xc655, 0xc65c, 0xc660, 0xc678, 0xc67c, 0xc694, 0xc695, 0xc6a9, 0xc6b0, 0xc6b1, 0xc6b4, 0xc6b8,
    0xc6c0, 0xc6c3, 0xc6c5, 0xc6cc, 0xc6d0, 0xc6d4, 0xc6e8, 0xc6ec, 0xc704, 0xc717, 0xc720, 0xc721,
    0xc728, 0xc73c, 0xc73d, 0xc740, 0xc744, 0xc74c, 0xc751, 0xc758, 0xc774, 0xc775, 0xc778, 0xc77c,
    0xc77d, 0xc783, 0xc784, 0xc785, 0xc787, 0xc788, 0xc78a, 0xc78e, 0xc790, 0xc791, 0xc794, 0xc796,
    0xc798, 0xc7a0, 0xc7a1, 0xc7a3, 0xc7a5, 0xc7a6, 0xc7ac, 0xc7c1, 0xc7e4, 0xc800, 0xc801, 0xc804,
    0xc808, 0xc80a, 0xc810, 0xc811, 0xc813, 0xc815, 0xc816, 0xc81c, 0xc820, 0xc82f, 0xc838, 0xc870,
    0xc871, 0xc874, 0xc878, 0xc880, 0xc881, 0xc885, 0xc88b, 0xc88c, 0xc8c4, 0xc8fc, 0xc8fd, 0xc900,
    0xc904, 0xc90c, 0xc90d, 0xc911, 0xc950, 0xc988, 0xc989, 0xc98c, 0xc990, 0xc998, 0xc99d, 0xc9c0,
    0xc9c1, 0xc9c4, 0xc9c8, 0xc9d0, 0xc9d1, 0xc9d3, 0xc9d5, 0xc9d9, 0xc9da, 0xc9dc, 0xc9dd, 0xc9e0,
    0xc9e2, 0xc9e7, 0xc9f8, 0xca0c, 0xca4c, 0xca4d, 0xca50, 0xca54, 0xca5c, 0xcabd, 0xcad3, 0xcb48,
    0xcb49, 0xcc0c, 0xcc0d, 0xcc22, 0xcc28, 0xcc29, 0xcc2c, 0xcc2e, 0xcc30, 0xcc38, 0xcc3b, 0xcc3d,
    0xcc3e, 0xcc44, 0xcc45, 0xcc54, 0xcc59, 0xcc98, 0xcc99, 0xcc9c, 0xcca0, 0xcca9, 0xccab, 0xccad,
    0xccb4, 0xccd0, 0xcd08, 0xcd09, 0xcd0c, 0xcd1b, 0xcd1d, 0xcd2c, 0xcd5c, 0xcd94, 0xcd95, 0xcd98,
    0xcd9c, 0xcda4, 0xcda5, 0xcda7, 0xcda9, 0xcde8, 0xce20, 0xce21, 0xce30, 0xce35, 0xce58, 0xce59,
    0xce5c, 0xce60, 0xce68, 0xce6b, 0xce6d, 0xce74, 0xce78, 0xce7c, 0xce84, 0xce90, 0xcea0, 0xcee4,
    0xcee8, 0xceec, 0xcef4, 0xcef5, 0xcef7, 0xcf00, 0xcf13, 0xcf1c, 0xcf54, 0xcf58, 0xcf5c, 0xcf64,
    0xcf69, 0xcf8c, 0xcfc4, 0xcfe0, 0xd034, 0xd06c, 0xd070, 0xd074, 0xd07c, 0xd0a4, 0xd0ac, 0xd0c0,
    0xd0c1, 0xd0c4, 0xd0c8, 0xd0d1, 0xd0d3, 0xd0d5, 0xd0dc, 0xd0dd, 0xd0e4, 0xd130, 0xd131, 0xd134,
    0xd138, 0xd145, 0xd14c, 0xd14d, 0xd154, 0xd15c, 0xd1a0, 0xd1a4, 0xd1a8, 0xd1b1, 0xd1b5, 0xd1f4,
    0xd22c, 0xd234, 0xd23c, 0xd241, 0xd280, 0xd29c, 0xd2b8, 0xd2b9, 0xd2bc, 0xd2bf, 0xd2c0, 0xd2c8,
    0xd2f0, 0xd2f1, 0xd300, 0xd305, 0xd30c, 0xd30e, 0xd310, 0xd314, 0xd31d, 0xd328, 0xd329, 0xd32c,
    0xd33d, 0xd37c, 0xd37d, 0xd398, 0xd39c, 0xd3b4, 0xd3b8, 0xd3bc, 0xd3c9, 0xd3d0, 0xd3ec, 0xd3ed,
    0xd3f0, 0xd45c, 0xd478, 0xd479, 0xd480, 0xd488, 0xd48d, 0xd4e8, 0xd504, 0xd50c, 0xd514, 0xd53c,
    0xd53d, 0xd544, 0xd54f, 0xd551, 0xd558, 0xd559, 0xd55c, 0xd560, 0xd568, 0xd569, 0xd56d, 0xd574,
    0xd575, 0xd578, 0xd584, 0xd587, 0xd589, 0xd5a5, 0xd5c8, 0xd5cc, 0xd5d8, 0xd5e4, 0xd5ec, 0xd600,
    0xd604, 0xd608, 0xd611, 0xd615, 0xd61c, 0xd638, 0xd639, 0xd63c, 0xd640, 0xd648, 0xd649, 0xd64d,
    0xd654, 0xd655, 0xd658, 0xd65c, 0xd669, 0xd68c, 0xd68d, 0xd69f, 0xd6a1, 0xd6a8, 0xd6c4, 0xd6c8,
    0xd6cc, 0xd6d4, 0xd6e8, 0xd718, 0xd734, 0xd749, 0xd750, 0xd751, 0xd754, 0xd758, 0xd759, 0xd761,
    0xd765, 0xd769, 0xd76c, 0xd770, 0xd788, 0xd798,
};
const unsigned short CaptchaGen::kJapaneseHirakana[72] = {
    0x3042, 0x3044, 0x3046, 0x3048, 0x304a, 0x304b, 0x304c, 0x304d, 0x304e, 0x304f, 0x3050, 0x3051,
    0x3052, 0x3053, 0x3054, 0x3055, 0x3056, 0x3057, 0x3058, 0x3059, 0x305a, 0x305b, 0x305c, 0x305d,
    0x305e, 0x305f, 0x3060, 0x3061, 0x3062, 0x3064, 0x3065, 0x3066, 0x3067, 0x3068, 0x3069, 0x306a,
    0x306b, 0x306c, 0x306d, 0x306e, 0x306f, 0x3070, 0x3071, 0x3072, 0x3073, 0x3074, 0x3075, 0x3076,
    0x3077, 0x3078, 0x3079, 0x307a, 0x307b, 0x307c, 0x307d, 0x307e, 0x307f, 0x3080, 0x3081, 0x3082,
    0x3084, 0x3086, 0x3088, 0x3089, 0x308a, 0x308b, 0x308c, 0x308d, 0x308f, 0x3090, 0x3092, 0x3093,
};
const unsigned short CaptchaGen::kTaiwanAlphabet[26] = {
    0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049, 0x004a, 0x004b, 0x004c,
    0x004d, 0x004e, 0x004f, 0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058,
    0x0059, 0x005a,
};
int CaptchaGen::kKoreanGeneralCount = 978;    // VA 0x08b2c444
int CaptchaGen::kTaiwanAlphabetCount = 26;    // VA 0x08b2c534
int CaptchaGen::kJapaneseHirakanaCount = 72;  // VA 0x08b2c4f0

// CaptchaGen 实现（G7 验证码；ORIG 走 FreeType，本环境无 FreeType，按算法等价实现）
#include "GlobalData/ImageOption.inc"
}

// FreeType API（ORIG 0x087bcac0，FT_Library = void* 句柄占位，仓内无 ftdriver
// 头；extern "C" 保 mangled 与 ORIG 符号一致）。本环境无 libfreetype：给占位
// 定义保链接（CaptchaGen ctor 的 call 站点 codegen 仍与 ORIG 一致；函数体本身
// 属静态链入的第三方库，不参与 identical 目标，同 crypto 豁免）。
extern "C" int FT_Init_FreeType(void** alibrary)
{
    if (alibrary)
    {
        *alibrary = 0;
    }
    return 0;
}

// FreeType 收尾 API（ORIG 0x087c14b0）。[SweepCAutoPunish 2026-09-10] 定义已迁
// StubS3A_deps.cpp（P210② 跨 TU 迁定义，Main 授权）：本 TU 只见声明 ⇒ 4.4 判该
// 调用可能抛，CaptchaGen dtor 复现 ORIG 0x080de706 的 EH landing（push esi/ebx +
// vector D2 cleanup + _Unwind_Resume）。探针 /tmp/gdprobe/p2.cpp（同 TU 定义 ⇒
// 无 landing；仅声明 ⇒ 有 landing）实证；nm 全链接集确认无其它定义。
extern "C" int FT_Done_Face(void* face);

// FreeType 库内部收尾函数（ORIG 0x087c3c00 FT_Done_Library / 0x087dc6b0
// FT_Done_Memory / 0x087c43a0 FT_Open_Face）。均真实存在于 ORIG，属静态链入
// 第三方库本体，函数体不参与 identical 目标（同 crypto 豁免）；wrapper
// FT_Done_FreeType / FT_New_Face 的调用站点 codegen 需与 ORIG 一致，故此处给
// 占位定义保链接。FT_Library 首字段为 memory（offset 0），见 idx49 报告汇编。
extern "C" int FT_Done_Library(void* library)
{
    (void)library;
    return 0;
}

extern "C" int FT_Done_Memory(void* memory)
{
    (void)memory;
    return 0;
}

// FT_Open_Face 用参结构（FreeType FT_Open_Args_，pathname@0xc、stream@0x10）
struct FT_Open_Args
{
    unsigned int flags;   // +0x00
    const void*  memory_base;  // +0x04
    long         memory_size;  // +0x08
    const char*  pathname;     // +0x0c
    void*        stream;       // +0x10
    void*        module;       // +0x14
};

extern "C" int FT_Open_Face(void* library, FT_Open_Args* args, long face_index,
                            void** aface)
{
    (void)library;
    (void)args;
    (void)face_index;
    if (aface)
    {
        *aface = 0;
    }
    return 0;
}

// [b22判词·N-codegen] FT_Done_FreeType 语义忠实 ORIG 0x087bca50（体已迁
// StubS3A_deps.cpp，见上条注释；本 TU 仅声明）。残余 DIFF 全属跨版本 codegen
// 结构差：①ORIG 用 callee-saved %ebx 跨两次 call 持有 memory（push/pop %ebx），
// 4.4.7 -O0 为具名局部 memory 建槽；②ORIG `test %eax,%eax` vs 本 `cmpl $0,0x8(%ebp)`；
// ③ORIG epilogue 有 %ebx 压栈 vs 本 `mov $0,%eax; leave; ret`；④call 未 relink
// 时为 .o 重定位占位（§6.3 假阳性），relink 后转 AE。语义等价。
extern "C" int FT_Done_FreeType(void* library);

// addFont 用 FreeType API（ORIG 0x087c4730/0x087c2f20/0x087bdd80）占位定义，
// 豁免同 FT_Init_FreeType（本环境无 libfreetype；调用点 codegen 与 ORIG 一致，
// 函数体属静态链入第三方库不参与 identical）。FT_ENCODING_UNICODE 以字面量
// 0x756e6963（"cinu"）代入。
// [b22判词·N-codegen] FT_New_Face 语义忠实 ORIG 0x087c4730（pathname==0 返 6；否则
// 构造 FT_Open_Args{flags=4,pathname,stream=0} 调 FT_Open_Face 并透传返回值）。
// 残余 DIFF 全属跨版本 codegen 结构差、无法源码复现：①ORIG 把返回值 FVar1 留在
// %eax 跨分支（寄存器化标量局部，违 P0），4.4.7 -O0 一律为具名局部建 result 槽；
// ②args 结构帧槽号（ORIG -0x28/-0x1c/-0x18 vs 本 -0x18/-0xc/-0x8）与帧尺寸
// （0x38 vs 0x30）随局部槽化次序错位；③ORIG `mov 0xc(%ebp),%edx;test %edx,%edx`
// vs 本 `cmpl $0x0,0xc(%ebp)`（test-reg vs cmp-slot）；④`call <FT_Open_Face>` 未
// relink 时为 .o 重定位占位（§6.3 假阳性），relink 后转 AE。语义等价。
extern "C" int FT_New_Face(void* library, const char* filepathname, long face_index,
                           void** aface)
{
    // ORIG 0x087c4730（idx73 报告）：pathname==0 返 6；否则构造
    // FT_Open_Args{flags=4, pathname, stream=0} 调 FT_Open_Face 并以其返回值返回。
    // ORIG 把返回值（FVar1）留在 %eax 跨分支（编译器寄存器化标量局部，违 P0 但
    // ORIG 事实）；本环境 4.4.7 -O0 一律为具名局部分配槽（见 diff：result 槽化 +
    // 帧尺寸/槽号/test-reg-vs-cmpl-slot 差），属跨版本 codegen 结构差，无法源码复现。
    if (filepathname == 0)
    {
        return 6;
    }
    FT_Open_Args args;
    args.flags = 4;
    args.pathname = filepathname;
    args.stream = 0;
    return FT_Open_Face(library, &args, face_index, aface);
}

extern "C" int FT_Set_Pixel_Sizes(void* face, unsigned int pixel_width,
                                  unsigned int pixel_height)
{
    (void)face;
    (void)pixel_width;
    (void)pixel_height;
    return 0;
}

extern "C" int FT_Select_Charmap(void* face, unsigned long encoding)
{
    (void)face;
    (void)encoding;
    return 0;
}

// ---- generateImage 用 FreeType 结构视图与 API 占位 ----
// [b28d-solo-GlobalData] 偏移实证：face+0x54=glyph（ORIG 0x080deff3
// `mov 0x54(%eax),%eax`）、glyph+0x4c=bitmap（0x080df034 `add $0x4c,%eax`）、
// FT_Bitmap{rows@0,width@4,pitch@8,buffer@0xc}（0x080df118/0x080df086/
// 0x080df0b2/0x080df0ac）。仅建模用到的字段；函数体属静态链入第三方库，
// 不参与 identical 目标（同既有 FT_* 占位豁免），符号名取自 ORIG nm。
typedef long FT_Fixed;
struct FT_Vector
{
    FT_Fixed x;   // +0x00
    FT_Fixed y;   // +0x04
};
struct FT_Matrix
{
    FT_Fixed xx;  // +0x00
    FT_Fixed xy;  // +0x04
    FT_Fixed yx;  // +0x08
    FT_Fixed yy;  // +0x0c
};
struct FT_Bitmap
{
    unsigned int rows;            // +0x00
    // ORIG 0x080df0xx：`mov -0x38(%ebp),%eax; lea 0x4(%eax),%edx` 把
    // **成员地址**直接喂 `std::min<int>`（`_ZSt3minIiERKT_S2_S2_`）⇒ 成员
    // 类型是 int（P321 T1：模板实例 + 取址直传，无临时中转）。
    int width;                    // +0x04
    int pitch;                    // +0x08
    unsigned char* buffer;        // +0x0c
    unsigned short num_grays;     // +0x10
    unsigned char pixel_mode;     // +0x12
    unsigned char palette_mode;   // +0x13
    void* palette;                // +0x14
};
struct FT_GlyphSlotRec
{
    char m_pad00[0x4c];
    FT_Bitmap bitmap;             // +0x4c
};
struct FT_FaceRec
{
    char m_pad00[0x54];
    FT_GlyphSlotRec* glyph;       // +0x54
};

extern "C" int FT_Set_Transform(FT_FaceRec* face, FT_Matrix* matrix,
                                FT_Vector* delta)
{
    (void)face;
    (void)matrix;
    (void)delta;
    return 0;
}

extern "C" unsigned int FT_Get_Char_Index(FT_FaceRec* face, unsigned long charcode)
{
    (void)face;
    (void)charcode;
    return 0;
}

extern "C" int FT_Load_Glyph(FT_FaceRec* face, unsigned int glyph_index,
                             int load_flags)
{
    (void)face;
    (void)glyph_index;
    (void)load_flags;
    return 0;
}

extern "C" int FT_Render_Glyph(FT_GlyphSlotRec* slot, int render_mode)
{
    (void)slot;
    (void)render_mode;
    return 0;
}

#include "GlobalData/CaptchaGen.inc"
}

// [SweepCAutoPunish 2026-09-10 已修·AE 49=49] ORIG 0x080de706 的 EH landing 复现：
// 根因是 FT_Done_Face/FT_Done_FreeType 占位定义**同 TU 可见** ⇒ 4.4 判 nothrow 不布
// landing。按 P210② 把这两个 extern "C" 定义迁 StubS3A_deps.cpp（Main 授权），本 TU
// 只留声明 ⇒ 4.4 判可能抛，逐条复现 ORIG：push esi/ebx + `call vector D2Ev` cleanup +
// `_Unwind_Resume` + 尾 `add $0x20; pop ebx/esi/ebp`。探针 /tmp/gdprobe/p2.cpp
// （同 TU 定义⇒无 landing；仅声明⇒有 landing）；nm 全链接集确认此前仅 GlobalData.o 定义。
// 旧判词「N-EH 占位定义可见性」作废。
#include "GlobalData/dtor_CaptchaGen.inc"
}

// [b28d-18 IDENTICAL_AE] 48=48 逐条同形（.o call 重定位标签伪差） | ORIG 0x080de794
#include "GlobalData/addFont.inc"
}

// [b28d-solo-GlobalData] 归零 100=100：P211 档③ 类型旋钮——三个静态计数
// unsigned int→int + 使用处 (unsigned int) cast，复现 `mov <A>,%edx; mov %edx,%ecx`
// 两段 divisor 装载（旧 N18a 判词作废）；CaptchaGen.h 同步（R2 已广播） | ORIG 0x080de83c
#include "GlobalData/generateText.inc"
}

// [b28d-18 N82] 残差=槽位/STL（N82/N89） | ORIG 0x080deade
// [b28d-solo-GlobalData] 语义缺口已闭合：p167 CLEAR（53/53，0 missing/0 extra）——补
// FT_Set_Transform/FT_Get_Char_Index/FT_Load_Glyph/FT_Render_Glyph extern "C" 占位 +
// FT_Matrix/FT_Vector/FT_Bitmap/FT_GlyphSlotRec/FT_FaceRec 视图，按 ORIG 0x080dee9x..
// 0x080df120 还原 16.16 定点矩阵与 bitmap 遍历（std::min<int>）；发明符号 static
// renderGlyph 已删。
// [SweepCAutoPunish 2026-09-10] 680→**682**（ORIG 685，余 3）：①字形 alpha 局部
// uchar→int（P321 T1 双段零扩展直证，探针 p4/p5.cpp）；②删 `(int)bm->width` cast +
// FT_Bitmap::width unsigned→int（ORIG 直取 &成员喂 min<int>）；③`dy*dy+dx*dx` 首项序
// （P243 同族）；④两个 row 守卫改 P223 负条件 goto 尾标签串，复现 ORIG 0x080df05c/
// 0x080df068 两岛（+4 条）。余 3 条 = XorRand4/min/setAlphaAsMax 段浮点/寄存器
// N85/N18a 族（无 jcc/调用差，p167 CLEAR）。
#include "GlobalData/generateImage.inc"
}

// s_capchaDataManager（ORIG 0x941f7a0；namespace GlobalData 成员）定义；
// 类型/方法见 CCapchaDataManager.h。
WongWork::CCapchaDataManager GlobalData::s_capchaDataManager;
#include "GlobalData/push.inc"
}

#include "GlobalData/isEmpty.inc"
}

#include "GlobalData/peekData.inc"
}

// ============================================================================
// 依赖类 stub 实现（GlobalData.cpp 自含声明的占位类；G2/G4/G6/G7/G8 批次接管）
// ============================================================================

#include "GlobalData/DBThread.inc"
}
#include "GlobalData/setLogDB.inc"
}
#include "GlobalData/isLogDB.inc"
}
// ---------------------------------------------------------------------------
// dispatch —— ORIG T 0x083ff786：new DBTask（0xd54）后进入 DB 事件循环；
// HandleEvents 处理 DB_Q/LOGDB_Q（isLogDB 决定 2/4），CMessageCounter(20000)
// 按 600s 周期 WriteLog；循环条件 !GlobalData::s_server_stop。
// ---------------------------------------------------------------------------
#include "GlobalData/dispatch.inc"
}
#include "GlobalData/DispatchThread.inc"
#include "GlobalData/NetworkThread.inc"
#include "GlobalData/UdpThread.inc"
class CPacketCounter
{
public:
    CPacketCounter(char* dir, char* name);
    ~CPacketCounter();
    void Reset();
    void IncrementPacketCount(int id);
    int m_totalCount;                 // +0x00
    int m_startTime;                  // +0x04
    unsigned int m_packetCount[0x2418];  // +0x08（id 域 0x3e8..0x27ff）
    int m_packetTime[0x2418];         // +0x9068
    char m_packetFlag[0x2418];        // +0x120c8
    int m_packetTotal[0x2418];        // +0x144e8
    char m_path[0x100];               // +0x1d548
    bool m_flagInit;                  // +0x1d648
    char m_flag2;                     // +0x1d649
};

template<int Lo, int Hi>
CPacketCounter<Lo, Hi>::CPacketCounter(char* dir, char* name)
{
    Reset();
    m_startTime = (int)time(0);
    if (dir)
    {
        sprintf(m_path, "./log/%s/%s", dir, name);
    }
    else
    {
        sprintf(m_path, "./log/%s", name);
    }
    m_flagInit = true;
}

template<int Lo, int Hi>
CPacketCounter<Lo, Hi>::~CPacketCounter() {}

template<int Lo, int Hi>
void CPacketCounter<Lo, Hi>::Reset()
{
    for (int i = 0; i <= 0x2417; ++i)
    {
        m_packetCount[i] = 0;
        m_packetTotal[i] = 0;
        m_packetTime[i] = 0;
        m_packetFlag[i] = 0;
    }
    m_totalCount = 0;
    m_flag2 = 0;
}

template<int Lo, int Hi>
void CPacketCounter<Lo, Hi>::IncrementPacketCount(int id)
{
    if (id > 0x27ff) return;
    if (id < 0x3e8) return;
    if (!m_flagInit && m_packetCount[id - 0x3e8] > 10) return;
    ++m_packetCount[id - 0x3e8];
}

// ---------------------------------------------------------------------------
// _bindUDPSocket —— ORIG 0x0857c3be：socket/bind/SetNonBlock，失败日志后 exit。
// ---------------------------------------------------------------------------
#include "GlobalData/_bindUDPSocket.inc"
}

// ---------------------------------------------------------------------------
// _processUDPData —— ORIG 0x0857c5a4：AddUdpLog → 静态计数 → dispatchPacket；
// 未识别包按首字节分流（0 互斥数据 / 1 换装队列 / 2 忽略 / 3 拍卖停止）。
// ---------------------------------------------------------------------------
// [b28d-18 N82] p167 CLEAR（51/51）；残差=帧槽（N82） | ORIG 0x0857c5a4
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（51/51）；残差=N82 槽位/寄存器 + N89。
#include "GlobalData/_processUDPData.inc"
}

// ---------------------------------------------------------------------------
// dispatch —— ORIG 0x0857c9de：bind UDP 口后 select 轮询 recv，转入
// _processUDPData；循环条件 !GlobalData::s_server_stop（0x940bd64）。
// ---------------------------------------------------------------------------
// [b28d-18 N82] p167 CLEAR（8/8）；残差=帧槽（N82） | ORIG 0x0857c9de
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR；残差=N82 槽位/寄存器分配。
// [L8 2026-09-10] ORIG 0x0857ca44 段实证：4.1 的 FD_ZERO 展开为**逐元素循环**
//   （`lea -0xc8,%eax; mov %eax,-0x24; movl $0x0,-0x28; …; movl $0x0,(%eax,%edx,4);
//   addl $1,-0x28; cmpl $0x1f,-0x28; setbe`）——即「具名 fd_set* 局部 + 独立循环
//   计数」两槽（-0x24/-0x28）；我方新头集 FD_ZERO 走 rep stos（0x22 段
//   `mov $0x20,%ecx; cld; rep stos`）完全无形且少这两槽，并把 readfds/buf 顶浅 0xc。
//   同时 ORIG 两处 for 使用**各自独立**的循环变量（-0x20/-0x1c 各 6 次），我方合并
//   为单个 `i`（-0x2c×12）⇒ 改为块内独立声明。
#include "GlobalData/dispatch_1.inc"
}
// ---------------------------------------------------------------------------
// dispatch —— ORIG T 0x0847e2d2：主分发循环。TimerEntry 取定时器消息 →
// TimerDispatcher::dispatch；DISPATCH_Q 取 Stream → 按流头 code 分流
// PacketDispatcher（客户端包）/ InterDispatcher（服务器互连包）；池归还按
// stream->m_poolType（0/1/2 = 默认/小/大流池）。CMessageCounter(10000) 与
// CUserPacketCounter 600s 周期落盘。pUserPacketCounter_ 指向本栈实例。
// ---------------------------------------------------------------------------
// [b28d-18 NEAR] 修：curSec/lastWriteSec 角色互换（ORIG 循环外初始化 curSec、循环内更新 lastWriteSec）；p167 CLEAR 53/53；余 N82 帧槽 | ORIG 0x0847e2d2
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（53/53）；gap 1=N82 帧伪影（勿强凑，P182）。
#include "GlobalData/dispatch_2.inc"
}

// ---------------------------------------------------------------------------
// dispatch —— ORIG T 0x0857d5be：网络收包线程。消费 AcceptThread 入队的
// s_accept_que（加锁逐个 SetEpoll），随后 epoll_wait(600, 100ms) 处理
// EPOLLIN(1)/EPOLLOUT(4)/错误掩码(0x18)；sendSize>0x6ddd0(450000) 判定
// 发送缓冲过大 → DisConnSig(6)。
// ---------------------------------------------------------------------------
// [b28d-solo-GlobalData] 修结构差（p167 29/29 CLEAR）：①s_server_stop 检查搬到
// 循环尾（ORIG 0x0857d89d `movzbl s_server_stop; xor $1; test; jne body` 为回边
// 目标，trace 落循环外函数尾 0x0857d8af）⇒ `while(!stop){body} trace;`；
// ②去 epoll 具名局部（P183，ORIG 0x0857d5c6 直 `call G_EpollHandler; mov %eax,(%esp)`）。
// [SweepCAutoPunish 2026-09-10 已修·AE 207/208] 旧判「gap=6/N82 槽位封顶」作废：
// ORIG 循环前 `movl $0,-0x1c`/`movl $0,-0x18` 与内层 `movl $0,-0x14` 是 P0 具名局部
// 直证 ⇒ 补函数级 user/nfds + 内层 quser + err 预清两句式（P198①）；err 守卫改
// P225 while 物化（ORIG 0x0857d68c setne/test/je）；sendSize 分支改正极性
// `if (>){DisConn}else{send}` 复现 ORIG 0x0857d80b `jbe` 落点（P47/P220）。
// compare_tu 已判 AE（.o 重定位标签伪差）；余 1 条 = N7 寄存器角色。
#include "GlobalData/dispatch_3.inc"
}
#include "GlobalData/MonitorTcpThread.inc"
#include "GlobalData/AcceptThread.inc"
}
// ---------------------------------------------------------------------------
// dispatch —— ORIG T 0x0857d95a：监控代理线程。monitor/guild 代理数组 +
// pcroom/community/auction/cera/hades 五个代理 socket 构建 fd_set，
// select(1s) 轮询 OnTcpRecv/OnTcpSend；断线按 60s 周期重连（auction 系
// InitTcp(true) + 虚 Disconnect + SendRegistPacket；community 仅整合 PvP
// 频道时处理）。
// ---------------------------------------------------------------------------
// [b28d-solo-GlobalData] p167 CLEAR（239/239，0 missing/0 extra）：FD_SET/FD_ISSET
// 三元链逐处重调（P157 禁 mon/s 缓存）+ 状态缓存消除（P48 独立调用）+
// 第二次 IsIntegratedPvPBaseChannel 守卫包裹 auction/cera 段（ORIG 0x0857edb5
// `xor $1; test; je 循环尾`——真语义缺口，整合 PvP 频道时跳过两段）。
// [SweepGD 2026-09-10] gap **256→18**（our 1830→1593，ORIG 1575）四修（按序）：
//   ① **FD_ZERO 手写旧宏形**：ORIG +0x0f5/+0x123 为逐元素循环（`lea fd; mov %eax,-0x50;
//      movl $0,-0x54; jmp; loop: mov -0x54,%edx; mov -0x50,%eax; movl $0,(%eax,%edx,4);
//      addl $1,-0x54; cmpl $0x1f,-0x54; setbe` = 具名 fd_set* + unsigned 计数两槽），
//      新头集宏恒 `rep stos` 无形 ⇒ 手写 `fd_set* fds=&readfds; __FDS_BITS(fds)[fi]=0;`
//      （`fd_set*` 成员数组成员视角产 ORIG 的 scaled `(%eax,%edx,4)`；用 `int*` 版多
//      `shl $2; add` 两条/处 —— P291/P288 具名数组成员视角）。
//   ② **FD_SET/FD_ISSET 实参 (unsigned) cast**（26 处）：ORIG 为 `shr $0x5` + `and $0x1f`
//      无符号形，我方 int 实参产有符号除 cmovs 修正链（`sar $0x1f; shr $0x1b; add; sar $0x5`
//      与 `% 32` 五条）⇒ 改 unsigned 后逐条同形（P230/N125 反向）。
//   ③ **外层循环重构**：ORIG +0x1a32 尾条件块 `movzbl s_server_stop; xor $0x1; test;
//      jne body`（函数头 `jmp +0x1a32`）⇒ 原 `while(true){if(s_server_stop)return;}` 改
//      `while (!GlobalData::s_server_stop)` 布尔字段裸条件，函数头 4 个 `jmp+nop` 中继岛
//      + 1 条头部多余跳转全部消失（P168a①/P292）。
//   ④ **函数头清零 12→5 条**：ORIG 0x0857d966 起仅 5 条 `movl $0,-N`（soError+lastReconnect×4）
//      ⇒ socket×5/proxyCount/ret 的 `= 0` 删除（-O0 裸声明零指令，P328⑤）。
// 残余 **+18 条**（判词族，勿再耗轮次）：
//   · 帧槽整体浅 8（我方 fd_set@-0x180 vs ORIG @-0x188，`sub` 两侧同 0x21c）⇒ **N126/N95**
//     版本首槽基址常数（P332 永封；l8 归一后 485 行差几乎全为槽号伪差，助记符级 NDIFF 仅 55）。
//   · 4 处 `cmp $0xffffffff; setne %al; test %al,%al; je`（ORIG +0x1c5/+0x2?? 段）vs 我方
//     `cmp; je` —— **setcc-fold 跨版本比较折叠**（P-B/setcc-fold 族；我方反而少 2 条/处，
//     且同族另 4 处我方多 5 条 choose 岛 `test;je;mov$1;jmp;mov$0`）。
//   · 尾部 1 处 `jmp+nop` 岛（N121 epilogue-threading）。
// p167 CLEAR（无调用缺口）；语义完成，byte 层 N 封顶。 | ORIG 0x0857d95a
#include "GlobalData/dispatch_4.inc"
}

// ---------------------------------------------------------------------------
// dispatch —— ORIG T 0x0857cc5a：accept 线程。socket/bind/listen/SetNonBlock
// 后进入 select(1s) accept 循环：按队列余量（m_acceptFlag 水位）与
// 用户池用量（Size-Remain、m_maxUserNum）拒绝/放行连接；放行时
// CGameManager::createUser → InitOnAccept → s_accept_que 入队（NetworkThread
// 消费）。IsProhibitConnectState / 监控 IP / LOCK STATE 分流拒绝包。
// §6.2 巨件核销（b25）：P167 修前 93/89 [缺4 G_CEnvironment]→修后 93/93
// [0缺0多]；gap 516→-6（our 524 超 6）；修法：①env 局部缓存去缓存化，
// 5 处恢复每处独立 G_CEnvironment() 调用（P183 反向，-O0 无 CSE）；
// ②FD_ZERO 改旧 glibc 宏手写展开（fd_set* + unsigned i 双局部逐 __fd_mask
// 清零；新 glibc 宏生 rep stos 形不符）；③FD_SET/FD_ISSET 实参 (unsigned)
// cast 消除有符号除 cmovs 修正链；④remain 预清两句式（P197③）；
// ⑤monitoring-ip strcmp 条件极性 ==0 {close;continue;} 复现 ORIG sete
// 物化 + fail fall-through（P59）。残差归因 P22 xor$1 物化 / setcc-fold /
// N7-N82 寄存器分配 / P63 jmp 中继（详见 docs/asm_translation_patterns.md
// 负例表）。
// ---------------------------------------------------------------------------
// [b28d-18 N82] p167 CLEAR（93/93）；残差=帧槽/寄存器（N82） | ORIG 0x0857cc5a
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（p167 全等）；gap 7=N82 槽位/寄存器。
#include "GlobalData/dispatch_5.inc"
}

// ---- GlobalData::Init 直接 new 的基础对象 ----
// CTimeGate ctor 现由 CTimeGate.cpp 提供（ORIG 0x8284512，继承 IObject）。
// [dedup] Arad_ServerStateManager ctor 权威定义在 Arad_ServerStateManager.cpp

// ORIG PacketPool::PacketPool() 0x0828fc76：DynamicPool(10)，容量与步长均为10，
// 检查回调清零；这是 GlobalData::Init 使用的真实默认池，而非空构造。
#include "GlobalData/PacketPool.inc"
}

#include "GlobalData/PacketPool_1.inc"
}
#include "GlobalData/dtor_PacketPool.inc"
}
#include "GlobalData/getThreadID.inc"
#include "GlobalData/setThreadID.inc"
#include "GlobalData/Acquire.inc"
}
#include "GlobalData/Free.inc"
}

// ---- CServerProxyMgr<CMonitorServerProxy>/<CGuildServerProxy>/<CStatisticServerProxy> ----
// 由 CServerProxyMgr.h 真实模板在下方 new/delete 处隐式实例化
// （ORIG 弱符号 0x82b314e/0x82b3212/0x82b3296/0x82b3b8e 等：+0x10 内嵌代理 +
// 字段清零 + Init；dtor 释放动态索引/代理数组）。CStatisticServerProxy 完整
// 类型来自 CStatisticServerProxy.h（经 CServerProxyMgr.h 模板），不再显式特化。

// CServerProxyMgr 三特化的 GetServerProxyArraySize（仅 Monitor）与
// GetServerProxyArrayAt（Monitor/Guild）在 ORIG 为弱符号实例（W 0x0857f8c2 /
// 0x0857f8ce / 0x0857f8fe），重建无调用点无法隐式实例化，故在此显式实例化
// 补齐（模板语义已与反汇编核对：ArraySize 读 m_count；ArrayAt 越界返 0，
// 元素步长 = sizeof(T)，Monitor 0x30e0c / Guild 0x30e24）。GetStartIndex/
// GetEndIndex/GetNextIndex 由 CGameManager.cpp 等消费 TU 直接成员调用隐式
// 实例化（非 asm 桥，旧注释已过时）。
template int CServerProxyMgr<CMonitorServerProxy>::GetServerProxyArraySize();
template CMonitorServerProxy* CServerProxyMgr<CMonitorServerProxy>::GetServerProxyArrayAt(int);
template CGuildServerProxy* CServerProxyMgr<CGuildServerProxy>::GetServerProxyArrayAt(int);


// CPCRoomServerProxy 仅在本 TU 定义（无独立源文件）。ORIG T 0x084717dc：
// CNetwork 基类构造 + string m_ip(ip)（初始化列表）；函数体写 m_tcpPort=port
// 再清 m_socketState（汇编：string 构造后 mov port→0x30dfc、movl $0→0x30e00）。
#include "GlobalData/CPCRoomServerProxy.inc"
}

// ORIG T 0x08471874：成员自动析构（string D1 → CNetwork D1，声明反向序）。
#include "GlobalData/dtor_CPCRoomServerProxy.inc"
}

// ORIG T 0x084718bc：!InitTcp() 返 0，否则返 1（P22 bool 取反形态）。
#include "GlobalData/Init.inc"
}

// ORIG T 0x084718e2：ip/port 有效且 m_sock<0 时 CreateConnectionSocket；
// 失败 cMyTrace 报错返 0；m_sock>=0 跳过连接，尾部汇合 return 1。
// 残余差归因（N 类，块内求值序，见下方 0x0847196d 区注释）：ORIG 因提前求值
// varargs 而多保存 %edi（0x084719d6 `add $0x4c,%esp; pop %ebx/esi/edi`），
// 4.4 帧 0x40/仅 %ebx ⇒ 块序与帧大小联动不可源码翻转。
// [b28d-18 N7+N82] p167 CLEAR（8/8）；残差=callee-saved 数量（ORIG push edi/esi/ebx vs 我们 esi/ebx） | ORIG 0x084718e2
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（8/8）；N80（cMyTrace ctor 前右起 varargs 跨
// ctor 保活 %edi/%esi/%ebx 不可翻——4.4 恒 ctor 后求值）+ N82 帧 0x4c vs 0x40。
#include "GlobalData/InitTcp.inc"
}

// ORIG T 0x084719de：连接有效（ip 非空且端口非 0）时 disconnect + 清 socketState。
#include "GlobalData/Disconnect.inc"
}

// ORIG T 0x08471a32：socketState==CONNECTED(2) 时经 m_tcp.send_packet 下发；
// 否则 cMyTrace 报 disconnected。
#include "GlobalData/SendTcpPacket.inc"
}

// ORIG T 0x08471a9a：recv_packet()>0 → Parsing 失败则 Disconnect 返 0；
// <0 → logFormat 后 Disconnect 返 0。
#include "GlobalData/OnTcpRecv.inc"
}

// ORIG T 0x08471b32：m_tcp.send_packet()<0 → 报错+Disconnect 返 -1；
// 否则返 m_recvOffset（+0x18）。
#include "GlobalData/OnTcpSend.inc"
}

// ORIG T 0x08471bd0：errno!=0 → 报错返 0；否则返 1。
#include "GlobalData/OnTcpError.inc"
}

// ORIG T 0x08471c5a：转发 Parsing_ServerPacket（返回 bool，OnTcpRecv 取反检验）。
#include "GlobalData/Parsing.inc"
}
// P119：ORIG W 弱符号（0x0857f444/0x0857f456/0x08471e08），inline 内联不发射
// 独立符号 → 本属主 TU 外联定义。GetSocketState 返回 int（ORIG mov 0x30e00,%eax）。
#include "GlobalData/SetSocketState.inc"
}
#include "GlobalData/GetSocketState.inc"
}
#include "GlobalData/GetTcpHandler.inc"
}
#include "GlobalData/CmmChannelProxy.inc"
}
#include "GlobalData/Init_1.inc"
}

// ---- WongWork ----
// CGMAccounts：ORIG isGM 0x08109346 —— 在 m_gmList 中查找 {mid, type=3}。
#include "GlobalData/CGMAccounts.inc"
}
#include "GlobalData/dtor_CGMAccounts.inc"
}
// [b28d-solo-GlobalData] 修：P56+P293 单表达式 if-return（end 先/find 后 + 分支物化） | ORIG 0x08109346
#include "GlobalData/isGM.inc"
}
#include "GlobalData/removeGM.inc"
}
#include "GlobalData/CCeraShop.inc"
}
#include "GlobalData/dtor_CCeraShop.inc"
}
#include "GlobalData/CAutoPunishRuleBlackIPMgr.inc"
#include "GlobalData/reset.inc"
}
#include "GlobalData/CAutoPunishRuleHackTypeMgr.inc"
}
#include "GlobalData/reset_1.inc"
}
// ORIG 0x82a6526：仅构造 multimap 成员（无其它标量初始化）。
#include "GlobalData/CBlackIPMonitor.inc"
}
#include "GlobalData/reset_2.inc"
}
// ORIG 无 stBlackIPMonitorEntry_t 默认 ctor（仅 0x84eba2c 带参版）⇒ 不定义。
#include "GlobalData/stBlackIPMonitorEntry_t.inc"
}
#include "GlobalData/insertNewIP.inc"
}
// [b28d-18 IDENTICAL_AE] 修：inet_addr 两次独立调用（P48）+ entry 整体拷贝（P157）；NDIFF 25→0 EQ | ORIG 0x084ebace
#include "GlobalData/deleteIP.inc"
}
// [b28d-18 IDENTICAL_AE] 修：inet_addr 两次独立调用 + entry 整体拷贝；NDIFF 46→0 EQ | ORIG 0x084ebba6
#include "GlobalData/getPunishType.inc"
}
// [b28d-18 IDENTICAL_AE] 修：inet_addr 两次独立调用 + entry 整体拷贝（P48/P157）；ORIG 切片缺失，objdump 手工核对 71=71 仅重定位伪差 | ORIG 0x084ebc8c
#include "GlobalData/GetEndTime.inc"
}
// ORIG 0x093ac220 数据表：31 行 × 24 列 SSO 密钥串（提取自 df_game_r .data）。
#include "GlobalData_SSO_keys.inc"

#include "GlobalData/CSimpleSSO.inc"
}

#include "GlobalData/dtor_CSimpleSSO.inc"
}

// [b28d-18 N18a] p167 CLEAR；ORIG 全局计数两段装载，4.4 恒单段（N18a 反向） | ORIG 0x086027fc
#include "GlobalData/init.inc"
}

// W7_BuildHarmonize 去重：CIPGHelper::{Initialize,Finalize} 唯一 owner 为
// W6_IPGDeps.cpp（ORIG 0x08100b3e/0x08100c92 完整转写，Finalize 先转发
// m_mgr->Finalize()）。此处原重复定义已移除。
#include "GlobalData/CIPGHelper.inc"
}
#include "GlobalData/dtor_CIPGHelper.inc"
}

// ---- 全局业务对象 ----
#include "GlobalData/ItemVendingMachine.inc"
}
#include "GlobalData/dtor_ItemVendingMachine.inc"
}
#include "GlobalData/BuyItem.inc"
}


// ============================================================================
// CheckPurcahseBonusItem —— ORIG 0x08179ed8 (T, sz 0x30)
// _ZN18ItemVendingMachine22CheckPurcahseBonusItemEm
// 报告 ItemVendingMachine/CheckPurcahseBonusItem.md：仅取 itemIdx 低 32 位作
// key 查 m_bonus（this+0x08，ARAD::CeraShopPurcahseCountBonus）.CheckBonusItem，
// 命中→true。Report C 体即 `CheckBonusItem((int)itemIdx)`。
// ============================================================================
#include "GlobalData/CheckPurcahseBonusItem.inc"
}

// ============================================================================
// ProcessPurcahseBonusItem —— ORIG 0x08179f08 (T)
// _ZN18ItemVendingMachine24ProcessPurcahseBonusItemEP5CUsermit
// 纯委托：m_bonus（ARAD::CeraShopPurcahseCountBonus，this+0x08）.
// GiveBonusItem(user, itemIdx, count, threshold)（ORIG 0x081abf44，已定义
// 于 Arad_ItemBonus.cpp）。报告 ProcessPurcahseBonusItem.md 全文转写。
// ============================================================================
#include "GlobalData/ProcessPurcahseBonusItem.inc"
}

// ============================================================================
// ProcessIPGUserData —— ORIG 0x0854c4f6 (T)
// _ZN18ItemVendingMachine17ProcessIPGUserDataEP5CUserPN8WongWork3IPG8SIPGDataES6_P6Stream
// 纯委托：按原参序转发 ProcessIPGUserDataJpnEx（ORIG 0x081783ea，同签名；
// JpnEx 定义属 IPG 计费应答族后续批次）。报告 ProcessIPGUserData.md 全文转写。
// ============================================================================
#include "GlobalData/ProcessIPGUserData.inc"
}
// DB 请求类最小声明（定义：DB_UpdateGiftCount.cpp / DB_UpdateCeraShopPurchaseCount.cpp；
// mangled 与 ORIG 一致：makeRequestEji / makeRequestEjmii）。
class DB_UpdateGiftCount
{
public:
    static void makeRequest(unsigned int accId, int count);
};
class DB_UpdateCeraShopPurchaseCount
{
public:
    static void makeRequest(unsigned int accId, unsigned long itemUid, int count, int param4);
};
class DB_SavePuUserCheck
{
public:
    static void makeRequest(unsigned int accId);   // ORIG T 0x0844c098（W8LT_DBTask.cpp 定义）
};

// ============================================================================
// IPG 计费应答族 —— LinkE2.JpnExImpl 批次（ORIG localjapan/AppendVendingMachine.cpp）。
// 覆盖：ProcessIPGUserDataJpnEx(0x081783ea) / ProcessIPG_ResultInput(0x0817866c) /
//   ProcessIPG_ResultQuery(0x081784d4) / ProcessIPG_ResultOutput(0x08178676)。
// 报告：docs/class_func_reports/ItemVendingMachine/{同名}.md，逐指令转写。
//
// SIPGData（TaiwanCash.h，0x2dc pad 权威布局）字段偏移实证（ORIG 汇编）：
//   +0x1c  应答状态 int（4=Query / 0=Input / 2=Output）
//   +0x69/+0x75/+0x81/+0x8d/+0x99/+0xa5  cera 数值项（atoi 目标，char[0xc] 步进）
//   +0xb1  返回码（atoi 目标）   +0xb5  cera point（atoi 目标）
//   +0x28/+0x3c/+0x69/+0x177  trace/log 文本槽；+0x7d/+0x177  itemName/accName
//   +0x173  结果返回码（atoi 目标）；+0x1e0/+0x1e4 计费 64 位 uid
//   （TaiwanCash.h SIPGData 属主批次未具名化，本 TU 以偏移视图访问【ORIG 实证】。）
// ============================================================================
namespace
{

// （旧 ipg_field1c/ipg_str 偏移寻址 helper 已删：TaiwanCash.h SIPGData 已按
// §8.3 字段化（W6SIPG 批次），本 TU 改用具名成员/命令族视图，P49。）
const char* const kPrettyJpnEx =
    "void ItemVendingMachine::ProcessIPGUserDataJpnEx(CUser*, WongWork::IPG::SIPGData*, WongWork::IPG::SIPGData*, Stream*)";
const char* const kPrettyResultQuery =
    "void ItemVendingMachine::ProcessIPG_ResultQuery(CUser*, WongWork::IPG::SIPGData*, WongWork::IPG::SIPGData*, Stream*)";
const char* const kPrettyResultOutput =
    "void ItemVendingMachine::ProcessIPG_ResultOutput(CUser*, WongWork::IPG::SIPGData*, WongWork::IPG::SIPGData*, Stream*)";

}  // namespace

// 商城 bonus 事件：权威声明见 CCeraShopBonusItemEvent.h（ORIG C1 0x0810a2dc /
// getBonusRate W 0x0817a2e2；定义归 CCeraShopAddRestrict.cpp）。

// ----------------------------------------------------------------------------
// ProcessIPGUserDataJpnEx —— ORIG 0x081783ea（0xe9 字节）。
// user 判空后按 resData+0x1c 分发；未匹配状态走 LogManager 告警。
// ----------------------------------------------------------------------------
#include "GlobalData/ProcessIPGUserDataJpnEx.inc"
}

// ----------------------------------------------------------------------------
// ProcessIPG_ResultInput —— ORIG 0x0817866c（0x9 字节：push/mov/cmp 0xc(%ebp)/pop/ret，
// 判空无副作用，函数体忠实为空）。
// ----------------------------------------------------------------------------
#include "GlobalData/ProcessIPG_ResultInput.inc"
}

// ----------------------------------------------------------------------------
// ProcessIPG_ResultQuery —— ORIG 0x081784d4（报告全文转写）。
// ret==0：六项 cera 求和 SetCera；+0xb5 SetCeraPoint；CeraPoint 日志；SendCashData(false)。
// ret!=0：SendCashData(true) 后 IPG_QUERY 日志。
// ----------------------------------------------------------------------------
#include "GlobalData/ProcessIPG_ResultQuery.inc"
}

// ----------------------------------------------------------------------------
// ProcessIPG_ResultOutput —— ORIG 0x08178676（0x12a8 字节，报告全文转写）。
// ----------------------------------------------------------------------------
// [b28d-18 N82] 超大件：残差=槽位/STL（N82/N89） | ORIG 0x08178676
// [b28d-solo-GlobalData] DIFF：p167 EXTRA memcpy×3/guard_abort×2/_Unwind_Resume×2/getItemList×2/
// make_pair/logFormat/ne —— 我方多发调用（多余局部/临时），已定位待收敛（非发明符号）。
#include "GlobalData/ProcessIPG_ResultOutput.inc"
}

// ----------------------------------------------------------------------------
// BillingRecord —— ORIG 0x0817991e（0x58f 字节，报告 BillingRecord.md + objdump 转写）。
// 打包 0x10e 字节计费记录（SIG 结构以文件内局部缓冲按 ORIG 偏移写入）后
// put_binary/入 DB 队列；PuUser 标志异常时补 DB_SavePuUserCheck。
// ----------------------------------------------------------------------------
// [b28d-18 N82] 残差=槽位/STL 展开（N82/N89） | ORIG 0x0817991e
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR；残差=N82 帧/槽 + N89 STL 展开。
#include "GlobalData/BillingRecord.inc"
}
// ============================================================================
// ItemVendingMachine 私有校验/串化 helper —— ORIG 成员（mangled 见
// docs/class_func_reports/ItemVendingMachine/README.md）
// ============================================================================
namespace
{
const char* const kPrettyGetItemString =
    "void ItemVendingMachine::_GetItemString(const CCeraShopGoods*, char*, size_t)";
const char* const kPrettyProcessBuy =
    "bool ItemVendingMachine::ProcessBuyRequestEx(CUser*, const MSG_BUY_CERASHOP_ITEM&)";

// asm 桥已清（§8 协议）：CInventory.h 现经 CGenUniqueNo.h 纳入权威完整类定义，
// GenUniqueNo 亦按 ORIG 单实参证据 static 化 ⇒ 调用点直接真实调用
// WongWork::CGenUniqueNo::GenUniqueNo（ORIG _ZN8WongWork12CGenUniqueNo11GenUniqueNoEPc）。
}

// _CheckEnableGiftCondition —— ORIG 0x08176722 (T)
#include "GlobalData/_CheckEnableGiftCondition.inc"
}

// _CheckEnableItemCondition —— ORIG 0x0817697c (T)
#include "GlobalData/_CheckEnableItemCondition.inc"
}

// _CheckEnableStateCondition —— ORIG 0x081768b0 (T)：恒真。
#include "GlobalData/_CheckEnableStateCondition.inc"
}

// _CheckEnableAvatarCondition —— ORIG 0x08176794 (T)
// ORIG 代码形态（Fix18_E 实证）：GetRelatedItem 两次调用、IsAvatarItem 前
// 无判空、out 经 stAvatarTypeSelect_t::init() 清零、四字段各自下标访问。
// [b28d-solo-GlobalData] 已修 AE 87=87（p167 10/10 CLEAR）：①首调 GetRelatedItem
// 结果 ORIG 0x081767a6 无槽往返（`call; mov (%eax),%edx; add $0x10,%edx;
// mov (%edx),%edx; mov %eax,(%esp); call *%edx`）⇒ 内联 IsAvatarItem（P0）；
// ②去 vec 具名指针（P183，ORIG 仅 equip -0x10/select -0xc 两槽）；③范围比较
// ORIG 0x0817680f `cmp -0x1c(%ebp),%al; jg` = 8 位有符号（T1 直证 char）。
#include "GlobalData/_CheckEnableAvatarCondition.inc"
}

// _GetItemString —— ORIG 0x0817645a (T)
#include "GlobalData/_GetItemString.inc"
}

// ============================================================================
// ProcessBuyRequestEx —— ORIG 0x081769f6 (T)
// SIGFIX:ItemVendingMachine::ProcessBuyRequestEx（头已 bool 化：ORIG
//   __PRETTY_FUNCTION__ 串 "bool ItemVendingMachine::ProcessBuyRequestEx
//   (CUser*, const MSG_BUY_CERASHOP_ITEM&)"，P174）⇒ 定义同步 bool。
// ============================================================================
// b28c-l23 SIGFIX：返回类型 int→bool（ORIG __PRETTY_FUNCTION__ 串 @0x08b87080，
// P174；体内 return 仅 0/1 语义）。mangled 不含返回类型，符号与 ABI 零变。
// [b28d-18 N82] 超大件：残差=槽位/STL（N82/N89） | ORIG 0x081769f6
// [b28d-solo-GlobalData] DIFF：p167 EXTRA memset×7+memcpy×4 = N78（4.1 内联清零/块搬移 vs
// 4.4 call）；大件三件套 gap 归 N82/N78，语义无缺口（MISSING 空）。
// [SweepGlobalData 真缺口修正] 旧 N78 判词错误：4 处 memcpy 与 7 处 memset 是**源形**问题，
// 非跨版本不可复现——ORIG 该段为直读直存（0x08177ed6/0x08177f87 等）与内联 rep stos
// （0x08176a7f/0x08176a9b/0x0817804f/0x081780fb）。改 `*reinterpret_cast<..*>` 直写 +
// `= {0}` value-init 后：OUR calls 139→128 = ORIG 128，MISSING 全清（仅剩 C1↔C2/D1↔D2
// 入口别名差），our_insns 1585→1614。残余 = N82/N89 槽位/STL。
// [F2a-C05② 复测·不适用] 件级 F2 旋钮（具名局部 unsigned→int 求比较内存直比）
//   逐条核对报告 14 处「ORIG 有 `cmp -0xNN(%ebp),%eax` 而我方无」的点：mem 操作数
//   绝大多数为 **call 返回值局部**（-0x6c/-0x74/-0x88 等 = getXxx() 结果槽，如
//   0x081775xx `call; cmp -0x6c(%ebp),%eax; setl`）⇒ C05② 边界外；其余为我方
//   上下文错位（difflib 在大函数上坍缩，ORIG 0x081773xx 段 ↔ 我方不同槽段，
//   属 N82/N126 全帧槽号错位）⇒ 无一处命中「两个具名局部比较」形，维持 N82 封顶。
//   无改动、无回滚（该件当前 gap −6，前述 5 处 SigTypes_E.h union 视角修复已落）。
#include "GlobalData/ProcessBuyRequestEx.inc"
}
#include "GlobalData/Secu_DataControl.inc"
#include "GlobalData/UpdateSecurityServiceState.inc"
}

#include "GlobalData/GetConnInfo.inc"
}

// [b28d-18 N82] 残差=槽位/STL 展开（N82/N89） | ORIG 0x080ec760
// [SweepGlobalData] 旧 gap 2 缺口修正：cMyTrace 两语句 → 单语句临时链
// （ORIG 0x080ec7ba..0x080ec864 五实参先于 ctor 求值，P37/P87/P329）后
// 117=117、call 序列 ratio=1.000（gap 0）；残余 = N82 槽位/寄存器。
#include "GlobalData/Connect.inc"
}

// [b28d-solo-GlobalData] 归零 67=67：group 内联为实参表达式复现 R2L 装载序
// （先 &second 后 m_tail；具名/register 局部均提前，P52/P157）；p167 CLEAR | ORIG 0x080ec8f8
#include "GlobalData/ConnectWithServerGroup.inc"
}

// ---- CDBConnector ----
#include "GlobalData/CDBConnector.inc"
}
#include "GlobalData/dtor_CDBConnector.inc"
#include "GlobalData/Init_2.inc"
}
#include "GlobalData/OpenMaster.inc"
}
#include "GlobalData/CloseMaster.inc"
}
#include "GlobalData/Close.inc"
}
#include "GlobalData/Open.inc"
}
// ---- CDBConnector::QueryConnInfo ORIG @ 0x080eca6c（U3ShopDb 批次） ----
// 用主 DB 句柄(0,0) 查 db_connect 表，逐列填 STDBConnInfo 后放入 outMap[idx]。
// 列序：db_name(0x1f→m_db) / db_ip(0x11→m_host) / db_port(→m_port) /
//       db_userid(0x15→m_user) / db_passwd（先清零 60 字节，0x3c→+0x2d 起）。
// 密文经 CEnvironment::DecryptValue 解出（ORIG 解密结果未回写结构，仅校验）。
// [b28d-18 N82] 残差=槽位/STL 展开（N82/N89） | ORIG 0x080eca6c
// [b28d-solo-GlobalData] DIFF：p167 EXTRA memset×1（N78 内联清零）；余 N82 槽位/STL 展开。
#include "GlobalData/QueryConnInfo.inc"
}
// sync_script::CSyncScript 权威定义见 sync_script.h（SetEnable/IsEnable 内联 +
// GetInstanceSyncScript 单例访问器）；本 TU 不再提供本地镜像。

// ---- 事件/GM/统计 ----
// CEventManager 构造/析构已移入 CEventManager.cpp（独立 TU）。
#include "GlobalData/CGM_Manager.inc"
}
#include "GlobalData/dtor_CGM_Manager.inc"

}
// b28c-l23 SIGFIX：返回类型 char→bool（ORIG __PRETTY_FUNCTION__ 串 + 体尾
// `movzbl -0x11(%ebp),%ebx` 直返 bool 局部；char 声明使 4.4 出 movsbl）。
// [b28d-solo-GlobalData] 已修 AE（p167 17/17 CLEAR）：ORIG 0x08299530 三处
// cMyTrace 各占独立槽（`lea -0x44/-0x34/-0x24` 三组互异，4.1 不复用），具名
// 对象在 4.4 下复用同槽致全帧浅 0x30（-0x48/-0x4c vs -0x18/-0x1c）⇒ 按 P221
// 改「构造即调用」全表达式临时链 `cMyTrace(__PRETTY_FUNCTION__,line,0)(fmt)`，
// 三处独立槽与 ORIG 逐条对齐（余重定位显示伪差，relink 后 AE）。
#include "GlobalData/LoadGmListFromDb.inc"
}
// ORIG T 0x0860ecc6 IncreaseLevelDungeonPlay(level, playCount)：
// level ∉ [1,200] → 仅 trace(type=0)；否则 find 命中 → 引用缓存后
// m_playCount += playCount、++m_userCount；未命中 → 先写 playCount 再置
// userCount=1，insert。
// [b28d-18 NEAR] 修：删发明符号 stDungeonPlayInfo::C1Ev（ORIG nm 无，POD 化）；p167 CLEAR；余 N80 | ORIG 0x0860ecc6
#include "GlobalData/IncreaseLevelDungeonPlay.inc"
}

// ---- 后续批次业务模块 ----
#include "GlobalData/CBreakAwayPreventSystem.inc"
}

// ORIG T 0x0831c486 [0x0831c486, 0x0831c49d]：仅析构 +0x2e4 的
// m_rewardMap（std::map<STRewardDataKey,STRewardDataList>），成员自动析构等价。
#include "GlobalData/dtor_CBreakAwayPreventSystem.inc"
}
// ORIG T 0x0831c49e（loadDbData.md 全量转写）：拷贝 SIG 基类区 0x2e4，
// 清 map 后按条目组装：条目 stride 0x18，key=+0x10/+0x11/+0x12 三字节，
// value=+0x14..+0x24 五个 int（STRewardData）。key 已存在 → list push_back；
// 否则新建 list 插入 map。同时维护 m_keys（5 项）。
// [b28d-18 N82] p167 CLEAR（21/21）；ORIG 243 vs 203 条，残差=槽位/STL（N82/N89） | ORIG 0x0831c49e
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（21/21）；gap 40=N82 帧/槽 + 表初始化展开。
// [b28d-tu-GlobalData] **gap 40→0 归位**（双判据真缺口，旧「N82 帧/槽」判词作废）：
//   ① 删 `entry` 缓存 ⇒ 每字段自形参+槽(i) 完整重物化（ORIG 0831c50b..5c9 七段
//     10 行/字段形实证，P48 无引用缓存族）；② P334 第二形：`STRewardData reward;`
//     声明上移至 key 赋值前（ORIG 循环体首连发 key+reward 两 ctor）。
//   现 243/243 gap 0、P167 21/21 CLEAR 保持、TU 四数不回退。残 ndiff 196/struct 145
//   = ORIG 把 key 三字节读折成 `add $0x10` 基 + disp(+0/+1/+2) 复用（4.1 表达式
//   分片）而 4.4 恒 `add $0x11; lea` 前置合常（装载形别名，非缺口）+ prologue
//   count 装载位（ORIG -0x10 先清后 memcpy，我方 count store 在 clear 后）+
//   循环尾 setg/setl 极性等价别名 ⇒ 判 N（形态）。
#include "GlobalData/loadDbData.inc"
}
// （b28 试 P216 throw() 回滚：4.4 判 ctor pure 后连 call 一起消除，ORIG 有 call，
//  得不偿失；CExpertJobMgr::C1 残差为 N77/P90 value-init 清零段。）
// [b28d-solo-GlobalData] P299/P318：CExpertJobFactory 用户空 ctor 声明+定义已删
// （ORIG 0x0849f68c 为 W 隐式合成）⇒ CExpertJobMgr::C1 恢复 value-init 前置 rep stos。
#include "GlobalData/dtor_CExpertJobFactory.inc"
}
// b28c-1 N77: 4.1 new T() value-init 前置 rep stos 0x18 dword，4.4 不 emit（去具名 p 后 15→13）。
// [b28d-18 N77] ORIG new(nothrow) T() value-init 前置 rep stos 0x18 dword + 临时指针槽；4.4 按 CWG434 不 emit（N77/P90） | ORIG 0x0849e102
#include "GlobalData/CExpertJobMgr.inc"
}
#include "GlobalData/dtor_CExpertJobMgr.inc"
}
// CVillageObjectMgr ctor/dtor 现由 CVillageObjectMgr.cpp 提供
//（ORIG 0x86b6f30 默认构造 std::map；dtor 0x86b6f44 调用 map dtor）。
// CVillageMonsterMgr ctor/dtor 现由 CVillageMonsterMgr.cpp 提供（LinkVillageSolo）。
#include "GlobalData/CRevengeDungeon.inc"
}
#include "GlobalData/IsOpenRevengeDungeon.inc"
}
#include "GlobalData/GetCloseTime.inc"
}
#include "GlobalData/OpenRevengeDungeon.inc"
}
#include "GlobalData/CloseRevengeDungeon.inc"
}
#include "GlobalData/SetCloseTime.inc"
}
#include "GlobalData/InstanceRentalSystem.inc"
}
// [b28d-18 IDENTICAL_AE] 68=68；唯一残差=尾对齐 nop（§6.1 豁免） | ORIG 0x08270110
#include "GlobalData/dtor_InstanceRentalSystem.inc"
}
#include "GlobalData/registerRentalMachine.inc"
}

#include "GlobalData/RentalMachinePcRoom.inc"
}
#include "GlobalData/initLoggingEnvironment.inc"
}

// [b28d-18 IDENTICAL_AE] 21=21 逐条同形 | ORIG 0x0826efdc
#include "GlobalData/initLoggingEnvironment_1.inc"
}

// ---- IRentalMachine 槽实现（PcRoom / Dimension 同构，逐报告转写）----

// PcRoom::Ready（ORIG 0x0827073e）：尾调 checkAllRentalItemScript()。
#include "GlobalData/Ready.inc"
}
// Dimension::Ready（ORIG 0x0826ee94）：同构。
#include "GlobalData/Ready_1.inc"
}

// PcRoom::CheckRentableItem（ORIG 0x08270752）：
// return m_impl->m_set.find(itemIdx) != m_impl->m_set.end()。
#include "GlobalData/CheckRentableItem.inc"
}
// Dimension::CheckRentableItem（ORIG 0x0826eea8）：同构。
// [b28d-18 IDENTICAL_AE] 32=32；唯一残差=尾对齐 nop（§6.1 豁免） | ORIG 0x0826eea8
#include "GlobalData/CheckRentableItem_1.inc"
}

// ---- InstanceRentalSystem 查询/分发（ORIG 报告 InstanceRentalSystem/*.md）----

// getMachine（ORIG T 0x08270348）：map find，未命中返回 0。
// [b28d-18 IDENTICAL_AE] 38=38 逐条同形（.o 重定位伪差） | ORIG 0x08270348
#include "GlobalData/getMachine.inc"
}

// Ready（ORIG T 0x082701dc）：遍历机器表；任一为空或其槽3 Ready() 返回非 1
// 即返回 false，全部就绪（含空表）返回 true。
// [b28 判词 N7] 残差=call *%edx↔%eax 换名+对调，主体 56 行同形封顶。
// [b28d-18 IDENTICAL_AE] 56=56 逐条同形；残差仅间接 call 寄存器名（N7 已判） | ORIG 0x082701dc
#include "GlobalData/Ready_2.inc"
}

// Rent 分发（ORIG T 0x08270288）：按 RentInput.m_machineKey 取机器并经槽2
// 调用；无机器时 ORIG 直接返回 1。
// [b28 判词 N7] 残差=call *%edx↔%eax 换名+实参链寄存对调，29 行主体同形封顶。
#include "GlobalData/Rent.inc"
}

#include "GlobalData/CDailyScheduleManager.inc"
#include "GlobalData/dtor_CDailyScheduleManager.inc"
#include "GlobalData/dtor_CTeamFactory.inc"
}
// [b28 判词 N77] ORIG 4.1 `new (nothrow) T()` value-init 前置 rep stos 12 dword
// 清零+临时槽；4.4 CWG434 不 emit——placement/显式 memset 变 call 均不可翻，封顶。
// [b28d-18 N77] ORIG new T() value-init 前置 rep stos + 临时指针槽；4.4 不 emit（N77/P90） | ORIG 0x0858860c
#include "GlobalData/COnlinePreliminaryTeamMgr.inc"
}
// ORIG T 0x085886fe：m_factory 非空时 delete（CTeamFactory D1 + operator delete）；
// m_teamMap@+0x04 由编译器析构（ORIG 尾调 map D1）。
#include "GlobalData/dtor_COnlinePreliminaryTeamMgr.inc"
}
// ORIG T 0x08588be4（OnLoadTeamInfo.md 全量转写）：data 首 4 字节为队伍 key，
// 命中 m_teamMap 时委托 loadDataPreliminaryTeamMatchList(data)。
#include "GlobalData/OnLoadTeamInfo.inc"
}
// game_master::CCommandMap / CGameMasterMgr 真实实现见 GmCore.cpp 与 GmRegister.cpp
//（ORIG CCommandMap ctor 0x082a7f64、add(int) 0x082a7c76、add(string) 0x082a7cdc、
// Mgr ctor 注册链 0x082a8a28）。

// ---- IBusiness / CBusinessImpl（报告 docs/class_func_reports/CBusinessImpl/）----

// P210 附注（b28 BL14 翻案，Main 2026-09-08 广播）：派生 ctor 定义前置到同
// TU 基类 ctor 之前 ⇒ 4.4 对该 callee 不可见、恢复 ORIG 保守 outgoing
// `sub $0x18`（0x082a7c3b），旧「帧舍入 N + dead pad」判词作废（pad 删）。
#include "GlobalData/CBusinessImpl.inc"
}

#include "GlobalData/dtor_CBusinessImpl.inc"
}

#include "GlobalData/IBusiness.inc"
}

// [b28d-solo-GlobalData] 改善 74→68（p167 12/12 CLEAR）：P259 去 stream 具名
// 局部（ORIG 0x080de3f6 `call Acquire; movl $1,0x8(%esp); mov %eax,0x4(%esp)`）+
// info 前 8 字节结构整体赋值（ORIG 0x080de46c `mov 0x4(%eax),%edx; mov (%eax),%eax`
// 先高字后低字）+ body.field04 1 字节（ORIG 0x080de469 `mov %al,-0x1c`，T1 直证）。
// 残差（已闭）：ORIG 尾部 `mov $0x0,%ebx … mov %ebx,%eax` ⇒ 原函数返回 int 0。
// [b28d-PoolGlobalData] R2 落地：CBusinessImpl.h:34/48 void→int + `return 0`
// ⇒ 0x080de3ee ndiff 19→0（AE）。
#include "GlobalData/PunishClient.inc"
}

#include "GlobalData/ProcessProtectMsg.inc"
}

// ---- DB 请求（ORIG 模式：Acquire 流 → CStreamGuard → <<msgId → <<-1 → 入 DB 队列）----
#include "GlobalData/makeRequest.inc"
}

#include "GlobalData/makeRequest_1.inc"
}

#include "GlobalData/makeRequest_2.inc"
}

#include "GlobalData/makeRequest_3.inc"
}

// ---- 杂项对象 ----
#include "GlobalData/Action.inc"
}
#include "GlobalData/proc_bingo.inc"
}
// [b28d-18 NEAR] 修：getCurCharacFatigue 两次独立调用（P48 无 CSE，p167 MISSING 实证）；
// 调用差集 CLEAR 8/8；余槽位/寄存器（N82） | ORIG 0x0827fbd6
// [b28d-solo-GlobalData] NEAR 73=73（p167 8/8 CLEAR）：P0 具名 user 局部
// （ORIG -0x10 三次复用）+ P183 去 costs 指针缓存（每次重发 item+0x3a0）后
// 由 74 条收敛；余 1 处槽互换（ORIG lower -0x14 / max 常量临时 -0x1c vs
// 4.4 -0x1c/-0x14，P212 头部声明/前置声明三形实测均反劣）⇒ N98 封顶。
#include "GlobalData/proc_addFatigue.inc"
}
#include "GlobalData/proc_lotteryCount.inc"
}

// [b28d-18 N82] 残差=槽位/STL 展开（N82/N89） | ORIG 0x0827f7c8
#include "GlobalData/init_function_type.inc"
}

// [b28d-18 IDENTICAL_AE] 修：register 指针 pt=&t 使 &t 提前物化 %esi 跨 operator new 保活（P231 新实证）；NDIFF 4→1（余 vtable 重定位伪差） | ORIG 0x084a2938
#include "GlobalData/CExtraction.inc"
}
#include "GlobalData/dtor_CExtraction.inc"
}
// [b28d-solo-GlobalData] P299/P318：三个子类用户空 ctor 声明+定义已删（ORIG C1/C2 均 W）
// ⇒ CExtractionMgr::C1 的 `new T()` 恢复 value-init 前置双清零（ORIG 0x084a3047/0x084a308f）。
#include "GlobalData/dtor_CAlchemistExtraction.inc"
#include "GlobalData/dtor_CEnchanterExtraction.inc"
#include "GlobalData/dtor_CDollControllerExtraction.inc"
#include "GlobalData/CExtractionMgr.inc"
}
#include "GlobalData/dtor_CExtractionMgr.inc"
}
// ---- Teleport / NPC_Teleport / User_Teleport（报告 Teleport/、NPC_Teleport/、User_Teleport/）----

#include "GlobalData/STCheckRuleData.inc"
#include "GlobalData/STTeleportData.inc"
}

#include "GlobalData/Teleport.inc"
}
#include "GlobalData/dtor_Teleport.inc"
}
#include "GlobalData/check_rule.inc"
}
#include "GlobalData/set_party_teleport.inc"
}
#include "GlobalData/get_party_teleport.inc"
}
#include "GlobalData/set_end_cool_time.inc"
}
#include "GlobalData/get_end_cool_time.inc"
}
#include "GlobalData/update_next_end_cool_time.inc"
}
#include "GlobalData/send_teleport_effect.inc"
}
#include "GlobalData/move_area.inc"
}
#include "GlobalData/NPC_Teleport.inc"
}
#include "GlobalData/dtor_NPC_Teleport.inc"
#include "GlobalData/check_rule_1.inc"
}


#include "GlobalData/check_rule_2.inc"
}
#include "GlobalData/set_teleport_data.inc"
}

#include "GlobalData/User_Teleport.inc"
}
#include "GlobalData/dtor_User_Teleport.inc"
#include "GlobalData/check_rule_3.inc"
}
#include "GlobalData/set_teleport_data_1.inc"
}
#include "GlobalData/GetRandomOption.inc"
}
// [b28d-18 N7] 修：register seed 使 seed 先载（P231）；余纯寄存器名轮换（N7 封顶） | ORIG 0x085f0d64
// [b28d-solo-GlobalData] NEAR 封顶 N7：p167 逐调用同形（imul/lea/seed 段一致）；
// 残差=callee-saved 角色轮换（ORIG eax/edx/ecx vs 4.4 ebx/eax/edx，P230 定稿：
// 4.4.7 分配固定，源形不可择）+ 首条 vptr 存的重定位显示伪差（`$&<L>` vs `<L>`）。
#include "GlobalData/CRandomOptionItemHandle.inc"
}
#include "GlobalData/dtor_CRandomOptionItemHandle.inc"
}

// [b28d-18 N] ORIG 切片缺失（objdump 直转 082a73cb）；p167 CLEAR（9/9）；残差=表内容占位+codegen | ORIG 0x082a73cb
// [b28d-solo-GlobalData] DIFF 封顶 N：ORIG 人证字符串表（.data 0x936e2a0，64 项）内容缺失，
// 重建以全 0 占位必须保留 s==0||*s==0 检查（否则 std::string(0) 崩溃），ORIG 无检查 ⇒ +10 条；
// p167 CLEAR（表访问调用同形）。
#include "GlobalData/init_1.inc"
}

void* CreateAntiServerObj(const char* path)
{
    // ORIG W 0x082a6539：path + "libantisvrimport.so" → dlopen(2) →
    // dlsym("GetAntiBotInterfaceAll") → 无参调用并返回结果。
    // buf 基址实证：ORIG 0x082a654f `lea -0x118(%ebp),%ebx` 且帧 `add $0x120,%esp`
    // ⇒ buf 尺寸 0x104（实测：0x100 落 -0x114、0x104 才落 -0x118；0x118 落 -0x12c）。
    // 尾部 4 字节是 4.1 给 handle/err 之外多留的一槽，无读写语义。
    // [b28c-l23 真语义 bug 修正] 原写法 `memset(buf, 0, 0x41)` 只清 0x41 **字节**，
    // 但 ORIG 0x082a654f 是 `mov $0x0,%eax; mov $0x41,%edx; mov %ebx,%edi;
    // mov %edx,%ecx; rep stos %eax,%es:(%edi)` —— rep stos 的立即数是 **dword 计数**
    // （P116），0x41×4 = 0x104 = 整个 buf ⇒ 原码清空的是全缓冲，strncat 依赖
    // buf[0]=='\0' 才不从脏数据起接。改为显式零初始化（P214③：大数组 `={0}`
    // 是 4.1 rep stos 的 4.4 复现突破口），语义与形态同源修正。
    char buf[0x104] = {0};
    strncat(buf, path, strlen(path));
    strncat(buf, "libantisvrimport.so", 0x14);
    void* handle = dlopen(buf, RTLD_NOW);
    const char* err = dlerror();
    if (err != 0)
    {
        // 语义修正：ORIG 0x082a65e1 `movl $0x8c16189,0x4(%esp)`（.rodata 实测
        // 0x8c16189 = "libantisvrimport.so"）+ 0x082a65e9 fmt@0x8c1619d ⇒
        // 原码第 2 参是**库名字面量**，不是拼接后的 buf（buf 只用于 dlopen）。
        printf("Open Dll %s failed, %s\n", "libantisvrimport.so", err);
        return 0;
    }
    void* fn = dlsym(handle, "GetAntiBotInterfaceAll");
    err = dlerror();
    if (err != 0)
    {
        dlclose(handle);
        printf("dlsym failed, %s\n", err);
        return 0;
    }
    return ((void* (*)())fn)();
}

// ---- CDBEnviroment ----
// ORIG W 0x082aa8e4：CEnvironment::CEnvironment() + m_dbType(+0x3e0)=0。
#include "GlobalData/CDBEnviroment.inc"
}

// ORIG W 0x082aa90e：vptr 复位 + CEnvironment::~CEnvironment()。
#include "GlobalData/dtor_CDBEnviroment.inc"
}

// ---- CDBEnviroment::parse ORIG @0x0829d8e8（GameDbType 环境脚本解析）----
// strtok(buf, "=") 取两 token 并 trim；首 token 为 "game_db_type" 时按第二
// token 文字映射 m_dbType（cain=1/diregie=2/siroco=3/prey=4/casillas=5/
// hilder=6/ruke=7/seria=8/anton=9/roxy=0xa/bakal=0xb/ozma=0xc/khazan=0xd/
// first1=0x63/first2=0x64/starter_server=0x11），并把空 map 存入
// G_CEnvironment()->m_serverGroupMap[m_dbType]；未知值 cMyTrace(0x2032,5)
// "Unknown value (%s)" 返 0。否则在 m_serverGroupMap[m_dbType] 中查找，
// 命中则 make_pair(p1,p2) insert。恒返 1。
// m_serverGroupMap 为 CEnvironment private，用 +0x50 偏移组件视图访问（P1）。
struct CDBEnvServerGroupMapView
{
    char m_pad[0x50];
    std::map<ENUM_SERVER_GROUP, std::map<std::string, std::string> > m_map;
};

// [b28d-18 DIFF] p167 MISSING G_CEnvironment×1（真缺口未闭合）；余槽位（N82） | ORIG 0x0829d8e8
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（42/42）；残差=N82 槽位/寄存器 + 行解析展开。
// [SweepGlobalData] trim 返回值丢弃（ORIG 0x0829d928/0x0829d933 无回写）后 289 vs 283；
// 残余 span 差 = N18a/P211 装载序（G_CEnvironment/make_pair 段各 1~4 条）。
#include "GlobalData/parse.inc"
}

// ============================================================================
// 静态成员定义（平凡/指针；对象型成员由全局 TU 定义，避免单 TU 静态初始化）
// ============================================================================

bool GlobalData::s_server_stop = false;
std::map<std::string, std::vector<int> > CHumanCertifier::humanCertifyTable_;  // ORIG B 0x93f5758
CSystemTime GlobalData::s_systemTime_;
CaptchaGen GlobalData::cg_;
MsgQueueMgr* GlobalData::s_msgq_mgr = 0;
StreamPool* GlobalData::s_stream_pool = 0;
PacketPool* GlobalData::s_packet_pool_list[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
BigStreamPool* GlobalData::s_big_stream_pool = 0;
SmallStreamPool* GlobalData::s_small_stream_pool = 0;
DBMgr* GlobalData::s_db_mgr = 0;
DBThread* GlobalData::s_db_thr = 0;
DBThread* GlobalData::s_db_thr_for_logDB = 0;
DispatchThread* GlobalData::s_dispatch_thr = 0;
NetworkThread* GlobalData::s_net_thr = 0;
UdpThread* GlobalData::s_udp_thr = 0;
AcceptThread* GlobalData::s_accept_thr = 0;
MonitorTcpThread* GlobalData::s_monitor_tcp_thr = 0;
CServerProxyMgr<CMonitorServerProxy>* GlobalData::s_monitor_proxy_mgr = 0;
CServerProxyMgr<CGuildServerProxy>* GlobalData::s_guild_proxy_mgr = 0;
CServerProxyMgr<CStatisticServerProxy>* GlobalData::s_statistic_proxy_mgr = 0;
CHadesServerProxy* GlobalData::s_hades_proxy = 0;
CDoubleConnCheckServerProxy* GlobalData::s_double_check_proxy = 0;
// 对象型成员（ORIG 地址：s_accept_lock @0940bdcc (0x18)、s_accept_que @0940be00 (0x28)）；
// 定义置于本 TU（GlobalData.h 注释的"全局 TU"即本文件，s_systemTime_/cg_ 同区）。
Mutex GlobalData::s_accept_lock;
std::queue<CUser*, std::deque<CUser*> > GlobalData::s_accept_que;
CSchoolServerProxy* GlobalData::s_school_proxy = 0;
CPCRoomServerProxy* GlobalData::s_pcroom_proxy = 0;
CCommunityServerProxy* GlobalData::s_community_proxy = 0;
CAuctionServerProxy* GlobalData::s_auction_proxy = 0;
CCeraAuctionServerProxy* GlobalData::s_cera_auction_proxy = 0;
CPowerManager* GlobalData::s_power_manager = 0;
secretshop::CSecretShop* GlobalData::s_secret_shop = 0;
CFatigueBatteryHandle* GlobalData::s_fatigueBatteryHandle_ = 0;
CmmChannelProxy* GlobalData::s_channel_proxy = 0;
CRijndael* GlobalData::s_pcryptRijndael_CharacView_ = 0;
CRijndael* GlobalData::s_pcryptRijndael_Password_ = 0;
CRijndael* GlobalData::s_pcryptRijndael_MousePassword_ = 0;
GlobalData::SecuCipherKeyRec GlobalData::s_secu_cipher_keystring_manager_[0x4e20] = {0};
CTEA* GlobalData::s_pcryptTEA_MousePassword_ = 0;
CTEA* GlobalData::s_pcryptTEA_SecurityCard_ = 0;
// [b22判词·N-工具名碰撞+编译器生成] idx9 __tcf_0 是本 TU 静态数组 g_SPremiumInfo[0x6a]
// 的编译器自动生成析构 thunk（倒序逐元素 call ~SPremiumInfo，elem size 0x120），非手写
// 函数，无法在源码层「改」其指令。真·ORIG 对应物 = 0x086b068d（紧邻 _GLOBAL__I_
// g_SPremiumInfo@0x86b080a，call _ZN12SPremiumInfoD1Ev@0x86b1732），其循环骨架与本 .o
// 逐项同构（mov 数组上界→%ebx; 与基址 cmp; je 出口; sub $0x120; mov; call dtor; jmp）。
// report=NO/DIFF（orig18/our15）是 game_func_report 按短名碰撞：ORIG 存在 10 个 file-local
// `__tcf_0`，工具取到 0x0828f940（CDeathTowerRanking 数组 cleanup，语义无关）。残余：
// ①ORIG 用字面上界地址 0x948b180 / 字面基址 0x9483a40 做 cmp，我们靠链接期 R_386_32
// reloc 填（未 relink 显示占位 0，§6.3 假阳性）；②18 vs 15 差在 ORIG 额外一条 `mov 上界,%ebx`
// 预载。语义等价，relink 后转 AE。
SPremiumInfo g_SPremiumInfo[0x6a];             // 09483a40；全局 C 名（无命名空间，ORIG B）；静态初始化逐元素构造（ORIG _GLOBAL__I_g_SPremiumInfo 0x86b080a）
WongWork::CSimpleSSO* GlobalData::s_psimpleSSO = 0;
WongWork::CSecurityCardCenter* GlobalData::s_securityCardCenter = 0;
WongWork::CGMAccounts* GlobalData::s_pGMAccounts_ = 0;
CEventManager* GlobalData::s_event_manager = 0;
WongWork::IPG::CIPGHelper* GlobalData::s_pIPGHelper = 0;
InstanceRentalSystem* GlobalData::s_rentalSystem = 0;
RefPvpGrade* GlobalData::s_ref_pvp_grade = 0;
WongWork::CLogGameChannel* GlobalData::s_pLogGameChannel = 0;
WongWork::CCeraShop* GlobalData::s_pCeraShop = 0;
CDBConnector* GlobalData::s_db_connector = 0;
void* GlobalData::s_pCompoundAvatar = 0;
void* GlobalData::s_pCompoundBindCube = 0;
void* GlobalData::s_pTowerOfDespairMgr = 0;
Event_Upgrade_Jar* GlobalData::s_pEvent_UpGrade_Jar = 0;
CGM_Manager* GlobalData::s_GM_Manager = 0;
ItemVendingMachine* GlobalData::s_pItemVendingMachine = 0;
CStatisticMgr* GlobalData::s_statisticMgr = 0;
expert_job::CExpertJobMgr* GlobalData::s_ExpertJobMgr = 0;
break_away_prevent::CBreakAwayPreventSystem* GlobalData::s_BreakAwaySys = 0;
Gm_List* GlobalData::gmList = 0;
village_object::CVillageObjectMgr* GlobalData::s_villageObjectMgr = 0;
game_master::CGameMasterMgr* GlobalData::s_GameMasterMgr = 0;
village_attacked::CVillageMonsterMgr* GlobalData::s_villageMonsterMgr = 0;
village_attacked::CRevengeDungeon* GlobalData::s_revengeDungeonMgr = 0;
CDailyScheduleManager* GlobalData::s_DailyScheduleManager = 0;
online_preliminary::COnlinePreliminaryTeamMgr* GlobalData::s_onlinePreliminaryTeamMgr = 0;
Secu_DataControl* GlobalData::s_pSecuDataControl = 0;
CSecu_ProtectionField* GlobalData::s_pSecuProtectionField = 0;
WongWork::CAutoPunishRuleBlackIPMgr* GlobalData::g_autoPunishRuleBlackIPMgr = 0;
WongWork::CAutoPunishRuleHackTypeMgr* GlobalData::g_autoPunishRuleHackTypeMgr = 0;
WongWork::CBlackIPMonitor* GlobalData::g_blackIPMonitor = 0;
unsigned int GlobalData::g_IPMonitorLastDBAccessTime = 0;
CBusinessImpl* GlobalData::s_antibotBusinessImpl_ = 0;
void* GlobalData::s_antibotChecker_ = 0;
CItemAmplifier* GlobalData::s_itemAmplifier_ = 0;
CGlobalEffectManager* GlobalData::s_globalEffectManager_ = 0;
ARAD::Arad_ServerStateManager* GlobalData::s_serverStateManager_ = 0;
CTimeGate* GlobalData::s_timeGate_ = 0;
unsigned int GlobalData::g_GoldControlDateFlag = 0;
StackableAction::Action* GlobalData::g_Actions = 0;
expert_extraction::CExtractionMgr* GlobalData::g_ExtractionMgr = 0;
void* GlobalData::g_pTeleport[2] = {0, 0};
random_option::CRandomOptionItemHandle* GlobalData::g_randomOptionHandle = 0;
void* GlobalData::s_pAvatarRoulette = 0;

// ============================================================================
// GlobalData::setCurrentPacketPool
// ============================================================================

#include "GlobalData/setCurrentPacketPool.inc"
}

// ============================================================================
// GlobalData::InitCaptcha
// ============================================================================

#include "GlobalData/InitCaptcha.inc"
}

// ============================================================================
// GlobalData::GenerateCaptchaData
// ============================================================================

#include "GlobalData/GenerateCaptchaData.inc"
}

// ============================================================================
// GlobalData::Fini
// ============================================================================

// [b28d-solo-GlobalData] 真 bug 修正（SweepGlobalData）：7 个线程对象 ORIG 为
// **delete**（多态 deleting-dtor 虚调用 `mov (%eax),%eax; add $0x8,%eax;
// call *%edx`；ThreadInterface vtable @0x8ce6b90 = [stop@vptr+0, D1@+4,
// D0@+8, pure_virtual@+0xc] 实测），旧重建误写 `->stop()`（vptr+0 虚调，
// 既不析构也不释放）⇒ 每处少 1 条指令，共 gap 7。
#include "GlobalData/Fini.inc"
}

// ============================================================================
// GlobalData::initTournamentDB
// ============================================================================

// [b28d-18 N82] p167 CLEAR（36/36）；残差=槽位分配（ORIG -0x44/-0x48/-0x50 vs 4.4 -0x30/-0x18/-0x20） | ORIG 0x0829dd82
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（36/36）；残差=N82 槽位/寄存器 + N89 STL 展开。
#include "GlobalData/initTournamentDB.inc"
}

// ============================================================================
// GlobalData::Init
// ============================================================================

// [b28d-18 N89] 超大件：残差=STL 实例展开差（N89）；§6.2 语义完成度按调用差集+gap 计 | ORIG 0x08299fa0
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（大件 3139 insn）；N89（ORIG out-of-line STL
// 实例 vs 4.4 全内联）+ N82 槽位；语义完成，byte 层 N 封顶。
// [b28d-BigGlobalInit] 真缺口修复（gap +16→0，3139=3139，四数 894/1781/8/52 零回退）：
//   ① CStatisticMgr 两处 new(nothrow) 缺 value-init 前置清零 —— 删用户默认 ctor
//      （ORIG nm `W _ZN13CStatisticMgrC1Ev` 0x45 = 隐式合成）⇒ ORIG 0x829c5bc 的
//      `rep stos 0xc dwords` 复现（P299/CWG434）；
//   ② antibot init 调用缺 vptr 两级解引用 + 第二实参两段装载 —— 改
//      `(*(AntibotInitFn**)A)[0](A,(int)B,seq)`（P265/P211 档②：int 形参触发 4.4 中转）；
//   ③ CFatigueBatteryHandle 的臆造 `char m_pad[8]` 使 nothrow new 分配 8B 而 ORIG 为 1B
//      （0x829b033 `movl $0x1,(%esp)`）—— 删 pad，空类 sizeof=1（头改动已全仓广播+6 TU 复编）。
//   残余 = 1 条 ORIG 0x829cd78 `jmp`（ORIG 失败块无任何入口=死块，4.4 生成 `je 成功`，d=0 等长）
//   + N82/P112 槽位差（帧 0x79c vs 0x78c）；p167 CLEAR（565=565，差集全 C1/C2 同址别名）。
#include "GlobalData/Init_3.inc"
}

// ###########################################################################
// # 租赁域（RandomRentalA2 批次）：pc_room 自由函数 + 两台租赁机器本体 +      #
// # InstanceRentalSystem::{RentalInfo,DeletedRentItem} C1 + CUser 别名       #
// # 全部逐报告转写；无报告者标注"报告缺失，objdump 直转"。                    #
// ###########################################################################

// ---- InstanceRentalSystem::RentalInfo C1 —— W 0x082719f6 ------------------
// （报告缺失，objdump 直转：清 +0 字/+2 半/+4 int/+8..+b 四字节，+0xc=-1）
#include "GlobalData/RentalInfo.inc"
}

// ---- InstanceRentalSystem::DeletedRentItem C1 —— W 0x08271a3a -------------
// （报告缺失，objdump 直转：清 +0 字节/+2 半字/+4 int）
#include "GlobalData/DeletedRentItem.inc"
}
// ---- InstanceRentalSystem::RentInput::RentInElem C1 —— W 0x0822f02e ------
// （objdump 直转：逐字节清 +0..+3，+4=-1；无帧无栈，每字段重读 this）
#include "GlobalData/RentInElem.inc"
}

// ---- InstanceRentalSystem::RentInput C1 —— W 0x0822f058 ------------------
// （objdump 直转：vector C1 后清 +0xc int、+0x10/+0x11 两字节）
#include "GlobalData/RentInput.inc"
}

// ---- CUser::GetCurrentCharacRentalInfo —— T 0x08677166 --------------------
// （GetCurrentCharacRentalInfo.md：尾调小写版 getCurrentCharacRentalInfo）
#include "GlobalData/GetCurrentCharacRentalInfo.inc"
}

// makeFairPVPEquipmentPackageKey —— T 0x088945ad（InitScriptProducers_2.cpp 定义；
// DataManagerInitDepends.h 与本 TU 本地类冲突，故此处仅前向声明）
#include "GlobalData/makeFairPVPEquipmentPackageKey.inc"

namespace pc_room
{

// ---- IsPcRoomUser —— T 0x08270fea（IsPcRoomUser.md）------------------------
#include "GlobalData/IsPcRoomUser.inc"
}

// ---- CheckRentableEnvironment —— T 0x08271002（同名报告）-------------------
#include "GlobalData/CheckRentableEnvironment.inc"
}

// ---- CheckRentableEnvironmentAtChannelType —— T 0x08271032（同名报告）------
#include "GlobalData/CheckRentableEnvironmentAtChannelType.inc"
}

// ---- GetSelectionIndex —— T 0x082711f7（同名报告全文转写）------------------
EventRewardSection* GetRewardSection(int idx, int type);   // 定义见下方
#include "GlobalData/GetSelectionIndex.inc"
}

// ---- GetCharacterJob —— T 0x082712a6（同名报告全文转写）--------------------
#include "GlobalData/GetCharacterJob.inc"
}

}  // namespace pc_room

// ---- PcBangItemRentarData::getSectionData —— T 0x08915518（同名报告）-------
#include "GlobalData/getSectionData.inc"
}

// ---- DimensionActivationRewardData 三查询（同名报告全文转写）---------------
#include "GlobalData/getSectionData_1.inc"
}

std::vector<std::vector<EventRewardItemInfo> >*
#include "GlobalData/getRewardSelectItemList.inc"
}

std::vector<std::vector<EventRewardItemInfo> >*
#include "GlobalData/getRewardGiveItemList.inc"
}

std::vector<std::vector<EventRewardItemInfo> >*
#include "GlobalData/getRewardSelectionList.inc"
}

namespace pc_room
{

// ---- GetRewardItemSelectionList —— T 0x082712da（同名报告全文转写）--------
std::vector<std::vector<EventRewardItemInfo> >*
GetRewardItemSelectionList(int packageKey, register int type)
{
    // ORIG 0x082712da：同 GetRewardSection 三分支跳链。
    switch (type)
    {
    case 1: {
        PcBangItemRentarData& rentar = G_CDataManager()->m_pcBangRentar;
        return rentar.getRewardSelectionList(packageKey);
    }
    case 2: {
        DimensionActivationRewardData& reward =
            G_CDataManager()->m_dimensionAct.m_rewardData;
        return reward.getRewardSelectItemList(packageKey);
    }
    default:
        return 0;
    }
}

// ---- GetRewardSection —— T 0x08271338（同名报告全文转写）------------------
EventRewardSection* GetRewardSection(int idx, register int type)
{
    // ORIG 0x08271338：cmp $1/je + cmp $2/je + jmp default 三分支跳链。
    switch (type)
    {
    case 1: {
        PcBangItemRentarData& rentar = G_CDataManager()->m_pcBangRentar;
        return rentar.getSectionData(idx);
    }
    case 2: {
        DimensionActivationRewardData& reward =
            G_CDataManager()->m_dimensionAct.m_rewardData;
        return reward.getSectionData(idx);
    }
    default:
        return 0;
    }
}

// ---- GetRentItemInfo —— T 0x08271396（同名报告全文转写）--------------------
// [b28d-18 NEAR] 修：委托 pc_room::GetRewardItemSelectionList(packageKey,type)（p167 MISSING）；CLEAR 9/9；余槽位 | ORIG 0x08271396
// [b28d-solo-GlobalData] 归零 AE 88=88（p167 9/9 CLEAR）：ORIG 0x08271444 族为
// `cmpb $0,-0x2c; js F1` + `movsbl; cmp -0x14; jl OK1`（失败块 F1 居中、成功体
// jl 直落）⇒ 源码须写负极性短路 `if (a < 0 || size <= a) return false;` + 体直落；
// 旧正条件包裹 `if (a>=0 && size>a) {…} return false` 产 jge 反形 + 尾部常量岛。
#include "GlobalData/GetRentItemInfo.inc"
}

// ---- CheckInventoryEquipmentEmptyCount —— T 0x08271062（同名报告全文转写）--
// ORIG 0x08271062：call; leave; ret 穿透。内外配对律（batch-13）：callee
// check_empty_count 终裁 bool ⇒ 本外层同步 bool（probe3 实证 bool+bool 直穿无
// movzbl；int 外 + bool 内必产 movzbl 归一段）。
#include "GlobalData/CheckInventoryEquipmentEmptyCount.inc"
}

// ---- CheckInventoryEmptyCount —— T 0x0827108c（同名报告全文转写）-----------
// [b28d-solo-GlobalData] 改善 103→96（p167 16/16 CLEAR）：P63 共享出口改写为
// 嵌套正向 if 包裹（消 4.4 每处独立 `mov $1;jmp` 中继）。残差 N：ORIG 7 处
// `xor $1;test;jne +0x15d` 全部直达同一 `mov $1` 尾块，4.4 恒产 `je NEXT`
// 极性（P222 正向包裹/短路链/goto 三形均实测不翻）⇒ 极性恒折封顶。
// `je next; mov $1; jmp`（+14 条）；goto 尾标签形（探针 /tmp/b28d_probe_pol.cpp t2）
// 得 jne 但每源多 1 个 nop 岛（+7），嵌套正向 if/`||` 链（t3/t6）丢 xor 或末项
// 物化 ⇒ 无源码解。**真语义 bug 修正**：ORIG jne 落点 +0x15d 即 return 1 ⇒
// 任一 check 为假返回 1（旧重建 `return false` 与 ORIG 相反，Ghidra C 同证）。
#include "GlobalData/CheckInventoryEmptyCount.inc"
}

// ---- DeleteAllRentalItemInSameSectionForNewRent —— T 0x08271807 ------------
// [b28d-18 NEAR] 修：(*it) 代 it->（P167 deEv）+ ResetSlot 双分支独立调用（P48）；p167 CLEAR 18/18 | ORIG 0x08271807
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（18/18）；残差=N82 槽位/STL 展开。
// [SweepGD 2026-09-10] **gap +5→−1**（net 123→117，ORIG 118）三修（全部 ORIG 直证）：
//   ① 删 `const int itemId` 具名槽（ORIG 0x08271874 `mov 0x4(%eax),%eax; mov %eax,%ebx`
//      ⇒ 值走 %ebx 跨 call、无槽；我方旧 -0x10 槽 = store+load 两条）→ 两处使用点内联
//      `(*it).m_itemIndex`（P0/P157 逐处重读）；
//   ② 删 `CInventory* invR` 具名槽（ORIG 0x08271879..0x08271899 `getCurCharacInvenR`
//      结果直传 CheckExist、无 -0xc 槽；我方旧 store+load 两条）→ 内联到调用表达式（P183）；
//   ③ entry 写序改 ORIG 序 **slot→bEquip→itemId**（0x08271902/909/910）且 m_bEquip 直存
//      `= isEquip`（ORIG movzbl+mov，P136/P54），m_itemId 从 `(*it)` 重读 +4 而非复用局部。
// 残余 **−1 条** = ORIG 收尾 `lea -0x8(%ebp),%esp; add $0x0,%esp`（双条，4.1 帧 8 对齐产物）
// vs 4.4 单条 `add $0x40,%esp` ⇒ 结构性（N115/N126 族），封顶；p167 仍 CLEAR 18/18。
#include "GlobalData/DeleteAllRentalItemInSameSectionForNewRent.inc"
}

// ---- ProcessEquipItem —— T 0x082714c5（同名报告全文转写；委托转写核对）-----
// 与 ProcessInsertItem 仅差 SetUpgrade/rentId 写入顺序、装备位插入与标记字节。
// [b28d-18 N71] 同 ProcessInsertItem：空析构 trivial 判定差异（P71） | ORIG 0x082714c5
// [b28d-solo-GlobalData] DIFF 封顶：p167 逐调用同形；残差=N82 槽位/寄存器分配。
// [b28d-PoolGlobalData 二压] P71 作废（同 RentalInfo 伪析构翻案）；0x082714c5 全切片
// 重对齐：①删 savedRentId/slot 具名（ORIG 无 -0x10 槽、`mov %ax,-0x1a` 直存）；
// ②ri 写序按 ORIG = slot→itemIndex→flag0→inEquip→f8→f9→fB→rentId；
// ③**语义修正**：m_field8/9/B 逐字节取自 **inElem**[0]/[1]/[3]
//   （0x08271610/619/623 `mov 0xc(%ebp); movzbl (1,%eax)` 直证，旧 `reward>>8/>>24`
//   为误推断）；④rentId 两处 if/else 直存形（ORIG cmp/je+jmp，无 setcc 物化）。
#include "GlobalData/ProcessEquipItem.inc"
}

// ---- ProcessInsertItem —— T 0x08271666（同名报告全文转写）------------------
// [b28d-18 N71] EXTRA RentalInfo::D1Ev×2 + _Unwind_Resume×1：ORIG 空析构被 4.1 判 trivial 不调用（P71；throw() 实测无效） | ORIG 0x08271666
// [b28d-solo-GlobalData] DIFF 封顶（已被二压推翻）：N71 判词倒果为因。
// [b28d-PoolGlobalData 二压] ①删 `~RentalInfo(){}` 伪建模（ORIG `_Destroy_aux<ILb1E>`
// 实证平凡）⇒ EXTRA D1Ev×2+_Unwind_Resume×1 归零；②同 0x082714c5 全切片重对齐：
// 删 savedRentId/slot 具名、ri 写序 slot→itemIndex→flag0→inEquip→f8→f9→fB→rentId、
// **语义修正** m_field8/9/B=inElem[0]/[1]/[3]（0x082717b1/7ba/7c4 `movzbl` 直证）。
#include "GlobalData/ProcessInsertItem.inc"
}

}  // namespace pc_room

// ===========================================================================
// RentalMachinePcRoom / RentalMachineDimension 本体
// ===========================================================================

// ---- PcRoom::log —— T 0x082707c0（同名报告全文转写）------------------------
// [b28d-18 N7] p167 CLEAR；残差=callee-saved 角色轮换 + 槽位（N7/N82） | ORIG 0x082707c0
// [b28d-solo-GlobalData] NEAR 封顶 N7/N82：p167 CLEAR；残差=callee-saved 角色
// 轮换（esi/ebx/edi 循环移位）+ 槽深差 4（-0x19/-0x1d、-0x2c/-0x1c，P230 定稿
// 4.4.7 分配固定不可择）+ 符号地址重定位显示伪差。
#include "GlobalData/log.inc"
}

// ---- Dimension::log —— T 0x0826ef16（同名报告全文转写）---------------------
// [b28d-18 N7+N82] 槽位+callee-saved 角色轮换；P76 待办：ORIG auto_ptr 模板参为嵌套类 RentalMachineDimension::RentalMachineDimensionImpl（p167 MISSING ptEv） | ORIG 0x0826ef16
// [b28d-solo-GlobalData] NEAR 封顶：p167 CLEAR（9/9）；N7（callee-saved 角色轮换 esi/ebx/edi）+
// N82（帧 0x4c vs 0x3c、槽位平移）。
#include "GlobalData/log_1.inc"
}

// ---- Dimension::checkRentalLevel —— T 0x0826f7e4（同名报告）----------------
#include "GlobalData/checkRentalLevel.inc"
}

// ---- Dimension::checkRentalGrowJob —— T 0x0826f7fc（同名报告）--------------
#include "GlobalData/checkRentalGrowJob.inc"
}

// ---- Dimension::GetRewardItemGiveList —— T 0x0826f814（同名报告：透传）-----
#include "GlobalData/GetRewardItemGiveList.inc"
}

// ---- Dimension::GetRentItemInfo —— T 0x0826f842（同名报告全文转写）---------
// [b28d-18 N82] p167 CLEAR（11/11）；残差=槽位/寄存器分配 | ORIG 0x0826f842
// [b28d-solo-GlobalData] DIFF 封顶（二压重开）：残差=N82 槽位/寄存器分配。
// [b28d-PoolGlobalData 二压] 0x0826f918.. 切片逐条重对齐（探针 pol_probe4 g4/g5）：
// ①size() 结果具名落槽（ORIG sz0@-0x14 / sz1@-0xc，各调一次）；②下标**不具名**
// （ORIG 每用点 `mov 0x10(%ebp); movzbl (%eax)` 重读）；③两段检查=js 正跳 ret 岛 +
// `movsbl; cmp sz; jl pass` 真跳 pass、ret 块 fall-through ⇒ goto 形「ret 标签置于
// 第二检查与 pass 之间」（4.1 布局，4.4 同形无中继 nop）；④inner 具名 @-0x10。
#include "GlobalData/GetRentItemInfo_1.inc"
}

// ---- PcRoom::checkAllRentalItemScript —— T 0x082708cc（同名报告全文转写）---
// [b28d-18 N82] 残差=槽位/STL（N82/N89） | ORIG 0x082708cc
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（50/50）；残差=N82 槽位/分支布局 + N89。
// [SweepGlobalData] itemId 值拷贝 → 对象引用（ORIG 0x082709b6 `call operator*;
// mov %eax,-0x14`）后每使用点复现重解引用；残余 = 4.4 寄存器中转（N18a/P211）。
#include "GlobalData/checkAllRentalItemScript.inc"
}

// ---- Dimension::checkSelectionListScript —— T 0x0826f14e -------------------
// （报告全文转写；五谓词次序同 PcRoom，cerr 前缀为 RentalDimensionSystem）
// [b28d-18 N82] 残差=槽位/STL 展开（N82/N89） | ORIG 0x0826f14e
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（43/43）；已按 P248 档① 改 bool 返回；残差=N82/N89。
// [SweepGlobalData] 同 PcRoom：itemId 改对象引用（ORIG 0x0826f232 同形）。
#include "GlobalData/checkSelectionListScript.inc"
}

// ---- Dimension::checkAllRentalItemScript —— T 0x0826f022（同名报告转写）----
// [b28d-18 N82] 残差=槽位/分支布局（N82） | ORIG 0x0826f022
#include "GlobalData/checkAllRentalItemScript_1.inc"
}

// ---- PcRoom::CheckupAllRentalItems —— T 0x08270d1e（同名报告全文转写）------
// [b28d-18 N7] 残差=callee-saved 角色轮换 + 单段 regmove（N7/N18a） | ORIG 0x08270d1e
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（18/18）；残差=N7/N82 寄存器角色与槽位。
#include "GlobalData/CheckupAllRentalItems.inc"
}

// ---- Dimension::CheckupAllRentalItems —— T 0x0826f52c（同名报告全文转写）---
// [b28d-18 N7] 残差=callee-saved 角色轮换 + 单段 regmove（N7/N18a） | ORIG 0x0826f52c
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（18/18）；残差=N7/N82 寄存器角色与槽位。
#include "GlobalData/CheckupAllRentalItems_1.inc"
}

// ---- PcRoom::Rent —— T 0x082704e6（Rent.md 全文转写）------------------------
// [b28d-solo-GlobalData] 归零 169=169：①真语义 bug——首守卫 ORIG 为
// `!IsPcRoomUser && !m_field11`（两条 je 同落继续块，旧写 `|| m_field11==1` 逻辑相反）；
// ②RentInput::m_field11 uchar→bool（P248 档②）+ 嵌套 if（P133①）；
// ③m_bEquip 取反+两臂交换（P47/P222）| ORIG 0x082704e6
#include "GlobalData/Rent_1.inc"
}

// ---- Dimension::Rent —— T 0x0826ec22（Rent.md 全文转写）---------------------
// [b28d-solo-GlobalData] 归零 176=176：m_bEquip 分支条件取反+两臂交换复现
// ORIG else-first 布局（`test;je <ProcessInsertItem>`，P47/P222）；p167 CLEAR 21/21 | ORIG 0x0826ec22
#include "GlobalData/Rent_2.inc"
}

// ORIG _ZN20InstanceRentalSystem17CheckRentableItemEij T 0x082702dc【objdump】：
// machine = getMachine(type)；machine==0 → 返 1（视为可用）；否则尾调虚槽
// vptr+0x10 CheckRentableItem(itemIdx)。（OnlyOrig_CreatureJob.EJEnchanter 批补）
// [b28 判词 N7] 残差=call *%edx↔%eax 换名+伴随寄存对调，主体同形封顶。
#include "GlobalData/CheckRentableItem_2.inc"
}

// ORIG _Z18get_seed_from_datev T 0x086b1d25：year*365 + mon*30 + mday
// （ORIG 以 mon*32-mon 形态实现 *30；反汇编 0x086b1d53 读 tm_mon(+0x10)）。
#include "GlobalData/get_seed_from_date.inc"
}

// ============================================================================
// [b28d-Main·检查点18] 原 7 条 GlobalInstance 显式实例化已删（同 StaticPool 问题）：
//   显式实例化为每个 T 发射**全部**成员（inst/create/destroy/inst_ptr），而 ORIG 只
//   发射实际使用的（如 GlobalInstance<EncryptTool> 仅 create/inst_ptr/m_p，
//   GlobalInstance<Cipher> 同）⇒ inst/destroy 成为 only-NEW。定义在 CEnvironment.h
//   类体内（实例化点可见）⇒ 按使用点隐式实例化即与 ORIG 一致。
// ============================================================================
// ---- 全局单例访问器（ORIG W：G_TimerDispatcher 0x0847e8a4 /
// G_InterDispatcher 0x0847e8b7，GlobalInstance inst_ptr 形态，同 G_TimerQueue）----
// 判据㉓（符号绑定）：ORIG nm `_Z17G_TimerDispatcherv` = **W** 0x0847e8c4 size 0xd；
// 我方原为 T 强绑定（Main 已批准「各 TU 对自己 TU 内 G_* 定义加 weak」的单点修法）。
__attribute__((weak)) TimerDispatcher* G_TimerDispatcher()
{
    return GlobalInstance<TimerDispatcher>::inst_ptr();
}

// 同上：ORIG `_Z17G_InterDispatcherv` = W 0x0847e8b7 size 0xd。
__attribute__((weak)) InterDispatcher* G_InterDispatcher()
{
    return GlobalInstance<InterDispatcher>::inst_ptr();
}
