// df_game_r CTraceMobDieHack 还原（G2-5 批次）。
// CParty +0x210，总尺寸 0x94。符号（nm -C，W/T 全实现）：
//   C1 0x082a517c / D1 0x082a5236 / reset 0x082a50b0 /
//   traceBegin 0x085bf4cc / traceEnd 0x08599a88 /
//   addMonsterDamage 0x085bf4f6 / setMobKiller 0x085bf628 /
//   setDungeonDiff 0x085bf646 / reportHackInfo 0x08599cb2 /
//   _collect 0x08599f64；附带 stMonsterDamageLog_t C1 0x085bf4a2、
//   stHackLog_t::operator== 0x085bf472。
// 布局（由 ctor/reset/traceEnd/reportHackInfo 反汇编推导）：
//   +0x00 long m_traceBeginTime、+0x04 map<ushort,stMonsterDamageLog_t>[2]
//   （各 0x18）、+0x34 vector<int>（0xc）、+0x40 vector<stHackLog_t>（0xc）、
//   +0x4c char m_bTracing、+0x50 CUser* m_users[4]、+0x54 int m_damage[4]、
//   +0x58 int m_count[4]、+0x80 bool m_bInspectUser、+0x84 int m_dungeonDiff、
//   +0x88 保留。std::map/vector 及跨类方法以 extern asm 标签引用（ORIG 真实符号）。

class CUser;

// CParty 布局探针：+0xcac 为当前地下城 CDungeon*
struct CPartyStub
{
    char m_pad[0xcac];
    void* m_dungeon;
};

// stMonsterDamageLog_t（0xc）：+0 ushort 怪物ID、+2 ushort 伤害、+4 int 次数、+8 int 怪物索引
struct stMonsterDamageLog_t
{
    stMonsterDamageLog_t();

    unsigned short m_monsterId;
    unsigned short m_damage;
    int m_count;
    int m_monsterIndex;
};

stMonsterDamageLog_t::stMonsterDamageLog_t()
{
    m_monsterId = 0;
    m_damage = 0;
    m_count = 0;
    m_monsterIndex = 0;
}

// stHackLog_t（0x14）：+0 accId、+4 characNo、+8 ushort hackType、+0xc value、+0x10 count
struct stHackLog_t
{
    bool operator==(const stHackLog_t& other) const;

    unsigned int m_accId;
    unsigned int m_characNo;
    unsigned short m_hackType;
    int m_value;
    int m_count;
};

bool stHackLog_t::operator==(const stHackLog_t& other) const
{
    return m_accId == other.m_accId && m_characNo == other.m_characNo;
}

// ---- std::map<unsigned short, stMonsterDamageLog_t>（各 0x18）----
extern "C" void stMap_ctor(void* self) asm("_ZNSt3mapIt20stMonsterDamageLog_tSt4lessItESaISt4pairIKtS0_EEEC1Ev");
extern "C" void stMap_dtor(void* self) asm("_ZNSt3mapIt20stMonsterDamageLog_tSt4lessItESaISt4pairIKtS0_EEED1Ev");
extern "C" void stMap_clear(void* self) asm("_ZNSt3mapIt20stMonsterDamageLog_tSt4lessItESaISt4pairIKtS0_EEE5clearEv");

struct stMapInsertResult
{
    void* m_iterator;
    unsigned char m_inserted;
};

extern "C" stMapInsertResult* stMap_insert(stMapInsertResult* out, void* self, const void* value)
    asm("_ZNSt3mapIt20stMonsterDamageLog_tSt4lessItESaISt4pairIKtS0_EEE6insertERKS5_");
extern "C" stMonsterDamageLog_t* stMap_index(void* self, const unsigned short* key)
    asm("_ZNSt3mapIt20stMonsterDamageLog_tSt4lessItESaISt4pairIKtS0_EEEixERS4_");

// ---- std::vector<int>（0xc）----
extern "C" void stVectorInt_ctor(void* self) asm("_ZNSt6vectorIiSaIiEEC1Ev");
extern "C" void stVectorInt_dtor(void* self) asm("_ZNSt6vectorIiSaIiEED1Ev");
extern "C" void stVectorInt_clear(void* self) asm("_ZNSt6vectorIiSaIiEE5clearEv");
extern "C" void stVectorInt_push_back(void* self, const int* value) asm("_ZNSt6vectorIiSaIiEE9push_backERKi");

// ---- std::vector<stHackLog_t>（0xc）----
extern "C" void stVectorLog_ctor(void* self) asm("_ZNSt6vectorI11stHackLog_tSaIS0_EEC1Ev");
extern "C" void stVectorLog_dtor(void* self) asm("_ZNSt6vectorI11stHackLog_tSaIS0_EED1Ev");
extern "C" void stVectorLog_clear(void* self) asm("_ZNSt6vectorI11stHackLog_tSaIS0_EE5clearEv");
extern "C" void stVectorLog_begin(void* out, void* self) asm("_ZNSt6vectorI11stHackLog_tSaIS0_EE5beginEv");
extern "C" void stVectorLog_end(void* out, void* self) asm("_ZNSt6vectorI11stHackLog_tSaIS0_EE3endEv");
extern "C" void stVectorLog_push_back(void* self, const stHackLog_t* value)
    asm("_ZNSt6vectorI11stHackLog_tSaIS0_EE9push_backERKS0_");
extern "C" void stLog_find(void* out, void* first, void* last, const stHackLog_t* value)
    asm("_ZSt4findIN9__gnu_cxx17__normal_iteratorIP11stHackLog_tSt6vectorIS2_SaIS2_EEEES2_ET_S8_S8_RKT0_");
extern "C" bool stLogIter_eq(void* a, void* b)
    asm("_ZN9__gnu_cxxeqIP11stHackLog_tSt6vectorIS1_SaIS1_EEEEbRKNS_17__normal_iteratorIT_T0_EESB_");
extern "C" stHackLog_t* stLogIter_arrow(void* it)
    asm("_ZNK9__gnu_cxx17__normal_iteratorIP11stHackLog_tSt6vectorIS1_SaIS1_EEEptEv");

// ---- 跨类方法（ORIG 真实符号）----
extern "C" bool sub_CUser_IsGameMasterMode(void* user) asm("_ZNK5CUser16IsGameMasterModeEv");
extern "C" void* sub_CUser_GetParty(void* user) asm("_ZN5CUser8GetPartyEv");
extern "C" int sub_CUserCharacInfo_get_charac_level(void* user) asm("_ZNK15CUserCharacInfo16get_charac_levelEv");
extern "C" int sub_CDungeon_get_standard_level(void* dungeon) asm("_ZNK8CDungeon18get_standard_levelEv");
extern "C" unsigned int sub_CUser_get_acc_id(void* user) asm("_ZNK5CUser10get_acc_idEv");
extern "C" int sub_CUserCharacInfo_getCurCharacNo(void* user) asm("_ZNK15CUserCharacInfo14getCurCharacNoEv");
extern "C" int sub_CUserCharacInfo_getCurCharacR(void* user) asm("_ZNK15CUserCharacInfo13getCurCharacREv");
extern "C" int sub_CDungeon_get_index(void* dungeon) asm("_ZNK8CDungeon9get_indexEv");
extern "C" void* sub_CUser_getHackAnalyzer(void* user) asm("_ZN5CUser15getHackAnalyzerEv");
extern "C" void sub_CHackAnalyzer_addServerHackCnt(void* analyzer, void* user, int hackType,
                                                   unsigned int a, unsigned int b, unsigned int c)
    asm("_ZN8WongWork13CHackAnalyzer16addServerHackCntEP5CUserNS_13ENUM_HACKTYPEEjjj");
extern "C" bool sub_CVillageMonsterMgr_OnEvent(void* mgr)
    asm("_ZN16village_attacked18CVillageMonsterMgr7OnEventEv");
extern "C" int sub_CSystemTime_getCurSec(void* sys) asm("_ZN11CSystemTime9getCurSecEv");

// ORIG 全局对象（GlobalData::s_systemTime_ @0x941f714 / s_villageMonsterMgr @0x941f77c）
static char g_systemTime[4];
static char g_villageMonsterMgr[0x10];

class CTraceMobDieHack
{
public:
    CTraceMobDieHack();
    ~CTraceMobDieHack();
    void reset();
    void traceBegin(long t, bool b);
    void traceEnd(CUser* killer, CUser** users);
    int addMonsterDamage(int nMonsterIndex, unsigned short monsterId, int damage);
    void setMobKiller(int killer);
    void setDungeonDiff(int diff);
    void reportHackInfo();
    void _collect(CUser** users, int param_2);

    long m_traceBeginTime;    // +0x00
    char m_map1[0x18];        // +0x04
    char m_map2[0x18];        // +0x1c
    char m_mobKillers[0x0c];  // +0x34
    char m_hackLogs[0x0c];    // +0x40
    char m_bTracing;          // +0x4c
    char m_pad4d[3];          // +0x4d
    CUser* m_users[4];        // +0x50
    int m_damage[4];          // +0x54
    int m_count[4];           // +0x58
    bool m_bInspectUser;      // +0x80
    char m_pad81[3];          // +0x81
    int m_dungeonDiff;        // +0x84
    char m_pad88[0x0c];       // +0x88
};

CTraceMobDieHack::CTraceMobDieHack()
{
    stMap_ctor(m_map1);
    stMap_ctor(m_map2);
    stVectorInt_ctor(m_mobKillers);
    stVectorLog_ctor(m_hackLogs);
    reset();
}

CTraceMobDieHack::~CTraceMobDieHack()
{
    stVectorLog_dtor(m_hackLogs);
    stVectorInt_dtor(m_mobKillers);
    stMap_dtor(m_map2);
    stMap_dtor(m_map1);
}

void CTraceMobDieHack::reset()
{
    stMap_clear(m_map1);
    m_traceBeginTime = 0;
    stVectorInt_clear(m_mobKillers);
    stVectorLog_clear(m_hackLogs);
    m_bTracing = 0;
    m_dungeonDiff = 5;
    for (int i = 0; i < 4; ++i)
    {
        m_users[i] = 0;
        m_damage[i] = 0;
        m_count[i] = 0;
    }
}

void CTraceMobDieHack::traceBegin(long t, bool b)
{
    m_traceBeginTime = t;
    m_bTracing = 1;
    m_bInspectUser = b;
}

void CTraceMobDieHack::traceEnd(CUser* killer, CUser** users)
{
    if (m_bTracing != 1)
    {
        return;
    }
    long curSec = sub_CSystemTime_getCurSec(g_systemTime);
    m_bTracing = 0;
    if (killer == 0)
    {
        return;
    }
    CPartyStub* party = (CPartyStub*)sub_CUser_GetParty(killer);
    if (party == 0 || party->m_dungeon == 0)
    {
        return;
    }
    int dungeonIndex = sub_CDungeon_get_index(party->m_dungeon);
    int specialDungeons[4] = {1, 2, 0x29, 0x21};
    for (int i = 0; i < 4; ++i)
    {
        if (specialDungeons[i] == dungeonIndex)
        {
            return;
        }
    }
    for (int j = 0; j < 4; ++j)
    {
        CUser* u = users[j];
        if (u == 0 || sub_CUserCharacInfo_getCurCharacR(u) == 0 ||
            (party = (CPartyStub*)sub_CUser_GetParty(u)) == 0)
        {
            continue;
        }
        int level = sub_CUserCharacInfo_get_charac_level(u);
        int stdLevel = sub_CDungeon_get_standard_level(party->m_dungeon);
        if (level > stdLevel + 10)
        {
            continue;
        }
        m_users[j] = u;
        m_damage[j] += (int)(curSec - m_traceBeginTime);
        m_count[j] += 1;
    }
}

int CTraceMobDieHack::addMonsterDamage(int nMonsterIndex, unsigned short monsterId, int damage)
{
    stMonsterDamageLog_t log;
    log.m_monsterId = monsterId;
    log.m_damage = (unsigned short)damage;
    log.m_count = 1;
    log.m_monsterIndex = nMonsterIndex;

    // map 的 value_type：pair<const unsigned short, stMonsterDamageLog_t>（0x10）
    char value[0x10];
    *(unsigned short*)value = monsterId;
    *(stMonsterDamageLog_t*)(value + 4) = log;

    stMapInsertResult result;
    stMap_insert(&result, m_map1, value);
    if (!result.m_inserted)
    {
        stMonsterDamageLog_t* entry = stMap_index(m_map1, &monsterId);
        if (entry->m_damage == damage)
        {
            entry->m_count += 1;
        }
        else
        {
            *entry = log;
        }
    }
    return stMap_index(m_map1, &monsterId)->m_count;
}

void CTraceMobDieHack::setMobKiller(int killer)
{
    stVectorInt_push_back(m_mobKillers, &killer);
}

void CTraceMobDieHack::setDungeonDiff(int diff)
{
    m_dungeonDiff = diff;
}

void CTraceMobDieHack::reportHackInfo()
{
    if (sub_CVillageMonsterMgr_OnEvent(g_villageMonsterMgr))
    {
        reset();
        return;
    }
    for (int i = 0; i < 4; ++i)
    {
        if (m_users[i] == 0 || m_count[i] <= 1)
        {
            continue;
        }
        int avg = m_damage[i] / m_count[i];
        if (m_dungeonDiff != 5 && m_bInspectUser != 1)
        {
            int limit = 10;
            if (m_dungeonDiff == 0)
            {
                limit = 5;
            }
            else if (m_dungeonDiff == 1)
            {
                limit = 6;
            }
            else if (m_dungeonDiff == 2)
            {
                limit = 7;
            }
            else if (m_dungeonDiff == 3)
            {
                limit = 8;
            }
            if (avg <= limit)
            {
                void* analyzer = sub_CUser_getHackAnalyzer(m_users[i]);
                sub_CHackAnalyzer_addServerHackCnt(
                    analyzer, m_users[i], 0x19e, (unsigned int)(limit - avg + 1), 0, 0);
            }
        }
        if (m_bInspectUser == 0)
        {
            if (avg < 0x14)
            {
                void* analyzer = sub_CUser_getHackAnalyzer(m_users[i]);
                sub_CHackAnalyzer_addServerHackCnt(analyzer, m_users[i], 0x193, 1, 0, 0);
            }
        }
        else if (avg < 10)
        {
            void* analyzer = sub_CUser_getHackAnalyzer(m_users[i]);
            sub_CHackAnalyzer_addServerHackCnt(analyzer, m_users[i], 0x194, 1, 0, 0);
        }
    }
    reset();
}

void CTraceMobDieHack::_collect(CUser** users, int param_2)
{
    for (int i = 0; i < 4; ++i)
    {
        CUser* u = users[i];
        if (u == 0)
        {
            continue;
        }
        if (sub_CUser_IsGameMasterMode(u))
        {
            continue;
        }
        CPartyStub* party = (CPartyStub*)sub_CUser_GetParty(u);
        if (party == 0 || party->m_dungeon == 0)
        {
            continue;
        }
        int level = sub_CUserCharacInfo_get_charac_level(u);
        if (level <= 10)
        {
            continue;
        }
        int diff = level - sub_CDungeon_get_standard_level(
                                ((CPartyStub*)sub_CUser_GetParty(u))->m_dungeon);
        if (diff > 5)
        {
            continue;
        }
        stHackLog_t log;
        log.m_accId = sub_CUser_get_acc_id(u);
        log.m_characNo = sub_CUserCharacInfo_getCurCharacNo(u);
        log.m_hackType = 0x193;
        log.m_value = param_2;
        log.m_count = 1;

        void* first = 0;
        void* last = 0;
        void* found = 0;
        void* endIter = 0;
        stVectorLog_begin(&first, m_hackLogs);
        stVectorLog_end(&last, m_hackLogs);
        stLog_find(&found, first, last, &log);
        stVectorLog_end(&endIter, m_hackLogs);
        if (stLogIter_eq(&found, &endIter))
        {
            stVectorLog_push_back(m_hackLogs, &log);
        }
        else
        {
            stLogIter_arrow(&found)->m_count += 1;
        }
    }
}
