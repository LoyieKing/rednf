// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 OnlyOrigBatch12_server_utility/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include <algorithm>
#include <list>
#include <map>
#include <queue>
#include <string>
#include <utility>
#include <vector>

#include "CEnvironment.h"        // CEnvironment / less_TCHAR / ENUM_SERVER_GROUP
#include "DNFLexWrapper.h"       // DNFLexWrapper::setStringTableFromBuffer
#include "LogManager.h"          // LogManager::logFormat
#include "PackSet.h"             // PackSet
#include "NpcScript.h"           // STScriptFileList / loadRDARScriptListFile
#include "CreatureScript.h"      // g_moduleNameTag
#include "GreyImage.h"           // GreyImage
#include "ServicePolicy.h"       // ServicePolicy
#include "ServiceRestrictManager.h"  // RestrictCategory::Enum
#include "CDataManager.h"        // rewardSubKey / stLimitItemUsageInfo
#include "CItemList.h"           // stCeraShopBonusInfo_t
#include "CGuildServerProxy.h"   // stGMInfo_t
#include "Quest.h"               // stQuestTargetCharacter_t
#include "TagRecommendNewbieOption.h"  // tagRecommendNewbieOptionTable
#include "Interval.h"                // IntervalSet

// ---------------------------------------------------------------------------
// 跨 TU 符号（ORIG 地址随注）
// ---------------------------------------------------------------------------
extern char g_szScriptFileBaseDirectory[0x104];   // B 0x949b180（ScriptExtras.cpp）
extern std::vector<std::string> g_szScriptFilePriorityBaseDirectory;  // B 0x949b168
#include "OnlyOrigBatch12_server_utility/getScriptFileFullPath.inc"
                           const char* dir, const char* name);        // T 0x088bba4e
// T 0x088bbd77（STScriptFileList.cpp 定义 int、恒 0/1；返回类型不入 mangled）。
// bool 视图声明：ORIG initPetScript 尾 `xor $1,%eax; test %al` 为 bool 取反形
// （P22/P168a①），int 声明使 4.4 产 test/sete 物化。
#include "OnlyOrigBatch12_server_utility/loadRDARScriptListFile.inc"
                            const char* listPath, std::map<int, int>* unused);

// ===========================================================================
// server_utility::get_rand_int  ORIG T 0x0808eafc：
//   range<0 → 0；range==0 → rand()；r=rand() 若 r<=range 走 LCG 三拍
//   （x = x*0x41c64e6d+0x3039，每拍取 (x/65536) 掩码位拼 c），
//   否则 rand()%range。
// （本文件首强实体 ⇒ GI 键名，ORIG TU = ServerUtility.cpp）
// ===========================================================================
namespace server_utility
{

#include "OnlyOrigBatch12_server_utility/get_rand_int.inc"
{
    if (range < 0)
        return 0;
    // P286：ORIG `mov 0x8(%ebp),%eax; test %eax,%eax; jne` 形（形参载入+寄存器
    // 测试）只能由单 case switch 复现（if 形 4.4 恒折 `cmpl $0,mem`）。
    switch (range)
    {
    case 0:
        return rand();
    }
    int r = rand();
    // [b28d-solo-OnlyOrigBatch12] AE（Main 采纳 -march=i486 路由后，clean 口径
    // 73/73）：switch(range) 复现 ORIG `mov 0x8(%ebp),%eax; test` 形（P286）；
    // 早返（P16/P214②）+ 拆语句（P38）+ unsigned 尾段 div 全部归位。三处
    // `(r/65536)` 有符号除法 4.4.7 默认产 lea+test+cmovs，-march=i486 下转为
    // ORIG 的 mov/sar/shr/lea（P230 补；探针 /tmp/probe_rand.cpp 实证）。
    // ORIG 0x0808eafc
    if (r > range)
        return rand() % range;
    r *= 0x41c64e6d;
    r += 0x3039;
    unsigned int c = static_cast<unsigned int>((r / 65536) & 0x7ff);
    r *= 0x41c64e6d;
    r += 0x3039;
    c <<= 10;
    c ^= (r / 65536) & 0x3ff;
    r *= 0x41c64e6d;
    r += 0x3039;
    c <<= 10;
    c ^= (r / 65536) & 0x3ff;
    // ORIG 尾段布局：`cmp c,%eax; jae <return c>`（fall-through 为 c%range）
    // ⇒ 条件反转写 `if (range < c) return c % range; return c;`；c 声明
    // unsigned int 令比较免 cast 中转（P209/P248 档③）。
    if (static_cast<unsigned int>(range) < c)
        return c % static_cast<unsigned int>(range);
    return c;
}

}  // namespace server_utility

// ---------------------------------------------------------------------------
// 本批新增全局（ORIG B 符号）—— 排在 get_rand_int 之后，不参与 GI 选名。
// g_script_package 的 ORIG TU = RDARScript.cpp（ORIG GI 键 = g_script_package）；
// g_petScriptBaseDirectory / g_petScriptFileList 属 PetScript 域。
// ---------------------------------------------------------------------------
char g_petScriptBaseDirectory[0x100];    // ORIG B 0x094ff3c0
STScriptFileList g_petScriptFileList;    // ORIG B 0x094ff4c0
PackSet g_script_package;                // ORIG B 0x09494fe0

// ===========================================================================
// initPetScript  ORIG T 0x08a3581b：基目录 strncpy 0x100 + 尾部补 '\\' +
// loadRDARScriptListFile；返回 load 结果 == 1。
// ===========================================================================
// [b28d-solo-OnlyOrigBatch12] AE：P303 `__builtin_strcat` 复现 ORIG 8 条内建展开
// （手写 memcpy 为 P68/P75 实参求值序反）；clean 口径 0 残差（43/43 条逐条同构）。
#include "OnlyOrigBatch12_server_utility/initPetScript.inc"
{
    strncpy(g_petScriptBaseDirectory, dir, 0x100);
    // ORIG 尾部校验/补 '\\'：strlen 三次内联右值重算（无长度局部，P0/Q189：
    // 数据全程寄存器直传 ⇒ 原码无具名 len 局部）。最后一次 strlen 结果 + base
    // 作 memcpy dest，取 "\\" 2 字节。
    if (g_petScriptBaseDirectory[strlen(g_petScriptBaseDirectory) - 1] != '\\' &&
        g_petScriptBaseDirectory[strlen(g_petScriptBaseDirectory) - 1] != '/')
    {
        // [b28d-solo-OnlyOrigBatch12] P303：目录分隔符追加必须 builtin 形
        // （4.4 -O0 内建展开 = ORIG 8 条形；手写 memcpy 求值序反，P68/P75）。
        __builtin_strcat(g_petScriptBaseDirectory, "\\");
    }
    // ORIG 尾：xor $1/test/je→mov$1 布尔物化。P0/N96 清绑：改 bool callee
    // 裸形（P168a① `if (!call()) return false;`）。
    if (!loadRDARScriptListFile(&g_petScriptFileList, g_petScriptBaseDirectory,
                                listPath, 0))
        return false;
    return true;
}

// ===========================================================================
// AVTVARIATION_TAG_NAME —— ORIG D 0x093f4840：47 个属性标签（[HP MAX] 等）。
// importRecommendNewbieOptionScript 的 [avatar index set] 逐槽按标签名查索引。
// （ORIG TU = CSCommonFunction.cpp，其 GI 键名即本符号）
// ===========================================================================
extern const char* const AVTVARIATION_TAG_NAME[47];
const char* const AVTVARIATION_TAG_NAME[47] = {
    "[none]", "[HP MAX]", "[MP MAX]", "[PHYSICAL_ATTACK]",
    "[PHYSICAL_DEFENSE]", "[MAGICAL_ATTACK]", "[MAGICAL_DEFENSE]",
    "[EQUIPMENT_PHYSICAL_ATTACK]", "[EQUIPMENT_PHYSICAL_DEFENSE]",
    "[EQUIPMENT_MAGICAL_ATTACK]", "[EQUIPMENT_MAGICAL_DEFENSE]",
    "[ELEMENT_TOLERANCE_FIRE]", "[ELEMENT_TOLERANCE_WATER]",
    "[ELEMENT_TOLERANCE_DARK]", "[ELEMENT_TOLERANCE_LIGHT]",
    "[ELEMENT_TOLERANCE_ALL]", "[ACTIVESTATUS_TOLERANCE_SLOW]",
    "[ACTIVESTATUS_TOLERANCE_FREEZE]", "[ACTIVESTATUS_TOLERANCE_POISON]",
    "[ACTIVESTATUS_TOLERANCE_STUN]", "[ACTIVESTATUS_TOLERANCE_CURSE]",
    "[ACTIVESTATUS_TOLERANCE_BLIND]", "[ACTIVESTATUS_TOLERANCE_LIGHTNING]",
    "[ACTIVESTATUS_TOLERANCE_STONE]", "[ACTIVESTATUS_TOLERANCE_SLEEP]",
    "[ACTIVESTATUS_TOLERANCE_BURN]", "[ACTIVESTATUS_TOLERANCE_WEAPON_BREAK]",
    "[ACTIVESTATUS_TOLERANCE_BLEEDING]", "[ACTIVESTATUS_TOLERANCE_PIERCE]",
    "[ACTIVESTATUS_TOLERANCE_STUCK]", "[ACTIVESTATUS_TOLERANCE_CONFUSE]",
    "[ACTIVESTATUS_TOLERANCE_HOLD]", "[ACTIVESTATUS_TOLERANCE_ALL]",
    "[INVENTORY_MAX_WEIGHT]", "[HP_REGENRATE]", "[MP_REGENRATE]",
    "[MP_REGENRATE_PERCENT_UP]", "[MOVE_SPEED]", "[ATTACK_SPEED]",
    "[CAST_SPEED]", "[HIT_RECOVERY]", "[JUMP_POWER]", "[CRITICAL]",
    "[SKILL_LEVEL]", "[PHYSICAL ABSOLUTE DEFENSE]",
    "[MAGICAL ABSOLUTE DEFENSE]", "[STUCK ON ATTACK]"
};

// ===========================================================================
// importRecommendNewbieOptionScript —— ORIG T 0x0890457c
// （docs/class_func_reports/_lt_global_gt_/importRecommendNewbieOptionScript.md）
// 双阶段（SpinLock 双检 + static lookup）：
//   "[character job]"0x1267 / "[newbie recommend option end]"0x1405。
// [character job] 块内 ScanStr(jobName)→getCharacterJob（0..10 有效）→
// [grow type] 子块：ScanStr(growName) 按 7 组名映射 m_growType=0..5 →
// [grade] 1/2/3 → [avatar index set] 9 槽 ScanStr 查 AVTVARIATION_TAG_NAME
// 写 m_jobValue[槽*3+grade-1]，槽 4/5 命中索引 43 时额外 ScanInt 写
// m_grow[槽][grade-1]；[/grow type] 时 param_2[job].push_back(opt)。
// loadRDARScriptFile 失败返 0，成功返 1。
// ===========================================================================
// [b28d-solo-OnlyOrigBatch12] P167 CLEAR（118=118）复验通过：按 ORIG 分界修
// MISSING String::compare×40 / EXTRA operator==×40——token/tokenGrade/tokenAv
// 用 operator==（Ghidra `std::operator==(&token,...)` 六处）、growName/avName
// 用 compare；EXTRA insert×1 改共享 0x8905606 调用点。结构修复：ScanType 失败块
// goto+`return !running` 合并（0x890568c）、job 两独立守卫（P133）。
// 【本波翻正 + 真语义 bug】off 派发链原三独立 if 被 4.4 折成
// `cmpl $imm,-0x50(%ebp)` 内存直比；改 `switch(off){ case 0: case 0x1267: … }`
// （P163 补 case 0 撑宽 range + P261/P286 寄存器比较链）后逐条复现 ORIG
// `mov -0x50(%ebp),%eax; cmp $0x1267,%eax; je; cmp $0x1405,%eax; je;
// test %eax,%eax; jne 主尾`（0x0890480d）。**真 bug**：原实现 case 0
// （注册轮 off=0）不落入 job body ⇒ 两条 insert（"[character job]"/
// "[newbie recommend option end]"）不可达、__lookup 恒空、job 分支永不命中；
// 补 case 0 fall-through（0890482a）后与 ORIG 一致。
// 残余 N82/P112/N7 封顶：string 组槽浅 4（-0x8c vs -0x90）、running 浅 8
// （-0x49 vs -0x51）、返回汇聚 %ebx vs ORIG %esi、析构链保存寄存器 %esi/%ebx
// （N7；c6446r 复测仍 %ebx 且 TU 总账更差 diff 5→7/missing 121→126 ⇒ 不路由）；
// gap 7 条、帧 0x19c 与槽位集一致。
// ORIG 0x0890457c
unsigned int importRecommendNewbieOptionScript(
    const char* path, tagRecommendNewbieOptionTable& out)
{
    // [round5 收口实测] ndiff=551（1138/1133，gap=-5）：p167 118=118 且差集仅
    // C1↔C2/D1↔D2 别名对（P93 伪差）⇒ 无语义缺口。残余为槽位族：string 组浅 4
    // （-0x8c vs -0x90）、running/标志浅 8（-0x49 vs -0x51）、若干语句序（P112/N82/
    // N93 + P75 装载序），与既有判词一致 ⇒ 维持 N 封顶，本轮无更优源形。
    if (!loadRDARScriptFile("", path))                      // 890458f movl $""（空基目录实参）
        return 0;                                           // 89045a2 esi=0
    out.clear();                                            // 89045b2

    // ORIG 帧 0x19c：五连函数级 string ctor 89045b7..45fc、出口逆序 dtor 链
    // 56b2..5761；jobName(-0xa8)/growName(-0xac) block 作用域每轮 ctor/dtor；
    // opt(-0x194) 每轮 grow 层构造、全文无 D1 调用点（隐式 trivial ⇒ 头删用户
    // 析构，P12 证据 import@0x0890457c 段内无 0x0893908c）。
    std::string token;          // -0x90
    std::string tokenGrow;      // -0x94
    std::string tokenAv;        // -0x98
    std::string avName;         // -0x9c
    std::string tokenGrade;     // -0xa0
    int grade;                  // -0xb0
    int gv;                     // -0xb4 slot4 [SKILL_LEVEL] 追加值
    int gv2;                    // -0xb8 slot6 追加值
    int off;                    // -0x50
    bool running = true;        // -0x51（movb $1,-0x51 @89045fd）
    int job;                    // -0x4c
    int growType;               // -0x44
    bool inJob;                 // -0x45
    bool inGrade;               // -0x3e
    bool inAvatar;              // -0x3d
    int avNameIdx0, avNameIdx1, avNameIdx2, avNameIdx3, avNameIdx4;   // -0x3c..-0x2c
    int avNameIdx5, avNameIdx6, avNameIdx7, avNameIdx8;               // -0x28..-0x1c

read_token:                                                 // 8904606（主尾 8905680 进）
    if (!ScanType(token, true))                             // 8904617
        goto scan_type_fail;                                // 8904621 jne 890568c

read_guard:                                                 // 8904627
    // ORIG static 声明位于循环区域内（两 __cxa_guard 每轮重执行 @8904627/89046a1）。
    static std::map<const char*, int, less_TCHAR> __lookup; // guard bss 0x094f7198
    static SpinLock __init_lock;                            // guard bss 0x094f71a0
    static bool __inited = false;

    if (__inited)                                           // 890471b movzbl; test; je 89047dd
    {
        // 890472a..89047db：c_str→find→!=end 汇聚 %eax 单点 store（三目形）
        std::map<const char*, int, less_TCHAR>::const_iterator it = __lookup.find(token.c_str());
        off = (it != __lookup.end()) ? it->second : -1;
    }
    else
    {
        __init_lock.enter();                                // 89047e4 SpinLock::enter()
        if (__inited)
        {
            __init_lock.leave();                            // 89047fb
            goto read_guard;                                // 8904801→8904627
        }
        off = 0;                                            // 8904806（注册轮直落派发链）
    }

    // 三相线性派发（890480d，P46/P163/P261 形）：ORIG 为寄存器比较链
    // `mov -0x50,%eax; cmp $0x1267,%eax; je; cmp $0x1405,%eax; je;
    // test %eax,%eax; jne 主尾` ⇒ 源形是 switch(off)，且 case 0 与 0x1267
    // 共享 body（注册轮 off=0 fall-through 进 job case 头，补 case 0 撑宽
    // range 并复现 `test %eax,%eax`）。原三独立 if 让 4.4 折成内存直比
    // `cmpl $imm,-0x50(%ebp)`（P286 差 5 条）且丢失 off==0 的注册路径。
    switch (off)
    {
    case 0:
    case 0x1267:
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[character job]", 0x1267));
            // 8904876 jmp 8905606（end case 头）：第二张 insert 与 0x1405 分支
            // **共享同一调用点**（P167 EXTRA insert×1 的根因），故用 goto 汇入。
            goto register_end_option;
        }
        {
            std::string jobName;                            // 890487b 每轮 ctor / 89055e9 dtor
            job = -1;                                       // 8904889
            // ORIG 8904899..89048d9：ScanStr 失败与 job 越界各自直落 ebx=0 主尾，
            // 无汇聚 bool 槽（xor/test/je + js/jle 直跳形）。
            if (!ScanStr(&jobName))                         // 8904899
                goto main_loop_end;                         // 89048a5 ebx=0
            job = getCharacterJob(jobName.c_str());         // 89048b8/89048c0
            // ORIG 0x89048cc `js fail` + 0x89048ce `jle pass` 两独立守卫（P133/P223）：
            // 合并 || 令 4.4 产 jns 反转布局。
            if (job < 0)
                goto main_loop_end;                         // 89048d4 ebx=0
            if (job > 10)
                goto main_loop_end;
            inJob = true;                                   // 89048de movb $1,-0x45
            while (inJob)
            {
                if (!ScanType(tokenGrow, true))             // 89048f8 fail→ebx=2→jobName dtor→主尾
                    break;
                tagRecommendNewbieOption opt;               // 8904908 每轮 C1（无 D1）
                if (tokenGrow == "[grow type]")             // 8904916
                {
                    opt.clear();                            // 890493d
                    growType = -1;                          // 8904942
                    std::string growName;                   // 8904949 每轮 ctor
                    if (!ScanStr(&growName))                // 8904952
                        goto main_loop_end;                 // ebx=0：双层 dtor→主尾
                    if (growName.compare("ranger") == 0 ||
                       growName.compare("nenmaster") == 0 ||
                       growName.compare("soulbringer") == 0 ||
                       growName.compare("rogue_dagger") == 0 ||
                       growName.compare("elementalmaster") == 0 ||
                       growName.compare("crusader_buff") == 0 ||
                       growName.compare("elementalbomber") == 0)
                        growType = 0;
                    else if (growName.compare("mechanic") == 0 ||
                       growName.compare("striker") == 0 ||
                       growName.compare("berserker") == 0 ||
                       growName.compare("rogue_dual") == 0 ||
                       growName.compare("summoner") == 0 ||
                       growName.compare("crusader_battle") == 0 ||
                       growName.compare("glacialmaster") == 0)
                        growType = 1;
                    else if (growName.compare("launcher") == 0 ||
                       growName.compare("streetfighter") == 0 ||
                       growName.compare("asura") == 0 ||
                       growName.compare("necromencer") == 0 ||
                       growName.compare("witch") == 0 ||
                       growName.compare("infighter") == 0)
                        growType = 2;
                    else if (growName.compare("spitfire_physical") == 0 ||
                       growName.compare("grappler") == 0 ||
                       growName.compare("weaponmaster") == 0 ||
                       growName.compare("battlemage_physical") == 0 ||
                       growName.compare("exorcist_physical") == 0)
                        growType = 3;
                    else if (growName.compare("spitfire_magical") == 0 ||
                       growName.compare("battlemage_magical") == 0 ||
                       growName.compare("exorcist_magical") == 0 ||
                       growName.compare("demonic swordman") == 0)
                        growType = 4;
                    else if (growName.compare("avenger") == 0 ||
                       growName.compare("creator mage") == 0)
                        growType = 5;
                    opt.m_growType = growType;              // 8904d50
                    inGrade = true;                         // 8904d59
                    while (inGrade)
                    {
                        if (!ScanType(tokenGrade, true))    // 8904d73 fail→ebx=2→grow 层继续
                            break;
                        if (tokenGrade == "[grade]")        // 8904d83
                        {
                            grade = -1;                     // 8904da1
                            if (!ScanInt(&grade))           // 8904db4 ScanInt(int*)
                                goto ret_one;               // ebx=1 → return 1
                            inAvatar = true;                // 8904dcf
                            while (inAvatar)
                            {
                                if (!ScanType(tokenAv, true))   // 8904de9 fail→grade 继续
                                    break;
                                if (tokenAv == "[avatar index set]")   // 8904df9
                                {
                                    if (!ScanStr(&avName))
                                        goto ret_one;
                                    avNameIdx0 = 0;
                                    while (avNameIdx0 <= 0x2e)
                                    {
                                        if (avName.compare(AVTVARIATION_TAG_NAME[avNameIdx0]) == 0)
                                        {
                                            if (grade == 1)
                                                opt.m_jobValue[0] = avNameIdx0;
                                            else if (grade == 2)
                                                opt.m_jobValue[1] = avNameIdx0;
                                            else if (grade == 3)
                                                opt.m_jobValue[2] = avNameIdx0;
                                        }
                                        ++avNameIdx0;
                                    }
                                    if (!ScanStr(&avName))
                                        goto ret_one;
                                    avNameIdx1 = 0;
                                    while (avNameIdx1 <= 0x2e)
                                    {
                                        if (avName.compare(AVTVARIATION_TAG_NAME[avNameIdx1]) == 0)
                                        {
                                            if (grade == 1)
                                                opt.m_jobValue[3] = avNameIdx1;
                                            else if (grade == 2)
                                                opt.m_jobValue[4] = avNameIdx1;
                                            else if (grade == 3)
                                                opt.m_jobValue[5] = avNameIdx1;
                                        }
                                        ++avNameIdx1;
                                    }
                                    if (!ScanStr(&avName))
                                        goto ret_one;
                                    avNameIdx2 = 0;
                                    while (avNameIdx2 <= 0x2e)
                                    {
                                        if (avName.compare(AVTVARIATION_TAG_NAME[avNameIdx2]) == 0)
                                        {
                                            if (grade == 1)
                                                opt.m_jobValue[6] = avNameIdx2;
                                            else if (grade == 2)
                                                opt.m_jobValue[7] = avNameIdx2;
                                            else if (grade == 3)
                                                opt.m_jobValue[8] = avNameIdx2;
                                        }
                                        ++avNameIdx2;
                                    }
                                    if (!ScanStr(&avName))
                                        goto ret_one;
                                    avNameIdx3 = 0;
                                    while (avNameIdx3 <= 0x2e)
                                    {
                                        if (avName.compare(AVTVARIATION_TAG_NAME[avNameIdx3]) == 0)
                                        {
                                            if (grade == 1)
                                                opt.m_jobValue[9] = avNameIdx3;
                                            else if (grade == 2)
                                                opt.m_jobValue[10] = avNameIdx3;
                                            else if (grade == 3)
                                                opt.m_jobValue[11] = avNameIdx3;
                                        }
                                        ++avNameIdx3;
                                    }
                                    if (!ScanStr(&avName))
                                        goto ret_one;
                                    avNameIdx4 = 0;
                                    while (avNameIdx4 <= 0x2e)
                                    {
                                        if (avName.compare(AVTVARIATION_TAG_NAME[avNameIdx4]) == 0 &&
                                            avNameIdx4 == 0x2b)
                                        {
                                            if (grade == 1)
                                                opt.m_jobValue[12] = avNameIdx4;
                                            else if (grade == 2)
                                                opt.m_jobValue[13] = avNameIdx4;
                                            else if (grade == 3)
                                                opt.m_jobValue[14] = avNameIdx4;
                                            gv = -1;
                                            if (!ScanInt(&gv))
                                                goto ret_one;
                                            if (grade == 1)
                                                opt.m_grow[4][0] = gv;
                                            else if (grade == 2)
                                                opt.m_grow[4][1] = gv;
                                            else if (grade == 3)
                                                opt.m_grow[4][2] = gv;
                                        }
                                        ++avNameIdx4;
                                    }
                                    if (!ScanStr(&avName))
                                        goto ret_one;
                                    avNameIdx5 = 0;
                                    while (avNameIdx5 <= 0x2e)
                                    {
                                        if (avName.compare(AVTVARIATION_TAG_NAME[avNameIdx5]) == 0)
                                        {
                                            if (grade == 1)
                                                opt.m_jobValue[15] = avNameIdx5;
                                            else if (grade == 2)
                                                opt.m_jobValue[16] = avNameIdx5;
                                            else if (grade == 3)
                                                opt.m_jobValue[17] = avNameIdx5;
                                        }
                                        ++avNameIdx5;
                                    }
                                    if (!ScanStr(&avName))
                                        goto ret_one;
                                    avNameIdx6 = 0;
                                    while (avNameIdx6 <= 0x2e)
                                    {
                                        if (avName.compare(AVTVARIATION_TAG_NAME[avNameIdx6]) == 0)
                                        {
                                            if (grade == 1)
                                                opt.m_jobValue[18] = avNameIdx6;
                                            else if (grade == 2)
                                                opt.m_jobValue[19] = avNameIdx6;
                                            else if (grade == 3)
                                                opt.m_jobValue[20] = avNameIdx6;
                                            if (avNameIdx6 == 0x2b)
                                            {
                                                gv2 = -1;
                                                if (!ScanInt(&gv2))
                                                    goto ret_one;
                                                if (grade == 1)
                                                    opt.m_grow[6][0] = gv2;
                                                else if (grade == 2)
                                                    opt.m_grow[6][1] = gv2;
                                                else if (grade == 3)
                                                    opt.m_grow[6][2] = gv2;
                                            }
                                        }
                                        ++avNameIdx6;
                                    }
                                    if (!ScanStr(&avName))
                                        goto ret_one;
                                    avNameIdx7 = 0;
                                    while (avNameIdx7 <= 0x2e)
                                    {
                                        if (avName.compare(AVTVARIATION_TAG_NAME[avNameIdx7]) == 0)
                                        {
                                            if (grade == 1)
                                                opt.m_jobValue[21] = avNameIdx7;
                                            else if (grade == 2)
                                                opt.m_jobValue[22] = avNameIdx7;
                                            else if (grade == 3)
                                                opt.m_jobValue[23] = avNameIdx7;
                                        }
                                        ++avNameIdx7;
                                    }
                                    if (!ScanStr(&avName))
                                        goto ret_one;
                                    avNameIdx8 = 0;
                                    while (avNameIdx8 <= 0x2e)
                                    {
                                        if (avName.compare(AVTVARIATION_TAG_NAME[avNameIdx8]) == 0)
                                        {
                                            if (grade == 1)
                                                opt.m_jobValue[24] = avNameIdx8;
                                            else if (grade == 2)
                                                opt.m_jobValue[25] = avNameIdx8;
                                            else if (grade == 3)
                                                opt.m_jobValue[26] = avNameIdx8;
                                        }
                                        ++avNameIdx8;
                                    }
                                }
                                else if (tokenAv == "[/avatar index set]")   // 89054dd
                                    inAvatar = false;       // 89054f7
                            }
                            continue;                       // avatar 完 → grade 条件
                        }
                        if (tokenGrade == "[/grow type]")   // 8905507
                        {
                            inGrade = false;                // 8905521
                            if (job >= 0)                   // 8905525 js → skip
                                out[job].push_back(opt);    // 8905538 op[] + 890554a push_back
                        }
                    }
                    continue;                               // grade 完 → grow 层条件 89055ba
                }
                if (tokenGrow == "[/character job]")        // 890559c（未命中 grow 才判）
                    inJob = false;                          // 89055b6
            }
        }
        goto main_loop_end;                                 // 89055c7 ebx=2 → jobName dtor → 主尾

    case 0x1405:
register_end_option:                                        // 8905606（case 0/0x1267 注册路径 jmp 至此重判）
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type(
                "[newbie recommend option end]", 0x1405));   // 8905606..8905652
        }
        else
        {
            running = false;                                // 8905654 movb $0,-0x51
        }
        goto main_loop_end;

    default:
        break;
    }

main_loop_end:                                              // 8905659
    if (!__inited)
    {
        __init_lock.leave();                                // 890566e SpinLock::leave()
        __inited = true;                                    // 8905673 movb $1
        goto read_guard;                                    // 890567a→8904627（同 token 重跑）
    }
    if (running)
        goto read_token;                                    // 8905684 jne 8904606
scan_type_fail:                                             // 890568c
    // ORIG 0x890568c：movzbl -0x51; mov $1,%esi; xor %eax,%esi ⇒ return !running
    // （失败块与主尾 fall-through 共享同一 return，P158 bool xor 物化）。
    return !running;

ret_one:                                                    // ebx=1 链 → return 1
    return 1;
}
