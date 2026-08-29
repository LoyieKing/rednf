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
bool processModuleTagSet(int& moduleId, int& subId, std::string* tag,
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

namespace {
static int MonsterTagLookup(const std::string& tag)
{
    if (tag == "[name]") return 2580;
    if (tag == "[width]") return 2590;
    if (tag == "[floating height]") return 2593;
    if (tag == "[layer]") return 2597;
    if (tag == "[ambient sound]") return 2620;
    if (tag == "[attack sound]") return 2630;
    if (tag == "[appear sound]") return 2635;
    if (tag == "[damage sound]") return 2640;
    if (tag == "[die sound]") return 2645;
    if (tag == "+") return 2672;
    if (tag == "[ability table]") return 2675;
    if (tag == "[summon skill status up rate]") return 2679;
    if (tag == "[level]") return 2683;
    if (tag == "[fire resistance]") return 2687;
    if (tag == "[water resistance]") return 2691;
    if (tag == "[dark resistance]") return 2695;
    if (tag == "[light resistance]") return 2699;
    if (tag == "[HP regen speed]") return 2703;
    if (tag == "[MP regen speed]") return 2707;
    if (tag == "[move speed]") return 2711;
    if (tag == "[attack speed]") return 2723;
    if (tag == "[cast speed]") return 2727;
    if (tag == "[hit recovery]") return 2731;
    if (tag == "[jump power]") return 2735;
    if (tag == "[jump speed]") return 2740;
    if (tag == "[normal dash speed]") return 2745;
    if (tag == "[slant dash speed]") return 2751;
    if (tag == "[weight]") return 2755;
    if (tag == "[superarmor on attack]") return 2759;
    if (tag == "[sight]") return 2762;
    if (tag == "[come in action]") return 2767;
    if (tag == "[come in action ex]") return 2774;
    if (tag == "[diving sight]") return 2778;
    if (tag == "[targeting time term]") return 2784;
    if (tag == "[targeting on damage]") return 2787;
    if (tag == "[targeting high level]") return 2791;
    if (tag == "[targeting low level]") return 2795;
    if (tag == "[targeting high HP]") return 2799;
    if (tag == "[targeting low HP]") return 2803;
    if (tag == "[targeting nearest]") return 2806;
    if (tag == "[targeting attacker]") return 2809;
    if (tag == "[targeting from direction]") return 2812;
    if (tag == "[targeting low physical attack]") return 2815;
    if (tag == "[targeting low physical defense]") return 2818;
    if (tag == "[targeting low magical attack]") return 2821;
    if (tag == "[targeting low magical defense]") return 2824;
    if (tag == "[targeting downed]") return 2827;
    if (tag == "[warlike]") return 2837;
    if (tag == "[stuckbonus on damage]") return 2843;
    if (tag == "[item]") return 2853;
    if (tag == "[common champion drop item]") return 2865;
    if (tag == "[hell monster]") return 2870;
    if (tag == "[boss drop item]") return 2880;
    if (tag == "[death tower item]") return 2987;
    if (tag == "[super champion drop item]") return 3016;
    if (tag == "[keep distance]") return 3030;
    if (tag == "[is on the object]") return 3036;
    if (tag == "[out of targetting]") return 3040;
    if (tag == "[die effect]") return 3066;
    if (tag == "[face image]") return 3075;
    if (tag == "[move motion]") return 3087;
    if (tag == "[sit motion]") return 3099;
    if (tag == "[damage motion 1]") return 3111;
    if (tag == "[damage motion 2]") return 3123;
    if (tag == "[down motion]") return 3135;
    if (tag == "[overturn motion]") return 3147;
    if (tag == "[jump motion]") return 3159;
    if (tag == "[jumpattack motion]") return 3171;
    if (tag == "[attack motion]") return 3183;
    if (tag == "[etc motion]") return 3209;
    if (tag == "[waiting action]") return 3235;
    if (tag == "[move action]") return 3244;
    if (tag == "[sit action]") return 3247;
    if (tag == "[damage action 1]") return 3250;
    if (tag == "[damage action 2]") return 3253;
    if (tag == "[down action]") return 3256;
    if (tag == "[overturn action]") return 3259;
    if (tag == "[jump action]") return 3262;
    if (tag == "[jumpattack action]") return 3265;
    if (tag == "[basic action]") return 3266;
    if (tag == "[action list]") return 3272;
    if (tag == "[etc action]") return 3277;
    if (tag == "[last action]") return 3280;
    if (tag == "[attack info]") return 3289;
    if (tag == "[etc attack info]") return 3295;
    if (tag == "[int data]") return 3306;
    if (tag == "[on attack]") return 3321;
    if (tag == "[on damage]") return 3330;
    if (tag == "[on die]") return 3333;
    if (tag == "[on revenge]") return 3336;
    if (tag == "[on etc]") return 3339;
    if (tag == "[on appear]") return 3342;
    if (tag == "[on disappear]") return 3345;
    if (tag == "[on go to help]") return 3348;
    if (tag == "[fix move direction]") return 3357;
    if (tag == "[passive object filename]") return 3370;
    if (tag == "[passive object number]") return 3380;
    if (tag == "[passive object index]") return 3386;
    if (tag == "[power]") return 3389;
    if (tag == "[passive object start x cood]") return 3396;
    if (tag == "[passive object start y cood]") return 3403;
    if (tag == "[passive object start z cood]") return 3410;
    if (tag == "[direction]") return 3417;
    if (tag == "[attack number]") return 3420;
    if (tag == "[attack index]") return 3426;
    if (tag == "[throw sound]") return 3429;
    if (tag == "[equipment]") return 3438;
    if (tag == "[ready action]") return 3451;
    if (tag == "[counter attack]") return 3478;
    if (tag == "[probability]") return 3489;
    if (tag == "[approach attack]") return 3511;
    if (tag == "[need hp]") return 3520;
    if (tag == "[start frame index]") return 3535;
    if (tag == "[end frame index]") return 3538;
    if (tag == "[jump height]") return 3541;
    if (tag == "[on collision custom animation index]") return 3547;
    if (tag == "[ememy distance rate]") return 3550;
    if (tag == "[limited x cood]") return 3553;
    if (tag == "[limited y cood]") return 3556;
    if (tag == "[jump direction]") return 3564;
    if (tag == "[function index]") return 3572;
    if (tag == "[summon term]") return 3593;
    if (tag == "[enterance effect]") return 3605;
    if (tag == "[dash attack]") return 3613;
    if (tag == "[shakeable]") return 3631;
    if (tag == "[first speed]") return 3634;
    if (tag == "[last speed]") return 3640;
    if (tag == "[dash time]") return 3643;
    if (tag == "[animation first index]") return 3651;
    if (tag == "[offset]") return 3676;
    if (tag == "[animation index]") return 3679;
    if (tag == "[max hold]") return 3687;
    if (tag == "[warp style]") return 3706;
    if (tag == "[after attack]") return 3709;
    if (tag == "[after appear time]") return 3712;
    if (tag == "[cool time]") return 3715;
    if (tag == "[target offset]") return 3718;
    if (tag == "[event super on scr]") return 3742;
    if (tag == "[on time]") return 3749;
    if (tag == "[on hp]") return 3761;
    if (tag == "[always]") return 3767;
    if (tag == "[counter]") return 3773;
    if (tag == "[breakable]") return 3780;
    if (tag == "[half]") return 3787;
    if (tag == "[unbreakable]") return 3794;
    if (tag == "[with blast]") return 3802;
    if (tag == "[/cooltime]") return 3875;
    if (tag == "[spawn prob]") return 3879;
    if (tag == "[drop item offset]") return 3883;
    if (tag == "[is grabable]") return 3886;
    if (tag == "[ai pattern]") return 3888;
    if (tag == "[think term]") return 3889;
    if (tag == "[destination change term]") return 3890;
    if (tag == "[targeting bonus]") return 3891;
    if (tag == "[keep distance with target]") return 3892;
    if (tag == "[vision]") return 3894;
    if (tag == "[intelligence]") return 3895;
    if (tag == "[keep range distance with target]") return 3896;
    if (tag == "[back vision]") return 3897;
    if (tag == "[no change target]") return 3898;
    if (tag == "[keep range distance with target 1]") return 3900;
    if (tag == "[keep range distance with target 2]") return 3901;
    if (tag == "[keep range distance with target 3]") return 3902;
    if (tag == "[keep range distance with target 4]") return 3910;
    if (tag == "[reflection object]") return 3927;
    if (tag == "[set damage check]") return 3932;
    if (tag == "[change direction by target]") return 3938;
    if (tag == "[is direction to move]") return 3943;
    if (tag == "[hp regen rate]") return 3954;
    if (tag == "[absolute defense ignore rate]") return 3958;
    if (tag == "[lie frame]") return 3963;
    if (tag == "[forced knockback]") return 3968;
    if (tag == "[No Combo Rate Monster]") return 3975;
    if (tag == "[proc action]") return 3978;
    if (tag == "[monster title]") return 3981;
    if (tag == "[destroy count]") return 3993;
    if (tag == "[destroy count damage]") return 3997;
    if (tag == "[overhead gauge pos]") return 4002;
    if (tag == "[flying mark]") return 4007;
    if (tag == "[moveback action]") return 4010;
    if (tag == "[dash action]") return 4016;
    if (tag == "[dashattack action]") return 4019;
    if (tag == "[level of difficulty]") return 4062;
    if (tag == "[hero]") return 4128;
    if (tag == "[fire attack]") return 4132;
    if (tag == "[water attack]") return 4136;
    if (tag == "[dark attack]") return 4140;
    if (tag == "[light attack]") return 4144;
    if (tag == "[rigidity]") return 4149;
    if (tag == "[pattern]") return 4192;
    if (tag == "[fix direction]") return 4205;
    if (tag == "[left]") return 4219;
    if (tag == "[right]") return 4235;
    if (tag == "[/ridableskill definition]") return 4254;
    if (tag == "normal") return 4282;
    if (tag == "defaultcancel") return 4297;
    if (tag == "movable") return 4302;
    if (tag == "charge") return 4307;
    if (tag == "[limit slowdown atkspeed]") return 4422;
    if (tag == "[non targeted]") return 4500;
    if (tag == "[damage time balance]") return 4512;
    if (tag == "[stun time balance]") return 4519;
    if (tag == "[add gravity in aerial]") return 4526;
    if (tag == "[summon skill status up type]") return 4529;
    if (tag == "[attack down prob]") return 4550;
    if (tag == "[strong]") return 4554;
    if (tag == "[weak]") return 4559;
    if (tag == "[explain]") return 4564;
    if (tag == "[consume mp]") return 4582;
    if (tag == "[catch item]") return 4607;
    if (tag == "[fixed height]") return 4627;
    if (tag == "[etc sound]") return 4640;
    if (tag == "[/can beat index]") return 4665;
    if (tag == "[mounting action]") return 4671;
    if (tag == "[mounted action]") return 4677;
    if (tag == "[unmounting action]") return 4683;
    if (tag == "[index]") return 4696;
    if (tag == "[x pos]") return 4712;
    if (tag == "[y pos]") return 4720;
    if (tag == "[height]") return 4736;
    if (tag == "[velocity x]") return 4744;
    if (tag == "[velocity y]") return 4752;
    if (tag == "convert y to z") return 4760;
    if (tag == "[transparency]") return 4771;
    if (tag == "[master]") return 4784;
    if (tag == "[others]") return 4800;
    if (tag == "inherit") return 4807;
    if (tag == "[/control passive object]") return 4855;
    if (tag == "[life max]") return 4859;
    return -1;
}
}  // namespace

int ImportMonsterScript(STMonsterScript* script, int type, const char* path,
                        bool flag)
{
    bool ok_flag = false;
    if (!loadRDARScriptFile(g_szMonsterScriptBaseDirectory, path))
        return 0;
    script->Clear();

    getDirectoryPath(script->m_field708, "%s%s",
                     g_szMonsterScriptBaseDirectory, path);
    script->m_aiInfo.m_dir = script->m_field708;
    script->m_throwAttacks.clear();
    script->m_counterAttacks.clear();
    script->m_jumpAttacks.clear();
    script->m_equipment.m_items.clear();
    script->m_attackReadies.clear();

    std::string tag;
    std::string token;
    std::string local_str;
    bool ok = false;
    int moduleId = -1;
    int subId = -1;
    while (processModuleTagSet(moduleId, subId, &tag, type, flag))
    {
        if (subId != -1)
            script->m_field1c9c = subId;

        while (scanTag(tag))
        {
            switch (MonsterTagLookup(tag))
            {
case 0xa14:  // "[name]"
        ScanStr(&script->m_name);  // +0x4
    break;

case 0xa1a:  // "[name]"
        script->m_width = ScanInt(&ok);  // +0xc
        script->m_height = ScanInt(&ok);  // +0x10
    break;

case 0xa1e:  // "[width]"
        script->m_floatingHeight = ScanInt(&ok);  // +0x14
    break;

case 0xa21:  // "[floating height]"
        ScanStr(&local_str);  // 局部 string @8a13905
    break;

case 0xa3c:  // "[ambient sound]"
        ScanStr(&local_str);  // 局部 string @8a13b4d
    break;

case 0xa41:  // "[ambient sound]"
        ScanStr(&local_str);  // 局部 string @8a13be9
    break;

case 0xa46:  // "[attack sound]"
        ScanStr(&local_str);  // 局部 string @8a13c85
    break;

case 0xa4b:  // "[appear sound]"
        ScanStr(&local_str);  // 局部 string @8a13d21
    break;

case 0xa50:  // "[damage sound]"
        ScanStr(&local_str);  // 局部 string @8a13dbd
    break;

case 0xa55:  // "[die sound]"
        ScanStr(&local_str);  // 局部 string @8a13ebb
        ScanInt(&ok);  // 局部用途 @8a13f11
        ScanStr(&local_str);  // 局部 string @8a13f58
    break;

case 0xa70:  // "+"
        script->m_abilityTable = ScanInt(&ok);  // +0x3bc
    break;

case 0xa73:  // "[ability table]"
        ScanFloat(&ok);  // @8a14057
    break;

case 0xa77:  // "[summon skill status up rate]"
        *(int*)((char*)script+0x3ec) = ScanInt(&ok);  // +0x3ec
        *(int*)((char*)script+0x3f0) = ScanInt(&ok);  // +0x3f0
    break;

case 0xa7b:  // "[level]"
        *(int*)((char*)script+0x3f4) = ScanInt(&ok);  // +0x3f4
        *(int*)((char*)script+0x3f8) = ScanInt(&ok);  // +0x3f8
    break;

case 0xa7f:  // "[fire resistance]"
        *(int*)((char*)script+0x3fc) = ScanInt(&ok);  // +0x3fc
        *(int*)((char*)script+0x400) = ScanInt(&ok);  // +0x400
    break;

case 0xa83:  // "[water resistance]"
        *(int*)((char*)script+0x404) = ScanInt(&ok);  // +0x404
        *(int*)((char*)script+0x408) = ScanInt(&ok);  // +0x408
    break;

case 0xa87:  // "[dark resistance]"
        *(int*)((char*)script+0x40c) = ScanInt(&ok);  // +0x40c
        *(int*)((char*)script+0x410) = ScanInt(&ok);  // +0x410
    break;

case 0xa8b:  // "[light resistance]"
        *(int*)((char*)script+0x49c) = ScanInt(&ok);  // +0x49c
        *(int*)((char*)script+0x4a0) = ScanInt(&ok);  // +0x4a0
    break;

case 0xa8f:  // "[HP regen speed]"
        *(int*)((char*)script+0x4a4) = ScanInt(&ok);  // +0x4a4
        *(int*)((char*)script+0x4a8) = ScanInt(&ok);  // +0x4a8
    break;

case 0xa93:  // "[MP regen speed]"
        *(int*)((char*)script+0x4ac) = ScanInt(&ok);  // +0x4ac
        *(int*)((char*)script+0x4b0) = ScanInt(&ok);  // +0x4b0
    break;

case 0xa97:  // "[move speed]"
        *(int*)((char*)script+0x4b4) = ScanInt(&ok);  // +0x4b4
        *(int*)((char*)script+0x4b8) = ScanInt(&ok);  // +0x4b8
    break;

case 0xaa3:  // "[attack speed]"
        *(int*)((char*)script+0x4bc) = ScanInt(&ok);  // +0x4bc
        *(int*)((char*)script+0x4c0) = ScanInt(&ok);  // +0x4c0
    break;

case 0xaa7:  // "[cast speed]"
        *(int*)((char*)script+0x4c4) = ScanInt(&ok);  // +0x4c4
        *(int*)((char*)script+0x4c8) = ScanInt(&ok);  // +0x4c8
    break;

case 0xaab:  // "[hit recovery]"
        *(int*)((char*)script+0x4cc) = ScanInt(&ok);  // +0x4cc
        *(int*)((char*)script+0x4d0) = ScanInt(&ok);  // +0x4d0
    break;

case 0xaaf:  // "[jump power]"
        *(int*)((char*)script+0x4d4) = ScanInt(&ok);  // +0x4d4
        *(int*)((char*)script+0x4d8) = ScanInt(&ok);  // +0x4d8
    break;

case 0xab4:  // "[jump speed]"
        *(int*)((char*)script+0x4dc) = ScanInt(&ok);  // +0x4dc
        *(int*)((char*)script+0x4e0) = ScanInt(&ok);  // +0x4e0
    break;

case 0xab9:  // "[normal dash speed]"
        *(int*)((char*)script+0x4e4) = ScanInt(&ok);  // +0x4e4
        *(int*)((char*)script+0x4e8) = ScanInt(&ok);  // +0x4e8
    break;

case 0xabf:  // "[slant dash speed]"
        *(int*)((char*)script+0x4ec) = ScanInt(&ok);  // +0x4ec
        *(int*)((char*)script+0x4f0) = ScanInt(&ok);  // +0x4f0
    break;

case 0xac3:  // "[weight]"
        script->m_superarmorOnAttack = ScanInt(&ok) != 0;  // +0x5a5
        script->m_field5a8 = ScanInt(&ok);  // +0x5a8
    break;

case 0xac7:  // "[superarmor on attack]"
        script->m_sight = ScanInt(&ok);  // +0x504
    break;

case 0xaca:  // "[sight]"
        ScanStr(&script->m_comeInAction);  // +0x530
        script->m_field538 = ScanInt(&ok);  // +0x538
        script->m_field53c = ScanInt(&ok);  // +0x53c
    break;

case 0xacf:  // "[come in action]"
        ScanStr(&script->m_comeInAction);  // +0x530
        ScanStr(&script->m_comeInActionEx);  // +0x534
        script->m_field538 = ScanInt(&ok);  // +0x538
        script->m_field53c = ScanInt(&ok);  // +0x53c
    break;

case 0xad6:  // "[come in action ex]"
        ScanInt(&ok);  // 局部用途 @8a14bf7
        ScanInt(&ok);  // 局部用途 @8a14c09
    break;

case 0xada:  // "[diving sight]"
        script->m_targetingTimeTerm = ScanInt(&ok);  // +0x574
    break;

case 0xadd:  // "[diving sight]"
        script->m_targetingOnAttack = ScanInt(&ok) != 0;  // +0x580
    break;

case 0xae0:  // "[targeting time term]"
        script->m_targetingOnDamage = ScanInt(&ok) != 0;  // +0x581
    break;

case 0xae3:  // "[targeting on damage]"
        ScanInt(&ok);  // 局部用途 @8a14dec
        script->m_targetingNearest = 0x2;  // +0x584 [literal]
    break;

case 0xae7:  // "[targeting high level]"
        ScanInt(&ok);  // 局部用途 @8a14e74
        script->m_targetingNearest = 0x1;  // +0x584 [literal]
    break;

case 0xaeb:  // "[targeting low level]"
        ScanInt(&ok);  // 局部用途 @8a14efc
        script->m_targetingAttacker = 0x2;  // +0x588 [literal]
    break;

case 0xaef:  // "[targeting high HP]"
        ScanInt(&ok);  // 局部用途 @8a14f84
        script->m_targetingAttacker = 0x1;  // +0x588 [literal]
    break;

case 0xaf3:  // "[targeting low HP]"
        script->m_targetingLowPhysAttack = ScanInt(&ok) != 0;  // +0x590
    break;

case 0xaf6:  // "[targeting nearest]"
        script->m_targetingLowPhysDefense = ScanInt(&ok) != 0;  // +0x591
    break;

case 0xaf9:  // "[targeting attacker]"
        script->m_targetingLowMagAttack = ScanInt(&ok) != 0;  // +0x592
    break;

case 0xafc:  // "[targeting from direction]"
        ScanInt(&ok);  // 局部用途 @8a15180
    break;

case 0xaff:  // "[targeting low physical attack]"
        ScanInt(&ok);  // 局部用途 @8a151ff
    break;

case 0xb02:  // "[targeting low physical defense]"
        ScanInt(&ok);  // 局部用途 @8a1527e
    break;

case 0xb05:  // "[targeting low magical attack]"
        ScanInt(&ok);  // 局部用途 @8a152fd
    break;

case 0xb08:  // "[targeting low magical defense]"
        script->m_field5a4 = ScanInt(&ok) != 0;  // +0x5a4
    break;

case 0xb0b:  // "[targeting downed]"
        script->m_warlike = ScanInt(&ok);  // +0x578
    break;

case 0xb12:  // "[targeting downed]"
        script->m_attackDelay = ScanInt(&ok);  // +0x57c
    break;

case 0xb15:  // "[warlike]"
        *(int*)((char*)script+0x4f4) = ScanInt(&ok);  // +0x4f4
        *(int*)((char*)script+0x4f8) = ScanInt(&ok);  // +0x4f8
        *(int*)((char*)script+0x4fc) = ScanInt(&ok);  // +0x4fc
        *(int*)((char*)script+0x500) = ScanInt(&ok);  // +0x500
    break;

case 0xb1b:  // "[stuckbonus on damage]"
        script->m_field028[0].clear();  // vector<pair<int,int>> @+0x28
        while (true) {
            int v1 = ScanInt(&ok);
            if (!ok) break;
            int v2 = ScanInt(&ok);
            script->m_field028[0].push_back(std::make_pair(v1, v2));
        }
    break;

case 0xb25:  // "[item]"
        script->m_field028[1].clear();  // vector<pair<int,int>> @+0x34
        while (true) {
            int v1 = ScanInt(&ok);
            if (!ok) break;
            int v2 = ScanInt(&ok);
            script->m_field028[1].push_back(std::make_pair(v1, v2));
        }
    break;

case 0xb31:  // "[common champion drop item]"
        script->m_flag3a8 = ScanInt(&ok) != 0;  // +0x3a8
    break;

case 0xb36:  // "[hell monster]"
        script->m_field028[3].clear();  // vector<pair<int,int>> @+0x4c
        while (true) {
            int v1 = ScanInt(&ok);
            if (!ok) break;
            int v2 = ScanInt(&ok);
            script->m_field028[3].push_back(std::make_pair(v1, v2));
        }
    break;

case 0xb40:  // "[boss drop item]"
        script->m_field08c.clear();  // vector<pair<int,int>> @+0x8c
        while (true) {
            int v1 = ScanInt(&ok);
            if (!ok) break;
            int v2 = ScanInt(&ok);
            script->m_field08c.push_back(std::make_pair(v1, v2));
        }
    break;

case 0xbab:  // "[death tower item]"
        script->m_field058.clear();  // vector<pair<int,int>> @+0x58
        while (true) {
            int v1 = ScanInt(&ok);
            if (!ok) break;
            int v2 = ScanInt(&ok);
            script->m_field058.push_back(std::make_pair(v1, v2));
        }
    break;

case 0xbb5:  // "[death tower item]"
        // ORIG：m_championElements.clear() → while(ScanStr) 匹配 0..4 元素名
        // getElementName(ENUM_ELEMENT) 声明于 CreatureScript.h（外部函数）
        script->m_championElements.clear();
        while (ScanStr(&token)) {
            static const char* const s_elemNames[5] = { "fire", "water", "dark", "light", "none" };  // 【推断】
            for (int i = 0; i <= 4; ++i) {
                if (token == s_elemNames[i]) {
                    script->m_championElements.push_back(i);
                    break;
                }
            }
        }
    break;

case 0xbc8:  // "[super champion drop item]"
        script->m_field028[2].clear();  // vector<pair<int,int>> @+0x40
        while (true) {
            int v1 = ScanInt(&ok);
            if (!ok) break;
            int v2 = ScanInt(&ok);
            script->m_field028[2].push_back(std::make_pair(v1, v2));
        }
    break;

case 0xbd3:  // "[super champion drop item]"
        script->m_keepDistance = ScanInt(&ok);  // +0x6e0
    break;

case 0xbd6:  // "[keep distance]"
        script->m_flag374 = ScanInt(&ok) != 0;  // +0x374
        script->m_field378 = ScanInt(&ok);  // +0x378
    break;

case 0xbdc:  // "[is on the object]"
        script->m_isOnTheObject = ScanInt(&ok) != 0;  // +0x6e4
    break;

case 0xbe0:  // "[out of targetting]"
        ScanFloat(&ok);  // @8a15e91
        ScanFloat(&ok);  // @8a15eb7
        ScanFloat(&ok);  // @8a15ec9
        ScanInt(&ok);  // 局部用途 @8a15edb
        ScanInt(&ok);  // 局部用途 @8a15ef5
        ScanInt(&ok);  // 局部用途 @8a15f07
        ScanInt(&ok);  // 局部用途 @8a15f19
        ScanInt(&ok);  // 局部用途 @8a15f2b
        ScanInt(&ok);  // 局部用途 @8a15f3d
        ScanInt(&ok);  // 局部用途 @8a15f4f
        ScanInt(&ok);  // 局部用途 @8a15f61
        ScanInt(&ok);  // 局部用途 @8a15f73
    break;

case 0xbfa:  // "[die effect]"
        *(int*)((char*)script+0x6fc) = ScanInt(&ok);  // +0x6fc
        *(int*)((char*)script+0x700) = ScanInt(&ok);  // +0x700
        *(int*)((char*)script+0x704) = ScanInt(&ok);  // +0x704
    break;

case 0xbff:  // "[die effect]"
        ScanStr(&script->m_faceImage);  // +0x5ac
        script->m_field5b0 = ScanInt(&ok);  // +0x5b0
    break;

case 0xc03:  // "[face image]"
        ScanStr(&local_str);  // 局部 string @8a1613d
    break;

case 0xc0f:  // "[move motion]"
        ScanStr(&local_str);  // 局部 string @8a162d4
    break;

case 0xc1b:  // "[sit motion]"
        ScanStr(&local_str);  // 局部 string @8a1646b
    break;

case 0xc27:  // "[damage motion 1]"
        ScanStr(&local_str);  // 局部 string @8a16602
    break;

case 0xc33:  // "[damage motion 2]"
        ScanStr(&local_str);  // 局部 string @8a16799
    break;

case 0xc3f:  // "[down motion]"
        ScanStr(&local_str);  // 局部 string @8a16930
    break;

case 0xc4b:  // "[overturn motion]"
        ScanStr(&local_str);  // 局部 string @8a16ac7
    break;

case 0xc57:  // "[jump motion]"
        ScanStr(&local_str);  // 局部 string @8a16c5e
    break;

case 0xc63:  // "[jumpattack motion]"
        ScanStr(&local_str);  // 局部 string @8a16df5
    break;

case 0xca3:  // "[waiting action]"
        ScanStr(&script->m_field5fc0);  // +0x5fc
    break;

case 0xca6:  // "[waiting action]"
        ScanStr(&script->m_field5fc1);  // +0x600
    break;

case 0xca9:  // "[waiting action]"
        ScanStr(&script->m_field5fc5);  // +0x610
    break;

case 0xcac:  // "[move action]"
        ScanStr((std::string*)((char*)script+0x614));  // +0x614
    break;

case 0xcaf:  // "[sit action]"
        ScanStr((std::string*)((char*)script+0x618));  // +0x618
    break;

case 0xcb2:  // "[damage action 1]"
        ScanStr(&script->m_field61c0);  // +0x61c
    break;

case 0xcb5:  // "[damage action 2]"
        ScanStr(&script->m_field61c1);  // +0x620
    break;

case 0xcb8:  // "[down action]"
        ScanStr(&script->m_field61c2);  // +0x624
    break;

case 0xcbb:  // "[overturn action]"
        ScanStr(&script->m_field61c3);  // +0x628
    break;

case 0xcbe:  // "[jump action]"
        ScanStr(&script->m_moveMotion);  // +0x698
    break;

case 0xcc1:  // "[jumpattack action]"
        ScanStr(&local_str);  // 局部 string @8a17873
    break;

case 0xcc2:  // "[basic action]"
        ScanStr(&local_str);  // 局部 string @8a17873
    break;

case 0xcc8:  // "[action list]"
        ScanStr(&local_str);  // 局部 string @8a17914
    break;

case 0xccd:  // "[etc action]"
        ScanStr(&script->m_field6440);  // +0x644
    break;

case 0xcd0:  // "[last action]"
        ScanStr(&script->m_jumpAttackMotion);  // +0x6b4
    break;

case 0xcd3:  // "[last action]"
        ScanStr(&local_str);  // 局部 string @8a17a99
    break;

case 0xcd9:  // "[attack info]"
        ScanStr(&local_str);  // 局部 string @8a17b3a
    break;

case 0xcdf:  // "[etc attack info]"
        ScanInt(&ok);  // 局部用途 @8a17bbe
    break;

case 0xd14:  // "[on go to help]"
        ScanStr(&local_str);  // 局部 string @8a183fb
    break;

case 0xd1d:  // "[fix move direction]"
        ScanInt(&ok);  // 局部用途 @8a1849c
    break;

case 0xd2a:  // "[passive object filename]"
        ScanStr(&local_str);  // 局部 string @8a1887d
    break;

case 0xd31:  // "[passive object filename]"
        ScanInt(&ok);  // 局部用途 @8a1891d
    break;

case 0xd34:  // "[passive object number]"
        ScanInt(&ok);  // 局部用途 @8a18991
    break;

case 0xd37:  // "[passive object number]"
        ScanInt(&ok);  // 局部用途 @8a18a05
    break;

case 0xd3a:  // "[passive object index]"
        ScanInt(&ok);  // 局部用途 @8a18a79
    break;

case 0xd3d:  // "[power]"
        ScanInt(&ok);  // 局部用途 @8a18b39
    break;

case 0xd44:  // "[passive object start x cood]"
        ScanInt(&ok);  // 局部用途 @8a18c0f
    break;

case 0xd4b:  // "[passive object start y cood]"
        ScanInt(&ok);  // 局部用途 @8a18ce5
    break;

case 0xd52:  // "[passive object start z cood]"
        ScanInt(&ok);  // 局部用途 @8a18dbb
    break;

case 0xd59:  // "[direction]"
        ScanInt(&ok);  // 局部用途 @8a18e45
    break;

case 0xd5c:  // "[attack number]"
        ScanInt(&ok);  // 局部用途 @8a18eb9
    break;

case 0xd5f:  // "[attack number]"
        ScanStr(&local_str);  // 局部 string @8a18f32
    break;

case 0xd62:  // "[attack index]"
        ScanInt(&ok);  // 局部用途 @8a18fa0
    break;

case 0xd65:  // "[throw sound]"
        ScanInt(&ok);  // 局部用途 @8a19011
        ScanStr(&local_str);  // 局部 string @8a1904c
    break;

case 0xd6e:  // "[equipment]"
        ScanInt(&ok);  // 局部用途 @8a19151
        ScanStr(&local_str);  // 局部 string @8a19171
        ScanStr(&local_str);  // 局部 string @8a191c2
    break;

case 0xd7b:  // "[ready action]"
        ScanInt(&ok);  // 局部用途 @8a192c1
        ScanInt(&ok);  // 局部用途 @8a192f6
        ScanStr(&local_str);  // 局部 string @8a19349
        ScanInt(&ok);  // 局部用途 @8a19370
        ScanStr(&local_str);  // 局部 string @8a193b6
    break;

case 0xda1:  // "[probability]"
        ScanInt(&ok);  // 局部用途 @8a1977d
    break;

case 0xda4:  // "[probability]"
        ScanInt(&ok);  // 局部用途 @8a197f1
    break;

case 0xda7:  // "[probability]"
        ScanStr(&local_str);  // 局部 string @8a19878
    break;

case 0xdb7:  // "[approach attack]"
        ScanInt(&ok);  // 局部用途 @8a1997e
        ScanStr(&local_str);  // 局部 string @8a199b9
    break;

case 0xdc6:  // "[attack index]"
        ScanInt(&ok);  // 局部用途 @8a19d75
    break;

case 0xdc9:  // "[attack index]"
        ScanInt(&ok);  // 局部用途 @8a19de9
    break;

case 0xdcc:  // "[attack index]"
        ScanInt(&ok);  // 局部用途 @8a19e5d
    break;

case 0xdcf:  // "[start frame index]"
        ScanInt(&ok);  // 局部用途 @8a19ed1
    break;

case 0xdd2:  // "[end frame index]"
        ScanInt(&ok);  // 局部用途 @8a19f45
    break;

case 0xdd5:  // "[jump height]"
        ScanInt(&ok);  // 局部用途 @8a19fb9
    break;

case 0xdd8:  // "[jump speed]"
        ScanFloat(&ok);  // @8a1a02d
    break;

case 0xddb:  // "[on collision custom animation index]"
        ScanInt(&ok);  // 局部用途 @8a1a0a1
    break;

case 0xdde:  // "[ememy distance rate]"
        ScanInt(&ok);  // 局部用途 @8a1a115
    break;

case 0xde1:  // "[limited x cood]"
        ScanInt(&ok);  // 局部用途 @8a1a189
    break;

case 0xde4:  // "[limited y cood]"
        ScanInt(&ok);  // 局部用途 @8a1a1fa
        ScanStr(&local_str);  // 局部 string @8a1a235
    break;

case 0xdf4:  // "[function index]"
        ScanInt(&ok);  // 局部用途 @8a1a5cf
    break;

case 0xdf7:  // "[function index]"
        ScanInt(&ok);  // 局部用途 @8a1a643
    break;

case 0xdfa:  // "[function index]"
        ScanInt(&ok);  // 局部用途 @8a1a6b7
    break;

case 0xdfd:  // "[attack index]"
        ScanInt(&ok);  // 局部用途 @8a1a72e
        ScanInt(&ok);  // 局部用途 @8a1a767
        ScanInt(&ok);  // 局部用途 @8a1a779
        ScanInt(&ok);  // 局部用途 @8a1a78b
        ScanInt(&ok);  // 局部用途 @8a1a79d
    break;

case 0xe09:  // "[summon term]"
        ScanInt(&ok);  // 局部用途 @8a1a846
    break;

case 0xe0c:  // "[summon term]"
        ScanInt(&ok);  // 局部用途 @8a1a8ba
    break;

case 0xe0f:  // "[summon term]"
        ScanInt(&ok);  // 局部用途 @8a1a92e
    break;

case 0xe12:  // "[probability]"
        ScanInt(&ok);  // 局部用途 @8a1a9a2
    break;

case 0xe15:  // "[enterance effect]"
        ScanInt(&ok);  // 局部用途 @8a1aa13
        ScanStr(&local_str);  // 局部 string @8a1aa4e
    break;

case 0xe24:  // "[function index]"
        ScanInt(&ok);  // 局部用途 @8a1adfa
    break;

case 0xe27:  // "[function index]"
        ScanInt(&ok);  // 局部用途 @8a1ae6e
    break;

case 0xe2a:  // "[function index]"
        ScanInt(&ok);  // 局部用途 @8a1aee2
        ScanInt(&ok);  // 局部用途 @8a1aef4
        ScanInt(&ok);  // 局部用途 @8a1af06
    break;

case 0xe2f:  // "[shakeable]"
        ScanInt(&ok);  // 局部用途 @8a1af7a
    break;

case 0xe32:  // "[first speed]"
        ScanInt(&ok);  // 局部用途 @8a1afee
    break;

case 0xe35:  // "[first speed]"
        ScanInt(&ok);  // 局部用途 @8a1b062
    break;

case 0xe38:  // "[last speed]"
        ScanInt(&ok);  // 局部用途 @8a1b0d6
    break;

case 0xe3b:  // "[dash time]"
        ScanInt(&ok);  // 局部用途 @8a1b147
        ScanStr(&local_str);  // 局部 string @8a1b182
    break;

case 0xe4a:  // "[function index]"
        ScanInt(&ok);  // 局部用途 @8a1b4d5
        ScanInt(&ok);  // 局部用途 @8a1b4f6
        ScanInt(&ok);  // 局部用途 @8a1b508
    break;

case 0xe52:  // "[function index]"
        ScanInt(&ok);  // 局部用途 @8a1b57c
    break;

case 0xe55:  // "[attack index]"
        ScanInt(&ok);  // 局部用途 @8a1b5f0
        ScanInt(&ok);  // 局部用途 @8a1b602
    break;

case 0xe59:  // "[attack index]"
        ScanInt(&ok);  // 局部用途 @8a1b676
    break;

case 0xe5c:  // "[offset]"
        ScanInt(&ok);  // 局部用途 @8a1b6ea
    break;

case 0xe5f:  // "[animation index]"
        ScanInt(&ok);  // 局部用途 @8a1b75b
        ScanStr(&local_str);  // 局部 string @8a1b799
    break;

case 0xe6e:  // "[function index]"
        ScanInt(&ok);  // 局部用途 @8a1bb25
    break;

case 0xe71:  // "[function index]"
        ScanInt(&ok);  // 局部用途 @8a1bb99
    break;

case 0xe74:  // "[function index]"
        ScanInt(&ok);  // 局部用途 @8a1bc0d
    break;

case 0xe77:  // "[attack index]"
        ScanInt(&ok);  // 局部用途 @8a1bc81
    break;

case 0xe7a:  // "[warp style]"
        ScanInt(&ok);  // 局部用途 @8a1bcf5
    break;

case 0xe7d:  // "[after attack]"
        ScanInt(&ok);  // 局部用途 @8a1bd69
    break;

case 0xe80:  // "[after appear time]"
        ScanInt(&ok);  // 局部用途 @8a1bddd
    break;

case 0xe83:  // "[cool time]"
        ScanInt(&ok);  // 局部用途 @8a1be51
    break;

case 0xe86:  // "[target offset]"
        ScanStr(&local_str);  // 局部 string @8a1bed8
        ScanStr(&local_str);  // 局部 string @8a1bfa2
    break;

case 0xea5:  // "[on time]"
        ScanInt(&ok);  // 局部用途 @8a1c32d
        ScanInt(&ok);  // 局部用途 @8a1c33f
        ScanInt(&ok);  // 局部用途 @8a1c351
    break;

case 0xeab:  // "[on time]"
        ScanInt(&ok);  // 局部用途 @8a1c3d4
        ScanInt(&ok);  // 局部用途 @8a1c3e6
        ScanInt(&ok);  // 局部用途 @8a1c3f8
    break;

case 0xeb1:  // "[on hp]"
        ScanInt(&ok);  // 局部用途 @8a1c47b
        ScanInt(&ok);  // 局部用途 @8a1c48d
    break;

case 0xeb7:  // "[always]"
        ScanInt(&ok);  // 局部用途 @8a1c510
        ScanInt(&ok);  // 局部用途 @8a1c522
        ScanInt(&ok);  // 局部用途 @8a1c534
    break;

case 0xebd:  // "[counter]"
        ScanInt(&ok);  // 局部用途 @8a1c5b7
        ScanInt(&ok);  // 局部用途 @8a1c5c9
        ScanInt(&ok);  // 局部用途 @8a1c5db
        ScanInt(&ok);  // 局部用途 @8a1c5ed
    break;

case 0xec4:  // "[breakable]"
        ScanInt(&ok);  // 局部用途 @8a1c66b
        ScanInt(&ok);  // 局部用途 @8a1c67d
        ScanInt(&ok);  // 局部用途 @8a1c68f
        ScanInt(&ok);  // 局部用途 @8a1c6a1
    break;

case 0xecb:  // "[half]"
        ScanInt(&ok);  // 局部用途 @8a1c71f
        ScanInt(&ok);  // 局部用途 @8a1c731
        ScanInt(&ok);  // 局部用途 @8a1c743
        ScanInt(&ok);  // 局部用途 @8a1c755
    break;

case 0xed2:  // "[unbreakable]"
        ScanStr(&local_str);  // 局部 string @8a1c7f6
    break;

case 0xeda:  // "[with blast]"
        ScanInt(&ok);  // 局部用途 @8a1c89a
        ScanStr(&local_str);  // 局部 string @8a1c8c2
        ScanInt(&ok);  // 局部用途 @8a1c93e
        ScanInt(&ok);  // 局部用途 @8a1c950
        ScanInt(&ok);  // 局部用途 @8a1c962
        ScanInt(&ok);  // 局部用途 @8a1c974
    break;

case 0xefb:  // "[breakable]"
        ScanInt(&ok);  // 局部用途 @8a1cab2
    break;

case 0xf23:  // "[/cooltime]"
        script->m_fieldc4c = ScanInt(&ok);  // +0xc4c
    break;

case 0xf27:  // "[spawn prob]"
        script->m_field36c = ScanInt(&ok);  // +0x36c
        script->m_field370 = ScanInt(&ok);  // +0x370
    break;

case 0xf2b:  // "[drop item offset]"
        script->m_flag368 = ScanInt(&ok) != 0;  // +0x368
    break;

case 0xf46:  // "[keep range distance with target 4]"
        ScanInt(&ok);  // 局部用途 @8a1d391
        ScanInt(&ok);  // 局部用途 @8a1d3b7
    break;

case 0xf57:  // "[reflection object]"
        ScanInt(&ok);  // 局部用途 @8a1d45e
    break;

case 0xf5c:  // "[set damage check]"
        *(int*)((char*)script+0x33c) = ScanInt(&ok) != 0;  // +0x33c
    break;

case 0xf62:  // "[change direction by target]"
        ScanInt(&ok);  // 局部用途 @8a1d56a
    break;

case 0xf67:  // "[is direction to move]"
        ScanFloat(&ok);  // @8a1d5ed
    break;

case 0xf6c:  // "[is direction to move]"
        ScanFloat(&ok);  // @8a1d664
        ScanFloat(&ok);  // @8a1d679
    break;

case 0xf72:  // "[hp regen rate]"
        ScanFloat(&ok);  // @8a1d6f0
    break;

case 0xf76:  // "[absolute defense ignore rate]"
        ScanInt(&ok);  // 局部用途 @8a1d779
    break;

case 0xf7b:  // "[lie frame]"
        script->m_field1cac = ScanInt(&ok);  // +0x1cac
    break;

case 0xf84:  // "[forced knockback]"
        ScanStr(&script->m_field6441);  // +0x648
    break;

case 0xf87:  // "[No Combo Rate Monster]"
        ScanStr(&script->m_waitingMotion);  // +0x694
    break;

case 0xf8a:  // "[proc action]"
        script->m_field6f4 = ScanInt(&ok);  // +0x6f4
    break;

case 0xf8d:  // "[monster title]"
        script->m_field6f8 = ScanInt(&ok);  // +0x6f8
    break;

case 0xf99:  // "[destroy count]"
        script->m_fieldc74 = ScanInt(&ok);  // +0xc74
    break;

case 0xf9d:  // "[destroy count damage]"
        script->m_fieldc78 = ScanInt(&ok);  // +0xc78
        script->m_fieldc7c = ScanInt(&ok);  // +0xc7c
    break;

case 0xfa2:  // "[overhead gauge pos]"
        ScanInt(&ok);  // 局部用途 @8a1dbc1
    break;

case 0xfa7:  // "[flying mark]"
        ScanStr(&script->m_field5fc2);  // +0x604
    break;

case 0xfaa:  // "[moveback action]"
        ScanStr(&script->m_field5fc3);  // +0x608
    break;

case 0xfad:  // "[moveback action]"
        ScanStr(&script->m_field5fc4);  // +0x60c
    break;

case 0xfb0:  // "[dash action]"
        ScanStr(&script->m_field098);  // +0x98
    break;

case 0xfb3:  // "[dashattack action]"
        ScanInt(&ok);  // 局部用途 @8a1dea4
        ScanInt(&ok);  // 局部用途 @8a1dee3
    break;

case 0xfde:  // "[level of difficulty]"
        ScanInt(&ok);  // 局部用途 @8a1e062
        ScanInt(&ok);  // 局部用途 @8a1e074
        ScanInt(&ok);  // 局部用途 @8a1e0a5
        ScanInt(&ok);  // 局部用途 @8a1e0b7
        ScanInt(&ok);  // 局部用途 @8a1e0e8
        ScanInt(&ok);  // 局部用途 @8a1e0fa
        ScanInt(&ok);  // 局部用途 @8a1e128
        ScanInt(&ok);  // 局部用途 @8a1e13a
        ScanInt(&ok);  // 局部用途 @8a1e168
        ScanInt(&ok);  // 局部用途 @8a1e17a
    break;

case 0x1020:  // "[hero]"
        *(int*)((char*)script+0x508) = ScanInt(&ok);  // +0x508
        *(int*)((char*)script+0x50c) = ScanInt(&ok);  // +0x50c
    break;

case 0x1024:  // "[fire attack]"
        *(int*)((char*)script+0x510) = ScanInt(&ok);  // +0x510
        *(int*)((char*)script+0x514) = ScanInt(&ok);  // +0x514
    break;

case 0x1028:  // "[water attack]"
        *(int*)((char*)script+0x518) = ScanInt(&ok);  // +0x518
        *(int*)((char*)script+0x51c) = ScanInt(&ok);  // +0x51c
    break;

case 0x102c:  // "[dark attack]"
        *(int*)((char*)script+0x520) = ScanInt(&ok);  // +0x520
        *(int*)((char*)script+0x524) = ScanInt(&ok);  // +0x524
    break;

case 0x1030:  // "[light attack]"
        *(int*)((char*)script+0x528) = ScanInt(&ok);  // +0x528
        *(int*)((char*)script+0x52c) = ScanInt(&ok);  // +0x52c
    break;

case 0x1035:  // "[rigidity]"
        ScanFloat(&ok);  // @8a1e50e
        ScanFloat(&ok);  // @8a1e562
        ScanInt(&ok);  // 局部用途 @8a1e5a4
        ScanInt(&ok);  // 局部用途 @8a1e5d5
        ScanInt(&ok);  // 局部用途 @8a1e606
        ScanInt(&ok);  // 局部用途 @8a1e62e
        ScanInt(&ok);  // 局部用途 @8a1e65a
    break;

case 0x1060:  // "[pattern]"
        ScanInt(&ok);  // 局部用途 @8a1e73d
    break;

case 0x107b:  // "[left]"
        script->m_fieldcd8 = 0x0;  // +0xcd8 [literal]
    break;

case 0x107e:  // "[left]"
        script->m_fieldcd8 = 0x1;  // +0xcd8 [literal]
    break;

case 0x10a4:  // "[/ridableskill definition]"
        ScanInt(&ok);  // 局部用途 @8a1efb3
    break;

case 0x10a7:  // "[/ridableskill definition]"
        ScanStr(&local_str);  // 局部 string @8a1f02c
    break;

case 0x10d3:  // "charge"
        ScanInt(&ok);  // 局部用途 @8a1f662
        ScanInt(&ok);  // 局部用途 @8a1f67a
        ScanInt(&ok);  // 局部用途 @8a1f6cf
        ScanInt(&ok);  // 局部用途 @8a1f6e8
    break;

case 0x1146:  // "[limit slowdown atkspeed]"
        script->m_field1c94 = ScanInt(&ok);  // +0x1c94
    break;

case 0x1149:  // "[limit slowdown atkspeed]"
        script->m_field1c98 = ScanInt(&ok);  // +0x1c98
    break;

case 0x114c:  // "[limit slowdown atkspeed]"
        ScanStr(&local_str);  // 局部 string @8a1f9b1
        ScanStr(&local_str);  // 局部 string @8a1f9de
        ScanStr(&local_str);  // 局部 string @8a1fa0e
        ScanStr(&local_str);  // 局部 string @8a1fa3e
        ScanInt(&ok);  // 局部用途 @8a1fa6d
    break;

case 0x1177:  // "[pattern]"
        ScanInt(&ok);  // 局部用途 @8a1fb7b
        ScanFloat(&ok);  // @8a1fbb2
    break;

case 0x1194:  // "[non targeted]"
        script->m_field56c = ScanInt(&ok) != 0;  // +0x56c
    break;

case 0x1197:  // "[non targeted]"
        ScanStr(&script->m_field570);  // +0x570
    break;

case 0x119a:  // "[non targeted]"
        ScanInt(&ok);  // 局部用途 @8a1fd99
    break;

case 0x11a0:  // "[damage time balance]"
        ScanInt(&ok);  // 局部用途 @8a1fe42
    break;

case 0x11a7:  // "[stun time balance]"
        script->m_field548 = ScanInt(&ok) != 0;  // +0x548
        ScanFloat(&ok);  // @8a1fefa
        ScanFloat(&ok);  // @8a1ff0f
    break;

case 0x11ae:  // "[add gravity in aerial]"
        script->m_field3c0 = ScanInt(&ok);  // +0x3c0
    break;

case 0x11b1:  // "[summon skill status up type]"
        ScanFloat(&ok);  // @8a2000e
    break;

case 0x11c6:  // "[attack down prob]"
        script->m_field1ca0 = ScanInt(&ok);  // +0x1ca0
        ScanFloat(&ok);  // @8a200c7
    break;

case 0x11ca:  // "[strong]"
        ScanInt(&ok);  // 局部用途 @8a2013e
        ScanFloat(&ok);  // @8a20157
    break;

case 0x11cf:  // "[weak]"
        ScanStr(&script->m_field6d0);  // +0x6d0
    break;

case 0x11d4:  // "[explain]"
        ScanInt(&ok);  // 局部用途 @8a20242
    break;

case 0x11e6:  // "[consume mp]"
        ScanStr(&local_str);  // 局部 string @8a202fd
    break;

case 0x11ff:  // "[catch item]"
        script->m_field064 = ScanInt(&ok);  // +0x64
        script->m_field068 = ScanInt(&ok);  // +0x68
        ScanFloat(&ok);  // @8a20453
    break;

case 0x1213:  // "[fixed height]"
        script->m_field1cb0 = ScanInt(&ok);  // +0x1cb0
    break;

case 0x1218:  // "[fixed height]"
        ScanStr(&local_str);  // 局部 string @8a205b5
    break;

case 0x1239:  // "[/can beat index]"
        ScanStr(&script->m_field6442);  // +0x64c [间接赋值]
    break;

case 0x123f:  // "[mounting action]"
        ScanStr(&script->m_field6443);  // +0x650 [间接赋值]
    break;

case 0x1245:  // "[mounted action]"
        ScanStr(&script->m_field6444);  // +0x654 [间接赋值]
    break;

case 0x1258:  // "[index]"
        ScanInt(&ok);  // 局部用途 @8a20ca4
    break;

case 0x1260:  // "[index]"
        ScanInt(&ok);  // 局部用途 @8a20d46
    break;

case 0x1268:  // "[x pos]"
        ScanInt(&ok);  // 局部用途 @8a20de8
    break;

case 0x1270:  // "[y pos]"
        ScanInt(&ok);  // 局部用途 @8a20e8a
    break;

case 0x1278:  // "[width]"
        ScanInt(&ok);  // 局部用途 @8a20f2c
    break;

case 0x1280:  // "[height]"
        ScanFloat(&ok);  // @8a20fce
    break;

case 0x1288:  // "[velocity x]"
        ScanFloat(&ok);  // @8a2106a
    break;

case 0x12e0:  // "[left]"
        script->m_field684 = 0x0;  // +0x684 [literal]
    break;

case 0x12e4:  // "[left]"
        script->m_field684 = 0x1;  // +0x684 [literal]
    break;

case 0x12f7:  // "[/control passive object]"
        ScanInt(&ok);  // 局部用途 @8a22132
    break;

case 0x12fb:  // "[life max]"
        ScanInt(&ok);  // 局部用途 @8a221ad
        ScanInt(&ok);  // 局部用途 @8a22222
        ScanInt(&ok);  // 局部用途 @8a2223e
    break;
            default:
                break;
            }
            if (!ok_flag && !ScanType(tag, false))
                break;
        }
    }
    return 1;
}

bool exportMonsterScript(
    STMonsterScript* script, const char* path,
    std::map<std::string, std::vector<std::string> >& extra)
{
    static char saveFileName[0x100];                      // getScriptFileFullPath 缓冲
    getScriptFileFullPath(saveFileName, 0x100,
                          g_szMonsterScriptBaseDirectory, path);   // +0x2c
    FILE* file = fopen(saveFileName, "w");         // +0x3a "w"
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
