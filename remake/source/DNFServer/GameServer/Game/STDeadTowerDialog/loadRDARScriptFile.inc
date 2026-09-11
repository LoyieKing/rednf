// 全局 C++ 权威签名（DNFLexWrapperHelpers.cpp 提供，匹配 ORIG mangled 符号）。
// 不 include DNFLexWrapper.h：该头经 DNFLex.h 引入 <iostream>，会给本 TU 带上
// std::ios_base::Init（__ioinit）动态初始化与 `_GLOBAL__I_` 键；ORIG 本 TU 无 GI
// （orig_tu_names.tsv 513 条无 STDeadTowerDialog.cpp）。签名与 DNFLexWrapper.h 一致。
bool loadRDARScriptFile(const char* directory, const char* path);
