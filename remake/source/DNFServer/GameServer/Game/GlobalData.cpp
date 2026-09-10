// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 GlobalData/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
                                      unsigned char* data, short len)
{
    (void)b;
    (void)c;
    // ORIG 0x080de240：按 stream 长档位取池；<<0x252 <<-1；
    // put_binary({int a, uint d, short len}, 0xc)；put_binary(data, len)。
    const int size = (int)len + 0x14;
    CStreamGuard guard(true);
    // ORIG 0x080de26c..0x080de2e3 三分支形态：首支 `cmpl $0xfff,-0xc; jg else`
    // （= size <= 0xfff）；次支重测 `cmpl $0xfff; jle big` + `cmpl $0x3fff; jg big`
    // ⇒ 源码在 else-if 中显式重复 `size > 0xfff`，并非纯区间判断。
    if (size <= 0xfff)
    {
        guard.Set(GlobalData::s_small_stream_pool->Acquire("BusinessImpl.cpp", 0x3a));
    }
    else if (size > 0xfff && size <= 0x3fff)
    {
        guard.Set(GlobalData::s_stream_pool->Acquire("BusinessImpl.cpp", 0x3e));
    }
    else
    {
        guard.Set(GlobalData::s_big_stream_pool->Acquire("BusinessImpl.cpp", 0x42));
    }
    **guard << 0x252;
    **guard << -1;
    struct
    {
        int field00;
        unsigned int field04;
        unsigned short field08;   // ORIG 0x080de355 `mov %ax,-0x18(%ebp)`（2 字节存）
    } hdr;
    hdr.field00 = a;
    hdr.field04 = d;
    hdr.field08 = (unsigned short)(len);   /* movzwl %ax */
    guard->put_binary(&hdr, 0xc);
    guard->put_binary(data, (int)len);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    // ORIG 0x080de3b9：`return 0` 先物化到 %ebx，再析构 guard，最后 mov %ebx,%eax。
    return 0;
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
{
    tagCS_UPDATE_CHANNEL_INFO pkt;
    // ORIG 0x081061b1：无 env 局部，每个字段各自 G_CEnvironment()（返回 %eax 直用）。
    pkt.m_gcNo = G_CEnvironment()->m_serverEnvir.m_gcNo;
    pkt.m_group = G_CEnvironment()->get_server_group();
    pkt.m_channelNo = G_CEnvironment()->m_serverEnvir.m_channelNo;
    pkt.m_maxUserNum = G_CEnvironment()->m_serverEnvir.m_maxUserNum;
    pkt.m_curUserNum = static_cast<int>(
        G_GameWorld()->m_UserInWorld.size());         // m_UserInWorld
    std::strcpy(pkt.m_serverIP,
                G_CEnvironment()->m_serverEnvir.m_serverIP);
    pkt.m_port = G_CEnvironment()->m_serverEnvir.m_tcpPort;

    m_udp.SendToServer(reinterpret_cast<char*>(&pkt), pkt.m_hdr.getSize(),
                       static_cast<unsigned short>(m_port), m_ip.c_str());
    return true;
}

// ORIG ConnectBridgeServer 0x08106278：socket/connect 桥接服务器并设非阻塞。
// [b28d-18 IDENTICAL_AE] 修：addrlen 改 int + (socklen_t) cast 翻实参装载序（P248 档③新实证，addrlen→%ecx 先载）；NDIFF 3→1（余尾 nop） | ORIG 0x08106278
#include "GlobalData/ConnectBridgeServer.inc"
{
    sockaddr_in addr;
    std::memset(&addr, 0, 0x10);
    // ORIG 0x08106287：socket 返回值直写成员 m_sock（this+0x18），后续每处成员直读。
    m_sock = socket(AF_INET, SOCK_STREAM, 0);          // socket(2,1,0)
    if (m_sock < 0)
    {
        std::puts("[CmmChannelProxy::SendCheckChannelScript] Can't create socket");
        return false;
    }
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(m_ip.c_str());    // inet_addr(c_str(this))
    addr.sin_port = htons((unsigned short)G_CEnvironment()->get_tcp_port_channel());
    // ORIG 0x08106336 区装载序 = addrlen→%ecx、&addr→%edx、m_sock→%eax
    // （右起第三实参最先落寄存器）。4.4 恒把 %ecx 装载推迟到 m_sock 之后
    // ⇒ 唯一能翻转装载序的手段是把 addrlen 做成 volatile 局部（探针 f2 证），
    // 但 4.4 的 volatile 特判把它塞进更远槽、并连带把 addr/nonblock 等**全部**
    // 后续槽号推偏：实测 volatile + volatile 垫块回推的写法让本函数
    // ndiff 2 → 20（非统一平移，无法补偿）⇒ 两难坐实，记 N82 族封顶。
    int addrlen = 16;
    if (connect(m_sock, reinterpret_cast<sockaddr*>(&addr), (socklen_t)addrlen) < 0)
    {
        std::puts("[CmmChannelProxy::SendCheckChannelScript] Can't connect channel bridge server");
        return false;
    }
    int nonblock = 1;
    ioctl(m_sock, FIONBIO, &nonblock);                 // 0x5421
    return true;
}

// ORIG SendPacket 0x0810637c：write 发送；EAGAIN/EINTR/0 → 0（可重试），
// 其它错误 cMyTrace 后返回 -1。
#include "GlobalData/SendPacket.inc"
{
    // ORIG 0x0810638b：fd 每处成员直读（this+0x18），无局部 spill。
    // ORIG 0x081063a4：赋值即条件 + (bool) 物化 → cmp/setle/test/je。
    int n;
    if ((bool)((n = write(m_sock, buf, len)) <= 0))
    {
        if (errno == EAGAIN || errno == EINTR || errno == EWOULDBLOCK ||
            errno == 0)
        {
            return 0;
        }
        // ORIG 0x081063e2：临时对象表达式 —— 实参先评估（strerror/errno/m_sock
        // 落 %ebx/%edi/%esi），随后 ctor，再 operator()。
        cMyTrace(__PRETTY_FUNCTION__, 200, 0)(
            "SEND ERROR DISCONNNECT NOW FD[%d] : %d(%s)",
            m_sock, errno, strerror(errno));
        return -1;
    }
    return n;
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
{
    // ORIG 0x0810651c `cmpl $0; jg body` + fall-through `mov $0;jmp end`——
    // early-return `if (ret <= 0) return 0;` 极性（技巧 17）。
    // ORIG 0x0810654a getsockopt 第三实参硬编码 $0x8（老 SO_ERROR 值，非本
    // 头文件宏的 4）；level 硬编码 $0x1（0x08106552）。
    // ORIG 0x081065ab 负判定 `shr $0x1f; test al`（0x081064d4 select 后同理
    // 无符号语义）——用无符号比较 `(unsigned)nread > 0x7fffffff` 复现。
    // 槽序对齐（4.4 顶层=先声明较近、block 变量优先占近槽）：目标
    // p -0x1c, i -0x20, nread -0x24, x -0x28, ret -0x2c, tv -0x50/54,
    // error -0x58, len -0x5c, fd -0xdc（ORIG 0x08106468-0x08106494）。
    // 槽位靶：ret -0x2c（0x08106468）、dead -0x28（0x0810647d，tv 写之后
    // 落）、nread -0x24、i -0x20、p -0x1c、tv -0x54/50、error -0x58、
    // len -0x5c、fd -0xdc。声明序控远槽，语句序控 init 位置。
    struct timeval tv;         // 先声明 → 远区 -0x54
    fd_set readfds;            // -0xdc 大块
    int ret = 0;               // 0x08106468 → -0x2c（先声明占更远槽）
    int nUnused;               // dead init@0x0810647d → -0x28
    tv.tv_sec = 300;
    tv.tv_usec = 0;
    nUnused = 0;
    int nread;
    {
        unsigned int i;         // ORIG i(-0x20) 比 pBits(-0x1c) 远 → 先声明
        // [SweepGD 2026-09-10] 旧判词 N125 作废：ORIG 0x0810648f `mov -0x20,%edx;
        // mov -0x1c,%eax; movl $0,(%eax,%edx,4)` 的 scaled 直存由**具名数组成员
        // 视角**复现 —— `fd_set* fds = &readfds; __FDS_BITS(fds)[i]`（P291/P288）；
        // 用 `unsigned long* pBits = (unsigned long*)&readfds` 时 4.4 恒折 `shl $2; add`。
        fd_set* fds = &readfds;
        for (i = 0; i <= 31u; ++i)
        {
            __FDS_BITS(fds)[i] = 0;
        }
    }
    // ORIG 0x081064b8：FD_SET 无符号 `shr $5`/`and $0x1f` 位图形
    // （`mov 0x18,%eax; shr $0x5,%eax; mov %eax,%edx; mov -0xdc(%ebp,%eax,4),%ebx`）。
    // [SweepGD 2026-09-10 实测] 具名 `unsigned long ix`（175 条）优于内联表达式
    // （176 条：4.4 重算两份 shr 各带一次 this 重读）；两形均非 ORIG 单份
    // `%eax`+`mov %eax,%edx` 复制形 ⇒ 取条数较优的具名形，余 4 条记 N18a/N96。
    // [GDataWriter 2026-09-11 负结论] ORIG 0x081064b8 段 FDELT 全程寄存器
    // （`shr $0x5,%eax; mov %eax,%edx` 无槽往返）；具名 `ix`（174 条）与纯
    // `register` 提示（174 条）**实测同读数**（register 零效），内联表达式 176 条
    // ⇒ 取具名形，余 N18a/N96 封顶。
    unsigned long ix = static_cast<unsigned long>(m_sock) >> 5;
    readfds.fds_bits[ix] |= (1UL << (static_cast<unsigned long>(m_sock) & 31));
    ret = select(m_sock + 1, &readfds, 0, 0, &tv);
    if (ret <= 0)
    {
        return 0;
    }
    int error = 0;
    socklen_t len = 4;
    // （取参序残余差：ORIG 0x0810653d &error→%edx 先载、m_sock→%eax、
    // &len→%ecx；4.4 恒 &len 先载且 m_sock 占 %eax ⇒ register asm("ecx")
    // 反而打乱 store 序，probe 证不可翻 ⇒ 记 N 类。）
    getsockopt(m_sock, 1, 8, &error, &len);
    OS_API::Sleep(100);
    nread = static_cast<int>(::read(m_sock, m_buf + m_field1c,
                                    static_cast<size_t>(0x80000 - m_field1c)));
    // ORIG 0x081065a8：`mov -0x24,%eax; shr $0x1f,%eax; test %al,%al; je`——
    // 显式移位形（`>0x7fffffffu` / 裸 `>>31` 均被 4.4 fold 成 cmpl/jns），
    // b28 P0 清绑后恢复：具名局部反引入槽往返，表达式内联 uchar 化实测
    // 与 ORIG 同形（探针 /tmp/l12p7.cpp f：`mov; shr $0x1f,%eax; test %al; je`）。
    if (static_cast<unsigned char>(static_cast<unsigned int>(nread) >> 31))
    {
        if (errno == EAGAIN || errno == EINTR || errno == EAGAIN || errno == 0)
        {
            return 0;
        }
        cMyTrace(__FUNCTION__, 0x114, 0)(
            "RECV ERROR DISCONNNECT NOW FD[%d] : %d(%s)",
            m_sock, errno, strerror(errno));
        return -1;
    }
    if (nread == 0)
    {
        cMyTrace(__FUNCTION__, 0x11a, 0)(
            "Recv ERROR = 0 (%d) : %s, MaxRead(%d) nRead(%d)",
            errno, strerror(errno), 0x80000, nread);
        return -1;
    }
    m_field1c += nread;
    return nread;
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
// [GDataWriter 2026-09-11 · **语义修正 + 减少 4 条**] ORIG 0x0810684b..0x08106869 逐条：
//   `mov -0x18(%ebp),%eax; movzbl -0x4d4(%ebp,%eax,1),%eax; cmp $0xa,%al; je <linePos=0>;
//    …cmp $0x23,%al; jne 0x08106870`（0x08106870 = fgets **循环条件头**）
// ⇒ 非注释行**不 break**（继续读下一行，line 保留末行）；旧写 `continue`+`break` 会在
// 首个普通行提前退出 ⇒ version 内容真语义 bug（AE 归一化看不见）。已改为单一
// `if (a=='\n'||a=='#') linePos=0;` ⇒ 实测 295→**291**（ORIG 289，gap +6→+2）。
#include "GlobalData/SendCheckChannelScript.inc"
{
    ConnectBridgeServer();

    char name[0x14];
    std::memset(name, 0, sizeof(name));
    std::strncpy(name, "channel_info", 0x14);
    struct stat st;
    if (stat(name, &st) < 0)
    {
        if (errno != ENOENT)
        {
            return false;
        }
        DNFFLib::Make_Dir(const_cast<char*>(name));
    }

    char line[0x400];
    FILE* fp = std::fopen("channel_info/version", "wb+");
    // ORIG 0x081067e4：`cmpl $0,-0x20; sete %al; test %al,%al; je` ⇒ 原码是
    // **while** 形（P295/IsGateArea 实证；探针 /tmp/gdprobe/p3.cpp f2/f3：
    // `while(p==0){…return false;}` 4.4 产同形，`if` 形恒折 `jne`）。
    while (fp == 0)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__, 0x143, 9,
            true, true);
        trace("\t- [ERROR] : cannt open channel_script_version file\n");
        return false;
    }
    // ORIG（0x08106870 段）：行缓冲为**栈局部** `line[0x400]`（`lea -0x4d4(%ebp)`
    // 直证；ORIG nm 无 s_version/s_linePos 符号，旧 static 建模为发明物已删），
    // 循环带行首空白剥离 + '\n'/'#' 注释行跳行。
    // [b28d-solo-GlobalData] p167 EXTRA fgets×1：ORIG 仅 1 次 fgets（0x08106888
    // 尾测 `test;setne;test;jne <body>` 旋转形）⇒ 单条件 while，删 init 弃用读。
    // ORIG 0x081067e4 `movl $0x0,-0x18(%ebp)`：loopPos 为栈局部（-0x18）。
    int linePos = 0;
    while (std::fgets(line, 0x400, fp) != 0)
    {
        char* src = line;
        char* dst = src;
        do
        {
            while (*src == ' ' || *src == '\t')
            {
                ++src;
            }
            // ORIG 0x08106829..0x08106849：赋值 → `++dst` → 读**未自增**的 `*src`
            // 物化 setne → `++src` → test（故条件写 `*src++ != '\0'`；旧
            // `*src++` + `*(src - 1)` 形多发 `sub $0x1,%eax` 一条）。
            *dst++ = *src;
        } while (*src++ != '\0');
        // [GDataWriter 2026-09-11 · **语义修正**] ORIG 0x0810684b..0x08106869 逐条：
        //   `mov -0x18,%eax; movzbl line(%ebp,%eax,1),%eax; cmp $0xa,%al; je <linePos=0>`
        //   `… cmp $0x23,%al; jne 0x08106870`（0x08106870 = **fgets 循环条件头**）
        // ⇒ 非注释/非换行行**不 break**、继续读下一行（line 保留最后一行的内容）。
        // 旧写 `if(\n){…continue;} if(!='#') break;` 会在首个普通行提前退出 ⇒ 运行期
        // version 内容/后续 strncpy 与 ORIG 不同（真语义 bug，AE 归一化看不见）。
        if (line[linePos] == '\n' || line[linePos] == '#')
        {
            linePos = 0;
        }
    }
    std::fclose(fp);

    tagCS_CHECK_SCRIPT_VERSION reqVer;
    std::strncpy(reqVer.m_version, line, 0x10);
    if (SendPacket(reinterpret_cast<char*>(&reqVer),
                   reqVer.m_hdr.getSize()) == 0)
    {
        return false;
    }
    if (RecvPacket() == 0)
    {
        return false;
    }
    // ORIG 08106922：m_buf 保存为局部头指针槽（mov this; add $0x20; mov -0x1c(%ebp)），
    // 循环条件经 tagPacketHeader::getSize（call 08106bcc）读 +0x02 dword size。
    tagPacketHeader* pVerResp = reinterpret_cast<tagPacketHeader*>(m_buf);
    while (m_field1c != pVerResp->getSize())
    {
        if (RecvPacket() == 0)
        {
            return false;
        }
    }

    fp = std::fopen("channel_info/version", "rb+");
    // ORIG 0x081069d7：同 P295 while 形（sete/test/je）。
    while (fp == 0)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__, 0x188, 9,
            true, true);
        trace("\t- [ERROR] : cannt open channel_script_version file\n");
        return false;
    }
    std::fputs(reinterpret_cast<char*>(pVerResp) + 0xf, fp); // ORIG lea 0xf(%eax) on 同一局部槽：头 0xb + first 4
    std::fclose(fp);

    m_field1c = 0;
    std::memset(m_buf, 0, 0x80000);

    tagCS_GET_SCRIPT reqGet;
    if (SendPacket(reinterpret_cast<char*>(&reqGet),
                   reqGet.m_hdr.getSize()) == 0)
    {
        return false;
    }
    if (RecvPacket() == 0)
    {
        return false;
    }
    // ORIG 08106a7c：第二段独立局部槽 -0xc(%ebp)，同 getSize 形态。
    tagPacketHeader* pScriptResp = reinterpret_cast<tagPacketHeader*>(m_buf);
    while (m_field1c != pScriptResp->getSize())
    {
        if (RecvPacket() == 0)
        {
            return false;
        }
    }
    fp = std::fopen("channel_info/channel_info.etc", "w+");
    // ORIG 0x08106ae8：`cmpl $0,-0x20; sete; test; je; mov $0,%eax; jmp` ⇒
    // while 形（探针 p3.cpp f3 逐条同形）。
    while (fp == 0)
    {
        return false;
    }
    int total = pScriptResp->getSize();
    std::fwrite(reinterpret_cast<char*>(pScriptResp) + 0xb, 1, total - 0xb, fp);
    std::fclose(fp);
    close(m_sock);
    return true;
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
{
    // ORIG 0x822ad44：旗标指针先落局部变量再解引用。
    bool* flag = GetLightServerFlag();
    return *flag;
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
{
    m_field0 = 100;
    m_field4 = 3;
    m_field8 = 2;
    m_fieldc = 0.8f;
    m_field10 = 1.3f;
    m_field14 = 0.0f;
    m_field18 = 0.5f;
    m_field1c = 8;
    m_field20 = 0.9f;
    m_field24 = 0.1f;
    m_field28 = 0.12f;
    m_field2c = 2.0f;
    m_field30 = 2.5f;
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
    : m_library(0)   // ORIG 0x080de697：m_library 清零先于 vector/XorRand 成员
                     // ctor —— mem-init（声明序）而非 body 赋值。
{
    // ORIG 0x080de6bf：FT_Init_FreeType(&m_library)（+0 ⇒ this 同值）结果落
    // 临时槽 -0xc(%ebp)（不检查，dead store 具名化）；随后 seed(now)。
    int err = FT_Init_FreeType(&m_library);
    (void)err;
    m_rng.seed(Tick::nowAsInternalValue());
    // ORIG 0x080de6de：seed call 后 jmp 越过 EH island 到尾部 epilogue——
    // 函数尾显式 return（技巧 7 尾岛）。
    return;
}

// [SweepCAutoPunish 2026-09-10 已修·AE 49=49] ORIG 0x080de706 的 EH landing 复现：
// 根因是 FT_Done_Face/FT_Done_FreeType 占位定义**同 TU 可见** ⇒ 4.4 判 nothrow 不布
// landing。按 P210② 把这两个 extern "C" 定义迁 StubS3A_deps.cpp（Main 授权），本 TU
// 只留声明 ⇒ 4.4 判可能抛，逐条复现 ORIG：push esi/ebx + `call vector D2Ev` cleanup +
// `_Unwind_Resume` + 尾 `add $0x20; pop ebx/esi/ebp`。探针 /tmp/gdprobe/p2.cpp
// （同 TU 定义⇒无 landing；仅声明⇒有 landing）；nm 全链接集确认此前仅 GlobalData.o 定义。
// 旧判词「N-EH 占位定义可见性」作废。
#include "GlobalData/dtor_CaptchaGen.inc"
{
    // ORIG 0x080de717：ix 实参先载 i 槽（int→unsigned 转换 ⇒ arg 先于 this，
    // 技巧5/15）；0x080de74b `setg` ⇒ 两侧 int 有符号比较；循环体 FT_Done_Face
    // （0x080de731）；循环后 FT_Done_FreeType(m_library)（0x080de75a）再落
    // vector 成员 dtor。
    // [b28 判词·N·EH 可见性] ORIG 49 vs 我们 33：ORIG 4.1 对**外部 FreeType .so**
    // 不透明调用布 landing（push esi/ebx + cleanup vector D1 + _Unwind_Resume 段，
    // 16 insn）；本仓 FreeType 以 extern "C" 占位定义**同 TU 内联可见**（1138 起），
    // 4.4 throw 分析判 nothrow → 无 landing。探针 /tmp/l2_p10.cpp 实证改 extern 声明
    // 即产 landing，但搬移 shim 定义会同时改 ctor/addFont（已 AE）调用点形态，
    // 风险>收益；`throw(...)` spec（-fno-enforce-eh-specs）与 try/catch{throw;}
    // 实测均无效（33 不变）。属 N 类「跨工具链第三方符号可见性」，语义等价豁免。
    for (int i = 0; i < (int)m_faces.size(); ++i)
    {
        FT_Done_Face(m_faces[i]);
    }
    FT_Done_FreeType(m_library);
}

// [b28d-18 IDENTICAL_AE] 48=48 逐条同形（.o call 重定位标签伪差） | ORIG 0x080de794
#include "GlobalData/addFont.inc"
{
    // ORIG 0x080de794（report 汇编+反编译 C 全读）：FT_New_Face(m_library, font, 0,
    // &face) → 失败返 false → FT_Set_Pixel_Sizes(face, 0, size) → 失败返 false →
    // FT_Select_Charmap(face, FT_ENCODING_UNICODE=0x756e6963) → 失败返 false →
    // m_faces.push_back(face)（vector<void*>）→ 返 true。
    // [b28 BM2 判词·AE] 清单 NEAR(2) 为全二进制全局地址标签伪差；fresh
    // /tmp/tu_GlobalData.o fn_diff_tu 归一 0 差（48=48 逐条）。
    // 原转写以 fopen/fclose/malloc/strcpy 假实现替代真 FT 序列（违 §4.1），已改为
    // 忠实 ORIG；face 错误码 slot 落 -0xc 与 `cmpl $0,slot; je 成功` 逐指令对应。
    void* face;       // ORIG local_18 @ -0x14（FT_New_Face 出参，未初始化）
    int err = FT_New_Face(m_library, font, 0, &face);   // ORIG local_10 @ -0xc
    if (err != 0)
    {
        return false;
    }
    err = FT_Set_Pixel_Sizes(face, 0, (unsigned int)size);
    if (err != 0)
    {
        return false;
    }
    err = FT_Select_Charmap(face, 0x756e6963ul);
    if (err != 0)
    {
        return false;
    }
    // [b28d-solo-GlobalData] P315/P234：ORIG 0x080de81a `mov -0x14,%eax; mov %eax,-0x10;
    // lea -0x10,%eax; mov %eax,0x4(%esp)` 调 `push_back(OS0_)`（void*&& 右值重载）；
    // 具名左值 toPush 选 RKS0_（p167 MISSING OS0_/EXTRA RKS0_）⇒ 写 `(void*)face`
    // 产生 prvalue 临时（-0x10 拷贝槽）绑定右值引用。
    m_faces.push_back((void*)face);
    return true;
}

// [b28d-solo-GlobalData] 归零 100=100：P211 档③ 类型旋钮——三个静态计数
// unsigned int→int + 使用处 (unsigned int) cast，复现 `mov <A>,%edx; mov %edx,%ecx`
// 两段 divisor 装载（旧 N18a 判词作废）；CaptchaGen.h 同步（R2 已广播） | ORIG 0x080de83c
#include "GlobalData/generateText.inc"
{
    // [b23S3C] ORIG 0x080de83c：三分支各独立循环（i 槽 -0x14/-0x10/-0xc），
    // 表/计数直引全局静态成员（movzwl table(,%eax,1) 索引形），勿归一成共享循环。
    // [b28 判词·N18a×3] 各分支 `m_rng() % kXxxCount`：ORIG 4.1 先 `mov g,%edx`
    // 再 `mov %edx,%ecx`（divisor 两段物化），4.4 恒直 `mov g,%ecx` 单段（N18a
    // 「4.4 恒折单步 regmove」反向不可造）；三分支共差 3 insn（97 vs 100），
    // div/mod 输入输出逐条等。
    if (code == CountryCode::korea)
    {
        for (int i = 0; i < len; ++i)
        {
            out[i] = kKoreanGeneral[m_rng() % (unsigned int)kKoreanGeneralCount];
        }
    }
    else if (code == CountryCode::japan)
    {
        for (int i = 0; i < len; ++i)
        {
            out[i] =
                kJapaneseHirakana[m_rng() % (unsigned int)kJapaneseHirakanaCount];
        }
    }
    else if (code == CountryCode::taiwan)
    {
        for (int i = 0; i < len; ++i)
        {
            out[i] =
                kTaiwanAlphabet[m_rng() % (unsigned int)kTaiwanAlphabetCount];
        }
    }
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
// [G7-sub 2026-09-11 判词] CaptchaGen::generateImage（ORIG T 0x080deade）：私有 .o 实测
// ORIG 685 / 我方 682（gap 3，我方短）。slug 级差 10 处，逐条归因：
//   · x87 控制字改舍入序列（`fldcw -0xec; fistpl -0xf0; fldcw -0xea; movzwl …;
//     mov %ax,-0xec` ×7 处）两侧同构**仅槽号平移**（我方同序列在 -0xdc 等）⇒ N82/N85。
//   · `flds/fmuls/fnstcw` 浮点链同构、槽基 -0x98/-0x94 vs 我方对应槽 ⇒ N82。
//   · 其余为 `.rodata` 符号文本（我方 .o 无解析名）与对齐。
// p167 调用点差集仅 `vector<int>::D2` 同址别名（ORIG nm 实测 D1=D2=0x08083dda）。
// 结论：**封顶，无改动**（语义完成；残差 N82/N85 + 数据符号）。
#include "GlobalData/generateImage.inc"
                               const ImageOption& opt, GreyImage* image)
{
    // [P190 附注①] ORIG 帧 0x120 vs 我方 0x110（差 0x10）：2 个**取址** double
    // 撑帧使槽基整体下移 0x10 对齐（x87 序列槽号 -0xec 族随之一并归位）。
    double framePad0;
    double framePad1;
    (void)&framePad0;
    (void)&framePad1;
    if (m_faces.empty())
    {
        return;
    }
    image->init(width, height, 0);

    // 噪点圆（ORIG 第一段循环）
    for (int i = 0; i < opt.m_field0; ++i)
    {
        int cx = m_rng() % (width - 10) + 5;
        int cy = m_rng() % (height - 10) + 5;
        int r = m_rng() % opt.m_field4 + 1;
        for (int dy = -r; dy <= r; ++dy)
        {
            for (int dx = -r; dx <= r; ++dx)
            {
                // ORIG 0x080debc7：先 `mov -0x80(dy),%eax; mov %eax,%edx;
                // imul -0x80,%edx` 再算 -0x7c(dx) 平方 ⇒ 源码首操作数是
                // dy*dy（P243 同族：首求值项=书写序左项；加法交换、语义等价）。
                int v = ((dy * dy + dx * dx) * 0xff) / (r * r);
                if (v < 0xff)
                {
                    image->setAlphaAsMax(cx + dx, cy + dy,
                                         (unsigned char)((0xff - v) / r));
                }
            }
        }
    }

    // 干扰线（ORIG 第二段循环）
    for (int i = 0; i < opt.m_field8; ++i)
    {
        int x1 = m_rng() % 10 + 1;
        int y1 = m_rng() % height;
        int x2 = (width - m_rng() % 10) - 2;
        int y2 = m_rng() % height;
        drawLine(image, x1, y1, x2, y2);
    }

    // 字符渲染（ORIG 第三段循环：随机面/旋转/缩放 -> FT 栅格 -> 粘贴）
    int penX = 10;
    int penY = 2;
    for (int i = 0; i < len; ++i)
    {
        unsigned int faceIdx = m_rng() % m_faces.size();
        // [b28d-solo-GlobalData] 按 ORIG 0x080dee9x..0x080df120 段还原真实
        // FreeType 栅格化序列（旧 static renderGlyph 占位为发明符号，已删）：
        // m_faces[idx] -> 4×getFloat(旋角/缩放) -> 2×镜像位 -> FT_Matrix 16.16
        // 定点（65536.0f=_DAT_08b2cab0）-> FT_Set_Transform -> FT_Get_Char_Index
        // -> FT_Load_Glyph -> FT_Render_Glyph(face->glyph,0) -> bitmap 遍历
        // （std::min<int>(width, width-penX) + setAlphaAsMax）-> penX 推进。
        FT_FaceRec* face = (FT_FaceRec*)m_faces[faceIdx];
        float rotA = RandGen::getFloat(&m_rng, opt.m_fieldc, opt.m_field10);
        float rotB = RandGen::getFloat(&m_rng, opt.m_fieldc, opt.m_field10);
        float sclA = RandGen::getFloat(&m_rng, opt.m_field14, opt.m_field18);
        float sclB = RandGen::getFloat(&m_rng, opt.m_field14, opt.m_field18);
        float mirX = (m_rng() & 2) ? 1.0f : -1.0f;
        float mirY = (m_rng() & 2) ? 1.0f : -1.0f;
        FT_Matrix matrix;
        matrix.xx = (FT_Fixed)(65536.0f * rotA);
        matrix.xy = (FT_Fixed)(65536.0f * sclA * mirX);
        matrix.yx = (FT_Fixed)(65536.0f * sclB * mirY);
        matrix.yy = (FT_Fixed)(65536.0f * rotB);
        FT_Set_Transform(face, &matrix, 0);
        unsigned int glyphIndex = FT_Get_Char_Index(face, text[i]);
        int err = FT_Load_Glyph(face, glyphIndex, 0);
        err = FT_Render_Glyph(face->glyph, 0);
        (void)err;

        int yOff = m_rng() % opt.m_field1c;
        FT_Bitmap* bm = &face->glyph->bitmap;
        for (int row = 0; row < (int)bm->rows; ++row)
        {
            int yy = penY + row + yOff;
            // ORIG 0x080df05c/0x080df068：`js 0x080df10d` + `jge 0x080df110`
            // 两个守卫各自落独立岛（岛形 `jmp <外层增量>; nop`×2，共 4 条）
            // ⇒ P223 正例形「负条件 goto 到共享尾空标签串，标签序=守卫序」
            // （if 正向包裹恒被 4.4 jump-thread 成 0 岛）。
            if (yy < 0)
            {
                goto rowSkip1;
            }
            if (yy >= height)
            {
                goto rowSkip2;
            }
            {
                int maxCols = width - penX;
                // ORIG 直接取 &bm->width 传 min<int>；`(int)` cast 会造右值
                // 临时（多 -0xa4 槽 + 两条 mov）⇒ 删 cast（width 已改 int）。
                int cols = std::min(bm->width, maxCols);
                for (int c = 0; c < cols; ++c)
                {
                    // ORIG 0x080df0xx：`movzbl (%eax),%eax; movzbl %al,%eax;
                    // mov %eax,-0x24(%ebp); cmpl $0,-0x24` ⇒ 局部是 **int**
                    // （P321 T1 双段零扩展 + 4 字节槽直证；uchar 形恒产
                    // `mov %al,槽; cmpb $0,槽`，探针 /tmp/gdprobe/p4.cpp
                    // f1/f3 vs f4 对照）。传参亦为 int→uchar 形
                    // （`mov -0x24,%eax; movzbl %al,%eax`，探针 p5.cpp）。
                    int a = bm->buffer[bm->pitch * row + c];
                    if (a != 0)
                    {
                        image->setAlphaAsMax(penX + c, yy, a);
                    }
                }
            }
rowSkip1:;
rowSkip2:;
        }
        penX += (int)(opt.m_field20 * (float)bm->width);
    }

    // 波形扭曲（ORIG 第四段：cos/sin 16.16 定点偏移 + 双线性采样）
    GreyImage tmp;
    tmp.init(width, height, 0);
    float freq = RandGen::getFloat(&m_rng, opt.m_field24, opt.m_field28);
    float amp = RandGen::getFloat(&m_rng, opt.m_field2c, opt.m_field30);
    // ORIG：默认构造 + resize(n, 0)（p167 MISSING `vector()`×2 + `resize`×2；
    // 我们原 `vector(n)` 产 EXTRA `vector(unsigned, int const&)` + allocator 族）。
    std::vector<int> cosTbl;
    cosTbl.resize((unsigned)width, 0);
    for (int x = 0; x < width; ++x)
    {
        cosTbl[x] = (int)(65536.0f * cosf((float)x * freq) * amp);
    }
    // ORIG call 序（0x080df0xx 段）：cosf 循环后才 `vector<int>::C1Ev` +
    // `resize` 构造 sinTbl（O=517/538/544 vs 我们 500/506/524 实证）⇒
    // sinTbl 的声明必须落在 cosTbl 循环之后（P331/P314 声明位置决定构造发射序）。
    std::vector<int> sinTbl;
    sinTbl.resize((unsigned)height, 0);
    for (int y = 0; y < height; ++y)
    {
        sinTbl[y] = (int)(65536.0f * sinf((float)y * freq) * amp);
    }
    for (int x = 0; x < width; ++x)
    {
        for (int y = 0; y < height; ++y)
        {
            tmp.setAlpha(x, y, (unsigned char)image->sampleAlpha(
                x * 65536 + sinTbl[y], y * 65536 + cosTbl[x]));
        }
    }
    memcpy(image->getImage(), tmp.getImage(), (size_t)width * height);
}

// s_capchaDataManager（ORIG 0x941f7a0；namespace GlobalData 成员）定义；
// 类型/方法见 CCapchaDataManager.h。
WongWork::CCapchaDataManager GlobalData::s_capchaDataManager;
#include "GlobalData/push.inc"
{
    // ORIG 0x08299f3a：push_back；超过 0x28 条则 pop_front
    m_queue.push_back(msg);
    if (m_queue.size() > 0x28)
    {
        m_queue.pop_front();
    }
}

#include "GlobalData/isEmpty.inc"
{
    // ORIG W 0x086977fa：deque::empty。
    return m_queue.empty();
}

#include "GlobalData/peekData.inc"
{
    // ORIG T 0x08299f74：at(get_rand_int(size()))——随机取一条。
    return &m_queue.at(static_cast<unsigned int>(
        get_rand_int(static_cast<int>(m_queue.size()))));
}

// ============================================================================
// 依赖类 stub 实现（GlobalData.cpp 自含声明的占位类；G2/G4/G6/G7/G8 批次接管）
// ============================================================================

#include "GlobalData/DBThread.inc"
{
    // ORIG 0x083ff5f2：先置 +0x0c=false，再隐式构造 CQueryCounter@+0x10。
}
#include "GlobalData/setLogDB.inc"
{
    // ORIG 0x083ff6c8：置标志后经成员回读；循环内经 isLogDB() 判定。
    m_bLogDB = logDB;
    m_queryCounter.setLogDB(m_bLogDB);
    for (int i = 0; i <= 0x10; ++i)
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)i,
                                                      (ENUM_SERVER_GROUP)0);
        if (isLogDB())
        {
            if (i == 4)
            {
                db->SetQueryCounter(&m_queryCounter);
            }
        }
        else if (i != 4)
        {
            db->SetQueryCounter(&m_queryCounter);
        }
    }
}
#include "GlobalData/isLogDB.inc"
{
    return m_bLogDB;
}
// ---------------------------------------------------------------------------
// dispatch —— ORIG T 0x083ff786：new DBTask（0xd54）后进入 DB 事件循环；
// HandleEvents 处理 DB_Q/LOGDB_Q（isLogDB 决定 2/4），CMessageCounter(20000)
// 按 600s 周期 WriteLog；循环条件 !GlobalData::s_server_stop。
// ---------------------------------------------------------------------------
#include "GlobalData/dispatch.inc"
{
    (void)arg;
    DBTask* task = new DBTask;
    if (task == 0)
    {
        std::cerr << "[DBThread::dispatch] Memory allocation fail!" << std::endl;
        OS_API::Exit(0);
    }

    // 形参类型精确匹配（不加调用处强制转换）：ORIG 0x083ff873 的 HandleEvents
    // 实参装载序为 `&signal → queueIdx → this`（右→左），4.4 在调用处写
    // `(MsgQueueMgr::QUEUE_IDX)queueIdx` 会把 queueIdx 的读取提前（寄存器换 %edx）。
    MsgQueueMgr::QUEUE_IDX queueIdx = (MsgQueueMgr::QUEUE_IDX)2;
    if (isLogDB())
    {
        queueIdx = (MsgQueueMgr::QUEUE_IDX)4;
    }
    else
    {
        // ORIG 0x083ff819：isLogDB 为假时仍有一条 `movl $0x2,-0x28(%ebp)`
        // 显式赋值（即源码写了冗余 else 分支），非 fall-through。
        queueIdx = (MsgQueueMgr::QUEUE_IDX)2;
    }

    message_counter::CMessageCounter msgCounter(20000);
    int curSec = 0;
    int lastWriteSec = 0;
    message_counter::CTimeInterval* interval =
        message_counter::CTimeInterval::Create();
    int signal = 0;

    while (!GlobalData::s_server_stop)
    {
        curSec = GlobalData::s_systemTime_.getCurSec();
        interval->SetLastTime();
        signal = 0;
        if (!task->HandleEvents(queueIdx, signal))
        {
            break;
        }
        if (signal != 0)
        {
            msgCounter.IncreaseMessage(
                (unsigned int)signal, (unsigned char)0, 0,
                interval->GetTimeInterval());
            if (curSec - lastWriteSec > 600)
            {
                lastWriteSec = curSec;
                msgCounter.WriteLog();
            }
        }
    }
    message_counter::CTimeInterval::Destory(interval);
    delete task;
}
#include "GlobalData/DispatchThread.inc"
#include "GlobalData/NetworkThread.inc"
#include "GlobalData/UdpThread.inc"
// g_bUdp_TH_start —— ORIG BSS 0x943e240；dispatch 置 1 标记 UDP 线程已启动。
bool g_bUdp_TH_start = false;

// ---------------------------------------------------------------------------
// CPacketCounter<1000,10240> —— ORIG 由本 TU（NetworkThread.cpp）实例化
// （C1 0x0857f4dc / D1 0x0857f55a / IncrementPacketCount 0x0857f560 /
// Reset 0x0857f92e，均为 W 弱符号；布局按 Reset/C1 反汇编偏移恢复）。
// ---------------------------------------------------------------------------
template<int Lo, int Hi>
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
{
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    // ORIG 0857c3be：栈槽 -0x20 sockaddr_in（memset 0x10）；写序 movw $2 @+0（family）
    // → htonl(0) 结果 @-0x1c（s_addr）→ htons 结果 @-0x1e（sin_port），见 _bindUDPSocket.md。
    struct sockaddr_in addr;
    memset(&addr, 0, 0x10);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(0);
    addr.sin_port = htons(port);
    if (bind(fd, reinterpret_cast<struct sockaddr*>(&addr), 0x10) < 0)
    {
        LogManager::logFormat(1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x84,
                              "[ERROR]UDP Thread Socket Binding ERROR");
        int err = *__errno_location();
        // ORIG 0x0857c4a2..：err 先 `mov %eax,-0xc(%ebp)` 再整段提升为
        // `mov -0xc(%ebp),%eax; cmp $0x62,%eax,je / cmp $0x63,%eax,je /
        // test %eax,%eax,je / jmp` 的判定链，三个 case 体依次后置 ⇒ 整数
        // switch 降形态（非 if/else-if 交错），case 0 为空实现。
        switch (err)
        {
        case 0x62:
            LogManager::logFormat(1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x9e,
                                  "Port %d for receiving UDP is in use\n",
                                  (int)(unsigned short)addr.sin_port);
            break;
        case 0x63:
            LogManager::logFormat(1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0xa2,
                                  "Cannot assign requested address\n");
            break;
        case 0:
            break;
        default:
            LogManager::logFormat(1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0xa6,
                                  "Could not bind UDP receive port. Error= %d , strerror = %s\n",
                                  err, strerror(err));
            break;
        }
        exit(-1);
    }
    if (!CNetwork<4096, 450000>::SetNonBlock(fd))
    {
        LogManager::logFormat(1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0xb0,
                              "Fail to SetNonBlock UdpThread.");
        exit(-1);
    }
    return fd;
}

// ---------------------------------------------------------------------------
// _processUDPData —— ORIG 0x0857c5a4：AddUdpLog → 静态计数 → dispatchPacket；
// 未识别包按首字节分流（0 互斥数据 / 1 换装队列 / 2 忽略 / 3 拍卖停止）。
// ---------------------------------------------------------------------------
// [b28d-18 N82] p167 CLEAR（51/51）；残差=帧槽（N82） | ORIG 0x0857c5a4
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（51/51）；残差=N82 槽位/寄存器 + N89。
// [G7-sub 2026-09-11 判词] UdpThread::_processUDPData（ORIG T 0x0857c5a4）：私有 .o 实测
// ORIG 267 / 我方 262（gap 5，我方短）。逐条对照（fdiff --counter --shape）后
// ORIG-only 项**全部**为：`nop`×3（对齐填充，§6.1 豁免）+ 符号/字符串重定位文本
// （我方 .o 侧无解析名）；无缺失语义指令。p167 调用点差集 CLEAR（51/51，且我方
// 3 处 CStreamGuard 序列与 ORIG 逐条同形：Acquire 直作 ctor 实参，无具名 stream 局部）。
// 结论：**封顶，无改动**（残差=对齐 nop + N82/N78 族）。
#include "GlobalData/_processUDPData.inc"
{
    // 跨服 UDP 包 = 10 字节 PacketHeader 线格式（shared/packet/include/PacketHeader.h）；
    // ORIG 0857c5ad：每处读都重取形参（mov 0xc(%ebp),%eax; movzwl (%eax)，P86 逐次重读）
    // → +0x00 ushort packetId。
    // [GlobalData-判据①] ORIG 0x0857c5b5 段：packetId 在 CPacketTracerInstance()
    // **之前**求值并升格 %ebx 跨 call 保活（`movzwl (%eax),%eax; movzwl %ax,%ebx;
    // call CPacketTracerInstance; mov %ebx,0x8(%esp)`）⇒ 原码是单语句链式实参
    // （P52/P226），无 `uVar1`/`tracer` 具名局部（旧形多 -0x36/-0x34 两槽 + 3 条）。
    CPacketTracerInstance()->AddUdpLog(
        3, (int)reinterpret_cast<PacketHeader*>(data)->packetId);

    static CPacketCounter<1000, 10240> packet_counter(
        G_CEnvironment()->get_file_name(), "InterDispatcher");
    packet_counter.IncrementPacketCount(
        (int)reinterpret_cast<PacketHeader*>(data)->packetId);

    // [GlobalData-判据①] ORIG 0x0857c68d `call dispatchPacket; test %al,%al;
    // jne +0x42a`（jne 落空 then 岛；无 `xor $1`）⇒ 原码是**正向 if + 空 then +
    // else 体**（探针 /tmp/l7/probe3.cpp t2 逐条同形）；旧写法 `if (!f()){体}`
    // （t1/t3/t4/t5）4.4 恒产 `xor $1,%eax; test %al,%al; je`，多一条 xor 且
    // 分支落点从空岛变成 epilogue 本体。
    if (CDispatchServerPacket::dispatchPacket(data))
    {
    }
    else
    {
        // [GlobalData-判据①] ORIG 0x0857c696 族逐次重读 `mov 0xc(%ebp),%eax;
        // movzbl (%eax),%eax; movsbl %al,%eax; cmp $0x1,%eax`（符号扩展后直接比，
        // 无栈槽往返）⇒ 原码**无具名 char 局部**（P0/P86 表达式直读）；旧写法
        // `char cVar2 = *data;` 产 `mov %al,-0x2d; cmpb $0x1,-0x2d` 槽化两段。
        // [L8 2026-09-10 判据①/P251/P261/判据⑨ 旧判推翻] ORIG 0x0857c696 段是
        // **switch(*data) 决策树**：`movzbl (%eax),%eax; movsbl %al,%eax;
        // cmp $0x1,%eax; je L63; cmp $0x1,%eax; jg L4; test %eax,%eax; je L7;
        // jmp L110; cmp $0x2,%eax; je L185; cmp $0x3,%eax; je L121`（case 1/0/2/3
        // + default 全 4 支）。旧 if/else 链 4.4 产 `cmp $0x1,%al; jne <case1>`
        // 二段形，结构与叶块序均不符 ⇒ 改 switch 复现（4.1 少 case 用线性比较链，P17）。
        switch (*data)
        {
        case 0:
        {
            char* local_24 = data;
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire("NetworkThread.cpp", 0xe8), true);
            **guard << 0x3d;
            **guard << -1;
            guard->put_binary(local_24, 0x52);
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            return;
        }
        // [L8 P324 叶块序] ORIG 决策树 `cmp $1;je +63`（case1 体远在 +63=i128）、
        // 树后紧跟 i76 起的是 **case 0 体** ⇒ 书写序 case0 在前、case1 在后。
        case 1:
        {
            // [L8 2026-09-10 P259] ORIG 0x0857c6ea 段：Acquire 返回值直接经 %eax 传
            // CStreamGuard ctor（`call Acquire; movl $0x1,0x8(%esp); mov %eax,0x4(%esp);
            // lea -0x28(%ebp),%eax; call ctor`），**无 stream 具名局部** ⇒ 我方
            // `Stream* stream = …` 每分支多发一个 4B 槽（×3 分支 = 0xc），把三个
            // CStreamGuard 槽整体顶深 0xc（我方 -0x34/-0x3c/-0x44 vs ORIG -0x28/-0x30/-0x38）
            // 并使帧 0x5c vs ORIG 0x4c。改为内联实参。
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire("NetworkThread.cpp", 0xf8), true);
            **guard << 0x9f;
            **guard << -1;
            // 线格式负载切块：header(+0x00..0x01) 之后原始字节，ORIG lea 0x2(%eax)（P156 豁免）。
            guard->put_binary(data + 2, len);
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            break;
        }
        case 2:
            return;
        case 3:
        {
            GlobalData::s_auction_proxy->SetRunning(false);
            GlobalData::s_cera_auction_proxy->SetRunning(false);
            char* local_20 = data;
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire("NetworkThread.cpp", 0x126), true);
            **guard << 0x3d;
            **guard << -1;
            guard->put_binary(local_20, 0x52);
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            return;
        }
        default:
            break;
        }
        if (reinterpret_cast<PacketHeader*>(data)->packetId < 1000)
        {
            LogManager::logFormat(1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x112,
                                  "ABNORMAL UDP PACKET [%d]", (int)*data);
        }
    }
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
// [G7-sub 2026-09-11 判词] UdpThread::dispatch（ORIG T 0x0857c9de）：私有 .o 实测
// ORIG 138 / 我方 136（gap 2，我方短）。逐条对照残差 = 三类：
//   ① ORIG 0x0857c9ea `movl $0x1,-0x38(%ebp)` 与 0x0857ca1a `movl $0x0,-0x30(%ebp)`
//      是两处**只写不读**的具名 dead 标量（全函数无第二处引用）；是否补 dead 局部
//      由父代理裁决 ⇒ 本泳道**不改**（补则帧内 tv_sec/tv_usec/maxfd 槽由
//      -0x3c/-0x38/-0x34 下移到 ORIG 的 -0x44/-0x40/-0x48）。
//   ② FD_ZERO 内层 ORIG `movl $0,(%eax,%edx,4)` scaled 直存 vs 我方 `shl $2; add` ⇒
//      已知 N125/N81（4.4 下标缩放恒折 shl）。
//   ③ 尾 `nop`（§6.1 对齐豁免）。
// p167 调用点差集 CLEAR（8/8）。结论：**封顶，无改动**（语义完成）。
// [GDataWriter 2026-09-11 重开 · 槽位大幅对齐] 按 P212/P309「函数头声明序控标量槽」
// 把 5 个标量前置声明（maxfd,nUnusedA,recvLen,nUnusedB,nfds）+ 聚合（buf/readfds/
// timeout/sockets）后置，并补 P171 两个 dead 标量（nUnusedA=1/nUnusedB=0，赋值为独立
// 语句）。实测槽位：nUnusedA -0x38✓ / nUnusedB -0x30✓ / recvLen -0x34✓ / nfds -0x2c✓ /
// timeout -0x44✓ / maxfd -0x3c✓(对齐)；**余 sockets(-0x48)↔maxfd(-0x3c) 互换**（第 2 试
// 把 sockets 提前声明 ⇒ timeval 反被顶到 -0x48，更劣，已回滚）= 4.4 聚合优先占低槽
// （N82附）不可翻。读数 137/138 不变（余 1 条 = FD_ZERO scaled 直存，N125/N81）。
#include "GlobalData/dispatch_1.inc"
{
    (void)arg;
    // [GDataWriter 2026-09-11] 槽位重排试验（P212/P309：函数头声明序控标量槽向）：
    // ORIG 标量序（深→浅）= maxfd(-0x48) / nUnusedA(-0x38) / recvLen(-0x34) /
    // nUnusedB(-0x30) / nfds(-0x2c)；旧声明序产出 sockets(-0x48)/maxfd(-0x3c) 互换。
    // 标量全部前置声明（先声明者占深槽），聚合（buf/readfds/timeout/sockets）后置。
    // [GDataWriter 2026-09-11 第 2 试负结论] 把数组 sockets 提到 maxfd 之前声明
    // ⇒ timeval 反被顶到 -0x48（`movl $0x1,-0x48`），更劣。4.4 聚合优先占低槽
    // （N82附）不可由声明序翻 ⇒ sockets(-0x48)↔maxfd(-0x3c) 互换封顶。
    int maxfd;
    int nUnusedA;
    int recvLen;
    int nUnusedB;
    int nfds;
    char buf[0x1800];
    fd_set readfds;
    struct timeval timeout;
    int sockets[1];

    // [P171] ORIG 0x0857c9ea `movl $0x1,-0x38(%ebp)` 与 0x0857ca1a
    // `movl $0x0,-0x30(%ebp)`：两处只写不读的具名 dead 标量（全函数无第二
    // 处引用），照抄原码死写；补后栈槽整体下移 8，与 ORIG 帧内布局一致。
    nUnusedA = 1;
    sockets[0] = _bindUDPSocket((unsigned short)G_CEnvironment()->get_udp_port());
    g_bUdp_TH_start = true;
    nUnusedB = 0;
    while (!GlobalData::s_server_stop)
    {
        memset(buf, 0, sizeof(buf));
        // FD_ZERO(&readfds) 的 ORIG 4.1-glibc 展开形（逐元素循环；见函数头判词）。
        // [L8] 槽序：ORIG p(-0x24)/i(-0x28)；且 ORIG 尾测为 `cmpl $0x1f,-0x28;
        // setbe`（**无符号**比较）⇒ 计数变量须 unsigned（我方 int 版产 setle）。
        unsigned int fi;
        int* fdBits = (int*)&readfds;
        for (fi = 0; fi <= 0x1f; ++fi)
            fdBits[fi] = 0;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        maxfd = 0;
        for (int i = 0; i <= 0; ++i)
        {
            // [L8] ORIG 0x0857ca90 段 `mov -0x3c(%ebp,%eax,4),%eax; shr $0x5,%eax`
            // 为**逻辑**右移（无 `lea 0x1f/test/cmovs` 修正）⇒ FD_SET 的实参须按
            // unsigned 参与 `__FDELT(d)=d/32` 折叠；而 max 侧按 int 左值传址
            // （0x0857cac6 `lea -0x3c(%ebp)`）⇒ 各处分别 cast。
            FD_SET((unsigned int)sockets[i], &readfds);
            maxfd = std::max(maxfd, sockets[i]);
        }
        maxfd += 1;
        nfds = select(maxfd, &readfds, NULL, NULL, &timeout);
        if (nfds < 0)
        {
            continue;
        }
        for (int i = 0; i <= 0; ++i)
        {
            if (FD_ISSET((unsigned int)sockets[i], &readfds))
            {
                recvLen = recv(sockets[i], buf, sizeof(buf), 0);
                if (recvLen > 0)
                {
                    _processUDPData(buf, recvLen);
                }
            }
        }
    }
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
// [G7-sub 2026-09-11 判词] DispatchThread::dispatch（ORIG T 0x0847e2d2）：私有 .o 实测
// ORIG 325 / 我方 325（gap 0），**助记符序列全等**（fdiff --mnemonic hunks=0），
// 残差仅寄存器/栈槽分配（N82 族）。p167 调用点差集仅 cMyTrace C1↔C2 同址别名
// （ORIG nm 实测 C1=C2=0x0854f746）。结论：**封顶，无改动**（本批最干净件）。
#include "GlobalData/dispatch_2.inc"
{
    (void)arg;
    TimerEntry entry;
    unsigned char typeByte = 0;      // 首字节（客户端包分流码）
    unsigned short sizeWord = 0;     // +1 起的 ushort
    message_counter::CMessageCounter msgCounter(10000);
    // ORIG 0847e322/0847e7xx：lastWriteSec（-0x34）循环外置 0，curSec（-0x30）
    // 循环外以 getCurSec() 初始化；循环内更新 lastWriteSec，比较
    // `lastWriteSec - curSec > 600` 后回写 curSec（两次 getCurSec 调用）。
    int lastWriteSec = 0;
    int curSec = GlobalData::s_systemTime_.getCurSec();
    message_counter::CUserPacketCounter userCounter(
        G_CEnvironment()->get_file_name(), "UserPacketCounter");
    PacketDispatcher::pUserPacketCounter_ = &userCounter;
    message_counter::CTimeInterval* interval =
        message_counter::CTimeInterval::Create();
    // ORIG 0x0847e2d2 函数头无 `movl $0x0,-0x2?` 预清 ⇒ 原码裸声明（P0）。
    int code;   // 流头读出的分发码
    int idx;    // 流头读出的用户 id

    while (!GlobalData::s_server_stop)
    {
        GlobalData::s_systemTime_.update();
        if (G_TimerQueue()->GetTimerMess(entry))
        {
            interval->SetLastTime();
            G_TimerDispatcher()->dispatch(entry);
            msgCounter.IncreaseMessage(
                0, (unsigned char)0,
                (unsigned short)((short)entry.msg + 30000),
                interval->GetTimeInterval());
        }

        Stream* stream = GlobalData::s_msgq_mgr->get(MsgQueueMgr::DISPATCH_Q);
        if (stream == 0)
        {
            OS_API::uSleep(100);
            continue;
        }
        int length = stream->length();
        int streamType = stream->m_poolType;
        *stream >> code;
        *stream >> idx;
        lastWriteSec = GlobalData::s_systemTime_.getCurSec();
        interval->SetLastTime();
        if (code == 0)
        {
            // 客户端包线格式头：get_buffer()+0 type(uchar)/+1 size(ushort)。
            // ORIG 0847e496..0847e4b2（DispatchThread/dispatch.md）`movzbl (%eax)`
            // / `add $0x1; movzwl (%eax)` 直译；MSG_BASE 权威头为 13B 占位 blob
            // （内部字段无证据，CMDPacketStruct.h 注），布局未建模，P156 豁免。
            typeByte = (unsigned char)*stream->get_buffer();
            sizeWord = *(unsigned short*)(stream->get_buffer() + 1);
            G_PacketDispatcher()->dispatch(
                idx, stream->get_buffer(), stream->length());
        }
        else
        {
            typeByte = 0;
            sizeWord = 0;
            G_InterDispatcher()->dispatch(
                code, idx, stream->get_buffer(), stream->length());
        }
        msgCounter.IncreaseMessage(
            (unsigned int)code, typeByte, sizeWord,
            interval->GetTimeInterval());
        if (lastWriteSec - curSec > 600)
        {
            curSec = lastWriteSec;
            msgCounter.WriteLog();
            userCounter.WriteLog();
            // ORIG 0x0847e???：varargs 在 cMyTrace ctor 前求值并落 callee-saved
            // （N80/P328⑩）⇒ 单语句全表达式临时链。
            cMyTrace(__PRETTY_FUNCTION__, 0x8f, 5)(
                "timer_queue_count(%u)", G_TimerQueue()->GetQueueSize());
            G_TimerQueue()->write_timer_queue_log();
        }
        switch (streamType)
        {
        // [GlobalData-判据①] ORIG 0x0847e61d 决策树 `cmp $1;je +0x35f;
        // cmp $2;je +0x442; test %eax,%eax;je +0x3d2; jmp +0x4b0`：
        //   case 1 → +0x35f（0xfff / "Small…" / s_small_stream_pool）
        //   case 0 → +0x3d2（0x3fff / "Default…" / s_stream_pool）★旧源码把
        //            case 0 并入 case 1（错发 SmallStreamPool::Free），且把
        //            s_stream_pool 体挂在 default（错：default 应为空体
        //            +0x4b0 直落循环尾）。
        //   case 2 → +0x442（0x3ffff / "Big…" / s_big_stream_pool）
        // 叶块物理序（P324）= 书写序 = 1, 0, 2；default 空体。
        case 1:
            if (0xfff < length)
            {
                cMyTrace(__PRETTY_FUNCTION__, 0x9e, 5)(
                    "Small Stream Size over code(%d), idx(%d), size(%u)",
                    code, idx, length);
            }
            GlobalData::s_small_stream_pool->Free(stream);
            break;
        case 0:
            if (0x3fff < length)
            {
                cMyTrace(__PRETTY_FUNCTION__, 0xa8, 5)(
                    "Default Stream Size over code(%d), idx(%d), size(%u)",
                    code, idx, length);
            }
            GlobalData::s_stream_pool->Free(stream);
            break;
        case 2:
            if (0x3ffff < length)
            {
                cMyTrace(__PRETTY_FUNCTION__, 0xb2, 5)(
                    "Big Stream Size over code(%d), idx(%d), size(%u)",
                    code, idx, length);
            }
            GlobalData::s_big_stream_pool->Free(stream);
            break;
        default:
            break;
        }
    }
    message_counter::CTimeInterval::Destory(interval);
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
// [G7-sub 2026-09-11 判词] NetworkThread::dispatch（ORIG T 0x0857d5be）：私有 .o 实测
// ORIG 208 / 我方 208（gap 0）。助记符差 2 处，均为 `nop`：我方函数中段 1 条对齐 nop、
// ORIG 尾 1 条（§6.1 纯对齐填充豁免）；无缺失/多余指令、无调用点差
// （p167 仅 cMyTrace C1↔C2 同址别名 0x0854f746）。结论：**封顶，无改动**。
#include "GlobalData/dispatch_3.inc"
{
    (void)arg;
    // [b28d-solo-GlobalData] P183：ORIG 0x0857d5c6 无 epoll 具名局部
    // （`call G_EpollHandler; mov %eax,(%esp); call GetEpollEvents`）。
    epoll_event* events = G_EpollHandler()->GetEpollEvents();
    int epollfd = G_EpollHandler()->GetEpollFD();
    // ORIG 0x0857d5e6/0x0857d5ed：循环前 `movl $0,-0x1c` / `movl $0,-0x18`
    // 两处槽清零 ⇒ 原码在外层 for 用的 CUser* 与 nfds 均为**函数级具名局部**
    // 并在声明处初始化（P0 槽往返直证）。当前缺这两条 store ⇒ 槽图整体上移。
    CUser* user = 0;
    int nfds = 0;

    // [b28d-solo-GlobalData] ORIG 0x0857d89d：s_server_stop 检查在**循环尾**
    // （`movzbl s_server_stop; xor $1,%eax; test %al,%al; jne body`，+0x2df 即
    // 回边目标），trace 落循环外函数尾 ⇒ 原码 `while (!stop) { body } trace;`。
    // 循环头 `if(stop){trace;return;}` 会把检查搬到头部并多出 movzbl/test 段。
    while (!GlobalData::s_server_stop)
    {
        *__errno_location() = 0;
        OS_API::Sleep(10);

        if (!GlobalData::s_accept_que.empty())
        {
            GlobalData::s_accept_lock.lock();
            // ORIG 0x0857d633：内层 front 结果槽 -0x14 在 lock 后先清零
            // （P0 具名局部，声明在 while 之前）。
            CUser* quser = 0;
            while (!GlobalData::s_accept_que.empty())
            {
                quser = GlobalData::s_accept_que.front();
                // ORIG 0x0857d650：`movl $0,-0x10` 先于 SetEpoll 调用
                // ⇒ 预清必写两句式（P198①），非初始化式。
                int err = 0;
                err = G_EpollHandler()->SetEpoll(
                    quser, quser->m_network.get_socket(), false);
                // ORIG 0x0857d68c：`cmpl $0,-0x10; setne %al; test %al,%al;
                // je` ⇒ P225 物化形（`while(v!=0){body;break;}`；if 形恒折直跳）。
                while (err != 0)
                {
                    LogManager::logFormat(
                        1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x2c4,
                        "(m_id: %s) G_EpollHandler()->SetEpoll(pUser->get_socket(%d)) %d(%s)",
                        NumberToString(quser->get_acc_id(), 0),
                        quser->m_network.get_socket(), err, strerror(err));
                    break;
                }
                GlobalData::s_accept_que.pop();
            }
            GlobalData::s_accept_lock.unlock();
        }

        nfds = epoll_wait(epollfd, events, 600, 100);
        if (nfds == 0)
        {
            continue;
        }
        if (nfds < 0 && errno != EINTR && errno != 0)
        {
            return;
        }
        for (int i = 0; i < nfds; ++i)
        {
            user = (CUser*)events[i].data.ptr;
            if (user == 0)
            {
                continue;
            }
            if (events[i].events & 1)
            {
                user->Recv();
            }
            if (user->m_network.m_sendSize != 0 && (events[i].events & 4))
            {
                // ORIG 0x0857d80b：`cmp $0x6ddd0,%eax; jbe 0857d837`（<= 跳
                // send_packet，DisConnSig 块 fall-through 在前）⇒ 源形是
                // 正极性 `if (>){DisConn}else{send}`（P47/P220 布局翻转）。
                if (user->m_network.m_sendSize > 0x6ddd0)
                {
                    user->DisConnSig((DISCONN_SIG)6, false, 0);
                }
                else
                {
                    user->m_network.send_packet();
                }
            }
            if (events[i].events & 0x18)
            {
                user->DisConnSig((DISCONN_SIG)9, false, 0);
            }
        }
    }
    // ORIG 0x0857d8af：循环退出后统一 trace + 落函数尾（P223 尾岛 nop）。
    cMyTrace trace(__PRETTY_FUNCTION__, 0x2fe, 0);
    trace("RecvThread Terminate");
}
#include "GlobalData/MonitorTcpThread.inc"
#include "GlobalData/AcceptThread.inc"
{
    // ORIG 0x0857cbe8：置 +0x0c 标志为 false。
    m_acceptFlag = false;
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
// [G7-sub 2026-09-11 判词] MonitorTcpThread::dispatch（ORIG 0x0857d95a，1575 条巨件，
// §6.2 三件套）：本轮**已改 2 类共 6 处**（1592→1577，gap +17→+2）：
//   ① 0x0857e089..0x0857e254 第二循环 4 处条件由 `&&` 改**嵌套 if**：ORIG 外条件发
//      `cmp $0xffffffff,%eax; setne %al; test %al,%al; je`、内层 FD_ISSET 直测
//      （探针 /tmp/g7c/probe4.cpp c1≡ORIG；`&&` 形 c2 恒折 `cmp; je` 且末操作数再发
//      `mov $1/jmp/mov $0/test` 四尾布尔物化岛）。
//   ② 0x0857da91..0x0857daa2 / 0x0857dd5e / 0x0857e7d3：原码为**函数级单变量 i**
//      （先独立 `i = 0;` 再 `for (i = 0; …)` ⇒ 连续两条 `movl $0,-0x58`），且两循环
//      条件内**逐次重调** `CServerProxyMgr<CMonitorServerProxy>::GetServerProxyArraySize()`
//      （该 getter 全函数仅 2 处调用，都在条件里）⇒ 删 `proxyCount` 具名缓存
//      （旧形多 1 前置 call + -0x50 槽，并把两循环索引拆成 -0x3c/-0x38 两槽）。
// 残留 2 条（均为 4.4 固有，探针实证不可翻）：
//   · 0x0857e3xx `while (ret < 0)`：4.4 发 `mov; shr $0x1f; test %al; jne` vs ORIG
//     `mov; test %eax,%eax; js`（probe5.cpp p1）。
//   · 0x0857e2xx `getsockopt(...,&soLen[3])`：4.4 发 `lea; add $0xc` vs ORIG 折单条
//     `lea`（probe5.cpp p2/p3）。
// p167：CALLSITE CLEAR（仅 C1/C2·D1/D2 同址别名，ORIG nm 实测如 0x0861c8d2）。
// 结论：语义完成，byte 层余 2 条 4.4 固有差 ⇒ 封顶。
#include "GlobalData/dispatch_4.inc"
{
    (void)arg;
    fd_set readfds;
    fd_set writefds;
    socklen_t soLen[4];
    struct timeval timeout;
    int soError = 0;
    // [SweepGD] ORIG 0x0857d966..0x0857d989 函数头只有 **5 条** `movl $0,-N`
    // （soError + lastReconnect×4），其余标量为**无初值裸声明**（-O0 裸声明零指令，
    // P328⑤）⇒ 去掉 socket/proxyCount/ret 的 `= 0`（我方原多 7 条清零）。
    int ret;
    int lastReconnectPcroom = 0;
    int lastReconnectCommunity = 0;
    int lastReconnectAuction = 0;
    int lastReconnectCera = 0;
    int pcroomSocket;
    int communitySocket;
    int auctionSocket;
    int ceraSocket;
    int hadesSocket;
    // [G7-sub] 原 `int proxyCount;` 已删：ORIG 循环条件内逐次重调
    // GetServerProxyArraySize（见下），无缓存局部。索引 `i` 为**函数级单变量**
    // （两循环同用 -0x58 槽：第一循环 12 次 + 第二循环 16 次引用）。
    int i;

    // [SweepGD] ORIG +0x1a32 尾条件块 `movzbl s_server_stop; xor $0x1; test %al;
    // jne <body>`（函数头 `jmp +0x1a32`）⇒ 原码是 `while (!s_server_stop)` 布尔字段
    // 裸条件（P168a①/P292），而非 `while(true){ if(stop) return; }`（后者头部多 4 个
    // jmp+nop 中继岛）。
    while (!GlobalData::s_server_stop)
    {
        do
        {
            OS_API::Sleep(1);
            pcroomSocket =
                GlobalData::s_pcroom_proxy->GetTcpHandler()->get_socket();
            communitySocket =
                GlobalData::s_community_proxy->GetTcpHandler()->get_socket();
            auctionSocket =
                GlobalData::s_auction_proxy->GetTcpHandler()->get_socket();
            ceraSocket =
                GlobalData::s_cera_auction_proxy->GetTcpHandler()->get_socket();
            hadesSocket =
                GlobalData::s_hades_proxy->GetTcpHandler()->get_socket();

            // [SweepGD 2026-09-10] ORIG 0x0857da21/0x0857da4f 的 FD_ZERO 展开 = 旧
            // glibc 逐元素循环（`lea fd; mov %eax,-0x50; movl $0,-0x54; …;
            // movl $0,(%eax,%edx,4); addl $1; cmpl $0x1f,-0x54; setbe`）——fd_set*
            // 具名局部 + unsigned 计数局部两槽；新头集宏走 `rep stos` 完全无形
            // ⇒ 手写等价形（同 UdpThread::dispatch 先例）。
            {
                unsigned int fi;
                fd_set* fds = &readfds;
                for (fi = 0; fi <= 0x1f; ++fi)
                    __FDS_BITS(fds)[fi] = 0;
            }
            {
                unsigned int fi;
                fd_set* fds = &writefds;
                for (fi = 0; fi <= 0x1f; ++fi)
                    __FDS_BITS(fds)[fi] = 0;
            }
            timeout.tv_sec = 1;
            timeout.tv_usec = 0;
            int maxfd = 0;
            // [G7-sub] ORIG 0x0857da91..0x0857daa2 是**连续两条** `movl $0,-0x58(%ebp)`
            // 后才 `jmp` 条件 ⇒ 原码一个具名 `int i`（先独立 `int i = 0;`、再
            // `for (i = 0; …)`）；且循环条件内**逐次重调**
            // `call 0857f8c2 <CServerProxyMgr<CMonitorServerProxy>::GetServerProxyArraySize()>`
            // （0x0857dd66 / 第二循环 0x0857e7d3 —— 该 getter 全函数仅此 2 处，
            // 都在条件里）⇒ 禁 `proxyCount` 具名缓存（旧形多 1 次前置 call + 1 个
            // -0x50 槽，且把两循环索引拆成 -0x3c/-0x38 两槽 ⇒ 与 ORIG -0x58 单槽不符）。
            i = 0;
            for (i = 0;
                 i < GlobalData::s_monitor_proxy_mgr->GetServerProxyArraySize(); ++i)
            {
                // ORIG 0x0857daae..0x0857dc04（p167 实证：每 mgr 6 次三元链）：
                // FD_SET/FD_ISSET 的 fd 实参逐处重调
                // GetServerProxyArrayAt(i)->GetTcpHandler()->get_socket()
                // （P157 逐字段重读，禁 mon/s 缓存；glibc FD_SET 宏对 fd 双求值）。
                if (GlobalData::s_monitor_proxy_mgr->GetServerProxyArrayAt(i)
                        ->GetTcpHandler()
                        ->get_socket() != -1)
                {
                    FD_SET((unsigned)GlobalData::s_monitor_proxy_mgr
                               ->GetServerProxyArrayAt(i)
                               ->GetTcpHandler()
                               ->get_socket(),
                           &readfds);
                    FD_SET((unsigned)GlobalData::s_monitor_proxy_mgr
                               ->GetServerProxyArrayAt(i)
                               ->GetTcpHandler()
                               ->get_socket(),
                           &writefds);
                    maxfd = std::max(
                        maxfd, GlobalData::s_monitor_proxy_mgr
                                   ->GetServerProxyArrayAt(i)
                                   ->GetTcpHandler()
                                   ->get_socket());
                }
                if (GlobalData::s_guild_proxy_mgr->GetServerProxyArrayAt(i)
                        ->GetTcpHandler()
                        ->get_socket() != -1)
                {
                    FD_SET((unsigned)GlobalData::s_guild_proxy_mgr
                               ->GetServerProxyArrayAt(i)
                               ->GetTcpHandler()
                               ->get_socket(),
                           &readfds);
                    FD_SET((unsigned)GlobalData::s_guild_proxy_mgr
                               ->GetServerProxyArrayAt(i)
                               ->GetTcpHandler()
                               ->get_socket(),
                           &writefds);
                    maxfd = std::max(
                        maxfd, GlobalData::s_guild_proxy_mgr
                                   ->GetServerProxyArrayAt(i)
                                   ->GetTcpHandler()
                                   ->get_socket());
                }
            }
            if (pcroomSocket != -1)
            {
                // [SweepGD] ORIG 0x0857dc34 段 `shr $0x5` / `and $0x1f`（无符号）⇒
                // 宏实参 (unsigned) cast（同 AcceptThread::dispatch 先例）消有符号除链。
                FD_SET((unsigned)pcroomSocket, &readfds);
                FD_SET((unsigned)pcroomSocket, &writefds);
                maxfd = std::max(maxfd, pcroomSocket);
            }
            if (communitySocket != -1)
            {
                FD_SET((unsigned)communitySocket, &readfds);
                FD_SET((unsigned)communitySocket, &writefds);
                maxfd = std::max(maxfd, communitySocket);
            }
            if (auctionSocket != -1)
            {
                FD_SET((unsigned)auctionSocket, &readfds);
                FD_SET((unsigned)auctionSocket, &writefds);
                maxfd = std::max(maxfd, auctionSocket);
            }
            if (ceraSocket != -1)
            {
                FD_SET((unsigned)ceraSocket, &readfds);
                FD_SET((unsigned)ceraSocket, &writefds);
                maxfd = std::max(maxfd, ceraSocket);
            }
            if (hadesSocket != -1)
            {
                FD_SET((unsigned)hadesSocket, &readfds);
                FD_SET((unsigned)hadesSocket, &writefds);
                maxfd = std::max(maxfd, hadesSocket);
            }
            maxfd += 1;
            ret = select(maxfd, &readfds, &writefds, 0, &timeout);
        } while (ret < 0);

        // [G7-sub] 与第一循环同一个具名 `i`（ORIG 全函数 -0x58 单槽 28 次引用），
        // 条件内重调 size（ORIG 0x0857e7d3 = 第二处也是唯一另一处调用）。
        for (i = 0;
             i < GlobalData::s_monitor_proxy_mgr->GetServerProxyArraySize(); ++i)
        {
            // ORIG 0x0857e089..0x0857e254：select 后同样逐处重调三元链
            // （if 1 次 + FD_ISSET 双求值 2 次；OnTcpRecv 前仅 1 次
            // GetServerProxyArrayAt，无 GetTcpHandler）。
            // [G7-sub] ORIG 0x0857e089 段是**嵌套 if**（非 `&&`）：外条件发
            // `cmp $0xffffffff,%eax; setne %al; test %al,%al; je`（probe
            // /tmp/g7c/probe4.cpp c1 与 ORIG 逐条同形；`&&` 形 c2 恒折成
            // `cmp; je`）⇒ 内层 FD_ISSET 直接 `and $1; test %al,%al; je`；
            // 旧 `&&` 写法另发 `mov $1/jmp/mov $0/test` 四尾布尔物化岛。
            if (GlobalData::s_monitor_proxy_mgr->GetServerProxyArrayAt(i)
                        ->GetTcpHandler()
                        ->get_socket() != -1)
            {
                if (FD_ISSET((unsigned)GlobalData::s_monitor_proxy_mgr
                                 ->GetServerProxyArrayAt(i)
                                 ->GetTcpHandler()
                                 ->get_socket(),
                             &readfds))
                {
                    // b28c-l23 SIGFIX 消费点：OnTcpRecv 已 bool 化（lane28/lane12），
                    // [SweepGD] 实参 (unsigned) cast：ORIG `shr $0x5`/`and $0x1f` 无符号形。
                    // bool callee ⇒ 裸形（P168a①：ORIG `call; xor$1; testb; je`）。
                    if (!GlobalData::s_monitor_proxy_mgr
                             ->GetServerProxyArrayAt(i)
                             ->OnTcpRecv())
                    {
                        LogManager::logFormat(
                            1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x449,
                            "Monitor::OnTcpRecv == false!!!");
                        CStreamGuard guard(
                            GlobalData::s_stream_pool->Acquire(
                                "NetworkThread.cpp", 0x44c),
                            true);
                        **guard << 0x15f;
                        **guard << -1;
                        **guard << 0xc9;
                        GlobalData::s_msgq_mgr->put(
                            MsgQueueMgr::DISPATCH_Q, guard);
                    }
                }
            }
            if (GlobalData::s_monitor_proxy_mgr->GetServerProxyArrayAt(i)
                        ->GetTcpHandler()
                        ->get_socket() != -1)
            {
                // [G7-sub] 同 readfds 段：ORIG 0x0857e1a2 嵌套 if（外条件
                // `setne;test;je`），内层 FD_ISSET 直测。
                if (FD_ISSET((unsigned)GlobalData::s_monitor_proxy_mgr
                                 ->GetServerProxyArrayAt(i)
                                 ->GetTcpHandler()
                                 ->get_socket(),
                             &writefds))
                {
                    int sendRet = GlobalData::s_monitor_proxy_mgr
                                      ->GetServerProxyArrayAt(i)
                                      ->OnTcpSend();
                    if (sendRet < 0)
                    {
                        LogManager::logFormat(
                            1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x45e,
                            "Monitor::OnTcpSend == false!!!remain_size(%d)",
                            sendRet);
                        CStreamGuard guard(
                            GlobalData::s_stream_pool->Acquire(
                                "NetworkThread.cpp", 0x460),
                            true);
                        **guard << 0x15f;
                        **guard << -1;
                        **guard << 0xc9;
                        GlobalData::s_msgq_mgr->put(
                            MsgQueueMgr::DISPATCH_Q, guard);
                    }
                }
            }
            if (GlobalData::s_guild_proxy_mgr->GetServerProxyArrayAt(i)
                        ->GetTcpHandler()
                        ->get_socket() != -1)
            {
                // [G7-sub] 同 monitor 段：ORIG 嵌套 if（外条件 setne;test;je）。
                if (FD_ISSET((unsigned)GlobalData::s_guild_proxy_mgr
                                 ->GetServerProxyArrayAt(i)
                                 ->GetTcpHandler()
                                 ->get_socket(),
                             &readfds))
                {
                    // b28c-l23 SIGFIX 消费点：bool callee ⇒ 裸形（P168a①）
                    if (!GlobalData::s_guild_proxy_mgr
                             ->GetServerProxyArrayAt(i)
                             ->OnTcpRecv())
                    {
                        LogManager::logFormat(
                            1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x46f,
                            "Guild::OnTcpRecv == false!!!");
                        CStreamGuard guard(
                            GlobalData::s_stream_pool->Acquire(
                                "NetworkThread.cpp", 0x471),
                            true);
                        **guard << 0x15f;
                        **guard << -1;
                        **guard << 0xcb;
                        GlobalData::s_msgq_mgr->put(
                            MsgQueueMgr::DISPATCH_Q, guard);
                    }
                }
            }
            if (GlobalData::s_guild_proxy_mgr->GetServerProxyArrayAt(i)
                        ->GetTcpHandler()
                        ->get_socket() != -1)
            {
                // [G7-sub] 同 monitor 段：ORIG 嵌套 if（外条件 setne;test;je）。
                if (FD_ISSET((unsigned)GlobalData::s_guild_proxy_mgr
                                 ->GetServerProxyArrayAt(i)
                                 ->GetTcpHandler()
                                 ->get_socket(),
                             &writefds))
                {
                    int sendRet = GlobalData::s_guild_proxy_mgr
                                      ->GetServerProxyArrayAt(i)
                                      ->OnTcpSend();
                    if (sendRet < 0)
                    {
                        LogManager::logFormat(
                            1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x480,
                            "Guild::OnTcpSend == false!!!remain_size(%d)",
                            sendRet);
                        CStreamGuard guard(
                            GlobalData::s_stream_pool->Acquire(
                                "NetworkThread.cpp", 0x481),
                            true);
                        **guard << 0x15f;
                        **guard << -1;
                        **guard << 0xcb;
                        GlobalData::s_msgq_mgr->put(
                            MsgQueueMgr::DISPATCH_Q, guard);
                    }
                }
            }
        }

        // HADES 代理
        if (hadesSocket != -1 &&
            GlobalData::s_hades_proxy->GetSocketState() ==
                ENUM_SOCKET_STATE_CONNECTED &&
            FD_ISSET((unsigned)hadesSocket, &readfds))
        {
            if (!GlobalData::s_hades_proxy->OnTcpRecv())
            {
                LogManager::logFormat(
                    1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x493,
                    "HADES::OnTcpRecv == false!!!");
            }
        }
        if (hadesSocket != -1 && FD_ISSET((unsigned)hadesSocket, &writefds))
        {
            int sendRet = GlobalData::s_hades_proxy->OnTcpSend();
            if (sendRet < 0)
            {
                LogManager::logFormat(
                    1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x49d,
                    "HADES::OnTcpSend == false!!!remain_size(%d)", sendRet);
            }
        }

        // PCROOM 代理
        if (pcroomSocket != -1 &&
            GlobalData::s_pcroom_proxy->GetSocketState() ==
                ENUM_SOCKET_STATE_CONNECTED &&
            FD_ISSET((unsigned)pcroomSocket, &readfds))
        {
            if (!GlobalData::s_pcroom_proxy->OnTcpRecv())
            {
                lastReconnectPcroom =
                    GlobalData::s_systemTime_.getCurSec();
                LogManager::logFormat(
                    1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x4e0,
                    "PCROOM::OnTcpRecv == false!!!");
            }
        }
        if (pcroomSocket != -1 && FD_ISSET((unsigned)pcroomSocket, &writefds))
        {
            if (GlobalData::s_pcroom_proxy->GetSocketState() ==
                ENUM_SOCKET_STATE_CONNECTING)
            {
                soError = 0;
                soLen[3] = 4;
                ret = getsockopt(pcroomSocket, SOL_SOCKET, SO_ERROR,
                                 &soError, &soLen[3]);
                if (soError == 0 && ret == 0)
                {
                    GlobalData::s_pcroom_proxy->SetSocketState(
                        ENUM_SOCKET_STATE_CONNECTED);
                }
            }
            int sendRet = GlobalData::s_pcroom_proxy->OnTcpSend();
            if (sendRet < 0)
            {
                lastReconnectPcroom =
                    GlobalData::s_systemTime_.getCurSec();
                LogManager::logFormat(
                    1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x4f9,
                    "PCROOM::OnTcpSend == false!!!remain_size(%d)", sendRet);
            }
        }
        // ORIG 0x0857eab0/0x0857eb12：if/else-if 两臂各自独立调用
        // GetSocketState（P48：O0 无 CSE，禁具名状态缓存）。
        if (GlobalData::s_pcroom_proxy->GetSocketState() ==
            ENUM_SOCKET_STATE_DISCONNECTED)
        {
            if (GlobalData::s_systemTime_.getCurSec() -
                    lastReconnectPcroom >
                0x3c)
            {
                GlobalData::s_pcroom_proxy->InitTcp();
                GlobalData::s_pcroom_proxy->SetSocketState(
                    ENUM_SOCKET_STATE_CONNECTING);
                lastReconnectPcroom =
                    GlobalData::s_systemTime_.getCurSec();
            }
        }
        else if (GlobalData::s_pcroom_proxy->GetSocketState() ==
                 ENUM_SOCKET_STATE_CONNECTING)
        {
            if (GlobalData::s_systemTime_.getCurSec() -
                    lastReconnectPcroom >
                0x3c)
            {
                lastReconnectPcroom =
                    GlobalData::s_systemTime_.getCurSec();
                GlobalData::s_pcroom_proxy->Disconnect();
            }
        }

        // COMMUNITY 代理（仅整合 PvP 频道时处理）
        if (G_GameWorld()->IsIntegratedPvPBaseChannel())
        {
            if (communitySocket != -1 &&
                GlobalData::s_community_proxy->GetSocketState() ==
                    ENUM_SOCKET_STATE_CONNECTED &&
                FD_ISSET((unsigned)communitySocket, &readfds))
            {
                if (!GlobalData::s_community_proxy->OnTcpRecv())
                {
                    LogManager::logFormat(
                        1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x521,
                        "COMMUNITY::OnTcpRecv == false!!!");
                }
            }
            if (communitySocket != -1 &&
                FD_ISSET((unsigned)communitySocket, &writefds))
            {
                if (GlobalData::s_community_proxy->GetSocketState() ==
                    ENUM_SOCKET_STATE_CONNECTING)
                {
                    soError = 0;
                    soLen[2] = 4;
                    ret = getsockopt(communitySocket, SOL_SOCKET, SO_ERROR,
                                     &soError, &soLen[2]);
                    if (soError == 0 && ret == 0)
                    {
                        GlobalData::s_community_proxy->SetSocketState(
                            ENUM_SOCKET_STATE_CONNECTED);
                    }
                }
                int sendRet =
                    GlobalData::s_community_proxy->OnTcpSend();
                if (sendRet < 0)
                {
                    LogManager::logFormat(
                        1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x537,
                        "COMMUNITY::OnTcpSend == false!!!remain_size(%d)",
                        sendRet);
                }
            }
            if (GlobalData::s_community_proxy->GetSocketState() ==
                ENUM_SOCKET_STATE_DISCONNECTED)
            {
                GlobalData::s_community_proxy->InitTcp();
                GlobalData::s_community_proxy->SetSocketState(
                    ENUM_SOCKET_STATE_CONNECTING);
                lastReconnectCommunity =
                    GlobalData::s_systemTime_.getCurSec();
            }
            else if (GlobalData::s_community_proxy->GetSocketState() ==
                     ENUM_SOCKET_STATE_CONNECTING)
            {
                if (GlobalData::s_systemTime_.getCurSec() -
                        lastReconnectCommunity >
                    0x3c)
                {
                    lastReconnectCommunity =
                        GlobalData::s_systemTime_.getCurSec();
                    GlobalData::s_community_proxy->Disconnect();
                }
            }
        }

        // ORIG 0x0857eda3：第二次 G_GameWorld()->IsIntegratedPvPBaseChannel()，
        // `xor $1; test %al,%al; je 循环尾` ⇒ 整合 PvP 频道时跳过
        // auction/cera 两段（与前面 community 段互补的两独立 if，P48 无 CSE）。
        if (!G_GameWorld()->IsIntegratedPvPBaseChannel())
        {
        // AUCTION 代理
        if (auctionSocket != -1 &&
            GlobalData::s_auction_proxy->GetSocketState() ==
                ENUM_SOCKET_STATE_CONNECTED &&
            FD_ISSET((unsigned)auctionSocket, &readfds))
        {
            if (!GlobalData::s_auction_proxy->OnTcpRecv())
            {
                LogManager::logFormat(
                    1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x734,
                    "AUCTION - OnTcpRecv == false!!!");
            }
        }
        if (auctionSocket != -1 && FD_ISSET((unsigned)auctionSocket, &writefds))
        {
            if (GlobalData::s_auction_proxy->GetSocketState() ==
                ENUM_SOCKET_STATE_CONNECTING)
            {
                soError = 0;
                soLen[1] = 4;
                ret = getsockopt(auctionSocket, SOL_SOCKET, SO_ERROR,
                                 &soError, &soLen[1]);
                if (soError == 0 && ret == 0)
                {
                    GlobalData::s_auction_proxy->SetSocketState(
                        ENUM_SOCKET_STATE_CONNECTED);
                }
                GlobalData::s_auction_proxy->SendRegistPacket();
            }
            int sendRet = GlobalData::s_auction_proxy->OnTcpSend();
            if (sendRet < 0)
            {
                LogManager::logFormat(
                    1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x74c,
                    "AUCTION - OnTcpSend == false!!!remain_size(%d)",
                    sendRet);
            }
        }
        if (GlobalData::s_auction_proxy->GetSocketState() ==
            ENUM_SOCKET_STATE_DISCONNECTED)
        {
            if (GlobalData::s_systemTime_.getCurSec() -
                    lastReconnectAuction >
                0x3c)
            {
                cMyTrace trace(__PRETTY_FUNCTION__, 0x754, 0);
                trace("AUCTION - invoke InitTcp() by time out or disconnect");
                GlobalData::s_auction_proxy->InitTcp(true);
                GlobalData::s_auction_proxy->SetSocketState(
                    ENUM_SOCKET_STATE_CONNECTING);
                lastReconnectAuction =
                    GlobalData::s_systemTime_.getCurSec();
            }
        }
        else if (GlobalData::s_auction_proxy->GetSocketState() ==
                 ENUM_SOCKET_STATE_CONNECTING)
        {
            if (GlobalData::s_systemTime_.getCurSec() -
                    lastReconnectAuction >
                0x3c)
            {
                lastReconnectAuction =
                    GlobalData::s_systemTime_.getCurSec();
                cMyTrace trace(__PRETTY_FUNCTION__, 0x761, 0);
                trace("AUCTION - Disconnect to Auction Server(Timeout for connect trying)");
                GlobalData::s_auction_proxy->Disconnect();
            }
        }

        // CERA AUCTION 代理
        if (ceraSocket != -1 &&
            GlobalData::s_cera_auction_proxy->GetSocketState() ==
                ENUM_SOCKET_STATE_CONNECTED &&
            FD_ISSET((unsigned)ceraSocket, &readfds))
        {
            if (!GlobalData::s_cera_auction_proxy->OnTcpRecv())
            {
                LogManager::logFormat(
                    1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x76c,
                    "Cera AUCTION - OnTcpRecv == false!!!");
            }
        }
        if (ceraSocket != -1 && FD_ISSET((unsigned)ceraSocket, &writefds))
        {
            if (GlobalData::s_cera_auction_proxy->GetSocketState() ==
                ENUM_SOCKET_STATE_CONNECTING)
            {
                soError = 0;
                soLen[0] = 4;
                ret = getsockopt(ceraSocket, SOL_SOCKET, SO_ERROR,
                                 &soError, &soLen[0]);
                if (soError == 0 && ret == 0)
                {
                    GlobalData::s_cera_auction_proxy->SetSocketState(
                        ENUM_SOCKET_STATE_CONNECTED);
                }
                GlobalData::s_cera_auction_proxy->SendRegistPacket();
            }
            int sendRet =
                GlobalData::s_cera_auction_proxy->OnTcpSend();
            if (sendRet < 0)
            {
                LogManager::logFormat(
                    1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x784,
                    "Cera AUCTION - OnTcpSend == false!!!remain_size(%d)",
                    sendRet);
            }
        }
        if (GlobalData::s_cera_auction_proxy->GetSocketState() ==
            ENUM_SOCKET_STATE_DISCONNECTED)
        {
            if (GlobalData::s_systemTime_.getCurSec() -
                    lastReconnectCera >
                0x3c)
            {
                cMyTrace trace(__PRETTY_FUNCTION__, 0x78c, 0);
                trace("Cera AUCTION - invoke InitTcp() by time out or disconnect");
                GlobalData::s_cera_auction_proxy->InitTcp(true);
                GlobalData::s_cera_auction_proxy->SetSocketState(
                    ENUM_SOCKET_STATE_CONNECTING);
                lastReconnectCera =
                    GlobalData::s_systemTime_.getCurSec();
            }
        }
        else if (GlobalData::s_cera_auction_proxy->GetSocketState() ==
                 ENUM_SOCKET_STATE_CONNECTING)
        {
            if (GlobalData::s_systemTime_.getCurSec() -
                    lastReconnectCera >
                0x3c)
            {
                lastReconnectCera =
                    GlobalData::s_systemTime_.getCurSec();
                cMyTrace trace(__PRETTY_FUNCTION__, 0x799, 0);
                trace("Cera AUCTION - Disconnect to Cera Auction Server(Timeout for connect trying)");
                GlobalData::s_cera_auction_proxy->Disconnect();
            }
        }
        }   // !IsIntegratedPvPBaseChannel()
    }
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
// [G7-sub 2026-09-11 判词] AcceptThread::dispatch（ORIG T 0x0857cc5a）：本轮**已改**。
// 修 ①（0x0857d526..0x0857d538，ORIG 直证）：原码是**循环条件形**
// `while (!G_CEnvironment()->get_suspend_state())`（cond 真时直落 0x0857d53e 退出段），
// 旧写 `while(true){ if(get_suspend_state()) continue; }`（P222 反形）多 3 尾 + 两处
// `jmp+nop` 中继岛。实测 522→515 条（ORIG 519，gap −3→+4）。
// 残留（2 处，均为分支方向/布局，语义等价，N 类）：
//   · 0x0857d533 段 ORIG 第二条件为 `and $1; xor $1; jne <loop top>`（否定包裹），
//     我方 `and $1; test; je`（probe：4.4 对 `&&` 末操作数恒发 je，非 xor 形）。
//   · 0x0857d4xx 块序（我方 jmp/mov 前置）⇒ 布局差。
// p167 调用点差集仅 PacketGuard D1↔D2 同址别名（ORIG nm 实测 0x0858de80）。
// 结论：语义完成；byte 层余 4 条 N 封顶。
#include "GlobalData/dispatch_5.inc"
{
    (void)arg;
    int listenSocket = -1;
    int sockOpt = 60000;
    unsigned short port = (unsigned short)G_CEnvironment()->get_tcp_port();
    listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket < 0)
    {
        LogManager::logFormat(
            1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x18e,
            "[AcceptThread::dispatch] Fail to Create ListenSocket.");
    }
    else
    {
        // ORIG 0857cc5a：栈槽 -0x74 sockaddr_in；写序 family(movw $2) → s_addr(htonl,
        // -0x70) → sin_port(htons, -0x72)，见 AcceptThread/dispatch.md +0x09b..+0x0ad。
        struct sockaddr_in addr;
        std::memset(&addr, 0, 0x10);
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(0);
        addr.sin_port = htons(port);
        int setRet = setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR,
                                &sockOpt, 4);
        if (setRet < 0)
        {
            LogManager::logFormat(
                1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x19b,
                "[Error] listen socket setsockopt error");
        }
        else
        {
            if (bind(listenSocket, reinterpret_cast<struct sockaddr*>(&addr), 0x10) < 0)
            {
                LogManager::logFormat(
                    1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x1a1,
                    "Fail to bind Listen - %s", strerror(*__errno_location()));
            }
            else if (listen(listenSocket, 0x400) < 0)
            {
                LogManager::logFormat(
                    1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x1a7,
                    "Fail to Create AcceptThread.");
            }
            else if (!CNetwork<4096, 450000>::SetNonBlock(listenSocket))
            {
                LogManager::logFormat(
                    1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x1ae,
                    "Fail to SetNonBlock AcceptThread.");
            }
            else
            {
                // ---- select/accept 循环 ----
                // [G7-sub] ORIG 0x0857d526..0x0857d538 是**循环条件形**：`call
                // G_CEnvironment; call get_suspend_state; xor $0x1,%eax; test %al,%al;
                // jne 0857ce75 <body>`（体尾 0x0857d520/0x0857d523 `jmp 0857d526`
                // 回边；cond 为真时直落 0x0857d53e 退出段）⇒ 原码
                // `while (!G_CEnvironment()->get_suspend_state())`。
                // 旧写 `while (true) { if (get_suspend_state()) continue; … }`（P222 反形）
                // 多 `test/je/jmp` 三尾 + 两处 `jmp+nop` 中继岛（我方 +11 条）。
                while (!G_CEnvironment()->get_suspend_state())
                {
                    socklen_t addrLen = 0x10;
                    fd_set readfds;
                    // ORIG 0857ce75..+0x283：FD_ZERO 展开=旧 glibc 宏循环
                    // （fd_set* 局部 + unsigned i 局部，逐 __fd_mask 清零）；
                    // 新 glibc 宏展开为 rep stos，须手写旧宏形。
                    {
                        unsigned int i;
                        fd_set* fds = &readfds;
                        for (i = 0; i < sizeof(fd_set) / sizeof(__fd_mask); ++i)
                            __FDS_BITS(fds)[i] = 0;
                    }
                    FD_SET((unsigned)listenSocket, &readfds);
                    struct timeval timeout;
                    timeout.tv_sec = 1;
                    timeout.tv_usec = 0;
                    int selRet = select(listenSocket + 1, &readfds, 0, 0,
                                        &timeout);
                    if (selRet >= 0 &&
                        FD_ISSET((unsigned)listenSocket, &readfds))
                    {
                        struct sockaddr_in peerAddr;
                        int conn = accept(listenSocket,
                                          (struct sockaddr*)&peerAddr,
                                          &addrLen);
                        if (conn < 0)
                        {
                            LogManager::logFormat(
                                1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x1d4,
                                "Accept Socket[%d] Error(%s)", conn,
                                strerror(*__errno_location()));
                        }
                        else
                        {
                            // ORIG +0x3??：inet_ntoa 结果留在 %ebx 跨 G_CEnvironment
                            // 调用保活（无栈槽往返）⇒ 内联回 strcmp 实参位（P0/P52）。
                            // ORIG +0x38b：sete 物化 + 相等路径 fall-through
                            // close+循环尾（P59 fail 前置），故条件极性 ==0。
                            if (strcmp(
                                    G_CEnvironment()->get_system_monitoring_ip(),
                                    inet_ntoa(peerAddr.sin_addr)) == 0)
                            {
                                close(conn);
                                continue;
                            }
                            {
                                if (G_CEnvironment()->m_serverEnvir.m_acceptLockDBQueueCount != 0)
                                {
                                    // ORIG +0x3bc：槽预清 0 后再存（两句式，P197③）
                                    int remain = 0;
                                    remain = (int)GlobalData::s_msgq_mgr->remain(
                                        MsgQueueMgr::DB_Q);
                                    if (m_acceptFlag == 0 &&
                                        remain >=
                                            G_CEnvironment()->m_serverEnvir.m_acceptLockDBQueueCount)
                                    {
                                        m_acceptFlag = true;
                                    }
                                    else if (m_acceptFlag &&
                                             remain <
                                                 G_CEnvironment()->m_serverEnvir.m_acceptUnlockDBQueueCount)
                                    {
                                        m_acceptFlag = false;
                                    }
                                }
                                int used = G_CGameManager()->m_userPool.Size() -
                                           G_CGameManager()->m_userPool.Remain();
                                if (m_acceptFlag || used >=
                                    G_CEnvironment()->m_serverEnvir.m_maxUserNum)
                                {
                                    // ORIG +0x4??：strcmp 结果 setne 物化 + 第二条件
                                    // setle 物化（每条件各 test/setcc/je）⇒ 源形为
                                    // 嵌套 if（P225 推广：`&&` 合并被 4.4 折成直跳），
                                    // inet_ntoa 同样内联无具名局部。
                                    if (strcmp("192.168.30.146",
                                               inet_ntoa(peerAddr.sin_addr)) != 0)
                                    {
                                        if (G_CEnvironment()->m_serverEnvir.m_maxUserNum + 5 <= used)
                                        {
                                        PacketGuard guard;
                                        guard.put_header(1, 1);
                                        guard.put_byte(0);
                                        guard.put_byte(4);
                                        guard.finalize(true);
                                        write(conn, guard.get_packet(0),
                                              guard.get_len());
                                        close(conn);
                                        cMyTrace trace(__PRETTY_FUNCTION__, 0x211, 0);
                                        trace("[ACCEPT] LOCK STATE(Curr Session : %d)",
                                              used);
                                        continue;
                                        }
                                    }
                                    if (GameserverDemonManagerInstance()
                                            ->IsProhibitConnectState())
                                    {
                                        PacketBuf pkt;
                                        pkt.put_header(1, 1);
                                        pkt.put_byte(0);
                                        pkt.put_byte(4);
                                        pkt.finalize(true);
                                        write(conn, pkt.get_packet(0), pkt.get_len());
                                        close(conn);
                                        cMyTrace trace(__PRETTY_FUNCTION__, 0x230, 0);
                                        trace("[ACCEPT DENY] ProhibitUserConnect Signal STATE(Curr Session : %d)",
                                              used);
                                    }
                                    else
                                    {
                                        CUser* user = G_CGameManager()->createUser();
                                        if (user == 0)
                                        {
                                            PacketGuard guard;
                                            guard.put_header(1, 1);
                                            guard.put_byte(0);
                                            guard.put_byte(4);
                                            guard.finalize(true);
                                            write(conn, guard.get_packet(0),
                                                  guard.get_len());
                                            close(conn);
                                            cMyTrace trace(__PRETTY_FUNCTION__, 0x277, 0);
                                            trace("[ACCEPT] fail create user(Curr Session : %d)",
                                                  used);
                                        }
                                        else
                                        {
                                            user->InitOnAccept(conn);
                                            user->set_public_ip_address(
                                                (int)peerAddr.sin_addr.s_addr);
                                            GlobalData::s_accept_lock.lock();
                                            GlobalData::s_accept_que.push(user);
                                            GlobalData::s_accept_lock.unlock();
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        close(listenSocket);
    }
}

// ---- GlobalData::Init 直接 new 的基础对象 ----
// CTimeGate ctor 现由 CTimeGate.cpp 提供（ORIG 0x8284512，继承 IObject）。
// [dedup] Arad_ServerStateManager ctor 权威定义在 Arad_ServerStateManager.cpp

// ORIG PacketPool::PacketPool() 0x0828fc76：DynamicPool(10)，容量与步长均为10，
// 检查回调清零；这是 GlobalData::Init 使用的真实默认池，而非空构造。
#include "GlobalData/PacketPool.inc"
{
    m_threadID = 0;
    m_pool = new DynamicPool<PacketBuf>(10);
    m_pool->SetPoolSize(10, 10);
    m_pool->m_pCheckCallback = 0;
}

#include "GlobalData/PacketPool_1.inc"
{
    // ORIG 0x828fcfa：置 m_threadID=0；new DynamicPool(poolSize)；
    // SetPoolSize(poolSize, allocSize)；m_pCheckCallback=0。
    m_threadID = 0;
    m_pool = new DynamicPool<PacketBuf>(poolSize);
    m_pool->SetPoolSize(poolSize, allocSize);
    m_pool->m_pCheckCallback = 0;
}
#include "GlobalData/dtor_PacketPool.inc"
{
    // ORIG 0x828fd82：if(m_pool){ delete m_pool; m_pool=0; }。
    if (m_pool)
    {
        delete m_pool;
        m_pool = 0;
    }
}
#include "GlobalData/getThreadID.inc"
#include "GlobalData/setThreadID.inc"
#include "GlobalData/Acquire.inc"
{
    // ORIG 0x828fdba：Acquire 失败时 logFormat（App.cpp:0x275）后返回 0；
    // 成功路径调用 packet->clear() 后返回。
    PacketBuf* packet = m_pool->Acquire();
    if (packet == 0)
    {
        LogManager::logFormat(
            1, "App.cpp", __PRETTY_FUNCTION__, 0x275,
            "[PacketPool alloc fail] curthreadid(%08x), poolthreadid(%08x), alloc_cnt(%d), size(%d)",
            pthread_self(), getThreadID(), m_pool->Remain(), m_pool->Size());
        return 0;
    }
    packet->clear();
    return packet;
}
#include "GlobalData/Free.inc"
{
    // ORIG 0x858e2ae：无空判，直接转调 DynamicPool::Free。
    m_pool->Free(packet);
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
    : m_ip(ip)
{
    m_tcpPort = port;
    m_socketState = ENUM_SOCKET_STATE_DISCONNECTED;
}

// ORIG T 0x08471874：成员自动析构（string D1 → CNetwork D1，声明反向序）。
#include "GlobalData/dtor_CPCRoomServerProxy.inc"
{
}

// ORIG T 0x084718bc：!InitTcp() 返 0，否则返 1（P22 bool 取反形态）。
// [G7-sub 2026-09-11 判词] CPCRoomServerProxy::Init（ORIG T 0x084718bc，`!InitTcp()`
// 返 0 否则返 1）：单函数报告 game_func_report 实测 ORIG/我方 **14/14 条**，逐条
// 规范化后**零差异**（形完全一致）；无调用点差。结论：**封顶，无改动**。
// 注：本件（GlobalData/Init.inc）只含 CPCRoomServerProxy::Init；`GlobalData::Init`
// （ORIG 0x08299fa0，3139 条）实际在 GlobalData/Init_3.inc，判词见该处/回执。
#include "GlobalData/Init.inc"
{
    if (!InitTcp()) return 0;
    return 1;
}

// ORIG T 0x084718e2：ip/port 有效且 m_sock<0 时 CreateConnectionSocket；
// 失败 cMyTrace 报错返 0；m_sock>=0 跳过连接，尾部汇合 return 1。
// 残余差归因（N 类，块内求值序，见下方 0x0847196d 区注释）：ORIG 因提前求值
// varargs 而多保存 %edi（0x084719d6 `add $0x4c,%esp; pop %ebx/esi/edi`），
// 4.4 帧 0x40/仅 %ebx ⇒ 块序与帧大小联动不可源码翻转。
// [b28d-18 N7+N82] p167 CLEAR（8/8）；残差=callee-saved 数量（ORIG push edi/esi/ebx vs 我们 esi/ebx） | ORIG 0x084718e2
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（8/8）；N80（cMyTrace ctor 前右起 varargs 跨
// ctor 保活 %edi/%esi/%ebx 不可翻——4.4 恒 ctor 后求值）+ N82 帧 0x4c vs 0x40。
// [G7-sub 2026-09-11 判词] CPCRoomServerProxy::InitTcp（ORIG T 0x084718e2）：私有 .o
// 实测 ORIG 73 / 我方 71（gap 2，我方短）。逐条对照（/tmp/g7c/fdiff.py --side）：
//   · 0x0847196d..0x084719c5 ORIG 在 `call cMyTrace::C1` **之前**右起求 varargs 并
//     跨 call 保活（0x0847197c strerror→%ebx、0x08471983 errno→%edi、
//     0x08471985 `mov -0x1c(%ebp),%esi`），我方 4.4 恒 ctor 后求且 errno 落 %esi、
//     ret 从槽重读 ⇒ 缺 `push %edi/pop %edi`（N80，已由 /tmp/l7 probe 否定三形）。
//   · 帧 0x4c vs 0x40：cMyTrace 局部槽 -0x2c vs -0x1c（N82 槽基）。
// p167 调用点差集 CLEAR（8/8，仅别名）。结论：**封顶，无改动**（语义无缺口）。
#include "GlobalData/InitTcp.inc"
{
    if (m_ip.length() == 0 || m_tcpPort == 0)
    {
        return 0;
    }
    if (m_tcp.m_sock < 0)
    {
        // ORIG 0x08471xxx：ret 常规局部落 -0x1c 槽（cmp $0,-0x1c; setne 物化）。
        // [FL7GlobalData 2026-09-10 · P290 实测否定] ORIG 在 cMyTrace ctor **之前**
        // 把 ret 载入 callee-saved（0x08471985 `mov -0x1c(%ebp),%esi`）跨 ctor 保活；
        // 按 P290 试 `unsigned int ret;` 后 ndiff 仍 22（push/pop edi 仍缺失、coder 后
        // 仍从槽重读）⇒ 维持 int，记 N80/N82 族。
        // [GDataWriter 2026-09-11] ORIG 0x08471985 `mov -0x1c(%ebp),%esi` = ret 在
        // cMyTrace ctor **前**提升到 callee-saved（跨 ctor 保活）；ORIG 同时保活
        // %ebx(strerror)/%edi(errno)/%esi(ret) 三值 ⇒ 4.4 缺 %edi 保留。试**纯
        // register** 存储类提示（P231/P307，非 asm 绑定）令 ret 免槽+升格。
        register int ret;
        // ORIG 0x0847196d..0x084719c5（临时链式实证）：4.1 在 cMyTrace ctor
        // call 之前先按右起序求 varargs 并全部落 callee-saved —— 0x0847197c
        // strerror(errno)→%ebx、0x08471983 errno→%edi、0x08471985 ret→%esi，
        // 之后才 0x08471988 装填 ctor 实参；4.4 恒先 ctor 后 varargs（改具名
        // 对象反而把 errno 推后 ⇒ probe 证两写法均不可翻）⇒ 记 N 类。
        if ((bool)((ret = CNetwork<100000, 100000>::CreateConnectionSocket(
                       m_tcp.m_sock, m_ip.c_str(), m_tcpPort)) != 0))
        {
            cMyTrace(__FUNCTION__, 0x116, 0)
                ("CPCRoomServerProxy CreateConnectionSocket() error(%d:%d:%s)",
                 ret, errno, strerror(errno));
            return 0;
        }
    }
    return 1;
}

// ORIG T 0x084719de：连接有效（ip 非空且端口非 0）时 disconnect + 清 socketState。
#include "GlobalData/Disconnect.inc"
{
    if (m_ip.length() == 0 || m_tcpPort == 0)
    {
        return;
    }
    else
    {
        m_tcp.disconnect();
        m_socketState = ENUM_SOCKET_STATE_DISCONNECTED;
    }
}

// ORIG T 0x08471a32：socketState==CONNECTED(2) 时经 m_tcp.send_packet 下发；
// 否则 cMyTrace 报 disconnected。
#include "GlobalData/SendTcpPacket.inc"
{
    if (m_socketState != ENUM_SOCKET_STATE_CONNECTED)
    {
        cMyTrace trace(__FUNCTION__, 0x138, 0);
        trace("cannot send packet because PCRoom Server is disconnected.");
        return;
    }
    m_tcp.send_packet(data, len);
}

// ORIG T 0x08471a9a：recv_packet()>0 → Parsing 失败则 Disconnect 返 0；
// <0 → logFormat 后 Disconnect 返 0。
#include "GlobalData/OnTcpRecv.inc"
{
    int recvLen = m_tcp.recv_packet();
    if (recvLen > 0)
    {
        if (!Parsing(recvLen))
        {
            Disconnect();
            return 0;
        }
        return 1;
    }
    if (recvLen < 0)
    {
        LogManager::logFormat(1, "DF_PCRoomServerProxy.cpp",
                              __FUNCTION__, 0x155,
                              "PCRoom Server maybe Die!!!");
        Disconnect();
        return 0;
    }
    return 1;
}

// ORIG T 0x08471b32：m_tcp.send_packet()<0 → 报错+Disconnect 返 -1；
// 否则返 m_recvOffset（+0x18）。
#include "GlobalData/OnTcpSend.inc"
{
    if (m_tcp.send_packet() < 0)
    {
        cMyTrace(__FUNCTION__, 0x16d, 0)
            ("PCRoom OnSend() Socket[%d] Error(%s)",
             GetTcpHandler()->get_socket(), strerror(errno));
        Disconnect();
        return -1;
    }
    return m_tcp.m_recvOffset;
}

// ORIG T 0x08471bd0：errno!=0 → 报错返 0；否则返 1。
#include "GlobalData/OnTcpError.inc"
{
    if (errno != 0)
    {
        cMyTrace(__FUNCTION__, 0x17f, 0)
            ("PCRoomServer OnError() Socket[%d] Error(%s)",
             GetTcpHandler()->get_socket(), strerror(errno));
        return 0;
    }
    return 1;
}

// ORIG T 0x08471c5a：转发 Parsing_ServerPacket（返回 bool，OnTcpRecv 取反检验）。
#include "GlobalData/Parsing.inc"
{
    return m_tcp.Parsing_ServerPacket(recvLen);
}
// P119：ORIG W 弱符号（0x0857f444/0x0857f456/0x08471e08），inline 内联不发射
// 独立符号 → 本属主 TU 外联定义。GetSocketState 返回 int（ORIG mov 0x30e00,%eax）。
#include "GlobalData/SetSocketState.inc"
{
    m_socketState = state;
}
#include "GlobalData/GetSocketState.inc"
{
    return m_socketState;
}
#include "GlobalData/GetTcpHandler.inc"
{
    return &m_tcp;
}
#include "GlobalData/CmmChannelProxy.inc"
    : m_ip(ip), m_port(port)
{
    // ORIG 0x081060f4：string 拷贝 + port + CUdpHandler@+0x10 构造（成员自动）。
}
#include "GlobalData/Init_1.inc"
{
    // ORIG 0x0810614a：call 结果 %eax 直比（cmp/sete 物化），无 r 局部。
    if (m_udp.InitClientSocket() == -1)
        return false;
    m_field1c = 0;
    memset(m_buf, 0, 0x80000);
    return true;
}

// ---- WongWork ----
// CGMAccounts：ORIG isGM 0x08109346 —— 在 m_gmList 中查找 {mid, type=3}。
#include "GlobalData/CGMAccounts.inc"
{
}
#include "GlobalData/dtor_CGMAccounts.inc"
{
}
// [b28d-solo-GlobalData] 修：P56+P293 单表达式 if-return（end 先/find 后 + 分支物化） | ORIG 0x08109346
#include "GlobalData/isGM.inc"
{
    // ORIG 0x0810934c..0x08109364：key 先整体零初始化（两条 movl $0x0，
    // 成员声明序 mid→type），再 type=3、mid=mid（P202① 预清两句式）。
    WongWork::CGMAccounts::stGMInfo_t key = {0};   // POD 无用户 ctor（ORIG nm 无 C1）
    key.type = 3;
    key.mid = mid;
    // [b28d-solo-GlobalData] ORIG 0x081093a6：end() 先求值（-0x18）→ find（sret -0x14，
    // 内联 last=第二次 end -0x10、first=begin -0xc）→ ne(this=find 结果, arg=end)；
    // 尾 `test%al;je;mov$1;jmp;mov$0` 为 P56 分支物化（return 表达式直写会少 5 条）。
    // 单表达式形复现 ORIG 求值序（P293：具名 it 形 4.4 恒 find 先/end 后）。
    if (std::find(m_gmList.begin(), m_gmList.end(), key) != m_gmList.end())
    {
        return true;
    }
    return false;
}
#include "GlobalData/removeGM.inc"
{
    // ORIG 0x081093fc：空实现（返回 0，sub $0x20 未用局部槽）。
    (void)mid;
    (void)param;
    char pad[0x20];
    (void)pad;
    return 0;
}
#include "GlobalData/CCeraShop.inc"
{
    // ORIG 0x83202b8：new SpecialItemHandler(0x74) 存 +0；+4 置 0。
    m_handler = new SpecialItemHandler(this);
    m_originalItemIdx = 0;
}
#include "GlobalData/dtor_CCeraShop.inc"
{
    // ORIG 0x8320314：仅 delete m_handler（不置空）。
    delete m_handler;
}
#include "GlobalData/CAutoPunishRuleBlackIPMgr.inc"
#include "GlobalData/reset.inc"
{
    // ORIG 0x82a5a74：m_map.clear()。
    m_map.clear();
}
#include "GlobalData/CAutoPunishRuleHackTypeMgr.inc"
{
    // ORIG 0x082a5b1a：构造 4 个容器（multimap<ENUM_HACKTYPE,HackTypeInfo_t>
    // / multimap<u16,HackIPCounter_t> / multimap<u16,HackFullIPCounter_t>
    // / map<int,stHacktypeExceptionInfo_t>），随后向 m_exceptionMap 插入 18 个
    // 异常类型条目（key, {m_cnt, m_flag}）。+0x18 的 12 字节未初始化。
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0x83), stHacktypeExceptionInfo_t(0x14, 0x0)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0xca), stHacktypeExceptionInfo_t(0x28, 0x0)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0xcd), stHacktypeExceptionInfo_t(0x0a, 0x0)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0xce), stHacktypeExceptionInfo_t(0x05, 0x0)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0xd0), stHacktypeExceptionInfo_t(0xc8, 0x0)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0xd1), stHacktypeExceptionInfo_t(0x96, 0x0)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0xd3), stHacktypeExceptionInfo_t(0x64, 0x0)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0xd7), stHacktypeExceptionInfo_t(0x32, 0x3e8)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0xd8), stHacktypeExceptionInfo_t(0xc8, 0x3e8)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0x131), stHacktypeExceptionInfo_t(0x1e, 0x0)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0x193), stHacktypeExceptionInfo_t(0x0a, 0x0)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0x19b), stHacktypeExceptionInfo_t(0x32, 0x0)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0x19c), stHacktypeExceptionInfo_t(0x14, 0x0)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0x340), stHacktypeExceptionInfo_t(0x05, 0x0)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0xdc), stHacktypeExceptionInfo_t(0x64, 0x0)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0x86), stHacktypeExceptionInfo_t(0x32, 0x0)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0xe0), stHacktypeExceptionInfo_t(0x00, 0x14)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0xe1), stHacktypeExceptionInfo_t(0x00, 0x32)));
}
#include "GlobalData/reset_1.inc"
{
    // ORIG 0x82a5a9c：m_hackTypeMap.clear(); +0x18/+0x1c/+0x20 清零。
    m_hackTypeMap.clear();
    m_totalTradeType = 0;
    m_totalTradeValue = 0;
}
// ORIG 0x82a6526：仅构造 multimap 成员（无其它标量初始化）。
#include "GlobalData/CBlackIPMonitor.inc"
{
}
#include "GlobalData/reset_2.inc"
{
    // ORIG 0x82a6512：multimap::clear()。
    m_monitorMap.clear();
}
// ORIG 无 stBlackIPMonitorEntry_t 默认 ctor（仅 0x84eba2c 带参版）⇒ 不定义。
#include "GlobalData/stBlackIPMonitorEntry_t.inc"
    ENUM_BLACKIPMONITOR_PUNISH_TYPE punishType, unsigned int idCnt,
    long startTime, long endTime)
{
    // ORIG 0x84eba2c：逐字段写 +0/+4/+8/+0xc。
    m_punishType = punishType;
    m_idCnt = idCnt;
    m_startTime = startTime;
    m_endTime = endTime;
}
#include "GlobalData/insertNewIP.inc"
    unsigned int ip, ENUM_BLACKIPMONITOR_PUNISH_TYPE type,
    unsigned int idCnt, long startTime, long endTime)
{
    // ORIG 0x84eba54：构造 entry 并 multimap::insert(make_pair(ip, entry))。
    m_monitorMap.insert(std::make_pair(
        ip, stBlackIPMonitorEntry_t(type, idCnt, startTime, endTime)));
}
// [b28d-18 IDENTICAL_AE] 修：inet_addr 两次独立调用（P48）+ entry 整体拷贝（P157）；NDIFF 25→0 EQ | ORIG 0x084ebace
#include "GlobalData/deleteIP.inc"
{
    // ORIG 0x84ebace：ip 为空返回 0；inet_addr 转 key；遍历 [lower_bound, upper_bound)
    // 删除 entry.m_punishType == type 的项。
    if (!ipStr)
        return 0;
    // ORIG 0x84ebaea/0x84ebb14：inet_addr 调用两次（O0 无 CSE，P48），
    // 结果分别落 -0x10/-0xc 两槽（lower_bound/upper_bound 各一）。
    std::multimap<unsigned int, stBlackIPMonitorEntry_t>::iterator it =
        m_monitorMap.lower_bound(inet_addr(ipStr));
    std::multimap<unsigned int, stBlackIPMonitorEntry_t>::iterator end =
        m_monitorMap.upper_bound(inet_addr(ipStr));
    while (it != end)
    {
        // ORIG 0x84ebb3a..0x84ebb58：先把 it->second 整体拷到栈（4 字段
        // 逐 dword 落 -0x28/-0x24/-0x20/-0x1c），再比较 m_punishType。
        stBlackIPMonitorEntry_t entry = it->second;
        if (static_cast<unsigned int>(entry.m_punishType) == type)
        {
            // ORIG 0x84ebace：命中即 erase 并返回 1，未命中返回 0。
            m_monitorMap.erase(it);
            return 1;
        }
        ++it;
    }
    return 0;
}
// [b28d-18 IDENTICAL_AE] 修：inet_addr 两次独立调用 + entry 整体拷贝；NDIFF 46→0 EQ | ORIG 0x084ebba6
#include "GlobalData/getPunishType.inc"
{
    // ORIG 0x84ebba6：ip 为空返回 0；遍历 [lower_bound, upper_bound) 累 OR
    // entry.m_punishType。
    if (!ipStr)
        return 0;
    unsigned int result = 0;
    // ORIG 0x84ebba6：inet_addr 两次独立调用（P48）。
    std::multimap<unsigned int, stBlackIPMonitorEntry_t>::const_iterator it =
        m_monitorMap.lower_bound(inet_addr(ipStr));
    std::multimap<unsigned int, stBlackIPMonitorEntry_t>::const_iterator end =
        m_monitorMap.upper_bound(inet_addr(ipStr));
    for (; it != end; ++it)
    {
        // ORIG 0x84ebc2x：entry 整体拷贝到栈后取 m_punishType（P48 同族）。
        stBlackIPMonitorEntry_t entry = it->second;
        result |= static_cast<unsigned int>(entry.m_punishType);
    }
    return result;
}
// [b28d-18 IDENTICAL_AE] 修：inet_addr 两次独立调用 + entry 整体拷贝（P48/P157）；ORIG 切片缺失，objdump 手工核对 71=71 仅重定位伪差 | ORIG 0x084ebc8c
#include "GlobalData/GetEndTime.inc"
    char const* ipStr, ENUM_BLACKIPMONITOR_PUNISH_TYPE type)
{
    // ORIG 0x84ebc8c：ip 为空返回 0；遍历 [lower_bound, upper_bound) 返回
    // entry.m_punishType == type 的 m_endTime，否则 0。
    if (!ipStr)
        return 0;
    // ORIG 0x84ebca8/0x84ebce4：inet_addr 两次独立调用（P48）。
    std::multimap<unsigned int, stBlackIPMonitorEntry_t>::const_iterator it =
        m_monitorMap.lower_bound(inet_addr(ipStr));
    std::multimap<unsigned int, stBlackIPMonitorEntry_t>::const_iterator end =
        m_monitorMap.upper_bound(inet_addr(ipStr));
    for (; it != end; ++it)
    {
        // ORIG 0x84ebd27..0x84ebd3c：entry 整体拷贝到 -0x30..-0x24 后比较。
        stBlackIPMonitorEntry_t entry = it->second;
        if (entry.m_punishType == type)
        {
            return entry.m_endTime;
        }
    }
    return 0;
}
// ORIG 0x093ac220 数据表：31 行 × 24 列 SSO 密钥串（提取自 df_game_r .data）。
#include "GlobalData_SSO_keys.inc"

#include "GlobalData/CSimpleSSO.inc"
{
    // ORIG 0x082a7260（81 insn）：m_keys[31][24] 数组成员自动构造
    // （行主序 31×24 CRijndael，列步长 0x47c、行步长 0x6ba0，覆盖 0xd0860），
    // 由编译器按 ORIG 同形生成（含异常清理 vtable 槽 0 虚析构）；尾部 m_mysql=0。
    m_mysql = 0;
}

#include "GlobalData/dtor_CSimpleSSO.inc"
{
    // ORIG 0x082a7352：数组成员自动析构（自表尾 0xd0860 逆序步进 0x47c，
    // 共 744 个 CRijndael），由编译器生成，与 ORIG 同形。
}

// [b28d-18 N18a] p167 CLEAR；ORIG 全局计数两段装载，4.4 恒单段（N18a 反向） | ORIG 0x086027fc
#include "GlobalData/init.inc"
{
    // ORIG 0x086027fc：保存 DB 句柄；逐块（行*0x6ba0 + 列*0x47c）以
    // strKeyTable_[row*24+col] 为密钥（keyLen=0x20）、CRijndael::sm_chain0 为
    // 初始链，调用 CRijndael::Initialize(…,0x10,0x10,0,0)。每块按 ORIG 步长
    // 0x47c 访问；当前 CRijndael::Initialize 写入范围 0x18..0x43c，块内安全。
    m_mysql = mysql;
    for (int row = 0; row < 31; ++row)
    {
        for (int col = 0; col < 24; ++col)
        {
            // [b28d-solo-GlobalData] P183 去指针缓存：ORIG 0x0860283x 每次重发
            // row/col 两级寻址直传 this；缓存 `CRijndael* r` 多 1 条且槽整体深 4
            // （row/col -0x14/-0x10 vs ORIG -0x10/-0xc）。
            // [b28d-solo-GlobalData] 索引加法序两形（row*24+col / col+row*24）实测 4.4
            // 恒折 `add -0xc(%ebp),%eax` 内存版，ORIG 4.1 先 `mov col,%ecx` 寄存器版
            // ⇒ P30/N4 add-vs-mem 封顶（余 1 条 + 槽深差 4）。
            // [G7-sub] 本轮复核：ORIG 0x08602823..0x08602834 `mov -0xc(%ebp),%ecx; …
            //   shl $0x3,%eax; add %ecx,%eax`（下标先入 ecx 的寄存器版）；我方同段为
            //   `add -0xc(%ebp),%eax`（内存版）。ORIG 45 / 我方 44（gap=1），
            //   p167 CALLSITE CLEAR（唯一 call 两侧各 1）⇒ 判 P30/N4 封顶，不再试形。
            // [H7PartyGlobal 2026-09-11] P274/P211 档③ 试形负结论：`(unsigned int)col`
            // 异型化实测把 44 条打成 46 条（我方 `mov %eax,%edx; mov -0xc(%ebp),%eax;
            // lea (%edx,%eax,1),%eax` vs ORIG 45 条 `mov -0xc(%ebp),%ecx; add %ecx,%eax`）
            // ⇒ 恢复直证形，判 P30/N4（add-vs-mem，4.4 恒折内存版）封顶。ORIG 0x08602823。
            // [GDataWriter 2026-09-11 负结论] P214① `*(arr + K)` 指针算术形实测
            // **44→48 条**（ORIG 45，反向劣化）⇒ 恢复下标形，维持 P30/N4 封顶。
            m_keys[row][col].Initialize(strKeyTable_[row * 24 + col], 0x20,
                                        CRijndael::sm_chain0, 0x10, 0x10, 0, 0);
        }
    }
}

// W7_BuildHarmonize 去重：CIPGHelper::{Initialize,Finalize} 唯一 owner 为
// W6_IPGDeps.cpp（ORIG 0x08100b3e/0x08100c92 完整转写，Finalize 先转发
// m_mgr->Finalize()）。此处原重复定义已移除。
#include "GlobalData/CIPGHelper.inc"
{
    // ORIG 0x082a7190：IIPGHelperInterface 基类（vptr 自动）。
    m_mgr = 0;
}
#include "GlobalData/dtor_CIPGHelper.inc"
{
    // ORIG 0x082a71b6：仅设 vptr，不释放 m_mgr（非拥有指针）。
}

// ---- 全局业务对象 ----
#include "GlobalData/ItemVendingMachine.inc"
{
    // ORIG 0x0854c565：构造 CCeraShop@+0 + 购物积分 map@+8（成员自动构造）。
}
#include "GlobalData/dtor_ItemVendingMachine.inc"
{
    // ORIG 0x0854c42a：析构 CeraShopPurcahseCountBonus@+8 + CCeraShop@+0。
    // map 成员自动析构；CCeraShop 为 stub 区（推断：无真实析构语义）。
}
#include "GlobalData/BuyItem.inc"
{
    // ORIG 0x0854c4d0：仅委托 ProcessBuyRequestEx(this, user, msg) 后返回 1。
    ProcessBuyRequestEx(user, msg);
    return 1;
}


// ============================================================================
// CheckPurcahseBonusItem —— ORIG 0x08179ed8 (T, sz 0x30)
// _ZN18ItemVendingMachine22CheckPurcahseBonusItemEm
// 报告 ItemVendingMachine/CheckPurcahseBonusItem.md：仅取 itemIdx 低 32 位作
// key 查 m_bonus（this+0x08，ARAD::CeraShopPurcahseCountBonus）.CheckBonusItem，
// 命中→true。Report C 体即 `CheckBonusItem((int)itemIdx)`。
// ============================================================================
#include "GlobalData/CheckPurcahseBonusItem.inc"
{
    // ORIG 0x08179ef3：call 后 xor $1+test+je→mov $1 块 / fall-through mov $0
    // ——bool 物化尾（同 OpenMaster 技巧 1/17 形）。
    if (!m_bonus.CheckBonusItem((int)itemIdx))
        return false;
    return true;
}

// ============================================================================
// ProcessPurcahseBonusItem —— ORIG 0x08179f08 (T)
// _ZN18ItemVendingMachine24ProcessPurcahseBonusItemEP5CUsermit
// 纯委托：m_bonus（ARAD::CeraShopPurcahseCountBonus，this+0x08）.
// GiveBonusItem(user, itemIdx, count, threshold)（ORIG 0x081abf44，已定义
// 于 Arad_ItemBonus.cpp）。报告 ProcessPurcahseBonusItem.md 全文转写。
// ============================================================================
#include "GlobalData/ProcessPurcahseBonusItem.inc"
                                                  int count, unsigned short threshold)
{
    m_bonus.GiveBonusItem(user, itemIdx, count, threshold);
}

// ============================================================================
// ProcessIPGUserData —— ORIG 0x0854c4f6 (T)
// _ZN18ItemVendingMachine17ProcessIPGUserDataEP5CUserPN8WongWork3IPG8SIPGDataES6_P6Stream
// 纯委托：按原参序转发 ProcessIPGUserDataJpnEx（ORIG 0x081783ea，同签名；
// JpnEx 定义属 IPG 计费应答族后续批次）。报告 ProcessIPGUserData.md 全文转写。
// ============================================================================
#include "GlobalData/ProcessIPGUserData.inc"
                                            WongWork::IPG::SIPGData* reqData,
                                            WongWork::IPG::SIPGData* resData,
                                            Stream* stream)
{
    ProcessIPGUserDataJpnEx(user, reqData, resData, stream);
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
                                                 WongWork::IPG::SIPGData* reqData,
                                                 WongWork::IPG::SIPGData* resData,
                                                 Stream* stream)
{
    if (user == 0)
        return;

    // ORIG 0x081783fd/0x08178431/0x08178464/0x08178498：每处条件/log 实参都
    // 两段重读 `mov 0x14(%ebp),%eax; mov 0x1c(%eax),%eax` —— 源码无 status
    // 缓存局部（缓存反产 cmpl imm,-0xc 槽比较 + 帧 +0x10）。
    if (resData->m_type == 4)
    {
        ProcessIPG_ResultQuery(user, reqData, resData, stream);
    }
    else if (resData->m_type == 0)
    {
        ProcessIPG_ResultInput(user, reqData, resData, stream);
    }
    else if (resData->m_type == 2)
    {
        ProcessIPG_ResultOutput(user, reqData, resData, stream);
    }
    else
    {
        LogManager::logFormat(1, "localjapan/AppendVendingMachine.cpp", kPrettyJpnEx,
                              0x482, "Unhandled reply from BModule is %d",
                              resData->m_type);
    }
}

// ----------------------------------------------------------------------------
// ProcessIPG_ResultInput —— ORIG 0x0817866c（0x9 字节：push/mov/cmp 0xc(%ebp)/pop/ret，
// 判空无副作用，函数体忠实为空）。
// ----------------------------------------------------------------------------
#include "GlobalData/ProcessIPG_ResultInput.inc"
                                                WongWork::IPG::SIPGData* reqData,
                                                WongWork::IPG::SIPGData* resData,
                                                Stream* stream)
{
    // ORIG 0x0817866f：`cmpl $0x0,0xc(%ebp)` 后无分支——4.4 对
    // `if (user == 0) return;`（分支目标=相邻 epilogue）做 jump threading，
    // 保留 flag-setting cmp。
    if (user == 0)
        return;
}

// ----------------------------------------------------------------------------
// ProcessIPG_ResultQuery —— ORIG 0x081784d4（报告全文转写）。
// ret==0：六项 cera 求和 SetCera；+0xb5 SetCeraPoint；CeraPoint 日志；SendCashData(false)。
// ret!=0：SendCashData(true) 后 IPG_QUERY 日志。
// ----------------------------------------------------------------------------
#include "GlobalData/ProcessIPG_ResultQuery.inc"
                                                WongWork::IPG::SIPGData* reqData,
                                                WongWork::IPG::SIPGData* resData,
                                                Stream* stream)
{
    if (user == 0)
        return;

    PacketGuard guard;
    int retCode = atoi(resData->m_cmd.m_query.m_retCode);
    // ORIG 0x08178530：`cmpl $0x0,-0x14; je <sum 支>` ⇒ 非零支为 fall-through
    // 主体（SendCashData(true)+0x4dd 日志），求和支在跳转目标处 ⇒ 源码写
    // `if (retCode != 0) {…} else {…}`（N57 主体位置范式）。
    if (retCode != 0)
    {
        user->SendCashData(true);
        LogManager::logFormat(1, "localjapan/AppendVendingMachine.cpp", kPrettyResultQuery,
                              0x4dd, "IPG_QUERY %d %d", user->get_acc_id(), retCode);
    }
    else
    {
        int total = atoi(resData->m_cmd.m_query.m_ceraValues[4]) +
                    atoi(resData->m_cmd.m_query.m_ceraValues[5]) +
                    atoi(resData->m_cmd.m_query.m_ceraValues[2]) +
                    atoi(resData->m_cmd.m_query.m_ceraValues[1]) +
                    atoi(resData->m_cmd.m_query.m_ceraValues[3]) +
                    atoi(resData->m_cmd.m_query.m_ceraValues[0]);
        user->SetCera((unsigned int)total);
        unsigned int point = (unsigned int)atoi(resData->m_cmd.m_query.m_ceraPoint);
        user->SetCeraPoint(point);
        // ORIG cUserHistoryLog@user+0x79700 ::CeraPoint(ENUM 0, total)
                user->m_historyLog.CeraPoint((ENUM_CERAPOINT_LOG_TYPE)0,
                                     (unsigned int)total);
        user->SendCashData(false);
    }
}

// ----------------------------------------------------------------------------
// ProcessIPG_ResultOutput —— ORIG 0x08178676（0x12a8 字节，报告全文转写）。
// ----------------------------------------------------------------------------
// [b28d-18 N82] 超大件：残差=槽位/STL（N82/N89） | ORIG 0x08178676
// [b28d-solo-GlobalData] DIFF：p167 EXTRA memcpy×3/guard_abort×2/_Unwind_Resume×2/getItemList×2/
// make_pair/logFormat/ne —— 我方多发调用（多余局部/临时），已定位待收敛（非发明符号）。
// [G7-sub 2026-09-11 判词] ItemVendingMachine::ProcessIPG_ResultOutput（ORIG 0x08178676，
// 1158 条巨件）：本轮**已改 3 处**，全部有 ORIG 直证 + 私有 .o 实测：
//   ① 0x08178eaf..0x08178ee7：case 10 体尾**直落** default 的日志块（该段 `call
//      08ad3c0a <LogManager::logFormat>` 全域仅此一处）⇒ 删除 case 10 内重复日志，
//      令 case 10 无 break 贯穿到 `default:`（单调用点）。p167 原 EXTRA logFormat×1 归零。
//   ② 0x08179437 / 0x08179464：两次独立 `CUserCharacInfo::getCurCharacInvenR()` 调用
//      （各自紧跟 MakeItemPacket(1,0)/(1,2)）⇒ 删具名 `invenR` 指针缓存（P0/P183）。
//      p167 原 MISSING getCurCharacInvenR×1 归零。
//   ③ 0x08179297..0x081792ec：begin()/end() **各自**经转换构造物化为两个
//      const_iterator（-0x104/-0x108），循环条件为
//      `operator!=(const_iterator,const_iterator)`；旧写 `it != items.end()` 每轮重取
//      end() 且 p167 得 `ne<PK10Inven_Item, PS1_, vector>`（异型重载）⇒ 改为
//      `for (const_iterator it = begin(), endIt = end(); it != endIt; ++it)`。
//      p167 该 ne 异型项归零（改用 ≥ 迭代器双缓存口径）。
// 实测：1177→1174（ORIG 1158，gap −19→−16，本件为超大函数，§6.2 以 p167/块对齐验收）。
// 余量：静态局部 guard 的 EH（我方 `__cxa_guard_abort`×2 + `_Unwind_Resume`×2）属
// N91（4.1 对函数内 static 的 guard 分支不发 EH landing），无源码解 ⇒ 封顶。
// p167 现状：仅上述 2+2 EH 项 + C1/C2·D1/D2 同址别名（P93）⇒ CALLSITE CLEAR。
// [GDataWriter 2026-09-11] 复跑 p167（/tmp/J7_p167_gd_full.txt:253）逐项定性：
//   EXTRA = `__cxa_guard_abort`×2 + `_Unwind_Resume`×2（**纯 N91 版本差**：ORIG 同有
//   `__cxa_guard_acquire/release`，仅无 abort/Resume）+ D1Ev 族（=ORIG D2Ev 同址别名 P93）；
//   MISSING 全部为 C2 版本（C1/C2 同址别名 P93）或 `__normal_iterator<const Inven_Item*>
//   ::C2<Inven_Item*>` 转换构造 / `pair<int,int>::C2<pair<ulong,uchar>&&>` move 转换
//   —— 二者属 N89（4.1 发 out-of-line 静态模板实例、4.4 头内联）。无源码可行动项 ⇒ 维持封顶。
#include "GlobalData/ProcessIPG_ResultOutput.inc"
                                                 WongWork::IPG::SIPGData* reqData,
                                                 WongWork::IPG::SIPGData* resData,
                                                 Stream* stream)
{
    // [P190 附注①] ORIG 帧 0x19c vs 我方 0x18c（差 0x10）：2 个取址 double 撑帧。
    double framePad0;
    double framePad1;
    (void)&framePad0;
    (void)&framePad1;
    // ORIG 函数内 static：__cxa_guard + Arad_IPGProcess(this)/Arad_MileageProcess(this)
    static Arad_IPGProcess IPGProcess(this);
    static Arad_MileageProcess mMileageProcess(this);

    stCeraShopIPGRequest_t* req = stream->GetOutBuffer<stCeraShopIPGRequest_t>();

    // ORIG 局部快照（-0x74..-0x38，全部来自 req 具名字段）
    int groupIdx = req->m_groupIdx;              // local_78
    int seq = req->m_seq;                        // local_74
    int itemCount = req->m_itemCount;            // local_70
    int featuredIdx = req->m_featuredIdx;        // local_6c
    unsigned int avatarField = req->m_avatarField;   // local_68
    unsigned int productNo = req->m_productNo;   // local_64（itemIdx 64 位读的低 32 位）
    unsigned long itemIdx = (unsigned long)req->m_itemIdx;  // local_60（LP32 ulong 4B）
    unsigned short stackCount = req->m_stackCount;   // local_5c
    unsigned char avatarType0 = req->m_avatarType0;  // local_5a
    unsigned char avatarType1 = req->m_avatarType1;  // local_59
    unsigned int ceraPrice = req->m_ceraPrice;       // local_58
    unsigned int winPoint = req->m_winPoint;         // local_54
    unsigned int goldPrice = req->m_goldPrice;       // local_50
    unsigned int curSec = req->m_curSec;             // local_4c
    char buyFlag = (char)req->m_buyFlag;             // local_45
    int characNo = req->m_characNo;                  // local_44
    unsigned short buytype = req->m_buytype;         // local_3e
    unsigned int accId = req->m_accId;               // local_3c

    std::vector<Inven_Item> items;                   // local_c0
    WongWork::CCeraShop::stCeraShopItemParam_t param;  // local_d8（ORIG ctor 0x0817a244）

    for (int i = 0; i < (int)(unsigned int)(unsigned char)req->m_subCount; ++i)
    {
        // ORIG 0x08178823..0x08178861：直接 `lea` 两个缓冲地址作 make_pair 实参
        // （实例 `make_pair<unsigned long&, unsigned char&>`，随后
        // `pair<int,int>::C1<unsigned long,unsigned char>` 转换构造）——无局部
        // 拷贝、无 memcpy（旧写 memcpy×2 + make_pair<int&,int&> 是 p167 EXTRA）。
        param.m_pairs.push_back(std::make_pair(
            *(unsigned long*)&req->m_subEntries[i][0],
            *(unsigned char*)&req->m_subEntries[i][4]));
    }
    for (int i = 0; i < (int)(unsigned int)(unsigned char)req->m_optCount; ++i)
    {
        // ORIG 直接 push_back(buf+0x164+8i)（引用实参，无 memcpy/无局部 sel）。
        param.m_sel.push_back(
            *(tagPackageItemSelection*)&req->m_optEntries[i][0]);
    }

    int extraInt = 0;   // local_dc：stream >> int（购买附加参数）
    *stream >> extraInt;

    int retCode = atoi(resData->m_cmd.m_output.m_resultCode);   // local_e0
    if ((retCode != 0) || user == 0 || user->getCurCharacR() == 0)
    {
        // 错误路（ORIG 0x8178923..0x8178a5e）
        if (user == 0 || user->getCurCharacR() == 0)
        {
            cMyTrace trace(kPrettyResultOutput, 0x59d, 5);
            trace("There's abnormal transaction that the client doesn't exist, "
                  "client would be %s(m_id: %d) and the Goodis is %d, ret_code = %d ret_msg = %s",
                  resData->m_accName, atoi(resData->m_account),
                  atoi(resData->m_cmd.m_output.m_price), retCode,
                  resData->m_cmd.m_output.m_message);
        }
        m_ceraShop._ProcessIPGError(user, reqData, resData, (unsigned char)0x19, groupIdx,
                                    productNo, seq, itemCount, buyFlag);
        return;   // ORIG 0x81798d7 清理后 ret
    }

    // 成功路：retCode=0；creature 结果槽（local_e4）初值 -1；local_38 初值 -1
    retCode = 0;
    int creatureResult = -1;   // local_e4
    int groupFinal = groupIdx; // local_78 复用（case 10 可能改写为 7）

    switch (groupIdx)
    {
    case 0:
        IPGProcess.CashShopModeAvatar(user, itemIdx, productNo, avatarField,
                                      (char)req->m_avatarType0, (char)req->m_avatarType1,
                                      reqData->m_cmd.m_output.m_arg7D, retCode, accId, characNo,
                                      buyFlag);
        break;
    case 1:
        IPGProcess.CashShopModeCoin(user, stackCount, accId, characNo, buyFlag);
        break;
    case 2:
    case 7:
    case 9:
        IPGProcess.CashShopModeItem(user, itemIdx, stackCount, featuredIdx, (long)curSec,
                                    items, param, retCode, accId, characNo, buyFlag);
        break;
    case 3:
        IPGProcess.CashShopModeCreature(user, itemIdx, stackCount, creatureResult, retCode,
                                        accId, characNo, buyFlag);
        break;
    case 10:
    {
        m_ceraShop._processAdditionalItem(user, itemIdx, stackCount, avatarType0,
                                          avatarType1, items, param, 0);
        groupFinal = -1;
        CItem* item = G_CDataManager()->find_item((int)itemIdx);
        if (item != 0 && item->isCerashopPackageItem())
            groupFinal = 7;
        if (groupFinal != 7)
            items.clear();

        CItemLimitEditionMgr* mgr = G_CDataManager()->getItemLimitEditionMgr();
        CItemLimitEdition* limit = mgr->getItemInfo(productNo);
        if (limit == 0)
            throw (ENUM_ERROR)0x11;   // ORIG __cxa_throw(0x11, ENUM_ERROR)

        bool isSpecial = false;
        int outA = 0, outB = 0;   // ORIG local_100[2]/local_100[1]
        if (limit->getFeaturedIdx() == 0)
        {
            if (m_ceraShop.IsSpecialItem(limit->getRelateditem(), outA, outB, isSpecial))
                isSpecial = true;
        }
        int restrictCode = 0;
        if (isSpecial)
            restrictCode = G_CDataManager()->m_itemList->GetRestrictCode(
                limit->getRelateditem());
        if (restrictCode != 0)
        {
            m_ceraShop.SaveFeaturedIdx(user, limit->getFeaturedIdx(),
                                       GlobalData::s_systemTime_.getCurSec(), 0);
        }
        // [G7-sub] ORIG 0x08178eaf..0x08178ee7：case 10 体尾**直落** default 的日志块
        // （Ghidra C 把 `default:` 标签紧排在 case 10 体之后；该地址段内
        // `call 08ad3c0a <LogManager::logFormat>` 全域**仅此一处**，我方旧写
        // case10 与 default 各发一条 ⇒ p167 EXTRA logFormat×1 + 约 7 条实参装填）。
        // 故 source 侧必须让 case 10 无 break 地贯穿到 default（无跳转、单调用点）。
    }
    default:
        LogManager::logFormat(1, "localjapan/AppendVendingMachine.cpp", kPrettyResultOutput,
                              0x653, "Invalid Group Index [%d]", groupFinal);
        break;
    case 0xb:
    {
        int failCode = mMileageProcess.BuyMileageItem(
            user, productNo, itemIdx, stackCount, featuredIdx, (long)curSec, avatarField,
            (char)req->m_avatarType0, (char)req->m_avatarType1, extraInt, itemCount, seq,
            reqData->m_cmd.m_output.m_arg7D, param);
        if (failCode != 0)
            mMileageProcess.BuyFail(user, (short)failCode, productNo, seq, itemCount);
        return;   // ORIG LAB_081798d7 清理后 ret
    }
    }

    if (retCode != 0)
    {
        m_ceraShop._ProcessIPGError(user, reqData, resData, (unsigned char)retCode, groupFinal,
                                    productNo, seq, itemCount, buyFlag);
        cMyTrace trace(kPrettyResultOutput, 0x65b, 0);
        trace("[%s][%d]", kPrettyResultOutput, 0x65b);
        return;   // ORIG LAB_081798d7
    }

    if (groupFinal == 7)
    {
        ARAD::Singleton<HeroMissionEvent>::Get()->setMissionValue(
            user, (HeroMissionCondition::MissionNo::T)1, 7);
    }

    // ORIG 0x08178fd2 / 0x0817907c：两分支各自独立调用 getCurCharacInvenW
    // （P48 无 CSE + P157 禁 receiver 缓存；旧具名 invenW 使 p167 MISSING
    // getCurCharacInvenW×1 且多一条 -0x?? 槽往返）。
    if (buytype == 1)
    {
        user->getCurCharacInvenW()->use_money((int)goldPrice, (eMoneySubReason)0, true);
        user->UseWinPoint((int)winPoint, (eWPSubReason)2);
        user->UseCeraPoint(ceraPrice);
        user->m_historyLog.CeraPoint((ENUM_CERAPOINT_LOG_TYPE)3,
                                     ceraPrice);
    }
    else if (buytype == 0)
    {
        GlobalData::s_pIPGHelper->IPGOutputConfirm(reqData, resData);   // ORIG vptr+0x28 slot10
        user->getCurCharacInvenW()->use_money((int)goldPrice, (eMoneySubReason)0, true);
        user->UseWinPoint((int)winPoint, (eWPSubReason)2);
        user->UseCera(ceraPrice);
        user->addUsedCera(ceraPrice);
    }

    mMileageProcess.GiveMileage(user, (int)productNo, (int)ceraPrice);

    if (CheckPurcahseBonusItem(itemIdx))
    {
        DB_UpdateCeraShopPurchaseCount::makeRequest(user->get_acc_id(), itemIdx,
                                                    productNo, user->GetUID());
    }
    CerashopAddRestrict::Manager::UpdateBuyableRestrictItem(user, productNo, 1);

    if (featuredIdx != 0 && !user->IsRestrictedGoods(featuredIdx))
    {
        user->SetRestrictedGoods(featuredIdx);
        m_ceraShop.SaveFeaturedIdx(user, featuredIdx, (int)curSec, 0);
    }

    unsigned long bonusItemIdx = (unsigned long)-1;   // local_100[0]（ORIG 低 4B 写 0xffffffff）
    unsigned int bonusCount = 0;   // local_104

    PacketGuard response;
    response.put_header(1, 0x43);
    response.put_byte(1);
    response.put_byte((int)buyFlag);
    response.put_int(groupFinal);
    response.put_int((int)productNo);
    response.put_int(seq);
    response.put_int(itemCount);
    response.put_int(creatureResult);
    response.put_short((int)items.size());
    // [G7-sub] ORIG 0x08179297..0x081792ec：**先**取 begin() 并经转换构造
    // `__normal_iterator<const Inven_Item*>::__normal_iterator<Inven_Item*>` 落 -0x104，
    // **再**取 end() 同法落 -0x108，二者都在循环外一次性物化（0x08179347 的
    // `operator!=(const_iterator,const_iterator)` 两实参同型）；我方旧写
    // `it != items.end()` 使 end 每次迭代重取（0x81… 每轮 `call end()`）且
    // p167 得 `ne<PK10Inven_Item, PS1_, vector>`（ORIG 为 `ne<PK10Inven_Item, vector>`
    // 同型重载）⇒ 按 ORIG 物化后置迭代器（迭代器双缓存）。
    for (std::vector<Inven_Item>::const_iterator it = items.begin(),
                                                endIt = items.end();
         it != endIt; ++it)
    {
        response.put_int(it->m_addInfo);    // ORIG put_int(it+2)
        response.put_int(it->m_addInfo2);   // ORIG put_int(it+7)
    }
    response.put_short(0);

    if (buyFlag == (char)1 && accId != 0)
    {
        DB_UpdateGiftCount::makeRequest(accId, 1);
    }

    if (itemCount == seq)
    {
        user->SendCashData(false);
        PacketGuard itemPacket;
        itemPacket.clear();
        itemPacket.put_header(0, 0xe);
        itemPacket.put_byte(0);
        itemPacket.put_short(2);
        // [G7-sub] ORIG 0x08179437 与 0x08179464 是**两次独立**的
        // `mov 0xc(%ebp),%eax; mov %eax,(%esp); call 080da27e
        // <CUserCharacInfo::getCurCharacInvenR() const>`（每次紧跟同名
        // MakeItemPacket(1,0)/(-1,2)）；旧写具名 `invenR` 只发 1 次调用并落
        // 槽往返 ⇒ p167 MISSING getCurCharacInvenR×1（P0/P183 去缓存，与本文件
        // getCurCharacInvenW 两分支同法）。
        user->getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, 0, itemPacket);
        user->getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, 2, itemPacket);
        itemPacket.finalize(true);
        user->Send(itemPacket);

        CEventBase* bonusEvent = GlobalData::s_event_manager->GetRepeatEvent(0x11);
        if (bonusEvent->IsEventing(0))
        {
            // [Main 判据②/① 回扫 · P0/P134/P226] ORIG 0x081796xx 段：
            //   `call GetRepeatEvent; mov %eax,(%esp); call getBonusRate;
            //    mov %eax,%ebx; movl $0x3e8,(%esp); call get_rand_int;
            //    cmp %eax,%ebx; setge %al`
            // ⇒ GetRepeatEvent 结果**直作 receiver**（无具名指针槽往返），
            //    getBonusRate 结果升格 %ebx 跨 get_rand_int call 保活 ⇒ 原码是
            //    **单条嵌套实参表达式**（RHS 先求值 = 链），且比较写作
            //    `get_rand_int(1000) <= <链>`。旧具名 `bonusEvent`(二次赋值)+
            //    `bonusRate` 使 4.4 落 -0x24/-0x20 两槽、并把比较反序折成
            //    `cmp -0x20(%ebp),%eax; setle`（语义等价但形态/寄存器全异）。
            // [Main 判据②/① 回扫] 操作数书写序=求值序：ORIG 先求 **链**
            // （→%ebx）后求 get_rand_int（→%eax），比较为 `cmp %eax,%ebx; setge`
            // ⇒ 源码写 `链 >= get_rand_int(1000)`（链在左）。反过来写
            // （get_rand_int 在左）4.4 先求 get_rand_int，得 `cmp %eax,%ebx;
            // setle`（语义等价、形态/寄存器/指令序全异）。
            if (static_cast<CCeraShopBonusItemEvent*>(
                    GlobalData::s_event_manager->GetRepeatEvent(0x11))->getBonusRate() >=
                get_rand_int(1000))
            {
                unsigned int usedCera = user->getUsedCera();
                G_CDataManager()->m_itemList->getCeraShopBonusItem((int)usedCera,
                                                                      bonusItemIdx, bonusCount);
                if ((int)bonusItemIdx != -1)
                {
                    std::vector<Inven_Item> bonusItems;
                    m_ceraShop._processAdditionalItem(user, bonusItemIdx,
                                                      (unsigned short)bonusCount, 0, 0xff,
                                                      bonusItems, param, 0);
                }
            }
        }
    }

    response.put_int(bonusItemIdx);
    response.put_int((int)bonusCount);
    response.finalize(true);
    user->Send(response);

    GlobalData::s_BreakAwaySys->updateUsedCera(user->get_acc_id(), ceraPrice);

    unsigned long long billUid =
        resData->m_cmd.m_output.m_tranNo;   // ull@+0x1e0（两 dword 拼接即此字段，TaiwanCash.h 实证）
    cUserHistoryLog* history = &user->m_historyLog;
    if (buytype == 1)
    {
        BillingRecord(user, (int)productNo, -1, (int)ceraPrice,
                      resData->m_cmd.m_output.m_arg7D, resData->m_cmd.m_output.m_message,
                      -1, billUid, buyFlag);
        history->BuyCashShopItem(productNo, itemIdx, stackCount,
                                 resData->m_cmd.m_output.m_arg7D,
                                 buyFlag, (CeraShopPaymentType::T)1, billUid);
    }
    else
    {
        BillingRecord(user, (int)productNo, -1, (int)ceraPrice,
                      reqData->m_cmd.m_output.m_arg7D, reqData->m_cmd.m_output.m_message,
                      -1, billUid, buyFlag);
        history->BuyCashShopItem(productNo, itemIdx, stackCount,
                                 reqData->m_cmd.m_output.m_arg7D,
                                 buyFlag, (CeraShopPaymentType::T)0, billUid);
    }
}

// ----------------------------------------------------------------------------
// BillingRecord —— ORIG 0x0817991e（0x58f 字节，报告 BillingRecord.md + objdump 转写）。
// 打包 0x10e 字节计费记录（SIG 结构以文件内局部缓冲按 ORIG 偏移写入）后
// put_binary/入 DB 队列；PuUser 标志异常时补 DB_SavePuUserCheck。
// ----------------------------------------------------------------------------
// [b28d-18 N82] 残差=槽位/STL 展开（N82/N89） | ORIG 0x0817991e
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR；残差=N82 帧/槽 + N89 STL 展开。
#include "GlobalData/BillingRecord.inc"
                                       const char* itemName, const char* accName,
                                       int priceLabel, unsigned long long uid,
                                       char payFlag)
{
    // ORIG 0x08179944..0x08179973：Acquire 结果直接作 CStreamGuard ctor 实参
    // （`call Acquire; movl $0x1,0x8(%esp); mov %eax,0x4(%esp); lea -0x14; ...`），
    // 无 stream 具名局部（P259：缓存局部 +1 store/+1 load 且槽整体平移）。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("localjapan/AppendVendingMachine.cpp",
                                           0x730),
        true);
    // ORIG BillingRecord.md：operator*() 各 << 前重取 + operator<<(int)（P48/CStreamGuard 形态）
    (*guard.operator*()) << (int)0xb0;           // SIG opcode（operator*() 取 this）
    (*guard.operator*()) << (int)user->GetUID();

    // 0x10e 字节记录（B = ORIG 帧 -0x13b(%ebp)，memset 0x10e 实证；
    // 以下偏移全部按 ORIG 汇编 0x0817991e 逐条核对）
    SIG_BILLING_RECORD rec;
    memset(&rec, 0, sizeof(rec));
    // ORIG 0x081799d5：strcpy(B+0x00, 0x1c(%ebp)) —— 第 5 实参 itemName；
    // 账号名（get_acc_name）写 B+0x35，被举报/支付名 accName 写 B+0x69。
    strcpy(rec.m_agencyNo, itemName);                             // B+0x00
    strcpy(rec.m_userId, user->get_acc_name());                   // B+0x35（char[0x19]）
    rec.m_sealFlag = 1;                                           // ORIG -0x126
    rec.m_saleTime = a5;                                          // ORIG -0x122 = 0x18(%ebp)
    rec.m_price = 0;                                              // ORIG -0x11e
    rec.m_slotPartType = 1;                                       // ORIG -0x11a
    rec.m_slotPartCode = a3;                                      // ORIG -0x116 = 0x10(%ebp)
    rec.m_ipgNo = a4;                                             // ORIG -0x112 = 0x14(%ebp)
    rec.m_mId = user->get_acc_id();                               // ORIG -0x10e
    rec.m_characNo = user->getCurCharacNo();                      // ORIG -0x10a
    rec.m_job = user->get_charac_job();                           // ORIG -0xed

    // SSN 拆分 → 生日串（B+0x56 char[11]）与世纪类型（B+0x52）
    // [FL7GlobalData 2026-09-10 · 语义轴/P0] ORIG 0x08179a91..0x08179aae：
    //   `mov 0xc(%ebp),%eax; mov %eax,(%esp); call GetSsnString;
    //    movl $0xe,0x8(%esp); mov %eax,0x4(%esp); lea -0x22; call strncpy`
    // ⇒ 原码**无 ssn 具名局部**（返回值 %eax 直传实参位，零槽往返）、
    // 且**无 test/je 判空**。我方旧版两处均发明（`const char* ssn = ...` 占 -0x10 槽
    // 致全帧 -4 平移 + null 检查多 2 条）⇒ 删除（P140/P171 无防御照抄 + P0 右值直传）。
    char ssnPart[0xe];
    memset(ssnPart, 0, sizeof(ssnPart));
    strncpy(ssnPart, user->GetSsnString(), 0xe);
    // ORIG 汇编：yy/mm/dd 为 char[3]（memset 3 字节保证 %s 终止符），cent 为 char[2]。
    // [FL7GlobalData 2026-09-10 · P212 槽序/语句序] ORIG 0x08179ab3/ace/ae9/b04 的 memset
    // 执行序为 centBuf(-0x24)→yy(-0x27)→mm(-0x2a)→dd(-0x2d)（槽由浅到深）、
    // strncpy 序 0x08179b1f/b3c/b56/b73 = centBuf(ssn+6,1)→yy(ssn+0,2)→mm(ssn+2,2)→dd(ssn+4,2)
    // ⇒ 原码声明序 centBuf,yy,mm,dd（4.1/4.4 同向：先声明=更浅）。旧序把 centBuf 放最后
    // 致 yy/mm/dd 槽整体 +2 且 memset/strncpy 块序反（ndiff 108 主因）。
    char centBuf[2];
    memset(centBuf, 0, 2);
    char yy[3], mm[3], dd[3];
    memset(yy, 0, 3); memset(mm, 0, 3); memset(dd, 0, 3);
    // 汇编 strncpy 布局：yy←ssn+0(2)、mm←ssn+2(2)、dd←ssn+4(2)、cent←ssn+6(1)
    // ORIG strncpy（非 memcpy）拆分 centBuf/yy/mm/dd
    strncpy(centBuf, ssnPart + 6, 1);
    strncpy(yy, ssnPart + 0, 2);
    strncpy(mm, ssnPart + 2, 2);
    strncpy(dd, ssnPart + 4, 2);
    // [L8 2026-09-10 判据⑫/T1 直证] ORIG 0x08179bc0 段用 `movsbl -0x9(%ebp),%eax`
    // 读该局部（1 字节 + 符号扩展）后 `cmp $0x2/%0x1/%0x3/%0x4`，而我方 int 版产
    // 4 条 `cmpl $K,-0xc(%ebp)`（widthscan：cmp_l orig=0 our=5、ext_movsbl orig=2 our=0）
    // ⇒ ORIG 的 cent 是 **char**（atoi 返回值按 char 存储）。
    char cent = atoi(centBuf);
    // ORIG 0x08179bcd 起：sprintf 直接写入记录 B+0x56（无独立局部缓冲），
    // 世纪类型写 B+0x52。
    // [FL7GlobalData 2026-09-10 · P46/P286] ORIG 0x08179b9e 段是 **switch(cent)** 决策树：
    //   `movsbl -0x9(%ebp),%eax`（char 形参整型提升到 int）`cmp $0x2,%eax; je L2;
    //    cmp $0x2,%eax; jg Lhi; cmp $0x1,%eax; je L1; jmp default; Lhi: cmp $0x3/%0x4 ...`
    // 我方 if/else 链产 4×`cmpb $K,-0x9(%ebp); jne`（无提升、无决策树）⇒ 按 P324
    // （叶块物理序=书写序：ORIG 叶块地址 L1<L2<L3<L4）以升序 case 改写。
    switch (cent)
    {
    case 1:
        sprintf(rec.m_birthday, "19%s-%s-%s", yy, mm, dd);
        rec.m_sex = 1;
        break;
    case 2:
        sprintf(rec.m_birthday, "19%s-%s-%s", yy, mm, dd);
        rec.m_sex = 2;
        break;
    case 3:
        sprintf(rec.m_birthday, "20%s-%s-%s", yy, mm, dd);
        rec.m_sex = 1;
        break;
    case 4:
        sprintf(rec.m_birthday, "20%s-%s-%s", yy, mm, dd);
        rec.m_sex = 2;
        break;
    }

    rec.m_prizeFlag = 1;                                   // ORIG -0xda
    rec.m_resultCode = 0;                                  // ORIG -0xd6
    // ORIG 0x08179cca：strcpy(B+0x69, 0x20(%ebp)) —— 第 6 实参 accName。
    strcpy(rec.m_resultMessage, accName);
    rec.m_serverId = G_CEnvironment()->m_serverEnvir.m_serverGroup;  // -0x6e
    // [H7PartyGlobal 2026-09-11 · P11/T2 调用点收窄] ORIG 0x08179cfb `movsbl %al,%eax;
    // mov %eax,-0x6a(%ebp)`——返回值按 **signed char 符号扩展**后 4B store；旧形
    // `movzbl` 在 growType>0x7f 时给 0x80..0xff 正值，与 ORIG 负值语义不同（真语义差）。
    // 只收窄本消费点，不动共享头返回类型（② 依 ① 教训改走窄修复，避免全 TU 劣化）。
    rec.m_growType = (signed char)user->getCurCharacGrowType();   // -0x6a
    rec.m_level = user->get_charac_level();                // -0x66
    rec.m_giftServerInfo = rec.m_serverId;                 // -0x62
    rec.m_giftGetCharacNo = 0;                             // -0x5e
    rec.m_chargePrice = rec.m_saleTime;                    // -0x5a = a5
    rec.m_freePrice = 0;                                   // -0x56
    rec.m_countNo = priceLabel;                            // -0x52 = 0x24(%ebp)
    rec.m_eachPrice = rec.m_saleTime;                      // -0x4e = a5
    // strcmp(accName,"mileage item"/"cera point")——ORIG 0x8179d49 源=0x20(%ebp) 实证
    if (strcmp(accName, "mileage item") == 0)
        rec.m_priceType = 2;
    else if (strcmp(accName, "cera point") == 0)
        rec.m_priceType = 3;
    else
        rec.m_priceType = 1;

    // ORIG 0x08179d82：memset(B+0xf5,0,0x10) → GetPeerIP2(B+0xf5,0x10)，
    // 失败（返回 0）时 strncpy(B+0xf5,"NONE",5)。
    memset(rec.m_userIp, 0, 0x10);
    if (user->m_network.GetPeerIP2(rec.m_userIp, 0x10) == 0)
        memcpy(rec.m_userIp, "NONE", 5);
    // [FL7GlobalData 2026-09-10 · u32 双字视图实测否定] 试
    // `(int)((const unsigned int*)&uid)[0/1]` 后我方反多 `lea`×2 + `add`（ndiff 12→14）
    // ⇒ 取址视图不可用；ORIG 0x08179e2x 的两段直读
    // （`mov -0x150(%ebp),%eax; mov -0x14c(%ebp),%edx; mov %eax,-0x36; mov %edx,-0x32`）
    // 对 4.4 的 `uid >> 32`（多 `mov %edx,%eax; xor %edx,%edx`）不可复现，维持原形记 N92 族。
    rec.m_tranNoLow = (int)(unsigned int)uid;                    // ORIG -0x36 = 0x28(%ebp)
    rec.m_tranNoHigh = (int)(unsigned int)(uid >> 32);           // ORIG -0x32 = 0x2c(%ebp)
    rec.m_buyType = payFlag;                                     // ORIG -0x2e = 0x30(%ebp)

    guard.operator->()->put_binary(reinterpret_cast<char*>(&rec), 0x10e);
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);

    // PuUser 标志：ORIG 08179e56..08179e73（BillingRecord.md）`call GetPuUser;
    // xor $0x1,%eax; test %al,%al; je skip` → 非 1（未认证）时补存（P22 bool 取反
    // xor 形态；原实现判定极性相反，按报告修正）。
    if (!user->GetPuUser())
        DB_SavePuUserCheck::makeRequest(user->get_acc_id());
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
                                                   CUser* user, char c)
{
    if (user->IsGameMasterMode())
        return true;
    if (goods->GetSticker() == 99)
        return false;
    if (c == 1 && goods->isGiftDisallowance_())
        return false;
    return true;
}

// _CheckEnableItemCondition —— ORIG 0x0817697c (T)
#include "GlobalData/_CheckEnableItemCondition.inc"
                                                   const CItem*& item,
                                                   std::vector<std::pair<int, int> >&)
{
    if (goods->GetGroupIdx() != 1)
    {
        // ORIG 0x081769c3：&& 短路结果落 %eax(0/1) 后 `test %al,%al; je`
        // 物化判（无局部槽）——(bool) 显式物化形。
        if ((bool)(goods->GetGroupIdx() == 11 && goods->GetSubGroupIndex() == 1))
            return true;
        item = goods->GetRelatedItem();
        if (item == 0)
            return false;
    }
    return true;
}

// _CheckEnableStateCondition —— ORIG 0x081768b0 (T)：恒真。
#include "GlobalData/_CheckEnableStateCondition.inc"
{
    return true;
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
                                                     char c, unsigned char uc,
                                                     stAvatarTypeSelect_t& out)
{
    // [b28d-solo-GlobalData] ORIG 0x081767a6：首次 GetRelatedItem 结果**无槽往返**
    // （`call; mov (%eax),%edx; add $0x10,%edx; mov (%edx),%edx; mov %eax,(%esp);
    // call *%edx`）⇒ 原码内联 `goods->GetRelatedItem()->IsAvatarItem()`（P0）；
    // 具名 related 局部会多 store/load 并把后续槽整体顶深。
    if (!goods->GetRelatedItem()->IsAvatarItem())
        return false;
    out.init();
    CEquipItem* equip = static_cast<CEquipItem*>(goods->GetRelatedItem());
    // ORIG 0x08176794 段（p167 MISSING `operator[](unsigned int) const`×4）：
    // 原码是 const 引用视图 ⇒ 声明 const 指针转型（P270 旋钮）。
    // ORIG 0x081767e4..0x081767f2 只有 equip(-0x10)/select(-0xc) 两个槽，
    // 中间无 vec 具名指针 ⇒ 内联 cast（P183 去缓存）。
    const std::vector<stAvatarTypeSelect_t>& select =
        *static_cast<const std::vector<stAvatarTypeSelect_t>*>(
            equip->getAvatarTypeSelect());
    // ORIG 0x0817680f `cmp -0x1c(%ebp),%al; jg` ⇒ 8 位有符号比较
    // （`(signed char)size() > c`；P287 同族），无具名 idx 局部。
    if (select.empty() || (signed char)select.size() <= c)
        return false;
    if (uc >= 100)
        return false;
    out.m_fieldc = select[(signed char)c].m_fieldc;
    out.m_field4 = select[(signed char)c].m_field4;
    out.m_field8 = select[(signed char)c].m_field8;
    out.m_field0 = select[(signed char)c].m_field0;
    return true;
}

// _GetItemString —— ORIG 0x0817645a (T)
#include "GlobalData/_GetItemString.inc"
                                        unsigned int len)
{
    // [b23S3C] ORIG 0x0817645a：无 switch/jump table——七条 if-else 链，每判重 call
    // GetItemIdx（P48 去缓存），命中才 call GetStackCount 作 snprintf 实参直落 0xc(%esp)。
    if (goods->GetItemIdx() == 1) { OS_API::snprintf(buf, len, "Continue Coin %d", goods->GetStackCount()); return; }
    else if (goods->GetItemIdx() == 0xca) { OS_API::snprintf(buf, len, "Mystic Coin %d", goods->GetStackCount()); return; }
    else if (goods->GetItemIdx() == 8) { OS_API::snprintf(buf, len, "Hands of Remy %d", goods->GetStackCount()); return; }
    else if (goods->GetItemIdx() == 0xf) { OS_API::snprintf(buf, len, "Kaleido Box %d", goods->GetStackCount()); return; }
    else if (goods->GetItemIdx() == 0x43d) { OS_API::snprintf(buf, len, "Manaade %d", goods->GetStackCount()); return; }
    else if (goods->GetItemIdx() == 0xe) { OS_API::snprintf(buf, len, "Golden Wax Candle %d", goods->GetStackCount()); return; }
    else if (goods->GetItemIdx() == 0xd8) { OS_API::snprintf(buf, len, "Mystic Cube %d", goods->GetStackCount()); return; }
    CItem* related = goods->GetRelatedItem();
    if (related == 0)
    {
        LogManager::logFormat(1, "localjapan/AppendVendingMachine.cpp", kPrettyGetItemString,
                              0x18d, "CashShop Relation Item is NULL [ITEM:%d]",
                              goods->GetItemIdx());
        return;
    }
    if (related->is_stackable() && goods->GetStackCount() >= 2)
        OS_API::snprintf(buf, len, "%s Count %d", related->GetItemName(), goods->GetStackCount());
    else
        strncpy(buf, related->GetItemName(), len);
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
{
    if (user == 0)
    {
        cMyTrace tr(kPrettyProcessBuy, 0x290, 0);
        tr("There is no User in ItemVendingMachine");
        return false;
    }

    // ORIG 0x08176a4e..0x08176abd 初始化序（Ghidra C 与反汇编双向核对）：
    //   0x08176a4e movl $0xffffffff,-0x78（groupIdx=-1）
    //   0x08176a55/-0x74、0x08176a5c/-0x70、0x08176a63/-0x6c、0x08176a6a/-0x68（4 累计量=0）
    //   0x08176a71 call vector<pair<int,int> >::vector（local_170=-0x16c）
    //   0x08176a7f rep stos 0x24 dword @-0x280（related 指针数组，非单变量）
    //   0x08176a95 rep stos 0x24 dword @-0x310（goods 指针数组）
    //   0x08176aab call CUser::resetUsedCera
    // 0x081769f6..0x081783e9 全区间不存在 $0x23 立即数 ⇒ 原码没有条目数上限钳制；
    // 循环上界每处现读 msg 字节（0x08176bc5/0x08177dcc 区 movzbl 0x36(%eax)）。
    int groupIdx = -1;
    int totalCera = 0, totalGold = 0, totalWin = 0, totalMileage = 0;

    std::vector<std::pair<int, int> > pairs;
    // ORIG 0x08176a7f/0x08176a9b：两数组各 `mov $0x24,%edx; rep stos`（内联清零，
    // 无 call memset）⇒ 源形用 = {0} 触发 4.4 内联 rep stos（P328⑨）。
    const CItem* related[0x24] = {0};   // ORIG local_284（逐条目 _CheckEnableItemCondition 输出）
    CCeraShopGoods* goods[0x24] = {0};  // ORIG local_314

    user->resetUsedCera();

    // [F2a-A轴·已试无效·N] 去 `const`（A 轴常量传播旋钮）：本件 ORIG 该两处
    //   （A08 筛查 idx 1250/1315，我方原为 `cmpl $0x22,-0x20(%ebp)`）确为槽读+寄存器
    //   比较 ⇒ 机理成立，但**方向不利**：横测 ours 1606→**1608**（+2，两处各 +1），
    //   ORIG 1600 ⇒ gap −6→**−8（劣化）**；本件我方指令数已多于 ORIG（N82 帧/槽膨胀），
    //   A 轴只会加深负 gap ⇒ 按「gap 严格减小」判据回滚，恢复 `const`。
    // 两处循环上限防御删除后本常量已无使用点，**结构保留不删**（Main 裁决 N）：
    //   ORIG 全函数无 `$0x22/$0x23` 只证「未参与比较」，不证「该局部不存在」；
    //   且 ORIG 帧 0x75c > 我方 0x69c ⇒ 删 4 字节会令帧更偏离 ORIG（P0 判据=ORIG 有无该槽）。
    const unsigned int MAX_ENTRIES = 0x23;   // 条目数上报上限（ProcessError 实参用）

    // ---- 第一遍：逐条目校验、总价累计 ----
    for (int i = 0; i < (int)msg.m_scalars.m_itemCount; ++i)
    {
        int productNo = msg.m_entrySection.m_arr[i].m_productNo;
        unsigned char slotByte = msg.m_entrySection.m_arr[i].m_avatarType0;
        unsigned char tagByte = msg.m_entrySection.m_arr[i].m_avatarType1;
        (void)slotByte; (void)tagByte;

        // ORIG 0x08176ad5 G_CDataManager() → 0x08176ae8 直调
        // _ZNK12CDataManager9FindGoodsEi（0x0835fde4，内部转发
        // CItemList::FindGoods），结果 0x08176aed 直存 goods[i]（无 getItemList 层）。
        goods[i] = G_CDataManager()->FindGoods(productNo);
        if (goods[i] == 0)
        {
            m_ceraShop.ProcessError(user, 0x15, groupIdx,
                                    msg.m_entrySection.m_arr[i].m_productNo, i + 1,
                                    msg.m_scalars.m_itemCount,
                                    (char)msg.m_scalars.m_buyFlag);
            goods[i] = 0;
            // ORIG 0x08176b95/0x08176bbb：每个校验失败分支 ProcessError + goods[i]=0
            // 之后固定跟一对 cMyTrace（具名槽 -0x150）：tr("[%s][%d]", PRETTY, line)。
            cMyTrace tr(__PRETTY_FUNCTION__, 0x2ad, 0);
            tr("[%s][%d]", __PRETTY_FUNCTION__, 0x2ad);
            continue;
        }

        groupIdx = goods[i]->GetGroupIdx();

        if (!_CheckEnableGiftCondition(goods[i], user, (char)msg.m_scalars.m_buyFlag))
        {
            m_ceraShop.ProcessError(user, 8, groupIdx,
                                    msg.m_entrySection.m_arr[i].m_productNo, i + 1,
                                    msg.m_scalars.m_itemCount,
                                    (char)msg.m_scalars.m_buyFlag);
            goods[i] = 0;
            cMyTrace tr(__PRETTY_FUNCTION__, 0x2b4, 0);   // ORIG 0x08176ca2/0x08176cc8
            tr("[%s][%d]", __PRETTY_FUNCTION__, 0x2b4);
            continue;
        }

        // IsBuyableRestrictItem(user, productNo, 1, i)：1=可购买（取反判不可购）。
        if (!CerashopAddRestrict::Manager::IsBuyableRestrictItem(user, productNo, 1, i))
        {
            m_ceraShop.ProcessError(user, 0x71, groupIdx,
                                    msg.m_entrySection.m_arr[i].m_productNo, i + 1,
                                    msg.m_scalars.m_itemCount,
                                    (char)msg.m_scalars.m_buyFlag);
            goods[i] = 0;
            cMyTrace tr(__PRETTY_FUNCTION__, 0x2bb, 0);   // ORIG 0x08176d9f
            tr("[%s][%d]", __PRETTY_FUNCTION__, 0x2bb);
            continue;
        }

        if (!_CheckEnableItemCondition(goods[i], related[i], pairs))
        {
            m_ceraShop.ProcessError(user, 0x11, groupIdx,
                                    msg.m_entrySection.m_arr[i].m_productNo, i + 1,
                                    msg.m_scalars.m_itemCount,
                                    (char)msg.m_scalars.m_buyFlag);
            goods[i] = 0;
            cMyTrace tr(__PRETTY_FUNCTION__, 0x2c2, 0);   // ORIG 0x08176e9f
            tr("[%s][%d]", __PRETTY_FUNCTION__, 0x2c2);
            continue;
        }

        if (!_CheckEnableStateCondition(goods[i], user))
        {
            m_ceraShop.ProcessError(user, 7, groupIdx,
                                    msg.m_entrySection.m_arr[i].m_productNo, i + 1,
                                    msg.m_scalars.m_itemCount,
                                    (char)msg.m_scalars.m_buyFlag);
            goods[i] = 0;
            cMyTrace tr(__PRETTY_FUNCTION__, 0x2c8, 0);   // ORIG 0x08176f89
            tr("[%s][%d]", __PRETTY_FUNCTION__, 0x2c8);
            continue;
        }

        int buyingCode = 0;
        if (!m_ceraShop._CheckBuyingCondition(user, goods[i], productNo,
                                              msg.m_entrySection.m_arr[i].m_avatarType0,
                                              (unsigned short)msg.m_entrySection.m_arr[i].m_avatarType1,
                                              (char)msg.m_scalars.m_buyFlag, buyingCode))
        {
            m_ceraShop.ProcessError(user, (unsigned short)buyingCode, groupIdx,
                                    msg.m_entrySection.m_arr[i].m_productNo,
                                    i + 1, msg.m_scalars.m_itemCount,
                                    (char)msg.m_scalars.m_buyFlag);
            goods[i] = 0;
            cMyTrace tr(__PRETTY_FUNCTION__, 0x2d6, 0);   // ORIG 0x08177105
            tr("[%s][%d]", __PRETTY_FUNCTION__, 0x2d6);
            continue;
        }

        if (groupIdx == 0)
        {
            stAvatarTypeSelect_t avatarSel;
            if (!_CheckEnableAvatarCondition(goods[i],
                                             (char)msg.m_entrySection.m_arr[i].m_avatarType1,
                                             msg.m_entrySection.m_arr[i].m_avatarType0,
                                             avatarSel))
            {
                m_ceraShop.ProcessError(user, 0x17, groupIdx,
                                        msg.m_entrySection.m_arr[i].m_productNo,
                                        i + 1, msg.m_scalars.m_itemCount,
                                        (char)msg.m_scalars.m_buyFlag);
                goods[i] = 0;
                cMyTrace tr(__PRETTY_FUNCTION__, 0x2ec, 0);   // ORIG 0x08177234
                tr("[%s][%d]", __PRETTY_FUNCTION__, 0x2ec);
                continue;
            }
            totalCera += avatarSel.m_fieldc;
            totalGold += avatarSel.m_field8;
            totalWin += avatarSel.m_field4;
        }
        else
        {
            totalCera += goods[i]->GetCeraPrice();
            totalGold += goods[i]->GetGoldPrice();
            totalWin += goods[i]->GetWinPoint();
        }
        totalMileage += goods[i]->GetMileagePrice();

        if (msg.m_scalars.m_buytype == 1 && goods[i]->isCeraPointDisallowance_())
        {
            m_ceraShop.ProcessError(user, 0x138b, groupIdx,
                                    msg.m_entrySection.m_arr[i].m_productNo, i + 1,
                                    msg.m_scalars.m_itemCount,
                                    (char)msg.m_scalars.m_buyFlag);
            goods[i] = 0;
            cMyTrace tr(__PRETTY_FUNCTION__, 0x305, 0);   // ORIG 0x0817738f
            tr("[%s][%d]", __PRETTY_FUNCTION__, 0x305);
            continue;
        }
    }

    // ---- 总校验 ----
    if (!m_ceraShop._CheckValidCount((const CCeraShopGoods**)goods))
    {
        m_ceraShop.ProcessError(user, 1, -1, msg.m_entrySection.m_arr[0].m_productNo, 1, 1,
                                (char)msg.m_scalars.m_buyFlag);
        return true;
    }

    // ---- 总价资金校验 ----
    int errCode = 0;
    if (user->GetCera() < totalCera) errCode = 0xb;
    else if (user->getCurCharacMoney() < totalGold) errCode = 10;
    else if (user->GetWinPoint() < totalWin) errCode = 0xc;
    else if (user->GetMileage() < totalMileage) errCode = 0x3ed;
    if (msg.m_scalars.m_buytype == 1)
    {
        errCode = 0;
        if (user->GetCeraPoint() < (unsigned int)totalCera)
            errCode = 0x138a;
    }
    if (errCode != 0)
    {
        LogManager::logFormat(
            1, "localjapan/AppendVendingMachine.cpp", kPrettyProcessBuy, 0x34b,
            "[ERROR] Not enough UserID[%s] ACera[%d] AGold[%d] AWP[%d] AMile[%d]",
            user->getCurCharacName(), totalCera, totalGold, totalWin, totalMileage);
        LogManager::logFormat(
            1, "localjapan/AppendVendingMachine.cpp", kPrettyProcessBuy, 0x34c,
            " / UCera[%d] UGold[%d] UWP[%d] AMile[%d]\n",
            user->GetCera(), user->getCurCharacInvenR()->get_money(),
            user->GetWinPoint(), user->GetMileage());
        m_ceraShop.ProcessError(user, (unsigned short)errCode, -1,
                                msg.m_entrySection.m_arr[0].m_productNo, 1, 1,
                                (char)msg.m_scalars.m_buyFlag);
        return true;
    }

    // ---- 总价资金校验 ----
    // ORIG 余额槽宽度实证（64 位比较序列 + max 模板实参 mangled）：
    //   0x0817765a call getCurCharacMoney → 0x08177661 `sar $0x1f,%edx`（**符号**
    //     扩展）→ mov %eax,-0x50 / mov %edx,-0x4c ⇒ curGold 是 signed long long。
    //   0x08177670 call GetWinPoint → 0x08177675 `mov $0x0,%edx`（**零**扩展）
    //     → -0x48/-0x44 ⇒ curWin 是 unsigned long long。
    //   0x0817768d call GetMileage → mov %eax,-0x38（单 dword）⇒ curMileage 是 int。
    // 扣减用 _ZSt3maxIi（0x08177991）+ _ZSt3maxIx ×2（0x081779e2/0x08177a37；
    //   c++filt 实证 `x` = **long long**（有符号），非 unsigned long long
    //   ⇒ 两个实参槽均须 long long 类型才能推导出同一实例）。
    long long curGold = user->getCurCharacMoney();
    long long curWin = user->GetWinPoint();
    int curMileage = user->GetMileage();
    int curSec = GlobalData::s_systemTime_.getCurSec();

    for (unsigned int j = 0; j < (unsigned int)msg.m_scalars.m_itemCount; ++j)
    {
        if (goods[j] == 0)
            continue;
        groupIdx = goods[j]->GetGroupIdx();

        int ceraPrice = 0, goldPrice = 0, winPrice = 0, avatarField = 0;
        if (groupIdx == 0)
        {
            stAvatarTypeSelect_t avatarSel;
            if (!_CheckEnableAvatarCondition(goods[j],
                                             (char)msg.m_entrySection.m_arr[j].m_avatarType1,
                                             msg.m_entrySection.m_arr[j].m_avatarType0,
                                             avatarSel))
            {
                m_ceraShop.ProcessError(user, 0x17, groupIdx,
                                        msg.m_entrySection.m_arr[j].m_productNo,
                                        j + 1, msg.m_scalars.m_itemCount,
                                        (char)msg.m_scalars.m_buyFlag);
                goods[j] = 0;
                cMyTrace tr(__PRETTY_FUNCTION__, 0x37b, 0);   // ORIG 0x081777d6
                tr("[%s][%d]", __PRETTY_FUNCTION__, 0x37b);
                continue;
            }
            ceraPrice = avatarSel.m_fieldc;
            goldPrice = avatarSel.m_field8;
            winPrice = avatarSel.m_field4;
            avatarField = avatarSel.m_field0;
        }
        else
        {
            ceraPrice = goods[j]->GetCeraPrice();
            goldPrice = goods[j]->GetGoldPrice();
            winPrice = goods[j]->GetWinPoint();
        }
        int mileagePrice = goods[j]->GetMileagePrice();
        int productNo = msg.m_entrySection.m_arr[j].m_productNo;

        int perErr = 0;
        if (user->GetCera() < (unsigned int)ceraPrice) perErr = 0xb;
        else if (user->getCurCharacInvenR()->get_money() < goldPrice) perErr = 10;
        else if (user->GetWinPoint() < (unsigned int)winPrice) perErr = 0xc;
        if (msg.m_scalars.m_buytype == 1)
        {
            perErr = 0;
            if (user->GetCeraPoint() < (unsigned int)ceraPrice)
                perErr = 0x138a;
        }
        // 0x08177917..0x08177962：三条**余额**校验（不是现读 user）。
        //  ① -0x50/-0x4c(curGold) vs sext(goldPrice) → 有符号 64 位比较，
        //     `jl/jg/cmp lo;jbe` 三段式 ⇒ `curGold < goldPrice` → perErr=10。
        //  ② -0x48/-0x44(curWin) vs zero-ext(winPrice) → 无符号 64 位比较 → 0xc。
        //  ③ -0x38(curMileage) vs -0x3c(mileagePrice) 32 位 `jge skip` → 0x3ed。
        // 0x08177917..0x08177a44：ORIG 把三条余额校验与**扣减**全部排在
        // 0x08177a47 `cmpl $0x0,-0x28(perErr); je 正常路径` 之前 ⇒ 即使本条目
        // 判错，余额照样被扣（原码行为，非笔误）。扣减三处 = std::max：
        // _ZSt3maxIi @0x08177991、_ZSt3maxIx @0x081779e2、@0x08177a37
        // （Itanium ABI `x` = unsigned long long，解释 i386 上为何读 0x4(%eax)）；
        // 声明顺序必须 diff 在前、zero 在后（ORIG 先 `mov %eax,-0xc0/-0xb8/-0xa8`
        // 后 `movl $0x0,-0xbc/-0xb0/-0xa0`，技巧 21 后声明占近 ebp 槽）。
        if (curGold < goldPrice) perErr = 10;         // 0x08177917 有符号 64 位比较
        if (curWin < (unsigned int)winPrice) perErr = 0xc;   // 0x08177935 winPrice 零扩展
        if (curMileage < mileagePrice) perErr = 0x3ed;             // 0x08177953 32 位
        {
            int diffMileage = curMileage - mileagePrice;   // ORIG -0xc0（先算差值）
            int zeroMileage = 0;                           // ORIG -0xbc
            curMileage = std::max(zeroMileage, diffMileage);   // 0x08177991
        }
        {
            long long diffGold = curGold - goldPrice;      // ORIG sub/sbb → -0xb8/-0xb4
            long long zeroGold = 0;                        // ORIG -0xb0/-0xac
            curGold = (long long)std::max(zeroGold, diffGold);   // 0x081779e2
        }
        {
            long long diffWin = curWin - (unsigned int)winPrice;  // ORIG -0xa8/-0xa4
            long long zeroWin = 0;                                // ORIG -0xa0/-0x9c
            curWin = std::max(zeroWin, diffWin);           // 0x08177a37
        }

        if (perErr != 0)
        {
            LogManager::logFormat(
                1, "localjapan/AppendVendingMachine.cpp", kPrettyProcessBuy, 0x3b3,
                "IPG ERROR [Dispatcher_BuyCeraShopItem::dispatch_sig] UserID[%s] ACera[%d] AGold[%d] AWP[%d] AMile[%d]",
                user->getCurCharacName(), ceraPrice, goldPrice, winPrice, mileagePrice);
            LogManager::logFormat(
                1, "localjapan/AppendVendingMachine.cpp", kPrettyProcessBuy, 0x3b4,
                " / UCera[%d] UGold[%d] UWP[%d] UMile[%d]\n",
                user->GetCera(), user->getCurCharacInvenR()->get_money(),
                user->GetWinPoint(), user->GetMileage());
            // 0x08177b1e..0x08177b30：第三处 logFormat 的 %d 实参是**现读**
            // msg 条目（`mov -0x2c,%eax; imul $0x1cf; lea (msg,…); add $0x30;
            // mov 0x9(%eax)` = entry+0x39 = m_productNo），不是局部 productNo。
            LogManager::logFormat(
                1, "localjapan/AppendVendingMachine.cpp", kPrettyProcessBuy, 0x3b9,
                "Error Goods No : %d", msg.m_entrySection.m_arr[j].m_productNo);
            m_ceraShop.ProcessError(user, (unsigned short)perErr, groupIdx,
                                    msg.m_entrySection.m_arr[j].m_productNo,
                                    j + 1, msg.m_scalars.m_itemCount,
                                    (char)msg.m_scalars.m_buyFlag);
            goods[j] = 0;
            cMyTrace tr(__PRETTY_FUNCTION__, 0x3bb, 0);   // ORIG 0x08177bf4
            tr("[%s][%d]", __PRETTY_FUNCTION__, 0x3bb);
            continue;
        }

        // （扣减已按 ORIG 0x08177962..0x08177a44 前移至上方 perErr 判定之前）

        if (!user->IsRestrictedGoods(goods[j]->GetFeaturedIdx()))
        {
            Stream* stream = GlobalData::s_stream_pool->Acquire(
                "localjapan/AppendVendingMachine.cpp", 0x3cb);
            CStreamGuard guard(stream, false);
            // ORIG 0x08177d3b..0x08177d59：`lea -0x178; mov %eax,(%esp);
            // call CStreamGuard::operator->(); mov %eax,(%esp);
            // call GetInBuffer<stCeraShopIPGRequest_t>()` ⇒ 源码就是
            // guard->GetInBuffer<...>()（CStreamGuard::operator-> 返回 this），
            // 不得引入 g 中转变量（技巧 35/36：代理链 + 删中转槽）。
            stCeraShopIPGRequest_t* req =
                guard->GetInBuffer<stCeraShopIPGRequest_t>();
            req->m_productNo = productNo;
            req->m_groupIdx = goods[j]->GetGroupIdx();
            req->m_stackCount = (unsigned short)goods[j]->GetStackCount();
            req->m_ceraPrice = (unsigned int)ceraPrice;
            req->m_goldPrice = (unsigned int)goldPrice;
            req->m_winPoint = (unsigned int)winPrice;
            req->m_itemIdx = goods[j]->GetItemIdx();
            req->m_avatarType0 = msg.m_entrySection.m_arr[j].m_avatarType0;
            req->m_avatarType1 = msg.m_entrySection.m_arr[j].m_avatarType1;
            req->m_avatarField = (unsigned int)avatarField;
            req->m_featuredIdx = goods[j]->GetFeaturedIdx();
            req->m_itemCount = msg.m_scalars.m_itemCount;
            req->m_seq = j + 1;
            req->m_curSec = curSec;
            req->m_buyFlag = msg.m_scalars.m_buyFlag;
            req->m_accId = msg.m_scalars.m_accId;
            req->m_characNo = msg.m_scalars.m_characNo;
            req->m_buytype = msg.m_scalars.m_buytype;
            req->m_subCount = msg.m_entrySection.m_arr[j].m_subCount;
            // [F2a-轴B+A 双旋钮·命中保留] ORIG 0x08177f2d/0x08177f30：
            //   `movzbl 0x3d(%eax),%eax; movzbl %al,%eax; cmp -0x20(%ebp),%eax;
            //    setg %al; test %al,%al; jne <回边>`（**有符号序 + 单条比较**）。
            //   我方原形：`unsigned int k` + `k < m_subCount && k < MAX_ENTRIES`
            //   ⇒ `cmp -0x20(%ebp),%eax; jbe` + 多出 `cmpl $0x22,-0x20(%ebp); ja`
            //   （发明上限防御；ORIG 全函数无任何 `$0x22/$0x23` 立即数，经报告段
            //    17..1620 全文 grep 实证 ⇒ 按 P171 照抄 ORIG 删除）。
            //   实改（单点）：`int k` + `(int)req->m_subCount` + 去 `&& k < MAX_ENTRIES`
            //   ⇒ 本函数 insns **1606→1600 = ORIG 1600（gap −6→0）**，该段四条形
            //   与 ORIG 逐条一致（cmp/setg/test/jne）；TU 四数
            //   899/1791/11/38 逐值不变（净 identical 未降）。
            //   **[语义轴：删除发明防御，不受 gap/ndiff 闸门约束]**（该件本为 DIFF、
            //   无 identical 回退风险；口径见 Main 分流判据）。
            //   [Main 裁决已落地] optCount 循环同族亦删（见下）；两点全删后
            //   insns **1606→1596**（ORIG 1600，gap −6→**+4**）——差值系**删除我方
            //   发明防御**所致（非劣化），Main 口径：「删除发明/多写代码以忠实 ORIG
            //   不受 gap 闸门约束」（该件本为 DIFF，无 identical 回退风险）。
            for (int k = 0; k < (int)req->m_subCount; ++k)
            {
                // 子项数组：entry+0x0e 起每项 5 字节 { int idx; uchar val; }
                // ORIG 0x08177ed6/0x08177f04：两处各自重算源地址直读直存
                // （`mov 0xe(%eax),%edx; mov %edx,0x8(..)` / `movzbl 0x42(%eax);
                //  mov %dl,0xc(..)`），无 memcpy call、无源字段缓存（P183/P329）。
                // [Main「视图 cast 反模式」修法 · R2 批准 · 探针 /tmp/l7/probe7.cpp]
                // 具名数组视角直访问：ORIG 0x08177edf `mov %edx,0x8(%eax,%ecx,8)` /
                // 0x08177f0d `mov %dl,0xc(%eax,%ecx,8)`（stride 8 scaled 直存）；
                // 旧 `*reinterpret_cast<int*>(&m_subEntries[k][0])=` 形被 4.4 折成
                // add+shl 再普通 store（probe7 f1）⇒ 改 SigTypes_E.h 的 union 具名
                // 视角 m_subItems（probe7 f2 = ORIG 形）。布局/sizeof/mangled 不变。
                req->m_subItems[k].m_itemIdx =
                    msg.m_entrySection.m_arr[j].m_subEntries[k].m_itemIdx;
                req->m_subItems[k].m_value =
                    msg.m_entrySection.m_arr[j].m_subEntries[k].m_value;
            }
            req->m_optCount = msg.m_entrySection.m_arr[j].m_optCount;
            // [F2a-轴B+照抄 ORIG·Main 裁决落地] optCount 循环同族：ORIG
            //   0x08178010/0x08178013 = `movzbl 0xed(%eax),%eax; movzbl %al,%eax;
            //   cmp -0x1c(%ebp),%eax; setg %al; test %al,%al; jne <回边>`
            //   （有符号序 + 单条比较；ORIG 全函数无 `$0x22/$0x23` 立即数）。
            //   Main 裁决：删除我方发明防御不受 gap 闸门约束（结构忠实优先）⇒
            //   `int k` + `(int)req->m_optCount` + 去 `&& k < MAX_ENTRIES`。
            //   实测：删后 insns 1600→1596、该段现为 `cmp -0x1c(%ebp),%eax; setg %al;
            //   test %al,%al; jne <回边>`（与 ORIG 逐条一致），全函数已无 `$0x22`；
            //   TU 四数 899/1791/11/38 仍逐值不变。
            for (int k = 0; k < (int)req->m_optCount; ++k)
            {
                // 选项数组：entry+0xbe 起每项 8 字节 { ulong itemIdx; ushort a; ushort b; }
                //（ORIG k 达 0x22 时读入邻条目尾，线格式遗留，数组越界读等价保留）
                // 选项项布局：{ ulong itemIdx(4, at +0); ushort a(2, at +4); ushort b(2, at +6) }
                //（dest 为 (k+0x2c)*8 + {4,8,10}，即 m_optEntries[k] 的 +0/+4/+6）。
                // ORIG 0x08177f87/0x08177fb4/0x08177fe3：三次独立重算源地址直读直存
                // （`mov 0xe(%eax),%edx; mov %edx,0x4(%eax,%ecx,8)` 形），
                // 无 memcpy call、无 srcOpt/itemIdx/a/b 具名缓存（P183/P329）。
                // 同上：ORIG 0x08177f87/0x08177fb4/0x08177fe3 三处 scaled 直存
                // （`mov …,0x4/0x8/0xa(%eax,%ecx,8)`）⇒ 用 m_optItems 具名视角。
                req->m_optItems[k].m_itemUid =
                    msg.m_entrySection.m_arr[j].m_optEntries[k].m_itemUid;
                req->m_optItems[k].m_a =
                    msg.m_entrySection.m_arr[j].m_optEntries[k].m_a;
                req->m_optItems[k].m_b =
                    msg.m_entrySection.m_arr[j].m_optEntries[k].m_b;
            }

            guard.operator*()->operator<<(mileagePrice);

            // ORIG 清零全部内联（4.1 rep stos/展开，无 call memset）：
            //   0x0817803b ceraPriceStr[-0x1f0] 0x24 → rep stos 0x9 dwords
            //   0x08178051 itemIdxStr[-0x18c] 0x14 → 5×movl
            //   0x08178083 uniqueStr[-0x1a1] 0x15 → 对齐剥离慢路
            //   0x081780e7 itemStr[-0x710] 0x400 → rep stos 0x100 dwords
            //   0x081780fd productStr[-0x1a9] 0x8 → 2×movl
            // ⇒ 源形用 = {0} 触发 4.4 内联清零（P312/P328⑨）；memset 在 4.4 恒 call。
            char itemStr[0x400] = {0};
            char ceraPriceStr[0x24] = {0};
            char productStr[8] = {0};
            char itemIdxStr[0x14] = {0};   // ORIG -0x18c（0x08178137 取址目标，结果未被使用）
            char uniqueStr[0x15] = {0};
            // ORIG 三次 snprintf 的**顺序与实参**（全部 OS_API::snprintf）：
            //  ① 0x08178140 snprintf(-0x18c, 0x14, "%d", goods[j]->GetItemIdx())
            //     （0x0817811e call _ZNK14CCeraShopGoods10GetItemIdxEv 结果作 %d，
            //      输出缓冲此后不再被读 ⇒ 原码遗留的死写入，照抄）
            //  ② 0x08178165 snprintf(-0x1f0, 0x24, "%d", ceraPrice[-0x58])
            //  ③ 0x0817819c snprintf(-0x1a9, 0x8, "%d", 现读 msg entry+0x39)
            OS_API::snprintf(itemIdxStr, sizeof(itemIdxStr), "%d",
                             goods[j]->GetItemIdx());
            OS_API::snprintf(ceraPriceStr, sizeof(ceraPriceStr), "%d", ceraPrice);
            OS_API::snprintf(productStr, sizeof(productStr), "%d",
                             msg.m_entrySection.m_arr[j].m_productNo);
            (void)itemIdxStr;
            _GetItemString(goods[j], itemStr, 0x400);
            // ORIG 0x081781d6..0x08178360（P5）：三分支各取 s_pIPGHelper vptr+0x18/
            // +0x10/+0x14 槽直调（slot6 IPGOutputSimulation / slot4 IPGOutputPoint /
            // slot5 IPGOutput，TaiwanCash.h 声明序实证）。每分支前各有一对
            // 0x081781eb/0x081781f3（及 0x0817827c/0x08178284、0x081782fc/0x08178304）
            // `call CStreamGuard::operator->()` + `call CStreamGuard::Get()` ⇒
            // 尾实参必须写 guard->Get()（链式，3 次），不得缓存到 theStream；
            // 也不得显式 guard.~CStreamGuard()（ORIG 仅 2 次 D1：作用域尾 1 次 +
            // EH 0x081783b0 1 次；显式调用会造成双重析构）。
            // 另：uniqueNo 与 characName 同为**分支内内联实参**（ORIG 三次
            // 0x08178210/0x081782a1/0x08178321 GenUniqueNo + 三次
            // 0x08178200/0x08178291/0x08178311 getCurCharacName）⇒ 不得提到
            // 分支外缓存成局部（否则各分支少 2 次 call）。
            if (ceraPrice == 0)
                GlobalData::s_pIPGHelper->IPGOutputSimulation(
                    user, productStr, ceraPriceStr,
                    WongWork::CGenUniqueNo::GenUniqueNo(uniqueStr), itemStr,
                    user->getCurCharacName(), guard->Get());
            else if (msg.m_scalars.m_buytype == 1)
                GlobalData::s_pIPGHelper->IPGOutputPoint(
                    user, productStr, ceraPriceStr,
                    WongWork::CGenUniqueNo::GenUniqueNo(uniqueStr), itemStr,
                    user->getCurCharacName(), guard->Get());
            else
                GlobalData::s_pIPGHelper->IPGOutput(
                    user, productStr, ceraPriceStr,
                    WongWork::CGenUniqueNo::GenUniqueNo(uniqueStr), itemStr,
                    user->getCurCharacName(), guard->Get());
        }
        else
        {
            m_ceraShop.ProcessError(user, 0x14, groupIdx,
                                    msg.m_entrySection.m_arr[j].m_productNo,
                                    j + 1, msg.m_scalars.m_itemCount,
                                    (char)msg.m_scalars.m_buyFlag);
            goods[j] = 0;
            cMyTrace tr(__PRETTY_FUNCTION__, 0x3c1, 0);   // ORIG 0x08177cdc/0x08177d02
            tr("[%s][%d]", __PRETTY_FUNCTION__, 0x3c1);
        }
    }

    return true;
}
#include "GlobalData/Secu_DataControl.inc"

// encryptAES_ 定义在 MouseRegisterEJ.cpp（保留方，0x148 更近 ORIG 0x14d）；此 TU 仅调用。
char* encryptAES_(CRijndael* cipher, char* inData, char* outHex);

// Secu_DataControl::UpdateSecurityServiceState —— ORIG T 0x08286e40
// [0x08286e40,0x08287081]。service==0：SecurityCard 已激活→init；
// service==1：CPad 已激活→若已认证则 flag=1，否则将 msg 截 8 字节 AES 加密后
// setFailCnt(0)+setPassword；任一分支生效则发送 0x159 通知包（service/flag）。
// [b28d-18 N78] EXTRA memset×2：4.1 内联清零 vs 4.4 call（N78） | ORIG 0x08286e40
// [b28d-solo-GlobalData] DIFF 封顶：p167 EXTRA memset×2 = N78（4.1 内联清零/4.4 call）；
// MISSING 空 ⇒ 语义无缺口。
// [G7-sub 2026-09-11 判词] Secu_DataControl::UpdateSecurityServiceState（ORIG T 0x08286e40）：
// 私有 .o 实测 ORIG 151 / 我方 153（gap 2，我方多）。逐条对照残差**仅两处**：
//   · 我方 145 位多 `jmp <epilogue>; nop` 尾岛（ORIG 0x08270777 自 PacketGuard D1 call
//     直落 epilogue）⇒ 已知 N121（4.4 epilogue-threading），否定早返/共享尾标签/
//     显式 return 三形（P285 反向）。
//   · 数据符/字符串归一后其余全等。p167 调用点差集 CLEAR（25/25，仅别名）。
// 结论：**封顶，无改动**（语义完成，byte 层 N）。
#include "GlobalData/UpdateSecurityServiceState.inc"
                                                  unsigned char flag, char* msg)
{
    if (user == 0)
    {
        return;
    }
    bool done = false;
    // ORIG 0x08286e6c..0x08286e7c：`movzbl service; test %eax,%eax; je L0;
    // cmp $0x1,%eax; je L1; jmp end`（形参载入 + 寄存器比较链）⇒ 源形是
    // `switch (service)`（P251/P286；if/else-if 在 4.4 恒折 cmpb 内存直比）。
    switch (service)
    {
    case 0:
        // ORIG 0x08286e81..0x08286eaf：getSecurityCard() 结果直作 receiver，
        // 全程无栈槽往返（P0/§4.0 槽-局部双向对应律）⇒ 禁具名指针局部。
        if (user->getSecurityCard()->isActivate())
        {
            user->getSecurityCard()->init();
            done = true;
        }
        break;
    case 1:
        if (user->getPad()->isActivate())
        {
            // ORIG 0x08286ee6..0x08286fcb：`call isCertified; xor $1; test %al,%al;
            // je <flag=1 块>` ⇒ 源形是否定式 `if (!isCertified())` 且 flag=1 走
            // else 块置于函数尾（P168a②/P292 判据）。
            if (!user->getPad()->isCertified())
            {
                // ORIG 0x08286ef1..0x08286f15：两段常量内联 rep stos（9 dword
                // = 36B）⇒ 源形 `char buf[36] = {0}`（P214③/P312/P328⑨），
                // 写 memset 在 4.4 退化成 call（N78 可翻形）。
                char plain[36] = {0};
                char enc[36] = {0};
                strncpy(plain, msg, 8);
                cMyTrace trace(
                    __PRETTY_FUNCTION__,
                    0xa0, 0);
                trace("[Secu_Web_Conn] (%s)  ", msg);
                encryptAES_(GlobalData::s_pcryptRijndael_MousePassword_, plain, enc);
                user->getPad()->setFailCnt(0);
                user->getPad()->setPassword(enc);
                done = true;
            }
            else
            {
                flag = 1;
            }
        }
        break;
    }
    if (done)
    {
        PacketGuard guard;
        guard.put_header(0, 0x159);
        guard.put_byte(service);
        guard.put_byte(flag);
        guard.finalize(true);
        user->Send(guard);
    }
}

#include "GlobalData/GetConnInfo.inc"
{
    // ORIG 0x080ec9ca：遍历 groupMap，逐组 QueryConnInfo(idx, group, infoMap)；
    // 任一失败返回 false，全部成功返回 true。
    for (DBGroupMap::iterator it = groupMap.begin(); it != groupMap.end(); ++it)
    {
        if (!QueryConnInfo(it->first, it->second, infoMap))
        {
            return false;
        }
    }
    return true;
}

// [b28d-18 N82] 残差=槽位/STL 展开（N82/N89） | ORIG 0x080ec760
// [SweepGlobalData] 旧 gap 2 缺口修正：cMyTrace 两语句 → 单语句临时链
// （ORIG 0x080ec7ba..0x080ec864 五实参先于 ctor 求值，P37/P87/P329）后
// 117=117、call 序列 ratio=1.000（gap 0）；残余 = N82 槽位/寄存器。
#include "GlobalData/Connect.inc"
{
    // ORIG 0x080ec760：按值遍历连接表；idx==0/5 跳过；cMyTrace 记录
    // "Open(type:%2d, %s/%d/%s/%s)"（.rodata 0x8b356a4）；Open 失败即 false。
    // [b28d-solo-GlobalData] p167 修：ORIG 全程逐字段重调 operator->（ptEv×7，
    // P157/P183 禁 idx/info 缓存）；cMyTrace 为 5 参重载 `C1EPKciibb`
    // （str, 0x4e, 9, true, true；旧 3 参形 MISSING 5 参 + EXTRA 3 参）。
    // [b28d-solo-GlobalData N·Main 裁决] STDBConnInfo 偏移两函数证据冲突：本件
    // ORIG 0x080ec760 段为 port@0x18/db@0x1c/user@0x46，而 QueryConnInfo
    // 0x080eca6c `add $0x42,%edx` 与现布局 m_db@0x42 直证吻合 ⇒ 判定本件实参序/
    // 字段名映射与我们不同（p167 CLEAR，语义等价），**维持 QueryConnInfo 直证
    // 布局**，不重排 CEnvironment.h 的 STDBConnInfo（风险远超收益）⇒ N 封顶。
    for (DBInfoMap::iterator it = infoMap.begin(); it != infoMap.end(); ++it)
    {
        // [b28d-solo-GlobalData 判据①] ORIG 0x080ec790/0x080ec7a1/0x080ec7b4 极性：
        // `test;je T(真)` + `cmp $5;jne F` + `test %al;jne <++it>` ⇒ 原码是
        // **正极性守卫** `if (a==0 || a==5) continue;`（旧 `if(!(…)){body}` 形把
        // 两侧跳转落点互换：ours je→F/jne→body，ORIG je→T/jne→跳过体）。
        if (it->first == 0 || it->first == (ENUM_DB_HANDLE_IDX)5)
        {
            continue;
        }
        // ORIG 0x080ec7ba..0x080ec864（P37/P87/P329）：五个实参在 cMyTrace
        // ctor **之前**全部求值（m_host/-0x40、m_port/-0x3c、m_db/%edi、
        // m_user/%esi、first/%ebx），随后 ctor + operator() ⇒ 源形是单语句
        // 临时链 `cMyTrace(...)(...)`，不是具名对象两语句。
        cMyTrace("CDBConnector::Connect", 0x4e, 9, true, true)
            // [GlobalData-判据①] 实参序照 ORIG 0x080ec7c8(+0x46=db)/0x080ec7d9
            // (+0x1c=user)：ORIG 先算 arg5=it->second.m_db(+0x46)、再 arg4=
            // it->second.m_user(+0x1c) ⇒ 原码 arg4/arg5 为 (user, db)；旧写法
            // (db, user) 反序（R2L 先算 user+4）。注意 +0x04 基址来自 pair 的
            // second 偏移，与 CEnvironment.h 布局一致（QueryConnInfo 0x080ecb53
            // db@+0x42 直证）。
            ("Open(type:%2d, %s/%d/%s/%s)", (int)it->first, it->second.m_host,
             it->second.m_port, it->second.m_user, it->second.m_db);
        if (!Open(it->first, (ENUM_SERVER_GROUP)0, it->second))
        {
            return false;
        }
    }
    return true;
}

// [b28d-solo-GlobalData] 归零 67=67：group 内联为实参表达式复现 R2L 装载序
// （先 &second 后 m_tail；具名/register 局部均提前，P52/P157）；p167 CLEAR | ORIG 0x080ec8f8
#include "GlobalData/ConnectWithServerGroup.inc"
{
    // ORIG 0x080ec8f8：遍历连接表；idx==0 跳过；以 +0x168 处 server group
    // 为组参数调 Open；失败即 false。
    for (DBInfoMap::iterator it = infoMap.begin(); it != infoMap.end(); ++it)
    {
        // ORIG 0x080ec91d/0x080ec93b/0x080ec949/0x080ec954：每次字段访问
        // 重新调用 operator->（4 次 ptEv，P157 逐字段重读，禁引用缓存）。
        // ORIG 0x080ec926 `sete %al; test; jne <跳过>` = 负极性守卫（P222）。
        if (it->first == (ENUM_DB_HANDLE_IDX)0)
        {
            continue;
        }
        // ORIG 080ec946（ConnectWithServerGroup.md）：`mov 0x168(%eax)`（eax=pair 基址）
        // = second+0x164 = STDBConnInfo::m_tail（存 server group；Open 第 2 实参 %ebx）。
        // 原写 +0x160 系偏移误读（pair/second 基混淆），此处以具名字段修正为 +0x164。
        // ORIG 0x080ec954 起装载序（R2L）：先 `lea 0x4(%eax),%esi`（第3实参
        // &second），再 `mov 0x168(%eax),%eax; mov %eax,%ebx`（第2实参 m_tail），
        // 最后 `mov (%eax),%eax`（第1实参 first）⇒ group 必须内联为实参表达式
        // （具名/register 局部都会把 m_tail 装载提前）。
        if (!Open(it->first,
                  static_cast<ENUM_SERVER_GROUP>(it->second.m_tail),
                  it->second))
        {
            return false;
        }
    }
    return true;
}

// ---- CDBConnector ----
#include "GlobalData/CDBConnector.inc"
    : m_dbMgr(dbMgr)   // ORIG C1 0x080ec5fc：先存 DBMgr*，再默认构造成员 STDBConnInfo。
{
}
#include "GlobalData/dtor_CDBConnector.inc"
#include "GlobalData/Init_2.inc"
{
    // ORIG 0x080ec620：rep movsl 0x5a dword（结构整体赋值，P83）→ this+4；OpenMaster()。
    // [b28c-lane17] 返回形态 `xor $1; test; je` = if(!OpenMaster()) return false; return true;
    m_connInfo = info;
    if (!OpenMaster())
        return false;
    return true;
}
#include "GlobalData/OpenMaster.inc"
{
    // ORIG 0x080ec67c：Open((ENUM_DB_HANDLE_IDX)0, (ENUM_SERVER_GROUP)0, this+4)
    // 结果取反返回。ORIG 0x080ec6aa：xor+test 后 fall-through 块为 mov $0、
    // je 目标块为 mov $1（技巧 17：fall-through=源码首分支）——
    // `if (!Open()) return false; return true;`（与 ORIG 机器码值域一致）。
    if (!Open((ENUM_DB_HANDLE_IDX)0, (ENUM_SERVER_GROUP)0, m_connInfo))
        return false;
    return true;
}
#include "GlobalData/CloseMaster.inc"
{
    // ORIG 0x080ec6bc：GlobalData::s_db_mgr->Close((ENUM_DB_HANDLE_IDX)0)。
    GlobalData::s_db_mgr->Close((ENUM_DB_HANDLE_IDX)0);
}
#include "GlobalData/Close.inc"
{
    // ORIG 0x080ec668：转发 CloseMaster()。
    CloseMaster();
}
#include "GlobalData/Open.inc"
                        STDBConnInfo& info)
{
    // ORIG 0x080ec6da：info 首字节为 0（m_host 空）→ 直接成功；否则调 DBMgr::Open。
    // [b28c-lane17] 实参地址序按 ORIG 压栈槽逐条对齐：p2=+0x00(m_host)、
    // p3=*(+0x14)(m_port)、p4=+0x42(m_db)、p5=+0x18(m_user)、p6=+0x2d(m_pass)
    // （原实现把 m_pass 传成 p2、m_host 传成 p6 —— 真语义 bug，此处修正）。
    if (info.m_host[0] == 0)
    {
        return true;
    }
    if (!GlobalData::s_db_mgr->Open(idx, info.m_host, (unsigned int)info.m_port,
                                    info.m_db, info.m_user, info.m_pass, group))
    {
        return false;
    }
    return true;
}
// ---- CDBConnector::QueryConnInfo ORIG @ 0x080eca6c（U3ShopDb 批次） ----
// 用主 DB 句柄(0,0) 查 db_connect 表，逐列填 STDBConnInfo 后放入 outMap[idx]。
// 列序：db_name(0x1f→m_db) / db_ip(0x11→m_host) / db_port(→m_port) /
//       db_userid(0x15→m_user) / db_passwd（先清零 60 字节，0x3c→+0x2d 起）。
// 密文经 CEnvironment::DecryptValue 解出（ORIG 解密结果未回写结构，仅校验）。
// [b28d-18 N82] 残差=槽位/STL 展开（N82/N89） | ORIG 0x080eca6c
// [b28d-solo-GlobalData] DIFF：p167 EXTRA memset×1（N78 内联清零）；余 N82 槽位/STL 展开。
#include "GlobalData/QueryConnInfo.inc"
                                 DBInfoMap& outMap)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)0,
                                                  (ENUM_SERVER_GROUP)0);
    if (!db->set_query("seLect db_name,db_ip,db_port,db_userid,db_passwd from "
                       "db_connect where db_server_group = %d and db_type = %d",
                       group, idx))
        return false;
    // ORIG 0x080ecb9d/0x080ecbd1：两条件各自 `xor $1; test; je <继续>; mov $0;
    // jmp <end>` 独立早退（P133 独立守卫，禁合并短路 `||`）。
    if (!db->exec(true))
        return false;
    if (!db->fetch())
        return false;
    if (db->get_n_rows() != 1)
        return false;

    // [DualMisc] ORIG 0x080eca6c：列索引是递增变量（`mov -0x1c,%eax; addl $1,-0x1c`
    // ×5，i 声明在 info 之前），非字面常量 ⇒ 改 i++。
    // [H7PartyGlobal 2026-09-11] P331 声明序旋钮负结论：把「写后未读」的 group 副本
    // （ORIG 0x080ecc9b 落最深槽 -0x64）前移到 i 之前 ⇒ 4.4 反把本件由 NEAR 打成 DIFF
    // （TU：near 11→10、diff 37→38），已回滚。⇒ 槽向旋钮在此无效，维持 N82/N126 封顶。
    int i = 0;
    STDBConnInfo info;
    if (!db->get_str(i++, info.m_db, 0x1f))
        return false;
    if (!db->get_str(i++, info.m_host, 0x11))
        return false;
    if (!db->get_int(i++, info.m_port))
        return false;
    if (!db->get_str(i++, info.m_user, 0x15))
        return false;
    // ORIG 0x080ecc23..0x080ecc34：此处对 passCipher 做常量内联 rep stos
    // （0xf dword = 0x3c 字节）⇒ 源形是**在此处**声明 `char passCipher[0x3c] = {0};`
    // （P214③/P328⑨）；旧写 `memset(info.m_pass,0,60)` 既错目标（ORIG 清的是
    // 密文缓冲）又退化成 call（N78）。
    char passCipher[0x3c] = {0};
    if (!db->get_str(i++, passCipher, 0x3c))
        return false;
    // ORIG 0x080ecc6e..0x080ecc85：第二实参 = `lea -0x1c8(%ebp); add $0x2d`
    // = &info.m_pass（解密结果**回写结构**），非独立栈缓冲（旧写 passPlain[0x15]
    // 是实参来源错 + 多一个 0x15 数组致槽漂）。
    if (!G_CEnvironment()->DecryptValue(passCipher, info.m_pass))
        return false;
    // ORIG 0x80eccb8：rep movsd 0x5a dword（0x168 字节）拷入 outMap[idx]。
    // ORIG 0x080ecc98..0x080ecca4：键取形参值的具名副本（-0x24）后再取址，
    // 另有 group 的副本（-0x64，写后未读 ⇒ P171 照抄）。
    ENUM_SERVER_GROUP groupCopy = group;
    ENUM_DB_HANDLE_IDX idxCopy = idx;
    // [G7-sub] 结论（N82/N126 封顶）：ORIG 169 / 我方 169；p167 仅 STDBConnInfo
    //   C1↔C2 别名。ORIG 帧 0x1cc / 我方 0x1dc(+0x10)，标量槽逐项同构但位置差：
    //   ORIG i(-0x1c)/db(-0x20)/键副本(-0x24)/passCipher(-0x60)/group 副本(-0x64)
    //   vs 我方 i(-0x20)/db(-0x24)/键(-0x28)/passCipher(-0x64)/group 副本(-0x1c)。
    //   ORIG 把「写后未读」的 group 副本放紧邻 passCipher 的深处
    //   （0x080ecc9b `mov %eax,-0x64(%ebp)`），4.4 恒放最浅槽；键副本取址在
    //   0x080ecca4 `lea -0x24(%ebp),%eax`。块级标量槽向跨版本不可控（N93），
    //   且该未读副本属 P171 必须照抄 ⇒ 封顶（不删具名、不试声明序）。
    outMap[idxCopy] = info;
    return true;
}
// sync_script::CSyncScript 权威定义见 sync_script.h（SetEnable/IsEnable 内联 +
// GetInstanceSyncScript 单例访问器）；本 TU 不再提供本地镜像。

// ---- 事件/GM/统计 ----
// CEventManager 构造/析构已移入 CEventManager.cpp（独立 TU）。
#include "GlobalData/CGM_Manager.inc"
{
    // ORIG 0x082990d0：map 构造（成员自动）+ rwLock 构造 + 存 DB 句柄(6) +
    // LoadGmListFromDb()。

    m_db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)6,
                                             (ENUM_SERVER_GROUP)0);
    LoadGmListFromDb();
}
#include "GlobalData/dtor_CGM_Manager.inc"
{
    // ORIG 0x08299159 附近：rwLock 析构 + map 自动析构。

}
// b28c-l23 SIGFIX：返回类型 char→bool（ORIG __PRETTY_FUNCTION__ 串 + 体尾
// `movzbl -0x11(%ebp),%ebx` 直返 bool 局部；char 声明使 4.4 出 movsbl）。
// [b28d-solo-GlobalData] 已修 AE（p167 17/17 CLEAR）：ORIG 0x08299530 三处
// cMyTrace 各占独立槽（`lea -0x44/-0x34/-0x24` 三组互异，4.1 不复用），具名
// 对象在 4.4 下复用同槽致全帧浅 0x30（-0x48/-0x4c vs -0x18/-0x1c）⇒ 按 P221
// 改「构造即调用」全表达式临时链 `cMyTrace(__PRETTY_FUNCTION__,line,0)(fmt)`，
// 三处独立槽与 ORIG 逐条对齐（余重定位显示伪差，relink 后 AE）。
#include "GlobalData/LoadGmListFromDb.inc"
{
    // [b23S3C] ORIG 0x08299530：ok 即返回变量（exec/get_int 逐次覆写），
    // mId/level/rows 函数级预清（P197③ 帧头 movl $0×N），writeGuard 保护填充。
    // ORIG 帧头 `movb $0x0,-0x11(%ebp)` ⇒ bool 预清两句式（P198①）。
    bool ok = false;
    int rows = 0;
    unsigned int mId = 0;
    int level = 0;
    m_db->set_query("seLect m_id, level from gm_manifest");
    ok = m_db->exec(true);
    if (!ok)
    {
        // [b28d-solo-GlobalData] P221 全表达式临时链：ORIG 三处 trace 各占
        // 独立槽（-0x44/-0x34/-0x24），具名对象在 4.4 下复用同槽致帧浅 0x30。
        cMyTrace(__PRETTY_FUNCTION__, 0x1579, 0)(
            "Failed to get GM list from DB.");
        return false;
    }
    writeGuard<rwLock> guard(&m_lock);
    rows = m_db->get_n_rows();
    for (int i = 0; i < rows; ++i)
    {
        m_db->fetch();
        ok = m_db->get_int(0, mId);
        if (!ok)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1588, 0)(
                "Failed to get GM's m_id from DB.");
            // [b28d-solo-GlobalData] ORIG 0x0829962x 该分支为 `mov $0x0,%ebx;
            // jmp <尾>`（直接物化返回寄存器）⇒ 原码是 `return false;` 而非
            // ok=false;break（后者多写 -0x11 槽且尾部经 movzbl 重载）。
            return false;
        }
        ok = m_db->get_int(1, level);
        if (!ok)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x158e, 0)(
                "Failed to get GM's lv from DB.");
            return false;
        }
        m_gmMap[mId].m_level = level;
    }
    return ok;
}
// ORIG T 0x0860ecc6 IncreaseLevelDungeonPlay(level, playCount)：
// level ∉ [1,200] → 仅 trace(type=0)；否则 find 命中 → 引用缓存后
// m_playCount += playCount、++m_userCount；未命中 → 先写 playCount 再置
// userCount=1，insert。
// [b28d-18 NEAR] 修：删发明符号 stDungeonPlayInfo::C1Ev（ORIG nm 无，POD 化）；p167 CLEAR；余 N80 | ORIG 0x0860ecc6
#include "GlobalData/IncreaseLevelDungeonPlay.inc"
{
    if (level < 1 || level > 200)
    {
        // ORIG 0x0860ecd2：`mov 0xc(%ebp),%ebx` 发生在 cMyTrace ctor 实参装填
        // 之前（level 跨 ctor call 保活 ⇒ 4.1 直落 callee-saved %ebx，
        // 并连带 push/pop %ebx 与帧 0x64）。4.4 默认在 operator() 处重新
        // reload ⇒ register asm("ebx") + 声明位置钉住早装载（技巧 2/13/28）。
        // ORIG 0x0860ecd2：`mov 0xc(%ebp),%ebx` 在 cMyTrace ctor 实参装填之前
        // （level 跨 ctor call 落 callee-saved %ebx）。P227 全表达式临时链
        // `cMyTrace(...)("fmt", expr)` 使 4.4 在 ctor 前求值 op() 实参并以
        // %ebx 保活（具名局部/裸内联两形均劣）。
        cMyTrace(__PRETTY_FUNCTION__, 0x550, 0)(
            "LEVEL_DUNGEON_PLAY_STATISTIC wrong level(%d)", level);
        return;
    }
    // ORIG 0x0860ed0x：find 两实参装载序（-0x44 先入 0x4(%esp)、-0x30 后入
    // (%esp)）；P50 先声明后赋值实测反劣（4→11，多默认构造 call+拷贝）⇒ 回退。
    std::map<int, stDungeonPlayInfo>::iterator it = m_levelPlayMap.find(level);
    // [b28d-solo-GlobalData] P74 迭代器实参序旋钮：ORIG 0x0860ed51 先 `lea -0x44(it)`
    // 入 0x4(%esp)、`lea -0x30(end)` 入 (%esp) ⇒ 原码是 `end() == it`（第一实参 end）。
    if (m_levelPlayMap.end() == it)
    {
        stDungeonPlayInfo info;
        info.m_playCount = playCount;
        info.m_userCount = 1;
        m_levelPlayMap.insert(std::make_pair(level, info));
    }
    else
    {
        stDungeonPlayInfo& info = it->second;
        info.m_playCount += playCount;
        info.m_userCount += 1;
    }
}

// ---- 后续批次业务模块 ----
#include "GlobalData/CBreakAwayPreventSystem.inc"
{
    // ORIG 0x0831c3e0：SIG 基类构造；map 构造；5×STRewardDataKey 构造；
    // memset(+0x30b, 0, 0x1f)；map.clear()。基类/Key 内容未还原（推断：
    // 以清零等价，容器真实构造）。
    // ORIG 0x0831c3e0：m_keys 仅构造 4 项（mov $0x4 循环 STRewardDataKeyC1，
    // 每项 3 字节）；第 5 槽（+0x308..+0x30a）保持垃圾，不清零。
    // 基类 SIG ctor / m_keys[5] 循环构造由编译器自动生成
    // （ORIG 0x0831c3e8 call SIG C1、0x0831c412 4-计数 key 循环）。
    memset(m_tail, 0, sizeof(m_tail));
    m_rewardMap.clear();
}

// ORIG T 0x0831c486 [0x0831c486, 0x0831c49d]：仅析构 +0x2e4 的
// m_rewardMap（std::map<STRewardDataKey,STRewardDataList>），成员自动析构等价。
#include "GlobalData/dtor_CBreakAwayPreventSystem.inc"
{
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
    SIG_BREAK_AWAY_PREVENT_SYSTEM_MANAGER* data)
{
    // [G7-gd] 语义修正（ORIG 0x0831c4a6 `movl $0x0,-0x10(%ebp)`）：count 是
    //   **去重后的插入计数**，声明在 memcpy 之前并预清 0；仅在新键插入路径
    //   （else 分支）写 m_keys[count] 三字节并 `++count`
    //   （ORIG 0x0831c6f2..0831c743：`mov -0x10,%edx … add $0x2f0,%eax;
    //   mov %cl,0xe/0xc/0xd(%eax); addl $0x1,-0x10`）。旧写把 m_keys[i] 三行放在
    //   if/else **之外**且下标用 i ⇒ 重复键时写入位置与 ORIG 不同（真 bug）。
    int count = 0;
    memcpy(this, data, 0x2e4);   // 基类子对象区（原 m_base）
    m_rewardMap.clear();

    // [G7-gd] ORIG 0x0831c773 循环条件每次都从 **this** 重读计数
    //   （`mov 0x8(%ebp),%eax; mov 0x2e0(%eax),%eax; cmp -0xc(%ebp),%eax; setg`），
    //   无 `count = data->m_field2e0` 的具名副本；memcpy 已把 data 拷入 this，
    //   语义等价（基址 this 与 ORIG 一致）。
    for (int i = 0;
         i < ((const SIG_BREAK_AWAY_PREVENT_SYSTEM_MANAGER*)this)->m_field2e0;
         ++i)
    {
        // 保留裸形式说明（本函数当前 identical）：ORIG 0831c50b..0831c5c9 就是
        // `add %i*0x18 基址; add $0x10; movzbl 0x1(%eax)/0x2(%eax)/(%eax)` 与
        // `lea; add $0x14/$0x18/$0x1c/$0x20/$0x24; mov (%eax)` 两步形态
        // （break_away_prevent__CBreakAwayPreventSystem/loadDbData.md）。等价的
        // 具名成员 data->m_rewards[i].m_field1 等（SIG_BREAK_AWAY_PREVENT_REWARD_MANAGER，
        // 基址 +0x10/stride 0x18 一致）在 gcc4.4 O0 会把 +0x10 折进单 disp，破坏
        // identical，故按 P30/P160 口径保留原转写形态（表寻址两步精确复现）。
        STRewardDataKey key;
        // [b28d-tu-GlobalData] P334 第二形修正：ORIG 0831c500/0831c50b 循环体首
        //   连发两个默认构造 `lea -0x37(key); call` + `lea -0x4c(reward); call`，
        //   之后才逐字段装载；我方原把 `STRewardData reward;` 声明写在 key 赋值
        //   之后 ⇒ reward ctor 被挪到第 3 个 key 读之后（构造站点错位）。
        //   声明上移（POD，无 ctor 提前副作用 ⇒ 仅移动 call 位次）。
        STRewardData reward;
        // [b28d-tu-GlobalData] 双判据真缺口修正（40 条指令 gap 的主体）：ORIG
        //   **每个字段访问都从形参 0x8(%ebp)+槽 -0xc(i) 完整重物化**
        //   （`mov -0xc,%edx; mov 0x8(%ebp),%ecx; 3x; shl $3; lea; add $0x10..;`
        //   10 行/字段，0831c4e3/50b/549/563/57d/597/5b1 七段 missing），我方
        //   原 `entry` 缓存 ⇒ 每字段只 4 行 ⇒ 少 40 条。删缓存、逐字段重算
        //   （P48 无引用缓存族），0x11/0x12/0x10 序 = ORIG 字节装载序。
        // [G7-gd] 基址改 this（ORIG 0x0831c4fc/518/534/…/5b7 全部
        //   `mov 0x8(%ebp),%ecx`）：memcpy(this,data,0x2e4) 之后两级源
        //   （this+… 与 data+…）内容相同，但 ORIG 用 this ⇒ 与 ORIG 同形。
        // [G7-gd] 三字节读改「元素指针 + 具名成员」形（P30 两步寻址）：ORIG
        //   0x0831c508..0x0831c54b 为 `lea (%ecx,%eax,1); add $0x10; movzbl
        //   0x1/0x2/0x0(%eax)`——0x10 属**指针算术**、字段偏移走 disp；
        //   `*((char*)this + i*0x18 + 0x11)` 会被 4.4 折成单 `add $0x11`
        //   （探针 fA 实证）⇒ 改视图形后 4.4 与 ORIG 同出 add $0x10 + disp。
        //   （int 字段保持裸形：ORIG 是 `add $0x14`+`mov (%eax)`，裸形已同。）
        key.m_data[0] = ((const SIG_BREAK_AWAY_PREVENT_REWARD_MANAGER*)
                         ((const char*)this + i * 0x18 + 0x10))->m_field1;
        key.m_data[1] = ((const SIG_BREAK_AWAY_PREVENT_REWARD_MANAGER*)
                         ((const char*)this + i * 0x18 + 0x10))->m_field2;
        key.m_data[2] = ((const SIG_BREAK_AWAY_PREVENT_REWARD_MANAGER*)
                         ((const char*)this + i * 0x18 + 0x10))->m_field0;

        reward.m_field0  = *(int*)((const char*)this + i * 0x18 + 0x14);
        reward.m_field4  = *(int*)((const char*)this + i * 0x18 + 0x18);
        reward.m_field8  = *(int*)((const char*)this + i * 0x18 + 0x1c);
        reward.m_fieldc  = *(int*)((const char*)this + i * 0x18 + 0x20);
        reward.m_field10 = *(int*)((const char*)this + i * 0x18 + 0x24);

        std::map<STRewardDataKey, STRewardDataList>::iterator it =
            m_rewardMap.find(key);
        if (it != m_rewardMap.end())
        {
            it->second.m_list.push_back(reward);
        }
        else
        {
            STRewardDataList list;
            list.m_list.push_back(reward);
            m_rewardMap.insert(std::make_pair(key, list));
            // [G7-gd] ORIG 0x0831c6f2..0831c743：写序 [2]→[0]→[1]，
            //   下标 = count（本分支专用计数），写完 `++count`。
            m_keys[count].m_data[2] = key.m_data[2];
            m_keys[count].m_data[0] = key.m_data[0];
            m_keys[count].m_data[1] = key.m_data[1];
            ++count;
        }
    }
    // [G7-sub] 收口（本件唯一真语义缺口已修，余为常量拆分伪影）：
    //   改后 ORIG 243 / 我方 243、p167 仅 STRewardDataKey/STRewardData/
    //   STRewardDataList/pair 的 C1↔C2 / D1↔D2 别名（0 真缺口）。
    //   三字节读已回 ORIG 形（0x0831c508 `lea (%ecx,%eax,1); add $0x10;
    //   movzbl 0x1(%eax)`）；m_keys 写 ORIG 0x0831c705
    //   `add $0x2f0,%eax; mov %cl,0xe(%eax)` 与我方 `add $0x2f2,%eax;
    //   mov %cl,0xc(%eax)` 同址 0x2fe、仅大常量拆分不同（P30/N94 后端选择）；
    //   memcpy 实参装载序属 P23/N76 族 ⇒ 封顶。
}
// （b28 试 P216 throw() 回滚：4.4 判 ctor pure 后连 call 一起消除，ORIG 有 call，
//  得不偿失；CExpertJobMgr::C1 残差为 N77/P90 value-init 清零段。）
// [b28d-solo-GlobalData] P299/P318：CExpertJobFactory 用户空 ctor 声明+定义已删
// （ORIG 0x0849f68c 为 W 隐式合成）⇒ CExpertJobMgr::C1 恢复 value-init 前置 rep stos。
#include "GlobalData/dtor_CExpertJobFactory.inc"
{
    // ORIG 0x0849f6d2：两个 StaticPool 成员析构。
}
// b28c-1 N77: 4.1 new T() value-init 前置 rep stos 0x18 dword，4.4 不 emit（去具名 p 后 15→13）。
// [b28d-18 N77] ORIG new(nothrow) T() value-init 前置 rep stos 0x18 dword + 临时指针槽；4.4 按 CWG434 不 emit（N77/P90） | ORIG 0x0849e102
#include "GlobalData/CExpertJobMgr.inc"
{
    // ORIG 0x0849e102：首条 `movl $0x0,(%eax)` 预清槽 + new(nothrow) T()
    //（4.1 value-init 前置 rep stos 清零，4.4 按 CWG434 不 emit = N77/P90
    //  跨版本记录）+ 结果统一写回 + 成员回读判 0 后 printf。
    m_factory = 0;
    // b28c-1：ORIG 0x0849e102 尾 `mov %ebx,%eax; mov 0x8(%ebp),%edx;
    //  mov %eax,(%edx)` 直存 ⇒ 无具名中间指针（去 -0x1c 中转）。
    m_factory = new (std::nothrow) CExpertJobFactory();
    if (m_factory == 0)
    {
        printf("memory error func(%s) line(%d)",
               __PRETTY_FUNCTION__, 0x57f);
    }
}
#include "GlobalData/dtor_CExpertJobMgr.inc"
{
    // ORIG 0x0849e1ac：外层 guard + delete 自身 guard（两次非空检查），无清零。
    if (m_factory != 0)
        delete m_factory;
}
// CVillageObjectMgr ctor/dtor 现由 CVillageObjectMgr.cpp 提供
//（ORIG 0x86b6f30 默认构造 std::map；dtor 0x86b6f44 调用 map dtor）。
// CVillageMonsterMgr ctor/dtor 现由 CVillageMonsterMgr.cpp 提供（LinkVillageSolo）。
#include "GlobalData/CRevengeDungeon.inc"
{
    // ORIG 0x82aa6a4：+0 字节清零 + +0x04 int 清零。
    m_isOpen = false;
    m_field04 = 0;
}
#include "GlobalData/IsOpenRevengeDungeon.inc"
{
    return m_isOpen;
}
#include "GlobalData/GetCloseTime.inc"
{
    return m_field04;
}
#include "GlobalData/OpenRevengeDungeon.inc"
{
    // movb $0x1,(%eax)：置 +0 字节为 1。
    m_isOpen = true;
}
#include "GlobalData/CloseRevengeDungeon.inc"
{
    // movb $0x0,(%eax)：置 +0 字节为 0。
    m_isOpen = false;
}
#include "GlobalData/SetCloseTime.inc"
{
    // mov %edx,0x4(%eax)：arg 写 +0x04 int。
    m_field04 = sec;
}
#include "GlobalData/InstanceRentalSystem.inc"
    : m_impl(new RentalSystemImpl)
{
    // ORIG 0x08270014：new RentalSystemImpl(0x18) 交 auto_ptr；注册 PcRoom(1)/Dimension(2)。
    registerRentalMachine(1, new RentalMachinePcRoom);
    registerRentalMachine(2, new RentalMachineDimension);
}
// [b28d-18 IDENTICAL_AE] 68=68；唯一残差=尾对齐 nop（§6.1 豁免） | ORIG 0x08270110
#include "GlobalData/dtor_InstanceRentalSystem.inc"
{
    // ORIG 0x08270110：遍历机器表虚析构；impl 由 auto_ptr D1 释放。
    // ORIG 0x08270162：每轮 delete 后 `it->second = 0`（de 重调 + movl $0）。
    for (std::map<int, IRentalMachine*>::iterator it = m_impl->m_machines.begin();
         it != m_impl->m_machines.end(); ++it)
    {
        // [b28d-solo-GlobalData] P167：ORIG 0x0827016x 发 `_ZNK...deEv`（operator*）
        // 非 `ptEv` ⇒ 源码写 `(*it).second`（it->second 选错实例化，p167 MISSING deEv）。
        delete (*it).second;
        (*it).second = 0;
    }
}
#include "GlobalData/registerRentalMachine.inc"
{
    m_impl->m_machines[idx] = m;
}

#include "GlobalData/RentalMachinePcRoom.inc"
{
    // ORIG 0x82703c0：IRentalMachine 基类 + vptr；new Impl(0x11c) 存 auto_ptr@+4；
    // initLoggingEnvironment()。
    initLoggingEnvironment();
}
#include "GlobalData/initLoggingEnvironment.inc"
{
    // ORIG 0x8270886：snprintf(impl, 0x103, "./log/%s/dw2_RentalItems", 文件名)。
    OS_API::snprintf(m_impl->m_data, 0x103, "./log/%s/dw2_RentalItems",
                     G_CEnvironment()->get_file_name());
}

// [b28d-18 IDENTICAL_AE] 21=21 逐条同形 | ORIG 0x0826efdc
#include "GlobalData/initLoggingEnvironment_1.inc"
{
    // ORIG 0x826efdc：同 PcRoom 日志路径（.rodata 0x8bf90c2 同串）。
    OS_API::snprintf(m_impl->m_data, 0x103, "./log/%s/dw2_RentalItems",
                     G_CEnvironment()->get_file_name());
}

// ---- IRentalMachine 槽实现（PcRoom / Dimension 同构，逐报告转写）----

// PcRoom::Ready（ORIG 0x0827073e）：尾调 checkAllRentalItemScript()。
#include "GlobalData/Ready.inc"
{
    return checkAllRentalItemScript();
}
// Dimension::Ready（ORIG 0x0826ee94）：同构。
#include "GlobalData/Ready_1.inc"
{
    return checkAllRentalItemScript();
}

// PcRoom::CheckRentableItem（ORIG 0x08270752）：
// return m_impl->m_set.find(itemIdx) != m_impl->m_set.end()。
#include "GlobalData/CheckRentableItem.inc"
{
    // ORIG 0x08270752：auto_ptr 解引用后 m_set.find(itemIdx) != end()。
    // ORIG 0x082707b2：ne 的 this = end() 临时槽(-0xc)、实参 = it 槽
    // (-0x10) ⇒ 源码序 `end() != it`（Dimension 同构已验证）。
    std::set<unsigned int>::const_iterator it = m_impl->m_set.find(itemIdx);
    return m_impl->m_set.end() != it;
}
// Dimension::CheckRentableItem（ORIG 0x0826eea8）：同构。
// [b28d-18 IDENTICAL_AE] 32=32；唯一残差=尾对齐 nop（§6.1 豁免） | ORIG 0x0826eea8
#include "GlobalData/CheckRentableItem_1.inc"
{
    // ORIG 0x0826ef01：const_iterator::ne 的 this = end() 临时槽(-0xc)、
    // 实参 = it 槽(-0x10)——源码序 `end() != it` 复现。
    std::set<unsigned int>::const_iterator it = m_impl->m_set.find(itemIdx);
    return m_impl->m_set.end() != it;
}

// ---- InstanceRentalSystem 查询/分发（ORIG 报告 InstanceRentalSystem/*.md）----

// getMachine（ORIG T 0x08270348）：map find，未命中返回 0。
// [b28d-18 IDENTICAL_AE] 38=38 逐条同形（.o 重定位伪差） | ORIG 0x08270348
#include "GlobalData/getMachine.inc"
{
    // ORIG 0x082703a0：`_Rb_tree_iterator::eq` + `test; je` 到 de 路径、
    // true 分支 `mov $0` 早返岛——源码 `if (it == end) return 0;` + 尾
    // `return it->second`（非三目 ne 形）。
    // ORIG find/eq 为非 const iterator 符号（4findERS5_/...eqERKS5_）——
    // const_iterator 声明会引入 iterator→const_iterator 转换调用。
    std::map<int, IRentalMachine*>::iterator it = m_impl->m_machines.find(idx);
    // ORIG 0x0827039a：eq 的 this = end() 临时槽(-0xc)、实参 = it(-0x10)——
    // 源码写 `end() == it`。
    if (m_impl->m_machines.end() == it)
    {
        return 0;
    }
    // [b28d-solo-GlobalData] P167：ORIG 发 deEv ⇒ `(*it).second`。
    return (*it).second;
}

// Ready（ORIG T 0x082701dc）：遍历机器表；任一为空或其槽3 Ready() 返回非 1
// 即返回 false，全部就绪（含空表）返回 true。
// [b28 判词 N7] 残差=call *%edx↔%eax 换名+对调，主体 56 行同形封顶。
// [b28d-18 IDENTICAL_AE] 56=56 逐条同形；残差仅间接 call 寄存器名（N7 已判） | ORIG 0x082701dc
#include "GlobalData/Ready_2.inc"
{
    for (std::map<int, IRentalMachine*>::iterator it = m_impl->m_machines.begin();
         it != m_impl->m_machines.end(); ++it)
    {
        // ORIG 0x08270214：判空条件极性 `jne body` + fall-through `mov $0;jmp
        // end`（技巧 17：if(x==0) return false; 直落形）；`!Ready()` 独立 if
        // （ORIG 0x08270233 xor+test+je 续循环，非 || 短路合并形）。
        // [b28d-solo-GlobalData] P167：ORIG 发 deEv ⇒ `(*it).second`。
        IRentalMachine* machine = (*it).second;
        if (machine == 0)
        {
            return false;
        }
        if (!machine->Ready())
        {
            return false;
        }
    }
    return true;
}

// Rent 分发（ORIG T 0x08270288）：按 RentInput.m_machineKey 取机器并经槽2
// 调用；无机器时 ORIG 直接返回 1。
// [b28 判词 N7] 残差=call *%edx↔%eax 换名+实参链寄存对调，29 行主体同形封顶。
#include "GlobalData/Rent.inc"
                               RentOutput& output)
{
    IRentalMachine* machine = getMachine(input.m_machineKey);
    if (machine == 0)
    {
        return true;
    }
    return machine->Rent(user, input, output);
}

#include "GlobalData/CDailyScheduleManager.inc"
#include "GlobalData/dtor_CDailyScheduleManager.inc"
// [b28d-solo-GlobalData] P299/P318：CTeamFactory 用户空 ctor 声明+定义已删
// （ORIG 0x08588e2e 为 W 隐式合成）⇒ COnlinePreliminaryTeamMgr::C1 恢复前置 rep stos。
#include "GlobalData/dtor_CTeamFactory.inc"
{
    // ORIG 0x08588e42：StaticPool 成员析构。
}
// [b28 判词 N77] ORIG 4.1 `new (nothrow) T()` value-init 前置 rep stos 12 dword
// 清零+临时槽；4.4 CWG434 不 emit——placement/显式 memset 变 call 均不可翻，封顶。
// [b28d-18 N77] ORIG new T() value-init 前置 rep stos + 临时指针槽；4.4 不 emit（N77/P90） | ORIG 0x0858860c
#include "GlobalData/COnlinePreliminaryTeamMgr.inc"
    : m_factory(0)
{
    // ORIG 0x0858860c：+0x00=0；map@+0x04 由编译器构造；new(0x30, nothrow)
    // CTeamFactory；失败 logFormat 后继续。
    // ORIG 0x0858860c 首两条 `mov 0x8(%ebp),%eax; movl $0x0,(%eax)` 先于 map
    // ctor —— m_factory 清零是 mem-init（声明序）而非 body 赋值。
    // N77 定论（DL7 复测）：ORIG 0x0858867e rep stos $0xc dword（0x30 全清）
    // ＝ `new T()` value-init；4.4 CWG434 对 user-ctor 类不 emit（加括号
    // 实测仍无 rep stos）⇒ 连同 0x10 帧差与 ebx/esi/edi 缓存差 N77+N82附 封顶。
    // [b28d-solo-GlobalData] P299/P318：用户空 ctor 已删，此处补括号 value-init
    // 即触发 4.4 前置 rep stos（ORIG 0x0858867e rep stos $0xc dword）。
    m_factory = new (std::nothrow) CTeamFactory();
    if (m_factory == 0)
    {
        LogManager::logFormat(1, "OnlinePreliminaryTeam.cpp",
                              __PRETTY_FUNCTION__,
                              0xd9, "memory error func(%s) line(%d)",
                              "online_preliminary::COnlinePreliminaryTeamMgr::COnlinePreliminaryTeamMgr()",
                              0xd9);
    }
}
// ORIG T 0x085886fe：m_factory 非空时 delete（CTeamFactory D1 + operator delete）；
// m_teamMap@+0x04 由编译器析构（ORIG 尾调 map D1）。
#include "GlobalData/dtor_COnlinePreliminaryTeamMgr.inc"
{
    // ORIG 0x08588706/0x0858870f：双 load 双判空——4.4 对
    // `if (m_factory) delete m_factory;` 恒产此形（probe 实证）。
    if (m_factory)
    {
        delete m_factory;
    }
}
// ORIG T 0x08588be4（OnLoadTeamInfo.md 全量转写）：data 首 4 字节为队伍 key，
// 命中 m_teamMap 时委托 loadDataPreliminaryTeamMatchList(data)。
#include "GlobalData/OnLoadTeamInfo.inc"
    CUser* user, char* data)
{
    // data = 队伍线格式缓冲（首 int 为 team key，ORIG 08588bed 直读 (%eax)）；
    // 该域包 struct 权威头未建模（布局未建模），P156 豁免保留裸读。
    // ORIG 0x08588bea：data 先落局部槽 -0xc(%ebp) 再回读传 find（4.1 实参
    // 地址表达式 spill 形）——具名局部拷贝复现。
    char* pTeam = data;
    std::map<int, COnlinePreliminaryTeam*>::iterator it =
        m_teamMap.find(*(int*)pTeam);
    if (it != m_teamMap.end())
    {
        it->second->loadDataPreliminaryTeamMatchList(data);
    }
}
// game_master::CCommandMap / CGameMasterMgr 真实实现见 GmCore.cpp 与 GmRegister.cpp
//（ORIG CCommandMap ctor 0x082a7f64、add(int) 0x082a7c76、add(string) 0x082a7cdc、
// Mgr ctor 注册链 0x082a8a28）。

// ---- IBusiness / CBusinessImpl（报告 docs/class_func_reports/CBusinessImpl/）----

// P210 附注（b28 BL14 翻案，Main 2026-09-08 广播）：派生 ctor 定义前置到同
// TU 基类 ctor 之前 ⇒ 4.4 对该 callee 不可见、恢复 ORIG 保守 outgoing
// `sub $0x18`（0x082a7c3b），旧「帧舍入 N + dead pad」判词作废（pad 删）。
#include "GlobalData/CBusinessImpl.inc"
{
    // ORIG 0x082a7c32：call IBusiness ctor + 设 vptr（编译器生成序列）。
}

#include "GlobalData/dtor_CBusinessImpl.inc"
{
    // ORIG 0x082a7c4e：仅重置 vptr（编译器生成序列）。
}

#include "GlobalData/IBusiness.inc"
{
    // ORIG W 0x082a7c24：仅设 vptr（编译器生成）。
}

// [b28d-solo-GlobalData] 改善 74→68（p167 12/12 CLEAR）：P259 去 stream 具名
// 局部（ORIG 0x080de3f6 `call Acquire; movl $1,0x8(%esp); mov %eax,0x4(%esp)`）+
// info 前 8 字节结构整体赋值（ORIG 0x080de46c `mov 0x4(%eax),%edx; mov (%eax),%eax`
// 先高字后低字）+ body.field04 1 字节（ORIG 0x080de469 `mov %al,-0x1c`，T1 直证）。
// 残差（已闭）：ORIG 尾部 `mov $0x0,%ebx … mov %ebx,%eax` ⇒ 原函数返回 int 0。
// [b28d-PoolGlobalData] R2 落地：CBusinessImpl.h:34/48 void→int + `return 0`
// ⇒ 0x080de3ee ndiff 19→0（AE）。
#include "GlobalData/PunishClient.inc"
                                tagAntibotPunishInfo* info, char* reason)
{
    (void)a;
    (void)b;
    (void)reason;
    // ORIG 0x080de3ee：StreamPool 取流；CStreamGuard(stream,true)；<<0x253 <<-1；
    // put_binary({uint c; EPUNISHMODE mode(byte); info 前 8 字节}, 0x10)。
    // [b28d-solo-GlobalData] P259：ORIG 0x080de3f6 无 stream 具名局部
    // （Acquire 结果经 %eax 直传 guard ctor 的 0x4(%esp)）⇒ 内联实参。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("BusinessImpl.cpp", 0x59),
                       true);
    **guard << 0x253;
    **guard << -1;
    // [b28d-solo-GlobalData] ORIG 0x080de46c：`mov 0x4(%eax),%edx; mov (%eax),%eax;
    // mov %eax,-0x18; mov %edx,-0x14` = info 前 8 字节**结构整体赋值**
    // （4.1 先读高字后读低字）；逐字赋值会反序且多具名指针局部。
    struct PunishTail
    {
        int field08;
        int field0c;
    };
    struct
    {
        unsigned int field00;
        // ORIG 0x080de469 `mov %al,-0x1c` ⇒ field04 为 1 字节（枚举形参按
        // 4 字节栈槽读入后截断存储）；声明 EPUNISHMODE（4 字节）会生 4 字节 store。
        unsigned char field04;
        PunishTail tail;
    } body;
    body.field00 = c;
    body.field04 = (unsigned char)mode;
    // tagAntibotPunishInfo 完整布局未建模（CBusinessImpl.h 仅前向声明，
    // 「antibot 域完整布局待专门批次」）；ORIG 080de3ee 取 info 前两 dword
    // 作 8 字节块赋值，此处以等价视图类型表达。
    body.tail = *reinterpret_cast<const PunishTail*>(info);
    guard->put_binary(&body, 0x10);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    // [b28d-PoolGlobalData R2] ORIG 0x080de3ee 尾 `mov $0,%ebx … ~CStreamGuard …
    // mov %ebx,%eax` ⇒ 返回 int 0 跨 guard 析构保活（头 void→int 已报 Main）。
    return 0;
}

#include "GlobalData/ProcessProtectMsg.inc"
{
    // ORIG 0x080de4ec：StreamPool 取流；<<0x254 <<-1；put_binary(pkg, 0x34)。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("BusinessImpl.cpp", 0x88),
                       true);
    **guard << 0x254;
    **guard << -1;
    guard->put_binary(pkg, 0x34);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 0;
}

// ---- DB 请求（ORIG 模式：Acquire 流 → CStreamGuard → <<msgId → <<-1 → 入 DB 队列）----
#include "GlobalData/makeRequest.inc"
{
    // ORIG 0x08432e80 同型（DBThread.cpp 0x7a8e）；本函数 msgId=0x108（报告 makeRequest.md）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7077), true);
    **guard << 0x108;
    **guard << -1;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

#include "GlobalData/makeRequest_1.inc"
{
    // msgId=0x109（报告 makeRequest.md）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x70e2), true);
    **guard << 0x109;
    **guard << -1;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

#include "GlobalData/makeRequest_2.inc"
{
    // ORIG 0x0842efa0：DBThread.cpp 0x73d8；msgId=0x127；第二字段 -1。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x73d8), true);
    **guard << 0x127;
    **guard << -1;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

#include "GlobalData/makeRequest_3.inc"
{
    // ORIG 0x08432e80：DBThread.cpp 0x7a8e；msgId=0x152；第二字段 -1。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7a8e), true);
    **guard << 0x152;
    **guard << -1;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ---- 杂项对象 ----
#include "GlobalData/Action.inc"
{
    // ORIG 0x0827f6d8：两个 map 构造（成员自动）+ init_function_type()。
    init_function_type();
}
#include "GlobalData/proc_bingo.inc"
{
    // ORIG 0x0827fb14：BingoEvent(0x6a) IsEventing(0) 检查后转发 reqBingoMark；
    // 未开启返回 0x13。
    // 0x0827fae6 区：GetRepeatEvent 结果 %eax 直作虚调用 this（`mov (%eax),%edx;
    // add $0x34,%edx; mov (%edx),%edx; mov %eax,(%esp); call *%edx`，无 spill
    // 槽 ⇒ 技巧 37 免槽直用）；判真伪为正向 `test %al,%al; je 0x13-岛`
    // （技巧 1：`!boolcall()` 会多 xor ⇒ 此处 ORIG 无 xor ⇒ 源码为正向 if）。
    // 0x0827fb14 区：bingoEvent 槽 -0x10、返回 temp 槽 -0xc（技巧 21 后声明占
    // 近槽）；`cmpl $0x0,-0x10; je 0x13-岛` ⇒ 内层也是正向 if（技巧 17：
    // fall-through 块=真路径在前，0x13 尾岛）。
    if (GlobalData::s_event_manager->GetRepeatEvent(0x6a)->IsEventing(0))
    {
        // BingoEvent 完整类型已建模（BingoEvent.h 权威头，OrigBingoEvt 批次）；
        // ORIG 0x0827fb4f 直调 _ZN10BingoEvent12reqBingoMarkERN15StackableAction11input_paramE。
        BingoEvent* bingoEvent = static_cast<BingoEvent*>(
            GlobalData::s_event_manager->GetRepeatEvent(0x6a));
        if (bingoEvent != 0)
        {
            int ret = bingoEvent->reqBingoMark(p);
            return ret;
        }
    }
    return 0x13;
}
// [b28d-18 NEAR] 修：getCurCharacFatigue 两次独立调用（P48 无 CSE，p167 MISSING 实证）；
// 调用差集 CLEAR 8/8；余槽位/寄存器（N82） | ORIG 0x0827fbd6
// [b28d-solo-GlobalData] NEAR 73=73（p167 8/8 CLEAR）：P0 具名 user 局部
// （ORIG -0x10 三次复用）+ P183 去 costs 指针缓存（每次重发 item+0x3a0）后
// 由 74 条收敛；余 1 处槽互换（ORIG lower -0x14 / max 常量临时 -0x1c vs
// 4.4 -0x1c/-0x14，P212 头部声明/前置声明三形实测均反劣）⇒ N98 封顶。
#include "GlobalData/proc_addFatigue.inc"
{
    // ORIG 0x0827fbd6：user/item 判空→0x13；item+0x3a0 的 vector<int> 取第 0 项
    // 与当前疲劳比较；疲劳不足返回 0x5f；否则扣减并 SendFatigue。
    if (p.m_user == 0)
    {
        return 0x13;
    }
    // [b28d-solo-GlobalData] P0：ORIG 0x0827fbf0 `mov 0x8(%eax),%eax;
    // mov %eax,-0x10(%ebp)` 在 m_item 判空前即写具名局部，后续三次调用
    // （getCurCharacFatigue×2/setCurCharacFatigue/SendFatigue）全部
    // `mov -0x10(%ebp),%eax` 复用该槽（无重读 p.m_user）。
    CUser* user = p.m_user;
    if (p.m_item == 0)
    {
        return 0x13;
    }
    // ORIG item+0x3a0 = CStackableItem::m_vec3a0（运行时 item 为 stackable）。
    // [b28d-solo-GlobalData] P183 去指针缓存：ORIG 0x0827fc0d/0x0827fc2e 每次
    // 重发 `mov 0xc(%ebp),%eax; mov 0xc(%eax),%eax; add $0x3a0,%eax`，具名
    // costs 局部会占 -0x10 槽并把后续 max 临时槽顶浅（-0x14 vs ORIG -0x1c）。
    if (static_cast<CStackableItem*>(p.m_item)->m_vec3a0.empty())
    {
        return 4;
    }
    int need = std::max(
        0, static_cast<CStackableItem*>(p.m_item)->m_vec3a0.at(0));
    // ORIG 0x0827fc6a/0x0827fc89：getCurCharacFatigue 两次独立调用
    // （O0 无 CSE，P48；原具名 cur 缓存少 1 次调用，p167 MISSING 实证）。
    if ((int)(user->getCurCharacFatigue() & 0xffff) < need)
    {
        return 0x5f;
    }
    // [b28d-solo-GlobalData] ORIG 0x0827fc97/0x0827fc9e：实参序
    // `lea -0x14(lower),%eax; mov %eax,(%esp)`（第一实参 lower）⇒
    // `std::max(lower, after)`。余 1 处槽互换（ORIG lower -0x14 / max 常量临时
    // -0x1c；4.4 按首次使用分配恒相反，P212 头部声明/前置声明三形实测均反劣）
    // ⇒ N98/N93 封顶。[b28d-PoolGlobalData P326 重试] 匿名 0 临时形实测：块1
    // 临时顶到 -0x18、块2 after 滑 -0x1c（4 条→8 条差）⇒ 反劣回退，N93 判不变。
    // [G7-sub] 本轮复核：ORIG 73 / 我方 73；p167 CALLSITE CLEAR（8 call 全中）。
    //   残差仅两处 std::max 临时槽互换：ORIG 0x0827fc6a 段常量临时 -0x1c 与我方
    //   -0x14 互置（0x0827fc97 段反之）⇒ 维持 N98/N93 封顶。
    int after = (int)(user->getCurCharacFatigue() & 0xffff) - need;
    int lower = 0;
    user->setCurCharacFatigue(std::max(lower, after));
    user->SendFatigue();
    return 0;
}
#include "GlobalData/proc_lotteryCount.inc"
{
    // ORIG 0x0827fb86：user/item 判空→0x13；否则 giveLotteryCountItem(
    // item->get_index(), false)，返回 0。
    // 两 guard 共享 0x13 尾块（|| 合并条件）。
    // 两个独立 if + 相同返回值 ⇒ gcc tail-merge 共享 0x13 尾块。
    // ORIG 0x0827fb86：正向嵌套（两 je 共享尾部 0x13 块），body 内 return 0
    // 走 jmp merge。
    if (p.m_user != 0)
    {
        if (p.m_item != 0)
        {
            p.m_user->giveLotteryCountItem(p.m_item->get_index(), false);
            return 0;
        }
    }
    return 0x13;
}

// [b28d-18 N82] 残差=槽位/STL 展开（N82/N89） | ORIG 0x0827f7c8
#include "GlobalData/init_function_type.inc"
{
    // ORIG 0x0827f7c8：以 boost::bind 将 proc_* 成员注册进 map2（key 0x34/0x36/0x37）。
    // 与 ORIG 一致用 insert 方式（非 operator[]）。
    // [b28d-solo-GlobalData] p167 EXTRA `boost::arg<1>::C1Ev`×3：本头集下
    // `boost::_1` 不可用（COMPILE FAILED），保留 `arg<1>()` 构造形 ⇒ N（绑定库
    // placeholder 构造跨版本差异）封顶。
    // [G7-sub] 本轮复核：ORIG 157 / 我方 169，gap 12 恰为 3×`call
    //   _ZN5boost3argILi1EEC1Ev`（p167 EXTRA，每处 lea+mov+call+sub=4 条）。
    //   ORIG 侧占位符是**未初始化字节槽直传**、无 call：
    //   0x0827f893 `movzbl -0xe9(%ebp),%eax; mov %al,0x10(%esp)`、
    //   0x0827f95c `movzbl -0xd9(%ebp),%ebx; mov %bl,0x10(%esp)`。
    //   本头集 `boost::_1` 编译失败（前轮实证）⇒ 维持 N 封顶。
    using boost::arg;
    m_map2.insert(std::make_pair(TYPE_BINGO,
        boost::bind(&Action::proc_bingo, this, arg<1>())));
    m_map2.insert(std::make_pair(TYPE_LOTTERY_COUNT,
        boost::bind(&Action::proc_lotteryCount, this, arg<1>())));
    m_map2.insert(std::make_pair(TYPE_ADD_FATIGUE,
        boost::bind(&Action::proc_addFatigue, this, arg<1>())));
}

// [b28d-18 IDENTICAL_AE] 修：register 指针 pt=&t 使 &t 提前物化 %esi 跨 operator new 保活（P231 新实证）；NDIFF 4→1（余 vtable 重定位伪差） | ORIG 0x084a2938
#include "GlobalData/CExtraction.inc"
{
    // ORIG 0x084a2938：vptr + new CMTRand 并以 time(0) seed。
    // 0x084a296c：CMTRand ctor 形参为 const unsigned long&（mangled ERKm，
    // 32 位 ulong=4B → time 结果单 dword 落槽 0x084a2956、lea 槽址作实参）。
    unsigned long t = (unsigned long)time(0);
    // ORIG 0x084a2959：seed 槽地址提前物化 %esi —— 4.4 对实参 &t 表达式
    // 自然产 lea（P0：寄存器仅承载表达式值）。
    register unsigned long* pt = &t;
    m_rand = new CMTRand(*pt);
}
#include "GlobalData/dtor_CExtraction.inc"
{
    // ORIG 0x084a2938：仅 delete m_rand（判空），不置零。
    delete m_rand;
}
// [b28d-solo-GlobalData] P299/P318：三个子类用户空 ctor 声明+定义已删（ORIG C1/C2 均 W）
// ⇒ CExtractionMgr::C1 的 `new T()` 恢复 value-init 前置双清零（ORIG 0x084a3047/0x084a308f）。
#include "GlobalData/dtor_CAlchemistExtraction.inc"
#include "GlobalData/dtor_CEnchanterExtraction.inc"
#include "GlobalData/dtor_CDollControllerExtraction.inc"
// [b28d-solo-GlobalData] 归零 75=75 IDENTICAL：P299/P318 删三个子类用户空 ctor
// （ORIG C1/C2 均 W）恢复 `new T()` value-init 前置双清零（旧 N77/N83 判词作废） | ORIG 0x084a302e
#include "GlobalData/CExtractionMgr.inc"
{
    // ORIG 0x084a302e：new 三个子提取器对象，依次存 +0/+4/+8。
    // ORIG 0x084a3047/0x084a308f：ctor 调用前先清 0x8 字节——value-init
    // `new T()`（zero-init 前置 + C1 调用）。
    m_slots[0] = new CAlchemistExtraction();
    m_slots[1] = new CEnchanterExtraction();
    m_slots[2] = new CDollControllerExtraction();
    // [b28d-solo-GlobalData] 4.4 现发 `movl $0x0,(%eax); movl $0x0,0x4(%eax)`
    // 前置双清零（与 ORIG 0x084a3047/0x084a308f 逐条同形）——关键是子类无用户
    // 提供 ctor（CWG434）；旧「4.4 折叠进 C1/N83 封顶」判词作废。
}
#include "GlobalData/dtor_CExtractionMgr.inc"
{
    // ORIG 0x84a311a：for i∈[0,2] if(m_slots[i]) delete m_slots[i];（不置空）。
    for (int i = 0; i < 3; ++i)
    {
        delete m_slots[i];   // 4.4 delete 自带 test/je 判空（P207）→ 与 ORIG 单判同形
    }
}
// ---- Teleport / NPC_Teleport / User_Teleport（报告 Teleport/、NPC_Teleport/、User_Teleport/）----

#include "GlobalData/STCheckRuleData.inc"
// ORIG W 0x0822d78e：仅清零 +0x00 的 int（m_field00），不触碰 m_tp 子对象。
#include "GlobalData/STTeleportData.inc"
{
    // ORIG 0x0822d794：movl $0x4,0x8(%esp) → memset 长度 4；this 实参最后
    // 装填（0x0822d7a4）⇒ 原码为 memset(this,0,4) 非 &m_field00（后者使 4.4
    // 提前缓存 this 指针）。
    memset(this, 0, 4);
}

#include "GlobalData/Teleport.inc"
{
    // ORIG 0x08283c30：设 vptr；stPartyTelePort::reset(this+4)；*(this+0x10)=0。
    m_tp.reset();
    m_endCoolTime = 0;
}
#include "GlobalData/dtor_Teleport.inc"
{
    // ORIG D1 0x082843b0：重置 vptr，无成员析构（POD）。
}
#include "GlobalData/check_rule.inc"
{
    // ORIG 0x08283d82：转发 check_rule(data.m_user)。
    return check_rule(data.m_user);
}
#include "GlobalData/set_party_teleport.inc"
{
    // ORIG 0x08283d98：拷贝 stPartyTelePort 前 10 字节（8 字节 + short）。
    // ORIG 形：this/tp 基址各载一次，字段全折叠 `(%edx)→0x4(%eax)` 等 ⇒
    // 结构整体赋值（POD 逐字段展开，10B 实拷不含尾 pad）。
    m_tp = tp;
}
#include "GlobalData/get_party_teleport.inc"
{
    // ORIG 0x08283db6：返回 this+4。
    return m_tp;
}
#include "GlobalData/set_end_cool_time.inc"
{
    // ORIG 0x08283dc2：endCoolTime=aura_option(2)；now=getCurSec；
    // endCoolTime = (now < endCoolTime ? endCoolTime - now : 0)。
    if (user != 0)
    {
        m_endCoolTime = user->get_aura_avatar_option_value(2);
        int now = GlobalData::s_systemTime_.getCurSec();
        // ORIG 0x08283dfc：end 侧作 lhs；两分支各自重载 this 显式 store。
        // ORIG 0x08283e12/0x08283e1e：then(8283e01)/else(8283e14) 两分支尾
        // 均有 jmp 到共享 epilogue(8283e21)；then 带 return，else 体展平到
        // 内层 if 之后 + 外层 if 块后尾置 return（技巧 7 尾岛）。
        if (m_endCoolTime > now)
        {
            m_endCoolTime = m_endCoolTime - now;
            return;
        }
        m_endCoolTime = 0;
    }
    return;
}
#include "GlobalData/get_end_cool_time.inc"
{
    // ORIG 0x08283e24：返回 this+0x10。
    return m_endCoolTime;
}
#include "GlobalData/update_next_end_cool_time.inc"
{
    // ORIG 0x08283e30：is_equip_aura_avatar('..', &remain) 取剩余；
    // set_aura_avatar_option_value(2, getCurSec()+remain)；UpdateAuraAvatarOption。
    if (user != 0)
    {
        int remain = 0;
        user->is_equip_aura_avatar(2, remain);
        int v = GlobalData::s_systemTime_.getCurSec() + remain;
        user->set_aura_avatar_option_value(2, v);
        // ORIG 0x08283e??：get_aura 结果 %eax 直落实参槽（无中间局部槽）。
        user->UpdateAuraAvatarOption(2, user->get_aura_avatar_option_value(2));
    }
    // ORIG 尾岛：末 call 后 jmp 到共享 epilogue（技巧 7 尾置 return）。
    return;
}
#include "GlobalData/send_teleport_effect.inc"
{
    // ORIG 0x08283eba：PacketGuard put_header(0,0x17b)/put_byte(type)/put_byte(1)
    // /put_short(GetUID)/finalize(true)；按 vill/area send_to_area。
    if (user != 0)
    {
        PacketGuard packet;
        packet.put_header(0, 0x17b);
        packet.put_byte((int)(char)type);
        packet.put_byte(1);
        packet.put_short(user->GetUID());
        packet.finalize(true);
        G_GameWorld()->send_to_area(
            (int)user->getCurCharacVill(), user->get_area(false), packet);
    }
    // ORIG send_teleport_effect 尾岛：call send_to_area 后有 jmp 到共享
    // epilogue；return 写在 if 块后（技巧 7 尾置 return 岛）。
    return;
}
#include "GlobalData/move_area.inc"
{
    // ORIG 0x08283fb8：effect(1) → GameWorld::move_area(...) → effect(2)+
    // update_next_end_cool_time。ORIG 经 stPartyTelePort getters 取 5 字段
    // （add this+4 → mov %eax,(%esp) → call *_get_teleport_*），故此处调 getter
    // 而非直读字段（P：getter 为 ORIG 真符号 CPartyTelePort.h 声明）。
    if (user == 0)
    {
        return false;
    }
    send_teleport_effect(user, 1);
    // ORIG xor $0x1,%eax; test %al,%al; je（直测返回寄存器，无槽，P198）：
    // 谓词整体内联进 if(!...)，禁提同名 bool 局部（反多 mov %al,-off; cmpb 两步）。
    if (!G_GameWorld()->move_area(
            user, (int)(unsigned char)m_tp.get_teleport_village(),
            (int)(unsigned char)m_tp.get_teleport_area_index(),
            // ORIG Ghidra sVar2/sVar3 = get_teleport_pos* 记作 short、以 (int)
            // 直传 ⇒ movswl/cwtl 有符号拓宽（而非 getter 的 unsigned short movzwl）。
            (int)(short)m_tp.get_teleport_posx(),
            (int)(short)m_tp.get_teleport_posy(),
            (int)(unsigned char)m_tp.get_teleport_direction(), true, 2, 0, 0))
    {
        return false;
    }
    send_teleport_effect(user, 2);
    update_next_end_cool_time(user);
    return true;
}
#include "GlobalData/NPC_Teleport.inc"
{
    // ORIG 0x082840da：call Teleport ctor + 设 vptr（编译器生成序列）。
}
#include "GlobalData/dtor_NPC_Teleport.inc"

// [b28d-18 N82] p167 CLEAR（11/11）；残差=槽位/寄存器分配 | ORIG 0x08283c5a
// [b28d-solo-GlobalData] 归零 AE 85=85（p167 11/11 CLEAR）：①各失败点 ORIG 为
// `mov $imm,%eax; jmp end` 直接物化 ⇒ 源码全 `return X;` 早返（旧 result 变量
// 汇聚形多 -0x1c 槽 + 4 条）；②pvp/expired 两处零槽短路物化用 P226 语句表达式
// `if (({ A || B; }))`（具名 bool 落槽 -0x16/-0xd）；③remain 声明在 if 外、
// 赋值在 if 体内（ORIG 0x08283cfb 在 isGMUser 之后）；④CUser.h is_equip_aura_avatar
// 返回 char→int（Main 代落，消调用点 movsbl）。
#include "GlobalData/check_rule_1.inc"
{
    // ORIG 0x08283c5a：状态/渠道/vill/aura 冷却校验链（报告 Teleport/check_rule.md）。
    // [b28d-solo-GlobalData] ①各失败点 ORIG 为 `mov $imm,%eax; jmp end` 直接
    // 物化 ⇒ 源码全 `return X;` 早返（Ghidra 的 iVar3 汇聚是假象，旧 result
    // 赋值形多 -0x1c 槽）；②pvp/expired 两处为 `mov $1/$0` 岛 + test 直测
    // （零槽，P226 语句表达式）——具名 bool 会落槽 -0x16/-0xd 并顶浅后续槽。
    if (user == 0)
    {
        return 1;
    }
    if (user->get_state() != 3)
    {
        return 0x13;
    }
    if (({ G_GameWorld()->IsPVPChannel() ||
           G_GameWorld()->IsPvPSkilTreeChannel(); }))
    {
        return 0x13;
    }
    if (user->getCurCharacVill() == 7)
    {
        return 0xbe;
    }
    // [b28d-solo-GlobalData] ORIG 0x08283cfb `movl $0,-0x14(%ebp)` 在
    // isGMUser 检查之后（Ghidra C 亦为 if 体内 local_18 = 0）⇒ 声明在 if 外、
    // 赋值在 if 体内（声明即初始化会提前到 0x08283ce9 之前 +1 条）。
    int remain;
    if (!user->isGMUser())
    {
        remain = 0;
        int equipped = (int)user->is_equip_aura_avatar(2, remain);
        if (equipped > 0)
        {
            return equipped;
        }
    }
    int auraEnd = user->get_aura_avatar_option_value(2);
    if (({ (!user->isGMUser()) &&
           (auraEnd > GlobalData::s_systemTime_.getCurSec()); }))
    {
        return 0x16;
    }
    return 0;
}


#include "GlobalData/check_rule_2.inc"
{
    // ORIG 0x082840f6：r = Teleport::check_rule(data)；r==0 → 返回 0。
    int r = Teleport::check_rule(data);
    if (r != 0)
    {
        return r;
    }
    return 0;
}
#include "GlobalData/set_teleport_data.inc"
{
    // ORIG 0x08284124：set_party_teleport(data.m_tp)。
    set_party_teleport(data.m_tp);   // STTeleportData::m_tp@+0x04（Teleport.h 权威成员）
}

#include "GlobalData/User_Teleport.inc"
{
    // ORIG 0x08284142：call Teleport ctor + 设 vptr（编译器生成序列）。
}
#include "GlobalData/dtor_User_Teleport.inc"
#include "GlobalData/check_rule_3.inc"
{
    // ORIG 0x0828415e（docs/class_func_reports/User_Teleport/check_rule.md）：
    // 基类 Teleport::check_rule 非 0 原样返回（result@-0x14 只服务于该值）；其后
    // 各分支均为 `mov $const,%eax; jmp 出口` ⇒ 源码是早退 return，不共用 result。
    // target@-0x10 先 `movl $0x0` 再赋值 ⇒ 显式 =0 初始化后单独赋值。
    // get_state / getCurCharacVill / GetAssaultPlace 返回值不入库（ORIG 直接在
    // %eax/%al 上 cmp ⇒ 条件表达式内直接调用）；三连村庄判定 ORIG 为三次独立
    // call getCurCharacVill（无缓存），逐条重调。
    int result = Teleport::check_rule(data);
    if (result != 0)
    {
        return result;
    }

    CUser* target = 0;
    target = G_GameWorld()->find_from_world((unsigned short)data.m_field00);
    if (target == 0)
    {
        return 0x15;
    }
    if ((bool)(target->get_state() != 3))
    {
        return 4;
    }
    if ((bool)(target->getCurCharacVill() == 7 ||
               target->getCurCharacVill() == 8 ||
               target->getCurCharacVill() == 10))
    {
        return 4;
    }
    // ORIG 0x08284250：checkSeriaRoom 返回值直接 `test %al,%al; je`（无 setne
    // 物化）⇒ 此处写纯 `if (char 表达式)`，不加 (bool) 强转。
    if (CHackLog_InvalidAreaMove::checkSeriaRoom(
            (int)target->getCurCharacVill(), target->get_area(false)))
    {
        return 4;
    }
    if (target->GetAssaultPlace() != 0)
    {
        return 0x9c;
    }

    CParty* party = static_cast<CParty*>(target->GetParty());
    if (party != 0)
    {
        if ((bool)(static_cast<CPartyTelePort*>(party->GetPartyTelePort())
                       ->get_teleport_state() != -1))
        {
            return 0x9e;
        }
    }
    return 0;
}
#include "GlobalData/set_teleport_data_1.inc"
{
    // ORIG 0x082842b8：find_from_world(data.field00)；命中则取 vill/area/pos/
    // direction 组装局部 stPartyTelePort 后 set_party_teleport。
    // ORIG 0x082842bf：target 槽先置 0（声明初始化），find 结果再赋值——
    // 两段式复现 `movl $0x0,-0xc(%ebp)`。
    CUser* target = 0;
    target = G_GameWorld()->find_from_world((unsigned short)data.m_field00);
    if (target != 0)
    {
        // ORIG 0x082842b8：局部 stPartyTelePort 不 reset（未设字段为栈垃圾），
        // 仅填 5 字段后 set_party_teleport——保持 ORIG 原样，不额外清零。
        // ORIG 0x0828433b：tp 基址 -0x16(%ebp)（4.1 紧排非 4 对齐）；4.4 恒
        // 4-对齐落 -0x18，槽偏移差 2 字节属布局伪影（N）。
        stPartyTelePort tp;
        tp.m_village = (unsigned char)target->getCurCharacVill();
        tp.m_areaIndex = (unsigned char)target->get_area(false);
        tp.m_posx = (short)target->get_posX();
        tp.m_posy = (short)target->get_posY();
        tp.m_direction = (unsigned char)target->get_direction();
        set_party_teleport(tp);
    }
    // ORIG 0x0828434a：body 尾 call 后 jmp 到 merge(nop@0828434c)——
    // 函数尾显式 return（技巧 7 尾岛）。
    return;
}
#include "GlobalData/GetRandomOption.inc"
{
    // ORIG T 0x085f0eb0：返回 &m_randomOption（CDataManager+0x4e10，objdump 实证）。
    return &G_CDataManager()->m_randomOption;
}
// [b28d-18 N7] 修：register seed 使 seed 先载（P231）；余纯寄存器名轮换（N7 封顶） | ORIG 0x085f0d64
// [b28d-solo-GlobalData] NEAR 封顶 N7：p167 逐调用同形（imul/lea/seed 段一致）；
// 残差=callee-saved 角色轮换（ORIG eax/edx/ecx vs 4.4 ebx/eax/edx，P230 定稿：
// 4.4.7 分配固定，源形不可择）+ 首条 vptr 存的重定位显示伪差（`$&<L>` vs `<L>`）。
#include "GlobalData/CRandomOptionItemHandle.inc"
{
    // ORIG 0x085f0d64：new[0x7f28] 13×CMTRand，逐元素 seed(time(0))；
    // 字段清零；+0x14 = GetRandomOption()+0x148 的指针。
    m_rand = new CMTRand[13];
    unsigned long t = (unsigned long)time(0);
    for (int i = 0; i < 13; ++i)
    {
        // [b28c-lane17] P0 清绑复测：ORIG 0x085f0d64 有 t 的**槽往返**
        // （`call time; mov %eax,-0x20` → 循环内 `mov -0x20(%ebp),%eax`），
        // 故具名局部 t 正确；但旧版在循环内另设 `unsigned long tv = t;`
        // 是**无槽依据的额外局部**（ORIG 无第二个 seed 槽）⇒ 反产 +2 条
        // store/load。直传 t 后指令数回到 ORIG 的 69。
        // 残余 = seed 实参与地址计算的**装载时刻序**：ORIG 4.1 在地址
        // imul/lea 之前把 t 载入 %eax（`mov -0x20,%eax` 先行），4.4 恒按
        // 「this→下标→实参」序装载 ⇒ P75/N86（实参装载时刻跨版本调度），
        // 语义等价（seed 实参同一槽值），保持可读源形。
        // [b28d-solo-GlobalData] P248 裸 register 实测：删 register 反生 seed 二段
        // 拷贝（69→71，多 mov -0x24→-0x1c），保留 register 回到 69=69；余纯寄存器
        // 名轮换（ORIG eax/edx/ecx vs 4.4 ebx/eax/edx）⇒ N7 封顶（P230：4.4.7 分配固定）。
        // [G7-sub] 本轮复核：ORIG 69 / 我方 69、帧 0x2c==0x2c；p167 仅
        //   `CMTRand` C1↔C2 别名。唯一残差是 seed 实参装载时刻：
        //   ORIG 0x085f0de2 `mov -0x20(%ebp),%eax` 值先载 %eax 再算地址；
        //   我方 0x…`mov -0x20(%ebp),%ebx` 落 callee-saved ⇒ P75/N96/N7 封顶。
        register unsigned long seed = t;
        m_rand[i].seed(seed);
    }
    m_field08 = 0;
    m_disabled = 0;
    m_field10 = 0;
    // ORIG 0x85f0e2d..0x85f0e3b：m_optionSet = GetRandomOption() + 0x148
    // （m_randomOption 内嵌 provider 对象基址 m_providerNormal148；虚槽0/1 见 RandomOptionScript.h）。
    m_optionSet =
        &reinterpret_cast<RandomOptionScript*>(GetRandomOption())->m_providerNormal148;
}
#include "GlobalData/dtor_CRandomOptionItemHandle.inc"
{
    // ORIG 0x085f0e46：vptr 前置写 + delete[]（guard/da），无成员清零。
    delete[] m_rand;
}

// [b28d-18 N] ORIG 切片缺失（objdump 直转 082a73cb）；p167 CLEAR（9/9）；残差=表内容占位+codegen | ORIG 0x082a73cb
// [b28d-solo-GlobalData] DIFF 封顶 N：ORIG 人证字符串表（.data 0x936e2a0，64 项）内容缺失，
// 重建以全 0 占位必须保留 s==0||*s==0 检查（否则 std::string(0) 崩溃），ORIG 无检查 ⇒ +10 条；
// p167 CLEAR（表访问调用同形）。
#include "GlobalData/init_1.inc"
{
    // ORIG 0x082a73cb：遍历 0..0x63 号人证字符串表（.data 0x936e2a0，64 项
    // char*），以字符串为键向 static map<string, vector<int>>
    // humanCertifyTable_（0x93f5758）插入下标。
    // 表指针原始值为 0x0814af00+2i（指向 .text，运行时不可用），重建工程无法
    // 还原内容，故以空串占位：循环结构/符号保持真实，表内容缺失（推断）。
    // [G7 语义修正·ORIG 全函数直证 0x082a73df..0x082a747c] ORIG 循环体仅
    // SaI ctor → SsC1EPKcRKSaIcE → map::operator[] → push_back → SsD1 → SaI D1，
    // **无任何判空/空串检查**；旧码 `if (s == 0 || *s == 0) continue;` 系发明防御
    // （多发 12 条 + 一具名 s 槽）。按 ORIG 删判空、去具名局部（P0/P183 实参内联）。
    static const char* const emptyStr[64] = {0};
    for (int i = 0; i <= 0x63; ++i)
    {
        humanCertifyTable_[std::string(emptyStr[i])].push_back(i);
    }
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
    : m_dbType((ENUM_SERVER_GROUP)0)
{
}

// ORIG W 0x082aa90e：vptr 复位 + CEnvironment::~CEnvironment()。
#include "GlobalData/dtor_CDBEnviroment.inc"
{
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
{
    const char* sep = "=";
    char* p1 = strtok(buf, sep);
    char* p2 = strtok(0, sep);
    // ORIG 0x0829d928/0x0829d933：trim 返回值被丢弃（`call trim` 后直接
    // `mov -0x48(%ebp),%eax` 复用 p1/p2 槽，无 `mov %eax,-0x48` 回写）⇒
    // 源形是语句调用 `trim(p1);`（trim 就地改缓冲，指针不变）。
    trim(p1);
    trim(p2);
    if (strcmp(p1, "game_db_type") == 0)
    {
        if (strcmp(p2, "cain") == 0)
            m_dbType = (ENUM_SERVER_GROUP)1;
        else if (strcmp(p2, "diregie") == 0)
            m_dbType = (ENUM_SERVER_GROUP)2;
        else if (strcmp(p2, "siroco") == 0)
            m_dbType = (ENUM_SERVER_GROUP)3;
        else if (strcmp(p2, "prey") == 0)
            m_dbType = (ENUM_SERVER_GROUP)4;
        else if (strcmp(p2, "casillas") == 0)
            m_dbType = (ENUM_SERVER_GROUP)5;
        else if (strcmp(p2, "hilder") == 0)
            m_dbType = (ENUM_SERVER_GROUP)6;
        else if (strcmp(p2, "ruke") == 0)
            m_dbType = (ENUM_SERVER_GROUP)7;
        else if (strcmp(p2, "seria") == 0)
            m_dbType = (ENUM_SERVER_GROUP)8;
        else if (strcmp(p2, "anton") == 0)
            m_dbType = (ENUM_SERVER_GROUP)9;
        else if (strcmp(p2, "roxy") == 0)
            m_dbType = (ENUM_SERVER_GROUP)0xa;
        else if (strcmp(p2, "bakal") == 0)
            m_dbType = (ENUM_SERVER_GROUP)0xb;
        else if (strcmp(p2, "ozma") == 0)
            m_dbType = (ENUM_SERVER_GROUP)0xc;
        else if (strcmp(p2, "khazan") == 0)
            m_dbType = (ENUM_SERVER_GROUP)0xd;
        else if (strcmp(p2, "first1") == 0)
            m_dbType = (ENUM_SERVER_GROUP)0x63;
        else if (strcmp(p2, "first2") == 0)
            m_dbType = (ENUM_SERVER_GROUP)0x64;
        else if (strcmp(p2, "starter_server") == 0)
            m_dbType = (ENUM_SERVER_GROUP)0x11;
        else
        {
            // ORIG 0x0829dbdd：p2 在 cMyTrace ctor 之前读入 %ebx 并跨 ctor 保活
            // ⇒ 源形为单语句全表达式临时链（P37/P227/P328⑩）。
            cMyTrace(__PRETTY_FUNCTION__, 0x2032, 5)("Unknown value (%s)", p2);
            return 0;
        }
        std::map<std::string, std::string> tmp;
        // ORIG 0x0829dc2f..0x0829dc4a：m_dbType 先值拷贝到具名局部（-0x34）再取址
        // 传 operator[](const ENUM_SERVER_GROUP&)（P0 槽往返 + ㉕实参来源判据）。
        // [L8 2026-09-10 判据⑩ 实验] 声明序对调（dbType 先于 tmp）后我方槽集合变为
        // {…-0x3c,-0x40,-0x48(map),-0x60,-0x64…}（map 由 -0x5c 移 -0x48），与 ORIG
        // {…-0x30,-0x34,-0x48,-0x4c,-0x64(map)…} 仍不匹配且 diff 未降 ⇒ 已回滚。
        // 结论：4.1 跨块槽分配与 4.4 的「声明序」旋钮方向不一致（P112 明文），判 N。
        ENUM_SERVER_GROUP dbType = m_dbType;
        ((CDBEnvServerGroupMapView*)G_CEnvironment())->m_map[dbType] = tmp;
    }
    else
    {
        // ORIG 0x0829dc9a/0x0829dcbb：find 的 receiver 与 end 的 receiver 各自
        // 独立调用 G_CEnvironment()（P48 无 CSE + P157 逐字段重读，禁 env 缓存；
        // 实证旧写缺 G_CEnvironment×1）。
        // ORIG 0x0829dc8e..0x0829dca5：m_dbType 同样先值拷贝到具名局部（-0x30）。
        // [G7-sub] 本轮复核：ORIG 289 / 我方 289、帧 0x70==0x70（0x0829d8ed
        //   `sub $0x70,%esp`）；p167 仅 map/pair/cMyTrace 的 C1↔C2 / D1↔D2 别名。
        //   残差 = **同一帧内局部槽基差 8**：sep ORIG -0x10==我方 -0x10，但
        //   p1/p2 ORIG -0x48/-0x4c vs 我方 -0x40/-0x44、dbType 副本 ORIG
        //   -0x34/-0x30 vs 我方 -0x60/-0x64、tmp map ORIG -0x64 vs 我方 -0x5c
        //   ⇒ N126（4.1 首槽 -0x4 / 4.4 -0xc 版本常数）封顶；声明序对调已实测
        //   回滚（见上条判词）。
        ENUM_SERVER_GROUP dbType = m_dbType;
        std::map<ENUM_SERVER_GROUP, std::map<std::string, std::string> >::iterator it =
            ((CDBEnvServerGroupMapView*)G_CEnvironment())->m_map.find(dbType);
        if (it != ((CDBEnvServerGroupMapView*)G_CEnvironment())->m_map.end())
        {
            // ORIG 0x0829dcf5..0x0829dcfd：先 `it.operator->()` + `add $0x4,%eax`
            // 把 &it->second 缓存进具名局部（-0xc），之后才求 make_pair 实参并
            // insert（P183 反向警告：二次访问处 ORIG 保留具名缓存；单表达式
            // `it->second.insert(...)` 4.4 会先算实参后算 receiver ⇒ 顺序反）。
            std::map<std::string, std::string>& second = it->second;
            second.insert(std::make_pair(p1, p2));
        }
    }
    return 1;
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
{
    if (type < 0 || type > 8)
    {
        LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0x2190,
                              "[PacketPool setCurrentThreadID err] pool_type(%d), thread_id(%08x)",
                              type, threadID);
        return false;
    }

    if (pthread_equal(0, s_packet_pool_list[type]->getThreadID()) == 0)
    {
        LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0x2195,
                              "[PacketPool already ThreadID setting] pool_type(%d), thread_id(%08x) alreadyID(%08x)",
                              type, threadID, s_packet_pool_list[type]->getThreadID());
        return false;
    }

    s_packet_pool_list[type]->setThreadID(threadID);
    return true;
}

// ============================================================================
// GlobalData::InitCaptcha
// ============================================================================

#include "GlobalData/InitCaptcha.inc"
{
    if (!cg_.addFont("captcha_font/SeoulHangangL.ttf", 0x24))
    {
        std::cerr << "[GlobalData::Init] cg.loadFont fail" << std::endl;
        return false;
    }

    for (int i = 0; i <= 0x27; ++i)
    {
        GenerateCaptchaData();
    }
    return true;
}

// ============================================================================
// GlobalData::GenerateCaptchaData
// ============================================================================

#include "GlobalData/GenerateCaptchaData.inc"
{
    GreyImage image;
    MSG_UDP_CAPCHA_CERTIFY msg;

    memset(&msg, 0, 0x339e);
    cg_.generateText(CountryCode::taiwan, 5, (unsigned short*)&msg);
    CaptchaGen::ImageOption opt;
    opt.m_field8 = 0;
    cg_.generateImage(0xdc, 0x3c, (const unsigned short*)&msg, 5, opt, &image);
    memcpy(msg.m_image, image.getImage(), image.getWidth() * image.getHeight());
    msg.m_size = image.getWidth() * image.getHeight();
    s_capchaDataManager.push(msg);
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
{
    delete s_ref_pvp_grade;
    delete s_accept_thr;          // ORIG 0x0829e0e8（vptr+8 虚 D0）
    if (s_monitor_proxy_mgr)
    {
        delete s_monitor_proxy_mgr;
        s_monitor_proxy_mgr = 0;
    }
    if (s_guild_proxy_mgr)
    {
        delete s_guild_proxy_mgr;
        s_guild_proxy_mgr = 0;
    }
    if (s_statistic_proxy_mgr)
    {
        delete s_statistic_proxy_mgr;
        s_statistic_proxy_mgr = 0;
    }
    delete s_udp_thr;             // ORIG 0x0829e196
    delete s_net_thr;             // ORIG 0x0829e1b5
    delete s_dispatch_thr;        // ORIG 0x0829e1d4
    delete s_db_thr_for_logDB;    // ORIG 0x0829e1f3
    delete s_db_thr;              // ORIG 0x0829e212
    delete s_monitor_tcp_thr;     // ORIG 0x0829e231
    delete s_db_mgr;
    delete s_stream_pool;
    delete s_small_stream_pool;
    delete s_big_stream_pool;
    delete s_msgq_mgr;
    delete s_pItemVendingMachine;
    CSyncSlangFilter::FreeInstance();
    delete s_pcryptRijndael_CharacView_;
    delete s_psimpleSSO;
    delete s_pGMAccounts_;
    delete s_GM_Manager;
    delete s_antibotBusinessImpl_;
    delete s_BreakAwaySys;
    delete s_ExpertJobMgr;
    delete s_villageObjectMgr;
    delete s_villageMonsterMgr;
    delete s_revengeDungeonMgr;
    delete s_DailyScheduleManager;
    delete s_onlinePreliminaryTeamMgr;
    delete s_auction_proxy;
    delete s_cera_auction_proxy;
    delete s_db_connector;
    delete s_rentalSystem;
    for (int i = 0; i < 9; ++i)
    {
        if (s_packet_pool_list[i])
        {
            delete s_packet_pool_list[i];
            s_packet_pool_list[i] = 0;
        }
    }
}

// ============================================================================
// GlobalData::initTournamentDB
// ============================================================================

// [b28d-18 N82] p167 CLEAR（36/36）；残差=槽位分配（ORIG -0x44/-0x48/-0x50 vs 4.4 -0x30/-0x18/-0x20） | ORIG 0x0829dd82
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（36/36）；残差=N82 槽位/寄存器 + N89 STL 展开。
#include "GlobalData/initTournamentDB.inc"
{
    typedef std::map<ENUM_SERVER_GROUP, std::map<std::string, std::string> > GroupNameMap;
    if (G_GameWorld()->GetChannelType() != 7 &&
        !G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        return true;
    }

    CDBEnviroment dbEnv;
    dbEnv.set_file_name(fileName);
    if (!dbEnv.load())
    {
        return false;
    }
    if (!s_db_connector)
    {
        return false;
    }

    GroupNameMap::iterator it =
        G_CEnvironment()->m_serverGroupMap.begin();
    while (it != G_CEnvironment()->m_serverGroupMap.end())
    {
        ENUM_SERVER_GROUP group = it->first;
        DBGroupMap groupMap;
        groupMap.insert(std::make_pair((ENUM_DB_HANDLE_IDX)2, group));
        groupMap.insert(std::make_pair((ENUM_DB_HANDLE_IDX)3, group));
        DBInfoMap& info = G_CEnvironment()->m_dbConnInfoByGroup[group];
        if (!s_db_connector->GetConnInfo(groupMap, info))
        {
            return false;
        }
        if (!s_db_connector->ConnectWithServerGroup(DBInfoMap(info)))
        {
            return false;
        }
        ++it;
    }
    // ORIG 0x829e040：cMyTrace 第一参即 __PRETTY_FUNCTION__
    // （rodata _ZZN10GlobalData16initTournamentDBEPKcE19__PRETTY_FUNCTION__ @08c1b100）。
    // [G7-sub] 结论（N93/N82 封顶）：ORIG 220 / 我方 220、帧 0x480==0x480
    //   （ORIG 0x0829dd87 `sub $0x480,%esp`）；p167 仅 C1↔C2 / D1↔D2 别名
    //   （map/pair/cMyTrace）。残差是**块级具名槽分配方向相反**（P112/N93）：
    //   dbEnv 副本 ORIG -0x474 / 我方 -0x478；groupMap 的 make_pair 临时
    //   ORIG 0x0829de7a 段 -0x54/-0x38 vs 我方 -0x44/-0x28；ORIG 另有
    //   -0x1c(×2) 我方无。声明序/作用域旋钮已在 parse 族实测回滚，不再试。
    cMyTrace t(__PRETTY_FUNCTION__, 0x20aa, 0);
    t("This is Tournament PvP channel");
    s_db_mgr->activateGlobalDB();
    return true;
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
// [G7-sub 2026-09-11 判词] GlobalData::Init（ORIG 0x08299fa0，3139 条巨件）。**巨件按
// guide §6.2 三件套验收，勿以 ndiff 论成败**；结论：**封顶，无改动**。证据：
//   ① gap = 0：私有 .o（同旗标/同路由 dnf_game_cxx.sh）实测 **ORIG 3139 / 我方 3139**；
//      两侧帧完全一致 `sub $0x79c,%esp`（0x08299fa6 vs 我方对应 prologue）。
//   ② p167 调用点差集：ORIG/我方各 565 call，差异**仅** C1↔C2 / D1↔D2 **同址别名**
//      （P93；ORIG `nm` 实测 `cMyTrace::C1==C2==0x0854f746`、`PacketGuard::D1==D2==0x0858de80`、
//      `CStreamGuard::D1==D2==0x0861c8d2`）：最大项 `-39 cMyTrace::C2` 全部为此类，
//      无真缺口（0 missing / 0 extra 语义项）。
//   ③ 逐块：助记符序列仅 **1 处**差异 —— 0x0829cd78 段 ORIG 为
//      `call *%ebx; test %eax,%eax; setne %al; test %al,%al; jmp 0829cdc0`
//      （其后的 "AntiBot init fail" cMyTrace 块 0x0829cd7a..0x0829cdbe **无任何入边**
//      = ORIG 侧不可达死块；P337 家族，可用恒 0 `volatile bool` 守卫强制发射复现，本轮未做）；
//      我方同行发 `je`（该块可达）。其余为 N82/N126 槽平移（-0x35c vs -0x2bc 等）+
//      N89（4.1 对本 TU 用到的 vector/map operator[]/size 发 out-of-line 静态实例）。
#include "GlobalData/Init_3.inc"
{
    s_server_stop = false;
    s_systemTime_.update();

    s_itemAmplifier_ = new CItemAmplifier;
    s_globalEffectManager_ = new CGlobalEffectManager;

    s_timeGate_ = new (std::nothrow) CTimeGate;
    if (!s_timeGate_)
    {
        std::cerr << "[GlobalData::Init] Can't allocate s_timeGate_ memory" << std::endl;
        return false;
    }

    s_serverStateManager_ = new ARAD::Arad_ServerStateManager;

    s_msgq_mgr = new MsgQueueMgr;
    if (!s_msgq_mgr)
    {
        std::cerr << "[GlobalData::Init] can't allocate msg queue memory" << std::endl;
        return false;
    }

    s_stream_pool = new StreamPool;
    if (!s_stream_pool)
    {
        std::cerr << "[GlobalData::Init] can't allocate stream pool memory" << std::endl;
        return false;
    }

    for (int i = 0; i < 9; ++i)
    {
        s_packet_pool_list[i] = new PacketPool;
        if (!s_packet_pool_list[i])
        {
            std::cerr << "[GlobalData::Init] can't allocate packet pool memory" << std::endl;
            return false;
        }
    }
    setCurrentPacketPool(THREAD_TYPE_ACCEPT, pthread_self());

    s_small_stream_pool = new SmallStreamPool;
    if (!s_small_stream_pool)
    {
        std::cerr << "[GlobalData::Init] can't allocate small stream pool memory" << std::endl;
        return false;
    }

    s_big_stream_pool = new BigStreamPool;
    if (!s_big_stream_pool)
    {
        std::cerr << "[GlobalData::Init] can't allocate big stream pool memory" << std::endl;
        return false;
    }

    s_db_mgr = new DBMgr;
    if (!s_db_mgr)
    {
        std::cerr << "[GlobalData::Init] can't allocate db manager memory" << std::endl;
        return false;
    }

    s_db_thr = new DBThread;
    if (!s_db_thr)
    {
        std::cerr << "[GlobalData::Init] Can't allocate db thread memory" << std::endl;
        return false;
    }
    s_db_thr->setLogDB(false);

    s_db_thr_for_logDB = new DBThread;
    if (!s_db_thr_for_logDB)
    {
        std::cerr << "[GlobalData::Init] Can't allocate db thread memory, s_db_thr_for_logDB"
                  << std::endl;
        return false;
    }
    s_db_thr_for_logDB->setLogDB(true);

    s_dispatch_thr = new DispatchThread;
    if (!s_dispatch_thr)
    {
        std::cerr << "[GlobalData::Init] Can't allocate dispath thread memory" << std::endl;
        return false;
    }

    s_net_thr = new NetworkThread;
    if (!s_net_thr)
    {
        std::cerr << "[GlobalData::Init] Can't allocate network thread memory" << std::endl;
        return false;
    }

    s_udp_thr = new UdpThread;
    if (!s_udp_thr)
    {
        std::cerr << "[GlobalData::Init] Can't allocate udp thread memory" << std::endl;
        return false;
    }

    s_monitor_tcp_thr = new (std::nothrow) MonitorTcpThread;
    if (!s_monitor_tcp_thr)
    {
        std::cerr << "[GlobalData::Init] Can't allocate monitor tcp thread memory" << std::endl;
        return false;
    }

    s_monitor_proxy_mgr = new (std::nothrow) CServerProxyMgr<CMonitorServerProxy>;
    if (!s_monitor_proxy_mgr)
    {
        std::cerr << "[GlobalData::Init] Can't allocate monitor server proxy memory" << std::endl;
        return false;
    }

    s_guild_proxy_mgr = new (std::nothrow) CServerProxyMgr<CGuildServerProxy>;
    if (!s_guild_proxy_mgr)
    {
        std::cerr << "[GlobalData::Init] Can't allocate guild server proxy memory" << std::endl;
        return false;
    }

    s_statistic_proxy_mgr = new (std::nothrow) CServerProxyMgr<CStatisticServerProxy>;
    if (!s_statistic_proxy_mgr)
    {
        std::cerr << "[GlobalData::Init] Can't allocate statistic server proxy memory" << std::endl;
        return false;
    }

    // ORIG 0x0829a851-0x0829a8bc：三个实参表达式求值结果由 callee-saved 寄存器/单 spill
    // 槽（-0x77c）承载（P24/P44 内联实参即时求值；命名局部会使 4.4 三值全 spill）。
    s_hades_proxy = new (std::nothrow) CHadesServerProxy(
        G_CEnvironment()->get_udp_ip_hades(),
        G_CEnvironment()->get_tcp_port_hades(),
        G_CEnvironment()->get_udp_port_hades());
    if (!s_hades_proxy)
    {
        std::cerr << "[GlobalData::Init] Can't allocate Hades server proxy memory" << std::endl;
        return false;
    }
    if (!s_hades_proxy->Init())
    {
        std::cerr << "[GlobalData::Init] Fail Init (Hades server proxy)" << std::endl;
        return false;
    }

    {
        s_double_check_proxy = new (std::nothrow)
            CDoubleConnCheckServerProxy(G_CEnvironment()->get_udp_ip_doublecheck(),
                                        G_CEnvironment()->get_udp_port_doublecheck());
        if (!s_double_check_proxy)
        {
            std::cerr << "[GlobalData::Init] Can't allocate double conn check server proxy memory"
                      << std::endl;
            return false;
        }
        if (!s_double_check_proxy->Init())
        {
            std::cerr << "[GlobalData::Init] Fail Init (double conn check server proxy)"
                      << std::endl;
            return false;
        }
    }

    if (*G_CEnvironment()->get_school_server_ip() != '\0' &&
        G_CEnvironment()->get_school_server_port() >= 1)
    {
        s_school_proxy = new (std::nothrow)
            CSchoolServerProxy(G_CEnvironment()->get_school_server_ip(),
                               G_CEnvironment()->get_school_server_port());
        if (!s_school_proxy)
        {
            std::cerr << "[GlobalData::Init] Can't allocate school server proxy memory"
                      << std::endl;
            return false;
        }
        if (!s_school_proxy->Init())
        {
            std::cerr << "[GlobalData::Init] Fail Init (school server proxy)" << std::endl;
            return false;
        }
    }

    s_pcroom_proxy = new (std::nothrow)
        CPCRoomServerProxy(G_CEnvironment()->get_pcroom_server_ip(),
                           G_CEnvironment()->get_pcroom_server_port());
    if (!s_pcroom_proxy)
    {
        std::cerr << "[GlobalData::Init] Can't allocate pcroom server proxy memory" << std::endl;
        return false;
    }

    s_community_proxy = new (std::nothrow)
        CCommunityServerProxy(G_CEnvironment()->get_community_server_ip(),
                              G_CEnvironment()->get_community_server_port());
    if (!s_community_proxy)
    {
        std::cerr << "[GlobalData::Init] Can't allocate community server proxy memory"
                  << std::endl;
        return false;
    }

    s_auction_proxy = new (std::nothrow)
        CAuctionServerProxy(G_CEnvironment()->get_auction_server_ip(),
                            G_CEnvironment()->get_auction_server_port(), "Auction Server");
    if (!s_auction_proxy)
    {
        std::cerr << "[GlobalData::Init] Can't allocate auction server proxy memory" << std::endl;
        return false;
    }

    s_cera_auction_proxy = new (std::nothrow)
        CCeraAuctionServerProxy(G_CEnvironment()->get_cera_auction_server_ip(),
                                G_CEnvironment()->get_cera_auction_server_port(),
                                "Cera Auction Server");
    if (!s_cera_auction_proxy)
    {
        std::cerr << "[GlobalData::Init] Can't allocate cera auction server proxy memory"
                  << std::endl;
        return false;
    }

    s_power_manager = new (std::nothrow) CPowerManager;
    if (!s_power_manager)
    {
        std::cerr << "[GlobalData::Init] Can't allocate power manager memory" << std::endl;
        return false;
    }
    if (!s_power_manager->Init())
    {
        std::cerr << "[GlobalData::Init] Fail Init (power manager memory)" << std::endl;
        return false;
    }
    s_power_manager->LoadPowerWarInfo();
    s_power_manager->LoadRankerInfo();

    s_fatigueBatteryHandle_ = new (std::nothrow) CFatigueBatteryHandle;
    if (!s_fatigueBatteryHandle_)
    {
        std::cerr << "[GlobalData::Init] Can't allocate Fatigue Battery Handle memory"
                  << std::endl;
        return false;
    }

    s_secret_shop = new (std::nothrow) secretshop::CSecretShop;
    if (!s_secret_shop)
    {
        std::cerr << "[GlobalData::Init] Can't allocate SecretShop Handle memory" << std::endl;
        return false;
    }

    {
        s_channel_proxy = new (std::nothrow)
            CmmChannelProxy(G_CEnvironment()->get_udp_ip_channel(),
                            G_CEnvironment()->get_udp_port_channel());
        if (!s_channel_proxy)
        {
            std::cerr << "[GlobalData::Init] Can't allocate channel server proxy memory"
                      << std::endl;
            return false;
        }
        if (!s_channel_proxy->Init())
        {
            std::cerr << "[GlobalData::Init] Fail Init (channel server proxy)" << std::endl;
            return false;
        }
    }

    s_pcryptRijndael_CharacView_ = new CRijndael;
    s_pcryptRijndael_CharacView_->Initialize("wongyaofejsvk007", 0x20,
                                             CRijndael::sm_chain0, 0x10, 0x10, 0, 0);
    s_pcryptRijndael_Password_ = new CRijndael;
    s_pcryptRijndael_Password_->Initialize("wongyaofneople00", 0x20,
                                           CRijndael::sm_chain0, 0x10, 0x10, 0, 0);
    s_pcryptRijndael_MousePassword_ = new CRijndael;
    s_pcryptRijndael_MousePassword_->Initialize("ejsvk@@key4mouse", 0x20,
                                                CRijndael::sm_chain0, 0x10, 0x10, 0, 0);

    s_pcryptTEA_MousePassword_ = new CTEA;
    s_pcryptTEA_MousePassword_->Initialize("tjsghTlWkdajtwla", 0x10, "", 0, 0);
    s_pcryptTEA_SecurityCard_ = new CTEA;
    s_pcryptTEA_SecurityCard_->Initialize("ejsvk@@@secucard", 0x10, "", 0, 0);

    s_pLogGameChannel = new (std::nothrow) WongWork::CLogGameChannel;
    s_pLogGameChannel->StartLogTimer();

    s_pCeraShop = new (std::nothrow) WongWork::CCeraShop;
    s_pItemVendingMachine = new (std::nothrow) ItemVendingMachine;

    s_pSecuDataControl = new Secu_DataControl;
    s_pSecuProtectionField = new CSecu_ProtectionField;

    g_autoPunishRuleBlackIPMgr = new WongWork::CAutoPunishRuleBlackIPMgr;
    g_autoPunishRuleBlackIPMgr->reset();
    g_autoPunishRuleHackTypeMgr = new WongWork::CAutoPunishRuleHackTypeMgr;
    g_autoPunishRuleHackTypeMgr->reset();
    g_blackIPMonitor = new WongWork::CBlackIPMonitor;
    g_blackIPMonitor->reset();

    s_accept_thr = new AcceptThread;
    if (!s_accept_thr)
    {
        std::cerr << "[GlobalData::Init] Can't allocate accept thread memory" << std::endl;
        return false;
    }

    s_ref_pvp_grade = new RefPvpGrade;
    if (!s_ref_pvp_grade)
    {
        std::cerr << "[GlobalData::Init] Can't allocate pvp grade ref" << std::endl;
        return false;
    }

    s_db_connector = new (std::nothrow) CDBConnector(s_db_mgr);
    if (!s_db_connector)
    {
        return false;
    }

    DBInfoMap& masterDbInfo = G_CEnvironment()->m_dbConnInfo;
    // ORIG make_pair<ENUM_DB_HANDLE_IDX,R17ENUM_SERVER_GROUP&>（0x0829b8ba）：
    // 第二参左值本体即 ENUM_SERVER_GROUP 类型局部。
    ENUM_SERVER_GROUP serverGroup = (ENUM_SERVER_GROUP)G_CEnvironment()->m_serverEnvir.m_serverGroup;

    // ORIG 0x0829b886-0x0829d41d：groupMap 为函数级作用域（正常析构在函数尾共享块
    // 0x0829d41d；其后各 new 的 cleanup 统一经 0x0829d3ff 先析构 map 再 Unwind），
    // 不使用内层 {}。
    DBGroupMap groupMap;
    for (int i = 1; i < 0xf; ++i)
    {
        // ORIG 0x0829b8f4：make_pair 第二参引用 -0x340(%ebp)=serverGroup 局部
        // （make_pair<ENUM_DB_HANDLE_IDX,R17ENUM_SERVER_GROUP&> 实证）——插入值为
        // serverGroup，非 i。key 侧 cast 为值传（__decay_and_strip）。
        groupMap.insert(std::make_pair((ENUM_DB_HANDLE_IDX)i, serverGroup));
    }
    // ORIG 0x0829b90e-0x0829b9ce：t1 构造前 4 个独立 operator[] 语句，**字段值本体**
    // 即被求值（db 地址→-0x778 槽、user 地址→edi、port→esi、host→ebx，无引用
    // 中间变量），随后 t1 ctor + op() 从槽/寄存器直读。register 局部避免 4.4 的
    // 引用槽物化（P35 同族）。
    register char* masterDb = masterDbInfo[(ENUM_DB_HANDLE_IDX)0].m_db;
    register char* masterUser = masterDbInfo[(ENUM_DB_HANDLE_IDX)0].m_user;
    register unsigned int masterPort = masterDbInfo[(ENUM_DB_HANDLE_IDX)0].m_port;
    register char* masterHost = masterDbInfo[(ENUM_DB_HANDLE_IDX)0].m_host;
    cMyTrace t1(__PRETTY_FUNCTION__, 0x1b0c, 9, true, false);
    t1("\t- DBConnector Init(masterdb %s/%d/%s/%s) ",
       masterHost, masterPort, masterUser, masterDb);
    if (!s_db_connector->Init(masterDbInfo[(ENUM_DB_HANDLE_IDX)0]))
        {
            cMyTrace t2(__PRETTY_FUNCTION__, 0x1b0f, 9, true, true);
            t2("\t- DBConnector Init Fail");
            return false;
        }
        cMyTrace t3(__PRETTY_FUNCTION__, 0x1b12, 9, false, true);
        t3("Success");
        cMyTrace t4(__PRETTY_FUNCTION__, 0x1b14, 9, true, false);
        t4("\t- DBConnector Get Connection Info ");
        if (!s_db_connector->GetConnInfo(groupMap, masterDbInfo))
        {
            cMyTrace t5(__PRETTY_FUNCTION__, 0x1b17, 9, true, true);
            t5("\t- DBConnector Get Connection Info Fail");
            return false;
        }
        cMyTrace t6(__PRETTY_FUNCTION__, 0x1b1a, 9, false, true);
        t6("Success");
        cMyTrace t7(__PRETTY_FUNCTION__, 0x1b1c, 9, true, true);
        t7("\t- Open DB ... ");
        if (!s_db_connector->Connect(DBInfoMap(masterDbInfo)))
        {
            cMyTrace t8(__PRETTY_FUNCTION__, 0x1b1f, 9, true, true);
            t8("\t- DBConnector Connect DB Fail");
            return false;
        }
        cMyTrace t9(__PRETTY_FUNCTION__, 0x1b22, 9, true, true);
        t9("\t- All DB Open Success");

    s_psimpleSSO = new WongWork::CSimpleSSO;
    s_psimpleSSO->init(s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)6, (ENUM_SERVER_GROUP)0));

    s_securityCardCenter = new WongWork::CSecurityCardCenter;
    s_securityCardCenter->init();

    // ORIG 0x0829be0e-0x0829be68：new 结果存**局部槽** -0x34(%ebp)（Ghidra local_38），
    // Initialize 失败路径 delete 该局部，**成功后**才 `s_pIPGHelper = pHelper`
    // （0x0829be63 全局写唯一出现）。直接赋全局会使初始化读回全局、全局写前移。
    WongWork::IPG::CIPGHelper* pIPGHelper = new (std::nothrow) WongWork::IPG::CIPGHelper;
    if (pIPGHelper->Initialize(1, 5000) < 0)
    {
        puts("Fail to init IPG Module");
        delete pIPGHelper;
        return false;
    }
    s_pIPGHelper = pIPGHelper;

    CHumanCertifier::init();

    s_pGMAccounts_ = new WongWork::CGMAccounts;

    {
        cMyTrace t1(__PRETTY_FUNCTION__, 0x1d4d, 9, true, false);
        t1("\t- Init Query Table ");
        if (!s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2, (ENUM_SERVER_GROUP)0)
                 ->GetQueryCounter()->LoadQueryTable())
        {
            cMyTrace t2(__PRETTY_FUNCTION__, 0x1d50, 9, true, true);
            t2("\t- Init Query Table Fail");
            return false;
        }
        if (!s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)4, (ENUM_SERVER_GROUP)0)
                 ->GetQueryCounter()->LoadQueryTable())
        {
            cMyTrace t3(__PRETTY_FUNCTION__, 0x1d55, 9, true, true);
            t3("\t- Init Query Table Fail");
            return false;
        }
        cMyTrace t4(__PRETTY_FUNCTION__, 0x1d58, 9, false, true);
        t4("Success");
    }

    // ORIG 0x0829c04f-0x0829c075：`movl $0x1,-0x30(%ebp)` + `movb $0x0,-0x29(%ebp)` +
    // `cmp -0x30(%ebp),%eax; sete %al` —— 比较右操作数是 int 局部（非立即数）；
    // P28 槽序即声明序：int 先声明（-0x30 深于 bool -0x29）。
    int expectedChannel = 1;
    bool syncEnable = false;
    if (G_CEnvironment()->get_channel_no() == expectedChannel)
    {
        syncEnable = true;
    }
    sync_script::GetInstanceSyncScript()->SetEnable(syncEnable);
    if (sync_script::GetInstanceSyncScript()->IsEnable())
    {
        // ORIG 0x0829c0ff-0x0829c19d：Open 实参 R2L 求值，每个 map[5] 表达式独立
        // 求值（5 次 G_CEnvironment + 5 次 operator[]，P48/O0 无 CSE）：
        // 第1次+0x2d(pass)→spill 槽、第2次+0x18(user)→edi、第3次+0x42(db)→esi、
        // 第4次+0x14(port)→ebx、第5次+0x00(host)→edx。勿缓存引用（P157）。
        if (!s_db_mgr->Open(
                (ENUM_DB_HANDLE_IDX)5,
                G_CEnvironment()->m_dbConnInfo[(ENUM_DB_HANDLE_IDX)5].m_host,
                G_CEnvironment()->m_dbConnInfo[(ENUM_DB_HANDLE_IDX)5].m_port,
                G_CEnvironment()->m_dbConnInfo[(ENUM_DB_HANDLE_IDX)5].m_db,
                G_CEnvironment()->m_dbConnInfo[(ENUM_DB_HANDLE_IDX)5].m_user,
                G_CEnvironment()->m_dbConnInfo[(ENUM_DB_HANDLE_IDX)5].m_pass,
                (ENUM_SERVER_GROUP)0))
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x1d8d, 9, true, true);
            t("\t- Failed to open web DB");
            return false;
        }
        cMyTrace t(__PRETTY_FUNCTION__, 0x1d90, 9, true, true);
        t("\t- Success WEB DB OPEN");
    }

    // ORIG 0x0829c24b-0x0829c281：聚合 bool 变量在 4.1 中寄存器化（mov $1/$0,%eax
    // join 汇聚，N3 负例——O0 源码无法复现）；此处以直接短路条件消除 slot 汇聚，
    // 语义与 ORIG 一致：两表任一失败即报错返回。
    if (!(CSyncSlangFilter::GetInstance()->AddSlangListName()
          && CSyncSlangFilter::GetInstance()->AddSlangList()))
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1d9f, 9, true, true);
        t("Failed to read slang list");
        return false;
    }

    s_event_manager = new (std::nothrow) CEventManager;
    if (!s_event_manager)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1da8, 9, true, true);
        t("Can't allocate CEventManager memory");
        return false;
    }

    cMyTrace t(__PRETTY_FUNCTION__, 0x1dc6, 9, true, true);
    t("\t- Load GM List From DB");
    gmList = new (std::nothrow) Gm_List;
    if (!gmList)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1dcd, 9, true, true);
        t("\t- Can't allocate memory for a Gm_List");
        return false;
    }

    s_GM_Manager = new (std::nothrow) CGM_Manager;
    if (!s_GM_Manager)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1dd6, 9, true, true);
        t("Can't allocate memory for a CGM_Manager");
        return false;
    }

    {
        int keyLen = G_Cipher()->GetTotalKeyLength();
        unsigned char key[1024];
        for (int i = 0; i < keyLen; ++i)
        {
            // ORIG 0x0829c54e-0x0829c564：rand() 结果经 sar/shr/and/sub 带符号取模
            // 展开（P14）⇒ 源码是 `rand() % 256`，非 C 截断强转。
            key[i] = (unsigned char)(rand() % 256);
        }
        G_Cipher()->Initialize(key, keyLen);
    }

    // ORIG 0x0829c5a2-0x0829c605：value-init nothrow new（guard 内 rep stos 清零 +
    // call C2 均为 4.1 value-init 调用侧展开；4.4 对 user-provided ctor 只发 call，
    // memset 6 行为跨版本结构性差异。手写 placement 会额外产生 _ZnwjPv/_ZdlPvS_
    // 调用，call 序列偏差更大，故用 value-init 形式）。
    s_statisticMgr = new (std::nothrow) CStatisticMgr();
    if (!s_statisticMgr)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1df7, 9, true, true);
        t("Can't allocate memory for a CStatisticMgr");
        return false;
    }

    s_BreakAwaySys = new (std::nothrow) break_away_prevent::CBreakAwayPreventSystem;
    if (!s_BreakAwaySys)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1dfe, 9, true, true);
        t("Can't allocate memory for a s_BreakAwaySys");
        return false;
    }

    s_ExpertJobMgr = new (std::nothrow) expert_job::CExpertJobMgr;
    if (!s_ExpertJobMgr)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1e05, 9, true, true);
        t("Can't allocate memory for a CExpertJobMgr");
        return false;
    }

    s_villageObjectMgr = new (std::nothrow) village_object::CVillageObjectMgr;
    if (!s_villageObjectMgr)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1e0c, 9, true, true);
        t("Can't allocate memory for a CVillageObjectMgr");
        return false;
    }

    s_villageMonsterMgr = new (std::nothrow) village_attacked::CVillageMonsterMgr;
    if (!s_villageMonsterMgr)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1e13, 9, true, true);
        t("Can't allocate memory for a CVillageMonsterMgr");
        return false;
    }

    s_rentalSystem = new (std::nothrow) InstanceRentalSystem;
    if (!s_rentalSystem)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1e1b, 9, true, true);
        t("an`t allocate memory for a InstanceRentalSystem");
        return false;
    }

    s_revengeDungeonMgr = new (std::nothrow) village_attacked::CRevengeDungeon;
    if (!s_revengeDungeonMgr)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1e24, 9, true, true);
        t("Can't allocate memory for a CRevengeDungeon");
        return false;
    }

    s_DailyScheduleManager = new (std::nothrow) CDailyScheduleManager;
    if (!s_DailyScheduleManager)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1e2d, 9, true, true);
        t("Can't allocate memory for a CDailyScheduleManager");
        return false;
    }

    s_onlinePreliminaryTeamMgr = new (std::nothrow) online_preliminary::COnlinePreliminaryTeamMgr;
    if (!s_onlinePreliminaryTeamMgr)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1e36, 9, true, true);
        t("Can't allocate memory for a COnlinePreliminaryTeamMgr");
        return false;
    }

    s_GameMasterMgr = new (std::nothrow) game_master::CGameMasterMgr;
    if (!s_GameMasterMgr)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1e47, 9, true, true);
        t("Can't allocate memory for a CGameMasterMgr");
        return false;
    }

    TimerUpdatePvPGrade::registNextTimer(s_systemTime_.getCurSec());
    DB_ReloadAutoPunishRuleBackIP::makeRequest();
    DB_ReloadAutoPunishRuleHackType::makeRequest();
    DB_LoadCleanPadPoint::makeRequest();
    DB_BlackIPMonitor::makeRequest();

    s_antibotBusinessImpl_ = new CBusinessImpl;
    s_antibotChecker_ = CreateAntiServerObj("./");
    if (!s_antibotChecker_)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1eb0, 5);
        t("AntiBot create fail");
        return false;
    }

    {
        // ORIG 0x0829cd18：`movl $0x0,-0x35c(%ebp)` 单 dword 清零 —— 标量初始化形态。
        char seq[4] = {'\0'};
        // ORIG 0x8471ebd：G_CEnvironment()->getProcessSequence() 真实读环境进程序列号
        sprintf(seq, "%d", G_CEnvironment()->getProcessSequence());
        // ORIG 0x0829cd6f：init 经 vtable 槽0 调用（对象首字段=表指针，表[0]=Init：
        // 0x829cd49 `mov A,%eax; mov (%eax),%eax; mov (%eax),%ebx` 两级解引用），
        // 返回值非 0 记 "AntiBot init fail" 并返 0。
        // 第二参按 32 位整型传递（ORIG 0x829cd57 `mov B,%eax; mov %eax,%edx` 两段装载，
        // 形参 int 才触发 4.4 同形；void* 形参 4.4 折单段）。
        typedef int (*AntibotInitFn)(void*, int, char*);
        // ORIG 0x0829cd6f-0x0829cdd8：调用结果 test/setne 物化（P15），表达式直放条件位，
        // 勿先存具名局部（具名 int + `!= 0` 生成 cmpl/je 无 setne）。
        if ((*(AntibotInitFn**)s_antibotChecker_)[0](
                s_antibotChecker_, (int)s_antibotBusinessImpl_, seq))
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x1eb7, 5);
            t("AntiBot init fail");
            return false;
        }
    }

    // ORIG 0x0829cdd4-0x0829ce2b：同上 value-init 第二处。
    s_statisticMgr = new (std::nothrow) CStatisticMgr();
    if (!s_statisticMgr)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1ebf, 9, true, true);
        t("Can't allocate memory for a CStatisticMgr");
        return false;
    }

    g_Actions = new (std::nothrow) StackableAction::Action;
    if (!g_Actions)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1ed9, 9, true, true);
        t("Can't allocate memory for a StackableAction::Action()");
        return false;
    }

    {
        // ORIG 0x0829cf56-0x0829cf70：Acquire 结果直放 guard ctor 实参（P24），
        // 不经具名局部（命名会多生 `mov %eax,-slot; mov -slot,%eax` 往返）。
        CStreamGuard guard(s_stream_pool->Acquire("App.cpp", 0x1ee0), true);
        **guard << 0x277;
        **guard << -1;
        s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }

    // P59/P16 guard 化（ORIG 0x0829cffb-0x0829d3fd）：InitCaptcha/initialize/
    // initGeolocation 均为 bool 返回 `xor $0x1; test; je body` 前置失败块；
    // Singleton::Get() 结果直放链式调用（P52），不落具名局部。
    if (!InitCaptcha())
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1eea, 9, true, true);
        t("captcha data initialization failed");
        return false;
    }

    Timer_GenerateCaptchaData::RegistNextTimer(3);
    g_ExtractionMgr = new (std::nothrow) expert_extraction::CExtractionMgr;
    if (!g_ExtractionMgr)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1ef5, 9, true, true);
        t("Can't allocate memory for a CExtractionMng");
        return false;
    }

    g_pTeleport[0] = new (std::nothrow) NPC_Teleport;
    if (!g_pTeleport[0])
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1f02, 9, true, true);
        t("Can't allocate memory for a NPC_Teleport()");
        return false;
    }

    g_pTeleport[1] = new (std::nothrow) User_Teleport;
    if (!g_pTeleport[1])
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1f09, 9, true, true);
        t("Can't allocate memory for a User_Teleport()");
        return false;
    }

    g_randomOptionHandle = new (std::nothrow) random_option::CRandomOptionItemHandle;
    if (!g_randomOptionHandle)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1f12, 9, true, true);
        t("Can't allocate memory for a CRandomOptionItemHandle");
        return false;
    }

    if (!ARAD::Singleton<ServiceRestrictManager>::Get()->initialize())
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1f1f, 9, true, true);
        t("Can't allocate memory for a ServiceRestrictManager");
        return false;
    }
    ARAD::Singleton<ServiceRestrictManager>::Get()->savePolicy();
    ARAD::Singleton<ServiceRestrictManager>::Get()->loadPolicy();
    if (!ARAD::Singleton<RestrictGeolocation>::Get()->initGeolocation())
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1f2a, 9, true, true);
        t("Can't initialize GeoIP");
        return false;
    }
    return true;
}

// ###########################################################################
// # 租赁域（RandomRentalA2 批次）：pc_room 自由函数 + 两台租赁机器本体 +      #
// # InstanceRentalSystem::{RentalInfo,DeletedRentItem} C1 + CUser 别名       #
// # 全部逐报告转写；无报告者标注"报告缺失，objdump 直转"。                    #
// ###########################################################################

// ---- InstanceRentalSystem::RentalInfo C1 —— W 0x082719f6 ------------------
// （报告缺失，objdump 直转：清 +0 字/+2 半/+4 int/+8..+b 四字节，+0xc=-1）
#include "GlobalData/RentalInfo.inc"
{
    m_flag0 = 0;
    m_slot = 0;
    m_itemIndex = 0;
    m_field8 = 0;
    m_field9 = 0;
    m_inEquip = 0;
    m_fieldB = 0;
    m_rentItemId = -1;
}

// ---- InstanceRentalSystem::DeletedRentItem C1 —— W 0x08271a3a -------------
// （报告缺失，objdump 直转：清 +0 字节/+2 半字/+4 int）
#include "GlobalData/DeletedRentItem.inc"
{
    m_bEquip = 0;
    m_slot = 0;
    m_itemId = 0;
}
// ---- InstanceRentalSystem::RentInput::RentInElem C1 —— W 0x0822f02e ------
// （objdump 直转：逐字节清 +0..+3，+4=-1；无帧无栈，每字段重读 this）
#include "GlobalData/RentInElem.inc"
{
    m_sectionIdx = 0;
    m_itemIdx = 0;
    m_bEquip = 0;
    m_type = 0;
    m_rentItemId = -1;
}

// ---- InstanceRentalSystem::RentInput C1 —— W 0x0822f058 ------------------
// （objdump 直转：vector C1 后清 +0xc int、+0x10/+0x11 两字节）
#include "GlobalData/RentInput.inc"
{
    m_machineKey = 0;
    m_logFlag = 0;
    m_field11 = 0;
}

// ---- CUser::GetCurrentCharacRentalInfo —— T 0x08677166 --------------------
// （GetCurrentCharacRentalInfo.md：尾调小写版 getCurrentCharacRentalInfo）
#include "GlobalData/GetCurrentCharacRentalInfo.inc"
    std::vector<InstanceRentalSystem::RentalInfo>& out, int param)
{
    return getCurrentCharacRentalInfo(out, param);
}

// makeFairPVPEquipmentPackageKey —— T 0x088945ad（InitScriptProducers_2.cpp 定义；
// DataManagerInitDepends.h 与本 TU 本地类冲突，故此处仅前向声明）
#include "GlobalData/makeFairPVPEquipmentPackageKey.inc"

namespace pc_room
{

// ---- IsPcRoomUser —— T 0x08270fea（IsPcRoomUser.md）------------------------
#include "GlobalData/IsPcRoomUser.inc"
{
    return user.GetPCRoomNo() != 0;
}

// ---- CheckRentableEnvironment —— T 0x08271002（同名报告）-------------------
#include "GlobalData/CheckRentableEnvironment.inc"
{
    int channelType = G_GameWorld()->GetChannelType();
    if (channelType == 0xf || channelType == 0x10)
    {
        return false;
    }
    return true;
}

// ---- CheckRentableEnvironmentAtChannelType —— T 0x08271032（同名报告）------
#include "GlobalData/CheckRentableEnvironmentAtChannelType.inc"
{
    int channelType = G_GameWorld()->GetChannelType();
    if (channelType == 0xf || channelType == 0x10)
    {
        return false;
    }
    return true;
}

// ---- GetSelectionIndex —— T 0x082711f7（同名报告全文转写）------------------
EventRewardSection* GetRewardSection(int idx, int type);   // 定义见下方
#include "GlobalData/GetSelectionIndex.inc"
{
    const _Charac_info* cur = user.getCurCharacR();
    if (cur == 0)
    {
        return false;
    }

    // 角色数据 +0x27 short 选择索引；isAffectedPremium(0x16) 时 +5；
    // type==2 用未加成原值（ORIG 重读）。+0x27 = _Charac_info::m_level（short，
    // ORIG 0827121e movzwl 0x27(%eax); cwtl，CUser.h 字段化已落，CUserOnlyOrig 确认）。
    int selIdx = (int)cur->m_level;
    if (user.isAffectedPremium((ENUM_PREMIUM_TYPE)0x16))
    {
        selIdx += 5;
    }
    if (type == 2)
    {
        selIdx = (int)cur->m_level;
    }

    // ORIG：std::max<int>(0, selIdx)（模板调用，非 ?: cmov），再统一经
    // GetRewardSection(idx, type) 三分支分派（ORIG 不在此内联 type 分支）。
    // [b24ab] ORIG 将 max 结果解引用直接内联为 GetRewardSection 首参（无 chosen
    // 中间槽，P134/P197③形）
    EventRewardSection* section = GetRewardSection(std::max(0, selIdx), type);
    if (section == 0)
    {
        return false;
    }
    outIndex = (int)section->m_field0;                 // 段首字节
    return true;
}

// ---- GetCharacterJob —— T 0x082712a6（同名报告全文转写）--------------------
#include "GlobalData/GetCharacterJob.inc"
{
    const _Charac_info* cur = user.getCurCharacR();
    if (cur == 0)
    {
        return false;
    }
    outJob = cur->m_field26;  // job 字节（+0x26）
    return true;
}

}  // namespace pc_room

// ---- PcBangItemRentarData::getSectionData —— T 0x08915518（同名报告）-------
#include "GlobalData/getSectionData.inc"
{
    // rbegin→rend：返回首个 m_field1（段上界字节）<= idx 的段。
    for (std::vector<EventRewardSection>::reverse_iterator it = m_vec0.rbegin();
         it != m_vec0.rend(); ++it)
    {
        if ((int)(unsigned)it->m_field1 <= idx)
        {
            return &(*it);
        }
    }
    return 0;
}

// ---- DimensionActivationRewardData 三查询（同名报告全文转写）---------------
#include "GlobalData/getSectionData_1.inc"
{
    for (std::vector<EventRewardSection>::reverse_iterator it = m_vec0.rbegin();
         it != m_vec0.rend(); ++it)
    {
        if ((int)(unsigned)it->m_field1 <= idx)
        {
            return &(*it);
        }
    }
    return 0;
}

std::vector<std::vector<EventRewardItemInfo> >*
#include "GlobalData/getRewardSelectItemList.inc"
{
    std::map<int, std::vector<std::vector<EventRewardItemInfo> > >::iterator it =
        m_mapC.find(packageKey);
    if (it != m_mapC.end())
    {
        return &it->second;
    }
    return 0;
}

std::vector<std::vector<EventRewardItemInfo> >*
#include "GlobalData/getRewardGiveItemList.inc"
{
    std::pair<int, int> key;
    key.first = job;
    key.second = growType;
    std::map<std::pair<int, int>,
             std::vector<std::vector<EventRewardItemInfo> > >::iterator it =
        m_map24.find(key);
    if (it != m_map24.end())
    {
        return &it->second;
    }
    return 0;
}

std::vector<std::vector<EventRewardItemInfo> >*
#include "GlobalData/getRewardSelectionList.inc"
{
    std::map<int, std::vector<std::vector<EventRewardItemInfo> > >::iterator it =
        m_mapC.find(packageKey);
    if (it != m_mapC.end())
    {
        return &it->second;
    }
    return 0;
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
                     EventRewardItemInfo& out, int type)
{
    int selectionIdx = 0;
    if (!GetSelectionIndex(user, selectionIdx, type))
    {
        return false;
    }
    char job = 0;
    if (!GetCharacterJob(user, job))
    {
        return false;
    }

    int packageKey =
        makeFairPVPEquipmentPackageKey((int)job, selectionIdx);

    // ORIG 0x08271396：委托 pc_room::GetRewardItemSelectionList(packageKey,type)
    // （p167 MISSING 实证；原内联三分支多发 G_CDataManager×2 + 具体方法×2）。
    std::vector<std::vector<EventRewardItemInfo> >* list =
        GetRewardItemSelectionList(packageKey, type);
    if (list == 0)
    {
        return false;
    }

    // [b28d-solo-GlobalData] P214①/P222：ORIG 0x0826f91e 先 call size() 落
    // -0x14，再 `movzbl (%eax); test %al,%al; js F1`、`movsbl; cmp -0x14(%ebp),%eax;
    // jl OK1`（失败块 F1 居中 `mov $0; jmp end`、成功体 jl 直落、无 setcc 汇聚）；
    // 第二段同构（0x0826f96e）。
    // [FL7GlobalData 2026-09-10 · P169 双向实测否定] 改两级正向 if 嵌套
    // （`if (sectionIdx >= 0) { if (sectionIdx < listSize) { … return true; } } return false;`）
    // 后单件 ndiff 仍 2（我方仍多一条 `jmp`）且 TU 四数 ae 1791→1790 回退 ⇒ 已回滚，
    // 维持早退两条件形（残余 2 条 jmp 记 N59/N82 族）。
    const int listSize = (int)list->size();
    if (sectionIdx < 0 || listSize <= sectionIdx)
    {
        return false;
    }
    std::vector<EventRewardItemInfo>& inner = list->at((std::size_t)sectionIdx);
    const int innerSize = (int)inner.size();
    if (seqIdx < 0 || innerSize <= seqIdx)
    {
        return false;
    }
    out = inner[(std::size_t)seqIdx];
    return true;
}

// ---- CheckInventoryEquipmentEmptyCount —— T 0x08271062（同名报告全文转写）--
// ORIG 0x08271062：call; leave; ret 穿透。内外配对律（batch-13）：callee
// check_empty_count 终裁 bool ⇒ 本外层同步 bool（probe3 实证 bool+bool 直穿无
// movzbl；int 外 + bool 内必产 movzbl 归一段）。
#include "GlobalData/CheckInventoryEquipmentEmptyCount.inc"
{
    return user.getCurCharacInvenR()->check_empty_count(
        (Inven_Item::ITEM_TYPE)1, need);
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
{
    // ORIG（0x0827108c，report 汇编+反编译 C 全读）：每次 check 前重调
    // getCurCharacInvenR()。**真语义 bug 修正（b28d-solo-GlobalData）**：ORIG
    // 0x082710b4 族为 `xor $1,%eax; test %al,%al; jne +0x15d`，而 +0x15d 即
    // `mov $0x1,%eax; jmp leave` ⇒ **任一 check 为假即 return 1**（Ghidra C 同形：
    // `if (cVar1 == 1) { … } return 1;`）；旧重建写 `return false` 与 ORIG 相反。
    // 末检查 0x082711e7 `jl +0x164(=mov $0,%eax)` ⇒ GetRemainCapacity < need 才
    // return 0，否则 return 1。照抄 ORIG 可观察行为（P171）。
    // [GlobalData-判据①] 0x082710b9 族 7 处 `xor $1;test %al,%al;jne 0x082711e9`
    // 全部直达**同一** `mov $1` 尾块（0x082711e9，同时也是末比较 `jl` 的直落臂）
    // ⇒ 原码是**单个三元表达式全表达式链**（4.1 短路 `||` 逐项 `jne L_true`，真臂
    // 与末项真臂共尾）：
    //   return (!c1 || … || !c7) ? true : (GetRemainCapacity(2,8) >= need);
    // 探针 /tmp/l7/probe1.cpp D 形逐条同形（A 嵌套 `if(!c)` 形反产 `je RET1`、
    // B/C/E 形各多一级 bool 物化或 setcc）。旧嵌套形另含**真语义 bug**：其运行语义
    // 为「任一 check 为真即 return true」，与 ORIG「任一 check 为假即 return 1」
    // 相反（旧注释自述即 ORIG 语义，代码却反写）。
    return (!user.getCurCharacInvenR()->check_empty_count((Inven_Item::ITEM_TYPE)1, need)
            || !user.getCurCharacInvenR()->check_empty_count((Inven_Item::ITEM_TYPE)2, need)
            || !user.getCurCharacInvenR()->check_empty_count((Inven_Item::ITEM_TYPE)3, need)
            || !user.getCurCharacInvenR()->check_empty_count((Inven_Item::ITEM_TYPE)10, need)
            || !user.getCurCharacInvenR()->check_empty_count((Inven_Item::ITEM_TYPE)9, need)
            || !user.getCurCharacInvenR()->check_empty_count((Inven_Item::ITEM_TYPE)5, need)
            || !user.getCurCharacInvenR()->check_empty_count((Inven_Item::ITEM_TYPE)6, need))
               ? true
               : (user.getCurCharacInvenR()->GetRemainCapacity(
                      (INVEN_TYPE)2, (Inven_Item::ITEM_TYPE)8) >= need);
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
    CUser& user,
    std::vector<InstanceRentalSystem::DeletedRentItem>& deleted, int rentalType)
{
    std::vector<InstanceRentalSystem::RentalInfo> rentalList;
    if (!user.GetCurrentCharacRentalInfo(rentalList, rentalType))
    {
        return;
    }

    bool isEquip = false;
    int slot = 0;
    for (std::vector<InstanceRentalSystem::RentalInfo>::iterator it =
             rentalList.begin();
         it != rentalList.end(); ++it)
    {
        // ORIG 0x08271807：`(*it).m_itemIndex`（deEv，非 ptEv；P167 判据）。
        // [SweepGD 2026-09-10 P0/P183 直证] ORIG 0x08271874 `mov 0x4(%eax),%eax;
        // mov %eax,%ebx`（itemId 经 %ebx 跨调用、**无具名槽**）+ 0x08271879..0x08271899
        // `getCurCharacInvenR` 结果**直传** CheckExist（**无 invR 槽**）⇒ 删
        // `const int itemId` 与 `CInventory* invR` 两具名局部（我方旧形多 4 条往返：
        // itemId -0x10 / invR -0xc 各一条 store + 一条 load），使用点内联重读。
        // [G7 恢复具名迭代器引用·P0 直证] ORIG 0x08271869 `call deEv; mov %eax,-0xc(%rbp)`
        // ⇒ 原码有具名引用 `RentalInfo& info = *it;`（槽往返），0x08271910 复读该槽
        // （`mov -0xc(%ebp),%eax; mov 0x4(%eax),%eax`）；旧码逐点写 `(*it).m_itemIndex`
        // ⇒ p167 我方 +1 `deEv`（多一次迭代器解引用）。P0：ORIG 有槽往返必须有具名局部。
        InstanceRentalSystem::RentalInfo& info = *it;
        if (!const_cast<CInventory*>(user.getCurCharacInvenR())
                 ->CheckExistItemInInventoryOrEquipment(
                     info.m_itemIndex, slot, isEquip))
        {
            continue;
        }

        // ORIG 0x082718c0/0x082718e4：两分支各自独立调用 getCurCharacInvenW
        // 与 ResetSlot（P48 无 CSE；原三元式只调一次 ⇒ p167 MISSING 各 1）。
        if (isEquip)
        {
            user.getCurCharacInvenW()->ResetSlot(0, slot);
        }
        else
        {
            user.getCurCharacInvenW()->ResetSlot(1, slot);
        }

        // [SweepGD P29/P54 直证] ORIG 0x08271902..0x08271916 写序 = slot(-0x2e)
        // → bEquip(-0x30) → itemId(-0x2c)；且 m_itemId 从 `(*it)` 引用**重读** +4
        // （0x08271910 `mov -0xc(%ebp),%eax; mov 0x4(%eax),%eax`），不复用任何 itemId
        // 局部；m_bEquip 为 unsigned char ⇒ `= isEquip` 直存（ORIG movzbl+mov，P136）。
        InstanceRentalSystem::DeletedRentItem entry;
        entry.m_slot = (unsigned short)slot;
        entry.m_bEquip = isEquip;
        entry.m_itemId = info.m_itemIndex;   // ORIG 0x08271910 复读 -0xc 槽（非再次 deEv）
        deleted.push_back(entry);
    }
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
                     const InstanceRentalSystem::RentInElem& inElem,
                     const EventRewardItemInfo& reward,
                     InstanceRentalSystem::RentOutput& out, int reason)
{
    // [b28d-PoolGlobalData] 余 1 条 = ORIG 0x082714d2 `mov (%eax),%eax;
    // mov %eax,%ebx` 两段装载（4.1 regmove），4.4 恒直落 %ebx；`register` 提示
    // 形实测反劣（ndiff 3→20：ebx 长命化引 push %esi/帧 0xb0/复用 ebx 少两读）
    // ⇒ 回退具名形，N18a 封顶。
    CItem* script = G_CDataManager()->find_item(reward.m_field0);
    if (script == 0)
    {
        return 1;
    }

    Inven_Item item;
    item.m_addInfo = reward.m_field0;                 // +0x02
    script->make_item(item);                          // ORIG 虚表槽 +0x08

    // ORIG 0x08271526：装备路径先写 rentId（-0x52=item+7），后 SetUpgrade。
    if (inElem.m_rentItemId != -1)
    {
        item.m_addInfo2 = inElem.m_rentItemId;        // +0x07
    }
    item.SetUpgrade((unsigned char)reward.m_short8);

    InstanceRentalSystem::RentalInfo ri;

    ri.m_slot = (short)user.getCurCharacInvenW()->insertItemIntoEquipment(
        item, (eItemAddReason)reason, true, false);    // ORIG `mov %ax,-0x1a` 直存
    ri.m_itemIndex = reward.m_field0;
    ri.m_flag0 = 3;                                   // 装备租借标记
    ri.m_inEquip = 1;
    ri.m_field8 = inElem.m_sectionIdx;                // ORIG 0xc[eax+0]
    ri.m_field9 = inElem.m_itemIdx;                   // ORIG 0xc[eax+1]
    ri.m_fieldB = inElem.m_type;                      // ORIG 0xc[eax+3]
    // ORIG 0x0827162a：重读 inElem+4 判 -1；真支重读存，假支回读 item+7。
    if (inElem.m_rentItemId != -1)
    {
        ri.m_rentItemId = inElem.m_rentItemId;
    }
    else
    {
        ri.m_rentItemId = item.m_addInfo2;
    }

    out.m_infos.push_back(ri);
    return 0;
}

// ---- ProcessInsertItem —— T 0x08271666（同名报告全文转写）------------------
// [b28d-18 N71] EXTRA RentalInfo::D1Ev×2 + _Unwind_Resume×1：ORIG 空析构被 4.1 判 trivial 不调用（P71；throw() 实测无效） | ORIG 0x08271666
// [b28d-solo-GlobalData] DIFF 封顶（已被二压推翻）：N71 判词倒果为因。
// [b28d-PoolGlobalData 二压] ①删 `~RentalInfo(){}` 伪建模（ORIG `_Destroy_aux<ILb1E>`
// 实证平凡）⇒ EXTRA D1Ev×2+_Unwind_Resume×1 归零；②同 0x082714c5 全切片重对齐：
// 删 savedRentId/slot 具名、ri 写序 slot→itemIndex→flag0→inEquip→f8→f9→fB→rentId、
// **语义修正** m_field8/9/B=inElem[0]/[1]/[3]（0x082717b1/7ba/7c4 `movzbl` 直证）。
#include "GlobalData/ProcessInsertItem.inc"
                      const InstanceRentalSystem::RentInElem& inElem,
                      const EventRewardItemInfo& reward,
                      InstanceRentalSystem::RentOutput& out, int reason)
{
    CItem* script = G_CDataManager()->find_item(reward.m_field0);
    if (script == 0)
    {
        return 1;
    }

    Inven_Item item;
    item.m_addInfo = reward.m_field0;                 // +0x02
    script->make_item(item);                          // ORIG 虚表槽 +0x08

    item.SetUpgrade((unsigned char)reward.m_short8);  // 插入路径：先升级后 rentId
    if (inElem.m_rentItemId != -1)
    {
        item.m_addInfo2 = inElem.m_rentItemId;        // +0x07
    }

    InstanceRentalSystem::RentalInfo ri;

    ri.m_slot = (short)user.getCurCharacInvenW()->insertItemIntoInventory(
        item, (eItemAddReason)reason, true, false);    // ORIG `mov %ax,-0x1a` 直存
    ri.m_itemIndex = reward.m_field0;
    ri.m_flag0 = 0;                                   // 背包租借标记
    ri.m_inEquip = 0;
    ri.m_field8 = inElem.m_sectionIdx;                // ORIG 0xc[eax+0]
    ri.m_field9 = inElem.m_itemIdx;                   // ORIG 0xc[eax+1]
    ri.m_fieldB = inElem.m_type;                      // ORIG 0xc[eax+3]
    // ORIG 0x082717cb：重读 inElem+4 判 -1；真支重读存，假支回读 item+7。
    if (inElem.m_rentItemId != -1)
    {
        ri.m_rentItemId = inElem.m_rentItemId;
    }
    else
    {
        ri.m_rentItemId = item.m_addInfo2;
    }

    out.m_infos.push_back(ri);
    return 0;
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
{
    char serverInfo = 0;
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        serverInfo = (char)user.GetServerGroup();
    }
    else
    {
        serverInfo = (char)G_CEnvironment()->m_serverEnvir.m_serverGroup;
    }
    // [b28d-solo-GlobalData] ORIG 0x082707e8 `movsbl -0x19; mov %eax,-0x2c` 槽往返
    // ⇒ 原码有具名 int 提升局部（P0 有槽必有局部）。
    const int serverEnv = (int)serverInfo;
    // [b28d-solo-GlobalData] P231/P248：ORIG 0x082707c0 三个值跨 CMyFileLog ctor
    // 保活于 callee-saved（esi=charNo/ebx=accId/edi=m_data）⇒ 纯 register 提示
    // （非 asm 绑定，P0 合规）；缺 register 时 4.4 落栈槽（-0x10/-0xc）。
    // [Lane7GlobalData · P248补四 声明序旋钮——**已实测回滚**] pData 提首位后
    // register 角色未转正，反把本件由 NEAR 打成 DIFF（TU：near 11→9、diff 38→40）
    // ⇒ 恢复原声明序（charNo/accId/pData）。判词：4.4 的 callee-saved 归属不随
    // 声明序轮转（与 P248补四 的「位置」结论在**多 register 局部**场景不适用）。
    register const int charNo = user.get_charac_no(-1);
    register const unsigned int accId = user.get_acc_id();
    register char* const pData = m_impl->m_data;

    CMyFileLog flog("void RentalMachinePcRoom::log(CUser&, int)", 0x12e);
    flog(pData,
         "[RENT] m_id(%d) char_no(%d) server_info(%d) item_id(%d)", accId,
         charNo, serverEnv, itemId);
}

// ---- Dimension::log —— T 0x0826ef16（同名报告全文转写）---------------------
// [b28d-18 N7+N82] 槽位+callee-saved 角色轮换；P76 待办：ORIG auto_ptr 模板参为嵌套类 RentalMachineDimension::RentalMachineDimensionImpl（p167 MISSING ptEv） | ORIG 0x0826ef16
// [b28d-solo-GlobalData] NEAR 封顶：p167 CLEAR（9/9）；N7（callee-saved 角色轮换 esi/ebx/edi）+
// N82（帧 0x4c vs 0x3c、槽位平移）。
#include "GlobalData/log_1.inc"
{
    char serverInfo = 0;
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        serverInfo = (char)user.GetServerGroup();
    }
    else
    {
        serverInfo = (char)G_CEnvironment()->m_serverEnvir.m_serverGroup;
    }
    // [b24ab] 显式 int 提升到命名局部（ORIG +0x3c movsbl → -0x2c 槽）
    const int serverEnv = (int)serverInfo;
    // [b24ab] ORIG 将 charNo/accId/m_data 缓存在 callee-saved（esi/ebx/edi）跨
    // CMyFileLog 构造存活 ⇒ register 强占（P197②形）
    // [Lane7GlobalData · P248补四 声明序旋钮——**已实测回滚**] 同 PcRoom 版：
    // 提前 pData 反使本件 NEAR→DIFF ⇒ 恢复原声明序。
    register const int charNo = user.get_charac_no(-1);
    register const unsigned int accId = user.get_acc_id();
    register char* const pData = m_impl->m_data;

    CMyFileLog flog("void RentalMachineDimension::log(CUser&, int)", 0x71);
    // [G7-sub] 结论（N115/N126/N82 封顶）：ORIG 58 / 我方 58。ORIG 0x0826ef1c
    //   `sub $0x4c,%esp` + 槽 serverInfo(-0x19)/CMyFileLog(-0x24)/serverEnv(-0x2c)
    //   （另有空槽 -0x28）；我方 `sub $0x3c,%esp` + -0x1d/-0x28/-0x1c。
    //   ORIG 0x0826ef52 `movsbl -0x19(%ebp),%eax; mov %eax,-0x2c(%ebp)` 把
    //   serverEnv 放 CMyFileLog **之下**，4.4 恒放其上方 ⇒ 4 槽向 + 帧 16B 属
    //   N126（版本首槽基址常数）+ N115（outgoing 预留）族。三值 register 保活
    //   (esi/ebx/edi) 已命中 P231，仅余寄存器名轮换（P230/N7）。
    flog(pData,
         "[RENT] Dimension m_id(%d) char_no(%d) server_info(%d) item_id(%d)",
         accId, charNo, serverEnv, itemId);
}

// ---- Dimension::checkRentalLevel —— T 0x0826f7e4（同名报告）----------------
#include "GlobalData/checkRentalLevel.inc"
{
    if (0x40 < level)
    {
        return true;
    }
    return false;
}

// ---- Dimension::checkRentalGrowJob —— T 0x0826f7fc（同名报告）--------------
#include "GlobalData/checkRentalGrowJob.inc"
{
    if (growType != 0)
    {
        return true;
    }
    return false;
}

// ---- Dimension::GetRewardItemGiveList —— T 0x0826f814（同名报告：透传）-----
#include "GlobalData/GetRewardItemGiveList.inc"
{
    // ORIG 0x0826f814：基址表达式 add 后落 -0xc 槽（指针局部），非直接 lea 传参。
    DimensionActivationRewardData* data =
        reinterpret_cast<DimensionActivationRewardData*>(
            reinterpret_cast<char*>(G_CDataManager()) + 0x4b34);
    return (int)data->getRewardGiveItemList(job, growType);
}

// ---- Dimension::GetRentItemInfo —— T 0x0826f842（同名报告全文转写）---------
// [b28d-18 N82] p167 CLEAR（11/11）；残差=槽位/寄存器分配 | ORIG 0x0826f842
// [b28d-solo-GlobalData] DIFF 封顶（二压重开）：残差=N82 槽位/寄存器分配。
// [b28d-PoolGlobalData 二压] 0x0826f918.. 切片逐条重对齐（探针 pol_probe4 g4/g5）：
// ①size() 结果具名落槽（ORIG sz0@-0x14 / sz1@-0xc，各调一次）；②下标**不具名**
// （ORIG 每用点 `mov 0x10(%ebp); movzbl (%eax)` 重读）；③两段检查=js 正跳 ret 岛 +
// `movsbl; cmp sz; jl pass` 真跳 pass、ret 块 fall-through ⇒ goto 形「ret 标签置于
// 第二检查与 pass 之间」（4.1 布局，4.4 同形无中继 nop）；④inner 具名 @-0x10。
// [GDataWriter 2026-09-11] 重开：旧 goto 早返形的失败块每处多 1 条 `jmp <ret岛>` ⇒
// 117 条；改**正极性短路包裹**（`if (a >= 0 && a < sz0) {…}`，P214①/P59；ORIG
// 0x0826f93c `jl <pass>` + fall-through 0x0826f93e `mov $0x0,%eax; jmp <leave>`
// 为该形直证）⇒ **117→109 条**（ORIG 111，gap +6→+2）。
// 残余 = 4.4 对两个 `return false` 做 tail-merge（单点 `mov $0` 块 + 成功路径
// `jmp <leave>`）vs 4.1 每失败点就地 `mov $0,%eax; jmp <leave>` 且成功路径直落
// —— §6.4 负 gap（our < ORIG）+ ORIG 独有段已定位 ⇒ N 封顶（跨版本块合并差）。
#include "GlobalData/GetRentItemInfo_1.inc"
    CUser& user, const InstanceRentalSystem::RentInElem& inElem,
    EventRewardItemInfo& out)
{
    int selectionIdx = 0;
    if (!pc_room::GetSelectionIndex(user, selectionIdx, 2))
    {
        return false;
    }
    char job = 0;
    if (!pc_room::GetCharacterJob(user, job))
    {
        return false;
    }

    const int packageKey =
        makeFairPVPEquipmentPackageKey((int)(signed char)job, selectionIdx);

    // [b28d-PoolGlobalData] 二压：ORIG 0x0826f8ba 无 `list=0` 初始化死槽写、
    // 无 growType 槽（0x0826f8d0 `movsbl %al,%edx` 直落实参位）、else 支第二实参
    // 为**常量 2**（0x0826f8f2 `movl $0x2,0x4(%esp)` 直证，非重读 m_type）、
    // m_type 比较为字节形（0x0826f8c1 `cmp $0x1,%al`）。
    std::vector<std::vector<EventRewardItemInfo> >* list;
    if (inElem.m_type == 1)
    {
        list = (std::vector<std::vector<EventRewardItemInfo> >*)
            GetRewardItemGiveList((int)(signed char)job,
                                  (int)(signed char)user.getCurCharFirstGrowType());
    }
    else
    {
        // ORIG 0x0826f900：pc_room::GetRewardItemSelectionList(packageKey, 2)。
        list = pc_room::GetRewardItemSelectionList(packageKey, 2);
    }
    if (list == 0)
    {
        return false;
    }

    const int sz0 = (int)list->size();
    // [GDataWriter 2026-09-11 重开] ORIG 0x0826f926/0x0826f939 段：
    // `movzbl (%eax); test %al,%al; js <ret0 岛>` + `movsbl; cmp -0x14(%ebp),%eax;
    // jl <pass>` + fall-through 到 0x0826f93e `mov $0x0,%eax; jmp end` ⇒ 原码是
    // **正极性短路包裹**（P214①/P59：负极性 goto 早返每处多 1 条 `jmp <ret 岛>`，
    // 我方旧形实测 117 条 vs ORIG 111）。改为 `if (a >= 0 && a < sz) { … }`，
    // 失败路径共享函数尾 return false。
    if ((int)(signed char)inElem.m_sectionIdx >= 0 &&
        (int)(signed char)inElem.m_sectionIdx < sz0)
    {
        std::vector<EventRewardItemInfo>* inner =
            &list->at((unsigned int)(int)(signed char)inElem.m_sectionIdx);
        const int sz1 = (int)inner->size();
        if ((int)(signed char)inElem.m_itemIdx >= 0 &&
            (int)(signed char)inElem.m_itemIdx < sz1)
        {
            out = (*inner)[(unsigned int)(int)(signed char)inElem.m_itemIdx];
            return true;
        }
    }
    return false;
}

// ---- PcRoom::checkAllRentalItemScript —— T 0x082708cc（同名报告全文转写）---
// [b28d-18 N82] 残差=槽位/STL（N82/N89） | ORIG 0x082708cc
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（50/50）；残差=N82 槽位/分支布局 + N89。
// [SweepGlobalData] itemId 值拷贝 → 对象引用（ORIG 0x082709b6 `call operator*;
// mov %eax,-0x14`）后每使用点复现重解引用；残余 = 4.4 寄存器中转（N18a/P211）。
#include "GlobalData/checkAllRentalItemScript.inc"
{
    // ORIG 0x082708cc 段（p167 实证）：外层 map 的 begin()/end() 各自独立重调
    // G_CDataManager()（P157/P48）；内两层为 const_iterator 遍历
    // （begin/end/operator*/operator++/operator!=），非 size()/operator[] 下标形
    // （P175 反例）；`(*it).second` 代 `it->second`（P167 deEv 非 ptEv）。
    for (std::map<int,
                  std::vector<std::vector<EventRewardItemInfo> > >::iterator
             outer = G_CDataManager()->m_pcBangRentar.m_mapC.begin();
         outer != G_CDataManager()->m_pcBangRentar.m_mapC.end(); ++outer)
    {
        const std::vector<std::vector<EventRewardItemInfo> >& sections =
            (*outer).second;
        for (std::vector<std::vector<EventRewardItemInfo> >::const_iterator
                 sit = sections.begin();
             sit != sections.end(); ++sit)
        {
            const std::vector<EventRewardItemInfo>& items = *sit;
            for (std::vector<EventRewardItemInfo>::const_iterator
                     iit = items.begin();
                 iit != items.end(); ++iit)
            {
                // ORIG 0x082709b6 `call operator*; mov %eax,-0x14` ⇒ 原码是引用
                // 绑定 `const EventRewardItemInfo& info = *iit;`（-0x14 存对象
                // 地址），6 个使用点各自重解引用（SPAN O=8/U=7 实证）。
                const EventRewardItemInfo& info = *iit;
                CItem* item = G_CDataManager()->find_item((int)info.m_field0);
                if (item == 0)
                {
                    std::cerr << "[RentalSystem::Ready()] Can`t find item["
                              << (unsigned)info.m_field0 << "]" << std::endl;
                    return false;
                }
                if (item->IsImpossibleAmplify() != 1)     // vt+0x34 槽13
                {
                    std::cerr << "[RentalSystem::Ready()] Impossible contents("
                                 "amplify upgrade) is not checked. ["
                              << (unsigned)info.m_field0 << "]" << std::endl;
                    return false;
                }
                if (item->IsImpossibleCharacCargo() != 1)  // vt+0x40 槽16
                {
                    std::cerr << "[RentalSystem::Ready()] Impossible contents("
                                 "charac cargo) is not checked. ["
                              << (unsigned)info.m_field0 << "]" << std::endl;
                    return false;
                }
                if (item->IsImpossibleDisjoint() != 1)     // vt+0x38 槽14
                {
                    std::cerr << "[RentalSystem::Ready()] Impossible contents("
                                 "disjoint) is not checked. ["
                              << (unsigned)info.m_field0 << "]" << std::endl;
                    return false;
                }
                if (item->IsImpossibleGift() != 1)         // vt+0x3c 槽15
                {
                    std::cerr << "[RentalSystem::Ready()] Impossible contents("
                                 "gift) is not checked. ["
                              << (unsigned)info.m_field0 << "]" << std::endl;
                    return false;
                }
                if (item->IsImpossibleUpgrade() != 1)      // vt+0x30 槽12
                {
                    std::cerr << "[RentalSystem::Ready()] Impossible contents("
                                 "upgrade) is not checked. ["
                              << (unsigned)info.m_field0 << "]" << std::endl;
                    return false;
                }
                if (item->GetAttachType() != 2)
                {
                    std::cerr << "[RentalSystem::Ready()] AtttachType is not "
                                 "trade delete. ["
                              << (unsigned)info.m_field0 << "]" << std::endl;
                    return false;
                }
                m_impl->m_set.insert((unsigned)info.m_field0);
            }
        }
    }
    return true;
}

// ---- Dimension::checkSelectionListScript —— T 0x0826f14e -------------------
// （报告全文转写；五谓词次序同 PcRoom，cerr 前缀为 RentalDimensionSystem）
// [b28d-18 N82] 残差=槽位/STL 展开（N82/N89） | ORIG 0x0826f14e
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（43/43）；已按 P248 档① 改 bool 返回；残差=N82/N89。
// [SweepGlobalData] 同 PcRoom：itemId 改对象引用（ORIG 0x0826f232 同形）。
// [G7-sub 2026-09-11 判词] RentalMachineDimension::checkSelectionListScript
// （ORIG T 0x0826f14e）：本轮**已改**。① 0x0826f41d..0x0826f42f：`call 080f12e2
// <CItem::GetAttachType() const>` 后 ORIG 是 `mov %eax,-0xc(%ebp); cmpl $0x2,-0xc(%ebp)`
// （槽往返 + 内存-立即数比较，P0 有槽 ⇒ 原码有具名 int 局部）；我方旧写内联比较使 4.4
// 发 `cmp; setne %al; test %al,%al`（多 1 条且分支形态异）⇒ 改为具名 `attachType`。
// 实测 256→255 条（ORIG 253，gap −3→−2）。
// 残留：0x0826f480 段 ORIG 把 `&info.m_field0` **直接**作 `set<unsigned>::insert(unsigned const&)`
// 实参（`mov -0x14(%ebp),%ebx; mov %ebx,0x8(%esp)`，无临时），我方 `(unsigned)` 值 cast
// 生成 -0x1c 临时槽 + `lea` ⇒ 多 2 条。根因是共享头 `ServerParameterScript.h
// EventRewardItemInfo::m_field0` 现声明为 **int**（ORIG 该实参是 unsigned 左值）；
// 共享头类型变更属前轮已登记待 Main 裁决项（本泳道按 §8.3 不擅自改共享头）⇒ 保留。
#include "GlobalData/checkSelectionListScript.inc"
    const std::vector<std::vector<EventRewardItemInfo> >& selectionList)
{
    // ORIG 0x0826f14e 段（p167 MISSING 迭代器族）：两层均为 const_iterator
    // 遍历（begin/end/operator*/operator++/operator!=），非下标形（P175 反例）。
    for (std::vector<std::vector<EventRewardItemInfo> >::const_iterator
             sit = selectionList.begin();
         sit != selectionList.end(); ++sit)
    {
        const std::vector<EventRewardItemInfo>& items = *sit;
        for (std::vector<EventRewardItemInfo>::const_iterator iit = items.begin();
             iit != items.end(); ++iit)
        {
            // ORIG 0x0826f232 同形：`call operator*; mov %eax,-0x14` 引用绑定。
            const EventRewardItemInfo& info = *iit;
            CItem* item = G_CDataManager()->find_item((int)info.m_field0);
            if (item == 0)
            {
                std::cerr << "[RentalDimensionSystem::Ready()] Can`t find item["
                          << (unsigned)info.m_field0 << "]" << std::endl;
                return 0;
            }
            if (item->IsImpossibleAmplify() != 1)
            {
                std::cerr << "[RentalDimensionSystem::Ready()] Impossible "
                             "contents(amplify upgrade) is not checked. ["
                          << (unsigned)info.m_field0 << "]" << std::endl;
                return 0;
            }
            if (item->IsImpossibleCharacCargo() != 1)
            {
                std::cerr << "[RentalDimensionSystem::Ready()] Impossible "
                             "contents(charac cargo) is not checked. ["
                          << (unsigned)info.m_field0 << "]" << std::endl;
                return 0;
            }
            if (item->IsImpossibleDisjoint() != 1)
            {
                std::cerr << "[RentalDimensionSystem::Ready()] Impossible "
                             "contents(disjoint) is not checked. ["
                          << (unsigned)info.m_field0 << "]" << std::endl;
                return 0;
            }
            if (item->IsImpossibleGift() != 1)
            {
                std::cerr << "[RentalDimensionSystem::Ready()] Impossible "
                             "contents(gift) is not checked. ["
                          << (unsigned)info.m_field0 << "]" << std::endl;
                return 0;
            }
            if (item->IsImpossibleUpgrade() != 1)
            {
                std::cerr << "[RentalDimensionSystem::Ready()] Impossible "
                             "contents(upgrade) is not checked. ["
                          << (unsigned)info.m_field0 << "]" << std::endl;
                return 0;
            }
            // [G7-sub] ORIG 0x0826f41d..0x0826f42f：`call 080f12e2
            // <CItem::GetAttachType() const>(int 返回)` 后 **`mov %eax,-0xc(%ebp);
            // cmpl $0x2,-0xc(%ebp)`**（槽往返 + 内存-立即数比较）⇒ 原码有具名 int
            // 局部（P0 槽-局部双向对应律）；我方旧写内联 `GetAttachType() != 2` 使
            // 4.4 发 `cmp $0x2,%eax; setne %al; test %al,%al`（多 2 条且分支形态异）。
            int attachType = item->GetAttachType();
            if (attachType != 2)
            {
                std::cerr << "[RentalDimensionSystem::Ready()] AtttachType is "
                             "not trade delete. ["
                          << (unsigned)info.m_field0 << "]" << std::endl;
                return 0;
            }
            m_impl->m_set.insert((unsigned)info.m_field0);
        }
    }
    return 1;
}

// ---- Dimension::checkAllRentalItemScript —— T 0x0826f022（同名报告转写）----
// [b28d-18 N82] 残差=槽位/分支布局（N82） | ORIG 0x0826f022
// [G7-sub 2026-09-11 判词] RentalMachineDimension::checkAllRentalItemScript
// （ORIG T 0x0826f022）：私有 .o 实测 ORIG **86 / 我方 86（gap 0）**，逐条规范化后
// **零形差**（仅 call 目标文本：报告侧 Ghidra 标签 vs 我方真实重定位名）。
// 注：本报告早前登记的 282/286 属**同名 PcRoom 版**（0x082708cc，在
// checkAllRentalItemScript.inc，非本件）；本件（Dimension 版）实测全等。
// 结论：**封顶，无改动**。
#include "GlobalData/checkAllRentalItemScript_1.inc"
{
    // ORIG 0x0826f028/0x0826f086/0x0826f0b9/0x0826f114：两循环的 begin()/end()
    // 各自独立重调 G_CDataManager()（P157 逐字段重读 + P48 无 CSE，禁 dm 缓存）；
    // `(*it).second` 代 `it->second`（P167：ORIG 发 deEv 非 ptEv）。
    for (std::map<int,
                  std::vector<std::vector<EventRewardItemInfo> > >::iterator
             it = G_CDataManager()
                      ->m_dimensionAct.m_rewardData.m_mapC.begin();
         it != G_CDataManager()
                   ->m_dimensionAct.m_rewardData.m_mapC.end(); ++it)
    {
        // [b28d-solo-GlobalData] P0：ORIG 0x0826f05x `add $0x4,%eax; mov %eax,-0x10;
        // mov -0x10,%eax` 槽往返 ⇒ 原码有具名引用局部再传参（直接传表达式少 2 条）。
        const std::vector<std::vector<EventRewardItemInfo> >& sel = (*it).second;
        if (!checkSelectionListScript(sel))
        {
            return false;
        }
    }
    for (std::map<std::pair<int, int>,
                  std::vector<std::vector<EventRewardItemInfo> > >::iterator
             it = G_CDataManager()
                      ->m_dimensionAct.m_rewardData.m_map24.begin();
         it != G_CDataManager()
                   ->m_dimensionAct.m_rewardData.m_map24.end(); ++it)
    {
        const std::vector<std::vector<EventRewardItemInfo> >& sel = (*it).second;
        if (!checkSelectionListScript(sel))
        {
            return false;
        }
    }
    return true;
}

// ---- PcRoom::CheckupAllRentalItems —— T 0x08270d1e（同名报告全文转写）------
// [b28d-18 N7] 残差=callee-saved 角色轮换 + 单段 regmove（N7/N18a） | ORIG 0x08270d1e
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（18/18）；残差=N7/N82 寄存器角色与槽位。
#include "GlobalData/CheckupAllRentalItems.inc"
    CUser& user, const InstanceRentalSystem::RentInput& input)
{
    int inventoryNeed = 0;
    // ORIG 0x08270d30：与 Dimension::CheckupAllRentalItems 同构 —— begin() 隐藏
    // 返回指针迭代器 @-0x1c、循环条件后置（end()+operator!=）、体内 operator*。
    for (std::vector<InstanceRentalSystem::RentInput::RentInElem>::const_iterator
             it = input.m_elems.begin();
         it != input.m_elems.end(); ++it)
    {
        const InstanceRentalSystem::RentInElem& elem = *it;
        EventRewardItemInfo info;
        if (!pc_room::GetRentItemInfo(user, (char)elem.m_sectionIdx,
                                      (char)elem.m_itemIdx, info, 1))
        {
            return 1;
        }
        CItem* script = G_CDataManager()->find_item(info.m_field0);
        if (script == 0)
        {
            return 1;
        }

        Inven_Item probe;
        probe.m_addInfo = info.m_field0;
        script->make_item(probe);                    // ORIG 虚表槽 +0x08

        if (elem.m_bEquip != 0)
        {
            if (user.getCurCharacInvenR()->tryInsertItemIntoEquipment(probe) < 0)
            {
                return 0xcb;
            }
        }
        else
        {
            // ORIG 0x08270f2a/0x08270f4d：`shr $0x1f,%eax; test %al,%al; je <++>`
            // —— 负值走 return，非负 fall 到 `addl $0x1` ⇒ 源码写 `< 0` 早退，
            // 不写 `>= 0 {…} else {…}`（后者 4.4 会多出 `not %eax` 反相）。
            if (user.getCurCharacInvenR()->tryInsertItemIntoInventory(probe) < 0)
            {
                return 0xcb;
            }
            ++inventoryNeed;
        }
    }
    // 同 Dimension 版：ORIG 尾 if-return 形（P47/P191）。
    if (!pc_room::CheckInventoryEmptyCount(user, inventoryNeed))
    {
        return 0xcb;
    }
    return 0;
}

// ---- Dimension::CheckupAllRentalItems —— T 0x0826f52c（同名报告全文转写）---
// [b28d-18 N7] 残差=callee-saved 角色轮换 + 单段 regmove（N7/N18a） | ORIG 0x0826f52c
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（18/18）；残差=N7/N82 寄存器角色与槽位。
#include "GlobalData/CheckupAllRentalItems_1.inc"
    CUser& user, const InstanceRentalSystem::RentInput& input)
{
    int inventoryNeed = 0;
    // ORIG 0x0826f53e：`begin()`（隐藏返回指针迭代器对象 @-0x1c）→ `jmp` 到底部
    // `end()`+`operator!=` 判定 → 体内 `operator*` 取元素指针存 @-0x10 ⇒
    // 源码为 const_iterator 三段 for 循环（非 size()/operator[] 下标循环）。
    for (std::vector<InstanceRentalSystem::RentInput::RentInElem>::const_iterator
             it = input.m_elems.begin();
         it != input.m_elems.end(); ++it)
    {
        const InstanceRentalSystem::RentInElem& elem = *it;
        EventRewardItemInfo info;
        if (!GetRentItemInfo(user, elem, info))
        {
            return 1;
        }
        CItem* script = G_CDataManager()->find_item(info.m_field0);
        if (script == 0)
        {
            return 1;
        }

        Inven_Item probe;
        probe.m_addInfo = info.m_field0;
        script->make_item(probe);

        if (elem.m_bEquip != 0)
        {
            if (user.getCurCharacInvenR()->tryInsertItemIntoEquipment(probe) < 0)
            {
                return 0xcb;
            }
        }
        else
        {
            // ORIG 0x0826f6fd：`shr $0x1f,%eax; test %al,%al; je <addl $0x1>`
            // ⇒ `< 0` 早退 + fall-through 自增（见 PcRoom 同名函数注释）。
            if (user.getCurCharacInvenR()->tryInsertItemIntoInventory(probe) < 0)
            {
                return 0xcb;
            }
            ++inventoryNeed;
        }
    }
    // ORIG 尾 `call; xor $1; test %al,%al; je <mov $0 岛>; mov $0xcb; jmp`
    // ⇒ if-return 形（P47/P191：三目与 if-return 块排布镜像，此处需 if 形）。
    if (!pc_room::CheckInventoryEquipmentEmptyCount(user, inventoryNeed))
    {
        return 0xcb;
    }
    return 0;
}

// ---- PcRoom::Rent —— T 0x082704e6（Rent.md 全文转写）------------------------
// [b28d-solo-GlobalData] 归零 169=169：①真语义 bug——首守卫 ORIG 为
// `!IsPcRoomUser && !m_field11`（两条 je 同落继续块，旧写 `|| m_field11==1` 逻辑相反）；
// ②RentInput::m_field11 uchar→bool（P248 档②）+ 嵌套 if（P133①）；
// ③m_bEquip 取反+两臂交换（P47/P222）| ORIG 0x082704e6
#include "GlobalData/Rent_1.inc"
                              const InstanceRentalSystem::RentInput& input,
                              InstanceRentalSystem::RentOutput& output)
{
    // ORIG 0x082704f4..0x08270513（真语义 bug 修正）：`call IsPcRoomUser; xor $1;
    // test; je <继续>` + 内层 `movzbl 0x11; xor $1; test; je <继续>`；两条 je 同落
    // 继续块 ⇒ 原码是 `if (!IsPcRoomUser(user) && !input.m_field11) return 7;`
    // （旧写 `|| m_field11 == 1` 逻辑相反）。
    // P133①/P225 推广：ORIG 两条守卫各自 `je <继续>`（非汇聚物化）⇒ 嵌套 if
    // （外层 !A、内层 !B → return 7），4.4 才逐条复现 `xor $1; test; je`。
    if (!pc_room::IsPcRoomUser(user))
    {
        if (!input.m_field11)
        {
            return 7;   // ENUM_ERROR 7
        }
    }
    if (!pc_room::CheckRentableEnvironment(user))
    {
        return 7;   // ENUM_ERROR 7
    }
    int err = CheckupAllRentalItems(user, input);
    if (err != 0)
    {
        return err;
    }

    pc_room::DeleteAllRentalItemInSameSectionForNewRent(user, output.m_deleted,
                                                        1);
    // ORIG 0x082704e6：const_iterator 三段循环（p167 MISSING 实证）。
    for (std::vector<InstanceRentalSystem::RentInput::RentInElem>::const_iterator
             it = input.m_elems.begin();
         it != input.m_elems.end(); ++it)
    {
        const InstanceRentalSystem::RentInElem& elem = *it;
        EventRewardItemInfo info;
        if (!pc_room::GetRentItemInfo(user, (char)elem.m_sectionIdx,
                                      (char)elem.m_itemIdx, info, 1))
        {
            LogManager::logFormat(
                1, "RentalSystem.cpp",
                "virtual ENUM_ERROR RentalMachinePcRoom::Rent(CUser&, const "
                "InstanceRentalSystem::RentInput&, "
                "InstanceRentalSystem::RentOutput&)",
                0xf8,
                "Rent error. GetRentItemInfo return false. "
                "scriptSectionIndex(%d), scriptSeqIndex(%d)",
                (int)(signed char)elem.m_sectionIdx,
                (int)(signed char)elem.m_itemIdx);
            return 1;   // ENUM_ERROR 1
        }
        // ORIG `test %al,%al; je <ProcessInsertItem>`（equip 直落）⇒ 条件取反 +
        // 两臂交换复现 4.1 else-first 布局（P47/P222，同 Dimension::Rent）。
        if (elem.m_bEquip != 0)
        {
            pc_room::ProcessEquipItem(user, elem, info, output, 0x3d);
        }
        else
        {
            pc_room::ProcessInsertItem(user, elem, info, output, 0x3d);
        }
        if (input.m_logFlag != 0)
        {
            log(user, info.m_field0);
        }
    }
    return 0;   // 0=成功
}

// ---- Dimension::Rent —— T 0x0826ec22（Rent.md 全文转写）---------------------
// [b28d-solo-GlobalData] 归零 176=176：m_bEquip 分支条件取反+两臂交换复现
// ORIG else-first 布局（`test;je <ProcessInsertItem>`，P47/P222）；p167 CLEAR 21/21 | ORIG 0x0826ec22
#include "GlobalData/Rent_2.inc"
                                 const InstanceRentalSystem::RentInput& input,
                                 InstanceRentalSystem::RentOutput& output)
{
    if (!pc_room::CheckRentableEnvironmentAtChannelType(user))
    {
        return 7;   // ENUM_ERROR 7
    }
    if (!checkRentalLevel(user.get_charac_level()))
    {
        return 14;  // ENUM_ERROR 0xe（14）
    }
    if (!checkRentalGrowJob((int)(signed char)user.getCurCharFirstGrowType()))
    {
        return 254;   // ENUM_ERROR 0xfe（254）
    }
    int err = CheckupAllRentalItems(user, input);
    if (err != 0)
    {
        return err;
    }

    pc_room::DeleteAllRentalItemInSameSectionForNewRent(user, output.m_deleted,
                                                        2);
    // ORIG 0x0826f3xx：const_iterator 三段循环（begin/end/ne/deEv/ppEv，
    // p167 MISSING 实证），非 size()/operator[] 下标形。
    for (std::vector<InstanceRentalSystem::RentInput::RentInElem>::const_iterator
             it = input.m_elems.begin();
         it != input.m_elems.end(); ++it)
    {
        const InstanceRentalSystem::RentInElem& elem = *it;
        EventRewardItemInfo info;
        if (!GetRentItemInfo(user, elem, info))
        {
            LogManager::logFormat(
                1, "RentalMachineDimension.cpp",
                "virtual ENUM_ERROR RentalMachineDimension::Rent(CUser&, const "
                "InstanceRentalSystem::RentInput&, "
                "InstanceRentalSystem::RentOutput&)",
                0x43,
                "Rent error. GetDimensionRentItemInfo return false. "
                "scriptSectionIndex(%d), scriptSeqIndex(%d)",
                (int)(signed char)elem.m_sectionIdx,
                (int)(signed char)elem.m_itemIdx);
            return 1;   // ENUM_ERROR 1
        }
        // ORIG 0x0826ed9f `test %al,%al; je <ProcessInsertItem>`（equip 直落）
        // ⇒ 条件取反 + 两臂交换复现 4.1 的 else-first 块布局（P47/P222 族）。
        if (elem.m_bEquip != 0)
        {
            pc_room::ProcessEquipItem(user, elem, info, output, 0x3e);
        }
        else
        {
            pc_room::ProcessInsertItem(user, elem, info, output, 0x3e);
        }
        if (input.m_logFlag != 0)
        {
            log(user, info.m_field0);
        }
    }
    return 0;   // 0=成功
}

// ORIG _ZN20InstanceRentalSystem17CheckRentableItemEij T 0x082702dc【objdump】：
// machine = getMachine(type)；machine==0 → 返 1（视为可用）；否则尾调虚槽
// vptr+0x10 CheckRentableItem(itemIdx)。（OnlyOrig_CreatureJob.EJEnchanter 批补）
// [b28 判词 N7] 残差=call *%edx↔%eax 换名+伴随寄存对调，主体同形封顶。
#include "GlobalData/CheckRentableItem_2.inc"
{
    IRentalMachine* machine = getMachine(type);
    if (machine == 0)
        return true;
    return machine->CheckRentableItem(itemIdx);
}

// ORIG _Z18get_seed_from_datev T 0x086b1d25：year*365 + mon*30 + mday
// （ORIG 以 mon*32-mon 形态实现 *30；反汇编 0x086b1d53 读 tm_mon(+0x10)）。
#include "GlobalData/get_seed_from_date.inc"
{
    // ORIG 0x086b1dxx：v 槽先声明(基址 -0x40)、t 次之(-0x14)、localtime_r
    // 返回值落指针槽 pv(-0x10)，字段经 pv-> 间接读（0x086b1d53 `mov
    // -0x10(%ebp),%eax; mov 0x14(%eax),%eax`）。
    // ORIG 0x086b1d7x：return 表达式结果先落临时槽 -0xc(%ebp) 再回读（4.1
    // dead store 具名化）。
    struct tm v;
    time_t t = GlobalData::s_systemTime_.getCurSec();
    struct tm* pv = localtime_r(&t, &v);
    int seed = pv->tm_year * 365 + pv->tm_mon * 30 + pv->tm_mday;
    return seed;
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
