// ORIG T 0x088bbd77
// _Z22loadRDARScriptListFileP16STScriptFileListPKcS2_PSt3mapIiiSt4lessIiESaISt4pairIKiiEEE
// 定义 STScriptFileList.cpp。本 TU 按 ORIG 调用点语义以 bool 视图声明
// （importComboList 的 ORIG 汇编为 xor $1 直接取反，P22：被调函数按 _Bool
// 使用；返回类型不参与 mangling，链接到 STScriptFileList.cpp 的 int 定义，
// 其返回恒为 0/1，ABI 兼容）。
bool loadRDARScriptListFile(STScriptFileList* list, const char* dir,
