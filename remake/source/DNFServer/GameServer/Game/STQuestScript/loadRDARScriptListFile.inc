// loadRDARScriptListFile 定义于 STScriptFileList.cpp（ORIG T 0x088bbd77，
// mangled _Z22loadRDARScriptListFileP16STScriptFileListPKcS2_PSt3mapIiiSt4lessIiESaISt4pairIKiiEEE）。
// STScriptFileList 无权威头，此处仅前置声明 + 正常 C++ 自由函数声明（非 asm 桥）。
// STScriptFileList 权威定义见 NpcScript.h（经 Quest.h include 链可见）。
// 本 TU 按 bool 视图声明（P22：ORIG init* 尾 @0x8a616e9 xor $1,%eax;test %al
// 为 bool 取反形；返回类型不入 mangled，链接 STScriptFileList.cpp 的 int 定义，
// 其返回恒 0/1，ABI 兼容——StrikerScript.cpp 同例先）。
bool loadRDARScriptListFile(STScriptFileList* list, const char* dir,
