// ============================================================================
// W8LT_PassiveObjectScript.cpp — PassiveObjGenerator* / RandomItemFairy /
// STPassiveObjectScript 构造与 clear 实现
//
// 来源（df_game_r，elf32-i386）：
//   PassiveObjGeneratorObject::PassiveObjGeneratorObject()   T 0x08a3ae56
//     （docs/class_func_reports/PassiveObjGeneratorObject/PassiveObjGeneratorObject.md）
//   PassiveObjGeneratorPattern::PassiveObjGeneratorPattern() T 0x08a3ad70
//     （docs/class_func_reports/PassiveObjGeneratorPattern/PassiveObjGeneratorPattern.md）
//   PassiveObjGeneratorSequence::PassiveObjGeneratorSequence() T 0x08a3ae0a
//     （docs/class_func_reports/PassiveObjGeneratorSequence/PassiveObjGeneratorSequence.md）
//   RandomItemFairy::RandomItemFairy/clear                W 0x08a3db5e/0x08a3db72
//   STPassiveObjectScript::STPassiveObjectScript()        T 0x08a3a538
//     （docs/class_func_reports/STPassiveObjectScript/STPassiveObjectScript.md）
//   STPassiveObjectScript::clear()                        T 0x08a3a9e6
//     （docs/class_func_reports/STPassiveObjectScript/clear.md）
// 拷贝构造/operator=/析构为 ORIG 弱符号（W），由编译器隐式生成即与 ORIG
// 形态一致（声明序逐字段，无 pad 空洞），故本 TU 不定义。
// ============================================================================
#include "W8LT_PassiveObjectScript.h"
#include "STMapScript.h"      // ENUM_DRAWLAYER/MAP_FOLLOW_TYPE/PASSAGETYPE + 名字函数声明
#include "GameEnums.h"       // ENUM_CATEGORY
std::string getObjectCategoryString(ENUM_CATEGORY cat);   // T 0x08a3ca81（ScriptDepsA.cpp）
#include "STPassiveObjectAdditionalParticleInfo.h"
#include "RDARScriptStringManager.h"  // getTagByName（[name] 段）
#include "SyncScriptDeps.h"           // toTString
#include <stdio.h>

// ---- 跨 TU 依赖声明 ----
extern char g_szPassiveObjectScriptBaseDirectory[0x100];   // InitScriptProducers_4.cpp（B 0x094f6ac0）
void getScriptFileFullPath(char* out, unsigned int size,
                           const char* dir, const char* name);   // T 0x088bba4e（STMonsterScript.cpp）
void convertBinaryToText(const char* path);                       // T 0x088be04c（STMapScript.cpp）
extern RDARScriptStringManager g_scriptStringManager_;            // RDARScriptStringManager.cpp
// ---- WarningDistance（ORIG C1/C1Eiii T 0x08a3aeca/0x08a3aeec）----
// C1：三字段 movl $0 直写，函数体逐句赋值（每字段重读 this 0x8(%ebp)）；
// C1Eiii：参数 edx 中转 store，同函数体赋值形态。
WarningDistance::WarningDistance()
{
    m_a = 0;
    m_b = 0;
    m_c = 0;
}

WarningDistance::WarningDistance(int a, int b, int c)
{
    m_a = a;
    m_b = b;
    m_c = c;
}

// ORIG 0x08a3ae56：+0=0（int 直写）、+4=0.0f（float 经 edx 中转）、+8 string("")
PassiveObjGeneratorObject::PassiveObjGeneratorObject()
    : m_i00(0), m_f04(0.0f), m_s08("")
{
}

// ORIG 0x08a3ad70：初始化列表 m_b00=1 → m_s04 string → m_i08=0 → m_i0c=0 →
// m_v10 vector<uchar>，函数体 m_s04.clear() 再 m_v10.clear()（含 unwind 清理路径）。
PassiveObjGeneratorPattern::PassiveObjGeneratorPattern()
    : m_b00(1), m_s04(), m_i08(0), m_i0c(0), m_v10()
{
    m_s04.clear();
    m_v10.clear();
}

// ORIG 0x08a3ae0a：初始化列表 m_s00 string → m_i04=0，函数体 m_s00.clear()。
PassiveObjGeneratorSequence::PassiveObjGeneratorSequence()
    : m_s00(), m_i04(0)
{
    m_s00.clear();
}

// ---- SpeedCtrlPassiveObjInfo（ORIG T 0x08a3ad5a）----
// +0=-1（movl 直写）、+4 byte=0（movb）；两次重读 this → 函数体逐句赋值。
// D1/D2 为 ORIG W 0x08a4eab2 trivial，由编译器隐式生成，本 TU 不定义。
SpeedCtrlPassiveObjInfo::SpeedCtrlPassiveObjInfo()
{
    m_field0 = -1;
    m_b4 = 0;
}

// ---- PassiveIObjSniperInfo（ORIG C1/clear T 0x08a3af0c/0x08a3afc0）----
// C1：4 × vector<int> 构造 → clear()（带 unwind 清理路径）；
// clear：4 × vector::clear → 标量 0x30/0x34/0x38/0x40/0x3c 清零 →
// 循环 i=0..5 清 +0x44..+0x58 → 循环 i=0..2 清 +0x74..+0x7c →
// 0x5c/0x60/0x64/0x6c/0x70 清零（+0x68 不重置）。
PassiveIObjSniperInfo::PassiveIObjSniperInfo()
    : m_vecMinDist(), m_vecMaxDist(), m_vecField18(), m_vecField24()
{
    clear();
}

void PassiveIObjSniperInfo::clear()
{
    m_vecMinDist.clear();
    m_vecMaxDist.clear();
    m_vecField18.clear();
    m_vecField24.clear();
    m_i30 = 0;
    m_i34 = 0;
    m_b38 = 0;
    m_b40 = 0;
    m_i3c = 0;
    for (int i = 0; i <= 5; ++i)
    {
        m_ints44[i] = 0;
    }
    for (int i = 0; i <= 2; ++i)
    {
        m_ints74[i] = 0;
    }
    m_i5c = 0;
    m_i60 = 0;
    m_i64 = 0;
    m_i6c = 0;
    m_i70 = 0;
}

// ---- STPassiveObjectDestroy（ORIG C1 W 0x08a3d926 / D1 W 0x08a3e04c）----
// C1：5 × vector 构造（+0x08/+0x14/+0x20/+0x2c/+0x44）→ clear()（带 unwind 清理路径）；
// D1：空体，逆序析构 5 × vector（带 unwind 清理路径）。
STPassiveObjectDestroy::STPassiveObjectDestroy()
    : m_ints08(), m_strs14(), m_strs20(), m_ints2c(), m_ints44()
{
    clear();
}

STPassiveObjectDestroy::~STPassiveObjectDestroy()
{
}

// ORIG W 0x08a3d8c2：+0/-1、+4/0、+0x38/+0x3c/+0x40/-1，再 clear +8/+0x2c int vec 与 +0x14 string vec。
void STPassiveObjectDestroy::clear()
{
    m_type = -1;
    m_flag04 = 0;
    m_i38 = -1;
    m_i3c = -1;
    m_i40 = -1;
    m_ints08.clear();
    m_ints2c.clear();
    m_strs14.clear();
}


// ---- STPassiveObjectAddEffect（ORIG C1 W 0x08a3da0c / D1 W 0x08a3e110）----
// C1：2 × vector 构造（+0x00 string/+0x0c int）→ clear()（带 unwind 清理路径）；
// D1：空体，逆序析构 2 × vector（带 unwind 清理路径）。
STPassiveObjectAddEffect::STPassiveObjectAddEffect()
    : m_strs00(), m_ints0c()
{
    clear();
}

STPassiveObjectAddEffect::~STPassiveObjectAddEffect()
{
}

void STPassiveObjectAddEffect::clear()
{
}
// ---- RandomItemFairy（ORIG C1/C2 W 0x8a3db5e / clear W 0x8a3db72）----
// C1：仅调 clear()（无成员构造，纯标量）；clear：+0 movl $0 直写 →
// 循环 i=0..2（setle）清 +4+i*4。D1/D2 W 0x8a4f436 空体（头内联）。
RandomItemFairy::RandomItemFairy()
{
    clear();
}

void RandomItemFairy::clear()
{
    m_i00 = 0;
    for (int i = 0; i <= 2; ++i)
    {
        m_ints04[i] = 0;
    }
}

// ---- STPassiveObjectScript（ORIG C1 T 0x08a3a538 / clear T 0x08a3a9e6）----
// C1：按声明序构造全部非平凡成员（string/vector/子对象，call 序实证）→ clear()；
// 标量由 clear 统一赋值。D1/D2 W 0x08a3e25a 为编译器隐式生成（逆序析构成员），
// 本 TU 不定义（P12）。
STPassiveObjectScript::STPassiveObjectScript()
    : m_name(), m_layer(), m_category(), m_s34(), m_basicActions(), m_s44(),
      m_etcActions(), m_s54(), m_s64(), m_s68(), m_attackInfos(), m_intData(),
      m_stringData(), m_s90(), m_destroy(), m_addEffect(), m_particles(),
      m_s154(), m_onAttackData(), m_s17c(), m_speedCtrls(), m_genPatterns(),
      m_genSequences(), m_genObjects(), m_warnDanger(),
      m_warnRecovery(), m_sniper(), m_itemFairies(), m_s284()
{
    clear();
}

// clear：语句顺序严格照 ORIG 汇编（含 +0x61 先于 +0x60、+0x15c 先于 +0x158、
// +0x140 vector clear 先于 +0x9c、float 字段 edx 中转形态）。
void STPassiveObjectScript::clear()
{
    m_idx = 0;
    m_name = "";
    m_layer = "";
    m_width = 0;
    m_height = 0;
    m_i14 = 0;
    m_i18 = 1;
    m_i1c = 0;
    m_i20 = 0;
    m_i24 = 0;
    m_category.clear();
    m_s34 = "";
    m_basicActions.clear();
    m_s64 = "";
    m_s68 = "";
    m_attackInfos.clear();
    m_s44 = "";
    m_etcActions.clear();
    m_s54 = "";
    m_b58 = 0;
    m_b59 = 0;
    m_b5a = 0;
    m_i5c = -1;
    m_b61 = 0;
    m_b60 = 0;
    m_b62 = 0;
    m_intData.clear();
    m_stringData.clear();
    m_s90 = "";
    m_i94 = -1;
    m_i98 = 0;
    m_particles.clear();
    m_b9c = 0;
    m_ia0 = 1;
    m_ia4 = 0;
    m_fa8 = 0.0f;
    m_fac = 0.0f;
    m_ib0 = 500;
    m_bb4 = 0;
    m_fb8 = 0.0f;
    m_fbc = 0.0f;
    m_fc0 = 180.0f;
    m_ic4 = 0;
    m_ic8 = 0;
    m_icc = -1;
    m_id0 = 0;
    m_bd4 = 0;
    m_bd5 = 0;
    m_b14c = 0;
    m_b14d = 0;
    m_b14e = 0;
    m_ub14f = 0xff;
    m_us150 = 0xffff;
    m_s154 = "";
    m_i15c = 0;
    m_b158 = 0;
    m_b159 = 0;
    m_onAttackData.clear();
    m_b16c = 0;
    m_f170 = -1.0f;
    m_i174 = 0;
    m_i178 = 0;
    m_s17c = "";
    m_speedCtrls.clear();
    m_i270 = 2;
    m_b280 = 0;
}
// ===========================================================================
// exportPassiveObjectScript —— ORIG T 0x08a29f69
// （docs/class_func_reports/_lt_global_gt_/exportPassiveObjectScript.md；
// 偏移经 objdump 复核，与 W8LT_PassiveObjectScript.h 成员一一对应）
// 导出 STPassiveObjectScript 为中间文本（fopen "wb" + fprintf/fwrite），
// 收尾 convertBinaryToText 转最终脚本；fopen 失败返 0，成功返 1。
// ===========================================================================
int exportPassiveObjectScript(STPassiveObjectScript* script, const char* path)
{
    static char saveFileName[0x100];
    getScriptFileFullPath(saveFileName, 0x100,
                          g_szPassiveObjectScriptBaseDirectory, path);
    FILE* file = fopen(saveFileName, "wb");
    if (!file)
        return 0;

    if (script->m_name != "")
    {
        const wchar_t* layer = 0;
        if (!script->m_layer.empty())
            layer = reinterpret_cast<const wchar_t*>(script->m_layer.c_str());
        const wchar_t* name = reinterpret_cast<const wchar_t*>(script->m_name.c_str());
        fprintf(file, "[name] %s\n",
                toTString(g_scriptStringManager_.getTagByName(
                    0x9, name, layer, false)));
    }
    if (script->m_width != 0 || script->m_height != 0)
        fprintf(file, "[width] %d %d\n", script->m_width, script->m_height);
    if (script->m_i14 != 0)
        fprintf(file, "[floating height] %d\n", script->m_i14);
    if (script->m_i18 != 1)
        fprintf(file, "[layer] `%s`\n", getDrawLayerName((ENUM_DRAWLAYER)script->m_i18));
    if (script->m_i15c != 0)
        fprintf(file, "[layer level] %d\n", script->m_i15c);
    if (script->m_s17c != "")
        fprintf(file, "[burning death path] `%s`\n", script->m_s17c.c_str());
    if (script->m_b158 != 0)
        fwrite("[is hp by difficulty] 1\n", 1, 0x18, file);
    if (script->m_b159 != 0)
        fwrite("[hp gage] 1\n", 1, 0xc, file);
    if (getPassageTypeName((ENUM_PASSAGETYPE)script->m_i1c) != 0 && script->m_i1c != 0)
        fprintf(file, "[pass type] `%s`\n", getPassageTypeName((ENUM_PASSAGETYPE)script->m_i1c));
    if (script->m_i20 != 0)
        fprintf(file, "[piercing power] %d\n", script->m_i20);
    if (script->m_i24 != 0)
        fprintf(file, "[piercing tolerance] %d\n", script->m_i24);
    if (!script->m_category.empty())
    {
        fwrite("[category]\n", 1, 0xb, file);
        for (std::vector<int>::size_type i = 0; i < script->m_category.size(); ++i)
            fprintf(file, "`%s`\n", getObjectCategoryString((ENUM_CATEGORY)script->m_category[i]).c_str());
        fwrite("[/category]\n\n", 1, 0xd, file);
    }
    if (script->m_s34 != "")
        fprintf(file, "[basic motion] `%s`\n", script->m_s34.c_str());
    if (!script->m_basicActions.empty())
    {
        fwrite("[etc motion]\n", 1, 0xd, file);
        for (std::vector<std::string>::const_iterator it = script->m_basicActions.begin();
             it != script->m_basicActions.end(); ++it)
            fprintf(file, "`%s`\n", it->c_str());
        fwrite("[/etc motion]\n", 1, 0xe, file);
    }
    if (script->m_s44 != "")
        fprintf(file, "[basic action] `%s`\n", script->m_s44.c_str());
    if (script->m_s54 != "")
        fprintf(file, "[last action] `%s`\n", script->m_s54.c_str());
    if (!script->m_etcActions.empty())
    {
        fwrite("[etc action]\n", 1, 0xd, file);
        for (std::vector<std::string>::const_iterator it = script->m_etcActions.begin();
             it != script->m_etcActions.end(); ++it)
            fprintf(file, "`%s`\n", it->c_str());
        fwrite("[/etc action]\n", 1, 0xe, file);
    }
    if (script->m_b58 != 0)
        fwrite("[under gravity] 1\n", 1, 0x12, file);
    if (script->m_b61 != 0)
        fprintf(file, "[vanish on move collision] %d\n",
                (int)(script->m_b60 != 0));
    if (script->m_b5a != 0)
        fwrite("[trap] 1\n", 1, 9, file);
    if (script->m_b62 != 0)
        fwrite("[allow friend hit] 1\n", 1, 0x15, file);
    if (script->m_i5c != -1)
        fprintf(file, "[team] %d\n", script->m_i5c);
    if (script->m_s64 != "")
        fprintf(file, "[destroy particle] `%s`\n", script->m_s64.c_str());
    if (script->m_s68 != "")
        fprintf(file, "[attack info] `%s`\n", script->m_s68.c_str());
    if (!script->m_attackInfos.empty())
    {
        fwrite("[etc attack info]\n", 1, 0x12, file);
        for (std::vector<std::string>::const_iterator it = script->m_attackInfos.begin();
             it != script->m_attackInfos.end(); ++it)
            fprintf(file, "`%s`\n", it->c_str());
        fwrite("[/etc attack info]\n", 1, 0x13, file);
    }
    if (!script->m_intData.empty())
    {
        fwrite("[int data]\n", 1, 0xb, file);
        for (std::vector<int>::const_iterator it = script->m_intData.begin();
             it != script->m_intData.end(); ++it)
            fprintf(file, "%d\n", *it);
        fwrite("[/int data]\n", 1, 0xc, file);
    }
    if (!script->m_stringData.empty())
    {
        fwrite("[string data]\n", 1, 0xe, file);
        for (std::vector<std::string>::const_iterator it = script->m_stringData.begin();
             it != script->m_stringData.end(); ++it)
            fprintf(file, "`%s`\n", it->c_str());
        fwrite("[/string data]\n", 1, 0xf, file);
    }
    if (script->m_i94 == 1)
        fwrite("[quest type] `[must be destroyed]`\n", 1, 0x23, file);
    else if (script->m_i94 == 2)
        fwrite("[quest type] `[chaos object]`\n", 1, 0x1e, file);
    else if (script->m_i94 == 0)
        fwrite("[quest type] `[none]`\n", 1, 0x16, file);
    if (script->m_i98 > 0)
        fprintf(file, "[hp max] %d\n", script->m_i98);
    if (script->m_b59 != 0)
        fwrite("[hp destroy] 1\n", 1, 0xf, file);

    {
        bool needDestroy =
            (script->m_destroy.m_type != -1 || script->m_destroy.m_flag04 >= 1 ||
             script->m_destroy.m_i40 != -1 ||
             !script->m_destroy.m_ints08.empty() ||
             !script->m_destroy.m_strs14.empty() ||
             !script->m_destroy.m_ints44.empty());
        if (needDestroy)
        {
            fwrite("[object destroy condition]\n", 1, 0x1b, file);
            switch (script->m_destroy.m_type)
            {
            case 0:
                fwrite("`[destroy condition]` `[on end of animation]`\n", 1, 0x2e, file);
                break;
            case 1:
                fwrite("`[destroy condition]` `[z pos equal zero]`\n", 1, 0x2b, file);
                break;
            case 2:
                fprintf(file, "`[destroy condition]` `[time limite]` %d\n",
                        script->m_destroy.m_i3c);
                break;
            case 3:
                fwrite("`[destroy condition]` `[not destroy and z pos equal zero]`\n",
                       1, 0x3b, file);
                break;
            case 4:
                fwrite("`[destroy condition]` `[parent dead]`\n", 1, 0x26, file);
                break;
            }
            if (script->m_destroy.m_flag04 == 1)
                fwrite("`[destroy action]` `[object create after destroy]`\n",
                       1, 0x33, file);
            if (script->m_destroy.m_i40 == 0)
                fwrite("`[on attack]` `[only destroy]`\n", 1, 0x1f, file);
            else if (script->m_destroy.m_i40 == 1)
                fwrite("`[on attack]` `[object create after destroy]`\n",
                       1, 0x2e, file);
            if (!script->m_destroy.m_ints08.empty())
            {
                fprintf(file, "`[add object index]`\n%d\n",
                        (int)script->m_destroy.m_ints08.size());
                for (std::vector<int>::const_iterator it = script->m_destroy.m_ints08.begin();
                     it != script->m_destroy.m_ints08.end(); ++it)
                    fprintf(file, "%d\n", *it);
            }
            if (!script->m_destroy.m_strs14.empty())
            {
                fprintf(file, "`[add object particle file]`\n%d\n",
                        (int)script->m_destroy.m_strs14.size());
                for (std::vector<std::string>::const_iterator it =
                         script->m_destroy.m_strs14.begin();
                     it != script->m_destroy.m_strs14.end(); ++it)
                    fprintf(file, "`%s`\n", it->c_str());
            }
            if (!script->m_destroy.m_ints44.empty())
            {
                fprintf(file, "`[add object index on attack]`\n%d\n",
                        (int)script->m_destroy.m_ints44.size());
                for (std::vector<int>::const_iterator it = script->m_destroy.m_ints44.begin();
                     it != script->m_destroy.m_ints44.end(); ++it)
                    fprintf(file, "%d\n", *it);
            }
            fwrite("[/object destroy condition]\n", 1, 0x1c, file);
        }
    }
    if (!script->m_destroy.m_strs20.empty())
    {
        fwrite("[sound category]\n", 1, 0x11, file);
        for (std::vector<std::string>::size_type i = 0;
             i < script->m_destroy.m_strs20.size(); ++i)
        {
            if (script->m_destroy.m_strs20[i] == "")
                continue;
            switch (i)
            {
            case 0:
                fwrite("`[after object create]`", 1, 0x17, file);
                break;
            case 1:
                fwrite("`[on frame]`", 1, 0xc, file);
                break;
            case 2:
                fwrite("`[on attack]`", 1, 0xd, file);
                break;
            case 3:
                fwrite("`[on damage]`", 1, 0xd, file);
                break;
            case 4:
                fwrite("`[on destroyobject]`", 1, 0x14, file);
                break;
            }
            fprintf(file, " `%s` %d\n",
                    script->m_destroy.m_strs20[i].c_str(),
                    script->m_destroy.m_ints2c[i]);
        }
        fwrite("[/sound category]\n", 1, 0x12, file);
    }
    if (!script->m_addEffect.m_strs00.empty())
    {
        fwrite("[add object effect]\n", 1, 0x14, file);
        for (std::vector<std::string>::size_type i = 0;
             i < script->m_addEffect.m_strs00.size(); ++i)
            fprintf(file, "%s %d\n",
                    script->m_addEffect.m_strs00[i].c_str(),
                    script->m_addEffect.m_ints0c[i]);
        fwrite("[/add object effect]\n", 1, 0x15, file);
    }
    if (!script->m_particles.empty())
    {
        fwrite("[add particles]\n", 1, 0x10, file);
        for (std::vector<STPassiveObjectAdditionalParticleInfo*>::size_type i = 0;
             i < script->m_particles.size(); ++i)
        {
            fwrite("\t[particle]\n", 1, 0xc, file);
            STPassiveObjectAdditionalParticleInfo* p = script->m_particles[i];
            if (!p->m_s00.empty())
                fprintf(file, "\t\t[file] `%s`\n", p->m_s00.c_str());
            if (p->m_i0c > 0)
                fprintf(file, "\t\t[create delay] %d\n", p->m_i0c);
            if (!p->m_s04.empty())
                fprintf(file, "\t\t[create sound] `%s`\n", p->m_s04.c_str());
            if (p->m_i10 != 0)
            {
                fwrite("\t\t[follow parent]", 1, 0x11, file);
                fprintf(file, "%d %d %d\n", p->m_i10, p->m_i14, p->m_i18);
            }
            if (p->m_b1c != 0)
                fwrite("\t\t[loop] 1\n", 1, 0xb, file);
            fwrite("\t[/particle]\n", 1, 0xd, file);
        }
        fwrite("[/add particles]\n", 1, 0x11, file);
    }
    if (script->m_b9c != 0)
    {
        fwrite("[homing]\n", 1, 9, file);
        fwrite("\t[homing use] 1\n", 1, 0x10, file);
        if (script->m_ia0 == 2)
            fprintf(file, "\t[homing follow] `[MONSTER]` %d\n", script->m_ia4);
        else if (script->m_ia0 == 3)
            fprintf(file, "\t[homing follow] `[CHARACTER]` %d\n", script->m_ia4);
        else if (script->m_ia0 == 1)
            fwrite("\t[homing follow] `[ENEMY]`\n", 1, 0x1b, file);
        else
            fwrite("\t[homing follow] `[ME]`\n", 1, 0x18, file);
        if (script->m_fa8 != 0.0f || script->m_fac != 0.0f)
            fprintf(file, "\t[homing velocity] %f %f\n", script->m_fa8, script->m_fac);
        if (script->m_ib0 != 0)
            fprintf(file, "\t[homing check gap] %d\n", script->m_ib0);
        if (script->m_bb4 != 0)
            fwrite("\t[sync animation rotation] 1\n", 1, 0x1d, file);
        if (script->m_fb8 != 0.0f)
            fprintf(file, "\t[max rotation] %f\n", script->m_fb8);
        if (script->m_fbc != 0.0f)
            fprintf(file, "\t[init rotation] %f\n", script->m_fbc);
        if (script->m_fc0 != 0.0f)
            fprintf(file, "\t[diff rotation] %f\n", script->m_fc0);
        if (script->m_ic4 != 0)
            fprintf(file, "\t[homing time] %d\n", script->m_ic4);
        if (script->m_ic8 != 0)
            fprintf(file, "\t[homing z axis] %d\n", script->m_ic8);
        if (script->m_icc != -1)
            fprintf(file, "\t[homing direction] %d\n", script->m_icc);
        if (script->m_id0 != 0)
            fprintf(file, "\t[homing keep distance] %d\n", script->m_id0);
        if (script->m_bd4 != 0)
            fwrite("\t[sync target move]\n", 1, 0x14, file);
        if (script->m_bd5 != 0)
            fwrite("\t[straight homing]\n", 1, 0x13, file);
        fwrite("[/homing]\n", 1, 10, file);
    }
    if (script->m_b14c != 0)
        fwrite("[for tournament] 1\n", 1, 0x13, file);
    if (script->m_b14d != 0)
        fwrite("[for school] 1\n", 1, 0xf, file);
    if (script->m_b14e != 0)
        fwrite("[die when parent die] 1\n", 1, 0x18, file);
    if (script->m_ub14f != 0xff)
        fwrite("[transparent on meet player] 1\n", 1, 0x1f, file);
    if (script->m_us150 != 0xffff)
        fprintf(file, "[pvp room transparent value for observer] %d\n",
                (int)script->m_us150);
    if (script->m_s154 != "")
        fprintf(file, "[create sound] `%s`\n", script->m_s154.c_str());
    if (!script->m_onAttackData.empty())
    {
        fwrite("[can beat index]\n", 1, 0x11, file);
        for (std::vector<int>::const_iterator it = script->m_onAttackData.begin();
             it != script->m_onAttackData.end(); ++it)
            fprintf(file, "%d\n", *it);
        fwrite("[/can beat index]\n", 1, 0x12, file);
    }
    if (script->m_b16c != 0)
        fwrite("[is blocking obj] 1\n", 1, 0x14, file);
    if (script->m_f170 != -1.0f)
        fprintf(file, "[apply physics] %f\n", script->m_f170);
    if (script->m_i174 != 0)
        fwrite("[following immortal] 1\n", 1, 0x17, file);
    if (getFollowTypeName((ENUM_MAP_FOLLOW_TYPE)script->m_i178) != 0)
        fprintf(file, "[follow parent] `%s`\n", getFollowTypeName((ENUM_MAP_FOLLOW_TYPE)script->m_i178));
    if ((int)script->m_speedCtrls.size() > 0)
    {
        fwrite("[speed control passive objects]\n", 1, 0x20, file);
        for (std::vector<SpeedCtrlPassiveObjInfo>::const_iterator it =
                 script->m_speedCtrls.begin();
             it != script->m_speedCtrls.end(); ++it)
        {
            fwrite("[passive object]\n", 1, 0x11, file);
            fprintf(file, "\t[index] %d\n", it->m_field0);
            fprintf(file, "\t[speed] `%s`\n",
                    it->m_b4 == 0 ? "[down]" : "[up]");
            fwrite("[/passive object]\n", 1, 0x12, file);
        }
        fwrite("[/speed control passive objects]\n", 1, 0x21, file);
    }
    if (!script->m_genObjects.empty())
    {
        fwrite("[objects]\n", 1, 10, file);
        for (std::vector<PassiveObjGeneratorObject>::const_iterator it =
                 script->m_genObjects.begin();
             it != script->m_genObjects.end(); ++it)
        {
            fwrite("\t[object]\n", 1, 10, file);
            fprintf(file, "\t\t[index] %d\n", it->m_i00);
            fprintf(file, "\t\t[probability] %f\n", it->m_f04);
            fprintf(file, "\t\t[warning] `%s`\n", it->m_s08.c_str());
            fwrite("\t[/object]\n", 1, 0xb, file);
        }
        fwrite("[/objects]\n", 1, 0xb, file);
    }
    if (!script->m_genPatterns.empty())
    {
        fwrite("[patterns]\n", 1, 0xb, file);
        for (std::vector<PassiveObjGeneratorPattern>::const_iterator it =
                 script->m_genPatterns.begin();
             it != script->m_genPatterns.end(); ++it)
        {
            fwrite("\t[pattern]\n", 1, 0xb, file);
            fprintf(file, "\t\t[name] `%s`\n", it->m_s04.c_str());
            fprintf(file, "\t\t[interval] %d\n", it->m_i0c);
            fprintf(file, "\t\t[columns] %d\n", it->m_i08);
            if (!it->m_v10.empty())
            {
                fwrite("\t\t[data]\n", 1, 9, file);
                for (std::vector<unsigned char>::size_type d = 0;
                     d < it->m_v10.size(); ++d)
                {
                    if (d % it->m_i08 == 0)
                        fprintf(file, "\t\t\t%d", (int)it->m_v10[d]);
                    else if (d % it->m_i08 == (unsigned int)it->m_i08 - 1)
                        fprintf(file, " %d\n", (int)it->m_v10[d]);
                    else
                        fprintf(file, " %d", (int)it->m_v10[d]);
                }
                fwrite("\t\t[/data]\n", 1, 10, file);
            }
            fwrite("\t[/pattern]\n", 1, 0xc, file);
        }
        fwrite("[/patterns]\n", 1, 0xc, file);
    }
    if (!script->m_genSequences.empty())
    {
        fwrite("[sequences]\n", 1, 0xc, file);
        for (std::vector<PassiveObjGeneratorSequence>::const_iterator it =
                 script->m_genSequences.begin();
             it != script->m_genSequences.end(); ++it)
        {
            fwrite("\t[pattern]", 1, 10, file);
            fprintf(file, " `%s` %d", it->m_s00.c_str(), it->m_i04);
            fwrite(" [/pattern]\n", 1, 0xc, file);
        }
        fwrite("[/sequences]\n", 1, 0xd, file);
    }
    if (script->m_warnDanger.m_a != 0 || script->m_warnDanger.m_b != 0 ||
        script->m_warnDanger.m_c != 0 ||
        script->m_warnRecovery.m_a != 0 || script->m_warnRecovery.m_b != 0 ||
        script->m_warnRecovery.m_c != 0)
    {
        fwrite("[warning]\n", 1, 10, file);
        fprintf(file, "[min distance] %d %d %d\n",
                script->m_warnDanger.m_a, script->m_warnDanger.m_b,
                script->m_warnDanger.m_c);
        fprintf(file, "[max distance] %d %d %d\n",
                script->m_warnRecovery.m_a, script->m_warnRecovery.m_b,
                script->m_warnRecovery.m_c);
        fwrite("[/warning]\n", 1, 0xb, file);
    }
    fclose(file);
    convertBinaryToText(saveFileName);
    return 1;
}
