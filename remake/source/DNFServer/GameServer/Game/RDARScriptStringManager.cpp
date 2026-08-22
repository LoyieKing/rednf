// ============================================================================
// RDARScriptStringManager（G2 字符串表批次）
// ----------------------------------------------------------------------------
// 全局 g_scriptStringManager_ 由 CTradeSpace.cpp 引用（findString(key) 邮件文案）。
// ORIG：
//   C1        _ZN23RDARScriptStringManagerC1Ev   0x8aa4e98
//   findString _ZNK23RDARScriptStringManager10findStringEiPKcPb  0x8aa57fe（强符号）
// ORIG findString 依赖类型 StringDictionary（+0x18 vector<StringDictionary*>）与
// LogManager::logFormat / toTString / ss_sprintf，当前重建缺失 StringDictionary
// 类型与表结构，完整语义无法在独立批恢复。本 TU 提供符号与最小生命周期，
// 保持 GameStubs 迁移前的返回行为（空串），并记录阻塞依赖。
// ============================================================================

class RDARScriptStringManager
{
public:
    RDARScriptStringManager();
    const char* findString(int table, const char* key, bool* flag) const;
};

RDARScriptStringManager::RDARScriptStringManager()
{
}

const char* RDARScriptStringManager::findString(int table, const char* key,
                                                bool* flag) const
{
    // ORIG 0x8aa57fe：校验 out/table 索引后经 vector<StringDictionary*>（+0x18）
    // 查表；StringDictionary 类型未建模，暂返回空串（迁移自 GameStubs 行为）。
    (void)table;
    (void)key;
    (void)flag;
    return "";
}

RDARScriptStringManager g_scriptStringManager_;