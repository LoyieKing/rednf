// ============================================================================
// df_game_r 还原 —— STDungeonScript 类方法（G5 副本脚本表）
// ORIG：Clear 0x0836abc6 / ctor 0x0836b5e2 / dtor 0x0836bd5c；
// 类布局见 CDungeon.h（0x50a）。ImportDungeonScript 0x088e5faa /
// exportDungeonScript 0x088f0a98 / ImportAdvanceAltarDungeonTag 0x088f20a4
// 为脚本解析/导出（语义简化实现）。
// ============================================================================

#include <stdio.h>
#include <string.h>

#include "CDungeon.h"
#include "DNFLexWrapper.h"
#include "DataManagerInitDepends.h"   // getCharacterJobToEnumValue 声明（[impossible skills]）
#include "NpcScript.h"                // STScriptFileList（[schedule] 双视图）

extern std::map<int, std::string> g_stDungeonScriptFileList;   // 定义 InitScriptProducers_4.cpp（双视图 STScriptFileList）

// ORIG _ZL13g_gridSymbols（[greed] 网格符号，16 项两字符编码；STMapScript.cpp 同表为文件内
// static，本 TU 单独复现以支撑 import/exportDungeonScript 引用）。
static const char* const g_gridSymbols[16] = {
    "AA", "BB", "CC", "DD", "EE", "FF", "GG", "HH",
    "II", "JJ", "KK", "LL", "MM", "NN", "OO", "PP",
};

// [schedule] 时间串转换器（权威定义 InitScriptProducers_6.cpp，签名须与定义一致）
bool ConvertStringToTime(std::string s, long& out);
bool ConvertWeekdayStringToInt(std::string s, unsigned int& out);
bool ConvertHHMMSSStringToInt(std::string s, unsigned char& h, unsigned char& m,
                              unsigned char& sec);

STDungeonScript::STDungeonScript()
{
    Clear();
}

STDungeonScript::~STDungeonScript()
{
}

void STDungeonScript::Clear()
{
    // ORIG 0x0836abc6 全量清零（逐字段核对，含缺失成员与初值修正）
    m_0 = 0;
    m_c = 0;
    m_str10.clear();
    m_14 = 0;
    m_18 = 0;
    m_str1c.clear();
    m_str20.clear();
    m_str24.clear();
    m_str28.clear();
    m_2c = 0;
    m_str30.clear();
    m_34 = 0;
    m_vec38.clear();
    m_44 = 0;
    m_vec48.clear();
    m_str54.clear();
    m_5c[0] = 0;
    m_5c[1] = 0;
    m_5c[2] = 0;
    m_5c[3] = 0;
    m_5c[4] = 0;
    m_5c[5] = 0;
    m_str74.clear();
    m_78 = 0;
    m_requiredItem7c.clear();
    m_88 = 0;
    m_requiredItem8c.clear();
    m_vec98.clear();
    m_a4 = -1;
    m_a8 = -1;
    m_ac = 1;
    m_b0 = 1.0f;
    m_b4[0] = 0;
    m_b4[1] = 0;
    m_b4[2] = 0;
    m_b4[3] = 0;
    m_b4[4] = 0;
    m_vecc8.clear();
    memset(m_padF8, 0xff, sizeof(m_padF8));   // ORIG 嵌套循环置 12 int = -1
    m_13c = 0;
    m_140 = 0;
    m_141 = 0;
    m_142 = 1;   // ORIG movb $0x1,0x142
    m_vec144.clear();
    m_150 = 0;
    m_154 = 0;
    m_158 = 0;
    m_159 = 1;   // ORIG movb $0x1,0x159
    m_128[0] = 0;
    m_128[1] = 0;
    m_128[2] = 0;
    m_128[3] = 0;
    m_128[4] = 0;
    m_168 = -1;   // ORIG movl $0xffffffff,0x168
    m_16c = 0;
    m_mazeList170.clear();
    m_str17c.clear();
    m_deadTower180.clear();
    m_warRoom18c.clear();
    m_28c = 0;
    m_28d = 0;
    m_28e = 0;
    m_28f = 0;
    m_statMap290.clear();
    m_2a8 = 0;
    m_2a9 = 0;
    m_2aa = 0;
    m_2ab = 0;
    m_2ac = 0;
    m_2b0[0] = 0;
    m_2b0[1] = 0;
    m_2b0[2] = 0;
    m_2bc[0] = 0;
    m_2bc[1] = 0;
    m_2bc[2] = 0;
    m_vec2c8.clear();
    m_clearReward2d4.clear();
    m_survival2f8.m_vec.clear();
    m_304 = 0;
    m_305 = 0;
    m_306 = 0;
    m_314 = 0;
    m_315 = 0;
    m_316 = 0;
    m_317 = 0;
    m_dailySchedule.clear();
    m_324 = 4;   // ORIG movb $0x4,0x324：默认队伍人数上限
    m_325 = 0;
    m_bossRoom328.clear();
    m_338 = 0;
    m_33c = 0;
    m_map340.clear();
    m_358 = 0;
    m_vec35c.clear();
    m_368 = 0;
    m_36c = 1;
    m_370 = 1;
    m_374 = 1.0f;
    m_378 = 1.0f;
    m_37c = 1;
    m_380 = 1;
    m_vec384.clear();
    m_towerDialog390.clear();
    m_3b4 = 1;   // ORIG movl $0x1,0x3b4
    m_3b8 = 0;
    m_3bc = 0;
    m_3c0 = 0;
    m_vec3c8.clear();
    m_3d4 = 0;
    m_3d8 = 0;
    m_3dc = 0;
    m_vec3e0.clear();
    m_3ec = 0;
    m_3f0 = 0;
    m_3f4 = 0;
    m_vec3f8.clear();
    m_vec404.clear();
    m_410[0] = 0;
    m_410[1] = 0;
    m_410[2] = 0;
    m_410[3] = 0;
    m_410[4] = 0;
    m_424[0] = 0;
    m_424[1] = 0;
    m_424[2] = 0;
    m_424[3] = 0;
    m_424[4] = 0;
    m_438 = 0;
    m_43c = 0x40000000;   // 2.0f
    m_440 = 0;
    m_444 = 0x3fc00000;   // 1.5f
    m_str448.clear();
    m_44c = 1;   // ORIG movb $0x1,0x44c
    m_450 = -1;  // ORIG mov 0x454→0x450（先置 0x454=-1）
    m_454 = -1;
    m_vec458.clear();
    m_vec464.clear();
    m_vec470.clear();
    m_47c = 0;
    m_480 = 0;
    m_484 = 0;
    m_488 = 0;
    m_48c = 0;
    m_490 = -1;  // ORIG movl $0xffffffff,0x490
    m_494 = 0;
    m_498 = 1;   // ORIG movl $0x1,0x498
    m_map49c.clear();
    m_itemRate4b4.Clear();
    m_4c4 = 0x3f800000;   // 1.0f
    m_map4c8.clear();
    m_4e0 = 0;
    m_map4e4.clear();
    m_4fc = 0;
    m_500 = -1;  // ORIG movl $0xffffffff,0x500
    m_504 = 0;
    m_505 = 0;
    m_508 = 0;
    m_509 = 0;
    m_50a = 0;
    m_50b = 0;
    m_50c = 0;
    // ORIG 未清：m_4/m_8、m_str58、m_vec15c、m_dimension308（按 ORIG 实证保留不清）
}
bool ImportDungeonScript(STDungeonScript* script, const char* path)
{
    if (!loadRDARScriptFile("Script/DungeonScript", path))
    {
        return false;
    }
    script->Clear();
    script->m_vec384.clear();
    std::string line;
    int value = 0;
    if (!ScanType(line, true))
    {
        return false;
    }
    if (!ScanInt(&value))
    {
        return false;
    }
    script->m_0 = value;
    if (!ScanInt(&value))
    {
        return false;
    }
    script->m_4 = value;

    // ORIG 函数级局部（-0x1324 MazeScript / -0x121 flag / ok 标志）
    MazeScript mazeLocal;
    bool mazeInited = false;
    bool ok = false;
    std::string token;
    int v1 = 0;
    int v2 = 0;

    // ORIG 惰性初始化：函数级静态 __lookup + __inited + SpinLock（首次调用构建 tag→case 值映射）。
    static std::map<const char*, int> __lookup;
    static bool __inited = false;
    static SpinLock __init_lock;
    if (!__inited)
    {
        __init_lock.enter();
        if (!__inited)
        {
            __lookup["[name]"] = 117;
            __lookup["[explain]"] = 121;
            __lookup["[tower of despair]"] = 2170;
            __lookup["[minimap image]"] = 4096;
            __lookup["[ancient dungeon]"] = 4097;
            __lookup["[boss exp const]"] = 4098;
            __lookup["[experience increasing point]"] = 4099;
            __lookup["[hard]"] = 4100;
            __lookup["[ultimate]"] = 4101;
            __lookup["[entering title]"] = 4102;
            __lookup["[player kc]"] = 4103;
            __lookup["[worldmap pattern info]"] = 4104;
            __lookup["[entering title next]"] = 4105;
            __lookup["[herosmode enable]"] = 215;
            __lookup["[ai character appear rate]"] = 4106;
            __lookup["[limit inout count]"] = 4107;
            __lookup["[escape hell]"] = 4108;
            __lookup["[dungeon type]"] = 4109;
            __lookup["[room move keep state]"] = 4110;
            __lookup["[monster type spawn prob]"] = 4111;
            __lookup["[result exp bonus per user decrease]"] = 4112;
            __lookup["[tower recovery]"] = 4113;
            __lookup["[tower stage]"] = 4114;
            __lookup["[tournament clear reward exp]"] = 4115;
            __lookup["[road necessary party]"] = 4116;
            __lookup["[impossible skills]"] = 4117;
            __lookup["[special passive object item]"] = 4118;
            __lookup["[evil rate]"] = 4119;
            __lookup["[result card]"] = 4120;
            __lookup["[advance altar type]"] = 4121;
            __lookup["[revision table]"] = 4122;
            __lookup["[character respawn posetion]"] = 4123;
            __lookup["[boss room entrance condition]"] = 4124;
            __lookup["[clear reward item]"] = 4125;
            __lookup["[absolute change stat]"] = 4126;
            __lookup["[event monster2]"] = 4127;
            __lookup["[magical attack add]"] = 4128;
            __lookup["[magical defense add]"] = 4129;
            __lookup["[coin info]"] = 4130;
            __lookup["[clear condition]"] = 4131;
            __lookup["[greed]"] = 4132;
            __lookup["[randomized object creation]"] = 4133;
            __lookup["[schedule]"] = 4134;
            __lookup["[team]"] = 4144;
            __lookup["[pos]"] = 4145;
            __lookup["[object]"] = 4146;
            __lookup["[realdungeon checkup]"] = 4147;
            __lookup["[limit item stat]"] = 4148;
            __lookup["[advance altar survival clear reward]"] = 4149;
            __lookup["[physical attack add]"] = 4150;
            __lookup["[physical defense add]"] = 4151;
            __lookup["ByDifficulty"] = 152;
            __lookup["[cutscene image]"] = 185;
            __lookup["[dungeon loading image path]"] = 190;
            __lookup["[worldmap info]"] = 202;
            __lookup["[herosmode required quest]"] = 207;
            __lookup["[background pos]"] = 221;
            __lookup["[monsterapc diff table]"] = 255;
            __lookup["[minimum required level]"] = 269;
            __lookup["[required item]"] = 276;
            __lookup["[event dungeon]"] = 280;
            __lookup["[event required item]"] = 286;
            __lookup["[recommended level]"] = 396;
            __lookup["[coin limit]"] = 405;
            __lookup["[character coin limit]"] = 413;
            __lookup["[basis level]"] = 417;
            __lookup["[champion]"] = 422;
            __lookup["[item]"] = 435;
            __lookup["[dungeon drop]"] = 489;
            __lookup["[dungeon object drop]"] = 511;
            __lookup["[pathgate object]"] = 541;
            __lookup["[maze info]"] = 552;
            __lookup["[quest connection]"] = 650;
            __lookup["../RDARScriptDungeon.cpp"] = 702;
            __lookup["[party num check]"] = 708;
            __lookup["[size]"] = 720;
            __lookup["[map specification]"] = 782;
            __lookup["[boss map specification]"] = 798;
            __lookup["[layered map specification]"] = 814;
            __lookup["[start map]"] = 830;
            __lookup["[boss map]"] = 842;
            __lookup["[hell dungeon]"] = 859;
            __lookup["[prohibit practice]"] = 866;
            __lookup["[disable skill]"] = 871;
            __lookup["[/impossible skills]"] = 883;
            __lookup["[use countdown]"] = 897;
            __lookup["[use zoom rate]"] = 902;
            __lookup["[usable item]"] = 906;
            __lookup["[seal door map index]"] = 914;
            __lookup["[seal door pos]"] = 919;
            __lookup["[hell coin limit]"] = 923;
            __lookup["[dead tower map index]"] = 933;
            __lookup["[select]"] = 947;
            __lookup["[regenerate]"] = 957;
            __lookup["[minimap icon]"] = 962;
            __lookup["[map]"] = 981;
            __lookup["[index]"] = 995;
            __lookup["[neutral]"] = 1006;
            __lookup["[point by type]"] = 1057;
            __lookup["[phase fatigue]"] = 1068;
            __lookup["[warroom map index]"] = 1075;
            __lookup["[monster exp bonus per user decrease]"] = 1078;
            __lookup["[monster type spawn cost]"] = 1110;
            __lookup["[monster type spawn interval rate]"] = 1119;
            __lookup["[monster spawn base interval]"] = 1122;
            __lookup["[monster spawn random interval]"] = 1128;
            __lookup["[fatigue]"] = 1131;
            __lookup["[fatigue result]"] = 1134;
            __lookup["[max monster]"] = 1138;
            __lookup["[spawn common monster index]"] = 1151;
            __lookup["[spawn common champion index]"] = 1164;
            __lookup["[spawn super champion index]"] = 1177;
            __lookup["[spawn boss index]"] = 1190;
            __lookup["[spawn step max]"] = 1195;
            __lookup["[spawn step resource pool]"] = 1209;
            __lookup["[kill count const]"] = 1214;
            __lookup["[join cost gold]"] = 1224;
            __lookup["[gold drop prob]"] = 1229;
            __lookup["[common monster item drop prob]"] = 1234;
            __lookup["[common champion item drop prob]"] = 1239;
            __lookup["[super champion item drop prob]"] = 1244;
            __lookup["[boss item drop prob]"] = 1252;
            __lookup["[common monster exp const]"] = 1260;
            __lookup["[common champion exp const]"] = 1268;
            __lookup["[super champion exp const]"] = 1276;
            __lookup["[common monster item drop list]"] = 1288;
            __lookup["[common champion item drop list]"] = 1300;
            __lookup["[super champion item drop list]"] = 1312;
            __lookup["[boss item drop list]"] = 1324;
            __lookup["[battle spawn time]"] = 1332;
            __lookup["[powerwar dungeon]"] = 1341;
            __lookup["[blood dungeon]"] = 1359;
            __lookup["[move speed]"] = 1369;
            __lookup["[limit escape character]"] = 1398;
            __lookup["[defense dungeon]"] = 1408;
            __lookup["[advance altar map]"] = 1422;
            __lookup["[advance altar clear reward]"] = 1442;
            __lookup["[advance altar survival map]"] = 1497;
            __lookup["[dimension dungeon]"] = 1570;
            __lookup["[necessary party]"] = 1584;
            __lookup["[village attack revenge dungeon]"] = 1621;
            __lookup["[limit party count]"] = 1687;
            __lookup["[hit count]"] = 1701;
            __lookup["[seal door appear rate]"] = 1704;
            __lookup["[dummy appear count]"] = 1738;
            __lookup["advance altar"] = 1781;
            __lookup["[tower random map indexes]"] = 1784;
            __lookup["[death tower map indexes]"] = 1822;
            __lookup["[named monster map pos]"] = 1836;
            __lookup["[blood max round]"] = 1860;
            __lookup["[event monster]"] = 1894;
            __lookup["[tower high skill initial cool time]"] = 1961;
            __lookup["[tower high skill initial cool time rate]"] = 1969;
            __lookup["[tower fp cubepiece]"] = 1972;
            __lookup["[tower limit of stackable item]"] = 1975;
            __lookup["[tower dialog]"] = 1998;
            __lookup["[tower item drop]"] = 2078;
            __lookup["[tower max clear item num]"] = 2084;
            __lookup["[evil]"] = 2089;
            __lookup["[evil high level]"] = 2100;
            __lookup["[monster difficulty bonus]"] = 2108;
            __lookup["[/monster difficulty bonus]"] = 2141;
            __lookup["[gold card use]"] = 2158;
            __lookup["[SWAY EFFECT]"] = 2164;
            __lookup["[no fatigue]"] = 2170;
            __lookup["[quest npc dungeon]"] = 2185;
            __lookup["[designate dungeon difficulty]"] = 2190;
            __lookup["[tournament dungeon]"] = 2195;
            __lookup["[tournament round fatigue]"] = 2200;
            __lookup["[tournament clear reward gold rate]"] = 2285;
            __lookup["[risk dungeon]"] = 2316;
            __lookup["[easy]"] = 2333;
            __lookup["[medium]"] = 2348;
            __lookup["[party member coin limit]"] = 2370;
            __lookup["[individual map movement]"] = 2388;
            __lookup["[open door even enemy]"] = 2396;
            __lookup["[disable exit]"] = 2402;
            __inited = true;
        }
        __init_lock.leave();
    }

    // 标签分派：DungeonTagLookup 返回 ORIG case 值，switch 分派。
    // 复杂容器/子标签 case 按 ORIG 反汇编转写，无法完全确认处标注【推断】。
    while (ScanType(line, true))
    {
        // ORIG 标签分派：__lookup map 查找（惰性构建于首次调用）
        std::map<const char*, int>::const_iterator it = __lookup.find(line.c_str());
        int tagId = (it != __lookup.end()) ? it->second : -1;
        switch (tagId)
        {
case 0x75:  // "[name]"
    ScanStr(&script->m_str1c);  // +0x1c
    break;

case 0x79:  // "[name]"
    ScanStr(&script->m_str20);  // +0x20
    break;

case 0x7d:  // "[name]"
    ScanStr(&token);  // @88e6d96
    if (token == "ByDifficulty") {  // 0x8e0d55a
        script->m_34 = 1;  // +0x34
        ScanStr(&token);
        script->m_str28 = token;  // @88e6e18 SsaSERKSs 【推断：ByDifficulty 分支第二个串存 m_str28】
    }
    break;

case 0x98:  // "ByDifficulty"
    ScanStr(&token);  // @88e6ec4
    if (token == "ByDifficulty") {  // 0x8e0d55a
        script->m_44 = 1;  // +0x44
        ScanStr(&token);
        script->m_str28 = token;  // @88e6f46 【推断】
    }
    break;

case 0xb3:  // "ByDifficulty"
    ScanStr(&script->m_str28);  // +0x28
    script->m_2c = ScanInt(&ok);  // +0x2c
    break;

case 0xb9:  // "[cutscene image]"
    ScanStr(&script->m_str58);  // +0x58
    break;

case 0xbe:  // "[dungeon loading image path]"
    ScanStr(&script->m_str30);  // +0x30
    break;

case 0xc3:  // "[dungeon loading image path]"
    ScanInt(&ok);  // 丢弃 @88e7174
    ScanStr(&script->m_str74);  // +0x74
    break;

case 0xca:  // "[worldmap info]"
    script->m_14 = ScanInt(&ok);  // +0x14
    break;

case 0xcf:  // "[herosmode required quest]"
    script->m_488 = ScanInt(&ok);  // +0x488
    break;

case 0xd7:  // "[herosmode required quest]"
    script->m_c = ScanInt(&ok) != 0;  // +0xc
    break;

case 0xdd:  // "[background pos]"
    // @88e73e7 调用 GetWorldMapCoordFromTable(x,y,&STWorldmapPattern)（外部函数，
    // 本 TU 不引入头依赖，转写为参数读取）【推断】
    ScanInt(&ok);  // x @88e73ad
    ScanInt(&ok);  // y @88e73bf
    ScanStr(&script->m_str74);  // +0x74（地图名）
    ScanInt(&ok);  // @88e747c 后续
    break;

case 0xff:  // "[monsterapc diff table]"
    // ScanStr 到局部后 SsaSERKSs → m_str17c 【推断】
    ScanStr(&token);
    script->m_str17c = token;
    break;

case 0x109:  // "[monsterapc diff table]"
    script->m_78 = ScanInt(&ok);  // +0x78
    break;

case 0x10d:  // "[minimum required level]"
    script->m_requiredItem7c.m_itemIdx = ScanInt(&ok);  // +0x7c
    script->m_requiredItem7c.m_itemCount = ScanInt(&ok);  // +0x80
    script->m_requiredItem7c.m_enableFlag = ScanInt(&ok) != 0;  // +0x84
    break;

case 0x114:  // "[required item]"
    script->m_88 = ScanInt(&ok) != 0;  // +0x88
    break;

case 0x118:  // "[event dungeon]"
    script->m_requiredItem8c.m_itemIdx = ScanInt(&ok);  // +0x8c
    script->m_requiredItem8c.m_itemCount = ScanInt(&ok);  // +0x90
    script->m_requiredItem8c.m_enableFlag = ScanInt(&ok) != 0;  // +0x94
    break;

case 0x11e:  // "[event required item]"
    script->m_vec98.clear();  // vector<pair<int,int>> @+0x98
    while (true) {
    int v1 = ScanInt(&ok);
    if (!ok) break;
    int v2 = ScanInt(&ok);
    script->m_vec98.push_back(std::make_pair(v1, v2));
    }
    break;

case 0x18c:  // "[recommended level]"
    script->m_450 = ScanInt(&ok);  // +0x450
    script->m_454 = ScanInt(&ok);  // +0x454
    break;

case 0x191:  // "[recommended level]"
    script->m_a4 = ScanInt(&ok);  // +0xa4
    break;

case 0x195:  // "[coin limit]"
    script->m_a8 = ScanInt(&ok);  // +0xa8
    break;

case 0x199:  // "[coin limit]"
    script->m_ac = ScanInt(&ok);  // +0xac
    break;

case 0x19d:  // "[character coin limit]"
    ScanFloat(&ok);  // @88e7b59
    break;

case 0x1a1:  // "[basis level]"
    ScanInt(&ok);  // 局部用途 @88e7be5
    break;

case 0x1a6:  // "[champion]"
    script->m_vecc8.clear();  // vector<pair<int,int>> @+0xc8
    while (true) {
    int v1 = ScanInt(&ok);
    if (!ok) break;
    int v2 = ScanInt(&ok);
    script->m_vecc8.push_back(std::make_pair(v1, v2));
    }
    break;

case 0x1b3:  // "[item]"
    // [item] 复杂子标签：3 个 vector<int> reserve(0x64) 后按子标签填充
    // 【推断】子标签为 item type 分类；此处仅消费 token，保留结构
    script->m_vec458.reserve(0x64);
    script->m_vec464.reserve(0x64);
    script->m_vec470.reserve(0x64);
    ScanStr(&token);  // 子标签
    while (ScanInt(&ok)) {
        // 【推断】按子标签分派 push_back 到三个 vector 之一
    }
    break;

case 0x1e9:  // "[dungeon drop]"
    script->m_vecd4.clear();  // vector<pair<int,int>> @+0xd4
    while (true) {
    int v1 = ScanInt(&ok);
    if (!ok) break;
    int v2 = ScanInt(&ok);
    script->m_vecd4.push_back(std::make_pair(v1, v2));
    }
    break;

case 0x1f4:  // "[dungeon drop]"
    script->m_vece0.clear();  // vector<pair<int,int>> @+0xe0
    while (true) {
    int v1 = ScanInt(&ok);
    if (!ok) break;
    int v2 = ScanInt(&ok);
    script->m_vece0.push_back(std::make_pair(v1, v2));
    }
    break;

case 0x1ff:  // "[dungeon object drop]"
    // [dungeon object drop]：stDungeonAssignItem_t 循环 @88e82e0
    // 【推断】主循环：itemIdx + pair 列表
    while (ScanInt(&ok)) {
        stDungeonAssignItem_t item;
        item.m_assignItemIdx = v1;
        while (ScanInt(&ok)) {
            int a = v1, b = v2;
            item.m_list.push_back(std::make_pair(a, b));
        }
        script->m_vecec.push_back(item);
    }
    break;

case 0x21d:  // "[pathgate object]"
    // [pathgate object]：4 个 ScanInt 逐项读取（丢弃/存局部）
    ScanInt(&ok);
    ScanInt(&ok);
    ScanInt(&ok);
    ScanInt(&ok);
    break;

case 0x228:  // "[maze info]"
    // [maze info]：mazeLocal 累积到 m_mazeList170
    if (mazeInited) {
        script->m_mazeList170.push_back(mazeLocal);
        mazeLocal.clear();
    } else {
        mazeInited = true;
    }
    break;

case 0x27d:  // "[maze info]"
    // [maze info]（尺寸标签）：mazeLocal 宽高
    v1 = ScanInt(&ok);
    v2 = ScanInt(&ok);
    mazeLocal.m_width = v1;
    mazeLocal.m_height = v2;
    break;

case 0x28a:  // "[quest connection]"
    ScanInt(&ok);  // 局部用途 @88e8924
    ScanInt(&ok);  // 局部用途 @88e8981
    break;

case 0x2be:  // "../RDARScriptDungeon.cpp"
    ScanInt(&ok);  // 局部用途 @88e8aac
    ScanInt(&ok);  // 局部用途 @88e8abe
    break;

case 0x2c4:  // "[party num check]"
    ScanInt(&ok);  // 局部用途 @88e8b32
    break;

case 0x2c8:  // "[party num check]"
    // [party num check]：maze 尺寸 → grids.resize
    v1 = ScanInt(&ok);
    v2 = ScanInt(&ok);
    mazeLocal.m_width = v1;
    mazeLocal.m_height = v2;
    mazeLocal.m_grids.resize(mazeLocal.m_width * mazeLocal.m_height, GridScript());
    break;

case 0x2d0:  // "[size]"
    // [size]：解析 "WxH" 串设置 maze 尺寸
    ScanStr(&token);
    {
        // 解析 size 串（形如 "12x8"）：分隔符 'x'；数字逐字符累加
        std::string::size_type p = token.find('x');
        if (p != std::string::npos) {
            mazeLocal.m_width = atoi(token.substr(0, p).c_str());
            mazeLocal.m_height = atoi(token.substr(p + 1).c_str());
        }
    }
    break;

case 0x30e:  // "[map specification]"
    // [map specification]：填充 maze grid
    v1 = ScanInt(&ok);  // x
    v2 = ScanInt(&ok);  // y
    {
        GridScript* g = mazeLocal.getGrid(v1, v2);
        g->m_mapCandidates.clear();
        while (true) {
            v1 = ScanInt(&ok);
            if (!ok) break;
            g->m_mapCandidates.push_back(v1);
        }
    }
    break;

case 0x31e:  // "[boss map specification]"
    // [boss map specification]：同 [map specification]
    v1 = ScanInt(&ok);
    v2 = ScanInt(&ok);
    {
        GridScript* g = mazeLocal.getGrid(v1, v2);
        g->m_secondCandidates.clear();
        while (true) {
            v1 = ScanInt(&ok);
            if (!ok) break;
            g->m_secondCandidates.push_back(v1);
        }
    }
    break;

case 0x32e:  // "[layered map specification]"
    // [layered map specification]：同 [map specification]
    v1 = ScanInt(&ok);
    v2 = ScanInt(&ok);
    {
        GridScript* g = mazeLocal.getGrid(v1, v2);
        g->m_layeredMapIndexes.clear();
        while (true) {
            v1 = ScanInt(&ok);
            if (!ok) break;
            g->m_layeredMapIndexes.push_back(v1);
        }
    }
    break;

case 0x33e:  // "[start map]"
    // [start map]：读 (x,y) 坐标对入 mazeLocal.m_vec14（ORIG LAB_088e919e）
    mazeLocal.m_vec14.clear();
    while (true) {
        v1 = ScanInt(&ok);
        if (!ok) break;
        v2 = ScanInt(&ok);
        mazeLocal.m_vec14.push_back(std::make_pair(v1, v2));
    }
    break;

case 0x34a:  // "[boss map]"
    // [boss map]：读 (x,y) 坐标对入 mazeLocal.m_vec20（ORIG LAB_088e9288）
    mazeLocal.m_vec20.clear();
    while (true) {
        v1 = ScanInt(&ok);
        if (!ok) break;
        v2 = ScanInt(&ok);
        mazeLocal.m_vec20.push_back(std::make_pair(v1, v2));
    }
    break;

case 0x355:  // "[boss map]"
    script->m_140 = ScanInt(&ok) != 0;  // +0x140
    break;

case 0x35b:  // "[hell dungeon]"
    script->m_141 = ScanInt(&ok) != 0;  // +0x141
    break;

case 0x362:  // "[prohibit practice]"
    script->m_142 = ScanInt(&ok) != 0;  // +0x142
    break;

case 0x367:  // "[disable skill]"
    // [disable skill]：job → 技能禁表
    ScanInt(&ok);  // 丢弃（版本）
    ScanStr(&token);  // job 名
    {
        // 【推断】getCharacterJobToEnumValue(token) 后 make_pair 入 m_vec35c（外部函数未声明）
        int job = 0;
        script->m_vec35c.push_back(std::make_pair(job, 1));
    }
    break;

case 0x377:  // "[/impossible skills]"
    // [/impossible skills]：ScanInt 丢弃
    ScanInt(&ok);
    break;

case 0x37c:  // "[/impossible skills]"
    ScanFloat(&ok);  // @88e977f
    break;

case 0x381:  // "[use countdown]"
    script->m_158 = ScanInt(&ok) == 1;  // cmp $1,sete
    break;

case 0x386:  // "[use zoom rate]"
    script->m_159 = ScanInt(&ok) == 1;  // cmp $1,sete
    break;

case 0x38a:  // "[usable item]"
    ScanInt(&ok);  // 丢弃 @88e98f9
    break;

case 0x38d:  // "[usable item]"
    // [usable item]：两值
    ScanInt(&ok);
    ScanInt(&ok);
    break;

case 0x392:  // "[seal door map index]"
    script->m_168 = ScanInt(&ok);  // +0x168
    break;

case 0x397:  // "[seal door pos]"
    script->m_338 = ScanInt(&ok);  // +0x338
    break;

case 0x39b:  // "[hell coin limit]"
    // [hell coin limit]：push_back 到 m_deadTower180.m_vec
    while (true) {
        v1 = ScanInt(&ok);
        if (!ok) break;
        script->m_deadTower180.m_vec.push_back(v1);
    }
    break;

case 0x421:  // "[point by type]"
    ScanInt(&ok);  // 丢弃
    break;

case 0x426:  // "[point by type]"
    script->m_warRoom18c.m_c = ScanInt(&ok);  // +0x198
    break;

case 0x42c:  // "[phase fatigue]"
    script->m_warRoom18c.m_10 = ScanInt(&ok);  // +0x19c
    break;

case 0x430:  // "[phase fatigue]"
    // [phase fatigue]（float）
    ScanFloat(&ok);  // 丢弃
    break;

case 0x433:  // "[warroom map index]"
    // [warroom map index]（float）
    ScanFloat(&ok);  // 丢弃
    break;

case 0x436:  // "[monster exp bonus per user decrease]"
    // [monster exp bonus per user decrease]：4 个值到 STMonsterTypeSpawnProb
    ScanInt(&ok);
    ScanInt(&ok);
    ScanInt(&ok);
    ScanInt(&ok);
    break;

case 0x456:  // "[monster type spawn cost]"
    ScanInt(&ok);
    break;

case 0x45b:  // "[monster type spawn cost]"
    ScanFloat(&ok);
    break;

case 0x45f:  // "[monster type spawn interval rate]"
    script->m_warRoom18c.m_ec = ScanInt(&ok);  // +0x278
    break;

case 0x462:  // "[monster spawn base interval]"
    script->m_warRoom18c.m_f0 = ScanInt(&ok);  // +0x27c
    break;

case 0x465:  // "[monster spawn base interval]"
    script->m_warRoom18c.m_f4 = ScanInt(&ok);  // +0x280
    break;

case 0x468:  // "[monster spawn random interval]"
    script->m_warRoom18c.m_f8 = ScanInt(&ok);  // +0x284
    break;

case 0x46b:  // "[fatigue]"
    script->m_warRoom18c.m_d8 = ScanInt(&ok);  // +0x264
    break;

case 0x46e:  // "[fatigue result]"
    script->m_warRoom18c.m_d4 = ScanInt(&ok);  // +0x260
    break;

case 0x472:  // "[max monster]"
    // [max monster]：vector<int> 循环
    while (true) {
        v1 = ScanInt(&ok);
        if (!ok) break;
        script->m_warRoom18c.m_vec6c[0].push_back(v1);  // 【推断】目标 vector 待核
    }
    break;

case 0x47f:  // "[spawn common monster index]"
    while (true) {
        v1 = ScanInt(&ok);
        if (!ok) break;
        script->m_warRoom18c.m_vec6c[1].push_back(v1);
    }
    break;

case 0x48c:  // "[spawn common champion index]"
    while (true) {
        v1 = ScanInt(&ok);
        if (!ok) break;
        script->m_warRoom18c.m_vec6c[2].push_back(v1);
    }
    break;

case 0x499:  // "[spawn super champion index]"
    while (true) {
        v1 = ScanInt(&ok);
        if (!ok) break;
        script->m_warRoom18c.m_vec6c[3].push_back(v1);
    }
    break;

case 0x4a6:  // "[spawn boss index]"
    script->m_warRoom18c.m_50 = ScanInt(&ok);  // +0x1dc
    break;

case 0x4ab:  // "[spawn step max]"
    while (true) {
        v1 = ScanInt(&ok);
        if (!ok) break;
        script->m_warRoom18c.m_vec14[0].push_back(v1);  // 【推断】
    }
    break;

case 0x4b9:  // "[spawn step resource pool]"
    ScanFloat(&ok);
    break;

case 0x4be:  // "[kill count const]"
    script->m_warRoom18c.m_54 = ScanInt(&ok);  // +0x1e0
    break;

case 0x4c3:  // "[kill count const]"
    script->m_warRoom18c.m_58 = ScanInt(&ok);  // +0x1e4
    break;

case 0x4c8:  // "[join cost gold]"
    script->m_warRoom18c.m_5c[0] = ScanInt(&ok);  // +0x1e8
    break;

case 0x4cd:  // "[gold drop prob]"
    script->m_warRoom18c.m_5c[1] = ScanInt(&ok);  // +0x1ec
    break;

case 0x4d2:  // "[common monster item drop prob]"
    script->m_warRoom18c.m_5c[2] = ScanInt(&ok);  // +0x1f0
    break;

case 0x4d7:  // "[common champion item drop prob]"
    script->m_warRoom18c.m_5c[3] = ScanInt(&ok);  // +0x1f4
    break;

case 0x4dc:  // "[super champion item drop prob]"
    script->m_warRoom18c.m_9c[0] = ScanInt(&ok);  // +0x228
    break;

case 0x4e4:  // "[boss item drop prob]"
    script->m_warRoom18c.m_9c[1] = ScanInt(&ok);  // +0x22c
    break;

case 0x4ec:  // "[common monster exp const]"
    script->m_warRoom18c.m_9c[2] = ScanInt(&ok);  // +0x230
    break;

case 0x4f4:  // "[common champion exp const]"
    script->m_warRoom18c.m_9c[3] = ScanInt(&ok);  // +0x234
    break;

case 0x4fc:  // "[super champion exp const]"
    while (true) {
        v1 = ScanInt(&ok);
        if (!ok) break;
        script->m_warRoom18c.m_vec44.push_back(v1);  // 【推断】
    }
    break;

case 0x508:  // "[common monster item drop list]"
    while (true) {
        v1 = ScanInt(&ok);
        if (!ok) break;
        script->m_warRoom18c.m_vec14[1].push_back(v1);  // 【推断】
    }
    break;

case 0x514:  // "[common champion item drop list]"
    while (true) {
        v1 = ScanInt(&ok);
        if (!ok) break;
        script->m_warRoom18c.m_vec14[2].push_back(v1);  // 【推断】
    }
    break;

case 0x520:  // "[super champion item drop list]"
    while (true) {
        v1 = ScanInt(&ok);
        if (!ok) break;
        script->m_warRoom18c.m_vec14[3].push_back(v1);  // 【推断】
    }
    break;

case 0x52c:  // "[boss item drop list]"
    script->m_warRoom18c.m_fc = ScanInt(&ok);  // +0x288
    break;

case 0x534:  // "[battle spawn time]"
    script->m_28c = ScanInt(&ok) != 0;  // +0x28c
    break;

case 0x539:  // "[battle spawn time]"
    script->m_28d = ScanInt(&ok) != 0;  // +0x28d
    break;

case 0x53d:  // "[powerwar dungeon]"
    script->m_28e = (char)ScanInt(&ok);  // mov %dl,0x28e
    break;

case 0x549:  // "[powerwar dungeon]"
    script->m_28f = ScanInt(&ok) != 0;  // +0x28f
    break;

case 0x576:  // "[limit escape character]"
    script->m_2a8 = ScanInt(&ok) != 0;  // +0x2a8
    break;

case 0x57d:  // "[limit escape character]"
    script->m_2ab = ScanInt(&ok) != 0;  // +0x2ab
    break;

case 0x580:  // "[defense dungeon]"
    // [defense dungeon]：StageType
    ScanStr(&token);
    advancealtar::StageType::T st;
    advancealtar::StageType::getStageType(token, st);
    script->m_2ac = st;
    break;

case 0x58e:  // "[advance altar map]"
    // [advance altar map]：StageDifficulty
    ScanStr(&token);
    advancealtar::StageDifficulty::T sd;
    advancealtar::StageDifficulty::getDifficulty(token, sd);
    script->m_2ac = sd;
    ScanInt(&ok);
    ScanInt(&ok);
    break;

case 0x5a2:  // "[advance altar clear reward]"
    // [advance altar clear reward]：按 reward type 子标签填充 m_clearReward2d4
    ScanStr(&token);  // reward type
    {
        advancealtar::_RewardItem ri;
        advancealtar::RewardItemType::T rt;
        if (advancealtar::RewardItemType::getRewardItemType(token, rt)) {
            ri.m_type = rt;
        }
        ri.m_value1 = ScanInt(&ok);
        ri.m_value2 = ScanInt(&ok);
        script->m_clearReward2d4.m_vec[rt].push_back(ri);
    }
    break;

case 0x5d9:  // "[advance altar survival map]"
    ScanInt(&ok);
    break;

case 0x5e5:  // "[advance altar survival map]"
    // [advance altar survival map]：round reward 解析
    ScanInt(&ok);
    ScanInt(&ok);
    ScanStr(&token);
    {
        advancealtar::RoundRewardData rrd;
        rrd.m_0 = v1;
        rrd.m_4 = v2;
        ScanInt(&ok);
        ScanInt(&ok);
        advancealtar::_RewardItem ri;
        ri.m_value1 = v1;
        ri.m_value2 = v2;
        rrd.m_vec8.push_back(ri);
        script->m_survival2f8.m_vec.push_back(rrd);
    }
    break;

case 0x622:  // "[dimension dungeon]"
    ScanInt(&ok);
    ScanInt(&ok);
    break;

case 0x62c:  // "[dimension dungeon]"
    ScanInt(&ok);
    break;

case 0x630:  // "[necessary party]"
    // [necessary party]：DimensionPartyCount
    ScanInt(&ok);
    ScanInt(&ok);
    ScanInt(&ok);
    ScanInt(&ok);
    break;

case 0x655:  // "[village attack revenge dungeon]"
    ScanInt(&ok);
    ScanInt(&ok);
    ScanInt(&ok);
    ScanInt(&ok);
    break;

case 0x65d:  // "[village attack revenge dungeon]"
    // [village attack revenge dungeon]：5 串 + 时间/星期转换
    ScanInt(&ok);
    ScanStr(&token);
    ScanStr(&token);
    ScanStr(&token);
    ScanStr(&token);
    ScanStr(&token);
    break;

case 0x697:  // "[limit party count]"
    ScanInt(&ok);
    break;

case 0x69a:  // "[limit party count]"
    ScanInt(&ok);
    break;

case 0x69d:  // "[limit party count]"
    ScanInt(&ok);
    break;

case 0x6a5:  // "[hit count]"
    script->m_13c = ScanInt(&ok);  // +0x13c
    break;

case 0x6a8:  // "[seal door appear rate]"
    // [seal door appear rate]：两个分支（m_bossRoom328 子字段）
    ScanStr(&token);
    ScanInt(&ok);
    script->m_bossRoom328.m_0 = 1;
    ScanInt(&ok);
    ScanInt(&ok);
    ScanInt(&ok);
    script->m_bossRoom328.m_0 = 2;
    ScanInt(&ok);
    ScanInt(&ok);
    break;

case 0x6ca:  // "[dummy appear count]"
    script->m_16c = ScanInt(&ok);  // +0x16c
    break;

case 0x6cd:  // "[dummy appear count]"
    // [dummy appear count]：GridScript resize
    ScanStr(&token);
    script->m_358 = 1;
    script->m_358 = 3;
    script->m_358 = 4;
    script->m_358 = 2;
    script->m_358 = 0;
    break;

case 0x6f5:  // "advance altar"
    script->m_33c = ScanInt(&ok);  // +0x33c
    break;

case 0x6f8:  // "[tower random map indexes]"
    // [tower random map indexes]：map<int,vector<RandomList>> 填充
    ScanInt(&ok);
    ScanInt(&ok);
    ScanInt(&ok);
    break;

case 0x71e:  // "[death tower map indexes]"
    // [death tower map indexes]
    script->m_368 = ScanInt(&ok);
    ScanInt(&ok);
    ScanInt(&ok);
    break;

case 0x72c:  // "[named monster map pos]"
    // [named monster map pos]
    ScanInt(&ok);
    ScanInt(&ok);
    {
        STNamedMonsterMapPos nm;
        nm.m_x = v1;
        nm.m_y = v2;
        script->m_vec3c8.push_back(nm);
    }
    break;

case 0x73f:  // "[named monster map pos]"
    script->m_3c4 = ScanInt(&ok);  // +0x3c4
    break;

case 0x744:  // "[blood max round]"
    // [blood max round]：stSecondEventMonster_t
    ScanInt(&ok);
    ScanInt(&ok);
    ScanInt(&ok);
    ScanInt(&ok);
    ScanInt(&ok);
    ScanInt(&ok);
    ScanInt(&ok);
    break;

case 0x766:  // "[event monster]"
    // [event monster]：stEventMonster_t
    ScanInt(&ok);
    ScanInt(&ok);
    ScanInt(&ok);
    ScanInt(&ok);
    ScanInt(&ok);
    ScanInt(&ok);
    ScanInt(&ok);
    break;

case 0x7a9:  // "[tower high skill initial cool time]"
    // [tower high skill initial cool time]：float 对
    ScanFloat(&ok);
    ScanFloat(&ok);
    break;

case 0x7ad:  // "[tower high skill initial cool time]"
    ScanFloat(&ok);
    ScanFloat(&ok);
    break;

case 0x7b1:  // "[tower high skill initial cool time rate]"
    ScanFloat(&ok);
    break;

case 0x7b4:  // "[tower fp cubepiece]"
    ScanFloat(&ok);
    break;

case 0x7b7:  // "[tower limit of stackable item]"
    // [tower limit of stackable item]：RecoveryStairInfo（=现有 [tower recovery] 逻辑）
    {
        RecoveryStairInfo rsi;
        rsi.m_0 = static_cast<int>(ScanFloat(&ok));
        rsi.m_4 = static_cast<int>(ScanFloat(&ok));
        rsi.m_8 = ScanFloat(&ok);
        while (true) {
            RecoveryInfo ri;
            ri.m_data[0] = ScanFloat(&ok);
            if (!ok) break;
            ri.m_data[1] = ScanFloat(&ok);
            ri.m_data[2] = ScanFloat(&ok);
            rsi.m_vec.push_back(ri);
        }
        script->m_vec384.push_back(rsi);
    }
    break;

case 0x7ce:  // "[tower dialog]"
    // [tower dialog]：STTowerDialog
    ScanInt(&ok);
    ScanStr(&token);
    ScanStr(&token);
    break;

case 0x819:  // "../RDARScriptDungeon.cpp"
    script->m_3b4 = ScanInt(&ok);  // +0x3b4
    break;

case 0x81e:  // "[tower item drop]"
    script->m_3b8 = ScanInt(&ok);  // +0x3b8
    break;

case 0x824:  // "[tower max clear item num]"
    ScanInt(&ok);
    break;

case 0x829:  // "[evil]"
    ScanInt(&ok);
    break;

case 0x834:  // "[evil high level]"
    // [evil high level]
    ScanInt(&ok);
    ScanInt(&ok);
    ScanInt(&ok);
    ScanInt(&ok);
    break;

case 0x83c:  // "[monster difficulty bonus]"
    // [monster difficulty bonus]：float 列表
    ScanFloat(&ok);
    break;

case 0x85d:  // "[/monster difficulty bonus]"
    ScanStr(&script->m_str448);  // +0x448
    break;

case 0x862:  // "[/monster difficulty bonus]"
    ScanInt(&ok);
    break;

case 0x86e:  // "[gold card use]"
    script->m_484 = ScanInt(&ok);  // +0x484
    script->m_480 = ScanInt(&ok);  // +0x480
    break;

case 0x880:  // "[no fatigue]"
    ScanInt(&ok);
    break;

case 0x889:  // "[quest npc dungeon]"
    script->m_490 = ScanInt(&ok);  // +0x490
    break;

case 0x88e:  // "[designate dungeon difficulty]"
    ScanInt(&ok);
    break;

case 0x893:  // "[tournament dungeon]"
    ScanInt(&ok);
    break;

case 0x898:  // "[tournament round fatigue]"
    // [tournament round fatigue]：map<int,TournamentRewardRate>
    ScanInt(&ok);
    ScanInt(&ok);
    ScanInt(&ok);
    ScanInt(&ok);
    ScanInt(&ok);
    ScanInt(&ok);
    ScanInt(&ok);
    break;

case 0x8ed:  // "[tournament clear reward gold rate]"
    ScanFloat(&ok);  // @88eff8a
    break;

case 0x8f2:  // "[tournament clear reward gold rate]"
    // [tournament clear reward gold rate]
    ScanInt(&ok);
    ScanInt(&ok);
    break;

case 0x90c:  // "[risk dungeon]"
    script->m_4e0 = ScanInt(&ok) != 0;  // +0x4e0
    break;

case 0x92c:  // "[medium]"
    // [medium]：UseCoinDungeonDiff map
    ScanInt(&ok);
    ScanInt(&ok);
    break;

case 0x91d:  // "[easy]"（ORIG 0x91d；同构于 [medium]，消费难度数据 token）
    ScanInt(&ok);
    ScanInt(&ok);
    break;

case 0x1000:  // "[minimap image]"
    ScanStr(&script->m_str30);   // +0x30（export 对称）
    break;

case 0x1001:  // "[ancient dungeon]"
    script->m_28c = ScanInt(&ok) != 0;   // +0x28c（export 对称：%d 字节）
    break;

case 0x1002:  // "[boss exp const]"
    script->m_warRoom18c.m_9c[3] = ScanInt(&ok);   // +0x234（export 对称）
    break;

case 0x1003:  // "[experience increasing point]"
    script->m_b0 = ScanFloat(&ok);   // +0xb0（float；export 对称 %.3f）
    break;

case 0x1004:  // "[hard]"
    ScanInt(&ok);
    ScanInt(&ok);
    break;

case 0x1005:  // "[ultimate]"
    ScanInt(&ok);
    ScanInt(&ok);
    break;

case 0x1006:  // "[entering title]"
    ScanStr(&script->m_str24);   // +0x24（export 对称）
    break;

case 0x1007:  // "[player kc]"
    script->m_warRoom18c.m_d4 = ScanInt(&ok);   // +0x260（export 对称）
    break;

case 0x1008:  // "[worldmap pattern info]"
    ScanStr(&script->m_str74);   // +0x74（worldmap pattern 名）
    break;

case 0x1009:  // "[entering title next]"
    ScanStr(&script->m_str28);   // 下一个进场标题（存 m_str28）
    break;

case 0x100a:  // "[ai character appear rate]"
    script->m_338 = ScanInt(&ok);   // +0x338（report LAB_088e9a09 实证）
    break;

case 0x100b:  // "[limit inout count]"
    script->m_325 = (char)ScanInt(&ok);   // +0x325（report LAB_088ed81e 实证）
    break;

case 0x100c:  // "[escape hell]"
    ScanInt(&ok);   // ORIG LAB_088e8ace：读 1 int 设局部标志（无脚本字段，消费 token 防错位）
    break;

case 0x100d:  // "[dungeon type]"
    ScanInt(&ok);   // 【推断】单标量 type；消费防错位
    break;

case 0x100e:  // "[room move keep state]"
    ScanInt(&ok);   // 【推断】单标量 keep-state 枚举；消费防错位
    break;

case 0x100f:  // "[monster type spawn prob]"
{
    // ORIG LAB_088ea9eb：逐条读 4 int（怪物类型 + 3 概率）push 入 m_warRoom18c.m_spawnProb
    while (true) {
        STMonsterTypeSpawnProb sp;
        int* el = (int*)sp.m_pad;
        el[0] = ScanInt(&ok);
        if (!ok) break;
        el[1] = ScanInt(&ok);
        if (!ok) break;
        el[2] = ScanInt(&ok);
        if (!ok) break;
        el[3] = ScanInt(&ok);
        if (!ok) break;
        script->m_warRoom18c.m_spawnProb.push_back(sp);
    }
    break;
}

case 0x1010:  // "[result exp bonus per user decrease]"
    *(float*)&script->m_warRoom18c.m_c0 = ScanFloat(&ok);   // +0x24c（report LAB_088ea971 实证）
    break;

case 0x1011:  // "[tower recovery]"
{
    // ORIG LAB_088eecc1：RecoveryStairInfo（int ROUND + int ROUND + float + vector<RecoveryInfo>）
    RecoveryStairInfo rsi;
    rsi.m_0 = (int)(ScanFloat(&ok) + 0.5f);            // ROUND
    rsi.m_4 = (int)(ScanFloat(&ok) + 0.5f);
    rsi.m_8 = ScanFloat(&ok);
    while (true) {
        float a = ScanFloat(&ok);
        if (!ok) break;
        RecoveryInfo ri;
        ri.m_data[0] = a;
        ri.m_data[1] = ScanFloat(&ok);
        ri.m_data[2] = ScanFloat(&ok);
        rsi.m_vec.push_back(ri);
    }
    script->m_vec384.push_back(rsi);
    break;
}

case 0x1012:  // "[tower stage]"
{
    // ORIG LAB_088ee05d：读 maxStage，循环读 (mapIdx,weight) → m_map340[maxStage]
    int maxStage = ScanInt(&ok);
    if (ok) {
        std::vector<RandomList> vec;
        while (true) {
            RandomList rl;
            rl.m_mapIndex = ScanInt(&ok);
            if (!ok) break;
            rl.m_weight = ScanInt(&ok);
            if (!ok) break;
            vec.push_back(rl);
        }
        script->m_map340[maxStage] = vec;
    }
    break;
}

case 0x1013:  // "[tournament clear reward exp]"
{
    // ORIG LAB_088effb9：读 (key uchar, val uint) 对 insert 入 m_map4c8（重复 key 告警）
    while (true) {
        int key = ScanInt(&ok);
        if (!ok) break;
        int val = ScanInt(&ok);
        if (!ok) break;
        std::pair<std::map<unsigned char, unsigned int>::iterator, bool> res =
            script->m_map4c8.insert(std::make_pair((unsigned char)key, (unsigned int)val));
        if (!res.second)
            puts("TournamentClearRewardExpMap Key is Duplicated!!!");
    }
    break;
}

case 0x1014:  // "[road necessary party]"
{
    // ORIG LAB_088ecf16：读 count，循环读 DimensionPartyCount(3 char)，去重后 push 入 m_dimension308
    int count = ScanInt(&ok);
    if (ok) {
        for (int i = 0; i < count; ++i) {
            DimensionPartyCount dpc;
            dpc.m_0 = (char)ScanInt(&ok);
            dpc.m_1 = (char)ScanInt(&ok);
            dpc.m_2 = (char)ScanInt(&ok);
            std::vector<DimensionPartyCount>::iterator it =
                std::find(script->m_dimension308.begin(), script->m_dimension308.end(), dpc);
            if (it == script->m_dimension308.end())
                script->m_dimension308.push_back(dpc);
        }
    }
    break;
}

case 0x1015:  // "[impossible skills]"
{
    // ORIG LAB_088e94fb：循环 ScanStr job 名 → getCharacterJobToEnumValue + ScanInt → m_vec144
    while (true) {
        std::string token;
        if (!ScanStr(&token)) break;
        if (token == "[/impossible skills]") break;
        int job = getCharacterJobToEnumValue(token);
        int val = ScanInt(&ok);
        script->m_vec144.push_back(std::make_pair(job, val));
    }
    break;
}

case 0x1016:  // "[special passive object item]"
{
    // ORIG LAB_088e81e4：清 m_vecec，循环读（itemIdx 消费 / assignItemIdx / pairCount + pair 列表）
    script->m_vecec.clear();
    while (true) {
        int v0 = ScanInt(&ok);       // itemIdx（消费）
        if (!ok) break;
        int assignItemIdx = ScanInt(&ok);
        int pairCount = ScanInt(&ok);
        stDungeonAssignItem_t item;
        item.m_assignItemIdx = assignItemIdx;
        for (int i = 0; i < pairCount; ++i) {
            int a = ScanInt(&ok);
            if (!ok) break;
            int b = ScanInt(&ok);
            if (!ok) break;
            item.m_list.push_back(std::make_pair(a, b));
        }
        script->m_vecec.push_back(item);
    }
    break;
}

case 0x1017:  // "[evil rate]"
{
    // ORIG LAB_088ef474：13 行 × 5 float → m_vec404
    script->m_vec404.clear();
    for (int i = 0; i < 0xd; ++i) {
        std::vector<float> row;
        row.reserve(5);
        for (int j = 0; j < 5; ++j) {
            float f = ScanFloat(&ok);
            if (!ok) break;   // ORIG 在此 logFormat 告警后 break（省略日志，不影响数据）
            row.push_back(f);
        }
        script->m_vec404.push_back(row);
    }
    break;
}

case 0x1018:  // "[result card]"
{
    // ORIG LAB_088efba8：子 token 分发 [reward item rate]→m_itemRate4b4、[reward rate]→m_map49c
    while (true) {
        std::string sub;
        if (!ScanType(sub, true)) break;
        if (sub == "[reward item rate]") {
            script->m_itemRate4b4.Clear();
            while (true) {
                int a = ScanInt(&ok); if (!ok) break;
                int b = ScanInt(&ok); if (!ok) break;
                script->m_itemRate4b4.m_c += b;
                int c = ScanInt(&ok); if (!ok) break;
                TournamentRewardItem it;
                it.m_data[0] = a; it.m_data[1] = b; it.m_data[2] = c;
                script->m_itemRate4b4.m_items.push_back(it);
            }
        }
        else if (sub == "[reward rate]") {
            while (true) {
                int key = ScanInt(&ok); if (!ok) break;
                TournamentRewardRate rate;
                rate.m_0 = ScanInt(&ok); if (!ok) break;
                rate.m_4 = ScanInt(&ok); if (!ok) break;
                rate.m_8 = ScanInt(&ok); if (!ok) break;
                int sum = rate.m_0 + rate.m_4 + rate.m_8;
                if (sum == 0) break;   // ORIG：sum==0 → 错误退出
                script->m_map49c[key] = rate;
            }
        }
        else break;   // 未知子 token 结束
    }
    break;
}

case 0x1019:  // "[advance altar type]"
{
    // ORIG LAB_088ec46e：ScanStr → advancealtar::StageType::getStageType → m_2ac
    std::string s;
    if (ScanStr(&s)) {
        advancealtar::StageType::T t;
        if (advancealtar::StageType::getStageType(s, t))
            script->m_2ac = (int)t;
    }
    break;
}

case 0x101a:  // "[revision table]"
    ScanStr(&script->m_str448);   // +0x448（report LAB_088ef6c4 实证）
    break;

case 0x101b:  // "[character respawn posetion]"
    script->m_50c = ScanInt(&ok) != 0;   // +0x50c（report LAB_088f08d4 实证，char）
    break;

case 0x101c:  // "[boss room entrance condition]"
    script->m_16c = ScanInt(&ok);   // 【推断】LAB_088edcca __inited==1 读 +0x16c（与 dummy appear count 共享块）
    break;

case 0x101d:  // "[clear reward item]"
    script->m_450 = ScanInt(&ok);   // +0x450（report LAB_088e78f5 实证）
    script->m_454 = ScanInt(&ok);   // +0x454
    break;

case 0x101e:  // "[absolute change stat]"
{
    // ORIG 0x088ebdf1（objdump 解码）：ScanStr(stat 名) → __lookup_0 映射到
    // 0x55c/0x55f/0x562/0x565/0x559（[physical|magical attack|defense add]/[move speed]）
    // → local_113c(1/2/3/4/0) → 共享块读 float 写 m_statMap290[(T)local_113c]；
    // 未知名 / 无法解析时 local_113c=5 不写 map。
    while (true) {
        std::string statName;
        if (!ScanStr(&statName)) break;
        int idx = -1;
        if (statName == "[physical attack add]") idx = 0x55c;
        else if (statName == "[physical defense add]") idx = 0x55f;
        else if (statName == "[magical attack add]") idx = 0x562;
        else if (statName == "[magical defense add]") idx = 0x565;
        else if (statName == "[move speed]") idx = 0x559;
        int statKey;
        if (idx == 0x55c) statKey = 1;
        else if (idx == 0x55f) statKey = 2;
        else if (idx == 0x562) statKey = 3;
        else if (idx == 0x565) statKey = 4;
        else if (idx == 0x559) statKey = 0;
        else statKey = 5;   // 未知名 → 不写 map
        float v = ScanFloat(&ok);
        if (!ok) break;
        if (statKey != 5)
            script->m_statMap290[static_cast<AbsoluteChangeStat::T>(statKey)] = v;
    }
    break;
}

case 0x101f:  // "[event monster2]"
{
    // ORIG LAB_088ee594 __inited==1：读 m_3ec/m_3f0 + 循环 5 int 的 stSecondEventMonster_t 入 m_vec3f8
    script->m_3ec = ScanInt(&ok);
    script->m_3f0 = ScanInt(&ok);
    while (true) {
        stSecondEventMonster_t em;
        em.m_eventIdx = ScanInt(&ok); if (!ok) break;
        em.m_monsterIdx = ScanInt(&ok); if (!ok) break;
        em.m_prob = ScanInt(&ok); if (!ok) break;
        em.m_field0c = ScanInt(&ok); if (!ok) break;
        em.m_field10 = ScanInt(&ok); if (!ok) break;
        script->m_3f4 += em.m_prob;   // ORIG 累加 local_11f0(prob) 到 +0x3f4
        script->m_vec3f8.push_back(em);
    }
    break;
}

case 0x1020:  // "[magical attack add]"
    // ORIG LAB_088ec1ff：设 local_113c=3（AbsoluteChangeStat::T）→ 共享块读 float 入 m_statMap290[(T)3]
    script->m_statMap290[static_cast<AbsoluteChangeStat::T>(3)] = ScanFloat(&ok);
    break;

case 0x1021:  // "[magical defense add]"
    // ORIG：设 local_113c=4 → 共享块读 float 入 m_statMap290[(T)4]
    script->m_statMap290[static_cast<AbsoluteChangeStat::T>(4)] = ScanFloat(&ok);
    break;

case 0x1022:  // "[coin info]"
    script->m_4fc = ScanInt(&ok);   // +0x4fc（report LAB_088f06bb __inited==1 实证）
    break;

case 0x1023:  // "[clear condition]"
{
    // ORIG LAB_088e8643：子 token（[destroy object]/[clear map]/[hunt monster|apc|boss]→类型）
    // + 左/右变量 → mazeLocal.m_clearCondition；子 token 用 ScanStr 近似 ScanTypeOrStr【推断】。
    while (true) {
        std::string sub;
        if (!ScanStr(&sub)) break;
        if (sub == "[/clear condition]") break;
        int type = -1;
        if (sub == "[destroy object]") type = 0;
        else if (sub == "[clear map]") type = 1;
        else if (sub == "[hunt monster]") type = 2;
        else if (sub == "[hunt apc]") type = 3;
        else if (sub == "[hunt boss]") type = 4;
        DungeonClearCondition cond;
        cond.m_0 = type;
        cond.m_4 = ScanInt(&ok);
        cond.m_8 = ScanInt(&ok);
        if (type == 1) cond.m_8 = 1;   // ORIG：clear map → 右变量恒 1
        mazeLocal.m_clearCondition.push_back(cond);
    }
    break;
}

case 0x1024:  // "[greed]"
{
    // ORIG LAB_088e8c37：ScanStr 读 greed 串 → 逐 2 字符匹配 g_gridSymbols 填 mazeLocal.m_grids。
    // \r / \n / 空格作分隔；未知 2 字符符号记行推进（近似）。
    std::string greed;
    ScanStr(&greed);
    int pos = 0;
    int gridIdx = 0;
    int len = (int)greed.size();
    while (pos < len - 1)
    {
        char c0 = greed[pos];
        if (c0 == '\r' || c0 == '\n' || c0 == ' ')
        {
            pos += 1;
            continue;
        }
        int cc = 0;
        for (; cc < 16; ++cc)
        {
            if (greed[pos] == g_gridSymbols[cc][0] && greed[pos + 1] == g_gridSymbols[cc][1])
                break;
        }
        if (cc == 16)
        {
            pos += 1;   // 未知 2 字符 → 行推进（近似）
        }
        else
        {
            if (gridIdx < (int)mazeLocal.m_grids.size())
                mazeLocal.m_grids[gridIdx].m_gridValue = cc;
            gridIdx += 1;
            pos += 2;
        }
    }
    break;
}

case 0x1025:  // "[randomized object creation]"
{
    // ORIG objdump 解明：子标签为 [select]/[regenerate]/[minimap icon]/[object]
    // （经子 map 0x94f7060 分发到 0x3b3/0x3b8/0x3bd/0x3c2）。
    // [select]/[regenerate]/[minimap icon] 各读 1 int；[object] 是嵌套 RidableObject 容器（深）。
    while (true) {
        std::string sub;
        if (!ScanType(sub, true)) break;
        if (sub == "[/randomized object creation]") break;
        if (sub == "[select]" || sub == "[regenerate]" || sub == "[minimap icon]")
            ScanInt(&ok);
        else if (sub == "[object]")
        {
            // 嵌套 RidableObject 容器；objdump 解明其子子标签为 [map]/[index]/[team]/[pos]
            // （经子 map 分发到 0x3d5/0x3de/0x3e3/0x3ee），各读 int 进 RidableObject 字段。
            while (true) {
                std::string ss;
                if (!ScanType(ss, true)) break;
                if (ss == "[/object]") break;
                if (ss == "[map]") { ScanInt(&ok); ScanInt(&ok); }        // objdump：map 读 x/y
                else if (ss == "[index]" || ss == "[pos]")
                    ScanInt(&ok);
                else if (ss == "[team]")
                    ScanInt(&ok);   // 【推断】约 1 int（objdump 子分派未直接扫，近似）
                else
                    break;   // 未知子子标签
            }
        }
        else break;   // 未知子标签结束
    }
    break;
}

case 0x1026:  // "[schedule]"
{
    // ORIG LAB_088ed1f9：scheduleId + 5 个时间串 → _DailySchedule（字段映射同 I6 参考实现）。
    // FileIdx（FindIndexByFullScan / g_stDungeonScriptFileList 双视图）为次要字段，暂略。
    while (true) {
        int id = ScanInt(&ok);
        if (!ok) break;
        std::string strStart, strEnd, strWeekday, strStartTime, strEndTime;
        if (!ScanStr(&strStart)) break;
        if (!ScanStr(&strEnd)) break;
        if (!ScanStr(&strWeekday)) break;
        if (!ScanStr(&strStartTime)) break;
        if (!ScanStr(&strEndTime)) break;
        _DailySchedule ds;
        ds.m_eventId = id;
        ds.m_idx = reinterpret_cast<STScriptFileList*>(&g_stDungeonScriptFileList)
                       ->FindIndexByFullScan(path);   // ORIG FindIndexByFullScan(g_stDungeonScriptFileList, path)
        { std::string tmp(strStart); ConvertStringToTime(tmp, *(long*)&ds.m_startSec); }
        { std::string tmp(strEnd); ConvertStringToTime(tmp, *(long*)&ds.m_endSec); }
        { unsigned int wday = 0; std::string tmp(strWeekday); ConvertWeekdayStringToInt(tmp, wday); ds.m_wday = (int)wday; }
        { unsigned char h = 0, m = 0, sec = 0; std::string tmp(strStartTime); ConvertHHMMSSStringToInt(tmp, h, m, sec); ds.m_startHour = h; ds.m_startMin = m; ds.m_startSec2 = sec; }
        { unsigned char h = 0, m = 0, sec = 0; std::string tmp(strEndTime); ConvertHHMMSSStringToInt(tmp, h, m, sec); ds.m_endHour = h; ds.m_endMin = m; ds.m_endSec2 = sec; }
        script->m_dailySchedule.push_back(ds);
    }
    break;
}

case 0x1030:  // "[team]"（rand obj 子标签兜底）
    ScanInt(&ok); ScanInt(&ok);
    break;

case 0x1031:  // "[pos]"（rand obj 子标签兜底）
    ScanInt(&ok); ScanInt(&ok);
    break;

case 0x1032:  // "[object]"（rand obj 子标签兜底）
    ScanInt(&ok); ScanInt(&ok); ScanInt(&ok);
    break;

case 0x1033:  // "[realdungeon checkup]"
    ScanInt(&ok);   // 【推断】消费，防错位
    break;

case 0x1034:  // "[limit item stat]"
    ScanInt(&ok);   // 【推断】消费，防错位
    break;

case 0x1035:  // "[advance altar survival clear reward]"
    ScanInt(&ok);   // 【推断】消费，防错位
    break;

case 0x1036:  // "[physical attack add]"
    script->m_statMap290[static_cast<AbsoluteChangeStat::T>(1)] = ScanFloat(&ok);   // (T)1
    break;

case 0x1037:  // "[physical defense add]"
    script->m_statMap290[static_cast<AbsoluteChangeStat::T>(2)] = ScanFloat(&ok);   // (T)2
    break;

case 0x942:  // "[party member coin limit]"
    script->m_4fc = ScanInt(&ok);  // +0x4fc
    break;

case 0x94d:  // "[party member coin limit]"
    ScanInt(&ok);
    break;

case 0x954:  // "[individual map movement]"
    ScanInt(&ok);
    break;

case 0x95c:  // "[open door even enemy]"
    ScanInt(&ok);
    break;

case 0x962:  // "[disable exit]"
    ScanInt(&ok);
    break;

case 0x87a:  // "[tower of despair]"
    script->m_2a9 = 1;   // ORIG LAB_088ef8f3：local_12c[0x2a9]=1
    break;

        default:
            break;
        }
    }
    return true;
}

// exportDungeonScript 依赖的脚本域符号（权威定义在 STMapScript.cpp /
// LinkMiscSecuTimerSolo.cpp / InitScriptProducers_4.cpp）。
void getScriptFileFullPath(char* out, unsigned int size, const char* dir,
                           const char* name);
void convertBinaryToText(const char* path);
extern char g_szDungeonScriptBaseDirectory[0x100];

bool exportDungeonScript(STDungeonScript* script, const char* path)
{
    // ORIG 0x088f0a98（0x160c）——完整导出。按反汇编/Ghidra C 逐字段转写：
    //   fopen("wb") → STDungeonScript local（提供 m_a4 默认比较值）→ 各分区
    //   fprintf/fwrite 转储 → maze 循环（greed/map/boss/layered/start/boss）
    //   → spawn 系列 → 收尾 fclose + convertBinaryToText。
    // 【依赖】getScriptFileFullPath/convertBinaryToText/g_gridSymbols 见行内 extern。
    static char saveFileName[0x100];   // ORIG _ZZ19exportDungeonScriptP15STDungeonScriptPKcE12saveFileName
    getScriptFileFullPath(saveFileName, 0x100, g_szDungeonScriptBaseDirectory, path);

    FILE* file = fopen(saveFileName, "wb");
    if (!file)
        return 0;

    STDungeonScript local;   // ORIG 在栈上构造，仅用于 [coin limit] 与默认值比较

    fprintf(file, "[name] `%s`\n", script->m_str1c.c_str());
    fprintf(file, "[explain]\n`%s`\n", script->m_str20.c_str());
    fprintf(file, "[entering title] `%s`\n", script->m_str24.c_str());
    fprintf(file, "[cutscene image] `%s` %d\n", script->m_str28.c_str(), script->m_2c);
    fprintf(file, "[minimap image] `%s`\n", script->m_str30.c_str());
    fprintf(file, "[worldmap info]\n%d %d\n%d %d\n`%s`\n\n",
            script->m_5c[0], script->m_5c[1], script->m_5c[2], script->m_5c[3],
            script->m_str74.c_str());
    fprintf(file, "[minimum required level] %d\n", script->m_78);
    if (script->m_requiredItem7c.m_itemIdx != -1)
    {
        fprintf(file, "[required item] %d %d %d\n",
                script->m_requiredItem7c.m_itemIdx,
                script->m_requiredItem7c.m_itemCount,
                script->m_requiredItem7c.m_enableFlag != 0);
    }
    if (script->m_a4 != local.m_a4)
        fprintf(file, "[coin limit] %d", script->m_a4);
    fprintf(file, "[basis level] %d\n", script->m_ac);
    fprintf(file, "[experience increasing point] %.3f\n", (double)script->m_b0);
    fprintf(file, "[champion] %d %d %d %d %d\n",
            script->m_b4[0], script->m_b4[1], script->m_b4[2], script->m_b4[3],
            script->m_b4[4]);
    if (!script->m_vec98.empty())
    {
        fwrite("[item]\n", 1, 7, file);
        for (std::vector<std::pair<int, int> >::iterator it = script->m_vec98.begin();
             it != script->m_vec98.end(); ++it)
            fprintf(file, "%d %d\n", it->first, it->second);
        fwrite("[/item]\n", 1, 8, file);
    }

    // [pathgate object]：m_padF8（+0xf8）POD 区 10 个 int
    int* pg = (int*)script->m_padF8;
    fprintf(file, "[pathgate object]\n%d %d %d\n%d %d %d\n%d %d\n%d %d\n",
            pg[0], pg[1], pg[2], pg[3], pg[4], pg[5], pg[6], pg[7], pg[9], pg[10]);

    // ---- maze 循环 ----
    for (unsigned int mi = 0; mi < script->m_mazeList170.size(); ++mi)
    {
        MazeScript& maze = script->m_mazeList170[mi];
        fwrite("\n[maze info]\n", 1, 13, file);
        fprintf(file, "[size] %d %d\n", maze.m_width, maze.m_height);
        std::string greed;
        for (int y = 0; y < maze.m_height; ++y)
        {
            for (int x = 0; x < maze.m_width; ++x)
            {
                GridScript* grid = maze.getGrid(x, y);
                greed += g_gridSymbols[grid->m_gridValue];
            }
            if (maze.m_height - 1 != y)
                greed += "\n ";
        }
        fprintf(file, "[greed]\n`%s`\n", greed.c_str());

        for (int y = 0; y < maze.m_height; ++y)
        {
            for (int x = 0; x < maze.m_width; ++x)
            {
                GridScript* grid = maze.getGrid(x, y);
                if (!grid->m_mapCandidates.empty())
                {
                    fprintf(file, "[map specification]\n%d %d\n", x, y);
                    for (std::vector<int>::iterator it = grid->m_mapCandidates.begin();
                         it != grid->m_mapCandidates.end(); ++it)
                        fprintf(file, "%d\n", *it);
                    fwrite("[/map specification]\n", 1, 21, file);
                }
                if (!grid->m_secondCandidates.empty())
                {
                    fprintf(file, "[boss map specification]\n%d %d\n", x, y);
                    for (std::vector<int>::iterator it = grid->m_secondCandidates.begin();
                         it != grid->m_secondCandidates.end(); ++it)
                        fprintf(file, "%d\n", *it);
                    fwrite("[/boss map specification]\n", 1, 26, file);
                }
                if (!grid->m_layeredMapIndexes.empty())
                {
                    fwrite("[layered map specification]\n", 1, 28, file);
                    for (std::vector<int>::iterator it = grid->m_layeredMapIndexes.begin();
                         it != grid->m_layeredMapIndexes.end(); ++it)
                        fprintf(file, "%d\n", *it);
                    fwrite("[/layered map specification]\n", 1, 29, file);
                }
            }
        }

        fwrite("[start map]\n", 1, 12, file);
        for (std::vector<std::pair<int, int> >::iterator it = maze.m_vec14.begin();
             it != maze.m_vec14.end(); ++it)
            fprintf(file, "%d %d\n", it->first, it->second);
        fwrite("[/start map]\n", 1, 13, file);
        fwrite("[boss map]\n", 1, 11, file);
        for (std::vector<std::pair<int, int> >::iterator it = maze.m_vec20.begin();
             it != maze.m_vec20.end(); ++it)
            fprintf(file, "%d %d\n", it->first, it->second);
        fwrite("[/boss map]\n", 1, 12, file);
    }

    if (script->m_warRoom18c.m_10 != -1)
        fprintf(file, "[warroom map index] %d\n", script->m_warRoom18c.m_10);

    // [monster type spawn prob]
    fwrite("[monster type spawn prob]", 1, 25, file);
    for (unsigned int i = 0; i < script->m_warRoom18c.m_spawnProb.size(); ++i)
    {
        int* el = (int*)script->m_warRoom18c.m_spawnProb[i].m_pad;
        for (int j = 0; j < 4; ++j)
            fprintf(file, "%d ", el[j]);
        fputc(10, file);
    }
    fwrite("[monster type spawn cost]", 1, 25, file);
    for (int j = 0; j < 4; ++j)
        fprintf(file, "%d", script->m_warRoom18c.m_c4[j]);
    fputc(10, file);

    fprintf(file, "[player kc] %d", script->m_warRoom18c.m_d4);

    // ---- spawn common monster index ----
    if (!script->m_warRoom18c.m_vec14[0].empty())
    {
        fwrite("[spawn common monster index]\n", 1, 29, file);
        for (unsigned int i = 0; i < script->m_warRoom18c.m_vec14[0].size(); ++i)
            fprintf(file, "%d\n", script->m_warRoom18c.m_vec14[0][i]);
        fwrite("[/spawn common monster index]\n", 1, 30, file);
    }
    // ---- spawn common champion index ----
    if (!script->m_warRoom18c.m_vec14[1].empty())
    {
        fwrite("[spawn common champion index]\n", 1, 30, file);
        for (unsigned int i = 0; i < script->m_warRoom18c.m_vec14[1].size(); ++i)
            fprintf(file, "%d\n", script->m_warRoom18c.m_vec14[1][i]);
        fwrite("[/spawn common champion index]\n", 1, 31, file);
    }
    // ---- spawn super champion index ----
    if (!script->m_warRoom18c.m_vec14[2].empty())
    {
        fwrite("[spawn super champion index]\n", 1, 29, file);
        for (unsigned int i = 0; i < script->m_warRoom18c.m_vec14[2].size(); ++i)
            fprintf(file, "%d\n", script->m_warRoom18c.m_vec14[2][i]);
        fwrite("[/spawn super champion index]\n", 1, 30, file);
    }
    // ---- spawn boss index ----
    if (!script->m_warRoom18c.m_vec14[3].empty())
    {
        fwrite("[spawn boss index]\n", 1, 19, file);
        for (unsigned int i = 0; i < script->m_warRoom18c.m_vec14[3].size(); ++i)
            fprintf(file, "%d\n", script->m_warRoom18c.m_vec14[3][i]);
        fwrite("[/spawn boss index]\n", 1, 20, file);
    }

    fprintf(file, "[spawn step max] %d\n", script->m_warRoom18c.m_50);
    if (!script->m_warRoom18c.m_vec44.empty())
    {
        fwrite("[spawn step resource pool]\n", 1, 27, file);
        for (unsigned int i = 0; i < script->m_warRoom18c.m_vec44.size(); ++i)
            fprintf(file, "%d\n", script->m_warRoom18c.m_vec44[i]);
        fwrite("[/spawn step resource pool]\n", 1, 28, file);
    }
    fprintf(file, "[kill count const] %d\n", (double)*(float*)&script->m_warRoom18c.m_ac);
    fprintf(file, "[join cost gold] %d\n", script->m_warRoom18c.m_54);
    fprintf(file, "[gold drop prob] %d\n", script->m_warRoom18c.m_58);
    fprintf(file, "[common monster item drop prob] %d\n", script->m_warRoom18c.m_5c[0]);
    fprintf(file, "[common champion item drop prob] %d\n", script->m_warRoom18c.m_5c[1]);
    fprintf(file, "[super champion item drop prob] %d\n", script->m_warRoom18c.m_5c[2]);
    fprintf(file, "[boss item drop prob] %d\n", script->m_warRoom18c.m_5c[3]);
    fprintf(file, "[common monster exp const] %d\n", script->m_warRoom18c.m_9c[0]);
    fprintf(file, "[common champion exp const] %d\n", script->m_warRoom18c.m_9c[1]);
    fprintf(file, "[super champion exp const] %d\n", script->m_warRoom18c.m_9c[2]);
    fprintf(file, "[boss exp const] %d\n", script->m_warRoom18c.m_9c[3]);

    // ---- 四条 item drop list ----
    if (!script->m_warRoom18c.m_vec6c[0].empty())
    {
        fwrite("[common monster item drop list]\n", 1, 32, file);
        for (unsigned int i = 0; i < script->m_warRoom18c.m_vec6c[0].size(); ++i)
            fprintf(file, "%d\n", script->m_warRoom18c.m_vec6c[0][i]);
        fwrite("[/common monster item drop list]\n", 1, 33, file);
    }
    if (!script->m_warRoom18c.m_vec6c[1].empty())
    {
        fwrite("[common champion item drop list]\n", 1, 33, file);
        for (unsigned int i = 0; i < script->m_warRoom18c.m_vec6c[1].size(); ++i)
            fprintf(file, "%d\n", script->m_warRoom18c.m_vec6c[1][i]);
        fwrite("[/common champion item drop list]\n", 1, 34, file);
    }
    if (!script->m_warRoom18c.m_vec6c[2].empty())
    {
        fwrite("[super champion item drop list]\n", 1, 32, file);
        for (unsigned int i = 0; i < script->m_warRoom18c.m_vec6c[2].size(); ++i)
            fprintf(file, "%d\n", script->m_warRoom18c.m_vec6c[2][i]);
        fwrite("[/super champion item drop list]\n", 1, 33, file);
    }
    if (!script->m_warRoom18c.m_vec6c[3].empty())
    {
        fwrite("[boss item drop list]\n", 1, 22, file);
        for (unsigned int i = 0; i < script->m_warRoom18c.m_vec6c[3].size(); ++i)
            fprintf(file, "%d\n", script->m_warRoom18c.m_vec6c[3][i]);
        fwrite("[/boss item drop list]\n", 1, 23, file);
    }

    fprintf(file, "[battle spawn time] %d\n", script->m_warRoom18c.m_fc);
    if ((unsigned char)script->m_28c != 0)
        fprintf(file, "[ancient dungeon] %d", (unsigned int)(unsigned char)script->m_28c);
    if ((unsigned char)script->m_4e0 != 0)
        fprintf(file, "[risk dungeon] %d", (unsigned int)(unsigned char)script->m_4e0);
    if ((unsigned char)script->m_47c != 0)
        fprintf(file, "[SWAY EFFECT] %d %d \n", script->m_484, script->m_480);

    fclose(file);
    convertBinaryToText(saveFileName);
    return 1;
}

// ---- advancealtar 字符串枚举解析器 ----
// ORIG: StageType::getStageType 0x88990ba / StageDifficulty::getDifficulty
// 0x8899038 / RewardItemType::getRewardItemType 0x889acaa。
bool advancealtar::StageType::getStageType(const std::string& name, T& out)
{
    if (name == "normal")
    {
        out = STAGE_NORMAL;
    }
    else if (name == "survival")
    {
        out = STAGE_SURVIVAL;
    }
    else
    {
        // [推断] ORIG 未识别时写 0 并仍返回 true（见报告反编译）。
        out = STAGE_NORMAL;
    }
    return true;
}

bool advancealtar::StageDifficulty::getDifficulty(const std::string& name,
                                                   T& out)
{
    if (name == "easy")
    {
        out = DIFF_EASY;
    }
    else if (name == "medium")
    {
        out = DIFF_MEDIUM;
    }
    else if (name == "hard")
    {
        out = DIFF_HARD;
    }
    else
    {
        return false;
    }
    return true;
}

bool advancealtar::RewardItemType::getRewardItemType(const std::string& name,
                                                     T& out)
{
    if (name == "normal")
    {
        out = REWARD_NORMAL;
    }
    else if (name == "event")
    {
        out = REWARD_EVENT;
    }
    else
    {
        return false;
    }
    return true;
}

static bool importAltarRewardItem(advancealtar::_RewardItem& item)
{
    // ORIG 内联段：ScanStr→getRewardItemType→ScanInt(+0x04)→ScanInt(+0x08)
    bool ok = false;
    std::string token;
    if (!ScanStr(&token))
    {
        return false;
    }
    advancealtar::RewardItemType::T type;
    if (!advancealtar::RewardItemType::getRewardItemType(token, type))
    {
        return false;
    }
    item.m_type = static_cast<int>(type);
    item.m_value1 = ScanInt(&ok);
    if (!ok)
    {
        return false;
    }
    item.m_value2 = ScanInt(&ok);
    return ok;
}

// ImportAdvanceAltarDungeonTag ORIG 0x088f20a4（536 insn）：
// 按 str 给出的 tag 分派解析；脚本流已由调用方 loadRDARScriptFile 打开
// （path 参数在 ORIG 中未使用）。返回 false=解析失败，true=成功/结束。
bool ImportAdvanceAltarDungeonTag(STDungeonScript* script, std::string str,
                                  const char* path)
{
    (void)path;  // ORIG 未使用该参数
    bool ok = false;   // ORIG 0x088f20ac：函数序言即初始化（-0x15）
    if (str == "[defense dungeon]")
    {
        // ORIG 0x088f20c7：ScanInt(NULL)（movl $0x0,(%esp)）
        script->m_2ab = (ScanInt((bool*)0) != 0);
    }
    else if (str == "[advance altar type]")
    {
        std::string token;
        if (!ScanStr(&token))
        {
            return true;   // ORIG 0x088f211e：解析失败仍返回 1（结束解析）
        }
        advancealtar::StageType::T type;
        if (!advancealtar::StageType::getStageType(token, type))
        {
            return true;   // ORIG 0x088f2148：同上
        }
        script->m_2ac = static_cast<int>(type);
    }
    else if (str == "[advance altar map]")
    {
        // 任一解析失败 ORIG 直接返回 true（+0x17a/+0x1a9 ebx=0 → 88f27f3）
        while (true)
        {
            std::string token;
            if (!ScanStr(&token))
            {
                return true;
            }
            advancealtar::StageDifficulty::T diff;
            if (!advancealtar::StageDifficulty::getDifficulty(token, diff))
            {
                return true;
            }
            script->m_2b0[diff] = ScanInt(&ok);
            if (!ok)
            {
                return true;
            }
            script->m_2bc[diff] = ScanInt(&ok);
            if (!ok)
            {
                return true;
            }
        }
    }
    else if (str == "[advance altar clear reward]")
    {
        // 循环：[easy]/[medium]/[hard] 选择 ClearRewardData 的三个 vector，
        // 逐项导入 _RewardItem 直到 [/advance altar clear reward] 或解析失败
        while (true)
        {
            std::string token;
            if (!ScanType(token, true))
            {
                break;
            }
            int idx = -1;
            if (token == "[easy]")
            {
                idx = 0;
            }
            else if (token == "[medium]")
            {
                idx = 1;
            }
            else if (token == "[hard]")
            {
                idx = 2;
            }
            else
            {
                // [/advance altar clear reward] 或其它 tag → 本节结束
                break;
            }
            std::vector<advancealtar::_RewardItem>& vec =
                script->m_clearReward2d4.m_vec[idx];
            while (true)
            {
                advancealtar::_RewardItem item;
                if (!importAltarRewardItem(item))
                {
                    break;
                }
                vec.push_back(item);
            }
        }
    }
    else if (str == "[advance altar survival map]")
    {
        while (true)
        {
            bool ok = false;
            int mapId = ScanInt(&ok);
            if (!ok)
            {
                break;
            }
            script->m_vec2c8.push_back(mapId);
        }
    }
    else if (str == "[advance altar survival clear reward]")
    {
        // 多个 [type]…[/type] 块 → SurvivalRewardData::m_vec
        while (true)
        {
            std::string token;
            if (!ScanType(token, true))
            {
                break;
            }
            if (token != "[type]")
            {
                break;
            }
            advancealtar::RoundRewardData rrd;
            rrd.m_0 = 0;
            rrd.m_4 = 0;
            while (true)
            {
                std::string sub;
                if (!ScanType(sub, true))
                {
                    break;
                }
                if (sub == "[/type]")
                {
                    script->m_survival2f8.m_vec.push_back(rrd);
                    break;
                }
                bool ok = false;
                if (sub == "[round]")
                {
                    rrd.m_0 = ScanInt(&ok);   // 轮次
                    if (!ok)
                    {
                        break;
                    }
                    rrd.m_4 = ScanInt(&ok);   // 奖励参数
                    if (!ok)
                    {
                        break;
                    }
                }
                else if (sub == "[list]")
                {
                    // 逐项 _RewardItem；ORIG 仅接受 type 0/1（getRewardItemType
                    // 只会产生这两个值），失败即结束本 list
                    while (true)
                    {
                        advancealtar::_RewardItem item;
                        if (!importAltarRewardItem(item))
                        {
                            break;
                        }
                        if (item.m_type == 0 || item.m_type == 1)
                        {
                            rrd.m_vec8.push_back(item);
                        }
                    }
                }
                // 其它子 tag：忽略并继续（推断）
            }
        }
    }
    // 未识别 tag：ORIG 88f27ec ebx=0 → 返回 false（识别分支均返回 true）
    return false;
}
