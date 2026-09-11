// DNFLexWrapper.h 在本 TU 只提供 loadRDARScriptFile/ScanType/ScanInt/ScanStr 四个
// 函数声明，但其 include 链 DNFLex.h:16 `#include <iostream>` 会带出
// std::ios_base::Init __ioinit 与 `_GLOBAL__I_<本 TU 首强符号>`（ORIG 本 TU 无 GI，
// P327/P338；属「仅 NEW」符号）⇒ 改用局部权威声明断链。签名由 ORIG mangled 实证：
// _Z18loadRDARScriptFilePKcS0_ / _Z8ScanTypeRSsb / _Z7ScanStrPSs / _Z7ScanIntPb；
// 返回类型由调用点判据定（P292/P168a：`xor $1;test;je` ⇒ bool；ScanInt 单字节/32 位
// 两处使用 ⇒ int）。
bool loadRDARScriptFile(const char* path, const char* fileName);
