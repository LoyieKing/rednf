// ORIG 0x088bbd77 _Z22loadRDARScriptListFileP16STScriptFileListPKcS2_PSt3mapIii..
// 定义 STScriptFileList.cpp（int：恒 0/1；第 4 参未使用）。bool 视图声明：
// P22（ORIG init* 尾 xor $1,%eax;test %al 为 bool 取反形；返回不入 mangled）。
bool loadRDARScriptListFile(STScriptFileList* list, const char* dir,
