// df_game_r Game/ user_creature::CCreatureScriptMgr 实现。
#include "CCreatureScriptMgr.h"
#include "GameTypes.h"
#include "CreatureScript.h"
#include "LogManager.h"

namespace user_creature
{

// ORIG CCreatureScriptMgr::CCreatureScriptMgr @ 0x833cb60：两个 map 默认构造。
CCreatureScriptMgr::CCreatureScriptMgr()
    : m_creatureIdToItemId(), m_scripts()
{
}

CCreatureScriptMgr::~CCreatureScriptMgr()
{
    // map<CCreatureScript*> 默认析构仅释放节点，不释放指针指向对象
    // （ORIG 显式释放见 Destroy()）。
}

// ORIG Destroy @ 0x833d3bc：遍历 +0x18 map，删除（D1 + operator delete）每个
// CCreatureScript*，最后 clear()。
void CCreatureScriptMgr::Destroy()
{
    if (m_scripts.empty())
    {
        return;
    }
    for (std::map<int, CCreatureScript*>::iterator it = m_scripts.begin();
         it != m_scripts.end(); ++it)
    {
        CCreatureScript* p = it->second;
        if (p != 0)
        {
            delete p;
        }
    }
    m_scripts.clear();
}

// ORIG LoadCreatureScriptFile @ 0x833cba6：
//   1) sync_script truncate_creature_info_table；
//   2) 遍历 g_creatureScriptFileList（std::map<int,std::string>），对每个文件
//      importCreatureScript(native, path)；
//   3) 构造 CCreatureScript，Set(native)，SetItemId(注册表 itemId)，注册进
//      m_scripts。
bool CCreatureScriptMgr::LoadCreatureScriptFile()
{
    for (std::map<int, std::string>::const_iterator it = g_creatureScriptFileList.begin();
         it != g_creatureScriptFileList.end(); ++it)
    {
        const std::string& path = it->second;
        if (path.empty())
        {
            continue;
        }

        CreatureScript native;
        if (!importCreatureScript(&native, path.c_str()))
        {
            LogManager::logFormat(1, "Creature.cpp",
                "bool user_creature::CCreatureScriptMgr::LoadCreatureScriptFile()",
                0xf6b, "importCreatureScript fail file - %s", path.c_str());
            return false;
        }

        CCreatureScript* script = new CCreatureScript();
        script->Set(&native);

        std::map<int, int>::const_iterator itId = m_creatureIdToItemId.find(native.m_id);
        int itemId = 0;
        if (itId != m_creatureIdToItemId.end())
        {
            itemId = itId->second;
        }
        script->SetItemId(itemId);

        std::map<int, CCreatureScript*>::const_iterator existing = m_scripts.find(native.m_id);
        if (existing != m_scripts.end())
        {
            delete script;
            continue;
        }
        m_scripts.insert(std::make_pair(native.m_id, script));
    }
    return true;
}

// ORIG FindCreatureScript @ 0x833d49a：在 +0x18 map 按 creatureId 查脚本指针；
// 未命中 cMyTrace 后返回 0。
CCreatureScript* CCreatureScriptMgr::FindCreatureScript(int creatureId)
{
    std::map<int, CCreatureScript*>::const_iterator it = m_scripts.find(creatureId);
    if (it == m_scripts.end())
    {
        cMyTrace trace(
            "user_creature::CCreatureScript* user_creature::CCreatureScriptMgr::FindCreatureScript(int)",
            0xfe8, 5);
        trace("[%s][%d]",
              "user_creature::CCreatureScript* user_creature::CCreatureScriptMgr::FindCreatureScript(int)",
              0xfe8);
        return 0;
    }
    return it->second;
}

// ORIG RegisterCreatureIdAndItemId @ 0x833d54a。
bool CCreatureScriptMgr::RegisterCreatureIdAndItemId(int creatureId, int itemId)
{
    std::map<int, int>::const_iterator it = m_creatureIdToItemId.find(creatureId);
    if (it != m_creatureIdToItemId.end())
    {
        cMyTrace trace(
            "bool user_creature::CCreatureScriptMgr::RegisterCreatureIdAndItemId(int, int)",
            0xfff, 5);
        trace("[%s][%d]",
              "bool user_creature::CCreatureScriptMgr::RegisterCreatureIdAndItemId(int, int)",
              0xfff);
        return false;
    }
    m_creatureIdToItemId.insert(std::make_pair(creatureId, itemId));
    return true;
}

// ORIG GetInstanceCreatureScriptMgr @ 0x8336bd0（构造即用单例）。
CCreatureScriptMgr* GetInstanceCreatureScriptMgr()
{
    static CCreatureScriptMgr instance;
    return &instance;
}

}  // namespace user_creature
