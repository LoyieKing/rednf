#ifndef DNF_GAME_CTRACE_MOB_DIE_HACK_H_
#define DNF_GAME_CTRACE_MOB_DIE_HACK_H_

#include <map>
#include <vector>

class CUser;

struct stMonsterDamageLog_t
{
    stMonsterDamageLog_t();
    unsigned short m_monsterId;
    unsigned short m_damage;
    int m_count;
    int m_monsterIndex;
};

struct stHackLog_t
{
    bool operator==(const stHackLog_t& other) const;
    unsigned int m_accId;
    unsigned int m_characNo;
    unsigned short m_hackType;
    int m_value;
    int m_count;
};

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

    long m_traceBeginTime;
    std::map<unsigned short, stMonsterDamageLog_t> m_map1;
    std::map<unsigned short, stMonsterDamageLog_t> m_map2;
    std::vector<int> m_mobKillers;
    std::vector<stHackLog_t> m_hackLogs;
    char m_bTracing;
    char m_pad4d[3];
    CUser* m_users[4];
    int m_damage[4];
    int m_count[4];
    bool m_bInspectUser;
    char m_pad81[3];
    int m_dungeonDiff;
    char m_pad88[0x0c];
};

#endif
