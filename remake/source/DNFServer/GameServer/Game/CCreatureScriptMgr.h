// df_game_r Game/ user_creature::CCreatureScriptMgr。
// 布局依据 ORIG ctor（CCreatureScriptMgr.md 0x833cb60）：+0x00 map<int,int>，
// +0x18 map<int,CCreatureScript*>（std::map 在目标工具链为 24B）。
// Destroy 0x833d3bc 遍历 +0x18 map 释放 CCreatureScript*（删除 + operator delete）。
#ifndef DNF_CCREATURESCRIPTMGR_H
#define DNF_CCREATURESCRIPTMGR_H

#include <map>

#include "CCreatureScript.h"

namespace user_creature
{

class CCreatureScriptMgr
{
public:
    CCreatureScriptMgr();                 // ORIG 0x833cb60（两 map 默认构造）
    ~CCreatureScriptMgr();

    // ORIG _ZN13user_creature18CCreatureScriptMgr7DestroyEv 0x833d3bc：
    // 释放 +0x18 map 中所有 CCreatureScript* 并 clear。依赖 CCreatureScript 完整类型。
    void Destroy();

    // ORIG _ZN13user_creature18CCreatureScriptMgr22LoadCreatureScriptFileEv 0x833cba6：
    // 遍历 g_creatureScriptFileList，importCreatureScript + 注册 + 建 CCreatureScript。
    bool LoadCreatureScriptFile();

    // ORIG 0x833d49a：find(creatureId) 命中返回 CCreatureScript*，否则 cMyTrace 后返回 0。
    CCreatureScript* FindCreatureScript(int creatureId);

    // ORIG 0x833d54a：find(creatureId) 未命中则 insert(creatureId→itemId) 返回 true，
    // 否则 cMyTrace 后返回 false。
    bool RegisterCreatureIdAndItemId(int creatureId, int itemId);

private:
    std::map<int, int> m_creatureIdToItemId;            // +0x00
    std::map<int, CCreatureScript*> m_scripts;          // +0x18
};

// ORIG _ZN13user_creature28GetInstanceCreatureScriptMgrEv T 0x8336bd0（单例）。
CCreatureScriptMgr* GetInstanceCreatureScriptMgr();
}

#endif  // DNF_CCREATURESCRIPTMGR_H