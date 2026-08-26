// ============================================================================
// df_game_r 还原 —— sync_script::CSyncScript 剩余方法（G5 数据/脚本域）
// 物品相关方法（insert_equiptment_item_to_db / insert_stackable_item_to_db /
// truncate_dnf_item_info_table / insertCeraShopItemIntoDB 等）已由 CItemList.cpp
// 实现；本 TU 补齐 skill/quest/dungeon/town/exp/creature/aicharacter/
// item_master/random_option 等。逐函数对照
// docs/class_func_reports/sync_script__CSyncScript.md 的 C 代码实现。
// ============================================================================

#include "sync_script.h"

#include <stdio.h>
#include <string.h>

#include "DBMgr.h"
#include "GlobalData.h"
#include "MySQL.h"
#include "STSkillScript.h"
#include "DNFLexWrapper.h"
#include "DNFFunctionLib.h"

// ---- ORIG 编码转换 / 脚本解析辅助（真实符号） ----
// toMbcs/getCharacterJob 声明唯一出处：SyncScriptDeps.h（sync_script.h 尾部已 include）。

#include "CDataManager.h"   // G_CDataManager()->m_randomOption 具名访问（CDataManager.h:3081，ORIG +0x4e10）

namespace
{
// 保留 ORIG 作业名（EUC-KR 字节，见 GetUserJob 0x861d2dc 常量表）。
const char* GetJobString(int job)
{
    switch (job)
    {
    case 0:
        return "\xb1\xcd\xb0\xcb\xbb\xe7";   // 귀검사
    case 1:
        return "\xb0\xdd\xc5\xf5\xb0\xa1";   // 격투가
    case 2:
        return "\xb0\xc5\xb3\xca";           // 거너
    case 3:
        return "\xb8\xb6\xb9\xfd\xbb\xe7";   // 마법사
    case 4:
        return "\xc7\xc1\xb8\xae\xbd\xba\xc6\xae";  // 프리스트
    case 5:
        return "\xbf\xa9\xb0\xc5\xb3\xca";   // 여거너
    case 6:
        return "\xb5\xb5\xc0\xfb";           // 도적
    case 7:
        return "\xb3\xb2\xb0\xdd\xc5\xf5\xb0\xa1";  // 남격투가
    default:
        return "";
    }
}

const char* GetAttachTypeString(int type)
{
    switch (type)
    {
    case 0:
        return "\xc0\xda\xc0\xaf";           // 검사
    case 1:
        return "\xb1\xb3\xc8\xaf";           // 격가
    case 2:
        return "\xb1\xb3\xc8\xaf\xbb\xe8\xc1\xa6";  // 격가프리
    case 3:
        return "\xb9\xd0\xba\xc0";           // 마공
    case 5:
        return "\xb0\xe8\xc1\xa4";           // 계열
    default:
        return "";
    }
}

const char* GetAttackElementString(int element)
{
    switch (element)
    {
    case 0:
        return "\xc8\xad";                   // 화
    case 1:
        return "\xbc\xf6";                   // 수
    case 2:
        return "\xbe\xcf";                   // 암
    case 3:
        return "\xb8\xed";                   // 명
    case 4:
        return "\xb9\xab";                   // 무
    default:
        return "";
    }
}

// ORIG sync_script::CStringMaker（ctor 0x08375f58 / operator()(int) 0x08375f6c /
// operator()(float) 0x08626cd0 / operator()(SkillUseItem) 0x08626d98 /
// operator()(vector<float>&) 0x08627146 / c_str 0x08376034，均 W 弱符号；
// 成员为单个 std::string）。各重载：sprintf 后按需前置 ","（非首元素）追加。
// vector<float> 重载：空向量直接返回；内层元素以迭代器!=begin 判定逗号。
class CStringMaker
{
public:
    CStringMaker() {}
    void operator()(int v)
    {
        char num[0x100] = {0};
        sprintf(num, "%d", v);
        append(num);
    }
    void operator()(float v)
    {
        char num[0x100] = {0};
        sprintf(num, "%f", v);
        append(num);
    }
    void operator()(const SkillUseItem& v)
    {
        char num[0x100] = {0};
        sprintf(num, "%d,%d,%d", v.m_index, v.m_useLevel[0], v.m_useLevel[1]);
        append(num);
    }
    void operator()(const std::vector<float>& values)
    {
        if (values.empty())
        {
            return;
        }
        if (!m_str.empty())
        {
            m_str += ",";
        }
        for (size_t i = 0; i < values.size(); ++i)
        {
            if (i != 0)
            {
                m_str += ",";
            }
            char num[0x100] = {0};
            sprintf(num, "%f", values[i]);
            m_str += num;
        }
    }
    const char* c_str() const { return m_str.c_str(); }

private:
    void append(const char* num)
    {
        if (!m_str.empty())
        {
            m_str += ",";
        }
        m_str += num;
    }

    std::string m_str;   // ORIG +0x00 单 string 成员
};

// ORIG CStreamBuffer（ctor 0x086269be memset(buf,0,0xa000) / c_str 0x086269e2 /
// operator<<(const char*) 0x08626a4a "%s" / <<(int) 0x08626a70 "%d" /
// <<(unsigned) 0x08626a96 "%u" / <<(float) 0x08626abc "%f"）：
// 0xa000 定长缓冲，operator<< 返回 *this 链式追加（Insert<T> → snprintf → strncat）。
class CStreamBuffer
{
public:
    CStreamBuffer() { memset(m_buf, 0, sizeof(m_buf)); }
    CStreamBuffer& operator<<(const char* v)
    {
        append(v);
        return *this;
    }
    CStreamBuffer& operator<<(int v)
    {
        char num[0x100] = {0};
        sprintf(num, "%d", v);
        append(num);
        return *this;
    }
    CStreamBuffer& operator<<(unsigned int v)
    {
        char num[0x100] = {0};
        sprintf(num, "%u", v);
        append(num);
        return *this;
    }
    CStreamBuffer& operator<<(float v)
    {
        char num[0x100] = {0};
        sprintf(num, "%f", v);
        append(num);
        return *this;
    }
    const char* c_str() const { return m_buf; }

private:
    void append(const char* s)
    {
        size_t used = strlen(m_buf);
        strncat(m_buf, s, sizeof(m_buf) - 1 - used);
    }

    char m_buf[0xa000];   // ORIG 对象尺寸 0xa000
};

// for_each 累加：std::for_each 按值收 functor，必须取返回副本再读结果
// （ORIG 反编译同样消费 for_each 返回值）。
template <class It>
CStringMaker makeString(It first, It last)
{
    return std::for_each(first, last, CStringMaker());
}
}

// ===================== CSyncScript 实现 =====================

namespace sync_script
{

// ---- CListTables.cpp / CDungeonList.cpp 引用的全局表（原 GameStubs.cpp 桩） ----
std::map<int, std::string> g_dungeonInfoTable;    // ORIG 0x94f6ea0
std::map<int, std::string> g_monsterInfoTable;    // ORIG 0x94ff260
std::map<int, std::string> g_questInfoTable;      // ORIG 0x9500480
std::map<int, std::string> g_skillInfoTable[11];  // ORIG 0x9500820

// CSyncScript 构造/析构由 CItemList.cpp（ORIG 0x861cbb4 区）提供；本 TU 不重复定义。

bool CSyncScript::GetType(std::string name, int job, int idx, int& out1,
                          int& out2, bool flag)
{
    if (name.empty())
    {
        return false;
    }
    if (name.find_first_of("[", 0) != 0)
    {
        return false;
    }
    name.erase(0, 1);
    name.erase(name.size() - 1, 1);
    std::string key = "stackable ";
    if (flag || name != "creature")
    {
        key += name;
    }
    else
    {
        key = name;
    }
    SITEMTYPE local = m_itemTypeMap[key];
    if (m_vec1.empty() || m_vec1[0] == local.m_field1)
    {
        SWEAPONTYPE wp;
        wp.m_job = job;
        wp.m_idx = idx;
        local = m_weaponMap[wp];
    }
    else if (m_vec1.size() >= 2 && m_vec1[1] == local.m_field1)
    {
        SARMORTYPE ap;
        ap.m_field0 = (unsigned char)idx;
        ap.m_str4 = name;
        local = m_armorMap[ap];
    }
    out1 = local.m_field1;
    out2 = local.m_field4;
    return true;
}

const char* CSyncScript::GetURL(const char* str, int len, int idx)
{
    static std::string strUrl;
    strUrl.clear();
    int start = 0;
    int end = 0;
    int i = 0;
    for (i = 0; i < len; ++i)
    {
        if (str[i] == '/')
        {
            start = i + 1;
        }
        if (str[i] == '.')
        {
            end = i;
        }
    }
    strUrl.insert(0, str + start, end - start);
    char tmp[16] = {0};
    sprintf(tmp, "/%05d.png", idx);
    strUrl += tmp;
    return strUrl.c_str();
}

const char* CSyncScript::GetUserJob(int job)
{
    return GetJobString(job);
}

const char* CSyncScript::GetAttachType(int type)
{
    switch (type)
    {
    case 0:
        return "\xc0\xda\xc0\xaf";           // 검사
    case 1:
        return "\xb1\xb3\xc8\xaf";           // 격가
    case 2:
        return "\xb1\xb3\xc8\xaf\xbb\xe8\xc1\xa6";  // 격가프리
    case 3:
        return "\xb9\xd0\xba\xc0";           // 마공
    case 5:
        return "\xb0\xe8\xc1\xa4";           // 계열
    default:
        return "";
    }
}

const char* CSyncScript::GetAttackElement(EquipmentParameterInfo* param)
{
    switch (param->m_elementalProperty)
    {
    case 0:
        return "\xc8\xad";                   // 화
    case 1:
        return "\xbc\xf6";                   // 수
    case 2:
        return "\xbe\xcf";                   // 암
    case 3:
        return "\xb8\xed";                   // 명
    case 4:
        return "\xb9\xab";                   // 무
    default:
        return "";
    }
}

void CSyncScript::GetLevelUpSkill(
    STEquipmentScript* script,
    EquipmentParameterInfo::ENUM_SKILL_UP_VECTOR_INDEX vectorIndex)
{
    static std::string level_up_skill;
    level_up_skill.clear();
    // ORIG 0x0861d472：param*0xc+(script+0x40c) → EquipmentParameterInfo 内第 idx 档
    // vector<LevelUpSkill>（+0x24c/+0x258/+0x264 三档，每档 0xc）。
    // 第三档（+0x264）与权威头 m_avatar264（[avatar select ability]，CEquipItem.cpp
    // 有消费证据）语义冲突且无 call 引用可证枚举值域，保守不实现。
    std::vector<LevelUpSkill>* vec = 0;
    if (vectorIndex == EquipmentParameterInfo::ENUM_SKILL_UP_VECTOR_INDEX_0)
    {
        vec = &script->m_param1c0.m_levelUpSkills24c;
    }
    else if (vectorIndex == EquipmentParameterInfo::ENUM_SKILL_UP_VECTOR_INDEX_1)
    {
        vec = &script->m_param1c0.m_levelUpSkills258;
    }
    else
    {
        return;   // default → 保持空串
    }
    const unsigned int skillCount = vec->size();
    for (unsigned int i = 0; i < skillCount; ++i)
    {
        level_up_skill += "|";
        const LevelUpSkill& skill = vec->at(i);   // ORIG 用 at
        const char* job = GetUserJob(skill.m_job);
        if (std::string(job) == "")
        {
            level_up_skill += "\xb0\xf8\xc5\xeb";   // 공용
        }
        else
        {
            level_up_skill += job;
        }
        char tmp[0x400];
        sprintf(tmp, "| %d| %d", skill.m_field4, skill.m_field8);
        level_up_skill += tmp;
    }
    level_up_skill.c_str();
}

const char* CSyncScript::GetUsableCharacter(STEquipmentScript* script)
{
    if (script->m_fielda0 == 0)
    {
        for (int i = 0; i < 0xb; ++i)
        {
            if (script->m_usable[i] != 0)
            {
                return GetUserJob(i);
            }
        }
        return "";
    }
    return "\xb8\xf0\xb5\xce";   // 모두
}

const char* CSyncScript::GetUsableCharacter(STStackableScript* script)
{
    if (script->m_fielda0 == 0)
    {
        for (int i = 0; i < 0xb; ++i)
        {
            if (script->m_usable[i] != 0)
            {
                return GetUserJob(i);
            }
        }
        return "";
    }
    return "\xb8\xf0\xb5\xce";   // 모두
}

bool CSyncScript::LoadItemInfoMasterScript(const char* path)
{
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }
    std::string line;
    std::string str;
    while (ScanType(line, true))
    {
        if (line == "[master type]")
        {
            int value = ScanInt((int*)0);
            ScanStr(&str);
            m_map64[(unsigned char)value] = str;
        }
        else if (line == "[type addition]")
        {
            int value = ScanInt((int*)0);
            m_vec1.push_back((unsigned char)value);
        }
        else if (line == "[reverse desc]")
        {
            int value = ScanInt((int*)0);
            m_vec2.push_back((unsigned char)value);
        }
        else if (line == "[material type]")
        {
            int value = ScanInt((int*)0);
            ScanStr(&str);
            m_map7c[(unsigned char)value] = str;
        }
        else if (line == "[weapon type]")
        {
            ScanStr(&str);
            int type = ScanInt((int*)0);
            int subType = ScanInt((int*)0);
            int value = ScanInt((int*)0);
            std::string name;
            ScanStr(&name);
            int job = getCharacterJob(str.c_str());
            SWEAPONTYPE wp;
            wp.m_job = job;
            wp.m_idx = subType;
            m_weaponMap[wp] = SITEMTYPE(false, (unsigned char)type,
                                         (unsigned char)subType,
                                         (unsigned short)value, name);
        }
        else if (line == "[armor type]")
        {
            ScanStr(&str);
            int type = ScanInt((int*)0);
            int subType = ScanInt((int*)0);
            int value = ScanInt((int*)0);
            std::string name;
            ScanStr(&name);
            SARMORTYPE ap;
            ap.m_field0 = (unsigned char)subType;
            ap.m_str4 = str;
            m_armorMap[ap] = SITEMTYPE(false, (unsigned char)type,
                                       (unsigned char)subType,
                                       (unsigned short)value, name);
        }
        else if (line == "[item type]")
        {
            ScanStr(&str);
            int type = ScanInt((int*)0);
            int subType = ScanInt((int*)0);
            int value = ScanInt((int*)0);
            int extra = ScanInt((int*)0);
            std::string name;
            ScanStr(&name);
            m_itemTypeMap[str] = SITEMTYPE(type != 0, (unsigned char)subType,
                                           (unsigned char)value,
                                           (unsigned short)extra, name);

        }
    }
    return true;
}

bool CSyncScript::insert_skill_index(STSkillScript* script, int job)
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)3, (ENUM_SERVER_GROUP)0);
    char name[0x100];
    memset(name, 0, sizeof(name));
    char escaped[0x300];
    memset(escaped, 0, sizeof(escaped));
    CodePage::script2Database((char*)script->m_str4.c_str(), name);
    db->escape_string(escaped, name);
    db->set_query("inSert into skill_index(job, skill_idx, skill_name) values(%d, %d, '%s')",
                  job, script->m_field0, escaped);
    return db->exec(true);
}

bool CSyncScript::truncate_skill_index()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)3, (ENUM_SERVER_GROUP)0);
    db->set_query("trUncate table skill_index");
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::trancate_skill_info()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    db->set_query("trUncate table skill_info; ");
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::insert_skill_info(STSkillScript* script, int job, int type)
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    // ORIG：localVec = m_vec44/m_vec50 交替展开（pre_required_skill 段）。
    std::vector<int> localVec;
    for (unsigned int i = 0; i < script->m_vec44.size(); ++i)
    {
        localVec.push_back(script->m_vec44[i]);
        localVec.push_back(script->m_vec50[i]);
    }
    // GetEscapeString 输出区：ORIG 栈上单块 0xb000 缓冲（入口即 memset），
    // 按 escCount*0x1000 步进取槽（共 7 次）；name/name2 转码缓冲各 0x400。
    char escapedBuf[0xb000];
    memset(escapedBuf, 0, sizeof(escapedBuf));
    int escCount = 0;
    #define SKILL_ESC_SLOT (escapedBuf + escCount++ * 0x1000)
    char name[0x400] = {0};
    if (!CodePage::script2Database((char*)script->m_str4.c_str(), name))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x7ff, 5)(
            "Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
            CodePage::database(), script->m_str4.c_str(),
            __PRETTY_FUNCTION__, 0x7ff);
    }
    char name2[0x400] = {0};
    if (!CodePage::script2Database((char*)script->m_str8.c_str(), name2))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x807, 5)(
            "Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
            CodePage::database(), script->m_str8.c_str(),
            __PRETTY_FUNCTION__, 0x807);
    }
    // GetEscapeString 输出区声明见函数头；流对象在 name2 处理后构造。
    CStreamBuffer sb;
    // 1)job_index 2)skill_index(m_field0) 3)module_type(type)
    sb << job;
    sb << ", " << script->m_field0;
    sb << ", " << type;
    // 4)name 5)name2（转码后 escape；GetEscapeString 结果写入槽缓冲）
    char* escName = SKILL_ESC_SLOT;
    GetEscapeString(db, name, escName);
    sb << ", '" << escName << "'";
    char* escName2 = SKILL_ESC_SLOT;
    GetEscapeString(db, name2, escName2);
    sb << ", '" << escName2 << "'";
    // 6)basic_explain(+0x10 先于 +0xc) 7)skill_explain(+0xc)
    char* escBasic = SKILL_ESC_SLOT;
    GetEscapeString(db, script->m_str10.c_str(), escBasic);
    sb << ", '" << escBasic << "'";
    char* escSkillExplain = SKILL_ESC_SLOT;
    GetEscapeString(db, script->m_strc.c_str(), escSkillExplain);
    sb << ", '" << escSkillExplain << "'";
    // 8)m_vec18
    {
        CStringMaker mk = makeString(script->m_vec18.begin(), script->m_vec18.end());
        sb << ", '" << mk.c_str() << "'";
    }
    // 9)+0x5c 10)+0x60
    sb << ", " << script->m_5c;
    sb << ", " << script->m_60;
    // 11)m_arr7c[6]
    {
        CStringMaker mk =
            makeString((int*)script->m_arr7c, (int*)script->m_arr7c + 6);
        sb << ", '" << mk.c_str() << "'";
    }
    // 12)growType[0..4].m_8 五段分号连接（单引号包裹）
    {
        CStringMaker mk0 = makeString(script->m_growType[0].m_8,
                                      script->m_growType[0].m_8 + 2);
        CStringMaker mk1 = makeString(script->m_growType[1].m_8,
                                      script->m_growType[1].m_8 + 2);
        CStringMaker mk2 = makeString(script->m_growType[2].m_8,
                                      script->m_growType[2].m_8 + 2);
        CStringMaker mk3 = makeString(script->m_growType[3].m_8,
                                      script->m_growType[3].m_8 + 2);
        CStringMaker mk4 = makeString(script->m_growType[4].m_8,
                                      script->m_growType[4].m_8 + 2);
        sb << ", '" << mk0.c_str() << ";" << mk1.c_str() << ";"
           << mk2.c_str() << ";" << mk3.c_str() << ";" << mk4.c_str() << "'";
    }
    // 13)m_vec1dc（lea 0x1dc@08623889）14)m_vecf4（lea 0xf4@0862399a）
    {
        CStringMaker mk = makeString(script->m_vec1dc.begin(), script->m_vec1dc.end());
        sb << ", '" << mk.c_str() << "'";
    }
    {
        CStringMaker mk = makeString(script->m_vecf4.begin(), script->m_vecf4.end());
        sb << ", '" << mk.c_str() << "'";
    }
    // 15)m_useItems11c（"%d,%d,%d"）
    {
        CStringMaker mk = makeString(script->m_useItems11c.begin(),
                                     script->m_useItems11c.end());
        sb << ", '" << mk.c_str() << "'";
    }
    // 16)+0x30 17)+0x34
    sb << ", " << script->m_30;
    sb << ", " << script->m_34;
    // 18)localVec（pre_required_skill）
    {
        CStringMaker mk = makeString(localVec.begin(), localVec.end());
        sb << ", '" << mk.c_str() << "'";
    }
    // 19)m_f100[2]（for_each<float*> +0x100..+0x108，"%f"）
    {
        CStringMaker mk = makeString((float*)script->m_f100,
                                     (float*)script->m_f100 + 2);
        sb << ", '" << mk.c_str() << "'";
    }
    // 20)m_arr138[2]
    {
        CStringMaker mk = makeString((int*)script->m_arr138,
                                     (int*)script->m_arr138 + 2);
        sb << ", '" << mk.c_str() << "'";
    }
    // 21)m_i108[2..3]（for_each<int*> +0x110..+0x118）
    {
        CStringMaker mk = makeString((int*)script->m_i108 + 2,
                                     (int*)script->m_i108 + 4);
        sb << ", '" << mk.c_str() << "'";
    }
    // 22)icon：esc(str160[0]),arr168[0],esc(str160[1]),arr168[1]
    char* icon1 = SKILL_ESC_SLOT;
    GetEscapeString(db, script->m_str160[1].c_str(), icon1);
    char* icon0 = SKILL_ESC_SLOT;
    GetEscapeString(db, script->m_str160[0].c_str(), icon0);
    sb << ", '" << icon0 << "," << script->m_arr168[0]
       << "," << icon1 << "," << script->m_arr168[1] << "'";
    // 23)command_key_explain：esc(m_str194)
    char* escCommandKey = SKILL_ESC_SLOT;
    GetEscapeString(db, script->m_str194.c_str(), escCommandKey);
    sb << ", '" << escCommandKey << "'";
    // 24)skill_command_advantage：m_240,m_244
    sb << ", '" << script->m_240 << "," << script->m_244 << "'";
    // 25)static_data：m_vec198（lea 0x198@086240e4）
    {
        CStringMaker mk = makeString(script->m_vec198.begin(), script->m_vec198.end());
        sb << ", '" << mk.c_str() << "'";
    }
    // 26)level_info：m_vec1a8（外层 for_each<vector<float>*> → operator()(vector<float>&)）
    {
        CStringMaker mk = makeString(script->m_vec1a8.begin(), script->m_vec1a8.end());
        sb << ", '" << mk.c_str() << "'";
    }
    // 27)start_cool_time：m_21c,m_220
    sb << ", '" << script->m_21c << "," << script->m_220 << "'";
#undef SKILL_ESC_SLOT
    db->set_query("inSert into skill_info(job_index, skill_index, module_type, name, name2, basic_explain, skill_explain, purchase_cost, type, skill_class, growtype_maximum_level, second_growtype_maximum_level, skill_fitness_growtype, skill_fitness_second_growtype, consume_item, required_level, required_level_range, pre_required_skill, consume_mp, cool_time, casting_time, icon, command_key_explain, skill_command_advantage, static_data, level_info, start_cool_time) values(%s)",
                  sb.c_str());
    return db->exec(true);
}

bool CSyncScript::insert_quest_category(QuestScript* script)
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)2, (ENUM_SERVER_GROUP)0);
    char name[0x100];
    memset(name, 0, sizeof(name));
    char escaped[0x100];
    memset(escaped, 0, sizeof(escaped));
    CodePage::script2Database((char*)script->m_name.c_str(), name);
    db->escape_string(escaped, name);
    db->set_query("inSert into quest_category(quest_idx, quest_name) values(%d, '%s')",
                  script->m_index, escaped);
    return db->exec(true);
}

bool CSyncScript::truncate_quest_category()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)2, (ENUM_SERVER_GROUP)0);
    db->set_query("trUncate table quest_category");
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::insert_charac_quest_category(unsigned int origin,
                                               unsigned int mapped)
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)2, (ENUM_SERVER_GROUP)0);
    db->set_query("inSert into charac_quest_ref(origin_idx, mapped_idx) values(%d, %d)",
                  origin, mapped);
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::truncate_charac_quest_ref()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)2, (ENUM_SERVER_GROUP)0);
    db->set_query("trUncate table charac_quest_ref; ");
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::insert_exp_level_to_db(unsigned int exp, int level)
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    db->set_query("inSert into exp_level_ref(exp, lev) values(%u, %d)", exp, level);
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::truncate_exp_level_ref_table()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    db->set_query("trUncate table exp_level_ref");
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::insert_item_parts_set(int index, STEquipmentPartSetPiece& piece)
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    db->set_query("inSert into item_part_set(part_set_index, part_name, part_type, part_grade, part_rarity) values(%d, '%s',%d,%d,%d)",
                  index, piece.m_name.c_str(), piece.m_field4, piece.m_field8,
                  piece.m_fieldc);
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::trancate_item_parts_set()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    db->set_query("trUncate table item_part_set");
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::insert_aicharacter_info_to_db(int idx,
                                                const STAICharacterCommonScript& script)
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    if (idx == 0 || script.m_str4.length() == 0)
    {
        return true;
    }
    char name[0xf0];
    memset(name, 0, sizeof(name));
    char escaped[0x100];
    memset(escaped, 0, sizeof(escaped));
    CodePage::script2Database((char*)script.m_str4.c_str(), name);
    db->escape_string(escaped, name);
    db->set_query("inSert into dnf_aicharacter_info(idx, ai_name_kr) values(%d, '%s')",
                  idx, escaped);
    return db->exec(true);
}

bool CSyncScript::truncate_aicharacter_info_table()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    if (!db->set_query("trUncate table dnf_aicharacter_info"))
    {
        return false;
    }
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::insert_town_info_to_db(const TownScript& script)
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    if (script.m_field0 == 0 || script.m_str4.length() == 0)
    {
        return true;
    }
    char name[0xf0];
    memset(name, 0, sizeof(name));
    char escaped[0x100];
    memset(escaped, 0, sizeof(escaped));
    CodePage::script2Database((char*)script.m_str4.c_str(), name);
    db->escape_string(escaped, name);
    db->set_query("inSert into dnf_town_info(idx, town_name_kr) values(%d, '%s')",
                  script.m_field0, escaped);
    return db->exec(true);
}

bool CSyncScript::truncate_town_info_table()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    if (!db->set_query("trUncate table dnf_town_info"))
    {
        return false;
    }
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::insert_monster_info_to_db(const STMonsterScript& script)
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    if (script.m_index == 0 || script.m_name.length() == 0)
    {
        return true;
    }
    char name[0xf0];
    memset(name, 0, sizeof(name));
    char escaped[0x100];
    memset(escaped, 0, sizeof(escaped));
    CodePage::script2Database((char*)script.m_name.c_str(), name);
    db->escape_string(escaped, name);
    db->set_query("inSert into dnf_monster_info(idx, mon_name_kr) values(%d, '%s')",
                  script.m_index, escaped);
    return db->exec(true);
}

bool CSyncScript::truncate_monster_info_table()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    if (!db->set_query("trUncate table dnf_monster_info"))
    {
        return false;
    }
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::insert_creature_info_to_db(unsigned int id,
                                             const CreatureScript* script,
                                             const char* param3,
                                             const char* param4)
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    // ORIG：artifact_slot 列 = '0'+(R/G/B 三槽标志) 的 3 字符串
    // （CreatureScript+0x50/0x54/0x58，asm 0x08624437..0x0862445d，
    // 低字节存 (v+'0')&0xff；缓冲 movl$0 预清零即 NUL 结尾）。
    char artifactSlot[4];
    memset(artifactSlot, 0, sizeof(artifactSlot));
    const int slotValues[3] = {script->m_artifactSlotRed,
                               script->m_artifactSlotBlue,
                               script->m_artifactSlotGreen};
    for (int i = 0; i < 3; ++i)
    {
        artifactSlot[i] = (char)(slotValues[i] + '0');
    }
    // ORIG：CStringMaker(vector<int>) → "a,b,c"（无尾逗号，空向量→空串）。
    const std::string piercingStr =
        makeString(script->m_piercing.begin(), script->m_piercing.end()).c_str();
    const std::string skillLevelValuesStr =
        makeString(script->m_skillLevelValues.begin(),
                   script->m_skillLevelValues.end()).c_str();
    const std::string overSkillLevelValuesStr =
        makeString(script->m_overSkillLevelValues.begin(),
                   script->m_overSkillLevelValues.end()).c_str();
    char escaped[11][0x100];
    for (int i = 0; i < 11; ++i)
    {
        memset(escaped[i], 0, 0x100);
    }
    const char* values[11];
    values[0] = script->m_name.c_str();
    values[1] = artifactSlot;
    values[2] = param3;
    values[3] = param4;
    values[4] = piercingStr.c_str();
    // ORIG：skill_name/skill_desc/overskill_name/overskill_desc ←
    // m_skillString（vector<string> @+0x128）前 4 项，越界项用 ""。
    for (int i = 0; i < 4; ++i)
    {
        if (i < (int)script->m_skillString.size())
        {
            values[5 + i] = script->m_skillString[i].c_str();
        }
        else
        {
            values[5 + i] = "";
        }
    }
    values[9] = skillLevelValuesStr.c_str();
    values[10] = overSkillLevelValuesStr.c_str();
    for (int i = 0; i < 11; ++i)
    {
        GetEscapeString(db, values[i], escaped[i]);
    }
    if (!db->set_query(
            "inSert into dnf_creature_info(it_id, creature_id, creature_name, skill_recovery_time, overskill_recovery_time, artifact_slot, learn_overskill_level, skill_info, overskill_info, piercing, skill_name, skill_desc, overskill_name, overskill_desc, skill_level_values, overskill_level_values, evolution_creature_id, evolution_level) values(%u, %d, '%s', %d, %d, '%s', %d, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', %d, %d)",
            id, script->m_id, escaped[0], script->m_field1c,
            script->m_field20, escaped[1], script->m_learnOverSkillLevel,
            escaped[2], escaped[3], escaped[4], escaped[5], escaped[6],
            escaped[7], escaped[8], escaped[9], escaped[10],
            script->m_evolutionCreatureId, script->m_evolutionLevel))
    {
        return false;
    }
    return db->exec(true);
}

bool CSyncScript::truncate_creature_info_table()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    if (!db->set_query("trUncate table dnf_creature_info"))
    {
        return false;
    }
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::insert_one_item_info_master(unsigned char masterNo,
                                              unsigned short subNo,
                                              const char* name,
                                              const char* explain)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    const char* names[2];
    names[0] = toMbcs(name);
    names[1] = toMbcs(explain);
    char escaped[2][100];
    memset(escaped, 0, sizeof(escaped));
    for (int i = 0; i < 2; ++i)
    {
        GetEscapeString(db, names[i], escaped[i]);
    }
    if (!db->set_query(
            "inSert into dnf_item_info_master(master_no, sub_no, name, master_explain) values(%u, %u, '%s', '%s')",
            (unsigned int)masterNo, (unsigned int)subNo, escaped[0], escaped[1]))
    {
        return false;
    }
    return db->exec(true);
}

bool CSyncScript::insert_item_info_master_to_db()
{
    if (m_field0 != 1)
    {
        return true;
    }
    for (std::map<SWEAPONTYPE, SITEMTYPE>::iterator it = m_weaponMap.begin();
         it != m_weaponMap.end(); ++it)
    {
        char name[0x100];
        char full[0x100];
        sprintf(name, "%s%s", m_map7c[it->second.m_field2].c_str(),
                it->second.m_str8.c_str());
        sprintf(full, "<%s>%s", m_map64[it->second.m_field1].c_str(), name);
        if (!insert_one_item_info_master(it->second.m_field1,
                                         it->second.m_field4, name, full))
        {
            return false;
        }
    }
    for (std::map<SARMORTYPE, SITEMTYPE>::iterator it = m_armorMap.begin();
         it != m_armorMap.end(); ++it)
    {
        char name[0x100];
        char full[0x100];
        sprintf(name, "%s%s", m_map7c[it->second.m_field2].c_str(),
                it->second.m_str8.c_str());
        sprintf(full, "<%s>%s", m_map64[it->second.m_field1].c_str(), name);
        if (!insert_one_item_info_master(it->second.m_field1,
                                         it->second.m_field4, name, full))
        {
            return false;
        }
    }
    for (std::map<std::string, SITEMTYPE>::iterator it = m_itemTypeMap.begin();
         it != m_itemTypeMap.end(); ++it)
    {
        if (it->second.m_field0 == 1)
        {
            bool matched = false;
            for (std::vector<unsigned char>::iterator vi = m_vec1.begin();
                 vi != m_vec1.end(); ++vi)
            {
                if (it->second.m_field1 == *vi)
                {
                    matched = true;
                    break;
                }
            }
            char full[0x100];
            sprintf(full, "<%s>%s", m_map64[it->second.m_field1].c_str(),
                    it->second.m_str8.c_str());
            if (matched)
            {
                if (!insert_one_item_info_master(it->second.m_field1,
                                                 it->second.m_field4,
                                                 m_map64[it->second.m_field1].c_str(),
                                                 full))
                {
                    return false;
                }
            }
            else
            {
                if (!insert_one_item_info_master(it->second.m_field1,
                                                 it->second.m_field4,
                                                 it->second.m_str8.c_str(),
                                                 full))
                {
                    return false;
                }
            }
        }
    }
    return true;
}

bool CSyncScript::truncate_item_info_master_table()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    if (!db->set_query("trUncate table dnf_item_info_master"))
    {
        return false;
    }
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::insert_old_equip_info_to_db()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    for (std::map<unsigned int, STEquipmentStatInfo>::iterator it =
             m_oldEquipMap.begin();
         it != m_oldEquipMap.end(); ++it)
    {
        if (!db->set_query(
                "inSert into dnf_old_equip_info(it_id, hp_max, mp_max, phy_att, phy_def, mag_att, mag_def, equip_phy_att, equip_phy_def, equip_mag_att, equip_mag_def, ref_fire, ref_water, ref_dark, ref_light, ref_all_elements, ref_slow, ref_freeze, ref_poison, ref_stun, ref_curse, ref_blind, ref_lightning, ref_stone, ref_sleep, ref_burn, ref_weapon_break, ref_bleeding, ref_pierce, ref_stuck, ref_confuse, ref_hold, ref_armor_break, ref_all_state) values(%d, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i)",
                it->first, it->second.m_hpMax, it->second.m_mpMax,
                it->second.m_phyAtt, it->second.m_phyDef,
                it->second.m_magAtt, it->second.m_magDef,
                it->second.m_equipPhyAtt, it->second.m_equipPhyDef,
                it->second.m_equipMagAtt, it->second.m_equipMagDef,
                it->second.m_refFire, it->second.m_refWater,
                it->second.m_refDark, it->second.m_refLight,
                it->second.m_refAll, it->second.m_refSlow,
                it->second.m_refFreeze, it->second.m_refPoison,
                it->second.m_refStun, it->second.m_refCurse,
                it->second.m_refBlind, it->second.m_refLightning,
                it->second.m_refStone, it->second.m_refSleep,
                it->second.m_refBurn, it->second.m_refWeaponBreak,
                it->second.m_refBleeding, it->second.m_refPierce,
                it->second.m_refStuck, it->second.m_refConfuse,
                it->second.m_refHold, it->second.m_refArmorBreak,
                it->second.m_refAllState))
        {
            return false;
        }
        if (!db->exec(true))
        {
            return false;
        }
    }
    return true;
}

bool CSyncScript::truncate_old_equip_info_table()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    if (!db->set_query("trUncate table dnf_old_equip_info"))
    {
        return false;
    }
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::insert_mapping_equip_info_to_db(std::map<int, int>& mapping)
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    for (std::map<int, int>::iterator it = mapping.begin();
         it != mapping.end(); ++it)
    {
        if (it->second != 0)
        {
            db->set_query("inSert into equip_mapping_info(equip_idx, mapping_idx) values(%d, %d)",
                          it->second, it->first);
            if (!db->exec(true))
            {
                return false;
            }
        }
    }
    return true;
}

bool CSyncScript::truncate_mapping_equip_info_table()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    if (!db->set_query("trUncate table equip_mapping_info"))
    {
        return false;
    }
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::insert_random_option_ref()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    // ORIG 0x08625ea2：外层 prefix 为 byte，初值 1，==0xff 终止（cmpb $0xff/setne，
    // 即使用域 [1,0xfe]）；内层 grade 为 byte 0..0x64（cmpb $0x64/setbe ≤）。
    std::string name;   // ORIG：循环外构造一次，循环内 operator=
    for (unsigned char prefix = 1; prefix != 0xff; ++prefix)
    {
        for (unsigned char grade = 0; grade <= 0x64; ++grade)
        {
            name = G_CDataManager()->m_randomOption.getPrefix(prefix, grade, false);
            if (name.length() != 0)
            {
                char converted[0x400] = {0};
                // script2Database 失败仅记日志（ORIG 行 0xb31），继续入库；
                // ORIG 无 escape_string，转码结果直接入 SQL。
                if (!CodePage::script2Database((char*)name.c_str(), converted))
                {
                    cMyTrace(__PRETTY_FUNCTION__, 0xb31, 5)(
                        "Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
                        CodePage::database(), name.c_str(), __PRETTY_FUNCTION__,
                        0xb31);
                }
                db->set_query("inSert into random_option_ref(random_option_index, random_option_value, random_option_name) values(%d,%d, '%s')",
                              prefix, grade, converted);
                if (!db->exec(true))
                {
                    return false;
                }
            }
        }
    }
    return true;
}

bool CSyncScript::truncate_random_option_ref()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    if (!db->set_query("trUncate table random_option_ref"))
    {
        return false;
    }
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::insert_charac_action_point_desc()
{
    if (m_field0 != 1)
    {
        return true;
    }
    return APSystem::CSyncScript::InsertDescTable();
}

bool CSyncScript::truncate_charac_action_point_desc()
{
    if (m_field0 != 1)
    {
        return true;
    }
    return APSystem::CSyncScript::TruncateDescTable();
}

bool CSyncScript::insert_charac_advance_altar_item_desc()
{
    if (m_field0 != 1)
    {
        return true;
    }
    return advancealtar::SyncScript::insertItemDescTable();
}

bool CSyncScript::truncate_charac_advance_altar_item_desc()
{
    if (m_field0 != 1)
    {
        return true;
    }
    return advancealtar::SyncScript::truncateItemDescTable();
}

}  // namespace sync_script
