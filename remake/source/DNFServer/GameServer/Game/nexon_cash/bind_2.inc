// ORIG 0x081aee82：同上但 INADDR_ANY；失败按 errno 分支：
//   EADDRINUSE(98)→printf("Port %d for receiving UDP is in use\n",port)
//                 （@0x08bb4a28），
//   EACCES(99)→puts("Cannot assign requested address")（@0x08bb4a50，**无句点**），
//   其他非零→printf("Could not bind UDP receive port. Error= %d , strerror = "
//                 "%s\n"…)（@0x08bb4a70）；errno==0 不打印。
//   [Q8 2026-09-11 旧N判词重扫 §6.5 串修复] 本函数两处串与 ORIG 逐字节不符已改：
//   ①`"Cannot assign requested address."` → 去尾句点（ORIG bytes @file-off
//   11979344 = `Cannot assign requested address\0`，objdump 直读）；
//   ②`"… , strerror=%s\n"` → `"… , strerror = %s\n"`（ORIG @11979410 含等号两侧空格）。
// 失败返回 false；成功且 nonBlock→setOptNonBlock()，true。
bool UDPSocket::bind(unsigned short port, bool nonBlock)
