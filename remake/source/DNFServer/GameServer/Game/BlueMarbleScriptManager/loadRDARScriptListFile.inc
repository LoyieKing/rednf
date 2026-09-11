// ORIG _Z22loadRDARScriptListFileP16STScriptFileListPKcS2_PSt3mapIii... 0x088bbd77 T
//（定义 CDataManagerScripts.cpp/STScriptFileList.cpp；DNFLexWrapper.h 未含声明）。
// b28d-22/168a①：ORIG initTileList/initMapList 调用点 `call; xor $1,%eax; test %al,%al`
// ⇒ 本 TU 按 bool 视图声明（返回类型不入 mangled；定义处恒 0/1，ABI 兼容）。
bool loadRDARScriptListFile(STScriptFileList* list, const char* dir,
