// df_game_r Game/ user_creature::CCreatureScriptMgr 实现。
#include "CCreatureScriptMgr.h"
#include "GameTypes.h"
#include "CreatureScript.h"
#include "LogManager.h"
#include "sync_script.h"   // CSyncScript::truncate/insert/IsEnable
#include <stdio.h>
#include <string.h>

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
// [b28d-30] 复核 DIFF：我方少 8 条（析构/clear 路径形差，N82/P12 族），ORIG 0x0833dxxx 见 report。
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

//   3) sync IsEnable 时按 native.m_attackInfo[0..1] 拼 Attack 路径并
//      ImportAttackInfoScript，序列化 "%d,%d,%f,%d"（元素集追加 ",%d"，
//      空集追加 ",4"；失败则 "%d,%d"）入 buf270/buf370；
//   4) 构造 CCreatureScript，Set(native)，SetItemId(注册表 itemId)，
//      insert_creature_info_to_db，查重后注册进 m_scripts。
// [b28d-30] 复核 DIFF：大件残差（N82/N89 族），ORIG 见 class_func_reports/user_creature__CCreatureScriptMgr/LoadCreatureScriptFile.md。
bool CCreatureScriptMgr::LoadCreatureScriptFile()
{
    CreatureScript native;
    char buf170[0x100] = {0};   // ORIG rep stos 0x100 清零
    char buf270[0x100] = {0};
    char buf370[0x100] = {0};
    AttackInfo attackInfo;

    if (!sync_script::GetInstanceSyncScript()->truncate_creature_info_table())
    {
        puts("failed to truncate creature info table");
        return false;
    }

    for (std::map<int, std::string>::const_iterator it = g_creatureScriptFileList.begin();
         it != g_creatureScriptFileList.end(); ++it)
    {
        const char* path = it->second.c_str();
        if (path == 0)
        {
            return true;   // ORIG cmpl $0x0 → return 1
        }

        if (!importCreatureScript(&native, path))
        {
            LogManager::logFormat(1, "Creature.cpp",
                "bool user_creature::CCreatureScriptMgr::LoadCreatureScriptFile()",
                0xf6b, "importCreatureScript fail file - %s", path);
            return false;
        }

        if (sync_script::GetInstanceSyncScript()->IsEnable())
        {
            if (!native.m_attackInfo.empty() && native.m_attackInfo[0][0] != 0)
            {
                sprintf(buf170, "%s%s", native.m_directory.c_str(),
                        native.m_attackInfo[0].c_str());
                if (ImportAttackInfoScript(&attackInfo, buf170, 0))
                {
                    sprintf(buf270, "%d,%d,%f,%d",
                            native.m_skillType, native.m_skillMP,
                            (double)attackInfo.m_damage, attackInfo.m_attackType);
                    if (attackInfo.m_elements.empty())
                    {
                        sprintf(buf270 + strlen(buf270), ",%d", 4);
                    }
                    else
                    {
                        for (std::set<ENUM_ELEMENT>::const_iterator e =
                                 attackInfo.m_elements.begin();
                             e != attackInfo.m_elements.end(); ++e)
                        {
                            sprintf(buf270 + strlen(buf270), ",%d", *e);
                        }
                    }
                }
                else
                {
                    sprintf(buf270, "%d,%d", native.m_skillType, native.m_skillMP);
                }
            }
            if (native.m_attackInfo.size() > 1 && native.m_attackInfo[1][0] != 0)
            {
                sprintf(buf170, "%s%s", native.m_directory.c_str(),
                        native.m_attackInfo[1].c_str());
                if (ImportAttackInfoScript(&attackInfo, buf170, 0))
                {
                    // ORIG 0x0833d018：第二块（attackInfo[1]）用 over 组字段
                    // (-0x64c=+0x6c m_overSkillType, -0x654=+0x64 m_overSkillMP)
                    sprintf(buf370, "%d,%d,%f,%d",
                            native.m_overSkillType, native.m_overSkillMP,
                            (double)attackInfo.m_damage, attackInfo.m_attackType);
                    if (attackInfo.m_elements.empty())
                    {
                        sprintf(buf370 + strlen(buf370), ",%d", 4);
                    }
                    else
                    {
                        for (std::set<ENUM_ELEMENT>::const_iterator e =
                                 attackInfo.m_elements.begin();
                             e != attackInfo.m_elements.end(); ++e)
                        {
                            sprintf(buf370 + strlen(buf370), ",%d", *e);
                        }
                    }
                }
                else
                {
                    // ORIG 0x0833d11b：同为 over 组字段
                    sprintf(buf370, "%d,%d", native.m_overSkillType,
                            native.m_overSkillMP);
                }
            }
        }

        CCreatureScript* script = new CCreatureScript();
        script->Set(&native);
        int itemId = m_creatureIdToItemId[native.m_id];
        script->SetItemId(itemId);

        if (!sync_script::GetInstanceSyncScript()->insert_creature_info_to_db(
                itemId, &native, buf270, buf370))
        {
            printf("failed to insert id number < %d > creature info to db\n",
                   native.m_id);
            return false;
        }

        if (m_scripts.find(native.m_id) != m_scripts.end())
        {
            cMyTrace trace(
                "bool user_creature::CCreatureScriptMgr::LoadCreatureScriptFile()",
                0xfbe, 5);
            trace("[%s][%d]",
                  "bool user_creature::CCreatureScriptMgr::LoadCreatureScriptFile()",
                  0xfbe);
            return false;
        }
        m_scripts.insert(std::make_pair(native.m_id, script));
    }
    return true;
}

// ORIG FindCreatureScript @ 0x833d49a：在 +0x18 map 按 creatureId 查脚本指针；
// 未命中 cMyTrace 后返回 0。
// [b28d-30] 复核 DIFF：我方多 10 条（find 迭代器 cv/转换构造形，P96/P270/N82 族），
// ORIG 见 class_func_reports/user_creature__CCreatureScriptMgr/FindCreatureScript.md。
CCreatureScript* CCreatureScriptMgr::FindCreatureScript(int creatureId)
{
    std::map<int, CCreatureScript*>::const_iterator it = m_scripts.find(creatureId);
    if (it == m_scripts.end())
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0xfe8, 5);
        trace("[%s][%d]",
              __PRETTY_FUNCTION__,
              0xfe8);
        return 0;
    }
    return it->second;
}

// ORIG RegisterCreatureIdAndItemId @ 0x833d54a。
// [b28d-30] 复核 NEAR：map 迭代器槽/转换临时差（P50/P96/N86 族），ORIG 0x0833d54a。
bool CCreatureScriptMgr::RegisterCreatureIdAndItemId(int creatureId, int itemId)
{
    // P50/P96：ORIG find 返回 iterator 直存（const_iterator 会额外引入转换临时+
    // 拷贝构造，帧 +0x10/多两条 lea——实测 we-larger 10 条）。
    // [b28-BM7 判词] NEAR 残余 = N95/N82：类对象槽基跨版本差——4.1 把
    // find/end/insert 临时深排（-0x34/-0x30/-0x38），4.4 恒从 ebp-8 浅排
    // （-0x24/-0x28），指令/助记符/调用集全等，仅 disp 槽号成串平移。
    std::map<int, int>::iterator it = m_creatureIdToItemId.find(creatureId);
    if (it != m_creatureIdToItemId.end())
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0xfff, 5);
        trace("[%s][%d]",
              __PRETTY_FUNCTION__,
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
