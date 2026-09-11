// [b28d-38b] 不 include DNFLexWrapper.h：其链上 DNFLex.h 含 <iostream>，会使本 TU
// 发射 ORIG 本 TU 无有的 _ZStL8__ioinit（SIAD 0x69 vs ORIG 0x40，P338 反例）。
// 改用局部权威签名声明（mangled 与 ORIG 一致，定义在 DNFLex TU）。
bool ScanType(std::string& token, bool consume);   // 0x088bc85f _Z8ScanTypeRSsb
