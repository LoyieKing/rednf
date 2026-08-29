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

namespace {
// ORIG 标签分派表（主 map 0x94f7040，155 标签；重复标签取最小 case 值）
static int DungeonTagLookup(const std::string& tag)
{
    if (tag == "[name]") return 117;
    if (tag == "ByDifficulty") return 152;
    if (tag == "[cutscene image]") return 185;
    if (tag == "[dungeon loading image path]") return 190;
    if (tag == "[worldmap info]") return 202;
    if (tag == "[herosmode required quest]") return 207;
    if (tag == "[background pos]") return 221;
    if (tag == "[monsterapc diff table]") return 255;
    if (tag == "[minimum required level]") return 269;
    if (tag == "[required item]") return 276;
    if (tag == "[event dungeon]") return 280;
    if (tag == "[event required item]") return 286;
    if (tag == "[recommended level]") return 396;
    if (tag == "[coin limit]") return 405;
    if (tag == "[character coin limit]") return 413;
    if (tag == "[basis level]") return 417;
    if (tag == "[champion]") return 422;
    if (tag == "[item]") return 435;
    if (tag == "[dungeon drop]") return 489;
    if (tag == "[dungeon object drop]") return 511;
    if (tag == "[pathgate object]") return 541;
    if (tag == "[maze info]") return 552;
    if (tag == "[quest connection]") return 650;
    if (tag == "../RDARScriptDungeon.cpp") return 702;
    if (tag == "[party num check]") return 708;
    if (tag == "[size]") return 720;
    if (tag == "[map specification]") return 782;
    if (tag == "[boss map specification]") return 798;
    if (tag == "[layered map specification]") return 814;
    if (tag == "[start map]") return 830;
    if (tag == "[boss map]") return 853;
    if (tag == "[hell dungeon]") return 859;
    if (tag == "[prohibit practice]") return 866;
    if (tag == "[disable skill]") return 871;
    if (tag == "[/impossible skills]") return 883;
    if (tag == "[use countdown]") return 897;
    if (tag == "[use zoom rate]") return 902;
    if (tag == "[usable item]") return 906;
    if (tag == "[seal door map index]") return 914;
    if (tag == "[seal door pos]") return 919;
    if (tag == "[hell coin limit]") return 923;
    if (tag == "[dead tower map index]") return 933;
    if (tag == "[select]") return 947;
    if (tag == "[regenerate]") return 957;
    if (tag == "[minimap icon]") return 962;
    if (tag == "[map]") return 981;
    if (tag == "[index]") return 995;
    if (tag == "[neutral]") return 1006;
    if (tag == "[point by type]") return 1057;
    if (tag == "[phase fatigue]") return 1068;
    if (tag == "[warroom map index]") return 1075;
    if (tag == "[monster exp bonus per user decrease]") return 1078;
    if (tag == "[monster type spawn cost]") return 1110;
    if (tag == "[monster type spawn interval rate]") return 1119;
    if (tag == "[monster spawn base interval]") return 1122;
    if (tag == "[monster spawn random interval]") return 1128;
    if (tag == "[fatigue]") return 1131;
    if (tag == "[fatigue result]") return 1134;
    if (tag == "[max monster]") return 1138;
    if (tag == "[spawn common monster index]") return 1151;
    if (tag == "[spawn common champion index]") return 1164;
    if (tag == "[spawn super champion index]") return 1177;
    if (tag == "[spawn boss index]") return 1190;
    if (tag == "[spawn step max]") return 1195;
    if (tag == "[spawn step resource pool]") return 1209;
    if (tag == "[kill count const]") return 1214;
    if (tag == "[join cost gold]") return 1224;
    if (tag == "[gold drop prob]") return 1229;
    if (tag == "[common monster item drop prob]") return 1234;
    if (tag == "[common champion item drop prob]") return 1239;
    if (tag == "[super champion item drop prob]") return 1244;
    if (tag == "[boss item drop prob]") return 1252;
    if (tag == "[common monster exp const]") return 1260;
    if (tag == "[common champion exp const]") return 1268;
    if (tag == "[super champion exp const]") return 1276;
    if (tag == "[common monster item drop list]") return 1288;
    if (tag == "[common champion item drop list]") return 1300;
    if (tag == "[super champion item drop list]") return 1312;
    if (tag == "[boss item drop list]") return 1324;
    if (tag == "[battle spawn time]") return 1332;
    if (tag == "[powerwar dungeon]") return 1341;
    if (tag == "[blood dungeon]") return 1359;
    if (tag == "[move speed]") return 1369;
    if (tag == "[physical attack add]") return 1378;
    if (tag == "[physical defense add]") return 1381;
    if (tag == "[limit escape character]") return 1398;
    if (tag == "[defense dungeon]") return 1408;
    if (tag == "[advance altar map]") return 1422;
    if (tag == "[advance altar clear reward]") return 1442;
    if (tag == "[advance altar survival map]") return 1497;
    if (tag == "[dimension dungeon]") return 1570;
    if (tag == "[necessary party]") return 1584;
    if (tag == "[village attack revenge dungeon]") return 1621;
    if (tag == "[limit party count]") return 1687;
    if (tag == "[hit count]") return 1701;
    if (tag == "[seal door appear rate]") return 1704;
    if (tag == "[dummy appear count]") return 1738;
    if (tag == "advance altar") return 1781;
    if (tag == "[tower random map indexes]") return 1784;
    if (tag == "[death tower map indexes]") return 1822;
    if (tag == "[named monster map pos]") return 1836;
    if (tag == "[blood max round]") return 1860;
    if (tag == "[event monster]") return 1894;
    if (tag == "[tower high skill initial cool time]") return 1961;
    if (tag == "[tower high skill initial cool time rate]") return 1969;
    if (tag == "[tower fp cubepiece]") return 1972;
    if (tag == "[tower limit of stackable item]") return 1975;
    if (tag == "[tower dialog]") return 1998;
    if (tag == "[tower item drop]") return 2078;
    if (tag == "[tower max clear item num]") return 2084;
    if (tag == "[evil]") return 2089;
    if (tag == "[evil high level]") return 2100;
    if (tag == "[monster difficulty bonus]") return 2108;
    if (tag == "[/monster difficulty bonus]") return 2141;
    if (tag == "[gold card use]") return 2158;
    if (tag == "[SWAY EFFECT]") return 2164;
    if (tag == "[no fatigue]") return 2170;
    if (tag == "[quest npc dungeon]") return 2185;
    if (tag == "[designate dungeon difficulty]") return 2190;
    if (tag == "[tournament dungeon]") return 2195;
    if (tag == "[tournament round fatigue]") return 2200;
    if (tag == "[tournament clear reward gold rate]") return 2285;
    if (tag == "[risk dungeon]") return 2316;
    if (tag == "[easy]") return 2333;
    if (tag == "[medium]") return 2348;
    if (tag == "[party member coin limit]") return 2370;
    if (tag == "[individual map movement]") return 2388;
    if (tag == "[open door even enemy]") return 2396;
    if (tag == "[disable exit]") return 2402;
    return -1;
}
}  // namespace

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

    // 标签分派：DungeonTagLookup 返回 ORIG case 值，switch 分派。
    // 复杂容器/子标签 case 按 ORIG 反汇编转写，无法完全确认处标注【推断】。
    while (ScanType(line, true))
    {
        switch (DungeonTagLookup(line))
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
    ScanInt(&ok);
    ScanInt(&ok);
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
    ScanInt(&ok);
    ScanInt(&ok);
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
    ScanInt(&ok);  // x
    ScanInt(&ok);  // y
    {
        GridScript* g = mazeLocal.getGrid(v1, v2);
        g->m_mapCandidates.clear();
        while (ScanInt(&ok)) {
            g->m_mapCandidates.push_back(v1);
        }
    }
    break;

case 0x31e:  // "[boss map specification]"
    // [boss map specification]：同 [map specification]
    ScanInt(&ok);
    ScanInt(&ok);
    {
        GridScript* g = mazeLocal.getGrid(v1, v2);
        g->m_mapCandidates.clear();
        while (ScanInt(&ok)) {
            g->m_mapCandidates.push_back(v1);
        }
    }
    break;

case 0x32e:  // "[layered map specification]"
    // [layered map specification]：同 [map specification]
    ScanInt(&ok);
    ScanInt(&ok);
    {
        GridScript* g = mazeLocal.getGrid(v1, v2);
        g->m_mapCandidates.clear();
        while (ScanInt(&ok)) {
            g->m_mapCandidates.push_back(v1);
        }
    }
    break;

case 0x33e:  // "[start map]"
    // [start map]：start map 对 (x,y) 读入（局部 pair 列表）
    ScanInt(&ok);
    ScanInt(&ok);
    break;

case 0x34a:  // "[start map]"
    // [start map]：同 0x33e
    ScanInt(&ok);
    ScanInt(&ok);
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
    while (ScanInt(&ok)) {
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
    while (ScanInt(&ok)) {
        script->m_warRoom18c.m_vec6c[0].push_back(v1);  // 【推断】目标 vector 待核
    }
    break;

case 0x47f:  // "[spawn common monster index]"
    while (ScanInt(&ok)) {
        script->m_warRoom18c.m_vec6c[1].push_back(v1);
    }
    break;

case 0x48c:  // "[spawn common champion index]"
    while (ScanInt(&ok)) {
        script->m_warRoom18c.m_vec6c[2].push_back(v1);
    }
    break;

case 0x499:  // "[spawn super champion index]"
    while (ScanInt(&ok)) {
        script->m_warRoom18c.m_vec6c[3].push_back(v1);
    }
    break;

case 0x4a6:  // "[spawn boss index]"
    script->m_warRoom18c.m_50 = ScanInt(&ok);  // +0x1dc
    break;

case 0x4ab:  // "[spawn step max]"
    while (ScanInt(&ok)) {
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
    while (ScanInt(&ok)) {
        script->m_warRoom18c.m_vec44.push_back(v1);  // 【推断】
    }
    break;

case 0x508:  // "[common monster item drop list]"
    while (ScanInt(&ok)) {
        script->m_warRoom18c.m_vec14[1].push_back(v1);  // 【推断】
    }
    break;

case 0x514:  // "[common champion item drop list]"
    while (ScanInt(&ok)) {
        script->m_warRoom18c.m_vec14[2].push_back(v1);  // 【推断】
    }
    break;

case 0x520:  // "[super champion item drop list]"
    while (ScanInt(&ok)) {
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
        default:
            break;
        }
    }
    return true;
}

bool exportDungeonScript(STDungeonScript* script, const char* path)
{
    FILE* file = fopen(path, "w");
    if (!file)
    {
        return false;
    }
    fprintf(file, "%d\n%d\n", script->m_0, script->m_4);
    fclose(file);
    return true;
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
