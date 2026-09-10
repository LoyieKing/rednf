// ============================================================================
// df_game_r 还原 —— STMonsterScript 类方法（G5 怪物脚本表）。
// 逐函数依据 ORIG objdump 转写（地址随注）：
//   Clear T 0x08a398aa / C1 W 0x08368e14 / D1 W 0x08378dac / op= W 0x08377994
//   ImportMonsterBaseTable T 0x08a3c736（全量转写）
//   getMonsterTableIndex T 0x088ba1c8 / getRandomChampionElement T 0x088b9cce
//   setBasicAbilityByLevel T 0x088b9603（全量转写）/ isNamedMonster W 0x088ba18c
//   ImportMonsterScript T 0x08a12827 / exportMonsterScript T 0x08a22520
// [b28d-23 N·N90] STMonsterScript::operator= W 0x08377994 为编译器隐式拷贝
// （1200/1272，ORIG 大 dword 成员分块启发式跨版本差）⇒ N。
// [b29-SweepSTMonsterScript2] ①ImportMonsterScript 五处 sound 字段补
//   `if (!ScanStr(&token)) return 0;` + `script->m_xxx = token`（ORIG 0x8a13b44..
//   0x8a13dee 五处 `_ZNSsaSERKSs` 目标 +0x6ac/+0x6a8/+0x6a4/+0x69c/+0x6a0）⇒
//   p167 该符号差集 -5→0、OUR calls 1700→1705；②MonsterBaseParameter::clear 与
//   ImportMonsterBaseTable 改二维 m_grid 视图 ⇒ 双件 DIFF→AE（旧 N·P30 判词作废）。
// 四数（check_tu_routed fresh）：803/1909/2/11/508 → 803/1911/2/9/508（零回退）。
//   （两者 >30KB 汇编：骨架+标签分派级复原，覆盖面见各函数注记）
// ============================================================================

#include "STMonsterScript.h"
#include "NpcScript.h"        // STScriptFileList::FindIndexByFullScan

#include <stdio.h>
#include <string.h>

#include "DNFLexWrapper.h"   // loadRDARScriptFile/getDirectoryPath/ScanType/ScanInt
#include "STMapScript.h"     // ENUM_DRAWLAYER + getDrawLayerName 声明

#include "GameEnums.h"       // ENUM_CATEGORY
#include "RDARScriptStringManager.h"  // g_scriptStringManager_/getTagByName
#include "SyncScriptDeps.h"           // toTString
#include "CreatureScript.h"           // getElementName(ENUM_ELEMENT)
#include "GameScriptEnumHelpers.h"    // getCategoryToEnumValue（warp [category of target]）

#include "CEnvironment.h"             // class SpinLock / struct less_TCHAR（ImportMonsterScript 18 张函数局部静态查找表）

int ScanStrOrInt(bool& isStr, std::string& value, bool& isInt,
                 bool consume);                                      // T 0x088bd0d5

// ---- 跨 TU 符号（ORIG 地址随注；定义在各自所属批次 TU）----
extern char g_szMonsterScriptBaseDirectory[0x100];              // B 0x094ff160
extern std::map<int, std::string> g_stMonsterScriptFileList;    // B 0x094ff260
extern std::vector<std::vector<MonsterBaseParameter> >
    g_monsterBaseParameters;                                    // B 0x094ff278
int GetLinearDataForMonsterAbility(int low, int high, int diff,
                                   int span);                   // T 0x08891bae
bool processModuleTagSet(int& moduleId, int& subId, std::string* tag,
                         int type, bool flag);                  // T 0x08a97637
void getScriptFileFullPath(char* out, unsigned int size,
                           const char* dir, const char* name);  // T 0x088bba4e
bool isValidExtension(const std::string& str, const std::string& ext);  // T 0x08a3c67d（OnlyOrigBatch12.cpp）
std::string getObjectCategoryString(ENUM_CATEGORY cat);         // T 0x08a3ca81（ScriptDepsA.cpp）
int getObjectCategory(std::string name);                        // T 0x08a3ca1b（ScriptDepsA.cpp）
void convertBinaryToText(const char* path);                     // T 0x088be04c（STMapScript.cpp）
bool isExistAiPattern(STAIInfoScript* info, int bucket);        // T 0x08a2232f（CharacterAIA2.cpp）
void exportAIPatternPath(FILE* f, STAIInfoScript* info, int bucket);  // T 0x08a223b0 (CharacterAIA2.cpp)
bool importAIInfoScript(STAIInfoScript* info, std::string* tag,
                        bool byLevel);                              // T 0x08a390db
extern "C" const char* ACTIVEOBJECT_STATE_STRING[16];           // D 0x093f4d80（CharacterAIA2.cpp）
extern "C" const char* ACTIVESTATUS_ATTACK_TAG[20];             // D 0x093f54e0（CharacterAIA2.cpp）

// ===========================================================================
// Clear —— ORIG T 0x08a398aa（逐条转写，顺序保持汇编原序）
// ===========================================================================
void STMonsterScript::Clear()
{
    // 循环变量声明风格经骨架实验校准：i/j 为 decl-init+for-init 双写模式，
    // k/l/m/n 为块作用域单槽（与 ORIG 槽位 -0x20/-0x1c/-0x18/-0x14/-0x10/-0xc 一致）。
    m_flagc50 = false;                       // movb $0,0xc50
    m_flag3a8 = false;                       // movb $0,0x3a8
    m_aiInfo.clear();                        // call STAIInfoScript::clear(this+0x138)
    m_flag33c = true;                        // movb $1,0x33c
    m_flag368 = true;                        // movb $1,0x368
    m_field36c = 0;
    m_field370 = 0;
    m_flag374 = false;
    m_field378 = 0;
    m_field37c = 0;
    m_field380 = 0;
    m_index = 0;                             // movl $0,(%eax)
    m_name = "";                             // call Ss::operator=(PKc)
    m_altName = "";
    m_width = 40;                            // movl $0x28,0xc
    m_height = 20;                           // movl $0x14,0x10
    m_floatingHeight = 0;
    m_layer = 1;
    m_field01c.clear();                      // vector<pair> clear(+0x1c)
    int i = 0;
    for (i = 0; i <= 3; ++i)                 // 首循环：decl+for-init 双写
        m_field028[i].clear();               // vector<pair>[4] 元素 0xc 步进
    m_parameterCategories.clear();           // call(+0x7c)
    m_flag3b8 = true;                        // movb $1,0x3b8
    m_abilityTable = 0;                      // +0x3bc
    m_field3c0 = 0;                          // +0x3c0
    for (i = 0; i <= 9; ++i)
        m_field3c4[i] = 0;                   // l[0x3c4+4i]=0
    for (i = 0; i <= 1; ++i)
    {
        int j = 0;
        for (j = 0; j <= 3; ++j)             // 单一二维表复合寻址（实证）
            m_resistTable[j][i] = 0;         // [0x3f4+8j+4i]=0
        for (j = 0; j <= 16; ++j)
            m_linearAbilities[j][i] = 0;     // [0x414+8j+4i]=0
        m_hpRegenSpeed[i] = 0;               // [0x49c+4i]
        m_mpRegenSpeed[i] = 0;               // [0x4a4+4i]
        m_moveSpeed[i] = 0;                  // [0x4ac+4i]
        m_attackSpeed[i] = 0;                // [0x4b4+4i]
        m_castSpeed[i] = 0;                  // [0x4bc+4i]
        m_hitRecovery[i] = 0;                // [0x4c4+4i]
        m_jumpPower[i] = 0;                  // [0x4cc+4i]
        m_jumpSpeed[i] = 100;                // movl $0x64,[0x4d4+4i]（无分支实证）
        m_normalDashSpeed[i] = 0;            // [0x4dc+4i]
        m_slantDashSpeed[i] = 0;             // [0x4e4+4i]
        m_weight[i] = 0;                     // [0x4ec+4i]
        m_stuckBonusOnDamage[i][0] = 0;      // scale8 +0x4f4
        m_stuckBonusOnDamage[i][1] = 0;      // scale8 +0x4f8
        for (j = 0; j <= 3; ++j)
            m_paramTable[j][i] = 0;          // [0x508+8j+4i]=0
        m_field528[i] = 0;                   // [0x528+4i]
        m_levelByDiff[i] = 0;                // [0x3ec+4i]
    }
    m_sight = 200;                           // movl $0xc8,0x504
    m_comeInAction = "";
    m_comeInActionEx = "";
    m_field538 = 100;
    m_field53c = 200;
    m_field540 = -1.0f;
    m_field544 = -1.0f;
    m_field548 = false;
    m_field54c = 20.0f;
    m_field550 = -300.0f;
    m_field554.clear();
    m_field560.clear();
    m_field56c = false;
    m_targetingTimeTerm = 1000;              // +0x574
    m_targetingOnAttack = false;             // +0x580
    m_targetingOnDamage = false;             // +0x581
    m_targetingLevel = 0;                    // +0x584
    m_targetingHp = 0;                       // +0x588
    m_field58c = 0;                          // +0x58c
    m_targetingNearest = false;              // +0x590
    m_targetingAttacker = false;             // +0x591
    m_targetingFromDirection = false;        // +0x592
    m_targetingLowPhysAttack = 0;            // +0x594
    m_targetingLowPhysDefense = 0;           // +0x598
    m_targetingLowMagAttack = 0;             // +0x59c
    m_targetingLowMagDefense = 0;            // +0x5a0
    m_targetingDowned = false;               // +0x5a4
    m_warlike = 50;                          // movl $0x32,0x578（序在 5a4 后，忠实 ORIG）
    m_attackDelay = 1000;                    // movl $0x3e8,0x57c
    m_superarmorOnAttack = false;            // movb $0,0x5a5
    m_field5a8 = 0;
    m_faceImage = "";                        // assign(+0x5ac)
    m_field5b0 = 0;                          // movl $0,0x5b0
    m_waitingMotion = "";                        // 三连展开（[2..4] 不清，实证）
    m_moveMotion = "";
    m_sitMotion = "";
    for (i = 0; i <= 1; ++i)
        m_damageMotion[i] = "";                  // 循环 ×2 @0x5cc
    m_downMotion = "";                      // 四连展开
    m_overturnMotion = "";
    m_jumpMotion = "";
    m_jumpAttackMotion = "";
    m_attackMotions.clear();
    m_etcMotions.clear();
    m_jumpAttackInfo = "";                 // assign(+0x6b4)
    m_attackInfos.clear();
    m_etcAttackInfos.clear();
    m_intData.clear();
    m_keepDistance = -1;                     // movl $<A>,0x6e0
    m_isOnTheObject = false;                 // movb $0,0x6e4
    m_attackMethods.clear();                 // call(+0x6e8)
    m_waitingAction = "";                      // 六连展开 @0x5fc（实证非循环）
    m_moveAction = "";
    m_movebackAction = "";
    m_dashAction = "";
    m_dashattackAction = "";
    m_sitAction = "";
    for (i = 0; i <= 1; ++i)
        m_damageAction[i] = "";                  // 循环 ×2 @0x614
    m_downAction = "";                      // 四连展开 @0x61c
    m_overturnAction = "";
    m_jumpAction = "";
    m_jumpAttackAction = "";
    m_attackActions.clear();
    m_etcActions.clear();
    m_lastAction = "";                      // assign(+0x644)
    m_procAction = "";                      // assign(+0x648)
    m_patterns.clear();                      // vector<vector<int>> (+0x688)
    m_monsterTitle = "";                    // assign(+0x694)
    m_basicAction = "";
    m_damageSound = "";
    m_dieSound = "";
    m_appearSound = "";
    m_attackSound = "";
    m_ambientSound = "";
    m_etcSound = "";                       // assign(+0x6b0)
    m_field6f4 = 0;                          // +0x6f4
    m_field6f8 = 0;                          // +0x6f8
    m_dieEffect[0] = -1;                     // movl $<A>,0x6fc（{-1,0,0} 实证）
    m_dieEffect[1] = 0;                      // movl $0,0x700
    m_dieEffect[2] = 0;                      // movl $0,0x704
    m_field708 = "";                         // assign(+0x708)
    for (int k = 0; k < 111; ++k)            // clear_vs(this+12k+0x70c)
        m_field70c[k].clear();
    m_championElements.clear();              // call(+0xc40)
    m_fieldc4c = 0;                          // +0xc4c
    m_field088 = 0;                          // +0x88
    m_field08c.clear();                      // vector<pair>(+0x8c)
    m_fieldc54 = 0;                          // mov $0,%edx 中转形态
    m_fieldc58 = 0;
    m_field058.clear();                      // vector<pair>(+0x58)
    for (int l = 0; l <= 4; ++l)
        m_fieldc5c[l] = 0;                   // [0xc5c+4l]=0
    m_independentDrops.clear();              // call(+0x3ac)
    m_flagc70 = false;
    m_flagc71 = false;
    m_fieldc74 = 0;
    m_fieldc78 = 0;
    m_fieldc7c = 0;
    m_fieldc80.clear();
    m_fieldc8c.clear();
    m_dropAvatarInfo.clear();                // call stEnamyDropAvatarInfo_t::clear(+0xc98)
    m_flagcd4 = false;                       // movb $0,0xcd4
    m_field1c9c = 3;                         // movl $0x3,0x1c9c
    m_field1ca0 = 0;                         // movl $0x0,0x1ca0
    m_field1ca4 = 0;                         // mov $0,%edx 中转形态
    for (int m = 0; m < 200; ++m)            // difficultyScales 全表 -1.0f
        for (int n = 0; n <= 4; ++n)
            m_difficultyScales[m][n] = -1.0f;  // mov $0xbf800000（ebx 承载 n）
    m_field1c94 = 0;                         // +0x1c94
    m_field1c98 = 0;                         // +0x1c98
    m_field1ca8 = false;                     // movb $0,0x1ca8
    m_catchItems.clear();                    // call(+0x70)
    m_field1cac = -1;                        // movl $<A>,0x1cac
    m_field1cb0 = -1;                        // movl $<A>,0x1cb0
    m_field1cb4.clear();                     // call(+0x1cb4)
    m_mountingAction = "";                      // assign(+0x64c)
    m_mountedAction = "";                      // assign(+0x650)
    m_unmountingAction = "";                      // assign(+0x654)
    m_field658 = -1;                         // movl $<A>,0x658
    m_field65c = 0;
    m_field660 = 0;
    m_field664 = 0;
    m_field668 = 0;
    m_field66c = 0;
    m_field670 = 0;
    m_field674 = false;
    m_field678 = 255;                        // movl $0xff,0x678
    m_field67c = 255;
    m_field680 = false;
    m_field684 = 4;                          // movl $0x4,0x684
    m_fieldcd8 = 4;                          // movl $0x4,0xcd8
    m_field120.clear();                      // call(+0x120)
    m_baseParameters.clear();                // call(+0x12c)
    m_field1cc0 = 0;                         // movl $0,0x1cc0
}

// ===========================================================================
// ctor / dtor / operator=
// ===========================================================================
// ORIG C1 W 0x08368e14：类类型成员逐个默认构造（POD 标量区不动），末尾统一
// call Clear()。此处编译器默认构造 + Clear() 与之语义一致。
STMonsterScript::STMonsterScript()
{
    Clear();
}

// ORIG D1 W 0x08378dac：成员按声明逆序自动析构，无附加逻辑。
STMonsterScript::~STMonsterScript() {}

// ORIG op= W 0x08377994 为编译器合成版（无自赋值检查、rep movsl 分组拷贝、
// 尾部 eax=this），头文件中以 = default 声明，由编译器按成员序生成。

// ---- STMonsterEquipment 成员（ORIG W 0x08368d92/0x08368d7e）----
STMonsterEquipment::STMonsterEquipment()
{
    clear();                       // ORIG C1：vector C1 后调 clear（冗余复位忠实）
}

void STMonsterEquipment::clear()
{
    m_items.clear();               // ORIG clear 即 vector<string>::clear
}

// ---- stEnamyDropAvatarInfo_t 成员（ORIG W 0x088cc10c / op= W 0x08366ea8）----
// [b28d-solo-STMonsterScript] 修法（P30 反例解 / P291 具名数组成员视图）：第二循环索引
// `m_field08[i+4]` 在 4.4.6 恒折 `mov i,%eax; lea 0x4(%eax),%r`（this 装载被排到 lea 后），
// 与 ORIG 的 `mov i,%edx; mov this,%eax; lea 0x4(%edx),%ecx` 反序。改走头内匿名 union 的
// 二维视角 `m_field08v[1][i]`（行宽 4 ⇒ 行偏移 add/lea $0x4）后，clear 第二循环与 ORIG
// 逐条全等（探针 /tmp/solo_probe1.cpp clear_v2，4.4.6）。i=4 时 [1][4] 越界至 +0x28 第 9 个
// float —— ORIG 原码即如此写（P171 照抄），内存仍在 union 36B 内。
// ORIG 0x088cc10c
void stEnamyDropAvatarInfo_t::clear()
{
    m_field00 = 0;                 // movl $0,(%eax)
    m_field04 = 0;                 // movl $0,0x4
    for (int i = 0; i <= 3; ++i)
        m_field08v[0][i] = 0;      // [base+8+4i]
    for (int i = 0; i <= 4; ++i)   // [base+8+4(i+4)]：ORIG `lea 0x4(%edx)` 形
        m_field08v[1][i] = 0;
    m_field2c = 0;
    m_elements.clear();            // vector<element>::clear(+0x30)
}

// [b28d-solo-STMonsterScript] 修法同 clear：ORIG 对两处 a[i+4] 就地 `mov i,%reg;
// add $0x4,%reg`（4.1 复用 i 所在寄存器），4.4 一维形恒 `lea`；二维视角 `m_field08v[1][i]`
// 逐条复现 ORIG add 形（探针 /tmp/solo_probe1.cpp op_v2，4.4.6）。
// ORIG 0x08366ea8
void stEnamyDropAvatarInfo_t::operator=(const stEnamyDropAvatarInfo_t& other)
{
    m_field00 = other.m_field00;
    m_field04 = other.m_field04;
    for (int i = 0; i <= 3; ++i)
        m_field08v[0][i] = other.m_field08v[0][i];
    for (int i = 0; i <= 4; ++i)
        m_field08v[1][i] = other.m_field08v[1][i];
    m_field2c = other.m_field2c;
    m_elements = other.m_elements;   // vector op=
}

// ===========================================================================
// isNamedMonster —— ORIG W 0x088ba18c（全量转写）
// ===========================================================================
bool isNamedMonster(int monsterIndex)
{
    if (monsterIndex > 0xc34f && monsterIndex <= 0xc738)
        return true;
    if (monsterIndex > 0xdabf && monsterIndex <= 0xea60)
        return true;
    return false;
}

// ===========================================================================
// getMonsterTableIndex —— ORIG T 0x088ba1c8（全量转写）
// ===========================================================================
int getMonsterTableIndex(const STMonsterScript* script, int dungeonIdx,
                         int roleType, bool flagA, bool flagB)
{
    int tableIdx = 0;
    if (script->m_abilityTable == 1)          // +0x3bc == 1 → idx 3
        tableIdx = 3;
    if (flagA)
        tableIdx = 3;
    if (roleType == 3)                        // 第 3 参（[ebp+0x10]）== 3 → idx 2
        tableIdx = 2;
    if (isNamedMonster(script->m_index))      // 命名怪 → idx 1
        tableIdx = 1;
    // ORIG +0x228..0x240：b≤1999 时查 d；b∈(1999,2002] 置 4（短路或形）
    if ((dungeonIdx > 1999 && dungeonIdx <= 2002) || flagB)
        tableIdx = 4;
    return tableIdx;
}

// ===========================================================================
// getRandomChampionElement —— ORIG T 0x088b9cce（全量转写）
// ===========================================================================
int getRandomChampionElement(const STMonsterScript* script, int seed)
{
    int result = 4;
    if (script == 0)
        return result;                         // 空指针守卫
    const int count =
        static_cast<int>(script->m_championElements.size());    // +0xc40
    srand(seed);                               // 全局种子污染与 ORIG 一致
    if (count == 0)
        result = rand() % 4 + 1;               // 有符号取模后 +1
    else
        result = script->m_championElements[rand() % count];
    return result;
}

// ===========================================================================
// ImportMonsterBaseTable —— ORIG T 0x08a3c736（全量转写）
// ===========================================================================
// [b28d-solo-STMonsterScript] 修法：①循环退出改 `if(!ok) break;` + 循环外 return true
// （P164 尾部共享块落点，消 OURS 多出的 `mov $1; jmp`）；②`row > size()` 保持无符号比较
// （ORIG `seta`，原 static_cast<int> 产有符号 `setl`）；③else 分支内 resize 后共享
// `vec[row] = param`（P276 共享调用点，消重复 rep movsl 段）。ORIG 0x08a3c736
// [b29-SweepSTMonsterScript2] 两处列组循环由一维 `m_values[i+12]/[i+16]` 改二维
// `m_grid[3][i]/m_grid[4][i]`（P311/S85）⇒ 复现 ORIG 0x8a3c8a8/0x8a3c8d9 的
// `mov i,%edx; mov this,%eax; add $0xc/$0x10,%edx; mov %eax,-0xe8(%ebp,%edx,4)`，本件 DIFF→AE。
bool ImportMonsterBaseTable(STMonsterScript* script, const char* path)
{
    if (!loadRDARScriptFile(g_szMonsterScriptBaseDirectory, path))
        return false;                          // +0x1f

    int level = 0;                             // 初始化执行序实证：level→ok→param C1→row
    bool ok = false;
    MonsterBaseParameter param;
    int row = 0;                               // 目标行号
    while (true)
    {
        level = ScanInt(&ok);                  // +0x53 首列 level；!ok → 正常结束
        if (!ok)
            break;                             // +0x2d4 跳出 → 循环外 mov eax,1
        param.m_value[0] = level;              // +0xac
        // +0x7b..0x87 写入序列按 ORIG：[3][4][5][6] → [1][2] → [7]..[12]
        param.m_value[3] = ScanInt(static_cast<bool*>(0));         // -0xe0
        param.m_value[4] = ScanInt(static_cast<bool*>(0));         // -0xdc
        param.m_value[5] = ScanInt(static_cast<bool*>(0));         // -0xd8
        param.m_value[6] = ScanInt(static_cast<bool*>(0));         // -0xd4
        param.m_value[1] = ScanInt(static_cast<bool*>(0));         // -0xe8
        param.m_value[2] = ScanInt(static_cast<bool*>(0));         // -0xe4
        param.m_value[7] = ScanInt(static_cast<bool*>(0));         // -0xd0
        param.m_value[8] = ScanInt(static_cast<bool*>(0));         // -0xcc
        param.m_value[9] = ScanInt(static_cast<bool*>(0));         // -0xc8
        param.m_value[10] = ScanInt(static_cast<bool*>(0));        // -0xc4
        param.m_value[11] = ScanInt(static_cast<bool*>(0));        // -0xc0
        param.m_value[12] = ScanInt(static_cast<bool*>(0));        // -0xbc
        for (int i = 0; i <= 3; ++i)           // +0x160 disp=+4、idx=i+0xc（m_value[13+i]）
            param.m_grid[3][i] = ScanInt(static_cast<bool*>(0));
        for (int i = 0; i <= 3; ++i)           // +0x191 disp=+4、idx=i+0x10（m_value[17+i]）
            param.m_grid[4][i] = ScanInt(static_cast<bool*>(0));
        param.m_value[21] = ScanInt(static_cast<bool*>(0));        // -0x98（param+0x54）
        param.m_value[22] = ScanInt(static_cast<bool*>(0));        // -0x94
        param.m_value[23] = ScanInt(static_cast<bool*>(0));        // -0x90

        if (row == script->m_baseParameters.size())            // ORIG seta 无符号族（P209 载寄存器）
            script->m_baseParameters.push_back(param);            // +0x1f4 追加
        else
        {
            if (row > script->m_baseParameters.size())            // +0x22b 行号跳跃（无符号）
                script->m_baseParameters.resize(row + 1, MonsterBaseParameter());
            script->m_baseParameters[row] = param;                // +0x29e 共享覆写 rep movsl
        }
        ++row;                                 // +0x2cb
    }
    return true;                               // +0x2d4
}

// ===========================================================================
// setBasicAbilityByLevel —— ORIG T 0x088b9603（全量转写）
// ActiveStaticInfo 槽位映射依据现占位头布局：
//   +0x38 elemAttack、+0x58 elemDefense、+0x78 attr17Base、+0x100+8k statBaseK、
//   +0x14 flag14；+0x1a8 float 槽现落 m_statBase27（位型访问【推断】）。
// ===========================================================================
// [b28d-solo-STMonsterScript] 修法（ndiff 424→33，gap=1，p167 25/25 CLEAR）：
// ①`!empty()` → `size() != 0`（P292：ORIG `test %eax,%eax; setne` 为非 bool callee 形）；
// ②四元素抗性表改 for 循环（ORIG 0x203 为循环，此前误作 4 次展开，多 ~28 条）；
// ③**真语义 bug ×2**：ORIG 0x3b7/0x3df 读 param+0x34+4i / param+0x44+4i 即
//   m_value[13+i]/[17+i]，原码误写 [12+i]/[16+i]（差 1 个字段）；
// ④补 `char framePad[0x8]`（P232）使帧 0x64→0x74、bool spill 槽 -0x3c/-0x40→-0x4c/-0x50
//   与 ORIG 精确对齐。
// 残余 N：P209（signed 局部 `if(t==4)` 4.4 恒 mem 直比）、P30/N4（索引 add↔lea 分解）、
// N7/N18a（bool→char 赋值 4.4 多 `mov %eax,%edx` 中转）、P223（循环出口 jmp/nop 岛位次）。
// ORIG 0x088b9603
void setBasicAbilityByLevel(const STMonsterScript* script, ActiveStaticInfo* info,
                            int level, int dungeonIdx, int roleType,
                            bool flagA, bool flagB)
{
    info->clear();                             // +0x19 ActiveStaticInfo::clear
    info->m_level = level;                     // +0x21

    const int diff = level - script->m_levelByDiff[0];        // -0x3c
    const int diffSpan = script->m_levelByDiff[1] -
                         script->m_levelByDiff[0];             // -0x38
    char framePad[0x8];                    // [b28d-solo] P232：ORIG 帧 0x74 的 0x10 空洞
    (void)framePad;
    const int tableIdx = getMonsterTableIndex(script, dungeonIdx,
                                              roleType, flagA, flagB);
    if (tableIdx == 4)                         // +0x85 难度表 4 用副本等级查行
        level = dungeonIdx;                    // （P209：4.4 signed 局部恒 mem 直比，N）

    // +0x93..0x15c：g_monsterBaseParameters[tableIdx] 中首列==level 的首个命中；
    // 脚本自带基础表时改取脚本内同下标行（operator[] 无界，忠实 ORIG）。
    const MonsterBaseParameter* param = 0;     // -0x30
    for (std::vector<MonsterBaseParameter>::size_type i = 0;
         i < g_monsterBaseParameters[tableIdx].size(); ++i)
    {
        if (g_monsterBaseParameters[tableIdx][i].m_value[0] == level)
        {
            if (script->m_baseParameters.size() != 0)           // +0x102
                param = &script->m_baseParameters[i];
            else
                param = &g_monsterBaseParameters[tableIdx][i];
            break;
        }
    }
    if (param == 0)
        return;                                // +0x5f9 未命中直接返回

    // 列映射按 ORIG 访问偏移实证：info+0c←v[2]、+18←v[3] … +28←v[6]
    info->m_hpBase = param->m_value[1];        // +0x169 [v1@+04]→info+04
    info->m_mpBase = param->m_value[2];        //         [v2@+08]→info+0c
    info->m_attack = param->m_value[3];        //         [v3@+0c]
    info->m_magicAttack = param->m_value[4];   //         [v4@+10]
    info->m_defense = param->m_value[5];       //         [v5@+14]
    info->m_magicDefense = param->m_value[6];  //         [v6@+18]
    info->m_statBase11 = param->m_value[7];    //         [v7@+1c]→info+158
    info->m_statBase12 = param->m_value[8];    //         [v8@+20]
    info->m_statBase13 = param->m_value[9];    //         [v9@+24]
    info->m_statBase14 = param->m_value[10];   //         [v10@+28]
    info->m_flag14 = script->m_flag3b8;        // +0x1ed movzbl/movb 直拷

    // +0x206：四属性抗性低/高对 → 元素攻击（GetLinearData(低, 高, 差, 跨度)）
    for (int i = 0; i <= 3; ++i)
    {
        info->m_elemAttack[i] = GetLinearDataForMonsterAbility(
            script->m_resistTable[i][0], script->m_resistTable[i][1], diff, diffSpan);
    }

    // +0x25c：17 组线性能力表对 → attr17Base
    for (int i = 0; i <= 16; ++i)
    {
        info->m_attr17Base[i] = GetLinearDataForMonsterAbility(
            script->m_linearAbilities[i][0], script->m_linearAbilities[i][1],
            diff, diffSpan);
    }

    // +0x2ac..0x39e：速度/恢复/跳跃/重量线性对 → statBase 槽
    info->m_statBase3 = GetLinearDataForMonsterAbility(
        script->m_moveSpeed[0], script->m_moveSpeed[1], diff, diffSpan);
    info->m_statBase4 = GetLinearDataForMonsterAbility(
        script->m_attackSpeed[0], script->m_attackSpeed[1], diff, diffSpan);
    info->m_statBase5 = GetLinearDataForMonsterAbility(
        script->m_castSpeed[0], script->m_castSpeed[1], diff, diffSpan);
    info->m_statBase6 = GetLinearDataForMonsterAbility(
        script->m_hitRecovery[0], script->m_hitRecovery[1], diff, diffSpan);
    info->m_statBase8 = GetLinearDataForMonsterAbility(
        script->m_jumpPower[0], script->m_jumpPower[1], diff, diffSpan);
    info->m_statBase9 = GetLinearDataForMonsterAbility(
        script->m_jumpSpeed[0], script->m_jumpSpeed[1], diff, diffSpan);
    info->m_statBase10 = GetLinearDataForMonsterAbility(
        script->m_weight[0], script->m_weight[1], diff, diffSpan);

    // +0x420：HP/MP 再生对 ÷1000.0f（_DAT_08e084a8）后按最大值折算（截断）
    const int hpRegen = GetLinearDataForMonsterAbility(
        script->m_hpRegenSpeed[0], script->m_hpRegenSpeed[1], diff, diffSpan);
    const int mpRegen = GetLinearDataForMonsterAbility(
        script->m_mpRegenSpeed[0], script->m_mpRegenSpeed[1], diff, diffSpan);
    const float hpFactor = static_cast<float>(hpRegen) / 1000.0f;
    const float mpFactor = static_cast<float>(mpRegen) / 1000.0f;
    info->m_statBase1 = static_cast<int>(
        static_cast<float>(info->getHpMax()) * hpFactor);
    info->m_statBase2 = static_cast<int>(
        static_cast<float>(info->getMpMax()) * mpFactor);

    // +0x30b：float 槽 info+0x1a8（m_statBase27，float 实证）+= param[11]/100.0f
    info->m_statBase27 += static_cast<float>(param->m_value[11]) / 100.0f;

    // +0x333：attr17Base[13] += trunc(param[12]/10.0)（double _DAT_08e084b0）
    info->m_attr17Base[13] += static_cast<int>(
        static_cast<double>(param->m_value[12]) / 10.0);

    // +0x367：paramTable 低/高对 → 元素防御；随后攻击/防御加 param 列
    for (int i = 0; i <= 3; ++i)
    {
        info->m_elemDefense[i] = GetLinearDataForMonsterAbility(
            script->m_paramTable[i][0], script->m_paramTable[i][1],
            diff, diffSpan);
        info->m_elemAttack[i] += param->m_value[13 + i];      // +0x3b7 [param+0x34+4i]
        info->m_elemDefense[i] += param->m_value[17 + i];     // +0x3df [param+0x44+4i]
    }

    info->m_statBase6 += param->m_value[21];   // +0x41c [+54]
    info->m_statBase7 = GetLinearDataForMonsterAbility(
        script->m_field528[0], script->m_field528[1], diff, diffSpan);
    info->m_statBase7 += param->m_value[22];   // +0x46b [+58]

    for (int i = 0; i <= 16; ++i)              // +0x485 i==13 跳过
    {
        if (i == 13)
            continue;
        info->m_attr17Base[i] += param->m_value[23];
    }
}

// ===========================================================================
// ImportMonsterScript —— ORIG T 0x08a12827（0xfb08 字节，~13.8k 指令）。
// 骨架级复原：文件装载/Clear/目录路径/AI 目录传递/模块标签循环与 ORIG 一致；
// 标签分派表按 exportMonsterScript 的 fprintf 格式串反推（导入导出配对），
// 覆盖全部已命名标量/字符串标签；复杂容器标签（throw attack 等）按其元素
// 布局写入主字段。未覆盖的 ORIG 内部分支（17 个 static map 加速表等）以
// 线性比较等价实现 —— 「推断」点见行内注。
// ===========================================================================
namespace {

static const char* const kMonsterActiveStatusTags[17] = {
    "[slow resistance]", "[freeze resistance]", "[poison resistance]",
    "[stun resistance]", "[curse resistance]", "[blind resistance]",
    "[lightning resistance]", "[stone resistance]", "[sleep resistance]",
    "[burn resistance]", "[weapon break resistance]",
    "[bleeding resistance]", "[piercing resistance]", "[stuck resistance]",
    "[confuse resistance]", "[hold resistance]", "[armor break resistance]"
};

int scanValue()
{
    return ScanInt(static_cast<bool*>(0));     // ORIG movl $0 占位 ok 指针
}

}  // namespace


// [BigMonsterScript 2026-09-09] 13759/13778，gap=−19；p167（1742/1700）：
// 游戏域差集已清零 ——
// ① 补 8 处缺失 ScanInt：ORIG 08a1e041/084/0c7/107/147 是五个 difficulty tag 的
//    **独立分支**（每分支 2×ScanInt(0) 写 -0x1b5c/-0x1b58），原合并条件只发 2 次；
// ② 删 1 处多余 importAIInfoScript（ORIG 仅 0x8a1d30a 一处；case 0xf2e 叶为空）。
// 残余（N 封顶，逐条已取证）：
// ③ +1 ScanStr（我们 46 stack/35 field vs ORIG 50/30：4~5 处 ORIG 先读临时 string，
//    本实现直传字段地址，属翻译形态差）；
// ④ 帧差 0x20 + 全帧槽号平移（N82/N126）；
// ⑤ STL 模板形态差（N130：ORIG C++11 右值 pair `C2*_OT_OT0_` / string `D2` 族，
//    本仓 C++98 头集无对应重载，语言级不可复现）。ORIG 0x08a12827
bool ImportMonsterScript(STMonsterScript* script, int type, const char* path,
                        bool flag)
{
    if (!loadRDARScriptFile(g_szMonsterScriptBaseDirectory, path))
        return 0;
    script->Clear();

    getDirectoryPath(script->m_field708, "%s%s",
                     g_szMonsterScriptBaseDirectory, path);
    script->m_aiInfo.m_dir = script->m_field708;
    script->m_throwAttacks.clear();
    script->m_counterAttacks.clear();
    script->m_jumpAttacks.clear();
    script->m_equipment.clear();
    script->m_attackReadies.clear();

    std::string tag;
    std::string token;
    bool ok = false;
    int moduleId = -1;
    int subId = -1;
    bool nameSeen = false;
    while (ScanType(tag, true))                // [b28d-solo] 去 scanTag wrapper（p167 EXTRA）
    {
        moduleId = -1;
        subId = -1;
        const bool dispatch = processModuleTagSet(
            moduleId, subId, &tag, type, false);
        if (subId != -1)
            script->m_field1c9c = subId;
        if (!dispatch)
            continue;

        nameSeen = false;
        int monsterTagId = -1;
        for (;;)
        {
            static std::map<const char*, int, less_TCHAR> __lookup;
            static SpinLock __init_lock;
            static bool __inited = false;
            if (!__inited)
            {
                __init_lock.enter();
                if (__inited)
                {
                    __init_lock.leave();
                    continue;
                }
                monsterTagId = 0;
            }
            else
            {
                std::map<const char*, int, less_TCHAR>::const_iterator __it =
                    __lookup.find(tag.c_str());
                if (__it != __lookup.end())
                    monsterTagId = __it->second;
            }
        switch (monsterTagId)
        {
case 0:
case 0xa14:  // "[name]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[name]", 2580));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_name);  // +0x4
            script->m_altName = script->m_name;
            nameSeen = true;
            break;
        }

case 0xa1a:  // "[width]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[width]", 2586));
            /* FALLTHRU */
        }
        else
        {
            script->m_width = ScanInt(&ok);  // +0xc
            script->m_height = ScanInt(&ok);  // +0x10
            break;
        }

case 0xa1e:  // "[floating height]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[floating height]", 2590));
            /* FALLTHRU */
        }
        else
        {
            script->m_floatingHeight = ScanInt(&ok);  // +0x14
            break;
        }

case 0xa21:  // "[layer]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[layer]", 2593));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&token);  // ORIG 08a13905：ScanStr→-0x1a10(token)→getDrawLayer→+0x18
            script->m_layer = getDrawLayer(token.c_str());  // +0x18
            break;
        }

case 0xa25:  // "[category]" → ScanTypeOrStr + getObjectCategory → m_field01c
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[category]", 2597));
            /* FALLTHRU */
        }
        else
        {
        {
            script->m_field01c.clear();
            //  ORIG 08a13994..08a13ac8：block 级 string -0x1a28 + isType -0x1a29；
            //  ScanTypeOrStr 失败 → return 0；isType 时 **!=** "[/category]" 即 return 0；
            //  getObjectCategory(std::string 值参拷贝临时 -0x1994)；push_back((int)cat 临时
            //  物化 -0x1990 → OOi)。
            std::string catstr;                     // ORIG -0x1a28
            bool isType = false;                    // ORIG -0x1a29
            while (true)
            {
                if (!ScanTypeOrStr(isType, catstr, true))
                    return 0;
                if (!isType)
                {
                    int category = getObjectCategory(std::string(catstr));
                    if (category == 0x2c)
                        continue;
                    script->m_field01c.push_back((int)category);
                    continue;
                }
                if (catstr != "[/category]")
                    return 0;
                break;
            }
        }
            break;
        }

case 0xa3c:  // "[ambient sound]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[ambient sound]", 2620));
            /* FALLTHRU */
        }
        else
        {
            // [b29-SweepSTMonsterScript2] ORIG 0x8a13b44..0x8a13b7e：`ScanStr(&token)`
            // 失败即 `mov $0,%ebx; jmp 0xfabf`（return false），成功才
            // `script->m_ambientSound = token`（`_ZNSsaSERKSs` @0x8a13b79，目标 +0x6ac）。
            if (!ScanStr(&token))
                return 0;
            script->m_ambientSound = token;  // +0x6ac
            break;
        }

case 0xa41:  // "[attack sound]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[attack sound]", 2625));
            /* FALLTHRU */
        }
        else
        {
            // ORIG 0x8a13be0..0x8a13c1a：同上（目标 +0x6a8，operator= @0x8a13c15）。
            if (!ScanStr(&token))
                return 0;
            script->m_attackSound = token;  // +0x6a8
            break;
        }

case 0xa46:  // "[appear sound]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[appear sound]", 2630));
            /* FALLTHRU */
        }
        else
        {
            // ORIG 0x8a13c7c..0x8a13cb6：同上（目标 +0x6a4，operator= @0x8a13cb1）。
            if (!ScanStr(&token))
                return 0;
            script->m_appearSound = token;  // +0x6a4
            break;
        }

case 0xa4b:  // "[damage sound]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[damage sound]", 2635));
            /* FALLTHRU */
        }
        else
        {
            // ORIG 0x8a13d18..0x8a13d52：同上（目标 +0x69c，operator= @0x8a13d4d）。
            if (!ScanStr(&token))
                return 0;
            script->m_damageSound = token;  // +0x69c
            break;
        }

case 0xa50:  // "[die sound]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[die sound]", 2640));
            /* FALLTHRU */
        }
        else
        {
            // ORIG 0x8a13db4..0x8a13dee：同上（目标 +0x6a0，operator= @0x8a13de9）。
            if (!ScanStr(&token))
                return 0;
            script->m_dieSound = token;  // +0x6a0
            break;
        }

case 0xa55:  // "[ability category]"（C @LAB_08a13df3 权威：clear+0x7c → while ScanStr 行 →
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[ability category]", 2645));
            /* FALLTHRU */
        }
        else
        {
            //   14 类名匹配 PARAMETER_CATEGORY_NAME 表 → 元素 push_back 到 m_parameterCategories）
            {
                static const char* const s_catName[14] = {
                    "[ATTACK_SPEED]", "[MOVE_SPEED]", "[PHYSICAL_ATTACK]",
                    "[PHYSICAL_DEFENSE]", "[MAGICAL_ATTACK]", "[MAGICAL_DEFENSE]",
                    "[EQUIPMENT_PHYSICAL_ATTACK]", "[EQUIPMENT_PHYSICAL_DEFENSE]",
                    "[EQUIPMENT_MAGICAL_ATTACK]", "[EQUIPMENT_MAGICAL_DEFENSE]",
                    "[HP MAX]", "[HIT_RECOVERY]", "[STUCK]", "[HP MAX BOSS]"
                };
                script->m_parameterCategories.clear();
                STMonsterParameterCategory item;
                while (ScanStr(&token))
                {
                    for (int i = 0; i <= 13; ++i)
                    {
                        if (token == s_catName[i])
                        {
                            item.m_field0 = i;
                            ScanStr(&token);
                            if (token == "+")
                                item.m_field4 = 0;
                            else if (token == "*")
                                item.m_field4 = 1;
                            item.m_field8 = ScanInt(&ok);
                            break;
                        }
                    }
                    script->m_parameterCategories.push_back(item);
                }
            }
            break;
        }

case 0xa70:  // "[ability table]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[ability table]", 2672));
            /* FALLTHRU */
        }
        else
        {
            script->m_abilityTable = ScanInt(&ok);  // +0x3bc
            break;
        }

case 0xa73:  // "[summon skill status up rate]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[summon skill status up rate]", 2675));
            /* FALLTHRU */
        }
        else
        {
            for (int i = 0; i < 10; ++i)  // C for(i<10) ScanFloat→+0x3c4+4i（m_field3c4）
                script->m_field3c4[i] = ScanFloat(&ok);
            break;
        }

case 0xa77:  // "[level]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[level]", 2679));
            /* FALLTHRU */
        }
        else
        {
            script->m_levelByDiff[0] = ScanInt(&ok);  // +0x3ec
            script->m_levelByDiff[1] = ScanInt(&ok);  // +0x3f0
            break;
        }

case 0xa7b:  // "[fire resistance]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[fire resistance]", 2683));
            /* FALLTHRU */
        }
        else
        {
            script->m_resistTable[0][0] = ScanInt(&ok);  // +0x3f4
            script->m_resistTable[0][1] = ScanInt(&ok);  // +0x3f8
            break;
        }

case 0xa7f:  // "[water resistance]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[water resistance]", 2687));
            /* FALLTHRU */
        }
        else
        {
            script->m_resistTable[1][0] = ScanInt(&ok);  // +0x3fc
            script->m_resistTable[1][1] = ScanInt(&ok);  // +0x400
            break;
        }

case 0xa83:  // "[dark resistance]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[dark resistance]", 2691));
            /* FALLTHRU */
        }
        else
        {
            script->m_resistTable[2][0] = ScanInt(&ok);  // +0x404
            script->m_resistTable[2][1] = ScanInt(&ok);  // +0x408
            break;
        }

case 0xa87:  // "[light resistance]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[light resistance]", 2695));
            /* FALLTHRU */
        }
        else
        {
            script->m_resistTable[3][0] = ScanInt(&ok);  // +0x40c
            script->m_resistTable[3][1] = ScanInt(&ok);  // +0x410
            break;
        }

case 0xa8b:  // "[HP regen speed]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[HP regen speed]", 2699));
            /* FALLTHRU */
        }
        else
        {
            script->m_hpRegenSpeed[0] = ScanInt(&ok);  // +0x49c
            script->m_hpRegenSpeed[1] = ScanInt(&ok);  // +0x4a0
            break;
        }

case 0xa8f:  // "[MP regen speed]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[MP regen speed]", 2703));
            /* FALLTHRU */
        }
        else
        {
            script->m_mpRegenSpeed[0] = ScanInt(&ok);  // +0x4a4
            script->m_mpRegenSpeed[1] = ScanInt(&ok);  // +0x4a8
            break;
        }

case 0xa93:  // "[move speed]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[move speed]", 2707));
            /* FALLTHRU */
        }
        else
        {
            script->m_moveSpeed[0] = ScanInt(&ok);  // +0x4ac
            script->m_moveSpeed[1] = ScanInt(&ok);  // +0x4b0
            break;
        }

case 0xa97:  // "[attack speed]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[attack speed]", 2711));
            /* FALLTHRU */
        }
        else
        {
            script->m_attackSpeed[0] = ScanInt(&ok);  // +0x4b4
            script->m_attackSpeed[1] = ScanInt(&ok);  // +0x4b8
            break;
        }

case 0xaa3:  // "[cast speed]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[cast speed]", 2723));
            /* FALLTHRU */
        }
        else
        {
            script->m_castSpeed[0] = ScanInt(&ok);  // +0x4bc
            script->m_castSpeed[1] = ScanInt(&ok);  // +0x4c0
            break;
        }

case 0xaa7:  // "[hit recovery]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[hit recovery]", 2727));
            /* FALLTHRU */
        }
        else
        {
            script->m_hitRecovery[0] = ScanInt(&ok);  // +0x4c4
            script->m_hitRecovery[1] = ScanInt(&ok);  // +0x4c8
            break;
        }

case 0xaab:  // "[jump power]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[jump power]", 2731));
            /* FALLTHRU */
        }
        else
        {
            script->m_jumpPower[0] = ScanInt(&ok);  // +0x4cc
            script->m_jumpPower[1] = ScanInt(&ok);  // +0x4d0
            break;
        }

case 0xaaf:  // "[jump speed]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[jump speed]", 2735));
            /* FALLTHRU */
        }
        else
        {
            script->m_jumpSpeed[0] = ScanInt(&ok);  // +0x4d4
            script->m_jumpSpeed[1] = ScanInt(&ok);  // +0x4d8
            break;
        }

case 0xab4:  // "[normal dash speed]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[normal dash speed]", 2740));
            /* FALLTHRU */
        }
        else
        {
            script->m_normalDashSpeed[0] = ScanInt(&ok);  // +0x4dc
            script->m_normalDashSpeed[1] = ScanInt(&ok);  // +0x4e0
            break;
        }

case 0xab9:  // "[slant dash speed]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[slant dash speed]", 2745));
            /* FALLTHRU */
        }
        else
        {
            script->m_slantDashSpeed[0] = ScanInt(&ok);  // +0x4e4
            script->m_slantDashSpeed[1] = ScanInt(&ok);  // +0x4e8
            break;
        }

case 0xabf:  // "[weight]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[weight]", 2751));
            /* FALLTHRU */
        }
        else
        {
            script->m_weight[0] = ScanInt(&ok);  // +0x4ec
            script->m_weight[1] = ScanInt(&ok);  // +0x4f0
            break;
        }

case 0xac3:  // "[superarmor on attack]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[superarmor on attack]", 2755));
            /* FALLTHRU */
        }
        else
        {
            script->m_superarmorOnAttack = ScanInt(&ok) != 0;  // +0x5a5
            script->m_field5a8 = ScanInt(&ok);  // +0x5a8
            break;
        }

case 0xac7:  // "[sight]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[sight]", 2759));
            /* FALLTHRU */
        }
        else
        {
            script->m_sight = ScanInt(&ok);  // +0x504
            break;
        }

case 0xaca:  // "[come in action]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[come in action]", 2762));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_comeInAction);  // +0x530
            script->m_field538 = ScanInt(&ok);  // +0x538
            script->m_field53c = ScanInt(&ok);  // +0x53c
            break;
        }

case 0xacf:  // "[come in action ex]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[come in action ex]", 2767));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_comeInAction);  // +0x530
            ScanStr(&script->m_comeInActionEx);  // +0x534
            script->m_field538 = ScanInt(&ok);  // +0x538
            script->m_field53c = ScanInt(&ok);  // +0x53c
            break;
        }

case 0xad6:  // "[diving sight]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[diving sight]", 2774));
            /* FALLTHRU */
        }
        else
        {
            ScanInt(&ok);  // 局部用途 @8a14bf7
            ScanInt(&ok);  // 局部用途 @8a14c09
            break;
        }

case 0xada:  // "[targeting time term]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[targeting time term]", 2778));
            /* FALLTHRU */
        }
        else
        {
            script->m_targetingTimeTerm = ScanInt(&ok);  // +0x574
            break;
        }

case 0xadd:  // "[targeting on attack]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[targeting on attack]", 2781));
            /* FALLTHRU */
        }
        else
        {
            script->m_targetingOnAttack = ScanInt(&ok) != 0;  // +0x580
            break;
        }

case 0xae0:  // "[targeting on damage]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[targeting on damage]", 2784));
            /* FALLTHRU */
        }
        else
        {
            script->m_targetingOnDamage = ScanInt(&ok) != 0;  // +0x581
            break;
        }

case 0xae3:  // "[targeting high level]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[targeting high level]", 2787));
            /* FALLTHRU */
        }
        else
        {
            if (ScanInt(&ok) != 0)
                script->m_targetingLevel = 0x2;  // +0x584 [literal]
        break;
            break;
        }

case 0xae7:  // "[targeting low level]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[targeting low level]", 2791));
            /* FALLTHRU */
        }
        else
        {
            if (ScanInt(&ok) != 0)
                script->m_targetingLevel = 0x1;  // +0x584 [literal]
        break;
            break;
        }

case 0xaeb:  // "[targeting high HP]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[targeting high HP]", 2795));
            /* FALLTHRU */
        }
        else
        {
            if (ScanInt(&ok) != 0)
                script->m_targetingHp = 0x2;  // +0x588 [literal]
        break;
            break;
        }

case 0xaef:  // "[targeting low HP]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[targeting low HP]", 2799));
            /* FALLTHRU */
        }
        else
        {
            if (ScanInt(&ok) != 0)
                script->m_targetingHp = 0x1;  // +0x588 [literal]
        break;
            break;
        }

case 0xaf3:  // "[targeting nearest]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[targeting nearest]", 2803));
            /* FALLTHRU */
        }
        else
        {
            script->m_targetingNearest = ScanInt(&ok) != 0;  // +0x590
        break;
            break;
        }

case 0xaf6:  // "[targeting attacker]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[targeting attacker]", 2806));
            /* FALLTHRU */
        }
        else
        {
            script->m_targetingAttacker = ScanInt(&ok) != 0;  // +0x591
        break;
            break;
        }

case 0xaf9:  // "[targeting from direction]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[targeting from direction]", 2809));
            /* FALLTHRU */
        }
        else
        {
            script->m_targetingFromDirection = ScanInt(&ok) != 0;  // +0x592
        break;
            break;
        }

case 0xafc:  // "[targeting low physical attack]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[targeting low physical attack]", 2812));
            /* FALLTHRU */
        }
        else
        {
            script->m_targetingLowPhysAttack = ScanInt(&ok) != 0;
            break;
        }

case 0xaff:  // "[targeting low physical defense]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[targeting low physical defense]", 2815));
            /* FALLTHRU */
        }
        else
        {
            script->m_targetingLowPhysDefense = ScanInt(&ok) != 0;
            break;
        }

case 0xb02:  // "[targeting low magical attack]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[targeting low magical attack]", 2818));
            /* FALLTHRU */
        }
        else
        {
            script->m_targetingLowMagAttack = ScanInt(&ok) != 0;
            break;
        }

case 0xb05:  // "[targeting low magical defense]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[targeting low magical defense]", 2821));
            /* FALLTHRU */
        }
        else
        {
            script->m_targetingLowMagDefense = ScanInt(&ok) != 0;
            break;
        }

case 0xb08:  // "[targeting downed]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[targeting downed]", 2824));
            /* FALLTHRU */
        }
        else
        {
            script->m_targetingDowned = ScanInt(&ok) != 0;  // +0x5a4
        break;
            break;
        }

case 0xb0b:  // "[warlike]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[warlike]", 2827));
            /* FALLTHRU */
        }
        else
        {
            script->m_warlike = ScanInt(&ok);  // +0x578
            break;
        }

case 0xb12:  // "[attack delay]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[attack delay]", 2834));
            /* FALLTHRU */
        }
        else
        {
            script->m_attackDelay = ScanInt(&ok);  // +0x57c
            break;
        }

case 0xb15:  // "[stuckbonus on damage]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[stuckbonus on damage]", 2837));
            /* FALLTHRU */
        }
        else
        {
            script->m_stuckBonusOnDamage[0][0] = ScanInt(&ok);  // +0x4f4
            script->m_stuckBonusOnDamage[0][1] = ScanInt(&ok);  // +0x4f8
            script->m_stuckBonusOnDamage[1][0] = ScanInt(&ok);  // +0x4fc
            script->m_stuckBonusOnDamage[1][1] = ScanInt(&ok);  // +0x500
            break;
        }

case 0xb1b:  // "[item]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[item]", 2843));
            /* FALLTHRU */
        }
        else
        {
            script->m_field028[0].clear();  // vector<pair<int,int>> @+0x28
            while (true) {
                int v1 = ScanInt(&ok);
                if (!ok) break;
                script->m_field028[0].push_back(std::pair<int, int>(v1, ScanInt(static_cast<bool*>(0))));
            }
            break;
        }

case 0xb25:  // "[common champion drop item]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[common champion drop item]", 2853));
            /* FALLTHRU */
        }
        else
        {
            script->m_field028[1].clear();  // vector<pair<int,int>> @+0x34
            while (true) {
                int v1 = ScanInt(&ok);
                if (!ok) break;
                script->m_field028[1].push_back(std::pair<int, int>(v1, ScanInt(static_cast<bool*>(0))));
            }
            break;
        }

case 0xb31:  // "[hell monster]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[hell monster]", 2865));
            /* FALLTHRU */
        }
        else
        {
            script->m_flag3a8 = ScanInt(&ok) != 0;  // +0x3a8
            break;
        }

case 0xb36:  // "[boss drop item]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[boss drop item]", 2870));
            /* FALLTHRU */
        }
        else
        {
            script->m_field028[3].clear();  // vector<pair<int,int>> @+0x4c
            while (true) {
                int v1 = ScanInt(&ok);
                if (!ok) break;
                script->m_field028[3].push_back(std::pair<int, int>(v1, ScanInt(static_cast<bool*>(0))));
            }
            break;
        }

case 0xb40:  // "[specific gen rate item]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[specific gen rate item]", 2880));
            /* FALLTHRU */
        }
        else
        {
        script->m_field08c.clear();  // vector<pair<int,int>> @+0x8c
        script->m_field088 = ScanInt(&ok);
        if (!ok)
            return 0;
        while (true) {
            int v1 = ScanInt(&ok);
            if (!ok) break;
            ok = false;
            int v2 = ScanInt(&ok);
            if (!ok) break;
            script->m_field08c.push_back(std::pair<int, int>(v1, v2));
        }
            break;
        }

case 0xbb5:  // "[common champion elemental property]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[common champion elemental property]", 2997));
            /* FALLTHRU */
        }
        else
        {
        script->m_championElements.clear();
        while (ScanStr(&token)) {
            for (int i = 0; i < 5; ++i) {
                if (token == getElementName(static_cast<ENUM_ELEMENT>(i))) {
                    script->m_championElements.push_back(i);
                    break;
                }
            }
        }
            break;
        }

case 0xbc8:  // "[super champion drop item]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[super champion drop item]", 3016));
            /* FALLTHRU */
        }
        else
        {
            script->m_field028[2].clear();  // vector<pair<int,int>> @+0x40
            while (true) {
                int v1 = ScanInt(&ok);
                if (!ok) break;
                script->m_field028[2].push_back(std::pair<int, int>(v1, ScanInt(static_cast<bool*>(0))));
            }
            break;
        }

case 0xbab:  // "[death tower item]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[death tower item]", 2987));
            /* FALLTHRU */
        }
        else
        {
            script->m_field058.clear();  // vector<pair<int,int>> @+0x58
            while (true) {
                int v1 = ScanInt(&ok);
                if (!ok) break;
                script->m_field058.push_back(std::pair<int, int>(v1, ScanInt(static_cast<bool*>(0))));
            }
            break;
        }

case 0xbd3:  // "[keep distance]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[keep distance]", 3027));
            /* FALLTHRU */
        }
        else
        {
            script->m_keepDistance = ScanInt(&ok);  // +0x6e0
            break;
        }

case 0xbd6:  // "[is on the object]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[is on the object]", 3030));
            /* FALLTHRU */
        }
        else
        {
            script->m_flag374 = ScanInt(&ok) != 0;  // +0x374
            script->m_field378 = ScanInt(&ok);  // +0x378
            break;
        }

case 0xbdc:  // "[out of targetting]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[out of targetting]", 3036));
            /* FALLTHRU */
        }
        else
        {
            script->m_isOnTheObject = ScanInt(&ok) != 0;  // +0x6e4
            break;
        }

case 0xbe0:  // "[attack kind]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[attack kind]", 3040));
            /* FALLTHRU */
        }
        else
        {
        {
            script->m_attackMethods.clear();
            for (;;)
            {
                STAttackMethod method;
                float value = ScanFloat(&ok);
                if (!ok)
                    break;
                method.m_field08 = value;      // [b28d-solo] 同型 float 直赋（原 memcpy 为 EXTRA）
                value = ScanFloat(static_cast<bool*>(0));
                method.m_field0c = value;
                value = ScanFloat(static_cast<bool*>(0));
                method.m_field10 = value;
                method.m_field00 = ScanInt(static_cast<bool*>(0)) != 0;
                method.m_field18 = ScanInt(static_cast<bool*>(0));
                method.m_field14 = ScanInt(static_cast<bool*>(0));
                method.m_field20 = ScanInt(static_cast<bool*>(0));
                method.m_field1c = ScanInt(static_cast<bool*>(0));
                method.m_field28 = ScanInt(static_cast<bool*>(0));
                method.m_field24 = ScanInt(static_cast<bool*>(0));
                method.m_field30 = ScanInt(static_cast<bool*>(0));
                method.m_field2c = ScanInt(static_cast<bool*>(0));
                script->m_attackMethods.push_back(method);
            }
        }
            break;
        }

case 0xbfa:  // "[die effect]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[die effect]", 3066));
            /* FALLTHRU */
        }
        else
        {
            script->m_dieEffect[0] = ScanInt(&ok);  // +0x6fc
            script->m_dieEffect[1] = ScanInt(&ok);  // +0x700
            script->m_dieEffect[2] = ScanInt(&ok);  // +0x704
            break;
        }

case 0xbff:  // "[face image]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[face image]", 3071));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_faceImage);  // +0x5ac
            script->m_field5b0 = ScanInt(&ok);  // +0x5b0
            break;
        }

case 0xc03:  // "[waiting motion]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[waiting motion]", 3075));
            /* FALLTHRU */
        }
        else
        {
            if (!ScanStr(&token) || !isValidExtension(token, ".ani"))
                return 0;
            script->m_waitingMotion = token;  // +0x5b4
            break;
        }

case 0xc0f:  // "[move motion]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[move motion]", 3087));
            /* FALLTHRU */
        }
        else
        {
            if (!ScanStr(&token) || !isValidExtension(token, ".ani"))
                return 0;
            script->m_moveMotion = token;  // +0x5b8
            break;
        }

case 0xc1b:  // "[sit motion]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[sit motion]", 3099));
            /* FALLTHRU */
        }
        else
        {
            if (!ScanStr(&token) || !isValidExtension(token, ".ani"))
                return 0;
            script->m_sitMotion = token;  // +0x5c8
            break;
        }

case 0xc27:  // "[damage motion 1]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[damage motion 1]", 3111));
            /* FALLTHRU */
        }
        else
        {
            if (!ScanStr(&token) || !isValidExtension(token, ".ani"))
                return 0;
            script->m_damageMotion[0] = token;  // +0x5cc
            break;
        }

case 0xc33:  // "[damage motion 2]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[damage motion 2]", 3123));
            /* FALLTHRU */
        }
        else
        {
            if (!ScanStr(&token) || !isValidExtension(token, ".ani"))
                return 0;
            script->m_damageMotion[1] = token;  // +0x5d0
            break;
        }

case 0xc3f:  // "[down motion]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[down motion]", 3135));
            /* FALLTHRU */
        }
        else
        {
            if (!ScanStr(&token) || !isValidExtension(token, ".ani"))
                return 0;
            script->m_downMotion = token;  // +0x5d4
            break;
        }

case 0xc4b:  // "[overturn motion]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[overturn motion]", 3147));
            /* FALLTHRU */
        }
        else
        {
            if (!ScanStr(&token) || !isValidExtension(token, ".ani"))
                return 0;
            script->m_overturnMotion = token;  // +0x5d8
            break;
        }

case 0xc57:  // "[jump motion]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[jump motion]", 3159));
            /* FALLTHRU */
        }
        else
        {
            if (!ScanStr(&token) || !isValidExtension(token, ".ani"))
                return 0;
            script->m_jumpMotion = token;  // +0x5dc
            break;
        }

case 0xc63:  // "[jumpattack motion]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[jumpattack motion]", 3171));
            /* FALLTHRU */
        }
        else
        {
            if (!ScanStr(&token) || !isValidExtension(token, ".ani"))
                return 0;
            script->m_jumpAttackMotion = token;  // +0x5e0
            break;
        }

case 0xc6f:  // "[attack motion]" → .ani 字符串列表
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[attack motion]", 3183));
            /* FALLTHRU */
        }
        else
        {
        {
            script->m_attackMotions.clear();
            bool isType = false;
            bool ended = false;
            while (ScanTypeOrStr(isType, token, true))
            {
                if (isType)
                {
                    if (token == "[/attack motion]")
                    {
                        ended = true;
                        break;
                    }
                    return 0;
                }
                if (!isValidExtension(token, ".ani"))
                    return 0;
                script->m_attackMotions.push_back(token);
            }
            if (!ended)
                return 0;
        }
            break;
        }

case 0xc89:  // "[etc motion]" → .ani 字符串列表
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[etc motion]", 3209));
            /* FALLTHRU */
        }
        else
        {
        {
            script->m_etcMotions.clear();
            bool isType = false;
            bool ended = false;
            while (ScanTypeOrStr(isType, token, true))
            {
                if (isType)
                {
                    if (token == "[/etc motion]")
                    {
                        ended = true;
                        break;
                    }
                    return 0;
                }
                if (!isValidExtension(token, ".ani"))
                    return 0;
                script->m_etcMotions.push_back(token);
            }
            if (!ended)
                return 0;
        }
            break;
        }

case 0xca3:  // "[waiting action]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[waiting action]", 3235));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_waitingAction);  // +0x5fc
            break;
        }

case 0xca6:  // "[move action]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[move action]", 3238));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_moveAction);  // +0x600
            break;
        }

case 0xca9:  // "[sit action]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[sit action]", 3241));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_sitAction);  // +0x610
            break;
        }

case 0xcac:  // "[damage action 1]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[damage action 1]", 3244));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_damageAction[0]);  // +0x614
            break;
        }

case 0xcaf:  // "[damage action 2]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[damage action 2]", 3247));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_damageAction[1]);  // +0x618
            break;
        }

case 0xcb2:  // "[down action]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[down action]", 3250));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_downAction);  // +0x61c
            break;
        }

case 0xcb5:  // "[overturn action]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[overturn action]", 3253));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_overturnAction);  // +0x620
            break;
        }

case 0xcb8:  // "[jump action]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[jump action]", 3256));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_jumpAction);  // +0x624
            break;
        }

case 0xcbb:  // "[jumpattack action]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[jumpattack action]", 3259));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_jumpAttackAction);  // +0x628
            break;
        }

case 0xcbe:  // "[basic action]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[basic action]", 3262));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_basicAction);  // +0x698
            break;
        }

case 0xcc1:  // "[attack action]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[attack action]", 3265));
            /* FALLTHRU */
        }
        else
        {
        script->m_attackActions.clear();
        while (ScanStr(&token))
            script->m_attackActions.push_back(token);
            break;
        }

case 0xcc2:  // "[action list]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[action list]", 3266));
            /* FALLTHRU */
        }
        else
        {
        // ORIG 与 [attack action] 共用 +0x62c。
        script->m_attackActions.clear();
        while (ScanStr(&token))
            script->m_attackActions.push_back(token);
            break;
        }

case 0xcc8:  // "[etc action]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[etc action]", 3272));
            /* FALLTHRU */
        }
        else
        {
        script->m_etcActions.clear();
        while (ScanStr(&token))
            script->m_etcActions.push_back(token);
            break;
        }

case 0xccd:  // "[last action]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[last action]", 3277));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_lastAction);  // +0x644
            break;
        }

case 0xcd0:  // "[jumpattack info]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[jumpattack info]", 3280));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_jumpAttackInfo);  // +0x6b4
            break;
        }

case 0xcd3:  // "[attack info]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[attack info]", 3283));
            /* FALLTHRU */
        }
        else
        {
        script->m_attackInfos.clear();
        while (ScanStr(&token))
            script->m_attackInfos.push_back(token);
            break;
        }

case 0xcd9:  // "[etc attack info]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[etc attack info]", 3289));
            /* FALLTHRU */
        }
        else
        {
        script->m_etcAttackInfos.clear();
        while (ScanStr(&token))
            script->m_etcAttackInfos.push_back(token);
            break;
        }

case 0xcdf:  // "[int data]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[int data]", 3295));
            /* FALLTHRU */
        }
        else
        {
        script->m_intData.clear();
        for (;;)
        {
            int value = ScanInt(&ok);
            if (!ok)
                break;
            script->m_intData.push_back(value);
        }
            break;
        }

case 0xcea:  // "[speech on situation]" → 111 个情境字符串桶
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[speech on situation]", 3306));
            /* FALLTHRU */
        }
        else
        {
        {
            for (int i = 0; i < 0x6f; ++i)
                script->m_field70c[i].clear();

            while (ScanStr(&token))
            {
                ScanStr(&token);
                const int index = ScanInt(static_cast<bool*>(0));
                int bucket = 0;
                int subId1;
                while (true)
                {
                    static std::map<const char*, int, less_TCHAR> __lookup;
                    static SpinLock __init_lock;
                    static bool __inited = false;
                    if (__inited)
                    {
                        std::map<const char*, int, less_TCHAR>::const_iterator __it =
                            __lookup.find(token.c_str());
                        if (__it != __lookup.end())
                            subId1 = __it->second;
                        else
                            subId1 = -1;
                    }
                    else
                    {
                        __init_lock.enter();
                        if (__inited)
                        {
                            __init_lock.leave();
                            continue;
                        }
                        subId1 = 0;
                    }
                    switch (subId1)
                    {
                    case 0:
                    case 3321:  // [on attack] → 当前行号桶（ORIG 08a17fca：-0xf8=-0x100）
                        if (!__inited)
                        {
                            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[on attack]", 3321));
                            goto sub1_3324;
                        }
                        bucket = index;
                        break;
                    sub1_3324:
                    case 3324:  // [on damage]
                        if (!__inited)
                        {
                            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[on damage]", 3324));
                            goto sub1_3327;
                        }
                        bucket = 0x65;
                        break;
                    sub1_3327:
                    case 3327:  // [on die]
                        if (!__inited)
                        {
                            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[on die]", 3327));
                            goto sub1_3330;
                        }
                        bucket = 0x66;
                        break;
                    sub1_3330:
                    case 3330:  // [on revenge]
                        if (!__inited)
                        {
                            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[on revenge]", 3330));
                            goto sub1_3333;
                        }
                        bucket = 0x67;
                        break;
                    sub1_3333:
                    case 3333:  // [on etc]
                        if (!__inited)
                        {
                            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[on etc]", 3333));
                            goto sub1_3336;
                        }
                        bucket = 0x68;
                        break;
                    sub1_3336:
                    case 3336:  // [on appear]
                        if (!__inited)
                        {
                            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[on appear]", 3336));
                            goto sub1_3339;
                        }
                        bucket = 0x69;
                        break;
                    sub1_3339:
                    case 3339:  // [on disappear]
                        if (!__inited)
                        {
                            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[on disappear]", 3339));
                            goto sub1_3342;
                        }
                        bucket = 0x6a;
                        break;
                    sub1_3342:
                    case 3342:  // [on go to help]
                        if (!__inited)
                        {
                            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[on go to help]", 3342));
                            goto sub1_3345;
                        }
                        bucket = 0x6b;
                        break;
                    sub1_3345:
                    case 3345:  // [on stand]
                        if (!__inited)
                        {
                            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[on stand]", 3345));
                            goto sub1_3348;
                        }
                        bucket = 0x6c;
                        break;
                    sub1_3348:
                    case 3348:  // [on village]
                        if (!__inited)
                        {
                            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[on village]", 3348));
                            goto end1;
                        }
                        bucket = 0x6e;
                        break;
                    default:
                        break;
                    }
                    end1:;
                if (!__inited)
                {
                    __init_lock.leave();
                    __inited = true;
                    continue;
                }
                break;
                }
                    // ORIG 08a183c6：m_field70c[bucket]（stride 0xc）.push_back(local_str)
                    script->m_field70c[bucket].push_back(token);
                }
            }
            break;
        }

case 0xd1d:  // "[fix move direction]"（ORIG 08a18438：ScanInt 非 0 → +0xc50 置 1）
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[fix move direction]", 3357));
            /* FALLTHRU */
        }
        else
        {
            if (ScanInt(static_cast<bool*>(0)) != 0)
                script->m_flagc50 = true;
            break;
        }

case 0xd22:  // "[throw attack]"（收集父 case；ORIG C LAB_08a184c3：
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[throw attack]", 3362));
            /* FALLTHRU */
        }
        else
        {
            //  STThrowAttack 局部 + while(ScanStr 子标签行) __lookup_1(14 键 passive 字段表)
            //  分派 → 收尾 push_back m_throwAttacks +0x9c；字段槽映射 local_1d24+t 直证）
            //  ORIG 08a184c3..08a19076：增量链式建表（case 0 哨兵 → 每 leaf
            //  if(!__inited_1){insert 本键; jmp 下一 leaf} 链 → 尾 leave+置位重入）。
            //  与 STMapScript [background animation] 同族范式。
            {
                STThrowAttack elem;          // C1（ORIG -0x1d24）
                std::string fname;           // -0x1a88（filename 循环暂存）
                int subId2;                  // ORIG -0xec
                int i;                       // ORIG -0xe8（各 resize/读循环共用）
                while (true)
                {
                    static std::map<const char*, int, less_TCHAR> __lookup_1;
                    static SpinLock __init_lock_1;
                    static bool __inited_1 = false;
                    if (__inited_1)
                    {
                        std::map<const char*, int, less_TCHAR>::const_iterator __it =
                            __lookup_1.find(token.c_str());
                        if (__it != __lookup_1.end())
                            subId2 = __it->second;
                        else
                            subId2 = -1;
                    }
                    else
                    {
                        __init_lock_1.enter();
                        if (__inited_1)
                        {
                            __init_lock_1.leave();
                            continue;                        // ORIG 08a18725 nop;jmp 08a18544
                        }
                        subId2 = 0;                          // ORIG 08a18730 movl $0,-0xec
                    }
                    switch (subId2)
                    {
                    case 0:
                    case 3370:       // [passive object filename]：count(=m_field50) 条
                        if (!__inited_1)
                        {
                            __lookup_1.insert(std::map<const char*, int, less_TCHAR>::value_type("[passive object filename]", 3370));
                            goto sub2_3377;                  // ORIG 08a18866 jmp 08a188b9
                        }
                        for (i = 0; i < elem.m_field50; ++i)
                        {
                            ScanStr(&fname);
                            elem.m_field00.push_back(fname);
                        }
                        break;
                    sub2_3377:
                    case 3377:       // [passive object number] → +0x50
                        if (!__inited_1)
                        {
                            __lookup_1.insert(std::map<const char*, int, less_TCHAR>::value_type("[passive object number]", 3377));
                            goto sub2_3380;                  // ORIG 08a18914 jmp 08a1892d
                        }
                        elem.m_field50 = ScanInt(static_cast<bool*>(0));
                        break;
                    sub2_3380:
                    case 3380:       // [passive object index] → +0xc
                        if (!__inited_1)
                        {
                            __lookup_1.insert(std::map<const char*, int, less_TCHAR>::value_type("[passive object index]", 3380));
                            goto sub2_3383;
                        }
                        elem.m_field0c = ScanInt(static_cast<bool*>(0));
                        break;
                    sub2_3383:
                    case 3383:       // [power] → +0x10
                        if (!__inited_1)
                        {
                            __lookup_1.insert(std::map<const char*, int, less_TCHAR>::value_type("[power]", 3383));
                            goto sub2_3386;
                        }
                        elem.m_field10 = ScanInt(static_cast<bool*>(0));
                        break;
                    sub2_3386:
                    case 3386:       // [throw frame] → +0x14
                        if (!__inited_1)
                        {
                            __lookup_1.insert(std::map<const char*, int, less_TCHAR>::value_type("[throw frame]", 3386));
                            goto sub2_3389;
                        }
                        elem.m_field14 = ScanInt(static_cast<bool*>(0));
                        break;
                    sub2_3389:
                    case 3389:       // [start x cood] → resize +0x18 后逐项 ScanInt
                        if (!__inited_1)
                        {
                            __lookup_1.insert(std::map<const char*, int, less_TCHAR>::value_type("[passive object start x cood]", 3389));
                            goto sub2_3396;
                        }
                        elem.m_field18.resize(elem.m_field50);
                        for (i = 0; i < elem.m_field50; ++i)
                            elem.m_field18[i] = ScanInt(static_cast<bool*>(0));
                        break;
                    sub2_3396:
                    case 3396:       // [start y cood] → +0x24
                        if (!__inited_1)
                        {
                            __lookup_1.insert(std::map<const char*, int, less_TCHAR>::value_type("[passive object start y cood]", 3396));
                            goto sub2_3403;
                        }
                        elem.m_field24.resize(elem.m_field50);
                        for (i = 0; i < elem.m_field50; ++i)
                            elem.m_field24[i] = ScanInt(static_cast<bool*>(0));
                        break;
                    sub2_3403:
                    case 3403:       // [start z cood] → +0x30
                        if (!__inited_1)
                        {
                            __lookup_1.insert(std::map<const char*, int, less_TCHAR>::value_type("[passive object start z cood]", 3403));
                            goto sub2_3410;
                        }
                        elem.m_field30.resize(elem.m_field50);
                        for (i = 0; i < elem.m_field50; ++i)
                            elem.m_field30[i] = ScanInt(static_cast<bool*>(0));
                        break;
                    sub2_3410:
                    case 3410:       // [direction] → resize +0x58（ORIG avStack_1ccc 直证）
                        if (!__inited_1)
                        {
                            __lookup_1.insert(std::map<const char*, int, less_TCHAR>::value_type("[direction]", 3410));
                            goto sub2_3417;
                        }
                        elem.m_field58.resize(elem.m_field50);
                        for (i = 0; i < elem.m_field50; ++i)
                            elem.m_field58[i] = ScanInt(static_cast<bool*>(0));
                        break;
                    sub2_3417:
                    case 3417:       // [attack number] → +0x3c
                        if (!__inited_1)
                        {
                            __lookup_1.insert(std::map<const char*, int, less_TCHAR>::value_type("[attack number]", 3417));
                            goto sub2_3420;
                        }
                        elem.m_field3c = ScanInt(static_cast<bool*>(0));
                        break;
                    sub2_3420:
                    case 3420:       // [attack index] → +0x44
                        if (!__inited_1)
                        {
                            __lookup_1.insert(std::map<const char*, int, less_TCHAR>::value_type("[attack index]", 3420));
                            goto sub2_3423;
                        }
                        elem.m_field44 = ScanInt(static_cast<bool*>(0));
                        break;
                    sub2_3423:
                    case 3423:       // [throw sound] → ScanStr 直读 +0x40
                        if (!__inited_1)
                        {
                            __lookup_1.insert(std::map<const char*, int, less_TCHAR>::value_type("[throw sound]", 3423));
                            goto sub2_3426;
                        }
                        ScanStr(&elem.m_field40);
                        break;
                    sub2_3426:
                    case 3426:       // [object type] → +0x4c
                        if (!__inited_1)
                        {
                            __lookup_1.insert(std::map<const char*, int, less_TCHAR>::value_type("[object type]", 3426));
                            goto sub2_3429;
                        }
                        elem.m_field4c = ScanInt(static_cast<bool*>(0));
                        break;
                    sub2_3429:
                    case 3429:       // [start kind] → +0x54
                        if (!__inited_1)
                        {
                            __lookup_1.insert(std::map<const char*, int, less_TCHAR>::value_type("[start kind]", 3429));
                            goto sub2_end;                   // ORIG 末键 insert 直落尾
                        }
                        elem.m_field54 = ScanInt(static_cast<bool*>(0));
                        break;
                    default:
                        break;
                    }
                sub2_end:
                    if (!__inited_1)
                    {
                        __init_lock_1.leave();               // ORIG 08a1902a..31
                        __inited_1 = true;                   // ORIG 08a19036
                        continue;                            // ORIG 08a1903d jmp 08a18544
                    }
                    if (!ScanStr(&token))                    // ORIG 08a19043..53
                        break;
                }
                script->m_throwAttacks.push_back(elem);      // +0x9c ORIG 08a1906f
            }
            break;
        }

case 0xd6e:  // "[equipment]"（收集；ORIG C LAB_08a190cc：clear +0x390 → ScanInt 数量 →
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[equipment]", 3438));
            /* FALLTHRU */
        }
        else
        {
            //  count 次 ScanStr push m_items；末尾一次 ScanStr 丢弃）
            {
                script->m_equipment.clear();
                std::string item;
                const int count = ScanInt(static_cast<bool*>(0));
                for (int i = 0; i < count; ++i)
                {
                    ScanStr(&item);
                    script->m_equipment.m_items.push_back(item);
                }
                std::string tail;        // ORIG -0x1a94：读后即弃
                ScanStr(&tail);
            }
            break;
        }

case 0xd7b:  // "[ready action]"（收集父 case；ORIG C LAB_08a1921f：STMosterAttackReady 局部
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[ready action]", 3451));
            /* FALLTHRU */
        }
        else
        {
            //  + while(ScanStr) 子标签 operator== 直判（无子表）→ push +0x39c）
            {
                STMosterAttackReady elem;
                std::string tmp;                         // -0x1ab4
                while (ScanStr(&token))
                {
                    if (token == "[motion index]")       // ORIG +0xc
                        elem.m_field0c = ScanInt(static_cast<bool*>(0));
                    else if (token == "[effect ani file & layer index]")
                    {
                        const int n = ScanInt(static_cast<bool*>(0));
                        elem.m_field10.reserve(n);       // 先 string 向量（C 顺序）
                        elem.m_field00.reserve(n);
                        for (int i = 0; i < n; ++i)
                        {
                            ScanStr(&tmp);
                            elem.m_field10.push_back(tmp);
                            const int v = ScanInt(static_cast<bool*>(0));
                            elem.m_field00.push_back(v);
                        }
                    }
                }
                script->m_attackReadies.push_back(elem);   // +0x39c
            }
            break;
        }

case 0xd96:  // "[counter attack]"（收集父 case；ORIG asm 08a19436 / C 直译：
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[counter attack]", 3478));
            /* FALLTHRU */
        }
        else
        {
            //  块首 ScanInt 元素数 → reserve → for(count){ while(ScanStr) 内 __lookup_3(4 键)
            //  字段分派 } → push +0xa8）
            {
                STCounterAttack elem;                    // -0x1acc
                const int count = ScanInt(static_cast<bool*>(0));
                script->m_counterAttacks.reserve(count);
                for (int k = 0; k < count; ++k)
                {
                    while (ScanStr(&token))
                    {
                        int subId3;
                        while (true)
                        {
                            static std::map<const char*, int, less_TCHAR> __lookup;
                            static SpinLock __init_lock;
                            static bool __inited = false;
                            if (__inited)
                            {
                                std::map<const char*, int, less_TCHAR>::const_iterator __it =
                                    __lookup.find(token.c_str());
                                if (__it != __lookup.end())
                                    subId3 = __it->second;
                                else
                                    subId3 = -1;
                            }
                            else
                            {
                                __init_lock.enter();
                                if (__inited)
                                {
                                    __init_lock.leave();
                                    continue;
                                }
                                subId3 = 0;
                            }
                            switch (subId3)
                            {
                            case 0:
                            case 3489:  // [probability] → +0x00
                                if (!__inited)
                                {
                                    __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[probability]", 3489));
                                    goto sub3_3492;
                                }
                            elem.m_field00 = ScanInt(static_cast<bool*>(0));
                            break;
                            sub3_3492:
                            case 3492:  // [attack index] → +0x04
                                if (!__inited)
                                {
                                    __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[attack index]", 3492));
                                    goto sub3_3495;
                                }
                            elem.m_field04 = ScanInt(static_cast<bool*>(0));
                            break;
                            sub3_3495:
                            case 3495:  // [damage type] → 三串比较映射 0/1/2 → +0x08
                                if (!__inited)
                                {
                                    __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[damage type]", 3495));
                                    goto sub3_3511;
                                }
                        {
                            std::string kind;            // -0x1ad4（块内构造/析构）
                            ScanStr(&kind);
                            if (kind == "[approach attack]")
                                elem.m_field08 = 0;
                            else if (kind == "[range attack]")
                                elem.m_field08 = 1;
                            else if (kind == "[all attack]")
                                elem.m_field08 = 2;
                        }
                            break;
                            sub3_3511:
                            case 3511:  // [need hp] → +0x10
                                if (!__inited)
                                {
                                    __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[need hp]", 3511));
                                    goto end3;
                                }
                            elem.m_field10 = ScanInt(static_cast<bool*>(0));
                            break;
                            default:
                                break;
                            }
                            end3:;
                        if (!__inited)
                        {
                            __init_lock.leave();
                            __inited = true;
                            continue;
                        }
                        break;
                        }
                    }
                    script->m_counterAttacks.push_back(elem);   // +0xa8
                }
            }
            break;
        }

case 0xdc0:  // "[jump attack]"（收集父 case；ORIG asm 08a19a00：STJumpAttack C1(-0x1d24) +
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[jump attack]", 3520));
            /* FALLTHRU */
        }
        else
        {
            //  while(ScanStr 子标签行) __lookup_4(11 键) 字段分派 → push +0xb4）
            {
                STJumpAttack elem;                       // -0x1d24
                while (ScanStr(&token))
                {
                    int subId4;
                    while (true)
                    {
                        static std::map<const char*, int, less_TCHAR> __lookup;
                        static SpinLock __init_lock;
                        static bool __inited = false;
                        if (__inited)
                        {
                            std::map<const char*, int, less_TCHAR>::const_iterator __it =
                                __lookup.find(token.c_str());
                            if (__it != __lookup.end())
                                subId4 = __it->second;
                            else
                                subId4 = -1;
                        }
                        else
                        {
                            __init_lock.enter();
                            if (__inited)
                            {
                                __init_lock.leave();
                                continue;
                            }
                            subId4 = 0;
                        }
                        switch (subId4)
                        {
                        case 0:
                        case 3526:  // [attack index] → +0x00
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[attack index]", 3526));
                                goto sub4_3529;
                            }
                        elem.m_field00 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub4_3529:
                        case 3529:  // [start frame index] → +0x1c
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[start frame index]", 3529));
                                goto sub4_3532;
                            }
                        elem.m_field1c = ScanInt(static_cast<bool*>(0));
                        break;
                        sub4_3532:
                        case 3532:  // [end frame index] → +0x20
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[end frame index]", 3532));
                                goto sub4_3535;
                            }
                        elem.m_field20 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub4_3535:
                        case 3535:  // [jump height] → +0x04
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[jump height]", 3535));
                                goto sub4_3538;
                            }
                        elem.m_field04 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub4_3538:
                        case 3538:  // [jump speed] → +0x08
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[jump speed]", 3538));
                                goto sub4_3541;
                            }
                        elem.m_field08 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub4_3541:
                        case 3541:  // [on collision custom animation index] → +0x0c
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[on collision custom animation index]", 3541));
                                goto sub4_3544;
                            }
                        elem.m_field0c = ScanInt(static_cast<bool*>(0));
                        break;
                        sub4_3544:
                        case 3544:  // [ememy distance rate] → +0x10（ScanFloat，P79 float 域）
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[ememy distance rate]", 3544));
                                goto sub4_3547;
                            }
                        elem.m_field10 = ScanFloat(static_cast<bool*>(0));
                        break;
                        sub4_3547:
                        case 3547:  // [limited x cood] → +0x24
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[limited x cood]", 3547));
                                goto sub4_3550;
                            }
                        elem.m_field24 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub4_3550:
                        case 3550:  // [limited y cood] → +0x28
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[limited y cood]", 3550));
                                goto sub4_3553;
                            }
                        elem.m_field28 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub4_3553:
                        case 3553:  // [jump type] → +0x14
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[jump type]", 3553));
                                goto sub4_3556;
                            }
                        elem.m_field14 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub4_3556:
                        case 3556:  // [jump direction] → +0x18
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[jump direction]", 3556));
                                goto end4;
                            }
                        elem.m_field18 = ScanInt(static_cast<bool*>(0));
                        break;
                        default:
                            break;
                        }
                        end4:;
                    if (!__inited)
                    {
                        __init_lock.leave();
                        __inited = true;
                        continue;
                    }
                    break;
                    }
                }
                script->m_jumpAttacks.push_back(elem);   // +0xb4
            }
            break;
        }

case 0xdec:  // "[summon]"（收集父 case；ORIG asm 08a1a262：STSummon C1(-0x1d24) +
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[summon]", 3564));
            /* FALLTHRU */
        }
        else
        {
            //  STSummonInfo 复用槽(-0x1ae8) + while(ScanStr) __lookup_5(9 键) 分派 → push +0xc0）
            {
                STSummon elem;                           // -0x1d24
                STSummonInfo info;                       // -0x1ae8（每轮四 int 覆写后 push）
                while (ScanStr(&token))
                {
                    int subId5;
                    while (true)
                    {
                        static std::map<const char*, int, less_TCHAR> __lookup;
                        static SpinLock __init_lock;
                        static bool __inited = false;
                        if (__inited)
                        {
                            std::map<const char*, int, less_TCHAR>::const_iterator __it =
                                __lookup.find(token.c_str());
                            if (__it != __lookup.end())
                                subId5 = __it->second;
                            else
                                subId5 = -1;
                        }
                        else
                        {
                            __init_lock.enter();
                            if (__inited)
                            {
                                __init_lock.leave();
                                continue;
                            }
                            subId5 = 0;
                        }
                        switch (subId5)
                        {
                        case 0:
                        case 3572:  // [function index] → +0x00
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[function index]", 3572));
                                goto sub5_3575;
                            }
                        elem.m_field00 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub5_3575:
                        case 3575:  // [attack index] → +0x04
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[attack index]", 3575));
                                goto sub5_3578;
                            }
                        elem.m_field04 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub5_3578:
                        case 3578:  // [pos random] → +0x08
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[pos random]", 3578));
                                goto sub5_3581;
                            }
                        elem.m_field08 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub5_3581:
                        case 3581:  // [monster kind] → +0x0c 数量 + infos reserve/loop
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[monster kind]", 3581));
                                goto sub5_3593;
                            }
                    {
                        elem.m_field0c = ScanInt(static_cast<bool*>(0));
                        elem.m_infos.reserve(elem.m_field0c);
                        for (int i = 0; i < elem.m_field0c; ++i)
                        {
                            info.m_field00 = ScanInt(static_cast<bool*>(0));
                            info.m_field04 = ScanInt(static_cast<bool*>(0));
                            info.m_field08 = ScanInt(static_cast<bool*>(0));
                            info.m_field0c = ScanInt(static_cast<bool*>(0));
                            elem.m_infos.push_back(info);
                        }
                    }
                        break;
                        sub5_3593:
                        case 3593:  // [summon term] → +0x1c
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[summon term]", 3593));
                                goto sub5_3596;
                            }
                        elem.m_field1c = ScanInt(static_cast<bool*>(0));
                        break;
                        sub5_3596:
                        case 3596:  // [probability] → +0x20
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[probability]", 3596));
                                goto sub5_3599;
                            }
                        elem.m_field20 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub5_3599:
                        case 3599:  // [enterance effect] → +0x24
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[enterance effect]", 3599));
                                goto sub5_3602;
                            }
                        elem.m_field24 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub5_3602:
                        case 3602:  // [max number] → +0x28
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[max number]", 3602));
                                goto sub5_3605;
                            }
                        elem.m_field28 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub5_3605:
                        case 3605:  // [max frequency] → +0x2c
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[max frequency]", 3605));
                                goto end5;
                            }
                        elem.m_field2c = ScanInt(static_cast<bool*>(0));
                        break;
                        default:
                            break;
                        }
                        end5:;
                    if (!__inited)
                    {
                        __init_lock.leave();
                        __inited = true;
                        continue;
                    }
                    break;
                    }
                }
                script->m_summons.push_back(elem);   // +0xc0
            }
            break;
        }

case 0xe1d:  // "[dash attack]"（收集父 case；ORIG asm 08a1aaa6：STDash C1(-0x1d24) +
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[dash attack]", 3613));
            /* FALLTHRU */
        }
        else
        {
            //  while(ScanStr) __lookup_6(8 键) 分派 → push +0xcc）
            {
                STDash elem;                             // -0x1d24
                while (ScanStr(&token))
                {
                    int subId6;
                    while (true)
                    {
                        static std::map<const char*, int, less_TCHAR> __lookup;
                        static SpinLock __init_lock;
                        static bool __inited = false;
                        if (__inited)
                        {
                            std::map<const char*, int, less_TCHAR>::const_iterator __it =
                                __lookup.find(token.c_str());
                            if (__it != __lookup.end())
                                subId6 = __it->second;
                            else
                                subId6 = -1;
                        }
                        else
                        {
                            __init_lock.enter();
                            if (__inited)
                            {
                                __init_lock.leave();
                                continue;
                            }
                            subId6 = 0;
                        }
                        switch (subId6)
                        {
                        case 0:
                        case 3620:  // [function index] → +0x00
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[function index]", 3620));
                                goto sub6_3623;
                            }
                        elem.m_field00 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub6_3623:
                        case 3623:  // [attack index] → +0x04
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[attack index]", 3623));
                                goto sub6_3626;
                            }
                        elem.m_field04 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub6_3626:
                        case 3626:  // [shakeable] → 连续三 int +0x08/+0x0c/+0x10
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[shakeable]", 3626));
                                goto sub6_3631;
                            }
                    {
                        elem.m_field08 = ScanInt(static_cast<bool*>(0));
                        elem.m_field0c = ScanInt(static_cast<bool*>(0));
                        elem.m_field10 = ScanInt(static_cast<bool*>(0));
                    }
                        break;
                        sub6_3631:
                        case 3631:  // [first speed] → +0x14
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[first speed]", 3631));
                                goto sub6_3634;
                            }
                        elem.m_field14 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub6_3634:
                        case 3634:  // [last speed] → +0x18
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[last speed]", 3634));
                                goto sub6_3637;
                            }
                        elem.m_field18 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub6_3637:
                        case 3637:  // [dash time] → +0x1c
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[dash time]", 3637));
                                goto sub6_3640;
                            }
                        elem.m_field1c = ScanInt(static_cast<bool*>(0));
                        break;
                        sub6_3640:
                        case 3640:  // [dash ready time] → +0x20
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[dash ready time]", 3640));
                                goto sub6_3643;
                            }
                        elem.m_field20 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub6_3643:
                        case 3643:  // [animation first index] → +0x24
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[animation first index]", 3643));
                                goto end6;
                            }
                        elem.m_field24 = ScanInt(static_cast<bool*>(0));
                        break;
                        default:
                            break;
                        }
                        end6:;
                    if (!__inited)
                    {
                        __init_lock.leave();
                        __inited = true;
                        continue;
                    }
                    break;
                    }
                }
                script->m_dashes.push_back(elem);   // +0xcc
            }
            break;
        }

case 0xe43:  // "[hold attack]"（收集父 case；ORIG asm 08a1b1af：STHold C1(-0x1d24) +
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[hold attack]", 3651));
            /* FALLTHRU */
        }
        else
        {
            //  while(ScanStr) __lookup_7(6 键) 分派 → push +0xd8）
            {
                STHold elem;                             // -0x1d24
                while (ScanStr(&token))
                {
                    int subId7;
                    while (true)
                    {
                        static std::map<const char*, int, less_TCHAR> __lookup;
                        static SpinLock __init_lock;
                        static bool __inited = false;
                        if (__inited)
                        {
                            std::map<const char*, int, less_TCHAR>::const_iterator __it =
                                __lookup.find(token.c_str());
                            if (__it != __lookup.end())
                                subId7 = __it->second;
                            else
                                subId7 = -1;
                        }
                        else
                        {
                            __init_lock.enter();
                            if (__inited)
                            {
                                __init_lock.leave();
                                continue;
                            }
                            subId7 = 0;
                        }
                        switch (subId7)
                        {
                        case 0:
                        case 3658:  // [function index] → +0x00；==1 时连读 +0x04/+0x08
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[function index]", 3658));
                                goto sub7_3666;
                            }
                    {
                        elem.m_field00 = ScanInt(static_cast<bool*>(0));
                        if (elem.m_field00 == 1)
                        {
                            elem.m_field04 = ScanInt(static_cast<bool*>(0));
                            elem.m_field08 = ScanInt(static_cast<bool*>(0));
                        }
                    }
                        break;
                        sub7_3666:
                        case 3666:  // [attack index] → +0x0c
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[attack index]", 3666));
                                goto sub7_3669;
                            }
                        elem.m_field0c = ScanInt(static_cast<bool*>(0));
                        break;
                        sub7_3669:
                        case 3669:  // [offset] → +0x10 与 +0x14
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[offset]", 3669));
                                goto sub7_3673;
                            }
                        // ORIG 0x8a1b5e9 body：ScanInt→-0x1d10、ScanInt→-0x1d0c（STHold C1 基座 -0x1d20）
                        elem.m_field10 = ScanInt(static_cast<bool*>(0));
                        elem.m_field14 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub7_3673:
                        case 3673:  // [animation index] → +0x18
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[animation index]", 3673));
                                goto sub7_3676;
                            }
                        elem.m_field18 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub7_3676:
                        case 3676:  // [min hold] → +0x1c
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[min hold]", 3676));
                                goto sub7_3679;
                            }
                        elem.m_field1c = ScanInt(static_cast<bool*>(0));
                        break;
                        sub7_3679:
                        case 3679:  // [max hold] → +0x20
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[max hold]", 3679));
                                goto end7;
                            }
                        elem.m_field20 = ScanInt(static_cast<bool*>(0));
                        break;
                        default:
                            break;
                        }
                        end7:;
                    if (!__inited)
                    {
                        __init_lock.leave();
                        __inited = true;
                        continue;
                    }
                    break;
                    }
                }
                script->m_holds.push_back(elem);   // +0xd8
            }
            break;
        }

case 0xe67:  // "[warp attack]"（收集父 case；ORIG asm 08a1b7c6：STWarp C1(-0x1d24) +
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[warp attack]", 3687));
            /* FALLTHRU */
        }
        else
        {
            //  while(ScanStr) __lookup_8(9 键) 分派 → push +0xe4；
            //  [category of target]：null→-2 / all→-1 / 其余 getCategoryToEnumValue）
            {
                STWarp elem;                             // -0x1d24
                while (ScanStr(&token))
                {
                    int subId8;
                    while (true)
                    {
                        static std::map<const char*, int, less_TCHAR> __lookup;
                        static SpinLock __init_lock;
                        static bool __inited = false;
                        if (__inited)
                        {
                            std::map<const char*, int, less_TCHAR>::const_iterator __it =
                                __lookup.find(token.c_str());
                            if (__it != __lookup.end())
                                subId8 = __it->second;
                            else
                                subId8 = -1;
                        }
                        else
                        {
                            __init_lock.enter();
                            if (__inited)
                            {
                                __init_lock.leave();
                                continue;
                            }
                            subId8 = 0;
                        }
                        switch (subId8)
                        {
                        case 0:
                        case 3694:  // [function index] → +0x00
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[function index]", 3694));
                                goto sub8_3697;
                            }
                        elem.m_field00 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub8_3697:
                        case 3697:  // [attack index] → +0x04
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[attack index]", 3697));
                                goto sub8_3700;
                            }
                        elem.m_field04 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub8_3700:
                        case 3700:  // [warp style] → +0x08
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[warp style]", 3700));
                                goto sub8_3703;
                            }
                        elem.m_field08 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub8_3703:
                        case 3703:  // [after attack] → +0x0c
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[after attack]", 3703));
                                goto sub8_3706;
                            }
                        elem.m_field0c = ScanInt(static_cast<bool*>(0));
                        break;
                        sub8_3706:
                        case 3706:  // [after appear time] → +0x10
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[after appear time]", 3706));
                                goto sub8_3709;
                            }
                        elem.m_field10 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub8_3709:
                        case 3709:  // [cool time] → +0x14
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[cool time]", 3709));
                                goto sub8_3712;
                            }
                        elem.m_field14 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub8_3712:
                        case 3712:  // [target offset] → +0x18
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[target offset]", 3712));
                                goto sub8_3715;
                            }
                        elem.m_field18 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub8_3715:
                        case 3715:  // [passive object index] → +0x1c
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[passive object index]", 3715));
                                goto sub8_3718;
                            }
                        elem.m_field1c = ScanInt(static_cast<bool*>(0));
                        break;
                        sub8_3718:
                        case 3718:  // [category of target] → +0x20
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[category of target]", 3718));
                                goto end8;
                            }
                    {
                        std::string cat;         // -0x1afc（块内构造/析构）
                        ScanStr(&cat);
                        if (cat == "null")
                            elem.m_field20 = -2;
                        else if (cat == "all")
                            elem.m_field20 = -1;
                        else
                            elem.m_field20 = getCategoryToEnumValue(cat);
                    }
                        break;
                        default:
                            break;
                        }
                        end8:;
                    if (!__inited)
                    {
                        __init_lock.leave();
                        __inited = true;
                        continue;
                    }
                    break;
                    }
                }
                script->m_warps.push_back(elem);   // +0xe4
            }
            break;
        }

case 0xe9e:  // "[event super on scr]"（收集父 case；ORIG asm 08a1bfcf：STEventSuper C1 +
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[event super on scr]", 3742));
            /* FALLTHRU */
        }
        else
        {
            //  while(ScanStr) __lookup_9(8 键触发模式表) 分派 → push +0xfc）
            {
                STEventSuper elem;                       // -0x1d24（每元素复用槽）
                while (ScanStr(&token))
                {
                    int subId9;
                    while (true)
                    {
                        static std::map<const char*, int, less_TCHAR> __lookup;
                        static SpinLock __init_lock;
                        static bool __inited = false;
                        if (__inited)
                        {
                            std::map<const char*, int, less_TCHAR>::const_iterator __it =
                                __lookup.find(token.c_str());
                            if (__it != __lookup.end())
                                subId9 = __it->second;
                            else
                                subId9 = -1;
                        }
                        else
                        {
                            __init_lock.enter();
                            if (__inited)
                            {
                                __init_lock.leave();
                                continue;
                            }
                            subId9 = 0;
                        }
                        switch (subId9)
                        {
                        case 0:
                        case 3749:
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[on time]", 3749));
                                goto sub9_3755;
                            }
                    {
                        elem.m_field00 = 0;
                        elem.m_field08 = ScanInt(&ok);   // +8
                        elem.m_field0c = ScanInt(&ok);   // +0xc
                        elem.m_field10 = ScanInt(&ok) != 0;   // +0x10 byte（CONCAT 低位）
                    }
                        break;
                        sub9_3755:
                        case 3755:
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[on hp]", 3755));
                                goto sub9_3761;
                            }
                    {
                        elem.m_field00 = 1;
                        elem.m_field08 = ScanInt(&ok);
                        elem.m_field0c = ScanInt(&ok);
                        elem.m_field10 = ScanInt(&ok) != 0;
                    }
                        break;
                        sub9_3761:
                        case 3761:
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[always]", 3761));
                                goto sub9_3767;
                            }
                    {
                        elem.m_field00 = 2;
                        elem.m_field08 = ScanInt(&ok);
                        elem.m_field10 = ScanInt(&ok) != 0;
                    }
                        break;
                        sub9_3767:
                        case 3767:
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[counter]", 3767));
                                goto sub9_3773;
                            }
                    {
                        elem.m_field00 = 3;
                        elem.m_field08 = ScanInt(&ok);
                        elem.m_field0c = ScanInt(&ok);
                        elem.m_field10 = ScanInt(&ok) != 0;
                    }
                        break;
                        sub9_3773:
                        case 3773:
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[breakable]", 3773));
                                goto sub9_3780;
                            }
                    {
                        elem.m_field04 = 0;
                        elem.m_field14 = ScanInt(&ok);   // +0x14（local_1d10）
                        elem.m_field18 = ScanInt(&ok);   // +0x18
                        elem.m_field1c = ScanInt(&ok);   // +0x1c
                        elem.m_field20 = ScanInt(&ok);   // +0x20
                    }
                        break;
                        sub9_3780:
                        case 3780:
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[half]", 3780));
                                goto sub9_3787;
                            }
                    {
                        elem.m_field04 = 1;
                        elem.m_field14 = ScanInt(&ok);
                        elem.m_field18 = ScanInt(&ok);
                        elem.m_field1c = ScanInt(&ok);
                        elem.m_field20 = ScanInt(&ok);
                    }
                        break;
                        sub9_3787:
                        case 3787:
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[unbreakable]", 3787));
                                goto sub9_3794;
                            }
                    {
                        elem.m_field04 = 2;
                        elem.m_field14 = ScanInt(&ok);
                        elem.m_field18 = ScanInt(&ok);
                        elem.m_field1c = ScanInt(&ok);
                        elem.m_field20 = ScanInt(&ok);
                    }
                        break;
                        sub9_3794:
                        case 3794:
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[with blast]", 3794));
                                goto end9;
                            }
                        elem.m_field24 = 1;   // +0x24 byte（CONCAT 置 1）
                        break;
                        default:
                            break;
                        }
                        end9:;
                    if (!__inited)
                    {
                        __init_lock.leave();
                        __inited = true;
                        continue;
                    }
                    break;
                    }
                    // ORIG：未识别引导无字段分支，循环继续
                }
                script->m_eventSupers.push_back(elem);   // +0xfc
            }
            break;
        }

case 0xeda:  // "[atk super on scr]"（收集父 case；ORIG asm 08a1c823 / C 段：
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[atk super on scr]", 3802));
            /* FALLTHRU */
        }
        else
        {
            //  每元素行 = <m_field04值> `[breakable|half|unbreakable]` 4×int → push +0x108）
            {
                STAtkSuper elem;   // C1（asm 08a1c883 每元素复用槽）
                while (true)
                {
                    int v = ScanInt(&ok);
                    if (!ok)
                        break;   // asm 08a1c8b1：首 ScanInt 失败退出收集
                    elem.m_field04 = v;   // 存 +4（asm mov -0x1b10）
                    if (ScanStr(&token))
                    {
                        if (token == "[breakable]")
                            elem.m_field00 = 0xd;
                        else if (token == "[half]")
                            elem.m_field00 = 0xe;
                        else if (token == "[unbreakable]")
                            elem.m_field00 = 0xf;
                    }
                    elem.m_field08 = ScanInt(&ok);
                    elem.m_field0c = ScanInt(&ok);
                    elem.m_field10 = ScanInt(&ok);
                    elem.m_field14 = ScanInt(&ok);
                    script->m_atkSupers.push_back(elem);   // +0x108
                }
            }
            break;
        }

case 0xefb:  // "[cooltime]"（收集父 case；ORIG asm 08a1c99f / C 5530：
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[cooltime]", 3835));
            /* FALLTHRU */
        }
        else
        {
            //  ScanTypeOrInt 循环：数字→m_field04；"[Pattern]"→m_field00=ScanInt；
            //  "[/cooltime]" 结束 → push +0x114）
            {
                STCoolTime elem;   // C1
                for (;;)
                {
                    bool isType = false;
                    bool isInt = false;
                    int v = ScanTypeOrInt(isType, token, isInt, true);
                    if (!isInt)
                        break;   // 读尽/EOF
                    if (isType)
                    {
                        if (token == "[Pattern]")
                            elem.m_field00 = ScanInt(&ok);
                        else if (token == "[/cooltime]")
                            break;
                    }
                    else
                        elem.m_field04.push_back(v);
                }
                script->m_coolTimes.push_back(elem);   // +0x114
            }
            break;
        }

case 0xf23:  // "[spawn prob]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[spawn prob]", 3875));
            /* FALLTHRU */
        }
        else
        {
            script->m_fieldc4c = ScanInt(&ok);  // +0xc4c
            break;
        }

case 0xf27:  // "[drop item offset]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[drop item offset]", 3879));
            /* FALLTHRU */
        }
        else
        {
            script->m_field36c = ScanInt(&ok);  // +0x36c
            script->m_field370 = ScanInt(&ok);  // +0x370
            break;
        }

case 0xf2b:  // "[is grabable]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[is grabable]", 3883));
            /* FALLTHRU */
        }
        else
        {
            script->m_flag368 = ScanInt(&ok) != 0;  // +0x368
            break;
        }

case 0xf2e:  // "[ai pattern]"（ORIG 叶为空；importAIInfoScript 仅在 0xf3e 调 1 次）
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[ai pattern]", 3886));
            /* FALLTHRU */
        }
        else
        {
            break;
        }

case 0xf2f:  // "[think term]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[think term]", 3887));
            /* FALLTHRU */
        }
        else
        {
            break;
        }
case 0xf30:  // "[destination change term]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[destination change term]", 3888));
            /* FALLTHRU */
        }
        else
        {
            break;
        }
case 0xf31:  // "[targeting bonus]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[targeting bonus]", 3889));
            /* FALLTHRU */
        }
        else
        {
            break;
        }
case 0xf32:  // "[keep distance with target]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[keep distance with target]", 3890));
            /* FALLTHRU */
        }
        else
        {
            break;
        }
case 0xf33:  // "[warlike]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[warlike]", 3891));
            /* FALLTHRU */
        }
        else
        {
            break;
        }
case 0xf34:  // "[vision]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[vision]", 3892));
            /* FALLTHRU */
        }
        else
        {
            break;
        }
case 0xf35:  // "[intelligence]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[intelligence]", 3893));
            /* FALLTHRU */
        }
        else
        {
            break;
        }
case 0xf36:  // "[keep range distance with target]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[keep range distance with target]", 3894));
            /* FALLTHRU */
        }
        else
        {
            break;
        }
case 0xf37:  // "[back vision]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[back vision]", 3895));
            /* FALLTHRU */
        }
        else
        {
            break;
        }
case 0xf38:  // "[no change target]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[no change target]", 3896));
            /* FALLTHRU */
        }
        else
        {
            break;
        }
case 0xf39:  // duplicate unnumbered keep-range key in ORIG map
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[keep range distance with target]", 3897));
            /* FALLTHRU */
        }
        else
        {
            break;
        }
case 0xf3a:  // "[keep range distance with target 1]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[keep range distance with target 1]", 3898));
            /* FALLTHRU */
        }
        else
        {
            break;
        }
case 0xf3b:  // "[keep range distance with target 2]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[keep range distance with target 2]", 3899));
            /* FALLTHRU */
        }
        else
        {
            break;
        }
case 0xf3c:  // "[keep range distance with target 3]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[keep range distance with target 3]", 3900));
            /* FALLTHRU */
        }
        else
        {
            break;
        }
case 0xf3d:  // "[keep range distance with target 4]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[keep range distance with target 4]", 3901));
            /* FALLTHRU */
        }
        else
        {
            break;
        }
case 0xf3e:  // "[keep range distance with target 5]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[keep range distance with target 5]", 3902));
            /* FALLTHRU */
        }
        else
        {
            importAIInfoScript(&script->m_aiInfo, &tag, true);
            break;
        }

case 0xf46:  // "[reflection object]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[reflection object]", 3910));
            /* FALLTHRU */
        }
        else
        {
        script->m_fieldc8c.clear();
        script->m_fieldc80.clear();
        for (;;)
        {
            int first = ScanInt(&ok);
            if (!ok)
                break;
            int second = ScanInt(static_cast<bool*>(0));
            script->m_fieldc8c.push_back(first);
            script->m_fieldc80.push_back(second);
        }
            break;
        }

case 0xf57:  // "[set damage check]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[set damage check]", 3927));
            /* FALLTHRU */
        }
        else
        {
            script->m_field380 = ScanInt(static_cast<bool*>(0));
            break;
        }

case 0xf5c:  // "[change direction by target]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[change direction by target]", 3932));
            /* FALLTHRU */
        }
        else
        {
            script->m_flag33c = ScanInt(&ok) != 0;  // +0x33c（asm movb %dl 实证）
            break;
        }

case 0xf62:  // "[is direction to move]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[is direction to move]", 3938));
            /* FALLTHRU */
        }
        else
        {
            script->m_field37c = ScanInt(static_cast<bool*>(0));
            break;
        }

case 0xf67:  // "[absolute damage resistance rate]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[absolute damage resistance rate]", 3943));
            /* FALLTHRU */
        }
        else
        {
            script->m_fieldc54 = ScanFloat(static_cast<bool*>(0));
            break;
        }

case 0xf6c:  // "[hp regen rate]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[hp regen rate]", 3948));
            /* FALLTHRU */
        }
        else
        {
            script->m_field540 = ScanFloat(static_cast<bool*>(0));
            script->m_field544 = ScanFloat(static_cast<bool*>(0));
            break;
        }

case 0xf72:  // "[absolute defense ignore rate]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[absolute defense ignore rate]", 3954));
            /* FALLTHRU */
        }
        else
        {
            script->m_fieldc58 = ScanFloat(static_cast<bool*>(0));
            break;
        }

case 0xf76:  // "[lie frame]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[lie frame]", 3958));
            /* FALLTHRU */
        }
        else
        {
        for (int i = 0; i < 5; ++i)
            script->m_fieldc5c[i] = ScanInt(static_cast<bool*>(0));
            break;
        }

case 0xf7b:  // "[forced knockback]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[forced knockback]", 3963));
            /* FALLTHRU */
        }
        else
        {
            script->m_field1cac = ScanInt(&ok);  // +0x1cac
            break;
        }

case 0xf80:  // "[No Combo Rate Monster]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[No Combo Rate Monster]", 3968));
            /* FALLTHRU */
        }
        else
        {
            script->m_field1ca8 = true;
            break;
        }

case 0xf84:  // "[proc action]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[proc action]", 3972));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_procAction);  // +0x648
            break;
        }

case 0xf87:  // "[monster title]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[monster title]", 3975));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_monsterTitle);  // +0x694
            break;
        }

case 0xf8a:  // "[destroy count]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[destroy count]", 3978));
            /* FALLTHRU */
        }
        else
        {
            script->m_field6f4 = ScanInt(&ok);  // +0x6f4
            break;
        }

case 0xf8d:  // "[destroy count damage]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[destroy count damage]", 3981));
            /* FALLTHRU */
        }
        else
        {
            script->m_field6f8 = ScanInt(&ok);  // +0x6f8
            break;
        }

case 0xf99:  // "[overhead gauge type]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[overhead gauge type]", 3993));
            /* FALLTHRU */
        }
        else
        {
            script->m_fieldc74 = ScanInt(&ok);  // +0xc74
            break;
        }

case 0xf9d:  // "[overhead gauge pos]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[overhead gauge pos]", 3997));
            /* FALLTHRU */
        }
        else
        {
            script->m_fieldc78 = ScanInt(&ok);  // +0xc78
            script->m_fieldc7c = ScanInt(&ok);  // +0xc7c
            break;
        }

case 0xfa2:  // "[flying mark]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[flying mark]", 4002));
            /* FALLTHRU */
        }
        else
        {
        {
            const int value = ScanInt(static_cast<bool*>(0));
            script->m_flagc70 = true;
            script->m_flagc71 = (value == 1);
        }
            break;
        }

case 0xfa7:  // "[moveback action]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[moveback action]", 4007));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_movebackAction);  // +0x604
            break;
        }

case 0xfaa:  // "[dash action]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[dash action]", 4010));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_dashAction);  // +0x608
            break;
        }

case 0xfad:  // "[dashattack action]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[dashattack action]", 4013));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_dashattackAction);  // +0x60c
            break;
        }

case 0xfb0:  // "[burning death path]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[burning death path]", 4016));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_field098);  // +0x98
            break;
        }

case 0xfb3:  // "[initial target]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[initial target]", 4019));
            /* FALLTHRU */
        }
        else
        {
        {
            STInitialTarget target;
            for (;;)
            {
                std::string sub;
                if (!ScanType(sub, true))
                    break;
                if (sub == "[/initial target]")
                    break;
                if (sub == "[initial target Permanent]")
                {
                    target.m_field0c =
                        (ScanInt(static_cast<bool*>(0)) == 1);
                }
                else
                {
                    target.m_field04 = sub;
                    target.m_field00 = ScanInt(static_cast<bool*>(0));
                }
            }
            script->m_initialTargets.push_back(target);
        }
            break;
        }

case 0xfde:  // "[level of difficulty]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[level of difficulty]", 4062));
            /* FALLTHRU */
        }
        else
        {
        {
            STDifficulty difficulty;
            bool isType = false;
            while (ScanTypeOrStr(isType, token, true))
            {
                if (!isType)
                    continue;
                if (token == "[/level of difficulty]")
                    break;
                // ORIG 08a1e041/084/0c7/107/147：五个 tag 各为**独立分支**
                // （每分支 2×ScanInt(0) 写 -0x1b5c/-0x1b58），汇合于 08a1e185 的
                // push_back；合并条件只发 2 次 ScanInt（p167 差集 −8 的根因）
                if (token == "[easy]")
                {
                    difficulty.m_field00 = ScanInt(static_cast<bool*>(0));
                    difficulty.m_field04 = ScanInt(static_cast<bool*>(0));
                }
                else if (token == "[medium]")
                {
                    difficulty.m_field00 = ScanInt(static_cast<bool*>(0));
                    difficulty.m_field04 = ScanInt(static_cast<bool*>(0));
                }
                else if (token == "[hard]")
                {
                    difficulty.m_field00 = ScanInt(static_cast<bool*>(0));
                    difficulty.m_field04 = ScanInt(static_cast<bool*>(0));
                }
                else if (token == "[ultimate]")
                {
                    difficulty.m_field00 = ScanInt(static_cast<bool*>(0));
                    difficulty.m_field04 = ScanInt(static_cast<bool*>(0));
                }
                else if (token == "[hero]")
                {
                    difficulty.m_field00 = ScanInt(static_cast<bool*>(0));
                    difficulty.m_field04 = ScanInt(static_cast<bool*>(0));
                }
                script->m_difficulties.push_back(difficulty);
            }
        }
            break;
        }

case 0x1020:  // "[fire attack]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[fire attack]", 4128));
            /* FALLTHRU */
        }
        else
        {
            script->m_paramTable[0][0] = ScanInt(&ok);  // +0x508
            script->m_paramTable[0][1] = ScanInt(&ok);  // +0x50c
            break;
        }

case 0x1024:  // "[water attack]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[water attack]", 4132));
            /* FALLTHRU */
        }
        else
        {
            script->m_paramTable[1][0] = ScanInt(&ok);  // +0x510
            script->m_paramTable[1][1] = ScanInt(&ok);  // +0x514
            break;
        }

case 0x1028:  // "[dark attack]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[dark attack]", 4136));
            /* FALLTHRU */
        }
        else
        {
            script->m_paramTable[2][0] = ScanInt(&ok);  // +0x518
            script->m_paramTable[2][1] = ScanInt(&ok);  // +0x51c
            break;
        }

case 0x102c:  // "[light attack]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[light attack]", 4140));
            /* FALLTHRU */
        }
        else
        {
            script->m_paramTable[3][0] = ScanInt(&ok);  // +0x520
            script->m_paramTable[3][1] = ScanInt(&ok);  // +0x524
            break;
        }

case 0x1030:  // "[rigidity]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[rigidity]", 4144));
            /* FALLTHRU */
        }
        else
        {
            script->m_field528[0] = ScanInt(&ok);  // +0x528
            script->m_field528[1] = ScanInt(&ok);  // +0x52c
            break;
        }

case 0x1035:  // "[drop avatar item]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[drop avatar item]", 4149));
            /* FALLTHRU */
        }
        else
        {
        {
            for (int i = 0; i < 4; ++i)
            {
                float rate = ScanFloat(&ok);
                if (!ok)
                    return 0;
                script->m_dropAvatarInfo.m_field08[i] = rate;   // [b28d-solo] 同型 float 直赋
            }
            for (int i = 4; i < 9; ++i)
            {
                float rate = ScanFloat(&ok);
                if (!ok)
                    return 0;
                script->m_dropAvatarInfo.m_field08[i] = rate;
            }
            script->m_dropAvatarInfo.m_field00 = ScanInt(&ok);
            if (!ok)
                return 0;
            script->m_dropAvatarInfo.m_field04 = ScanInt(&ok);
            if (!ok)
                return 0;
            for (;;)
            {
                stEnamyDropAvatarElement_t element;
                element.m_field0 = ScanInt(&ok);
                if (!ok)
                    break;
                element.m_field4 = ScanInt(&ok);
                if (!ok)
                    return 0;
                element.m_field8 = ScanInt(&ok);
                if (!ok)
                    return 0;
                script->m_dropAvatarInfo.m_field2c += element.m_field4;
                element.m_field4 = script->m_dropAvatarInfo.m_field2c;
                script->m_dropAvatarInfo.m_elements.push_back(element);
            }
        }
            break;
        }

case 0x1060:  // "[pattern]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[pattern]", 4192));
            /* FALLTHRU */
        }
        else
        {
        {
            std::vector<int> pattern;
            for (;;)
            {
                int value = ScanInt(&ok);
                if (!ok)
                    break;
                pattern.push_back(value);
            }
            if (!pattern.empty())
                script->m_patterns.push_back(pattern);
        }
            break;
        }

case 0x106d:  // "[fix direction]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[fix direction]", 4205));
            /* FALLTHRU */
        }
        else
        {
        {
            bool isString = false;
            bool isInt = false;
            std::string direction;
            const int value = ScanStrOrInt(isString, direction, isInt, true);
            if (!isString)
            {
                script->m_flagcd4 = (value != 0);
            }
            else
            {
                script->m_flagcd4 = true;
                int fixDirKind = -1;
                for (;;)
                {
                    static std::map<const char*, int, less_TCHAR> __lookup;
                    static SpinLock __init_lock;
                    static bool __inited = false;
                    if (!__inited)
                    {
                        __init_lock.enter();
                        if (__inited)
                        {
                            __init_lock.leave();
                            continue;
                        }
                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[left]", 4219));
                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[right]", 4222));
                        __init_lock.leave();
                        __inited = true;
                        continue;
                    }
                    std::map<const char*, int, less_TCHAR>::const_iterator __it = __lookup.find(direction.c_str());
                    if (__it != __lookup.end())
                        fixDirKind = __it->second;
                    break;
                }
                // 好 .o f349/f366：left→+0xcd8=0；right→+0xcd8=1；无 else 分支
                if (fixDirKind == 4219)
                    script->m_fieldcd8 = 0;
                else if (fixDirKind == 4222)
                    script->m_fieldcd8 = 1;
            }
        }
            break;
        }

case 0x108b:  // "[ridableskill definition]"（收集父 case；ORIG asm 08a1ebdb：
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[ridableskill definition]", 4235));
            /* FALLTHRU */
        }
        else
        {
            //  STRidableSkill sk(-0x1d24) + do{ do{ScanTypeOrStr}while(!isStr)
            //    __lookup_10(4 键) 分派；[skill]→string+__lookup_11(6 键 type 表) }while(!closed)
            //    push m_ridableSkills(+0x6f4)）
            {
                std::string token2;                    // local_1b90
                int closed = 0;                        // local_78
                STRidableSkill sk;                     // -0x1d24
                do
                {
                    bool isStr = false;
                    while (true)
                    {
                        if (!ScanTypeOrStr(isStr, token2, true))
                            // [b29-SweepSTMonsterScript2 真 bug] ORIG 0x8a1ec89
                            // `jne 8a1f765` 跳到「析构 sk/token2 + jmp case 尾」，
                            // **不 push**（push 在循环体内正常出口 0x8a1ef41，
                            // 随后 `movb $0x1,-0x6d`=closed=1 退出）。旧实现
                            // `goto rid_def_exit` 落在 push 之前 ⇒ 读失败也 push
                            // 半成品 sk（多一条 ridable skill）。
                            goto rid_def_fail;         // 读失败 → 丢弃（不 push）
                        if (isStr)
                            break;
                    }
                    bool doneLine = false;             // local_71
                    int subId10;
                    while (true)
                    {
                        static std::map<const char*, int, less_TCHAR> __lookup;
                        static SpinLock __init_lock;
                        static bool __inited = false;
                        if (__inited)
                        {
                            std::map<const char*, int, less_TCHAR>::const_iterator __it =
                                __lookup.find(token2.c_str());
                            if (__it != __lookup.end())
                                subId10 = __it->second;
                            else
                                subId10 = -1;
                        }
                        else
                        {
                            __init_lock.enter();
                            if (__inited)
                            {
                                __init_lock.leave();
                                continue;
                            }
                            subId10 = 0;
                        }
                        switch (subId10)
                        {
                        case 0:
                        case 4254:  // [/ridableskill definition]
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[/ridableskill definition]", 4254));
                                goto sub10_4260;
                            }
                        doneLine = true;
                        break;
                        sub10_4260:
                        case 4260:  // [pattern] → sk +0x00
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[pattern]", 4260));
                                goto sub10_4263;
                            }
                        sk.m_field00 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub10_4263:
                        case 4263:  // [image path] → sk +0x04
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[image path]", 4263));
                                goto sub10_4266;
                            }
                        ScanStr(&sk.m_name);
                        break;
                        sub10_4266:
                        case 4266:  // [skill]：8 组（2×int + type 词）
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[skill]", 4266));
                                goto end10;
                            }
                    {
                        for (int i = 0; i < 8; ++i)
                        {
                            sk.m_field08[i] = ScanInt(static_cast<bool*>(0));
                            sk.m_fielda8[i] = ScanInt(static_cast<bool*>(0));
                            std::string ty;            // local_1b9c：clear 后读
                            ty.clear();
                            ScanStr(&ty);
                            sk.m_field88[i] = 6;
                            int subId11;
                            while (true)
                            {
                                static std::map<const char*, int, less_TCHAR> __lookup;
                                static SpinLock __init_lock;
                                static bool __inited = false;
                                if (__inited)
                                {
                                    std::map<const char*, int, less_TCHAR>::const_iterator __it =
                                        __lookup.find(ty.c_str());
                                    if (__it != __lookup.end())
                                        subId11 = __it->second;
                                    else
                                        subId11 = -1;
                                }
                                else
                                {
                                    __init_lock.enter();
                                    if (__inited)
                                    {
                                        __init_lock.leave();
                                        continue;
                                    }
                                    subId11 = 0;
                                }
                                switch (subId11)
                                {
                                case 0:
                                case 4282:                                    // normal
                                    if (!__inited)
                                    {
                                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("normal", 4282));
                                        goto sub11_4287;
                                    }
                                    sk.m_field88[i] = 0;
                                    break;
                                sub11_4287:
                                case 4287:                                    // defaultcancel
                                    if (!__inited)
                                    {
                                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("defaultcancel", 4287));
                                        goto sub11_4292;
                                    }
                                    sk.m_field88[i] = 1;
                                    break;
                                sub11_4292:
                                case 4292:                                    // movable
                                    if (!__inited)
                                    {
                                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("movable", 4292));
                                        goto sub11_4297;
                                    }
                                    sk.m_field88[i] = 2;
                                    break;
                                sub11_4297:
                                case 4297:                                    // charge
                                    if (!__inited)
                                    {
                                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("charge", 4297));
                                        goto sub11_4302;
                                    }
                                    sk.m_field88[i] = 3;
                                    break;
                                sub11_4302:
                                case 4302:                                    // evasion
                                    if (!__inited)
                                    {
                                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("evasion", 4302));
                                        goto sub11_4307;
                                    }
                                    sk.m_field88[i] = 4;
                                    break;
                                sub11_4307:
                                case 4307:                                    // super
                                    if (!__inited)
                                    {
                                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("super", 4307));
                                        goto end11;
                                    }
                                    sk.m_field88[i] = 5;
                                    break;
                                default:
                                    break;
                                }
                            end11:;
                                if (!__inited)
                                {
                                    __init_lock.leave();
                                    __inited = true;
                                    continue;
                                }
                                break;
                            }
                        // ORIG 0x8a1f5fc 公共尾：按刚写入的类型值附加读取——
                        //  v∈{0,1,2,4,5}(1<<v & 0x37)：n=ScanInt，随后 n 个 ScanInt
                        //    逐个 push 进 sk.m_levels[i]（+0x28+i*12 vector<int>）；
                        //  v==3(charge)：m_fieldc8[i]/m_fielde8[i] 各一 ScanInt；
                        //  v==6（未识别/默认）不读。
                        {
                            const int v = sk.m_field88[i];
                            if (v <= 5)
                            {
                                const unsigned bit = 1u << v;
                                if ((bit & 0x37u) != 0)
                                {
                                    const int n = ScanInt(static_cast<bool*>(0));
                                    for (int j = 0; j < n; ++j)
                                        sk.m_levels[i].push_back(ScanInt(static_cast<bool*>(0)));
                                }
                                else if ((bit & 8u) != 0)
                                {
                                    sk.m_fieldc8[i] = ScanInt(static_cast<bool*>(0));
                                    sk.m_fielde8[i] = ScanInt(static_cast<bool*>(0));
                                }
                            }
                        }
                        }
                    }
                        break;
                        default:
                            break;
                        }
                        end10:;
                    if (!__inited)
                    {
                        __init_lock.leave();
                        __inited = true;
                        continue;
                    }
                    break;
                    }
                    if (doneLine)
                        closed = 1;
                } while (closed == 0);
                script->m_ridableSkills.push_back(sk);
            rid_def_fail:;   // 读失败落点：跳过 push（ORIG 0x8a1f765 同）
            }
            break;
        }

case 0x1146:  // "[limit slowdown atkspeed]"（ORIG 08a1f7bc：ScanFloat → m_fieldcc0）
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[limit slowdown atkspeed]", 4422));
            /* FALLTHRU */
        }
        else
        {
            script->m_field1c94 = ScanInt(static_cast<bool*>(0));
            break;
        }

case 0x1149:  // "[limit slowdown movespeed]"（ScanFloat → m_fieldcc4）
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[limit slowdown movespeed]", 4425));
            /* FALLTHRU */
        }
        else
        {
            script->m_field1c98 = ScanInt(static_cast<bool*>(0));
            break;
        }

case 0x114c:  // "[ridableskill explanation]"（ORIG 08a1f8aa：STDifficulty 未用局部 +
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[ridableskill explanation]", 4428));
            /* FALLTHRU */
        }
        else
        {
            //  STRidableSkillExplain exp + ScanTypeOrStr 串词分派 → push +0xce8）
            {
                STDifficulty unused;                   // local_1ba8：ORIG 构造不使用
                (void)unused;
                std::string word;                      // local_1bac
                STRidableSkillExplain exp;             // -0x1bc0
                while (true)
                {
                    bool isStr = false;
                    if (!ScanTypeOrStr(isStr, word, true))
                        break;
                    if (!isStr)
                        continue;
                    if (word == "[/ridableskill explanation]")
                        break;
                    if (word == "[title]")
                        ScanStr(&exp.m_field00);
                    else if (word == "[subtitle]")
                        ScanStr(&exp.m_field04);
                    else if (word == "[category]")
                        ScanStr(&exp.m_field08);
                    else if (word == "[detail]")
                        ScanStr(&exp.m_field0c);
                    else if (word == "[pattern]")
                        exp.m_field10 = ScanInt(static_cast<bool*>(0));
                }
                script->m_ridableSkillExplains.push_back(exp);
            }
            break;
        }

case 0x1177:  // "[physical attack besed on diff]"（ORIG 08a1faf4：200×5 float 表 +0xcf4，
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[physical attack besed on diff]", 4471));
            /* FALLTHRU */
        }
        else
        {
            //  diff-1 定行、ScanFloat 失败或表满置 ended）
            {
                std::string unused;                    // local_1bc8：ORIG 构造不使用
                (void)unused;
                bool ended = false;
                for (int i = 0; i < 200 && !ended; ++i)
                {
                    const int diff = ScanInt(&ok);
                    if (!ok)
                    {
                        ended = true;
                        break;
                    }
                    for (int j = 0; j < 5; ++j)
                    {
                        const float v = ScanFloat(&ok);
                        script->m_difficultyScales[diff - 1][j] = v;
                        if (!ok)
                        {
                            ended = true;
                            break;
                        }
                    }
                }
            }
            break;
        }

case 0x1194:  // "[non targeted]"（ScanInt 非 0 → m_field56c byte 1/0）
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[non targeted]", 4500));
            /* FALLTHRU */
        }
        else
        {
            script->m_field56c = (ScanInt(static_cast<bool*>(0)) != 0);
            break;
        }

case 0x1197:  // "[target appendage]"（ScanStr 直读 string +0x570）
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[target appendage]", 4503));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_field570);
            break;
        }

case 0x119a:  // "[damage time balance]"（4×ScanInt(&ok) push vector +0x554）
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[damage time balance]", 4506));
            /* FALLTHRU */
        }
        else
        {
            for (int i = 0; i <= 3; ++i)          // ORIG 08a1fd87 cmp $3,setle
                script->m_field554.push_back(ScanInt(static_cast<bool*>(0)));
            break;
        }

case 0x11a0:  // "[stun time balance]"（4× push +0x560）
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[stun time balance]", 4512));
            /* FALLTHRU */
        }
        else
        {
            for (int i = 0; i <= 3; ++i)          // ORIG 同族 i<=3 形
                script->m_field560.push_back(ScanInt(static_cast<bool*>(0)));
            break;
        }

case 0x11a7:  // "[add gravity in aerial]"（ORIG 08a1fe7c：bool +0x548 + float×2 +0x54c/550）
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[add gravity in aerial]", 4519));
            /* FALLTHRU */
        }
        else
        {
            script->m_field548 = (ScanInt(static_cast<bool*>(0)) != 0);
            script->m_field54c = ScanFloat(static_cast<bool*>(0));
            script->m_field550 = ScanFloat(static_cast<bool*>(0));
            break;
        }

case 0x11ae:  // "[summon skill status up type]"（ScanInt(0) → m_field3c0）
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[summon skill status up type]", 4526));
            /* FALLTHRU */
        }
        else
        {
            script->m_field3c0 = ScanInt(static_cast<bool*>(0));
            break;
        }

case 0x11b1:  // "[attack down prob]"（ScanFloat(&ok) 循环写 m_attackMethods[i].+4）
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[attack down prob]", 4529));
            /* FALLTHRU */
        }
        else
        {
            {
                int i = 0;
                while (true)
                {
                    const float v = ScanFloat(&ok);
                    if (!ok)
                        break;
                    script->m_attackMethods[i].m_field04 = v;
                    ++i;
                }
            }
            break;
        }

case 0x11c6:  // "[strong]"（ScanInt(0)→m_field1ca0；ScanFloat(0)→m_field1ca4）
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[strong]", 4550));
            /* FALLTHRU */
        }
        else
        {
            script->m_field1ca0 = ScanInt(static_cast<bool*>(0));
            script->m_field1ca4 = ScanFloat(static_cast<bool*>(0));
            break;
        }

case 0x11ca:  // "[weak]"（取负写同槽 + float）
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[weak]", 4554));
            /* FALLTHRU */
        }
        else
        {
            script->m_field1ca0 = -ScanInt(static_cast<bool*>(0));
            script->m_field1ca4 = ScanFloat(static_cast<bool*>(0));
            break;
        }

case 0x11cf:  // "[explain]"（ScanStr 直读 string +0x6d0）
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[explain]", 4559));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_explain);
            break;
        }

case 0x11d4:  // "[consume mp]"（ORIG 08a201dc：ScanInt(&ok) 循环 push vector +0x120）
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[consume mp]", 4564));
            /* FALLTHRU */
        }
        else
        {
            {
                int v = 0;
                while ((v = ScanInt(&ok), ok == true))
                    script->m_field120.push_back(v);
            }
            break;
        }

case 0x11e6:  // "[base table]"（ORIG 08a2027e：ScanStr 路径 → clear +0x12c →
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[base table]", 4582));
            /* FALLTHRU */
        }
        else
        {
            //  ImportMonsterBaseTable(script,c_str)；成功且 size()==0 → return 0）
            {
                std::string path2;                     // local_1bd8
                if (ScanStr(&path2) == true)
                {
                    script->m_baseParameters.clear();  // +0x12c
                    if (ImportMonsterBaseTable(script, path2.c_str()))
                    {
                        if (script->m_baseParameters.size() == 0)
                            return 0;
                    }
                    else
                        return 0;
                }
            }
            break;
        }

case 0x11ff:  // "[catch item]"（ORIG 08a203c8：+0x64 三字段暂存后按 CatchItemInfo
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[catch item]", 4607));
            /* FALLTHRU */
        }
        else
        {
            //  push 至 +0x70 vector；随后一次 ScanType 消耗尾词）
            {
                CatchItemInfo ci;
                ci.m_field0 = ScanInt(static_cast<bool*>(0));
                ci.m_field4 = ScanInt(static_cast<bool*>(0));
                ci.m_field8 = ScanFloat(static_cast<bool*>(0));
                std::string tail;                      // local_1bdc：ScanType 消耗
                ScanType(tail, true);
                script->m_catchItems.push_back(ci);
            }
            break;
        }

case 0x1213:  // "[fixed height]"（ScanInt(0) → m_field1cb0）
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[fixed height]", 4627));
            /* FALLTHRU */
        }
        else
        {
            script->m_field1cb0 = ScanInt(static_cast<bool*>(0));
            break;
        }

case 0x1218:  // "[etc sound]"（ScanStr 成功则 m_field6b0 = 词）
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[etc sound]", 4632));
            /* FALLTHRU */
        }
        else
        {
            {
                std::string snd;                       // local_1be0
                if (ScanStr(&snd))
                    script->m_etcSound = snd;
            }
            break;
        }

case 0x1220:  // "[can beat index]"（ORIG 08a20609：ScanTypeOrInt 循环 push +0x1cb4；
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[can beat index]", 4640));
            /* FALLTHRU */
        }
        else
        {
            //  非 int 词：仅 "[/can beat index]" 继续，否则整体 return 0）
            {
                bool isInt = false;
                bool isStr = false;
                std::string word;                      // local_1be8
                bool ended = false;
                while (true)
                {
                    const int v = ScanTypeOrInt(isStr, word, isInt, true);
                    if (!isInt)
                        break;
                    if (isStr)
                    {
                        if (word == "[/can beat index]")
                            ended = true;
                        break;
                    }
                    script->m_field1cb4.push_back(v);
                }
                if (!ended)
                    return 0;
            }
            break;
        }

case 0x1239:  // "[mounting action]"（ScanStr→token，赋 +0x64c）
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[mounting action]", 4665));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&token);
            script->m_mountingAction = token;
            break;
        }

case 0x123f:  // "[mounted action]"（赋 +0x650）
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[mounted action]", 4671));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&token);
            script->m_mountedAction = token;
            break;
        }

case 0x1245:  // "[unmounting action]"（赋 +0x654）
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[unmounting action]", 4677));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&token);
            script->m_unmountingAction = token;
            break;
        }

case 0x124b:  // "[control passive object]"（ORIG 08a208ff：do{ScanType→__lookup_12
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[control passive object]", 4683));
            /* FALLTHRU */
        }
        else
        {
            //  11 键；[option]→ScanStr+__lookup_13；[transparency]→词循环 __lookup_14/15；
            //  [fix direction]→ScanStr+__lookup_16；[/control passive object]→退出}
            //  while(ScanType 成功)）
            //  ORIG 08a2095f..08a22096：增量链式建表（case 0 哨兵 + 每 leaf
            //  if(!__inited_12){insert 本键; jmp 下 leaf}；建轮 continue 跳 guard
            //  08a209a9 而非 loop head 08a2096f）。
            {
                std::string tag2;                      // local_1bf0
                bool ended = false;                    // local_1bf5
                do
                {
                    ended = ScanType(tag2, true);
                    if (ended != true)
                        return 0;
                    int subId12;                       // ORIG -0x30
                    while (true)
                    {
                        static std::map<const char*, int, less_TCHAR> __lookup;
                        static SpinLock __init_lock;
                        static bool __inited = false;
                            if (__inited)
                        {
                            std::map<const char*, int, less_TCHAR>::const_iterator __it =
                                __lookup.find(tag2.c_str());
                        if (__it != __lookup.end())
                            subId12 = __it->second;
                            else
                                subId12 = -1;
                        }
                        else
                        {
                            __init_lock.enter();
                            if (__inited)
                            {
                                __init_lock.leave();
                                continue;                // ORIG 08a20b8d jmp 08a209a9
                            }
                            subId12 = 0;                // ORIG 08a20b92
                        }
                        {
                    switch (subId12)
                    {
                    case 0:
                    case 4696:  // [index] → +0x658
                        if (!__inited)
                        {
                            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[index]", 4696));
                            goto sub12_4704;             // ORIG 08a20c86 链
                        }
                    {
                        const int v = ScanInt(&ok);
                        if (!ok) return 0;
                        script->m_field658 = v;
                    }
                        break;
                    sub12_4704:
                    case 4704:  // [x pos] → +0x65c
                        if (!__inited)
                        {
                            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[x pos]", 4704));
                            goto sub12_4712;
                        }
                    {
                        const int v = ScanInt(&ok);
                        if (!ok) return 0;
                        script->m_field65c = v;
                    }
                        break;
                    sub12_4712:
                    case 4712:  // [y pos] → +0x660
                        if (!__inited)
                        {
                            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[y pos]", 4712));
                            goto sub12_4720;
                        }
                    {
                        const int v = ScanInt(&ok);
                        if (!ok) return 0;
                        script->m_field660 = v;
                    }
                        break;
                    sub12_4720:
                    case 4720:  // [width] → +0x664
                        if (!__inited)
                        {
                            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[width]", 4720));
                            goto sub12_4728;
                        }
                    {
                        const int v = ScanInt(&ok);
                        if (!ok) return 0;
                        script->m_field664 = v;
                    }
                        break;
                    sub12_4728:
                    case 4728:  // [height] → +0x668
                        if (!__inited)
                        {
                            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[height]", 4728));
                            goto sub12_4736;
                        }
                    {
                        const int v = ScanInt(&ok);
                        if (!ok) return 0;
                        script->m_field668 = v;
                    }
                        break;
                    sub12_4736:
                    case 4736:  // [velocity x] → float +0x66c
                        if (!__inited)
                        {
                            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[velocity x]", 4736));
                            goto sub12_4744;
                        }
                    {
                        const float f = ScanFloat(&ok);
                        if (!ok) return 0;
                        script->m_field66c = f;
                    }
                        break;
                    sub12_4744:
                    case 4744:  // [velocity y] → float +0x670
                        if (!__inited)
                        {
                            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[velocity y]", 4744));
                            goto sub12_4752;
                        }
                    {
                        const float f = ScanFloat(&ok);
                        if (!ok) return 0;
                        script->m_field670 = f;
                    }
                        break;
                    sub12_4752:
                    case 4752:  // [option] → ScanStr + convert y to z
                        if (!__inited)
                        {
                            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[option]", 4752));
                            goto sub12_4771;
                        }
                    {
                        std::string opt;               // local_1c00
                        bool okflag = false;
                        if (ScanStr(&opt))
                        {
                            int subId13;
                            while (true)
                            {
                                static std::map<const char*, int, less_TCHAR> __lookup;
                                static SpinLock __init_lock;
                                static bool __inited = false;
                                if (__inited)
                                {
                                    std::map<const char*, int, less_TCHAR>::const_iterator __it =
                                        __lookup.find(opt.c_str());
                                    if (__it != __lookup.end())
                                        subId13 = __it->second;
                                    else
                                        subId13 = -1;
                                }
                                else
                                {
                                    __init_lock.enter();
                                    if (__inited)
                                    {
                                        __init_lock.leave();
                                        continue;
                                    }
                                    subId13 = 0;
                                }
                                switch (subId13)
                                {
                                case 0:
                                case 4760:      // convert y to z → +0x674 = 1
                                    if (!__inited)
                                    {
                                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("convert y to z", 4760));
                                        goto end13;
                                    }
                                    script->m_field674 = 1;
                                    okflag = true;
                                    break;
                                default:
                                    break;
                                }
                            end13:;
                                if (!__inited)
                                {
                                    __init_lock.leave();
                                    __inited = true;
                                    continue;
                                }
                                break;
                            }
                        }
                        if (!okflag)
                            return 0;
                    }
                        break;
                    sub12_4771:
                    case 4771:  // [transparency] → 词循环
                        if (!__inited)
                        {
                            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[transparency]", 4771));
                            goto sub12_4823;
                        }
                    {
                        std::string word;              // local_1c08
                        bool done = false;
                        while (ScanType(word, true))
                        {
                            if (word == "[/transparency]")
                            {
                                done = true;
                                break;
                            }
                            int subId14;
                            while (true)
                            {
                                static std::map<const char*, int, less_TCHAR> __lookup;
                                static SpinLock __init_lock;
                                static bool __inited = false;
                                if (__inited)
                                {
                                    std::map<const char*, int, less_TCHAR>::const_iterator __it =
                                        __lookup.find(word.c_str());
                                    if (__it != __lookup.end())
                                        subId14 = __it->second;
                                    else
                                        subId14 = -1;
                                }
                                else
                                {
                                    __init_lock.enter();
                                    if (__inited)
                                    {
                                        __init_lock.leave();
                                        continue;
                                    }
                                    subId14 = 0;
                                }
                                switch (subId14)
                                {
                                case 0:
                                case 4784:      // [master] → int +0x678
                                    if (!__inited)
                                    {
                                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[master]", 4784));
                                        goto sub14_4792;
                                    }
                                    {
                                        int mv;                    // ORIG -0x1bf0（ScanInt(int*) 直写）
                                        if (!ScanInt(&mv))
                                            return 0;
                                        script->m_field678 = mv;
                                    }
                                    break;
                                sub14_4792:
                                case 4792:      // [others] → int +0x67c
                                    if (!__inited)
                                    {
                                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[others]", 4792));
                                        goto sub14_4800;
                                    }
                                    {
                                        int mv;                    // ORIG -0x1bf0（ScanInt(int*) 直写）
                                        if (!ScanInt(&mv))
                                            return 0;
                                        script->m_field67c = mv;
                                    }
                                    break;
                                sub14_4800:
                                case 4800:      // [option] → ScanStr + inherit → +0x680
                                    if (!__inited)
                                    {
                                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[option]", 4800));
                                        goto end14;
                                    }
                                    {
                                        std::string opt2;      // local_1c10
                                        bool okflag = false;
                                        if (ScanStr(&opt2))
                                        {
                                            int subId15;
                                            while (true)
                                            {
                                                static std::map<const char*, int, less_TCHAR> __lookup;
                                                static SpinLock __init_lock;
                                                static bool __inited = false;
                                                if (__inited)
                                                {
                                                    std::map<const char*, int, less_TCHAR>::const_iterator __it =
                                                        __lookup.find(opt2.c_str());
                                                    if (__it != __lookup.end())
                                                        subId15 = __it->second;
                                                    else
                                                        subId15 = -1;
                                                }
                                                else
                                                {
                                                    __init_lock.enter();
                                                    if (__inited)
                                                    {
                                                        __init_lock.leave();
                                                        continue;
                                                    }
                                                    subId15 = 0;
                                                }
                                                switch (subId15)
                                                {
                                                case 0:
                                                case 4807:
                                                    if (!__inited)
                                                    {
                                                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("inherit", 4807));
                                                        goto end15;
                                                    }
                                                    script->m_field680 = 1;
                                                    okflag = true;
                                                    break;
                                                default:
                                                    break;
                                                }
                                            end15:;
                                                if (!__inited)
                                                {
                                                    __init_lock.leave();
                                                    __inited = true;
                                                    continue;
                                                }
                                                break;
                                            }
                                        }
                                        if (!okflag)
                                            return 0;
                                    }
                                    break;
                                default:
                                    return 0;
                                }
                            end14:;
                                if (!__inited)
                                {
                                    __init_lock.leave();
                                    __inited = true;
                                    continue;
                                }
                                break;
                            }
                        }
                        if (!done)
                            return 0;
                    }
                        break;
                    sub12_4823:
                    case 4823:  // [fix direction] → ScanStr + left/right
                        if (!__inited)
                        {
                            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[fix direction]", 4823));
                            goto sub12_4844;
                        }
                    {
                        std::string dir;               // local_1c18
                        bool okflag = false;
                        if (ScanStr(&dir))
                        {
                            int subId16;
                            while (true)
                            {
                                static std::map<const char*, int, less_TCHAR> __lookup;
                                static SpinLock __init_lock;
                                static bool __inited = false;
                                if (__inited)
                                {
                                    std::map<const char*, int, less_TCHAR>::const_iterator __it =
                                        __lookup.find(dir.c_str());
                                    if (__it != __lookup.end())
                                        subId16 = __it->second;
                                    else
                                        subId16 = -1;
                                }
                                else
                                {
                                    __init_lock.enter();
                                    if (__inited)
                                    {
                                        __init_lock.leave();
                                        continue;
                                    }
                                    subId16 = 0;
                                }
                                switch (subId16)
                                {
                                case 0:
                                case 4832:      // [left] → +0x684=0
                                    if (!__inited)
                                    {
                                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[left]", 4832));
                                        goto sub16_4836;
                                    }
                                    script->m_field684 = 0;
                                    okflag = true;
                                    break;
                                sub16_4836:
                                case 4836:      // [right] → +0x684=1
                                    if (!__inited)
                                    {
                                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[right]", 4836));
                                        goto end16;
                                    }
                                    script->m_field684 = 1;
                                    okflag = true;
                                    break;
                                default:
                                    break;
                                }
                            end16:;
                                if (!__inited)
                                {
                                    __init_lock.leave();
                                    __inited = true;
                                    continue;
                                }
                                break;
                            }
                        }
                        if (!okflag)
                            return 0;
                    }
                        break;
                    sub12_4844:
                    case 4844:  // [/control passive object] → 结束本 case
                        if (!__inited)
                        {
                            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[/control passive object]", 4844));
                            goto sub12_end;            // ORIG 08a2204e jmp 08a22057
                        }
                        ended = false;
                        break;
                    default:
                        break;
                    }
                    // ORIG：未命中键（含 -1）无动作，继续外层 do-while 读下一词
                        sub12_end:;
                        }
                        if (!__inited)
                        {
                            __init_lock.leave();       // ORIG 08a22065
                            __inited = true;           // ORIG 08a22071
                            continue;                  // ORIG 08a22078 jmp 08a209a9
                        }
                        break;
                    }
                } while (ended == true);
            }
            break;
        }

case 0x12f7:  // "[life max]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[life max]", 4855));
            /* FALLTHRU */
        }
        else
        {
            script->m_flag3b8 = ScanInt(&ok) != 0;
            break;
        }

case 0x12fb:  // "[start immune time]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[start immune time]", 4859));
            /* FALLTHRU */
        }
        else
        {
            script->m_field1cc0 = ScanInt(&ok);
            if (!ok)
                return 0;
            break;
        }

            default:
                // ORIG fallback for the 17 ACTIVESTATUS_TAG entries:
                // each tag writes the low/high difficulty pair in
                // m_linearAbilities[0..16].
                for (int i = 0; i < 17; ++i)
                {
                    if (tag == kMonsterActiveStatusTags[i])
                    {
                        script->m_linearAbilities[i][0] =
                            ScanInt(static_cast<bool*>(0));
                        script->m_linearAbilities[i][1] =
                            ScanInt(static_cast<bool*>(0));
                        break;
                    }
                }
                break;
        }
        if (!__inited)
        {
            __init_lock.leave();
            __inited = true;
            continue;
        }
        break;
    }
        if (flag && nameSeen)
            break;
    }
    script->m_index = reinterpret_cast<STScriptFileList*>(
        &g_stMonsterScriptFileList)->FindIndexByFullScan(path);
    return 1;
}

// [BigMonsterScript 2026-09-09] 6790/6791，gap=−1；p167 CLEAR。
// i486 路由已由 Main 落地（dnf_game_cxx.sh i486 组）：9 处 x87 比较由
// `fucomip/fstp(+fxch)` 翻回 ORIG 的 `fucompp/fnstsw/sahf`；本轮另修 3 处：
// ① 08a27756 目标循环缺 break（原码置位后 `jmp` 循环出口）；
// ② 08a2976b 位测试 else-if 段保留 register 提示（删提示反落槽 +2，P248 边界）；
// ③ 08a29d34 while 判据实读 m_difficultyScales[i][1]（`add $0xcf8`）且比较式
//    写 `x < 0`（探针 g2 复现 `fucompp/test $0x45,%ah/sete`）。
// 残余 −1 = ORIG `mov %ebx,%eax; mov %eax,%edx`（4.1 保留 eax→edx 中转）vs
// 4.4 折单段（N18a）⇒ N 封顶。ORIG 0x08a22520
bool exportMonsterScript(
    STMonsterScript* script, const char* path,
    std::map<std::string, std::vector<std::string> >& extra)
{
    static char saveFileName[0x100];                      // getScriptFileFullPath 缓冲
    getScriptFileFullPath(saveFileName, 0x100,
                          g_szMonsterScriptBaseDirectory, path);   // +0x2c
    FILE* file = fopen(saveFileName, "wb");               // +0x3a "wb"
    if (file == 0)
        return false;

    STMonsterScript defaultScript;                        // +0x5c 局部默认对象（Clear 全默认值）

    // ---- [name] +0x110：m_name != "" 时经 getTagByName(7,..) 导出 ----
    if (script->m_name != "")
    {
        const wchar_t* tag = 0;
        if (!script->m_altName.empty())
            tag = (const wchar_t*)script->m_altName.c_str();
        fprintf(file, "[name] %s\n",
                toTString(g_scriptStringManager_.getTagByName(
                    7, (const wchar_t*)script->m_name.c_str(),
                    tag, false)));
    }
    // ---- [face image] +0x5ac 非空 ----
    if (!script->m_faceImage.empty())
        fprintf(file, "[face image] `%s` %d\n\n",
                script->m_faceImage.c_str(), script->m_field5b0);
    // ---- [die effect]（默认对象比较）----
    if (script->m_dieEffect[0] != defaultScript.m_dieEffect[0] ||
        script->m_dieEffect[1] != defaultScript.m_dieEffect[1] ||
        script->m_dieEffect[2] != defaultScript.m_dieEffect[2])
        fprintf(file, "[die effect] %d %d %d\n",
                script->m_dieEffect[0], script->m_dieEffect[1],
                script->m_dieEffect[2]);
    // ---- [width] ----
    if (script->m_width != defaultScript.m_width ||
        script->m_height != defaultScript.m_height)
        fprintf(file, "[width] %d %d\n", script->m_width, script->m_height);
    // ---- [floating height] ----
    if (script->m_floatingHeight != defaultScript.m_floatingHeight)
        fprintf(file, "[floating height] %d\n", script->m_floatingHeight);
    // ---- [layer]（getDrawLayerName）----
    if (script->m_layer != defaultScript.m_layer)
        fprintf(file, "[layer] `%s`\n",
                getDrawLayerName((ENUM_DRAWLAYER)script->m_layer));
    fputc('\n', file);

    // ---- [category] +0x1c vector<int> 非空 ----
    if (!script->m_field01c.empty())
    {
        fwrite("[category]\n", 1, 0xb, file);
        for (unsigned int i = 0; i < script->m_field01c.size(); ++i)
        {
            ENUM_CATEGORY cat = (ENUM_CATEGORY)script->m_field01c[i];
            std::string str(getObjectCategoryString(cat));
            fprintf(file, "`%s`\n", str.c_str());
        }
        fwrite("[/category]\n\n", 1, 0xd, file);
    }
    // ---- 六声音段：ambient/attack/appear/damage/die 无条件，etc 非空 ----
    fprintf(file, "[ambient sound] `%s`\n", script->m_ambientSound.c_str());
    fprintf(file, "[attack sound] `%s`\n", script->m_attackSound.c_str());
    fprintf(file, "[appear sound] `%s`\n", script->m_appearSound.c_str());
    fprintf(file, "[damage sound] `%s`\n", script->m_damageSound.c_str());
    fprintf(file, "[die sound] `%s`\n", script->m_dieSound.c_str());
    if (!script->m_etcSound.empty())
        fprintf(file, "[etc sound] `%s`\n", script->m_etcSound.c_str());
    // ---- [ability category] +0x7c size != 0；内循环匹配 0x93f5640 表 14 类 ----
    if (script->m_parameterCategories.size() != 0)
    {
        static const char* const s_abilityCategoryName[14] = {
            "[ATTACK_SPEED]", "[MOVE_SPEED]", "[PHYSICAL_ATTACK]",
            "[PHYSICAL_DEFENSE]", "[MAGICAL_ATTACK]", "[MAGICAL_DEFENSE]",
            "[EQUIPMENT_PHYSICAL_ATTACK]", "[EQUIPMENT_PHYSICAL_DEFENSE]",
            "[EQUIPMENT_MAGICAL_ATTACK]", "[EQUIPMENT_MAGICAL_DEFENSE]",
            "[HP MAX]", "[HIT_RECOVERY]", "[STUCK]", "[HP MAX BOSS]"
        };
        fwrite("[ability category]\n", 1, 0x13, file);
        for (unsigned int i = 0; i < script->m_parameterCategories.size(); ++i)
        {
            const STMonsterParameterCategory* cat =
                &script->m_parameterCategories[i];
            for (int j = 0; j <= 13; ++j)
            {
                if (cat->m_field0 == j)
                {
                    fprintf(file, "`%s` `%s` %d\n",
                            s_abilityCategoryName[j],
                            cat->m_field4 ? "*" : "+",
                            cat->m_field8);
                    break;
                }
            }
        }
        fwrite("[/ability category]\n", 1, 0x14, file);
    }
    // ---- [ability table] ----
    if (script->m_abilityTable != defaultScript.m_abilityTable)
        fprintf(file, "[ability table] %d\n", script->m_abilityTable);
    // ---- [summon skill status up rate]：0..9 float 非零则输出（"%.2f " 累积）----
    {
        char rateBuf[0x1000];
        strncpy(rateBuf, "", 0x1000);
        bool anyRate = false;
        for (int i = 0; i <= 9; ++i)
        {
            if (script->m_field3c4[i] != 0.0f)
                anyRate = true;
            char tmp[0x40];
            snprintf(tmp, 0x40, "%.2f ", (double)script->m_field3c4[i]);
            strncat(rateBuf, tmp, 0x40);
        }
        if (anyRate)
            fprintf(file, "[summon skill status up rate]\n%s\n", rateBuf);
    }
    // ---- [level] 无条件 ----
    fprintf(file, "[level] %d %d\n",
            script->m_levelByDiff[0], script->m_levelByDiff[1]);
    // ---- 四系 resistance（任一非零）----
    // ORIG 08a22c9f 实证：条件两项均读 [k][0]（原源码笔误，忠实复现）
    if (script->m_resistTable[0][0] != 0 || script->m_resistTable[0][0] != 0)
        fprintf(file, "[fire resistance] %d %d\n",
                script->m_resistTable[0][0], script->m_resistTable[0][1]);
    if (script->m_resistTable[1][0] != 0 || script->m_resistTable[1][0] != 0)
        fprintf(file, "[water resistance] %d %d\n",
                script->m_resistTable[1][0], script->m_resistTable[1][1]);
    if (script->m_resistTable[2][0] != 0 || script->m_resistTable[2][0] != 0)
        fprintf(file, "[dark resistance] %d %d\n",
                script->m_resistTable[2][0], script->m_resistTable[2][1]);
    if (script->m_resistTable[3][0] != 0 || script->m_resistTable[3][0] != 0)
        fprintf(file, "[light resistance] %d %d\n",
                script->m_resistTable[3][0], script->m_resistTable[3][1]);
    // ---- 线性 ability 17 项（任一非零；名称表 0x93f5480）----
    {
        static const char* const s_linearAbilityName[17] = {
            "[slow resistance]", "[freeze resistance]", "[poison resistance]",
            "[stun resistance]", "[curse resistance]", "[blind resistance]",
            "[lightning resistance]", "[stone resistance]", "[sleep resistance]",
            "[burn resistance]", "[weapon break resistance]", "[bleeding resistance]",
            "[piercing resistance]", "[stuck resistance]", "[confuse resistance]",
            "[hold resistance]", "[armor break resistance]"
        };
        for (int i = 0; i <= 16; ++i)
        {
            if (script->m_linearAbilities[i][0] ||
                script->m_linearAbilities[i][1])
                fprintf(file, "%s %d %d\n", s_linearAbilityName[i],
                        script->m_linearAbilities[i][0],
                        script->m_linearAbilities[i][1]);
        }
    }
    // ---- [HP regen speed]..[jump power]：任一非零 ----
    if (script->m_hpRegenSpeed[0] || script->m_hpRegenSpeed[1])
        fprintf(file, "[HP regen speed] %d %d\n",
                script->m_hpRegenSpeed[0], script->m_hpRegenSpeed[1]);
    if (script->m_mpRegenSpeed[0] || script->m_mpRegenSpeed[1])
        fprintf(file, "[MP regen speed] %d %d\n",
                script->m_mpRegenSpeed[0], script->m_mpRegenSpeed[1]);
    if (script->m_moveSpeed[0] || script->m_moveSpeed[1])
        fprintf(file, "[move speed] %d %d\n",
                script->m_moveSpeed[0], script->m_moveSpeed[1]);
    if (script->m_attackSpeed[0] || script->m_attackSpeed[1])
        fprintf(file, "[attack speed] %d %d\n",
                script->m_attackSpeed[0], script->m_attackSpeed[1]);
    if (script->m_castSpeed[0] || script->m_castSpeed[1])
        fprintf(file, "[cast speed] %d %d\n",
                script->m_castSpeed[0], script->m_castSpeed[1]);
    if (script->m_hitRecovery[0] || script->m_hitRecovery[1])
        fprintf(file, "[hit recovery] %d %d\n",
                script->m_hitRecovery[0], script->m_hitRecovery[1]);
    if (script->m_jumpPower[0] || script->m_jumpPower[1])
        fprintf(file, "[jump power] %d %d\n",
                script->m_jumpPower[0], script->m_jumpPower[1]);
    // ---- [jump speed]：默认 100 比较 ----
    if (script->m_jumpSpeed[0] != 100 || script->m_jumpSpeed[1] != 100)
        fprintf(file, "[jump speed] %d %d\n",
                script->m_jumpSpeed[0], script->m_jumpSpeed[1]);
    // ---- [normal dash speed]/[slant dash speed]/[weight]（默认对象比较）----
    if (script->m_normalDashSpeed[0] != defaultScript.m_normalDashSpeed[0] ||
        script->m_normalDashSpeed[1] != defaultScript.m_normalDashSpeed[1])
        fprintf(file, "[normal dash speed] %d %d\n",
                script->m_normalDashSpeed[0], script->m_normalDashSpeed[1]);
    if (script->m_slantDashSpeed[0] != defaultScript.m_slantDashSpeed[0] ||
        script->m_slantDashSpeed[1] != defaultScript.m_slantDashSpeed[1])
        // ORIG 08a2312f 实证：条件判 slant，**打印却是 normalDash 两字段**（原笔误忠实）
        fprintf(file, "[slant dash speed] %d %d\n",
                script->m_normalDashSpeed[0], script->m_normalDashSpeed[1]);
    if (script->m_weight[0] != defaultScript.m_weight[0] ||
        script->m_weight[1] != defaultScript.m_weight[1])
        fprintf(file, "[weight] %d %d\n",
                script->m_weight[0], script->m_weight[1]);
    fputc('\n', file);
    // ---- [superarmor on attack] ----
    if (script->m_superarmorOnAttack != defaultScript.m_superarmorOnAttack)
        fprintf(file, "[superarmor on attack] %d %d\n",
                (int)script->m_superarmorOnAttack, script->m_field5a8);
    // ---- [sight] ----
    if (script->m_sight != defaultScript.m_sight)
        fprintf(file, "[sight] %d\n", script->m_sight);
    // ---- [come in action ex]/[come in action] +0x530 非空 ----
    if (!script->m_comeInAction.empty())
    {
        if (!script->m_comeInActionEx.empty())
            fprintf(file, "[come in action ex] `%s` `%s` %d %d\n",
                    script->m_comeInAction.c_str(),
                    script->m_comeInActionEx.c_str(),
                    script->m_field538, script->m_field53c);
        else
            fprintf(file, "[come in action] `%s` %d %d\n",
                    script->m_comeInAction.c_str(),
                    script->m_field538, script->m_field53c);
    }
    // ---- targeting 系列 ----
    if (script->m_targetingTimeTerm != defaultScript.m_targetingTimeTerm)
        fprintf(file, "[targeting time term] %d\n",
                script->m_targetingTimeTerm);
    if (script->m_targetingOnAttack != defaultScript.m_targetingOnAttack)
        fprintf(file, "[targeting on attack] %d\n",
                script->m_targetingOnAttack ? 1 : 0);
    if (script->m_targetingOnDamage != defaultScript.m_targetingOnDamage)
        fprintf(file, "[targeting on damage] %d\n",
                script->m_targetingOnDamage ? 1 : 0);
    if (script->m_targetingLevel == 2)
        fwrite("[targeting high level] 1\n", 1, 0x19, file);
    else if (script->m_targetingLevel == 1)
        fwrite("[targeting low level] 1\n", 1, 0x18, file);
    if (script->m_targetingHp == 2)
        fwrite("[targeting high HP] 1\n", 1, 0x16, file);
    else if (script->m_targetingHp == 1)
        fwrite("[targeting low HP] 1\n", 1, 0x15, file);
    if (script->m_targetingNearest != defaultScript.m_targetingNearest)
        fprintf(file, "[targeting nearest] %d\n",
                script->m_targetingNearest ? 1 : 0);
    if (script->m_targetingAttacker != defaultScript.m_targetingAttacker)
        fprintf(file, "[targeting attacker] %d\n",
                script->m_targetingAttacker ? 1 : 0);
    if (script->m_targetingFromDirection !=
        defaultScript.m_targetingFromDirection)
        fprintf(file, "[targeting from direction] %d\n",
                script->m_targetingFromDirection ? 1 : 0);
    if (script->m_targetingLowPhysAttack !=
        defaultScript.m_targetingLowPhysAttack)
        fprintf(file, "[targeting low physical attack] %d\n",
                script->m_targetingLowPhysAttack ? 1 : 0);
    if (script->m_targetingLowPhysDefense !=
        defaultScript.m_targetingLowPhysDefense)
        fprintf(file, "[targeting low physical defense] %d\n",
                script->m_targetingLowPhysDefense ? 1 : 0);
    if (script->m_targetingLowMagAttack !=
        defaultScript.m_targetingLowMagAttack)
        fprintf(file, "[targeting low magical attack] %d\n",
                script->m_targetingLowMagAttack ? 1 : 0);
    if (script->m_targetingLowMagDefense !=
        defaultScript.m_targetingLowMagDefense)
        fprintf(file, "[targeting low magical defense] %d\n",
                script->m_targetingLowMagDefense ? 1 : 0);
    if (script->m_targetingDowned != defaultScript.m_targetingDowned)
        fprintf(file, "[targeting downed] %d\n",
                script->m_targetingDowned ? 1 : 0);
    if (script->m_warlike != defaultScript.m_warlike)
        fprintf(file, "[warlike] %d\n", script->m_warlike);
    if (script->m_attackDelay != defaultScript.m_attackDelay)
        fprintf(file, "[attack delay] %d\n", script->m_attackDelay);
    // ---- [stuckbonus on damage] 无条件 ----
    fprintf(file, "[stuckbonus on damage] %d %d %d %d\n",
            script->m_stuckBonusOnDamage[0][0], script->m_stuckBonusOnDamage[0][1],
            script->m_stuckBonusOnDamage[1][0], script->m_stuckBonusOnDamage[1][1]);
    fputc('\n', file);
    // ---- [item] 四段（pair<int,int> 循环）+ champion element + super 第二段 ----
    if (!script->m_field028[0].empty())
    {
        fwrite("[item]\n", 1, 7, file);
        for (unsigned int i = 0; i < script->m_field028[0].size(); ++i)
            fprintf(file, "%d %d\n", script->m_field028[0][i].first,
                    script->m_field028[0][i].second);
        fwrite("[/item]\n\n", 1, 9, file);
    }
    if (!script->m_field028[1].empty())
    {
        fwrite("[common champion drop item]\n", 1, 0x1c, file);
        for (unsigned int i = 0; i < script->m_field028[1].size(); ++i)
            fprintf(file, "%d %d\n", script->m_field028[1][i].first,
                    script->m_field028[1][i].second);
        fwrite("[/common champion drop item]\n\n", 1, 0x1e, file);
    }
    if (script->m_field028[2].size() != 0)
    {
        fwrite("[super champion drop item]\n", 1, 0x1b, file);
        for (unsigned int i = 0; i < script->m_field028[2].size(); ++i)
            fprintf(file, "%d %d\n", script->m_field028[2][i].first,
                    script->m_field028[2][i].second);
        fwrite("[/super champion drop item]\n\n", 1, 0x1d, file);
    }
    if (!script->m_field058.empty())
    {
        fwrite("[death tower item]\n", 1, 0x13, file);
        for (unsigned int i = 0; i < script->m_field058.size(); ++i)
            fprintf(file, "%d %d\n", script->m_field058[i].first,
                    script->m_field058[i].second);
        fwrite("[/death tower item]\n", 1, 0x14, file);
    }
    if (!script->m_championElements.empty())
    {
        fwrite("[common champion elemental property]\n", 1, 0x25, file);
        for (unsigned int i = 0; i < script->m_championElements.size(); ++i)
            fprintf(file, "`%s`\n",
                    getElementName((ENUM_ELEMENT)script->m_championElements[i]));
        fwrite("[/common champion elemental property]\n\n", 1, 0x27, file);
    }
    if (!script->m_field028[2].empty())   // ORIG 同一 +0x40 段重复导出（忠实）
    {
        fwrite("[super champion drop item]\n", 1, 0x1b, file);
        for (unsigned int i = 0; i < script->m_field028[2].size(); ++i)
            fprintf(file, "%d %d\n", script->m_field028[2][i].first,
                    script->m_field028[2][i].second);
        fwrite("[/super champion drop item]\n\n", 1, 0x1d, file);
    }
    // ---- [keep distance] ----
    if (script->m_keepDistance != defaultScript.m_keepDistance)
        fprintf(file, "[keep distance] %d\n", script->m_keepDistance);
    // ---- [is on the object] 两处 ----
    if (script->m_flag374)
        fprintf(file, "[is on the object] 1 %d\n", script->m_field378);
    if (script->m_isOnTheObject)
        fwrite("[is on the object] 1\n", 1, 0x15, file);
    // ---- [attack kind] +0x6e8 vector<STAttackMethod> ----
    if (!script->m_attackMethods.empty())
    {
        fwrite("[attack kind]\n", 1, 0xe, file);
        for (unsigned int i = 0; i < script->m_attackMethods.size(); ++i)
        {
            // ORIG 逐字段重做下标（0x2c,0x30,0x24,0x28,0x1c,0x20,0x14,0x18,
            // 0x00(三目),0x10,0x0c,0x08 求值序）；%f 三值经 flds→fstpl 双精度。
            fprintf(file,
                    "%f\t%f\t%f\t%d %d %d %d %d %d %d %d %d",
                    (double)script->m_attackMethods[i].m_field08,
                    (double)script->m_attackMethods[i].m_field0c,
                    (double)script->m_attackMethods[i].m_field10,
                    script->m_attackMethods[i].m_field00 ? 1 : 0,
                    script->m_attackMethods[i].m_field18,
                    script->m_attackMethods[i].m_field14,
                    script->m_attackMethods[i].m_field20,
                    script->m_attackMethods[i].m_field1c,
                    script->m_attackMethods[i].m_field28,
                    script->m_attackMethods[i].m_field24,
                    script->m_attackMethods[i].m_field30,
                    script->m_attackMethods[i].m_field2c);
            fwrite("\t\n", 1, 2, file);
        }
        fwrite("[/attack kind]\n\n", 1, 0x10, file);
    }
    // ---- motion 系列（!= "" 比较）----
    if (script->m_waitingMotion != "")
        fprintf(file, "[waiting motion] `%s`\n",
                script->m_waitingMotion.c_str());
    if (script->m_moveMotion != "")
        fprintf(file, "[move motion] `%s`\n", script->m_moveMotion.c_str());
    if (script->m_sitMotion != "")
        fprintf(file, "[sit motion] `%s`\n", script->m_sitMotion.c_str());
    if (script->m_damageMotion[0] != "")
        fprintf(file, "[damage motion 1] `%s`\n",
                script->m_damageMotion[0].c_str());
    if (script->m_damageMotion[1] != "")
        fprintf(file, "[damage motion 2] `%s`\n",
                script->m_damageMotion[1].c_str());
    if (script->m_downMotion != "")
        fprintf(file, "[down motion] `%s`\n", script->m_downMotion.c_str());
    if (script->m_overturnMotion != "")
        fprintf(file, "[overturn motion] `%s`\n",
                script->m_overturnMotion.c_str());
    if (script->m_jumpMotion != "")
        fprintf(file, "[jump motion] `%s`\n", script->m_jumpMotion.c_str());
    if (script->m_jumpAttackMotion != "")
        fprintf(file, "[jumpattack motion] `%s`\n",
                script->m_jumpAttackMotion.c_str());
    // ---- [attack motion] +0x5e4 ----
    if (!script->m_attackMotions.empty())
    {
        fwrite("[attack motion]\n", 1, 0x10, file);
        for (std::vector<std::string>::iterator it =
                 script->m_attackMotions.begin();
             it != script->m_attackMotions.end(); it++)
            fprintf(file, "`%s`\n", (*it).c_str());
        fwrite("[/attack motion]\n", 1, 0x11, file);
    }
    // ---- [etc motion] +0x5f0 ----
    if (!script->m_etcMotions.empty())
    {
        fwrite("[etc motion]\n", 1, 0xd, file);
        for (std::vector<std::string>::iterator it =
                 script->m_etcMotions.begin();
             it != script->m_etcMotions.end(); it++)
            fprintf(file, "`%s`\n", (*it).c_str());
        fwrite("[/etc motion]\n", 1, 0xe, file);
    }
    fputc('\n', file);
    // ---- action 系列 ----
    if (script->m_waitingAction != "")
        fprintf(file, "[waiting action] `%s`\n",
                script->m_waitingAction.c_str());
    if (script->m_moveAction != "")
        fprintf(file, "[move action] `%s`\n", script->m_moveAction.c_str());
    if (!script->m_movebackAction.empty())
        fprintf(file, "[moveback action] `%s`\n",
                script->m_movebackAction.c_str());
    if (!script->m_dashAction.empty())
        fprintf(file, "[dash action] `%s`\n", script->m_dashAction.c_str());
    if (!script->m_dashattackAction.empty())
        fprintf(file, "[dashattack action] `%s`\n",
                script->m_dashattackAction.c_str());
    if (script->m_sitAction != "")
        fprintf(file, "[sit action] `%s`\n", script->m_sitAction.c_str());
    if (script->m_damageAction[0] != "")
        fprintf(file, "[damage action 1] `%s`\n",
                script->m_damageAction[0].c_str());
    if (script->m_damageAction[1] != "")
        fprintf(file, "[damage action 2] `%s`\n",
                script->m_damageAction[1].c_str());
    if (script->m_downAction != "")
        fprintf(file, "[down action] `%s`\n", script->m_downAction.c_str());
    if (script->m_overturnAction != "")
        fprintf(file, "[overturn action] `%s`\n",
                script->m_overturnAction.c_str());
    if (script->m_jumpAction != "")
        fprintf(file, "[jump action] `%s`\n", script->m_jumpAction.c_str());
    if (script->m_jumpAttackAction != "")
        fprintf(file, "[jumpattack action] `%s`\n",
                script->m_jumpAttackAction.c_str());
    if (script->m_basicAction != "")
        fprintf(file, "[basic action] `%s`\n", script->m_basicAction.c_str());
    // ---- [attack action] +0x62c（extra map 同 key 向量配对输出）----
    if (!script->m_attackActions.empty())
    {
        std::vector<std::string> extraActions;
        std::map<std::string, std::vector<std::string> >::iterator it =
            extra.find("[attack action]");
        if (it != extra.end())
            extraActions = it->second;
        fwrite("[attack action]\n", 1, 0x10, file);
        for (unsigned int i = 0; i < script->m_attackActions.size(); ++i)
        {
            std::string s;
            s.clear();
            if (i < extraActions.size())
                s = extraActions[i];
            fprintf(file, "`%s`\t%s\n", script->m_attackActions[i].c_str(),
                    s.c_str());
        }
        fwrite("[/attack action]\n", 1, 0x11, file);
    }
    // ---- [etc action] +0x638 ----
    if (!script->m_etcActions.empty())
    {
        fwrite("[etc action]\n", 1, 0xd, file);
        for (std::vector<std::string>::iterator it =
                 script->m_etcActions.begin();
             it != script->m_etcActions.end(); it++)
            fprintf(file, "`%s`\n", (*it).c_str());
        fwrite("[/etc action]\n", 1, 0xe, file);
    }
    if (script->m_lastAction != "")
        fprintf(file, "[last action] `%s`\n", script->m_lastAction.c_str());
    if (!script->m_procAction.empty())
        fprintf(file, "[proc action] `%s`\n", script->m_procAction.c_str());
    if (script->m_jumpAttackInfo != "")
        fprintf(file, "[jumpattack info] `%s`\n",
                script->m_jumpAttackInfo.c_str());
    // ---- [attack info] +0x6b8 / [etc attack info] +0x6c4 ----
    if (!script->m_attackInfos.empty())
    {
        fwrite("[attack info]\n", 1, 0xe, file);
        for (std::vector<std::string>::iterator it =
                 script->m_attackInfos.begin();
             it != script->m_attackInfos.end(); it++)
            fprintf(file, "`%s`\n", (*it).c_str());
        fwrite("[/attack info]\n", 1, 0xf, file);
    }
    if (!script->m_etcAttackInfos.empty())
    {
        fwrite("[etc attack info]\n", 1, 0x12, file);
        for (std::vector<std::string>::iterator it =
                 script->m_etcAttackInfos.begin();
             it != script->m_etcAttackInfos.end(); it++)
            fprintf(file, "`%s`\n", (*it).c_str());
        fwrite("[/etc attack info]\n", 1, 0x13, file);
    }
    fputc('\n', file);
    // ---- [int data] +0x6d4（extra map "[int data]" 配对）----
    if (!script->m_intData.empty())
    {
        fwrite("[int data]\n", 1, 0xb, file);
        std::string s;
        for (unsigned int i = 0; i < script->m_intData.size(); ++i)
        {
            // ORIG 每轮：clear → 临时 key find → 命中则拷贝整个 vector 再取元素
            s.clear();
            std::map<std::string, std::vector<std::string> >::iterator it =
                extra.find("[int data]");
            if (it != extra.end())
            {
                std::vector<std::string> tmp(it->second);
                if (i < tmp.size())
                    s = tmp[i];
            }
            fprintf(file, "%d\t%s\n", script->m_intData[i], s.c_str());
        }
        fwrite("[/int data]\n", 1, 0xc, file);
    }
    // ---- [speech on situation] +0x70c[111]（0..0x64 用 [on attack]:+序号；0x65..0x6e switch）----
    // ORIG speechTag(-0x19c) 为**函数作用域**：ctor 在此处、dtor 在函数尾
    // （08a29e4c），故不得用 {} 包裹（否则块尾 dtor + 独立 landing pad）。
    bool anySpeech = false;
    std::string speechTag;
    int speechIndex = 0;
    for (int si = 0; si <= 0x6e; ++si)
    {
        if (!script->m_field70c[si].empty())
        {
            fwrite("[speech on situation]\n", 1, 0x16, file);
            anySpeech = true;
            break;
        }
    }
    for (int si = 0; si <= 0x6e; ++si)
    {
        if (script->m_field70c[si].size() != 0)
        {
            if (0 <= si && si <= 100)       // ORIG 08a25313：js/jg → 后段 switch 块
            {
                speechTag = "`[on attack]:`";
                speechIndex = si;
                for (unsigned int j = 0; j < script->m_field70c[si].size(); ++j)
                {
                    fprintf(file, "%s %s %d\n", speechTag.c_str(),
                            toTString(g_scriptStringManager_.getTagByName(
                                7,
                                (const wchar_t*)script->m_field70c[si][j].c_str(),
                                0, false)),
                            speechIndex);
                }
            }
            else
            {
                switch (si - 0x65)
                {
                case 0: speechTag = "`[on damage]`"; break;
                case 1: speechTag = "`[on die]`"; break;
                case 2: speechTag = "`[on revenge]`"; break;
                case 3: speechTag = "`[on etc]`"; break;
                case 4: speechTag = "`[on appear]`"; break;
                case 5: speechTag = "`[on disappear]`"; break;
                case 6: speechTag = "`[on go to help]`"; break;
                case 7: speechTag = "`[on stand]`"; break;
                // case 8（si==0x6d）：ORIG 表项直指 merge（保留上一轮 tag）
                case 9: speechTag = "`[on village]`"; break;
                }
                speechIndex = 0;            // ORIG 08a25533：switch merge 后
                for (unsigned int j = 0; j < script->m_field70c[si].size(); ++j)
                {
                    fprintf(file, "%s %s %d\n", speechTag.c_str(),
                            toTString(g_scriptStringManager_.getTagByName(
                                7,
                                (const wchar_t*)script->m_field70c[si][j].c_str(),
                                0, false)),
                            speechIndex);
                }
            }
        }
    }
    if (anySpeech)
        fwrite("[/speech on situation]\n", 1, 0x17, file);
    // ---- [fix move direction] ----
    if (script->m_flagc50)
        fwrite("[fix move direction] 1\n", 1, 0x17, file);
    // ---- [throw attack] +0x9c ----
    for (unsigned int i = 0; i < script->m_throwAttacks.size(); ++i)
    {
        fwrite("[throw attack]\n", 1, 0xf, file);
        fwrite("`[attack index]`", 1, 0x10, file);
        fprintf(file, " %d\n", script->m_throwAttacks[i].m_field44);
        fwrite("`[passive object number]`", 1, 0x19, file);
        fprintf(file, " %d\n", script->m_throwAttacks[i].m_field50);
        fwrite("`[passive object filename]`", 1, 0x1b, file);
        for (int j = 0; j < script->m_throwAttacks[i].m_field50; ++j)
            fprintf(file, "`%s`\n",
                    script->m_throwAttacks[i].m_field00[j].c_str());
        fwrite("`[object type]`", 1, 0xf, file);
        fprintf(file, " %d\n", script->m_throwAttacks[i].m_field4c);
        fwrite("`[passive object index]`", 1, 0x18, file);
        fprintf(file, " %d\n", script->m_throwAttacks[i].m_field0c);
        fwrite("`[power]`", 1, 9, file);
        fprintf(file, " %d\n", script->m_throwAttacks[i].m_field10);
        fwrite("`[throw frame]`", 1, 0xf, file);
        fprintf(file, " %d\n", script->m_throwAttacks[i].m_field14);
        if (!script->m_throwAttacks[i].m_field18.empty())
        {
            fwrite("`[passive object start x cood]` ", 1, 0x20, file);
            for (int j = 0; j < script->m_throwAttacks[i].m_field50; ++j)
                fprintf(file, "%d ", script->m_throwAttacks[i].m_field18[j]);
            fputc('\n', file);
        }
        if (!script->m_throwAttacks[i].m_field24.empty())
        {
            fwrite("`[passive object start y cood]` ", 1, 0x20, file);
            for (int j = 0; j < script->m_throwAttacks[i].m_field50; ++j)
                fprintf(file, "%d ", script->m_throwAttacks[i].m_field24[j]);
            fputc('\n', file);
        }
        if (!script->m_throwAttacks[i].m_field30.empty())
        {
            fwrite("`[passive object start z cood]` ", 1, 0x20, file);
            for (int j = 0; j < script->m_throwAttacks[i].m_field50; ++j)
                fprintf(file, "%d ", script->m_throwAttacks[i].m_field30[j]);
            fputc('\n', file);
        }
        fwrite("`[attack number]`", 1, 0x11, file);
        fprintf(file, " %d\n", script->m_throwAttacks[i].m_field3c);
        fwrite("`[throw sound]`", 1, 0xf, file);
        fprintf(file, "`%s`\n", script->m_throwAttacks[i].m_field40.c_str());
        fwrite("[/throw attack]\n", 1, 0x10, file);
    }
    // ---- [equipment] +0x390 ----
    if (!script->m_equipment.m_items.empty())
    {
        fwrite("[equipment]\n", 1, 0xc, file);
        fprintf(file, "%d\n", (int)script->m_equipment.m_items.size());
        for (unsigned int i = 0; i < script->m_equipment.m_items.size(); ++i)
            fprintf(file, "`%s`\n", script->m_equipment.m_items[i].c_str());
        fwrite("[/equipment]\n", 1, 0xd, file);
    }
    // ---- [ready action] +0x39c ----
    for (unsigned int i = 0; i < script->m_attackReadies.size(); ++i)
    {
        fwrite("[ready action]\n", 1, 0xf, file);
        fprintf(file, "`[motion index]` %d\n",
                script->m_attackReadies[i].m_field0c);
        fwrite("`[effect ani file & layer index]`\n", 1, 0x22, file);
        fprintf(file, "%d\n", (int)script->m_attackReadies[i].m_field10.size());
        for (unsigned int j = 0; j < script->m_attackReadies[i].m_field10.size(); ++j)
        {
            fprintf(file, "`%s` ",
                    script->m_attackReadies[i].m_field10[j].c_str());
            fprintf(file, "%d\n", script->m_attackReadies[i].m_field00[j]);
        }
        fwrite("[/ready action]\n", 1, 0x10, file);
    }
    // ---- [counter attack] +0xa8 ----
    for (unsigned int i = 0; i < script->m_counterAttacks.size(); ++i)
    {
        fwrite("[counter attack]\n", 1, 0x11, file);
        // ORIG 08a26107：结构体值拷贝进局部（10 条 mov/mov 对）
        const STCounterAttack ca = script->m_counterAttacks[i];
        fprintf(file, "%d\n", (int)script->m_counterAttacks.size());
        fprintf(file, "`[probability]` %d\n", ca.m_field00);
        fprintf(file, "`[attack index]` %d\n", ca.m_field04);
        fwrite("`[damage type]`", 1, 0xf, file);
        // ORIG 08a261c8 switch 线性决策（cmp1 je; cmp2 je; test jne; 0-body fall）
        switch (ca.m_field08)
        {
        case 0:
            fwrite("`[approach attack]`\n", 1, 0x14, file);
            break;
        case 1:
            fwrite("`[range attack]`\n", 1, 0x11, file);
            break;
        case 2:
            fwrite("`[all attack]`\n", 1, 0xf, file);
            break;
        }
        if (ca.m_field10 != 0)
            fprintf(file, "`[need hp]` %d\n", ca.m_field10);
        fwrite("[/counter attack]\n", 1, 0x12, file);
    }
    // ---- [jump attack] +0xb4 ----
    for (unsigned int i = 0; i < script->m_jumpAttacks.size(); ++i)
    {
        // ORIG 08a262ef：值拷贝（11 槽 -0x28c..-0x264）
        const STJumpAttack ja = script->m_jumpAttacks[i];
        fwrite("[jump attack]\n", 1, 0xe, file);
        fprintf(file, "`[attack index]` %d\n", ja.m_field00);
        fprintf(file, "`[start frame index]` %d\n", ja.m_field1c);
        fprintf(file, "`[end frame index]` %d\n", ja.m_field20);
        fprintf(file, "`[jump height]` %d\n", ja.m_field04);
        fprintf(file, "`[jump speed]` %d\n", ja.m_field08);
        fprintf(file, "`[on collision custom animation index]` %d\n",
                ja.m_field0c);
        fprintf(file, "`[ememy distance rate]` %f\n",
                (double)ja.m_field10);
        fprintf(file, "`[limited x cood]` %d\n", ja.m_field24);
        fprintf(file, "`[limited y cood]` %d\n", ja.m_field28);
        fprintf(file, "`[jump type]` %d\n", ja.m_field18);
        fprintf(file, "`[jump direction]` %d\n", ja.m_field14);
        fwrite("[/jump attack]\n", 1, 0xf, file);
    }
    // ---- [dash attack] +0xcc ----
    for (unsigned int i = 0; i < script->m_dashes.size(); ++i)
    {
        fwrite("[dash attack]\n", 1, 0xe, file);
        // ORIG 08a2657a：值拷贝（10 槽）
        const STDash d = script->m_dashes[i];
        fprintf(file, "`[function index]` %d\n", d.m_field00);
        fprintf(file, "`[attack index]` %d\n", d.m_field04);
        fprintf(file, "`[shakeable]` %d %d %d\n",
                d.m_field08, d.m_field0c, d.m_field10);
        fprintf(file, "`[first speed]` %d\n", d.m_field14);
        fprintf(file, "`[last speed]` %d\n", d.m_field18);
        fprintf(file, "`[dash time]` %d\n", d.m_field1c);
        fprintf(file, "`[dash ready time]` %d\n", d.m_field20);
        fprintf(file, "`[animation first index]` %d\n", d.m_field24);
        fwrite("[/dash attack]\n", 1, 0xf, file);
    }
    // ---- [hold attack] +0xd8 ----
    for (unsigned int i = 0; i < script->m_holds.size(); ++i)
    {
        fwrite("[hold attack]\n", 1, 0xe, file);
        // ORIG 08a26785：值拷贝（9 槽）
        const STHold h = script->m_holds[i];
        fprintf(file, "`[function index]` %d", h.m_field00);
        if (h.m_field00 == 1)
            fprintf(file, " %d %d", h.m_field08, h.m_field04);   // ORIG 08a267e6：先 +0x8 后 +0x4
        fputc('\n', file);
        fprintf(file, "`[attack index]` %d\n", h.m_field0c);
        fprintf(file, "`[offset]` %d %d\n", h.m_field10, h.m_field14);
        fprintf(file, "`[animation index]` %d\n", h.m_field18);
        fprintf(file, "`[min hold]` %d\n", h.m_field1c);
        fprintf(file, "`[max hold]` %d\n", h.m_field20);
        fwrite("[/hold attack]\n", 1, 0xf, file);
    }
    // ---- [warp attack] +0xe4（逐字段与默认对象比较）----
    for (unsigned int i = 0; i < script->m_warps.size(); ++i)
    {
        fwrite("[warp attack]\n", 1, 0xe, file);
        STWarp defaultWarp;                      // ORIG 循环体内 C1 → 后 ix
        const STWarp w = script->m_warps[i];     // ORIG 08a26996：值拷贝（9 槽）
        if (w.m_field00 != defaultWarp.m_field00)
            fprintf(file, "`[function index]` %d\n", w.m_field00);
        if (w.m_field04 != defaultWarp.m_field04)
            fprintf(file, "`[attack index]` %d\n", w.m_field04);
        if (w.m_field08 != defaultWarp.m_field08)
            fprintf(file, "`[warp style]` %d\n", w.m_field08);
        // ORIG 08a26xxx 实证字段映射：target offset=+0x18、after attack=+0x0c、
        // after appear time=+0x10、cool time=+0x14（按此输出序）
        if (w.m_field18 != defaultWarp.m_field18)
            fprintf(file, "`[target offset]` %d\n", w.m_field18);
        if (w.m_field0c != defaultWarp.m_field0c)
            fprintf(file, "`[after attack]` %d\n", w.m_field0c);
        if (w.m_field10 != defaultWarp.m_field10)
            fprintf(file, "`[after appear time]` %d\n", w.m_field10);
        if (w.m_field14 != defaultWarp.m_field14)
            fprintf(file, "`[cool time]` %d\n", w.m_field14);
        if (w.m_field1c != defaultWarp.m_field1c)
            fprintf(file, "`[passive object index]` %d\n", w.m_field1c);
        if (w.m_field20 != defaultWarp.m_field20)
        {
            fwrite("`[category of target]`", 1, 0x16, file);
            // ORIG 08a26ba0 switch(-2,null; -1,all; default,string)
            switch (w.m_field20)
            {
            case -2:
                fwrite(" `null`\n", 1, 8, file);
                break;
            case -1:
                fwrite(" `all`\n", 1, 7, file);
                break;
            default:
            {
                std::string cat(getObjectCategoryString(
                    (ENUM_CATEGORY)w.m_field20));
                fprintf(file, " `%s`\n", cat.c_str());
            }
            break;
            }
        }
        fwrite("[/warp attack]\n", 1, 0xf, file);
    }
    // ---- [event super on scr] +0xfc ----
    if (!script->m_eventSupers.empty())
    {
        for (unsigned int i = 0; i < script->m_eventSupers.size(); ++i)
        {
            fwrite("[event super on scr]\n", 1, 0x15, file);
            const STEventSuper es = script->m_eventSupers[i];  // ORIG 08a26d2e：值拷贝
            // ORIG 08a26d88：switch 二分决策树（根=1，je HP / jg{2,3} / else 0）
            switch (es.m_field00)
            {
            case 0:
                fprintf(file, "`[on time]` %d %d %d\n", es.m_field08,
                        es.m_field0c, es.m_field10);
                break;
            case 1:
                fprintf(file, "`[on hp]` %d %d %d\n", es.m_field08,
                        es.m_field0c, es.m_field10);
                break;
            case 2:
                fprintf(file, "`[always]` %d %d\n", es.m_field08,
                        es.m_field10);
                break;
            case 3:
                fprintf(file, "`[counter]` %d %d %d\n", es.m_field08,
                        es.m_field0c, es.m_field10);
                break;
            }
            // ORIG 08a26eb3 实证：%d×4 = f14、f18、f1c、f20（非 f24）
            // ORIG 08a26e97 switch 线性决策（cmp1 je; cmp2 je; test 0 fall）
            switch (es.m_field04)
            {
            case 0:
                fprintf(file, "`[breakable]` %d %d %d %d\n", es.m_field14,
                        es.m_field18, es.m_field1c, es.m_field20);
                break;
            case 1:
                fprintf(file, "`[half]` %d %d %d %d\n", es.m_field14,
                        es.m_field18, es.m_field1c, es.m_field20);
                break;
            case 2:
                fprintf(file, "`[unbreakable]` %d %d %d %d\n", es.m_field14,
                        es.m_field18, es.m_field1c, es.m_field20);
                break;
            }
            // ORIG 此处重做下标读取 +0x24（不复用拷贝缓存）
            if (script->m_eventSupers[i].m_field24)
                fwrite("`[with blast]`\n", 1, 0xf, file);
            fwrite("[/event super on scr]\n", 1, 0x16, file);
        }
    }
    // ---- [atk super on scr] +0x108 ----
    if (!script->m_atkSupers.empty())
    {
        fwrite("[atk super on scr]\n", 1, 0x13, file);
        for (unsigned int i = 0; i < script->m_atkSupers.size(); ++i)
        {
            const STAtkSuper as = script->m_atkSupers[i];  // ORIG 08a27073：值拷贝（6 槽）
            fprintf(file, "%d", as.m_field04);
            // ORIG 08a270c8 switch 二分（cmp 0xe je; cmp 0xf je; cmp 0xd jne）
            switch (as.m_field00)
            {
            case 0xd:
                fwrite(" `[breakable]`", 1, 0xe, file);
                break;
            case 0xe:
                fwrite(" `[half]`", 1, 9, file);
                break;
            case 0xf:
                fwrite(" `[unbreakable]`", 1, 0x10, file);
                break;
            }
            fprintf(file, " %d %d %d %d\n", as.m_field08, as.m_field0c,
                    as.m_field10, as.m_field14);
        }
        fwrite("[/atk super on scr]\n", 1, 0x14, file);
    }
    // ---- [cooltime] +0x114（front()+0x4 vector<int>）----
    if (!script->m_coolTimes.empty())
    {
        fwrite("[cooltime]\n", 1, 0xb, file);
        // ORIG 条件与体内各自重新求值 front()（P38）
        for (unsigned int i = 0; i < script->m_coolTimes.front().m_field04.size(); ++i)
            fprintf(file, "%d\n", script->m_coolTimes.front().m_field04[i]);
        fwrite("[/cooltime]\n", 1, 0xc, file);
    }
    // ---- [spawn prob] ----
    if (script->m_fieldc4c != defaultScript.m_fieldc4c)
        fprintf(file, "[spawn prob] %d\n", script->m_fieldc4c);
    // ---- [drop item offset] ----
    if (script->m_field36c != defaultScript.m_field36c ||
        script->m_field370 != defaultScript.m_field370)
        fprintf(file, "[drop item offset] %d %d\n",
                script->m_field36c, script->m_field370);
    // ---- [is grabable]（==0 才输出）----
    if (!script->m_flag368)
        fwrite("[is grabable] 0\n", 1, 0x10, file);
    // ---- [think term]/[destination change term]（m_aiInfo）----
    STAIInfoScript* aiInfo = &script->m_aiInfo;   // ORIG -0x128 命名指针槽
    if (aiInfo->m_thinkTerm != defaultScript.m_aiInfo.m_thinkTerm)
        fprintf(file, "[think term] %d\n", aiInfo->m_thinkTerm);
    if (aiInfo->m_destinationChangeTerm !=
        defaultScript.m_aiInfo.m_destinationChangeTerm)
        fprintf(file, "[destination change term] %d\n",
                aiInfo->m_destinationChangeTerm);
    // ---- [ai pattern]：5 难度 ----
    if (isExistAiPattern(aiInfo, -1))
    {
        fwrite("[ai pattern]\n", 1, 0xd, file);
        int patternIdx = 0;                        // ORIG -0x80 命名槽
        if (isExistAiPattern(aiInfo, patternIdx))
        {
            fwrite("[easy]\n", 1, 7, file);
            exportAIPatternPath(file, aiInfo, patternIdx);
            fwrite("[/easy]\n", 1, 8, file);
        }
        patternIdx = 1;
        if (isExistAiPattern(aiInfo, patternIdx))
        {
            fwrite("[medium]\n", 1, 9, file);
            exportAIPatternPath(file, aiInfo, patternIdx);
            fwrite("[/medium]\n", 1, 0xa, file);
        }
        patternIdx = 2;
        if (isExistAiPattern(aiInfo, patternIdx))
        {
            fwrite("[hard]\n", 1, 7, file);
            exportAIPatternPath(file, aiInfo, patternIdx);
            fwrite("[/hard]\n", 1, 8, file);
        }
        patternIdx = 3;
        if (isExistAiPattern(aiInfo, patternIdx))
        {
            fwrite("[ultimate]\n", 1, 0xb, file);
            exportAIPatternPath(file, aiInfo, patternIdx);
            fwrite("[/ultimate]\n", 1, 0xc, file);
        }
        patternIdx = 4;
        if (isExistAiPattern(aiInfo, patternIdx))
        {
            fwrite("[hero]\n", 1, 7, file);
            exportAIPatternPath(file, aiInfo, patternIdx);
            fwrite("[/hero]\n", 1, 8, file);
        }
        fwrite("[/ai pattern]\n\n", 1, 0xf, file);
    }
    // ---- [targeting bonus]（22 项；i==20/21 用状态/攻击标签表）----
    {
        bool anyBonus = false;
        for (int i = 0; i <= 0x15; ++i)
        {
            // ORIG 08a27756：置位后 `jmp 08a2776e`（循环出口）= 原码有 break
            if (!script->m_aiInfo.m_targetingBonusStr[i].empty())
            {
                anyBonus = true;
                break;
            }
        }
        if (anyBonus)
        {
            fwrite("[targeting bonus]\n", 1, 0x12, file);
            for (int i = 0; i <= 0x15; ++i)
            {
                // ORIG 08a277b0：此两处走 **this 基址**（script->m_aiInfo.x），非 aiInfo 指针
                int bonus = script->m_aiInfo.m_targetingBonus[i];   // ORIG -0x74
                fprintf(file, "%d ", bonus);
                for (unsigned int j = 0;
                     j < script->m_aiInfo.m_targetingBonusStr[i].size(); ++j)
                {
                    int v = script->m_aiInfo.m_targetingBonusStr[i][j];   // ORIG -0x6c
                    // ORIG 08a277e6：switch 双 case（单 load i + 双 je 前跳）
                    switch (i)
                    {
                    case 0x14:
                        fprintf(file, "`%s` ", ACTIVEOBJECT_STATE_STRING[v]);
                        break;
                    case 0x15:
                        fprintf(file, "`%s` ", ACTIVESTATUS_ATTACK_TAG[v]);
                        break;
                    }
                }
                if ((unsigned int)(i - 0x14) < 2)   // ORIG 08a2789b：sub/cmp/ja
                    fputc('0', file);   // ORIG 08a278b0：字面量 0x30（'0'）
                fputc('\n', file);
            }
            fwrite("[/targeting bonus]\n", 1, 0x13, file);
        }
    }
    // ---- [keep distance with target] ----
    if (aiInfo->m_keepDistance !=
        defaultScript.m_aiInfo.m_keepDistance)
        fprintf(file, "[keep distance with target] %d\n",
                aiInfo->m_keepDistance);
    // ---- [keep range distance with target]（4 组 vector，i==0 带序号）----
    for (unsigned int i = 0; i < aiInfo->m_keepRange0.size(); ++i)
    {
        // ORIG 08a27953：主路径 i!=0（带 %d] 序号）；i==0 走 je 无序号块。
        // 两分支各 4 次 operator[]（共 8 静态调用点，P38）；求值序 0x1d4→0x1b0。
        if (i != 0)
            fprintf(file, "[keep range distance with target %d] %d %d %d %d\n",
                    i,
                    aiInfo->m_keepRange0[i],
                    aiInfo->m_keepRange1[i],
                    aiInfo->m_keepRange2[i],
                    aiInfo->m_keepRange3[i]);
        else
            fprintf(file, "[keep range distance with target] %d %d %d %d\n",
                    aiInfo->m_keepRange0[i],
                    aiInfo->m_keepRange1[i],
                    aiInfo->m_keepRange2[i],
                    aiInfo->m_keepRange3[i]);
    }
    // ---- [warlike]（ORIG 用 m_aiInfo.m_intelligence 二次导出）----
    if (aiInfo->m_intelligence !=
        defaultScript.m_aiInfo.m_intelligence)
        fprintf(file, "[warlike] %d\n", aiInfo->m_intelligence);
    // ---- [intelligence]/[vision]/[back vision] ----
    // ORIG 08a27afe：条件仅比较 +0x1f0/+0x1f4（不含 intelligence）
    if (aiInfo->m_field1f0 != defaultScript.m_aiInfo.m_field1f0 ||
        aiInfo->m_field1f4 != defaultScript.m_aiInfo.m_field1f4)
        fprintf(file, "[intelligence] %d %d %d\n",
                aiInfo->m_intelligence,
                aiInfo->m_field1f0, aiInfo->m_field1f4);
    if (aiInfo->m_vision != defaultScript.m_aiInfo.m_vision)
        fprintf(file, "[vision] %d\n", aiInfo->m_vision);
    if (aiInfo->m_backVision != defaultScript.m_aiInfo.m_backVision)
        fprintf(file, "[back vision] %d\n", aiInfo->m_backVision);
    // ---- [reflection object] +0xc80/+0xc8c ----
    if (!script->m_fieldc8c.empty())
    {
        fwrite("[reflection object]\n", 1, 0x14, file);
        for (unsigned int i = 0; i < script->m_fieldc8c.size(); ++i)
            // ORIG 08a27c66：先 c8c[i] 后 c80[i]（先求值 c80→ebx）
            fprintf(file, "%d %d\n", script->m_fieldc8c[i],
                    script->m_fieldc80[i]);
        fwrite("[/reflection object]\n", 1, 0x15, file);
    }
    // ---- [set damage check] / [is direction to move] ----
    if (script->m_field380 != defaultScript.m_field380)
        fprintf(file, "[set damage check] %d\n", script->m_field380);
    if (script->m_field37c != defaultScript.m_field37c)
        fwrite("[is direction to move] 1\n", 1, 0x19, file);
    // ---- float 三率 ----
    if (script->m_fieldc54 != defaultScript.m_fieldc54)
        fprintf(file, "[absolute damage resistance rate] %f\n",
                (double)script->m_fieldc54);
    if (script->m_field540 != defaultScript.m_field540 ||
        script->m_field544 != defaultScript.m_field544)
        fprintf(file, "[hp regen rate] %f %f\n",
                (double)script->m_field540, (double)script->m_field544);
    if (script->m_fieldc58 != defaultScript.m_fieldc58)
        fprintf(file, "[absolute defense ignore rate] %f\n",
                (double)script->m_fieldc58);
    // ---- [lie frame]（5 项任一不同）----
    {
        bool anyLie = false;
        for (int i = 0; i <= 4; ++i)
        {
            // ORIG 08a27e27：无 break，全 5 轮跑完（仅置位）
            if (script->m_fieldc5c[i] != defaultScript.m_fieldc5c[i])
                anyLie = true;
        }
        if (anyLie)
        {
            fwrite("[lie frame] ", 1, 0xc, file);
            for (int i = 0; i <= 4; ++i)
                fprintf(file, "%d ", script->m_fieldc5c[i]);
            fputc('\n', file);
        }
    }
    // ---- [forced knockback]/[No Combo Rate Monster] ----
    if (script->m_field1cac != defaultScript.m_field1cac)
        fprintf(file, "[forced knockback] %d\n", script->m_field1cac);
    if (script->m_field1ca8 != defaultScript.m_field1ca8)
        fwrite("[No Combo Rate Monster]\n", 1, 0x18, file);
    // ---- [monster title]/[destroy count]/[destroy count damage] ----
    if (!script->m_monsterTitle.empty())
        fprintf(file, "[monster title] `%s`\n", script->m_monsterTitle.c_str());
    if (script->m_field6f4 != defaultScript.m_field6f4)
        fprintf(file, "[destroy count] %d\n", script->m_field6f4);
    if (script->m_field6f8 != defaultScript.m_field6f8)
        fprintf(file, "[destroy count damage] %d\n", script->m_field6f8);
    // ---- [overhead gauge type]/[overhead gauge pos]/[flying mark] ----
    if (script->m_fieldc74 != defaultScript.m_fieldc74)
        fprintf(file, "[overhead gauge type] %d\n", script->m_fieldc74);
    if (script->m_fieldc78 != defaultScript.m_fieldc78 &&
        script->m_fieldc7c != defaultScript.m_fieldc7c)   // ORIG 08a2803b：&&
        fprintf(file, "[overhead gauge pos] %d %d\n",
                script->m_fieldc78, script->m_fieldc7c);
    if (script->m_flagc70 != defaultScript.m_flagc70)
        fwrite("[flying mark] 1\n", 1, 0x10, file);
    // ---- [burning death path] ----
    if (!script->m_field098.empty())
        fprintf(file, "[burning death path] `%s`\n",
                script->m_field098.c_str());
    // ---- [initial target] +0x340 deque ----
    if (!script->m_initialTargets.empty())
    {
        fwrite("[initial target]\n", 1, 0x11, file);
        // ORIG：非 const iterator；两字段各解引用一次（P38）
        for (std::deque<STInitialTarget>::iterator it =
                 script->m_initialTargets.begin();
             it != script->m_initialTargets.end(); ++it)
            fprintf(file, "%s %d\n", (*it).m_field04.c_str(), (*it).m_field00);
        fwrite("[/initial target]\n", 1, 0x12, file);
    }
    // ---- [level of difficulty] +0xf0（5 项）----
    if (!script->m_difficulties.empty())
    {
        fwrite("[level of difficulty]\n", 1, 0x16, file);
        fprintf(file, "[easy] %d %d\n",
                script->m_difficulties[0].m_field00,
                script->m_difficulties[0].m_field04);
        fprintf(file, "[medium] %d %d\n",
                script->m_difficulties[1].m_field00,
                script->m_difficulties[1].m_field04);
        fprintf(file, "[hard] %d %d\n",
                script->m_difficulties[2].m_field00,
                script->m_difficulties[2].m_field04);
        fprintf(file, "[ultimate] %d %d\n",
                script->m_difficulties[3].m_field00,
                script->m_difficulties[3].m_field04);
        fprintf(file, "[hero] %d %d\n",
                script->m_difficulties[4].m_field00,
                script->m_difficulties[4].m_field04);
        fwrite("[/level of difficulty]\n", 1, 0x17, file);
    }
    // ---- 四系 attack（m_paramTable）与 [rigidity]（任一非零）----
    if (script->m_paramTable[0][0] || script->m_paramTable[0][1])
        fprintf(file, "[fire attack] %d %d\n",
                script->m_paramTable[0][0], script->m_paramTable[0][1]);
    if (script->m_paramTable[1][0] || script->m_paramTable[1][1])
        fprintf(file, "[water attack] %d %d\n",
                script->m_paramTable[1][0], script->m_paramTable[1][1]);
    if (script->m_paramTable[2][0] || script->m_paramTable[2][1])
        fprintf(file, "[dark attack] %d %d\n",
                script->m_paramTable[2][0], script->m_paramTable[2][1]);
    if (script->m_paramTable[3][0] || script->m_paramTable[3][1])
        fprintf(file, "[light attack] %d %d\n",
                script->m_paramTable[3][0], script->m_paramTable[3][1]);
    if (script->m_field528[0] || script->m_field528[1])
        fprintf(file, "[rigidity] %d %d\n",
                script->m_field528[0], script->m_field528[1]);
    // ---- [pattern] +0x688（vector<vector<int>> front() 循环）----
    if (!script->m_patterns.empty())
    {
        fwrite("[pattern]\n", 1, 0xa, file);
        // ORIG 条件与体内各自重新求值 front()（P38）
        for (unsigned int i = 0; i < script->m_patterns.front().size(); ++i)
            fprintf(file, "%d\n", script->m_patterns.front()[i]);
        fwrite("[/pattern]\n", 1, 0xb, file);
    }
    // ---- [fix direction] +0xcd4/+0xcd8 ----
    if (script->m_flagcd4 != defaultScript.m_flagcd4)
    {
        std::string dir = "";
        if (script->m_fieldcd8 != defaultScript.m_fieldcd8)
        {
            // ORIG 08a286e2 switch：单 load 入 eax + test je / cmp 1 je 前跳
            switch (script->m_fieldcd8)
            {
            case 0:
                dir = "[left]";
                break;
            case 1:
                dir = "[right]";
                break;
            }
        }
        if (dir == "")
            fwrite("[fix direction] 1\n", 1, 0x12, file);
        else
            fprintf(file, "[fix direction] `%s`\n", dir.c_str());
    }
    // ---- [limit slowdown atkspeed]/[limit slowdown movespeed] ----
    if (script->m_field1c94 != defaultScript.m_field1c94)
        fprintf(file, "[limit slowdown atkspeed] %d\n", script->m_field1c94);
    if (script->m_field1c98 != defaultScript.m_field1c98)
        fprintf(file, "[limit slowdown movespeed] %d\n", script->m_field1c98);
    // ---- [non targeted]/[target appendage] ----
    if (script->m_field56c)
        fwrite("[non targeted] 1\n", 1, 0x11, file);
    if (!script->m_field570.empty())
        fprintf(file, "[target appendage] `%s`\n",
                script->m_field570.c_str());
    // ---- [damage time balance]/[stun time balance] ----
    if (!script->m_field554.empty())
    {
        fwrite("[damage time balance]", 1, 0x15, file);
        for (unsigned int i = 0; i < script->m_field554.size(); ++i)
            fprintf(file, "%d ", script->m_field554[i]);
    }
    if (!script->m_field560.empty())
    {
        fwrite("[stun time balance]", 1, 0x13, file);
        for (unsigned int i = 0; i < script->m_field560.size(); ++i)
            fprintf(file, "%d ", script->m_field560[i]);
    }
    // ---- [add gravity in aerial] ----
    if (script->m_field548)
        fprintf(file, "[add gravity in aerial] 1 %.1f %.1f\n",
                (double)script->m_field54c, (double)script->m_field550);
    // ---- [summon skill status up type] ----
    if (script->m_field3c0 != defaultScript.m_field3c0)
        fprintf(file, "[summon skill status up type] %d\n",
                script->m_field3c0);
    // ---- [attack down prob]（任一 attackMethod +0x4 float 非零）----
    {
        bool anyProb = false;
        for (unsigned int i = 0; i < script->m_attackMethods.size(); ++i)
        {
            // ORIG 08a28a6b：无 break，逐轮置位（flds+cmp0 经 or 物化）
            if (script->m_attackMethods[i].m_field04 != 0.0f)
                anyProb = true;
        }
        if (anyProb)
        {
            fwrite("[attack down prob]\n", 1, 0x13, file);
            for (unsigned int i = 0; i < script->m_attackMethods.size(); ++i)
                fprintf(file, "%f\n",
                        (double)script->m_attackMethods[i].m_field04);
            fwrite("[/attack down prob]\n", 1, 0x14, file);
        }
    }
    // ---- [strong]/[weak]（+0x1ca0 非零；>0 strong，<0 weak 取反）----
    if (script->m_field1ca0)
    {
        if (script->m_field1ca0 > 0)
            fprintf(file, "[strong] %d %.2f\n", script->m_field1ca0,
                    (double)script->m_field1ca4);
        else
            fprintf(file, "[weak] %d %.2f\n", -script->m_field1ca0,
                    (double)script->m_field1ca4);
    }
    // ---- [explain] ----
    if (!script->m_explain.empty())
        fprintf(file, "[explain] `%s`\n", script->m_explain.c_str());
    // ---- [catch item] +0x70（+0x8 按 float→double→%d 低位，ORIG 忠实）----
    if (!script->m_catchItems.empty())
    {
        fwrite("[catch item]\n", 1, 0xd, file);
        for (unsigned int i = 0; i < script->m_catchItems.size(); ++i)
        {
            // ORIG 逐字段重做下标（f8→f4→f0 求值序）；f8 flds→fstpl 双精度
            // 槽（-0x2f60），传参时 fldl/fstpl 二次物化 ⇒ 实参为 double 值。
            fprintf(file, "%d %d %d\n", script->m_catchItems[i].m_field0,
                    script->m_catchItems[i].m_field4,
                    (double)script->m_catchItems[i].m_field8);
        }
        fwrite("[/catch item]\n", 1, 0xe, file);
    }
    // ---- [fixed height]/[can beat index] ----
    if (script->m_field1cb0 != defaultScript.m_field1cb0)
        fprintf(file, "[fixed height] %d\n", script->m_field1cb0);
    if (!script->m_field1cb4.empty())
    {
        fwrite("[can beat index]\n", 1, 0x11, file);
        // ORIG：非 const iterator + 前置 ++（ppEv 实证）
        for (std::vector<int>::iterator it =
                 script->m_field1cb4.begin();
             it != script->m_field1cb4.end(); ++it)
            fprintf(file, "%d\n", *it);
        fwrite("[/can beat index]\n", 1, 0x12, file);
    }
    // ---- [control passive object] 判定（ORIG 08a28e64：bool 组在 mounting 之前算，
    //      函数级无块作用域，槽 -0x120..-0x115）----
    bool cpoIndex   = script->m_field658 != defaultScript.m_field658;
    bool cpoX       = script->m_field65c != defaultScript.m_field65c;
    bool cpoY       = script->m_field660 != defaultScript.m_field660;
    bool cpoW       = script->m_field664 != defaultScript.m_field664;
    bool cpoH       = script->m_field668 != defaultScript.m_field668;
    bool cpoVelX    = script->m_field66c != defaultScript.m_field66c;
    bool cpoVelY    = script->m_field670 != defaultScript.m_field670;
    bool cpoMaster  = script->m_field678 != defaultScript.m_field678;
    bool cpoOthers  = script->m_field67c != defaultScript.m_field67c;
    bool cpoInherit = script->m_field680 != defaultScript.m_field680;
    // ORIG 08a28f7a：transp 以短路 || 物化（mov $1/$0）先于 fix 计算（槽序 -0x116/-0x115）
    bool cpoTransp  = cpoMaster || cpoOthers || cpoInherit;
    bool cpoFix     = script->m_field684 != defaultScript.m_field684;
    // ---- [mounting action]/[mounted action]/[unmounting action]（size!=0）----
    if (script->m_mountingAction.size() != 0)
        fprintf(file, "[mounting action] `%s`\n",
                script->m_mountingAction.c_str());
    if (script->m_mountedAction.size() != 0)
        fprintf(file, "[mounted action] `%s`\n",
                script->m_mountedAction.c_str());
    if (script->m_unmountingAction.size() != 0)
        fprintf(file, "[unmounting action] `%s`\n",
                script->m_unmountingAction.c_str());
    {
        // ORIG 08a290c0：主 OR 链 …,vely(-0x11a) → fix(-0x115) → transp(-0x116)
        if (cpoIndex || cpoX || cpoY || cpoW || cpoH || cpoVelX || cpoVelY ||
            cpoFix || cpoTransp)
        {
            fwrite("[control passive object]\n", 1, 0x19, file);
            if (cpoIndex)
                fprintf(file, "\t[index] %d\n", script->m_field658);
            if (cpoX)
                fprintf(file, "\t[x pos] %d\n", script->m_field65c);
            if (cpoY)
                fprintf(file, "\t[y pos] %d\n", script->m_field660);
            if (cpoW)
                fprintf(file, "\t[width] %d\n", script->m_field664);
            if (cpoH)
                fprintf(file, "\t[height] %d\n", script->m_field668);
            if (cpoVelX)
                fprintf(file, "\t[velocity x] %lf\n",
                        (double)script->m_field66c);
            if (cpoVelY)
                fprintf(file, "\t[velocity y] %lf\n",
                        (double)script->m_field670);
            if (cpoTransp)
            {
                fwrite("\t[transparency]\n", 1, 0x10, file);
                if (cpoMaster)
                    fprintf(file, "\t\t[master] %d\n", script->m_field678);
                if (cpoOthers)
                    fprintf(file, "\t\t[others] %d\n", script->m_field67c);
                if (cpoInherit)
                    fwrite("\t\t[option] `inherit`\n", 1, 0x15, file);
                fwrite("\t[/transparency]\n", 1, 0x11, file);
            }
            if (cpoFix)
            {
                std::string dir;
                // ORIG 08a29334 switch：单 load + test je / cmp1 je 前跳
                switch (script->m_field684)
                {
                case 0:
                    dir = "[left]";
                    break;
                case 1:
                    dir = "[right]";
                    break;
                }
                fprintf(file, "\t[fix direction] `%s`\n", dir.c_str());
            }
            if (script->m_field674)
                fwrite("\t[option] `convert y to z`\n", 1, 0x1b, file);
            fwrite("[/control passive object]\n\n", 1, 0x1b, file);
        }
    }
    // ---- [ridableskill definition] +0xcdc（8 技能槽）----
    for (unsigned int i = 0; i < script->m_ridableSkills.size(); ++i)
    {
        fwrite("[ridableskill definition]\n", 1, 0x1a, file);
        fprintf(file, "\t[pattern] %d\n", script->m_ridableSkills[i].m_field00);
        fwrite("\t[image path] ", 1, 0xe, file);
        std::string img("``");
        if (!script->m_ridableSkills[i].m_name.empty())
            img = script->m_ridableSkills[i].m_name.c_str();
        fprintf(file, "`%s`\n", img.c_str());
        fwrite("\t[skill]\n", 1, 9, file);
        for (int j = 0; j <= 7; ++j)
        {
            // ORIG 每轮一次 operator[] 拷指针，随后字段经该指针访问
            STRidableSkill* rs = &script->m_ridableSkills[i];
            fprintf(file, "\t\t%d", rs->m_field08[j]);
            fprintf(file, " %d", rs->m_fielda8[j]);
            std::string typeName = "normal";
            switch (rs->m_field88[j])
            {
            case 0: typeName = "`normal`"; break;
            case 1: typeName = "`defaultcancel`"; break;
            case 2: typeName = "`movable`"; break;
            case 3: typeName = "`charge`"; break;
            case 4: typeName = "`evasion`"; break;
            case 5: typeName = "`super`"; break;
            default: break;
            }
            fprintf(file, " %s", typeName.c_str());
            // ORIG 08a29735：switch 用表达式（load#1）；此处 register type 重新
            // load（load#2 入 eax），随后 cmp $5 与 shl 复用同一寄存器值。
            register unsigned int type = (unsigned int)rs->m_field88[j];
            if (type < 6)
            {
                // ORIG 08a2974b-08a29766：bits 常驻 ebx（register 局部，不落栈槽）
                register unsigned int bit = 1u << type;
                if ((bit & 0x37u) != 0)
                {
                    int cnt = (int)rs->m_levels[j].size();
                    fprintf(file, " %d", cnt);
                    for (int k = 0; k < cnt; ++k)
                        fprintf(file, " %d", rs->m_levels[j][k]);
                }
                else if ((bit & 8u) != 0)
                    fprintf(file, " %d %d", rs->m_fieldc8[j],
                            rs->m_fielde8[j]);
            }
            fputc('\n', file);
        }
        fwrite("[/ridableskill definition]\n\n", 1, 0x1c, file);
    }
    // ---- [ridableskill explanation] +0xce8 ----
    for (unsigned int i = 0; i < script->m_ridableSkillExplains.size(); ++i)
    {
        fwrite("[ridableskill explanation]\n", 1, 0x1b, file);
        fprintf(file, "\t[pattern] %d\n",
                script->m_ridableSkillExplains[i].m_field10);
        // ORIG 每字段：key=0 在 if 外初始化（08a298xx），命中后 key=c_str()，
        // getTagByName 第2参重做 c_str()、第3参用 key 槽（P38+P15）
        // ORIG 四段共用**同一** key 槽（-0x28 复用）：单声明 + 每段前手动置 0
        const wchar_t* key = 0;
        if (!script->m_ridableSkillExplains[i].m_field00.empty())
        {
            key = (const wchar_t*)script->m_ridableSkillExplains[i].m_field00.c_str();
            fprintf(file, "\t[title] %s\n",
                    toTString(g_scriptStringManager_.getTagByName(
                        7,
                        (const wchar_t*)script->m_ridableSkillExplains[i].m_field00.c_str(),
                        key, false)));
        }
        if (!script->m_ridableSkillExplains[i].m_field04.empty())
        {
            key = (const wchar_t*)script->m_ridableSkillExplains[i].m_field04.c_str();
            fprintf(file, "\t[subtitle] %s\n",
                    toTString(g_scriptStringManager_.getTagByName(
                        7,
                        (const wchar_t*)script->m_ridableSkillExplains[i].m_field04.c_str(),
                        key, false)));
        }
        if (!script->m_ridableSkillExplains[i].m_field08.empty())
        {
            key = (const wchar_t*)script->m_ridableSkillExplains[i].m_field08.c_str();
            fprintf(file, "\t[category] %s\n",
                    toTString(g_scriptStringManager_.getTagByName(
                        7,
                        (const wchar_t*)script->m_ridableSkillExplains[i].m_field08.c_str(),
                        key, false)));
        }
        if (!script->m_ridableSkillExplains[i].m_field0c.empty())
        {
            key = (const wchar_t*)script->m_ridableSkillExplains[i].m_field0c.c_str();
            fprintf(file, "\t[detail] %s\n",
                    toTString(g_scriptStringManager_.getTagByName(
                        7,
                        (const wchar_t*)script->m_ridableSkillExplains[i].m_field0c.c_str(),
                        key, false)));
        }
        fwrite("[/ridableskill explanation]\n\n", 1, 0x1d, file);
    }
    // ---- [physical attack besed on diff]（0xcf4 != -1.0f；200 行 ×5）----
    if (script->m_difficultyScales[0][0] != -1.0f)
    {
        fwrite("[physical attack besed on diff]\n", 1, 0x20, file);
        // ORIG 08a29d21/08a29df2：while (i < 200 && 0.0 <= scales[i][1])，
        // cond1 在 ++i 后（loop 尾 setle 物化），cond2 在 body 头（fucompp+sete）
        int diffRow = 0;
        while (diffRow < 200)
        {
            // ORIG 08a29d34 `add $0xcf8`（= &m_difficultyScales[diffRow][1]）：
            // 原码终止判据读第 2 列，打印循环仍从 col=0 起（P171 照抄）；
            // 比较式写 `x < 0`（探针 g2）才出 ORIG 的 fucompp/test $0x45,%ah/sete
            if (script->m_difficultyScales[diffRow][1] < 0.0f)
                break;
            fprintf(file, "%d", diffRow + 1);
            for (int col = 0; col <= 4; ++col)
            {
                fputc('\t', file);
                // ORIG 08a29d8e：rate 命名 float 槽（mov %eax,-0x1c; flds -0x1c）
                float rate = script->m_difficultyScales[diffRow][col];
                fprintf(file, "%.2f",
                        (double)rate);
            }
            fputc('\n', file);
            ++diffRow;
        }
        fwrite("[/physical attack besed on diff]\n", 1, 0x21, file);
    }
    fclose(file);
    convertBinaryToText(saveFileName);
    return true;
}
// ===========================================================================
// ORIG 中 STMonsterScript.h 各成员类型的 clear/C1/隐式拷贝均为 W 弱符号，
// 产生点由原 TU 中的实际调用触发；本 TU 无自然调用方（STMonsterScript 仅
// 以 vector 容器持有它们，容器操作不 odr-use 元素值类型），此处补 odr-use
// 以在本 TU 产出与 ORIG 符号集对齐的 W 符号（不改变任何语义/布局）。
// ===========================================================================
namespace
{
void EmitStMonsterScriptWeakSymbols()
{
    // 用户内联实现：C1/C2 + clear
    STAtkSuper atkSuper;          atkSuper.clear();
    STCoolTime coolTime;          coolTime.clear();
    STDifficulty difficulty;      difficulty.clear();
    STEventSuper eventSuper;      eventSuper.clear();
    STJumpAttack jumpAttack;      jumpAttack.clear();
    STSummonInfo summonInfo;      summonInfo.clear();
    STThrowAttack throwAttack;    throwAttack.clear();
    STAttackMethod attackMethod;  attackMethod.clear();
    STCounterAttack counterAttack; counterAttack.clear();
    STDash dash;                  dash.clear();
    STHold hold;                  hold.clear();
    STWarp warp;                  warp.clear();
    STSummon summon;              summon.clear();
    STMosterAttackReady attackReady; attackReady.clear();

    // 隐式拷贝构造/拷贝赋值（P74：编译器按成员序合成）
    STCoolTime ctCopy(coolTime);  coolTime = ctCopy;
    STThrowAttack taCopy(throwAttack);  throwAttack = taCopy;
    STInitialTarget initTarget;   STInitialTarget itCopy(initTarget);
                                initTarget = itCopy;
    STRidableSkillExplain skillExplain;
    STRidableSkillExplain seCopy(skillExplain);  skillExplain = seCopy;
    STMosterAttackReady arCopy(attackReady);  attackReady = arCopy;
    STRidableSkill rSkill;        STRidableSkill rsCopy(rSkill);
                                rSkill = rsCopy;

    // 用户 ctor（非 clear 类）
    STMonsterParameterCategory category;

    // STAIInfoScript：destroy W 0x0836753a（头内联实现）+ 隐式拷贝
    // 构造/赋值（ORIG C1ERKS_/aSERKS_ 均为编译器合成版，P74）。
    STAIInfoScript aiInfo;
    aiInfo.destroy();
    STAIInfoScript aiCopy(aiInfo);   // → C1ERKS_/C2ERKS_
    aiInfo = aiCopy;                 // → aSERKS_

    // STMonsterScript：隐式 op=（ORIG W 0x08377994 编译器合成版）；
    // ORIG 无 C1ERKS_，故只 force-use 赋值不触发拷贝构造。
    STMonsterScript monster1;
    STMonsterScript monster2;
    monster1 = monster2;

    // 无用户 ctor 类的隐式 C1（STRidableSkill/STInitialTarget/
    // STRidableSkillExplain 已在上面默认构造触发）

    // 隐式 op=（cpp 已定义 C1/clear）
    STMonsterEquipment eq1;       STMonsterEquipment eq2;
    eq1 = eq2;

    // POD 类显式析构（D1/D2；trivial 析构容器路径不实例化）
    CatchItemInfo catchItem;
    catchItem.~CatchItemInfo();
    stEnamyDropAvatarElement_t dropElement;
    dropElement.~stEnamyDropAvatarElement_t();

    (void)atkSuper; (void)coolTime; (void)difficulty; (void)eventSuper;
    (void)jumpAttack; (void)summonInfo; (void)throwAttack; (void)attackMethod;
    (void)counterAttack; (void)dash; (void)hold; (void)warp; (void)summon;
    (void)attackReady; (void)ctCopy; (void)taCopy; (void)initTarget;
    (void)itCopy; (void)skillExplain; (void)seCopy; (void)arCopy;
    (void)rSkill; (void)rsCopy;
    (void)aiInfo; (void)aiCopy; (void)monster1; (void)monster2;
    (void)category;
}
}  // namespace
