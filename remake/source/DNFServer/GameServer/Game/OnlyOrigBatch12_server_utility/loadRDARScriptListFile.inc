// T 0x088bbd77（STScriptFileList.cpp 定义 int、恒 0/1；返回类型不入 mangled）。
// bool 视图声明：ORIG initPetScript 尾 `xor $1,%eax; test %al` 为 bool 取反形
// （P22/P168a①），int 声明使 4.4 产 test/sete 物化。
bool loadRDARScriptListFile(STScriptFileList* list, const char* dir,
