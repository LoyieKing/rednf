// df_game_r CItemList 还原（2026-08-16 / 2026-08-17 第九批 I1 收尾）。
// 成员布局以 ORIG 构造/析构为基准；函数形态以 class_func_reports/CItemList.md
// 与 class_func_reports/sync_script__CSyncScript.md 逐条对照
// （AE 口径：调用目标/全局地址归一，指令形态逐条一致）。

#include "CItemList.h"

#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <utility>

#include "GameTypes.h"
#include "GlobalData.h"
#include "CDataManager.h"
#include "CItemAmplifier.h"
#include "MySQL.h"
#include "DBMgr.h"
#include "DNFFunctionLib.h"

namespace sync_script
{
class CSyncScript;
}

// ============================================================================
// sync_script 域辅助类型（TU 私有；ORIG 对应符号属后续批次，调用目标在 AE
// 口径下归一，这里只保证尺寸/调用形态一致）。
// ============================================================================
namespace
{
// ORIG CStreamBuffer：0xa000 字节（栈上对象），operator<< 返回流引用。
class MyStream
{
public:
    MyStream() {}
    MyStream& operator<<(const char* s);
    MyStream& operator<<(int v);
    MyStream& operator<<(unsigned int v);
    MyStream& operator<<(float v);
    char* c_str() { return m_buf; }

    char m_buf[0xa000];   // +0x00
};

// ORIG CFieldLengthChecker：12 字节对象（ctor/AddField/dtor）。
class FieldChecker
{
public:
    FieldChecker() {}
    void AddField(char* query);
    ~FieldChecker() {}
    char m_pad[12];       // +0x00
};

// ORIG sync_script::CStringMaker：for_each 累加器（ctor/()/c_str/dtor）。
class StringMaker
{
public:
    StringMaker() {}
    template <class T> void operator()(const T& v) {}
    const char* c_str() const { return m_str; }
    ~StringMaker() {}
    const char* m_str;   // +0x00
    char m_pad[12];      // +0x04
};

char* EscapeDB(MySQL* db, const char* src, char* dst);
char* URLString(sync_script::CSyncScript* self, const char* str, int len,
                int idx);
char* UsableCharString(sync_script::CSyncScript* self, const void* script);
char* AttachTypeString(sync_script::CSyncScript* self, int type);
char* AttackElementString(sync_script::CSyncScript* self, const void* param);
char SocketTypeChar(unsigned short type);
bool StackableGetType(sync_script::CSyncScript* self, std::string& name,
                      int job, int idx, int& out1, int& out2, bool flag);
void InsertArtifact(sync_script::CSyncScript* self, void* script);

MyStream& MyStream::operator<<(const char* s)
{
    strcat(m_buf, s);
    return *this;
}

MyStream& MyStream::operator<<(int v)
{
    char tmp[32];
    sprintf(tmp, "%d", v);
    strcat(m_buf, tmp);
    return *this;
}

MyStream& MyStream::operator<<(unsigned int v)
{
    char tmp[32];
    sprintf(tmp, "%u", v);
    strcat(m_buf, tmp);
    return *this;
}

MyStream& MyStream::operator<<(float v)
{
    char tmp[32];
    sprintf(tmp, "%f", v);
    strcat(m_buf, tmp);
    return *this;
}

void FieldChecker::AddField(char* query)
{
}

char* EscapeDB(MySQL* db, const char* src, char* dst)
{
    strcpy(dst, src);
    return dst;
}

char* URLString(sync_script::CSyncScript* self, const char* str, int len,
                int idx)
{
    return (char*)str;
}

char* UsableCharString(sync_script::CSyncScript* self, const void* script)
{
    return (char*)"";
}

char* AttachTypeString(sync_script::CSyncScript* self, int type)
{
    return (char*)"";
}

char* AttackElementString(sync_script::CSyncScript* self, const void* param)
{
    return (char*)"";
}

char SocketTypeChar(unsigned short type)
{
    return (char)type;
}

bool StackableGetType(sync_script::CSyncScript* self, std::string& name,
                      int job, int idx, int& out1, int& out2, bool flag)
{
    return true;
}

void InsertArtifact(sync_script::CSyncScript* self, void* script)
{
}

// ORIG STStackableScript 字段镜像（insert_stackable_item_to_db 访问位）。
struct StackScriptFull
{
    int m_field0;                     // +0x00
    int m_field4;                     // +0x04
    int m_field8;                     // +0x08
    int m_fieldc;                     // +0x0c
    int m_field10;                    // +0x10
    int m_field14;                    // +0x14
    int m_field18;                    // +0x18
    int m_field1c;                    // +0x1c
    char m_pad20[0x14];               // +0x20
    int m_field34;                    // +0x34
    char m_pad38[0x24];               // +0x38
    int m_field5c;                    // +0x5c
    char m_pad60[8];                  // +0x60
    int m_field68;                    // +0x68
    std::string m_str6c;              // +0x6c
    int m_field70;                    // +0x70
    char m_pad74[0xac - 0x74];        // +0x74
    std::string m_strac;              // +0xac
    std::string m_strb0;              // +0xb0
    std::string m_strb4;              // +0xb4
    std::string m_strb8;              // +0xb8
    std::string m_strbc;              // +0xbc
    int m_fieldc0;                    // +0xc0
    int m_fieldc4;                    // +0xc4
    char m_padc8[0x4bc - 0xc8];
    std::map<int, void*> m_map4bc;    // +0x4bc（ORIG map<ENUM_MODULE_TYPE, EquipmentParameterInfo*>）
};

// ORIG EquipmentParameterInfo 字段镜像（insert_stackable/insert_equiptment 访问位）。
struct EquipParamInfo
{
    char m_pad00[0x188];
    std::vector< ::ItemAuraInfo> m_vec188;   // +0x188
    char m_pad194[0x1b8 - 0x194];
    std::vector< ::EquipmentSkillDataUp> m_vec1b8;  // +0x1b8
    char m_pad1c4[0x24c - 0x1c4];
    std::vector< ::LevelUpSkill> m_levelUpSkills;  // +0x24c
    char m_pad258[0x264 - 0x258];
    std::vector< ::STAvatarAbilityVariation> m_vec264;  // +0x264
    char m_pad270[0x480 - 0x270];
};

struct stAvatarTypeSelectFull
{
    int m_field0;                            // +0x00
    int m_field4;                            // +0x04
    int m_field8;                            // +0x08
    int m_fieldc;                            // +0x0c
    int m_field10;                           // +0x10
    unsigned short m_ushort14[8];            // +0x14
};

struct STEquipmentAniScript
{
    char m_pad[0x60];
    std::string m_str60;                     // +0x60
};

// ORIG STEquipmentScript 字段镜像（insert_equiptment_item_to_db 访问位）。
struct EquipScriptFull
{
    int m_field0;                          // +0x00
    int m_field4;                          // +0x04
    int m_field8;                          // +0x08
    int m_fieldc;                          // +0x0c
    int m_field10;                         // +0x10
    int m_field14;                         // +0x14
    int m_field18;                         // +0x18
    int m_field1c;                         // +0x1c
    int m_field20;                         // +0x20
    int m_field24;                         // +0x24
    int m_field28;                         // +0x28
    int m_field2c;                         // +0x2c
    int m_field30;                         // +0x30
    int m_field34;                         // +0x34
    char m_aniFlags[0xb];              // +0x38（11 个 ani 标志字节）
    char m_pad43[0xb4 - 0x43];
    std::string m_strb4;               // +0xb4
    std::string m_strbc;                   // +0xbc
    int m_fieldc0;                         // +0xc0
    int m_fieldc4;                         // +0xc4
    char m_padc8[0x184 - 0xc8];
    int m_field184;                        // +0x184
    char m_pad188[0x198 - 0x188];
    std::vector<int> m_vec198;             // +0x198
    char m_pad1a4[0x1b4 - 0x1a4];
    int m_field1b4;                        // +0x1b4
    int m_field1b8;                        // +0x1b8
    char m_pad1bc[0x1c0 - 0x1bc];
    EquipParamInfo m_equipParam;           // +0x1c0（0x480 字节 → +0x640）
    char m_pad640[0x658 - 0x640];
    std::vector<std::string> m_vec658;     // +0x658
    std::string m_str664;                  // +0x664
    std::string m_str668;                  // +0x668
    std::vector<int> m_vec66c;             // +0x66c
    std::map<int, EquipParamInfo> m_map678;  // +0x678
    char m_pad690[0x6a0 - 0x690];
    int m_field6a0;                        // +0x6a0
    std::string m_str6a4;                  // +0x6a4
    std::string m_str6a8;                  // +0x6a8
    std::string m_str6ac;                  // +0x6ac
    std::string m_str6b0;                  // +0x6b0
    std::string m_str6b4;                  // +0x6b4
    std::string m_str6b8;                  // +0x6b8
    char m_pad6bc[0x6c0 - 0x6bc];
    std::vector<stAvatarTypeSelectFull> m_vec6c0;  // +0x6c0
    std::vector<unsigned short> m_vec6cc;  // +0x6cc
    char m_pad6d8[0x768 - 0x6d8];
    std::vector<ENUM_EQUIPMENTTYPE> m_vec768;   // +0x768
    std::vector<int> m_vec774;             // +0x774
    std::vector<int> m_vec780;             // +0x780
    int m_field78c;                        // +0x78c
    int m_field790;                        // +0x790
    int m_field794;                        // +0x794
    int m_field798;                        // +0x798
    int m_field79c;                        // +0x79c
    char m_pad7a0[0x878 - 0x7a0];
    std::string m_str878;                  // +0x878
};

std::vector<STEquipmentAniScript>* AniInfoVector(void* script, int job)
{
    static std::vector<STEquipmentAniScript> dummy;
    return &dummy;
}
}

// ===================== 外部依赖（其它 TU 提供，仅声明） =====================

namespace sync_script
{
// 布局依据 ORIG 构造（0x861cc6c）/ 析构（0x861cd7a）：
//   +0x00 bool m_field0
//   +0x04 vector<unsigned char>（12 字节）
//   +0x10 vector<unsigned char>
//   +0x1c map<string, SITEMTYPE>
//   +0x34 map<SWEAPONTYPE, SITEMTYPE>
//   +0x4c map<SARMORTYPE, SITEMTYPE>
//   +0x64 map<unsigned char, string>
//   +0x7c map<unsigned char, string>
struct SITEMTYPE
{
    char m_field0;         // +0x00
    char m_pad1[3];
    unsigned short m_field4;  // +0x04（ORIG GetType 读 *(ushort*)(p+4)）
};

struct SWEAPONTYPE
{
    int m_field0;
    int m_field4;
    bool operator<(const SWEAPONTYPE& o) const
    {
        return m_field0 < o.m_field0 ||
               (m_field0 == o.m_field0 && m_field4 < o.m_field4);
    }
};

struct SARMORTYPE
{
    unsigned char m_field0;
    std::string m_str4;
    bool operator<(const SARMORTYPE& o) const
    {
        return m_field0 < o.m_field0 ||
               (m_field0 == o.m_field0 && m_str4 < o.m_str4);
    }
};

class CSyncScript
{
public:
    CSyncScript();
    ~CSyncScript();

    bool truncate_dnf_item_info_table();
    bool insert_equiptment_item_to_db(STEquipmentScript* script,
                                     unsigned int idx, int mode, int key);
    bool insert_stackable_item_to_db(STStackableScript* script);
    bool truncateCeraShopItemTable();
    bool insertCeraShopItemIntoDB(unsigned int itemIdx, unsigned int itemType,
                                  int value, int count, short c,
                                  const char* name, const char* iconName);
    bool truncate_artifact_info_table();

    bool m_field0;                                   // +0x00
    std::vector<unsigned char> m_vec1;               // +0x04
    std::vector<unsigned char> m_vec2;               // +0x10
    std::map<std::string, SITEMTYPE> m_itemTypeMap;  // +0x1c
    std::map<SWEAPONTYPE, SITEMTYPE> m_weaponMap;    // +0x34
    std::map<SARMORTYPE, SITEMTYPE> m_armorMap;      // +0x4c
    std::map<unsigned char, std::string> m_map64;    // +0x64
    std::map<unsigned char, std::string> m_map7c;    // +0x7c
};

std::map<int, std::string> g_equipItemInfoTable;      // ORIG 0x94fac00
std::map<int, std::string> g_stackableItemInfoTable;  // ORIG 0x94fdc00

namespace
{
// ORIG 里是 CFieldLengthChecker::fieldNames_ / fieldLength_（0x94734a0 /
// 0x94734ac）。引用地址在 AE 口径下归一，本地全局即可。
std::vector<std::string> g_fieldNames;
std::vector<unsigned int> g_fieldLengths;
}

CSyncScript* GetInstanceSyncScript()                  // ORIG 0x861cbb4
{
    static CSyncScript inst;
    return &inst;
}

void DisplayMaxLength()                               // ORIG 0x861ca94
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    for (unsigned int i = 0; i < g_fieldNames.size(); ++i)
    {
        if (g_fieldLengths[i] == 0)
        {
            continue;
        }
        db->set_query("seLect max(length(%s)) from dnf_item_info",
                      g_fieldNames[i].c_str());
        if (!db->exec(true))
        {
            continue;
        }
        if (!db->fetch())
        {
            continue;
        }
        unsigned int len = 0;
        if (!db->get_uint(0, len))
        {
            continue;
        }
        if (g_fieldLengths[i] != len) { }
    }
}

CSyncScript::CSyncScript()
{
    m_field0 = 0;
}

CSyncScript::~CSyncScript()
{
    m_vec1.clear();
    m_vec2.clear();
    m_itemTypeMap.clear();
    m_weaponMap.clear();
    m_armorMap.clear();
    m_map64.clear();
    m_map7c.clear();
}

bool CSyncScript::truncate_dnf_item_info_table()      // ORIG 0x8622374
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    db->set_query("trUncate table dnf_item_info");
    if (!db->exec(true))
    {
        return false;
    }
    db->set_query("trUncate table avatar_select_ability");
    if (!db->exec(true))
    {
        return false;
    }
    if (!truncate_artifact_info_table())
    {
        return false;
    }
    return true;
}

bool CSyncScript::truncate_artifact_info_table()      // ORIG 0x8624b0a
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    if (!db->set_query("trUncate table dnf_artifact_info"))
    {
        return false;
    }
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::truncateCeraShopItemTable()         // ORIG 0x8622a5a
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    db->set_query("trUncate table dnf_item_price; ");
    return db->exec(true);
}

bool CSyncScript::insert_equiptment_item_to_db(STEquipmentScript* script,
                                              unsigned int idx, int mode,
                                              int key)
{
    if (m_field0 != 1)
    {
        return true;
    }
    EquipScriptFull* es = (EquipScriptFull*)script;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    unsigned int local_154 = 0;
    int local_158 = 0;
    int foundAni = 0;
    for (int i = 0; i <= 0xa; ++i)
    {
        if (*(char*)&es->m_aniFlags[i] != 0)
        {
            foundAni = i;
            break;
        }
    }
    int local_74 = 0;
    unsigned int param_2 = idx;
    if (mode == 1)
    {
        local_74 = (int)param_2;
        param_2 = (unsigned int)-1;
    }
    else if (mode == 2)
    {
        local_74 = (int)param_2;
        param_2 = (unsigned int)-2;
    }
    else if (mode == 0)
    {
        std::string typeName = es->m_strbc;
        if (!StackableGetType(this, typeName, foundAni, es->m_field1b8,
                              (int&)local_154, local_158, true))
        {
            return false;
        }
        param_2 = (unsigned int)es->m_field0;
        std::map<int, EquipParamInfo>::iterator it = es->m_map678.begin();
        while (it != es->m_map678.end())
        {
            EquipScriptFull temp;
            temp.m_equipParam = it->second;
            insert_equiptment_item_to_db((STEquipmentScript*)&temp, param_2, 1,
                                        it->first);
            ++it;
        }
    }
    char local_162[10];
    local_162[0] = 0;
    local_162[1] = 0;
    local_162[2] = 0;
    local_162[3] = 0;
    local_162[4] = 0;
    int socketCount = 0;
    if (!es->m_vec6cc.empty())
    {
        for (std::vector<unsigned short>::iterator vit = es->m_vec6cc.begin();
             vit != es->m_vec6cc.end() && socketCount <= 4; ++vit)
        {
            local_162[socketCount + 5] = SocketTypeChar(*vit);
            ++socketCount;
        }
    }
    if (!es->m_vec6c0.empty())
    {
        for (std::vector<stAvatarTypeSelectFull>::iterator ait =
                 es->m_vec6c0.begin();
             ait != es->m_vec6c0.end(); ++ait)
        {
            if (ait->m_field10 > 0)
            {
                for (int k = 0; k < ait->m_field10 && socketCount < 5;
                     ++socketCount)
                {
                    local_162[socketCount + 5] =
                        SocketTypeChar(ait->m_ushort14[8 + k]);
                    ++k;
                }
            }
        }
    }
    char local_1fc[64] = {0};
    int equipCount = 0;
    if (!es->m_vec768.empty())
    {
        for (std::vector<ENUM_EQUIPMENTTYPE>::iterator eit =
                 es->m_vec768.begin();
             eit != es->m_vec768.end(); ++eit)
        {
            char tmp[8] = {0};
            if (equipCount == 0)
            {
                sprintf(tmp, "%d", (int)*eit);
            }
            else
            {
                sprintf(tmp, "*%d", (int)*eit);
            }
            strcat(local_1fc, tmp);
            ++equipCount;
        }
    }
    char local_2fc[256] = {0};
    char local_3fc[256] = {0};
    char local_4fc[256] = {0};
    char local_5fc[256] = {0};
    char local_69 = 0;
    for (int i = 0; i <= 0xa; ++i)
    {
        if (*(char*)&es->m_aniFlags[i] == 0)
        {
            continue;
        }
        std::vector<STEquipmentAniScript>* anis = AniInfoVector(script, i);
        if (anis->empty())
        {
            continue;
        }
        int f30 = *(int*)((char*)&(*anis)[0] + 0x30);
        int f34 = *(int*)((char*)&(*anis)[0] + 0x34);
        int fc_ = *(int*)((char*)&(*anis)[0] + 0xc);
        int f10 = *(int*)((char*)&(*anis)[0] + 0x10);
        if ((f30 != -1 || f34 != -1) && (fc_ == -1 && f10 == -1))
        {
            char tmp[256] = {0};
            sprintf(tmp, "%02d%02d", f30, f34);
            if (local_69 != 0)
            {
                char t2[8] = {0};
                sprintf(t2, "*%d", (int)anis->size());
                strcat(tmp, t2);
            }
            char t3[8] = {0};
            sprintf(t3, "*%d", (int)anis->size());
            strcat(tmp, t3);
            for (std::vector<STEquipmentAniScript>::iterator ait =
                     anis->begin();
                 ait != anis->end(); ++ait)
            {
                strcat(tmp, "*");
                char clean[256] = {0};
                strcpy(clean, ait->m_str60.c_str());
                int w = 0;
                for (unsigned int r = 0; r < strlen(clean); ++r)
                {
                    if (clean[r] != '_')
                    {
                        clean[w++] = clean[r];
                    }
                }
                clean[w] = 0;
                strcat(tmp, clean);
            }
            if (local_69 == 1)
            {
                strcpy(local_4fc, tmp);
            }
            else
            {
                strcpy(local_2fc, tmp);
                local_69 = 1;
            }
        }
        else if (fc_ != -1 || f10 != -1)
        {
            char tmp[256] = {0};
            sprintf(tmp, "%02d%02d", fc_, f10);
            if (local_69 != 0)
            {
                char t2[8] = {0};
                sprintf(t2, "*%d", 0);
                strcat(tmp, t2);
            }
            char t3[8] = {0};
            sprintf(t3, "*%d", 0);
            strcat(tmp, t3);
            std::vector<STEquipmentAniScript>* sub =
                (std::vector<STEquipmentAniScript>*)((char*)anis + 0x18);
            for (std::vector<STEquipmentAniScript>::iterator ait =
                     sub->begin();
                 ait != sub->end(); ++ait)
            {
                strcat(tmp, "*");
                char clean[256] = {0};
                strcpy(clean, ait->m_str60.c_str());
                int w = 0;
                for (unsigned int r = 0; r < strlen(clean); ++r)
                {
                    if (clean[r] != '_')
                    {
                        clean[w++] = clean[r];
                    }
                }
                clean[w] = 0;
                strcat(tmp, clean);
            }
            if (local_69 == 1)
            {
                strcpy(local_5fc, tmp);
            }
            else
            {
                strcpy(local_3fc, tmp);
                local_69 = 1;
            }
        }
    }
    local_162[0] = 0;
    local_162[1] = 0;
    local_162[2] = 0;
    local_162[3] = 0;
    local_162[4] = 0;
    if (!es->m_vec198.empty())
    {
        int cnt = 0;
        for (std::vector<int>::iterator iit = es->m_vec198.begin();
             iit != es->m_vec198.end() && cnt <= 4; ++iit)
        {
            local_162[cnt] = (char)*iit + '0';
            ++cnt;
        }
    }
    char local_3e90[0x2c00] = {0};
    int local_68 = 4;
    std::string local_168;
    if (es->m_strb4.empty())
    {
        local_168 = es->m_str6a4;
    }
    else
    {
        local_168 = es->m_strb4;
    }
    char local_9fc[1024] = {0};
    if (!CodePage::script2Database((char*)es->m_str6ac.c_str(), local_9fc))
    {
        cMyTrace(
            "bool sync_script::CSyncScript::insert_equiptment_item_to_db(STEquipmentScript*, unsigned int, int, int)",
            0x398, 5)("Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
                      CodePage::database(), es->m_str6ac.c_str(),
                      "bool sync_script::CSyncScript::insert_equiptment_item_to_db(STEquipmentScript*, unsigned int, int, int)",
                      0x398);
        return false;
    }
    char local_1290[1024] = {0};
    if (!CodePage::script2Database((char*)local_168.c_str(), local_1290))
    {
        cMyTrace(
            "bool sync_script::CSyncScript::insert_equiptment_item_to_db(STEquipmentScript*, unsigned int, int, int)",
            0x39f, 5)("Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
                      CodePage::database(), es->m_strb4.c_str(),
                      "bool sync_script::CSyncScript::insert_equiptment_item_to_db(STEquipmentScript*, unsigned int, int, int)",
                      0x39f);
        return false;
    }
    MyStream buf;
    buf << param_2;
    buf << ", '" << EscapeDB(db, local_9fc, local_3e90) << "'";
    buf << ", '"
        << EscapeDB(db, (char*)es->m_str6b0.c_str(), local_3e90 + 0x400)
        << "'";
    buf << ", '" << EscapeDB(db, local_1290, local_3e90 + 0x800) << "'";
    buf << ", " << local_158;
    buf << ", " << local_154;
    buf << ", '" << UsableCharString(this, script) << "'";
    buf << ", " << es->m_field4;
    buf << ", '" << AttachTypeString(this, es->m_field8) << "'";
    buf << ", " << *(int*)((char*)es + 0x5c);
    buf << ", " << *(int*)((char*)es + 0x184);
    buf << ", " << es->m_fieldc;
    buf << ", " << es->m_field10;
    buf << ", " << es->m_field34;
    buf << ", " << es->m_field14;
    buf << ", " << es->m_field18;
    buf << ", " << es->m_field1c;
    buf << ", " << *(int*)((char*)es + 0x1bc);
    buf << ", " << *(int*)((char*)es + 0x68);
    buf << ", " << *(int*)((char*)es + 0x1c4);
    buf << ", " << *(int*)((char*)es + 0x1cc);
    buf << ", " << *(int*)((char*)es + 0x1d4);
    buf << ", " << *(int*)((char*)es + 0x1d8);
    buf << ", " << *(int*)((char*)es + 0x1dc);
    buf << ", " << *(int*)((char*)es + 0x1e4);
    buf << ", " << *(int*)((char*)es + 0x214);
    buf << ", " << *(int*)((char*)es + 0x21c);
    buf << ", " << *(int*)((char*)es + 0x224);
    buf << ", " << *(int*)((char*)es + 0x22c);
    buf << ", " << *(int*)((char*)es + 0x270);
    buf << ", " << *(int*)((char*)es + 0x274);
    buf << ", " << *(int*)((char*)es + 0x278);
    buf << ", " << *(int*)((char*)es + 0x27c);
    buf << ", " << *(int*)((char*)es + 0x280);
    buf << ", " << *(int*)((char*)es + 0x284);
    buf << ", " << *(int*)((char*)es + 0x288);
    buf << ", " << *(int*)((char*)es + 0x28c);
    buf << ", " << *(int*)((char*)es + 0x290);
    buf << ", " << *(int*)((char*)es + 0x294);
    buf << ", " << *(int*)((char*)es + 0x298);
    buf << ", " << *(int*)((char*)es + 0x29c);
    buf << ", " << *(int*)((char*)es + 0x2a0);
    buf << ", " << *(int*)((char*)es + 0x2a4);
    buf << ", " << *(int*)((char*)es + 0x2a8);
    buf << ", " << *(int*)((char*)es + 0x2ac);
    buf << ", " << *(int*)((char*)es + 0x2b0);
    buf << ", " << *(int*)((char*)es + 0x2b4);
    buf << ", " << *(int*)((char*)es + 0x2b8);
    buf << ", " << *(int*)((char*)es + 0x2bc);
    buf << ", " << *(int*)((char*)es + 0x2c0);
    buf << ", " << *(int*)((char*)es + 0x2c4);
    buf << ", " << *(int*)((char*)es + 0x2c8);
    buf << ", " << *(int*)((char*)es + 0x2cc);
    buf << ", " << *(int*)((char*)es + 0x2d0);
    buf << ", " << *(int*)((char*)es + 0x2d4);
    buf << ", " << *(int*)((char*)es + 0x2dc);
    buf << ", " << *(int*)((char*)es + 0x2e8);
    buf << ", " << *(int*)((char*)es + 0x2ec);
    buf << ", " << *(int*)((char*)es + 0x2f0);
    buf << ", " << *(int*)((char*)es + 0x2f4);
    buf << ", '" << AttackElementString(this, &es->m_equipParam) << "'";
    buf << ", " << *(int*)((char*)es + 0x304);
    buf << ", " << *(float*)((char*)es + 0x308);
    buf << ", " << *(int*)((char*)es + 0x30c);
    buf << ", " << *(int*)((char*)es + 0x3e0);
    buf << ", " << *(int*)((char*)es + 0x3e4);
    buf << ", " << (unsigned int)(unsigned char)*(char*)((char*)es + 0x3ec);
    buf << ", " << (unsigned int)(unsigned char)*(char*)((char*)es + 0x3fc);
    buf << ", " << (unsigned int)(unsigned char)*(char*)((char*)es + 0x3fd);
    buf << ", " << *(float*)((char*)es + 0x2f8);
    buf << ", " << *(float*)((char*)es + 0x3e8);
    buf << ", " << (unsigned int)(unsigned char)*(char*)((char*)es + 0x3ec);
    buf << ", '"
        << std::for_each(es->m_equipParam.m_levelUpSkills.begin(), es->m_equipParam.m_levelUpSkills.end(),
                         StringMaker()).c_str()
        << "'";
    const char* yn = "n";
    if (es->m_str668.empty() && es->m_vec66c.empty() &&
        *(int*)((char*)es + 0x6a0) == -1 &&
        *(int*)((char*)es + 0x798) == -1)
    {
        yn = "n";
    }
    else
    {
        yn = "y";
    }
    buf << ", '" << yn << "'";
    buf << ", '" << local_162 + 5 << "'";
    int iVar = local_68 * 0x400;
    ++local_68;
    buf << ", '"
        << EscapeDB(db, (char*)es->m_str6ac.c_str(), local_3e90 + iVar)
        << "'";
    iVar = local_68 * 0x400;
    ++local_68;
    buf << ", '"
        << EscapeDB(db, (char*)es->m_str6b8.c_str(), local_3e90 + iVar)
        << "'";
    buf << ", " << *(int*)((char*)es + 0x440);
    buf << ", " << *(int*)((char*)es + 0x24);
    buf << ", " << *(int*)((char*)es + 0x184);
    buf << ", '" << *(int*)((char*)es + 0x28) << ","
        << *(int*)((char*)es + 0x2c) << "'";
    buf << ", " << *(int*)((char*)es + 0x234);
    buf << ", " << *(int*)((char*)es + 0x238);
    buf << ", " << *(float*)((char*)es + 0x23c);
    buf << ", " << *(int*)((char*)es + 0x240);
    buf << ", " << *(int*)((char*)es + 0x244);
    buf << ", " << *(float*)((char*)es + 0x248);
    buf << ", " << *(int*)((char*)es + 0x448);
    buf << ", " << *(int*)((char*)es + 0x44c);
    buf << ", " << *(int*)((char*)es + 0x450);
    buf << ", " << *(int*)((char*)es + 0x454);
    buf << ", " << *(int*)((char*)es + 0x458);
    buf << ", " << *(int*)((char*)es + 0x2ac);
    buf << ", " << *(int*)((char*)es + 0x2c4);
    buf << ", " << *(int*)((char*)es + 0x2c8);
    buf << ", " << *(int*)((char*)es + 0x444);
    buf << ", '"
        << std::for_each(es->m_equipParam.m_vec188.begin(), es->m_equipParam.m_vec188.end(),
                         StringMaker()).c_str()
        << "'";
    buf << ", " << *(float*)((char*)es + 0x2fc);
    iVar = local_68 * 0x400;
    ++local_68;
    buf << ", '"
        << EscapeDB(db, (char*)es->m_str668.c_str(), local_3e90 + iVar)
        << "'";
    buf << ", '"
        << std::for_each(es->m_vec66c.begin(), es->m_vec66c.end(),
                         StringMaker()).c_str()
        << "'";
    iVar = local_68 * 0x400;
    ++local_68;
    buf << ", '"
        << EscapeDB(db, (char*)es->m_str6b0.c_str(), local_3e90 + iVar)
        << "'";
    iVar = local_68 * 0x400;
    ++local_68;
    buf << ", '"
        << EscapeDB(db, (char*)es->m_str6b4.c_str(), local_3e90 + iVar)
        << "'";
    iVar = local_68 * 0x400;
    ++local_68;
    buf << ", '"
        << EscapeDB(db, (char*)es->m_str6ac.c_str(), local_3e90 + iVar)
        << "'";
    buf << ", " << *(int*)((char*)es + 0x798);
    buf << ", '" << local_1fc << "'";
    buf << ", '"
        << std::for_each(es->m_equipParam.m_vec1b8.begin(), es->m_equipParam.m_vec1b8.end(),
                         StringMaker()).c_str()
        << "'";
    buf << ", '"
        << std::for_each(es->m_vec198.begin(), es->m_vec198.end(),
                         StringMaker()).c_str()
        << "'";
    buf << ", " << *(float*)((char*)es + 0x434);
    buf << ", '"
        << URLString(this, (char*)es->m_str6ac.c_str(),
                     (int)es->m_str6ac.length(), *(int*)((char*)es + 0x70))
        << "'";
    buf << ", " << *(int*)((char*)es + 0xc4);
    db->set_query(
        "inSert into dnf_item_info(it_no, it_name, it_eng_name, it_explain, "
        "master_type, sub_type, job, class, revert, level, skill, "
        "create_ratio, rarity, weight, price, cash, medal, durability, "
        "cooltime, hp_max, mp_max, phy_att, phy_def, mag_att, mag_def, "
        "equip_phy_att, equip_phy_def, equip_mag_att, equip_mag_def, "
        "ref_fire, ref_water, ref_dark, ref_light, ref_all, ref_slow, "
        "ref_freeze, ref_poison, ref_stun, ref_cus, ref_blind, ref_lite, "
        "ref_ston, ref_sleep, ref_deekement, ref_deadlystrike, ref_bleeding, "
        "ref_confuse, ref_hold, ref_all_stat, ref_pierce, ref_stuck, "
        "inven_max, hp_regenrate, mp_regenrate, mov_speed, att_speed, quest, "
        "hit_recovery, jump, att_element, att_active_status, "
        "att_active_status_ratio, att_active_status_pow, att_backforce, "
        "att_upforce, att_hp_drain, att_mp_drain, criticalhit_rate, "
        "stuck_rate, att_defenseIgnore, skill_levelup, set_type, jewel_type, "
        "detail_explain, flavor_text, anti_evil, value, required_skill, "
        "need_material, physical_absolute_damage, physical_damage_reduce, "
        "physical_absolute_defense, magical_absolute_damage, "
        "magical_damage_reduce, magical_absolute_defense, fire_attack, "
        "water_attack, dark_attack, light_attack, all_attack, "
        "weapon_break_resistance, armor_break_resistance, "
        "all_activestatus_resistance, rigidity, item_aura, "
        "magical_critical_hit, set_name, set_item, fullset_basic_explain, "
        "fullset_detail_explain, parameter_basic_explain, "
        "parameter_detail_explain, part_set_index, hide_equipment, "
        "skill_data_up, it_set_no, db_piece_count, set_item_master, "
        "ani_variation, ani_variation_expand, ani_variation2, "
        "ani_variation_expand2, hide_growtype_avatar, "
        "room_list_move_speed_rate, url, icon_mark_number, "
        "extra_icon_idx_list, hp_max_rate, mp_max_rate) values(%s)",
        buf.c_str());
    FieldChecker fc;
    fc.AddField(db->GetQueryString());
    if (!db->exec(true))
    {
        return false;
    }
    if (es->m_equipParam.m_vec264.empty())
    {
        return true;
    }
    for (std::vector< ::STAvatarAbilityVariation>::iterator avit =
             es->m_equipParam.m_vec264.begin();
         avit != es->m_equipParam.m_vec264.end(); ++avit)
    {
        db->set_query(
            "inSert into avatar_select_ability(it_no, ability_no, "
            "ability_type, rate_change, value, job, skill_index, "
            "skill_level) values(%d,%d,%d,%d,%f,%d,%d,%d)",
            0, 0, 0, 0, 0.0f, 0, 0, 0);
        if (!db->exec(true))
        {
            return false;
        }
    }
    std::string artifactName = "artifact red";
    bool isArtifact = false;
    SITEMTYPE* st = &m_itemTypeMap[artifactName];
    if (st->m_field4 == (unsigned short)key)
    {
        isArtifact = true;
    }
    else
    {
        artifactName = "artifact blue";
        st = &m_itemTypeMap[artifactName];
        if (st->m_field4 == (unsigned short)key)
        {
            isArtifact = true;
        }
        else
        {
            artifactName = "artifact green";
            st = &m_itemTypeMap[artifactName];
            if (st->m_field4 == (unsigned short)key)
            {
                isArtifact = true;
            }
        }
    }
    if (isArtifact)
    {
        InsertArtifact(this, script);
    }
    return true;
}

bool CSyncScript::insert_stackable_item_to_db(STStackableScript* script)
{
    if (m_field0 != 1)
    {
        return true;
    }
    EquipParamInfo* ep;
    EquipParamInfo* e2;
    MySQL* db;
    int key;
    db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    std::string typeName = ((StackScriptFull*)script)->m_strbc;
    int out2;
    int out1;
    if (!StackableGetType(this, typeName, 0, 0, out1, out2, false))
    {
        return false;
    }
    MyStream buf;
    char local_1874[1024] = {0};
    char local_1474[1024];
    char acStack_1074[1024];
    char local_c74[1024];
    char local_874[1024] = {0};
    char local_474[1024] = {0};
    if (!CodePage::script2Database(
            (char*)((StackScriptFull*)script)->m_strac.c_str(), local_474))
    {
        cMyTrace(
            "bool sync_script::CSyncScript::insert_stackable_item_to_db(STStackableScript*)",
            0x523, 5)("Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
                      CodePage::database(),
                      ((StackScriptFull*)script)->m_strac.c_str(),
                      "bool sync_script::CSyncScript::insert_stackable_item_to_db(STStackableScript*)",
                      0x523);
        return false;
    }
    if (!CodePage::script2Database(
            (char*)((StackScriptFull*)script)->m_strb4.c_str(), local_874))
    {
        cMyTrace(
            "bool sync_script::CSyncScript::insert_stackable_item_to_db(STStackableScript*)",
            0x52a, 5)("Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
                      CodePage::database(),
                      ((StackScriptFull*)script)->m_strb4.c_str(),
                      "bool sync_script::CSyncScript::insert_stackable_item_to_db(STStackableScript*)",
                      0x52a);
        return false;
    }
    buf << ((StackScriptFull*)script)->m_field0;
    buf << ", '" << EscapeDB(db, local_474, local_1874) << "'";
    buf << ", '"
        << EscapeDB(db, (char*)((StackScriptFull*)script)->m_strb0.c_str(),
                    local_1474)
        << "'";
    buf << ", '" << EscapeDB(db, local_874, acStack_1074) << "'";
    buf << ", " << out1;
    buf << ", " << out2;
    buf << ", '" << UsableCharString(this, script) << "'";
    buf << ", " << ((StackScriptFull*)script)->m_field4;
    buf << ", '"
        << AttachTypeString(this, ((StackScriptFull*)script)->m_field8)
        << "'";
    buf << ", " << ((StackScriptFull*)script)->m_field5c;
    buf << ", " << 0;
    buf << ", " << ((StackScriptFull*)script)->m_fieldc;
    buf << ", " << ((StackScriptFull*)script)->m_field10;
    buf << ", " << ((StackScriptFull*)script)->m_field34;
    buf << ", " << ((StackScriptFull*)script)->m_field14;
    buf << ", " << ((StackScriptFull*)script)->m_field18;
    buf << ", " << ((StackScriptFull*)script)->m_field1c;
    buf << ", " << 0;
    buf << ", " << ((StackScriptFull*)script)->m_field68;
    key = 0x13;
    ep = (EquipParamInfo*)((StackScriptFull*)script)->m_map4bc[key];
    if (ep == 0)
    {
        buf << ", 0";  buf << ", 0";  buf << ", 0";  buf << ", 0";
        buf << ", 0";  buf << ", 0";  buf << ", 0";  buf << ", 0";
        buf << ", 0";  buf << ", 0";  buf << ", 0";  buf << ", 0";
        buf << ", 0";  buf << ", 0";  buf << ", 0";  buf << ", 0";
        buf << ", 0";  buf << ", 0";  buf << ", 0";  buf << ", 0";
        buf << ", 0";  buf << ", 0";  buf << ", 0";  buf << ", 0";
        buf << ", 0";  buf << ", 0";  buf << ", 0";  buf << ", 0";
        buf << ", 0";  buf << ", 0";  buf << ", 0";  buf << ", 0";
        buf << ", 0";  buf << ", 0";  buf << ", 0";  buf << ", 0";
        buf << ", 0";  buf << ", 0";  buf << ", 0";  buf << ", 0";
        buf << ", ''";
        buf << ", 0";  buf << ", 0";  buf << ", 0";  buf << ", 0";
        buf << ", 0";  buf << ", 0";  buf << ", 0";  buf << ", 0";
        buf << ", 0";  buf << ", 0";  buf << ", 0";  buf << ", 0";
        buf << ", 0";  buf << ", 0";  buf << ", 0";  buf << ", 0";
        buf << ", 0";  buf << ", 0";
        buf << ", 0.0";  buf << ", 0.0";  buf << ", 0.0";
        buf << ", 0";  buf << ", 0";  buf << ", 0";  buf << ", 0";
        buf << ", 0";  buf << ", 0";  buf << ", 0";  buf << ", 0";
        buf << ", 0";  buf << ", 0";  buf << ", 0";  buf << ", 0";
        buf << ", 0";  buf << ", 0";  buf << ", 0";
        buf << ", ''";
    }
    else
    {
        buf << ", " << *(int*)((char*)ep + 4);
        e2 = ep;
        buf << ", " << *(int*)((char*)e2 + 0xc);
        buf << ", " << *(int*)((char*)e2 + 0x14);
        buf << ", " << *(int*)((char*)e2 + 0x18);
        buf << ", " << *(int*)((char*)e2 + 0x1c);
        buf << ", " << *(int*)((char*)e2 + 0x24);
        buf << ", " << *(int*)((char*)e2 + 0x54);
        buf << ", " << *(int*)((char*)e2 + 0x5c);
        buf << ", " << *(int*)((char*)e2 + 0x64);
        buf << ", " << *(int*)((char*)e2 + 0x6c);
        buf << ", " << *(int*)((char*)e2 + 0xb0);
        buf << ", " << *(int*)((char*)e2 + 0xb4);
        buf << ", " << *(int*)((char*)e2 + 0xb8);
        buf << ", " << *(int*)((char*)e2 + 0xbc);
        buf << ", " << *(int*)((char*)e2 + 0xc0);
        buf << ", " << *(int*)((char*)e2 + 0xc4);
        buf << ", " << *(int*)((char*)e2 + 0xc8);
        buf << ", " << *(int*)((char*)e2 + 0xcc);
        buf << ", " << *(int*)((char*)e2 + 0xd0);
        buf << ", " << *(int*)((char*)e2 + 0xd4);
        buf << ", " << *(int*)((char*)e2 + 0xd8);
        buf << ", " << *(int*)((char*)e2 + 0xdc);
        buf << ", " << *(int*)((char*)e2 + 0xe0);
        buf << ", " << *(int*)((char*)e2 + 0xe4);
        buf << ", " << *(int*)((char*)e2 + 0xe8);
        buf << ", " << *(int*)((char*)e2 + 0xec);
        buf << ", " << *(int*)((char*)e2 + 0xf0);
        buf << ", " << *(int*)((char*)e2 + 0xfc);
        buf << ", " << *(int*)((char*)e2 + 0x100);
        buf << ", " << *(int*)((char*)e2 + 0x108);
        buf << ", " << *(int*)((char*)e2 + 0xf4);
        buf << ", " << *(int*)((char*)e2 + 0xf8);
        buf << ", " << *(int*)((char*)e2 + 0x10c);
        buf << ", " << *(int*)((char*)e2 + 0x110);
        buf << ", " << *(int*)((char*)e2 + 0x114);
        buf << ", " << *(int*)((char*)e2 + 0x11c);
        buf << ", " << *(int*)((char*)e2 + 0x128);
        buf << ", " << *(int*)((char*)e2 + 0x12c);
        buf << ", " << *(int*)((char*)e2 + 0x130);
        buf << ", " << *(int*)((char*)e2 + 0x134);
        buf << ", '" << AttackElementString(this, e2) << "'";
        buf << ", " << *(int*)((char*)e2 + 0x144);
        buf << ", " << *(float*)((char*)e2 + 0x148);
        buf << ", " << *(int*)((char*)e2 + 0x14c);
        buf << ", " << *(int*)((char*)e2 + 0x220);
        buf << ", " << *(int*)((char*)e2 + 0x224);
        buf << ", " << (unsigned int)(unsigned char)*(char*)((char*)e2 + 0x23c);
        buf << ", " << (unsigned int)(unsigned char)*(char*)((char*)e2 + 0x23d);
        buf << ", " << *(float*)((char*)e2 + 0x138);
        buf << ", " << *(float*)((char*)e2 + 0x228);
        buf << ", " << (unsigned int)(unsigned char)*(char*)((char*)e2 + 0x22c);
        buf << ", " << *(float*)((char*)e2 + 0x274);
        buf << ", " << *(float*)((char*)e2 + 8);
        buf << ", " << *(float*)((char*)e2 + 0x10);
        buf << ", " << *(int*)((char*)e2 + 0x288);
        buf << ", " << *(int*)((char*)e2 + 0x28c);
        buf << ", " << *(int*)((char*)e2 + 0x290);
        buf << ", " << *(int*)((char*)e2 + 0x294);
        buf << ", " << *(int*)((char*)e2 + 0x298);
        buf << ", " << *(int*)((char*)e2 + 0x284);
        buf << ", " << *(float*)((char*)e2 + 0x13c);
        buf << ", " << *(int*)((char*)e2 + 0x74);
        buf << ", " << *(int*)((char*)e2 + 0x78);
        buf << ", " << *(float*)((char*)e2 + 0x7c);
        buf << ", " << *(int*)((char*)e2 + 0x80);
        buf << ", " << *(int*)((char*)e2 + 0x84);
        buf << ", " << *(float*)((char*)e2 + 0x88);
        buf << ", " << *(int*)((char*)e2 + 0x104);
        buf << ", '"
            << std::for_each(e2->m_levelUpSkills.begin(),
                             e2->m_levelUpSkills.end(),
                             StringMaker()).c_str()
            << "'";
    }
    buf << ", 0";
    buf << ", '"
        << EscapeDB(db,
                    URLString(this,
                              (char*)((StackScriptFull*)script)->m_str6c.c_str(),
                              (int)((StackScriptFull*)script)->m_str6c.length(),
                              ((StackScriptFull*)script)->m_field70),
                    local_c74)
        << "'";
    buf << ", " << ((StackScriptFull*)script)->m_fieldc4;
    FieldChecker fc;
    db->set_query(
        "inSert into dnf_item_info(it_no, it_name, it_eng_name, it_explain, "
        "master_type, sub_type, job, class, revert, level, skill, "
        "create_ratio, rarity, weight, price, cash, medal, durability, "
        "cooltime, hp_max, mp_max, phy_att, phy_def, mag_att, mag_def, "
        "equip_phy_att, equip_phy_def, equip_mag_att, equip_mag_def, "
        "ref_fire, ref_water, ref_dark, ref_light, ref_all, ref_slow, "
        "ref_freeze, ref_poison, ref_stun, ref_cus, ref_blind, ref_lite, "
        "ref_ston, ref_sleep, ref_deekement, ref_deadlystrike, ref_bleeding, "
        "ref_confuse, ref_hold, ref_all_stat, ref_pierce, ref_stuck, "
        "inven_max, hp_regenrate, mp_regenrate, mov_speed, att_speed, quest, "
        "hit_recovery, jump, att_element, att_active_status, "
        "att_active_status_ratio, att_active_status_pow, att_backforce, "
        "att_upforce, att_hp_drain, att_mp_drain, criticalhit_rate, "
        "stuck_rate, att_defenseIgnore, room_list_move_speed_rate, hp_max_rate, "
        "mp_max_rate, fire_attack, water_attack, dark_attack, light_attack, "
        "all_attack, rigidity, magical_critical_hit, physical_absolute_damage, "
        "physical_damage_reduce, physical_absolute_defense, "
        "magical_absolute_damage, magical_damage_reduce, "
        "magical_absolute_defense, armor_break_resistance, skill_levelup, "
        "set_type, url, icon_mark_number) values(%s)",
        buf.c_str());
    fc.AddField(db->GetQueryString());
    return db->exec(true);
}

bool CSyncScript::insertCeraShopItemIntoDB(unsigned int itemIdx,
                                           unsigned int itemType, int value,
                                           int count, short c, const char* name,
                                           const char* iconName)
{
    if (m_field0 != 1)
    {
        return true;
    }
    if (itemIdx != (unsigned int)-1)
    {
        char query[255] = {0};
        switch (c)
        {
        case 1:
            memcpy(query, "tem_", 4);
            break;
        case 2:
            memcpy(query, "30\xc0\xcf", 5);
            break;
        case 3:
            memcpy(query, "\xbf\xb5\xb1\xb8", 5);
            break;
        case 4:
            memcpy(query, "\xbf\xa5\xba\xed\xb7\xbd", 7);
            break;
        }
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
            (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
        db->set_query(
            "inSert into dnf_item_price(ipg_no, it_no, it_cnt, cera_price) "
            "values(%d,%d,%d,%d)",
            itemIdx, itemType, value, count);
        return db->exec(true);
    }
    return true;
}
}

class CDataManagerExt
{
public:
    char m_pad[0x4a50];
    int m_amplifyRate;  // +0x4a50 (float 位型)
    char m_pad4a54[0x41c];
    std::map<int, std::map<int, BaseItemValue> > m_baseItemInfo;  // +0x4e70
};

// ====================================================================

CItemList::CItemList()
{
}

CItemList::~CItemList()
{
    destroy();
}

void CItemList::destroy()
{
    __gnu_cxx::hash_map<int, CItem*>::iterator it = m_itemMap.begin();
    __gnu_cxx::hash_map<int, CItem*>::iterator end = m_itemMap.end();
    while (it != end)
    {
        CItem* item = it->second;
        delete item;
        ++it;
    }
    m_itemMap.clear();
    if (m_gradeItemList)
    {
        delete[] m_gradeItemList;
        m_gradeItemList = 0;
    }
}

CItem* CItemList::find_item(int idx)
{
    __gnu_cxx::hash_map<int, CItem*>::iterator it;
    it = m_itemMap.find(idx);
    if (it == m_itemMap.end())
    {
        printf("[CItemList::find_item] Can't find item : index(%d)\n", idx);
        return 0;
    }
    return it->second;
}

CItem* CItemList::find_item(const char* name)
{
    for (__gnu_cxx::hash_map<int, CItem*>::iterator it = m_itemMap.begin();
         it != m_itemMap.end(); ++it)
    {
        CItem* item = it->second;
        if (strcmp(item->GetItemName(), name) == 0)
        {
            return item;
        }
    }
    return 0;
}

CCeraShopGoods* CItemList::FindGoods(int goodsIdx)
{
    __gnu_cxx::hash_map<int, CCeraShopGoods*>::iterator it;
    __gnu_cxx::hash_map<int, CCeraShopGoods*>::iterator e = m_goodsMap.end();
    it = m_goodsMap.find(goodsIdx);
    if (it == e)
    {
        return 0;
    }
    return it->second;
}

BonusItem* CItemList::GetProperBonusItem(unsigned int bonusIdx)
{
    for (unsigned int i = 0; i < m_bonusItem.size(); ++i)
    {
        if (m_bonusItem[i].m_min <= bonusIdx && bonusIdx < m_bonusItem[i].m_max)
        {
            return &m_bonusItem[i];
        }
    }
    return 0;
}

void CItemList::getCeraShopBonusItem(int idx, unsigned long& outItem,
                                     unsigned int& outCount) const
{
    outItem = (unsigned long)-1;
    outCount = 0;
    int rand;
    std::vector<stCeraShopBonusInfo_t>::const_iterator it =
        m_bonusList.begin();
    std::vector<stCeraShopBonusInfo_t>::const_iterator end =
        m_bonusList.end();
    while (it != end)
    {
        if (it->m_field0 <= idx && idx <= it->m_field4)
        {
            rand = get_rand_int(1000);
            int sum = 0;
            std::vector<stCeraShopBonusItemInfo_t>::const_iterator it2 =
                it->m_items.begin();
            std::vector<stCeraShopBonusItemInfo_t>::const_iterator end2 =
                it->m_items.end();
            while (it2 != end2)
            {
                sum = it2->m_field8 + sum;
                if (sum > rand)
                {
                    outItem = it2->m_field0;
                    outCount = (unsigned short)it2->m_field4;
                    return;
                }
                ++it2;
            }
        }
        ++it;
    }
}

void CItemList::get_lotto_item(int idx, char* out)
{
    Inven_Item* pItem = (Inven_Item*)out;
    CItem* item = find_item(idx);
    if (!item)
    {
        pItem->m_addInfo = 0;
        pItem->set_add_info(0);
        pItem->m_fieldb = 0;
        pItem->m_field1 = 0;
        pItem->ResetItemAttr();
        return;
    }
    if (!item->is_stackable())
    {
        pItem->m_addInfo = 0;
        pItem->set_add_info(0);
        pItem->m_fieldb = 0;
        pItem->m_field1 = 1;
        pItem->ResetItemAttr();
    }
    else
    {
    if (((CStackableItem*)item)->m_nItemType != 6 &&
        ((CStackableItem*)item)->m_nItemType != 7 &&
        ((CStackableItem*)item)->m_nItemType != 0xd)
        {
            pItem->m_addInfo = 0;
            pItem->set_add_info(0);
            pItem->m_fieldb = 0;
            pItem->m_field1 = 2;
            pItem->ResetItemAttr();
        }
        else
        {
            ((CStackableItem*)item)->get_lotto_item(out);
        }
    }
}

void CItemList::create_item(ENUM_ITEM_CREATE_TYPE type, Inven_Item& item,
                            int param)
{
    item.set_add_info(param);
    CItem* pItem = find_item(item.m_addInfo);
    if (!pItem)
    {
        LogManager::logFormat(1, "item.cpp",
            "void CItemList::create_item(ENUM_ITEM_CREATE_TYPE, Inven_Item&, int)",
            0x98f, "find_item(%d)", item.m_addInfo);
        item.m_addInfo = -1;
        item.set_add_info(0);
        item.m_fieldb = 0;
        item.m_field1 = 0;
        item.ResetItemAttr();
        return;
    }
    pItem->make_item(item);
    if (pItem->is_stackable())
    {
        if (item.get_add_info() < 0)
        {
            item.set_add_info(0);
        }
        if (item.get_add_info() > 999)
        {
            cMyTrace(
                "void CItemList::create_item(ENUM_ITEM_CREATE_TYPE, Inven_Item&, int)",
                0x9a1, 5)(
                "CItemList::create_item, add_info >= STACK_LIMIT_ERROR , %d ITEM #%d",
                item.get_add_info(), item.m_addInfo);
        }
    }
    item.m_amp.reset();
    if (type == 1)
    {
        if (GlobalData::s_itemAmplifier_->checkInvestableItem(pItem, item, 0))
        {
            GlobalData::s_itemAmplifier_->generateAmplifyItem(pItem, item);
        }
        if (item.m_field1 == 1)
        {
            int equip = ((CEquipItem*)pItem)->get_endurance();
            int rand = get_rand_int(100);
            int rate = ((CDataManagerExt*)G_CDataManager())->m_amplifyRate;
            int bonus = (int)((float)equip * ((float)(100 - rate) / 100.0f));
            int result = (int)((float)equip + (float)bonus * ((float)rand / 100.0f));
            if (result < 0)
            {
                result = 1;
            }
            else if (result > equip)
            {
                result = equip;
            }
            item.m_fieldb = (unsigned short)result;
        }
    }
}

bool CItemList::insert_item(int group, int idx, CItem* item)
{
    __gnu_cxx::hash_map<int, CItem*>::iterator it;
    it = m_gradeItemList[group].find(idx);
    if (it != m_gradeItemList[group].end())
    {
        m_gradeItemList[group].insert(std::pair<const int, CItem*>(idx, item));
        m_itemMap.insert(std::pair<const int, CItem*>(idx, item));
        if (item->GetQuestItemDropBouns())
        {
            m_lottoCount.insert(
                std::make_pair(item->GetQuestItemDropBouns(), (int)idx));
        }
        typedef bool (*VCheckFn)(void*);
        if ((*(VCheckFn*)(*(char**)item + 0x4c))(item))
        {
            BaseItemKey key(item->get_rarity(), item->getUsableLevel(),
                            item->getItemGroupName());
            std::map<int, std::map<int, BaseItemValue> >::iterator it2 =
                ((CDataManagerExt*)G_CDataManager())
                    ->m_baseItemInfo.find(*(int*)&key);
            int value = item->get_index();
            if (it2 !=
                ((CDataManagerExt*)G_CDataManager())->m_baseItemInfo.end())
            {
                it2->second.insert(std::make_pair<int, BaseItemValue&>(
                    item->get_grade(), *(BaseItemValue*)&value));
            }
            else
            {
                std::map<int, BaseItemValue> inner;
                inner.insert(std::make_pair<int, BaseItemValue&>(
                    item->get_grade(), *(BaseItemValue*)&value));
                ((CDataManagerExt*)G_CDataManager())
                    ->m_baseItemInfo.insert(
                        std::make_pair<int&, std::map<int, BaseItemValue>&>(
                            *(int*)&key, inner));
            }
        }
        return true;
    }
    LogManager::logFormat(1, "item.cpp",
        "bool CItemList::insert_item(int, int, CItem*)", 0x81f,
        "Insert Fail Item Group[%d] Index: %d", group, idx);
    return false;
}

bool CItemList::init()
{
    m_gradeItemList = new __gnu_cxx::hash_map<int, CItem*, __gnu_cxx::hash<int>,
                        std::equal_to<int>,
                        std::allocator<CItem*> >[0xc9];
    if (!m_gradeItemList)
    {
        LogManager::logFormat(1, "item.cpp", "bool CItemList::init()", 0x78c,
            "m_pGradeItemList = new ITEM_LIST[ITEM_GRADE_COUNT] fail");
        return false;
    }
    if (!load_item_files())
    {
        LogManager::logFormat(1, "item.cpp", "bool CItemList::init()", 0x792,
            "load_item_files() fail");
        return false;
    }
    ((ExtreamDungeon::CCompound_ExtreamDun_Item*)
        GlobalData::s_pCompoundBindCube)->after_addItem();
    if (!Load_Goods_File(G_CDataManager()->GetCeraShopGoodsName()))
    {
        LogManager::logFormat(1, "item.cpp", "bool CItemList::init()", 0x7a5,
            "Load_Goods_File() of ItemVendingMachine fail ");
        return false;
    }
    if (!Load_Goods_File(G_CDataManager()->SuddenShopFileName1()))
    {
        LogManager::logFormat(1, "item.cpp", "bool CItemList::init()", 0x7c8,
            "SuddenShopFileName1() of ItemVendingMachine fail ");
        return false;
    }
    if (!Load_Goods_File(G_CDataManager()->SuddenShopFileName2()))
    {
        LogManager::logFormat(1, "item.cpp", "bool CItemList::init()", 0x7ce,
            "SuddenShopFileName2() of ItemVendingMachine fail ");
        return false;
    }
    if (!Load_Goods_File(G_CDataManager()->SuddenShopFileName3()))
    {
        LogManager::logFormat(1, "item.cpp", "bool CItemList::init()", 0x7d4,
            "SuddenShopFileName3() of ItemVendingMachine fail ");
        return false;
    }
    return true;
}

bool CItemList::load_item_files()
{
    STStackableScript stackScript;
    CEquipItem* pEquip = 0;
    CStackableItem* pStack = 0;
    const char* path = 0;
    std::map<int, std::string>::iterator it;
    int status = 0;
    bool result;

    sync_script::GetInstanceSyncScript()->truncate_dnf_item_info_table();
    it = sync_script::g_equipItemInfoTable.begin();

    for (;;)
    {
        if (it == sync_script::g_equipItemInfoTable.end())
        {
            it = sync_script::g_stackableItemInfoTable.begin();
            break;
        }
        {
            STEquipmentScript eqScript;
            path = it->second.c_str();
            if (!path)
            {
                status = 0;
            }
            else if (!ImportEquipmentScript(&eqScript, path, false, 0, it->first))
            {
                LogManager::logFormat(1, "item.cpp",
                    "bool CItemList::load_item_files()", 0x8e8,
                    "ImportEquipmentScript() fail file - %s", path);
                result = false;
                status = 1;
            }
            else
            {
                sync_script::CSyncScript* sync =
                    sync_script::GetInstanceSyncScript();
                if (!sync->insert_equiptment_item_to_db(&eqScript, 0, 0, 0) &&
                    eqScript.m_field1b4 != 0x16 &&
                    eqScript.m_field1b4 != 0x17 &&
                    eqScript.m_field1b4 != 0x18 &&
                    eqScript.m_field1b4 != 0x19)
                {
                    printf("insert_equiptment_item_to_db() fail file - %s", path);
                }
                pEquip = new CEquipItem;
                if (!pEquip)
                {
                    LogManager::logFormat(1, "item.cpp",
                        "bool CItemList::load_item_files()", 0x903,
                        "new CEquipItem error");
                    result = false;
                    status = 1;
                }
                else
                {
                    pEquip->set_item(eqScript);
                    if (!pEquip->verify())
                    {
                        LogManager::logFormat(1, "item.cpp",
                            "bool CItemList::load_item_files()", 0x90a,
                            "Equip item verify fail. [%s]", path);
                        result = false;
                        status = 1;
                    }
                    else
                    {
                        insert_item(((CItem*)pEquip)->get_grade(),
                                    ((CItem*)pEquip)->get_index(),
                                    (CItem*)pEquip);
                        ((ExtreamDungeon::CCompound_ExtreamDun_Item*)
                            GlobalData::s_pCompoundBindCube)
                            ->check_N_addItem(*pEquip);
                        ++it;
                        status = 2;
                    }
                }
            }
        }
        if (status == 0)
        {
            it = sync_script::g_stackableItemInfoTable.begin();
            break;
        }
        if (status == 1)
        {
            return result;
        }
    }

    for (;;)
    {
        if (it == sync_script::g_stackableItemInfoTable.end())
        {
            sync_script::DisplayMaxLength();
            result = true;
            break;
        }
        path = it->second.c_str();
        if (!path)
        {
            sync_script::DisplayMaxLength();
            result = true;
            break;
        }
        if (!ImportStackableScript(&stackScript, path, false, it->first))
        {
            LogManager::logFormat(1, "item.cpp",
                "bool CItemList::load_item_files()", 0x926,
                "importStackableScript fail file - %s", path);
            result = false;
            break;
        }
        if (GlobalInstance<Arad_GiftItem_Set>::inst()->open(&stackScript))
        {
            LogManager::logFormat(1, "item.cpp",
                "bool CItemList::load_item_files()", 0x931,
                "[ERROR] Arad GiftItem Setting!! [item_idx:%d][levelt:%d]",
                stackScript.m_field0, stackScript.m_field5c);
            result = false;
            break;
        }
        if (!sync_script::GetInstanceSyncScript()
                 ->insert_stackable_item_to_db(&stackScript))
        {
            printf("insert_stackable_item_to_db() fail file - %s", path);
        }
        pStack = new CStackableItem;
        if (!pStack)
        {
            LogManager::logFormat(1, "item.cpp",
                "bool CItemList::load_item_files()", 0x941,
                "ptemp_stack_data is null");
            result = false;
            break;
        }
        pStack->set_item(stackScript);
        if (!pStack->verify())
        {
            LogManager::logFormat(1, "item.cpp",
                "bool CItemList::load_item_files()", 0x948,
                "Stackable item verify fail. [%s]", path);
            result = false;
            break;
        }
        if (pStack->GetItemType() == 7)
        {
            if (pStack->m_lotto.m_items.size() != pStack->m_lotto.m_counts.size())
            {
                cMyTrace trace("bool CItemList::load_item_files()", 0x94f, 5);
                trace("Stackable item set_item fail (file [%s])", path);
                result = false;
                break;
            }
        }
        if (pStack->GetItemType() == 0xd)
        {
            if (pStack->m_lotto.m_items.size() != pStack->m_lotto.m_counts.size())
            {
                cMyTrace trace("bool CItemList::load_item_files()", 0x957, 5);
                trace("Stackable item set_item fail 2 (file [%s])", path);
                result = false;
                break;
            }
        }
        insert_item(((CItem*)pStack)->get_grade(),
                    ((CItem*)pStack)->get_index(), (CItem*)pStack);
        ++it;
    }
    return result;
}

bool CItemList::Load_Goods_File(const char* path)
{
    std::vector<STCeraShopItem> shopList[0x10];
    int i;
    int j;
    bool result;

    if (!importCashShopItemList((char*)path, shopList, m_bonusList, 0, 0, 0, 0))
    {
        LogManager::logFormat(1, "item.cpp",
            "bool CItemList::Load_Goods_File(const char*)", 0xa76,
            "importCashShopItemList() fail file - %s", path);
        return false;
    }
    sync_script::GetInstanceSyncScript()->truncateCeraShopItemTable();
    for (i = 0; i < 0x10; ++i)
    {
        for (j = 0; j < (int)shopList[i].size(); ++j)
        {
            CCeraShopGoods* goods = new CCeraShopGoods(shopList[i][j]);
            m_goodsMap[shopList[i][j].m_field8] = goods;
            int value = shopList[i][j].m_field38;
            if (shopList[i][j].m_field14 == 1)
            {
                value = shopList[i][j].m_field38;
            }
            else
            {
                CEquipItem* equip = (CEquipItem*)
                    m_goodsMap[shopList[i][j].m_field8]->GetRelatedItem();
                std::vector<stAvatarTypeSelect_t>* avatarList =
                    (std::vector<stAvatarTypeSelect_t>*)
                    equip->getAvatarTypeSelect();
                value = shopList[i][j].m_field38;
                if (shopList[i][j].m_fieldc == 0 &&
                    shopList[i][j].m_field8 != -1)
                {
                    if (!avatarList->empty())
                    {
                        value = (*avatarList)[(int)(short)shopList[i][j].m_field22 - 1]
                                    .m_fieldc;
                    }
                }
            }
            if (value != 0 &&
                ((short)shopList[i][j].m_field22 == 1 ||
                 (short)shopList[i][j].m_field22 == 0))
            {
                CItem* item = G_CDataManager()->find_item(shopList[i][j].m_field14);
                if (item && shopList[i][j].m_field8 != -1)
                {
                    const char* iconName = item->GetItemIconName();
                    const char* name = item->GetItemName();
                    if (!sync_script::GetInstanceSyncScript()
                             ->insertCeraShopItemIntoDB(
                                 shopList[i][j].m_field8,
                                 shopList[i][j].m_field14,
                                 shopList[i][j].m_field1c,
                                 value, (char)(short)shopList[i][j].m_field22,
                                 name, iconName))
                    {
                        LogManager::logFormat(1, "item.cpp",
                            "bool CItemList::Load_Goods_File(const char*)", 0xb01,
                            "insertCeraShopItemIntoDB() fail IPG - %d",
                            shopList[i][j].m_field8);
                        return false;
                    }
                }
            }
            if (shopList[i][j].m_field10 != 0)
            {
                m_lottoMap.insert(std::make_pair<unsigned long&, int&>(
                    (unsigned long&)shopList[i][j].m_field10,
                    shopList[i][j].m_field14));
            }
        }
    }
    return true;
}
