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
void MonitorTcpThread::dispatch(void* arg)
