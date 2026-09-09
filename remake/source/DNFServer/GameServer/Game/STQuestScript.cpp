// ============================================================================
// df_game_r 还原 —— G5 任务/城镇脚本域（STQuestScript.cpp）
// 对照 docs/class_func_reports/QuestScript.md 与 ORIG 反汇编：
//   QuestScript        ctor 0x0836e4e2 / copy 0x0837adc8 / dtor 0x0836eaf0
//                      clear 0x0836deba / destroy 0x08a6130a
//                      getName 0x08a611fc / getRequestMessage 0x08a61240
//                      getConditionMessage 0x08a61284 / getSolveMessage 0x08a612c6
//   TownScript         clear 0x086d1e2e / ctor 0x086d1f04 / dtor 0x086d2ab8
//   TrainingQuestScript ctor 0x08371cc8 / clear 0x08371dd4 / dtor 0x0837d488
//                      computeDayIndex 0x08aafc64 / get_random_fake_value 0x08aafc90
//                      suffleTrainingQuests 0x08aafcbc / getTrainingQuest 0x08aafb44
//                      getApplyLevel 0x08aaf8b6
// 解析辅助（loadRDARScriptFile/ScanType/ScanInt/loadRDARScriptListFile）经
// DNFLexWrapper.h 权威声明调用真实符号。
// ============================================================================

#include "STQuestScript.h"

#include <algorithm>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "Quest.h"
#include "MyRandom.h"          // suffleTrainingQuests 的 random_shuffle 随机数器

#include "DNFLexWrapper.h"
#include "CEnvironment.h"      // class SpinLock / less_TCHAR（importQuestScript [type] 静态表）
#include "SyncScriptDeps.h"    // checkValiedJobName（importQuestScript [target character]）

// loadRDARScriptListFile 定义于 STScriptFileList.cpp（ORIG T 0x088bbd77，
// mangled _Z22loadRDARScriptListFileP16STScriptFileListPKcS2_PSt3mapIiiSt4lessIiESaISt4pairIKiiEEE）。
// STScriptFileList 无权威头，此处仅前置声明 + 正常 C++ 自由函数声明（非 asm 桥）。
// STScriptFileList 权威定义见 NpcScript.h（经 Quest.h include 链可见）。
// 本 TU 按 bool 视图声明（P22：ORIG init* 尾 @0x8a616e9 xor $1,%eax;test %al
// 为 bool 取反形；返回类型不入 mangled，链接 STScriptFileList.cpp 的 int 定义，
// 其返回恒 0/1，ABI 兼容——StrikerScript.cpp 同例先）。
bool loadRDARScriptListFile(STScriptFileList* list, const char* dir,
                            const char* listPath, std::map<int, int>* map);
// ---- importQuestScript 依赖的外部符号（权威定义分属其他 TU）----
bool IsLightServer();                                        // _Z13IsLightServerv 0x0822ad44（GlobalData.cpp）
void OutputErrorMessage(const char* msg);                    // _Z18OutputErrorMessagePKc（STMapScript.cpp）
bool ScanRewardItems(std::vector<RewardItemInfo>& out, std::string token);  // _Z15ScanRewardItems... 0x08a65da5（OnlyOrigBatch14.cpp）
bool ConvertStringToTime(std::string s, long& out);          // 0x088c2341（InitScriptProducers_6.cpp）
bool ConvertWeekdayStringToInt(std::string s, unsigned int& out);  // 0x088c284a
bool ConvertHHMMSSStringToInt(std::string s, unsigned char& h,
                              unsigned char& m, unsigned char& sec);  // 0x088c25df
bool ScanJobStr(std::string* jobName, std::string** out);  // _Z10ScanJobStrPSsPS_ 0x08a6143f（ScanJobStr.cpp）


// ---- ORIG 全局脚本文件列表缓冲（initQuestScript/initTownScript 使用）。
// g_questScriptBaseDirectory 定义见下（ORIG B 0x9500380，供 EpisodeScript 等
// loadRDARScriptFile 首参，另一处 0x580 行已定义）；此处仅 town 目录定义。
char g_townScriptBaseDirectory[0x100];    // ORIG B 0x9500da0
// [b28d-solo-STQuestScript · P304] 权威型声明：ORIG 调用点 `movl $&g_..,(%esp)`
// 为地址直压；声明成 std::map 后再 reinterpret_cast 会物化 %eax（P304）。
// STScriptFileList 的 +0 即 std::map<int,std::string>（NpcScript.h，sizeof 0x18），
// 定义处（GameWorld.cpp map / InitScriptProducers_4.cpp map）符号同名同址，ABI 不变。
extern STScriptFileList g_townScriptFileList;  // 定义于 GameWorld.cpp（ORIG B 0x9500ea0）
// g_questScriptFileList 权威定义见 InitScriptProducers_4.cpp（ORIG B 0x9500480）。
extern STScriptFileList g_questScriptFileList;

// ===================== QuestScript =====================

// [b28d-solo-STQuestScript · P69] ORIG C1 0x0836e4e2（487 条）：40+ 成员子对象隐式
// 构造（含 EH cleanup 链）后**单一 `call QuestScript::clear()`**（836e738），ctor 体
// 无任何 memset ⇒ 原码 `QuestScript::QuestScript() { clear(); }`。
QuestScript::QuestScript()
{
    clear();
}

// [b28d-Lane SymSrvParam · N84 处置③] ORIG _ZN11QuestScriptC1ERKS_ 0x0837adc8
// 为 W 隐式合成（0xd65）；CListTables.cpp 镜像类仅有拷贝 ctor **声明**（U 引用，
// 防 0x364/0x30c ODR 竞争，Main b28d 决定），全仓唯一 W 发射点必须在本 TU。原
// forceQuestScriptCopyEmit（t 符号，ORIG 无）改为数据锚：文件作用域 static 对象
// 定义（b 符号，非函数、不导出于链接命名空间）以拷贝初始化 ODR-use 合成拷贝
// ctor ⇒ 4.4 在本 TU COMDAT 发射同名 W；构造/析构并入既有
// __static_initialization_and_destruction_0 / _GLOBAL__I_*（t，ORIG 同名并存）。
// 已核 ORIG 无该类型全局对象：`nm -S $O | grep ' [BbDd] '` 无 size 0x30c
// （QuestScript）命中（同名前缀仅 g_questScriptFileList 0x18 /
// g_questScriptBaseDirectory 0x100）⇒ 本锚唯一作用是保留 W 发射点，
// 非物化任何 ORIG 数据。
// 语义中性：clear() 纯成员置零、destroy() 对 0 指针安全，无外部可观察副作用。
static QuestScript g_questScriptCopyEmitSrc;
static QuestScript g_questScriptCopyEmitUse(g_questScriptCopyEmitSrc);



// ORIG D1 0x08a70700：先 destroy()（delete 四组 11 指针），再成员自动析构。
QuestScript::~QuestScript()
{
    destroy();
}


void QuestScript::clear()
{
    m_index = -1;                    // +0x00
    m_name = "";                     // +0x04
    m_requestDefault = "";           // +0x08
    m_conditionDefault = "";         // +0x0c
    m_solveDefault = "";             // +0x10
    m_fieldC8.clear();               // +0xc8
    m_str2e8 = "";                   // +0x2e8（ORIG 顺序：早于 m_questType）
    m_2ec = 0;                       // +0x2ec
    m_questType = 4;                 // +0xd4
    m_npcIdx = 0;                    // +0xe4
    m_fieldE8 = -1;                  // +0xe8
    m_field134 = -1;                 // +0x134
    m_jobChangeQuest = 0;            // +0xdc
    m_field1b0.clear();              // +0x1b0
    m_field1bc = 0;                  // +0x1bc
    m_field1c0 = -1;                 // +0x1c0
    m_postalReward.clear();          // +0x298
    m_field1c4 = 4;                  // +0x1c4
    m_targetCharacters.clear();      // +0xf4
    m_fieldE0 = -1;                  // +0xe0
    m_fieldD8 = 3;                   // +0xd8
    m_fieldEC = 0xb;                 // +0xec
    m_fieldF0 = -1;                  // +0xf0
    for (int i = 0; i <= 1; ++i)
        m_levelMin[i] = 0;           // +0x100/+0x104
    m_field108 = 0;                  // +0x108
    m_intDataList.clear();           // +0x10c
    m_antiQuestList.clear();         // +0x118
    m_questCountLimit = 0;           // +0x124
    m_field130 = 0;                  // +0x130
    m_clearItems.clear();            // +0x138
    m_killItems.clear();             // +0x144
    m_appearMap.clear();             // +0x150
    m_rewardSelectList.clear();      // +0x160
    m_triggerType = 0;               // +0x178
    m_triggerSubType = 0;            // +0x17c
    m_triggerData.clear();           // +0x180
    m_field18c.clear();              // +0x18c
    m_rewardType = 0;                // +0x1d4
    m_titleReward.clear();           // +0x1d8
    m_field1e4 = -1;                 // +0x1e4
    m_field228 = -1;                 // +0x228
    m_field20c.clear();              // +0x20c
    m_field218 = 0;                  // +0x218
    m_rewardData.clear();            // +0x21c
    m_field22c = 0;                  // +0x22c
    m_field23d = 0;                  // +0x23d
    m_field240 = 100;                // +0x240
    m_str244 = "";                   // +0x244
    m_field248 = -1;                 // +0x248
    m_field24c = -1;                 // +0x24c
    m_field250 = -1;                 // +0x250
    m_field254 = -1;                 // +0x254
    m_mailQuestFlag = -1;            // +0x258
    m_mailTitle.clear();             // +0x25c
    m_mailContents.clear();          // +0x260
    m_mailItems.clear();             // +0x264
    m_field1f4.clear();              // +0x1f4
    m_selectReward.clear();          // +0x1e8
    m_eventDialogs.clear();          // +0x200
    m_field22d = 1;                  // +0x22d
    m_field22e = 0;                  // +0x22e
    for (int i = 0; i <= 2; ++i)
        m_autoDisplay.m_values[i] = -1;          // +0x230..+0x23b
    m_field23c = 0;                  // +0x23c
    m_field270 = 0;                  // +0x270
    m_field271 = 1;                  // +0x271
    m_powerSide = 0xff;              // +0x272
    m_field274 = -1;                 // +0x274
    m_field278 = 0;                  // +0x278
    m_field27c = 0;                  // +0x27c
    m_questIdx = 0;                  // +0x14
    for (int i = 0; i <= 10; ++i)
    {
        m_jobNames[i] = 0;            // +0x18
        m_jobRequests[i] = 0;         // +0x44
        m_jobConditions[i] = 0;       // +0x70
        m_jobSolves[i] = 0;           // +0x9c
    }
    m_field280 = 0;                  // +0x280
    m_field282 = 0;                  // +0x282
    m_field284 = -1;                 // +0x284
    m_field288 = -1;                 // +0x288
    m_field28c = -1;                 // +0x28c
    m_field290 = -1;                 // +0x290
    m_str294.clear();                // +0x294
    m_field2a8 = 0;                  // +0x2a8
    m_field2ac = -1;                 // +0x2ac
    m_2c8 = 0;                       // +0x2c8
    m_str2cc = "";                   // +0x2cc
    m_exceptionList.clear();         // +0x2d0
    m_field128 = 0;                  // +0x128
    m_field12c = 0;                  // +0x12c
    m_dailySchedule.clear();         // +0x2dc
    m_field12c = 0;                  // +0x12c
    m_field128 = 0;                  // +0x128
    m_vec2f0.clear();                // +0x2f0
    m_vec2fc.clear();                // +0x2fc
    m_308 = 0;                       // +0x308
}

void QuestScript::destroy()
{
    for (int i = 0; i < 0xb; ++i)
    {
        if (m_jobNames[i])
        {
            delete m_jobNames[i];
        }
        m_jobNames[i] = 0;
        if (m_jobRequests[i])
        {
            delete m_jobRequests[i];
        }
        m_jobRequests[i] = 0;
        if (m_jobConditions[i])
        {
            delete m_jobConditions[i];
        }
        m_jobConditions[i] = 0;
        if (m_jobSolves[i])
        {
            delete m_jobSolves[i];
        }
        m_jobSolves[i] = 0;
    }
}

std::string& QuestScript::getName(ENUM_CHARACTERJOB job)
{
    if (m_questIdx != 0 && job >= 0 && job <= 10 && m_jobNames[job])
    {
        return *m_jobNames[job];
    }
    return m_name;
}

std::string& QuestScript::getRequestMessage(ENUM_CHARACTERJOB job)
{
    if (m_questIdx != 0 && job >= 0 && job <= 10 && m_jobRequests[job])
    {
        return *m_jobRequests[job];
    }
    return m_requestDefault;
}

std::string& QuestScript::getConditionMessage(ENUM_CHARACTERJOB job)
{
    if (m_questIdx != 0 && job >= 0 && job <= 10 && m_jobConditions[job])
    {
        return *m_jobConditions[job];
    }
    return m_conditionDefault;
}

std::string& QuestScript::getSolveMessage(ENUM_CHARACTERJOB job)
{
    if (m_questIdx != 0 && job >= 0 && job <= 10 && m_jobSolves[job])
    {
        return *m_jobSolves[job];
    }
    return m_solveDefault;
}

// ===================== TownScript =====================

TownScript::TownScript()
{
    clear();
}

TownScript::~TownScript()
{
}

void TownScript::clear()
{
    m_field0 = 0;
    m_str4 = "";
    m_str8 = "";
    m_strc = "";
    m_str10 = "";
    m_14 = 0;
    m_18 = -1;
    m_1c = -1;
    m_areas.clear();
    m_str38 = "";
    m_40 = 0;
    m_3c = m_40;
}

// ===================== TrainingQuestScript =====================

ST_TrainingQuest_Select_Prob::ST_TrainingQuest_Select_Prob()
{
    m_count = 0;
    m_pairs.clear();   // ORIG 正常路径仅 m_count=0 + m_pairs.clear()；数组由隐式构造初始化
}

ST_TrainingQuest_Select_Prob::~ST_TrainingQuest_Select_Prob()
{
}

TrainingQuestScript::TrainingQuestScript()
{
    // ORIG C1 0x8371cc8：成员数组隐式构造（70 个 vector）后仅调 clear()。
    clear();
}

TrainingQuestScript::~TrainingQuestScript()
{
}

void TrainingQuestScript::clear()
{
    m_selectProb.m_count = 0;
    m_selectProb.m_pairs.clear();
    m_applyCount = 0;
    for (int i = 0; i <= 0x45; ++i)
    {
        m_questProb[i].clear();
    }
    m_sameApplyQuest.clear();
}

void TrainingQuestScript::reset_same_apply_level_cnt()
{
    m_applyCount = 0;
}

int TrainingQuestScript::computeDayIndex()
{
    time_t now = time(0);
    struct tm* t = localtime(&now);
    return t->tm_yday;
}

int TrainingQuestScript::get_random_fake_value(int idx, int day)
{
    return m_selectProb.m_dayProb[day].at(idx);
}

void TrainingQuestScript::suffleTrainingQuests()
{
    int day = computeDayIndex();
    int count = m_selectProb.m_dayProb[day].at(0);
    // ORIG 0x08aafcbc：MyRandom(count) 作随机数器（3 参 random_shuffle）。
    MyRandom rnd(count);
    for (int i = 0; i <= 0x45; ++i)
    {
        // b28c-28（P223 + P168a①）：ORIG 0x08aafd2f 段为
        // `call empty; test %al,%al; jne <nop 岛>` + then 体尾 `jmp <岛后>`——
        // 即「正向 empty 判定 + goto 尾空标签」形；`if(!empty()){}` 被 4.4 折成
        // `xor$1; test; je`（极性反、少 jmp 岛）。
        if (m_questProb[i].empty())
            goto SHUFFLE_DONE;
        std::random_shuffle(m_questProb[i].begin(), m_questProb[i].end(), rnd);
SHUFFLE_DONE:;
    }
}

int TrainingQuestScript::getTrainingQuest(int dayIndex, int questCount, int job)
{
    if (questCount == job)
    {
        ++m_applyCount;
    }
    else
    {
        m_applyCount = 0;
    }
    if (questCount == -100)
    {
        if (m_applyCount >= m_sameApplyQuest.size())
        {
            return 0x7fffffff;
        }
        return m_sameApplyQuest.at(m_applyCount);
    }
    if (dayIndex - 1 + questCount < 0)
    {
        return 0x7fffffff;
    }
    if (m_applyCount >= m_questProb[dayIndex - 1 + questCount].size())
    {
        return 0x7fffffff;
    }
    return m_questProb[dayIndex - 1 + questCount].at(m_applyCount);
}

// [b28d-solo-STQuestScript · P212] ORIG 先构造 tmp（-0x38）再 call computeDayIndex
// ⇒ 头部声明序 tmp 在前（4.1/4.4 全头部声明同为先声明更深）；ORIG 0x08aaf8b6。
bool TrainingQuestScript::getApplyLevel(std::multiset<int>* out)
{
    if (m_selectProb.m_count <= 0 || m_selectProb.m_pairs.empty())
    {
        return false;
    }
    std::vector<int> tmp;
    int day = computeDayIndex();
    for (int i = 0; i < m_selectProb.m_count; ++i)
    {
        int val = get_random_fake_value(i, day);
        int acc = 0;
        for (std::vector<std::pair<int, int> >::iterator it =
                 m_selectProb.m_pairs.begin();
             it != m_selectProb.m_pairs.end(); ++it)
        {
            acc += it->second;
            if (val < acc)
            {
                tmp.push_back(it->first);
                break;
            }
        }
    }
    for (int level = 1; level <= 0x46; ++level)
    {
        for (std::vector<int>::iterator it = tmp.begin(); it != tmp.end(); ++it)
        {
            if (*it == -100)
            {
                out[level - 1].insert(*it);
            }
            // ORIG else 段 = `*it != -100 && *it + level >= 1` 短路链（cmp $-100;je
            // 跳过 + jle 跳过 + mov$1/mov$0/test 物化，P214①）。
            else if (*it != -100 && *it + level >= 1)
            {
                out[level - 1].insert(*it);
            }
        }
    }
    return true;
}

// ===================== 脚本导入 =====================

// [b28d-solo-STQuestScript · P303] ORIG 追加分隔符段 = __builtin_strcat 内建展开
// （movl $2,0x8; movl $str,0x4; movl $buf,(%esp); call strlen; add $buf,%eax;
//  mov %eax,(%esp); mov (%esp),%eax; mov %eax,(%esp); call memcpy，ORIG 0x8a6166a 族）
bool initQuestScript(const char* dir, const char* list)
{
    strncpy(g_questScriptBaseDirectory, dir, 0x100);
    // ORIG 0x8a6166a..0x8a616c1 族形：内联补齐（strlen×3 + 追加 "\\"），无空串
    // 防御（ORIG len==0 时读 buf[-1] 为原始 UB 形态，P140 忠实保留）；P49 禁
    // 本 TU 外 helper——原 AppendDirSeparator 属发明符号（ORIG nm 无）。
    // P303：追加段必须 __builtin_strcat 形（手写 memcpy(dst+strlen,..) 恒 DIFF）。
    if (g_questScriptBaseDirectory[strlen(g_questScriptBaseDirectory) - 1] != '\\' && g_questScriptBaseDirectory[strlen(g_questScriptBaseDirectory) - 1] != '/')
    {
        __builtin_strcat(g_questScriptBaseDirectory, "\\");
    }
    // P304：权威型 &g_questScriptFileList 直压（ORIG movl $&g_..,(%esp)）。
    if (!loadRDARScriptListFile(&g_questScriptFileList,
                                g_questScriptBaseDirectory, list, 0))
    {
        return 0;
    }
    return 1;
}

// [b28d-solo-STQuestScript · §6.2 三件套] ORIG 0x08a61813（raw 3883 / norm 3836）
// ---------------------------------------------------------------------------
// [2026-09-10 SweepSTQuestScript-2 收口判词] 本函数三项修复（逐项实测）：
// ① [type]/[exception quest] 状态机 27+1 处块序翻转为
//    `if(!__inited){insert}else{dst;break}` —— 与 ORIG 汇编逐条同构：
//    `movzbl __inited;xor $1;test;je <dst>` → insert 块 → `jmp <下一 case>`
//    → dst 块（ORIG 0x8a632b6..0x8a63ea4 / 0x8a65563 段实证）。效果：
//    xor 61→89（ORIG 87）、jmp 283→311（ORIG 315）、raw 3841→3897。
//    旧判词「翻转后 fn_diff 变差（3635→3676）」系在未做 ②③ 的基线上测得；
//    三项联改后 difflib 对齐率 0.7255→0.7544（matched 2803→2932）⇒ 保留。
// ② ConvertHHMMSSStringToInt 6 处改直接传 entry 成员引用（去 h/m/sec
//    具名局部）：ORIG 0x8a6214d/15a/167/1e1/1ee/1fb 为
//    `lea -0x45c; add $0x18/19/1a/1b/1c/1d`，我方原多 6 组
//    `movb $0 + movb %al,entry` ⇒ 修后 raw 3897→3885。
// ③ entry.m_eventId 改经具名局部 eventId（ORIG -0x40 中间槽实证
//    0x8a61ee7/0x8a61efc：`mov %eax,-0x40; …; mov -0x40,%eax; mov %eax,-0x45c`）
//    ⇒ raw 3885→3887（+2 条与 ORIG 对齐）。
// 现状：raw 3883/3887（gap -4）、xor 87/89、jmp 315/311、
// p167 517/517 CLEAR、四数 458/917/0/12 不回退。
// 残余 gap -4 = 4.1/4.4 块布局差（非语义缺口，逐点核对）：
//   · ORIG 序言 +0x84..+0xbf 的 20 组 `nop;jmp loop_head` continue 岛，
//     20 个前驱全部实证在案（0x8a61a5a jne / 0x8a61e16 je / 0x8a623cd…/
//     0x8a6572c je；**旧判词「17 组无前驱=死岛」系误判，已更正**）；
//     我方 4.4 把 71 处 continue 汇入 1 个中继（jmp loop_head）并把 14 处
//     break 生成 `jmp 收尾;nop` 岛链 ⇒ 岛归属/落点不可控（P223/N82 族）。
//   · 13 处 `if(!ok) break;` 我方 `xor;test;jne <break 岛>` + fall-through
//     到 continue；ORIG `xor;test;je <continue 岛>` + `jmp 收尾` —— 两侧
//     极性/落点互补、语义逐点等价（抽查 .o 岛链 60b3..60c5 与
//     ORIG 0x8a64e37 等 7 点）。
//   · 槽漂移（line -0x320↔-0x308 族）与 setne/lea 级分散形态差 ⇒ N82/N126。
// 以下为 2026-09-09 历史判词（gap 42 /「翻转变差」结论已被本次推翻）：
// vs 我方（raw 3841 / norm 3811，gap 25）；p167 调用点差集 517/517 CLEAR
// （唯一差异为 C1↔C2 / D1↔D2 镜像对，豁免）。残余 byte 层 N 封顶：
// ① P223 实测复验（BigQuestScript 2026-09-09）：ORIG +0x86..+0xbf 为 19 组
//    `nop;jmp +0xc0` 中继岛（仅 +0x86/+0x8c/+0x92 有 jne 前驱 +0x247/+0xbba/
//    +0xcf1，其余 16 组无前驱）；我方 4.4 只发 15 组且全部落在函数尾
//    （+0x3f6a 附近 8 连 + 7 处分散）⇒ 岛数/落点不可控，19 条封顶。
// ② [type]/[exception quest] 守卫块序镜像（P272/P194）：ORIG 为
//    `if(!__inited){insert}else{dst;break}`（movzbl;xor $1;test;je dst，
//    insert 后 jmp 下一 case），我方为 `if(__inited){dst;break}insert`
//    （movzbl;test;je insert，insert fall-through）。27+1 处全量翻转实测：
//    ORIG-only 指令 181→36、xor 61→88（ORIG 87）、jmp 283→311（ORIG 315）
//    ⇒ 补回 54 条真实指令，但净 gap +25→-31、fn_diff 不等行 3635→3676
//    （本区固有 mov+22/jne+11/movb+8 冗余未消）⇒ 保持现形，判镜像块序差。
// ③ 其余 6 条为 je/add/lea/sete 级分散形态差（我方 3 处 setcc 分支形、
//    我方多 mov/movb/movzbl 与 ORIG 多 je/add/lea 互为抵消），非语义缺口。
// [b28d-solo-STQuestScript · P292/P315 档①] int→bool（与头同步）：
// ORIG 调用点 0x08353e51 `call; xor $0x1,%eax; test %al,%al; je`（无 setne）。
// 函数体 return 0/1 均为常量 ⇒ bool 化不引入 setne，定义侧形态不变；
// mangled _Z17importQuestScriptP11QuestScriptPKc 不变（返回类型不入 mangled）。
// [b28d-solo-STQuestScript 2026-09-09 复核] 定义侧同快照 A/B（bool vs int）
// 四数逐项相同 460/906/0/23 ⇒ 返回类型改动零形态影响；fresh 读数
// 458/917/0/12（missing 362，全 TU）。gap 42（raw 3883/3841）主体 = ORIG
// 序言 +0x32..+0x71 共 20 组 `nop;jmp 0x8a618d3` 中继岛（仅 +0x86/+0x8c/
// +0x92 三组有 jne 前驱，余 17 组无前驱 = 死岛）vs 我方 1 组（+0x21cb
// jmp +0x21cd nop）⇒ 4.4 jump-thread 合并；死岛无法用合法 C++ 生成
// （P337 volatile 守卫每处固有 +4 条 > 收益，净负）⇒ P223 负例封顶；
// 余 4 条为槽漂移（非恒定：line -0x320↔-0x308、qd0 -0x3d8↔-0x3e0）
// 与 [type] 块序 ⇒ N82/N126 封顶。p167 517/517 CLEAR（差集全 C1/C2·D1/D2
// 同址镜像，P93 豁免），miss=0（无 ORIG-only 游戏域 callee）。
bool importQuestScript(QuestScript* script, const char* path)
{
    // ORIG importQuestScript 的函数局部辅助类 QuestData（报告
    // importQuestScript_QuestScript___char_const____QuestData/：
    //   C1 0x08a616fe / D1 0x08a617a6）。重建版按报告布局语义落地并
    // force 实例化以发射 _ZZ17importQuestScript...QuestData C1/C2/D1/D2 符号。
    // 布局/初值顺序依 ORIG C1 0x08a616fe：string+两容器自动构造后，
    // m_int14=0 → m_int10=0 → 5 字节清 0 → m_byte2c=0。
    struct QuestData
    {
        std::string m_str0;                     // +0x00
        std::vector<int> m_vec4;                // +0x04
        int m_int10;                            // +0x10
        int m_int14;                            // +0x14
        char m_byte18;                          // +0x18
        char m_byte19;                          // +0x19
        char m_byte1a;                          // +0x1a
        char m_byte1b;                          // +0x1b
        char m_byte1c;                          // +0x1c
        std::vector<MonsterKillItem> m_vec20;   // +0x20
        char m_byte2c;                          // +0x2c

        QuestData()
        {
            m_int14 = 0;   // ORIG 先清 +0x14
            m_int10 = 0;   // 再清 +0x10
            m_byte18 = 0;
            m_byte19 = 0;
            m_byte1a = 0;
            m_byte1b = 0;
            m_byte1c = 0;
            m_byte2c = 0;
        }

        ~QuestData()
        {
        }
    };
    QuestData qd0;   // ORIG -0x3d8（light 系临时数据）
    QuestData qd1;   // ORIG -0x408（first 系临时数据）

    // ---- ORIG T 0x08a61813（3883 条）逐指令转写：QuestScript 键值解析器。
    // 主循环 ScanType 读键；[light/first] 系数据先暂存 QuestData，文件读完
    // 收尾时（IsLightServer）合并；[type]/[exception quest] 用 SpinLock+flag
    // 双检锁保护的静态 map 惰性填充（ORIG BSS 0x95004d8/0x95004f0/0x95004f4
    // 与 0x95004f8/0x9500510/0x9500514）。 ----
    //
    // 【槽布局判词（BigQuest 2026-09-09 实测，勿随手删）】
    // ORIG 槽锚：line=-0x320 sub=-0x324 value=-0x328 ok=-0x329 schedule五串
    // -0x330..-0x344 qd0=-0x3d8 qd1=-0x408 entry=-0x45c；frame 0x46c 两侧一致。
    // 我方现：line=-0x308（浅 24B）、qd0=-0x3e0/qd1=-0x410（深 8B）。
    // 负形实验：在 qd 前/qd 后插无引用 `char pad[24]` 均可令 line 命中 -0x320
    // （~260 条 lea/mov -0x320 行全对平），但同一实验把 qd/entry/收尾 temps
    // 整体再压深 28B（qd0→-0x3f8），净收益 -80 行，**故未采用**。⇒ GCC 4.4.6
    // 对本帧的槽分配与「源声明位置」无关（探针放 qd 前/后结果逐位相同），
    // 与 ORIG 的 24B 缺口在 line 之下的隐式临时链（pair 链 20B×27 +
    // value-pass 串槽 4B×5 + title7 remove temps 2×16B 的整体错位），
    // 属帧分配律问题（FrameDeltaProbe 在测），不是本函数指令语义缺口：
    // 分段指令数已齐平（type 216/215、schedule 358/348、exception 145/136），
    // AE 残差 51/62 块为纯槽漂移（schedule 实测）、唯一“真差”是 reloc 显示。
    // ⇒ 本函数剩余残差按 **N82/N126 封账**（P332：Δ槽数非单调、4.1/4.4 首槽
    // 常数差 -0x4/-0xc 不可修）；状态机形已入库 **P333**（含指纹与 p167 漏检因）。
    if (!loadRDARScriptFile(g_questScriptBaseDirectory, path))
    {
        return 0;
    }
    script->clear();

    std::string line;      // ORIG -0x320
    std::string sub;       // ORIG -0x324
    int value = 0;         // ORIG -0x328
    bool ok = false;       // ORIG -0x329

    for (;;)
    {
        if (!ScanType(line, true))
        {
            break;         // 文件读完 → 收尾（ORIG 0x8a6576a）
        }
        if (line == "[name]")
        {
            // ORIG：m_questIdx(+0x14)!=0 时走 ScanJobStr（职业串表），否则 ScanStr
            if (script->m_questIdx != 0)
            {
                ScanJobStr(&script->m_name, script->m_jobNames);
            }
            else
            {
                ScanStr(&script->m_name);
            }
            continue;
        }
        if (line == "[depend message]")
        {
            if (script->m_questIdx != 0)
            {
                ScanJobStr(&script->m_requestDefault, script->m_jobRequests);
            }
            else
            {
                ScanStr(&script->m_requestDefault);
            }
            continue;
        }
        if (line == "[light condition message]")
        {
            ScanStr(&qd0.m_str0);
            qd0.m_byte19 = 1;
            continue;
        }
        if (line == "[first condition message]")
        {
            ScanStr(&qd1.m_str0);
            qd1.m_byte19 = 1;
            continue;
        }
        if (line == "[condition message]")
        {
            if (script->m_questIdx != 0)
            {
                ScanJobStr(&script->m_conditionDefault, script->m_jobConditions);
            }
            else
            {
                ScanStr(&script->m_conditionDefault);
            }
            // ORIG 8a61a4e：title 任务（questType==7）时 m_name 取条件串，
            // **只调 std::remove 不调 erase**（ORIG 段 2×begin/2×end、零 erase
            // 调用 ⇒ 原码丢弃 remove 返回值，P171 照抄），随后 resize 0x1d。
            if (script->m_questType == 7)
            {
                script->m_name = script->m_conditionDefault;
                std::remove(script->m_name.begin(), script->m_name.end(),
                            '\'');
                std::remove(script->m_name.begin(), script->m_name.end(),
                            '`');
                script->m_name.resize(0x1d);
            }
            continue;
        }
        if (line == "[solve message]")
        {
            if (script->m_questIdx != 0)
            {
                ScanJobStr(&script->m_solveDefault, script->m_jobSolves);
            }
            else
            {
                ScanStr(&script->m_solveDefault);
            }
            continue;
        }
        if (line == "[grade]")
        {
            ScanStr(&sub);
            // p167'（本泳道修正）：ORIG 字面量在左的 5 处是 training/special/
            // urgent/village attack revenge/schedule（内层）——调 operator==
            // (const char*, string)（ORIG 08237ae0）；normaly repeat/common
            // unique/special daily 在 ORIG 均为 sub 在左（080809a1）。
            if (sub == "[normaly repeat]")
            {
                script->m_questType = 4;
            }
            else if (sub == "[common unique]")
            {
                script->m_questType = 5;
            }
            else if (sub == "[epic]")
            {
                script->m_questType = 0;
            }
            else if (sub == "[achievement]")
            {
                script->m_questType = 2;
            }
            else if ("[training]" == sub)
            {
                script->m_questType = 1;
            }
            else if ("[special]" == sub)
            {
                script->m_questType = 6;
            }
            else if ("[urgent]" == sub)
            {
                script->m_questType = 8;
            }
            else if (sub == "[daily]")
            {
                script->m_questType = 3;
                script->m_fieldD8 = 0;
            }
            else if (sub == "[special daily]")
            {
                script->m_questType = 3;
                ScanStr(&sub);
                if ("[village attack revenge]" == sub)
                {
                    script->m_fieldD8 = 1;
                }
                else if ("[schedule]" == sub)
                {
                    script->m_fieldD8 = 2;
                }
                else
                {
                    script->m_fieldD8 = 0;
                }
            }
            else if (sub == "[title]")
            {
                script->m_questType = 7;
            }
            continue;
        }
        if (line == "[schedule]")
        {
            // ORIG 0x8a61e4c..0x8a6238d：循环装配 _DailySchedule（0x24）。
            // 首字段 ScanInt 失败=哨兵（schedule 数据读完→继续主循环）；
            // 其余解析失败→return 0。m_type(+0x04)=1、m_idx(+0x08)=
            // FindIndexByFullScan(path) 在循环外固定。
            // ORIG 0x8a61e4c..0x8a61e8d：5 个时间串在循环**外**构造一次
            // （-0x330..-0x340），循环内复用（ScanStr 覆盖）⇒ 声明顺序在
            // idx/entry 之前。
            std::string strStart;      // ORIG -0x330
            std::string strEnd;        // ORIG -0x334
            std::string strWeekday;    // ORIG -0x338
            std::string strStartTime;  // ORIG -0x33c
            std::string strEndTime;    // ORIG -0x340
            int idx = reinterpret_cast<STScriptFileList*>(
                          &g_questScriptFileList)->FindIndexByFullScan(path);
            _DailySchedule entry;
            memset(&entry, 0, sizeof(entry));
            entry.m_type = 1;
            entry.m_idx = idx;
            for (;;)
            {
                int eventId = ScanInt(&ok);          // ORIG -0x40（具名局部）
                if (!ok)
                {
                    break;                           // 哨兵 → continue 主循环
                }
                entry.m_eventId = eventId;           // +0x00
                if (!ScanStr(&strStart))
                {
                    break;                          // ORIG 首个时间串失败=本段结束，回主循环
                }
                if (!ScanStr(&strEnd))
                {
                    return 0;
                }
                if (!ScanStr(&strWeekday))
                {
                    return 0;
                }
                if (!ScanStr(&strStartTime))
                {
                    return 0;
                }
                if (!ScanStr(&strEndTime))
                {
                    return 0;
                }
                // p167：ORIG 8a61fc1/8a6203b 只为按值形参构造**一个**临时
                // （C1ERKSs + D1Ev 各一次）⇒ 源码直接传原变量，勿再套具名 tmp
                // （多一次拷贝+析构，EXTRA SsC1ERKSs/SsD1Ev 来源）。
                {
                    if (!ConvertStringToTime(strStart, *(long*)&entry.m_startSec))
                    {
                        return 0;                    // +0x0c
                    }
                }
                {
                    if (!ConvertStringToTime(strEnd, *(long*)&entry.m_endSec))
                    {
                        return 0;                    // +0x10
                    }
                }
                {
                    unsigned int wday = 0;
                    if (!ConvertWeekdayStringToInt(strWeekday, wday))
                    {
                        return 0;
                    }
                    entry.m_wday = static_cast<int>(wday);   // +0x14
                }
                {
                    if (!ConvertHHMMSSStringToInt(strStartTime,
                                                  entry.m_startHour,
                                                  entry.m_startMin,
                                                  entry.m_startSec2))
                    {
                        return 0;
                    }
                }
                {
                    if (!ConvertHHMMSSStringToInt(strEndTime,
                                                  entry.m_endHour,
                                                  entry.m_endMin,
                                                  entry.m_endSec2))
                    {
                        return 0;
                    }
                }
                script->m_dailySchedule.push_back(entry);
            }
            continue;
        }
        if (line == "[job change quest]")
        {
            script->m_jobChangeQuest = ScanInt((bool*)0);
            // ORIG 8a623c1：jobChangeQuest==1 且 growType(+0xf0)!=-1 为非法组合
            if (script->m_jobChangeQuest == 1 && script->m_fieldF0 != -1)
            {
                printf("questScript->classChangeQuestType_(%d) > 0  and "
                       "questScript->growType_(%d) != -1",
                       script->m_jobChangeQuest, script->m_fieldF0);
                return 0;
            }
            continue;
        }
        if (line == "[npc index]")
        {
            script->m_npcIdx = ScanInt((bool*)0);
            continue;
        }
        if (line == "[complete npc index]")
        {
            script->m_fieldE8 = ScanInt((bool*)0);
            continue;
        }
        if (line == "[job]")
        {
            ScanStr(&sub);
            script->m_fieldEC = getCharacterJob(sub.c_str());
            continue;
        }
        if (line == "[grow type]")
        {
            script->m_fieldF0 = ScanInt((bool*)0);
            if (script->m_jobChangeQuest == 1 && script->m_fieldF0 != -1)
            {
                printf("questScript->classChangeQuestType_(%d) > 0  and "
                       "questScript->growType_(%d) != -1",
                       script->m_jobChangeQuest, script->m_fieldF0);
                return 0;
            }
            continue;
        }
        if (line == "[awakening type]")
        {
            script->m_fieldE0 = ScanInt((bool*)0);
            continue;
        }
        if (line == "[target character]")
        {
            // ORIG 0x8a6259e..0x8a62689：循环 职业名+两 int，ScanStr 失败=读完
            for (;;)
            {
                if (!ScanStr(&sub))
                {
                    break;                           // 读完 → continue 主循环
                }
                if (!checkValiedJobName(sub.c_str()))
                {
                    return 0;
                }
                int job = getCharacterJob(sub.c_str());
                int v1 = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                int v2 = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                stQuestTargetCharacter_t t(
                    static_cast<ENUM_CHARACTERJOB>(job), v1, v2);
                // p167：ORIG 8a62684 调 push_backEOS0_（右值重载）。
                script->m_targetCharacters.push_back(
                    static_cast<stQuestTargetCharacter_t&&>(t));
            }
            continue;
        }
        if (line == "[reward one more check]")
        {
            ScanStr(&script->m_str2e8);
            continue;
        }
        if (line == "[quest window extension]")
        {
            script->m_2ec = ScanInt((bool*)0) > 0;   // ORIG setg 直写（勿加三目）
            continue;
        }
        if (line == "[light level]")
        {
            qd0.m_int10 = ScanInt((bool*)0);         // +0x10
            qd0.m_int14 = ScanInt((bool*)0);         // +0x14
            qd0.m_byte1b = 1;
            continue;
        }
        if (line == "[level]")
        {
            script->m_levelMin[0] = ScanInt((bool*)0);
            script->m_levelMin[1] = ScanInt((bool*)0);
            continue;
        }
        if (line == "[ignore quest level 4 exp]")
        {
            script->m_field108 = (ScanInt((bool*)0) != 0) ? 1 : 0;
            continue;
        }
        if (line == "[pre required quest]")
        {
            // ORIG 0x8a627e2..0x8a62888：临时 vector<int>，ScanInt 失败=读完；
            // 非空才整体入 m_intDataList
            std::vector<int> tmp;
            for (;;)
            {
                value = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                tmp.push_back(value);
            }
            if (!tmp.empty())
            {
                script->m_intDataList.push_back(tmp);
            }
            continue;
        }
        if (line == "[collision quest]")
        {
            script->m_antiQuestList.clear();
            for (;;)
            {
                value = ScanInt(&ok);
                if (!ok)
                {
                    break;                           // 读完 → continue 主循环
                }
                script->m_antiQuestList.push_back(value);
            }
            continue;
        }
        if (line == "[PVP RANK]")
        {
            script->m_questCountLimit = ScanInt((bool*)0);
            continue;
        }
        if (line == "[creature kind]")
        {
            script->m_field128 = ScanInt((bool*)0);
            continue;
        }
        if (line == "[creature level]")
        {
            script->m_field12c = ScanInt((bool*)0);
            continue;
        }
        if (line == "[effect after clear quest]")
        {
            script->m_field130 = (ScanInt((bool*)0) != 0) ? 1 : 0;
            continue;
        }
        if (line == "[delete npc index]")
        {
            script->m_field134 = ScanInt((bool*)0);
            continue;
        }
        if (line == "[clear reward item]")
        {
            // ORIG 0x8a62a23..0x8a62ade：6×ScanInt 一条 DungeonClearItem
            script->m_clearItems.clear();
            for (;;)
            {
                DungeonClearItem item;
                item.m_itemIdx = ScanInt(&ok);
                if (!ok)
                {
                    break;                           // 读完 → continue 主循环
                }
                item.m_dungeonIdx = ScanInt((bool*)0);
                item.m_rewardItem = ScanInt((bool*)0);
                item.m_count = ScanInt((bool*)0);
                item.m_percent = ScanInt((bool*)0);
                item.m_field14 = ScanInt((bool*)0);
                script->m_clearItems.push_back(item);
            }
            continue;
        }
        if (line == "[light monster reward item]" || line == "[light enemy reward item]")
        {
            // ORIG 0x8a62b2b..0x8a62c32：先读 m_killCount(+0x4)，enemy 系再读
            // m_enemyType(+0x0)，随后 6 值；首值失败=读完（置 m_byte2c 供收尾合并）
            qd0.m_vec20.clear();
            for (;;)
            {
                MonsterKillItem item;
                item.m_killCount = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                if (line == "[light enemy reward item]")
                {
                    item.m_enemyType = ScanInt((bool*)0);
                }
                item.m_mapIdx = ScanInt((bool*)0);
                item.m_dungeonIdx = ScanInt((bool*)0);
                item.m_field10 = ScanInt((bool*)0);
                item.m_field14 = ScanInt((bool*)0);
                item.m_field18 = ScanInt((bool*)0);
                item.m_field1c = ScanInt((bool*)0);
                qd0.m_vec20.push_back(item);
            }
            qd0.m_byte2c = 1;
            continue;
        }
        if (line == "[monster reward item]" || line == "[enemy reward item]")
        {
            script->m_killItems.clear();
            for (;;)
            {
                MonsterKillItem item;
                item.m_killCount = ScanInt(&ok);
                if (!ok)
                {
                    break;                           // 读完 → continue 主循环
                }
                if (line == "[enemy reward item]")
                {
                    item.m_enemyType = ScanInt((bool*)0);
                }
                item.m_mapIdx = ScanInt((bool*)0);
                item.m_dungeonIdx = ScanInt((bool*)0);
                item.m_field10 = ScanInt((bool*)0);
                item.m_field14 = ScanInt((bool*)0);
                item.m_field18 = ScanInt((bool*)0);
                item.m_field1c = ScanInt((bool*)0);
                script->m_killItems.push_back(item);
            }
            continue;
        }
        if (line == "[appear map]")
        {
            script->m_appearMap.m_mapA = ScanInt((bool*)0);
            script->m_appearMap.m_mapB = ScanInt((bool*)0);
            script->m_appearMap.m_appearMap = ScanInt((bool*)0);
            script->m_appearMap.m_percent = ScanInt((bool*)0);
            continue;
        }
        if (line == "[depend give item]")
        {
            script->m_rewardSelectList.clear();
            for (;;)
            {
                int a = ScanInt(&ok);
                if (!ok)
                {
                    break;                           // 读完 → continue 主循环
                }
                int b = ScanInt((bool*)0);
                // p167：ORIG 8a62e77 调 pairC1IRiiEEOT_OT0_（右值实参）+
                // 8a62e92 push_backEOS1_（右值重载）。
                // c++filt 实测 `C1IRiiEEOT_OT0_` = pair<int&,int>(int&, int&&)
                // ⇒ 第一实参左值、第二实参右值。
                std::pair<int, int> pair(a, static_cast<int&&>(b));
                script->m_rewardSelectList.push_back(
                    static_cast<std::pair<int, int>&&>(pair));
            }
            continue;
        }
        if (line == "[limit dungeon index]")
        {
            for (;;)
            {
                value = ScanInt(&ok);
                if (!ok)
                {
                    break;                           // 读完 → continue 主循环
                }
                script->m_field1c8.push_back(value);
            }
            continue;
        }
        if (line == "[type]")
        {
            // ORIG 0x8a62f16..0x8a63ea4：静态 map<const char*,int,less_TCHAR>
            // （BSS 0x95004d8）+ SpinLock（0x95004f0）+ 惰性填充标志（0x95004f4）。
            // 首次进入填表后重查；未知 type → return 0；m_triggerType = 值-0x32c。
            if (!ScanStr(&sub))
            {
                return 0;
            }
            static std::map<const char*, int, less_TCHAR> __lookup;
            static SpinLock __init_lock;
            static bool __inited = false;
            // ORIG 0x8a62f35..0x8a63ea4 状态机（BigQuest 破译）：未初始化时
            // v=0 哨兵进 switch → **链式一次建全表**（每 case：if(__inited)
            // {set;break;} else insert(本key) 后 **fallthrough 到下一 case**），
            // 末 case insert 后落到 default：若仍 !__inited ⇒ **return 0**
            // （+0x2655→+0x2660：原码首次建表把本条目判失败，忠实保留）；
            // epilog +0x266a：!__inited ⇒ leave+置位+goto 重查；inited ⇒ continue。
        retry_type_lookup:;
            int v;
            if (__inited)
            {
                std::map<const char*, int, less_TCHAR>::const_iterator it =
                    __lookup.find(sub.c_str());
                v = (it != __lookup.end()) ? it->second : -1;
            }
            else
            {
                __init_lock.enter();
                if (__inited)
                {
                    __init_lock.leave();
                    goto retry_type_lookup;
                }
                v = 0;
            }
            switch (v)
            {
            case 0:
            case 0x32c:
                if (!__inited)
                {
                    __lookup.insert(std::pair<const char* const, int>(
                        "[seeking]", 0x32c));
                }
                else
                {
                    script->m_triggerType = 0x0;
                    break;
                }
            case 0x32f:
                if (!__inited)
                {
                    __lookup.insert(std::pair<const char* const, int>(
                        "[condition under clear]", 0x32f));
                }
                else
                {
                    script->m_triggerType = 0x1;
                    break;
                }
            case 0x332:
                if (!__inited)
                {
                    __lookup.insert(std::pair<const char* const, int>(
                        "[hunt monster]", 0x332));
                }
                else
                {
                    script->m_triggerType = 0x2;
                    break;
                }
            case 0x335:
                if (!__inited)
                {
                    __lookup.insert(std::pair<const char* const, int>(
                        "[clear map]", 0x335));
                }
                else
                {
                    script->m_triggerType = 0x3;
                    break;
                }
            case 0x338:
                if (!__inited)
                {
                    __lookup.insert(std::pair<const char* const, int>(
                        "[meet npc]", 0x338));
                }
                else
                {
                    script->m_triggerType = 0x4;
                    break;
                }
            case 0x33b:
                if (!__inited)
                {
                    __lookup.insert(std::pair<const char* const, int>(
                        "[pvp rank]", 0x33b));
                }
                else
                {
                    script->m_triggerType = 0x5;
                    break;
                }
            case 0x33e:
                if (!__inited)
                {
                    __lookup.insert(std::pair<const char* const, int>(
                        "[hunt enemy]", 0x33e));
                }
                else
                {
                    script->m_triggerType = 0x6;
                    break;
                }
            case 0x342:
                if (!__inited)
                {
                    __lookup.insert(std::pair<const char* const, int>(
                        "[powerwar win]", 0x342));
                }
                else
                {
                    script->m_triggerType = 0x7;
                    break;
                }
            case 0x345:
                if (!__inited)
                {
                    __lookup.insert(std::pair<const char* const, int>(
                        "[powerwar point]", 0x345));
                }
                else
                {
                    script->m_triggerType = 0x8;
                    break;
                }
            case 0x34a:
                if (!__inited)
                {
                    __lookup.insert(std::pair<const char* const, int>(
                        "[mobile]", 0x34a));
                }
                else
                {
                    script->m_triggerType = 0x9;
                    break;
                }
            case 0x34f:
                if (!__inited)
                {
                    __lookup.insert(std::pair<const char* const, int>(
                        "[use item]", 0x34f));
                }
                else
                {
                    script->m_triggerType = 0xa;
                    break;
                }
            case 0x352:
                if (!__inited)
                {
                    __lookup.insert(std::pair<const char* const, int>(
                        "[get score]", 0x352));
                }
                else
                {
                    script->m_triggerType = 0xb;
                    break;
                }
            case 0x355:
                if (!__inited)
                {
                    __lookup.insert(std::pair<const char* const, int>(
                        "[clear quest]", 0x355));
                }
                else
                {
                    script->m_triggerType = 0xc;
                    break;
                }
            case 0x358:
                if (!__inited)
                {
                    __lookup.insert(std::pair<const char* const, int>(
                        "[check time]", 0x358));
                }
                else
                {
                    script->m_triggerType = 0xd;
                    break;
                }
            case 0x35b:
                if (!__inited)
                {
                    __lookup.insert(std::pair<const char* const, int>(
                        "[check life]", 0x35b));
                }
                else
                {
                    script->m_triggerType = 0xe;
                    break;
                }
            case 0x35e:
                if (!__inited)
                {
                    __lookup.insert(std::pair<const char* const, int>(
                        "[amplify item]", 0x35e));
                }
                else
                {
                    script->m_triggerType = 0xf;
                    break;
                }
            case 0x361:
                if (!__inited)
                {
                    __lookup.insert(std::pair<const char* const, int>(
                        "[disjoint item]", 0x361));
                }
                else
                {
                    script->m_triggerType = 0x10;
                    break;
                }
            case 0x364:
                if (!__inited)
                {
                    __lookup.insert(std::pair<const char* const, int>(
                        "[equipped item]", 0x364));
                }
                else
                {
                    script->m_triggerType = 0x11;
                    break;
                }
            case 0x367:
                if (!__inited)
                {
                    __lookup.insert(std::pair<const char* const, int>(
                        "[get item]", 0x367));
                }
                else
                {
                    script->m_triggerType = 0x12;
                    break;
                }
            case 0x36a:
                if (!__inited)
                {
                    __lookup.insert(std::pair<const char* const, int>(
                        "[custom quest]", 0x36a));
                }
                else
                {
                    script->m_triggerType = 0x13;
                    break;
                }
            case 0x36f:
                if (!__inited)
                {
                    __lookup.insert(std::pair<const char* const, int>(
                        "[send chatting]", 0x36f));
                }
                else
                {
                    script->m_triggerType = 0x14;
                    break;
                }
            case 0x372:
                if (!__inited)
                {
                    __lookup.insert(std::pair<const char* const, int>(
                        "[turn gold card]", 0x372));
                }
                else
                {
                    script->m_triggerType = 0x15;
                    break;
                }
            case 0x375:
                if (!__inited)
                {
                    __lookup.insert(std::pair<const char* const, int>(
                        "[use fortune coin]", 0x375));
                }
                else
                {
                    script->m_triggerType = 0x16;
                    break;
                }
            case 0x378:
                if (!__inited)
                {
                    __lookup.insert(std::pair<const char* const, int>(
                        "[meet secret npc]", 0x378));
                }
                else
                {
                    script->m_triggerType = 0x17;
                    break;
                }
            case 0x37d:
                if (!__inited)
                {
                    __lookup.insert(std::pair<const char* const, int>(
                        "[ui click]", 0x37d));
                }
                else
                {
                    script->m_triggerType = 0x18;
                    break;
                }
            case 0x382:
                if (!__inited)
                {
                    __lookup.insert(std::pair<const char* const, int>(
                        "[seek n meet npc]", 0x382));
                }
                else
                {
                    script->m_triggerType = 0x19;
                    break;
                }
            case 0x387:
                if (!__inited)
                {
                    __lookup.insert(std::pair<const char* const, int>(
                        "[assault count]", 0x387));
                }
                else
                {
                    script->m_triggerType = 0x1a;
                    break;
                }
            default:
                if (!__inited)
                {
                    return 0;                        // ORIG +0x2660
                }
                break;
            }
            if (!__inited)
            {
                __init_lock.leave();
                __inited = true;
                goto retry_type_lookup;              // ORIG +0x2691 重查
            }
            continue;
        }
        if (line == "[dungeon info]")
        {
            // ORIG 0x8a63ec3..0x8a63f18：两 int 一条 AdditionalDungeonInfo
            for (;;)
            {
                int a = ScanInt(&ok);
                if (!ok)
                {
                    break;                           // 读完 → continue 主循环
                }
                int b = ScanInt((bool*)0);
                AdditionalDungeonInfo info;
                info.m_field0 = a;
                info.m_field4 = b;
                script->m_field16c.push_back(info);
            }
            continue;
        }
        if (line == "[sub type]")
        {
            script->m_triggerSubType = ScanInt((bool*)0);
            continue;
        }
        if (line == "[light int data]")
        {
            qd0.m_vec4.clear();
            for (;;)
            {
                value = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                qd0.m_vec4.push_back(value);
            }
            qd0.m_byte1a = 1;
            continue;
        }
        if (line == "[first int data]")
        {
            qd1.m_vec4.clear();
            for (;;)
            {
                value = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                qd1.m_vec4.push_back(value);
            }
            qd1.m_byte1a = 1;
            continue;
        }
        if (line == "[int data]")
        {
            script->m_triggerData.clear();
            for (;;)
            {
                value = ScanInt(&ok);
                if (!ok)
                {
                    break;                           // 读完 → continue 主循环
                }
                script->m_triggerData.push_back(value);
            }
            continue;
        }
        if (line == "[string data]")
        {
            script->m_field18c.clear();
            std::string s;
            for (;;)
            {
                if (!ScanStr(&s))
                {
                    break;                           // 读完 → continue 主循环
                }
                script->m_field18c.push_back(s);
            }
            continue;
        }
        if (line == "[send postal]")
        {
            script->m_postalReward.m_itemIdx = ScanInt((bool*)0);   // +0x2a0
            script->m_postalReward.m_addInfo = ScanInt((bool*)0);   // +0x2a4
            ScanStr(&script->m_postalReward.m_title);               // +0x298
            ScanStr(&script->m_postalReward.m_contents);            // +0x29c
            continue;
        }
        if (line == "[condition data]")
        {
            script->m_field198.clear();
            std::string s;
            for (;;)
            {
                int key = ScanInt(&ok);
                if (!ok)
                {
                    break;                           // 读完 → continue 主循环
                }
                ScanStr(&s);
                std::pair<std::string, std::string> p;
                p.first = s;
                script->m_field198[key] = p;
            }
            continue;
        }
        if (line == "[check count]")
        {
            script->m_field1b0.clear();
            for (;;)
            {
                value = ScanInt(&ok);
                if (!ok)
                {
                    break;                           // 读完 → continue 主循环
                }
                script->m_field1b0.push_back(value);
            }
            continue;
        }
        if (line == "[limit showing msg]")
        {
            script->m_field1c0 = ScanInt((bool*)0);
            continue;
        }
        if (line == "[reward type]")
        {
            ScanStr(&sub);
            if (sub == "[item]")
            {
                script->m_rewardType = 0;
            }
            else if (sub == "[grow type]")
            {
                script->m_rewardType = 1;
            }
            else if (sub == "[awakening type]")
            {
                script->m_rewardType = 2;
            }
            else if (sub == "[hell challenge]")
            {
                script->m_rewardType = 3;
            }
            else if (sub == "[creature evolution]")
            {
                script->m_rewardType = 0xa;
            }
            else if (sub == "[expert job]")
            {
                script->m_rewardType = 0x14;
            }
            else if (sub == "[hero]")
            {
                script->m_rewardType = 0x15;
            }
            else if (sub == "[slot expansion]")
            {
                script->m_rewardType = 0x16;
            }
            else if (sub == "[ridable]")
            {
                script->m_rewardType = 0x17;
            }
            else if (sub == "[title]")
            {
                script->m_rewardType = 0x18;
            }
            else if (sub == "[event creature evolution]")
            {
                script->m_rewardType = 0x19;
            }
            // 未知：不设置（ORIG 无 else）
            continue;
        }
        if (line == "[reward selection int data]")
        {
            script->m_selectReward.clear();
            if (script->m_rewardType == 0 || script->m_rewardType == 0x19)
            {
                if (!ScanRewardItems(script->m_selectReward,
                                     "[/reward selection int data]"))
                {
                    return 0;
                }
            }
            else
            {
                ScanType(line, true);                // ORIG 8a6467a：跳过
            }
            continue;
        }
        if (line == "[dialog]")
        {
            dungeonEventHanaseStruct ev;
            std::string s;
            ev.m_field0 = ScanInt((bool*)0);
            ev.m_field4 = ScanInt((bool*)0);
            ev.m_field8 = ScanInt((bool*)0);
            ev.m_fieldc = ScanInt((bool*)0);
            ev.m_field10 = (ScanInt((bool*)0) != 0) ? 1 : 0;
            ev.m_field14 = ScanInt((bool*)0);
            ev.m_field18 = ScanInt((bool*)0);
            if (script->m_questIdx != 0)
            {
                ScanJobStr(&ev.m_str20, ev.m_dialogs);
            }
            else
            {
                ScanStr(&ev.m_str20);
            }
            bool isType = false;
            ScanTypeOrStr(isType, s, 1);             // ORIG 返回值丢弃
            script->m_eventDialogs.push_back(ev);
            continue;
        }
        if (line == "[reward int data]")
        {
            script->m_titleReward.clear();
            // ORIG 8a6485a：rewardType<=0x19 且 (1<<rewardType)&0x3000001
            // （即 0/0x18/0x19）→ ScanRewardItems；否则仅读一 int + 跳行
            if (script->m_rewardType >= 0 && script->m_rewardType <= 0x19 &&
                ((1u << static_cast<unsigned int>(script->m_rewardType)) & 0x3000001u) != 0)
            {
                if (!ScanRewardItems(script->m_titleReward,
                                     "[/reward int data]"))
                {
                    return 0;
                }
            }
            else
            {
                script->m_field1e4 = ScanInt((bool*)0);
                ScanType(line, true);                // ORIG 8a64963：跳过
            }
            continue;
        }
        if (line == "[collision reward int data]")
        {
            // ORIG 0x8a6498b..0x8a64abe：值读至 -1 结束，其后 [and]/[or] 决定
            // 组合类型，最后是 reward data 列表
            script->m_field20c.clear();
            for (;;)
            {
                value = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                if (value == -1)
                {
                    break;
                }
                script->m_field20c.push_back(value);
            }
            if (!ScanStr(&sub))
            {
                return 0;
            }
            if (sub == "[and]")
            {
                script->m_field218 = 0;
            }
            else if (sub == "[or]")
            {
                script->m_field218 = 1;
            }
            else
            {
                return 0;
            }
            script->m_rewardData.clear();
            for (;;)
            {
                value = ScanInt(&ok);
                if (!ok)
                {
                    break;                           // 读完 → continue 主循环
                }
                script->m_rewardData.push_back(value);
            }
            continue;
        }
        if (line == "[relation quest]")
        {
            script->m_field228 = ScanInt((bool*)0);
            continue;
        }
        if (line == "[light exposed by npc]")
        {
            qd0.m_byte18 = (ScanInt(&ok) == 1) ? 1 : 0;
            if (!ok)
            {
                return 0;
            }
            qd0.m_byte1c = 1;
            continue;
        }
        if (line == "[first exposed by npc]")
        {
            qd1.m_byte18 = (ScanInt(&ok) == 1) ? 1 : 0;
            if (!ok)
            {
                return 0;
            }
            qd1.m_byte1c = 1;
            continue;
        }
        if (line == "[exposed by npc]")
        {
            script->m_field22d = (ScanInt(&ok) == 1) ? 1 : 0;
            if (!ok)
            {
                return 0;
            }
            continue;
        }
        if (line == "[event]")
        {
            script->m_field22e = (ScanInt(&ok) == 1) ? 1 : 0;
            if (!ok)
            {
                return 0;
            }
            continue;
        }
        if (line == "[auto display]")
        {
            // ORIG 8a64c64..0x8a64cb1：3 个 int 写入 QuestScript +0x230/+0x234/+0x238。
            // 这些字段由 QuestAutoDisplayData 具名建模，不能复用职业条件指针槽。
            for (int i = 0; i <= 2; ++i)
            {
                script->m_autoDisplay.m_values[i] = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
            }
            script->m_field23c = 1;
            continue;
        }
        if (line == "[substitutive names]")
        {
            // ORIG 0x8a64cde..0x8a64d71：ScanTypeOrStr；读到 '[/substitutive
            // names]' 结束，其余行入 m_fieldC8；解析失败打错误日志
            for (;;)
            {
                bool isType = false;
                if (!ScanTypeOrStr(isType, line, 1))
                {
                    OutputErrorMessage(path);
                    break;
                }
                if (isType)
                {
                    if (line != "[/substitutive names]")
                    {
                        OutputErrorMessage(path);
                    }
                    break;
                }
                script->m_fieldC8.push_back(line);
            }
            continue;
        }
        if (line == "[difficulty]")
        {
            ScanStr(&sub);
            script->m_field23d = sub.c_str()[0];
            if (islower(script->m_field23d))
            {
                script->m_field23d = static_cast<char>(toupper(script->m_field23d));
            }
            continue;
        }
        if (line == "[gold multiple]")
        {
            script->m_field240 = ScanInt(&ok);
            if (!ok)
            {
                break;                               // 文件结束 → 收尾
            }
            else
            {
                continue;
            }
        }
        if (line == "[mail time]")
        {
            script->m_mailQuestFlag = static_cast<char>(ScanInt(&ok));
            if (!ok)
            {
                break;                               // 文件结束 → 收尾
            }
            else
            {
                continue;
            }
        }
        if (line == "[mail title]")
        {
            ScanStr(&script->m_mailTitle);
            continue;
        }
        if (line == "[mail contents]")
        {
            ScanStr(&script->m_mailContents);
            continue;
        }
        if (line == "[mail enclose]")
        {
            script->m_mailItems.clear();
            for (;;)
            {
                value = ScanInt(&ok);
                if (!ok)
                {
                    break;                           // 读完 → continue 主循环
                }
                script->m_mailItems.push_back(value);
            }
            continue;
        }
        if (line == "[cant giveup]")
        {
            script->m_field270 = static_cast<char>(ScanInt(&ok));
            if (!ok)
            {
                break;                               // 文件结束 → 收尾
            }
            else
            {
                continue;
            }
        }
        if (line == "[searching information]")
        {
            script->m_field271 = static_cast<char>(ScanInt(&ok));
            if (!ok)
            {
                break;                               // 文件结束 → 收尾
            }
            else
            {
                continue;
            }
        }
        if (line == "[grow number]")
        {
            script->m_field274 = ScanInt(&ok);
            if (!ok)
            {
                break;                               // 文件结束 → 收尾
            }
            else
            {
                continue;
            }
        }
        if (line == "[belong to winning power]")
        {
            script->m_powerSide = static_cast<char>(ScanInt(&ok));
            if (!ok)
            {
                break;                               // 文件结束 → 收尾
            }
            else
            {
                continue;
            }
        }
        if (line == "[online]")
        {
            script->m_field27c = ScanInt(&ok);
            if (!ok)
            {
                break;                               // 文件结束 → 收尾
            }
            else
            {
                continue;
            }
        }
        if (line == "[job message]")
        {
            script->m_questIdx = ScanInt((bool*)0);
            continue;
        }
        if (line == "[quest point]")
        {
            value = ScanInt(&ok);
            if (!ok)
            {
                return 0;
            }
            script->m_field280 = static_cast<short>(value);
            continue;
        }
        if (line == "[quest piece]")
        {
            value = ScanInt(&ok);
            if (!ok)
            {
                return 0;
            }
            script->m_field282 = static_cast<short>(value);
            continue;
        }
        if (line == "[go guide]")
        {
            script->m_field284 = ScanInt((bool*)0);
            script->m_field288 = ScanInt((bool*)0);
            script->m_field28c = ScanInt((bool*)0);
            script->m_field290 = ScanInt((bool*)0);
            ScanStr(&script->m_str294);
            continue;
        }
        if (line == "[expertjob level]")
        {
            script->m_field2a8 = ScanInt((bool*)0);
            script->m_field2ac = ScanInt((bool*)0);
            continue;
        }
        if (line == "[change dungeon info message]")
        {
            script->m_2c8 = 1;
            ScanStr(&script->m_str2cc);
            continue;
        }
        if (line == "[exception quest]")
        {
            // ORIG 0x8a652d7..0x8a65651：静态 map（BSS 0x95004f8）+ SpinLock
            // （0x9500510）+ flag（0x9500514）；表仅 "[urgent]"→0x5c1。
            for (;;)
            {
                bool isType = false;
                // ORIG 8a652de：循环体内局部 std::string（-0x398，每轮 C1Ev/D1Ev
                // 各一次）+ 8a652ec 预清 bool（P198① 两句式）。
                std::string sub;
                bool ok = false;
                ok = ScanTypeOrStr(isType, sub, 1);
                if (!ok)
                {
                    return 0;
                }
                if (isType)
                {
                    // p167：ORIG 8a65349 调 operator==（相等→正常结束，否则 return 0）。
                    if (sub == "[/exception quest]")
                    {
                        break;                       // 正常结束 → continue 主循环
                    }
                    return 0;
                }
                // ORIG 0x8a65365..0x8a65613 状态机（BigQuest 破译，与 [type]
                // 同构）：未初始化 ⇒ v=0 哨兵进 case 0：if(__inited){push_back
                // (0)} else {insert("[urgent]",0x5c1)} 后 **fallthrough 到
                // default**；default(+0x3dc2)：!__inited ⇒ epilog
                // （leave+置位+goto 重查 +0x3b52），inited ⇒ **return 0**
                // （+0x3dcd：v=-1 未知即失败）。
                static std::map<const char*, int, less_TCHAR> __lookup;
                static SpinLock __init_lock;
                static bool __inited = false;
              retry_exception_lookup:;
                int v;
                if (__inited)
                {
                    std::map<const char*, int, less_TCHAR>::const_iterator it =
                        __lookup.find(sub.c_str());
                    v = (it != __lookup.end()) ? it->second : -1;
                }
                else
                {
                    __init_lock.enter();
                    if (__inited)
                    {
                        __init_lock.leave();
                        goto retry_exception_lookup;
                    }
                    v = 0;
                }
                switch (v)
                {
                case 0:
                case 0x5c1:
                    if (!__inited)
                    {
                        __lookup.insert(std::pair<const char* const, int>(
                            "[urgent]", 0x5c1));
                    }
                    else
                    {
                        script->m_exceptionList.push_back(
                            ENUM_QUEST_EXCEPTION_TYPE_0);
                        break;
                    }
                    /* fall through */
                default:
                    if (__inited)
                    {
                        return 0;                    // ORIG +0x3dcd
                    }
                    break;
                }
                if (!__inited)
                {
                    // ORIG +0x3dd9→+0x3dfa：leave + 置位 + **必须 goto 重查**
                    // （本条元素在填表轮后立即生效，不可漏）。
                    __init_lock.leave();
                    __inited = true;
                    goto retry_exception_lookup;
                }
            }
            continue;
        }
        if (line == "[hide npc on clear]")
        {
            value = ScanInt(&ok);
            // p167：ORIG 8a65694 调 push_backEOi（右值重载）。
            script->m_vec2f0.push_back(static_cast<int&&>(value));
            if (!ok)
            {
                return 0;
            }
            continue;
        }
        if (line == "[show npc on clear]")
        {
            value = ScanInt(&ok);
            script->m_vec2fc.push_back(static_cast<int&&>(value));
            if (!ok)
            {
                return 0;
            }
            continue;
        }
        if (line == "[show click on complete npc]")
        {
            if (!ScanInt(&value))
            {
                return 0;
            }
            script->m_308 = value != 0;               // ORIG setne 直写
            continue;
        }
        // 未知键：忽略（回循环读下一行）
    }

    // ---- 收尾（ORIG 0x8a6576a..0x8a65a18）----
    if (IsLightServer())
    {
        if (qd0.m_byte1c)                  // [light exposed by npc]
        {
            script->m_field22d = qd0.m_byte18;
        }
        if (qd0.m_byte1a)                  // [light int data]
        {
            script->m_triggerData.clear();
            script->m_triggerData.assign(qd0.m_vec4.begin(),
                                         qd0.m_vec4.end());
        }
        if (qd0.m_byte1b)                  // [light level]
        {
            script->m_levelMin[0] = qd0.m_int10;
            script->m_levelMin[1] = qd0.m_int14;
        }
        if (qd0.m_byte19)                  // [light condition message]
        {
            script->m_conditionDefault = qd0.m_str0;
        }
        if (qd0.m_byte2c)                  // [light monster reward item]
        {
            script->m_killItems.clear();
            script->m_killItems.assign(qd0.m_vec20.begin(),
                                       qd0.m_vec20.end());
        }
    }
    script->m_index = reinterpret_cast<STScriptFileList*>(
                          &g_questScriptFileList)->FindIndexByFullScan(path);
    if (script->m_fieldE8 == -1)
    {
        script->m_fieldE8 = script->m_npcIdx;
    }
    // ORIG：仅当已有目标且 job==0xb 且 growType==-1 时跳过默认目标，其余情况补入。
    if (script->m_targetCharacters.empty() ||
        script->m_fieldEC != 0xb || script->m_fieldF0 != -1)
    {
        stQuestTargetCharacter_t t(
            static_cast<ENUM_CHARACTERJOB>(script->m_fieldEC),
            script->m_fieldF0, script->m_fieldE0);
        if (std::find(script->m_targetCharacters.begin(),
                      script->m_targetCharacters.end(), t) ==
            script->m_targetCharacters.end())
        {
            script->m_targetCharacters.push_back(t);
        }
    }
    return 1;
}
// [b28d-solo-STQuestScript · P303] 同 initQuestScript：追加分隔符段 =
// __builtin_strcat 内建展开（ORIG 0x8aad9ec memcpy 段 8 条形）。
bool initTownScript(char* dir, char* list)
{
    strncpy(g_townScriptBaseDirectory, dir, 0x100);
    // ORIG 0x8aad9ec 族形：内联补齐（strlen×3 + 追加 "\\"），无空串防御
    // （ORIG len==0 时读 buf[-1] 为原始 UB 形态，P140 忠实保留）；P49 禁本 TU
    // 外 helper——原 AppendDirSeparator 属发明符号（ORIG nm 无）。
    // P303：追加段必须 __builtin_strcat 形。
    if (g_townScriptBaseDirectory[strlen(g_townScriptBaseDirectory) - 1] != '\\' && g_townScriptBaseDirectory[strlen(g_townScriptBaseDirectory) - 1] != '/')
    {
        __builtin_strcat(g_townScriptBaseDirectory, "\\");
    }
    // P304：权威型 &g_townScriptFileList 直压（ORIG movl $&g_..,(%esp)）。
    if (!loadRDARScriptListFile(&g_townScriptFileList,
                                g_townScriptBaseDirectory, list, 0))
    {
        return 0;
    }
    return 1;
}

// [b28d-solo-STQuestScript · §6.2 三件套] ORIG 0x08aadaa8（382 条）vs 我方 384
// （gap −2）；p167 调用点差集 57/57 CLEAR（0/0，含 P304 权威型 &g_townScriptFileList
// 直压、TownAreaScript 循环外构造 + 块内 clear、ScanInt(NULL)、bool 直存 setne、
// if/else 正极性守卫、Main 代落 T_Rect 去用户析构）。残余 byte 层 N 封顶：
// ① N104/EH 类：我方 TownAreaScript+两 string 的 cleanup 链相对正常路径的块序与
//    ORIG 相反（内容逐条等价，仅尾块互换）；② P223 单源尾岛合并（`je` 落点）；
// ③ 少量 `mov %eax,A(%esp)` vs `(%esp)` 寻址形态。
bool importTownScript(TownScript* script, const char* path)
{
    // ORIG T 0x8aada90（382 条）：ScanType 失败=文件读完并成功收尾；各标签
    // 的 ScanStr/ScanInt 按报告保留其原始检查边界。
    if (!loadRDARScriptFile(g_townScriptBaseDirectory, path))
    {
        return false;
    }
    script->clear();

    // ORIG：getDirectoryPath(script->m_str38, "%s%s", g_townScriptBaseDirectory, path)
    getDirectoryPath(script->m_str38, "%s%s", g_townScriptBaseDirectory, path);

    std::string line;
    std::string sub;
    int value = 0;
    bool flag = false;
    // ORIG 8aadb2b：TownAreaScript 在循环**外**构造（-0x54），[area] 段内只
    // `call clear`（8aadc82）后复用同一对象。
    TownAreaScript area;
    for (;;)
    {
        if (!ScanType(line, true))
        {
            break;
        }
        if (line == "[name]")
        {
            ScanStr(&script->m_str4);
            continue;
        }
        if (line == "[entering title]")
        {
            ScanStr(&script->m_str8);
            continue;
        }
        if (line == "[entering title next]")
        {
            ScanStr(&script->m_str10);
            continue;
        }
        if (line == "[cutscene image]")
        {
            ScanStr(&script->m_strc);
            script->m_14 = ScanInt((bool*)0);   // ORIG：ScanInt 返回直接存（无检查）
            continue;
        }
        if (line == "[dungeon what must be cleared]")
        {
            script->m_18 = ScanInt((bool*)0);
            continue;
        }
        if (line == "[only server parsing dungeon what must be cleared]")
        {
            script->m_1c = ScanInt((bool*)0);
            continue;
        }
        if (line == "[area]")
        {
            area.clear();
            // ORIG 8aadc87 `movl $0x0,(%esp)` ⇒ 传 NULL，无检查；flag 全程用外层
            // -0x19 槽（块内再声明会另占槽，P212）。
            area.m_areaIndex = ScanInt((bool*)0);
            ScanStr(&area.m_mapName);               // ORIG 8aadc9f 无检查
            ScanStr(&sub);                          // ORIG 8aadca4 ScanStr（非 ScanType）
            if (sub == "[minimap rect]")
            {
                // ORIG 8aadcca..8aadd75：T_Rect 复用 + 多个 rect；内层每次
                // ScanInt 得到下一项，四项后的值直接作为下一矩形首项复用。
                T_Rect r;
                value = ScanInt(&flag);                 // 8aadcd5
                while (flag)
                {
                    int i = 0;
                    do
                    {
                        if (i == 0)
                            r.m_00 = value;
                        else if (i == 1)
                            r.m_08 = value;
                        else if (i == 2)
                            r.m_04 = value;
                        else
                            r.m_0c = value;
                        value = ScanInt(&flag);         // 8aadd1e
                        ++i;
                    } while (i <= 3 && flag);           // 8aadd30
                    area.m_rects.push_back(r);           // 8aadd4e
                }
                ScanStr(&sub);                      // ORIG 8aadd6f ScanStr
            }
            if (sub == "[use private store]")
            {
                value = ScanInt(&flag);             // ORIG 8aadd97
                area.m_field24 = (value != 0);      // ORIG 8aadda2 setne（bool 直存）
                ScanStr(&sub);                      // ORIG 8aaddb0 ScanStr
            }
            // 区域类型：normal/gate/fishing place/pub/dungeon gate/
            //          safe zone against a / safe zone against b
            if (sub == "[normal]")
                area.m_areaType = 0;
            else if (sub == "[gate]")
                area.m_areaType = 1;
            else if (sub == "[fishing place]")
                area.m_areaType = 2;
            else if (sub == "[pub]")
                area.m_areaType = 3;
            else if (sub == "[dungeon gate]")
                area.m_areaType = 4;
            else if (sub == "[safe zone against a]")
                area.m_areaType = 5;
            else if (sub == "[safe zone against b]")
                area.m_areaType = 6;
            else
                area.m_areaType = 0;
            // ORIG 8aade99..8aadec7：循环 ScanInt 直到 flag 失败，全部 push
            for (;;)
            {
                value = ScanInt(&flag);
                if (!flag)
                    break;
                area.m_field18.push_back(value);
            }
            script->m_areas[area.m_areaIndex] = area;
            continue;
        }
        if (line == "[limit level]")
        {
            int level = ScanInt(&flag);
            // ORIG 8aadf18：`test; je <return false 块（在后）>` ⇒ if/else 正极性
            // （早返形 4.4 恒把 return false 块前置，P59/P222）。
            if (flag)
            {
                script->m_3c = level;
            }
            else
            {
                return false;
            }
            continue;
        }
        if (line == "[need quest]")
        {
            int quest = ScanInt(&flag);
            if (flag)
            {
                script->m_40 = quest;
            }
            else
            {
                return false;
            }
            continue;
        }
        // 未知键：忽略（回到循环读下一行）
    }
    // ORIG 尾部：script->m_field0 = g_townScriptFileList.FindIndexByFullScan(path)
    script->m_field0 = g_townScriptFileList.FindIndexByFullScan(path);
    return true;
}

// [b28d-solo-STQuestScript · P0/P244/P171] ORIG 0x08aaf554（257 条）逐条还原：
// ①具名指针副本 s（ORIG `mov 0x8(%ebp),%eax; mov %eax,-0x1c; mov -0x1c,%eax`
//   槽往返 ⇒ 原码有具名指针局部，P0）；②pair 循环外默认构造 + 成员赋值；
// ③各段尾 `sub` 与闭合标签比较（ORIG 中 sub 全函数无写入点 = 原码遗留恒假
//   死条件，P171 照抄）；④ScanType 失败/[training quest end] 走统一 `return ok`。
// [§6.2 三件套] 2026-09-09 复核：257 vs 253（gap 4）；p167 30/30 CLEAR。
// 逐条归一化 diff 实测 4 处残差全为跳转中继岛/对齐 nop（无语义）：
// ① +0x5e..+0x63 ORIG `jmp;nop;jmp;nop` 双中继岛（[level] else 的 continue
//    落 +0x5b4、[training quest end] 不等落 +0x5b7）vs 我方单 `jmp`（差 2）；
// ② +0x71e nop（`je` 后 `jmp` 前）与 +0x802 nop 两处 4.1 对齐填充（差 2）；
// ③ +0x680/+0x685 两条同目标 jmp（5b8）4.4 合并为一条（差 1），我方
//    +0x2cb/+0x2ee 侧多一条 jmp（+1）⇒ 净 -4。
// 4.4 对同址目标恒 jump-thread（P223 负例），nop 属 §6.1 对齐产物 ⇒ N 封顶。
// 尾部 `movzbl -0x25(%ebp),%ebx` 已与 ORIG 单段形一致（旧 N18a 残差已消）。
// [2026-09-10 SweepSTQuestScript-2] [level] 分支补 2 处显式 continue
// （ORIG 0x8aaf7d6 / 0x8aaf802 两条 `jmp loop_head`）⇒ raw 253→254；
// 残余 gap 3 = ORIG 循环头前 2 组 `jmp loop_head;nop`（0x8aaf5b2 入口 +
// 0x8aaf5b5，后者是 0x8aaf7df `jne` 的落点）被 4.4 jump-thread 合并为 1 组，
// 0x8aaf685 中继同被合并 ⇒ P223 族封顶（无合法 C++ 源形可分裂）。
// 四数 458/917/0/12 不回退；p167 无 ORIG-only 游戏域 callee。
bool importTrainingQuestScript(TrainingQuestScript* script, const char* path)  // [P330] 与 STQuestScript.h 同步
{
    if (!loadRDARScriptFile("", path))
    {
        return 0;
    }
    TrainingQuestScript* s = script;
    s->clear();
    std::string line;
    int prev = 0;
    std::string sub;
    bool ok = false;
    for (;;)
    {
        if (!ScanType(line, true))
        {
            break;
        }
        if (line == "[select prob]")
        {
            s->m_selectProb.m_count = ScanInt(&ok);
            if (!ok)
            {
                break;
            }
            std::pair<int, int> p;
            for (;;)
            {
                p.first = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                p.second = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                s->m_selectProb.m_pairs.push_back(p);
                // p167：ORIG 用 _ZSteq…（operator==）⇒ 原码正极性 ==；相等→退出循环。
                if (sub == "[/select prob]")
                {
                    break;
                }
            }
            continue;
        }
        if (line == "[rand value]")
        {
            for (int day = 0; day <= 0x16d; ++day)
            {
                int count = 0;
                while (count < s->m_selectProb.m_count)
                {
                    int v = ScanInt(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    s->m_selectProb.m_dayProb[day].push_back(v);
                    ++count;
                }
                if (sub == "[/rand value]")
                {
                    break;
                }
            }
            continue;
        }
        if (line == "[level]")
        {
            int level = ScanInt((bool*)0);
            if (prev + 1 == level)
            {
                // ORIG：递增序号段的数值进 m_questProb[prev]，行首失败仅结束该段。
                for (;;)
                {
                    int value = ScanInt(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    s->m_questProb[prev].push_back(value);
                    if (sub == "[/level]")
                    {
                        break;
                    }
                }
                prev = level;
                continue;
            }
            else if (level == -100)
            {
                // ORIG：-100 段的数值进 m_sameApplyQuest。
                for (;;)
                {
                    int value = ScanInt(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    s->m_sameApplyQuest.push_back(value);
                    if (sub == "[/level]")
                    {
                        break;
                    }
                }
                continue;
            }
            continue;
        }
        if (line == "[training quest end]")
        {
            ok = true;
            break;
        }
        // 未知键：忽略（回循环读下一行）
    }
    return ok;
}

// ORIG B 0x09500380：脚本根目录（EpisodeScript 等 loadRDARScriptFile 首参）。
char g_questScriptBaseDirectory[0x100];

// ORIG T 0x8a60f78（0x95）：0x14 字节标量区清零（+0x10 为字节）+ m_str20 构造
// + m_field24 清零 + 11 个 dialog 指针清零（i=0..10，P151）。
dungeonEventHanaseStruct::dungeonEventHanaseStruct()
    : m_field0(0), m_field4(0), m_field8(0), m_fieldc(0), m_field10(0),
      m_field14(0), m_field18(0), m_field1c(0), m_str20(), m_field24(0)
{
    for (int i = 0; i <= 10; ++i)
        m_dialogs[i] = 0;
}

// ORIG T 0x8a6100e：逐字段拷贝 + 深拷贝 11 个 dialog 串
// （new std::string + operator=，非拷贝构造；ORIG asm _Znwj→SsC1→SsaSERKSs 实证）。
dungeonEventHanaseStruct::dungeonEventHanaseStruct(
    const dungeonEventHanaseStruct& o)
    : m_field0(o.m_field0), m_field4(o.m_field4), m_field8(o.m_field8),
      m_fieldc(o.m_fieldc), m_field10(o.m_field10), m_field14(o.m_field14),
      m_field18(o.m_field18), m_field1c(o.m_field1c), m_str20(o.m_str20),
      m_field24(o.m_field24)
{
    for (int i = 0; i <= 10; ++i)
    {
        m_dialogs[i] = 0;
        if (o.m_dialogs[i] != 0)
        {
            m_dialogs[i] = new std::string;
            *m_dialogs[i] = *o.m_dialogs[i];
        }
    }
}

// ORIG T 0x8a615fe（0x44）：m_field24!=0 且 0<=job<=10 且 m_dialogs[job]!=0 →
// 返回 m_dialogs[job]；否则返回 &m_str20（+0x20）。
std::string* dungeonEventHanaseStruct::getNewDialog(ENUM_CHARACTERJOB job)
{
    if (m_field24 != 0 && static_cast<int>(job) >= 0 &&
        static_cast<int>(job) <= 10 && m_dialogs[job] != 0)
    {
        return m_dialogs[job];
    }
    return &m_str20;
}
