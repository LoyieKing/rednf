// ============================================================================
// df_game_r 还原 —— STStackableScript（G5 堆叠物脚本表）
// Clear 0x0899087a / ctor 0x0898f218 / copy 0x0898f8e6 / dtor 0x0899013c /
// operator= 0x089c418e；Arad_STStackableScript 0x089c33fa..0x089c4052。
// 子对象（STStatusRecovery/STBuff/STSummonMonster/STSummonApc/
// CountinuousItemOption/AvatarOptionChangeType）与 STStackableScript 同 TU
// （ORIG 0x089c30dc..0x089c34e8 弱符号紧邻），方法实现于此。
// ============================================================================

#include "STStackableScript.h"
#include "CStackableItem.h"  // RecipeInfo 权威布局（0x4c，ORIG 0x8513c6a 族）

#include <string.h>

#include "DNFLexWrapper.h"
#include "NpcScript.h"   // STScriptFileList（g_stStackableScriptFileList 双视图）

// ---- stStackableBooster_t::clear（ORIG W 0x089c3076：
//      vector::clear(@+0x00) + map::clear(@+0x0c)）----
void stStackableBooster_t::clear()
{
    m_elements.clear();
    m_map.clear();
}

stStackableBooster_t& stStackableBooster_t::operator=(
    const stStackableBooster_t& other)
{
    m_elements = other.m_elements;
    m_map = other.m_map;
    return *this;
}

// ---- STStatChange（ORIG clear 0x089c329e：+0x0c→+0x00→+0x04→+0x08 字节清零）----
STStatChange::STStatChange()
{
    clear();
}
void STStatChange::clear()
{
    m_fieldC = 0;
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
}
STStatChange::~STStatChange() {}

// ---- STStatChangeVector（ORIG C1 0x089c32c8 / clear 0x089c330a /
//      D1 0x089c3ace / operator= 0x089c40fe）----
STStatChangeVector::STStatChangeVector() : m_vec0()
{
    clear();
}
STStatChangeVector::~STStatChangeVector() {}
STStatChangeVector& STStatChangeVector::operator=(const STStatChangeVector& other)
{
    // ORIG：vector operator= 后逐个拷贝 +0x0c/+0x10 int 与 +0x14 字节。
    m_vec0 = other.m_vec0;
    m_fieldC = other.m_fieldC;
    m_field10 = other.m_field10;
    m_field14 = other.m_field14;
    return *this;
}

// ---- ORIG clear 0x089c330a：容器清空 + 三标量清零。保持显式定义供引用。----
void STStatChangeVector::clear()
{
    m_vec0.clear();
    m_fieldC = 0;
    m_field10 = 0;
    m_field14 = 0;
}

// ---- STStatusRecovery（ORIG C1 0x089c30dc 调 clear；clear 0x089c30f0）----
STStatusRecovery::STStatusRecovery()
{
    clear();
}
void STStatusRecovery::clear()
{
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
    m_fieldC = 0;
    m_field10 = 0;
}

// ---- STBuff（ORIG C1 0x089c3194 调 clear；clear 0x089c31a8）----
STBuff::STBuff()
{
    clear();
}
void STBuff::clear()
{
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
    m_fieldC = 0;
}

// ---- STSummonMonster（ORIG C1 0x089c31d2 调 clear；clear 0x089c31e6）----
STSummonMonster::STSummonMonster()
{
    clear();
}
void STSummonMonster::clear()
{
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
    m_fieldC = 0;
    m_field10 = 0;
    m_field14 = 0;
    m_field14 = 0;
    m_field18 = 0;
}

// ---- STSummonApc（ORIG C1 0x089c3238 调 clear；clear 0x089c324c）----
STSummonApc::STSummonApc()
{
    clear();
}
void STSummonApc::clear()
{
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
    m_fieldC = 0;
    m_field10 = 0;
    m_field14 = 0;
}

// ---- CountinuousItemOption（ORIG T 0x0898c770 C1 调 clear；clear 0x0898c784）----
CountinuousItemOption::CountinuousItemOption()
{
    clear();
}
void CountinuousItemOption::clear()
{
    m_field0 = 0;
    m_field1 = 0;
    m_field2 = 0;
}

// ---- AvatarOptionChangeType（ORIG C1 0x08513b6e 调 clear；clear 0x08513b48）----
// clear 链：m_b2 = 0xff → m_b1 = m_b2 → m_b0 = m_b1。
AvatarOptionChangeType::AvatarOptionChangeType()
{
    clear();
}
void AvatarOptionChangeType::clear()
{
    m_field2 = 0xff;
    m_field1 = m_field2;
    m_field0 = m_field1;
}

Arad_STStackableScript::Arad_STStackableScript()
{
    AradStClear();
}

Arad_STStackableScript::~Arad_STStackableScript()
{
}

Arad_STStackableScript& Arad_STStackableScript::operator=(
    const Arad_STStackableScript& other)
{
    m_0 = other.m_0;
    m_4 = other.m_4;
    m_str8 = other.m_str8;
    m_strc = other.m_strc;
    m_str10 = other.m_str10;
    return *this;
}

// ORIG 0x089c348e：清 +0x00/+0x04/-0x08..+0x10 三个 string；+0x14 由
// STStackableScript::Clear 单独清（见 Clear 尾部 movl $0,0x18c）。
void Arad_STStackableScript::AradStClear()
{
    m_0 = 0;
    m_4 = -1;
    m_str8 = "";
    m_strc = "";
    m_str10 = "";
}

STStackableScript::STStackableScript()
{
    Clear();
}

STStackableScript::STStackableScript(STStackableScript& other)
{
    operator=(other);
    if (!other.m_module4bc.empty())
    {
        std::map<int, EquipmentParameterInfo*>::iterator it = other.m_module4bc.begin();
        while (true)
        {
            std::map<int, EquipmentParameterInfo*>::iterator end_tmp = other.m_module4bc.end();
            if (!(it != end_tmp))
            {
                break;
            }
            std::map<int, EquipmentParameterInfo*>::iterator dst_end = m_module4bc.end();
            std::map<int, EquipmentParameterInfo*>::iterator found = m_module4bc.find(it->first);
            if (found != dst_end)
            {
                std::pair<int, EquipmentParameterInfo*> src_pair;
                src_pair = *it;
                EquipmentParameterInfo* p = new EquipmentParameterInfo();
                *p = *it->second;
                m_module4bc.insert(src_pair);
            }
            ++it;
        }
    }
}

// ORIG 0x0899013c：体部先清理 m_module4bc 的指针值（空检查 + 遍历 delete +
// clear），随后编译器按声明序析构全部成员。
STStackableScript::~STStackableScript()
{
    if (!m_module4bc.empty())
    {
        for (std::map<int, EquipmentParameterInfo*>::iterator it = m_module4bc.begin();
             it != m_module4bc.end(); ++it)
        {
            if (it->second != 0)
            {
                delete it->second;
                it->second = 0;
            }
        }
        m_module4bc.clear();
    }
}

// ORIG 0x089c418e：无 self 检查；基类 op= → arad op= → +0x18c →
// 依序拷贝各容器/string/标量；STRecovery×2/STStatusRecovery 与
// STBuff/STSummonMonster/STSummonApc 三个平凡区用 memcpy（ORIG call memcpy@plt）。
STStackableScript& STStackableScript::operator=(const STStackableScript& other)
{
    STItemScript::operator=(other);
    m_arad178 = other.m_arad178;
    m_arad178.m_field14 = other.m_arad178.m_field14;
    m_vec190 = other.m_vec190;
    m_rabbit19c = other.m_rabbit19c;
    m_1a8 = other.m_1a8;
    m_1ac = other.m_1ac;
    m_1b0 = other.m_1b0;
    m_1b4 = other.m_1b4;
    m_1b8 = other.m_1b8;
    m_option1b9 = other.m_option1b9;
    m_option1bc = other.m_option1bc;
    m_1bf = other.m_1bf;
    m_1c0 = other.m_1c0;
    m_1c4 = other.m_1c4;
    m_1c8 = other.m_1c8;
    m_1cc = other.m_1cc;
    m_1d0 = other.m_1d0;
    m_1d4 = other.m_1d4;
    m_str1d8 = other.m_str1d8;
    m_1dc = other.m_1dc;
    m_1e0 = other.m_1e0;
    m_1e4 = other.m_1e4;
    m_1e8 = other.m_1e8;
    m_vec1ec = other.m_vec1ec;
    m_place1f8 = other.m_place1f8;
    m_204 = other.m_204;
    m_205 = other.m_205;
    m_passive208 = other.m_passive208;
    m_server214 = other.m_server214;
    m_220 = other.m_220;
    m_vec224 = other.m_vec224;
    m_str230 = other.m_str230;
    m_str234 = other.m_str234;
    m_str238 = other.m_str238;
    m_23c = other.m_23c;
    m_240 = other.m_240;
    m_vec244 = other.m_vec244;
    m_vec250 = other.m_vec250;
    m_vec25c = other.m_vec25c;
    m_package268 = other.m_package268;
    m_package274 = other.m_package274;
    m_booster280 = other.m_booster280;
    m_package2a4 = other.m_package2a4;
    memcpy(&m_recovery2b0, &other.m_recovery2b0, 0x11);
    memcpy(&m_recovery2c4, &other.m_recovery2c4, 0x11);
    memcpy(&m_statusRecovery2d8, &other.m_statusRecovery2d8, 0x11);
    m_status2ec = other.m_status2ec;
    memcpy(&m_buff304, &other.m_buff304, 0xd);
    memcpy(&m_summonMonster314, &other.m_summonMonster314, 0x19);
    memcpy(&m_summonApc330, &other.m_summonApc330, 0x15);
    m_348 = other.m_348;
    m_statChange34c = other.m_statChange34c;
    m_364 = other.m_364;
    m_36c = other.m_36c;
    m_370 = other.m_370;
    m_str368 = other.m_str368;
    m_380 = other.m_380;
    m_384 = other.m_384;
    m_388 = other.m_388;
    m_38c = other.m_38c;
    m_390 = other.m_390;
    m_394 = other.m_394;
    m_396 = other.m_396;
    m_398 = other.m_398;
    m_39a = other.m_39a;
    m_39b = other.m_39b;
    m_39c = other.m_39c;
    m_3a0 = other.m_3a0;
    m_3a1 = other.m_3a1;
    m_vec374 = other.m_vec374;
    m_map3a4 = other.m_map3a4;
    m_vec3c0 = other.m_vec3c0;
    m_3cc = other.m_3cc;
    m_3d0 = other.m_3d0;
    m_3d1 = other.m_3d1;
    m_cube3d4 = other.m_cube3d4;
    m_enchant3f4 = other.m_enchant3f4;
    m_40c = other.m_40c;
    *(unsigned short*)&m_40d[0] = *(const unsigned short*)&other.m_40d[0];
    m_str410 = other.m_str410;
    m_414 = other.m_414;
    m_418 = other.m_418;
    m_boosterSelect41c = other.m_boosterSelect41c;
    m_438 = other.m_438;
    m_pair43c = other.m_pair43c;
    m_444 = other.m_444;
    m_448 = other.m_448;
    m_44c = other.m_44c;
    m_450 = other.m_450;
    m_454 = other.m_454;
    m_458 = other.m_458;
    m_45c = other.m_45c;
    m_460 = other.m_460;
    m_464 = other.m_464;
    m_468 = other.m_468;
    m_46c = other.m_46c;
    m_470 = other.m_470;
    m_474 = other.m_474;
    m_478 = other.m_478;
    m_avatar47c = other.m_avatar47c;
    m_set480 = other.m_set480;
    m_set498 = other.m_set498;
    m_4b0 = other.m_4b0;
    m_str4b4 = other.m_str4b4;
    m_4b8 = other.m_4b8;
    m_module4bc = other.m_module4bc;
    m_set4d4 = other.m_set4d4;
    m_4ec = other.m_4ec;
    m_4f0 = other.m_4f0;
    m_4f4 = other.m_4f4;
    m_4f8 = other.m_4f8;
    m_vec4fc = other.m_vec4fc;
    m_508 = other.m_508;
    m_set50c = other.m_set50c;
    return *this;
}

// ORIG 0x0899087a：赋值序严格照 ORIG 反编译逐条转写。
void STStackableScript::Clear()
{
    STItemScript::Clear();
    m_field4 = 0;
    m_arad178.AradStClear();
    m_rabbit19c.clear();
    m_vec190.clear();
    m_1a8 = 0;
    m_1ac = 0;
    m_1b0 = 0;
    m_1b4 = -1;
    m_1b8 = 0;
    m_str1d8 = "";
    m_1dc = 0;
    m_1d4 = 0;
    m_1e0 = 0x27;
    m_1e4 = 0;
    m_str238 = "";
    m_23c = 1;
    m_240 = -1;
    m_option1b9.m_field0 = 0;
    m_option1b9.m_field1 = 0;
    m_option1b9.m_field2 = 0;
    m_statChange34c.clear();
    m_option1bc.m_field0 = 0;
    m_option1bc.m_field1 = 0;
    m_option1bc.m_field2 = 0;
    m_1bf = 0;
    m_1c0 = 0;
    m_1c4 = 0;
    m_1c8 = 0;
    m_1cc = 0;
    m_1d0 = 0;
    m_package268.clear();
    m_package274.clear();
    m_booster280.clear();
    m_364 = 9999999;
    m_module4bc.clear();
    m_str368 = "";
    m_36c = -1;
    m_vec374.clear();
    m_380 = 0;
    m_384 = 0;
    m_388 = 1;
    m_38c = -1;
    m_summonMonster314.clear();
    m_summonApc330.clear();
    m_348 = -1;
    m_390 = 1;
    m_394 = 0;
    m_396 = 0;
    m_398 = 0;
    m_438 = 0;
    m_pair43c.first = 0;
    m_pair43c.second = 0;
    m_39a = 0;
    m_39b = 0;
    m_39c = 0;
    m_3a0 = 0;
    m_3a1 = 0;
    m_map3a4.clear();
    m_3bc = 0;
    m_vec3c0.clear();
    m_3cc = 0;
    m_vec25c.clear();
    m_3d0 = 0;
    m_3d1 = 1;
    m_cube3d4.m_materials.clear();
    m_cube3d4.m_2ndMaterialIdx = 0;
    m_cube3d4.m_2ndMaterialCount = 0;
    m_cube3d4.m_results.clear();
    m_enchant3f4.clear();
    m_40c = 0xff;
    memset(m_40d, 0, 2);
    m_414 = 0;
    m_str410 = "";
    m_418 = 0;
    m_444 = 0xb;
    m_448 = 0;
    m_44c = -1;
    m_450 = -1;
    m_454 = -1;
    m_458 = -1;
    m_45c = -1;
    m_460 = -1;
    m_464 = -1;
    m_468 = -1;
    m_46c = 0;
    m_470 = -1;
    m_478 = 0;
    m_474 = 0;
    m_avatar47c.clear();
    m_set480.clear();
    m_set498.clear();
    m_4b0 = 0;
    m_1e8 = 0;
    m_vec1ec.clear();
    m_place1f8.clear();
    m_204 = 0;
    m_server214.clear();
    m_205 = 0;
    m_passive208.clear();
    m_220 = 0;
    m_vec224.clear();
    m_4b8 = 0;
    m_str1d8 = "";
    m_1dc = 0;
    m_414 = 0;
    m_str410 = "";
    m_418 = 0;
    m_370 = -1;
    m_4ec = 0;
    m_arad178.m_field14 = 0;
    m_4f0 = 0;
    m_4f4 = 0;
    m_4f8 = 0;
    m_vec4fc.clear();
    m_set4d4.clear();
    m_508 = 0;
    m_set50c.clear();
}

void RecipeInfo::additionalInfo(STStackableScript& script)
{
    // ORIG 0x8513d44（_ZN10RecipeInfo14additionalInfoER17STStackableScript）：
    // m_field34 = script.m_4f0; m_field38 = script.m_4f4; m_field3c = script.m_4f8;
    // 随后 while (size() > (unsigned)i) 循环：每轮读 v[i] 与 v[i+1] 组成 pair
    // push_back 到 +0x40 的 vector<pair<int,int>>，i 步进 2（ORIG 循环体两次
    // `addl $1,-0xc`，即相邻不重叠配对）；越界保护 size()<=i 时退出；
    // 比较为无符号（setbe/seta）。
    m_field34 = script.m_4f0;
    m_field38 = script.m_4f4;
    m_field3c = script.m_4f8;
    int i = 0;
    while (script.m_vec4fc.size() > (unsigned)i)
    {
        int first = script.m_vec4fc[i++];
        if (script.m_vec4fc.size() <= (unsigned)i)
        {
            break;
        }
        int second = script.m_vec4fc[i++];
        std::pair<int, int> item(first, second);
        m_vec40.push_back(item);
    }
}

// ---- ORIG 全局（定义 InitScriptProducers_4.cpp）：脚本目录与文件列表 ----
extern char g_szStackableScriptBaseDirectory[0x100];
extern std::map<int, std::string> g_stStackableScriptFileList;

bool ImportStackableScript(STStackableScript* script, const char* path,
                           bool flag, int idx)
{
    // ORIG 0x089aeec8（0xb974，Ghidra C 逐句转写）：
    // loadRDARScriptFile(g_szStackableScriptBaseDirectory, path) → Clear() →
    // ScanType 主循环；顶层 if-else token 分派（[name]/[emancipate ticket]/
    // [equipment reinforcement ticket]/[check usable itemlevel]/
    // [passive object in stackable]/[amplification random value] 等有体分支，
    // 其余标签经 5 个惰性 static map<const char*,int> 查表分派——该分派域
    // 数百标签的解析体未随本批次落地，查不到的标签按"忽略继续"处理）。
    // 循环结束：m_field24==-1 时回填 m_field14；m_field0 = (idx==-1 ?
    // FindIndexByFullScan(path) : idx)；返回 true。
    if (!loadRDARScriptFile(g_szStackableScriptBaseDirectory, path))
    {
        return false;
    }
    script->Clear();
    std::string line;
    bool ok = false;
    while (ScanType(line, true))
    {
        if (line == "[name]")
        {
            ScanStr(&script->m_strac);
            if (flag)
            {
                goto done;
            }
        }
        else if (line == "[emancipate ticket]")
        {
            script->m_field10e = 1;
            script->m_394 = (unsigned short)ScanInt(&ok);
        }
        else if (line == "[equipment reinforcement ticket]")
        {
            script->m_396 = 1;
            script->m_398 = (unsigned short)ScanInt(&ok);
            if (!ok)
            {
                return false;
            }
            script->m_39a = (char)ScanInt(&ok);
            if (!ok)
            {
                return false;
            }
        }
        else if (line == "[check usable itemlevel]")
        {
            script->m_pair43c.first = ScanInt(&ok);
            if (!ok)
            {
                return false;
            }
            script->m_pair43c.second = ScanInt(&ok);
            if (!ok)
            {
                return false;
            }
            script->m_438 = 1;
        }
        else if (line == "[passive object in stackable]")
        {
            while (true)
            {
                STPassiveObjectInStackable obj;
                obj.m_field0 = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                obj.m_field4 = ScanInt(&ok);
                obj.m_field8 = ScanInt(&ok);
                obj.m_fieldC = ScanInt(&ok);
                obj.m_field10 = ScanInt(&ok);
                script->m_passive208.push_back(obj);
            }
        }
        else if (line == "[amplification random value]")
        {
            script->m_3a1 = 1;
            while (true)
            {
                int a = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                int b = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                script->m_map3a4.insert(std::pair<int, int>(a, b));
            }
        }
        // 其余标签（[equipment amplify reinforcement ticket] 等空体分支及
        // static 表分派域）按"忽略继续"处理。
    }
done:
    if (script->m_field24 == -1)
    {
        script->m_field24 = script->m_field14;
    }
    if (idx == -1)
    {
        script->m_field0 = reinterpret_cast<STScriptFileList*>(
            &g_stStackableScriptFileList)->FindIndexByFullScan(path);
    }
    else
    {
        script->m_field0 = idx;
    }
    return true;
}

bool StackableScriptSpecialMonsterDrop(STStackableScript* script,
                                       std::string str, const char* path)
{
    // ORIG 0x89ae906（Ghidra C 逐句转写）：
    // 局部：bool local_15、std::string local_14、char local_d。
    // "[special monster drop]"：local_15=false → ScanInt(&local_15) 返回 int iVar3 →
    //   local_d = (iVar3 != 0)；若 local_15 != true 返回 false；否则
    //   m_specialDrop.m_enable = local_d。
    // "[difficulty for extra drop]" / "[dungeon type for extra drop]"：循环 ScanStr，
    //   命中标签置对应 flag（+0x106..+0x10a 难度 / +0x10b..+0x10d 类型），
    //   "[all]" memset 5 / 3 字节。其余输入返回 true。
    (void)path;
    std::string local_14;
    bool local_15;
    char local_d;
    if (str == "[special monster drop]")
    {
        local_15 = false;
        local_d = ScanInt(&local_15) != 0;
        if (local_15 != true)
        {
            return false;
        }
        script->m_specialDrop.m_enable = local_d;
    }
    else if (str == "[difficulty for extra drop]")
    {
        while (ScanStr(&local_14))
        {
            if (local_14 == "[easy]")
            {
                script->m_specialDrop.m_difficulty[0] = 1;
            }
            else if (local_14 == "[medium]")
            {
                script->m_specialDrop.m_difficulty[1] = 1;
            }
            else if (local_14 == "[hard]")
            {
                script->m_specialDrop.m_difficulty[2] = 1;
            }
            else if (local_14 == "[ultimate]")
            {
                script->m_specialDrop.m_difficulty[3] = 1;
            }
            else if (local_14 == "[hero]")
            {
                script->m_specialDrop.m_difficulty[4] = 1;
            }
            else if (local_14 == "[all]")
            {
                memset(script->m_specialDrop.m_difficulty, 1, 5);
            }
        }
    }
    else if (str == "[dungeon type for extra drop]")
    {
        while (ScanStr(&local_14))
        {
            if (local_14 == "[normal]")
            {
                script->m_specialDrop.m_dungeonType[0] = 1;
            }
            else if (local_14 == "[ancient]")
            {
                script->m_specialDrop.m_dungeonType[1] = 1;
            }
            else if (local_14 == "[dimension]")
            {
                script->m_specialDrop.m_dungeonType[2] = 1;
            }
            else if (local_14 == "[all]")
            {
                memset(script->m_specialDrop.m_dungeonType, 1, 3);
            }
        }
    }
    return true;
}
