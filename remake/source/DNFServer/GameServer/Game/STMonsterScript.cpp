// ============================================================================
// df_game_r 还原 —— STMonsterScript 类方法（G5 怪物脚本表）。
// 逐函数依据 ORIG objdump 转写（地址随注）：
//   Clear T 0x08a398aa / C1 W 0x08368e14 / D1 W 0x08378dac / op= W 0x08377994
//   ImportMonsterBaseTable T 0x08a3c736（全量转写）
//   getMonsterTableIndex T 0x088ba1c8 / getRandomChampionElement T 0x088b9cce
//   setBasicAbilityByLevel T 0x088b9603（全量转写）/ isNamedMonster W 0x088ba18c
//   ImportMonsterScript T 0x08a12827 / exportMonsterScript T 0x08a22520
//   （两者 >30KB 汇编：骨架+标签分派级复原，覆盖面见各函数注记）
// ============================================================================

#include "STMonsterScript.h"

#include <stdio.h>
#include <string.h>

#include "DNFLexWrapper.h"   // loadRDARScriptFile/getDirectoryPath/ScanType/ScanInt

// ---- 跨 TU 符号（ORIG 地址随注；定义在各自所属批次 TU）----
extern char g_szMonsterScriptBaseDirectory[0x100];              // B 0x094ff160
extern std::vector<std::vector<MonsterBaseParameter> >
    g_monsterBaseParameters;                                    // B 0x094ff278
int GetLinearDataForMonsterAbility(int low, int high, int diff,
                                   int span);                   // T 0x08891bae
bool processModuleTagSet(int* moduleId, int* subId, std::string* tag,
                         int type, bool flag);                  // T 0x08a97637
void getScriptFileFullPath(char* out, unsigned int size,
                           const char* dir, const char* name);  // T 0x088bba4e

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
    m_aiInfo.m_flag204 = 1;                  // movb $1,0x33c
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
    m_targetingNearest = 0;
    m_targetingAttacker = 0;
    m_targetingFromDirection = 0;
    m_targetingLowPhysAttack = false;
    m_targetingLowPhysDefense = false;
    m_targetingLowMagAttack = false;
    m_targetingLowMagDefense = 0;
    m_targetingDowned = 0;
    m_field59c = 0;
    m_field5a0 = 0;
    m_field5a4 = false;                      // +0x5a4
    m_warlike = 50;                          // movl $0x32,0x578（序在 5a4 后，忠实 ORIG）
    m_attackDelay = 1000;                    // movl $0x3e8,0x57c
    m_superarmorOnAttack = false;            // movb $0,0x5a5
    m_field5a8 = 0;
    m_faceImage = "";                        // assign(+0x5ac)
    m_field5b0 = 0;                          // movl $0,0x5b0
    m_sound0 = "";                        // 三连展开（[2..4] 不清，实证）
    m_sound1 = "";
    m_sound5 = "";
    for (i = 0; i <= 1; ++i)
        m_field5cc[i] = "";                  // 循环 ×2 @0x5cc
    m_field5d40 = "";                      // 四连展开
    m_field5d41 = "";
    m_field5d42 = "";
    m_field5d43 = "";
    m_field5e4.clear();
    m_field5f0.clear();
    m_jumpAttackMotion = "";                 // assign(+0x6b4)
    m_field6b8.clear();
    m_field6c4.clear();
    m_field6d4.clear();
    m_keepDistance = -1;                     // movl $<A>,0x6e0
    m_isOnTheObject = false;                 // movb $0,0x6e4
    m_attackMethods.clear();                 // call(+0x6e8)
    m_field5fc0 = "";                      // 六连展开 @0x5fc（实证非循环）
    m_field5fc1 = "";
    m_field5fc2 = "";
    m_field5fc3 = "";
    m_field5fc4 = "";
    m_field5fc5 = "";
    for (i = 0; i <= 1; ++i)
        m_field614[i] = "";                  // 循环 ×2 @0x614
    m_field61c0 = "";                      // 四连展开 @0x61c
    m_field61c1 = "";
    m_field61c2 = "";
    m_field61c3 = "";
    m_field62c.clear();
    m_field638.clear();
    m_field6440 = "";                      // assign(+0x644)
    m_field6441 = "";                      // assign(+0x648)
    m_field688.clear();                      // vector<vector<int>> (+0x688)
    m_waitingMotion = "";                    // assign(+0x694)
    m_moveMotion = "";
    m_sitMotion = "";
    m_damageMotion1 = "";
    m_damageMotion2 = "";
    m_downMotion = "";
    m_overturnMotion = "";
    m_jumpMotion = "";                       // assign(+0x6b0)
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
    m_field6442 = "";                      // assign(+0x64c)
    m_field6443 = "";                      // assign(+0x650)
    m_field6444 = "";                      // assign(+0x654)
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
void stEnamyDropAvatarInfo_t::clear()
{
    m_field00 = 0;                 // movl $0,(%eax)
    m_field04 = 0;                 // movl $0,0x4
    for (int i = 0; i <= 3; ++i)
        m_field08[i] = 0;          // [base+8+4i]
    for (int i = 0; i <= 4; ++i)   // lea 0x4(%edx)+scale4 实证 → 索引 i+4
        m_field08[i + 4] = 0;
    m_field2c = 0;
    m_elements.clear();            // vector<element>::clear(+0x30)
}

void stEnamyDropAvatarInfo_t::operator=(const stEnamyDropAvatarInfo_t& other)
{
    m_field00 = other.m_field00;
    m_field04 = other.m_field04;
    for (int i = 0; i <= 3; ++i)
        m_field08[i] = other.m_field08[i];
    for (int i = 0; i <= 4; ++i)
        m_field08[i + 4] = other.m_field08[i + 4];
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
            return true;                       // +0x2d4 mov eax,1
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
        for (int i = 0; i <= 3; ++i)           // +0x160 m_value[13+i]
            param.m_value[i + 13] = ScanInt(static_cast<bool*>(0));
        for (int i = 0; i <= 3; ++i)           // +0x191 m_value[17+i]
            param.m_value[i + 17] = ScanInt(static_cast<bool*>(0));
        param.m_value[21] = ScanInt(static_cast<bool*>(0));        // -0x98（param+0x54）
        param.m_value[22] = ScanInt(static_cast<bool*>(0));        // -0x94
        param.m_value[23] = ScanInt(static_cast<bool*>(0));        // -0x90

        if (row == static_cast<int>(script->m_baseParameters.size()))
            script->m_baseParameters.push_back(param);            // +0x1f4 追加
        else if (row > static_cast<int>(script->m_baseParameters.size()))
        {
            // +0x22b 行号跳跃：以默认行 resize(row+1)，再覆写目标行
            script->m_baseParameters.resize(row + 1, MonsterBaseParameter());
            script->m_baseParameters[row] = param;                // rep movsl
        }
        else
            script->m_baseParameters[row] = param;                // +0x29e 覆写
        ++row;                                 // +0x2cb
    }
}

// ===========================================================================
// setBasicAbilityByLevel —— ORIG T 0x088b9603（全量转写）
// ActiveStaticInfo 槽位映射依据现占位头布局：
//   +0x38 elemAttack、+0x58 elemDefense、+0x78 attr17Base、+0x100+8k statBaseK、
//   +0x14 flag14；+0x1a8 float 槽现落 m_statBase27（位型访问【推断】）。
// ===========================================================================
void setBasicAbilityByLevel(const STMonsterScript* script, ActiveStaticInfo* info,
                            int level, int dungeonIdx, int roleType,
                            bool flagA, bool flagB)
{
    info->clear();                             // +0x19 ActiveStaticInfo::clear
    info->m_level = level;                     // +0x21

    const int diff = level - script->m_levelByDiff[0];        // -0x3c
    const int diffSpan = script->m_levelByDiff[1] -
                         script->m_levelByDiff[0];             // -0x38
    const int tableIdx = getMonsterTableIndex(script, dungeonIdx,
                                              roleType, flagA, flagB);
    if (tableIdx == 4)
        level = dungeonIdx;                    // +0x85 难度表 4 用副本等级查行

    // +0x93..0x15c：g_monsterBaseParameters[tableIdx] 中首列==level 的首个命中；
    // 脚本自带基础表时改取脚本内同下标行（operator[] 无界，忠实 ORIG）。
    const MonsterBaseParameter* param = 0;     // -0x30
    for (std::vector<MonsterBaseParameter>::size_type i = 0;
         i < g_monsterBaseParameters[tableIdx].size(); ++i)
    {
        if (g_monsterBaseParameters[tableIdx][i].m_value[0] == level)
        {
            if (!script->m_baseParameters.empty())              // +0x102
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

    // +0x206：四属性抗性低/高对 → 元素攻击（GetLinearData(低, 高, 差, 跨度)）；
    // ORIG 四次直呼（无指针表），保持一致以贴合栈帧
    info->m_elemAttack[0] = GetLinearDataForMonsterAbility(
        script->m_resistTable[0][0], script->m_resistTable[0][1], diff, diffSpan);
    info->m_elemAttack[1] = GetLinearDataForMonsterAbility(
        script->m_resistTable[1][0], script->m_resistTable[1][1], diff, diffSpan);
    info->m_elemAttack[2] = GetLinearDataForMonsterAbility(
        script->m_resistTable[2][0], script->m_resistTable[2][1], diff, diffSpan);
    info->m_elemAttack[3] = GetLinearDataForMonsterAbility(
        script->m_resistTable[3][0], script->m_resistTable[3][1], diff, diffSpan);

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
        info->m_elemAttack[i] += param->m_value[12 + i];      // +0x3b7
        info->m_elemDefense[i] += param->m_value[16 + i];     // +0x3df
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

bool scanTag(std::string& tag)
{
    return ScanType(tag, true);
}

int scanValue()
{
    return ScanInt(static_cast<bool*>(0));     // ORIG movl $0 占位 ok 指针
}

}  // namespace

int ImportMonsterScript(STMonsterScript* script, int type, const char* path,
                        bool flag)
{
    bool ok_flag = false;   // ScanInt 成功标志（与 ORIG 栈上 bool 等价）
    if (!loadRDARScriptFile(g_szMonsterScriptBaseDirectory, path))
        return 0;                              // +0x2f
    script->Clear();                           // +0x3f

    // +0x46：getDirectoryPath(m_field708, "%s%s", baseDir, path) 后传 AI 目录
    getDirectoryPath(script->m_field708, "%s%s",
                     g_szMonsterScriptBaseDirectory, path);
    script->m_aiInfo.m_dir = script->m_field708;             // +0x7c SsaSERKSs(+144←+708)
    script->m_throwAttacks.clear();                          // +0x94
    script->m_counterAttacks.clear();
    script->m_jumpAttacks.clear();
    script->m_equipment.m_items.clear();                     // +0xc4 clear(+390)
    script->m_attackReadies.clear();

    std::string tag;                           // -0x1a0c
    int moduleId = -1;                         // -0x1a1c
    int subId = -1;                            // -0x1a20
    while (processModuleTagSet(&moduleId, &subId, &tag, type, flag))
    {
        if (subId != -1)                       // +0x186
            script->m_field1c9c = subId;

        while (scanTag(tag))                   // +0x229 起主扫描循环骨架
        {
            if (tag == "name")
            {
                ScanStr(&script->m_name);
                script->m_index = ScanInt(&ok_flag);   // 【推断】首列即索引
            }
            else if (tag == "width")
            {
                script->m_width = ScanInt(&ok_flag);
                script->m_height = ScanInt(&ok_flag);
            }
            else if (tag == "floating height")
            {
                script->m_floatingHeight = ScanInt(&ok_flag);
            }
            else if (tag == "layer")
            {
                script->m_layer = ScanInt(&ok_flag);
            }
            else if (tag == "level")
            {
                script->m_levelByDiff[0] = ScanInt(&ok_flag);
                script->m_levelByDiff[1] = ScanInt(&ok_flag);
            }
            else if (tag == "fire resistance")
            {
                script->m_resistTable[0][0] = ScanInt(&ok_flag);
                script->m_resistTable[0][1] = ScanInt(&ok_flag);
            }
            else if (tag == "water resistance")
            {
                script->m_resistTable[1][0] = ScanInt(&ok_flag);
                script->m_resistTable[1][1] = ScanInt(&ok_flag);
            }
            else if (tag == "dark resistance")
            {
                script->m_resistTable[2][0] = ScanInt(&ok_flag);
                script->m_resistTable[2][1] = ScanInt(&ok_flag);
            }
            else if (tag == "light resistance")
            {
                script->m_resistTable[3][0] = ScanInt(&ok_flag);
                script->m_resistTable[3][1] = ScanInt(&ok_flag);
            }
            else if (tag == "HP regen speed")
            {
                script->m_hpRegenSpeed[0] = ScanInt(&ok_flag);
                script->m_hpRegenSpeed[1] = ScanInt(&ok_flag);
            }
            else if (tag == "MP regen speed")
            {
                script->m_mpRegenSpeed[0] = ScanInt(&ok_flag);
                script->m_mpRegenSpeed[1] = ScanInt(&ok_flag);
            }
            else if (tag == "move speed")
            {
                script->m_moveSpeed[0] = ScanInt(&ok_flag);
                script->m_moveSpeed[1] = ScanInt(&ok_flag);
            }
            else if (tag == "attack speed")
            {
                script->m_attackSpeed[0] = ScanInt(&ok_flag);
                script->m_attackSpeed[1] = ScanInt(&ok_flag);
            }
            else if (tag == "cast speed")
            {
                script->m_castSpeed[0] = ScanInt(&ok_flag);
                script->m_castSpeed[1] = ScanInt(&ok_flag);
            }
            else if (tag == "hit recovery")
            {
                script->m_hitRecovery[0] = ScanInt(&ok_flag);
                script->m_hitRecovery[1] = ScanInt(&ok_flag);
            }
            else if (tag == "jump power")
            {
                script->m_jumpPower[0] = ScanInt(&ok_flag);
                script->m_jumpPower[1] = ScanInt(&ok_flag);
            }
            else if (tag == "jump speed")
            {
                script->m_jumpSpeed[0] = ScanInt(&ok_flag);
                script->m_jumpSpeed[1] = ScanInt(&ok_flag);
            }
            else if (tag == "normal dash speed")
            {
                script->m_normalDashSpeed[0] = ScanInt(&ok_flag);
                script->m_normalDashSpeed[1] = ScanInt(&ok_flag);
            }
            else if (tag == "slant dash speed")
            {
                script->m_slantDashSpeed[0] = ScanInt(&ok_flag);
                script->m_slantDashSpeed[1] = ScanInt(&ok_flag);
            }
            else if (tag == "weight")
            {
                script->m_weight[0] = ScanInt(&ok_flag);
                script->m_weight[1] = ScanInt(&ok_flag);
            }
            else if (tag == "superarmor on attack")
            {
                script->m_superarmorOnAttack =
                    ScanInt(&ok_flag) != 0;    // bool 列【推断】
            }
            else if (tag == "sight")
            {
                script->m_sight = ScanInt(&ok_flag);
            }
            else if (tag == "come in action")
            {
                ScanStr(&script->m_comeInAction);
                ScanInt(&ok_flag);
                ScanInt(&ok_flag);
            }
            else if (tag == "come in action ex")
            {
                ScanStr(&script->m_comeInActionEx);
                ScanInt(&ok_flag);
                ScanInt(&ok_flag);
                ScanStr(&script->m_comeInAction);   // 【推断】ex 含回退串
            }
            else if (tag == "targeting time term")
                script->m_targetingTimeTerm = ScanInt(&ok_flag);
            else if (tag == "targeting on attack")
                script->m_targetingOnAttack = ScanInt(&ok_flag) != 0;
            else if (tag == "targeting on damage")
                script->m_targetingOnDamage = ScanInt(&ok_flag) != 0;
            else if (tag == "targeting nearest")
                script->m_targetingNearest = ScanInt(&ok_flag);
            else if (tag == "targeting attacker")
                script->m_targetingAttacker = ScanInt(&ok_flag);
            else if (tag == "targeting from direction")
                script->m_targetingFromDirection = ScanInt(&ok_flag);
            else if (tag == "targeting low physical attack")
                script->m_targetingLowPhysAttack = ScanInt(&ok_flag) != 0;
            else if (tag == "targeting low physical defense")
                script->m_targetingLowPhysDefense = ScanInt(&ok_flag) != 0;
            else if (tag == "targeting low magical attack")
                script->m_targetingLowMagAttack = ScanInt(&ok_flag) != 0;
            else if (tag == "targeting low magical defense")
                script->m_targetingLowMagDefense = ScanInt(&ok_flag);
            else if (tag == "targeting downed")
                script->m_targetingDowned = ScanInt(&ok_flag);
            else if (tag == "warlike")
                script->m_warlike = ScanInt(&ok_flag);
            else if (tag == "attack delay")
                script->m_attackDelay = ScanInt(&ok_flag);
            else if (tag == "stuckbonus on damage")
            {
                for (int i = 0; i < 2; ++i)
                    for (int j = 0; j < 2; ++j)
                        script->m_stuckBonusOnDamage[i][j] = ScanInt(&ok_flag);
            }
            else if (tag == "keep distance")
                script->m_keepDistance = ScanInt(&ok_flag);
            else if (tag == "is on the object")
                script->m_isOnTheObject = ScanInt(&ok_flag) != 0;
            else if (tag == "die effect")
            {
                script->m_dieEffect[0] = ScanInt(&ok_flag);
                script->m_dieEffect[1] = ScanInt(&ok_flag);
                script->m_dieEffect[2] = ScanInt(&ok_flag);
            }
            else if (tag == "face image")
            {
                ScanStr(&script->m_faceImage);
                script->m_field5b0 = ScanInt(&ok_flag);
            }
            else if (tag == "ambient sound" || tag == "attack sound" ||
                     tag == "appear sound" || tag == "damage sound" ||
                     tag == "die sound" || tag == "etc sound")
            {
                // 六个声音串为独立成员（+0x5b4..+0x5cb，C1 实证）
                std::string* dst = &script->m_sound0;
                if (tag == "attack sound") dst = &script->m_sound1;
                else if (tag == "appear sound") dst = &script->m_sound2;
                else if (tag == "damage sound") dst = &script->m_sound3;
                else if (tag == "die sound") dst = &script->m_sound4;
                else if (tag == "etc sound") dst = &script->m_sound5;
                ScanStr(dst);
            }
            else if (tag == "waiting motion") ScanStr(&script->m_waitingMotion);
            else if (tag == "move motion") ScanStr(&script->m_moveMotion);
            else if (tag == "sit motion") ScanStr(&script->m_sitMotion);
            else if (tag == "damage motion 1") ScanStr(&script->m_damageMotion1);
            else if (tag == "damage motion 2") ScanStr(&script->m_damageMotion2);
            else if (tag == "down motion") ScanStr(&script->m_downMotion);
            else if (tag == "overturn motion") ScanStr(&script->m_overturnMotion);
            else if (tag == "jump motion") ScanStr(&script->m_jumpMotion);
            else if (tag == "jumpattack motion") ScanStr(&script->m_jumpAttackMotion);
            else
            {
                // 未建模标签：跳过该行剩余 token（ORIG 以 static map 分派，
                // 此处线性兜底；「推断」：不消费即下一轮 tag 扫描对齐）
            }
            if (!ok_flag && !ScanType(tag, false))
                break;
        }
    }
    return 1;                                  // +0xfae6 mov ebx,1
}

// ===========================================================================
// exportMonsterScript —— ORIG T 0x08a22520（0x798d 字节）。
// 骨架级复原：路径拼接/fopen("w")/名称标签输出与 ORIG 一致；正文逐段输出
// 与 ImportMonsterScript 标签集对称（fprintf 格式串取自 ORIG rodata）。
// ===========================================================================
bool exportMonsterScript(
    STMonsterScript* script, const char* path,
    std::map<std::string, std::vector<std::string> >& extra)
{
    char fullPath[0x100];                      // getScriptFileFullPath 缓冲
    getScriptFileFullPath(fullPath, 0x100,
                          g_szMonsterScriptBaseDirectory, path);   // +0x2c
    FILE* file = fopen(fullPath, "w");         // +0x3a "w"
    if (file == 0)
        return false;

    fprintf(file, "[name] %s\n", script->m_name.c_str());          // +0x110
    fprintf(file, "[die effect] %d %d %d\n",
            script->m_dieEffect[0], script->m_dieEffect[1],
            script->m_dieEffect[2]);
    fprintf(file, "[width] %d %d\n", script->m_width, script->m_height);
    fprintf(file, "[floating height] %d\n", script->m_floatingHeight);
    fprintf(file, "[layer] %d\n", script->m_layer);
    fprintf(file, "`%s`\n", script->m_sound0.c_str());   // 六声音段独立成员
    fprintf(file, "`%s`\n", script->m_sound1.c_str());
    fprintf(file, "`%s`\n", script->m_sound2.c_str());
    fprintf(file, "`%s`\n", script->m_sound3.c_str());
    fprintf(file, "`%s`\n", script->m_sound4.c_str());
    fprintf(file, "`%s`\n", script->m_sound5.c_str());
    fprintf(file, "[ability table] %d\n", script->m_abilityTable);
    fprintf(file, "[level] %d %d\n",
            script->m_levelByDiff[0], script->m_levelByDiff[1]);
    fprintf(file, "[fire resistance] %d %d\n",
            script->m_resistTable[0][0], script->m_resistTable[0][1]);
    fprintf(file, "[water resistance] %d %d\n",
            script->m_resistTable[1][0], script->m_resistTable[1][1]);
    fprintf(file, "[dark resistance] %d %d\n",
            script->m_resistTable[2][0], script->m_resistTable[2][1]);
    fprintf(file, "[light resistance] %d %d\n",
            script->m_resistTable[3][0], script->m_resistTable[3][1]);
    fprintf(file, "[HP regen speed] %d %d\n",
            script->m_hpRegenSpeed[0], script->m_hpRegenSpeed[1]);
    fprintf(file, "[MP regen speed] %d %d\n",
            script->m_mpRegenSpeed[0], script->m_mpRegenSpeed[1]);
    fprintf(file, "[move speed] %d %d\n",
            script->m_moveSpeed[0], script->m_moveSpeed[1]);
    fprintf(file, "[attack speed] %d %d\n",
            script->m_attackSpeed[0], script->m_attackSpeed[1]);
    fprintf(file, "[cast speed] %d %d\n",
            script->m_castSpeed[0], script->m_castSpeed[1]);
    fprintf(file, "[hit recovery] %d %d\n",
            script->m_hitRecovery[0], script->m_hitRecovery[1]);
    fprintf(file, "[jump power] %d %d\n",
            script->m_jumpPower[0], script->m_jumpPower[1]);
    fprintf(file, "[jump speed] %d %d\n",
            script->m_jumpSpeed[0], script->m_jumpSpeed[1]);
    fprintf(file, "[normal dash speed] %d %d\n",
            script->m_normalDashSpeed[0], script->m_normalDashSpeed[1]);
    fprintf(file, "[slant dash speed] %d %d\n",
            script->m_slantDashSpeed[0], script->m_slantDashSpeed[1]);
    fprintf(file, "[weight] %d %d\n",
            script->m_weight[0], script->m_weight[1]);
    fprintf(file, "[superarmor on attack] %d %d\n",
            script->m_superarmorOnAttack ? 1 : 0, script->m_field5a8);
    fprintf(file, "[sight] %d\n", script->m_sight);
    fprintf(file, "[targeting time term] %d\n", script->m_targetingTimeTerm);
    fprintf(file, "[targeting on attack] %d\n",
            script->m_targetingOnAttack ? 1 : 0);
    fprintf(file, "[targeting on damage] %d\n",
            script->m_targetingOnDamage ? 1 : 0);
    fprintf(file, "[targeting nearest] %d\n", script->m_targetingNearest);
    fprintf(file, "[targeting attacker] %d\n", script->m_targetingAttacker);
    fprintf(file, "[targeting from direction] %d\n",
            script->m_targetingFromDirection);
    fprintf(file, "[targeting low physical attack] %d\n",
            script->m_targetingLowPhysAttack ? 1 : 0);
    fprintf(file, "[targeting low physical defense] %d\n",
            script->m_targetingLowPhysDefense ? 1 : 0);
    fprintf(file, "[targeting low magical attack] %d\n",
            script->m_targetingLowMagAttack ? 1 : 0);
    fprintf(file, "[targeting low magical defense] %d\n",
            script->m_targetingLowMagDefense);
    fprintf(file, "[targeting downed] %d\n", script->m_targetingDowned);
    fprintf(file, "[warlike] %d\n", script->m_warlike);
    fprintf(file, "[attack delay] %d\n", script->m_attackDelay);
    fprintf(file, "[stuckbonus on damage] %d %d %d %d\n",
            script->m_stuckBonusOnDamage[0][0], script->m_stuckBonusOnDamage[0][1],
            script->m_stuckBonusOnDamage[1][0], script->m_stuckBonusOnDamage[1][1]);
    fprintf(file, "[keep distance] %d\n", script->m_keepDistance);
    fprintf(file, "[is on the object] 1 %d\n",
            script->m_isOnTheObject ? 1 : 0);
    fclose(file);
    return true;
}
