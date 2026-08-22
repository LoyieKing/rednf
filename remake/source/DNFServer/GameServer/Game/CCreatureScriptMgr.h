// df_game_r Game/ user_creature::CCreatureScriptMgr（批3：从 GameStubs.cpp 迁移）。
#ifndef DNF_CCREATURESCRIPTMGR_H
#define DNF_CCREATURESCRIPTMGR_H

namespace user_creature
{
class CCreatureScriptMgr
{
public:
    void RegisterCreatureIdAndItemId(int creatureId, int itemId);
};

CCreatureScriptMgr* GetInstanceCreatureScriptMgr();
}

#endif  // DNF_CCREATURESCRIPTMGR_H