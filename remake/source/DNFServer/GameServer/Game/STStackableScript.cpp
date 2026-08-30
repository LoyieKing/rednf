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
        std::map<ENUM_MODULE_TYPE, EquipmentParameterInfo*>::iterator it = other.m_module4bc.begin();
        while (true)
        {
            std::map<ENUM_MODULE_TYPE, EquipmentParameterInfo*>::iterator end_tmp = other.m_module4bc.end();
            if (!(it != end_tmp))
            {
                break;
            }
            std::map<ENUM_MODULE_TYPE, EquipmentParameterInfo*>::iterator dst_end = m_module4bc.end();
            std::map<ENUM_MODULE_TYPE, EquipmentParameterInfo*>::iterator found = m_module4bc.find(it->first);
            if (found != dst_end)
            {
                std::pair<ENUM_MODULE_TYPE, EquipmentParameterInfo*> src_pair;
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
        for (std::map<ENUM_MODULE_TYPE, EquipmentParameterInfo*>::iterator it = m_module4bc.begin();
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
    // ORIG 对 6 个平凡区 memcpy 均带 self 守卫（cmp this+off vs other+off; je 跳过）
    if (&m_recovery2b0 != &other.m_recovery2b0)
        memcpy(&m_recovery2b0, &other.m_recovery2b0, 0x11);
    if (&m_recovery2c4 != &other.m_recovery2c4)
        memcpy(&m_recovery2c4, &other.m_recovery2c4, 0x11);
    if (&m_statusRecovery2d8 != &other.m_statusRecovery2d8)
        memcpy(&m_statusRecovery2d8, &other.m_statusRecovery2d8, 0x11);
    m_status2ec = other.m_status2ec;
    if (&m_buff304 != &other.m_buff304)
        memcpy(&m_buff304, &other.m_buff304, 0xd);
    if (&m_summonMonster314 != &other.m_summonMonster314)
        memcpy(&m_summonMonster314, &other.m_summonMonster314, 0x19);
    if (&m_summonApc330 != &other.m_summonApc330)
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

namespace {
static int StackableTagLookup(const std::string& tag)
{
    if (tag == "[name2]") return 8292;
    if (tag == "[explain]") return 8303;
    if (tag == "[explain2]") return 8306;
    if (tag == "[flavor text]") return 8309;
    if (tag == "[free]") return 8313;
    if (tag == "[trade]") return 8322;
    if (tag == "[trade delete]") return 8325;
    if (tag == "[sealing]") return 8329;
    if (tag == "[trade limit]") return 8336;
    if (tag == "[creation rate]") return 8339;
    if (tag == "[rarity]") return 8345;
    if (tag == "[usable job]") return 8359;
    if (tag == "[additional item]") return 8374;
    if (tag == "[collision item]") return 8384;
    if (tag == "[waste]") return 8407;
    if (tag == "[material]") return 8416;
    if (tag == "[material expert job]") return 8419;
    if (tag == "[recipe]") return 8422;
    if (tag == "[throw]") return 8425;
    if (tag == "[set]") return 8428;
    if (tag == "[legacy]") return 8431;
    if (tag == "[stackable legacy]") return 8434;
    if (tag == "[quest]") return 8437;
    if (tag == "[creature]") return 8441;
    if (tag == "[feed]") return 8445;
    if (tag == "[creature expitem]") return 8449;
    if (tag == "[etc]") return 8453;
    if (tag == "[only effect]") return 8456;
    if (tag == "[upgradable legacy]") return 8460;
    if (tag == "[cera package]") return 8464;
    if (tag == "[usable cera package]") return 8467;
    if (tag == "[cera booster]") return 8470;
    if (tag == "[booster]") return 8478;
    if (tag == "[booster random]") return 8484;
    if (tag == "[unlimited waste]") return 8490;
    if (tag == "[multi upgradable legacy]") return 8494;
    if (tag == "[multi upgradable legacy bonus cera]") return 8497;
    if (tag == "[quest receive]") return 8500;
    if (tag == "[avatar emblem]") return 8503;
    if (tag == "[unlimited etc]") return 8506;
    if (tag == "[disguise]") return 8510;
    if (tag == "[expert town potion]") return 8514;
    if (tag == "[global effect]") return 8517;
    if (tag == "[contract]") return 8526;
    if (tag == "[enchant waste]") return 8540;
    if (tag == "[random upgradable legacy]") return 8544;
    if (tag == "[upgrade limit cube]") return 8547;
    if (tag == "[tarot card lottery]") return 8552;
    if (tag == "[tarot card]") return 8555;
    if (tag == "[town and dungeon]") return 8564;
    if (tag == "[unlimited town and dungeon]") return 8569;
    if (tag == "[booster selection]") return 8597;
    if (tag == "[teleport potion]") return 8644;
    if (tag == "[premium]") return 8648;
    if (tag == "[action type]") return 8666;
    if (tag == "[use action packet]") return 8674;
    if (tag == "[daily delete item]") return 8684;
    if (tag == "[increase status type]") return 8701;
    if (tag == "[action usable place]") return 8720;
    if (tag == "[action usable server type]") return 8733;
    if (tag == "[max having count]") return 8737;
    if (tag == "[minimum level]") return 8743;
    if (tag == "[minimum rank]") return 8746;
    if (tag == "[appendage group]") return 8749;
    if (tag == "[cool time]") return 8752;
    if (tag == "[cooltime group]") return 8755;
    if (tag == "[cooltime maintenance]") return 8760;
    if (tag == "[effect maintenance]") return 8766;
    if (tag == "[shared effect]") return 8771;
    if (tag == "[exp bonus rate]") return 8776;
    if (tag == "[pc exp bonus rate]") return 8781;
    if (tag == "[icon]") return 8786;
    if (tag == "[tarotcard view]") return 8791;
    if (tag == "[field image]") return 8795;
    if (tag == "[price]") return 8798;
    if (tag == "[value]") return 8805;
    if (tag == "[finish point price]") return 8808;
    if (tag == "[disable icon]") return 8813;
    if (tag == "[need material]") return 8818;
    if (tag == "[recieve postal message]") return 8822;
    if (tag == "[linking quest index]") return 8828;
    if (tag == "[cash]") return 8831;
    if (tag == "[auction free]") return 8840;
    if (tag == "[medal]") return 8844;
    if (tag == "[usable upgrade chracter level]") return 8847;
    if (tag == "[move wav]") return 8850;
    if (tag == "[int data]") return 8863;
    if (tag == "[default list]") return 9025;
    if (tag == "[string data]") return 9031;
    if (tag == "[applying range]") return 9034;
    if (tag == "[stack limit]") return 9045;
    if (tag == "[packagable]") return 9048;
    if (tag == "[creature minimum level]") return 9051;
    if (tag == "[icon mark]") return 9064;
    if (tag == "[usable period]") return 9067;
    if (tag == "[package data]") return 9098;
    if (tag == "[secret add item]") return 9114;
    if (tag == "[booster info]") return 9162;
    if (tag == "[booster select category]") return 9367;
    if (tag == "[booster category num]") return 9486;
    if (tag == "[booster selection num]") return 9522;
    if (tag == "[equipment upgrade]") return 9541;
    if (tag == "[hp recovery]") return 9555;
    if (tag == "myself") return 9619;
    if (tag == "blind") return 9745;
    if (tag == "slow") return 9753;
    if (tag == "haste") return 9757;
    if (tag == "stone") return 9760;
    if (tag == "confuse") return 9763;
    if (tag == "weakness") return 9766;
    if (tag == "armor break") return 9769;
    if (tag == "weapon break") return 9772;
    if (tag == "poison") return 9775;
    if (tag == "lightning") return 9778;
    if (tag == "bleeding") return 9781;
    if (tag == "bless") return 9787;
    if (tag == "[buff]") return 9798;
    if (tag == "[stat change duration]") return 9851;
    if (tag == "[monster summons]") return 9878;
    if (tag == "[event monster summons]") return 9889;
    if (tag == "[summon apc]") return 9917;
    if (tag == "[summon npc]") return 9920;
    if (tag == "[/enchant]") return 9936;
    if (tag == "[avatar package preview info]") return 9965;
    if (tag == "[creature preview info]") return 9974;
    if (tag == "[avatar emblem target type]") return 9988;
    if (tag == "[usable date]") return 10010;
    if (tag == "../RDARScriptItem.cpp") return 10025;
    if (tag == "[npc gift disallowance]") return 10030;
    if (tag == "[guild power war point]") return 10033;
    if (tag == "[user power war point]") return 10039;
    if (tag == "trade") return 10061;
    if (tag == "[limit dungeon]") return 10088;
    if (tag == "[disable]") return 10091;
    if (tag == "[use item emoticon index]") return 10097;
    if (tag == "[alchemist]") return 10109;
    if (tag == "[monster card id]") return 10124;
    if (tag == "[special monster drop]") return 10129;
    if (tag == "[dungeon type for extra drop]") return 10133;
    if (tag == "[use 2nd upgrade table]") return 10160;
    if (tag == "[do not delete]") return 10176;
    if (tag == "[arad_item_kind]") return 10180;
    if (tag == "[usable first growtype]") return 10185;
    if (tag == "[arad_use]") return 10189;
    if (tag == "[mail from]") return 10192;
    if (tag == "[mail content]") return 10199;
    if (tag == "[onemore check]") return 10209;
    if (tag == "[evolution creature]") return 10242;
    if (tag == "[type]") return 10314;
    if (tag == "[usable world]") return 10388;
    if (tag == "weapon") return 10422;
    if (tag == "cloth") return 10431;
    if (tag == "leather") return 10434;
    if (tag == "larmor") return 10437;
    if (tag == "harmor") return 10440;
    if (tag == "plate") return 10443;
    if (tag == "amulet") return 10446;
    if (tag == "wrist") return 10449;
    if (tag == "ring") return 10452;
    if (tag == "[expertjob only]") return 10466;
    if (tag == "[prof material variation]") return 10474;
    if (tag == "[prof compound rate]") return 10478;
    if (tag == "[portable disjoint]") return 10483;
    if (tag == "[shop doll]") return 10488;
    if (tag == "[unseal scroll level limit]") return 10493;
    if (tag == "[monstercard bind]") return 10498;
    if (tag == "[alchemist extraction]") return 10503;
    if (tag == "[enchanter extraction]") return 10508;
    if (tag == "[doll_controller extraction]") return 10514;
    if (tag == "[item category]") return 10526;
    if (tag == "[usable game server type]") return 10532;
    if (tag == "[levelup range]") return 10539;
    if (tag == "[lottery use cost]") return 10546;
    if (tag == "[chat notice gold]") return 10554;
    if (tag == "[/chat notice gold]") return 10558;
    if (tag == "[all]") return 10594;
    if (tag == "[random option unseal]") return 10665;
    if (tag == "[success rate]") return 10683;
    if (tag == "[broadcast type]") return 10695;
    if (tag == "[fail output]") return 10709;
    if (tag == "[trade limit max]") return 10719;
    return -1;
}
}  // namespace

bool ImportStackableScript(STStackableScript* script, const char* path,
                           bool flag, int idx)
{
    if (!loadRDARScriptFile(g_szStackableScriptBaseDirectory, path))
    {
        return false;
    }
    script->Clear();
    std::string line;
    bool ok = false;
    std::string token;
    std::string local_str;

    // 标签分派：StackableTagLookup 返回 ORIG case 值，switch 分派。
    // ORIG 0x089aeec8 主 map 181 标签；复杂容器 case（package/booster/recovery/
    // status/usable job 等）按 ORIG 反汇编转写，未完全确认处标注【推断】。
    while (ScanType(line, true))
    {
        switch (StackableTagLookup(line))
        {
case 0x2064:  // "[name2]"
    ScanStr(&script->m_strb0);  // +0xb0
    break;

case 0x2067:  // "[name2]"
    ScanStr(&script->m_strb4);  // +0xb4
    break;

case 0x206b:  // "[name2]"
    ScanStr(&script->m_strb4);  // +0xb4
    break;

case 0x206f:  // "[explain]"
    ScanStr(&script->m_strb8);  // +0xb8
    break;

case 0x2072:  // "[explain2]"
    script->m_field4 = ScanInt(&ok);  // +0x4
    break;

case 0x2079:  // "[free]"
    script->m_field8 = 0x0;
    break;

case 0x207c:  // "[free]"
    script->m_field8 = 0x1;
    break;

case 0x207f:  // "[free]"
    script->m_field8 = 0x2;
    break;

case 0x2082:  // "[trade]"
    script->m_field8 = 0x3;
    break;

case 0x2085:  // "[trade delete]"
    script->m_field8 = 0x5;
    break;

case 0x2089:  // "[sealing]"
    script->m_field8 = 0x7;
    break;

case 0x2090:  // "[trade limit]"
    script->m_fieldc = ScanInt(&ok);  // +0xc
    break;

case 0x2093:  // "[creation rate]"
    ScanInt(&ok);  // 局部用途 @89b0726
    break;

case 0x2096:  // "[creation rate]"
    script->m_field34 = ScanInt(&ok);  // +0x34
    break;

case 0x2099:  // "[rarity]"
    ScanStr(&local_str);  // 局部 string @89b0887
    break;

case 0x20a7:  // "[usable job]"
    ScanStr(&local_str);  // 局部 string @89b09b9
    break;

case 0x20b6:  // "[additional item]"
    ScanInt(&ok);  // 局部用途 @89b0a50
    ScanInt(&ok);  // 局部用途 @89b0a6e
    ScanInt(&ok);  // 局部用途 @89b0a8c
    break;

case 0x20c0:  // "[collision item]"
    ScanInt(&ok);  // 局部用途 @89b0b6b
    break;

case 0x20d7:  // "[waste]"
    script->m_1e0 = 0x0;
    break;

case 0x20da:  // "[waste]"
    script->m_1e0 = 0x1;
    break;

case 0x20dd:  // "[waste]"
    script->m_1e0 = 0x2;
    break;

case 0x20e0:  // "[material]"
    script->m_1e0 = 0x3;
    break;

case 0x20e3:  // "[material expert job]"
    script->m_1e0 = 0x4;
    break;

case 0x20e6:  // "[recipe]"
    script->m_1e0 = 0x5;
    break;

case 0x20e9:  // "[throw]"
    script->m_1e0 = 0x6;
    break;

case 0x20ec:  // "[set]"
    script->m_1e0 = 0x7;
    break;

case 0x20ef:  // "[legacy]"
    script->m_1e0 = 0x8;
    break;

case 0x20f2:  // "[stackable legacy]"
    script->m_1e0 = 0x9;
    break;

case 0x20f5:  // "[quest]"
    script->m_1e0 = 0xa;
    break;

case 0x20f9:  // "[creature]"
    script->m_1e0 = 0x23;
    break;

case 0x20fd:  // "[feed]"
    script->m_1e0 = 0xb;
    break;

case 0x2101:  // "[creature expitem]"
    script->m_1e0 = 0xc;
    break;

case 0x2105:  // "[etc]"
    script->m_1e0 = 0xd;
    break;

case 0x2108:  // "[only effect]"
    script->m_1e0 = 0xe;
    break;

case 0x210c:  // "[upgradable legacy]"
    script->m_1e0 = 0x26;
    break;

case 0x2110:  // "[cera package]"
    script->m_1e0 = 0xf;
    break;

case 0x2113:  // "[usable cera package]"
    script->m_1e0 = 0x10;
    break;

case 0x2116:  // "[cera booster]"
    script->m_1e0 = 0x11;
    break;

case 0x211e:  // "[booster]"
    script->m_1e0 = 0x12;
    break;

case 0x2124:  // "[booster random]"
    script->m_1e0 = 0x1a;
    break;

case 0x212a:  // "[unlimited waste]"
    script->m_1e0 = 0x1b;
    break;

case 0x212e:  // "[multi upgradable legacy]"
    script->m_1e0 = 0x13;
    break;

case 0x2131:  // "[multi upgradable legacy bonus cera]"
    script->m_1e0 = 0x14;
    break;

case 0x2134:  // "[quest receive]"
    script->m_1e0 = 0x15;
    break;

case 0x2137:  // "[avatar emblem]"
    script->m_1e0 = 0x16;
    break;

case 0x213a:  // "[unlimited etc]"
    script->m_1e0 = 0x18;
    break;

case 0x213e:  // "[disguise]"
    script->m_1e0 = 0x17;
    break;

case 0x2142:  // "[expert town potion]"
    script->m_1e0 = 0x19;
    break;

case 0x2145:  // "[global effect]"
    script->m_1e0 = 0x1c;
    break;

case 0x214e:  // "[contract]"
    script->m_1e0 = 0x1a;
    break;

case 0x215c:  // "[enchant waste]"
    script->m_1e0 = 0x1d;
    break;

case 0x2160:  // "[random upgradable legacy]"
    script->m_1e0 = 0x1e;
    break;

case 0x2163:  // "[upgrade limit cube]"
    script->m_1e0 = 0x1f;
    break;

case 0x2168:  // "[tarot card lottery]"
    script->m_1e0 = 0x20;
    break;

case 0x216b:  // "[tarot card]"
    script->m_1e0 = 0x21;
    break;

case 0x2174:  // "[town and dungeon]"
    script->m_1e0 = 0x22;
    break;

case 0x2179:  // "[unlimited town and dungeon]"
    script->m_1e0 = 0x24;
    break;

case 0x2195:  // "[booster selection]"
    script->m_1e0 = 0x25;
    script->m_fielda4 = ScanInt(&ok);  // +0xa4
    break;

case 0x21c4:  // "[teleport potion]"
    script->m_arad178.m_field14 = ScanInt(&ok);  // +0x18c
    break;

case 0x21c8:  // "[premium]"
    script->m_1e4 = ScanInt(&ok);  // +0x1e4
    break;

case 0x21cb:  // "[premium]"
    ScanStr(&local_str);  // 局部 string @89b2396
    ScanInt(&ok);  // 局部用途 @89b23bb
    break;

case 0x21da:  // "[action type]"
    script->m_204 = ScanInt(&ok) != 0;  // +0x204
    break;

case 0x21e2:  // "[use action packet]"
    ScanInt(&ok);  // 局部用途 @89b24f9
    break;

case 0x21ec:  // "[daily delete item]"
    ScanStr(&local_str);  // 局部 string @89b2592
    ScanInt(&ok);  // 局部用途 @89b25b7
    break;

case 0x21fd:  // "[increase status type]"
    ScanStr(&local_str);  // 局部 string @89b2659
    break;

case 0x2210:  // "[action usable place]"
    ScanStr(&local_str);  // 局部 string @89b2702
    break;

case 0x221d:  // "[action usable server type]"
    script->m_field60 = ScanInt(&ok);  // +0x60
    break;

case 0x2221:  // "[max having count]"
    script->m_field5c = ScanInt(&ok);  // +0x5c
    break;

case 0x2224:  // "[max having count]"
    script->m_field64 = ScanInt(&ok);  // +0x64
    break;

case 0x2227:  // "[minimum level]"
    script->m_1b0 = ScanInt(&ok);  // +0x1b0
    break;

case 0x222a:  // "[minimum rank]"
    script->m_field68 = ScanInt(&ok);  // +0x68
    break;

case 0x222d:  // "[appendage group]"
    script->m_1b4 = ScanInt(&ok);  // +0x1b4
    break;

case 0x2230:  // "[cool time]"
    script->m_1b8 = ScanInt(&ok) != 0;  // +0x1b8
    break;

case 0x2233:  // "[cooltime group]"
    *(char*)((char*)script+0x1b9) = ScanInt(&ok) != 0;  // +0x1b9
    *(int*)((char*)script+0x1ba) = ScanInt(&ok) != 0;  // +0x1ba
    *(int*)((char*)script+0x1bb) = ScanInt(&ok) != 0;  // +0x1bb
    break;

case 0x2238:  // "[cooltime maintenance]"
    *(char*)((char*)script+0x1bc) = ScanInt(&ok) != 0;  // +0x1bc
    *(int*)((char*)script+0x1bd) = ScanInt(&ok) != 0;  // +0x1bd
    *(int*)((char*)script+0x1be) = ScanInt(&ok) != 0;  // +0x1be
    script->m_1bf = ScanInt(&ok) != 0;  // +0x1bf
    break;

case 0x223e:  // "[effect maintenance]"
    ScanInt(&ok);  // 局部用途 @89b2c5f
    break;

case 0x2243:  // "[shared effect]"
    ScanFloat(&ok);  // @89b2cff
    break;

case 0x2248:  // "[exp bonus rate]"
    ScanFloat(&ok);  // @89b2d80
    break;

case 0x224d:  // "[pc exp bonus rate]"
    ScanStr(&script->m_str6c);  // +0x6c
    script->m_field70 = ScanInt(&ok);  // +0x70
    break;

case 0x2252:  // "[icon]"
    ScanStr(&script->m_str7c);  // +0x7c
    script->m_field80 = ScanInt(&ok);  // +0x80
    break;

case 0x2257:  // "[tarotcard view]"
    ScanStr(&script->m_str74);  // +0x74
    script->m_field78 = ScanInt(&ok);  // +0x78
    break;

case 0x225b:  // "[field image]"
    script->m_field14 = ScanInt(&ok);  // +0x14
    break;

case 0x225e:  // "[price]"
    script->m_field24 = ScanInt(&ok);  // +0x24
    break;

case 0x2261:  // "[price]"
    script->m_field20 = ScanInt(&ok);  // +0x20
    break;

case 0x2265:  // "[value]"
    *(int*)((char*)script+0x4ec) = ScanInt(&ok);  // +0x4ec
    break;

case 0x2268:  // "[finish point price]"
    ScanStr(&script->m_str1d8);  // +0x1d8
    script->m_1dc = ScanInt(&ok);  // +0x1dc
    break;

case 0x226d:  // "[disable icon]"
    script->m_pair28.first = ScanInt(&ok);  // +0x28
    *(int*)((char*)script+0x2c) = ScanInt(&ok);  // +0x2c
    break;

case 0x2272:  // "[need material]"
    ScanStr(&script->m_str174);  // +0x174
    break;

case 0x2276:  // "[recieve postal message]"
    script->m_1a8 = ScanInt(&ok);  // +0x1a8
    break;

case 0x2279:  // "[recieve postal message]"
    script->m_field18 = ScanInt(&ok);  // +0x18
    break;

case 0x227f:  // "[cash]"
    script->m_field1c = ScanInt(&ok);  // +0x1c
    break;

case 0x2288:  // "[auction free]"
    ScanInt(&ok);  // 局部用途 @89b349a
    break;

case 0x228c:  // "[medal]"
    ScanStr(&script->m_str84);  // +0x84
    break;

case 0x228f:  // "[usable upgrade chracter level]"
    ScanStr(&script->m_str238);  // +0x238
    break;

case 0x2292:  // "[move wav]"
    ScanInt(&ok);  // 局部用途 @89b360a
    break;

case 0x229f:  // "[int data]"
    ScanInt(&ok);  // 局部用途 @89b372f
    ScanInt(&ok);  // 局部用途 @89b37e1
    ScanInt(&ok);  // 局部用途 @89b3844
    break;

case 0x2341:  // "[default list]"
    ScanStr(&local_str);  // 局部 string @89b3918
    break;

case 0x2347:  // "[string data]"
    script->m_1d4 = ScanInt(&ok);  // +0x1d4
    break;

case 0x234a:  // "[applying range]"
    script->m_364 = ScanInt(&ok);  // +0x364
    break;

case 0x2352:  // "[applying range]"
    script->m_fieldfc = ScanInt(&ok) != 0;  // +0xfc
    break;

case 0x2355:  // "[stack limit]"
    script->m_23c = ScanInt(&ok);  // +0x23c
    break;

case 0x2358:  // "[packagable]"
    script->m_240 = ScanInt(&ok);  // +0x240
    break;

case 0x235b:  // "[creature minimum level]"
    ScanStr(&local_str);  // 局部 string @89b3be4
    break;

case 0x2368:  // "[icon mark]"
    script->m_fieldf4 = ScanInt(&ok);  // +0xf4
    break;

case 0x236b:  // "[usable period]"
    script->m_fieldf8 = ScanInt(&ok);  // +0xf8
    break;

case 0x237a:  // "[usable period]"
    ScanInt(&ok);  // 局部用途 @89b3d8d
    ScanInt(&ok);  // 局部用途 @89b3dcd
    break;

case 0x238a:  // "[package data]"
    ScanInt(&ok);  // 局部用途 @89b3e6e
    ScanInt(&ok);  // 局部用途 @89b3eae
    break;

case 0x239a:  // "[secret add item]"
    ScanInt(&ok);  // 局部用途 @89b3f4d
    ScanInt(&ok);  // 局部用途 @89b3f87
    break;

case 0x2497:  // "[booster select category]"
    ScanInt(&ok);  // 局部用途 @89b4518
    ScanInt(&ok);  // 局部用途 @89b45b9
    ScanInt(&ok);  // 局部用途 @89b4663
    ScanInt(&ok);  // 局部用途 @89b4689
    ScanInt(&ok);  // 局部用途 @89b46b3
    ScanInt(&ok);  // 局部用途 @89b46c7
    break;

case 0x250e:  // "[booster category num]"
    ScanInt(&ok);  // 局部用途 @89b49fe
    ScanInt(&ok);  // 局部用途 @89b4a31
    ScanInt(&ok);  // 局部用途 @89b4a5a
    *(char*)((char*)script+0x41c) = 0;
    *(int*)((char*)script+0x420) = 0x0;
    *(int*)((char*)script+0x420) = 0x0;
    break;

case 0x2532:  // "[booster selection num]"
    ScanInt(&ok);  // 局部用途 @89b4b4a
    break;

case 0x253c:  // "[booster selection num]"
    ScanInt(&ok);  // 局部用途 @89b4bdb
    break;

case 0x2545:  // "[equipment upgrade]"
    ScanStr(&local_str);  // 局部 string @89b4c8d
    break;

case 0x2553:  // "[hp recovery]"
    ScanStr(&local_str);  // 局部 string @89b4df8
    *(int*)((char*)script+0x2b4) = ScanInt(&ok);  // +0x2b4
    *(int*)((char*)script+0x2b8) = ScanInt(&ok);  // +0x2b8
    ScanStr(&local_str);  // 局部 string @89b4e60
    *(int*)((char*)script+0x2bc) = 0x0;
    *(int*)((char*)script+0x2bc) = 0x1;
    break;

case 0x2593:  // "myself"
    ScanStr(&local_str);  // 局部 string @89b4f4a
    *(int*)((char*)script+0x2c8) = ScanInt(&ok);  // +0x2c8
    *(int*)((char*)script+0x2cc) = ScanInt(&ok);  // +0x2cc
    ScanStr(&local_str);  // 局部 string @89b4fb2
    *(int*)((char*)script+0x2d0) = 0x0;
    *(int*)((char*)script+0x2d0) = 0x1;
    break;

case 0x25b1:  // "myself"
    ScanStr(&local_str);  // 局部 string @89b509c
    script->m_statusRecovery2d8.m_field8 = ScanInt(&ok);  // +0x2e0
    ScanStr(&local_str);  // 局部 string @89b50d6
    script->m_statusRecovery2d8.m_fieldC = 0x0;
    script->m_statusRecovery2d8.m_fieldC = 0x1;
    break;

case 0x25c6:  // "myself"
    ScanStr(&local_str);  // 局部 string @89b51c0
    script->m_statusRecovery2d8.m_field8 = ScanInt(&ok);  // +0x2e0
    ScanStr(&local_str);  // 局部 string @89b51fa
    script->m_statusRecovery2d8.m_fieldC = 0x0;
    script->m_statusRecovery2d8.m_fieldC = 0x1;
    break;

case 0x2611:  // "blind"
    ScanFloat(&ok);  // @89b56d9
    ScanFloat(&ok);  // @89b5706
    break;

case 0x2615:  // "blind"
    ScanFloat(&ok);  // @89b5795
    ScanFloat(&ok);  // @89b57c2
    break;

case 0x2619:  // "slow"
    ScanFloat(&ok);  // @89b5851
    ScanFloat(&ok);  // @89b587e
    break;

case 0x261d:  // "haste"
    ScanFloat(&ok);  // @89b590d
    break;

case 0x2620:  // "stone"
    ScanFloat(&ok);  // @89b599c
    break;

case 0x2623:  // "confuse"
    ScanFloat(&ok);  // @89b5a2b
    break;

case 0x2626:  // "weakness"
    ScanFloat(&ok);  // @89b5aba
    break;

case 0x2629:  // "armor break"
    ScanFloat(&ok);  // @89b5b49
    break;

case 0x262c:  // "weapon break"
    ScanFloat(&ok);  // @89b5bd8
    break;

case 0x262f:  // "poison"
    ScanFloat(&ok);  // @89b5c67
    break;

case 0x2632:  // "lightning"
    ScanFloat(&ok);  // @89b5cf6
    break;

case 0x2635:  // "bleeding"
    ScanFloat(&ok);  // @89b5d88
    ScanFloat(&ok);  // @89b5db5
    ScanFloat(&ok);  // @89b5de2
    ScanFloat(&ok);  // @89b5e0f
    break;

case 0x263b:  // "bless"
    ScanFloat(&ok);  // @89b5ea7
    break;

case 0x263f:  // "bless"
    ScanFloat(&ok);  // @89b5f4e
    break;

case 0x2646:  // "[buff]"
    ScanStr(&local_str);  // 局部 string @89b604f
    script->m_buff304.m_field4 = ScanInt(&ok);  // +0x308
    ScanStr(&local_str);  // 局部 string @89b608b
    script->m_buff304.m_field8 = 0x0;
    script->m_buff304.m_field8 = 0x1;
    break;

case 0x265a:  // "myself"
    ScanInt(&ok);  // 局部用途 @89b61b9
    ScanStr(&local_str);  // 局部 string @89b61cd
    ScanStr(&local_str);  // 局部 string @89b6231
    break;

case 0x267b:  // "[stat change duration]"
    *(int*)((char*)script+0x358) = ScanInt(&ok);  // +0x358
    ScanStr(&local_str);  // 局部 string @89b62fa
    *(int*)((char*)script+0x35c) = 0x0;
    *(int*)((char*)script+0x35c) = 0x1;
    break;

case 0x268c:  // "myself"
    script->m_summonMonster314.m_field0 = ScanInt(&ok);  // +0x314
    *(int*)((char*)script+0x318) = ScanInt(&ok);  // +0x318
    *(int*)((char*)script+0x31c) = ScanInt(&ok);  // +0x31c
    *(int*)((char*)script+0x320) = ScanInt(&ok);  // +0x320
    *(int*)((char*)script+0x324) = ScanInt(&ok);  // +0x324
    break;

case 0x2696:  // "[monster summons]"
    script->m_summonMonster314.m_field0 = ScanInt(&ok);  // +0x314
    *(int*)((char*)script+0x318) = ScanInt(&ok);  // +0x318
    *(int*)((char*)script+0x31c) = ScanInt(&ok);  // +0x31c
    *(int*)((char*)script+0x320) = ScanInt(&ok);  // +0x320
    *(int*)((char*)script+0x324) = ScanInt(&ok);  // +0x324
    *(int*)((char*)script+0x328) = ScanInt(&ok);  // +0x328
    break;

case 0x26a1:  // "[event monster summons]"
    script->m_summonApc330.m_field0 = ScanInt(&ok);  // +0x330
    *(int*)((char*)script+0x334) = ScanInt(&ok);  // +0x334
    *(int*)((char*)script+0x338) = ScanInt(&ok);  // +0x338
    *(int*)((char*)script+0x33c) = ScanInt(&ok);  // +0x33c
    *(int*)((char*)script+0x340) = ScanInt(&ok);  // +0x340
    break;

case 0x26bd:  // "[summon apc]"
    script->m_348 = ScanInt(&ok);  // +0x348
    break;

case 0x26d3:  // "[/enchant]"
    script->m_414 = 0x2;
    break;

case 0x26ed:  // "[avatar package preview info]"
    ScanStr(&script->m_str368);  // +0x368
    script->m_36c = ScanInt(&ok);  // +0x36c
    break;

case 0x26f2:  // "[avatar package preview info]"
    script->m_370 = ScanInt(&ok);  // +0x370
    break;

case 0x26f6:  // "[creature preview info]"
    ScanInt(&ok);  // 局部用途 @89b6e6c
    ScanInt(&ok);  // 局部用途 @89b6e90
    break;

case 0x2704:  // "[avatar emblem target type]"
    ScanStr(&local_str);  // 局部 string @89b6f58
    break;

case 0x2711:  // "[avatar emblem target type]"
    ScanInt(&ok);  // 局部用途 @89b7015
    ScanInt(&ok);  // 局部用途 @89b7021
    break;

case 0x271a:  // "[usable date]"
    ScanStr(&local_str);  // 局部 string @89b7094
    break;

case 0x2729:  // "../RDARScriptItem.cpp"
    ScanInt(&ok);  // 局部用途 @89b7253
    break;

case 0x272e:  // "[npc gift disallowance]"
    script->m_fieldc8 = ScanInt(&ok);  // +0xc8
    break;

case 0x2731:  // "[guild power war point]"
    script->m_fieldcc = ScanInt(&ok);  // +0xcc
    break;

case 0x2734:  // "[guild power war point]"
    script->m_384 = ScanInt(&ok);  // +0x384
    break;

case 0x2737:  // "[user power war point]"
    ScanStr(&local_str);  // 局部 string @89b74fa
    break;

case 0x276b:  // "[disable]"
    script->m_38c = ScanInt(&ok);  // +0x38c
    break;

case 0x276e:  // "[disable]"
    script->m_390 = ScanInt(&ok);  // +0x390
    break;

case 0x2771:  // "[use item emoticon index]"
    ScanStr(&local_str);  // 局部 string @89b77a0
    script->m_3bc = 0x2;
    script->m_3bc = 0x1;
    script->m_3bc = 0x4;
    script->m_3bc = 0x0;
    break;

case 0x277d:  // "[alchemist]"
    ScanInt(&ok);  // 局部用途 @89b78a4
    ScanInt(&ok);  // 局部用途 @89b78c8
    break;

case 0x278c:  // "[monster card id]"
    script->m_3cc = ScanInt(&ok);  // +0x3cc
    break;

case 0x2795:  // "[dungeon type for extra drop]"
    ScanInt(&ok);  // 局部用途 @89b7bf5
    ScanStr(&local_str);  // 局部 string @89b7c7f
    break;

case 0x27ba:  // "[use 2nd upgrade table]"
    script->m_1ac = ScanInt(&ok);  // +0x1ac
    break;

case 0x27c0:  // "[do not delete]"
    ScanStr(&script->m_arad178.m_str8);  // +0x180
    break;

case 0x27c4:  // "[arad_item_kind]"
    ScanInt(&ok);  // 局部用途 @89b7f1b
    break;

case 0x27c9:  // "[usable first growtype]"
    script->m_arad178.m_0 = ScanInt(&ok);  // +0x178
    break;

case 0x27cd:  // "[arad_use]"
    ScanStr(&script->m_arad178.m_strc);  // +0x184
    break;

case 0x27d0:  // "[mail from]"
    ScanStr(&script->m_arad178.m_str10);  // +0x188
    break;

case 0x27d7:  // "[mail content]"
    ScanInt(&ok);  // 局部用途 @89b8141
    ScanStr(&script->m_str410);  // +0x410
    break;

case 0x27e1:  // "[onemore check]"
    script->m_418 = ScanInt(&ok);  // +0x418
    break;

case 0x2802:  // "[evolution creature]"
    ScanInt(&ok);  // 局部用途 @89b82b1
    ScanInt(&ok);  // 局部用途 @89b82e2
    ScanInt(&ok);  // 局部用途 @89b8394
    ScanInt(&ok);  // 局部用途 @89b83c5
    ScanStr(&local_str);  // 局部 string @89b8404
    ScanInt(&ok);  // 局部用途 @89b8486
    ScanStr(&local_str);  // 局部 string @89b851c
    break;

case 0x284a:  // "[type]"
    ScanInt(&ok);  // 局部用途 @89b871d
    break;

case 0x2850:  // "[type]"
    ScanInt(&ok);  // 局部用途 @89b8801
    ScanInt(&ok);  // 局部用途 @89b8823
    ScanInt(&ok);  // 局部用途 @89b88a5
    ScanInt(&ok);  // 局部用途 @89b88c8
    ScanInt(&ok);  // 局部用途 @89b8921
    ScanInt(&ok);  // 局部用途 @89b8944
    ScanInt(&ok);  // 局部用途 @89b8963
    break;

case 0x28ee:  // "[prof compound rate]"
    script->m_44c = ScanInt(&ok);  // +0x44c
    break;

case 0x28f3:  // "[portable disjoint]"
    script->m_450 = ScanInt(&ok);  // +0x450
    break;

case 0x28f8:  // "[shop doll]"
    script->m_454 = ScanInt(&ok);  // +0x454
    break;

case 0x28fd:  // "[unseal scroll level limit]"
    script->m_458 = ScanInt(&ok);  // +0x458
    break;

case 0x2902:  // "[monstercard bind]"
    script->m_45c = ScanInt(&ok);  // +0x45c
    break;

case 0x2907:  // "[alchemist extraction]"
    script->m_460 = ScanInt(&ok);  // +0x460
    break;

case 0x290c:  // "[enchanter extraction]"
    *(int*)((char*)script+0x464) = ScanInt(&ok);  // +0x464
    break;

case 0x2912:  // "[doll_controller extraction]"
    *(int*)((char*)script+0x470) = ScanInt(&ok);  // +0x470
    break;

case 0x291e:  // "[item category]"
    *(int*)((char*)script+0x468) = ScanInt(&ok);  // +0x468
    break;

case 0x2924:  // "[usable game server type]"
    *(int*)((char*)script+0x46c) = ScanInt(&ok);  // +0x46c
    break;

case 0x292b:  // "[levelup range]"
    ScanInt(&ok);  // 局部用途 @89b9b18
    break;

case 0x2932:  // "[lottery use cost]"
    ScanInt(&ok);  // 局部用途 @89b9baa
    *(int*)((char*)script+0x478) = ScanInt(&ok);  // +0x478
    break;

case 0x293e:  // "[/chat notice gold]"
    ScanStr(&local_str);  // 局部 string @89b9cd3
    ScanStr(&local_str);  // 局部 string @89b9d5b
    ScanInt(&ok);  // 局部用途 @89b9de3
    break;

case 0x2962:  // "[all]"
    ScanInt(&ok);  // 局部用途 @89b9eb9
    break;

case 0x2975:  // "../RDARScriptItem.cpp"
    ScanInt(&ok);  // 局部用途 @89b9fd0
    break;

case 0x2989:  // "../RDARScriptItem.cpp"
    ScanInt(&ok);  // 局部用途 @89ba0d5
    break;

case 0x2993:  // "../RDARScriptItem.cpp"
    ScanInt(&ok);  // 局部用途 @89ba185
    ScanInt(&ok);  // 局部用途 @89ba1b8
    break;

case 0x29a9:  // "[random option unseal]"
    *(int*)((char*)script+0x4b8) = 0x0;
    *(int*)((char*)script+0x4b8) = ScanInt(&ok);  // +0x4b8
    break;

case 0x29ae:  // "[random option unseal]"
    ScanStr(&local_str);  // 局部 string @89ba324
    break;

case 0x29bb:  // "[success rate]"
    *(int*)((char*)script+0x4f0) = ScanInt(&ok);  // +0x4f0
    break;

case 0x29bf:  // "[success rate]"
    *(int*)((char*)script+0x4f4) = ScanInt(&ok);  // +0x4f4
    break;

case 0x29c3:  // "[success rate]"
    *(int*)((char*)script+0x4f8) = ScanInt(&ok);  // +0x4f8
    break;

case 0x29c7:  // "[broadcast type]"
    ScanInt(&ok);  // 局部用途 @89ba575
    break;

case 0x29d5:  // "[fail output]"
    ScanInt(&ok);  // 局部用途 @89ba617
    break;

case 0x29df:  // "[trade limit max]"
    ScanInt(&ok);  // 局部用途 @89ba6ab
    break;
        default:
            break;
        }
        if (flag && !script->m_strac.empty())
        {
            break;  // ORIG flag 语义：[name] 读入后提前结束
        }
    }
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
