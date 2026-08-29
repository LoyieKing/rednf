#ifndef DNF_GAME_C_EXPERT_JOB_LIST_H_
#define DNF_GAME_C_EXPERT_JOB_LIST_H_

// df_game_r 专家职业脚本列表（G5 批次）。
// 布局依据 CExpertJobList::init（0x896d8ec）/ destroy（0x896dac6）反汇编：
//   +0x00 std::map<int, STExpertJobScript*> m_jobMap（0x18）
//   +0x18 STExpertJobEtcScript m_etc（int 族 + std::map<uchar,short>，0x18）
//   ctor（0x896e312 键）默认 map；init 填充；destroy 遍历 delete。
// STExpertJobScript 权威声明见 STExpertJobScript.h（0x290，m_expVec）。

#include <map>

#include "STExpertJobScript.h"

// STExpertJobEtcScript：专家职业「.etc」脚本。布局由 ImportExpertJobEtcScript
// 反汇编（0x896ae21）恢复：
//   +0x00 std::map<unsigned char, short>（[expertjob level limit]：等级→所需角色等级）
//   +0x18 std::map<unsigned char, unsigned char>（[disjointer big win rate]）
// CUser::GetCurExpertJobLevel 将 etc 首部作为 map<uchar,short> 读取（+0x00 不变）。
class STExpertJobEtcScript
{
public:
    std::map<unsigned char, short>   m_levelMap;             // +0x00
    std::map<unsigned char, unsigned char> m_disjointerBigWinRate;  // +0x18

    // ORIG C1 0x0849f438：构造两 map 后调 Clear()。
    STExpertJobEtcScript();
    void Clear();

    // ORIG W 0x0849f47c：遍历 m_levelMap，任一 mapped short == level 返回 true。
    bool isExpertJobLimitLevel(int level);
};

// ORIG 全局（B g_expertJobScriptFileList 0x094f7760 为 0x18 STScriptFileList
// 对象 / g_expertJobScriptBaseDirectory 0x094f7660[0x100]）。由
// initExpertJobScript（InitScriptProducers_5.cpp）填充；init() 直接遍历对象
// m_fileMap（ORIG 0x896d90a map begin 直取 0x94f7760，无空指针检查）。
class STScriptFileList;
extern STScriptFileList g_expertJobScriptFileList;
extern char g_expertJobScriptBaseDirectory[0x100];

// ORIG 导入函数（数据域 TU 提供；本头仅声明）：
//   _Z21ImportExpertJobScriptP17STExpertJobScriptPKc       0x896b0e4
//   _Z24ImportExpertJobEtcScriptR20STExpertJobEtcScriptPKc 0x896ae21
bool ImportExpertJobScript(STExpertJobScript* script, const char* path);
bool ImportExpertJobEtcScript(STExpertJobEtcScript& script, const char* path);

class CExpertJobList
{
public:
    CExpertJobList();                                  // ORIG 0x896e312（map 构造）
    ~CExpertJobList();                                 // 派生销毁

    bool init();                                       // ORIG 0x896d8ec
    void destroy();                                    // ORIG 0x896dac6

    STExpertJobScript* get(int job);                   // ORIG W：m_jobMap 查 job
    STExpertJobEtcScript* get_etc() { return &m_etc; } // ORIG W：return this+0x18

    std::map<int, STExpertJobScript*> m_jobMap;   // +0x00
    STExpertJobEtcScript m_etc;                   // +0x18
};

#endif  // DNF_GAME_C_EXPERT_JOB_LIST_H_
