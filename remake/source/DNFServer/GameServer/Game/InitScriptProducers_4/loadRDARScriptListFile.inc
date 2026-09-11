// bool 视图声明（P22：ORIG init* 尾 xor $1,%eax;test %al 为 bool 取反形；
// 返回类型不入 mangled，链接 STScriptFileList.cpp 的 int 定义恒 0/1，ABI 兼容）。
bool loadRDARScriptListFile(STScriptFileList* list, const char* dir,
