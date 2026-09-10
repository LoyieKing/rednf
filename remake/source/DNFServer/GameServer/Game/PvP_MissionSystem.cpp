#include "CDataManager.h"
// ---- b28d-61 判词汇总（本 TU 4 件非 identical）----
// 已修 AE：Factory（P171 去 default 冗余赋值）、Load_EtcParameter（P292 早返）、
//   sort_list（P175/P225 索引循环+P159 int 返回）、C1（new CMTRand(time(0))+
//   resize(0x24,list)）、D1（P141 delete 直删）、Load_list（P198/P140/P167 全段还原）。
// b28d-70：get_WithinMissionIndex 0x085e2f00 已修（CMission.h MissionRankRange 改为
//   std::pair<int,int>，本函数同步 pair 形，P167 CLEAR）。
// [R26D 本轮修正] 三件 DIFF 全部推进（p167 真缺口清零/收敛）：
//  · importPvpMissionParameterScript 0x08a5b7ec —— 头置条件形（for(;;)+
//    if(!ScanType) break；ORIG +0x4a 循环头、+0x266 出口）⇒ gap −7→−3。
//    残余：①`push_back(idx)` 左值选 ERKj 而 ORIG 用 EOj 右值重载（改右值实测
//    identical −1：本 TU 唯一的 push_backERKj 实例不再发射，代价 > 收益，已回退；
//    p167 仍 MISSING EOj/EXTRA ERKj）；②`m_field0c = ScanInt()` 的 RHS-first+
//    edx 中转需该字段 unsigned 化（P211 档③；探针 h3 实证，cast 形 h6 无效）⇒
//    属共享头 MissionTypes.h，未落；③槽号/帧镜像 N82（ORIG idx 复用 −0x38，
//    4.4 独立分槽 ⇒ 帧 0x50 vs ORIG 0x40）。
//  · importPvpMissionScript 0x08a5ac28 —— 尾置条件形（while(ScanType)…；ORIG
//    +0xb26 尾条件块 `test %al; jne +0x79`、失败 +0xb47 `mov $1,%ebx`）+
//    `!(token == "[condition]")`（P167：ORIG 调 eq 版，旧文 `!=` 生成 ne）⇒
//    gap +5→+2；残余 = N89（ORIG 内联头 inline，4.4 恒 call）+ C1/C2 别名伪差。
//  · get_New_DailyMission 0x085e26e0 —— `make_pair` 去多余 static_cast<unsigned>
//    （ORIG 用 make_pairIiiE；旧文选 IjjE）+ `!(mi==end)` 取反式（eq 版）+
//    `(*mi).second` 与 `it->second` 混用（ptEv/deEv 各 1 = ORIG 同量）⇒ p167
//    差集由 7 项收敛到 3 项（余 _Unwind_Resume×2 = EH 段、vector::clear×1 = N89、
//    C1/C2 别名）。
//  · 全 TU 四数（fresh check_tu_routed）：225/466/0/6/691/missing171 →
//    225/465/0/6/690/missing170（missing −1、仅NEW −1、identical 不变）。
#include "CMission.h"
#include "MissionLessThan.h"
#include "PvPTypes.h"          // CMTRand
#include "DNFLexWrapper.h"     // loadRDARScriptFile / Scan* helper
#include <stdio.h>
#include "SyncScriptDeps.h"   // checkValiedJobName / getCharacterJob
#include "NpcScript.h"

// ============================================================================
// PvP_MissionSystem 装载支撑（G5 批次）。
// ORIG 各函数地址/语义见 docs/class_func_reports/PvP_MissionSystem/*.md。
// ============================================================================

// ---- 本地 STScriptFileList（与 CItemShop.cpp / STScriptFileList.cpp 同布局：
//      +0x00 std::map<int,std::string>，大小 0x18）----


// [L32 GI 调序] ORIG `_GLOBAL__I_g_PvpMissionScriptBaseDirectory`（CU=RDARScriptPvpMission.cpp）
//   ⇒ 本 TU 首个强符号须为 g_PvpMissionScriptBaseDirectory。仅交换两块定义的先后：
//   零语义/零布局改动（g_PvpMissionScriptFileList 仍是本 TU 唯一动态初始化对象，
//   相对构造序不变，P327）。
// ORIG 全局（BSS 0x09500100，0x100 字节）：PvP 任务脚本根目录（运行期由
// 部署环境填充；BSS 初值即空串）。文件列表全局见下方 g_PvpMissionScriptFileList。
char g_PvpMissionScriptBaseDirectory[0x100];
// ORIG 全局 STScriptFileList（PvP 任务脚本列表，BSS 0x0949…）。由
// CDataManager::init 的 loadRDARScriptListFile 填充；此处提供符号。
STScriptFileList g_PvpMissionScriptFileList;

// ---- ORIG _ZN13MissionScript5clearEv @0x08a5bbb2 ----
void MissionScript::clear()
{
    // [b28c-lane17] ORIG 0x08a5bbc6：pair 临时构造（两 int 0 槽）+ operator=。
    m_grade = std::pair<int, int>(0, 0);   // +0x10/+0x14
    m_field20 = 4;               // +0x20
    m_job = 0xb;                 // +0x24（无职业）
    m_field28.clear();
    m_field34 = 0x10;            // +0x34（默认 kind）
    m_field00 = -1;              // +0x00（[next mission]）
    m_prevMissionIndex = -1;     // +0x04（[prev mission]）
    m_str38.clear();
    m_str3c.clear();
    m_rewardItems.clear();
    m_expRate = 0.0f;            // +0x40
    m_stealingSkill = 0;         // +0x5c
    m_rewardSkill = 0;           // +0x5d
    m_rewardSp = 0;              // +0x5e
    m_comboMap.clear();
    m_field08 = 0;
    m_field0c = 0;
}

// ---- ORIG _ZN22MissionParameterScript5clearEv @0x08a5bca2 ----
void MissionParameterScript::clear()
{
    m_dailyIndices.clear();      // +0x00
    m_field0c = 0;
    m_dailyList.clear();         // +0x10
    m_field18 = 0;
    m_baseExp.clear();           // +0x1c
}

// ---- ORIG _Z13_ReadThreeIntP13MissionScript @0x08a5ab91 ----
// 条件参数三连读：+0x78 / +0x7c / +0x80。
void _ReadThreeInt(MissionScript* script)
{
    script->m_kind = ScanInt((bool*)0);
    script->m_field7c = ScanInt((bool*)0);
    script->m_field80 = ScanInt((bool*)0);
}

// ---- ORIG _Z12_ReadFourIntP13MissionScript @0x08a5abd2 ----
// [within] 条件四连读：+0x78 / +0x7c / +0x80 / +0x84。
void _ReadFourInt(MissionScript* script)
{
    script->m_kind = ScanInt((bool*)0);
    script->m_field7c = ScanInt((bool*)0);
    script->m_field80 = ScanInt((bool*)0);
    script->m_field84 = ScanInt((bool*)0);
}

// ============================================================================
// importPvpMissionScript（ORIG _Z22importPvpMissionScriptP13MissionScriptPKc，
// 0x8a5ac28）。反编译见 docs/class_func_reports/_lt_global_gt_/
// importPvpMissionScript.md。返回 false=装载/解析失败；true=成功或 EOF 正常收尾。
// ============================================================================
bool importPvpMissionScript(MissionScript* script, const char* path)
{
    if (!loadRDARScriptFile(g_PvpMissionScriptBaseDirectory, path))
        return false;

    script->clear();
    script->m_field0c =
        g_PvpMissionScriptFileList.FindIndexByFullScan(path);

    std::string token;
    std::string sub;
    std::string text;

    // [R26D] 尾置条件形（与 importPvpMissionParameterScript 的 ORIG 形相反）：
    // ORIG 0x8a5ac9c 入口 `jmp +0xb26` 直落函数尾的条件块（+0xb26 `test %al,%al;
    // jne +0x79` 成功回体、失败 +0xb47 `mov $1,%ebx` 返回 true）；写成
    // `for(;;)+if(!ScanType) return true` 会被 4.4 生成头置条件（我方旧形 +0x7d）。
    while (ScanType(token, true))
    {
        if (token == "[rank range]")
        {
            script->m_grade.first = ScanInt((bool*)0);    // +0x10
            script->m_grade.second = ScanInt((bool*)0);   // +0x14
        }
        else if (token == "[kind]")
        {
            script->m_field08 = ScanInt((bool*)0);    // +0x08
        }
        else if (token == "[rate range]")
        {
            script->m_rate.first = ScanInt((bool*)0);     // +0x18
            script->m_rate.second = ScanInt((bool*)0);    // +0x1c
        }
        else if (token == "[grade]")
        {
            ScanStr(&token);   // ORIG 复用同一 token 缓冲
            if (token == "[normal]")
                script->m_field20 = 0;
            else if (token == "[repeatable]")
                script->m_field20 = 1;
            else if (token == "[daily]")
                script->m_field20 = 2;
            else if (token == "[rank up]")
                script->m_field20 = 3;
        }
        else if (token == "[target character]")
        {
            if (!ScanStr(&sub))
                return true;                     // ORIG 扫描失败→正常收尾
            if (!checkValiedJobName(sub.c_str()))
            {
                printf("parsing error : %s\n", sub.c_str());
                return false;
            }
            script->m_job = getCharacterJob(sub.c_str());   // +0x24
        }
        else if (token == "[pre required mission]")
        {
            for (;;)
            {
                bool ok = false;
                int idx = ScanInt(&ok);
                if (!ok)
                    break;
                script->m_field28.push_back(idx);       // +0x28
            }
        }
        // [R26D] P167 一击命中：ORIG 调 `_ZSteqIcSt11char_traitsIcESaIcEEbRKSb
        // IT_T0_T1_EPKS3_`（string==字面量）而旧文写 `token != "[condition]"`
        // 生成 `_ZStne...`（MISSING eq -1 / EXTRA ne +1）。取反写回 eq 形。
        else if (!(token == "[condition]"))
        {
            if (token == "[prev mission]")
                script->m_prevMissionIndex = ScanInt((bool*)0);   // +0x04
            else if (token == "[next mission]")
                script->m_field00 = ScanInt((bool*)0);            // +0x00
            else if (token == "[name_text]")
            {
                ScanStr(&text);
                script->m_str38 = text;                           // +0x38
            }
            else if (token == "[cond_text]")
            {
                ScanStr(&text);
                script->m_str3c = text;                           // +0x3c
            }
            else if (token == "[reward item]")
            {
                for (;;)
                {
                    bool ok = false;
                    int itemIdx = ScanInt(&ok);
                    if (!ok)
                        break;
                    int count = ScanInt(&ok);
                    if (!ok)
                        return false;
                    script->m_rewardItems.push_back(
                        std::make_pair(itemIdx, count));          // +0x44
                }
            }
            else if ("[reward skill]" == token)
                script->m_rewardSkill = 1;                        // +0x5d
            else if ("[reward sp]" == token)
                script->m_rewardSp = 1;                           // +0x5e
            else if (token == "[difficulty_ratio]")
                script->m_expRate = ScanFloat((bool*)0);          // +0x40
            else if (token == "[stealing skill]")
                script->m_stealingSkill = 1;                      // +0x5c
        }
        else
        {
            // ---- [condition] 子段 ----
            if (!ScanStr(&sub))
                return true;                 // ORIG 扫描失败→正常收尾

            if (sub == "[winning count]")         { script->m_field34 = 0x0; _ReadThreeInt(script); }
            else if (sub == "[winning streak]")   { script->m_field34 = 0x1; _ReadThreeInt(script); }
            else if (sub == "[join]")             { script->m_field34 = 0x2; _ReadThreeInt(script); }
            else if (sub == "[revenge]")          { script->m_field34 = 0x7; _ReadThreeInt(script); }
            else if (sub == "[kill]")             { script->m_field34 = 0x9; _ReadThreeInt(script); }
            else if (sub == "[kill count]")       { script->m_field34 = 0x3; _ReadThreeInt(script); }
            else if (sub == "[hereafter join]")   { script->m_field34 = 0x4; _ReadThreeInt(script); }
            else if (sub == "[hereafter winning count]") { script->m_field34 = 0x5; _ReadThreeInt(script); }
            else if (sub == "[remain HP]")        { script->m_field34 = 0x6; _ReadThreeInt(script); }
            else if (sub == "[hereafter kill count]") { script->m_field34 = 0x8; _ReadThreeInt(script); }
            else if ("[within]" == sub)           { script->m_field34 = 0xb; _ReadFourInt(script); }
            else if (sub == "[remainS HP]")       { script->m_field34 = 0xc; _ReadThreeInt(script); }
            else if ("[move_channel]" == sub)     { script->m_field34 = 0xd; _ReadThreeInt(script); }
            else if ("[move_channel_total]" == sub) { script->m_field34 = 0xe; _ReadThreeInt(script); }
            else if ("[combo clear]" == sub)
            {
                script->m_field34 = 0xf;
                std::string comboTok;
                for (;;)
                {
                    if (!ScanType(comboTok, true))
                        break;               // 内层 EOF → 回外层关键字循环

                    combo_script_key key;    // ctor：job=0xb/growType=0
                    combo_script_data data;  // ctor

                    if ("[job]" == comboTok)
                    {
                        if (!ScanStr(&sub))
                        {
                            printf("parsing error %d, %s\n", 0x129,
                                   sub.c_str());
                            return false;
                        }
                        if (!checkValiedJobName(sub.c_str()))
                        {
                            printf("parsing error : %s\n", sub.c_str());
                            return false;
                        }
                        key.m_job = (char)getCharacterJob(sub.c_str());
                        key.m_growType = (short)ScanInt((bool*)0);
                        int n = ScanInt((bool*)0);
                        std::pair<int, int> fill;
                        data.m_rewardList.resize(n, fill);
                        for (int i = 0; i < n; ++i)
                        {
                            data.m_rewardList[i].first = ScanInt((bool*)0);
                            data.m_rewardList[i].second = ScanInt((bool*)0);
                        }

                        if (!ScanType(comboTok, true))
                            continue;        // ORIG esi=1 → 循环头再读一次

                        if ("[reward item]" == comboTok)
                        {
                            // ORIG 读入两个 int 后未使用（忠实转写）。
                            (void)ScanInt((bool*)0);
                            (void)ScanInt((bool*)0);
                        }
                        (void)ScanType(comboTok, true);   // 再读一枚，结果忽略

                        if (script->m_comboMap.find(key) ==
                            script->m_comboMap.end())
                            script->m_comboMap.insert(
                                std::make_pair(key, data));
                        // 非 [job] 分支与插入完成均继续内层循环。
                    }
                }
            }
        }
    }
    return true;
}

// ============================================================================
// importPvpMissionParameterScript（ORIG
// _Z31importPvpMissionParameterScriptR22MissionParameterScript，0x8a5b7ec）。
// 反编译见 docs/class_func_reports/_lt_global_gt_/
// importPvpMissionParameterScript.md。
// ============================================================================
bool importPvpMissionParameterScript(MissionParameterScript& param)
{
    param.clear();
    if (!loadRDARScriptFile("Etc/", "MissionSystem.etc"))
        return false;

    std::string token;
    bool ok = false;
    // [R26D] 头置条件形（P283/P164 反向）：ORIG 0x8a5b847 入口 `jmp +0x4a` 后
    // 循环头即 ScanType（`xor $1;test;jne +0x266` 失败跳尾部出口），
    // `while(ScanType(...))` 会被 4.4 旋转成尾置条件（我方旧形循环头在 +0x239）。
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if ("[frequent daily mission]" == token)
        {
            int count = ScanInt(&ok);
            if (!ok)
                return false;
            for (int i = 0; i < count; ++i)
            {
                unsigned int idx = (unsigned int)ScanInt(&ok);
                if (!ok)
                    return false;
                param.m_dailyIndices.push_back(idx);      // +0x00
            }
        }
        else if ("[casual daily mission]" == token)
        {
            param.m_field0c = ScanInt(&ok);               // +0x0c
            if (!ok)
                return false;
            for (;;)
        {
                int weight = ScanInt(&ok);
                if (!ok)
                break;                                 // ORIG 0x8a5b944 nop / 945 jmp 836
                                                       //（4.4 goto 形会复制一份条件头，实测 +6，故留 break）
                int missionIdx = ScanInt(&ok);
                if (!ok)
                    return false;
                param.m_dailyList.push_back(
                std::make_pair(weight, missionIdx));   // +0x10
                param.m_field18 += weight;                      // +0x18
        }
    }
        else if ("[mission reward exp table]" == token)
            {
            param.m_baseExp.resize(0x1e);                 // +0x1c
            for (int i = 0; i < 0x1e; ++i)
        {
            // ORIG 0x8a5ba1d..0x8a5ba35：operator[] 取址 → ScanInt → 直 store → ok 检查
            //（无 v 具名槽，P0）
            param.m_baseExp[i] = (unsigned int)ScanInt(&ok);
                if (!ok)
                    return false;
        }
    }
}
    return true;
}

// ============================================================================
// 构造（ORIG 0x85e1abc）
// 顺序：map@+0x00、vector<list<CMission*>>@+0x18、vector<uint>@+0x24、
//       list<pair<uint,uint>>@+0x34、bitset@+0x40、vector<uint>@+0x48；
//       m_trand = new CMTRand(time(0))；m_field3c=0；reset bitset；
//       m_kindMissionList.resize(0x24)。
// ============================================================================
// ORIG 0x85e1abc：+0x30(m_field30)/+0x3c(m_field3c)/+0x44(m_trand) 仅
// 构造函数体内赋值一次，初始化列表不产生写入（m_field30 保持垃圾，与 ORIG 一致）。
PvP_MissionSystem::PvP_MissionSystem()
    : m_missionMap(),
      m_kindMissionList(),
      m_dailyMissionIndices(),
      m_dailyMissionList(),
      m_dailyMissionKind(),
      m_baseMissionExp()
{
    // b28d-61 [ORIG 0x085e1b16-0x085e1b5d `movl $0x0,(%esp); call time; mov %eax,-0x24;
    // lea -0x24,%esi; movl $0x9c8,(%esp); call new; call CMTRand::C1ERKm`]：直接
    // `new CMTRand(time(0))`（ctor 吃 unsigned long const&，临时占槽 -0x24），
    // 非「默认构造 + seed()」两段。
    m_trand = new CMTRand((unsigned long)time(0));
    m_field3c = 0;
    m_dailyMissionKind.reset();
    // ORIG 0x085e1b78-0x085e1b9b：先构造空 list 临时（-0x20），再调
    // vector::resize(0x24, list)（085e7444 按值传第二参）——非单参 resize。
    m_kindMissionList.resize(0x24, std::list<CMission*>());
}

// ============================================================================
// 析构（ORIG 0x85e1c48）
// 顺序：delete m_trand；m_trand=0；遍历 m_missionMap delete 各 CMission；
//       m_missionMap.clear(); 其余成员默认析构。
// ============================================================================
PvP_MissionSystem::~PvP_MissionSystem()
{
    if (m_trand != 0)
        delete (CMTRand*)m_trand;
    m_trand = 0;
    puts("Mission System Delete");   // ORIG 0x85e1c72-0x85e1c79

    for (std::map<int, CMission*>::iterator it = m_missionMap.begin();
         it != m_missionMap.end(); ++it)
    {
        // b28d-61 [P141/P157，ORIG 0x085e1c8x `mov 0x4(%eax),%eax; test %eax,%eax;
        // je`]：无具名局部、无显式判空——delete 自带空指针守卫；旧形多
        // `mov %eax,-0xc` + 双重 cmpl。
        delete it->second;
    }
    m_missionMap.clear();
}

// ============================================================================
// Factory（ORIG 0x85e2132）
// 依 script.m_field34（m_kind 的派生分派）switch 构造对应 CMission_* 子类。
// 各子类 ctor 实参布局（script 偏移）：
//   +0x7c count / +0x80 第二参数 / +0x84 第三参数 / +0x60 comboMap。
// ============================================================================
CMission* PvP_MissionSystem::Factory(const MissionScript& script)
{
    CMission* result = 0;
    switch (script.m_field34)
    {
    case 0:   result = new CMission_winning_count(script, script.m_field7c); break;
    case 1:   result = new CMission_winning_streak(script, script.m_field7c, script.m_field80); break;
    case 2:   result = new CMission_join(script, script.m_field7c); break;
    case 3:   result = new CMission_kill_count(script, script.m_field7c); break;
    case 4:   result = new CMission_hereafter_join(script, script.m_field7c); break;
    case 5:   result = new CMission_hereafter_winning_count(script, script.m_field7c); break;
    case 6:   result = new CMission_remain_hp(script, script.m_field7c, script.m_field80); break;
    case 7:   result = new CMission_revenge(script, script.m_field7c); break;
    case 8:   result = new CMission_hereafter_kill_count(script, script.m_field7c); break;
    case 9:   result = new CMission_kill(script, script.m_field7c, script.m_field80); break;
    case 0xb: result = new CMission_within(script, script.m_field7c, script.m_field80, script.m_field84); break;
    case 0xc: result = new CMission_remains_hp(script, script.m_field7c, script.m_field80); break;
    case 0xd: result = new CMission_move_channel(script, script.m_field7c); break;
    case 0xe: result = new CMission_move_channel_total(script, script.m_field7c); break;
    case 0xf: result = new CMission_combo_clear(script, script.m_comboMap); break;
    // b28d-61 [P171 照抄不写]：ORIG 0x85e213b 入口已 `movl $0x0,-0x1c`，default
    // 无赋值直落 return；旧 `default: result = 0;` 多出 `movl $0x0,-0x1c`+jmp
    // （384 vs 382）。
    default:  break;
    }
    return result;
}

// ============================================================================
// Load_list（ORIG 0x85e1dc6）
// 遍历 g_PvpMissionScriptFileList.m_fileMap，逐条 import，成功则 Factory、
// 按 GetMissionIndex 插入 m_missionMap、按 GetMissionKind 挂入 m_kindMissionList、
// kind==2 时置 m_dailyMissionKind 位；最后 sort_list。
// ============================================================================
bool PvP_MissionSystem::Load_list()
{
    // b28d-61 按 ORIG 0x085e1dc6 逐段还原：
    // ① path 函数级声明先清零（P198 两句式，ORIG `movl $0x0,-0xc` 在函数头）；
    // ② 非 const iterator（ORIG begin 为非 const 版，无转换构造）；
    // ③ insert(make_pair(idx,mission))（ORIG 085e7567 make_pair + 085e75de
    //    map::insert(pair const&)，非 operator[]）；
    // ④ m_kindMissionList[kind] 无边界防御（P140，ORIG 直接下标）；
    // ⑤ grade/kind 每次内联调用 getter（无具名 int 槽，ORIG sete/test/je 形）；
    // ⑥ `m_dailyMissionKind[kind] = true`（ORIG bitset::operator[] + reference::op=，
    //    非 set()&0x1f）。
    const char* path = 0;
    // ⑦ 声明序 path→it→script：ORIG 发射序为 path 清零、begin、script 构造
    //    （script 槽 -0xc4 最深），故 it 的初始化必须写在 script 声明之前。
    std::map<int, std::string>::iterator it =
        g_PvpMissionScriptFileList.m_fileMap.begin();
    MissionScript script;
    for (; it != g_PvpMissionScriptFileList.m_fileMap.end(); ++it)
    {
        path = it->second.c_str();
        if (path == 0)
            break;
        if (!importPvpMissionScript(&script, path))
        {
            printf("Mission Script Error: %s\n", path);
            return false;
        }
        CMission* mission = Factory(script);
        if (mission == 0)
            return false;
        m_missionMap.insert(std::make_pair(mission->GetMissionIndex(), mission));
        m_kindMissionList[mission->GetMissionKind()].push_back(mission);
        if (mission->GetMissionGrade() == 2)
            m_dailyMissionKind[mission->GetMissionKind()] = true;
    }
    sort_list();
    return true;
}

// ============================================================================
// Load_EtcParameter（ORIG 0x85e1fee）
// importPvpMissionParameterScript 成功则 copy_missionParameter。
// ============================================================================
bool PvP_MissionSystem::Load_EtcParameter()
{
    MissionParameterScript param;
    // b28d-61 [P292/P63，ORIG 0x085e1ffx `call; xor $0x1,%eax; test %al,%al;
    // je +0x38`]：具名 bool ok + return ok 使 4.4 产槽物化 `mov %al,-0x9;
    // cmpb $0,-0x9`；改负条件早返（bool 直判 + 两臂共享 ebx 出口）。
    if (!importPvpMissionParameterScript(param))
    {
        puts("Mission Parameter Error!");
        return false;
    }
    copy_missionParameter(param);
    return true;
}

// ============================================================================
// copy_missionParameter（ORIG 0x85e206e）
//   m_dailyMissionIndices(+0x24) = param.m_dailyIndices(+0x00)
//   m_field30(+0x30)             = param.m_field0c
//   m_dailyMissionList(+0x34)    = param.m_dailyList(+0x10)
//   m_field3c(+0x3c)             = param.m_field18
//   m_baseMissionExp(+0x48)      = param.m_baseExp(+0x1c)
// ============================================================================
void PvP_MissionSystem::copy_missionParameter(const MissionParameterScript& param)
{
    m_dailyMissionIndices = param.m_dailyIndices;
    m_field30 = param.m_field0c;
    m_dailyMissionList = param.m_dailyList;
    m_field3c = param.m_field18;
    m_baseMissionExp = param.m_baseExp;
}

CMission* PvP_MissionSystem::find_mission(int idx)
{
    std::map<int, CMission*>::iterator it = m_missionMap.find(idx);
    // [b28c-L18] ORIG 0x085e26xx 尾段实证：调用符号为 iterator::operator!=（this=it、
    // 实参 end），true→hit fall-through、`mov $0` 尾岛 ⇒ 原码 if(it!=end) return
    // it->second; return 0;（三目形在 4.4 产反序岛，P222 正向包裹）。
    if (it != m_missionMap.end())
        return it->second;
    return 0;
}

// ORIG 0x85e2f00（b28d-70 P167 CLEAR）：循环外构造 `std::pair<int,int> grade(0, 0)`
// （0x85e2f06/0x85e2f0d 双 movl $0 + C1IiiEEOT_OT0_ 0x081627d4），循环内
// `grade = (*it)->GetRankRange()`（aSEOS0_ 0x080ddf1c move 赋值），`grade.first == idx`
// 命中即返回 GetMissionIndex()；无匹配返回 0。ORIG 循环体无判空（P140：0x85e2f57 直接
// operator*→GetRankRange），故不写 `if (!*it)`。init/cond 两处**各调一次**
// `m_kindMissionList[0x1b]` 的 const operator[]（0x85e2f3e/0x85e2fc3）⇒ 不缓存 list
// 引用（P48/P183 重物化）。
// [b28d-70] 判词：DIFF→AE 70/71；P167 CLEAR 12=12（C1IiiE/aSEOS0_ 双符号命中）。
int PvP_MissionSystem::get_WithinMissionIndex(int idx) const
{
    std::pair<int, int> grade(0, 0);
    for (std::list<CMission*>::const_iterator it = m_kindMissionList[0x1b].begin();
         it != m_kindMissionList[0x1b].end(); ++it)
    {
        grade = (*it)->GetRankRange();
        if (grade.first == idx)
            return (*it)->GetMissionIndex();
    }
    return 0;
}

// ORIG 0x85e26a0：else 分支 operator[](0)（m_kindMissionList[0]，非静态空表）。
std::list<CMission*>& PvP_MissionSystem::get_kind_mission_list(unsigned int idx)
{
    // [b28c-L18] ORIG 尾形 `call size; cmp idx,%eax; seta %al; test %al,%al; je`
    // = 四段物化（P225 while+break 形：值无槽往返 ⇒ 禁具名 bool，P0）。三目直返
    // 在 4.4 恒折 `jbe` 直跳少 3 条；具名 bool 形恒槽化（-0x9 byte 槽实证）。
    while (m_kindMissionList.size() > idx)
    {
        return m_kindMissionList[idx];
        break;
    }
    return m_kindMissionList[0];
}

std::bitset<32> PvP_MissionSystem::get_daily_mission_kind() const
{
    return m_dailyMissionKind;
}

// ORIG 0x85e2e74：越界时 cMyTrace 记录 InvalidRank 并返回 0。
int PvP_MissionSystem::get_BaseMissionExp_byRank(unsigned short rank) const
{
    if (rank >= m_baseMissionExp.size())
    {
        cMyTrace(
            "unsigned int PvP_MissionSystem::get_BaseMissionExp_byRank(short unsigned int) const",
            0x162, 5)
            ("[@missionSystem] <InvalidRank : %d>", rank);
        return 0;
    }
    return static_cast<int>(m_baseMissionExp[rank]);
}

void PvP_MissionSystem::get_New_DailyMission(
    std::vector<std::pair<unsigned int, unsigned int> >& out)
{
    // ORIG 0x85e26dc 逐指令转写：三段——①dailyMissionIndices→map 查 mission
    // 推 {kind,idx}；②按 m_dailyMissionList 加权随机补 m_field30 个；③bitset
    // 校验 kind 范围/重复，越界或重复打印 cMyTrace 并清空 out。
    out.clear();
    for (std::vector<unsigned int>::size_type i = 0;
         i < m_dailyMissionIndices.size(); ++i)
    {
        const unsigned int id = m_dailyMissionIndices[i];
        std::map<int, CMission*>::iterator it =
            m_missionMap.find(static_cast<int>(id));
        if (it == m_missionMap.end())
            continue;
        CMission* mission = it->second;
        if (mission == 0)
            continue;
        // [R26D] P167：ORIG 用 `_ZSt9make_pairIiiE`（int,int），旧文的多余
        // static_cast<unsigned int> 使其选 `make_pairIjjE`（MISSING i 版 ×2 /
        // EXTRA j 版 ×2）；push_back 处由 pair<unsigned,unsigned> 转换构造承接。
        out.push_back(std::make_pair(
            mission->GetMissionKind(),
            mission->GetMissionIndex()));
    }
    // [R26D · 轴 B 反向命中] ORIG 0x085e2ac0 段 `cmp -0x34(%ebp),%eax; seta %al`
    // （无符号序）⇒ 循环变量须为 unsigned；旧文 `int j` 使 4.4 产 `setg`
    // （有符号，/tmp/r5/axisB_points.tsv 记 seta→setg）。`j < m_field30`（int 成员）
    // 经同宽无指令转换即无符号比较，P196 前置已核 ORIG 为 seta ⇒ 允许改。
    for (unsigned int j = 0; j < (unsigned int)m_field30; ++j)
    {
        std::list<std::pair<unsigned int, unsigned int> > tmp(m_dailyMissionList);
        int weight = m_field3c;
        int r = static_cast<CMTRand*>(m_trand)->randInt() % weight;
        int sum = 0;
        for (std::list<std::pair<unsigned int, unsigned int> >::iterator li =
                 tmp.begin(); li != tmp.end(); ++li)
        {
            sum += static_cast<int>(li->first);
            if (r < sum)
            {
                const unsigned int missionIdx = li->second;
                std::map<int, CMission*>::iterator mi =
                    m_missionMap.find(static_cast<int>(missionIdx));
                // [R26D] P167：ORIG 该点用 `==`（`_ZNK...eqERKS5_` ×1 差集）⇒
                // 写取反式而非 `!=`（ne 版是 EXTRA）。
                if (!(mi == m_missionMap.end()) && mi->second != 0)
                {
                    CMission* m = (*mi).second;
                    out.push_back(std::make_pair(
                        m->GetMissionKind(),
                        m->GetMissionIndex()));
                }
                weight -= static_cast<int>(li->first);
                tmp.erase(li);
                break;
            }
        }
        if (weight <= 0)
        {
            cMyTrace("void PvP_MissionSystem::get_New_DailyMission", 0x123, 5)
                ("InvalidMaxDice %d", weight);
        }
    }
    std::bitset<32> dailyKind(0);
    for (std::vector<std::pair<unsigned int, unsigned int> >::size_type k = 0;
         k < out.size(); ++k)
    {
        const unsigned int kind = out[k].first;
        if (kind == 0)
            continue;
        if (kind > 0x20)
        {
            std::string s;
            for (std::vector<std::pair<unsigned int, unsigned int> >::size_type
                     m = 0; m < out.size(); ++m)
            {
                char buf[0x110];
                sprintf(buf, "<K:%d, I:%d>", out[m].first, out[m].second);
                s += buf;
            }
            cMyTrace("void PvP_MissionSystem::get_New_DailyMission", 0x139, 5)
                ("[@missionSystem] OverKind: <Invalid mission kind : %d> "
                 "<mission index : %d> <Created DailyMission Size : %d> "
                 "<List : %s >",
                 kind, out[k].second, out.size(), s.c_str());
            out.clear();
        }
        else if (dailyKind[kind])
        {
            std::string s;
            for (std::vector<std::pair<unsigned int, unsigned int> >::size_type
                     m = 0; m < out.size(); ++m)
            {
                char buf[0x110];
                sprintf(buf, "<K:%d, I:%d>", out[m].first, out[m].second);
                s += buf;
            }
            cMyTrace("void PvP_MissionSystem::get_New_DailyMission", 0x14d, 5)
                ("[@missionSystem] OverlappedKind: <Invalid mission kind : %d> "
                 "<mission index : %d> <Created DailyMission Size : %d> "
                 "<List : %s >",
                 kind, out[k].second, out.size(), s.c_str());
            out.clear();
        }
        else
        {
            dailyKind[kind] = true;
        }
    }
}

int PvP_MissionSystem::sort_list()
{
    // b28d-61 [P175/P225，ORIG 0x085e20d4 `call operator[](i)` + `cmp -0x10(%ebp),
    // %eax; seta %al`]：索引循环（size() 左操作数 + seta 四段物化），非迭代器。
    // P159：ORIG 尾 `mov $0x0,%eax` ⇒ 返回 int（非 void）。
    // P0：ORIG 把 operator[] 结果写 -0xc 槽再回读 ⇒ 原码有具名引用局部。
    unsigned int i = 0;
    while (m_kindMissionList.size() > i)
    {
        std::list<CMission*>& l = m_kindMissionList[i];
        l.sort(MissionLessThan());
        i++;
    }
    return 0;
}
