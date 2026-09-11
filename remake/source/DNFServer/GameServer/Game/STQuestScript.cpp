// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 STQuestScript/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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

#include "STQuestScript/loadRDARScriptListFile.inc"
                            const char* listPath, std::map<int, int>* map);
#include "STQuestScript/IsLightServer.inc"
#include "STQuestScript/OutputErrorMessage.inc"
#include "STQuestScript/ScanRewardItems.inc"
#include "STQuestScript/ConvertStringToTime.inc"
#include "STQuestScript/ConvertWeekdayStringToInt.inc"
#include "STQuestScript/ConvertHHMMSSStringToInt.inc"
                              unsigned char& m, unsigned char& sec);  // 0x088c25df
#include "STQuestScript/ScanJobStr.inc"


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

#include "STQuestScript/QuestScript.inc"


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



#include "STQuestScript/dtor_QuestScript.inc"



#include "STQuestScript/clear.inc"


#include "STQuestScript/destroy.inc"


#include "STQuestScript/getName.inc"


#include "STQuestScript/getRequestMessage.inc"


#include "STQuestScript/getConditionMessage.inc"


#include "STQuestScript/getSolveMessage.inc"


// ===================== TownScript =====================

#include "STQuestScript/TownScript.inc"


#include "STQuestScript/dtor_TownScript.inc"


#include "STQuestScript/clear_1.inc"


// ===================== TrainingQuestScript =====================

#include "STQuestScript/ST_TrainingQuest_Select_Prob.inc"


#include "STQuestScript/dtor_ST_TrainingQuest_Select_Prob.inc"
{
}

#include "STQuestScript/TrainingQuestScript.inc"
{
    // ORIG C1 0x8371cc8：成员数组隐式构造（70 个 vector）后仅调 clear()。
    clear();
}

#include "STQuestScript/dtor_TrainingQuestScript.inc"
{
}

#include "STQuestScript/clear_2.inc"
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

#include "STQuestScript/reset_same_apply_level_cnt.inc"
{
    m_applyCount = 0;
}

#include "STQuestScript/computeDayIndex.inc"
{
    time_t now = time(0);
    struct tm* t = localtime(&now);
    return t->tm_yday;
}

#include "STQuestScript/get_random_fake_value.inc"


#include "STQuestScript/suffleTrainingQuests.inc"


#include "STQuestScript/getTrainingQuest.inc"


#include "STQuestScript/getApplyLevel.inc"


// ===================== 脚本导入 =====================

#include "STQuestScript/initQuestScript.inc"


#include "STQuestScript/importQuestScript.inc"
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
// [P2S2STQuest 2026-09-11 复核] 当前实测槽位：qd0=-0x3dc/qd1=-0x40c（比 ORIG 深 4）、
// line=-0x30c/sub=-0x310/value=-0x314/ok=-0x315（比 ORIG 浅 20）、frame 0x46c（两侧
// 相同）；ORIG 锚 line=-0x320/sub=-0x324/value=-0x328/ok=-0x329/qd0=-0x3d8/qd1=-0x408。
// ⇒ 「qd0→line」段我方多占 24B、「line→entry」段少占 24B，属帧分配律错位（非某个
// 块内新具名局部，已按 §具名局部判据排除）⇒ 本轮未动，封账维持。
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
#include "STQuestScript/initTownScript.inc"


#include "STQuestScript/importTownScript.inc"


// [SplitC 2026-09-11 batch4 · TU 迁移] importTrainingQuestScript 已迁出至 RDARScriptTrainingQuest.cpp（ORIG GI 键对齐）。



// ORIG B 0x09500380：脚本根目录（EpisodeScript 等 loadRDARScriptFile 首参）。
extern char g_questScriptBaseDirectory[0x100];   // [SplitC batch5] 定义已迁出至 RDARScriptQuest.cpp

#include "STQuestScript/dungeonEventHanaseStruct.inc"


#include "STQuestScript/dungeonEventHanaseStruct_1.inc"


#include "STQuestScript/getNewDialog.inc"

