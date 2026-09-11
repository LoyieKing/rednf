// ORIG 0x081afa98 (sz 0x230)：非阻塞连接 + select 等待 + SO_ERROR 校验。
// [b28d-D43] P75 实参装载时刻 + 帧差（155/154）（ORIG 0x081afa98）。
// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N3/N76/N78/N97）→ 试=四 hunk 逐条复核 +
//   前人探针复核（/tmp/probe/argo.cpp A1..A5）：①len `mov -0x2c(%ebp),%ecx` 装载时刻
//   （0x081afada 段）= 4.1 纯右→左 vs 4.4 恒 arg2(lea)/arg1/arg3 固定序 ②0x081afb04
//   `mov; shr $0x1f; test %al,%al; je`：result<0 / (unsigned)result>>31 / (result>>31)
//   三形实测均折 `cmpl $0; jns`（N76）③0x081afc03 `mov $1,%eax; jmp; mov $0,%eax; test`
//   物化：`&&` 裸形 / static_cast<bool> / static_cast<int>!=0 三形实测均短路直跳（N3）
//   ④getsockopt 五实参右→左（A4/A5 同序实证）⇒ 维持 N3/N76/N78/N97（155 vs 148，gap +7）。
bool TCPSocket::connect_nonb(const char* host, unsigned short port,
