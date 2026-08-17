// ============================================================================
// df_game_r 还原 —— STEquipmentScript（G5 装备脚本表）
// 逐函数对照 docs/class_func_reports/STEquipmentScript.md：
//   Clear 0x0898e9e4 / ctor 0x0898db82 / dtor 0x0898e298
//   addAniInfo 0x0898efa8 / getAniInfo 0x0898ef1a
//   isUpgradableLevel 0x0898efc8 / isExistUpgradableLevel 0x0898f066
//   artifact_info ctor 0x0898f084 / isUsable 0x0898f0fe
// ============================================================================

#include "STEquipmentScript.h"
#include "STSkillScript.h"

#include <string.h>

extern "C" bool sub_loadRDARScriptFile(const char* dir, const char* path)
    asm("_Z18loadRDARScriptFilePKcS0_");
extern "C" bool sub_ScanType(std::string& line, bool value)
    asm("_Z8ScanTypeRSsb");
extern "C" int sub_ScanInt(int* out) asm("_Z7ScanIntPi");

STEquipmentScript::STEquipmentScript()
{
    Clear();
}

STEquipmentScript::~STEquipmentScript()
{
    if (m_keyCommand)
    {
        delete m_keyCommand;
    }
    m_keyCommand = 0;
}

void STEquipmentScript::Clear()
{
    STItemScript::Clear();
    m_6bc = 0;
    m_764 = 0;
    m_str6a4 = "";
    m_str6ac = "";
    m_str6b8 = "";
    m_str668 = "";
    m_178 = -1;
    m_17c = 0x46;
    m_180 = -1;
    m_184 = -1;
    m_188 = -1;
    m_str1a4 = "";
    m_vec1a8.clear();
    m_field1b4 = 0x1a;
    m_1b8 = 0;
    m_1bc = 0;
    m_param1c0.clear();
    m_aniInfo640.clear();
    m_vec658.clear();
    m_str664 = "";
    m_vec66c.clear();
    m_paramMap678.clear();
    m_6a0 = -1;
    m_equipTypes768.clear();
    m_vec774.clear();
    m_vec780.clear();
    m_78c = 0;
    m_794 = 0;
    m_790 = 0;
    m_798 = -1;
    m_79c = -1;
    m_vec7a0.clear();
    m_7ac = 0;
    m_vec7b0.clear();
    m_avatar6c0.clear();
    m_vec6cc.clear();
    m_6d8 = -1;
    m_6dc = -1;
    m_6e4 = -1;
    m_7bc = 0;
    m_7c0 = -1;
    m_7c4 = -1;
    m_7c8 = 0;
    m_6f4 = 0;
    m_6f5 = 0;
    m_6f6 = 0;
    m_6f8 = 0;
    m_vec7d8.clear();
    m_vec7e4.clear();
    m_str7f0.clear();
    m_enchant7cc.clear();
    m_str7f4.clear();
    m_87c = 0;
    m_808 = 3;
    m_80c = 0;
    m_80e = 0;
    m_810 = 0;
    m_paramVec690.clear();
    m_69c = 0;
    m_814 = 2;
    m_818 = 0;
    m_effect84c.clear();
    m_set714.clear();
    m_set714.insert(0x14);
    m_72c = 0;
    m_730 = 0;
    m_734 = 0;
    m_6e0 = 0;
    m_vec858.clear();
    m_vec864.clear();
    m_str878.clear();
    m_880 = -1;
    m_884 = 0;
    m_888 = 0;
    m_889 = 1;
    m_82c = 0;
    m_vec830.clear();
    m_str83c = "";
    m_setItem840.clear();
    m_88a = 0;
    m_88b = 0;
    m_890 = 0;
    m_88c = 0;
    m_72d = 0;
    m_keyCommand = 0;
}

EquipmentAniInfoScript& STEquipmentScript::addAniInfo(ENUM_CHARACTERJOB job)
{
    return m_aniInfo640[job];
}

EquipmentAniInfoScript& STEquipmentScript::getAniInfo(ENUM_CHARACTERJOB job)
{
    static EquipmentAniInfoScript empty;
    std::map<ENUM_CHARACTERJOB, EquipmentAniInfoScript>::iterator it =
        m_aniInfo640.find(job);
    if (it == m_aniInfo640.end())
    {
        return empty;
    }
    return it->second;
}

bool STEquipmentScript::isUpgradableLevel(ItemUpgradeType::T type, int level)
{
    std::map<ItemUpgradeType::T, std::pair<int, int> >::iterator it =
        m_upgrade6fc.find(type);
    if (it == m_upgrade6fc.end())
    {
        return true;
    }
    return it->second.first > level || it->second.second < level;
}

bool STEquipmentScript::isExistUpgradableLevel()
{
    return m_upgrade6fc.size() != 0;
}

STEquipmentScript::artifact_info::artifact_info()
{
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
    m_c = 0.0f;
    m_10 = 0.0f;
    m_14 = 0.0f;
    m_18 = 0;
    m_1c = 0;
    m_20 = 0;
    m_24 = 0;
    m_28 = 0;
}

bool STEquipmentScript::artifact_info::isUsable() const
{
    if (m_field0 != 0 || m_field4 != 0 || m_field8 != 0)
    {
        return true;
    }
    if (m_c != 0.0f || m_10 != 0.0f || m_14 != 0.0f)
    {
        return true;
    }
    if (m_18 != 0 || m_1c != 0 || m_20 != 0 || m_24 != 0 || m_28 != 0)
    {
        return true;
    }
    return false;
}

// ===================== 脚本导入（语义简化） =====================

bool ImportEquipmentScript(STEquipmentScript* script, const char* path,
                           bool flag, int a, int idx)
{
    (void)flag;
    (void)a;
    (void)idx;
    if (!sub_loadRDARScriptFile("Script/EquipmentScript", path))
    {
        return false;
    }
    script->Clear();
    std::string line;
    int value = 0;
    if (!sub_ScanType(line, true))
    {
        return false;
    }
    if (!sub_ScanInt(&value))
    {
        return false;
    }
    script->m_178 = value;
    if (!sub_ScanType(line, true))
    {
        return false;
    }
    script->m_str6a4 = line;
    if (!sub_ScanType(line, true))
    {
        return false;
    }
    script->m_str6ac = line;
    if (!sub_ScanType(line, true))
    {
        return false;
    }
    script->m_str6b8 = line;
    return true;
}

bool EquipmentScriptSpecialMonsterDrop(STEquipmentScript* script,
                                       std::string str, const char* path)
{
    (void)script;
    (void)str;
    (void)path;
    return false;
}

bool importArtifactInfo(STEquipmentScript::artifact_info& info,
                        std::string str)
{
    (void)str;
    info = STEquipmentScript::artifact_info();
    return true;
}
