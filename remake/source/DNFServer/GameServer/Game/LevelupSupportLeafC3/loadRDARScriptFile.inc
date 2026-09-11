// RDAR 脚本扫描 helper（真实符号，定义在 DNFLexWrapperHelpers.cpp / CDataManager.cpp；
// 此处仅原型。不 include DNFLexWrapper.h：其 DNFLex.h→<iostream> 会引入
// std::ios_base::Init 静态初始化块，ORIG 本域 TU 无该块——经 TU 对比实证）。
bool loadRDARScriptFile(const char* directory, const char* path);   // T 0x088bbd29
