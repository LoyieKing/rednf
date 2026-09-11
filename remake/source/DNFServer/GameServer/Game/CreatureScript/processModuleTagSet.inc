// processModuleTagSet ORIG T 0x08a97637（RDARScriptSkill.cpp 模块标签域；实现于
// LinkMiscSecuTimerSolo.cpp）。ImportAttackInfoScript 每行先送其辨认 module 开/闭
// 边界行（返回 0 表示已消费，跳过该行数据解析）。
bool processModuleTagSet(int& moduleId, int& subId, std::string* tag,
