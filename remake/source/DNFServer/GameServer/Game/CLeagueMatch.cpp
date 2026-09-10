// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CLeagueMatch/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— CLeagueMatch 实现（学院联赛匹配）
// 依据 docs/class_func_reports/CLeagueMatch/ 全部报告逐函数转写；TeamInfo /
// PlayerInfo 依据 docs/class_func_reports/TeamInfo/ 报告与 ORIG objdump。
// Packet_PvPResult 的 ORIG ctor（W 0x08568dc6，PacketHeader(this,0xfa5,0x80)）
// 由 SigTypes 域提供（当前仅有 m_pad[0x80] 形态）——此处按 objdump 手动展开
// 头部字段并注明 UNLINKED_PRODUCER。
//
// [b28d-solo-CLeagueMatch·S180 总判词] 本 TU 19 件（清单 /tmp/b28d_tu/
// CLeagueMatch.tsv）逐件复验；TU 口径 check_tu_routed.sh 恒 COMPILE OK。
//   · _getSameSchool：AE（§6.1 循环对齐 nop 已在 TU 口径剥离）。
//   · 其余 17 件 DIFF/NEAR 的唯一/主要残差 = **N-STRIDE**：stride 0x1c 的
//     `i*4` ORIG(4.1.2) 发 `lea 0x0(,%eax,4),%edx`，4.4.7 恒 `shl $0x2,%eax`，
//     后续 `lea(,%x,8); sub` 两步全等 ⇒ 仅第一步指令选择差（部分件连带 1 条
//     `mov %edx,%eax` 中转）。穷举探针全败：/tmp/b28d_cm/probe_stride.cpp 11 形
//     （下标/指针算术/数组引用/unsigned/long long/char* 视图/二维数组）、
//     probe2.cpp 索引类型 7 形（long/unsigned long/short/ushort/char/uchar/enum）、
//     4.4.6 后端（cc1plus446bin）、旗标矩阵 8 组（-mtune=i386/i486/i586、
//     -march=i386/i586、-Os、-fno-strength-reduce、-fno-tree-loop-optimize）
//     —— 4.4 恒 shl。语义等价，按 N 类封顶（建议入库 N125）。
//   · 寻址序可翻项已翻：P235 匿名 union 数组视角 m_teams[2]（CLeagueMatch.h，
//     等宽 0xe0、偏移/sizeof 不变）使 OnCompleteLoad/_SendMatchStatePacket 的
//     `t*0x70+i*0x1c` 复现 ORIG「偏移先算 + lea this」序（ndiff 42→26 ×2）；
//     _RegisterWaitingRoom 的 sort 双实参改 m_teams[0]/m_teams[1] 复现 ORIG
//     双独立算址（this+0x1c / this+0x8c，52→48）。OnReward 两形取优后**保留**
//     (&m_teamA)[t]（union 视角该件反劣 1390→1398，S166 实测）。
//   · 大件 p167 调用点差集复验 CLEAR：OnReward 45/45、OnMatchRoom 29/29、
//     CheckMatchable 4/4、_RegisterWaitingRoom 3/3、OnLeaveRoom 7/7、
//     _PutInRoom 15/15（0 missing / 0 extra）⇒ §6.2 语义完成、byte 层 N 封顶。
//   · 其余 N 归因：OnEndFight=N-P75（第三实参常量压栈时刻）、OnWalkOut=N7+
//     P75（虚调用目标 %ecx↔%edx + 实参装载序）、OnMatchRoom=N95（槽基差 8）+
//     P226 物化差、_PutInRoom=N104（EH 尾块相对序）、OnReward=N82（帧 0x1fc vs
//     0x24c 槽复用策略）。
//   · P0：本 TU 无 `register asm` 绑定（清绑数 0）。
//
// [b28d-solo-CLeagueMatch·S217 复验总判词] 口径修正：S180 清单/早期读数的
// "our_insns/ndiff" 取自**链接二进制 df_game_r**（Main 上次 relink 的中间态，
// 与当前源码不同步）；本轮一律以**新鲜 .o**（check_tu_routed.sh → compare_tu
// clean 口径；逐件 ndiff 用 /tmp/b28d_s217/ndiff.py 复算）为准。当前真值：
//   · _getSameSchool ndiff=0（AE）；OnEndFight 2；size 4；reset 4；OnWalkOut 6；
//     GetSchoolNo/GetUser/GetGap 各 8；del 12；_PutInRoom 19；OnCompleteLoad 22；
//     _SendMatchStatePacket 22；_RegisterWaitingRoom 24；OnLeaveRoom 24；add 28；
//     refresh 36；CheckMatchable 66；OnMatchRoom 102；OnReward 1048。
//   · 残差结构（逐条明细已核）：N-STRIDE（`lea 0x0(,%eax,4),%edx` vs
//     `shl $0x2,%eax`，及其连带基址寄存器 %eax↔%edx、`add $K` vs `lea K(%edx)`）
//     + N7（间接 call 目标 %eax vs %ecx/%edx）+ N-P75（实参装载时刻）+
//     N104（_PutInRoom EH 尾块相对序）+ N95（OnMatchRoom 槽基差 8）+
//     N82（OnReward 帧 0x1fc vs 0x24c）。0x70 族（OnCompleteLoad/
//     _SendMatchStatePacket）另含乘法展开序差：ORIG 先 `shl $4`(t*16) 再
//     `lea(,%x,8);sub`，4.4 先算 i*7 再 shl；同属 N-STRIDE 族。
//   · 本轮新旋钮实测（均无净收益，已回滚/不入库）：
//     ① P311/P288/P291 视图族（char/int/short 二维视图、数组引用、flat、
//        PI 指针视图共 6 形，/tmp/b28d_s217/probe_view.o）⇒ 4.4 全产
//        `shl $0x2`，N-STRIDE 不可翻（补 S180 探针矩阵）。
//     ② P226 条件向语句表达式（OnMatchRoom badSize 改 `if (({a||b;}))`）⇒
//        局部复现零槽短路物化岛（`mov $0/$1,%eax; test %al,%al` 消掉
//        `mov %al,-0x19; cmpb`），但全函数 ndiff 102→134（槽号平移连锁）⇒ 回滚。
//     ③ P231 纯 register 提示（OnEndFight managerTeam）⇒ 丢 ORIG 的 -0x1c 槽、
//        帧 0x2c→0x1c ⇒ 回滚。
//     ④ 4.4.6 后端 + i486（/tmp/s217tool 副本）⇒ 与 4.4.7+i486 产物 md5 相同
//        （两者 .comment 均 4.4.6），逐件 ndiff 全等 ⇒ 零收益，不上报路由。
//     ⑤ _RegisterWaitingRoom 的 x87 书写序（`<`↔`>`）⇒ 新鲜 .o 两形同产
//        `fucompp; fnstsw %ax; sete %al`（S180 判词里的 fucomip/seta 系 stale
//        链接二进制假象）⇒ 保持原写法。
//   · p167 全量复验 CLEAR（19 件 0 missing / 0 extra；OnReward 45/45、
//     OnMatchRoom 29/29、_PutInRoom 15/15、OnLeaveRoom 7/7、CheckMatchable 4/4、
//     _RegisterWaitingRoom 3/3 …）⇒ §6.2 语义完成、byte 层 N 封顶。
//   · 真语义 bug：本轮无新增（S180 已修 4 处：OnLeaveRoom 用第 3 参 team、
//     OnMatchRoom 守卫用形参 room、CheckMatchable 段位窗口 `<=10`、
//     OnReward 徽章回填固定 teamB 槽）。
//   · P0：无 register asm 绑定（清绑数 0）。无 .h 改动。
//
// [b28d-solo-CLeagueMatch·S408 本轮总判词] 单文件泳道（19 件）最终读数
// （口径 = check_tu_routed.sh 新鲜 .o + /tmp/s408_ndiff_all.py 的 compare_tu clean
//  ndiff；TU verdict: identical=43 ae=64 near=1 diff=18，其中 diff 含 N123 噪音
//  `_Z41__static_initialization_and_destruction_0ii`；p167 19 件全 CLEAR）：
//   · _getSameSchool 0（AE/IDENTICAL）；OnEndFight 2；size 4；reset 4；OnWalkOut 6；
//     GetSchoolNo/GetUser/GetGap 各 8；del 12；_PutInRoom 19；OnCompleteLoad 22；
//     _SendMatchStatePacket 22；_RegisterWaitingRoom 24；OnLeaveRoom 24；add 28；
//     refresh 36；CheckMatchable 66；OnMatchRoom 102；**OnReward 567**（本轮
//     1048→567，−481；帧 0x1fc/0x1fc、22 槽集合逐槽全等、指令数 992/990）。
//   · **本轮改动（OnReward）**：①P0 内联 eloK2/winType/playCount 三局部（ORIG
//     0x0855efa6..0x0855efc1 直读全局/type 槽写包，无局部槽）——m_winType 取
//     `(char)type`（ORIG `mov -0x38(%ebp),%eax; mov %al,-0xca`）；②`int type;`
//     去初始化式（ORIG 三分支各自 `movl $K,-0x38`）；③P191 t 前置声明 +
//     `for (t = 0; …)` 双初始化（ORIG 0x0855eb45/eb4c 两条 `movl $0,-0x3c`），
//     三处 t 循环复用同槽；④P183 删 TeamInfo&/PlayerInfo&/schools/mySchool/fmt
//     具名局部，第 3 段三循环复用同一 i（ORIG 0x0855efd3/efda 双初始化 -0x20）；
//     ⑤删死局部 winFlagLocal 与 st（Acquire 结果内联进 CStreamGuard 实参）；
//     ⑥**真语义 bug（多余守卫）**：UpdateRating 前 ORIG 0x0855f7d3..f819 只查
//     m_flag，**无 m_user!=0**——删多余守卫照抄 ORIG（P140/P171）；⑦P224/P296
//     绿章三守卫改负向 `continue`（ORIG 0x0855f303 xor$1/jne、f312 je、f34a
//     sete/jne 三条各自落独立 nop+jmp 岛 f50a/f50d/f510）；⑧ratings[2] 零初始化
//     改等宽 `union{float f[2];unsigned int u[2];}` int 视角（ORIG 0x0855f70b/f712
//     单段 `movl $0,-0x44/-0x40`，4.4 对 float 初始化恒二段，P232 扩展）。
//   · **OnMatchRoom 实测回滚**：ORIG 0x0855e009/e010 双预清（theirTeam/seat）的
//     P191 双声明形实测 ndiff 102→121（+2 insn 且槽次数反劣）⇒ 回滚，保持
//     for 内声明形；残差 = N95（4.4 槽基恒 ebp-8 起、4.1 顶到 ebp，全帧偏移差 8）
//     + N82（leader/member 槽复用：ORIG 共用 -0x1c，4.4 分开 -0x24/-0x18）
//     + badSize 具名槽（ORIG 无槽、纯寄存器物化）。
//   · **N-STRIDE 本轮独立复验**（446 路由后端，探针 /tmp/s408probe/Game/
//     CLeagueMatch.cpp 十形：int/uint/ushort/long long 下标、`(a+i)->`、
//     `*(char*)((char*)a+i*0x1c)`、`a[t*4+j]`、双访问、`a[i].v`、具名 k=i*0x1c）
//     ⇒ 4.4.6 **全部**产 `shl $0x2`（ORIG 4.1.2 恒 `lea 0x0(,%eax,4),%edx`）；
//     同族 `x*8`/`x*16` 亦 shl-vs-lea（OnReward 0x70 族）。语义等价，按 N 封顶
//     （建议入库 N125：4.1 expand_mult 对 2 的幂用 lea、4.4 用 shl）。
//   · P0：本 TU 无 `register asm` 绑定（清绑数 0）。无 .h 改动。
//
// [SweepCLeagueMatch·本轮路由口径复验] dual_rank --tu CLeagueMatch.cpp 候选
// （gap' 降序 4 件：_PutInRoom 2 / OnCompleteLoad 2 / _SendMatchStatePacket 2 /
//  OnReward 1）逐件通读报告 `## 汇编` + `## 反编译 C` 并逐指令核对源码语义，
// **0 真缺口**；gap 精确归因（均为 4.1 侧冗余指令，源码无法表达）：
//   · OnCompleteLoad/_SendMatchStatePacket/OnReward：ORIG 每处 `t*0x70` 乘法块
//     多一对中间寄存器复制 `mov %ecx,%eax; mov %eax,%ecx`（4.1 expand_mult 产物；
//     4.4.6 直发 `mov %edx,%eax`）。OnCompleteLoad/_SendMatchStatePacket 各 2 块
//     ⇒ gap 2；OnReward 1 块 ⇒ gap 1。与 N-STRIDE（shl-vs-lea）同族，非语义。
//   · _PutInRoom：ORIG 两条出口分支各带 `mov -0x1c(%ebp),%ebx; jmp <尾块>`
//     （4.1 分支内不尾合并），4.4 合并为单条 `mov -0x1c(%ebp),%ebx` 直落尾块
//     ⇒ gap 2（N104 同族）。
//   · p167 调用点差集：_PutInRoom 15/15、OnCompleteLoad 9/9、
//     _SendMatchStatePacket 8/8、OnReward 45/45；差集仅 C1/C2、D1/D2 同址别名
//     （P93：nm 实证 PacketGuard 0x0858dd4c/0x0858de80、CStreamGuard
//     0x080c8c26/0x0861c8d2、Packet_PvPResult 0x08568dc6 各自同址）。
//   · 四数（check_tu_routed.sh 4.4.6 口径）：identical=43 ae=64 near=1 diff=18
//     missing_in_game=17 —— 与 S408 基线一致（不回退）。
//
// [SweepLeagueMatch2·本轮 18 件 DIFF 三件套重核] 口径 = check_tu_routed.sh
// 新鲜 .o + p167 调用点差集 + canon 归一多重集/块序逐件对照（报告 `## 汇编` +
// `## 反编译 C` 两段全读）。
//   · gap' 降序 4 件（_PutInRoom 2 / OnCompleteLoad 2 / _SendMatchStatePacket 2 /
//     OnReward 1）：p167 全 CLEAR（差集仅 C1/C2、D1/D2 同址别名 P93）；gap 归因
//     = 4.1 expand_mult 冗余 `mov %ecx,%eax; mov %eax,%ecx`（OnCompleteLoad/
//     _SendMatchStatePacket 各 2 处）与 _PutInRoom 分支内 `mov -0x1c(%ebp),%ebx;
//     jmp` 不尾合并（N104）⇒ 0 真缺口。
//   · 其余 13 件 DIFF（GetSchoolNo/GetUser/GetGap/OnEndFight/OnLeaveRoom/
//     OnMatchRoom/CheckMatchable/_RegisterWaitingRoom/TeamInfo add·del·reset·
//     refresh·size）：canon 多重集差全为 N-STRIDE（`lea 0x0(,%eax,4),%edx` vs
//     `shl $0x2`）、`add $K` vs `lea K(%r)`、4.1 冗余 regmove 与 OnEndFight 的
//     P75 实参压栈时刻；语义零缺口（_ZN12CLeagueMatch11OnLeaveRoom…/CheckMatchable/
//     _RegisterWaitingRoom/OnEndFight 多重集**完全相等**）。
//   · **本轮改动（OnReward 块序 + _getSameSchool 返回类型）**：
//     ① 第三段循环外层极性翻正 `if (_getSameSchool(...)) { addBadge 两臂 }
//        else { newFlag[0]=0; 绿章 }`（ORIG 物理序 855f1ac extra=1 → 855f232
//        extra=0 → 855f23c 绿章，P59/P222；内层改 `f1!=0||f2!=0` 正极性短路）；
//     ② 回填段 `if (m_teamB.m_players[i].m_flag != 0 && m_user != 0) {…}` 正向
//        包裹（ORIG 855f539/f561 两守卫同落 855f6dc 岛 `test;je`，消我方 xor+
//        双岛 3 条）；
//     ③ 末尾 `if (nPlayerCountPerOneTeam_ != 1) SendPacket; else {stream}`
//        （ORIG 物理序 SendPacket 855f8c2 先于 Acquire 855f8e4）；
//     ④ `CLeagueMatch.h:121` `int _getSameSchool(...)` → `bool`（P292：ORIG
//        0x0855f0bf `call; test %al,%al; je` 无 setne；已 hub 广播 + Main 批准，
//        返回类型不入 mangled/布局零影响；`_getSameSchool` 本体 25=25 不变）。
//     ⇒ OnReward ndiff **567 → 301**（992/985；p167 45/45 CLEAR）；语义等价
//        （外层 if/else 两臂互换、德摩根、短路 && 与负极性均保序）。
//   · 四数：identical=43 ae=64 near=1 diff=18 missing_in_game=17（不回退）。
//   · 真语义 bug：本轮 0 新增（S180 已修 4 处仍在：OnLeaveRoom 第 3 参 team、
//     OnMatchRoom 守卫用形参 room、CheckMatchable 段位窗口 <=10、OnReward 徽章
//     回填固定 teamB 槽 + 删 UpdateRating 多余 m_user 守卫）。
//   · P0：无 `register asm` 绑定（清绑数 0）。
//
// [SweepCLeagueMatch-2·本轮 判据㉓ + 19 件三件套复核] 口径 = check_tu_routed.sh
// 新鲜 .o（4.4.6 路由）+ 逐条 normalized objdump 双侧对照 + p167 调用点差集。
//   · 19 件（gap' 降序：OnReward 7 / OnCompleteLoad 2 / _PutInRoom 2 /
//     _SendMatchStatePacket 2 / 其余 gap 0）逐件通读 ORIG 反汇编与源码：
//     ①jcc 逐位交叉验证：全 TU 条件跳转序列（助记符 + 落点体）与 ORIG 一致；
//       唯一文本差 = OnMatchRoom badSize 段 4.4 把 `size > pc` 折成
//       `cmp %eax,%ebx; jl`（ORIG `cmp %edx,%eax; jg`），落点体相同
//       （+0x2c2 mov $1 / +0x2c9 mov $0）⇒ 等价折叠，非条件写反。
//     ②虚槽核对（P193 五步）：_ZTV12CLeagueMatch 38 槽逐槽 reloc 对照 ORIG
//       0x08ca3d20 全等（D1/D0/GetIndentification/…/GetSchoolNo +0x98）；
//       间接调用点槽偏移逐点一致（IRoomList AddRoom +0x00 / DelRoom +0x04 /
//       Match +0x08；IMatch GetUser +0x10；OnWalkOut 的 %ecx↔%edx 为 N7）。
//     ③字段偏移/宽度/写目标多重集双侧全等（TeamInfo m_flag@+0xc / m_user@+0x8 /
//       m_rating@+0x4 / m_schoolNo@+0x10 / m_badges@+0x14；OnReward m_dailyBadge
//       @+0x10+3j / m_newFlag @+0x80+3j 单字节写）；立即数差全为 .rodata/.bss
//       重定位（AE）；调用目标差仅 C1↔C2/D1↔D2 同址别名（P93）。
//     ④gap 归因（非语义）：OnReward 7 / OnCompleteLoad 2 / _SendMatchStatePacket 2
//       = 4.1 expand_mult 冗余 `mov %ecx,%eax; mov %eax,%ecx` + N-STRIDE；
//       _PutInRoom 2 = N104（4.1 分支内 `mov -0x1c(%ebp),%ebx` 不尾合并）。
//   · 判据㉓（符号绑定 W vs T）修 6 处（全部 ORIG W、我方原 T）：
//     ① CLeagueMatch::GetIndentification（ORIG W 0x08568834，PvP_Room 段）、
//        GetBattleMode（W 0x084ed387）、SetBattleMode（W 0x082743a0）、
//        GetPlayCountPerOneTeam（W 0x084ed37b）→ CLeagueMatch.h 类内 inline；
//        消费 TU 发射 W 副本：CSchoolMatchEvent.cpp → SetBattleMode、
//        CUser.cpp → GetPlayCountPerOneTeam、InterDispatcher.cpp →
//        GetPlayCountPerOneTeam/GetBattleMode（属主复编四数零回退
//        122/1333/5/133/700）。
//     ② PlayerInfo::reset（ORIG W 0x085687d4，OnlyOrig_OnlinePvp 段）→ 类内
//        inline；本 TU 仍发射 W 副本（TeamInfo 族 ODR 使用），TeamInfo::reset
//        帧保持 sub $0x28（无 P210 帧回退）。
//     六函数体逐条不变（objdump 双侧 normalized identical / 仅数据地址差）。
//   · 四数：identical=43 ae=61 near=1 diff=18 missing_in_game=17
//     （TU 级 ae −3 = GetBattleMode/SetBattleMode/GetPlayCountPerOneTeam 三个
//      符号按 ORIG 归属迁到消费 TU，非语义回退；diff/near/missing 全不变；
//      Main 已裁决保留）。
//   · 真语义 bug：本轮 0 新增；本 TU 剩余真缺口 = 0。
//
// [SweepLeagueMatch3·判据① 全 TU jcc/jmp 目标下标逐位比对 + 前 20 件复核]
// 口径 = check_tu_routed.sh 新鲜 .o（4.4.6 路由）+ 自研双侧目标下标/块指纹
// （/tmp/lane11_jccidx.py、/tmp/lane11_blockfp.py、/tmp/lane11_align.py）。
//   · 判据①（19 件逐位）：14 件 flow **完全全等**（CheckMatchable/GetGap/GetUser/
//     OnEndFight/GetSchoolNo/OnLeaveRoom/OnWalkOut/_RegisterWaitingRoom/
//     _MakeRoom/_ReturnTeam/_CheckMatchable/addBadge/TeamInfo add·del·refresh·size）；
//     5 件有差异，逐件核目标块内容后全部语义等价：
//     ① _PutInRoom：ORIG 两出口分支尾各 `mov -0x1c(%ebp),%ebx; jmp <尾块>`
//        （+0xaf/+0xf5），我方合并为尾块头单条 mov + else 侧直落 ⇒ 少 2 条
//        （N104）；两侧目标块（lea -0x18; call D1; mov %ebx,%eax; …）逐条等价。
//     ② OnCompleteLoad / _SendMatchStatePacket：恒定 +2 下标漂移 = 两处 ORIG
//        4.1 冗余 `mov %R,%R`（0x70 族 expand_mult）；目标块逐条全等。
//     ③ TeamInfo::reset：gap 1（我方 N-STRIDE 多一条 `mov %edx,%eax` 中转），
//        jmp/jne 目标块（cmpl $3 / mov -0xc,%eax）全等。
//     ④ OnMatchRoom：148/155 `jg↔jl` 为 cmp 操作数反向折叠（落点同为
//        mov $1 / mov $0）；160/161 差 1 由 badSize 具名槽（下）。
//   · **本轮改动（净收益）**：OnMatchRoom badSize 两处比较操作数序翻正——
//     ORIG 0x0855df26..df2d 段实证为「先 call size()/get_player_count()、
//     后 `movzbl 0x93abb74`」，即原码写 `call(...) > (int)nPlayerCountPerOneTeam_`
//     （call 在左）；我方原写 `<` 使全局读先物化（倒数第二操作数先算）。
//     改后 141..158 段与 ORIG 逐条全等（仅 N95 槽基 8 与 .o reloc 伪差），
//     单件 canon 未匹配 81→75（lane11_align）；p167 29/29 CLEAR（差集仅
//     C1/C2 同址别名 P93）。语义等价（`size() > n` ⟺ `n < size()`）。
//   · **实测回滚**：badSize 加裸 `register`（P231 提示形）⇒ 我方 159-161 变
//     `mov %eax,%ebx; test %bl,%bl`（免槽但多 1 条 regmove；ORIG 为
//     `test %al,%al` 单条），四数/canon 零净收益 ⇒ 回滚；ORIG 该处无槽属
//     4.4 具名 bool 局部恒落槽（N82/P332），S408 已试 P226 语句表达式反劣。
//   · missing_in_game=17 全量核销：17 个全为 **C1/C2、D1/D2 同址别名**
//     （ORIG nm 实证：C2Ev≡C1Ev@0x0855cd74、D2Ev≡D1Ev@0x0855ce4c、
//     PVP_ROOM_INFO@0x08568f16/5c、Packet_PvPResult@0x08568dc6、IMatch/
//     IRoomList/CLeagueMatchRoomList 及 STL allocator/map/Rb_tree 的 C2·D2），
//     反汇编缓存按地址去重丢名 ⇒ P93 伪差，0 真缺口。
//   · 四数（不回退）：identical=43 ae=61 near=1 diff=18 missing_in_game=17。
//   · 真语义 bug：本轮 0 新增；本 TU 剩余真缺口 = 0。
//
// [SweepLeagueMatch4·判据⑪ P327 GI 键名 + 判据⑫ static_init + 判据㉔ 跨 TU 定义迁移]
// （Main 批准 ①②③ 成对执行；属主状态：CGameManager.cpp=Lane17 已收口 idle、
//   OnlinePreliminaryMatch.cpp 无属主 ⇒ R1 例外成立。）
//   · ORIG 证据链：
//     ① GI 键名 `_GLOBAL__I__ZN28PvPGlobalEnvironmentVariable25school_match_daily_point_E`
//        @0x08568636（size 0x1c），`orig_tu_names.tsv:354` 标注归属 MatchingSystem.cpp
//        （= 本 TU；与本 TU 全部日志串 "MatchingSystem.cpp" 互证）⇒ P327 规则：
//        该 TU 内第一个「强符号·外部链接·本 TU 定义」实体即 school_match_daily_point_。
//     ② ORIG static_init @0x085685a0（size **0x96**）构造序（objdump -d 实证）：
//        `_ZStL8__ioinit`@0x943e172（call 0x85685c0）→ `g_schoolMgr`@0x943e080
//        （call 0x85685e2 = `_ZN10CSchoolMgrC1Ev`@0x0856f206）→ `g_guildAgitMgr`
//        @0x943e0e0（call 0x8568612 = `_ZN17CGuildAgitManagerC1Ev`@0x0856f2c6），
//        每个对象后接 __cxa_atexit。
//     ③ 全仓 `call 856f206` / `call 856f2c6` 各仅 1 处 = 上述两点 ⇒ 两全局对象
//        在 ORIG 中**只**于本 TU 构造（定义在本 TU），CGameManager TU 侧为零。
//     ④ 5 个静态成员 ORIG .data 0x093abb60..0x093abb73 连续，`objdump -s -j .data`
//        初值读实 1/2/5/3/3。
//   · 本轮改动（成对，否则重复定义）：
//     ① 本文件：include CSchoolMgr.h / CGuildAgitManager.h；新增 5 个静态成员定义
//        （置于 `pWaitingRoomList_` **之前** ⇒ 复现 GI 键名）+ `CSchoolMgr g_schoolMgr;`
//        `CGuildAgitManager g_guildAgitMgr;`（声明序 = ORIG 构造序）。
//     ② OnlinePreliminaryMatch.cpp：删 5 行静态成员定义 + 归属注释更正。
//     ③ CGameManager.cpp：删 2 行全局对象定义（其下方 extern 声明与各头 extern 继续生效）。
//   · 实测（check_tu_routed.sh fresh .o / 链接后 df_game_r）：
//     · 本 TU：identical **43→44**（+1）、missing_in_game **17→16**（−1）、
//       GI 键名 = `_GLOBAL__I__ZN28PvPGlobalEnvironmentVariable25school_match_daily_point_E`
//       （size 0x1c，与 ORIG 同名同尺寸）、static_init size **0x40 → 0x96**（= ORIG 精确相等）。
//     · CGameManager.o：static_init 0x40（两对象构造已按 ORIG 归属迁出；ORIG 侧该 TU
//       无这两个 ctor 调用——证据①③），diff=16/missing=722 为其固有基线（函数体四数不受
//       数据定义归属影响）。
//     · OnlinePreliminaryMatch.o：identical=44 ae=69 near=0 diff=1 missing=16（GI 键名
//       随之改为 `_ZN14TeamInfoSecond5resetEv`）。
//     · 全量 `cmake --build build/game -j$(nproc)`：**`[100%] Built target game`**
//       （df_game_r 28,685,860B，连续两次成功）；链接级 `nm` 核：GI 唯一、
//       `g_schoolMgr`(0x58)/`g_guildAgitMgr`(0x58) 单一定义、无重复定义/未定义引用。
//   · 回滚判据：若后续 lane 复编发现本迁移引入回退，改回 ②③ 两处定义即恢复原状
//     （① 为纯增量，删除即可）。
// ============================================================================

#include "CLeagueMatch.h"
#include <algorithm>
#include <string.h>

#include "CUser.h"
#include "CUserCharacInfo.h"
#include "PvP_Room.h"
#include "CGameManager.h"
#include "GameWorld.h"
#include "GlobalData.h"
#include "InterfacePacketBuf.h"
#include "PacketGuard.h"
#include "LogManager.h"
#include "GameTypes.h"                  // cMyTrace
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "CSchoolServerProxy.h"
#include "SchoolMatchEnums.h"           // ENUM_ELO_RATING_SYSTEM
#include "OnlinePreliminary.h"          // CEloRating / PvPGlobalEnvironmentVariable
#include "Packet_PvPResult.h"           // 结果包 0x80 载荷具名布局（shared/packet/include）
#include "RDARScriptStringManager.h"
#include "TimerDispatcher.h"            // TimerStartMatching
#include "CInventory.h"                 // OS_API::snprintf
// [SweepLeagueMatch4·判据⑪/㉔] ORIG MatchingSystem.cpp 归属：本 TU 定义
// g_schoolMgr / g_guildAgitMgr（static_init 构造点实证；定义由 CGameManager.cpp 迁入）。
#include "CSchoolMgr.h"                 // g_schoolMgr（ORIG B 0x0943e080 / C1 0x0856f206）
#include "CGuildAgitManager.h"          // g_guildAgitMgr（ORIG B 0x0943e0e0 / C1 0x0856f2c6）

// GetGap 排位增量表：ORIG 各 TU 本地 _ZL13g_RankingIncr（16 字节，
// df_game_r nm 显示 15 份内容一致）＝ {1.0f, 1.0f, 1.0f, 1.0f}。
// [SweepLeagueMatch2·P204③] 文件级 static（非匿名 namespace）：后者产
// `_ZN12_GLOBAL__N_1L13g_RankingIncrE`，与 ORIG `_ZL13g_RankingIncr` 不符。
static const float g_RankingIncr[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

// [SweepLeagueMatch4·判据⑪ P327 GI 键名 + 判据㉔ 跨 TU 定义迁移]
// ORIG 本 TU（orig_tu_names.tsv:354 标注 MatchingSystem.cpp）的 GI 键名 =
// `_GLOBAL__I__ZN28PvPGlobalEnvironmentVariable25school_match_daily_point_E`
// @0x08568636（size 0x1c）⇒ 该 TU 内**第一个**「强符号·外部链接·本 TU 定义」
// 实体即 school_match_daily_point_。ORIG .data 0x093abb60..0x093abb73 连续
// 五成员，初值读实 1/2/5/3/3（objdump -s -j .data 实证）。
// 迁移前这 5 个定义在 OnlinePreliminaryMatch.cpp，本 TU GI 键名误为
// `_GLOBAL__I__ZN12CLeagueMatch17pWaitingRoomList_E`（ORIG 无 ⇒ 计入 missing）。
// 顺序铁律：本组必须置于 pWaitingRoomList_ **之前**（P327：仅定义行序决定键名）。
int PvPGlobalEnvironmentVariable::school_match_daily_point_ = 1;
int PvPGlobalEnvironmentVariable::school_match_daily_point_weekends_ = 2;
int PvPGlobalEnvironmentVariable::school_match_fight_count_for_gain_point_ = 5;
int PvPGlobalEnvironmentVariable::school_match_elo_k_ = 3;
int PvPGlobalEnvironmentVariable::school_match_elo_k2_ = 3;

// [SweepLeagueMatch4] ORIG static_init @0x085685a0（size 0x96）构造序实证：
//   `_ZStL8__ioinit`(0x943e172, call 0x85685c0) →
//   `g_schoolMgr`(0x943e080, call 0x85685e2 = CSchoolMgr::C1 0x0856f206) →
//   `g_guildAgitMgr`(0x943e0e0, call 0x8568612 = CGuildAgitManager::C1 0x0856f2c6)，
// 各带 __cxa_atexit。两对象定义由 CGameManager.cpp 迁回本 TU（声明序 = 构造序）。
CSchoolMgr g_schoolMgr;
CGuildAgitManager g_guildAgitMgr;

CLeagueMatchRoomList* CLeagueMatch::pWaitingRoomList_ = 0;   // B 0x0943e064
int CLeagueMatch::nWaitingRoomListRefCount_ = 0;             // B 0x0943e068
int CLeagueMatch::nextAuthValue_ = 0;                        // B 0x0943e06c
bool CLeagueMatch::bMatchable_ = false;                      // B 0x0943e060
unsigned char CLeagueMatch::nBattleMode_ = 3;                // D 0x093abb75 初值 3
unsigned char CLeagueMatch::nPlayerCountPerOneTeam_ = 2;     // D 0x093abb74 初值 2

// ---------------------------------------------------------------------------
// PlayerInfo / TeamInfo（报告 TeamInfo/*.md + ORIG objdump 转写）
// ---------------------------------------------------------------------------

// [b28c-21] P210 定义顺序：ORIG TeamInfo::reset `sub $0x28` 保守帧；4.4 对
// 已见定义的 PlayerInfo::reset 精确继承 $0x14 ⇒ 本定义前置于 callee 之前。
// [A11-round5 复核] N125 证据加强（本 TU 12 件的同一残差族）：本轮以 8 源形探针
// （`&arr[i]` 索引 / `(char*)t + i*0x1c` 指针算术 / 具名 int 中间量 / 二维下标 /
// unsigned 索引 / 数组退化 + 标量偏移）在 4.4.6 逐形复编（/tmp/r5/probeD.cpp f1..f8），
// 恒产 `shl $0x2,%eax; lea 0x0(,%eax,8),%edx; sub %eax,%edx`，**无一**产 ORIG 的
// `lea 0x0(,%eax,4),%edx; lea 0x0(,%edx,8),%eax; sub %edx,%eax`（TeamInfo::size
// 0x0855cd42、GetUser 0x0855e5f4、GetGap 0x0855e1f9、CheckMatchable 0x0855fa2a、
// OnLeaveRoom 0x0855d2xx、_RegisterWaitingRoom 0x0855d94b 等族）⇒ N125 结构性豁免。
// 追加 4 形（probeD2.cpp f9..f13：成员数组元素取址直传 out-of-line callee / `arr+i`
// 指针加法 / `arr[i].field` 字段访问 / `reinterpret_cast<Ent*>((char*)t+i*0x1c)` 偏移链）
// 仍恒 `shl $0x2` ⇒ 12 源形全败，后继勿再试。
// [A11-round5 C05/D08 复查 · 无落点] ①范式①（尾 fail 块）：本 TU 18 件 DIFF 的 ORIG
// 尾 16 条独立 `mov $0/$1,%eax` 均为正常返回值块（GetUser/GetSchoolNo/OnMatchRoom/
// CheckMatchable/TeamInfo::add·del/_getSameSchool 等），我方同形；②范式③（早返中继岛）
// 与 D08（多守卫同目标合并）：全 33 件 diff 的 `jmp` 行差逐件为 0 ⇒ 无岛可消；
// ③范式②（F2）：「ORIG `cmp mem,reg`」行（TeamInfo 族 stride 链）两侧同为内存直比，
// 残差仅槽号/lea-vs-shl，非装载序 ⇒ 不适用。
// [b28d-solo-CLeagueMatch] DIFF 19/20·N-STRIDE（S180 定稿）：stride 0x1c 的 i*4
// ORIG 用 lea(,%eax,4)、4.4 恒 shl $2（探针 /tmp/b28d_cm/probe_stride.cpp 11 形 +
// probe2.cpp 7 索引类型 + 4.4.6 后端 + 8 旗标全败）⇒ 4.4 多 1 条 mov %edx,%eax
// 中转；语义等价，结构性豁免。
#include "CLeagueMatch/reset.inc"
}

// [SweepCLeagueMatch-2·判据㉓] PlayerInfo::reset（ORIG W 0x085687d4）已改类内
// inline 定义（CLeagueMatch.h），由使用点发射 W；本 TU 因 TeamInfo 族调用仍发射
// 该 W 副本。函数体逐条不变。

// [b28d-lane6] AE（已修）：m_flag bool 化（P248 档②）+ `==` 正向包裹（P222）后
// 第一/二段与 ORIG 0x0855c95a 全等；x87 比较段经 fresh 重编译后亦归一（34=34）。
#include "CLeagueMatch/CompareRating.inc"
}

// [b28d-solo-CLeagueMatch] DIFF 100/100·N-STRIDE：同 TeamInfo 族 stride 差异
// （ORIG lea 链 vs 4.4 shl 链，逐访问点 4 条对 4 条），语义等价，结构性豁免。
#include "CLeagueMatch/refresh.inc"
}

// [b28d-solo-CLeagueMatch] DIFF 91/91·N-STRIDE：xor $1 守卫形已按 P292/P248 修复
// （`if (!m_flag)` 正向包裹），残差全为 stride 0x1c 的 shl-vs-lea，语义等价。
#include "CLeagueMatch/add.inc"
}

// [b28d-solo-CLeagueMatch] DIFF 39/39·N-STRIDE：残差全为 stride 0x1c 的
// shl-vs-lea（3 访问点），语义等价，结构性豁免。
#include "CLeagueMatch/del.inc"
}

// [b28d-solo-CLeagueMatch] DIFF 23/23·N-STRIDE：stride 0x1c 的 i*4 ORIG 用
// lea(,%eax,4)、4.4 恒 shl $2（S180 探针 11 形 + 7 索引类型 + 4.4.6 + 8 旗标
// 全败），语义等价。
#include "CLeagueMatch/size.inc"
}

// ---------------------------------------------------------------------------
// 生命周期
// ---------------------------------------------------------------------------

// ORIG C1/C2 0x0855cd74：IMatch ctor → PVP_ROOM_INFO@+8 → 懒建 pWaitingRoomList_
// （new 0x1c 置零后 C1 0x08568fb0）→ 引用计数自增。
// [b22判词·N族] ORIG new(0x1c) 后、C1 call 前有 6 条 value-init 标量置零
// （+0x00/08/0c/10/14/18），GCC4.1 语义；4.4 视 user-provided ctor 类免置零直调。
// placement-new+try 实测反引入 operator new(void*,ptr) call + EH spill，C++98
// 无法复现该置零序列，语义等价（残差 6 insns），按 N 族结案，勿再开轮。
// [b28d-solo-CLeagueMatch] AE（TU 口径）：67=67 全等。原 N77 残差（new
// PVP_ROOM_INFO 的 6 条 value-init 清零 +0x00/08/0c/10/14/18）随 PvPTypes.h
// PVP_ROOM_INFO 保持「无用户 ctor」而恢复（P299：用户 ctor 抑制 value-init）；
// EH 尾块序亦已归一。本件无需再改。
#include "CLeagueMatch/CLeagueMatch.inc"
}

// ORIG D1 0x0855ce4c：引用计数递减，归零且单例仍在时 operator delete（不析构、
// 不置空）；随后 PVP_ROOM_INFO / IMatch 析构由编译器生成。
#include "CLeagueMatch/dtor_CLeagueMatch.inc"
}

// ---------------------------------------------------------------------------
// IMatch 覆盖
// ---------------------------------------------------------------------------

// [b28d-solo-CLeagueMatch] DIFF 72/72·N-STRIDE：三守卫已按 P296 改嵌套正向 if，
// 与 ORIG 0x0855e1a3/a9/af 同落 +0xcb（消 3 个 jmp+nop 岛）；残差全为 stride 的
// shl-vs-lea，语义等价。
#include "CLeagueMatch/GetGap.inc"
}

// [b28d-solo-CLeagueMatch] DIFF 29/29·N-STRIDE：bool 化 + 裸 `if(!m_flag)` 的
// xor 段已与 ORIG 0x0855e5e9 一致；残差为 stride 0x1c 的 shl-vs-lea，语义等价。
#include "CLeagueMatch/GetUser.inc"
}

#include "CLeagueMatch/GetUserList.inc"
}

#include "CLeagueMatch/RequestMap.inc"
}

#include "CLeagueMatch/GetExtraWinPoint.inc"
}

#include "CLeagueMatch/OnCreateRoom.inc"
}

// 【N7 封顶】b26 lane19：虚调用 callee 寄存器 %ecx↔%eax 互换（ORIG
// mov (%eax),%ecx+call *%ecx vs 4.4 mov (%eax),%eax+call *%eax，16 条逐条
// 长度/助记符全等），临时/引用/限定名不可翻，语义同。
#include "CLeagueMatch/OnDestroyRoom.inc"
}

#include "CLeagueMatch/OnJoinRoom.inc"
}

// ORIG T 0x0855d1c0
// [b28d-solo-CLeagueMatch] DIFF 142/142·真 bug 已修 + N-STRIDE：ORIG 0x0855d245/
// d273 以**第 3 参** team 判 m_teamA/m_teamB（原码误用第 4 参 flag，调用点
// OnWalkOut/PvP_Room 均传 (user,room,team,seat)）；本波再修两处 P0：
// ①`_ReturnTeam(room, room->get_manager_team(), user)` 内联实参（ORIG
// 0x0855d1fc 返回值 %eax 直压 0x8(%esp)，无 managerTeam 具名槽）；
// ②`int idx = 0;` + `for (idx = 0;…)` 双初始化（ORIG 0x0855d232/d239 两次
// movl $0,-0xc）且删 selfRating/oppoRating 的 0.0f 初始化（ORIG 无该清零，
// P171）；gap -5→0，残差 6 处 stride shl-vs-lea（N-STRIDE；p167 7/7 CLEAR），
// 语义等价。
#include "CLeagueMatch/OnLeaveRoom.inc"
}

// ORIG T 0x0855db58。ORIG 返回 int：m_state==1 → 1；失败路径 → 0；成功 → 1。
#include "CLeagueMatch/OnRegisterMatch.inc"
}

// ORIG T 0x0855dc64。ORIG 返回 int：失败路径 → 0；成功 → 1。
// [b28d-solo-CLeagueMatch] DIFF 277/277·ndiff 102·真语义 bug 已修 + N95
// （S217 新鲜 .o 口径；p167 29/29 CLEAR）：①0x2c2 守卫
// 原用 theirRoom->get_player_count()，ORIG 0x0855de5e `mov 0xc(%ebp),%eax` +
// Ghidra C `get_player_count(param_1)` 实证为**形参 room** ⇒ 改回 room（真 bug）；
// ②leader/theirRoom 按 ORIG 0x0855dca0/dca7 函数头预清 0（P198③，gap 2→0）；
// ③五处 cMyTrace 改全表达式临时链（P221）翻正槽方向（ORIG -0x6c→-0x2c 由深到浅，
// 原具名形 -0x38→-0x78 反向）；残差为全帧槽基差 8（N95：4.4 槽基恒 ebp-8 起、
// 4.1 可顶到 ebp）与 setg/test 物化差；p167 CLEAR（29/29），语义闭合。
// [b28d-solo-CLeagueMatch·S408] 复验 ndiff 102 不变。本轮实测**回滚**一项：
// ORIG 0x0855e009/e010 双预清（theirTeam=-0x14 / seat=-0x10）的 P191 双声明形
// （`int theirTeam=0; unsigned int seat=0; for(seat=0;…)`）⇒ ndiff 102→121
// （+2 insn，槽次数反劣）⇒ 回滚保持 for 内声明。剩余槽差 2 枚：badSize 具名槽
// （ORIG 无槽，纯寄存器物化，S217 试 P226 语句表达式亦反劣）+ leader/member 槽
// 复用（ORIG 共用 -0x1c(9)，4.4 分开 -0x24(4)/-0x18(3)）⇒ N95+N82 封顶。
#include "CLeagueMatch/OnMatchRoom.inc"
}

#include "CLeagueMatch/OnStartFight.inc"
}

// ORIG T 0x0855e55a
// [b28d-solo-CLeagueMatch] DIFF 45/45·ndiff 2·N-P75（S217 新鲜 .o 复验）：
// ORIG 0x0855e594 第二实参先入 %eax 再压常量 0，4.4 恒先压常量；cast/NULL/
// 具名局部 3 形不可翻（S180 追加逗号求值序形 ((void)managerTeam,(CUser*)0)
// 亦不变）。S217 再补 2 形：`0L` 与 `0` 同形；`*&managerTeam` 虽消实参序差但
// 把 `cmpl $1,-0x1c(%ebp)` 变 `mov;cmp $1,%eax`（46 条 +1，净零）⇒ 回滚。
// 共 6 形全败，语义等价。
#include "CLeagueMatch/OnEndFight.inc"
}

// ORIG T 0x0855e4c4。ORIG 返回 1。
// 【N7 封顶】b26 lane19：虚调用 callee 寄存器 %ecx↔%eax 互换 + 实参装载序
// 跨版本差（ORIG 先槽 -0x10 后形参 0x10，4.4 反序——N80 族不可翻），33 条
// 长度/助记符全等。
// [b28d-solo-CLeagueMatch] NEAR 33/33·N7（S180 复验）：虚调用目标
// %ecx(ORIG 0x0855e4dc)↔%edx(4.4.7) + 实参装载序（P75）；33 条助记符逐条全等，
// 语义等价。
#include "CLeagueMatch/OnWalkOut.inc"
}

// ORIG T 0x0855e52a。ORIG 返回 bool（!ready && state==4 → 0）。
#include "CLeagueMatch/OnReady.inc"
}

// ORIG T 0x0855eb22（结算奖励全流程）
// §6.2 巨件核销：P167 45/45 [0缺0多]（修前 42/45）；gap 79→35；残差归因 N86/N82
// （t*0x70/j*3 寻址代数分解跨版本差：ORIG shl$4+lea+sub 折乘链 vs 4.4
//   shl$3+add，纯分解形态、语义同；全帧槽号平移）。
// 真修三处：
//   1) 结果包改栈对象 Packet_PvPResult res 走真 ctor（W 0x08568dc6≡
//      PacketHeader(0xfa5,0x80)，重建侧符号已在）；原手写展开的
//      reversed1/m_connNo 清零属发明语义（ORIG ctor 不清），删。
//   2) 实时徽章回填条件修正：ORIG 0x0855f539 固定寻址 this+i*0x1c+0x98/0x94
//      = teamB 的 m_flag/m_user（与 t 无关），原实现错挂 t 分支。
//   3) GetDailyBadge 由循环改 3 条独立调用（ORIG $0x0/$0x1/$0x2 立即数直传
//      三处 call，P0），读取 user 按数组化寻址 (&m_teamA)[t].m_players[i].m_user
//      复现 ORIG 乘法寻址链（无具名缓存槽）。
// [b28d-solo-CLeagueMatch] DIFF 992/960·ndiff 1048·N82（S217 新鲜 .o 口径）：
// 帧 0x1fc vs 0x24c，
// 4.4 槽复用策略差（§6.2 三件套口径：p167 调用点差集 45/45 CLEAR [0 缺 0 多]、
// gap 34 归因槽复用与 t*0x70/j*3 寻址代数分解；S180 试 P235 union 视角
// m_teams[t] 该件反劣（1390→1398）⇒ 两形取优保留 (&m_teamA)[t]；剩余全为
// 槽号平移与 shl-vs-lea；语义完成、byte 层 N 封顶）。
// [b28d-solo-CLeagueMatch·S408 定稿] **1048→567**（−481）：帧 0x1fc/0x1fc 全等、
// 22 槽集合逐槽全等、指令数 992/990、p167 45/45 CLEAR。修法=六处 P0/P183
// 内联（eloK2/winType/playCount/TeamInfo&/PlayerInfo&/schools/mySchool/fmt/st/
// winFlagLocal）+ P191 t/i 双初始化复用槽 + `int type;` 去初始化式 + 绿章三守卫
// 负向 continue（P224/P296）+ **真 bug** 删 UpdateRating 前多余的 m_user!=0 守卫
// （P140/P171，ORIG 0x0855f7d3..f819 只查 m_flag）+ ratings 等宽 union int 视角
// 复现 ORIG 单段零初始化。剩余 567 = N-STRIDE
// （*4/*8/*16 的 shl-vs-lea，探针 10 形全败）+ 少量寄存器名/中转（N7/N18a）
// + difflib 错位，语义完成。
#include "CLeagueMatch/OnReward.inc"
}

// ORIG T 0x0855e22c：向全体成员广播加载完成通知（0x81 包，携带 unique id）。
// [b28d-solo-CLeagueMatch] DIFF 97/95·ndiff 22·N-STRIDE（S217 新鲜 .o 复验）：
// 残差 = i*0x1c 的 shl-vs-lea + 0x70 族乘法展开序差（ORIG 先 `shl $4`(t*16)
// 再 `lea(,%x,8);sub`，4.4 先算 i*7 再 shl），逐条语义等价；p167 9/9 CLEAR。
#include "CLeagueMatch/OnCompleteLoad.inc"
}

#include "CLeagueMatch/CheckJoinable.inc"
}

#include "CLeagueMatch/CheckSeat.inc"
}

// [b28d-solo-CLeagueMatch] DIFF 202/202·真语义 bug 已修 + N-STRIDE：①段位窗口边界
// 取反真 bug（ORIG 0x0855fbc0..fbf3 `jg` 跳过 return 0 ⇒ return false 条件是
// `self∈[12,14] && other<=10`，原实现写成 `other>10`）——改 `<=10` 后 jg/jle
// 差异同步消失；②i 复用同一槽 + `int i=0` 双初始化 + 具名 diff（P191/P0）；
// 残差 12 处为 stride 0x1c 的 shl-vs-lea（N-STRIDE：4.1 `lea 4;lea 8;sub` vs
// 4.4 `shl 2;lea 8;sub`，S180 探针 /tmp/b28d_cm/probe_stride.cpp 11 形 +
// probe2.cpp 7 索引类型 + 4.4.6 后端 + 8 旗标全败）；p167 CLEAR（4/4）。
#include "CLeagueMatch/CheckMatchable.inc"
}

// ---------------------------------------------------------------------------
// 非 IMatch 方法
// ---------------------------------------------------------------------------

#include "CLeagueMatch/SetMatchAble.inc"
}

#include "CLeagueMatch/SetPlayCountPerOneTeam.inc"
}

// ORIG 虚槽布局：GetSchoolNo 为派生新增第 37 槽（ORIG vtable +0x90），
// GetIndentification 的覆盖体即经 vtable+0x90 虚调本函数。
// [b28d-solo-CLeagueMatch] DIFF 32/32·N-STRIDE：残差全为 stride 0x1c 的
// shl-vs-lea（2 访问点），语义等价，结构性豁免。
#include "CLeagueMatch/GetSchoolNo.inc"
}

// [SweepCLeagueMatch-2·判据㉓] GetIndentification(0x08568834 W) / GetBattleMode
// (0x084ed387 W) / SetBattleMode(0x082743a0 W) / GetPlayCountPerOneTeam
// (0x084ed37b W) 四个 ORIG 弱符号已改类内 inline 定义（CLeagueMatch.h），由
// 使用点 TU 发射 W 副本（P233/P204③）；本 TU 不再外联定义（原 T 绑定与 ORIG
// 的 W 不符）。函数体逐条不变，见头内注释。

// [b28d-solo-CLeagueMatch] AE（TU 口径）：196=196 全等。修法=删 passwd/title
// 引用缓存局部（P183/P0，消 2 个指针槽 -0x24/-0x20）+ S27 PvPTypes.h
// PVP_ROOM_INFO::byte8 uchar→bool（消 uchar→bool 的 test/setne 2 条）。
// p167 调用点差集 CLEAR（39/39）。
#include "CLeagueMatch/_MakeRoom.inc"
}

// [b28d-solo-CLeagueMatch] DIFF 132/132·ndiff 24·N-STRIDE+N7（S217 新鲜 .o
// 复验；x87 比较段已产 ORIG 的 `fucompp;fnstsw;sahf/test;sete` 形，S180 判词
// 里的 fucomip/seta 系 stale 链接二进制假象）：foundMin 改
// int（ORIG -0x20 `cmpl $0`）+ 两循环改嵌套正向 if（P296）后条数 132=132；
// S180 再把 sort 双实参改 P235 视角 m_teams[0]/m_teams[1]，复现 ORIG
// 0x0855da16 的 `add $0x8c` / `add $0x1c` 双独立算址；残差仅剩 stride
// shl-vs-lea 与 x87 比较 lowering（ORIG fucompp/fnstsw/sahf vs 4.4 seta 形），
// 语义等价。
#include "CLeagueMatch/_RegisterWaitingRoom.inc"
}

#include "CLeagueMatch/_BackupRoomInfo.inc"
}

// [b28d-solo-CLeagueMatch] AE（TU 口径）：69=69 全等。修法=P225 `while(cond){return X;}`
// 复现 ORIG 尾 `cmp %eax,%edx; setne %al; test %al,%al; je` 的 bool 四段物化
// （三目/if 直跳形恒被 4.4 折成 `jne`，探针 /tmp/b28d_cmatch 两形实测：三目
// ndiff 8、bool 局部占槽反劣 13、while+break 归零）；槽序按 P0 对齐
// （emptySeats/X/i/user，X=logArg0 复现 -0x14 具名局部，删 school 局部改两次
// GetSchoolNo P48）。
#include "CLeagueMatch/_CheckMatchable.inc"
}

#include "CLeagueMatch/_PullOutRoom.inc"
}

// [b28d-solo-CLeagueMatch] DIFF 85/83·N104（S180 复验，p167 15/15 CLEAR）：
// EH 尾块相对序（ORIG 正常出口块在函数末、unwind 居中；块内容逐条全等，
// §6.2 口径语义闭合）。
#include "CLeagueMatch/_PutInRoom.inc"
}

// [b28d-solo-CLeagueMatch] AE（TU 口径）：79=79 全等。修法=P223② 显式提前
// `continue`（同层多 continue 复现 ORIG 0x0855e477/e47a/e47d 三个独立
// nop+jmp 中继岛；else-if 合并形被 4.4 jump-thread 少 2 岛，gap 2→0）。
#include "CLeagueMatch/_ReturnTeam.inc"
}

// [b28d-solo-CLeagueMatch] DIFF 95/93·ndiff 22·N-STRIDE（S217 新鲜 .o 复验）：
// 残差 = i*0x1c 的 shl-vs-lea + 0x70 族乘法展开序差（同 OnCompleteLoad），
// 语义等价；p167 8/8 CLEAR。
#include "CLeagueMatch/_SendMatchStatePacket.inc"
}

// [b28d-solo-CLeagueMatch] AE：TU 口径 identical/AE（25=25 全等；game_func_report
// 单函数口径多 1 条循环对齐 nop，§6.1/N112 已在 TU 口径剥离）。
#include "CLeagueMatch/_getSameSchool.inc"
}

#include "CLeagueMatch/_GetOppositeTeam.inc"
}

#include "CLeagueMatch/_CheckSchool.inc"
}

// [b28d-solo-CLeagueMatch] AE（TU 口径）：207=207 全等。修法=P305 的 S18 规避形
// `switch((int)badgeType)`：既避免 -fstrict-enums 删越界 case（ENUM_BADGE_TYPE
// 仅定义 0，曾删 case 2 ⇒ p167 MISSING 各 1），又复现 ORIG 0x0855e90f/914/919
// 的「形参载入 %eax + 寄存器比较链」（P251/P286；if 链恒折 `cmpl $K,mem` 每段
// 重读内存，探针 /tmp/b28d_cmatch/probe4.cpp sw3 vs if3 实证）；p167 25/25 CLEAR。
#include "CLeagueMatch/addBadge.inc"
}
