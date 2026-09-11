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

#include "CAICharacterList/getByAP.inc"


#include "CAICharacterList/getMinAP.inc"


#include "CAICharacterList/getAPCount.inc"


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

#include "CAICharacterList/init.inc"


#include "CAICharacterList/_add.inc"

