float ScanTypeOrFloat(bool& isType, std::string& s, bool& isFloat, bool consume);
    // _Z15ScanTypeOrFloatRbRSsS_b T 0x088c2e01（定义 DNFLexWrapperHelpers.cpp）
// [bigfix-enum 待同步] ORIG [job] 0x08a0cfcd store 为 LHS-first ⇒ callee 返回
// 与 m_job 同型（enum）。本 TU 已按权威声明 NpcScript.h:74 保持 int；全链
// enum 化（SyncScriptDeps.cpp 定义 + 各 TU 镜像声明）已上报 Main，届时本处
// 的 static_cast 可删、[job] store 翻正。
