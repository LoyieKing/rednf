// ----------------------------------------------------------------------------
// getCharacterJob —— ORIG T 0x088be57e（定义在 SyncScriptDeps.cpp，本 TU 仅声明
// 引用）。全局作业名表 g_characterJobNames（ORIG .data 0x93f4d40，12 项实测）：
// "[swordman]".."[creator mage]" 共 11 项 + "[all]"；i=0..10 命中返回 i，否则 11。
// ----------------------------------------------------------------------------
int getCharacterJob(const char* name);
