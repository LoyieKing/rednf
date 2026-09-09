// CAICharacterList.cpp
// ORIG 依据：objdump 0x0834aaf4(getByAP T) / 0x08150872(getMinAP W) /
//   0x08150880(getAPCount W)。布局见 CDataManager.h mirror 类：
//   m_groupMap(+0x4c0, map<int, vector<CAICharacter*>>)、
//   m_maxAP(+0x4bc, init 置 0x7fffffff)、m_apStatMap(+0x520, map<int,int>)。
#include "CDataManager.h"
#include "GameRand.h"  // get_rand_int
#include "GameTypes.h"  // cMyTrace（_add 越界告警路径）

#include <cstdio>
#include <string>
#include <utility>

#include <map>
#include <vector>
#include "NpcScript.h"

typedef std::map<int, std::vector<CAICharacter*> > APGroupMap;

// ORIG 0x0834aaf4 _ZN16CAICharacterList7getByAPEj
//   m_groupMap.empty() → 0；
//   it = lower_bound(ap)；it==end → --it；
//   (unsigned)it->first > ap 且 it!=begin → --it；
//   it->second.size()==1 分支的 at(0) 结果在 ORIG 中被丢弃（忠实转写）；
//   返回 at(get_rand_int(size()))。
CAICharacter* CAICharacterList::getByAP(unsigned int ap)
{
    if (m_groupMap.empty())
        return 0;

    // [b28d-L14 实证] ORIG 0x0834a38e 全用 _Rb_tree_const_iterator（含 4 次
    // 转换构造 + const 版 operator--/at，P96/P270 反向：此处须 const_iterator）。
    APGroupMap::const_iterator it = m_groupMap.lower_bound((int)ap);
    // ORIG 0x0834ab66/0x0834aba9：end() 求值两次（比较 + 赋值后再自减）。
    if (it == m_groupMap.end())
    {
        it = m_groupMap.end();
        --it;
    }

    if (((unsigned int)it->first > ap) && it != m_groupMap.begin())
        --it;

    if (it->second.size() == 1)
        it->second.at(0);  // ORIG 汇编如此调用但未使用结果

    return it->second.at(get_rand_int((int)it->second.size()));
}

// ORIG 0x08150872 _ZN16CAICharacterList8getMinAPEv（非 const）
int CAICharacterList::getMinAP()
{
    return m_maxAP;
}

// ORIG 0x08150880 _ZNK16CAICharacterList10getAPCountEv
// ORIG 调用方（CBossStage/CDeathTower consistMap）把返回值作为
// map<int,int>::operator=(const map&) 的实参 —— 即返回 m_apStatMap 引用。
std::map<int, int> const& CAICharacterList::getAPCount() const
{
    return m_apStatMap;
}

// ============================================================================
// CAICharacterList::init（ORIG T 0x0834a28c，报告 CAICharacterList/init.md）
// 与 _add（ORIG T 0x0834a61c，报告 CAICharacterList/_add.md）。
//
// 待落地引用（定义归脚本导入/AI 批次，此处仅产生 ORIG mangled 引用）：
//   STAICharacterCommonScript C1 0x088c8a78 / clear 0x088c9592 / D1 0x088c903c
//   importAICharacterCommonScript 0x088c9ccc（报告 _lt_global_gt_/importAICharacterCommonScript.md）
//   CAICharacter C1 0x08373a9a / D1 0x08373b32（stEnamyDropAvatarInfo_t 未建模，
//     无法忠实转写生命周期，留待 AI 角色批次）
// ============================================================================

// ---- sync_script 局部声明域（模式同 GameWorld.cpp：sync_script.h 内
//      STAICharacterCommonScript 占位与本 TU 完整视图冲突，故不含该头；
//      声明与 ORIG mangled 一致，真实定义在 sync_script.cpp / CItemList.cpp）----
class STAICharacterCommonScript;   // 不透明前置：完整视图见下方（init 仅传址）

namespace sync_script
{
class CSyncScript
{
public:
    bool truncate_aicharacter_info_table();
    bool insert_aicharacter_info_to_db(
        int idx, const STAICharacterCommonScript& script);
};
CSyncScript* GetInstanceSyncScript();   // _ZN11sync_script21GetInstanceSyncScriptEv
}  // namespace sync_script

// ---- STAICharacterCommonScript 本地完整视图（sizeof 0x1010：init 栈帧
//      -0x104c..-0x3c 实证；字段转写同 CtorProducers_1.cpp 视图。本 TU 仅作
//      存储与传址，不访问成员；生命周期符号由脚本导入批次提供）----
class STAICharacterCommonScript
{
public:
    STAICharacterCommonScript();    // ORIG T 0x088c8a78
    ~STAICharacterCommonScript();   // ORIG D1 0x088c903c
    void clear();                   // ORIG T 0x088c9592

    int m_field0;                                    // +0x000
    std::string m_str4;                              // +0x004
    int m_level;                                     // +0x008
    char m_padC[0xd80 - 0xc];                        // +0x00c..
    int m_d80;                                       // +0xd80
    int m_d84;                                       // +0xd84
    std::vector<std::pair<int, int> > m_d88;         // +0xd88
    char m_padD90[0xda4 - 0xd94];                    // +0xd90..
    int m_da4;                                       // +0xda4
    int m_da8;                                       // +0xda8
    char m_padDac[0xf2c - 0xdac];                    // +0xdac..
    std::vector<stMonsterIndependentDrop_t> m_f2c;   // +0xf2c
    char m_padF38[0xf50 - 0xf38];                    // +0xf38..
    char m_avatarDrops[0x100c - 0xf50];              // +0xf50（stEnamyDropAvatarInfo_t 区）
    unsigned char m_rarityByte;                      // +0x100c
    char m_pad100d[3];
    int m_animationJob;                              // +0x1010（ORIG clear 置 0xb；
                                                     //   importAI [animation job] 写入）
};
typedef char assert_stai_script_size[
    sizeof(STAICharacterCommonScript) == 0x1014 ? 1 : -1];

// ---- CAICharacter 本地完整镜像（sizeof 0x74 = init 的 operator new $0x74
//      实证；前缀布局与 MonsterIndependentDrop.h 镜像一致。四个访问器为
//      类内 inline → 弱符号，与 ORIG W 属性一致【报告缺失，objdump 转写】；
//      ctor/dtor/set 定义待 AI 批次/CtorProducers_1 提供）----
class CAICharacter
{
public:
    CAICharacter();                                               // ORIG W 0x08373a9a（外部定义）
    ~CAICharacter();                                              // ORIG W 0x08373b32（外部定义）
    int getIdx() const { return m_idx; }                          // ORIG W 0x08150850
    int getLevel() const { return m_level; }                      // ORIG W 0x0815085a
    int getRarity() const { return m_field8; }                    // ORIG W 0x08373a8e：取 +0x08 int
    int getAppearancePoint() const { return m_appearancePoint; }  // ORIG W 0x08150866
    void set(int idx, const STAICharacterCommonScript& script);   // ORIG T 0x0834a066（CtorProducers_1 定义）

    int m_idx;                                                   // +0x00
    int m_level;                                                 // +0x04
    int m_field8;                                                // +0x08（ORIG getRarity 取值处）
    int m_appearancePoint;                                       // +0x0c
    std::vector<std::pair<int, int> > m_dropItems;               // +0x10
    int m_pvpWinLow;                                             // +0x1c
    int m_pvpWinHigh;                                            // +0x20
    std::vector<stMonsterIndependentDrop_t> m_independentDrops;  // +0x24
    char m_avatarDrops[0x6c - 0x30];                             // +0x30 stEnamyDropAvatarInfo_t 区（未建模）
    std::string m_name;                                          // +0x6c
    unsigned char m_rarity;                                      // +0x70（set 写入）
    char m_pad71[3];
};
typedef char assert_cai_character_size[sizeof(CAICharacter) == 0x74 ? 1 : -1];


extern STScriptFileList g_aiCharacterScriptFileList;  // B 0x094f6a20

bool importAICharacterCommonScript(STAICharacterCommonScript* script,
                                   const char* path, bool byLevel);

// ---- init（ORIG 0x0834a28c）：truncate 失败打印并返 false；m_maxAP 置
//      0x7fffffff；遍历文件列表逐条 clear→new→import（失败 printf+析构释放+
//      返 false）→FindIndex→insert_db（失败 printf+返 false，ORIG 不释放
//      aiChar）→独立掉落收集→set（重新 FindIndex）→_add。
//      【偏离点】ORIG 将掉落匹配项 push_back 到一个从未构造/从未析构、亦无
//      任何读者的栈上 vector（ebp-0x120）；本实现以正常构造的局部 vector
//      表达同一扫描，可观察语义不变（消除对未初始化栈内存的 UB 复制）。----
bool CAICharacterList::init()
{
    sync_script::CSyncScript* sync = sync_script::GetInstanceSyncScript();
    if (!sync->truncate_aicharacter_info_table())
    {
        std::puts("Fail truncate_aicharacter_info_table");
        return false;
    }

    m_maxAP = static_cast<int>(0x7fffffff);   // +0x4bc

    bool result = true;
    STAICharacterCommonScript script;
    // [b28d-L14 实证] ORIG 0x0834a28c：非 const _Rb_tree_iterator + 默认构造
    // （C1Ev）+ 后置 it++（ppEi）——P50 复活 / P28 增量侧判据。
    std::map<int, std::string>::iterator it;
    it = g_aiCharacterScriptFileList.m_fileMap.begin();
    while (it != g_aiCharacterScriptFileList.m_fileMap.end())
    {
        const char* fileName = it->second.c_str();
        if (fileName == 0)
            break;

        script.clear();

        CAICharacter* aiChar = new CAICharacter;
        if (!importAICharacterCommonScript(&script, fileName, false))
        {
            std::printf("AI charac Script Error: %s\n", fileName);
            aiChar->~CAICharacter();          // ORIG：显式 dtor + operator delete
            ::operator delete(aiChar);        // （无 deleting dtor，勿改 delete）
            result = false;
            break;
        }

        const int fileIdx =
            g_aiCharacterScriptFileList.FindIndexByFullScan(fileName);
        if (!sync->insert_aicharacter_info_to_db(fileIdx, script))
        {
            std::printf("insert_aicharacter_category Error: %s\n", fileName);
            result = false;                   // ORIG 此路径不释放 aiChar（忠实保留）
            break;
        }

        // 独立掉落收集（G_CDataManager()+0xa890 = m_independentDrop.m_vecC；
        // 访问模式同 CListTables.cpp:632）。m_field0==1 且 m_monsterIndex 匹配
        // 时收集元素内嵌 stMonsterIndependentDrop_t（+0x08）。
        {
            std::vector<stIndependentDropInfo>& drops =
                G_CDataManager()->m_independentDrop.m_vecC;
            std::vector<stMonsterIndependentDrop_t> collected;
            for (unsigned int i = 0; i < drops.size(); ++i)
            {
                stIndependentDropInfo& entry = drops[i];
                if (entry.m_field0 == 1 && entry.m_monsterIndex == fileIdx)
                    collected.push_back(entry.m_drop);
            }
        }

        aiChar->set(
            g_aiCharacterScriptFileList.FindIndexByFullScan(fileName), script);
        _add(aiChar);
        it++;
    }
    return result;
}

// ---- _add（ORIG 0x0834a61c）：charMap 按 idx insert，重复返 false；
//      byLevel 桶 [level-5, level+5) 无符号循环盲写（lv>0x62 时 cMyTrace
//      告警仍写入）；ap!=0 时 groupMap find-or-insert、m_maxAP 取无符号
//      min、apStatMap 以常量键 1 计数；最后按 getRarity（+0x08 int）入
//      m_byAPList 桶。----
bool CAICharacterList::_add(CAICharacter* aiChar)
{
    std::pair<std::map<int, CAICharacter*>::iterator, bool> ins =
        m_charMap.insert(std::make_pair(
            static_cast<unsigned int>(aiChar->getIdx()), aiChar));   // ORIG make_pair<unsigned int,...>
    if (!ins.second)
        return false;

    // ORIG 0x0834a697/0x0834a70e：getLevel 两次独立求值（P48，无 level 缓存）。
    for (unsigned int lv = (unsigned int)aiChar->getLevel() - 5;
         (unsigned int)aiChar->getLevel() + 5 > lv; ++lv)
    {
        if (lv > 0x62)   // ORIG cmp $0x62,jbe：仅告警不拦截
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x537, 0);
            trace("aiCharacter Script file load failed");
        }
        m_byLevelList[lv].push_back(aiChar);   // this+lv*12+0x18 盲写
    }

    // ORIG 0x0834a726：外层判定、m_maxAP 段、apStatMap 段均逐次调用
    // getAppearancePoint（P48）；仅 find 用块内具名 ap（-0x58）。
    if (aiChar->getAppearancePoint() != 0)
    {
        int ap = aiChar->getAppearancePoint();
        std::map<int, std::vector<CAICharacter*> >::iterator group =
            m_groupMap.find(ap);
        // ORIG 0x0834a79c 用 operator!=（then=push_back / else=fresh，P220）。
        if (group != m_groupMap.end())
        {
            group->second.push_back(aiChar);
        }
        else
        {
            std::vector<CAICharacter*> fresh;
            fresh.push_back(aiChar);
            m_groupMap.insert(std::make_pair(aiChar->getAppearancePoint(), fresh));
        }

        // ORIG 0x0834a8be：无符号 min，两分支各赋值（else 支重调 getAppearancePoint）。
        if ((unsigned int)m_maxAP < (unsigned int)aiChar->getAppearancePoint())
            m_maxAP = m_maxAP;
        else
            m_maxAP = aiChar->getAppearancePoint();

        std::pair<std::map<int, int>::iterator, bool> stat =
            m_apStatMap.insert(std::make_pair(1, aiChar->getAppearancePoint()));   // ORIG 键恒为 1
        if (!stat.second)
            stat.first->second = stat.first->second + 1;
    }

    m_byAPList[aiChar->getRarity()].push_back(aiChar);   // this+rarity*12+0x4d8
    return true;
}
