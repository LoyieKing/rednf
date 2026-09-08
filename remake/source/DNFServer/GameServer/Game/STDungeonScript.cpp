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
#include "CEnvironment.h"             // less_TCHAR：ORIG map 按字符串内容比较
#include "DNFLexWrapper.h"
#include "DataManagerInitDepends.h"   // getCharacterJobToEnumValue 声明（[impossible skills]）
#include "LogManager.h"               // ORIG clear-condition 错误日志
#include "NpcScript.h"                // STScriptFileList（[schedule] 双视图）
#include "STWorldmapPattern.h"        // STWorldmapPattern（[worldmap pattern info] 局部）

// ORIG _Z25GetWorldMapCoordFromTableiiR17STWorldmapPattern 0x088e5ef8 T（WorldMapScript.h 权威声明）
void GetWorldMapCoordFromTable(int a, int b, STWorldmapPattern& out);

extern std::map<int, std::string> g_stDungeonScriptFileList;   // 定义 InitScriptProducers_4.cpp（双视图 STScriptFileList）

// ORIG _ZL13g_gridSymbols（[greed] 网格符号，16 项两字符编码；STMapScript.cpp 同表为文件内
// static，本 TU 单独复现以支撑 import/exportDungeonScript 引用）。
static const char* const g_gridSymbols[16] = {
    "AA", "BB", "CC", "DD", "EE", "FF", "GG", "HH",
    "II", "JJ", "KK", "LL", "MM", "NN", "OO", "PP",
};

// ORIG B 0x94f6da0（权威定义 InitScriptProducers_4.cpp；目录前缀，import 传 loadRDARScriptFile）
extern char g_szDungeonScriptBaseDirectory[0x100];

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
    // b28d-L3: DIFF/P212+N82 — ORIG 0x... 帧 0x24 + 二维索引 `mov -0x18,%ecx; mov -0x14,%edx; add %ecx,%ecx; lea (%ecx,%edx,1),%edx` vs 我方 0x34/-0
    // ORIG 0x0836abc6（473 insn 二进制切片逐段转写）：语句序、string 混用形
    // （assign "" = _ZNSsaSEPKc vs _ZNSs5clearEv）与物理循环全部按汇编还原。
    m_18 = 0;                                        // 0836abd0
    m_str1c = "";                                    // 0836abe8 assign
    m_str20 = "";
    m_str24 = "";
    m_str28 = "";
    m_2c = 0;
    m_str30 = "";
    m_34 = 0;
    m_vec38.clear();
    m_44 = 0;
    m_vec48.clear();
    m_14 = 0;
    m_vec3c8.clear();                                // 0836ac94 vector<STNamedMonsterMapPos>
    m_0 = 0;
    m_c = 0;                                         // movb 0836aca5
    // 0836acb0：ORIG 物理双层循环 (i<=2)×(j<=1)，地址 0xc+4*(0x14+2i+j)=+0x5c..
    for (int i = 0; i <= 2; ++i)
        for (int j = 0; j <= 1; ++j)
            m_5c[i * 2 + j] = 0;
    m_str10.clear();                                 // 0836ad02 本串用 clear 形
    m_str74 = "";                                    // 0836ad18 assign
    m_78 = 0;
    m_requiredItem7c.clear();                        // 0836ad30 RequiredItem::clear
    m_a4 = -1;
    m_str54 = "";                                    // 0836ad53 assign
    m_88 = 0;                                        // movb
    m_requiredItem8c.clear();
    m_vec98.clear();
    m_a8 = -1;
    m_ac = 1;
    m_b0 = 1.0f;
    for (int i = 0; i <= 4; ++i)                     // 0836adb0 物理循环 +0xb4+4i
        m_b4[i] = 0;
    m_vecc8.clear();                                 // 0836adde
    // 0836ade3：ORIG 物理双层循环 (i<=3)×(j<=2)，地址 +0xf8+(3i+j)*4 连续 12 int 写 -1
    for (int i = 0; i <= 3; ++i)
        for (int j = 0; j <= 2; ++j)
            m_pathgate.m_cells[i][j] = -1;
    m_mazeList170.clear();                           // 0836ae3a vector<MazeScript>
    m_str17c = "";                                   // 0836ae52 assign
    m_str1c.clear();                                 // 0836ae60：ORIG 对 +0x1c 二次操作
    m_str20.clear();                                 // 0836ae6e：同上 +0x20
    m_warRoom18c.clear();                            // 0836ae7e STWarRoomData::clear
    m_deadTower180.clear();                          // 0836ae8e STDeadTowerData::clear
    m_28c = 0;
    m_28d = 0;
    m_28e = 0;
    m_2ab = 0;
    m_2ac = 0;
    memset(&m_2b0, 0, 0xc);                          // 0836aee3 memset 形
    memset(&m_2bc, 0, 0xc);                          // 0836af03
    m_vec2c8.clear();
    m_clearReward2d4.clear();                        // advancealtar::ClearRewardData
    m_survival2f8.m_vec.clear();                     // vector<RoundRewardData>
    m_304 = 0;
    m_305 = 0;
    m_306 = 0;
    m_314 = 0;
    m_315 = 0;
    m_316 = 0;
    m_317 = 0;
    m_dailySchedule.clear();
    m_324 = 4;                                       // ORIG movb $0x4,0x324：默认队伍人数上限
    m_325 = 0;
    m_bossRoom328.clear();
    m_140 = 0;
    m_141 = 0;
    m_142 = 1;                                       // ORIG movb $0x1,0x142
    m_vec144.clear();
    m_150 = 0;
    m_154 = 0;
    m_158 = 0;
    m_159 = 1;                                       // ORIG movb $0x1,0x159
    for (int i = 0; i <= 4; ++i)                     // 0836b00c 物理循环 +0x128+4i
        m_128[i] = 0;
    m_13c = 0;
    m_168 = -1;                                      // ORIG movl $0xffffffff,0x168
    m_vecd4.clear();
    m_vece0.clear();
    m_vecec.clear();                                 // 0836b07a vector<stDungeonAssignItem_t>
    m_16c = 0;
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
    m_towerDialog390.clear();                        // STTowerFullDialog::clear
    m_3b4 = 1;                                       // ORIG movl $0x1,0x3b4
    m_3b8 = 0;
    m_3bc = 0;
    m_3c0 = 0;
    m_3d4 = 0;
    m_3d8 = 0;
    m_3dc = 0;
    m_vec3e0.clear();
    m_3ec = 0;
    m_3f0 = 0;
    m_3f4 = 0;
    m_vec3f8.clear();
    m_vec404.clear();
    for (int i = 0; i <= 4; ++i)                     // 0836b202 同一循环写 +0x410/+0x424
    {
        m_410[i] = 0;
        m_424[i] = 0;
    }
    m_438 = 0.0f;
    m_43c = 2.0f;
    m_440 = 0.0f;
    m_444 = 1.5f;
    m_str448.clear();                                // ORIG 此处用 _ZNSs5clearEv
    m_44c = 1;                                       // ORIG movb $0x1,0x44c
    m_454 = -1;                                      // ORIG 先写 0x454…
    m_450 = m_454;                                   // …再回读 0x454→0x450（0836b2a3）
    m_vec458.clear();
    m_vec464.clear();
    m_vec470.clear();
    m_47c = 0;
    m_484 = 0;                                       // ORIG 顺序 0x484 先于 0x480
    m_480 = 0;
    m_488 = 0;
    m_48c = 0;
    m_2a9 = 0;
    m_2aa = 0;
    m_490 = -1;                                      // ORIG movl $0xffffffff,0x490
    m_494 = 0;
    m_498 = 1;                                       // ORIG movl $0x1,0x498
    m_map49c.clear();
    m_itemRate4b4.Clear();
    m_4c4Value = 1.0f;
    m_map4c8.clear();
    m_4e0 = 0;
    m_map4e4.clear();
    m_500 = -1;                                      // ORIG 顺序 0x500 先于 0x4fc
    m_4fc = 0;
    m_504 = 0;
    m_505 = 0;
    m_28f = 0;
    m_statMap290.clear();
    m_2a8 = 0;
    m_508 = 0;
    m_50c = 0;
    // ORIG 未清：m_4/m_8、m_str58、m_vec15c、m_dimension308（按 ORIG 实证保留不清）
}
bool ImportDungeonScript(STDungeonScript* script, const char* path)
{
    // b28d-L3: DIFF/N91+N82+N89 — ORIG 0x08... 9263 vs 9404
    // 【验收标注 §6.2】超大函数（ORIG 9343 insn）。调用点差集已归零：
    // printf(3)/puts(2)/memset(2,N78 rep-stos 对位)/ScanInt 196+4 位点形态、
    // 静态五组 guard/atexit(10)、map insert(172)、string ctor/dtor 全部与
    // ORIG 切片一一对位（ORIG 侧 libstdc++ 无符号名地址已按 nearest 反查）。
    // 残余指令差（gap 256→131）归因：N82（4.1 帧 0x134c vs 4.4 0x138c 槽号
    // 全帧漂移）+ N78/N76（memset 内联/store RHS 物化）+ EH esi 载值死
    // test（4.4 jump-threading 岛，如 title 叶 `mov$1,%esi;test%esi`）+
    // difflib 对齐岛噪音（P182 不强凑）。语义已逐块核对完成。
    // ORIG 0x88e5fbd：loadRDARScriptFile(g_szDungeonScriptBaseDirectory, path)
    if (!loadRDARScriptFile(g_szDungeonScriptBaseDirectory, path))
    {
        return false;
    }
    script->Clear();
    script->m_vec384.clear();
    // ORIG 0x88e6027：getDirectoryPath(&script->m_str17c, "%s%s", 基目录, path)
    // → m_str17c 为脚本完整路径（目录 + 文件名）。
    getDirectoryPath(script->m_str17c, "%s%s", g_szDungeonScriptBaseDirectory, path);
    std::string line;
    std::string token;
    int iv0 = 0;      // ORIG -0x109c：子表 ScanInt(int*) 共用实参槽（Ghidra local_10a0）
    bool ok = false;  // ORIG -0x109d：ScanInt(bool*) 实参槽（Ghidra local_10a1）
    // ORIG 函数级局部（-0x1324 MazeScript / -0x121 flag）
    MazeScript mazeLocal;
    bool mazeInited = false;
    int v1;
    int v2;

    // ORIG 惰性初始化：五组静态（主表 + 4 张嵌套子表），每组带独立
    // __cxa_guard 初始化的 map/SpinLock 与 bool __inited_N。ORIG 的声明位于
    // 各自的使用循环内（LAB_088e608f / LAB_088e9bee / LAB_088ea0d8 /
    // LAB_088ebe77 / LAB_088f0202 处每轮 __cxa_guard 测试），因此下方把声明
    // 放进对应 while 循环体首；首过以 typeId=0 进入该表 case 链逐例 insert，
    // 链尾 leave + 置位 + goto 循环头重试当前 tag。
    typedef std::map<const char*, int, less_TCHAR> DungeonTagMap;
    int tagId;

    // 标签分派：主循环每轮 ScanType → __lookup 表查找 → switch。
    while (true)
    {
        // ORIG 0x88e606e：ScanType 线性置于循环头（4.4 对 while(cond) 会把
        // 检查挪到循环尾并加前置 jmp；改写成 if-break 恢复线性形态）。
        if (!ScanType(line, true))
            break;
    retry_dispatch:
        static DungeonTagMap __lookup;
        static bool __inited = false;
        static SpinLock __init_lock;
    tag_dispatch:
        if (__inited)
        {
            DungeonTagMap::const_iterator it =
                __lookup.find(line.c_str());
            tagId = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                goto retry_dispatch;
            }
            tagId = 0;   // ORIG：首过以 typeId=0 进入主表填充链（case 0）
        }
        switch (tagId)
        {
case 0:
case 0x75:  // "[name]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[name]", 0x75));
    }
    else
    {
        ScanStr(&script->m_str1c);  // +0x1c
        break;
    }

case 0x79:  // "[explain]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[explain]", 0x79));
    }
    else
    {
        ScanStr(&script->m_str20);  // +0x20
        break;
    }

case 0x7d:  // "[entering title]"（ORIG LAB_088e6d1f/88e6dcd：ByDifficulty→m_34=1
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[entering title]", 0x7d));
    }
    else
    {
        // 且循环收集到 m_vec38；否则首串直接赋 m_str24）
        {
            std::string title;
            if (!ScanStr(&title))
                break;
            if (title == "ByDifficulty") {  // 0x8e0d55a
                script->m_34 = 1;  // +0x34
                while (ScanStr(&title))  // @88e6dcd 循环 push 到 +0x38
                    script->m_vec38.push_back(title);  // @88e6dfb push_back(+0x38)
            }
            else
            {
                script->m_str24 = title;  // @88e6e18 SsaSERKSs（非 ByDifficulty 时单标题串）
            }
        }
        break;
    }

case 0x98:  // "[entering title next]"（ORIG LAB_088e6ead/88e6efb：同构 [entering title]，
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[entering title next]", 0x98));
    }
    else
    {
        // 目标 +0x44/+0x48 vector/+0x54）
        {
            std::string title;
            if (!ScanStr(&title))
                break;
            if (title == "ByDifficulty") {  // 0x8e0d55a
                script->m_44 = 1;  // +0x44
                while (ScanStr(&title))  // @88e6efb 循环 push 到 +0x48
                    script->m_vec48.push_back(title);  // @88e6f29 push_back(+0x48)
            }
            else
            {
                script->m_str54 = title;  // @88e6f46 SsaSERKSs（+0x54）
            }
        }
        break;
    }

case 0xb3:  // "[cutscene image]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[cutscene image]", 0xb3));
    }
    else
    {
        ScanStr(&script->m_str28);  // +0x28
        script->m_2c = ScanInt((bool*)0);  // +0x2c
        break;
    }

case 0xb9:  // "[dungeon loading image path]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[dungeon loading image path]", 0xb9));
    }
    else
    {
        ScanStr(&script->m_str58);  // +0x58
        break;
    }

case 0xbe:  // "[minimap image]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[minimap image]", 0xbe));
    }
    else
    {
        ScanStr(&script->m_str30);  // +0x30
        break;
    }

case 0xc3:  // "[worldmap info]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[worldmap info]", 0xc3));
    }
    else
    {
        // ORIG LAB_088e70e9（Ghidra C 实证）：2x2 循环 4 个 int → m_5c[4]
        // （偏移 (2i+j+0x14)*4+0xc = +0x5c..+0x68），随后 ScanStr → m_str74(+0x74)。
        for (int rr = 0; rr < 2; ++rr)
        {
            for (int cc = 0; cc < 2; ++cc)
            {
                script->m_5c[rr * 2 + cc] = ScanInt((bool*)0);
            }
        }
        ScanStr(&script->m_str74);
        break;
    }

case 0xca:  // "[herosmode required quest]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[herosmode required quest]", 0xca));
    }
    else
    {
        script->m_14 = ScanInt((bool*)0);  // +0x14
        break;
    }

case 0xcf:  // "[background pos]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[background pos]", 0xcf));
    }
    else
    {
        script->m_488 = ScanInt((bool*)0);  // +0x488
        break;
    }

case 0xd7:  // "[herosmode enable]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[herosmode enable]", 0xd7));
    }
    else
    {
        script->m_c = ScanInt((bool*)0) != 0;  // +0xc
        break;
    }

case 0xdd:  // "[worldmap pattern info]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[worldmap pattern info]", 0xdd));
    }
    else
    {
    {
        // ORIG LAB_088e7338（Ghidra C + STWorldmapPattern.h）：ScanInt x/y →
        // GetWorldMapCoordFromTable(x,y,pat) 取表格图案；把 pat.m_coord（+0x08 6 int）平铺拷贝到
        // m_5c[0..5]；ScanStr → m_str74；ScanInt → m_0；m_4 = x-1；m_8 = y-1。
        int wx = ScanInt((bool*)0);
        int wy = ScanInt((bool*)0);
        STWorldmapPattern pat;
        GetWorldMapCoordFromTable(wx, wy, pat);
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 2; ++j)
            {
                script->m_5c[i * 2 + j] = pat.m_coord.m_coord[i][j];
            }
        }
        ScanStr(&script->m_str74);
        script->m_0 = ScanInt((bool*)0);
        script->m_4 = wx - 1;
        script->m_8 = wy - 1;
        break;
    }
    }

case 0xff:  // "[monsterapc diff table]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[monsterapc diff table]", 0xff));
    }
    else
    {
        // ORIG 0x88e7538：块级临时 string ctor→ScanStr→operator= m_str10→dtor；
        // +0x17c 已由函数入口保存脚本完整路径，不能在此覆盖。
        {
            std::string diffName;
            ScanStr(&diffName);
            script->m_str10 = diffName;
        }
        break;
    }

case 0x109:  // "[minimum required level]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[minimum required level]", 0x109));
    }
    else
    {
        script->m_78 = ScanInt((bool*)0);  // +0x78
        break;
    }

case 0x10d:  // "[required item]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[required item]", 0x10d));
    }
    else
    {
        script->m_requiredItem7c.m_itemIdx = ScanInt((bool*)0);  // +0x7c
        script->m_requiredItem7c.m_itemCount = ScanInt((bool*)0);  // +0x80
        script->m_requiredItem7c.m_enableFlag = ScanInt(&ok) != 0;  // +0x84
        break;
    }

case 0x114:  // "[event dungeon]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[event dungeon]", 0x114));
    }
    else
    {
        script->m_88 = ScanInt((bool*)0) != 0;  // +0x88
        break;
    }

case 0x118:  // "[event required item]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[event required item]", 0x118));
    }
    else
    {
        script->m_requiredItem8c.m_itemIdx = ScanInt((bool*)0);  // +0x8c
        script->m_requiredItem8c.m_itemCount = ScanInt((bool*)0);  // +0x90
        script->m_requiredItem8c.m_enableFlag = ScanInt(&ok) != 0;  // +0x94
        break;
    }

case 0x11e:  // "[clear reward item]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[clear reward item]", 0x11e));
    }
    else
    {
        script->m_vec98.clear();  // vector<pair<int,int>> @+0x98
        while (true) {
        int v1 = ScanInt(&ok);
        if (!ok) break;
        int v2 = ScanInt(&ok);
        if (!ok) break;
        script->m_vec98.push_back(std::pair<int, int>(v1, v2));
        }
        break;
    }

case 0x18c:  // "[recommended level]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[recommended level]", 0x18c));
    }
    else
    {
        script->m_450 = ScanInt((bool*)0);  // +0x450
        script->m_454 = ScanInt((bool*)0);  // +0x454
        break;
    }

case 0x191:  // "[coin limit]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[coin limit]", 0x191));
    }
    else
    {
        script->m_a4 = ScanInt((bool*)0);  // +0xa4
        break;
    }

case 0x195:  // "[character coin limit]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[character coin limit]", 0x195));
    }
    else
    {
        script->m_a8 = ScanInt((bool*)0);  // +0xa8
        break;
    }

case 0x199:  // "[basis level]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[basis level]", 0x199));
    }
    else
    {
        script->m_ac = ScanInt((bool*)0);  // +0xac
        break;
    }

case 0x19d:  // "[experience increasing point]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[experience increasing point]", 0x19d));
    }
    else
    {
        script->m_b0 = ScanFloat(&ok);   // +0xb0（float；export 对称 %.3f）
        break;
    }

case 0x1a1:  // "[champion]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[champion]", 0x1a1));
    }
    else
    {
        // ORIG LAB_088e7b6f（Ghidra C）：循环 5×ScanInt → m_b4[i]（偏移 (i+0x2c)*4+4=+0xb4）。
        for (int ci = 0; ci < 5; ++ci)
        {
            script->m_b4[ci] = ScanInt((bool*)0);
        }
        break;
    }

case 0x1a6:  // "[item]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[item]", 0x1a6));
    }
    else
    {
        script->m_vecc8.clear();  // vector<pair<int,int>> @+0xc8
        while (true) {
        // ORIG pair<int,int>::pair(int,int)（080dd93a@088e7cdb）：v1 具名 +
        // 第二值 ScanInt(NULL)（忽略 ok），4.4.7 形参 const _U& 推导值类型。
        int v1 = ScanInt(&ok);
        if (!ok) break;
        script->m_vecc8.push_back(std::pair<int, int>(v1, ScanInt((bool*)0)));
        }
        break;
    }

case 0x1b3:  // "[realdungeon checkup]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[realdungeon checkup]", 0x1b3));
    }
    else
    {
    {
        // ORIG LAB_088e7d00（Ghidra C）：3 个 vector reserve(100)；循环 ScanStr 子标签，
        // [monster]→m_vec458、[passive]→m_vec464、[apc]→m_vec470：先 ScanInt 读数量，
        // 再循环 ScanInt push（单项失败退出子循环）；遇到 "[/realdungeon checkup]" 或 EOF 结束。
        script->m_vec458.reserve(100);
        script->m_vec464.reserve(100);
        script->m_vec470.reserve(100);
        std::string subtok;
        while (true)
        {
            if (!ScanStr(&subtok)) break;
            if (subtok == "[/realdungeon checkup]") break;
            if (subtok == "[monster]")
            {
                int n = ScanInt((bool*)0);
                for (int i = 0; i < n; ++i)
                {
                    int v = ScanInt(&ok);
                    if (!ok) break;
                    script->m_vec458.push_back(v);
                }
            }
            else if (subtok == "[passive]")
            {
                int n = ScanInt((bool*)0);
                for (int i = 0; i < n; ++i)
                {
                    int v = ScanInt(&ok);
                    if (!ok) break;
                    script->m_vec464.push_back(v);
                }
            }
            else if (subtok == "[apc]")
            {
                int n = ScanInt((bool*)0);
                for (int i = 0; i < n; ++i)
                {
                    int v = ScanInt(&ok);
                    if (!ok) break;
                    script->m_vec470.push_back(v);
                }
            }
        }
        break;
    }
    }


case 0x1e9:  // "[dungeon drop]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[dungeon drop]", 0x1e9));
    }
    else
    {
        script->m_vecd4.clear();  // vector<pair<int,int>> @+0xd4
        while (true) {
        int v1 = ScanInt(&ok);
        if (!ok) break;
        script->m_vecd4.push_back(std::pair<int, int>(v1, ScanInt((bool*)0)));
        }
        break;
    }

case 0x1f4:  // "[dungeon object drop]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[dungeon object drop]", 0x1f4));
    }
    else
    {
        script->m_vece0.clear();  // vector<pair<int,int>> @+0xe0
        while (true) {
        int v1 = ScanInt(&ok);
        if (!ok) break;
        script->m_vece0.push_back(std::pair<int, int>(v1, ScanInt((bool*)0)));
        }
        break;
    }

case 0x1ff:  // "[special passive object item]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[special passive object item]", 0x1ff));
    }
    else
    {
    {
        // ORIG LAB_088e81e4：清 m_vecec，循环读（itemIdx 消费 / assignItemIdx / pairCount + pair 列表）
        script->m_vecec.clear();
        while (true) {
            int v0 = ScanInt(&ok);       // itemIdx（消费）
            if (!ok) break;
            int assignItemIdx = ScanInt(&ok);
            if (!ok) return false;
            int pairCount = ScanInt(&ok);
            if (!ok) return false;
            stDungeonAssignItem_t item;
            item.m_assignItemIdx = assignItemIdx;
            for (int i = 0; i < pairCount; ++i) {
                int a = ScanInt(&ok);
                if (!ok) return false;
                int b = ScanInt(&ok);
                if (!ok) return false;
                item.m_list.push_back(std::pair<int, int>(a, b));
            }
            script->m_vecec.push_back(item);
        }
        break;
    }
    }

case 0x21d:  // "[pathgate object]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[pathgate object]", 0x21d));
    }
    else
    {
    {
        // ORIG LAB_088e8418：4 组 ScanInt 写入 +0xf8 起 POD 区共 10 个 int
        // （3+3+2+2：偏移 (0x3c+i)*4+8=+0xf8、+0x104、+0x110、+0x11c；与 export 对称）。
        for (int i = 0; i < 3; ++i) script->m_pathgate.m_cells[0][i] = ScanInt((bool*)0);
        for (int i = 0; i < 3; ++i) script->m_pathgate.m_cells[1][i] = ScanInt((bool*)0);
        for (int i = 0; i < 2; ++i) script->m_pathgate.m_cells[2][i] = ScanInt((bool*)0);
        for (int i = 0; i < 2; ++i) script->m_pathgate.m_cells[3][i] = ScanInt((bool*)0);
        break;
    }
    }

case 0x228:  // "[maze info]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[maze info]", 0x228));
    }
    else
    {
        // ORIG LAB_088e8587：首轮置 -0x121 标志；后续轮先取 m_mazeList170.size()
        // （结果仅落槽不使用，ORIG 088e85fb 实证）再 push_back + mazeLocal.clear()。
        if (mazeInited) {
            script->m_mazeList170.size();
            script->m_mazeList170.push_back(mazeLocal);
            mazeLocal.clear();
        } else {
            mazeInited = true;
        }
        break;
    }

case 0x27d:  // "[quest connection]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[quest connection]", 0x27d));
    }
    else
    {
    {
        // ORIG LAB_088e8643：questType ∈ {-1,0,1}；type==0 时再读 1 int（quest index）。
        // 越界 → logFormat("error! maze quest type(%d)") 后 import 失败。
        int questType = ScanInt((bool*)0);
        ScanInt((bool*)0);   // 第 2 值（未用）
        if (questType >= -1 && questType <= 1)
        {
            if (questType == 0)
            {
                ScanInt((bool*)0);
            }
        }
        else
        {
            LogManager::logFormat(0, "../RDARScriptDungeon.cpp",
                                  __FUNCTION__, 0x282,
                                  "error! maze quest type(%d)", questType);
            return false;
        }
        break;
    }
    }

case 0x28a:  // "[clear condition]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[clear condition]", 0x28a));
    }
    else
    {
    {
        // ORIG LAB_088e8742：ScanTypeOrStr 读取条件标签；未知标签、左/右变量
        // 缺失均记录 RDARScriptDungeon.cpp 日志并使整个导入失败。
        DungeonClearCondition cond;
        bool isType = false;
        while (true) {
            std::string sub;
            if (!ScanTypeOrStr(isType, sub, true)) break;
            if (sub == "[/clear condition]") break;
            int type = -1;
            if (sub == "[destroy object]") type = 0;
            else if (sub == "[clear map]") type = 1;
            else if (sub == "[hunt monster]") type = 2;
            else if (sub == "[hunt apc]") type = 3;
            else if (sub == "[hunt boss]") type = 4;
            else
            {
                LogManager::logFormat(0, "../RDARScriptDungeon.cpp",
                                      __FUNCTION__, 0x2a1,
                                      "error! dungeon clear condition(%s)",
                                      sub.c_str());
                return false;
            }
            cond.m_0 = type;
            cond.m_4 = ScanInt(&ok);
            if (!ok)
            {
                LogManager::logFormat(0, "../RDARScriptDungeon.cpp",
                                      __FUNCTION__, 0x2a8,
                                      "error! dungeon clear condition left variable");
                return false;
            }
            cond.m_8 = ScanInt(&ok);
            if (!ok)
            {
                LogManager::logFormat(0, "../RDARScriptDungeon.cpp",
                                      __FUNCTION__, 0x2ae,
                                      "error! dungeon clear condition right variable");
                return false;
            }
            if (type == 1) cond.m_8 = 1;   // ORIG：clear map → 右变量恒 1
            mazeLocal.m_clearCondition.push_back(cond);
        }
        break;
    }
    }

case 0x2be:  // "[party num check]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[party num check]", 0x2be));
    }
    else
    {
        ScanInt((bool*)0);  // 局部用途 @88e8aac
        ScanInt((bool*)0);  // 局部用途 @88e8abe
        break;
    }

case 0x2c4:  // "[escape hell]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[escape hell]", 0x2c4));
    }
    else
    {
        ScanInt((bool*)0);  // 局部用途 @88e8b32
        break;
    }

case 0x2c8:  // "[size]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[size]", 0x2c8));
    }
    else
    {
        // [party num check]：maze 尺寸 → grids.resize
        v1 = ScanInt((bool*)0);
        v2 = ScanInt((bool*)0);
        mazeLocal.m_width = v1;
        mazeLocal.m_height = v2;
        mazeLocal.m_grids.resize(mazeLocal.m_width * mazeLocal.m_height, GridScript());
        break;
    }

case 0x2d0:  // "[greed]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[greed]", 0x2d0));
    }
    else
    {
    {
        // ORIG LAB_088e8c37：每轮 substr(pos,2) 临时串 + 多次 .c_str() 重读比较
        // 分隔符/符号表（O0 无 CSE，P48）；未命中符号 pos+=1 并 bad++；命中直接
        // 写 m_grids[gridIdx]（ORIG 无越界检查）。bad 非零时导入失败。
        // ORIG ScanStr 复用函数级路径串槽（-0x1098，无独立 ctor/dtor）→ 用 token。
        ScanStr(&token);
        int len = (int)token.size();
        int pos = 0;
        int gridIdx = 0;
        int bad = 0;
        while (pos < len - 1)
        {
            std::string two = token.substr(pos, 2);
            if (two.c_str()[0] == '\r' || two.c_str()[0] == '\n' ||
                two.c_str()[0] == ' ')
            {
                pos += 1;
            }
            else
            {
                int cc = 0;
                for (; cc < 0x10; ++cc)
                {
                    if (two.c_str()[0] == g_gridSymbols[cc][0] &&
                        two.c_str()[1] == g_gridSymbols[cc][1])
                        break;
                }
                if (cc == 0x10)
                {
                    pos += 1;
                    bad += 1;
                }
                else
                {
                    mazeLocal.m_grids[gridIdx].m_gridValue = cc;
                    gridIdx += 1;
                    pos += 2;
                }
            }
        }
        if (bad != 0)
            return false;
        break;
    }
    }

case 0x30e:  // "[map specification]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[map specification]", 0x30e));
    }
    else
    {
        // [map specification]：填充 maze grid
        v1 = ScanInt((bool*)0);  // x
        v2 = ScanInt((bool*)0);  // y
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
    }

case 0x31e:  // "[boss map specification]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[boss map specification]", 0x31e));
    }
    else
    {
        // [boss map specification]：同 [map specification]
        v1 = ScanInt((bool*)0);
        v2 = ScanInt((bool*)0);
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
    }

case 0x32e:  // "[layered map specification]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[layered map specification]", 0x32e));
    }
    else
    {
        // [layered map specification]：同 [map specification]
        v1 = ScanInt((bool*)0);
        v2 = ScanInt((bool*)0);
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
    }

case 0x33e:  // "[start map]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[start map]", 0x33e));
    }
    else
    {
        // [start map]：读 (x,y) 坐标对入 mazeLocal.m_vec14（ORIG LAB_088e919e）
        mazeLocal.m_vec14.clear();
        while (true) {
            int mx = ScanInt(&ok);
            if (!ok) break;
            mazeLocal.m_vec14.push_back(std::pair<int, int>(mx, ScanInt((bool*)0)));
        }
        break;
    }

case 0x34a:  // "[boss map]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[boss map]", 0x34a));
    }
    else
    {
        // [boss map]：读 (x,y) 坐标对入 mazeLocal.m_vec20（ORIG LAB_088e9288）
        mazeLocal.m_vec20.clear();
        while (true) {
            int mx = ScanInt(&ok);
            if (!ok) break;
            mazeLocal.m_vec20.push_back(std::pair<int, int>(mx, ScanInt((bool*)0)));
        }
        break;
    }

case 0x355:  // "[hell dungeon]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[hell dungeon]", 0x355));
    }
    else
    {
        script->m_140 = ScanInt((bool*)0) != 0;  // +0x140
        break;
    }

case 0x35b:  // "[prohibit practice]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[prohibit practice]", 0x35b));
    }
    else
    {
        script->m_141 = ScanInt((bool*)0) != 0;  // +0x141
        break;
    }

case 0x362:  // "[disable skill]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[disable skill]", 0x362));
    }
    else
    {
        // ORIG LAB_088e947c：ScanInt → m_142 = (值 == 0)（disable skill 段标记）
        script->m_142 = ScanInt((bool*)0) == 0;  // +0x142
        break;
    }

case 0x367:  // "[impossible skills]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[impossible skills]", 0x367));
    }
    else
    {
    {
        // ORIG LAB_088e94fb：循环 ScanStr job 名 → getCharacterJobToEnumValue + ScanInt → m_vec144
        while (true) {
            std::string token;
            if (!ScanStr(&token)) break;
            if (token == "[/impossible skills]") break;
            int job = getCharacterJobToEnumValue(token);
            int val = ScanInt((bool*)0);
            script->m_vec144.push_back(std::make_pair(job, val));
        }
        break;
    }
    }

case 0x373:  // "[/impossible skills]"（ORIG 88e9648：__inited==1 时直接落到 default，
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[/impossible skills]", 0x373));
    }
    else
    {
        // 无字段操作——结束 token 由 [impossible skills] 的 while 内部自行消费）
        break;
    }

case 0x377:  // "[use countdown]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[use countdown]", 0x377));
    }
    else
    {
        // ORIG LAB_088e969b：仅当值等于 1 时置位 m_150。
        script->m_150 = ScanInt((bool*)0) == 1;
        break;
    }

case 0x37c:  // "[use zoom rate]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[use zoom rate]", 0x37c));
    }
    else
    {
        script->m_154 = ScanFloat(&ok);
        break;
    }

case 0x381:  // "[room move keep state]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[room move keep state]", 0x381));
    }
    else
    {
        script->m_158 = ScanInt((bool*)0) == 1;  // cmp $1,sete
        break;
    }

case 0x386:  // "[usable item]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[usable item]", 0x386));
    }
    else
    {
        script->m_159 = ScanInt((bool*)0) == 1;  // cmp $1,sete
        break;
    }

case 0x38a:  // "[seal door map index]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[seal door map index]", 0x38a));
    }
    else
    {
        ScanInt((bool*)0);  // 丢弃 @88e98f9
        break;
    }

case 0x38d:  // "[seal door pos]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[seal door pos]", 0x38d));
    }
    else
    {
        // [usable item]：两值
        ScanInt((bool*)0);
        ScanInt((bool*)0);
        break;
    }

case 0x392:  // "[hell coin limit]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[hell coin limit]", 0x392));
    }
    else
    {
        script->m_168 = ScanInt((bool*)0);  // +0x168
        break;
    }

case 0x397:  // "[ai character appear rate]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[ai character appear rate]", 0x397));
    }
    else
    {
        script->m_338 = ScanInt((bool*)0);  // +0x338
        break;
    }

case 0x39b:  // "[dead tower map index]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[dead tower map index]", 0x39b));
    }
    else
    {
        // [hell coin limit]：push_back 到 m_deadTower180.m_vec
        while (true) {
            v1 = ScanInt(&ok);
            if (!ok) break;
            script->m_deadTower180.m_vec.push_back(v1);
        }
        break;
    }

case 0x3a5:  // "[randomized object creation]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[randomized object creation]", 0x3a5));
    }
    else
    {
        std::string rsub;
        RidableObject ridable;
        int t0 = 0;
        while (true)
        {
            // ORIG 088e9bb5：ScanType 失败 → import 返回 false（非静默块结束）。
            if (!ScanType(rsub, true))
                return false;
            if (rsub == "[/randomized object creation]")
                break;
        retry_0_head:
            static DungeonTagMap __lookup_0;
            static bool __inited_0 = false;
            static SpinLock __init_lock_0;
        retry_0:
            if (__inited_0)
            {
                DungeonTagMap::const_iterator it0 = __lookup_0.find(rsub.c_str());
                t0 = (it0 != __lookup_0.end()) ? it0->second : -1;
            }
            else
            {
                __init_lock_0.enter();
                if (__inited_0)
                {
                    __init_lock_0.leave();
                    goto retry_0_head;
                }
                t0 = 0;   // ORIG：首过 subId=0 进子表 0 填充链
            }
            switch (t0)
            {
            case 0:
            case 0x3b3:  // "[select]" → mazeLocal.m_ridable.m_0
                if (!__inited_0)
                {
                    __lookup_0.insert(DungeonTagMap::value_type("[select]", 0x3b3));
                }
                else
                {
                    if (!ScanInt(&iv0)) return false;
                    mazeLocal.m_ridable.m_0 = iv0;
                    break;
                }
            case 0x3b8:  // "[regenerate]" → mazeLocal.m_ridable.m_4
                if (!__inited_0)
                {
                    __lookup_0.insert(DungeonTagMap::value_type("[regenerate]", 0x3b8));
                }
                else
                {
                    if (!ScanInt(&iv0)) return false;
                    mazeLocal.m_ridable.m_4 = (iv0 != 0);
                    break;
                }
            case 0x3bd:  // "[minimap icon]" → script+0x500
                if (!__inited_0)
                {
                    __lookup_0.insert(DungeonTagMap::value_type("[minimap icon]", 0x3bd));
                }
                else
                {
                    if (!ScanInt(&iv0)) return false;
                    script->m_500 = iv0;
                    break;
                }
            case 0x3c2:  // "[object]" → 内层 __lookup_1 表分派收集 RidableObject
                if (!__inited_0)
                {
                    __lookup_0.insert(DungeonTagMap::value_type("[object]", 0x3c2));
                }
                else
                {
                    std::string ss;
                    int t1 = 0;
                    ridable.clear();   // ORIG 088ea072：每轮 [object] 前 clear
                    while (true)
                    {
                        // ORIG 088ea083：ScanType 失败 → import false。
                        if (!ScanType(ss, true))
                            return false;
                        if (ss == "[/object]")
                        {
                            mazeLocal.m_ridable.m_objects.push_back(ridable);
                            break;
                        }
                    retry_1_head:
                        static DungeonTagMap __lookup_1;
                        static bool __inited_1 = false;
                        static SpinLock __init_lock_1;
                    retry_1:
                        if (__inited_1)
                        {
                            DungeonTagMap::const_iterator it1 = __lookup_1.find(ss.c_str());
                            t1 = (it1 != __lookup_1.end()) ? it1->second : -1;
                        }
                        else
                        {
                            __init_lock_1.enter();
                            if (__inited_1)
                            {
                                __init_lock_1.leave();
                                goto retry_1_head;
                            }
                            t1 = 0;
                        }
                        switch (t1)
                        {
                        case 0:
                        case 0x3d5:  // "[map]" → m_x/m_y
                            if (!__inited_1)
                            {
                                __lookup_1.insert(DungeonTagMap::value_type("[map]", 0x3d5));
                            }
                            else
                            {
                                if (!ScanInt(&ridable.m_x)) return false;
                                if (!ScanInt(&ridable.m_y)) return false;
                                break;
                            }
                        case 0x3de:  // "[index]" → m_field0c
                            if (!__inited_1)
                            {
                                __lookup_1.insert(DungeonTagMap::value_type("[index]", 0x3de));
                            }
                            else
                            {
                                if (!ScanInt(&ridable.m_field0c)) return false;
                                break;
                            }
                        case 0x3e3:  // "[team]" → m_field18；未识别串静默越过
                            if (!__inited_1)
                            {
                                __lookup_1.insert(DungeonTagMap::value_type("[team]", 0x3e3));
                            }
                            else
                            {
                                std::string team;
                                if (!ScanType(team, true)) return false;
                                if (team == "[neutral]") ridable.m_field18 = 200;
                                else if (team == "[monster]") ridable.m_field18 = 100;
                                else if (team == "[character]") ridable.m_field18 = 0;
                                break;
                            }
                        case 0x3ee:  // "[pos]" → m_field10/m_field14
                            if (!__inited_1)
                            {
                                __lookup_1.insert(DungeonTagMap::value_type("[pos]", 0x3ee));
                            }
                            else
                            {
                                if (!ScanInt(&ridable.m_field10)) return false;
                                if (!ScanInt(&ridable.m_field14)) return false;
                                break;
                            }
                        default:
                            return false;   // ORIG LAB_088ea62c：未知子 tag → 失败
                        }
                        if (!__inited_1)
                        {
                            __init_lock_1.leave();
                            __inited_1 = true;
                            goto retry_1_head;   // ORIG LAB_088ea665
                        }
                    }
                    break;
                }
            default:
                return false;   // ORIG LAB_088ea6e0：未知子 tag → 失败
            }
            if (!__inited_0)
            {
                __init_lock_0.leave();
                __inited_0 = true;
                goto retry_0_head;   // ORIG LAB_088ea711
            }
        }
    }

case 0x421:  // "[point by type]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[point by type]", 0x421));
    }
    else
    {
        // ORIG LAB_088ea761（Ghidra：local_12c+(i+0x60)*4+0xc）：i=0..2 循环，
        // ScanInt((bool*)0) 结果按 stride 0x180 写 script+0x18c/0x30c/0x48c 三处
        // int——原码跨成员指针算术（非 warRoom 内连续 m_0/4/8），P171 照抄。
        // §4.3 例外：ORIG 遗留线形算术，无对应成员数组可具名。
        for (int i = 0; i <= 2; ++i)
            *(int*)((char*)script + 0x18c + i * 0x180) = ScanInt((bool*)0);
        break;
    }

case 0x426:  // "[phase fatigue]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[phase fatigue]", 0x426));
    }
    else
    {
        script->m_warRoom18c.m_c = ScanInt((bool*)0);  // +0x198
        break;
    }

case 0x42c:  // "[warroom map index]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[warroom map index]", 0x42c));
    }
    else
    {
        script->m_warRoom18c.m_10 = ScanInt((bool*)0);  // +0x19c
        break;
    }

case 0x430:  // "[monster exp bonus per user decrease]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[monster exp bonus per user decrease]", 0x430));
    }
    else
    {
        script->m_warRoom18c.m_bc = ScanFloat(&ok);
        break;
    }

case 0x433:  // "[result exp bonus per user decrease]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[result exp bonus per user decrease]", 0x433));
    }
    else
    {
        script->m_warRoom18c.m_c0 = ScanFloat(&ok);
        break;
    }

case 0x436:  // "[monster type spawn prob]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[monster type spawn prob]", 0x436));
    }
    else
    {
    {
        // ORIG LAB_088ea9eb：逐条读 4 int（怪物类型 + 3 概率）push 入 m_warRoom18c.m_spawnProb
        while (true) {
            STMonsterTypeSpawnProb sp;
            sp.m_probability[0] = ScanInt(&ok);
            if (!ok) break;
            sp.m_probability[1] = ScanInt(&ok);
            if (!ok) return false;
            sp.m_probability[2] = ScanInt(&ok);
            if (!ok) return false;
            sp.m_probability[3] = ScanInt(&ok);
            if (!ok) return false;
            script->m_warRoom18c.m_spawnProb.push_back(sp);
        }
        break;
    }
    }

case 0x456:  // "[monster type spawn cost]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[monster type spawn cost]", 0x456));
    }
    else
    {
        for (int i = 0; i < 4; ++i)
            script->m_warRoom18c.m_c4[i] = ScanInt((bool*)0);
        break;
    }

case 0x45b:  // "[monster type spawn interval rate]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[monster type spawn interval rate]", 0x45b));
    }
    else
    {
        for (int i = 0; i < 4; ++i)
            script->m_warRoom18c.m_dc[i] = ScanFloat(&ok);
        break;
    }

case 0x45f:  // "[monster spawn base interval]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[monster spawn base interval]", 0x45f));
    }
    else
    {
        script->m_warRoom18c.m_ec = ScanInt((bool*)0);  // +0x278
        break;
    }

case 0x462:  // "[monster spawn random interval]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[monster spawn random interval]", 0x462));
    }
    else
    {
        script->m_warRoom18c.m_f0 = ScanInt((bool*)0);  // +0x27c
        break;
    }

case 0x465:  // "[fatigue]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[fatigue]", 0x465));
    }
    else
    {
        script->m_warRoom18c.m_f4 = ScanInt((bool*)0);  // +0x280
        break;
    }

case 0x468:  // "[fatigue result]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[fatigue result]", 0x468));
    }
    else
    {
        script->m_warRoom18c.m_f8 = ScanInt((bool*)0);  // +0x284
        break;
    }

case 0x46b:  // "[max monster]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[max monster]", 0x46b));
    }
    else
    {
        script->m_warRoom18c.m_d8 = ScanInt((bool*)0);  // +0x264
        break;
    }

case 0x46e:  // "[player kc]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[player kc]", 0x46e));
    }
    else
    {
        script->m_warRoom18c.m_d4 = ScanInt((bool*)0);  // +0x260
        break;
    }

case 0x472:  // "[spawn common monster index]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[spawn common monster index]", 0x472));
    }
    else
    {
        // ORIG LAB_088eafab：clear(+0x1a0) 后循环 push（写 STWarRoomData+0x14，
        // 不是 item-drop vector +0x6c）。
        script->m_warRoom18c.m_vec14[0].clear();
        while (true) {
            v1 = ScanInt(&ok);
            if (!ok) break;
            // ORIG 0x088eafbe：写 STWarRoomData+0x14（不是 item-drop
            // vector +0x6c）。
            script->m_warRoom18c.m_vec14[0].push_back(v1);
        }
        break;
    }

case 0x47f:  // "[spawn common champion index]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[spawn common champion index]", 0x47f));
    }
    else
    {
        script->m_warRoom18c.m_vec14[1].clear();   // ORIG clear(+0x1ac)
        while (true) {
            v1 = ScanInt(&ok);
            if (!ok) break;
            script->m_warRoom18c.m_vec14[1].push_back(v1);
        }
        break;
    }

case 0x48c:  // "[spawn super champion index]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[spawn super champion index]", 0x48c));
    }
    else
    {
        script->m_warRoom18c.m_vec14[2].clear();   // ORIG clear(+0x1b8)
        while (true) {
            v1 = ScanInt(&ok);
            if (!ok) break;
            script->m_warRoom18c.m_vec14[2].push_back(v1);
        }
        break;
    }

case 0x499:  // "[spawn boss index]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[spawn boss index]", 0x499));
    }
    else
    {
        script->m_warRoom18c.m_vec14[3].clear();   // ORIG clear(+0x1c4)
        while (true) {
            v1 = ScanInt(&ok);
            if (!ok) break;
            script->m_warRoom18c.m_vec14[3].push_back(v1);
        }
        break;
    }

case 0x4a6:  // "[spawn step max]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[spawn step max]", 0x4a6));
    }
    else
    {
        script->m_warRoom18c.m_50 = ScanInt((bool*)0);  // +0x1dc
        break;
    }

case 0x4ab:  // "[spawn step resource pool]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[spawn step resource pool]", 0x4ab));
    }
    else
    {
        // ORIG LAB_088eb2a8（Ghidra C 实证）：clear(+0x1d0) 后循环 ScanInt push；
        // +0x1d0 = STWarRoomData+0x44 = m_vec44。
        script->m_warRoom18c.m_vec44.clear();
        while (true) {
            int poolVal = ScanInt(&ok);
            if (!ok) break;
            script->m_warRoom18c.m_vec44.push_back(poolVal);
        }
        break;
    }

case 0x4b9:  // "[kill count const]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[kill count const]", 0x4b9));
    }
    else
    {
        script->m_warRoom18c.m_ac = ScanFloat(&ok);
        break;
    }

case 0x4be:  // "[join cost gold]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[join cost gold]", 0x4be));
    }
    else
    {
        script->m_warRoom18c.m_54 = ScanInt((bool*)0);  // +0x1e0
        break;
    }

case 0x4c3:  // "[gold drop prob]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[gold drop prob]", 0x4c3));
    }
    else
    {
        script->m_warRoom18c.m_58 = ScanInt((bool*)0);  // +0x1e4
        break;
    }

case 0x4c8:  // "[common monster item drop prob]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[common monster item drop prob]", 0x4c8));
    }
    else
    {
        script->m_warRoom18c.m_5c[0] = ScanInt((bool*)0);  // +0x1e8
        break;
    }

case 0x4cd:  // "[common champion item drop prob]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[common champion item drop prob]", 0x4cd));
    }
    else
    {
        script->m_warRoom18c.m_5c[1] = ScanInt((bool*)0);  // +0x1ec
        break;
    }

case 0x4d2:  // "[super champion item drop prob]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[super champion item drop prob]", 0x4d2));
    }
    else
    {
        script->m_warRoom18c.m_5c[2] = ScanInt((bool*)0);  // +0x1f0
        break;
    }

case 0x4d7:  // "[boss item drop prob]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[boss item drop prob]", 0x4d7));
    }
    else
    {
        script->m_warRoom18c.m_5c[3] = ScanInt((bool*)0);  // +0x1f4
        break;
    }

case 0x4dc:  // "[common monster exp const]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[common monster exp const]", 0x4dc));
    }
    else
    {
        script->m_warRoom18c.m_9c[0] = ScanInt((bool*)0);  // +0x228
        break;
    }

case 0x4e4:  // "[common champion exp const]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[common champion exp const]", 0x4e4));
    }
    else
    {
        script->m_warRoom18c.m_9c[1] = ScanInt((bool*)0);  // +0x22c
        break;
    }

case 0x4ec:  // "[super champion exp const]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[super champion exp const]", 0x4ec));
    }
    else
    {
        script->m_warRoom18c.m_9c[2] = ScanInt((bool*)0);  // +0x230
        break;
    }

case 0x4f4:  // "[boss exp const]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[boss exp const]", 0x4f4));
    }
    else
    {
        script->m_warRoom18c.m_9c[3] = ScanInt((bool*)0);  // +0x234
        break;
    }

case 0x4fc:  // "[common monster item drop list]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[common monster item drop list]", 0x4fc));
    }
    else
    {
        script->m_warRoom18c.m_vec6c[0].clear();   // ORIG clear(+0x1f8)
        while (true) {
            v1 = ScanInt(&ok);
            if (!ok) break;
            script->m_warRoom18c.m_vec6c[0].push_back(v1);
        }
        break;
    }

case 0x508:  // "[common champion item drop list]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[common champion item drop list]", 0x508));
    }
    else
    {
        script->m_warRoom18c.m_vec6c[1].clear();   // ORIG clear(+0x204)
        while (true) {
            v1 = ScanInt(&ok);
            if (!ok) break;
            script->m_warRoom18c.m_vec6c[1].push_back(v1);
        }
        break;
    }

case 0x514:  // "[super champion item drop list]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[super champion item drop list]", 0x514));
    }
    else
    {
        // ORIG 088eba6b：clear 的是 +0x204（前一 vector），push 目标 +0x210 —— 忠实保留。
        script->m_warRoom18c.m_vec6c[1].clear();
        while (true) {
            v1 = ScanInt(&ok);
            if (!ok) break;
            script->m_warRoom18c.m_vec6c[2].push_back(v1);
        }
        break;
    }

case 0x520:  // "[boss item drop list]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[boss item drop list]", 0x520));
    }
    else
    {
        script->m_warRoom18c.m_vec6c[3].clear();   // ORIG clear(+0x21c)
        while (true) {
            v1 = ScanInt(&ok);
            if (!ok) break;
            script->m_warRoom18c.m_vec6c[3].push_back(v1);
        }
        break;
    }

case 0x52c:  // "[battle spawn time]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[battle spawn time]", 0x52c));
    }
    else
    {
        script->m_warRoom18c.m_fc = ScanInt((bool*)0);  // +0x288
        break;
    }

case 0x534:  // "[ancient dungeon]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[ancient dungeon]", 0x534));
    }
    else
    {
        script->m_28c = ScanInt((bool*)0) != 0;  // +0x28c
        break;
    }

case 0x539:  // "[powerwar dungeon]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[powerwar dungeon]", 0x539));
    }
    else
    {
        script->m_28d = ScanInt((bool*)0) != 0;  // +0x28d
        break;
    }

case 0x53d:  // "[blood dungeon]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[blood dungeon]", 0x53d));
    }
    else
    {
        script->m_28e = (char)ScanInt((bool*)0);  // mov %dl,0x28e
        break;
    }

case 0x549:  // "[limit item stat]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[limit item stat]", 0x549));
    }
    else
    {
        script->m_28f = ScanInt((bool*)0) != 0;  // +0x28f
        break;
    }

case 0x54f:  // "[absolute change stat]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[absolute change stat]", 0x54f));
    }
    else
    {
        // ORIG LAB_088ebe52：每轮 statKind=5 → ScanStr 名 → __lookup_2 表惰性链
        // （[move speed]/[physical attack add]/[physical defense add]/
        //   [magical attack add]/[magical defense add] = 0x559..0x565 → 0..4，
        //   子表条目在 088ec0bb..088ec2c5 的 per-case !__inited 分支中逐例 insert）；
        // 未知名保持 5。收尾 ScanFloat(float*)（088bc58e）忽略返回值，statKind!=5
        // 时以本地左值 key/val 构造 value_type 写 m_statMap290（重复 key 不覆盖）。
        int statKind = 5;
        int t2 = 0;
        while (true)
        {
            statKind = 5;   // ORIG 088ebe52：每轮初值 5（不写 map）
            // ORIG 088ebe62：复用函数级串槽（-0x1098），无独立 string ctor/dtor。
            if (!ScanStr(&token))
                break;
        retry_2_head:
            static DungeonTagMap __lookup_2;
            static bool __inited_2 = false;
            static SpinLock __init_lock_2;
        retry_2:
            if (__inited_2)
            {
                DungeonTagMap::const_iterator it2 = __lookup_2.find(token.c_str());
                t2 = (it2 != __lookup_2.end()) ? it2->second : -1;
            }
            else
            {
                __init_lock_2.enter();
                if (__inited_2)
                {
                    __init_lock_2.leave();
                    goto retry_2_head;
                }
                t2 = 0;   // ORIG：首过 subId=0 并入 [move speed] case
            }
            switch (t2)
            {
            case 0:
            case 0x559:  // "[move speed]"
                if (!__inited_2)
                {
                    __lookup_2.insert(DungeonTagMap::value_type("[move speed]", 0x559));
                }
                else
                {
                    statKind = 0;
                    break;
                }
            case 0x55c:  // "[physical attack add]"
                if (!__inited_2)
                {
                    __lookup_2.insert(DungeonTagMap::value_type("[physical attack add]", 0x55c));
                }
                else
                {
                    statKind = 1;
                    break;
                }
            case 0x55f:  // "[physical defense add]"
                if (!__inited_2)
                {
                    __lookup_2.insert(DungeonTagMap::value_type("[physical defense add]", 0x55f));
                }
                else
                {
                    statKind = 2;
                    break;
                }
            case 0x562:  // "[magical attack add]"
                if (!__inited_2)
                {
                    __lookup_2.insert(DungeonTagMap::value_type("[magical attack add]", 0x562));
                }
                else
                {
                    statKind = 3;
                    break;
                }
            case 0x565:  // "[magical defense add]"
                if (!__inited_2)
                {
                    __lookup_2.insert(DungeonTagMap::value_type("[magical defense add]", 0x565));
                }
                else
                {
                    statKind = 4;
                    break;
                }
            default:
                break;
            }
            if (!__inited_2)
            {
                __init_lock_2.leave();
                __inited_2 = true;
                goto retry_2_head;   // ORIG LAB_088ec2e9
            }
            float v = 0.0f;
            ScanFloat(&v);   // ORIG 088ec301：调用后忽略返回值
            if (statKind == 5)
                continue;    // ORIG 088ec36e：未知名只消费浮点值
            AbsoluteChangeStat::T key = static_cast<AbsoluteChangeStat::T>(statKind);
            script->m_statMap290.insert(
                std::map<AbsoluteChangeStat::T, float>::value_type(key, v));
        }
    }


case 0x576:  // "[limit escape character]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[limit escape character]", 0x576));
    }
    else
    {
        script->m_2a8 = ScanInt((bool*)0) != 0;  // +0x2a8
        break;
    }

case 0x57d:  // "[defense dungeon]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[defense dungeon]", 0x57d));
    }
    else
    {
        script->m_2ab = ScanInt((bool*)0) != 0;  // +0x2ab
        break;
    }

case 0x580:  // "[advance altar type]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[advance altar type]", 0x580));
    }
    else
    {
        // ORIG 088ec4d7：独立 string 槽（-0x1144），失败按 ebx=1 出口返回 true。
    {
        std::string typeName;
        if (!ScanStr(&typeName))
            return true;  // ORIG 0x088ec4ea 失败时直接以 true 结束，不写字段
        advancealtar::StageType::T st;
        if (!advancealtar::StageType::getStageType(typeName, st))
            return true;
        script->m_2ac = st;
    }
        break;
    }

case 0x58e:  // "[advance altar map]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[advance altar map]", 0x58e));
    }
    else
    {
    {
        // ORIG LAB_088ec56a：循环 ScanStr 难度名 → getDifficulty（easy/medium/hard），
        // 命中则读 2 int → m_2b0[diff]/m_2bc[diff]；任意失败/不可识别 token 结束。
        while (true)
        {
            std::string dtok;
            if (!ScanStr(&dtok)) break;
            advancealtar::StageDifficulty::T sd;
            if (!advancealtar::StageDifficulty::getDifficulty(dtok, sd)) break;
            int a = ScanInt(&ok);
            script->m_2b0[(int)sd] = a;
            if (!ok) break;
            int b = ScanInt(&ok);
            script->m_2bc[(int)sd] = b;
            if (!ok) break;
        }
        break;
    }
    }

case 0x5a2:  // "[advance altar clear reward]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[advance altar clear reward]", 0x5a2));
    }
    else
    {
    {
        // ORIG LAB_088ec6dc（同 ImportAdvanceAltarDungeonTag）：ScanType 子 tag 选
        // [easy]/[medium]/[hard]→vec 槽 0..2，然后逐项 importAltarRewardItem push；
        // "[/advance altar clear reward]" 显式比对后结束（ORIG 088ec803 eq 实证），
        // 其余子 tag 同样结束本 case。
        while (true)
        {
            std::string dsub;
            if (!ScanType(dsub, true)) break;
            int idx = -1;
            if (dsub == "[easy]") idx = 0;
            else if (dsub == "[medium]") idx = 1;
            else if (dsub == "[hard]") idx = 2;
            else if (dsub == "[/advance altar clear reward]") break;
            else break;
            std::vector<advancealtar::_RewardItem>& vec =
                script->m_clearReward2d4.m_vec[idx];
            while (true)
            {
                advancealtar::_RewardItem item;
                std::string rewardType;
                if (!ScanStr(&rewardType)) break;
                advancealtar::RewardItemType::T rewardItemType;
                if (!advancealtar::RewardItemType::getRewardItemType(
                        rewardType, rewardItemType))
                    break;
                item.m_type = static_cast<int>(rewardItemType);
                item.m_value1 = ScanInt(&ok);
                if (!ok) break;
                item.m_value2 = ScanInt(&ok);
                if (!ok) break;
                vec.push_back(item);
            }
        }
        break;
    }
    }

case 0x5d9:  // "[advance altar survival map]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[advance altar survival map]", 0x5d9));
    }
    else
    {
    {
        // ORIG LAB_088ec99f：循环 ScanInt → push m_vec2c8 直到失败
        while (true)
        {
            int mapId = ScanInt(&ok);
            if (!ok) break;
            script->m_vec2c8.push_back(mapId);
        }
        break;
    }
    }

case 0x5e5:  // "[advance altar survival clear reward]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[advance altar survival clear reward]", 0x5e5));
    }
    else
    {
    {
        // ORIG（Ghidra C，同 ImportAdvanceAltarDungeonTag 语义）：多个 [type]…[/type] 块 →
        // SurvivalRewardData::m_vec；[round] 两项、[list] 逐项 importAltarRewardItem（type 0/1）。
        while (true)
        {
            std::string token;
            if (!ScanType(token, true)) break;
            if (token != "[type]") break;
            advancealtar::RoundRewardData rrd;
            rrd.m_0 = 0;
            rrd.m_4 = 0;
            while (true)
            {
                std::string sub;
                if (!ScanType(sub, true)) break;
                if (sub == "[/type]")
                {
                    script->m_survival2f8.m_vec.push_back(rrd);
                    break;
                }
                bool ok = false;
                if (sub == "[round]")
                {
                    rrd.m_0 = ScanInt(&ok);
                    if (!ok) break;
                    rrd.m_4 = ScanInt(&ok);
                    if (!ok) break;
                }
                else if (sub == "[list]")
                {
                    while (true)
                    {
                        advancealtar::_RewardItem item;
                        std::string rewardType;
                        if (!ScanStr(&rewardType)) break;
                        advancealtar::RewardItemType::T rewardItemType;
                        if (!advancealtar::RewardItemType::getRewardItemType(
                                rewardType, rewardItemType))
                            break;
                        item.m_type = static_cast<int>(rewardItemType);
                        bool itemOk = false;
                        item.m_value1 = ScanInt(&itemOk);
                        if (!itemOk) break;
                        item.m_value2 = ScanInt(&itemOk);
                        if (!itemOk) break;
                        // ORIG 088ecd14/088ecd36：type==0 与 ==1 分别为独立 push_back
                        // 调用点（两条实例化调用），非合并条件。
                        if (item.m_type == 0)
                        {
                            rrd.m_vec8.push_back(item);
                        }
                        else if (item.m_type == 1)
                        {
                            rrd.m_vec8.push_back(item);
                        }
                    }
                }
            }
        }
        break;
    }
    }

case 0x622:  // "[dimension dungeon]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[dimension dungeon]", 0x622));
    }
    else
    {
        // ORIG LAB_088ece04：2×ScanInt → m_304/m_305（byte）
        script->m_304 = (char)ScanInt((bool*)0);
        script->m_305 = (char)ScanInt((bool*)0);
        break;
    }
case 0x62c:  // "[necessary party]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[necessary party]", 0x62c));
    }
    else
    {
        // ORIG LAB_088ece9a：ScanInt → m_306（byte）
        script->m_306 = (char)ScanInt((bool*)0);
        break;
    }
case 0x630:  // "[road necessary party]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[road necessary party]", 0x630));
    }
    else
    {
    {
        // ORIG LAB_088ecf16：读 count，循环读 DimensionPartyCount(3 char)，去重后 push 入 m_dimension308
        int count = ScanInt(&ok);
        for (int i = 0; i < count; ++i) {
            DimensionPartyCount dpc;
            dpc.m_0 = (char)ScanInt(&ok);
            dpc.m_1 = (char)ScanInt(&ok);
            dpc.m_2 = (char)ScanInt(&ok);
            std::vector<DimensionPartyCount>::iterator it =
                std::find(script->m_dimension308.begin(), script->m_dimension308.end(), dpc);
            // ORIG 088ed08e：operator== 后取反判失败（__gnu_cxx::operator== 而非 !=）。
            if (!(it == script->m_dimension308.end()))
            {
                // 重复值不是幂等去重，而是错误并
                // 立即失败（日志参数为 dungeon/type/difficulty/count）。
                printf("road necessary party invalid value (dungeon:%d,type:%d,difficulty:%d,count:%d)\n",
                       script->m_18, (int)dpc.m_0, (int)dpc.m_1, (int)dpc.m_2);
                return false;
            }
            script->m_dimension308.push_back(dpc);
        }
        // ORIG 0x088ed12a：列表结束后无条件消费一个 type token。
        ScanType(token, true);
        break;
    }
    }

case 0x655:  // "[village attack revenge dungeon]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[village attack revenge dungeon]", 0x655));
    }
    else
    {
        // ORIG LAB_088ed12f：4×ScanInt → m_316/m_317/m_314/m_315（按 C 顺序 byte）
        script->m_316 = (char)ScanInt((bool*)0);
        script->m_317 = (char)ScanInt((bool*)0);
        script->m_314 = (char)ScanInt((bool*)0);
        script->m_315 = (char)ScanInt((bool*)0);
        break;
    }
case 0x65d:  // "[schedule]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[schedule]", 0x65d));
    }
    else
    {
    {
        // ORIG LAB_088ed1f9：5 个时间串默认构造在循环外（088ed259-088ed297），
        // FindIndexByFullScan 循环外一次（088ed2a6→local_78）；循环体固定槽
        // ds(-0x12b0)：memset 0x24(088ed2ce)→m_type=0 显式写(088ed2d3)→
        // m_idx(088ed2dd)→ScanInt id(088ed2ef)，ok!=1 则 break 出块回主循环
        // (088ed303→088ed681 esi=1)→m_eventId=id→ScanStr×5：首串失败同为
        // break 语义(088ed325→088ed684)，后四串失败 ebx=0 返回 false
        // (088ed340)。Convert 各段：拷贝临时串一次+值传参（088ed3ce），
        // ConvertStringToTime 直接把 long& 写进 +0x0c/+0x10 字段
        // (lea ds+0xc, 088ed3d3)、weekday 经循环内 unsigned int 局部回填
        // +0x14(088ed530)、两组 HHMMSS 直接取 +0x18/+0x19/+0x1a 与
        // +0x1b/+0x1c/+0x1d 字段地址(088ed554-088ed577)，无 h/m/sec 局部。
        std::string strStart, strEnd, strWeekday, strStartTime, strEndTime;
        const int scheduleFileIdx = reinterpret_cast<STScriptFileList*>(
            &g_stDungeonScriptFileList)->FindIndexByFullScan(path);
        while (true) {
            _DailySchedule ds;
            memset(&ds, 0, sizeof(ds));              // 0x24，ORIG 088ed2ce
            ds.m_type = 0;                           // ORIG 088ed2d3 显式零写
            ds.m_idx = scheduleFileIdx;              // ORIG 088ed2dd
            int id = ScanInt(&ok);
            if (!ok) break;
            ds.m_eventId = id;                       // ORIG 088ed30c
            if (!ScanStr(&strStart)) break;          // ORIG 088ed325 首串失败=结束
            if (!ScanStr(&strEnd)) return false;
            if (!ScanStr(&strWeekday)) return false;
            if (!ScanStr(&strStartTime)) return false;
            if (!ScanStr(&strEndTime)) return false;
            if (!ConvertStringToTime(std::string(strStart),
                    *(long*)&ds.m_startSec))
                return false;                        // +0x0c
            if (!ConvertStringToTime(std::string(strEnd),
                    *(long*)&ds.m_endSec))
                return false;                        // +0x10
            unsigned int wday = 0;                   // ORIG -0x11b4 循环内初始化
            if (!ConvertWeekdayStringToInt(std::string(strWeekday), wday))
                return false;
            ds.m_wday = static_cast<int>(wday);      // +0x14
            if (!ConvertHHMMSSStringToInt(std::string(strStartTime),
                    ds.m_startHour, ds.m_startMin, ds.m_startSec2))
                return false;                        // +0x18..0x1a 字段直写
            if (!ConvertHHMMSSStringToInt(std::string(strEndTime),
                    ds.m_endHour, ds.m_endMin, ds.m_endSec2))
                return false;                        // +0x1b..0x1d 字段直写
            script->m_dailySchedule.push_back(ds);
        }
        break;
    }
    }

case 0x697:  // "[limit party count]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[limit party count]", 0x697));
    }
    else
    {
        // ORIG LAB_088ed7a2：写入 +0x324，默认值为 4。
        script->m_324 = (unsigned char)ScanInt((bool*)0);
        break;
    }

case 0x69a:  // "[limit inout count]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[limit inout count]", 0x69a));
    }
    else
    {
        script->m_325 = (char)ScanInt((bool*)0);   // +0x325（report LAB_088ed81e 实证）
        break;
    }

case 0x69d:  // "[hit count]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[hit count]", 0x69d));
    }
    else
    {
        // ORIG LAB_088ed89a：连续五个整数写入 +0x128 的数组。
        for (int i = 0; i < 5; ++i)
        {
            script->m_128[i] = ScanInt(&ok);
            if (!ok)
                break;
        }
        break;
    }

case 0x6a5:  // "[seal door appear rate]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[seal door appear rate]", 0x6a5));
    }
    else
    {
        script->m_13c = ScanInt((bool*)0);  // +0x13c
        break;
    }

case 0x6a8:  // "[boss room entrance condition]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[boss room entrance condition]", 0x6a8));
    }
    else
    {
    {
        // ORIG LAB_088ed9c0（Ghidra C 实证）：token∈{[hunt monster]→m_0=1 且每组 3 int /
        // [seeking]→m_0=2 且每组 2 int}，先 reserve(count*n)，再逐组循环 ScanInt push 到
        // m_vec4(+0x32c)；任一 ScanStr/ScanInt 失败 → import 返回 false。
        if (!ScanStr(&token))
        {
            return false;
        }
        int condCount = ScanInt(&ok);
        if (!ok)
        {
            return false;
        }
        if (token == "[hunt monster]")
        {
            script->m_bossRoom328.m_0 = 1;
            script->m_bossRoom328.m_vec4.reserve(condCount * 3);
            for (int i = 0; i < condCount; ++i)
            {
                script->m_bossRoom328.m_vec4.push_back(ScanInt(&ok));
                if (!ok) return false;
                script->m_bossRoom328.m_vec4.push_back(ScanInt(&ok));
                if (!ok) return false;
                script->m_bossRoom328.m_vec4.push_back(ScanInt(&ok));
                if (!ok) return false;
            }
        }
        else if (token == "[seeking]")
        {
            script->m_bossRoom328.m_0 = 2;
            script->m_bossRoom328.m_vec4.reserve(condCount * 2);
            for (int i = 0; i < condCount; ++i)
            {
                script->m_bossRoom328.m_vec4.push_back(ScanInt(&ok));
                if (!ok) return false;
                script->m_bossRoom328.m_vec4.push_back(ScanInt(&ok));
                if (!ok) return false;
            }
        }
        else
        {
            return false;
        }
        break;
    }
    }

case 0x6ca:  // "[dummy appear count]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[dummy appear count]", 0x6ca));
    }
    else
    {
        script->m_16c = ScanInt((bool*)0);  // +0x16c
        break;
    }

case 0x6cd:  // "[dungeon type]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[dungeon type]", 0x6cd));
    }
    else
    {
    {
        // ORIG LAB_088edd44（Ghidra C 实证）：ScanStr 读 dungeon 类型名 → m_358(+0x358)；
        // "tower of death"/"tower of boss" 额外把 maze 置 2x2 并 resize grids(4)；
        // "advance altar" 额外置 m_2ab=1；未知名 → m_358=0。ScanStr 失败 → import false。
        if (!ScanStr(&token))
        {
            return false;
        }
        if (token == "tower of death")
        {
            script->m_358 = 1;
            mazeLocal.m_width = 2;
            mazeLocal.m_height = 2;
            mazeLocal.m_grids.resize(4, GridScript());
        }
        else if (token == "tower of boss")
        {
            script->m_358 = 3;
            mazeLocal.m_width = 2;
            mazeLocal.m_height = 2;
            mazeLocal.m_grids.resize(4, GridScript());
        }
        else if (token == "advance altar")
        {
            script->m_358 = 4;
            script->m_2ab = 1;
        }
        else if (token == "power dungeon")
        {
            script->m_358 = 2;
        }
        else
        {
            script->m_358 = 0;
        }
        break;
    }
    }

case 0x6f5:  // "[tower random map indexes]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[tower random map indexes]", 0x6f5));
    }
    else
    {
        script->m_33c = ScanInt(&ok);  // +0x33c
        break;
    }

case 0x6f8:  // "[tower stage]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[tower stage]", 0x6f8));
    }
    else
    {
    {
        // ORIG LAB_088ee05d：vector 构造后显式 clear()（088ee0de）；每项先
        // memset(8B 临时 RandomList) 再落两个 ScanInt 值槽；weight==0（含读失败）
        // 直接导入失败。完成后 printf 并以 make_pair(具名左值) insert（重复 key 不覆盖）。
        int maxStage = ScanInt(&ok);
        if (!ok)
            return false;
        std::vector<RandomList> vec;
        vec.clear();
        while (true) {
            RandomList rl;
            memset(&rl, 0, sizeof(rl));
            const int mapIdx = ScanInt(&ok);
            if (!ok)
                break;
            rl.m_mapIndex = mapIdx;
            rl.m_weight = ScanInt(&ok);
            if (rl.m_weight == 0)
                return false;
            vec.push_back(rl);
        }
        printf("\n[max stage] : %d, [stage] : %d\n",
               script->m_33c, maxStage);
        for (unsigned int i = 0; i < vec.size(); ++i)
            printf("[map] : %d, [rate] : %d\n",
                   vec[i].m_mapIndex, vec[i].m_weight);
        script->m_map340.insert(std::make_pair(maxStage, vec));
        break;
    }
    }

case 0x71e:  // "[death tower map indexes]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[death tower map indexes]", 0x71e));
    }
    else
    {
    {
        // ORIG LAB_088ee328：m_368=ScanInt；随后循环 (index,index) 对 push m_vec35c
        // （第二值读取失败 → import false）。
        script->m_368 = ScanInt(&ok);
        while (true)
        {
            int a = ScanInt(&ok);
            if (!ok) break;
            int b = ScanInt(&ok);
            if (!ok) return false;
            // ORIG 088ee413 调用 make_pair<int&,int&>（非 pair 直构）
            script->m_vec35c.push_back(std::make_pair(a, b));
        }
        break;
    }
    }

case 0x72c:  // "[named monster map pos]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[named monster map pos]", 0x72c));
    }
    else
    {
    {
        // ORIG LAB_088ee43e：循环 (x,y) → STNamedMonsterMapPos push m_vec3c8，直到 x 读取失败。
        while (true)
        {
            int x = ScanInt(&ok);
            if (!ok) break;
            int y = ScanInt(&ok);
            STNamedMonsterMapPos nm;
            nm.m_x = x;
            nm.m_y = y;
            script->m_vec3c8.push_back(nm);
        }
        break;
    }
    }

case 0x73f:  // "[blood max round]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[blood max round]", 0x73f));
    }
    else
    {
        script->m_3c4 = ScanInt(&ok);  // +0x3c4
        break;
    }

case 0x744:  // "[event monster2]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[event monster2]", 0x744));
    }
    else
    {
    {
        // ORIG LAB_088ee594 __inited==1：读 m_3ec/m_3f0 + 循环 5 int 的 stSecondEventMonster_t 入 m_vec3f8
        script->m_3ec = ScanInt(&ok);
        if (!ok) return false;
        script->m_3f0 = ScanInt(&ok);
        if (!ok) return false;
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
        // ORIG：非零 m_3ec 必须伴随至少一个事件条目。
        if (script->m_3ec != 0 && script->m_vec3f8.empty())
            return false;
        break;
    }
    }

case 0x766:  // "[event monster]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[event monster]", 0x766));
    }
    else
    {
    {
        // ORIG LAB_088ee7bb（Ghidra C）：m_3d4/m_3d8=ScanInt；随后循环 stEventMonster_t
        // （eventIdx/monsterIdx/prob/field0c/field10）push m_vec3e0 且 m_3dc 累加 prob；
        // 任一组内 ScanInt 失败 → import false。收尾校验：非零 m_3d4 必须有至少
        // 一个事件条目，否则 import false。
        script->m_3d4 = ScanInt(&ok);
        if (!ok) return false;
        script->m_3d8 = ScanInt(&ok);
        if (!ok) return false;
        while (true)
        {
            stEventMonster_t em;
            em.m_eventIdx = ScanInt(&ok);
            if (!ok) break;
            em.m_monsterIdx = ScanInt(&ok);
            if (!ok) return false;
            em.m_prob = ScanInt(&ok);
            if (!ok) return false;
            em.m_field0c = ScanInt(&ok);
            if (!ok) return false;
            em.m_field10 = ScanInt(&ok);
            if (!ok) return false;
            script->m_3dc += em.m_prob;
            script->m_vec3e0.push_back(em);
        }
        if (script->m_3d4 != 0 && script->m_vec3e0.empty())
            return false;
        break;
    }
    }

case 0x7a9:  // "[tower high skill initial cool time]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[tower high skill initial cool time]", 0x7a9));
    }
    else
    {
        // ORIG LAB_088ee9e2：2×ScanFloat 经 ROUND → m_36c/m_370（int）
        script->m_36c = (int)(ScanFloat(&ok) + 0.5f);
        script->m_370 = (int)(ScanFloat(&ok) + 0.5f);
        break;
    }
case 0x7ad:  // "[tower high skill initial cool time rate]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[tower high skill initial cool time rate]", 0x7ad));
    }
    else
    {
        // ORIG LAB_088eead7：2×ScanFloat → +0x374/+0x378（float 位）
        script->m_374 = ScanFloat(&ok);
        script->m_378 = ScanFloat(&ok);
        break;
    }
case 0x7b1:  // "[tower fp cubepiece]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[tower fp cubepiece]", 0x7b1));
    }
    else
    {
        // ORIG LAB_088eeb6d：ScanFloat 经 ROUND(fldcw+fistpl) → +0x37c（int）
        script->m_37c = (int)(ScanFloat(&ok) + 0.5f);
        break;
    }
case 0x7b4:  // "[tower limit of stackable item]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[tower limit of stackable item]", 0x7b4));
    }
    else
    {
        // ORIG LAB_088eec17：ScanFloat 经 ROUND → +0x380（int）
        script->m_380 = (int)(ScanFloat(&ok) + 0.5f);
        break;
    }
case 0x7b7:  // "[tower recovery]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[tower recovery]", 0x7b7));
    }
    else
    {
        // [tower limit of stackable item]：RecoveryStairInfo（=现有 [tower recovery] 逻辑）
        {
            RecoveryStairInfo rsi;
            // ORIG 0x88eed3d/0x88eed7f：fistpl 使用 ROUND，脚本值为正时
            // 等价于加 0.5 后转 int；不能直接采用 C++ 截断。
            rsi.m_0 = static_cast<int>(ScanFloat(&ok) + 0.5f);
            rsi.m_4 = static_cast<int>(ScanFloat(&ok) + 0.5f);
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
    }

case 0x7ce:  // "[tower dialog]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[tower dialog]", 0x7ce));
    }
    else
    {
    {
        // ORIG LAB_088eee7d（Ghidra C）：STTowerDialog{index,name,flag8/9/A}。
        // ScanInt index → ScanStr name → ScanStr 事件串；"on start"/"on complete"/"on die"
        // 置对应 flag 并 push 到 m_towerDialog390 三个 vector（+0x390/+0x39c/+0x3a8）；
        // 未匹配串 logFormat（省略日志）；任一 Scan 失败仅终止本条。
        STTowerDialog dlg;
        dlg.m_index = ScanInt(&ok);
        if (!ok) return false;
        if (!ScanStr(&dlg.m_name)) return false;
        std::string eventStr;
        if (!ScanStr(&eventStr)) return false;
        if (eventStr == "on start")
        {
            dlg.m_flag8 = 1;
            script->m_towerDialog390.m_vec0.push_back(dlg);
        }
        else if (eventStr == "on complete")
        {
            dlg.m_flag9 = 1;
            script->m_towerDialog390.m_vecc.push_back(dlg);
        }
        else if (eventStr == "on die")
        {
            dlg.m_flagA = 1;
            script->m_towerDialog390.m_vec18.push_back(dlg);
        }
        else
        {
            LogManager::logFormat(
                0, "../RDARScriptDungeon.cpp", __FUNCTION__, 0x7ee,
                "Error on scanning tower dialog. There is no match string %s",
                eventStr.c_str());
        }
        break;
    }
    }

case 0x819:  // "[tower item drop]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[tower item drop]", 0x819));
    }
    else
    {
        script->m_3b4 = ScanInt(&ok);  // +0x3b4
        if (!ok)
            return false;
        break;
    }

case 0x81e:  // "[tower max clear item num]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[tower max clear item num]", 0x81e));
    }
    else
    {
        script->m_3b8 = ScanInt(&ok);  // +0x3b8
        if (!ok)
            return false;
        break;
    }

case 0x824:  // "[evil]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[evil]", 0x824));
    }
    else
    {
        // ORIG LAB_088ef22e：循环 5×ScanInt → m_410[i]（偏移 (i+0x104)*4=+0x410）
        for (int ei = 0; ei < 5; ++ei)
        {
            script->m_410[ei] = ScanInt((bool*)0);
        }
        break;
    }
case 0x829:  // "[evil high level]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[evil high level]", 0x829));
    }
    else
    {
        // ORIG LAB_088ef2c6：循环 5×ScanInt → m_424[i]（偏移 (i+0x108)*4+4=+0x424）
        for (int ei = 0; ei < 5; ++ei)
        {
            script->m_424[ei] = ScanInt((bool*)0);
        }
        break;
    }
case 0x834:  // "[evil rate]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[evil rate]", 0x834));
    }
    else
    {
    {
        // ORIG LAB_088ef35f：四个整数百分比写 m_438/m_440/m_43c/m_444。
        script->m_438 = static_cast<float>(ScanInt((bool*)0)) / 100.0f;
        script->m_440 = static_cast<float>(ScanInt((bool*)0)) / 100.0f;
        script->m_43c = static_cast<float>(ScanInt((bool*)0)) / 100.0f;
        script->m_444 = static_cast<float>(ScanInt((bool*)0)) / 100.0f;
        break;
    }
    }

case 0x83c:  // "[monster difficulty bonus]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[monster difficulty bonus]", 0x83c));
    }
    else
    {
    {
        // ORIG LAB_088ef474：13 行 × 5 float → m_vec404；每个元素即使
        // ScanFloat 失败也先 push 该值，随后记录日志并终止本段，不提交不完整行。
        for (int i = 0; i < 0xd; ++i) {
            std::vector<float> row;
            row.clear();   // ORIG 088ef4f7：ctor 后显式 clear()
            row.reserve(5);
            bool rowOk = true;
            for (int j = 0; j < 5; ++j) {
                float f = ScanFloat(&ok);
                row.push_back(f);
                if (!ok) {
                    rowOk = false;
                    LogManager::logFormat(
                        0, "../RDARScriptDungeon.cpp", __FUNCTION__, 0x848,
                        "[%s] : Error - DungeonScript MonsterDifficultyBouns Number",
                        __FUNCTION__);
                    break;
                }
            }
            if (!rowOk)
                break;
            script->m_vec404.push_back(row);
        }
        if (!ok)
        {
            LogManager::logFormat(
                0, "../RDARScriptDungeon.cpp", __FUNCTION__, 0x84e,
                "[%s] : Error - DungeonScript MonsterDifficultyBouns Number",
                __FUNCTION__);
        }
        else
        {
            std::string endTag;
            ScanType(endTag, true);
            if (endTag != "[/monster difficulty bonus]")
            {
                LogManager::logFormat(
                    0, "../RDARScriptDungeon.cpp", __FUNCTION__, 0x858,
                    "[%s] : Error - DungeonScript [/monster difficulty bonus]",
                    __FUNCTION__);
            }
        }
        break;
    }
    }

case 0x85d:  // "[revision table]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[revision table]", 0x85d));
    }
    else
    {
        ScanStr(&script->m_str448);  // +0x448
        break;
    }

case 0x862:  // "[gold card use]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[gold card use]", 0x862));
    }
    else
    {
        ok = false;
        script->m_44c = static_cast<unsigned char>(ScanInt(&ok) != 0);
        if (!ok)
            return false;
        break;
    }

case 0x86e:  // "[SWAY EFFECT]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[SWAY EFFECT]", 0x86e));
    }
    else
    {
        // ORIG LAB_088ef7e5（Ghidra C 实证）：先置 m_47c=1，再读 0x484/0x480。
        script->m_47c = 1;
        script->m_484 = ScanInt((bool*)0);  // +0x484
        script->m_480 = ScanInt((bool*)0);  // +0x480
        break;
    }

case 0x874:  // "[no fatigue]"（ORIG LAB_088ef884/Ghidra C：置 +0x48c 标志）
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[no fatigue]", 0x874));
    }
    else
    {
        script->m_48c = 1;
        break;
    }

case 0x87a:  // "[tower of despair]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[tower of despair]", 0x87a));
    }
    else
    {
        script->m_2a9 = 1;   // ORIG LAB_088ef8f3：local_12c[0x2a9]=1
        break;
    }

case 0x880:  // "[quest npc dungeon]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[quest npc dungeon]", 0x880));
    }
    else
    {
        // ORIG LAB_088ef962：ScanInt 非零 → m_2aa(+0x2aa)；失败 → import false
        {
            int value = ScanInt(&ok);
            if (!ok) return false;
            script->m_2aa = value != 0;
        }
        break;
    }
case 0x889:  // "[designate dungeon difficulty]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[designate dungeon difficulty]", 0x889));
    }
    else
    {
        script->m_490 = ScanInt((bool*)0);  // +0x490
        break;
    }

case 0x88e:  // "[tournament dungeon]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[tournament dungeon]", 0x88e));
    }
    else
    {
        // ORIG LAB_088efa7a：ScanInt → m_494(+0x494 byte)；失败 → import false
        script->m_494 = (char)ScanInt(&ok);
        if (!ok) return false;
        break;
    }
case 0x893:  // "[tournament round fatigue]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[tournament round fatigue]", 0x893));
    }
    else
    {
        // ORIG LAB_088efb11：ScanInt → m_498(+0x498)；失败 → import false
        script->m_498 = ScanInt(&ok);
        if (!ok) return false;
        break;
    }
case 0x898:  // "[result card]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[result card]", 0x898));
    }
    else
    {
    {
        // ORIG LAB_088efba8：先 ScanType 一次；仅 [reward item rate] 触发
        // 奖励物品表，随后无论该首标签内容都直接读取奖励卡三元组。
        // 原始格式没有 [reward rate] 子标签。
        std::string sub;
        if (!ScanType(sub, true))
            return false;
        if (sub == "[reward item rate]")
        {
            script->m_itemRate4b4.Clear();
            for (;;)
            {
                TournamentRewardItem item;
                item.m_data[0] = ScanInt(&ok);
                if (!ok)
                    break;
                item.m_data[1] = ScanInt(&ok);
                if (!ok)
                    return false;
                script->m_itemRate4b4.m_c += item.m_data[1];
                item.m_data[2] = ScanInt(&ok);
                if (!ok)
                    return false;
                script->m_itemRate4b4.m_items.push_back(item);
            }
        }

        for (;;)
        {
            TournamentRewardRate rate;
            int key = ScanInt(&ok);   // ORIG make_pair<int&,…>：非 const 左值
            if (!ok)
                break;                  // ORIG：卡片条目正常结束
            rate.m_0 = ScanInt(&ok);
            if (!ok)
                return false;
            rate.m_4 = ScanInt(&ok);
            if (!ok)
                return false;
            rate.m_8 = ScanInt(&ok);
            if (!ok)
                return false;
            if (rate.m_0 + rate.m_4 + rate.m_8 == 0)
                return false;
            std::pair<std::map<int, TournamentRewardRate>::iterator, bool> res =
                script->m_map49c.insert(std::make_pair(key, rate));
            if (!res.second)
            {
                puts("TournamentRewardRateMap Key is Duplicated!!!");
                return false;
            }
        }
        break;
    }
    }

case 0x8ed:  // "[tournament clear reward gold rate]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[tournament clear reward gold rate]", 0x8ed));
    }
    else
    {
        // ORIG LAB_088eff24：ScanFloat → +0x4c4（float 语义，m_4c4 初值 1.0f）
        script->m_4c4Value = ScanFloat(&ok);
        break;
    }
case 0x8f2:  // "[tournament clear reward exp]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[tournament clear reward exp]", 0x8f2));
    }
    else
    {
    {
        // ORIG LAB_088effb9：循环 (key uchar, val uint)；m_map4c8 insert，重复 key →
        // puts("TournamentClearRewardExpMap Key is Duplicated!!!") 后 import 返回 false。
        while (true)
        {
            int key = ScanInt(&ok);
            if (!ok) break;
            int val = ScanInt(&ok);
            if (!ok) return false;
            // ORIG make_pair<unsigned_char&,unsigned_int&>：实参为具名左值槽
            // （-0x1275 uchar / -0x127c uint），非 cast 临时。
            unsigned char kSlot = static_cast<unsigned char>(key);
            unsigned int vSlot = static_cast<unsigned int>(val);
            std::pair<std::map<unsigned char, unsigned int>::iterator, bool> res =
                script->m_map4c8.insert(std::make_pair(kSlot, vSlot));
            if (!res.second)
            {
                puts("TournamentClearRewardExpMap Key is Duplicated!!!");
                return false;
            }
        }
        break;
    }
    }

case 0x90c:  // "[risk dungeon]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[risk dungeon]", 0x90c));
    }
    else
    {
        script->m_4e0 = ScanInt((bool*)0) != 0;  // +0x4e0
        break;
    }


















































case 0x912:  // "[coin info]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[coin info]", 0x912));
    }
    else
    {
        // ORIG LAB_088f01dd：循环 ScanStr 难度子 tag → __lookup_3 表惰性链
        // （[easy]/[medium]/[hard]/[ultimate] = 0x91d/0x922/0x927/0x92c → 0..3，
        //   条目在 088f0432..088f05ce 的 per-case !__inited 分支逐例 insert）；
        // 未识别串（含 -1）同样走收尾：2×ScanInt(bool)（值写槽、不检查失败）→
        // UseCoinDungeonDiff{m_0=v1, m_4=(v2!=0)} → map+0x4e4 以 make_pair
        // 插入（重复 key 不覆盖）。
        int diff = -1;
        int t3 = 0;
        while (true)
        {
            // ORIG 088f01e3：复用函数级串槽，无独立 string ctor/dtor。
            if (!ScanStr(&token))
                break;   // ORIG ScanStr false → 结束本 case
            diff = -1;   // ORIG 088f01f8：每轮先置 -1
        retry_3_head:
            static DungeonTagMap __lookup_3;
            static bool __inited_3 = false;
            static SpinLock __init_lock_3;
        retry_3:
            if (__inited_3)
            {
                DungeonTagMap::const_iterator it3 = __lookup_3.find(token.c_str());
                t3 = (it3 != __lookup_3.end()) ? it3->second : -1;
            }
            else
            {
                __init_lock_3.enter();
                if (__inited_3)
                {
                    __init_lock_3.leave();
                    goto retry_3_head;
                }
                t3 = 0;   // ORIG：首过 subId=0 并入 [easy] case
            }
            switch (t3)
            {
            case 0:
            case 0x91d:  // "[easy]"
                if (!__inited_3)
                {
                    __lookup_3.insert(DungeonTagMap::value_type("[easy]", 0x91d));
                }
                else
                {
                    diff = 0;
                    break;
                }
            case 0x922:  // "[medium]"
                if (!__inited_3)
                {
                    __lookup_3.insert(DungeonTagMap::value_type("[medium]", 0x922));
                }
                else
                {
                    diff = 1;
                    break;
                }
            case 0x927:  // "[hard]"
                if (!__inited_3)
                {
                    __lookup_3.insert(DungeonTagMap::value_type("[hard]", 0x927));
                }
                else
                {
                    diff = 2;
                    break;
                }
            case 0x92c:  // "[ultimate]"
                if (!__inited_3)
                {
                    __lookup_3.insert(DungeonTagMap::value_type("[ultimate]", 0x92c));
                }
                else
                {
                    diff = 3;
                    break;
                }
            default:
                break;
            }
            if (!__inited_3)
            {
                __init_lock_3.leave();
                __inited_3 = true;
                goto retry_3_head;   // ORIG LAB_088f05f4
            }
            UseCoinDungeonDiff d;
            d.m_0 = ScanInt(&ok);
            d.m_4 = (ScanInt(&ok) != 0);
            // ORIG 088f0670：make_pair<int,UseCoinDungeonDiff>（两实参均 prvalue
            // 推导）+ pair<const int,U>(pair&&) 转换 + ~pair×2。
            script->m_map4e4.insert(std::make_pair(static_cast<int>(diff),
                                                   static_cast<UseCoinDungeonDiff>(d)));
        }
    }


case 0x942:  // "[party member coin limit]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[party member coin limit]", 0x942));
    }
    else
    {
        script->m_4fc = ScanInt(&ok);  // +0x4fc
        if (!ok)
            return false;
        break;
    }

case 0x94d:  // "[individual map movement]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[individual map movement]", 0x94d));
    }
    else
    {
        // ORIG LAB_088f074e：ScanInt 与 1 比较 → m_504(+0x504 byte)
        script->m_504 = ScanInt(&ok) == 1;
        break;
    }
case 0x954:  // "[open door even enemy]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[open door even enemy]", 0x954));
    }
    else
    {
        // ORIG LAB_088f07d0：ScanInt 与 1 比较 → m_505(+0x505 byte)
        script->m_505 = ScanInt(&ok) == 1;
        break;
    }
case 0x95c:  // "[disable exit]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[disable exit]", 0x95c));
    }
    else
    {
        // ORIG LAB_088f0852：ScanInt → +0x508 dword（char m_508 起 4 字节按 int 写入）
        script->m_508 = ScanInt(&ok);
        break;
    }
case 0x962:  // "[character respawn posetion]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[character respawn posetion]", 0x962));
    }
    else
    {
        script->m_50c = ScanInt(&ok) != 0;   // +0x50c（report LAB_088f08d4 实证，char）
        break;

    }

        default:
            break;
        }
        if (!__inited)
        {
            __init_lock.leave();
            __inited = true;
            goto retry_dispatch;   // ORIG LAB_088f09a9
        }
    }
    // ORIG 0x88f09c8..0x88f09ff：主循环结束后无条件提交最后一份
    // MazeScript，并把当前脚本在 g_stDungeonScriptFileList 中的索引写入
    // STDungeonScript+0x18。
    // ORIG LAB_088f09bb：收尾 push_back 前先取一次 size()（结果不使用）。
    script->m_mazeList170.size();
    script->m_mazeList170.push_back(mazeLocal);
    script->m_18 = reinterpret_cast<STScriptFileList*>(
        &g_stDungeonScriptFileList)->FindIndexByFullScan(path);
    return true;
}



// exportDungeonScript 依赖的脚本域符号（权威定义在 STMapScript.cpp /
// LinkMiscSecuTimerSolo.cpp / InitScriptProducers_4.cpp）。
void getScriptFileFullPath(char* out, unsigned int size, const char* dir,
                           const char* name);
void convertBinaryToText(const char* path);

bool exportDungeonScript(STDungeonScript* script, const char* path)
{
    // b28d-L3: DIFF/N82+N89 — ORIG 0x... 1314 vs 1306
    // 【验收标注 §6.2】超大函数（ORIG 1314 insn）。调用点差集零
    // （176 vs 175，ORIG PLT 无符号地址聚类：fprintf 0x0807da90×52/
    // fwrite 0x0807e320×31/fopen/fclose/fputc 全对位）；9 处 vector<int>
    // size()!=0 检查形 + 6 处迭代器后自增形已按 ORIG 还原（-41→-8）。
    // 残 -8 归因：[required item] enableFlag 4.1 分支独立物化 vs 4.4
    // setne 折叠（setcc-fold/N18b）、maze operator[] 结果 4.1 具名槽
    // 缓存（N82 槽号）、greed string 构造位序漂移岛（跨版本 EH 岛）。
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
             it != script->m_vec98.end(); it++)
            fprintf(file, "%d %d\n", it->first, it->second);
        fwrite("[/item]\n", 1, 8, file);
    }

    // [pathgate object]：+0xf8 POD 区 10 个具名 int
    fprintf(file, "[pathgate object]\n%d %d %d\n%d %d %d\n%d %d\n%d %d\n",
            script->m_pathgate.m_cells[0][0], script->m_pathgate.m_cells[0][1],
            script->m_pathgate.m_cells[0][2], script->m_pathgate.m_cells[1][0],
            script->m_pathgate.m_cells[1][1], script->m_pathgate.m_cells[1][2],
            script->m_pathgate.m_cells[2][0], script->m_pathgate.m_cells[2][1],
            script->m_pathgate.m_cells[3][0], script->m_pathgate.m_cells[3][1]);

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
                         it != grid->m_mapCandidates.end(); it++)
                        fprintf(file, "%d\n", *it);
                    fwrite("[/map specification]\n", 1, 21, file);
                }
                if (!grid->m_secondCandidates.empty())
                {
                    fprintf(file, "[boss map specification]\n%d %d\n", x, y);
                    for (std::vector<int>::iterator it = grid->m_secondCandidates.begin();
                         it != grid->m_secondCandidates.end(); it++)
                        fprintf(file, "%d\n", *it);
                    fwrite("[/boss map specification]\n", 1, 26, file);
                }
                if (!grid->m_layeredMapIndexes.empty())
                {
                    fwrite("[layered map specification]\n", 1, 28, file);
                    for (std::vector<int>::iterator it = grid->m_layeredMapIndexes.begin();
                         it != grid->m_layeredMapIndexes.end(); it++)
                        fprintf(file, "%d\n", *it);
                    fwrite("[/layered map specification]\n", 1, 29, file);
                }
            }
        }

        fwrite("[start map]\n", 1, 12, file);
        for (std::vector<std::pair<int, int> >::iterator it = maze.m_vec14.begin();
             it != maze.m_vec14.end(); it++)
            fprintf(file, "%d %d\n", it->first, it->second);
        fwrite("[/start map]\n", 1, 13, file);
        fwrite("[boss map]\n", 1, 11, file);
        for (std::vector<std::pair<int, int> >::iterator it = maze.m_vec20.begin();
             it != maze.m_vec20.end(); it++)
            fprintf(file, "%d %d\n", it->first, it->second);
        fwrite("[/boss map]\n", 1, 12, file);
    }

    if (script->m_warRoom18c.m_10 != -1)
        fprintf(file, "[warroom map index] %d\n", script->m_warRoom18c.m_10);

    // [monster type spawn prob]
    fwrite("[monster type spawn prob]", 1, 25, file);
    for (unsigned int i = 0; i < script->m_warRoom18c.m_spawnProb.size(); ++i)
    {
        for (int j = 0; j < 4; ++j)
            fprintf(file, "%d ", script->m_warRoom18c.m_spawnProb[i].m_probability[j]);
        fputc(10, file);
    }
    fwrite("[monster type spawn cost]", 1, 25, file);
    for (int j = 0; j < 4; ++j)
        fprintf(file, "%d", script->m_warRoom18c.m_c4[j]);
    fputc(10, file);

    fprintf(file, "[player kc] %d", script->m_warRoom18c.m_d4);

    // ---- spawn common monster index ----
    if (script->m_warRoom18c.m_vec14[0].size() != 0)
    {
        fwrite("[spawn common monster index]\n", 1, 29, file);
        for (unsigned int i = 0; i < script->m_warRoom18c.m_vec14[0].size(); ++i)
            fprintf(file, "%d\n", script->m_warRoom18c.m_vec14[0][i]);
        fwrite("[/spawn common monster index]\n", 1, 30, file);
    }
    // ---- spawn common champion index ----
    if (script->m_warRoom18c.m_vec14[1].size() != 0)
    {
        fwrite("[spawn common champion index]\n", 1, 30, file);
        for (unsigned int i = 0; i < script->m_warRoom18c.m_vec14[1].size(); ++i)
            fprintf(file, "%d\n", script->m_warRoom18c.m_vec14[1][i]);
        fwrite("[/spawn common champion index]\n", 1, 31, file);
    }
    // ---- spawn super champion index ----
    if (script->m_warRoom18c.m_vec14[2].size() != 0)
    {
        fwrite("[spawn super champion index]\n", 1, 29, file);
        for (unsigned int i = 0; i < script->m_warRoom18c.m_vec14[2].size(); ++i)
            fprintf(file, "%d\n", script->m_warRoom18c.m_vec14[2][i]);
        fwrite("[/spawn super champion index]\n", 1, 30, file);
    }
    // ---- spawn boss index ----
    if (script->m_warRoom18c.m_vec14[3].size() != 0)
    {
        fwrite("[spawn boss index]\n", 1, 19, file);
        for (unsigned int i = 0; i < script->m_warRoom18c.m_vec14[3].size(); ++i)
            fprintf(file, "%d\n", script->m_warRoom18c.m_vec14[3][i]);
        fwrite("[/spawn boss index]\n", 1, 20, file);
    }

    fprintf(file, "[spawn step max] %d\n", script->m_warRoom18c.m_50);
    if (script->m_warRoom18c.m_vec44.size() != 0)
    {
        fwrite("[spawn step resource pool]\n", 1, 27, file);
        for (unsigned int i = 0; i < script->m_warRoom18c.m_vec44.size(); ++i)
            fprintf(file, "%d\n", script->m_warRoom18c.m_vec44[i]);
        fwrite("[/spawn step resource pool]\n", 1, 28, file);
    }
    fprintf(file, "[kill count const] %d\n", (double)script->m_warRoom18c.m_ac);
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
    if (script->m_warRoom18c.m_vec6c[0].size() != 0)
    {
        fwrite("[common monster item drop list]\n", 1, 32, file);
        for (unsigned int i = 0; i < script->m_warRoom18c.m_vec6c[0].size(); ++i)
            fprintf(file, "%d\n", script->m_warRoom18c.m_vec6c[0][i]);
        fwrite("[/common monster item drop list]\n", 1, 33, file);
    }
    if (script->m_warRoom18c.m_vec6c[1].size() != 0)
    {
        fwrite("[common champion item drop list]\n", 1, 33, file);
        for (unsigned int i = 0; i < script->m_warRoom18c.m_vec6c[1].size(); ++i)
            fprintf(file, "%d\n", script->m_warRoom18c.m_vec6c[1][i]);
        fwrite("[/common champion item drop list]\n", 1, 34, file);
    }
    if (script->m_warRoom18c.m_vec6c[2].size() != 0)
    {
        fwrite("[super champion item drop list]\n", 1, 32, file);
        for (unsigned int i = 0; i < script->m_warRoom18c.m_vec6c[2].size(); ++i)
            fprintf(file, "%d\n", script->m_warRoom18c.m_vec6c[2][i]);
        fwrite("[/super champion item drop list]\n", 1, 33, file);
    }
    if (script->m_warRoom18c.m_vec6c[3].size() != 0)
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
    // P101 同族：ORIG 每分支独立 `mov $1,%eax; jmp` 出口岛，须独立 return true。
    if (name == "normal")
    {
        out = STAGE_NORMAL;
        return true;
    }
    if (name == "survival")
    {
        out = STAGE_SURVIVAL;
        return true;
    }
    // [推断] ORIG 未识别时写 0 并仍返回 true（见报告反编译）。
    out = STAGE_NORMAL;
    return true;
}

bool advancealtar::StageDifficulty::getDifficulty(const std::string& name,
                                                   T& out)
{
    // P63 反向：ORIG 各成功分支独立 `mov$1;jmp` 尾岛，return false 独占尾块。
    if (name == "easy")
    {
        out = DIFF_EASY;
        return true;
    }
    if (name == "medium")
    {
        out = DIFF_MEDIUM;
        return true;
    }
    if (name == "hard")
    {
        out = DIFF_HARD;
        return true;
    }
    return false;
}

bool advancealtar::RewardItemType::getRewardItemType(const std::string& name,
                                                     T& out)
{
    if (name == "normal")
    {
        out = REWARD_NORMAL;
        return true;
    }
    else if (name == "event")
    {
        out = REWARD_EVENT;
        return true;
    }
    return false;
}

// ImportAdvanceAltarDungeonTag ORIG 0x088f20a4（536 insn）：
// 按 str 给出的 tag 分派解析；脚本流已由调用方 loadRDARScriptFile 打开
// （path 参数在 ORIG 中未使用）。返回 false=解析失败，true=成功/结束。
bool ImportAdvanceAltarDungeonTag(STDungeonScript* script, std::string str,
                                  const char* path)
{
    // b28d-L3: DIFF/N82+N89 — ORIG 0x... 533 vs 527
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
                std::string rewardType;
                if (!ScanStr(&rewardType))
                    break;
                advancealtar::RewardItemType::T rewardItemType;
                // ORIG 088f23d9/088f23ee：getRewardItemType 的 bool 结果先落槽
                // （mov %al,-0x39）并作为**同一变量**传入后续两个 ScanInt 的
                // bool* 实参（lea -0x39 复用）——必须保留单变量形（P84/P158）。
                bool parsed = advancealtar::RewardItemType::getRewardItemType(
                    rewardType, rewardItemType);
                if (!parsed)
                    break;
                item.m_type = static_cast<int>(rewardItemType);
                item.m_value1 = ScanInt(&parsed);
                if (!parsed)
                    break;
                item.m_value2 = ScanInt(&parsed);
                if (!parsed)
                    break;
                vec.push_back(item);
            }
        }
    }
    else if (str == "[advance altar survival map]")
    {
        while (true)
        {
            // ORIG 088f24c0：int 局部先置 0（dead init，P171 照抄），
            // ScanInt 用**函数级 ok(-0x15)**，返回值落 int 槽。
            int mapId = 0;
            mapId = ScanInt(&ok);
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
                        std::string rewardType;
                        if (!ScanStr(&rewardType))
                            break;
                        advancealtar::RewardItemType::T rewardItemType;
                        // ORIG 088f26d9 区（Ghidra local_71 复用）：结果 bool 与
                        // 后续两个 ScanInt 的 bool* 实参为**同一变量**。
                        bool parsed = advancealtar::RewardItemType::getRewardItemType(
                            rewardType, rewardItemType);
                        if (!parsed)
                            break;
                        item.m_type = static_cast<int>(rewardItemType);
                        item.m_value1 = ScanInt(&parsed);
                        if (!parsed)
                            break;
                        item.m_value2 = ScanInt(&parsed);
                        if (!parsed)
                            break;
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
    else
    {
        // ORIG 0x088f27ec：未识别 tag 才返回 0；上述识别分支即使
        // 在内部遇到结束/解析失败，也统一返回 1。
        return false;
    }
    return true;
}
