#ifndef GAME_STMONSTERSCRIPT_H_
#define GAME_STMONSTERSCRIPT_H_

// ============================================================================
// df_game_r 还原 —— STMonsterScript 类方法（G5 怪物脚本表）
// ORIG：Clear 0x08a398aa / ctor 0x08368e14 / dtor 0x08378dac /
// operator= 0x08377994。自由函数 ImportMonsterScript 0x08a12827 /
// ImportMonsterBaseTable 0x08a3c736 / getMonsterTableIndex 0x088ba1c8 /
// setBasicAbilityByLevel 0x088b9603 / getRandomChampionElement 0x088b9cce /
// exportMonsterScript 0x08a22520（语义简化实现）。
// 怪物能力自由函数（upgradeAbilityBy*/makeChampionFeatureList）权威声明见本头。
// ============================================================================

#include <map>
#include <string>
#include <vector>

#include "ActiveStaticInfo.h"

class STMonsterScript
{
public:
    STMonsterScript();
    ~STMonsterScript();
    STMonsterScript& operator=(const STMonsterScript& other);
    void Clear();

    int m_field0;                            // +0x00
    std::string m_str4;                      // +0x04
    char m_pad[0x74];                        // +0x08..+0x7b
    std::vector<int> m_vec7c;                // +0x7c
};

int ImportMonsterScript(STMonsterScript* script, int type, const char* path,
                        bool flag);
bool ImportMonsterBaseTable(STMonsterScript* script, const char* path);
bool exportMonsterScript(
    STMonsterScript* script, const char* path,
    std::map<std::string, std::vector<std::string> >& extra);
int getMonsterTableIndex(const STMonsterScript* script, int a, int b,
                         bool c, bool d);
void setBasicAbilityByLevel(STMonsterScript* script, ActiveStaticInfo* info,
                            unsigned int level, int dungeonIdx, int roleType,
                            bool a, bool b);
int getRandomChampionElement(STMonsterScript* script, int seed);

// ---- 怪物能力升级自由函数（ORIG 实现见 CBattle_Field_unresolved.cpp）----
void upgradeAbilityByMonsterParameterCategory(std::vector<int>& vec,
                                              ActiveStaticInfo* info,
                                              bool isChampion);
void upgradeAbilityByDifficultyOfDungeon(int diff, ActiveStaticInfo* info,
                                         unsigned int level, std::vector<int>& vec);
void upgradeAbilityByBossFeature(ActiveStaticInfo* info);
void makeChampionFeatureList(std::vector<int>& out, unsigned int level, int seed,
                             std::vector<int>& tmp, int diff);
void upgradeAbilityByChampionFeature(int elem, int roleType, std::vector<int>& vec,
                                     ActiveStaticInfo* info);

#endif  // GAME_STMONSTERSCRIPT_H_
