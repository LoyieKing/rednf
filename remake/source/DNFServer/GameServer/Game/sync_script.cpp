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
sync_script::CStringMaker makeString(It first, It last)
{
    return std::for_each(first, last, sync_script::CStringMaker());
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

// ---- CStringMaker 实现（ORIG 各符号均为 W 内联；唯一定义发射点） ----

CStringMaker::CStringMaker() {}                       // C1 0x08375f58
CStringMaker::CStringMaker(const CStringMaker& o)     // copy C1 0x0837f594
    : m_str(o.m_str)
{
}
CStringMaker::~CStringMaker() {}                      // D1 0x0837f580

void CStringMaker::operator()(int v)                  // 0x08375f6c "%d"
{
    char num[0xff];   // ORIG 缓冲 0xff 字节
    std::memset(num, 0, sizeof(num));
    sprintf(num, "%d", v);
    if (!m_str.empty())
    {
        m_str += ",";
    }
    m_str += num;
}

void CStringMaker::operator()(float v)                // 0x08626cd0 "%f"
{
    char num[0xff];   // ORIG 缓冲 0xff 字节
    std::memset(num, 0, sizeof(num));
    sprintf(num, "%f", v);
    if (!m_str.empty())
    {
        m_str += ",";
    }
    m_str += num;
}

void CStringMaker::operator()(SkillUseItem v)         // 0x08626d98 "%d,%d,%d"
{
    char num[0xff];   // ORIG 缓冲 0xff 字节
    std::memset(num, 0, sizeof(num));
    sprintf(num, "%d,%d,%d", v.m_index, v.m_useLevel[0], v.m_useLevel[1]);
    if (!m_str.empty())
    {
        m_str += ";";   // ORIG 分隔串 @0x8ce2df7 为 ';'
    }
    m_str += num;
}

void CStringMaker::operator()(EquipmentSkillDataUp v) // 0x08626e6e 七个 %d
{
    char num[0xff];   // ORIG 缓冲 0xff 字节
    std::memset(num, 0, sizeof(num));
    sprintf(num, "%d,%d,%d,%d,%d,%d,%d", v.m_job, v.m_v4, v.m_v8,
            v.m_percent, v.m_v10, v.m_key, v.m_module);
    if (!m_str.empty())
    {
        m_str += ";";
    }
    m_str += num;
}

void CStringMaker::operator()(LevelUpSkill v)         // 0x08626f7a job==0xb→99
{
    char num[0xff];   // ORIG 缓冲 0xff 字节
    std::memset(num, 0, sizeof(num));
    int job = (v.m_job == 0xb) ? 0x63 : v.m_job;
    sprintf(num, "%d,%d,%d", job, v.m_field4, v.m_field8);
    if (!m_str.empty())
    {
        m_str += ";";
    }
    m_str += num;
}

void CStringMaker::operator()(ItemAuraInfo v)         // 0x08627066 "%d,%d,%d,%d"
{
    char num[0xff];   // ORIG 缓冲 0xff 字节
    std::memset(num, 0, sizeof(num));
    // ORIG 参数栈：v+0 int / (uchar)v+4 / float v+8（vararg 提升 double）/
    // v+c int，fmt "%d,%d,%d,%d"。
    sprintf(num, "%d,%d,%d,%d", v.m_field0, (int)v.m_field4, v.m_float8,
            v.m_fieldc);
    if (!m_str.empty())
    {
        m_str += ";";
    }
    m_str += num;
}

void CStringMaker::operator()(std::vector<float>& values)  // 0x08627146
{
    if (values.empty())
    {
        return;
    }
    if (!m_str.empty())
    {
        m_str += ";";
    }
    for (std::vector<float>::iterator it = values.begin(); it != values.end();
         ++it)
    {
        if (it != values.begin())
        {
            m_str += ",";   // 元素间 ORIG 逗号 @0x8ce2dec
        }
    char num[0xff];   // ORIG 缓冲 0xff 字节
    std::memset(num, 0, sizeof(num));
        sprintf(num, "%f", *it);
        m_str += num;
    }
}

const char* CStringMaker::c_str()                     // 0x08376034
{
    return m_str.c_str();
}

CStringMaker::operator std::string()                  // 0x086272c0
{
    return m_str;
}

// CSyncScript 构造/析构由 CItemList.cpp（ORIG 0x861cbb4 区）提供；本 TU 不重复定义。

bool CSyncScript::GetType(std::string name, ENUM_CHARACTERJOB job, int idx,
                          int& out1, int& out2, bool flag)
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
        SWEAPONTYPE wp(job, idx);
        local = m_weaponMap[wp];
    }
    else if (m_vec1.size() >= 2 && m_vec1[1] == local.m_field1)
    {
        // ORIG 0x861d1d5：SARMORTYPE((uchar)idx, key)，key 为 "stackable "/名
        // 组合串（拷贝自 local_50），非去括号原名。
        SARMORTYPE ap((unsigned char)idx, key);
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

const char* CSyncScript::GetUserJob(ENUM_CHARACTERJOB job)
{
    // ORIG 0x861d2dc switch 跳表 → 常量字符串（GetJobString 同语义）。
    return GetJobString(job);
}

bool CSyncScript::insert_artifact_info_to_db(STEquipmentScript* script)
{
    // ORIG 0x08624b8e：未启用直接 return true；启用则向 dnf_artifact_info 写入
    // it_id（script+0）与 m_artifact738（script+0x738 起 11 列）。
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)5,
                                                  (ENUM_SERVER_GROUP)0);
    const STEquipmentScript::artifact_info& art = script->m_artifact738;
    if (!db->set_query(
            "inSert into dnf_artifact_info(it_id, creature_min_level, "
            "physical_attack, magical_attack, skill_consume_mp_rate, "
            "skill_charge_time_rate, skill_overcharge_time_rate, "
            "experience_amount, physical_critical_hit, magical_critical_hit, "
            "stuck) values(%d, %d, %d, %d, %f, %f, %f, %f, %f, %f, %f)",
            (int)script->m_field0, art.m_minLevel, art.m_physAttack,
            art.m_magAttack, art.m_mpRate, art.m_chargeRate,
            art.m_overChargeRate, art.m_expRate, art.m_physCrit,
            art.m_magCrit, art.m_stuck))
    {
        return false;
    }
    if (!db->exec(true))
    {
        return false;
    }
    return true;
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
        vec = &script->m_param1c0.m_levelUpSkills[0];
    }
    else if (vectorIndex == EquipmentParameterInfo::ENUM_SKILL_UP_VECTOR_INDEX_1)
    {
        vec = &script->m_param1c0.m_levelUpSkills[1];
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
        const char* job = GetUserJob((ENUM_CHARACTERJOB)skill.m_job);
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
                return GetUserJob((ENUM_CHARACTERJOB)i);
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
                return GetUserJob((ENUM_CHARACTERJOB)i);
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
    // ORIG 实证（0x08625064，size=1585）：本函数只有 3 个 _ZNSsC1Ev 调用点
    // （line/str/name）与 14 个 _ZNSsD1Ev。name 为**函数作用域单例**并在三个分支
    // 间复用（ScanStr 成功时整体覆盖），而非每分支各建一个具名局部；
    // 分支内声明会额外产生 2 个 _ZNSsC1Ev + 4 个 _ZNSsD1Ev（含 unwind 清理）。
    std::string name;
    // ORIG 0x08625093：循环条件在**循环体首部**以 `xor $1 / test / jne` 求反后
    // break（多出 1 个 xor + 2 个 nop），即 while(true){ if(!ScanType) break; }，
    // 而非 while(ScanType(...)) 的底部条件形态。
    while (true)
    {
        if (!ScanType(line, true))
        {
            break;
        }
        if (line == "[master type]")
        {
            int value = ScanInt((bool*)0);
            ScanStr(&str);
            m_map64[(unsigned char)value] = str;
        }
        else if (line == "[type addition]")
        {
            int value = ScanInt((bool*)0);
            m_vec1.push_back((unsigned char)value);
        }
        else if (line == "[reverse desc]")
        {
            int value = ScanInt((bool*)0);
            m_vec2.push_back((unsigned char)value);
        }
        else if (line == "[material type]")
        {
            int value = ScanInt((bool*)0);
            ScanStr(&str);
            m_map7c[(unsigned char)value] = str;
        }
        else if (line == "[weapon type]")
        {
            ScanStr(&str);
            int type = ScanInt((bool*)0);
            int subType = ScanInt((bool*)0);
            int value = ScanInt((bool*)0);
            ScanStr(&name);
            int job = getCharacterJob(str.c_str());
            // ORIG 0x086252d7..0x08625318：先构造右值 SITEMTYPE(local_64)，
            // 再构造下标键 SWEAPONTYPE(local_54)——键不是具名局部，
            // 而是 operator[] 实参处的临时（gcc 右→左求值）。
            m_weaponMap[SWEAPONTYPE((ENUM_CHARACTERJOB)job, subType)] =
                SITEMTYPE(false, (unsigned char)type,
                          (unsigned char)subType,
                          (unsigned short)value, name);
        }
        else if (line == "[armor type]")
        {
            ScanStr(&str);
            int type = ScanInt((bool*)0);
            int subType = ScanInt((bool*)0);
            int value = ScanInt((bool*)0);
            ScanStr(&name);
            // ORIG 0x08625408..0x0862545e：同上，SITEMTYPE(local_4c) 先建，
            // SARMORTYPE(local_3c) 作为 operator[] 处的临时键后建。
            m_armorMap[SARMORTYPE((unsigned char)subType, str)] =
                SITEMTYPE(false, (unsigned char)type,
                          (unsigned char)subType,
                          (unsigned short)value, name);
        }
        else if (line == "[item type]")
        {
            ScanStr(&str);
            int type = ScanInt((bool*)0);
            int subType = ScanInt((bool*)0);
            int value = ScanInt((bool*)0);
            int extra = ScanInt((bool*)0);
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
    char name[0xff];
    memset(name, 0, sizeof(name));
    char escaped[0xbf];
    memset(escaped, 0, sizeof(escaped));
    if (!CodePage::script2Database((char*)script->m_str4.c_str(), name))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x6e0, 5)(
            "Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
            CodePage::database(), name, __PRETTY_FUNCTION__, 0x6e0);
    }
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
    char name[0x40];
    memset(name, 0, sizeof(name));
    char escaped[0x40];
    memset(escaped, 0, sizeof(escaped));
    if (!CodePage::script2Database((char*)script->m_name.c_str(), name))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x6a4, 5)(
            "Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
            CodePage::database(), script->m_name.c_str(),
            __PRETTY_FUNCTION__, 0x6a4);
    }
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
    static char aicharacter_info_name[0xf0];
    memset(aicharacter_info_name, 0, sizeof(aicharacter_info_name));
    char escaped[0x400];
    memset(escaped, 0, sizeof(escaped));
    // ORIG：script2Database 输出到 escaped（栈），escape_string 目标与 set_query 用 static 缓冲。
    if (!CodePage::script2Database((char*)script.m_str4.c_str(), escaped))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xb95, 5)(
            "Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
            CodePage::database(), script.m_str4.c_str(),
            __PRETTY_FUNCTION__, 0xb95);
    }
    db->escape_string(aicharacter_info_name, escaped);
    db->set_query("inSert into dnf_aicharacter_info(idx, ai_name_kr) values(%d, '%s')",
                  idx, aicharacter_info_name);
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
    static char town_name[0xf0];
    memset(town_name, 0, sizeof(town_name));
    char escaped[0x400];
    memset(escaped, 0, sizeof(escaped));
    // ORIG：script2Database 输出到 escaped（栈），escape_string 目标与 set_query 用 static 缓冲。
    if (!CodePage::script2Database((char*)script.m_str4.c_str(), escaped))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xbc2, 5)(
            "Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
            CodePage::database(), script.m_str4.c_str(),
            __PRETTY_FUNCTION__, 0xbc2);
    }
    db->escape_string(town_name, escaped);
    db->set_query("inSert into dnf_town_info(idx, town_name_kr) values(%d, '%s')",
                  script.m_field0, town_name);
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
    static char monster_name[0xf0];
    memset(monster_name, 0, sizeof(monster_name));
    char escaped[0x400];
    memset(escaped, 0, sizeof(escaped));
    // ORIG：script2Database 输出到 escaped（栈），escape_string 目标与 set_query 用 static 缓冲。
    if (!CodePage::script2Database((char*)script.m_name.c_str(), escaped))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xb63, 5)(
            "Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
            CodePage::database(), script.m_name.c_str(),
            __PRETTY_FUNCTION__, 0xb63);
    }
    db->escape_string(monster_name, escaped);
    db->set_query("inSert into dnf_monster_info(idx, mon_name_kr) values(%d, '%s')",
                  script.m_index, monster_name);
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
    static const int BUFSIZE = 0x100;  // ORIG _ZZN11sync_script11CSyncScript26insert_creature_info_to_db…E7BUFSIZE
    // ORIG：artifact_slot 列 = '0'+(R/G/B 三槽标志) 的 3 字符串
    // （CreatureScript+0x50/0x54/0x58，asm 0x08624437..0x0862445d，
    // 低字节存 (v+'0')&0xff；缓冲 movl$0 预清零即 NUL 结尾）。
    // ORIG local_5c 为 `movl $0x0` 单指令清零（聚合初始化），不是 memset 调用。
    char artifactSlot[4] = {0};
    // ORIG：直接按 +0x50+i*4 读三槽标志（无中间数组）；上界写成 sizeof-1
    //（ORIG 0x0862444f 为 `mov i,%eax; cmp $0x2,%eax; setbe`：有符号 i 与
    // size_t 上界比较产生的转换临时，非 `cmpl $imm,mem` 直比形态）。
    for (int i = 0; i < sizeof(artifactSlot) - 1; ++i)
    {
        artifactSlot[i] = (char)((&script->m_artifactSlotRed)[i] + '0');
    }
    // ORIG 0x0862448c/0x08624562/0x08624635：三处均为
    //   CStringMaker::CStringMaker(临时 functor)
    //   → std::for_each<__normal_iterator<int const*,vector<int>>, CStringMaker>
    //   → CStringMaker::operator std::string()（sret 直接产出结果串）
    //   → ~CStringMaker ×2
    // 即 ORIG 无 c_str() → std::string(const char*, allocator) 的二次构造；
    // 走 makeString(...).c_str() 会多出 _ZNSaIcEC1Ev ×3 / _ZNSsC1EPKcRKSaIcE ×3
    // （及 _ZNSaIcED1Ev ×6）这些 ORIG 不存在的 eager 构造调用点。
    const std::string piercingStr =
        std::for_each(script->m_piercing.begin(), script->m_piercing.end(),
                      CStringMaker());
    const std::string skillLevelValuesStr =
        std::for_each(script->m_skillLevelValues.begin(),
                      script->m_skillLevelValues.end(), CStringMaker());
    const std::string overSkillLevelValuesStr =
        std::for_each(script->m_overSkillLevelValues.begin(),
                      script->m_overSkillLevelValues.end(), CStringMaker());
    // ORIG：values 区 11 个 dword 就地清零（rep stos 0xb），非 memset 调用。
    const char* values[11] = {0};
    values[0] = script->m_name.c_str();
    values[1] = artifactSlot;
    values[2] = param3;
    values[3] = param4;
    values[4] = piercingStr.c_str();
    // ORIG：skill_name/skill_desc/overskill_name/overskill_desc ←
    // m_skillString（vector<string> @+0x128）前 4 项，越界项用 ""；
    // ORIG 为 4 段展开（size() ×4 + at() ×4），非循环 + operator[]。
    values[5] = script->m_skillString.size() == 0
                    ? "" : script->m_skillString.at(0).c_str();
    values[6] = script->m_skillString.size() < 2
                    ? "" : script->m_skillString.at(1).c_str();
    values[7] = script->m_skillString.size() < 3
                    ? "" : script->m_skillString.at(2).c_str();
    values[8] = script->m_skillString.size() < 4
                    ? "" : script->m_skillString.at(3).c_str();
    values[9] = skillLevelValuesStr.c_str();
    values[10] = overSkillLevelValuesStr.c_str();
    // ORIG：escaped 区 0x2c0 个 dword 就地清零（= 11*0x100 字节），非 memset 调用；
    // GetDBHandle 在清零之后才取。
    char escaped[11][BUFSIZE] = {{0}};
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    // ORIG 0x08624a0e：同为 `mov i,%eax; cmp $0xa,%eax; setbe`（size_t 上界）。
    for (int i = 0; i < sizeof(values) / sizeof(values[0]); ++i)
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
    // ORIG 0x08624a5f：exec 结果经 `xor $1 / test` 分支回填 0/1，
    // 而非 `return db->exec(true)` 的直接搬运。
    if (!db->exec(true))
    {
        return false;
    }
    return true;
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
    static const int BUFSIZE = 0x64;  // ORIG _ZZN11sync_script11CSyncScript27insert_one_item_info_master…E7BUFSIZE
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    const char* names[2];
    names[0] = toMbcs(name);
    names[1] = toMbcs(explain);
    char escaped[2][BUFSIZE];
    memset(escaped, 0, 0x32);  // ORIG：仅清 0x32
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
    static const int BUFSIZE = 0x64;  // ORIG _ZZN11sync_script11CSyncScript29insert_item_info_master_to_dbEvE7BUFSIZE
    if (m_field0 != 1)
    {
        return true;
    }
    for (std::map<SWEAPONTYPE, SITEMTYPE>::iterator it = m_weaponMap.begin();
         it != m_weaponMap.end(); ++it)
    {
        char name[0x100];
        char full[0x100];
        memset(full, 0, 0x19);   // ORIG：memset 0x19（先 full 后 name）
        memset(name, 0, 0x19);
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
        memset(full, 0, 0x19);   // ORIG：memset 0x19（先 full 后 name）
        memset(name, 0, 0x19);
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
