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
void AcceptThread::dispatch(void* arg)
