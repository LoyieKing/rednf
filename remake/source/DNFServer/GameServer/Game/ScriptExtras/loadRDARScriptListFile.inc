// 定义于 STScriptFileList.cpp（ORIG T 0x088bbd77）。
// [L10-2] P292/档①：ORIG 调用点 0x088bffdc `xor $0x1,%eax; test %al,%al; je`
// ⇒ 该 callee 实为 bool（int 声明会产 `test%eax; sete%al`）。返回类型不入
// mangled/布局；定义宿主 STScriptFileList.cpp 应同步（已上报 Main）。
bool loadRDARScriptListFile(STScriptFileList* list, const char* dir,
