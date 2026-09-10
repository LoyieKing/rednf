// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CAICharacterList/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
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
#include "CAICharacterList/getByAP.inc"
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
#include "CAICharacterList/getMinAP.inc"
{
    return m_maxAP;
}

// ORIG 0x08150880 _ZNK16CAICharacterList10getAPCountEv
// ORIG 调用方（CBossStage/CDeathTower consistMap）把返回值作为
// map<int,int>::operator=(const map&) 的实参 —— 即返回 m_apStatMap 引用。
#include "CAICharacterList/getAPCount.inc"
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

#include "CAICharacterList/importAICharacterCommonScript.inc"
                                   const char* path, bool byLevel);

// ---- init（ORIG 0x0834a28c）：truncate 失败打印并返 false；m_maxAP 置
//      0x7fffffff；遍历文件列表逐条 clear→new→import（失败 printf+析构释放+
//      返 false）→FindIndex→insert_db（失败 printf+返 false，ORIG 不释放
//      aiChar）→独立掉落收集（push_back 到 script.m_f2c）→set（重新
//      FindIndex）→_add。----
#include "CAICharacterList/init.inc"
{
    if (!sync_script::GetInstanceSyncScript()->truncate_aicharacter_info_table())
    {
        std::puts("Fail truncate_aicharacter_info_table");
        return false;
    }

    m_maxAP = 0x7fffffff;   // +0x4bc（ORIG 0x0834a2c5 `movl $0x7fffffff,0x4bc(%eax)` 直落）

    // ORIG 0x0834a2cf：fileName 在循环外置 0（P28 声明序 = 槽序）。
    const char* fileName = 0;
    // [b28d-L14 实证] ORIG 0x0834a28c：非 const _Rb_tree_iterator + 默认构造
    // （C1Ev）+ 后置 it++（ppEi）——P50 复活 / P28 增量侧判据。
    std::map<int, std::string>::iterator it;
    STAICharacterCommonScript script;
    it = g_aiCharacterScriptFileList.m_fileMap.begin();
    while (it != g_aiCharacterScriptFileList.m_fileMap.end())
    {
        fileName = it->second.c_str();
        if (fileName == 0)
            break;

        script.clear();

        CAICharacter* aiChar = new CAICharacter;
        if (!importAICharacterCommonScript(&script, fileName, false))
        {
            std::printf("AI charac Script Error: %s\n", fileName);
            delete aiChar;                    // ORIG 0x0834a3b3 test/je + D1 + _ZdlPv
            return false;
        }

        const int fileIdx =
            g_aiCharacterScriptFileList.FindIndexByFullScan(fileName);
        if (!sync_script::GetInstanceSyncScript()->insert_aicharacter_info_to_db(fileIdx, script))
        {
            std::printf("insert_aicharacter_category Error: %s\n", fileName);
            return false;                     // ORIG 此路径不释放 aiChar（忠实保留）
        }

        // 独立掉落收集（G_CDataManager()+0xa890 = m_independentDrop.m_vecC）。
        // ORIG 0x0834a4a9：push_back 目标是 script+0xf2c（m_f2c），非局部 vector；
        // 0x0834a4b2 cmp/setg 为有符号比较 ⇒ 下标 int；每字段独立 operator[]
        // （P157，不缓存元素引用）。
        std::vector<stIndependentDropInfo>& drops =
            G_CDataManager()->m_independentDrop.m_vecC;
        for (int i = 0; i < (int)drops.size(); ++i)
        {
            if (drops[i].m_field0 == 1 && drops[i].m_monsterIndex == fileIdx)
                script.m_f2c.push_back(drops[i].m_drop);
        }
        // [S2CAICharList 2026-09-11 负结论①] 上面的 push_back 触发本 TU 两个 weak 符号
        //   _ZN26stMonsterIndependentDrop_tC1ERKS_（ORIG 0x08376bb2，58 条）与
        //   _ZN26stMonsterIndependentDrop_taSERKS_（ORIG 0x083a3942，59 条）；本 TU 52/53
        //   条（gap=+6，DIFF）。根因**不是**编译器分块差异，而是 CDataManager.h:398 的成员
        //   划分：ORIG 的 +0x04..+0x2c 是 4 个独立成员（边界 5/4/1/1，与语义分组一致：
        //   分档掉率×5 / 期望值×4 / 数量上限 / 主掉率），非 int[11] 一体数组。
        //   探针实证：int[5]+int[4]+int+int ⇒ C1ERKS_/aSERKS_ 与 ORIG **逐条一致**（含每块
        //   结尾 `mov 0x14(%edx),%edx` 破 src 基址、随后重载）；int[11] ⇒ 11 连发（我方现状）；
        //   union 数组视图（保留 m_rate[11] 名）⇒ 仍 11 连发（GCC 按最大成员整体拷贝）。
        //   ⇒ 可修但**不能只在本 TU 内修**：拆成员会破坏 3 个其它 TU 的 `m_rate[i]` 访问
        //   （MonsterIndependentDrop.cpp / DomainMiscDeps.cpp / GmCmds_D.cpp），需父泳道协调。
        // [负结论②] _ZNSt20__uninitialized_copyILb0EE…<__normal_iterator<const pair<int,int>*
        //   …>,pair<int,int>*>（ORIG 0x0823eade）：我方 56 条 vs ORIG 39 条，差全在 EH
        //   landing pad（我方循环体 39 条与 ORIG 逐条一致）。探针：同头集 + -fno-exceptions
        //   ⇒ size=0x6f = ORIG 的 nm size 0x6f，n1_v verdict=AE（39/39）；带 -fexceptions
        //   恒 0xa5/56 条。ORIG 本 TU 确有 EH（_add 0x0834a8a8 `call _Unwind_Resume`）⇒
        //   ORIG 链接时该 COMDAT 取自另一个 -fno-exceptions TU（PacketBuf.cpp 族），per-TU
        //   工具按符号名归属给本 TU ⇒ **工具口径缺口（weak COMDAT 跨 TU 归属）**，源码无解。

        aiChar->set(
            g_aiCharacterScriptFileList.FindIndexByFullScan(fileName), script);
        _add(aiChar);
        it++;
    }
    return true;
}

// ---- _add（ORIG 0x0834a61c）：charMap 按 idx insert，重复返 false；
//      byLevel 桶 [level-5, level+5) 无符号循环盲写（lv>0x62 时 cMyTrace
//      告警仍写入）；ap!=0 时 groupMap find-or-insert、m_maxAP 取无符号
//      min、apStatMap 以 ap 为键计数（value 恒 1）；最后按 getRarity
//      （+0x08 int）入 m_byAPList 桶。----
#include "CAICharacterList/_add.inc"
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
            cMyTrace(__PRETTY_FUNCTION__, 0x537, 0)(
                "aiCharacter Script file load failed");
        }
        m_byLevelList[lv].push_back(aiChar);   // this+lv*12+0x18 盲写
    }

    // ORIG 0x0834a726：外层判定、m_maxAP 段、apStatMap 段均逐次调用
    // getAppearancePoint（P48）；仅 find 用具名 ap。
    if (aiChar->getAppearancePoint() != 0)
    {
        // [S2CAICharList] ORIG 0x0834a749 `mov %eax,-0x58(%ebp)` + 0x0834a75b
        // `lea -0x58(%ebp),%edx`（find 实参）⇒ -0x58 是**表达式临时**而非具名局部。
        // 负结论（三个写法的实测）：具名 ap 声明在函数头 → 槽 -0x7c；声明在 if 块首
        // → 槽 -0x84；两者都让整函数槽位全线偏移（ndiff 39 行）。本写法（直接传
        // getAppearancePoint()）令槽位逐条对齐：ndiff 39 → 5（余仅 m_maxAP 类型差）。
        std::map<int, std::vector<CAICharacter*> >::iterator group =
            m_groupMap.find(aiChar->getAppearancePoint());
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

        // ORIG 0x0834a8be：三元（cond 走 jae 无 bool 物化、两臂共用尾部 store；
        // then 臂重载 m_maxAP，else 臂重调 getAppearancePoint）。m_maxAP 为
        // int 与 unsigned 右操作数比较 ⇒ 无符号序（jae），勿写成有符号。
        // 残余 1 条（ORIG 0x0834a8c1 `mov 0x4bc(%eax),%ebx` 直落 vs 我方
        // `mov 0x4bc(%eax),%eax; mov %eax,%ebx`）根因 = **m_maxAP 的声明类型 int**：
        // 探针 A/B（同表达式、同编译器）实证 int 成员恒多一条中转、unsigned 成员直落。
        // 本 TU 内三个等价写法实测无效（负结论）：①双侧显式 (unsigned) cast 不变；
        // ②reinterpret_cast<unsigned int&> → `add $0x4bc,%eax; mov (%eax),%ebx`（2 条）；
        // ③*(unsigned int*)((char*)this+0x4bc) → 同上 add 形式。**已落地**：
        // CDataManager.h 的 `m_maxAP` 改 `unsigned int`（2026-09-11，父泳道授权后）。
        m_maxAP = (m_maxAP < (unsigned int)aiChar->getAppearancePoint())
                      ? m_maxAP
                      : aiChar->getAppearancePoint();

        // ORIG 0x0834a8f5：make_pair 第一实参 = getAppearancePoint()（key），
        // 第二实参 = 常量 1（value）——0x0834a910 arg2=&(-0x14)=1、
        // 0x0834a917 arg1=&(-0x10)=ap。插入失败时把该 AP 的计数 +1。
        std::pair<std::map<int, int>::iterator, bool> stat =
            m_apStatMap.insert(std::make_pair(aiChar->getAppearancePoint(), 1));
        if (!stat.second)
            stat.first->second = stat.first->second + 1;
    }

    m_byAPList[aiChar->getRarity()].push_back(aiChar);   // this+rarity*12+0x4d8
    return true;
}
