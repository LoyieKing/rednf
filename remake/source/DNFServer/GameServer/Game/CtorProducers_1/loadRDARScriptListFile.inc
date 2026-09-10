bool loadRDARScriptListFile(STScriptFileList* list, const char* dir,
                            const char* listPath, std::map<int, int>* unused);
   // ORIG T 0x088bbd77 _Z22loadRDARScriptListFileP16STScriptFileListPKcS2_
   //   _PSt3mapIiiSt4lessIiESaISt4pairIKiiEEE；定义 STScriptFileList.cpp。
   // 旧本地声明 bool(void*,...) mangled 不匹配（Pv ≠ P16STScriptFileList），
   // 曾产生未解析引用——本批次统一为 ORIG 签名（ScriptFileAndGlobalsB2）。
   // 返回类型按 bool 声明（mangled 不含返回类型；StrikerScript.cpp 先例；
   // ORIG 体 return 0/1，调用侧 xor$1/test %al 岛证实 1 字节语义）。

// ---- STAICharacterCommonScript 权威视图（sizeof 0x1010；ORIG 同名类型，
//      sync_script.h 的 8 字节占位与本视图冲突故本 TU 不含该头）----
// +0x08 字段声明为枚举：P254 判据（ORIG `mov 0x8(%eax),%eax; mov %eax,%edx`
// 两段装载中转 ⇒ 源字段类型是 enum；int 字段 4.4 恒折单段）。
