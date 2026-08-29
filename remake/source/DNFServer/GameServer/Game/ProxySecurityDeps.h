#ifndef GAME_PROXYSECURITYDEPS_H_
#define GAME_PROXYSECURITYDEPS_H_

// ProxySecurityDeps —— DisPatcher_SetIP 域安全依赖（报告缺失，按 ORIG objdump
// 全文转写；地址/mangled 实证）：
//   T _Z11checkDenyIPPKcS0_        @0x081f44bb sz 0x1d0
//   T _Z11cutIPdclassPKc           @0x081f440a sz 0x0b1（sret，返回 std::string）
//   T _Z18getSha256UsingSaltPKhiPc @0x0808cc6a sz 0x18c

#include <string>

// d-class 截断：s.rfind('.') 后 erase(pos,npos) —— "a.b.c.d" → "a.b.c"
std::string cutIPdclass(const char* ip);

// IP 拒绝判定（SetIP dispatch_sig 调用）：
bool checkDenyIP(const char* innerIp, const char* peerIp);

// mac 串 SHA256(SHA("nhs" 盐)) 十六进制写出（0x20 字节摘要 → %02X×32）。
bool getSha256UsingSalt(const unsigned char* src, int len, char* out);

#endif  // GAME_PROXYSECURITYDEPS_H_
