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

#include "DNFLexWrapper.h"

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

// ===================== 脚本导入（ORIG 0x08991aeb，9821 insn，EDR/Scan 解析） =====================
// 依据 docs/class_func_reports/_lt_global_gt_/ImportEquipmentScript.md 反编译 C 逐段还原。
// 结构：loadRDARScriptFile → Clear → 目录路径 → 首行头解析 → 关键字循环（每条目按
// ScanInt/ScanStr 灌入对应成员）→ 尾部 importEquipmentParameterInfo / 升级表补全。
// 备注：依赖尚未由其它 TU 提供的全局辅助（GetItemCategory / GetAuraAvatarOption /
// GetExpertJob* / scanWorldType / scanModuleType / getCalculationType / importEffect /
// getAvatarEmblemType / getEquipmentTypeToEnumValue / GetEquipmentType）处仅还原可直接
// 经 Scan*/成员写入的部分，其余标「推断」并跳过，不伪造返回值。

namespace
{
// ORIG 使用关键字→索引 map（__lookup）+ switch 分发。此处按同文件其它 ST*Script
// 导入惯例（见 ImportSkillScript）以字符串比较实现等效分发布局。
const char* kEquipmentScriptBaseDir = "Script/EquipmentScript";
}

// 依据 ORIG 0x08991aeb 的 Scan 解析流程还原。签名与导出符号保持不变。
bool ImportEquipmentScript(STEquipmentScript* script, const char* path,
                           bool flag, int a, int idx)
{
    (void)flag;
    (void)a;
    if (!loadRDARScriptFile(kEquipmentScriptBaseDir, path))
    {
        return false;
    }

    script->Clear();

    // ORIG：getDirectoryPath(&pathstr, "%s%s", g_szEquipmentScriptBaseDirectory, path)
    // 写入 +0x664（m_str664）。getDirectoryPath/g_szEquipmentScriptBaseDirectory 尚未在本工程
    // 声明，按语义等价拼接标记「推断」。
    script->m_str664 = std::string(kEquipmentScriptBaseDir) + "/" + path;

    // ---- 主关键字循环（ORIG LAB_08991d82 起 ScanType 循环 + __lookup map→switch 分发） ----
    std::string line;
    while (ScanType(line, true))
    {
        int v = 0;

        // ---------- 基础/物品（STItemScript 基类成员） ----------
        if (line == "[name]")                  { ScanStr(&script->m_strac); }
        else if (line == "[name2]")            { ScanStr(&script->m_strb0); }
        else if (line == "[explain]")          { ScanStr(&script->m_strb4); }
        else if (line == "[explain2]")         { ScanStr(&script->m_strb4); }
        else if (line == "[basic explain]")    { ScanStr(&script->m_str6a4); }
        else if (line == "[detail explain]")   { ScanStr(&script->m_str6a8); }
        else if (line == "[flavor text]")      { ScanStr(&script->m_strb8); }
        else if (line == "[attack info]")      { ScanStr(&script->m_str6ac); }
        else if (line == "[fullset basic explain]")  { ScanStr(&script->m_str6b0); }
        else if (line == "[fullset detail explain]") { ScanStr(&script->m_str6b4); }
        else if (line == "[fullset explain]")         { ScanStr(&script->m_str6b8); }
        else if (line == "[set name]")         { ScanStr(&script->m_str6a8); }
        else if (line == "[set item master]")  { ScanInt(&script->m_6a0); }
        else if (line == "[attach type]")
        {
            // ORIG: "[free]"/"[trade]"/"[trade delete]"/"[sealing]"/"[sealing trade]"/"[account]" → +8
            std::string s;
            if (ScanStr(&s))
            {
                if (s == "[free]")               script->m_field8 = 0;
                else if (s == "[trade]")         script->m_field8 = 1;
                else if (s == "[trade delete]")  script->m_field8 = 2;
                else if (s == "[sealing]")       script->m_field8 = 3;
                else if (s == "[sealing trade]") script->m_field8 = 4;
                else if (s == "[account]")       script->m_field8 = 5;
            }
        }
        else if (line == "[creation rate]")    { ScanInt(&script->m_fieldc); }
        else if (line == "[rarity]")           { ScanInt(&script->m_field10); }
        else if (line == "[value]")            { ScanInt(&script->m_field24); }
        else if (line == "[price]")            { ScanInt(&script->m_field24); }
        else if (line == "[output index]")     { ScanInt(&script->m_field14); }
        else if (line == "[max having count]") { ScanInt(&script->m_field60); }
        else if (line == "[cool time]")        { ScanInt(&script->m_field68); }
        else if (line == "[minimum rank]")     { ScanInt(&script->m_180); }
        else if (line == "[required skill]")   { ScanInt(&script->m_184); }
        else if (line == "[control index]")    { ScanInt(&script->m_188); }
        else if (line == "[usable period]")    { ScanInt(&script->m_fieldf4); }
        else if (line == "[expiration date]")  { ScanInt(&script->m_fieldf8); }
        else if (line == "[equipment type]")
        {
            // ORIG: ScanStr → GetEquipmentType → m_1b4(+0x1b4)；GetEquipmentType 未定义「推断」
            std::string s;
            if (ScanStr(&s)) { script->m_strbc = s; }
        }
        else if (line == "[avatar task num]")  { int dv; ScanInt(&dv); }
        else if (line == "[weapon full avatar]") { script->m_72d = 1; }
        else if (line == "[sub type]")         { ScanInt(&script->m_1b8); }
        else if (line == "[durability]")       { ScanInt(&script->m_1bc); }
        else if (line == "[creature species]") { ScanInt(&script->m_6bc); }
        else if (line == "[grade]")
        {
            // ORIG 0x790/0x794：4 字节颜色 + 1 值，打包进 m_790
            unsigned r = 0, g = 0, b = 0, a4 = 0;
            ScanInt((int*)&r);
            ScanInt((int*)&g);
            ScanInt((int*)&b);
            ScanInt((int*)&a4);
            script->m_790 = (int)((b & 0xff) << 16 | (r & 0xff) | ((g & 0xff) << 8) | (a4 << 24));
            ScanInt(&script->m_794);
        }

        // ---------- 装备动画/层级 ----------
        else if (line == "[hide equipment]")
        {
            script->m_equipTypes768.clear();
            while (ScanStr(&line))
            {
                if (line == "[/hide equipment]") break;
                int t = 0x16;
                if (line == "[hat avatar]") t = 0;
                else if (line == "[hair avatar]") t = 1;
                else if (line == "[face avatar]") t = 2;
                else if (line == "[coat avatar]") t = 3;
                else if (line == "[pants avatar]") t = 4;
                else if (line == "[shoes avatar]") t = 5;
                else if (line == "[breast avatar]") t = 6;
                else if (line == "[waist avatar]") t = 7;
                else if (line == "[skin avatar]") t = 8;
                else if (line == "[aurora avatar]") t = 9;
                else if (line == "[weapon]") t = 10;
                else if (line == "[title name]") t = 0xb;
                if (t != 0x16) script->m_equipTypes768.push_back((ENUM_EQUIPMENTTYPE)t);
            }
        }
        else if (line == "[hide layer]")
        {
            script->m_vec774.clear();
            while (ScanInt(&v)) script->m_vec774.push_back(v);
        }
        else if (line == "[hide growtype avatar]")
        {
            script->m_vec780.clear();
            while (ScanInt(&v)) script->m_vec780.push_back(v);
        }
        else if (line == "[/setback img]")
        {
            script->m_vec66c.clear();
            while (ScanInt(&v)) script->m_vec66c.push_back(v);
        }

        // ---------- 套装/参数 ----------
        else if (line == "[set item]")
        {
            ScanInt(&v);
            EquipmentParameterInfo& info = script->m_paramMap678[v];
            info.clear();
            // ORIG：选中 paramMap678[k] 后经嵌套 importEquipmentParameterInfo 段灌入。
            // 该嵌套段依赖外部上下文与自身读取顺序，标「推断」暂不在此展开。
        }
        else if (line == "[clear avatar]")
        {
            ScanInt(&v);
            if (v == 1) { script->m_7ac = 1; }
        }

        // ---------- 创造/状态 ----------
        else if (line == "[creature piercing]") { ScanInt(&script->m_764); }
        else if (line == "[usable even peace]") { ScanInt(&v); script->m_78c = (v == 1); }
        else if (line == "[usable period after unsealing]") { ScanInt(&script->m_6f8); }
        else if (line == "[mp recovery add rate]") { ScanInt(&script->m_734); }
        else if (line == "[hidden option]")  { script->m_field31 = 1; }
        else if (line == "[npc gift disallowance]") { ScanInt(&v); script->m_field104 = (v != 1); }
        else if (line == "[epic routing]")   { ScanInt(&v); script->m_7bc = (v == 1); }
        else if (line == "[packagable]")     { ScanInt(&v); script->m_fieldfc = (v != 0); }
        else if (line == "[random option]")  { ScanInt(&v); script->m_888 = 1; script->m_889 = (char)v; }
        else if (line == "[enable dye]")
        {
            ScanInt(&v); script->m_88a = (v != 0);
            ScanInt(&v); script->m_88b = (v != 0);
        }

        // ---------- 通用容器 ----------
        else if (line == "[input]")
        {
            script->m_vec7d8.clear();
            script->m_field10e = 1;
            while (ScanInt(&v))
            {
                int v2;
                if (!ScanInt(&v2)) break;
                script->m_vec7d8.push_back(std::make_pair(v, v2));
            }
        }
        else if (line == "[output]")
        {
            script->m_vec7e4.clear();
            while (ScanInt(&v))
            {
                int v2;
                if (!ScanInt(&v2)) break;
                script->m_vec7e4.push_back(std::make_pair(v, v2));
            }
        }
        else if (line == "[emancipate explain]") { ScanStr(&script->m_str7f0); }
        else if (line == "[emancipate]")         { ScanInt(&script->m_6e4); }
        else if (line == "[image packs]")
        {
            script->m_vec7b0.clear();
            while (ScanStr(&line)) script->m_vec7b0.push_back(line);
        }
        else if (line == "[reference effect part set index]") { ScanInt(&v); script->m_vec7a0.push_back(v); }
        else if (line == "[part set index]")    { ScanInt(&script->m_798); }
        else if (line == "[effect part set index]") { ScanInt(&script->m_79c); }
        else if (line == "[icon mark]")         { ScanStr(&script->m_strc0); ScanInt(&script->m_fieldc4); }
        else if (line == "[message]")           { ScanStr(&script->m_str7f4); }

        // ---------- 需要未定义外部辅助的关键字（仅『推断』，不伪造返回值） ----------
        else if (line == "[parameter basic explain]")  { /* 写 param1c0+0x278，段未展开 */ }
        else if (line == "[parameter detail explain]") { /* 写 param1c0+0x27c，段未展开 */ }
        else if (line == "[creature experience amount rate]") { /* +0x750 落在 pad 区，无具名成员 */ }
        else if (line == "[creature physical critical hit]")  { /* +0x754 */ }
        else if (line == "[creature magical critical hit]")   { /* +0x758 */ }
        else if (line == "[creature stuck]")  { /* +0x75c */ }
        else if (line == "[creature physical attack]") { /* +0x73c */ }
        else if (line == "[creature magical attack]")  { /* +0x740 */ }
        else if (line == "[usable world]")    { /* scanWorldType()→m_setdc，未定义 */ }
        else if (line == "[usable module]")   { script->m_set714.clear(); while (ScanInt(&v)) { if (v == 0x11) break; script->m_set714.insert(v); } }
        else if (line == "[no random]")       { script->m_72c = 1; }
        else if (line == "[msg balloon on mucu skill]") { /* +0x87c/+0x878 */ }
        else if (line == "[aurora graphic effects]")    { /* +0x864/+0x858 */ }
        else if (line == "[change type ultimateSkillCurScene]") { ScanInt(&script->m_6dc); }
        else if (line == "[not amplify]")     { ScanInt(&script->m_890); }
        else if (line == "[equip effect]")    { /* importEffect→m_effect84c，未定义 */ }
        else if (line == "[/aura ability]")   { /* aura 终段 */ }
        else if (line == "[aura ability]")    { /* GetAuraAvatarOption 未定义 */ }
        else if (line == "[item category]")   { /* GetItemCategory 未定义 */ }
        else if (line == "[expertjob only]")  { /* GetExpertJobTypeList 未定义 */ }
        else if (line == "[prof material variation]") { /* GetExpertJob* 未定义 */ }
        else if (line == "[prof compound rate]")      { /* GetExpertJob* 未定义 */ }
        else if (line == "[prof result variation]")   { /* GetExpertJob* 未定义 */ }
        else if (line == "[prof disjoint big win rate]") { /* GetExpertJob* 未定义 */ }
        else if (line == "[prof disjoint result variation]"){ /* GetExpertJob* 未定义 */ }
        else if (line == "[prof additional gain exp]") { /* GetExpertJob* 未定义 */ }
        else if (line == "[stuck]") { continue; }

        // ---------- 未知关键字：ORIG 置 local_98=-1 继续（不报错） ----------
        else { /* 静默跳过，与 ORIG -1 分支一致 */ }
    }

    // 尾部升级表补全（ORIG code_r0x0899b289：local_116c 驱动 m_upgrade6fc）与文件索引
    // （FindIndexByFullScan(g_stEquipmentScriptFileList, path) 或 param_5 → *param_1）。
    // 二者依赖未建模的局部计数/全局列表，标「推断」省略主动填充，跨 TU 结果由调用方兜底。

    unloadRDARScriptFile();
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
