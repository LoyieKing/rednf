// ============================================================================
// df_game_r 还原 —— STStackableScript（G5 堆叠物脚本表）
// Clear 0x0899087a / ctor 0x0898f218 / copy 0x0898f8e6 / dtor 0x0899013c /
// operator= 0x089c418e；Arad_STStackableScript 0x089c33fa..0x089c4052。
// 子对象（STStatusRecovery/STBuff/STSummonMonster/STSummonApc/
// CountinuousItemOption/AvatarOptionChangeType）与 STStackableScript 同 TU
// （ORIG 0x089c30dc..0x089c34e8 弱符号紧邻），方法实现于此。
// ============================================================================
// b28d-45 判词汇总（本 TU 4 件）：
//   STStackableScript::Clear 0x0899087a DIFF190 N82/N89：322:320 基类 Clear
//     转发 + AradStClear 调用点显示差。
//   STStackableScript::operator= 0x089c418e DIFF379 N84/N82：621:627 基类
//     operator= 转发 + Arad_STStackableScript::operator= 槽序。
//   STStackableScript::C1ERKS_ 0x0898f218 DIFF434 N84/N82：643:647 拷贝构造链。
//   ImportStackableScript 0x089aeaf4 DIFF9936 N89/N82：10096:10147 万行级
//     解析件，§6.2 三件套判语义完成、byte 层封顶。

#include "STStackableScript.h"
#include "CStackableItem.h"  // RecipeInfo 权威布局（0x4c，ORIG 0x8513c6a 族）
#include "GameScriptEnumHelpers.h"   // getActiveStatusToEnumValue（my status）
#include "EquipmentParameterInfo.h"  // importEquipmentParameterInfo / m_414（enchant）
#include "CCubeStatistic.h"            // getItemGroupNameTable（item group name）
#include "LogManager.h"                // ImportStackableScript 错误日志
#include "CEnvironment.h"              // SpinLock / less_TCHAR（ImportStackableScript 六张静态标签表）

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

// [b28d-solo-STStackableScript] NEAR（orig=643 our=643，ndiff 44 全为块内
// 迭代器槽号分配；p167 仅 STItemScript C2/D2 vs D1 同址别名伪差（P93）。
// 本轮修正：for 布局 + src_pair 逐成员赋值（p.first/p.second）+ insert 传值对。
STStackableScript::STStackableScript(STStackableScript& other)
{
    operator=(other);
    if (!other.m_module4bc.empty())
    {
        // ORIG 0x0898fbf7..0x0898fd68：4.1 for 布局（begin 后 jmp 尾条件、每次
        // 迭代重调 other.end()）；循环体内 dst_end=end() → it->first(ptEv) →
        // find → ne(found,dst_end)；body 为 src_pair 逐成员赋值
        // （src_pair.first=it->first；src_pair.second=new ...；*second=*it->second）。
        // [b28d-solo-STStackableScript] N93/P212 封顶：ORIG 槽序（4.1 块内先声明
        // 深）it(-0x38) > dst_end(-0x30) > found(-0x2c)；4.4 块内先声明浅
        // （dst_end -0x34 / found -0x38，方向相反）。三种改写实测：
        // ①三迭代器提函数头：+3 C1Ev call、帧 +0x10（P50 反例）；
        // ②found/dst_end 提 for 前：+2 C1Ev call、帧 +0x10（ndiff 24 但引入
        //   EXTRA 调用）；③循环体内声明（本形）：orig=643 our=643、无额外 call、
        //   残差仅槽号，ndiff 44 NEAR ⇒ 取 ③，p167 无缺口。
        for (std::map<ENUM_MODULE_TYPE, EquipmentParameterInfo*>::iterator it =
                 other.m_module4bc.begin();
             it != other.m_module4bc.end(); ++it)
        {
            std::map<ENUM_MODULE_TYPE, EquipmentParameterInfo*>::iterator dst_end = m_module4bc.end();
            std::map<ENUM_MODULE_TYPE, EquipmentParameterInfo*>::iterator found = m_module4bc.find(it->first);
            if (found != dst_end)
            {
                std::pair<ENUM_MODULE_TYPE, EquipmentParameterInfo*> src_pair;
                src_pair.first = it->first;
                src_pair.second = new EquipmentParameterInfo();
                *src_pair.second = *it->second;
                m_module4bc.insert(src_pair);
            }
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
                delete it->second;
            it->second = 0;   // ORIG +0x79：赋 0 在 if 之外（两次 je 同落 +0x79）
        }
        m_module4bc.clear();
    }
}

// ORIG 0x089c418e：无 self 检查；基类 op= → arad op= → +0x18c →
// 依序拷贝各容器/string/标量；STRecovery×2/STStatusRecovery 与
// STBuff/STSummonMonster/STSummonApc 三个平凡区用 memcpy（ORIG call memcpy@plt）。
// [b28d-solo-STStackableScript] DIFF→N-ptrfold 封顶（621 vs 627：6 处
// self 守卫 4.4 折成 `this != &other` 且 memcpy 实参重算地址；p167 CLEAR 43=43）。
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
    // ORIG 对 6 个平凡区 memcpy 均带 self 守卫（cmp this+off vs other+off; je 跳过）。
    // [b28d-solo-STStackableScript] N-ptrfold：4.4 把 `&a->f != &b->f`（同偏移）
    // 折成 `a != b`，且 memcpy 实参重算地址；ORIG 4.1 保留两次 add+cmp 并复用
    // 地址寄存器跨 call。整数化守卫（(int)&f）可阻 fold 但 memcpy 段仍重算
    // （探针 /tmp/probe_guard.cpp 四形实证：原形 +6、整数化 +30）⇒ 封顶。
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
// [b28d-solo-STStackableScript] AE（322=322）：+0x1c4/+0x1cc 两处
// `mov $0,%edx; mov %edx,disp` 常量中转 = P238 float 字段形（m_1c4/m_1cc
// 改 float + 0.0f；同时修真语义 bug：ImportStackableScript 写 ScanFloat）。
void STStackableScript::Clear()
{
    // [b28c-27] N18a：+0x1c4/+0x1cc 两处 ORIG `mov $0x0,%edx; mov %edx,disp`
    // （4.1 常量经寄存器中转）；字段 unsigned int 化后 4.4 仍 `movl $0,disp` 直写，
    // 不可翻。其余为 .o 重定位伪差（"\"\" 字面量 → <A>，AE 口径）。
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
    m_1c4 = 0.0f;
    m_1c8 = 0;
    m_1cc = 0.0f;
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
    // i 声明 unsigned（P209：4.4 对 unsigned 局部比较恒 mem 直比 `cmp -0xc,%eax`；
    // int+cast 恒先载 edx，+2 条）。
    unsigned int i = 0;
    // ORIG 槽序：i(-0xc) 最浅、pair 临时(-0x14)、first(-0x18)、second(-0x1c)
    // ⇒ first/second 提到函数头声明（P212 全头部声明；块内仅赋值）。
    int first;
    int second;
    while (script.m_vec4fc.size() > i)
    {
        first = script.m_vec4fc[i++];
        if (script.m_vec4fc.size() <= i)
        {
            break;
        }
        second = script.m_vec4fc[i++];
        // ORIG 调 push_back(OS1_)（右值重载）⇒ 源形为临时 pair 直传。
        m_vec40.push_back(std::pair<int, int>(first, second));
    }
}

// ---- ORIG 全局（定义 InitScriptProducers_4.cpp）：脚本目录与文件列表 ----
extern char g_szStackableScriptBaseDirectory[0x100];
extern std::map<int, std::string> g_stStackableScriptFileList;

// ---- ORIG TU-local 静态 helper（ORIG 与 ImportStackableScript 同 TU：
//      RDARScriptItem.cpp，ORIG t 0x089bf6ca/0x089bfc2f/0x089bfc84/0x089bfdc5）。
//      重建按 TU 拆分，本 TU 需自有副本（mangled _ZL… 内部链接）。----
static bool GetItemCategory(STItemScript* item);
static bool GetExpertJobTypeList(STItemScript* item);
static bool GetExpertJobValueMaterialVariation(STItemScript* item);
static bool GetExpertJobValueCompoundRate(STItemScript* item);

static bool GetItemCategory(STItemScript* item)
{
    static std::map<const char*, int, less_TCHAR> __lookup;
    static SpinLock __init_lock;
    static bool __inited = false;

    std::string token;
    while (ScanStr(&token))
    {
        if (!__inited)
        {
            __init_lock.enter();
            if (!__inited)
            {
                // ORIG 首轮 case 链依次插入三项，随后重查当前 token。
                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("monster card", 0x30ae));
                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("cannot bind", 0x30b1));
                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("boss drop", 0x30b4));
                __inited = true;
            }
            __init_lock.leave();
        }

        int state = 0;
        std::map<const char*, int, less_TCHAR>::const_iterator it =
            __lookup.find(token.c_str());
        switch (it != __lookup.end() ? it->second : -1)
        {
        case 0x30ae:                                   // "monster card"
            state = STItemScript::ITEM_CATEGORY_1;
            break;
        case 0x30b1:                                   // "cannot bind"
            state = STItemScript::ITEM_CATEGORY_2;
            break;
        case 0x30b4:                                   // "boss drop"
            state = STItemScript::ITEM_CATEGORY_3;
            break;
        default:
            return false;                              // 未命中 → 导入失败
        }
        item->m_set11c.insert(
            static_cast<STItemScript::ENUM_ITEM_CATEGORY>(state));
    }
    return true;
}

static bool GetExpertJobTypeList(STItemScript* item)
{
    if (item == 0)
        return false;
    item->m_expertJob.m_usable.clear();

    static std::map<const char*, int, less_TCHAR> __lookup;   // 0x94fde48
    static SpinLock __init_lock;                              // 0x94fde60
    static bool __inited = false;                             // 0x94fde64

    std::string token;
    bool scanning = true;
    while (scanning)
    {
        int typeSel = 0;
        if (!ScanStr(&token))
            break;

        // 首个 token 惰性登记全部关键字映射（锁内连续 insert），随后重查本 token。
        if (!__inited)
        {
            __init_lock.enter();
            if (!__inited)
            {
                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("enchanter", 0x303a));
                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("alchemist", 0x303d));
                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("disjointer", 0x3040));
                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("doll_controller", 0x3043));
                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("all", 0x3046));
                __inited = true;
            }
            __init_lock.leave();

            typeSel = 0;
            std::map<const char*, int, less_TCHAR>::const_iterator found =
                __lookup.find(token.c_str());
            if (found != __lookup.end())
                typeSel = found->second;
        }
        else
        {
            std::map<const char*, int, less_TCHAR>::const_iterator it =
                __lookup.find(token.c_str());
            if (it == __lookup.end())
                return false;
            switch (it->second)
            {
            case 0x303a: typeSel = 1; break;   // enchanter
            case 0x303d: typeSel = 2; break;   // alchemist
            case 0x3040: typeSel = 3; break;   // disjointer
            case 0x3043: typeSel = 4; break;   // doll_controller
            case 0x3046: typeSel = 5; break;   // all
            default: return false;
            }
        }

        if (typeSel != 0)
        {
            int level = 0;
            if (!ScanInt(&level))
                return false;
            STItemScript::SEXPERTJOB::SUSABLE entry;
            entry.m_field0 = static_cast<ENUM_EXPERT_JOB_TYPE>(typeSel);
            entry.m_field4 = level;
            item->m_expertJob.m_usable.push_back(entry);
        }
    }
    return !item->m_expertJob.m_usable.empty();
}

// ORIG t 0x089bfc2f：-100.0 哨兵（0x8e1a574）视为未设置。
static bool GetExpertJobValueMaterialVariation(STItemScript* item)
{
    if (item == 0)
        return false;
    bool ok = true;
    item->m_expertJob.m_field18 = ScanFloat(&ok);
    if (item->m_expertJob.m_field18 == -100.0f)
        return false;
    return ok;
}

// ORIG t 0x089bfc84：仅透传 ScanFloat 成败。
static bool GetExpertJobValueCompoundRate(STItemScript* item)
{
    if (item == 0)
        return false;
    bool ok = true;
    item->m_expertJob.m_field1c = ScanFloat(&ok);
    return ok;
}


// [b28d-solo-STStackableScript] DIFF→§6.2 三件套语义完成、byte 层 N89/N82 封顶：
// ①p167 调用点差集 CLEAR（1174=1174，0 missing / 0 extra；本轮修三处语义缺口：
//   [enchant] 段独立 static 三件套 __lookup_3/__init_lock_3/__inited_3（原复用
//   主表，ORIG 089b6862 实证 6 组 static 而非 5 组）、[skill explain] 只比较一次
//   （删多余 `if (!(line==...)) return false;`，ORIG 089b85d4 无该分支）、
//   STStatChange/stLimitCubeResult 删用户空析构（P317，消 EXTRA D1 ×4）；
// ②gap −49（orig 10197 / our 10246）；③槽位/帧差属 N82/N89（4.1 不按 lifetime
//   复用槽 + libstdc++ 全内联），万行级解析件不再逐条强凑。
bool ImportStackableScript(STStackableScript* script, const char* path,
                           bool flag, int idx)
{
    if (!loadRDARScriptFile(g_szStackableScriptBaseDirectory, path))
    {
        return false;
    }
    script->Clear();
    std::string line;
    std::string token;
    int vzero = 0;   // ORIG -0x144c：函数级 int 暂存槽，顶部 movl $0 初始化
    (void)vzero;
    bool ok = false; // ORIG -0x144d：顶部唯一一次 movb $0 清零

    // ORIG 0x089aeec8..0x089ba83b（10197 指令）结构（docs 报告 ImportStackableScript.md
    // 汇编 16..10217 / 反编译 C 10218..17101）：
    //   while (ScanType(line,1)) {
    //     if (line=="[name]") ScanStr(&m_strac(+0xac)); flag 置位则立即跳出循环（
    //       jmp +0xb8cd 收尾）；
    //     else if (line=="[emancipate ticket]") m_field10e=1 后 ScanInt 写 +0x394(short)，
    //       失败仅继续循环；                    [+0x394.. = m_394 ushort]
    //     else if (line=="[equipment reinforcement ticket]") m_396=1、ScanInt(+ok)→m_398、
    //       再 ScanInt→+0x39a 字节；任一 !ok → return false（mov $0,%ebx 出口）；
    //     else if (line=="[check usable itemlevel]") 两 ScanInt 写 +0x43c/+0x440，
    //       任一 !ok → return false；ok 后 m_438=1；
    //     else if (line=="[passive object in stackable]") 内层循环连读 4 int（field0..C）
    //       push 到 m_passive208(+0x208)，行首失败跳出；
    //     else if (line=="[equipment amplify reinforcement ticket]") m_39b=1、ScanInt→m_39c、
    //       ScanInt→+0x3a0 字节；任一 !ok → return false；
    //     else if (line=="[amplification random value]") m_3a1=1 后循环 {ScanInt v1(v2)；
    //       !ok 跳出；pair(v1,v2) insert m_map3a4}；
    //     else if (line=="[/amplification random value]") 空（继续循环）；
    //     else 主分发：静态 __lookup 惰性填充（[name2]..[attach type] 等 157 主标签
    //       → 下述 hash，case 顺序即 hash 升序填充链）后 find(line.c_str())，switch 分派；
    //       命中失败（无 case）→ 089ba768 桩（continue）。
    //   每行处理完毕都经公共桩 089ba768 → +0x80 回 ScanType 循环。
    // 扫描结束（ScanType false）→ +0xb8cd 收尾：if(m_field24==-1) m_field24=m_field14；
    //   idx==-1 → m_field0=FindIndexByFullScan(path) else m_field0=idx；返回 true。
    // 转写形态（本轮三步法 Step2 重构，ORIG 证据同上报告）：
    //   主表 + 4 张子表(attach/stackable-type/my-status/itemmaking)均为
    //   for(;;){ find-或-双检锁 → switch(tag){ case V: if(!__inited){单条
    //   value_type 惰性 insert} else {work; break;} } → if(__inited)break;
    //   leave;置位 } 的**逐例 guard 链**（非集中批量 insert）；case 0 与首个
    //   case 共叶、子表首 case 亦然；[enchant] 内层循环复用主表 __lookup
    //   （[/enchant]/[pvp] 作为主表条目在循环内 guard 链登记）。
    //   gcc4.4.7 对 if(!__inited){insert} else {work;break} + case 贯穿生成的
    //   块布局（check:xor/test/je → insert 块 → jmp 下叶 → work 块）与 ORIG
    //   逐条同形（探针验证，见 docs/asm_translation_patterns 新范式提报）。
    while (ScanType(line, true))
    {
        if (line == "[name]")
        {
            ScanStr(&script->m_strac);  // +0xac（ORIG +0x3c..+0x46 ScanStr(&+0xac)）
            if (flag)
            {
                break;  // ORIG 089aef9a：flag → jmp +0xb8cd 收尾
            }
        }
        else if (line == "[emancipate ticket]")
        {
            // ORIG 089aefbb..089aeff1：movb $1,0x10e；ScanInt(&ok)→(short)+0x394；
            // 其后 if(!ok) continue 的物化检查（双 landing pad +0x76/+0x7f，
            // 两向均回主循环——逐指令转写需要保留该语句）。
            script->m_field10e = 1;
            script->m_394 = static_cast<unsigned short>(ScanInt(&ok));
            if (!ok)
            {
                continue;
            }
        }
        else if (line == "[equipment reinforcement ticket]")
        {
            // ORIG 089aeff6..089af080：movb $1,0x396；ScanInt→0x398(short)；
            // ScanInt→0x39a(byte)；!ok 任一 → return false。
            script->m_396 = 1;
            script->m_398 = static_cast<unsigned short>(ScanInt(&ok));
            if (!ok)
            {
                return false;
            }
            script->m_39a = static_cast<char>(ScanInt(&ok));
            if (!ok)
            {
                return false;
            }
        }
        else if (line == "[check usable itemlevel]")
        {
            // ORIG 089af081..089af107：两 ScanInt 写 +0x43c/+0x440；任一失败 return false；
            // 成功 movb $1,0x438。
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
            // ORIG 089af108..089af1ce：局部 STPassiveObjectInStackable 反复 5×ScanInt
            // （+0x00..+0x10；仅每轮首值失败即跳出），成功后 push_back 到 m_passive208
            // （call 089c6f9e = vector::push_back；+0x208 元素 sizeof 0x10）。
            // 089af126：段内局部 bool ok2 先置 true，5 次 ScanInt 全部传 &ok2。
            bool ok2 = true;
            STPassiveObjectInStackable obj;
            for (;;)
            {
                obj.m_field0 = ScanInt(&ok2);
                if (!ok2)
                {
                    break;
                }
                // ORIG 089af163..089af1ad：首值失败才结束本段；后四次
                // ScanInt 的返回值直接写入对象，不检查 ok2（实参仍是 &ok2）。
                obj.m_field4 = ScanInt(&ok2);
                obj.m_field8 = ScanInt(&ok2);
                obj.m_fieldC = ScanInt(&ok2);
                obj.m_field10 = ScanInt(&ok2);
                script->m_passive208.push_back(obj);
            }
        }
        else if (line == "[equipment amplify reinforcement ticket]")
        {
            // ORIG 089af1d3..089af25a：movb $1,0x39b；ScanInt→0x39c；
            // ScanInt→0x3a0(byte)；!ok 任一 return false。
            script->m_39b = 1;
            script->m_39c = ScanInt(&ok);
            if (!ok)
            {
                return false;
            }
            script->m_3a0 = static_cast<char>(ScanInt(&ok));
            if (!ok)
            {
                return false;
            }
        }
        else if (line == "[amplification random value]")
        {
            // ORIG 089af25b..089af34e：movb $1,0x3a1 后循环 {v1=ScanInt(!ok break);
            // v2=ScanInt(!ok break); m_map3a4.insert(pair(v1,v2))}。
            script->m_3a1 = 1;
            for (;;)
            {
                int v1 = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                int v2 = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                script->m_map3a4.insert(std::make_pair(v1, v2));
            }
            // ORIG 089af339..089af34b：循环出口后的 ok 物化检查（两向均 continue）。
            if (!ok)
            {
                continue;
            }
        }
        else if (line == "[/amplification random value]")
        {
            // ORIG 089af350..089af36a：无操作，继续主循环。
        }
        else
        {
    static std::map<char const*, int, less_TCHAR> __lookup;
    static SpinLock __init_lock;
    static bool __inited = false;
    int tag;
    for (;;)
    {
        if (__inited)
        {
            std::map<char const*, int, less_TCHAR>::const_iterator it = __lookup.find(line.c_str());
            tag = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                continue;
            }
            tag = 0;
        }
        switch (tag)
        {
            case 0:
            case 0x2064:  // "[name2]"


                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[name2]", 0x2064));
                }
                else
                {
    ScanStr(&script->m_strb0);
    break;
                }
            case 0x2067:  // "[explain]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[explain]", 0x2067));
                }
                else
                {
    ScanStr(&script->m_strb4);
    break;
                }
            case 0x206b:  // "[explain2]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[explain2]", 0x206b));
                }
                else
                {
    ScanStr(&script->m_strb4);
    break;
                }
            case 0x206f:  // "[flavor text]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[flavor text]", 0x206f));
                }
                else
                {
    ScanStr(&script->m_strb8);
    break;
                }
            case 0x2072:  // "[grade]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[grade]", 0x2072));
                }
                else
                {
    script->m_field4 = ScanInt((bool*)0);
    break;
                }
            case 0x2075:  // "[attach type]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[attach type]", 0x2075));
                }
                else
                {
{
    ScanStr(&token);
    static std::map<char const*, int, less_TCHAR> __lookup_0;
    static SpinLock __init_lock_0;
    static bool __inited_0 = false;
    int sub;
    for (;;)
    {
        if (__inited_0)
        {
            std::map<char const*, int, less_TCHAR>::const_iterator it0 = __lookup_0.find(token.c_str());
            sub = (it0 != __lookup_0.end()) ? it0->second : -1;
        }
        else
        {
            __init_lock_0.enter();
            if (__inited_0)
            {
                __init_lock_0.leave();
                continue;
            }
            sub = 0;
        }
        switch (sub)
        {
            case 0:
            case 0x2079:  // "[free]"
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[free]", 0x2079));
                }
                else
                {
    script->m_field8 = 0x0; break;   // "[free]"
                }
            case 0x207c:  // "[trade]"
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[trade]", 0x207c));
                }
                else
                {
    script->m_field8 = 0x1; break;   // "[trade]"
                }
            case 0x207f:  // "[trade delete]"
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[trade delete]", 0x207f));
                }
                else
                {
    script->m_field8 = 0x2; break;   // "[trade delete]"
                }
            case 0x2082:  // "[sealing]"
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[sealing]", 0x2082));
                }
                else
                {
    script->m_field8 = 0x3; break;   // "[sealing]"
                }
            case 0x2085:  // "[account]"
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[account]", 0x2085));
                }
                else
                {
    script->m_field8 = 0x5; break;   // "[account]"
                }
            case 0x2089:  // "[trade limit]"
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[trade limit]", 0x2089));
                }
                else
                {
    script->m_field8 = 0x7; break;   // "[trade limit]"
                }
            default:
    break;
        }
        if (__inited_0)
        {
            break;
        }
        __init_lock_0.leave();
        __inited_0 = true;
    }
}
break;
                }
            case 0x2090:  // "[creation rate]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[creation rate]", 0x2090));
                }
                else
                {
    script->m_fieldc = ScanInt((bool*)0);
    break;
                }
            case 0x2093:  // "[rarity]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[rarity]", 0x2093));
                }
                else
                {
    script->m_field10 = ScanInt((bool*)0);
    break;
                }
            case 0x2096:  // "[weight]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[weight]", 0x2096));
                }
                else
                {
    script->m_field34 = ScanInt((bool*)0);
    break;
                }
            case 0x2099:  // "[usable job]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[usable job]", 0x2099));
                }
                else
                {
    {
        while (ScanStr(&token))
        {
            const int job = getCharacterJob(token.c_str());
            if (job == 0xb)
            {
                for (int i = 0; i < 0xb; ++i)
                {
                    script->m_usable[i] = 1;
                }
                script->m_fielda0 = 1;
            }
            else
            {
                // ORIG 直接以 getCharacterJob 返回值索引 +0x38；脚本
                // 输入仅允许既有职业名，保留该寻址语义。
                script->m_usable[job] = 1;
            }
        }
    }
    break;
                }
            case 0x20a7:  // "[suitable job]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[suitable job]", 0x20a7));
                }
                else
                {
    {
        script->m_set44.clear();
        while (ScanStr(&token))   // ORIG 089b0908/089b0af1：条件后置的 while 布局
        {
            const int job = getCharacterJob(token.c_str());
            if (job == 0xb)
            {
                for (int j = 0; j <= 0xa; ++j)
                {
                    script->m_set44.insert(j);
                }
            }
            else
            {
                script->m_set44.insert(job);
            }
        }
    }
    break;
                }
            case 0x20b6:  // "[additional item]"
    //  +0x19c 元素）再 3×ScanInt 逐字段后 push_back 到 m_rabbit19c(+0x19c)。
    //  CRabbitBox 字段/接口依据 AradRabbitBox.h；ORIG 先 clear 容器，再以
    // setItemIndex/SetItemCount/SetDropRate 写入三个扫描值并 push 一个元素。
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[additional item]", 0x20b6));
                }
                else
                {
    {
        script->m_rabbit19c.clear();
        CRabbitBox rabbit;
        rabbit.setItemIndex(ScanInt((bool*)0));
        rabbit.SetItemCount(ScanInt((bool*)0));
        rabbit.SetDropRate(ScanInt((bool*)0));
        script->m_rabbit19c.push_back(rabbit);
    }
    break;
                }
            case 0x20c0:  // "[collision item]"
    //  !ok break；push 到 +0x190 m_vec190}。
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[collision item]", 0x20c0));
                }
                else
                {
    {
        script->m_vec190.clear();
        for (;;)
        {
            int v = ScanInt(&ok);
            if (!ok)
            {
                break;
            }
            script->m_vec190.push_back(v);
        }
    }
    break;
                }
            case 0x20cf:  // "[stackable type]"
//  静态子表(40)+双检锁 find(token) → m_1e0(+0x1e0)；公共落点 0x089b2201 恒消费一整数。
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[stackable type]", 0x20cf));
                }
                else
                {
{
    ScanStr(&token);
    script->m_strbc = token;  // +0xbc
    static std::map<char const*, int, less_TCHAR> __lookup_1;
    static SpinLock __init_lock_1;
    static bool __inited_1 = false;
    int sub;
    for (;;)
    {
        if (__inited_1)
        {
            std::map<char const*, int, less_TCHAR>::const_iterator it1 = __lookup_1.find(token.c_str());
            sub = (it1 != __lookup_1.end()) ? it1->second : -1;
        }
        else
        {
            __init_lock_1.enter();
            if (__inited_1)
            {
                __init_lock_1.leave();
                continue;
            }
            sub = 0;
        }
        switch (sub)
        {
            case 0:
            case 0x20d7:  // "[waste]"
                if (!__inited_1)
                {
                    __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("[waste]", 0x20d7));
                }
                else
                {
    script->m_1e0 = 0x0; break;   // "[waste]"
                }
            case 0x20da:  // "[material]"
                if (!__inited_1)
                {
                    __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("[material]", 0x20da));
                }
                else
                {
    script->m_1e0 = 0x1; break;   // "[material]"
                }
            case 0x20dd:  // "[material expert job]"
                if (!__inited_1)
                {
                    __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("[material expert job]", 0x20dd));
                }
                else
                {
    script->m_1e0 = 0x2; break;   // "[material expert job]"
                }
            case 0x20e0:  // "[recipe]"
                if (!__inited_1)
                {
                    __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("[recipe]", 0x20e0));
                }
                else
                {
    script->m_1e0 = 0x3; break;   // "[recipe]"
                }
            case 0x20e3:  // "[throw]"
                if (!__inited_1)
                {
                    __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("[throw]", 0x20e3));
                }
                else
                {
    script->m_1e0 = 0x4; break;   // "[throw]"
                }
            case 0x20e6:  // "[set]"
                if (!__inited_1)
                {
                    __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("[set]", 0x20e6));
                }
                else
                {
    script->m_1e0 = 0x5; break;   // "[set]"
                }
            case 0x20e9:  // "[legacy]"
                if (!__inited_1)
                {
                    __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("[legacy]", 0x20e9));
                }
                else
                {
    script->m_1e0 = 0x6; break;   // "[legacy]"
                }
            case 0x20ec:  // "[stackable legacy]"
                if (!__inited_1)
                {
                    __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("[stackable legacy]", 0x20ec));
                }
                else
                {
    script->m_1e0 = 0x7; break;   // "[stackable legacy]"
                }
            case 0x20ef:  // "[quest]"
                if (!__inited_1)
                {
                    __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("[quest]", 0x20ef));
                }
                else
                {
    script->m_1e0 = 0x8; break;   // "[quest]"
                }
            case 0x20f2:  // "[creature]"
                if (!__inited_1)
                {
                    __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("[creature]", 0x20f2));
                }
                else
                {
    script->m_1e0 = 0x9; break;   // "[creature]"
                }
            case 0x20f5:  // "[feed]"
                if (!__inited_1)
                {
                    __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("[feed]", 0x20f5));
                }
                else
                {
    script->m_1e0 = 0xa; break;   // "[feed]"
                }
            case 0x20f9:  // "[creature expitem]"
                if (!__inited_1)
                {
                    __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("[creature expitem]", 0x20f9));
                }
                else
                {
    script->m_1e0 = 0x23; break;   // "[creature expitem]"
                }
            case 0x20fd:  // "[etc]"
                if (!__inited_1)
                {
                    __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("[etc]", 0x20fd));
                }
                else
                {
    script->m_1e0 = 0xb; break;   // "[etc]"
                }
            case 0x2101:  // "[only effect]"
                if (!__inited_1)
                {
                    __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("[only effect]", 0x2101));
                }
                else
                {
    script->m_1e0 = 0xc; break;   // "[only effect]"
                }
            case 0x2105:  // "[upgradable legacy]"
                if (!__inited_1)
                {
                    __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("[upgradable legacy]", 0x2105));
                }
                else
                {
    script->m_1e0 = 0xd; break;   // "[upgradable legacy]"
                }
            case 0x2108:  // "[cera package]"
                if (!__inited_1)
                {
                    __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("[cera package]", 0x2108));
                }
                else
                {
    script->m_1e0 = 0xe; break;   // "[cera package]"
                }
            case 0x210c:  // "[usable cera package]"
                if (!__inited_1)
                {
                    __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("[usable cera package]", 0x210c));
                }
                else
                {
    script->m_1e0 = 0x26; break;   // "[usable cera package]"
                }
            case 0x2110:  // "[cera booster]"
                if (!__inited_1)
                {
                    __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("[cera booster]", 0x2110));
                }
                else
                {
    script->m_1e0 = 0xf; break;   // "[cera booster]"
                }
            case 0x2113:  // "[booster]"
                if (!__inited_1)
                {
                    __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("[booster]", 0x2113));
                }
                else
                {
    script->m_1e0 = 0x10; break;   // "[booster]"
                }
            case 0x2116:  // "[booster random]"
                if (!__inited_1)
                {
                    __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("[booster random]", 0x2116));
                }
                else
                {
    script->m_1e0 = 0x11; break;   // "[booster random]"
                }
            case 0x211e:  // "[unlimited waste]"
                if (!__inited_1)
                {
                    __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("[unlimited waste]", 0x211e));
                }
                else
                {
    script->m_1e0 = 0x12; break;   // "[unlimited waste]"
                }
            case 0x2124:  // "[multi upgradable legacy]"
                if (!__inited_1)
                {
                    __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("[multi upgradable legacy]", 0x2124));
                }
                else
                {
    script->m_1e0 = 0x1a; break;   // "[multi upgradable legacy]"
                }
            case 0x212a:  // "[multi upgradable legacy bonus cera]"
                if (!__inited_1)
                {
                    __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("[multi upgradable legacy bonus cera]", 0x212a));
                }
                else
                {
    script->m_1e0 = 0x1b; break;   // "[multi upgradable legacy bonus cera]"
                }
            case 0x212e:  // "[quest receive]"
                if (!__inited_1)
                {
                    __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("[quest receive]", 0x212e));
                }
                else
                {
    script->m_1e0 = 0x13; break;   // "[quest receive]"
                }
            case 0x2131:  // "[avatar emblem]"
                if (!__inited_1)
                {
                    __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("[avatar emblem]", 0x2131));
                }
                else
                {
    script->m_1e0 = 0x14; break;   // "[avatar emblem]"
                }
            case 0x2134:  // "[unlimited etc]"
                if (!__inited_1)
                {
                    __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("[unlimited etc]", 0x2134));
                }
                else
                {
    script->m_1e0 = 0x15; break;   // "[unlimited etc]"
                }
            case 0x2137:  // "[disguise]"
                if (!__inited_1)
                {
                    __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("[disguise]", 0x2137));
                }
                else
                {
    script->m_1e0 = 0x16; break;   // "[disguise]"
                }
            case 0x213a:  // "[expert town potion]"
                if (!__inited_1)
                {
                    __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("[expert town potion]", 0x213a));
                }
                else
                {
    script->m_1e0 = 0x18; break;   // "[expert town potion]"
                }
            case 0x213e:  // "[global effect]"
                if (!__inited_1)
                {
                    __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("[global effect]", 0x213e));
                }
                else
                {
    script->m_1e0 = 0x17; break;   // "[global effect]"
                }
            case 0x2142:  // "[contract]"
                if (!__inited_1)
                {
                    __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("[contract]", 0x2142));
                }
                else
                {
    script->m_1e0 = 0x19; break;   // "[contract]"
                }
            case 0x2145:  // "[enchant waste]"
                if (!__inited_1)
                {
                    __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("[enchant waste]", 0x2145));
                }
                else
                {
    script->m_1e0 = 0x1c; break;   // "[enchant waste]"
                }
            case 0x214e:  // "[random upgradable legacy]"
                if (!__inited_1)
                {
                    __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("[random upgradable legacy]", 0x214e));
                }
                else
                {
    script->m_1e0 = 0x1a; break;   // "[random upgradable legacy]"
                }
            case 0x215c:  // "[upgrade limit cube]"
                if (!__inited_1)
                {
                    __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("[upgrade limit cube]", 0x215c));
                }
                else
                {
    script->m_1e0 = 0x1d; break;   // "[upgrade limit cube]"
                }
            case 0x2160:  // "[tarot card lottery]"
                if (!__inited_1)
                {
                    __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("[tarot card lottery]", 0x2160));
                }
                else
                {
    script->m_1e0 = 0x1e; break;   // "[tarot card lottery]"
                }
            case 0x2163:  // "[tarot card]"
                if (!__inited_1)
                {
                    __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("[tarot card]", 0x2163));
                }
                else
                {
    script->m_1e0 = 0x1f; break;   // "[tarot card]"
                }
            case 0x2168:  // "[town and dungeon]"
                if (!__inited_1)
                {
                    __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("[town and dungeon]", 0x2168));
                }
                else
                {
    script->m_1e0 = 0x20; break;   // "[town and dungeon]"
                }
            case 0x216b:  // "[unlimited town and dungeon]"
                if (!__inited_1)
                {
                    __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("[unlimited town and dungeon]", 0x216b));
                }
                else
                {
    script->m_1e0 = 0x21; break;   // "[unlimited town and dungeon]"
                }
            case 0x2174:  // "[booster selection]"
                if (!__inited_1)
                {
                    __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("[booster selection]", 0x2174));
                }
                else
                {
    script->m_1e0 = 0x22; break;   // "[booster selection]"
                }
            case 0x2179:  // "[dye]"
                if (!__inited_1)
                {
                    __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("[dye]", 0x2179));
                }
                else
                {
    script->m_1e0 = 0x24; break;   // "[dye]"
                }
            case 0x2195:  // "[teleport potion]"
                if (!__inited_1)
                {
                    __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("[teleport potion]", 0x2195));
                }
                else
                {
    script->m_1e0 = 0x25; break;   // "[teleport potion]"
                }
            default:
    break;
        }
        if (__inited_1)
        {
            break;
        }
        __init_lock_1.leave();
        __inited_1 = true;
    }
    // ORIG 0x089b2201：无论类型 token 是否命中，消费一个整数写 +0xa4。
    script->m_fielda4 = ScanInt((bool*)0);
}
break;
                }
            case 0x21c4:  // "[premium]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[premium]", 0x21c4));
                }
                else
                {
    script->m_arad178.m_field14 = ScanInt(&ok);
    break;
                }
            case 0x21c8:  // "[sub type]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[sub type]", 0x21c8));
                }
                else
                {
    script->m_1e4 = ScanInt(&ok);
    break;
                }
            case 0x21cb:  // "[action type]"
    //  再 ScanInt(&ok) 循环 push +0x1ec（m_vec1ec），!ok 跳出。
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[action type]", 0x21cb));
                }
                else
                {
    {
        ScanStr(&token);
        script->m_1e8 = getStackableActionType(token);  // +0x1e8
        for (;;)
        {
            int v = ScanInt(&ok);
            if (!ok)
            {
                break;
            }
            script->m_vec1ec.push_back(v);
        }
    }
    break;
                }
            case 0x21da:  // "[use action packet]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[use action packet]", 0x21da));
                }
                else
                {
    {
        int v = ScanInt(&ok);
        if (ok)
        {
            // ORIG 089b23f7：每次有效读取都直接写 (v != 0)，包括
            // 重复标签给出的 0，不能只在非零时置位。
            script->m_204 = static_cast<char>(v != 0);
        }
    }
    break;
                }
            case 0x21e2:  // "[daily delete item]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[daily delete item]", 0x21e2));
                }
                else
                {
    {
        int v = ScanInt(&ok);
        if (ok)
        {
            script->m_205 = static_cast<char>(v);
        }
    }
    break;
                }
            case 0x21ec:  // "[increase status type]"
    //  循环 ScanInt push +0x224（m_vec224）。
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[increase status type]", 0x21ec));
                }
                else
                {
    {
        ScanStr(&token);
        script->m_220 = getStackableIncreaseStatusType(token);
        for (;;)
        {
            int v = ScanInt(&ok);
            if (!ok)
            {
                break;
            }
            script->m_vec224.push_back(v);
        }
    }
    break;
                }
            case 0x21fd:  // "[action usable place]"
    //  getStackableActionUsablePlace → push m_place1f8(+0x1f8)，循环。
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[action usable place]", 0x21fd));
                }
                else
                {
    {
        while (ScanStr(&token))
        {
            script->m_place1f8.push_back(
                static_cast<StackableAction::UsablePlace>(getStackableActionUsablePlace(token)));
        }
    }
    break;
                }
            case 0x2210:  // "[action usable server type]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[action usable server type]", 0x2210));
                }
                else
                {
    {
        while (ScanStr(&token))
        {
            script->m_server214.push_back(
                static_cast<StackableAction::UsableServerType>(
                    getStackableActionUsableServerType(token)));
        }
    }
    break;
                }
            case 0x221d:  // "[max having count]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[max having count]", 0x221d));
                }
                else
                {
    script->m_field60 = ScanInt(&ok);
    break;
                }
            case 0x2221:  // "[minimum level]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[minimum level]", 0x2221));
                }
                else
                {
    script->m_field5c = ScanInt(&ok);
    break;
                }
            case 0x2224:  // "[minimum rank]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[minimum rank]", 0x2224));
                }
                else
                {
    script->m_field64 = ScanInt(&ok);
    break;
                }
            case 0x2227:  // "[appendage group]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[appendage group]", 0x2227));
                }
                else
                {
    script->m_1b0 = ScanInt(&ok);
    break;
                }
            case 0x222a:  // "[cool time]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[cool time]", 0x222a));
                }
                else
                {
    script->m_field68 = ScanInt(&ok);
    break;
                }
            case 0x222d:  // "[cooltime group]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[cooltime group]", 0x222d));
                }
                else
                {
    script->m_1b4 = ScanInt(&ok);
    break;
                }
            case 0x2230:  // "[party cooltime]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[party cooltime]", 0x2230));
                }
                else
                {
    script->m_1b8 = static_cast<char>(ScanInt(&ok) != 0);
    break;
                }
            case 0x2233:  // "[cooltime maintenance]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[cooltime maintenance]", 0x2233));
                }
                else
                {
    script->m_option1b9.m_field0 = static_cast<unsigned char>(ScanInt(&ok) != 0);
    script->m_option1b9.m_field1 = static_cast<unsigned char>(ScanInt(&ok) != 0);
    script->m_option1b9.m_field2 = static_cast<unsigned char>(ScanInt(&ok) != 0);
    break;
                }
            case 0x2238:  // "[effect maintenance]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[effect maintenance]", 0x2238));
                }
                else
                {
    script->m_option1bc.m_field0 = static_cast<unsigned char>(ScanInt(&ok) != 0);
    script->m_option1bc.m_field1 = static_cast<unsigned char>(ScanInt(&ok) != 0);
    script->m_option1bc.m_field2 = static_cast<unsigned char>(ScanInt(&ok) != 0);
    script->m_1bf = static_cast<char>(ScanInt(&ok) != 0);
    break;
                }
            case 0x223e:  // "[shared effect]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[shared effect]", 0x223e));
                }
                else
                {
    {
        int v = ScanInt(&ok);
        if (ok)
        {
            // ORIG 089b2bf9：每次有效读取都覆盖该标志，0 也必须清零。
            script->m_1d0 = static_cast<char>(v == 1);
        }
    }
    break;
                }
            case 0x2243:  // "[exp bonus rate]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[exp bonus rate]", 0x2243));
                }
                else
                {
    script->m_1c0 = 1;
    script->m_1c4 = ScanFloat(&ok);
    break;
                }
            case 0x2248:  // "[pc exp bonus rate]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[pc exp bonus rate]", 0x2248));
                }
                else
                {
    script->m_1c8 = 1;
    script->m_1cc = ScanFloat(&ok);
    break;
                }
            case 0x224d:  // "[icon]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[icon]", 0x224d));
                }
                else
                {
    ScanStr(&script->m_str6c);
    script->m_field70 = ScanInt(&ok);
    break;
                }
            case 0x2252:  // "[tarotcard view]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[tarotcard view]", 0x2252));
                }
                else
                {
    ScanStr(&script->m_str7c);
    script->m_field80 = ScanInt(&ok);
    break;
                }
            case 0x2257:  // "[field image]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[field image]", 0x2257));
                }
                else
                {
    ScanStr(&script->m_str74);
    script->m_field78 = ScanInt(&ok);
    break;
                }
            case 0x225b:  // "[price]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[price]", 0x225b));
                }
                else
                {
    script->m_field14 = ScanInt(&ok);
    break;
                }
            case 0x225e:  // "[value]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[value]", 0x225e));
                }
                else
                {
    script->m_field24 = ScanInt(&ok);
    break;
                }
            case 0x2261:  // "[finish point price]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[finish point price]", 0x2261));
                }
                else
                {
    script->m_field20 = ScanInt(&ok);
    break;
                }
            case 0x2265:  // "[advance altar need gage]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[advance altar need gage]", 0x2265));
                }
                else
                {
    script->m_4ec = ScanInt(&ok);
    break;
                }
            case 0x2268:  // "[disable icon]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[disable icon]", 0x2268));
                }
                else
                {
    ScanStr(&script->m_str1d8);
    script->m_1dc = ScanInt(&ok);
    break;
                }
            case 0x226d:  // "[need material]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[need material]", 0x226d));
                }
                else
                {
    script->m_pair28.first = ScanInt(&ok);
    script->m_pair28.second = ScanInt(&ok);
    break;
                }
            case 0x2272:  // "[recieve postal message]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[recieve postal message]", 0x2272));
                }
                else
                {
    ScanStr(&script->m_str174);
    break;
                }
            case 0x2276:  // "[linking quest index]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[linking quest index]", 0x2276));
                }
                else
                {
    script->m_1a8 = ScanInt(&ok);
    break;
                }
            case 0x2279:  // "[cash]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[cash]", 0x2279));
                }
                else
                {
    script->m_field18 = ScanInt(&ok);
    break;
                }
            case 0x227c:  // "[auction free]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[auction free]", 0x227c));
                }
                else
                {
    script->m_field30 = 1;
    break;
                }
            case 0x227f:  // "[medal]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[medal]", 0x227f));
                }
                else
                {
    script->m_field1c = ScanInt(&ok);
    break;
                }
            case 0x2288:  // "[usable upgrade chracter level]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[usable upgrade chracter level]", 0x2288));
                }
                else
                {
    script->m_field134 = static_cast<unsigned short>(ScanInt(&ok));
    break;
                }
            case 0x228c:  // "[move wav]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[move wav]", 0x228c));
                }
                else
                {
    ScanStr(&script->m_str84);
    break;
                }
            case 0x228f:  // "[use wav]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[use wav]", 0x228f));
                }
                else
                {
    ScanStr(&script->m_str238);
    break;
                }
            case 0x2292:  // "[int data]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[int data]", 0x2292));
                }
                else
                {
    {
        script->m_vec88.clear();
        for (;;)
        {
            int v = ScanInt(&ok);
            if (!ok)
            {
                break;
            }
            script->m_vec88.push_back(v);
        }
    }
    break;
                }
            case 0x229f:  // "[random]"
    //  （089b47e1 push/清空），随后 while (ScanType(&line,1)) 内部读子行直到
    //  "[/random]"： "[random list]" 段循环 ScanInt 收集后整段推入 +0x244；
    //  "[sealing removal item]" 段循环 ScanInt push +0x250（m_vec250）；
    //  [default list] 段循环 ScanInt push +0x25c（m_vec25c）。
    //  三个子段各自先清理目标容器，再按 ScanInt 消费到下一个标签。
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[random]", 0x229f));
                }
                else
                {
    {
        script->m_vec244.clear();
        for (;;)
        {
            // ORIG 0x089b36ca：子循环直接复用主 line 槽（-0x1444）
            if (!ScanType(line, true))
            {
                break;  // 089b3880 出口
            }
            if (line == "[/random]")
            {
                break;  // 089b482d jmp 089b3880
            }
            else if (line == "[random list]")
            {
                std::vector<int> group;
                for (;;)
                {
                    int v = ScanInt(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    group.push_back(v);
                }
                script->m_vec244.push_back(group);
            }
            else if (line == "[sealing removal item]")
            {
                script->m_vec250.clear();
                for (;;)
                {
                    int v = ScanInt(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    script->m_vec250.push_back(v);
                }
            }
            else if (line == "[default list]")
            {
                for (;;)
                {
                    int v = ScanInt(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    script->m_vec25c.push_back(v);
                }
            }
        }
    }
    break;
                }
            case 0x2341:  // "[string data]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[string data]", 0x2341));
                }
                else
                {
    {
        script->m_vec94.clear();
        while (ScanStr(&token))
        {
            script->m_vec94.push_back(token);
        }
    }
    break;
                }
            case 0x2347:  // "[applying range]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[applying range]", 0x2347));
                }
                else
                {
    script->m_1d4 = ScanInt(&ok);
    break;
                }
            case 0x234a:  // "[stack limit]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[stack limit]", 0x234a));
                }
                else
                {
    script->m_364 = ScanInt((bool*)0);
    break;
                }
            case 0x2352:  // "[packagable]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[packagable]", 0x2352));
                }
                else
                {
    script->m_fieldfc = static_cast<char>(ScanInt(&ok) != 0);
    break;
                }
            case 0x2355:  // "[creature minimum level]"
    // ORIG 089b3a90：ScanInt 写 +0x23c（与 [stack limit] 同一字段）。
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[creature minimum level]", 0x2355));
                }
                else
                {
    script->m_23c = ScanInt((bool*)0);
    break;
                }
            case 0x2358:  // "[set item master]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[set item master]", 0x2358));
                }
                else
                {
    ScanStr(&token);                 // ORIG 0x089b3bf6：读入公共 token
    script->m_strc0 = token;         // 0x089b3c0d：op= 写 +0xc0
    ScanInt(&script->m_fieldc4);     // 0x089b3bf2：ScanInt(int*) 直写 +0xc4
    break;
                }
            case 0x235b:  // "[icon mark]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[icon mark]", 0x235b));
                }
                else
                {
    script->m_fieldf4 = ScanInt(&ok);
    break;
                }
            case 0x2368:  // "[usable period]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[usable period]", 0x2368));
                }
                else
                {
    script->m_fieldf8 = ScanInt(&ok);
    break;
                }
            case 0x236b:  // "[expiration date]"
    // ORIG 089b3c9d：ScanInt 写 +0x240。
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[expiration date]", 0x236b));
                }
                else
                {
    script->m_240 = ScanInt((bool*)0);
    break;
                }
            case 0x237a:  // "[package data]"
    //  （元素 STStackablePackageData = 2 int）。每行两 int（失败即断）。
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[package data]", 0x237a));
                }
                else
                {
    {
        script->m_package268.clear();
        for (;;)
        {
            int a = ScanInt(&ok);
            if (!ok)
            {
                break;
            }
            int b = ScanInt((bool*)0);
            STStackablePackageData d;
            d.m_field0 = a;
            d.m_field4 = b;
            script->m_package268.push_back(d);
        }
    }
    break;
                }
            case 0x238a:  // "[secret add item]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[secret add item]", 0x238a));
                }
                else
                {
    {
        script->m_package2a4.clear();
        for (;;)
        {
            int a = ScanInt(&ok);
            if (!ok)
            {
                break;
            }
            int b = ScanInt((bool*)0);
            STStackablePackageData d;
            d.m_field0 = a;
            d.m_field4 = b;
            script->m_package2a4.push_back(d);
        }
    }
    break;
                }
            case 0x239a:  // "[package data selection]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[package data selection]", 0x239a));
                }
                else
                {
    {
        std::vector<STStackablePackageData> group;
        for (;;)
        {
            int a = ScanInt(&ok);
            if (!ok)
                break;
            STStackablePackageData d;
            d.m_field0 = a;
            d.m_field4 = ScanInt((bool*)0);
            group.push_back(d);
        }
        script->m_package274.push_back(group);
    }
    break;
                }
            case 0x23ca:  // "[booster info]"
    //  外层 ScanType 读组头标签行（tag2Type 0..8；9=未知 → return false），组内同
    //  行 ScanTypeOrInt 连续取元素：整数值 = item 字段组（itemIdx + prob 累计 +
    //  附加信息 + type 0/1 追加 avatarType/avatarLevel），遇字符串 = 结束标签
    //  "[/xxx]"，tag2EndType 必须等于组 type，随后整组 stStackableBoosterElement_t
    //  push m_booster280.m_elements。外层遇 "[/booster info]" 结束；任一步扫描失败
    //  （isInt=false / ScanInt 失败 / ScanType EOF）→ return false。
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[booster info]", 0x23ca));
                }
                else
                {
    {
        std::string bline;   // ORIG 局部串槽 -0x14cc（本段子循环专用）
        for (;;)
        {
            if (!ScanType(bline, true))
            {
                return false;   // ORIG 407f ScanType 失败 → return false
            }
            if (bline == "[/booster info]")
            {
                break;          // ORIG 40ad..4367 → 正常收尾 continue
            }
            stStackableBoosterElement_t elem;   // ORIG 40bc ctor（m_items）
            int type = script->m_booster280.tag2Type(bline.c_str());
            if (type == 9)
            {
                return false;   // ORIG 40f3：未知标签 → esi=0 → return false
            }
            elem.m_field0 = type;                       // +0x00（40ff 区）
            if (!ScanInt(&elem.m_count))                // +0x04（4117）
            {
                return false;
            }
            int probAcc = 0;                            // -0x94（413e）
            for (;;)
            {
                bool isType = false;
                bool isInt = false;
                // ORIG 0x089b4168：字符串出参直接复用 bline 槽(-0x14cc)
                int v = ScanTypeOrInt(isType, bline, isInt, 1);
                if (!isInt)
                {
                    return false;   // 4186..41a3 扫描失败
                }
                if (isType)
                {
                    // 字符串 = 结束标签；tag2EndType 必须等于组 type
                    int endType = script->m_booster280.tag2EndType(bline.c_str());
                    if (elem.m_field0 != endType)
                    {
                        return false;   // 41d7..41e9 不匹配 → return false
                    }
                    break;              // 提交本组（4303）
                }
                stStackableBoosterItemInfo_t item;
                item.m_itemIdx = v;     // -0x14f4（41f8..4204）
                int v2 = 0;
                if (!ScanInt(&v2))      // 420d
                {
                    return false;
                }
                probAcc += v2;
                item.m_prob = probAcc;  // -0x14f0（4228..423a）
                int v3 = 0;
                if (!ScanInt(&v3))      // 4249
                {
                    return false;
                }
                item.m_addInfo = static_cast<unsigned short>(v3);  // -0x14ec
                if (type == 0 || type == 1)
                {
                    int v4 = 0;
                    if (!ScanInt(&v4))  // 428f
                    {
                        return false;
                    }
                    item.m_avatarType = static_cast<unsigned char>(v4);   // -0x14e7
                    int v5 = 0;
                    if (!ScanInt(&v5))  // 42bc
                    {
                        return false;
                    }
                    item.m_avatarLevel = static_cast<unsigned char>(v5);   // -0x14e6
                }
                elem.m_items.push_back(item);   // 42e3..42fe
            }
            elem.m_prob = probAcc;               // 4304..4310（+0x08）
            script->m_booster280.m_elements.push_back(elem);  // 4326
        }
    }
    break;
                }
            case 0x2497:  // "[booster select category]"
    // 读取 pair<char,char> 键；default/recommend 列表用于回填条目 byteB/byteC，
    // 普通条目最终写入 m_booster280.m_map[key]。
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[booster select category]", 0x2497));
                }
                else
                {
    {
        int key0 = 0;
        int key1 = 0;
        ScanInt(&key0);
        ScanInt(&key1);
        std::vector<stStackableBoosterItemInfo_t> items;
        std::vector<int> defaultSelect;
        std::vector<int> recommend;
        int equipmentUpgrade = 0;
        bool success = true;
        for (;;)
        {
            if (!ScanType(line, true))
                break;
            if (line == "[/booster select category]")
                break;
            if (line == "[booster equipment upgrade]")
            {
                if (!ScanInt(&equipmentUpgrade))
                {
                    success = false;
                    break;
                }
                continue;
            }
            if (line == "[default select]")
            {
                int count = 0;
                if (!ScanInt(&count))
                {
                    success = false;
                    break;
                }
                for (int i = 0; i < count; ++i)
                {
                    int v = ScanInt((bool*)0);
                    defaultSelect.push_back(v);
                }
                continue;
            }
            if (line == "[recommend]")
            {
                int count = 0;
                if (!ScanInt(&count))
                {
                    success = false;
                    break;
                }
                for (int i = 0; i < count; ++i)
                {
                    int v = ScanInt((bool*)0);
                    recommend.push_back(v);
                }
                continue;
            }

            // ORIG 0x089b4619：每轮先构造组元素对象（tag2Type 结果写 m_field0）
            stStackableBoosterElement_t elem;
            const int type = script->m_booster280.tag2Type(line.c_str());
            elem.m_field0 = type;
            for (;;)
            {
                stStackableBoosterItemInfo_t item;
                item.m_itemIdx = ScanInt(&ok);
                if (!ok)
                    break;
                item.m_addInfo = static_cast<unsigned short>(ScanInt((bool*)0));
                // ORIG 0x089b46b3..0x089b46cc：type 0/1 的两字节追加扫描在
                // 键值回填(0x089b46d2 起)之前。
                if (type == 0 || type == 1)
                {
                    item.m_avatarType = static_cast<unsigned char>(ScanInt((bool*)0));
                    item.m_avatarLevel = static_cast<unsigned char>(ScanInt((bool*)0));
                }
                item.m_byteA = (type == 4)
                    ? static_cast<unsigned char>(equipmentUpgrade) : 0;
                item.m_byteB = 0;
                for (std::vector<int>::iterator dit = defaultSelect.begin();
                     dit < defaultSelect.end(); ++dit)
                {
                    if (*dit == item.m_itemIdx)
                    {
                        item.m_byteB = 1;
                        break;
                    }
                }
                item.m_byteC = 0;
                for (std::vector<int>::iterator rit = recommend.begin();
                     rit < recommend.end(); ++rit)
                {
                    if (*rit == item.m_itemIdx)
                    {
                        item.m_byteC = 1;
                        break;
                    }
                }
                items.push_back(item);
            }
        }
        if (!success)
            return false;
        // ORIG 0x089b48bb：make_pair(int&,int&) → ix 临时 pair<char,char> 转换
        script->m_booster280.m_map[std::make_pair(key0, key1)] = items;
    }
    break;
                }
            case 0x250e:  // "[booster category num]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[booster category num]", 0x250e));
                }
                else
                {
    {
        int category = ScanInt(&ok);
        if (ok && category < 3)
        {
            int first = 0;
            int second = 0;
            if (category >= 1)
            {
                first = ScanInt(&ok);
                if (!ok)
                    break;
            }
            if (category == 2)
            {
                second = ScanInt(&ok);
                if (!ok)
                    break;
            }
            if (category < 1)
            {
                script->m_boosterSelect41c.m_field0 = 0;
                script->m_boosterSelect41c.m_field4 = 0;
            }
            else if (category == 1)
            {
                script->m_boosterSelect41c.m_field0 = first;
                script->m_boosterSelect41c.m_field4 = 0;
            }
            else
            {
                script->m_boosterSelect41c.m_field0 = first;
                script->m_boosterSelect41c.m_field4 = second;
            }
        }
    }
    break;
                }
            case 0x2532:  // "[booster selection num]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[booster selection num]", 0x2532));
                }
                else
                {
    {
        int value = ScanInt(&ok);
        if (ok)
            script->m_boosterSelect41c.m_field8 = value;
    }
    break;
                }
            case 0x253c:  // "[equipment upgrade]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[equipment upgrade]", 0x253c));
                }
                else
                {
    {
        int value = ScanInt(&ok);
        if (ok)
        {
            script->m_boosterSelect41c.m_field18 = static_cast<char>(value);
        }
    }
    break;
                }
            case 0x2545:  // "[booster category name]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[booster category name]", 0x2545));
                }
                else
                {
    {
        std::string name;   // ORIG -0x1568（每次构造 + clear）
        name.clear();
        if (ScanStr(&name))
            script->m_boosterSelect41c.m_nameVec.push_back(std::string(name.c_str()));
    }
    break;
                }
            case 0x2553:  // "[hp recovery]"
    //  ScanStr→"+"? m_bAddMode=0 : =1；ScanInt→m_nValue1(+0x2b4)；ScanInt→m_nValue2
    //  (+0x2b8)；ScanStr→"myself"? m_nTarget(+0x2bc)=0 : =1；m_bEnable(+0x2c0)=1
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[hp recovery]", 0x2553));
                }
                else
                {
    {
        std::string val;   // ORIG 局部串槽 -0x156c（两次 ScanStr 复用）
        ScanStr(&val);
        // 【判词·勿回改】本文件 9 处 `x ? 0 : 1` 三目（+0x2b0/+0x2bc/+0x2c4/
        // +0x2d0/+0x2e4×2/+0x30c/+0x35c）ORIG 编成两分支各自 mov $0/$1 物化，
        // 已实测改成 if/else 两分支写法后 **归一化**(去槽号/立即数) 差异确实下降，
        // 但台账口径的 **raw 行级 ndiff 反弹 +716**（4.4 的 test/jne 取向与单点
        // `mov %al,OFF` 收束在全局 difflib 对齐下反而更贴 ORIG）。按 raw 口径
        // 保留三目原形；后续泳道勿再以 normalized 口径"优化"回去。
        script->m_recovery2b0.m_bAddMode = (val == "+") ? 0 : 1;
        script->m_recovery2b0.m_nValue1 = ScanInt(&ok);
        script->m_recovery2b0.m_nValue2 = ScanInt(&ok);
        ScanStr(&val);
        script->m_recovery2b0.m_nTarget = (val == "myself") ? 0 : 1;
        script->m_recovery2b0.m_bEnable = 1;
    }
    break;
                }
            case 0x2593:  // "[mp recovery]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[mp recovery]", 0x2593));
                }
                else
                {
    {
        std::string val;   // ORIG 局部串槽 -0x1570（两次 ScanStr 复用）
        ScanStr(&val);
        script->m_recovery2c4.m_bAddMode = (val == "+") ? 0 : 1;
        script->m_recovery2c4.m_nValue1 = ScanInt(&ok);
        script->m_recovery2c4.m_nValue2 = ScanInt(&ok);
        ScanStr(&val);
        script->m_recovery2c4.m_nTarget = (val == "myself") ? 0 : 1;
        script->m_recovery2c4.m_bEnable = 1;
    }
    break;
                }
            case 0x25b1:  // "[active status recovery]"
    //  getActiveStatusToEnumValue→+0x2dc；ScanInt→+0x2e0；ScanStr "myself"→+0x2e4
    //  0/1；+0x2e8=1。无本地 helper 映射表 → job/status 数字由 token 直译【推断】
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[active status recovery]", 0x25b1));
                }
                else
                {
    {
        std::string val;   // ORIG 局部串槽 -0x1574
        ScanStr(&val);
        // +0x2dc.. 属 m_statusRecovery2d8 前的状态块（+0x2d8..+0x2eb 见 .h 相邻）
        // ORIG 写 +0x2dc：getActiveStatusToEnumValue(val)。
        script->m_statusRecovery2d8.m_field4 =
            getActiveStatusToEnumValue(val);
        script->m_statusRecovery2d8.m_field8 = ScanInt(&ok);         // +0x2e0
        ScanStr(&token);
        script->m_statusRecovery2d8.m_fieldC = (token == "myself") ? 0 : 1;  // +0x2e4
        script->m_statusRecovery2d8.m_field10 = 1;                           // +0x2e8
    }
    break;
                }
            case 0x25c6:  // "[except status recovery]"
    //  /+0x2e0/+0x2e4/+0x2e8
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[except status recovery]", 0x25c6));
                }
                else
                {
    {
        std::string val;   // ORIG 局部串槽 -0x1578
        ScanStr(&val);
        script->m_statusRecovery2d8.m_field0 =
            getExcepttionStatusToEnumValue(val);    // +0x2d8
        script->m_statusRecovery2d8.m_field8 = ScanInt(&ok);         // +0x2e0
        ScanStr(&token);
        script->m_statusRecovery2d8.m_fieldC = (token == "myself") ? 0 : 1;  // +0x2e4
        script->m_statusRecovery2d8.m_field10 = 1;                           // +0x2e8
    }
    break;
                }
            case 0x25fb:  // "[my status]"
    //  +0x2ec STStatusGenerate）：
    //  主块（089b52cd..089b55a7）：m_field14(+0x300)=1 → vec0.push(4.0f 预置) →
    //  ScanStr(&name) → vec0.push((float)getActiveStatusToEnumValue(name))（ORIG
    //  645f fildl 转 float，call 088c29a2 实证符号）→ ScanInt→m_field10(+0x2fc) →
    //  ScanInt→m_fieldC(+0x2f8) → ScanInt→(float) push vec0（64a7..64b3 fildl）。
    //  二级（089b53b7..089b5f83）：静态查找表(lookup_2) find(name) → 子 switch 值
    //  0x2611..0x263f；命中分支把 ScanFloat 依次 push 同一 vec0（每组次数实证：
    //  blind/slow/haste=2、stone/confuse/weakness/armor break/weapon break/poison/
    //  lightning/bleeding=1、burn/bless/curse=4）；无命中(default → 089b5f83) 无附加。
    //  子表 hash 与主 StackableTagLookup 同值域，故此处用 if 链按 name 直比。
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[my status]", 0x25fb));
                }
                else
                {
    {
        script->m_status2ec.m_field14 = 1;
        script->m_status2ec.m_vec0.push_back(4.0f);  // ORIG 6428..643e（0x40c00000）
        std::string name;
        ScanStr(&name);                               // 6443..644c
        float fStat = static_cast<float>(getActiveStatusToEnumValue(name)); // 6451..646b
        script->m_status2ec.m_vec0.push_back(fStat);
        script->m_status2ec.m_field10 = ScanInt(&ok); // 6471..6480 → +0x2fc
        script->m_status2ec.m_fieldC = ScanInt(&ok);  // 6486..6495 → +0x2f8
        float fInt = static_cast<float>(ScanInt(&ok)); // 649b..64b3（64ad fildl）
        script->m_status2ec.m_vec0.push_back(fInt);
        // 二级子标签（ORIG 0x089b53b7 静态子表(14)+双检锁 → switch）：
        {
        static std::map<char const*, int, less_TCHAR> __lookup_2;
        static SpinLock __init_lock_2;
        static bool __inited_2 = false;
        int sub;
        for (;;)
        {
            if (__inited_2)
            {
                std::map<char const*, int, less_TCHAR>::const_iterator it2 = __lookup_2.find(name.c_str());
                sub = (it2 != __lookup_2.end()) ? it2->second : -1;
            }
            else
            {
                __init_lock_2.enter();
                if (__inited_2)
                {
                    __init_lock_2.leave();
                    continue;
                }
                sub = 0;
            }
            switch (sub)
            {
                case 0:
                case 0x2611:  // "blind"
                    if (!__inited_2)
                    {
                        __lookup_2.insert(std::map<char const*, int, less_TCHAR>::value_type("blind", 0x2611));
                    }
                    else
                    {
            script->m_status2ec.m_vec0.push_back(ScanFloat((bool*)0));
            script->m_status2ec.m_vec0.push_back(ScanFloat((bool*)0));
            break;
                    }
                case 0x2615:  // "slow"
                    if (!__inited_2)
                    {
                        __lookup_2.insert(std::map<char const*, int, less_TCHAR>::value_type("slow", 0x2615));
                    }
                    else
                    {
            script->m_status2ec.m_vec0.push_back(ScanFloat((bool*)0));
            script->m_status2ec.m_vec0.push_back(ScanFloat((bool*)0));
            break;
                    }
                case 0x2619:  // "haste"
                    if (!__inited_2)
                    {
                        __lookup_2.insert(std::map<char const*, int, less_TCHAR>::value_type("haste", 0x2619));
                    }
                    else
                    {
            script->m_status2ec.m_vec0.push_back(ScanFloat((bool*)0));
            script->m_status2ec.m_vec0.push_back(ScanFloat((bool*)0));
            break;
                    }
                case 0x261d:  // "stone"
                    if (!__inited_2)
                    {
                        __lookup_2.insert(std::map<char const*, int, less_TCHAR>::value_type("stone", 0x261d));
                    }
                    else
                    {
            script->m_status2ec.m_vec0.push_back(ScanFloat((bool*)0));
            break;
                    }
                case 0x2620:  // "confuse"
                    if (!__inited_2)
                    {
                        __lookup_2.insert(std::map<char const*, int, less_TCHAR>::value_type("confuse", 0x2620));
                    }
                    else
                    {
            script->m_status2ec.m_vec0.push_back(ScanFloat((bool*)0));
            break;
                    }
                case 0x2623:  // "weakness"
                    if (!__inited_2)
                    {
                        __lookup_2.insert(std::map<char const*, int, less_TCHAR>::value_type("weakness", 0x2623));
                    }
                    else
                    {
            script->m_status2ec.m_vec0.push_back(ScanFloat((bool*)0));
            break;
                    }
                case 0x2626:  // "armor break"
                    if (!__inited_2)
                    {
                        __lookup_2.insert(std::map<char const*, int, less_TCHAR>::value_type("armor break", 0x2626));
                    }
                    else
                    {
            script->m_status2ec.m_vec0.push_back(ScanFloat((bool*)0));
            break;
                    }
                case 0x2629:  // "weapon break"
                    if (!__inited_2)
                    {
                        __lookup_2.insert(std::map<char const*, int, less_TCHAR>::value_type("weapon break", 0x2629));
                    }
                    else
                    {
            script->m_status2ec.m_vec0.push_back(ScanFloat((bool*)0));
            break;
                    }
                case 0x262c:  // "poison"
                    if (!__inited_2)
                    {
                        __lookup_2.insert(std::map<char const*, int, less_TCHAR>::value_type("poison", 0x262c));
                    }
                    else
                    {
            script->m_status2ec.m_vec0.push_back(ScanFloat((bool*)0));
            break;
                    }
                case 0x262f:  // "lightning"
                    if (!__inited_2)
                    {
                        __lookup_2.insert(std::map<char const*, int, less_TCHAR>::value_type("lightning", 0x262f));
                    }
                    else
                    {
            script->m_status2ec.m_vec0.push_back(ScanFloat((bool*)0));
            break;
                    }
                case 0x2632:  // "bleeding"
                    if (!__inited_2)
                    {
                        __lookup_2.insert(std::map<char const*, int, less_TCHAR>::value_type("bleeding", 0x2632));
                    }
                    else
                    {
            script->m_status2ec.m_vec0.push_back(ScanFloat((bool*)0));
            break;
                    }
                case 0x2635:  // "burn"
                    if (!__inited_2)
                    {
                        __lookup_2.insert(std::map<char const*, int, less_TCHAR>::value_type("burn", 0x2635));
                    }
                    else
                    {
            script->m_status2ec.m_vec0.push_back(ScanFloat((bool*)0));
            script->m_status2ec.m_vec0.push_back(ScanFloat((bool*)0));
            script->m_status2ec.m_vec0.push_back(ScanFloat((bool*)0));
            script->m_status2ec.m_vec0.push_back(ScanFloat((bool*)0));
            break;
                    }
                case 0x263b:  // "bless"
                    if (!__inited_2)
                    {
                        __lookup_2.insert(std::map<char const*, int, less_TCHAR>::value_type("bless", 0x263b));
                    }
                    else
                    {
            for (int bi = 0; bi <= 3; ++bi)
            {
                script->m_status2ec.m_vec0.push_back(ScanFloat((bool*)0));
            }
            break;
                    }
                case 0x263f:  // "curse"
                    if (!__inited_2)
                    {
                        __lookup_2.insert(std::map<char const*, int, less_TCHAR>::value_type("curse", 0x263f));
                    }
                    else
                    {
            for (int ci = 0; ci <= 3; ++ci)
            {
                script->m_status2ec.m_vec0.push_back(ScanFloat((bool*)0));
            }
            break;
                    }
                default:
    break;
            }
            if (__inited_2)
            {
                break;
            }
            __init_lock_2.leave();
            __inited_2 = true;
        }
        }
    }
break;
                }
            case 0x2646:  // "[buff]"
    //  m_buff304.m_field0(+0x304)；ScanInt→m_field4(+0x308)；ScanStr "myself"→
    //  m_field8(+0x30c) 0/1；m_fieldC(+0x310)=1
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[buff]", 0x2646));
                }
                else
                {
    {
        std::string val;   // ORIG 局部串槽 -0x158c（两次 ScanStr 复用）
        ScanStr(&val);
        script->m_buff304.m_field0 =
            getBuffToEnumValue(val);    // +0x304
        script->m_buff304.m_field4 = ScanInt(&ok);
        ScanStr(&val);
        script->m_buff304.m_field8 = (val == "myself") ? 0 : 1;
        script->m_buff304.m_fieldC = 1;
    }
    break;
                }
            case 0x265a:  // "[stat change]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[stat change]", 0x265a));
                }
                else
                {
    {
        std::string status;
        // ORIG 0x089b6175 仅调 C1、无析构调用点（N2 结构性差异：本 gcc 对自动
        // 对象必发 scope-end D1，语义等价豁免）。
        STStatChange change;
        while (ScanStr(&status))
        {
            change.m_field8 = (status == "+") ? 0 : 1;
            change.m_field4 = ScanInt(&ok);
            ScanStr(&status);
            int type = getChangeStatusType(status);
            if (type == 0x4d)
            {
                type = getActivePropertyTypeToEnumValue(status);
                change.m_fieldC = 1;
            }
            change.m_field0 = type;
            script->m_statChange34c.m_vec0.push_back(change);
        }
    }
    break;
                }
            case 0x267b:  // "[stat change duration]"
    //  .m_fieldC(+0x358=+0x34c+0xc)；ScanStr "myself"→m_field10(+0x35c) 0/1；
    //  m_field14(+0x360)=1。
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[stat change duration]", 0x267b));
                }
                else
                {
    {
    script->m_statChange34c.m_fieldC = ScanInt(&ok);
    std::string val;   // ORIG 局部串槽 -0x15a8
    ScanStr(&val);
    script->m_statChange34c.m_field10 = (val == "myself") ? 0 : 1;
    script->m_statChange34c.m_field14 = 1;
    }
    break;
                }
            case 0x268c:  // "[monster summons]"
    //  （+0x314 field0..field10 5 int），并置 m_field18(+0x32c)=1。
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[monster summons]", 0x268c));
                }
                else
                {
    script->m_summonMonster314.m_field0 = ScanInt(&ok);
    script->m_summonMonster314.m_field4 = ScanInt(&ok);
    script->m_summonMonster314.m_field8 = ScanInt(&ok);
    script->m_summonMonster314.m_fieldC = ScanInt(&ok);
    script->m_summonMonster314.m_field10 = ScanInt(&ok);
    script->m_summonMonster314.m_field18 = 1;
    break;
                }
            case 0x2696:  // "[event monster summons]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[event monster summons]", 0x2696));
                }
                else
                {
    script->m_summonMonster314.m_field0 = ScanInt(&ok);
    script->m_summonMonster314.m_field4 = ScanInt(&ok);
    script->m_summonMonster314.m_field8 = ScanInt(&ok);
    script->m_summonMonster314.m_fieldC = ScanInt(&ok);
    script->m_summonMonster314.m_field10 = ScanInt(&ok);
    script->m_summonMonster314.m_field14 = ScanInt(&ok);
    script->m_summonMonster314.m_field18 = 1;
    break;
                }
            case 0x26a1:  // "[summon apc]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[summon apc]", 0x26a1));
                }
                else
                {
    script->m_summonApc330.m_field0 = ScanInt(&ok);
    script->m_summonApc330.m_field4 = ScanInt(&ok);
    script->m_summonApc330.m_field8 = ScanInt(&ok);
    script->m_summonApc330.m_fieldC = ScanInt(&ok);
    script->m_summonApc330.m_field10 = ScanInt(&ok);
    script->m_summonApc330.m_field14 = 1;
    break;
                }
            case 0x26bd:  // "[summon npc]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[summon npc]", 0x26bd));
                }
                else
                {
    script->m_348 = ScanInt(&ok);
    break;
                }
            case 0x26c0:  // "[enchant]"
    //  089b66db 起：确保 m_module4bc 键 0x13 模块存在（已有键复用原指针，
    //  仅未命中时 new+insert），置 m_414=2（089b6825）。
    //  循环（089b684c..089b6d00）：ScanType 逐行 —— "[/enchant]"（089b6ac8）
    //  → bMore=false 结束；"[pvp]"（089b6ad1..089b6c7c）→ 切到键 0x12 模块
    //  （同样确保存在并 m_414=2，089b6c74 实证）；其余行（089b6c94..089b6cca）
    //  → importEquipmentParameterInfo(cur, line, path, cntA, cntB)
    //  （ORIG 0x089a75ea，EquipmentParameterInfo.cpp 已定义；cntA=-0x15ac、
    //   cntB=-0x15b0 循环内累计，返回值 ORIG 忽略）。
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[enchant]", 0x26c0));
                }
                else
                {
    {
        EquipmentParameterInfo* cur = 0;      // ORIG -0x5c
        {
            std::map<ENUM_MODULE_TYPE, EquipmentParameterInfo*>::iterator it =
                script->m_module4bc.find(static_cast<ENUM_MODULE_TYPE>(0x13));
            if (it == script->m_module4bc.end())
            {
                cur = new EquipmentParameterInfo();
                // ORIG 0x089b67a9：显式实参模板 make_pair（值语义签名）
                script->m_module4bc.insert(std::make_pair<ENUM_MODULE_TYPE, EquipmentParameterInfo*>(
                    static_cast<ENUM_MODULE_TYPE>(0x13), cur));
            }
        }
        // ORIG 0x089b6818：ix 统一取回（键缺失时 operator[] 建默认值）
        cur = script->m_module4bc[static_cast<ENUM_MODULE_TYPE>(0x13)];
        cur->m_414 = 2;                       // 089b6825 movl $0x2,0x414
        int cntA = 0;                         // -0x15ac
        int cntB = 0;                         // -0x15b0
        bool bMore = true;
        while (bMore)
                {
            if (!ScanType(line, true))
        {
                break;                        // ScanType EOF → 结束本段
            }
                    // [b28d-solo-STStackableScript] ORIG 089b6862..089b6ccf：本内层
                    // 分发有**独立 static 三件套**（map 0x94fdfe8 / SpinLock
                    // 0x94fe000 / __inited 0x94fe004；guard 0x94fdd08/0x94fdd10 =
                    // nm 实证 `_ZGVZ...E8__lookup_3`/`E11__init_lock_3`，故命名
                    // 必须是 __lookup_3 而非 _5，否则静态局部符号名与 ORIG 不符）。
                    // 原「复用主表」判词被 p167 推翻（ORIG 比复用多一组 map C1 +
                    // SpinLock C1 + guard×2）。static 声明落 ScanType 之后 = 每轮
                    // 穿 guard（P177），[/enchant](0x26d0)/[pvp](0x26d3) 插本表。
            static std::map<char const*, int, less_TCHAR> __lookup_3;
            static SpinLock __init_lock_3;
            static bool __inited_3 = false;
            int etag;
            for (;;)
            {
                        if (__inited_3)
            {
                            std::map<char const*, int, less_TCHAR>::const_iterator it3 = __lookup_3.find(line.c_str());
                            etag = (it3 != __lookup_3.end()) ? it3->second : -1;
                    break;
                }
                        __init_lock_3.enter();
                        if (__inited_3)
                {
                            __init_lock_3.leave();
                    continue;
                }
                etag = 0;
                break;
            }
            if (etag == 0)
                {
                        __lookup_3.insert(std::map<char const*, int, less_TCHAR>::value_type("[/enchant]", 0x26d0));
                        __lookup_3.insert(std::map<char const*, int, less_TCHAR>::value_type("[pvp]", 0x26d3));
                        __init_lock_3.leave();
                        __inited_3 = true;
            }
            else if (etag == 0x26d0)
            {
                bMore = false;                // 089b6ac8：[/enchant] → 出口
            }
            else if (etag == 0x26d3)
            {
                // 模块键 0x12（pvp 附魔段）ORIG 0x089b6bf2..0x089b6c7c 同型
                cur = 0;
                std::map<ENUM_MODULE_TYPE, EquipmentParameterInfo*>::iterator it2 =
                    script->m_module4bc.find(static_cast<ENUM_MODULE_TYPE>(0x12));
                if (it2 == script->m_module4bc.end())
                {
                    cur = new EquipmentParameterInfo();
                    script->m_module4bc.insert(std::make_pair<ENUM_MODULE_TYPE, EquipmentParameterInfo*>(
                        static_cast<ENUM_MODULE_TYPE>(0x12), cur));
                }
                cur = script->m_module4bc[static_cast<ENUM_MODULE_TYPE>(0x12)];
                cur->m_414 = 2;               // 089b6c74 movl $0x2,0x414
                // ORIG 089b6c74 后立即清零两个 import 计数器，PVP
                // 段不能沿用普通附魔段的累计值。
                cntA = 0;
                cntB = 0;
            }
            else
            {
                importEquipmentParameterInfo(cur, line, path, cntA, cntB);
            }
        }
    }
    break;
                }
            case 0x26ed:  // "[avatar package preview info]"
    //  ScanInt → +0x36c（m_36c）
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[avatar package preview info]", 0x26ed));
                }
                else
                {
    ScanStr(&script->m_str368);
    script->m_36c = ScanInt(&ok);
    break;
                }
            case 0x26f2:  // "[creature preview info]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[creature preview info]", 0x26f2));
                }
                else
                {
    script->m_370 = ScanInt(&ok);
    break;
                }
            case 0x26f6:  // "[consume item]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[consume item]", 0x26f6));
                }
                else
                {
    for (;;)
    {
        int first = ScanInt(&ok);
        if (!ok)
            break;
        int second = ScanInt(&ok);
        if (!ok)
            break;
        script->m_vec374.push_back(std::pair<int, int>(first, second));  // ORIG EOS1
    }
    break;
                }
            case 0x2704:  // "[avatar emblem target type]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[avatar emblem target type]", 0x2704));
                }
                else
                {
    {
        while (ScanStr(&token))
        {
            int type = getAvatarEmblemType(token.c_str());
            if (type == 0)
                return false;
            script->m_380 = static_cast<unsigned short>(script->m_380 | type);
        }
    }
    break;
                }
            case 0x2711:  // "[usable date]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[usable date]", 0x2711));
                }
                else
                {
    ScanInt(&ok);
    ScanInt(&ok);
    break;
                }
            case 0x271a:  // "[item group name]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[item group name]", 0x271a));
                }
                else
                {
    {
        if (!ScanStr(&token))
        {
            return false;
        }
        std::string grp(token);   // ORIG -0x15c0 拷贝
        std::map<std::string, int>::const_iterator it =
            getItemGroupNameTable().find(grp);
        if (it == getItemGroupNameTable().end())
        {
            LogManager::logFormat(
                1, "../RDARScriptItem.cpp", "ImportStackableScript", 0x2723,
                "Unknown item group name %s", grp.c_str());
            return false;
        }
        script->m_field100 = it->second;
    }
    break;
                }
            case 0x2729:  // "[npc gift disallowance]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[npc gift disallowance]", 0x2729));
                }
                else
                {
    {
        int v = ScanInt(&ok);
        if (!ok)
        {
            return false;
        }
        script->m_field104 = static_cast<char>(v != 1);
    }
    break;
                }
            case 0x272e:  // "[guild power war point]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[guild power war point]", 0x272e));
                }
                else
                {
    script->m_fieldc8 = ScanInt(&ok);
    break;
                }
            case 0x2731:  // "[user power war point]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[user power war point]", 0x2731));
                }
                else
                {
    script->m_fieldcc = ScanInt(&ok);
    break;
                }
            case 0x2734:  // "[guild exp amount]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[guild exp amount]", 0x2734));
                }
                else
                {
    script->m_384 = ScanInt(&ok);
    break;
                }
            case 0x2737:  // "[impossible contents]"
    //  "gift"→m_fieldd0=1；"trade"→m_fieldd1=1；"charac cargo"→m_fieldda=1；
    //  "ban redeemitem"→m_fieldd8=1；直到 ScanStr 失败
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[impossible contents]", 0x2737));
                }
                else
                {
    {
        std::string val;   // ORIG 局部串槽 -0x15c8（循环复用）
        while (ScanStr(&val))
        {
            if (val == "gift") { script->m_fieldd0 = 1; }
            else if (val == "trade") { script->m_fieldd1 = 1; }
            else if (val == "charac cargo") { script->m_fieldda = 1; }
            else if (val == "ban redeemitem") { script->m_fieldd8 = 1; }
        }
    }
    break;
                }
            case 0x274d:  // "[limit dungeon]"
    //  → insert m_set4d4(+0x4d4)
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[limit dungeon]", 0x274d));
                }
                else
                {
    {
        for (;;)
        {
            int v = 0;
            if (!ScanInt(&v))
            {
                break;
            }
            script->m_set4d4.insert(v);
        }
    }
    break;
                }
            case 0x2768:  // "[disable]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[disable]", 0x2768));
                }
                else
                {
    script->m_388 = 0;
    break;
                }
            case 0x276b:  // "[use item emoticon index]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[use item emoticon index]", 0x276b));
                }
                else
                {
    script->m_38c = ScanInt(&ok);
    break;
                }
            case 0x276e:  // "[use effect explain]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[use effect explain]", 0x276e));
                }
                else
                {
    script->m_390 = ScanInt(&ok);
    break;
                }
            case 0x2771:  // "[expert type]"
    //  "[doll_controller]"→4；其它 → 0
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[expert type]", 0x2771));
                }
                else
                {
    {
        ScanStr(&token);
        if (token == "[alchemist]") { script->m_3bc = 2; }
        else if (token == "[enchanter]") { script->m_3bc = 1; }
        else if (token == "[doll_controller]") { script->m_3bc = 4; }
        else { script->m_3bc = 0; }
    }
    break;
                }
            case 0x277d:  // "[bead item]"
    //  ScanInt b(ok); !ok 断；pair(a,b) push m_vec3c0(+0x3c0)}
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[bead item]", 0x277d));
                }
                else
                {
    {
        for (;;)
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
            script->m_vec3c0.push_back(std::pair<int, int>(a, b));  // ORIG EOS1
        }
    }
    break;
                }
            case 0x278c:  // "[monster card id]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[monster card id]", 0x278c));
                }
                else
                {
    script->m_3cc = ScanInt(&ok);
    break;
                }
            case 0x278f:  // "[special monster drop]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[special monster drop]", 0x278f));
                }
            case 0x2790:  // "[difficulty for extra drop]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[difficulty for extra drop]", 0x2790));
                }
            case 0x2791:  // "[dungeon type for extra drop]"
//  三者共用单一 helper 调用点（ORIG StackableScriptSpecialMonsterDrop 全函数仅 call x1）
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[dungeon type for extra drop]", 0x2791));
                }
                else
                {
    if (!StackableScriptSpecialMonsterDrop(script, line, path))
    {
        return false;
    }
break;
                }
            case 0x2795:  // "[routing priority]"
    //  循环 {ScanStr 子行；"[/routing priority]" 断；job=getCharacterJobToEnumValue；
    //  !=-1 时 ScanInt pri(ok)；!ok 或 pri<=0 或 >4 → return false；pair(job,pri)
    //  push m_vec110(+0x110)}
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[routing priority]", 0x2795));
                }
                else
                {
    {
        script->m_field10f = 1;
        std::string val;   // ORIG 局部串槽 -0x15d4
        while (ScanStr(&val))
        {
            if (val == "[/routing priority]")
            {
                break;
            }
            int job = getCharacterJobToEnumValue(val);
            if (job == -1)
            {
                break;
            }
            int pri = ScanInt(&ok);
            if (!ok)
            {
                break;
            }
            if (pri <= 0 || pri > 4)
            {
                return false;
            }
            script->m_vec110.push_back(std::pair<int, int>(job, pri));  // ORIG EOS1
        }
    }
    break;
                }
            case 0x27b0:  // "[use 2nd upgrade table]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[use 2nd upgrade table]", 0x27b0));
                }
                else
                {
    script->m_3d0 = 1;
    break;
                }
            case 0x27b3:  // "[do not delete]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[do not delete]", 0x27b3));
                }
                else
                {
    if (script->m_1e0 == 0x10)
    {
        script->m_3d1 = 0;
    }
    break;
                }
            case 0x27ba:  // "[maximum level]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[maximum level]", 0x27ba));
                }
                else
                {
    script->m_1ac = ScanInt(&ok);
    break;
                }
            case 0x27c0:  // "[arad_item_kind]"
    //  失败 → return false
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[arad_item_kind]", 0x27c0));
                }
                else
                {
    if (!ScanStr(&script->m_arad178.m_str8))
    {
        return false;
    }
    break;
                }
            case 0x27c4:  // "[usable first growtype]"
    //  !ok → return false
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[usable first growtype]", 0x27c4));
                }
                else
                {
    {
        int v = ScanInt(&ok);
        script->m_arad178.m_4 = static_cast<char>(v);
        if (!ok)
        {
            return false;
        }
    }
    break;
                }
            case 0x27c9:  // "[arad_use]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[arad_use]", 0x27c9));
                }
                else
                {
    {
        int v = ScanInt(&ok);
        script->m_arad178.m_0 = v;
        if (!ok)
        {
            return false;
        }
    }
    break;
                }
            case 0x27cd:  // "[mail from]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[mail from]", 0x27cd));
                }
                else
                {
    if (!ScanStr(&script->m_arad178.m_strc))
    {
        return false;
    }
    break;
                }
            case 0x27d0:  // "[mail content]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[mail content]", 0x27d0));
                }
                else
                {
    if (!ScanStr(&script->m_arad178.m_str10))
    {
        return false;
    }
    break;
                }
            case 0x27d7:  // "[onemore check]"
    //  ScanStr(&m_str410(+0x410))；!ok 仅结束本分支并继续主循环
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[onemore check]", 0x27d7));
                }
                else
                {
    {
        int v = ScanInt(&ok);
        if (ok)
        {
            (void)v;
            script->m_414 = 1;
            ScanStr(&script->m_str410);
        }
    }
    break;
                }
            case 0x27e1:  // "[evolution creature]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[evolution creature]", 0x27e1));
                }
                else
                {
    {
        int v = ScanInt(&ok);
        script->m_418 = v;
        if (!ok)
        {
            return false;
        }
    }
    break;
                }
            case 0x2802:  // "[3choro enchant]"
    //  段1（089b8267..089b8351）：ScanType 逐行，"[probability]" 行 2×ScanInt 写
    //  m_40d[0/1]（任一 !ok → return false）；首行非 [probability] 即转段2。
    //  段2：行 "[/3choro enchant]" → 完成本段；行 "[check]" → 条目组处理
    //  （089b8433 起，key=STEnchantSystemMapKey/-0x15ec、val=STEnchantSystemMapData
    //  /-0x15fc）：
    //    key.m_a=ScanInt（94c3..94d1）→ key.m_b(byte)=ScanInt（94f4..9502）→
    //    ScanStr(equipType) → key.m_c=getEquipmentTypeToEnumValue(equipType)
    //    （9557..9565，-0x15e4=key+8）→ 内层循环 ScanType：
    //      "[skill]"：key.m_fieldC(byte)=ScanInt（95b5..95e1，-0x15e0=key+0xc）后
    //        importSkillDataUp(val.m_vec0)（ORIG 0x88c049c 自读行）；
    //      "[skill explain]"：ScanStr(&val.m_strC)（9648..，-0x15fc+0xc）；
    //      "[/check]"：结束条目组 → m_enchant3f4.insert(make_pair(key,val))
    //        （089c73fe map insert；insert 位置/重复组语义见 Blocked 注）。
    //  段2 其它行 → return false（ORIG 089b86ad 出口）。
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[3choro enchant]", 0x2802));
                }
                else
                {
    {
        // 段1
        for (;;)
        {
            if (!ScanType(line, true))
            {
                return false;   // 93bc
            }
            if (line == "[probability]")
            {
                script->m_40d[0] = static_cast<char>(ScanInt(&ok));
                if (!ok)
                {
                    return false;
                }
                script->m_40d[1] = static_cast<char>(ScanInt(&ok));
                if (!ok)
                {
                    return false;
                }
            }
            else
            {
                break;          // 非 [probability] → 段2（line 已就绪）
            }
        }
        // 段2
        for (;;)
        {
            if (line == "[/3choro enchant]")
            {
                break;          // 完成本段（9484 → 公共 stub）
            }
            else if (line == "[check]")
            {
                STEnchantSystemMapKey key;      // -0x15ec（基类 ctor clear + m_fieldC=0）
                STEnchantSystemMapData val;     // -0x15fc（vec0 + m_strC）
                key.m_a = ScanInt(&ok);         // 94c3..94d1
                if (!ok)
                {
                    return false;
                }
                key.m_b = static_cast<unsigned char>(ScanInt(&ok));  // 94f4..9502
                if (!ok)
                {
                    return false;
                }
                std::string eqType;
                if (!ScanStr(&eqType))          // 9525..9552（-0x1600）
                {
                    return false;
                }
                key.m_c = getEquipmentTypeToEnumValue(eqType);  // 9557..9565
                for (;;)
                {
                    if (!ScanType(line, true))
                    {
                        return false;           // 内层 ScanType 失败
                    }
                    if (line == "[skill]")
                    {
                        key.m_fieldC = static_cast<unsigned char>(ScanInt(&ok));  // 95b5..95e1
                        if (!ok)
                        {
                            return false;
                        }
                        val.m_vec0.clear();
                        importSkillDataUp(val.m_vec0);  // 95fd（0x88c049c，自读行）
                        if (!ScanType(line, true))
                        {
                            return false;
                        }
                        // ORIG 089b8507..089b85d4：**[skill explain] 只比较一次**
                        // （相等则 ScanStr(&val.m_strC)，随后无条件 make_pair+insert
                        // 并 jmp 回内层循环头）；原代码在 insert 后又比较一次并
                        // return false 是多余检查（p167 EXTRA operator== +1 实证），
                        // ORIG 无该分支，删除。
                        if (line == "[skill explain]")
                        {
                            ScanStr(&val.m_strC);          // ORIG 忽略返回值
                        }
                        script->m_enchant3f4.insert(std::make_pair(key, val));
                    }
                    else if (line == "[/check]")
                    {
                        break;
                    }
                    else
                    {
                        return false;           // 内层未知行
                    }
                }
            }
            else
            {
                return false;   // 段2 未知行（089b86ad）
            }
            // ORIG 0x089b89da：段2 每组处理完读取下一行
            if (!ScanType(line, true))
            {
                return false;
            }
        }
    }
    break;
                }
            case 0x284a:  // "[type]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[type]", 0x284a));
                }
                else
                {
    {
        int v = ScanInt(&ok);
        script->m_40c = static_cast<char>(v);
        if (!ok)
        {
            return false;
        }
    }
    break;
                }
            case 0x2850:  // "[upgrade limit cube info]"
    //  至 "[/upgrade limit cube info]"；"[A condition item]" 读 pair push
    //  m_cube3d4.m_materials(+0x3d4)；"[B condition item]" 2 int 写 +0x3e0/+0x3e4
    //  （m_cube3d4.m_2ndMaterialIdx/Count）；"[result item]" 3 int push m_cube3d4
    //  .m_results(+0x3e8)
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[upgrade limit cube info]", 0x2850));
                }
                else
                {
    {
        for (;;)
        {
            std::string sub;
            if (!ScanType(sub, true))
            {
                break;
            }
            if (sub == "[/upgrade limit cube info]")
            {
                break;
            }
            else if (sub == "[A condition item]")
            {
                int a = ScanInt(&ok);
                if (!ok)
                    continue;
                int b = ScanInt(&ok);
                if (ok)
                {
                    script->m_cube3d4.m_materials.push_back(
                        std::make_pair(a, b));
                }
            }
            else if (sub == "[B condition item]")
            {
                int index = ScanInt(&ok);
                if (!ok)
                    break;
                int count = ScanInt(&ok);
                if (!ok)
                    break;
                script->m_cube3d4.m_2ndMaterialIdx = index;
                script->m_cube3d4.m_2ndMaterialCount = count;
            }
            else if (sub == "[result item]")
            {
                int a = ScanInt(&ok);
                if (!ok)
                    continue;
                int b = ScanInt(&ok);
                if (!ok)
                    continue;
                int c = ScanInt(&ok);
                if (ok)
                {
                    // ORIG 0x089b8982 同上：仅 C1 调用点，无 D1（N2 豁免）。
                    stLimitCubeResult r;
                    r.m_itemIdx = a;
                    r.m_count = b;
                    r.m_prob = c;
                    script->m_cube3d4.m_results.push_back(r);
                }
            }
        }
    }
    break;
                }
            case 0x2894:  // "[usable world]"
    //  循环 {scanWorldType()==2 停；insert 到 m_setdc(+0xdc)}
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[usable world]", 0x2894));
                }
                else
                {
    {
        script->m_setdc.clear();
        // ORIG 0x089b8a8b/0x089b8ac0：预读 + 循环尾再读（两处调用点）
        int world = scanWorldType();
        while (world != 2)
        {
            script->m_setdc.insert(world);
            world = scanWorldType();
        }
    }
    break;
                }
            case 0x28ae:  // "[minimum itemmaking level]"
//  静态子表(11)+双检锁 find → m_444(+0x444)=装备位；未识别 → return false；
//  命中后 ScanInt(&m_448)（失败 → return false）。
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[minimum itemmaking level]", 0x28ae));
                }
                else
                {
    {
        std::string val;   // ORIG 局部串槽 -0x1620
        if (!ScanStr(&val))
        {
            return false;   // 089b8b57..8b6d
        }
    static std::map<char const*, int, less_TCHAR> __lookup_4;
    static SpinLock __init_lock_4;
    static bool __inited_4 = false;
    int sub;
    int pos;   // ORIG -0x34(%ebp)：case 命中的装备位暂存，循环出口统一写 +0x444
    for (;;)
    {
        if (__inited_4)
        {
            std::map<char const*, int, less_TCHAR>::const_iterator it4 = __lookup_4.find(val.c_str());
            sub = (it4 != __lookup_4.end()) ? it4->second : -1;
        }
        else
        {
            __init_lock_4.enter();
            if (__inited_4)
            {
                __init_lock_4.leave();
                continue;
            }
            sub = 0;
        }
        switch (sub)
        {
            case 0:
            case 0x28b6:  // "weapon"
                if (!__inited_4)
                {
                    __lookup_4.insert(std::map<char const*, int, less_TCHAR>::value_type("weapon", 0x28b6));
                }
                else
                {
    pos = 0; break;                // "weapon"
                }
            case 0x28b9:  // "cloth"
                if (!__inited_4)
                {
                    __lookup_4.insert(std::map<char const*, int, less_TCHAR>::value_type("cloth", 0x28b9));
                }
                else
                {
    pos = 1; break;                // "cloth"
                }
            case 0x28bc:  // "leather"
                if (!__inited_4)
                {
                    __lookup_4.insert(std::map<char const*, int, less_TCHAR>::value_type("leather", 0x28bc));
                }
                else
                {
    pos = 2; break;                // "leather"
                }
            case 0x28bf:  // "larmor"
                if (!__inited_4)
                {
                    __lookup_4.insert(std::map<char const*, int, less_TCHAR>::value_type("larmor", 0x28bf));
                }
                else
                {
    pos = 3; break;                // "larmor"
                }
            case 0x28c2:  // "harmor"
                if (!__inited_4)
                {
                    __lookup_4.insert(std::map<char const*, int, less_TCHAR>::value_type("harmor", 0x28c2));
                }
                else
                {
    pos = 4; break;                // "harmor"
                }
            case 0x28c5:  // "plate"
                if (!__inited_4)
                {
                    __lookup_4.insert(std::map<char const*, int, less_TCHAR>::value_type("plate", 0x28c5));
                }
                else
                {
    pos = 5; break;                // "plate"
                }
            case 0x28c8:  // "amulet"
                if (!__inited_4)
                {
                    __lookup_4.insert(std::map<char const*, int, less_TCHAR>::value_type("amulet", 0x28c8));
                }
                else
                {
    pos = 6; break;                // "amulet"
                }
            case 0x28cb:  // "wrist"
                if (!__inited_4)
                {
                    __lookup_4.insert(std::map<char const*, int, less_TCHAR>::value_type("wrist", 0x28cb));
                }
                else
                {
    pos = 7; break;                // "wrist"
                }
            case 0x28ce:  // "ring"
                if (!__inited_4)
                {
                    __lookup_4.insert(std::map<char const*, int, less_TCHAR>::value_type("ring", 0x28ce));
                }
                else
                {
    pos = 8; break;                // "ring"
                }
            case 0x28d1:  // "support"
                if (!__inited_4)
                {
                    __lookup_4.insert(std::map<char const*, int, less_TCHAR>::value_type("support", 0x28d1));
                }
                else
                {
    pos = 9; break;                // "support"
                }
            case 0x28d4:  // "magic stone"
                if (!__inited_4)
                {
                    __lookup_4.insert(std::map<char const*, int, less_TCHAR>::value_type("magic stone", 0x28d4));
                }
                else
                {
    pos = 0xa; break;              // "magic stone"
                }
            default:
    return false;   // ORIG default(089b9284) → return false
        }
        if (__inited_4)
        {
            break;
        }
        __init_lock_4.leave();
        __inited_4 = true;
    }
    script->m_444 = pos;   // ORIG 0x089b92c2..0x089b92c8：循环出口单点写
        if (!ScanInt(&script->m_448))
        {
            return false;   // 089b92d9..92ef
        }
    }
break;
                }
            case 0x28e2:  // "[expertjob only]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[expertjob only]", 0x28e2));
                }
                else
                {
    if (!GetExpertJobTypeList(script))
    {
        return false;
    }
break;
                }
            case 0x28e6:  // "[prof material variation]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[prof material variation]", 0x28e6));
                }
                else
                {
    if (!GetExpertJobValueMaterialVariation(script))
    {
        return false;
    }
break;
                }
            case 0x28ea:  // "[prof compound rate]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[prof compound rate]", 0x28ea));
                }
                else
                {
    if (!GetExpertJobValueCompoundRate(script))
    {
        return false;
    }
break;
                }
            case 0x28ee:  // "[portable disjoint]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[portable disjoint]", 0x28ee));
                }
                else
                {
    script->m_44c = ScanInt(&ok);
    if (!ok)
        return false;
    break;
                }
            case 0x28f3:  // "[shop doll]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[shop doll]", 0x28f3));
                }
                else
                {
    script->m_450 = ScanInt(&ok);
    if (!ok)
        return false;
    break;
                }
            case 0x28f8:  // "[unseal scroll level limit]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[unseal scroll level limit]", 0x28f8));
                }
                else
                {
    script->m_454 = ScanInt(&ok);
    if (!ok)
        return false;
    break;
                }
            case 0x28fd:  // "[monstercard bind]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[monstercard bind]", 0x28fd));
                }
                else
                {
    script->m_458 = ScanInt(&ok);
    if (!ok)
        return false;
    break;
                }
            case 0x2902:  // "[alchemist extraction]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[alchemist extraction]", 0x2902));
                }
                else
                {
    script->m_45c = ScanInt(&ok);
    if (!ok)
        return false;
    break;
                }
            case 0x2907:  // "[enchanter extraction]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[enchanter extraction]", 0x2907));
                }
                else
                {
    script->m_460 = ScanInt(&ok);
    if (!ok)
        return false;
    break;
                }
            case 0x290c:  // "[doll_controller extraction]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[doll_controller extraction]", 0x290c));
                }
                else
                {
    script->m_464 = ScanInt(&ok);
    if (!ok)
        return false;
    break;
                }
            case 0x2912:  // "[unlimited waste]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[unlimited waste]", 0x2912));
                }
                else
                {
    script->m_470 = ScanInt(&ok);
    if (!ok)
    {
        return false;
    }
    break;
                }
            case 0x291b:  // "[item category]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[item category]", 0x291b));
                }
                else
                {
    if (!GetItemCategory(script))
    {
        return false;
    }
break;
                }
            case 0x291e:  // "[usable game server type]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[usable game server type]", 0x291e));
                }
                else
                {
    script->m_468 = ScanInt(&ok);
    if (!ok)
        return false;
    break;
                }
            case 0x2924:  // "[levelup range]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[levelup range]", 0x2924));
                }
                else
                {
    script->m_46c = ScanInt(&ok);
    if (!ok)
    {
        return false;
    }
    break;
                }
            case 0x292b:  // "[lottery use cost]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[lottery use cost]", 0x292b));
                }
                else
                {
    script->m_474 = ScanInt(&ok);
    if (!ok)
        return false;
    break;
                }
            case 0x2932:  // "[chat notice gold]"
    //  第一值仅暂存（-0x2c），第二值写 m_478(+0x478)；任一 !ok → return false。
    //  "[/chat notice gold]"（0x293a）在主表为纯结束标记，命中即继续（等价 default）。
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[chat notice gold]", 0x2932));
                }
                else
                {
    {
        int v1 = ScanInt(&ok);
        if (!ok)
        {
            return false;   // 089b9bc0
        }
        int v2 = ScanInt(&ok);
        // ORIG 089b9b44：第二次 ScanInt 的返回值先写入 m_478，
        // 再检查 ok；失败时字段也已被覆盖为 0。
        script->m_478 = v2;
        if (!ok)
        {
            return false;   // 089b9bf3
        }
        (void)v1;
    }
    break;
                }
            case 0x293a:  // "[/chat notice gold]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[/chat notice gold]", 0x293a));
                }
                else
                {
break;
                }
            case 0x293e:  // "[avatar select ability change]"
    // getCharacterJob/GetEquipmentType 解析两类字符串；未识别值仅允许 [all]，
    // 随后写 m_avatar47c 前两字节、变化类型及 m_4b0 标志。
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[avatar select ability change]", 0x293e));
                }
                else
                {
    {
        if (!ScanStr(&token))
        {
            return false;
        }
        int job = getCharacterJob(token.c_str());
        if (job == 0xb && token != "[all]")
            return false;
        const unsigned char avatarJob = static_cast<unsigned char>(job);
        if (!ScanStr(&token))
        {
            return false;
        }
        int equipmentType = GetEquipmentType(token.c_str());
        if (equipmentType == 0x1a && token != "[all]")
            return false;
        const unsigned char avatarEquipmentType =
            static_cast<unsigned char>(equipmentType);
        int v = ScanInt(&ok);
        if (!ok)
        {
            return false;
        }
        if (v < 0 || v > 3)
        {
            return false;
        }
        // ORIG 0x089b9cc5：局部 AvatarOptionChangeType(-0x1627) 先 clear 链初始化，
        // 三字节全部有效后一次拷贝回 m_avatar47c(+0x47c) 并置 m_4b0。
        AvatarOptionChangeType avatar;
        avatar.m_field0 = avatarJob;
        avatar.m_field1 = avatarEquipmentType;
        avatar.m_field2 = static_cast<unsigned char>(v);
        script->m_avatar47c = avatar;
        script->m_4b0 = 1;
    }
    break;
                }
            case 0x2962:  // "[usable avatar index]"
    //  m_4b0=1 后继续）；值>0 时 set insert m_set480(+0x480)，重复 → logFormat +
    //  return false
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[usable avatar index]", 0x2962));
                }
                else
                {
    {
        int v = ScanInt(&ok);
        if (!ok)
        {
            script->m_4b0 = 1;
            break;
        }
        if (v <= 0)
        {
            // ORIG 089b9e4f：有效但小于 1 的值直接失败，不先置
            // m_4b0；该标志只在“列表正常结束/首个 ScanInt 失败”时置位。
            return false;
        }
        if (!script->m_set480.insert(v).second)
        {
            LogManager::logFormat(
                0, "../RDARScriptItem.cpp", "ImportStackableScript", 0x296f,
                "[usable avatar index] is duplicated");
            return false;
        }
    }
    break;
                }
            case 0x2975:  // "[avatar select ability except]"
    //  直接 break）；值>=0 时 insert m_set498(+0x498)，重复 → return false
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[avatar select ability except]", 0x2975));
                }
                else
                {
    {
        int v = ScanInt(&ok);
        if (!ok)
        {
            break;
        }
        if (v < 0)
        {
            return false;
        }
        if (!script->m_set498.insert(v).second)
        {
            LogManager::logFormat(
                0, "../RDARScriptItem.cpp", "ImportStackableScript", 0x2982,
                "[avatar select ability except] is duplicated");
            return false;
        }
    }
    break;
                }
            case 0x2989:  // "[dye type]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[dye type]", 0x2989));
                }
                else
                {
    {
        int v = ScanInt(&ok);
        if (ok)
        {
            // DyeInfo 的数据成员为 private；此 view 只复现已验证的
            // {int, vector<pair<int,int>>} 布局，不改变 ABI。
            struct DyeInfoView
            {
                int m_field0;
                std::vector<std::pair<int, int> > m_vec4;
            };
            DyeInfoView* dye = reinterpret_cast<DyeInfoView*>(&script->m_dyeInfo);
            dye->m_field0 = v;
        }
    }
    break;
                }
            case 0x2993:  // "[dye info]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[dye info]", 0x2993));
                }
                else
                {
    {
        struct DyeInfoView
        {
            int m_field0;
            std::vector<std::pair<int, int> > m_vec4;
        };
        DyeInfoView* dye = reinterpret_cast<DyeInfoView*>(&script->m_dyeInfo);
        int first = 0;
        int second = 0;
        while (true)
        {
            first = ScanInt(&ok);
            if (!ok)
            {
                break;
            }
            second = ScanInt(&ok);
            if (!ok)
            {
                break;
            }
            dye->m_vec4.push_back(std::make_pair(first, second));
        }
    }
    break;
                }
            case 0x29a9:  // "[random option unseal]"
    //  ScanInt → m_4b8(+0x4b8)
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[random option unseal]", 0x29a9));
                }
                else
                {
    script->m_4b8 = 0;
    script->m_4b8 = ScanInt(&ok);
    break;
                }
            case 0x29ae:  // "[stat desc]"
//  return false；成功 op= 写 m_str4b4(+0x4b4)。
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[stat desc]", 0x29ae));
                }
                else
                {
    {
        std::string val;
        if (!ScanStr(&val))
        {
            return false;
        }
        script->m_str4b4 = val;
    }
    break;
                }
            case 0x29bb:  // "[success rate]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[success rate]", 0x29bb));
                }
                else
                {
    script->m_4f0 = ScanInt(&ok);
    break;
                }
            case 0x29bf:  // "[broadcast type]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[broadcast type]", 0x29bf));
                }
                else
                {
    script->m_4f4 = ScanInt(&ok);
    break;
                }
            case 0x29c3:  // "[maintain ability]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[maintain ability]", 0x29c3));
                }
                else
                {
    script->m_4f8 = ScanInt(&ok);
    break;
                }
            case 0x29c7:  // "[fail output]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[fail output]", 0x29c7));
                }
                else
                {
    {
        script->m_vec4fc.clear();
        for (;;)
        {
            int v = ScanInt(&ok);
            if (!ok)
            {
                break;
            }
            script->m_vec4fc.push_back(v);
        }
    }
    break;
                }
            case 0x29d5:  // "[trade limit max]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[trade limit max]", 0x29d5));
                }
                else
                {
    {
        int v = ScanInt(&ok);
        if (ok)
        {
            script->m_508 = v;
        }
    }
    break;
                }
            case 0x29df:  // "[target item id]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[target item id]", 0x29df));
                }
                else
                {
    {
        for (;;)
        {
            int v = ScanInt(&ok);
            if (!ok)
            {
                break;
            }
            script->m_set50c.insert(v);
        }
    }
    break;
                }
            default:
                break;
        }
        if (__inited)
        {
            break;
        }
        __init_lock.leave();
        __inited = true;
    }
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


// [b28d-solo-STStackableScript] AE（172=172）：local_d 由 char 改 bool（P136/P32：
// bool 源→bool 字段直存，char 源恒多 `cmpb $0; setne`），`local_15 != true`
// 改 `!local_15`（ORIG 089ae947 `movzbl; xor $1; test; je` = N122 取反形）。
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
    bool local_d;
    if (str == "[special monster drop]")
    {
        local_15 = false;
        local_d = ScanInt(&local_15) != 0;
        if (!local_15)
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
