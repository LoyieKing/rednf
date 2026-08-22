#ifndef GAME_RDARSCRIPTSTRINGMANAGER_H_
#define GAME_RDARSCRIPTSTRINGMANAGER_H_

// ============================================================================
// df_game_r RDARScriptStringManager（G2 字符串表批次）——唯一声明点
// 实现见 RDARScriptStringManager.cpp（符号：C1 0x8aa4e98、
// findString 0x8aa57fe 强符号）。
// ============================================================================

class RDARScriptStringManager
{
public:
    RDARScriptStringManager();
    const char* findString(int table, const char* key, bool* flag) const;
};

extern RDARScriptStringManager g_scriptStringManager_;

#endif  // GAME_RDARSCRIPTSTRINGMANAGER_H_
