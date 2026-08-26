// ============================================================================
// df_game_r 还原 —— STMapScript 类方法 + MapScript 导入/导出（G5 地图脚本表）
// 逐函数依据 ORIG objdump / Ghidra 伪 C 转写（地址随注）：
//   MapArea::clear 0x08366050 / MapArea ctor 0x0836608e / dtor 0x083660d6 /
//     copy 0x0837e5ca —— ORIG 与本 TU 同段归口
//   支撑类型：STAssignAnimation ctor 0x089e836c/Clear 0x089e832c、
//     STQuestAnimation 0x089e83d2/Clear 0x089e83ae、BackAniScript
//     0x089e8898/clear 0x089e884a、BackGroundEffect 0x089e8936/
//     clear 0x089e88da、BackGroundEffectBlock 0x089e897e/clear 0x089e89c6、
//     STDialog 0x089e8a1c/clear 0x089e8a5e、AssignNpc::clear 0x089e87ee、
//     STAssignItem copy 0x083ef750/dtor 0x083b6a8c（布局见 CMap.h）
//   STMapScript C1 0x0836cc94 / copy C1 0x0837e698 / D2 0x0836d4bc /
//     Clear T 0x0836c688（逐条转写，顺序保持汇编原序）
//   ScanChampionFeatures 0x089e6cf4 / WriteChampionFeatures 0x089e6e94
//     （CHAMPION_FEATURE_LIST/NAME 表自 ORIG rodata 解出，37 项）
//   ImportMapScript T 0x089dcf54：152 个标签分派分支级复原；ORIG 的
//     static const map<const char*,int> 查找表还原为等价 if/else 链。
//   ExportMapScript T 0x089e3c55：全段落输出转写（fopen "wb" → fprintf →
//     fclose → convertBinaryToText）。
// ============================================================================

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <list>
#include <map>
#include <queue>
#include <string>
#include <utility>
#include <vector>

#include "CMap.h"
#include "CVillageMonsterMgr.h"   // MapArea / STAttackedMonster（全局作用域权威头）
#include "DNFLexWrapper.h"        // loadRDARScriptFile/ScanType/ScanInt/ScanStr...
#include "LogManager.h"           // LogManager::logFormat
#include "CDataManager.h"         // stHellParty_t 视图链头（DataManagerInitDepends 须在其后）
#include "DataManagerInitDepends.h" // stHellParty_t / stHellPartyMonsterGroupList
#include "HellPartyGroupOrder.h"  // STHellPartyGroupOrder/Rate + pool 单例声明

// ---- 跨 TU 符号（ORIG 地址随注；定义在各自所属批次 TU）----
extern char g_szMapScriptBaseDirectory[0x100];                    // B 0x094fe2a0
extern std::map<int, std::string> g_stMapScriptFileList;          // B 0x094fe3a0
void getScriptFileFullPath(char* out, unsigned int size,
                           const char* dir, const char* name);    // T 0x088bba4e
const char* getElementName(int element);                          // 0x088bfee0
const char* getDrawLayerName(ENUM_DRAWLAYER drawLayer);          // T 0x088be212
int getDrawLayer(const char* s);                                  // 0x088be12d
int getElement(const char* s);                                    // 0x088bfe5b
void convertBinaryToText(const char* path);                       // T 0x088be04c（Export 收尾转换）
bool procSpecialPassiveObject(SPOBJ_ACTION_DATAS& action,         // T 0x089e7958
                              STAssignSpecialPassiveObj& owner, int fileIdx);
const char* toMbcs(const char* src);                              // T 0x08adee98
extern int* Get_instance_stHellParty_t();                         // T 0x089097c4（CDataManager.cpp 单例）
int ScanStrOrInt(bool& isStr, std::string& s, bool& isInt, bool consume);  // DNFLexWrapperHelpers.cpp

// STScriptFileList::FindIndexByFullScan 等价遍历（ORIG T 0x088bae56；
// g_stMapScriptFileList 布局同 STScriptFileList，map<path→index>）
static int findMapScriptIndex(const char* path)
{
    for (std::map<int, std::string>::const_iterator it = g_stMapScriptFileList.begin();
         it != g_stMapScriptFileList.end(); ++it)
    {
        if (it->second == path)
            return it->first;
    }
    return -1;
}

// ============================================================================
// 局部静态表（ORIG rodata 数据解出）
// ============================================================================

// ORIG _ZL13g_gridSymbols（[greed] 网格符号，16 项两字符编码）
static const char* const g_gridSymbols[16] = {
    "AA", "BB", "CC", "DD", "EE", "FF", "GG", "HH",
    "II", "JJ", "KK", "LL", "MM", "NN", "OO", "PP",
};

// ORIG CHAMPION_FEATURE_NAME @0x093f55a0（名称按特征值低字节索引）
static const char* const CHAMPION_FEATURE_NAME[37] = {
    "[Immune-Magic]", "[Immune-Physic]", "[Enhance-Physicaldefense]",
    "[Enhance-Magicaldefense]", "[Immune-Fire]", "[Immune-Water]",
    "[Immune-Light]", "[Immune-Dark]", "[Immune-ActiveStatus]",
    "[Enchant-Fire]", "[Enchant-Water]", "[Enchant-Light]",
    "[Enchant-Dark]", "[Speedup-Move]", "[Speedup-Attack]",
    "[Speedup-Hitrecovery]", "[Speedup-Wakeup]", "[Speedup-Hpregen]",
    "[Etc-Neverfly]", "[Etc-Reserrection]", "[onAttack-stun]",
    "[onAttack-bleeding]", "[onAttack-confuse]", "[onAttack-slow]",
    "[onAttack-lightning]", "[onAttack-poison]", "[onAttack-blind]",
    "[high-activestatus]", "[Etc-divide]", "[Etc-invisible]",
    "[Etc-healed]", "[Etc-infected]", "[Etc-counted]",
    "[Etc-berserk]", "[Etc-sleep]", "[Etc-revive]", "[Etc-teleport]",
};

// ORIG _ZL21CHAMPION_FEATURE_LIST @0x08ae5c80（37 项完整特征值）
static const int CHAMPION_FEATURE_LIST[37] = {
    0x001ff00, 0x000ff01, 0x002ff02, 0x002ff03, 0x0030004, 0x0030005,
    0x0030006, 0x0030007, 0x0040008, 0x003fe09, 0x003fe0a, 0x003fe0b,
    0x003fe0c, 0x002000d, 0x003000e, 0x003000f, 0x0020010, 0x0040011,
    0x0010012, 0x0020013, 0x002fd14, 0x002fd15, 0x002fd16, 0x002fd17,
    0x002fd18, 0x002fd19, 0x002fd1a, 0x003001b, 0x004fd1c, 0x004fd1d,
    0x004fd1e, 0x004fd1f, 0x004fd20, 0x004fd21, 0x004fd22, 0x004fd23,
    0x004fd24,
};

// ORIG 函数局部静态（b 段）：convertBinaryToText 缓冲 0x0949b4e0（0x5b400）、
// getDrawLayerName 缓冲 0x0949b4d0（0x10）。
static char s_convertToTextBuffer[0x5b400];
static char s_drawLayerNameBuffer[0x10];

// ORIG rodata 层名表 @0x08e086b2..0x08e086fa（getDrawLayer/getDrawLayerName 共用）
static const char* const DRAW_LAYER_NAMES[7] = {
    "[close]", "[normal]", "[bottom]", "[closeback]",
    "[middleback]", "[distantback]", "[cover]",
};

// ===========================================================================
// ScanChampionFeatures / WriteChampionFeatures —— ORIG 0x089e6cf4 / 0x089e6e94
// ===========================================================================

void ScanChampionFeatures(int count, STAssignMonster& monster)
{
    std::string tok;
    for (int i = 0; i < count; ++i)
    {
        ScanStr(&tok);
        for (int k = 0; k < 37; ++k)
        {
            if (tok == CHAMPION_FEATURE_NAME[CHAMPION_FEATURE_LIST[k] & 0xff])
            {
                monster.m_vec28.push_back(CHAMPION_FEATURE_LIST[k]);
                break;
            }
        }
    }
}

static void WriteChampionFeatures(FILE* file, const STAssignMonster& monster)
{
    fprintf(file, "%d ", static_cast<int>(monster.m_vec28.size()));
    for (size_t i = 0; i < monster.m_vec28.size(); ++i)
        fprintf(file, "`%s` ", CHAMPION_FEATURE_NAME[monster.m_vec28[i] & 0xff]);
    fputc('\n', file);
}

// ===========================================================================
// [time line] 段解析 —— advancealtar::StageTimeLineParameter::importPhase
// 0x0889a930 的流式语义转写（该成员函数属 advancealtar 批次未落地；其伪 C
// 仅操作 TimeLine& 目标，此处以等价自由函数承载）。返回 0 成功 / 错误码。
// ===========================================================================

static int importTimeLinePhases(advancealtar::TimeLine* timeline)
{
    for (;;)
    {
        std::string tok;
        if (!ScanType(tok, true))
            return 0x2be;                                  // [/time line] 缺失
        if (tok == "[/time line]")
            return 0;
        if (tok != "[phase]")
            continue;

        advancealtar::Phase phase;
        bool ok = true;
        for (;;)
        {
            std::string sub;
            if (!ScanType(sub, true))
            {
                ok = false;
                break;
            }
            if (sub == "[/phase]")
                break;
            if (sub == "[duration]")
            {
                phase.m_field0 = ScanInt(static_cast<bool*>(0));
                continue;
            }
            if (sub == "[type]")
            {
                std::string name;
                if (!ScanStr(&name))
                {
                    ok = false;
                    break;
                }
                // normal=0 / named=1 / boss=2（非法名错误码 0x2dc）
                if (name == "normal")
                    phase.m_field4 = 0;
                else if (name == "named")
                    phase.m_field4 = 1;
                else if (name == "boss")
                    phase.m_field4 = 2;
                else
                {
                    ok = false;
                    break;
                }
                continue;
            }
            if (sub == "[action assign]")
            {
                for (;;)
                {
                    advancealtar::_ActionAssign assign;    // ctor memset 0x10
                    assign.m_field0 = ScanInt(static_cast<bool*>(0));
                    if (!ScanInt(static_cast<bool*>(0)))
                        break;                             // 缺参路径合并为循环退出
                    assign.m_maxAliveCount =
                        static_cast<short>(ScanInt(static_cast<bool*>(0)));
                    phase.m_assigns.push_back(assign);
                }
                continue;
            }
        }
        if (!ok)
            return 0x2cc;
        timeline->m_phases.push_back(phase);
    }
}


// ===========================================================================
// getDrawLayerName / convertBinaryToText —— ORIG 0x088be212 / 0x088be04c
// （STMapScript Export 段使用的 ORIG 静态；逐条转写）
// ===========================================================================

const char* getDrawLayerName(ENUM_DRAWLAYER drawLayer)
{
    switch (drawLayer)
    {
    case DRAWLAYER_CLOSE:       strncpy(s_drawLayerNameBuffer, "[close]", 0x10); break;
    case DRAWLAYER_NORMAL:      strncpy(s_drawLayerNameBuffer, "[normal]", 0x10); break;
    case DRAWLAYER_BOTTOM:      strncpy(s_drawLayerNameBuffer, "[bottom]", 0x10); break;
    case DRAWLAYER_CLOSEBACK:   strncpy(s_drawLayerNameBuffer, "[closeback]", 0x10); break;
    case DRAWLAYER_MIDDLEBACK:  strncpy(s_drawLayerNameBuffer, "[middleback]", 0x10); break;
    case DRAWLAYER_DISTANTBACK: strncpy(s_drawLayerNameBuffer, "[distantback]", 0x10); break;
    case DRAWLAYER_COVER:       strncpy(s_drawLayerNameBuffer, "[cover]", 0x10); break;
    default:                    strncpy(s_drawLayerNameBuffer, "[normal]", 0x10); break; // ORIG 缺省 → "[normal]"
    }
    return s_drawLayerNameBuffer;
}

void convertBinaryToText(const char* path)
{
    int count = 0;                                        // -0x10(%ebp)
    FILE* fp = fopen(toMbcs(path), "rb");                 // 0x08e086ac
    for (int c = fgetc(fp); c != -1; c = fgetc(fp))       // -0xc(%ebp)
    {
        if (count > 0x5b3ff)                              // 缓冲上限
            break;
        if (c == '\n')
        {
            // 首字符或前一字符非 '\r' 时补 '\r'（\n 归一为 \r\n）
            if (count == 0 || s_convertToTextBuffer[count - 1] != '\r')
                s_convertToTextBuffer[count++] = '\r';
        }
        s_convertToTextBuffer[count++] = static_cast<char>(c);
    }
    fclose(fp);
    fp = fopen(toMbcs(path), "wb");                       // 0x08e086af
    fwrite(s_convertToTextBuffer, 1, count, fp);
    fclose(fp);
}

// ===========================================================================
// procSpecialPassiveObject —— ORIG T 0x089e7958（[special passive object]
// action 表扫描；"[hellparty]" 分支联动地狱派对单例与组序池）
// ===========================================================================

bool procSpecialPassiveObject(SPOBJ_ACTION_DATAS& action,
                              STAssignSpecialPassiveObj& owner, int fileIdx)
{
    std::string tok;                                      // -0xc4(%ebp)
    std::priority_queue<int> groupQueue;                  // less<int> 大顶（-0xd4）
    int curTop = 0;                                       // -0x28
    int lastTop = 0;                                      // -0x24
    int fieldIdx = 0;                                     // -0x20
    int slotIdx = 0;                                      // -0x1c

    for (int j = 0; j <= 5; ++j)                          // -0x18；action 六个 int 字段
    {
        if (j != 0)
        {
            (&action.m_type)[j] = ScanInt(static_cast<bool*>(0));   // 0x89e80ac
            continue;
        }

        // j == 0：动作类型名
        if (!ScanStr(&tok))                               // 0x88bc7a2
            continue;
        if (tok == "[monster]")                           // 0x08e1ad7b
        {
            action.m_type = 0;
            continue;
        }
        if (tok == "[trap]")                              // 0x08e1c0de
        {
            action.m_type = 1;
            continue;
        }
        if (tok == "[item]")                              // 0x08e1c0e5
        {
            action.m_type = 2;
            continue;
        }
        if (tok == "[quest]")                             // 0x08e1c0ec
        {
            action.m_type = 3;
            continue;
        }
        if (tok != "[hellparty]")                         // 0x08e1c0f4
            continue;

        // ---- [hellparty] 分支（0x89e7ae3..）----
        std::map<unsigned short, STHellPartyGroupOrder> localOrders;   // -0xf0
        for (;;)
        {
            bool ok = false;                              // -0xd5
            int tvIdx = ScanInt(&ok);                     // -0xf4
            if (!ok)
                break;
            groupQueue.push(tvIdx);

            stHellParty_t* hellParty =
                reinterpret_cast<stHellParty_t*>(Get_instance_stHellParty_t());
            std::map<unsigned short, stHellPartyMonsterGroupList>::iterator git =
                hellParty->m_groupMap.find(static_cast<unsigned short>(tvIdx));
            if (git == hellParty->m_groupMap.end())
            {
                LogManager::logFormat(0, "../RDARScriptMap.cpp", __FUNCTION__,
                                      0xd5c,
                                      "HELLPARTY_LOG : GROUP INDEX NOT FOUND : %d\n",
                                      tvIdx);
                return false;
            }

            // pair 视图 +0x58 = second.m_field54（难度槽位）
            const unsigned char diffSlot = git->second.m_field54;

            bool ok2 = false;
            int rate = ScanInt(&ok2);                     // -0x10 组序号
            if (!ok2)
                return false;
            bool ok3 = false;
            int cnt = ScanInt(&ok3);                      // -0xfc 数量
            if (!ok3)
                return false;
            if (cnt <= 0 || cnt > 9)
            {
                LogManager::logFormat(0, "../RDARScriptMap.cpp", __FUNCTION__,
                                      0xd69,
                                      "HELLPARTY_LOG : GROUP ORDER SCRIPT ERROR : %d\n",
                                      cnt);
                return false;
            }

            STHellPartyGroupRate orderRate;               // -0x108（前两 int 赋值）
            orderRate.m_groupIdx = tvIdx;
            orderRate.m_rate = rate;

            std::map<unsigned short, STHellPartyGroupOrder>::iterator mit =
                localOrders.find(static_cast<unsigned short>(cnt));
            if (mit != localOrders.end())                 // 0x89e7d64 前分支
            {
                mit->second.m_rates.push_back(orderRate);
                mit->second.m_rateByDifficulty[diffSlot] = static_cast<unsigned short>(
                    mit->second.m_rateByDifficulty[diffSlot] + rate);
            }
                                            // 0x89e7d64 未命中分支
            else
            {
                STHellPartyGroupOrder newOrder;           // ctor memset 0xc + list@+0xc
                newOrder.m_rates.push_back(orderRate);
                newOrder.m_rateByDifficulty[diffSlot] = static_cast<unsigned short>(
                    newOrder.m_rateByDifficulty[diffSlot] + rate);
                localOrders.insert(std::make_pair(
                    static_cast<unsigned short>(cnt), newOrder));
            }
        }

        // 组序表入池（key=fileIdx；ORIG make_pair(0x10(%ebp), localOrders)）
        Get_instance_groupOrder_map_pool()->insert(
            HellPartyGroupOrderMapPool::value_type(fileIdx, localOrders));

        // 大顶队列去重排空：每个唯一非零 top 处理一轮（0x89e7f31..）
        while (!groupQueue.empty())
        {
            curTop = groupQueue.top();
            if (curTop == lastTop || curTop == 0)
            {
                groupQueue.pop();
                continue;
            }
            lastTop = curTop;

            stHellParty_t* hellParty =
                reinterpret_cast<stHellParty_t*>(Get_instance_stHellParty_t());
            std::map<unsigned short, stHellPartyMonsterGroupList>::iterator git =
                hellParty->m_groupMap.find(static_cast<unsigned short>(curTop));
            // ORIG 此处未检查 end()（保持原样）

            fieldIdx = 0;                                 // 89e8056/89e805d 等价复位
            slotIdx = 0;
            (&action.m_type)[fieldIdx++] = 5;             // 动作类型 5
            while (git->second.m_monsterIdx[slotIdx] > 0) // pair+0x4 数组逐槽展开
            {
                (&action.m_type)[fieldIdx++] = git->second.m_monsterIdx[slotIdx];
                (&action.m_type)[fieldIdx++] = 1;
                (&action.m_type)[fieldIdx++] = 1;
                (&action.m_type)[fieldIdx++] = 1;
                (&action.m_type)[fieldIdx++] = -1;
                owner.m_vec10.push_back(action);
                fieldIdx = 1;                             // 保留 action[0]=5
                ++slotIdx;
            }
            fieldIdx = 0;
            slotIdx = 0;
        }
        return false;                                     // 89e8097 ebx=0
    }
    return true;                                          // 89e80da ebx=1
}
// ===========================================================================
// MapArea 方法（ORIG 与本 TU 同段；布局见 CVillageMonsterMgr.h 注记）
// ===========================================================================

void MapArea::clear()                       // ORIG 0x08366050
{
    m_x = 0;
    m_y = 0;
    m_w = 0;
    m_h = 0;
    m_list.clear();
}

MapArea::MapArea()                          // ORIG 0x0836608e
{
    clear();
}

MapArea::MapArea(const MapArea& other)      // ORIG 0x0837e5ca
    : m_x(other.m_x),
      m_y(other.m_y),
      m_w(other.m_w),
      m_h(other.m_h),
      m_list(other.m_list)
{
}

MapArea::~MapArea()                         // ORIG 0x083660d6（仅析构 +0x10 向量）
{
}

// ===========================================================================
// 支撑类型方法（Clear/clear 逐条转写）
// ===========================================================================


// ---------------------------------------------------------------------------
// TownPath ctor / clear —— ORIG C1 0x089e828e / clear 0x089e8258。
// ORIG 前段为 MapArea 子对象调用（clear→MapArea::clear 0x08366050、
// ctor→MapArea::C1 0x0836608e）；本 TU TownPath 为平铺布局（CMap.h），
// 按字段等价展开。
// ---------------------------------------------------------------------------
TownPath::TownPath()
{
    m_x = 0;
    m_y = 0;
    m_w = 0;
    m_h = 0;
    m_areaList.clear();
    m_field1c = 0;
    m_field20 = 0;
    m_conds.clear();
}

void TownPath::clear()
{
    m_x = 0;
    m_y = 0;
    m_w = 0;
    m_h = 0;
    m_areaList.clear();
    m_field1c = 0;                       // +0x1c movl $0
    m_field20 = 0;                       // +0x20 movl $0
    m_conds.clear();                     // vector<int>@0x24 clear
}

// ---------------------------------------------------------------------------
// Assign 系列结构 Clear —— ORIG 逐条转写
// ---------------------------------------------------------------------------

void STAssignBloodSystem::Clear()           // ORIG 0x089e8454
{
    m_id = -1;                              // +0x00 movl $-1
    m_field04 = 0;
    m_phase = 0;
    m_field0c = 0;
    m_field10 = 0;
    m_timeA = 0;
    m_timeB = 0;
    m_startCnt = 0;
    m_cntInc = 0;
    m_totalCnt = 0;                         // +0x24
}

void STAssignBloodPhaseTime::Clear()        // ORIG 0x089e84d0
{
    m_smallRoundOrder = 0;                  // +0x00
    m_phaseIdx = -1;                        // +0x04（-1 = 随机）
    m_interval = 0;                         // +0x08
    m_time = 1.0f;                          // +0x0c mov $0x3f800000
    m_field10 = 0;
    m_spawnCnt = 0;
    m_difficulty = 0;                       // +0x18 movb $0
}

void STAssignDefenseSystem::Clear()         // ORIG 0x089e852c（与 Blood 同构）
{
    m_id = -1;
    m_field04 = 0;
    m_phase = 0;
    m_field0c = 0;
    m_field10 = 0;
    m_timeA = 0;
    m_timeB = 0;
    m_startCnt = 0;
    m_cntInc = 0;
    m_totalCnt = 0;
}

void STAssignDefensePhaseTime::Clear()      // ORIG 0x089e85a8
{
    m_smallRoundOrder = 0;
    m_phaseIdx = -1;
    m_interval = 0;
    m_time = 1.0f;
    m_field10 = 0;
    m_spawnCnt = 0;                         // 无 +0x18 字节写（布局止于 +0x14）
}

void STAssignSpecialPassiveObj::Clear()     // ORIG 0x089e875a
{
    m_0 = -1;                               // +0x00 movl $-1
    m_c = 0;                                // +0x0c movl $0
    m_8 = m_c;                              // +0x08 ← +0x0c
    m_4 = m_8;                              // +0x04 ← +0x08
    m_vec10.clear();                        // vector@0x10
    m_1c = -1;                              // +0x1c movl $-1
}

// ---------------------------------------------------------------------------
// STAICharacterMapArrangeData —— ORIG Clear 0x089e85fe / C1 0x089e8676 /
// copy C1ERKS_ 0x083d31d2 / D1 0x083dc9a0
// ---------------------------------------------------------------------------

void STAICharacterMapArrangeData::Clear()
{
    m_type = 0;
    m_field04 = 0;
    m_field08 = 0;
    m_field0c = 0;
    m_camp = 0;
    m_roleType = 0;
    m_posX = 0;                             // +0x18 movb $0
    m_posY = 0;                             // +0x19 movb $0
    m_championFeatures.clear();             // vector<int>@0x1c
    m_npcIdx = -1;                          // +0x28 movl $-1
    m_collisionGroup.clear();               // vector<uchar>@0x2c
}

STAICharacterMapArrangeData::STAICharacterMapArrangeData()
{
    Clear();
}

STAICharacterMapArrangeData::STAICharacterMapArrangeData(
    const STAICharacterMapArrangeData& other)
    : m_type(other.m_type),
      m_field04(other.m_field04),
      m_field08(other.m_field08),
      m_field0c(other.m_field0c),
      m_camp(other.m_camp),
      m_roleType(other.m_roleType),
      m_posX(other.m_posX),
      m_posY(other.m_posY),
      m_championFeatures(other.m_championFeatures),   // copy 于 +0x28 前
      m_npcIdx(other.m_npcIdx),
      m_collisionGroup(other.m_collisionGroup)        // 尾部向量拷贝
{
}

STAICharacterMapArrangeData::~STAICharacterMapArrangeData()
{
    // ORIG 仅按逆序析构两个向量（编译器生成），无额外语句
}
void AssignNpc::clear()                     // ORIG 0x089e87ee
{
    m_npcIdx = 0;                           // +0x00 movl $0
    m_field04 = -1;                         // +0x04 movl $-1
    m_visible = false;                      // +0x08 movb $0
    m_dir = 0;                              // +0x0c
    m_x = 0;                                // +0x10
    m_y = 0;                                // +0x14
    m_z = 0;                                // +0x18
}

STAssignItem::STAssignItem(const STAssignItem& other)   // ORIG 0x083ef750
{
    m_field00 = other.m_field00;
    m_field04 = other.m_field04;
    m_field08 = other.m_field08;
    m_field0c = other.m_field0c;
    m_field10 = other.m_field10;
    m_field14 = other.m_field14;
    m_field18 = other.m_field18;
    m_field1c = other.m_field1c;
    m_field20 = other.m_field20;
    m_generateInfos = other.m_generateInfos;
}

STAssignItem::~STAssignItem()               // ORIG 0x083b6a8c（仅析构 +0x24 向量）
{
}

void STAssignAnimation::Clear()             // ORIG 0x089e832c
{
    m_name.assign("");                      // Ss::assign("")
    m_layer = 1;                            // +0x04 movl $1
    m_field08 = 0;                          // +0x08
    m_field0c = 0;                          // +0x0c
    m_field10 = 0;                          // +0x10
}

STAssignAnimation::STAssignAnimation()      // ORIG 0x089e836c
{
    Clear();
}

void STQuestAnimation::Clear()              // ORIG 0x089e83ae
{
    STAssignAnimation::Clear();
    m_field14 = 0;                          // +0x14
    m_flag18 = true;                        // +0x18 movb $1
}

STQuestAnimation::STQuestAnimation()        // ORIG 0x089e83d2
{
    Clear();
}

void BackAniScript::clear()                 // ORIG 0x089e884a
{
    m_filename.assign("");                  // Ss::assign("")
    m_layer = 1;                            // +0x04
    m_orderUpper = false;                   // +0x08
    m_loopYAxis = false;                    // +0x09
    m_hasScreenPos = false;                 // +0x0a
    m_screenX = 0;                          // +0x0c
    m_screenY = 0;                          // +0x10
}

BackAniScript::BackAniScript()              // ORIG 0x089e8898
{
    clear();
}

void BackGroundEffect::clear()              // ORIG 0x089e88da
{
    m_type = 2;                             // +0x00 movl $2
    m_layer = 7;                            // +0x04 movl $7
    m_field08 = -1;                         // +0x08
    m_field0c = -1;                         // +0x0c
    m_field10 = -1;                         // +0x10
    m_field14 = -1;                         // +0x14
    m_field18 = -1;                         // +0x18
    m_animName.clear();                     // +0x1c
}

BackGroundEffect::BackGroundEffect()        // ORIG 0x089e8936
{
    clear();
}

void BackGroundEffectBlock::clear()         // ORIG 0x089e89c6
{
    m_type = 2;                             // +0x00
    m_mode = 2;                             // +0x04
    m_val = 0;                              // +0x08
    m_animName.clear();                     // +0x0c
}

BackGroundEffectBlock::BackGroundEffectBlock()  // ORIG 0x089e897e
{
    clear();
}

void STDialog::clear()                      // ORIG 0x089e8a5e
{
    m_name.clear();
    m_onStart = false;                      // byte +0x04..+0x06 循环清零
    m_onComplete = false;
    m_flag06 = false;
}

STDialog::STDialog()                        // ORIG 0x089e8a1c
{
    clear();
}

// ===========================================================================
// STMapScript ctor / copy / dtor / Clear
// ===========================================================================

STMapScript::STMapScript()                  // ORIG C1 0x0836cc94
    : m_index(),
      m_list4(),
      m_c(),
      m_recoveryObjects(),
      m_listAnimation(),
      m_listQuestAnimation(),
      m_listPassiveObject(),
      m_vecMonster(),
      m_vecConditionMonster(),
      m_championRate(),
      m_listItem(),
      m_listSpecialPassive(),
      m_vecNpc(),
      m_element(),
      m_vecTile(),
      m_vecExtendedTile(),
      m_sightScroll(),
      m_vecBackAni(),
      m_vecBackGroundEffect(),
      m_vecBlockEffect(),
      m_vecDialog(),
      m_startPathDir(),
      m_blockPath(),
      m_tileOptionUpper(),
      m_vecTownMovableArea(),
      m_vecVirtualMovableArea(),
      m_vecGuildAgitEntrance(),
      m_pathgatePos(),
      m_pathgateRange(),
      m_dungeonStartArea(),
      m_summonStartArea(),
      m_178(),
      m_17c(),
      m_vec180(),
      m_openingBgm(),
      m_openingBgmParam(),
      m_vecSound(),
      m_str1a0(),
      m_str1a4(),
      m_str1a8(),
      m_sortBottom(),
      m_str1b0(),
      m_playerCountMin(),
      m_playerCountMax(),
      m_pvpStartArea(),
      m_pvpByPartySlot(),
      m_keepStateAxisX(),
      m_keepStateAxisY(),
      m_keepStateAxisZ(),
      m_24c(),
      m_vecMonsterSpawnPos(),
      m_vecAICharacter(),
      m_apcRandomPoint(),
      m_mapLoadingImagePath(),
      m_270(),
      m_274(),
      m_vecEventMonsterPos(),
      m_maxMonsterDropException(),
      m_vecBloodSystem(),
      m_vecBloodPhaseTime(),
      m_vecDefenseSystem(),
      m_vecDefensePhaseTime(),
      m_tournamentMonsters(),
      m_tournamentStartPos1(),
      m_tournamentStartPos2(),
      m_vecStaticStartPos(),
      m_vecStaticStartPos2(),
      m_limitPlayerMoveAreas(),
      m_vecAttackedMonster(),
      m_maxMonsterCountException(),
      m_328(),
      m_32c(),
      m_timeLine(),
      m_str34c(),
      m_moveLayeredType(),
      m_zposUnder(),
      m_allDeadCasePassable(),
      m_screenEdgeLeft(),
      m_screenEdgeRight(),
      m_screenEdgeTop(),
      m_screenEdgeBottom(),
      m_blockUseActiveSkill(),
      m_blockUseStackableItem(),
      m_fixChampion(),
      m_pad36f(),
      m_str370(),
      m_visibleMinimap(),
      m_visibleTownMinimap()
{
    Clear();
}

STMapScript::STMapScript(const STMapScript& other)      // ORIG C1 0x0837e698
    : m_index(other.m_index),                           // *param_1 = *param_2
      m_list4(other.m_list4),                           // list<int> copy
      m_c(other.m_c),                                   // param_1[3]
      m_recoveryObjects(other.m_recoveryObjects),       // vector<pair>@0x10
      m_listAnimation(other.m_listAnimation),           // list@0x1c
      m_listQuestAnimation(other.m_listQuestAnimation), // list@0x24
      m_listPassiveObject(other.m_listPassiveObject),   // list@0x2c
      m_vecMonster(other.m_vecMonster),                  // vector@0x34
      m_vecConditionMonster(other.m_vecConditionMonster), // vector@0x40
      m_championRate(other.m_championRate),             // param_1[0x13]
      m_listItem(other.m_listItem),                     // list@0x50
      m_listSpecialPassive(other.m_listSpecialPassive), // list@0x58
      m_vecNpc(other.m_vecNpc),                         // vector@0x60
      m_element(other.m_element),                       // param_1[0x1b]
      m_vecTile(other.m_vecTile),                       // vector<string>@0x70
      m_vecExtendedTile(other.m_vecExtendedTile),       // vector<string>@0x7c
      m_sightScroll(),                                  // 0x22..0x27 见下
      m_vecBackAni(other.m_vecBackAni),                 // vector@0xa0
      m_vecBackGroundEffect(other.m_vecBackGroundEffect), // vector@0xac
      m_vecBlockEffect(other.m_vecBlockEffect),         // vector@0xb8
      m_vecDialog(other.m_vecDialog),                   // vector@0xc4
      m_startPathDir(other.m_startPathDir),             // param_1[0x34]
      m_blockPath(),                                    // bytes 0xd4..0xd7 见下
      m_tileOptionUpper(other.m_tileOptionUpper),       // byte @0xd8
      m_vecTownMovableArea(other.m_vecTownMovableArea), // TownPath@0xdc
      m_vecVirtualMovableArea(other.m_vecVirtualMovableArea), // MapArea@0xe8
      m_vecGuildAgitEntrance(other.m_vecGuildAgitEntrance),   // TownPath@0xf4
      m_pathgatePos(),                                  // words 0x40..0x4f 见下
      m_pathgateRange(),
      m_dungeonStartArea(other.m_dungeonStartArea),     // MapArea@0x140
      m_summonStartArea(other.m_summonStartArea),       // MapArea@0x15c
      m_178(other.m_178),                               // param_1[0x5e]
      m_17c(other.m_17c),                               // param_1[0x5f]
      m_vec180(other.m_vec180),                         // vector<pair>@0x180
      m_openingBgm(other.m_openingBgm),                 // string@99(0x18c)
      m_openingBgmParam(other.m_openingBgmParam),       // param_1[100](0x190)
      m_vecSound(other.m_vecSound),                     // vector<string>@0x194
      m_str1a0(other.m_str1a0),                         // string@0x68(0x1a0)
      m_str1a4(other.m_str1a4),                         // string@0x69(0x1a4)
      m_str1a8(other.m_str1a8),                         // string@0x6a(0x1a8)
      m_sortBottom(other.m_sortBottom),                 // byte @0x6b(0x1ac)
      m_str1b0(other.m_str1b0),                         // string@0x6c(0x1b0)
      m_playerCountMin(other.m_playerCountMin),         // param_1[0x6d]/[0x6e] 成对
      m_playerCountMax(other.m_playerCountMax),
      m_pvpStartArea(),                                 // 5×MapArea 循环拷贝
      m_pvpByPartySlot(other.m_pvpByPartySlot),         // byte @0x92(0x248)
      m_keepStateAxisX(other.m_keepStateAxisX),         // u16 @0x249 成对
      m_keepStateAxisY(other.m_keepStateAxisY),
      m_keepStateAxisZ(other.m_keepStateAxisZ),         // byte @0x24b
      m_24c(0),                                         // copy 未复制（Ghidra 视角）
      m_vecMonsterSpawnPos(other.m_vecMonsterSpawnPos), // vector@0x94(0x250)
      m_vecAICharacter(other.m_vecAICharacter),         // vector@0x97(0x25c)
      m_apcRandomPoint(other.m_apcRandomPoint),         // param_1[0x9a]
      m_mapLoadingImagePath(other.m_mapLoadingImagePath), // string@0x9b(0x26c)
      m_270(other.m_270),                               // param_1[0x9c]/[0x9d] 成对
      m_274(other.m_274),
      m_vecEventMonsterPos(other.m_vecEventMonsterPos), // vector@0x9e(0x278)
      m_maxMonsterDropException(other.m_maxMonsterDropException), // param_1[0xa1]
      m_vecBloodSystem(other.m_vecBloodSystem),         // vector@0xa2(0x288)
      m_vecBloodPhaseTime(other.m_vecBloodPhaseTime),   // vector@0xa5(0x294)
      m_vecDefenseSystem(other.m_vecDefenseSystem),     // vector@0xa8(0x2a0)
      m_vecDefensePhaseTime(other.m_vecDefensePhaseTime), // vector@0xab(0x2ac)
      m_tournamentMonsters(other.m_tournamentMonsters), // map@0xae(0x2b8)
      m_tournamentStartPos1(other.m_tournamentStartPos1), // vector@0xb4(0x2d0)
      m_tournamentStartPos2(other.m_tournamentStartPos2), // vector@0xb7(0x2dc)
      m_vecStaticStartPos(other.m_vecStaticStartPos),   // vector@0xba(0x2e8)
      m_vecStaticStartPos2(other.m_vecStaticStartPos2), // vector@0xbd(0x2f4)
      m_limitPlayerMoveAreas(other.m_limitPlayerMoveAreas), // map@0xc0(0x300)
      m_vecAttackedMonster(other.m_vecAttackedMonster), // vector@0xc6(0x318)
      m_maxMonsterCountException(other.m_maxMonsterCountException), // param_1[0xc9]
      m_328(other.m_328),                               // param_1[0xca]
      m_32c(other.m_32c),                               // param_1[0xcb]
      m_timeLine(other.m_timeLine),                     // TimeLine@0xcc(0x330)
      m_str34c(other.m_str34c),                         // string@0xd3(0x34c)
      m_moveLayeredType(other.m_moveLayeredType),       // param_1[0xd4]/[0xd5] 成对
      m_zposUnder(other.m_zposUnder),
      m_allDeadCasePassable(other.m_allDeadCasePassable), // byte @0xd6(0x358)
      m_screenEdgeLeft(other.m_screenEdgeLeft),         // param_1[0xd7](0x35c)
      m_screenEdgeRight(other.m_screenEdgeRight),       // param_1[0xd8](0x360)
      m_screenEdgeTop(other.m_screenEdgeTop),           // param_1[0xd9](0x364)
      m_screenEdgeBottom(other.m_screenEdgeBottom),     // param_1[0xda](0x368)
      m_blockUseActiveSkill(other.m_blockUseActiveSkill), // byte @0xdb(0x36c)
      m_blockUseStackableItem(other.m_blockUseStackableItem), // byte 0x36d
      m_fixChampion(other.m_fixChampion),               // byte 0x36e
      m_pad36f(0),
      m_str370(other.m_str370),                         // string@0xdc(0x370)
      m_visibleMinimap(other.m_visibleMinimap),         // param_1[0xdd](0x374)
      m_visibleTownMinimap(other.m_visibleTownMinimap)  // param_1[0xde](0x378)
{
    // 标量区补拷贝（copy 伪 C 中间段）：
    for (int i = 0; i < 6; ++i)                         // param_1[0x22..0x27]=0x88..0x9f
        m_sightScroll[i] = other.m_sightScroll[i];
    for (int i = 0; i < 4; ++i)                         // param_1[0x35]+i = 0xd4..0xd7
        m_blockPath[i] = other.m_blockPath[i];
    for (int i = 0; i < 8; ++i)                         // param_1[0x40..0x4f]=0x100..0x13f
    {
        m_pathgatePos[i] = other.m_pathgatePos[i];
        m_pathgateRange[i] = other.m_pathgateRange[i];
    }
    for (int i = 0; i < 5; ++i)                         // MapAreaC2ERKS_ ×5（步进 7 words）
        m_pvpStartArea[i] = other.m_pvpStartArea[i];
}

STMapScript::~STMapScript()                 // ORIG D2 0x0836d4bc（逆序析构由编译器生成）
{
}

void STMapScript::Clear()                   // ORIG T 0x0836c688（顺序保持汇编原序）
{
    m_maxMonsterDropException = 0;          // param_1[0xa1]=0
    m_vecBloodSystem.clear();               // +0xa2
    m_vecBloodPhaseTime.clear();            // +0xa5
    m_vecDefenseSystem.clear();             // +0xa8
    m_vecDefensePhaseTime.clear();          // +0xab
    m_summonStartArea.clear();              // MapArea::clear(+0x57)
    m_178 = 0;                              // param_1[0x5e]=0
    m_timeLine.reset();                     // advancealtar::TimeLine::reset(+0xcc)
    m_17c = 0;                              // param_1[0x5f]=0
    m_vec180.clear();                       // vector<pair>::clear(+0x60)
    m_list4.clear();                        // list<int>::clear(+1)
    m_c = 0;                                // param_1[3]=0
    m_recoveryObjects.clear();              // vector<pair>::clear(+4)
    m_listAnimation.clear();                // list<STAssignAnimation>::clear(+7)
    m_listQuestAnimation.clear();           // list<STQuestAnimation>::clear(+9)
    m_listPassiveObject.clear();            // list<STAssignPassiveObject>::clear(+0xb)
    m_vecMonster.clear();                   // vector<STAssignMonster>::clear(+0xd)
    m_vecConditionMonster.clear();          // vector<STAssignMonster>::clear(+0x10)
    m_championRate = 0;                     // param_1[0x13]=0
    m_listItem.clear();                     // list<STAssignItem>::clear(+0x14)
    m_listSpecialPassive.clear();           // list<STAssignSpecialPassiveObj>::clear(+0x16)
    m_element = 4;                          // param_1[0x1b]=4
    m_vecTile.clear();                      // vector<string>::clear(+0x1c)
    m_vecExtendedTile.clear();              // vector<string>::clear(+0x1f)
    for (int i = 0; i < 6; ++i)             // 2×3 循环 param_1[0x22+..]=100
        m_sightScroll[i] = 100;
    m_vecBackAni.clear();                   // vector<BackAniScript>::clear(+0x28)
    m_vecBackGroundEffect.clear();          // vector<BackGroundEffect>::clear(+0x2b)
    m_vecBlockEffect.clear();               // vector<BackGroundEffectBlock>::clear(+0x2e)
    m_vecDialog.clear();                    // vector<STDialog>::clear(+0x31)
    m_startPathDir = 4;                     // param_1[0x34]=4
    for (int i = 0; i < 4; ++i)             // byte(0xd4+i)=0
        m_blockPath[i] = 0;
    m_tileOptionUpper = 0;                  // byte(param_1+0x36)=0
    m_vecTownMovableArea.clear();           // vector<TownPath>::clear(+0x37)
    m_vecVirtualMovableArea.clear();        // vector<MapArea>::clear(+0x3a)
    m_vecGuildAgitEntrance.clear();         // vector<TownPath>::clear(+0x3d)
    for (int i = 0; i < 8; ++i)             // 4×2 循环 param_1[0x40+..]=0
    {
        m_pathgatePos[i] = 0;               // param_1[i*2+j+0x40]
        m_pathgateRange[i] = 0;             // param_1[i*2+j+0x48]
    }
    m_dungeonStartArea.clear();             // MapArea::clear(+0x50)
    m_openingBgm.clear();                   // Ss::clear(+99=0x18c)
    m_openingBgmParam = 0;                  // param_1[100]=0(0x190)
    m_sortBottom = 0;                       // byte(param_1+0x6b)=0
    m_vecSound.clear();                     // vector<string>::clear(+0x65=0x194)
    m_str1a0.assign("");                    // Ss::assign(+0x68,"")（UNK_08c2f620=""）
    m_str1b0.assign("PVP놀이게임");          // Ss::assign(+0x6c,&UNK_08c2f621)
    m_playerCountMin = 2;                   // param_1[0x6d]=2
    m_playerCountMax = 8;                   // param_1[0x6e]=8
    for (int i = 0; i < 5; ++i)             // 5×MapArea::clear(+0x6f+i*7 words)
        m_pvpStartArea[i].clear();
    m_24c = 0;                              // param_1[0x93]=0
    m_str1b0.clear();                       // Ss::clear(+0x6c)（汇编原序保留的重复清空）
    m_vecMonsterSpawnPos.clear();           // vector<STRivalMapData>::clear(+0x94)
    m_vecAICharacter.clear();               // vector<STAICharacterMapArrangeData>::clear(+0x97)
    m_apcRandomPoint = 0;                   // param_1[0x9a]=0
    m_mapLoadingImagePath.clear();          // Ss::clear(+0x9b)
    m_274 = 0;                              // param_1[0x9d]=0
    m_270 = m_274;                          // param_1[0x9c]=param_1[0x9d]
    m_vecEventMonsterPos.clear();           // vector<STRivalMapData>::clear(+0x9e)
    m_vecNpc.clear();                       // vector<AssignNpc>::clear(+0x18)
    m_index = 0;                            // *param_1=0
    m_vecAttackedMonster.clear();           // vector<STAttackedMonster>::clear(+0xc6)
    m_limitPlayerMoveAreas.clear();         // map<int,MapArea>::clear(+0xc0)
    m_maxMonsterCountException = 0;         // param_1[0xc9]=0
    m_328 = 0;                              // param_1[0xca]=0
    m_32c = 0;                              // param_1[0xcb]=0
    m_tournamentMonsters.clear();           // map<char,vector<TournamentMonster>>::clear(+0xae)
    m_moveLayeredType = 4;                  // param_1[0xd4]=4
    m_zposUnder = -1;                       // param_1[0xd5]=-1
    m_allDeadCasePassable = 0;              // byte(param_1+0xd6)=0
    m_screenEdgeLeft = -1;                  // param_1[0xd7]=-1
    m_screenEdgeRight = -1;                 // param_1[0xd8]=-1
    m_screenEdgeTop = -1;                   // param_1[0xd9]=-1
    m_screenEdgeBottom = -1;                // param_1[0xda]=-1
    m_blockUseActiveSkill = 0;              // byte(param_1+0xdb)=0
    m_blockUseStackableItem = 0;            // byte(0x36d)=0
    m_fixChampion = 0;                      // byte(0x36e)=0
    m_str370.clear();                       // Ss::clear(+0xdc)
    m_visibleMinimap = 1;                   // param_1[0xdd]=1
    m_visibleTownMinimap = 1;               // param_1[0xde]=1
}

// ORIG ScanChampionFeatures(int, STAICharacterMapArrangeData&) @0x089e6dc4
//（同表匹配，push 到 +0x1c vector<int>）
static void ScanAiChampionFeatures(int count, STAICharacterMapArrangeData& ai)
{
    std::string tok;
    for (int i = 0; i < count; ++i)
    {
        ScanStr(&tok);
        for (int k = 0; k < 37; ++k)
        {
            if (tok == CHAMPION_FEATURE_NAME[CHAMPION_FEATURE_LIST[k] & 0xff])
            {
                ai.m_championFeatures.push_back(CHAMPION_FEATURE_LIST[k]);
                break;
            }
        }
    }
}

// ===========================================================================
// ImportMapScript —— ORIG T 0x089dcf54（标签分派逐分支转写；ORIG 的
// static map<const char*,int> 查找表还原为等价 if/else 链）
// ===========================================================================

bool ImportMapScript(STMapScript* script, const char* path)
{
    if (!loadRDARScriptFile(g_szMapScriptBaseDirectory, path))
        return false;
    script->Clear();

    // getDirectoryPath(m_str1a0, "%s%s", g_szMapScriptBaseDirectory, path)
    script->m_str1a0.assign(g_szMapScriptBaseDirectory);
    script->m_str1a0.append(path);

    int normalMonsterCount = 0;     // iStack_140（[monster] 计数）
    bool pvpPracticeRead = false;   // cStack_139

    std::string tok;                // iStack_430 主循环 token
    std::string sub;                // iStack_434 分支内子 token
    bool ok = false;
    for (;;)
    {
        if (!ScanType(tok, true))
            break;

        if (tok == "[map name]")                            // 0x089dd0c1
        {
            ScanStr(&script->m_str1b0);
            continue;
        }
        if (tok == "[player number]")                       // 0x089dd10e
        {
            script->m_playerCountMin = ScanInt(&ok);
            script->m_playerCountMax = ScanInt(&ok);
            continue;
        }
        if (tok == "[show dust]")                           // 0x089dd157
        {
            ScanStr(&sub);
            script->m_str1a4.append(sub);                   // Ss::append(+0x69)
            ScanStr(&sub);
            script->m_str1a8.append(sub);                   // Ss::append(+0x6a)
            continue;
        }
        if (tok == "[sort bottom]")                         // 0x089dd1c8
        {
            script->m_sortBottom = ScanInt(&ok) == 1;
            continue;
        }
        if (tok == "[pvp start area]")                      // 0x089dd202
        {
            for (int i = 0; i < 3; ++i)
            {
                script->m_pvpStartArea[i].m_x = ScanInt(&ok);
                script->m_pvpStartArea[i].m_y = ScanInt(&ok);
                script->m_pvpStartArea[i].m_w = ScanInt(&ok);
                script->m_pvpStartArea[i].m_h = ScanInt(&ok);
            }
            continue;
        }
        if (tok == "[pvp start area by party slot]")        // 0x089dd309
        {
            script->m_pvpByPartySlot = ScanInt(&ok) != 0;
            continue;
        }
        if (tok == "[map move keep state axis pos]")        // 0x089dd34d
        {
            script->m_keepStateAxisX = ScanInt(&ok) != 0;
            script->m_keepStateAxisY = ScanInt(&ok) != 0;
            script->m_keepStateAxisZ = ScanInt(&ok) != 0;
            continue;
        }
        if (tok == "[pvp practice start area]")             // 0x089dd3db
        {
            for (int i = 3; i < 5; ++i)
            {
                script->m_pvpStartArea[i].m_x = ScanInt(&ok);
                script->m_pvpStartArea[i].m_y = ScanInt(&ok);
                script->m_pvpStartArea[i].m_w = ScanInt(&ok);
                script->m_pvpStartArea[i].m_h = ScanInt(&ok);
            }
            pvpPracticeRead = true;                         // cStack_139=1
            continue;
        }
        if (tok == "[shadow type]")                         // 0x089dd4e9
        {
            ScanStr(&sub);
            // ORIG 仅 [more darker] 写 param_1[0x93]=2；[mirror] 不写
            //（保持 Clear 值，Export 侧以 0 呈现 mirror 分支）。
            if (sub == "[more darker]")
                script->m_24c = 2;
            continue;
        }
        if (tok == "[dungeon]")                             // 0x089dd56d
        {
            script->m_list4.clear();
            for (;;)
            {
                int v = ScanInt(&ok);
                if (!ok)
                    break;
                script->m_list4.push_back(v);
            }
            continue;
        }
        if (tok == "[type]")                                // 0x089dd5d7
        {
            ScanStr(&sub);
            if (sub == "[normal]")
                script->m_c = 0;
            else if (sub == "[boss]")
                script->m_c = 1;
            else if (sub == "[dummy]")
                script->m_c = 2;
            continue;
        }
        if (tok == "[background pos]")                      // 0x089dd682
        {
            script->m_32c = ScanInt(&ok);                   // param_1[0xcb]
            continue;
        }
        if (tok == "[greed]")                               // 0x089dd6b6
        {
            script->m_recoveryObjects.clear();
            ScanStr(&sub);
            size_t len = sub.size();
            int pending = -1;                               // iStack_4b8
            size_t pos = 0;                                 // iStack_118
            while (pos + 1 < len)
            {
                int sym = -1;                               // iStack_4c0
                for (int k = 0; k < 16; ++k)
                {
                    if (sub[pos] == g_gridSymbols[k][0] &&
                        sub[pos + 1] == g_gridSymbols[k][1])
                    {
                        sym = k;
                        break;
                    }
                }
                if (sym < 0)
                {
                    pos += 1;
                    continue;
                }
                if (pending < 0)
                    pending = sym;
                else
                {
                    script->m_recoveryObjects.push_back(
                        std::make_pair(pending, sym));
                    pending = -1;
                }
                pos += 2;
            }
            continue;
        }
        if (tok == "[tile]")                                // 0x089dd90x 区
        {
            script->m_vecTile.clear();
            for (;;)
            {
                std::string tileName;
                if (!ScanStr(&tileName))
                    break;
                script->m_vecTile.push_back(tileName);
            }
            continue;
        }
        if (tok == "[extended tile]")
        {
            script->m_vecExtendedTile.clear();
            for (;;)
            {
                std::string tileName;
                if (!ScanStr(&tileName))
                    break;
                script->m_vecExtendedTile.push_back(tileName);
            }
            continue;
        }
        if (tok == "[elemental property]")                  // 0x089ddaef 区
        {
            ScanStr(&sub);
            script->m_element = getElement(sub.c_str());    // param_1[0x1b]
            continue;
        }
        if (tok == "[far sight scroll]")
        {
            script->m_sightScroll[0] = ScanInt(&ok);        // param_1[0x22]
            continue;
        }
        if (tok == "[middle sight scroll]")
        {
            script->m_sightScroll[1] = ScanInt(&ok);        // param_1[0x23]
            continue;
        }
        if (tok == "[near sight scroll]")
        {
            script->m_sightScroll[2] = ScanInt(&ok);        // param_1[0x24]
            continue;
        }
        if (tok == "[town movable area]")                   // 0x089dbc5 区
        {
            script->m_vecTownMovableArea.clear();
            for (;;)
            {
                int first = ScanInt(&ok);
                if (!ok)
                    break;
                TownPath tp;                                // TownPath::TownPath
                tp.m_x = first;
                tp.m_y = ScanInt(&ok);
                tp.m_w = ScanInt(&ok);
                tp.m_h = ScanInt(&ok);
                tp.m_field1c = ScanInt(&ok);
                tp.m_field20 = ScanInt(&ok);
                {                                           // 可选 [quest condition]
                    bool isType = false;
                    std::string s;
                    bool isInt = false;
                    ScanTypeOrInt(isType, s, isInt, true);
                    if (isType && s == "[quest condition]")
                    {
                        for (;;)
                        {
                            int cond = ScanInt(&ok);
                            if (!ok)
                                break;
                            tp.m_conds.push_back(cond);
                        }
                    }
                }
                script->m_vecTownMovableArea.push_back(tp);
            }
            continue;
        }
        if (tok == "[virtual movable area]")
        {
            script->m_vecVirtualMovableArea.clear();
            for (;;)
            {
                int first = ScanInt(&ok);
                if (!ok)
                    break;
                MapArea area;                               // MapArea::MapArea
                area.m_x = first;
                area.m_y = ScanInt(&ok);
                area.m_w = ScanInt(&ok);
                area.m_h = ScanInt(&ok);
                {                                           // 可选 [quest condition]
                    bool isType = false;
                    std::string s;
                    bool isInt = false;
                    ScanTypeOrInt(isType, s, isInt, true);
                    if (isType && s == "[quest condition]")
                    {
                        for (;;)
                        {
                            int cond = ScanInt(&ok);
                            if (!ok)
                                break;
                            area.m_list.push_back(cond);
                        }
                    }
                }
                script->m_vecVirtualMovableArea.push_back(area);
            }
            continue;
        }
        if (tok == "[guild agit entrance info]")
        {
            script->m_vecGuildAgitEntrance.clear();
            for (;;)
            {
                int first = ScanInt(&ok);
                if (!ok)
                    break;
                TownPath tp;
                tp.m_x = first;
                tp.m_y = ScanInt(&ok);
                tp.m_w = ScanInt(&ok);
                tp.m_h = ScanInt(&ok);
                tp.m_field1c = ScanInt(&ok);
                tp.m_field20 = ScanInt(&ok);
                script->m_vecGuildAgitEntrance.push_back(tp);
            }
            continue;
        }
        if (tok == "[pathgate pos]")
        {
            for (int i = 0; i < 4; ++i)
                for (int j = 0; j < 2; ++j)
                    script->m_pathgatePos[i * 2 + j] = ScanInt(&ok);
            continue;
        }
        if (tok == "[pathgate recognize range]")
        {
            for (int i = 0; i < 4; ++i)
                for (int j = 0; j < 2; ++j)
                    script->m_pathgateRange[i * 2 + j] = ScanInt(&ok);
            continue;
        }
        if (tok == "[dungeon start area]")
        {
            script->m_dungeonStartArea.m_x = ScanInt(&ok);
            script->m_dungeonStartArea.m_y = ScanInt(&ok);
            script->m_dungeonStartArea.m_w = ScanInt(&ok);
            script->m_dungeonStartArea.m_h = ScanInt(&ok);
            continue;
        }
        if (tok == "[opening bgm]")
        {
            ScanStr(&sub);
            script->m_openingBgm = sub;                     // Ss::assign(RKSs)
            script->m_openingBgmParam = ScanInt(&ok);       // param_1[100]
            continue;
        }
        if (tok == "[sound]")
        {
            script->m_vecSound.clear();
            for (;;)
            {
                std::string soundName;
                if (!ScanStr(&soundName))
                    break;
                script->m_vecSound.push_back(soundName);
            }
            continue;
        }
        if (tok == "[animation]")
        {
            script->m_listAnimation.clear();
            for (;;)
            {
                STAssignAnimation ani;                      // 局部对象循环复用
                if (!ScanStr(&ani.m_name))
                    break;
                ScanStr(&sub);
                ani.m_layer = getDrawLayer(sub.c_str());
                ani.m_field08 = ScanInt(&ok);
                ani.m_field0c = ScanInt(&ok);
                ani.m_field10 = ScanInt(&ok);
                script->m_listAnimation.push_back(ani);
            }
            continue;
        }
        if (tok == "[passive object]")
        {
            script->m_listPassiveObject.clear();
            for (;;)
            {
                int id = ScanInt(&ok);
                if (!ok)
                    break;
                STAssignPassiveObject obj;                  // 0x10 四字段
                obj.m_0 = id;
                obj.m_4 = ScanInt(&ok);
                obj.m_8 = ScanInt(&ok);
                obj.m_c = ScanInt(&ok);
                script->m_listPassiveObject.push_back(obj);
            }
            continue;
        }
        if (tok == "[monster]" || tok == "[monster condition]")  // code_r0x089deeb7
        {
            if (tok != "[monster]")
                script->m_vecConditionMonster.clear();      // vector(+0x40)
            else
                script->m_vecMonster.clear();               // vector(+0x34)
            for (;;)
            {
                int mobId = ScanInt(&ok);
                if (!ok)
                    break;
                STAssignMonster mon;                        // STAssignMonster::Clear
                mon.Clear();
                mon.m_0 = mobId;
                static int named_count = 0;                 // 函数级静态校验计数
                if ((mobId > 49999 && mobId < 0xc739) ||
                    (mobId > 55999 && mobId < 0xea61))
                {
                    if (++named_count > 6000)
                        return false;
                }
                mon.m_4 = static_cast<char>(ScanInt(&ok) != 0);
                mon.m_8 = ScanInt(&ok);
                mon.m_c = ScanInt(&ok);
                mon.m_10 = ScanInt(&ok);
                mon.m_14 = ScanInt(&ok);
                mon.m_18 = ScanInt(&ok);
                mon.m_1c = ScanInt(&ok);
                ScanStr(&sub);
                if (sub == "[fixed]")                       // uStack_750
                    mon.m_20 = 0;
                else if (sub == "[half]")
                    mon.m_20 = 1;
                else if (sub == "[ratio]")
                    mon.m_20 = 2;
                else
                    mon.m_20 = 0;
                ScanStr(&sub);
                if (sub == "[NPC]")                         // NPC 携带 idx→+0x58
                {
                    mon.m_58 = ScanInt(&ok);
                    ScanStr(&sub);
                }
                if (sub == "[normal]")                      // uStack_74c
                    mon.m_24 = 0;
                else if (sub == "[champion]")
                {
                    mon.m_24 = 1;
                    int cnt = ScanInt(&ok);
                    if (cnt < 1)
                        mon.m_vec28.clear();
                    else
                        ScanChampionFeatures(cnt, mon);
                }
                else if (sub == "[super champion]")
                {
                    mon.m_24 = 2;
                    int cnt = ScanInt(&ok);
                    if (cnt < 1)
                        mon.m_vec28.clear();
                    else
                        ScanChampionFeatures(cnt, mon);
                }
                else if (sub == "[boss]")
                {
                    static int boss_count = 0;
                    if (++boss_count > 0x5dc)
                        return false;
                    mon.m_24 = 3;
                }
                else
                    mon.m_24 = 0;
                if (tok == "[monster]")
                {
                    script->m_vecMonster.push_back(mon);
                    ++normalMonsterCount;                   // iStack_140++
                }
                else
                    script->m_vecConditionMonster.push_back(mon);
            }
            continue;
        }
        if (tok == "[blood monster]" || tok == "[ultimate monster]")  // code_r0x089df2ff
        {
            script->m_vecBloodSystem.clear();
            for (;;)
            {
                int id = ScanInt(&ok);
                if (!ok)
                    break;
                STAssignBloodSystem sys;                    // ::Clear
                sys.Clear();
                sys.m_id = id;
                sys.m_field04 = ScanInt(&ok);
                sys.m_phase = ScanInt(&ok);
                sys.m_field0c = ScanInt(&ok);
                sys.m_field10 = ScanInt(&ok);
                sys.m_timeA = ScanInt(&ok);
                sys.m_timeB = ScanInt(&ok);
                sys.m_startCnt = ScanInt(&ok);
                sys.m_cntInc = ScanInt(&ok);
                sys.m_totalCnt = ScanInt(&ok);
                printf("\n(%s) [blood monster](%d) GROUP : MATCHING TIME ERROR!!\n",
                       path, id);
                script->m_vecBloodSystem.push_back(sys);
            }
            continue;
        }
        if (tok == "[blood phase time]" || tok == "[ultimate phase time]")  // code_r0x089df4b2
        {
            script->m_vecBloodPhaseTime.clear();
            for (;;)
            {
                int order = ScanInt(&ok);
                if (!ok)
                    break;
                STAssignBloodPhaseTime pt;                  // ::Clear
                pt.Clear();
                pt.m_smallRoundOrder = order;
                pt.m_phaseIdx = ScanInt(&ok);
                pt.m_interval = ScanInt(&ok);
                pt.m_time = ScanFloat(static_cast<bool*>(0));
                pt.m_field10 = ScanInt(&ok);
                pt.m_spawnCnt = ScanInt(&ok);
                {                                           // A..E 难度名 → difficulty
                    std::string diffTok;
                    if (!ScanType(diffTok, true))
                        return false;
                    if (diffTok == "A")      pt.m_difficulty = 1;
                    else if (diffTok == "B") pt.m_difficulty = 2;
                    else if (diffTok == "C") pt.m_difficulty = 3;
                    else if (diffTok == "D") pt.m_difficulty = 4;
                    else if (diffTok == "E") pt.m_difficulty = 5;
                    else                     return false;
                }
                script->m_vecBloodPhaseTime.push_back(pt);
            }
            continue;
        }
        if (tok == "[time line]")                           // call 0x0889a930
        {
            importTimeLinePhases(&script->m_timeLine);
            continue;
        }
        if (tok == "[defense monster]")
        {
            script->m_vecDefenseSystem.clear();
            for (;;)
            {
                int id = ScanInt(&ok);
                if (!ok)
                    break;
                STAssignDefenseSystem sys;                  // ::Clear
                sys.Clear();
                sys.m_id = id;
                sys.m_field04 = ScanInt(&ok);
                sys.m_phase = ScanInt(&ok);
                sys.m_field0c = ScanInt(&ok);
                sys.m_field10 = ScanInt(&ok);
                sys.m_timeA = ScanInt(&ok);
                sys.m_timeB = ScanInt(&ok);
                sys.m_startCnt = ScanInt(&ok);
                sys.m_cntInc = ScanInt(&ok);
                sys.m_totalCnt = ScanInt(&ok);
                script->m_vecDefenseSystem.push_back(sys);
            }
            continue;
        }
        if (tok == "[defense phase time]")
        {
            script->m_vecDefensePhaseTime.clear();
            for (;;)
            {
                int order = ScanInt(&ok);
                if (!ok)
                    break;
                STAssignDefensePhaseTime pt;                // ::Clear
                pt.Clear();
                pt.m_smallRoundOrder = order;
                pt.m_phaseIdx = ScanInt(&ok);
                pt.m_interval = ScanInt(&ok);
                pt.m_time = ScanFloat(static_cast<bool*>(0));
                pt.m_field10 = ScanInt(&ok);
                pt.m_spawnCnt = ScanInt(&ok);
                script->m_vecDefensePhaseTime.push_back(pt);
            }
            continue;
        }
        if (tok == "[summon start area]")
        {
            script->m_summonStartArea.m_x = ScanInt(&ok);
            script->m_summonStartArea.m_y = ScanInt(&ok);
            script->m_summonStartArea.m_w = ScanInt(&ok);
            script->m_summonStartArea.m_h = ScanInt(&ok);
            continue;
        }
        if (tok == "[camera edge recovery object]")         // 0x089e036c
        {
            for (;;)
            {
                int first = ScanInt(&ok);
                if (!ok)
                    break;
                int second = ScanInt(&ok);
                if (!ok)
                    break;
                script->m_vec180.push_back(std::make_pair(first, second));
            }
            continue;
        }
        if (tok == "[heroes mode map index]")               // 0x089e0435
        {
            script->m_maxMonsterDropException = ScanInt(&ok);
            continue;
        }
        if (tok == "[monster team]")                        // 0x089e049f 区
        {
            for (size_t i = 0;
                 i < static_cast<size_t>(normalMonsterCount) &&
                 i < script->m_vecMonster.size(); ++i)
                script->m_vecMonster[i].m_4c = ScanInt(&ok);
            ScanStr(&sub);
            continue;
        }
        if (tok == "[monster specific AI]")                 // 0x089e050f 区
        {
            for (size_t i = 0;
                 i < static_cast<size_t>(normalMonsterCount) &&
                 i < script->m_vecMonster.size(); ++i)
            {
                if (!ScanStr(&sub))
                    break;
                if (sub == "[normal]")
                    script->m_vecMonster[i].m_34 = 0;
                else if (sub == "[zone defence]")
                {
                    script->m_vecMonster[i].m_34 = 1;
                    script->m_vecMonster[i].m_38 = ScanInt(&ok);
                    script->m_vecMonster[i].m_3c = ScanInt(&ok);
                }
                else if (sub == "[roaming]")
                {
                    script->m_vecMonster[i].m_34 = 2;
                    int cnt = ScanInt(&ok);
                    for (int k = 0; k < cnt; ++k)
                    {
                        std::pair<int, int> way;
                        way.first = ScanInt(&ok);
                        way.second = ScanInt(&ok);
                        script->m_vecMonster[i].m_vec40.push_back(way);
                    }
                }
            }
            ScanStr(&sub);
            continue;
        }
        if (tok == "[monster collision group]")             // 0x089e0660 区
        {
            for (size_t i = 0; i < script->m_vecMonster.size(); ++i)
            {
                script->m_vecMonster[i].m_vec5c.clear();
                int cnt = ScanInt(&ok);
                for (int k = 0; k < cnt; ++k)
                    script->m_vecMonster[i].m_vec5c.push_back(
                        static_cast<unsigned char>(ScanInt(&ok)));
            }
            ScanType(tok, true);                            // 收尾 consume 标记
            continue;
        }
        if (tok == "[apc collision group]")                 // 0x089e07f0 区
        {
            for (size_t i = 0; i < script->m_vecAICharacter.size(); ++i)
            {
                script->m_vecAICharacter[i].m_collisionGroup.clear();
                int cnt = ScanInt(&ok);
                for (int k = 0; k < cnt; ++k)
                    script->m_vecAICharacter[i].m_collisionGroup.push_back(
                        static_cast<unsigned char>(ScanInt(&ok)));
            }
            ScanType(tok, true);
            continue;
        }
        if (tok == "[champion]")                            // 0x089e097b
        {
            script->m_championRate = ScanInt(&ok);          // param_1[0x13]
            continue;
        }
        if (tok == "[special passive object]")              // 0x089e099c 区
        {
            script->m_listSpecialPassive.clear();
            for (;;)
            {
                int id = ScanInt(&ok);
                if (!ok)
                    break;
                STAssignSpecialPassiveObj spo;              // ::Clear
                spo.Clear();
                spo.m_0 = id;
                spo.m_4 = ScanInt(&ok);
                spo.m_8 = ScanInt(&ok);
                spo.m_c = ScanInt(&ok);
                int actionCount = ScanInt(&ok);
                for (int k = 0; k < actionCount; ++k)
                {
                    SPOBJ_ACTION_DATAS action;              // ctor 清零
                    memset(&action, 0, sizeof(action));
                    procSpecialPassiveObject(action, spo,
                                             findMapScriptIndex(path));
                    if (action.m_type != 5)
                        spo.m_vec10.push_back(action);
                }
                script->m_listSpecialPassive.push_back(spo);
            }
            continue;
        }
        if (tok == "[NPC]")                                 // 0x089e0ba1
        {
            script->m_vecNpc.clear();
            for (;;)
            {
                int npcIdx = ScanInt(&ok);
                if (!ok)
                    break;
                AssignNpc npc;
                npc.m_npcIdx = npcIdx;
                bool wasStr = true;                         // cStack_55d
                std::string nameTok;                        // iStack_438
                bool isInt = false;
                int second = ScanStrOrInt(wasStr, nameTok, isInt, true);
                if (!ok)
                    break;
                if (!wasStr)                                // 第二参为 int
                {
                    npc.m_field04 = second;                 // iStack_47c
                    ScanStr(&sub);                          // 子 token
                }
                else
                {
                    sub = nameTok;
                }
                if (sub == "[visible on dungeon clear]")
                {
                    npc.m_visible = true;                   // uStack_478
                    ScanStr(&sub);
                }
                if (sub == "[left]")
                    npc.m_dir = 0;                          // uStack_474=0
                else if (sub == "[right]")
                    npc.m_dir = 1;
                npc.m_x = ScanInt(&ok);
                npc.m_y = ScanInt(&ok);
                npc.m_z = ScanInt(&ok);
                script->m_vecNpc.push_back(npc);
            }
            continue;
        }
        if (tok == "[monster spawn pos]")                   // 0x089e0d43
        {
            script->m_vecMonsterSpawnPos.clear();
            int count = ScanInt(&ok);
            STRivalMapData proto;                           // STRivalMapData ctor
            script->m_vecMonsterSpawnPos.reserve(count);
            for (int i = 0; i < count; ++i)
            {
                STRivalMapData data = proto;
                data.m_field00 = ScanInt(&ok);
                data.m_field04 = ScanInt(&ok);
                data.m_field08 = ScanInt(&ok);
                script->m_vecMonsterSpawnPos.push_back(data);
            }
            continue;
        }
        if (tok == "[ai character]")                        // 0x089e0e00 区
        {
            for (;;)
            {
                int type = ScanInt(&ok);                    // iStack_48
                if (!ok)
                    break;
                STAICharacterMapArrangeData ai;             // ctor（含 vector 成员）
                ai.m_type = type;
                ai.m_field04 = ScanInt(&ok);
                ai.m_field08 = ScanInt(&ok);
                ai.m_field0c = ScanInt(&ok);
                {                                           // 阵营：character/monster/neutral
                    bool isType = false;
                    std::string camp;
                    bool consumed = false;
                    ScanTypeOrStr(isType, camp, consumed);
                    if (!isType)
                        ai.m_camp = 0;
                    else if (camp == "[neutral]")
                        ai.m_camp = 200;
                    else
                        ai.m_camp = 100;
                }
                bool hasTok = ScanStr(&sub);                // cStack_41
                if (hasTok && sub == "[NPC]")               // uStack_6e0→+0x28
                {
                    ai.m_npcIdx = ScanInt(&ok);
                    hasTok = ScanStr(&sub);
                }
                if (!hasTok)
                {
                    ai.m_roleType = 5;                      // 缺省 normal
                }
                else if (sub == "[champion]")               // uStack_6f4
                {
                    ai.m_roleType = 6;
                    int cnt = ScanInt(&ok);
                    if (cnt < 1)
                        ai.m_championFeatures.clear();
                    else
                        ScanAiChampionFeatures(cnt, ai);
                }
                else if (sub == "[super champion]")
                {
                    ai.m_roleType = 7;
                    int cnt = ScanInt(&ok);
                    if (cnt < 1)
                        ai.m_championFeatures.clear();
                    else
                        ScanAiChampionFeatures(cnt, ai);
                }
                else if (sub == "[boss]")
                {
                    static int apc_boss_count = 0;
                    if (++apc_boss_count >= 0x5dd)
                        return false;
                    ai.m_roleType = 8;
                }
                else                                        // normal 等
                {
                    ai.m_roleType = 5;
                }
                {                                           // 尾部两 int → 坐标字节
                    int x = ScanInt(&ok);
                    if (!ok)
                        return false;
                    int y = ScanInt(&ok);
                    if (!ok)
                        return false;
                    ai.m_posX = static_cast<char>(x);
                    ai.m_posY = static_cast<char>(y);
                }
                script->m_vecAICharacter.push_back(ai);
            }
            continue;
        }
        if (tok == "[apc random point]")                    // 0x089e11c8
        {
            script->m_apcRandomPoint = ScanInt(&ok);        // param_1[0x9a]
            continue;
        }
        if (tok == "[event monster position]")              // 0x089e11ec
        {
            STRivalMapData proto;
            for (;;)
            {
                STRivalMapData data = proto;
                data.m_field00 = ScanInt(&ok);
                if (!ok)
                    break;
                data.m_field04 = ScanInt(&ok);
                if (!ok)
                    return false;
                data.m_field08 = ScanInt(&ok);
                if (!ok)
                    return false;
                script->m_vecEventMonsterPos.push_back(data);
            }
            continue;
        }
        if (tok == "[map loading image path]")              // 0x089e12cc
        {
            ScanStr(&script->m_mapLoadingImagePath);        // param_1+0x9b
            continue;
        }
        if (tok == "[ani info]" || tok == "[background animation]")  // lookup 主表
        {
            BackAniScript ani;                              // BackAniScript::clear
            ani.clear();
            bool done = false;
            while (!done)
            {
                std::string subTok;
                if (!ScanType(subTok, true))
                    return false;
                if (subTok == "[filename]")                 // lookup_0=0x1d7
                {
                    std::string name;
                    if (!ScanStr(&name))
                        return false;
                    ani.m_filename = name;                  // Ss::assign(RKSs)
                }
                else if (subTok == "[order]")               // 0x1e3
                {
                    std::string order;
                    if (!ScanStr(&order))
                        return false;
                    ani.m_orderUpper = (order == "[upper]");// [below]=0/[upper]=1
                }
                else if (subTok == "[loop y axis]")         // 0x1ee
                {
                    int flag = -1;
                    if (!ScanInt(&flag))
                        return false;
                    ani.m_loopYAxis = (flag != 0);          // uStack_4cb
                }
                else if (subTok == "[screen pos]")          // 0x1f8
                {
                    int sx = -1;
                    if (!ScanInt(&sx))
                        return false;
                    ani.m_screenX = sx;                     // uStack_4c8
                    int sy = -1;
                    if (!ScanInt(&sy))
                        return false;
                    ani.m_screenY = sy;                     // uStack_4c4
                    ani.m_hasScreenPos = true;              // uStack_4ca
                }
                else if (subTok == "[/ani info]")           // 0x205
                {
                    script->m_vecBackAni.push_back(ani);    // vector(+0xa0)
                    done = true;
                }
            }
            continue;
        }
        if (tok == "[quest animation]")
        {
            script->m_listQuestAnimation.clear();
            for (;;)
            {
                STQuestAnimation qani;
                if (!ScanStr(&qani.m_name))
                    break;
                ScanStr(&sub);
                qani.m_layer = getDrawLayer(sub.c_str());
                qani.m_field08 = ScanInt(&ok);
                qani.m_field0c = ScanInt(&ok);
                qani.m_field10 = ScanInt(&ok);
                qani.m_field14 = ScanInt(&ok);
                qani.m_flag18 = ScanInt(&ok) != 0;
                script->m_listQuestAnimation.push_back(qani);
            }
            continue;
        }
        if (tok == "[attacked monster info]")
        {
            script->m_vecAttackedMonster.clear();
            int acc = 0;                                    // iStack_38 累加权重
            for (;;)
            {
                STAttackedMonster mon;
                mon.m_id = static_cast<unsigned short>(ScanInt(&ok));
                if (!ok)
                    break;
                if (!ScanInt(reinterpret_cast<int*>(&mon.m_field04)))
                    return false;
                if (!ScanInt(reinterpret_cast<int*>(&mon.m_field08)))
                    return false;
                if (!ScanInt(reinterpret_cast<int*>(&mon.m_field0c)))
                    return false;
                int w = ScanInt(&ok);
                if (!ok)
                    return false;
                acc += w;
                mon.m_attackWeight = acc;                   // iStack_5a0 = acc
                if (!ScanInt(reinterpret_cast<int*>(&mon.m_field14)))
                    return false;
                if (!ScanInt(reinterpret_cast<int*>(&mon.m_field18)))
                    return false;
                script->m_vecAttackedMonster.push_back(mon);
            }
            continue;
        }
        if (tok == "[max monster drop exception]")          // 0x089e17xx 区
        {
            script->m_maxMonsterCountException = ScanInt(&ok);  // param_1[0xc9]
            continue;
        }
        if (tok == "[max monster count exception]")
        {
            script->m_328 = ScanInt(&ok);                   // param_1[0xca]
            continue;
        }
        if (tok == "[background effect]")                   // 0x089e1640 区
        {
            script->m_vecBackGroundEffect.clear();
            for (;;)
            {
                BackGroundEffect effect;
                effect.clear();
                if (!ScanStr(&sub))
                    break;
                if (sub == "[swing]")                       // type=0 分支
                {
                    effect.m_type = 0;                      // iStack_708=0
                    ScanStr(&sub);
                    effect.m_layer = getDrawLayer(sub.c_str());
                    effect.m_field08 = ScanInt(&ok);
                    effect.m_field0c = ScanInt(&ok);
                    effect.m_field10 = ScanInt(&ok);
                    effect.m_field14 = ScanInt(&ok);
                    effect.m_field18 = ScanInt(&ok);
                    {                                       // 可选动画名 → +0x1c string
                        std::string anim;
                        anim.clear();
                        if (ScanStr(&anim) && !anim.empty())
                            effect.m_animName = anim;
                        // 空串时 assign("")（汇编原序）
                    }
                }
                else if (sub == "[dust]")                   // type=1 分支
                {
                    effect.m_type = 1;
                    ScanStr(&sub);
                    effect.m_layer = getDrawLayer(sub.c_str());
                    effect.m_field0c = ScanInt(&ok);
                    effect.m_field10 = ScanInt(&ok);
                    effect.m_field14 = ScanInt(&ok);
                }
                script->m_vecBackGroundEffect.push_back(effect);
            }
            continue;
        }
        if (tok == "[block effect]")                        // 0x089e18fc 区
        {
            script->m_vecBlockEffect.clear();
            for (;;)
            {
                BackGroundEffectBlock block;
                block.clear();
                if (!ScanStr(&sub))
                    break;
                block.m_type = (sub == "[swing]") ? 0 : 1;  // swing=0/dust=1
                if (!ScanStr(&sub))
                    break;
                if (sub == "[object]")                      // uStack_5c0=0
                {
                    block.m_mode = 0;
                    block.m_val = ScanInt(&ok);
                }
                else if (sub == "[animation]")              // uStack_5c0=1
                {
                    block.m_mode = 1;
                    ScanStr(&block.m_animName);
                }
                script->m_vecBlockEffect.push_back(block);
            }
            continue;
        }
        if (tok == "[absolute start path]")                 // 0x089e19xx 区
        {
            script->m_startPathDir = 4;
            for (;;)
            {
                if (!ScanStr(&sub))
                    break;
                if (sub == "[left]")        script->m_startPathDir = 0;
                else if (sub == "[right]")  script->m_startPathDir = 1;
                else if (sub == "[up]")     script->m_startPathDir = 2;
                else if (sub == "[down]")   script->m_startPathDir = 3;
            }
            continue;
        }
        if (tok == "[block path]")
        {
            for (int i = 0; i < 4; ++i)
            {
                if (!ScanStr(&sub))
                    break;
                script->m_blockPath[i] =
                    static_cast<char>(sub == "[close]");
            }
            continue;
        }
        if (tok == "[tile option]")
        {
            for (;;)
            {
                if (!ScanStr(&sub))
                    break;
                if (sub == "[upper]")
                    script->m_tileOptionUpper = 1;          // byte(param_1+0x36)
            }
            continue;
        }
        if (tok == "[map dialog]")                          // 0x089e1c61
        {
            ScanStr(&sub);
            continue;
        }
        if (tok == "[tournament monster]")                  // 0x089e1e53 区
        {
            std::vector<TournamentMonster> mons;
            int teamCount = ScanInt(static_cast<bool*>(0)); // cStack_5dd
            if (teamCount < 1 || teamCount > 2)
                return false;
            for (;;)
            {
                TournamentMonster mon;                      // ctor
                mon.m_mobId = ScanInt(&ok);
                if (!ok)
                    break;
                mon.m_rate = ScanInt(&ok);
                if (!ok)
                    return false;
                mons.push_back(mon);
            }
            if (!script->m_tournamentMonsters.insert(
                    std::make_pair(
                        static_cast<char>(teamCount), mons)).second)
            {
                LogManager::logFormat(0, "../RDARScriptMap.cpp", __FUNCTION__,
                                      0x84f,
                                      "[tournament monster] map index(%d) is duplicated",
                                      teamCount);
                return false;
            }
            continue;
        }
        if (tok == "[tournament start area]")               // 0x089e1f00 区
        {
            TournamentStartPos pos;
            int teamCount = ScanInt(static_cast<bool*>(0)); // cStack_2e 校验
            if (teamCount < 1 || teamCount > 2)
                return false;
            pos.m_slot = teamCount;
            pos.m_x = ScanInt(&ok);
            if (!ok)
                return false;
            pos.m_y = ScanInt(&ok);
            if (!ok)
                return false;
            int slot = ScanInt(static_cast<bool*>(0));      // cStack_2d
            if (slot < 1 || slot > 2)
                return false;
            pos.m_slot = slot;
            if (slot == 1)
                script->m_tournamentStartPos1.push_back(pos);   // vector@0xb4
            else
                script->m_tournamentStartPos2.push_back(pos);   // vector@0xb7
            continue;
        }
        if (tok == "[move layered map]")                    // 0x089e2040 区
        {
            for (;;)
            {
                std::string subTok;
                if (!ScanType(subTok, true))
                    break;
                if (subTok == "[zpos under]")               // lookup_1=0x8b8
                {
                    script->m_moveLayeredType = 1;          // param_1[0xd4]=1
                    script->m_zposUnder = ScanInt(&ok);     // param_1[0xd5]
                }
                else if (subTok == "[all dead case passable]")  // 0x8be
                {
                    script->m_allDeadCasePassable = 1;      // byte @0xd6
                }
                else if (subTok == "[move layered map]" ||
                         subTok == "[/move layered map]")   // 0x8c3 / 结束
                {
                    break;
                }
            }
            continue;
        }
        if (tok == "[static player start pos]")             // 0x089e21xx 区
        {
            for (;;)
            {
                std::string subTok;
                if (!ScanType(subTok, true))
                    break;
                if (subTok == "[/static player start pos]")
                    break;
                int playerIdx = -1;                         // lookup_2
                if (subTok == "[player 1]")      playerIdx = 0;
                else if (subTok == "[player 2]") playerIdx = 1;
                else if (subTok == "[player 3]") playerIdx = 2;
                else if (subTok == "[player 4]") playerIdx = 3;
                if (playerIdx < 0)
                    continue;
                StaticStartPos pos;
                pos.m_playerIdx = playerIdx;                // uStack_608→+0xc
                if (!ScanInt(&pos.m_x)) return false;
                if (!ScanInt(&pos.m_y)) return false;
                if (!ScanInt(&pos.m_z)) return false;
                script->m_vecStaticStartPos.push_back(pos); // vector(+0xba)
            }
            continue;
        }
        if (tok == "[limit player move area]")              // 0x089e2c6c 区
        {
            for (;;)
            {
                std::string subTok;
                if (!ScanType(subTok, true))
                    break;
                if (subTok == "[/limit player move area]")
                    break;
                int playerIdx = -1;                         // lookup_3
                if (subTok == "[player 1]")      playerIdx = 0;
                else if (subTok == "[player 2]") playerIdx = 1;
                else if (subTok == "[player 3]") playerIdx = 2;
                else if (subTok == "[player 4]") playerIdx = 3;
                if (playerIdx < 0)
                    continue;
                MapArea area;                               // MapArea ctor + clear
                if (!ScanInt(&area.m_x)) return false;
                if (!ScanInt(&area.m_y)) return false;
                if (!ScanInt(&area.m_w)) return false;
                if (!ScanInt(&area.m_h)) return false;
                std::map<int, MapArea>::iterator it =
                    script->m_limitPlayerMoveAreas.find(playerIdx);
                if (it == script->m_limitPlayerMoveAreas.end())
                    script->m_limitPlayerMoveAreas.insert(
                        std::make_pair(playerIdx, area));   // make_pair(idx, area)
            }
            continue;
        }
        if (tok == "[customized screen edge]")              // 0x089e33xx 区
        {
            for (;;)
            {
                std::string subTok;
                if (!ScanType(subTok, true))
                    break;
                if (subTok == "[left]")                     // lookup_4=0x946
                {
                    if (!ScanInt(&script->m_screenEdgeLeft)) return false;
                }
                else if (subTok == "[right]")
                {
                    if (!ScanInt(&script->m_screenEdgeRight)) return false;
                }
                else if (subTok == "[top]")                 // 0x958
                {
                    if (!ScanInt(&script->m_screenEdgeTop)) return false;
                }
                else if (subTok == "[bottom]")              // 0x961
                {
                    if (!ScanInt(&script->m_screenEdgeBottom)) return false;
                }
                else if (subTok == "[customized screen edge]" ||
                         subTok == "[/customized screen edge]")
                {
                    break;
                }
            }
            continue;
        }
        if (tok == "[block use active skill]")              // else 链最外层
        {
            script->m_blockUseActiveSkill = 1;              // byte @0xdb
            continue;
        }
        if (tok == "[block use stackable item]")
        {
            script->m_blockUseStackableItem = 1;            // byte 0x36d
            continue;
        }
        if (tok == "[fix champion]")
        {
            script->m_fixChampion = 1;                      // byte 0x36e
            continue;
        }
        if (tok == "[basic action]")
        {
            ScanStr(&sub);
            continue;
        }
        if (tok == "[visible minimap]")
        {
            int v = 0;
            if (!ScanInt(&v))
                return false;
            script->m_visibleMinimap = v;                   // param_1[0xdd]
            continue;
        }
        if (tok == "[visible town minimap]")
        {
            int v = 0;
            if (!ScanInt(&v))
                return false;
            script->m_visibleTownMinimap = v;               // param_1[0xde]
            continue;
        }
        if (tok == "[dialog]")                              // 0x089e1c73 区
        {
            STDialog dlg;                                   // STDialog ctor
            if (!ScanStr(&dlg.m_name))
                return false;
            std::string when;                               // auStack_5d0
            if (!ScanStr(&when))
                return false;
            if (when == "on start")
            {
                dlg.m_onStart = true;                       // uStack_5c8
                script->m_vecDialog.push_back(dlg);         // vector(+0xc4)
            }
            else if (when == "on complete")
            {
                dlg.m_onComplete = true;                    // uStack_5c7
                script->m_vecDialog.push_back(dlg);
            }
            else
            {
                LogManager::logFormat(0, "../RDARScriptMap.cpp", __FUNCTION__,
                                      0x84f,
                                      "Error on scanning tower dialog. There is no match string %s",
                                      when.c_str());
            }
            continue;
        }
        // 未识别 token：继续扫描（ORIG 无 default 分支动作）
    }

    // ---- 收尾（code_r0x089e3a93）----
    if (!pvpPracticeRead)
    {
        // words: [0x84..0x87]=[0x76..0x79]、[0x8b..0x8e]=[0x7d..0x80]
        // 即 MapArea[3].xywh = MapArea[1].xywh、MapArea[4].xywh = MapArea[2].xywh
        script->m_pvpStartArea[3].m_x = script->m_pvpStartArea[1].m_x;
        script->m_pvpStartArea[3].m_y = script->m_pvpStartArea[1].m_y;
        script->m_pvpStartArea[3].m_w = script->m_pvpStartArea[1].m_w;
        script->m_pvpStartArea[3].m_h = script->m_pvpStartArea[1].m_h;
        script->m_pvpStartArea[4].m_x = script->m_pvpStartArea[2].m_x;
        script->m_pvpStartArea[4].m_y = script->m_pvpStartArea[2].m_y;
        script->m_pvpStartArea[4].m_w = script->m_pvpStartArea[2].m_w;
        script->m_pvpStartArea[4].m_h = script->m_pvpStartArea[2].m_h;
    }
    script->m_index = findMapScriptIndex(path);             // *param_1 = 文件表索引
    return true;
}

// ===========================================================================
// ExportMapScript —— ORIG T 0x089e3c55（全段落输出转写；ORIG 以 fopen("wb") +
// fprintf 生成中间文本，收尾 convertBinaryToText 转最终脚本）
// ===========================================================================

bool ExportMapScript(STMapScript* script, const char* path)
{
    char szSaveFileName[0x100];                 // 函数级静态 szSaveFileName
    getScriptFileFullPath(szSaveFileName, 0x100,
                          g_szMapScriptBaseDirectory, path);
    FILE* file = fopen(szSaveFileName, "wb");
    if (!file)
        return false;

    STMapScript defaults;                       // auStack_48c：缺省值对照

    if (!script->m_str1b0.empty())              // +0x1b0 非空才输出 [map name]
        fprintf(file, "[map name] `%s`\n", script->m_str1b0.c_str());
    fprintf(file, "[player number] %d %d\n",
            script->m_playerCountMin, script->m_playerCountMax);
    fwrite("[pvp start area]\n", 1, 17, file);
    for (int i = 0; i < 3; ++i)                 // words 0x6f..0x72 步进 7
        fprintf(file, "%d %d %d %d\n",
                script->m_pvpStartArea[i].m_x,
                script->m_pvpStartArea[i].m_y,
                script->m_pvpStartArea[i].m_w,
                script->m_pvpStartArea[i].m_h);
    {                                           // practice 区（3/4）任一非零才输出
        bool any = false;
        for (int i = 3; i < 5; ++i)
            if (script->m_pvpStartArea[i].m_x || script->m_pvpStartArea[i].m_y ||
                script->m_pvpStartArea[i].m_w || script->m_pvpStartArea[i].m_h)
                any = true;
        if (any)
        {
            fwrite("[pvp practice start area]\n", 1, 26, file);
            for (int i = 3; i < 5; ++i)
                fprintf(file, "%d %d %d %d\n",
                        script->m_pvpStartArea[i].m_x,
                        script->m_pvpStartArea[i].m_y,
                        script->m_pvpStartArea[i].m_w,
                        script->m_pvpStartArea[i].m_h);
        }
    }
    if (script->m_24c == 1)                     // ORIG Export 判 0x24c==1
        fwrite("[shadow type] `[mirror]`\n", 1, 25, file);
    else if (script->m_24c == 2)
        fwrite("[shadow type] `[more darker]`\n", 1, 30, file);
    if (!script->m_list4.empty())
    {
        fwrite("[dungeon]\n", 1, 10, file);
        for (std::list<int>::const_iterator it = script->m_list4.begin();
             it != script->m_list4.end(); ++it)
            fprintf(file, "%d\n", *it);
        fwrite("[/dungeon]\n", 1, 11, file);
    }
    if (script->m_c == 0)
        fwrite("[type] `[normal]`\n", 1, 18, file);
    else if (script->m_c == 1)
        fwrite("[type] `[boss]`\n", 1, 16, file);
    else if (script->m_c == 2)
        fwrite("[type] `[dummy]`\n", 1, 17, file);
    if (!script->m_recoveryObjects.empty())     // +0x10 [greed]
    {
        std::string grid;
        for (size_t i = 0; i < script->m_recoveryObjects.size(); ++i)
        {
            grid += g_gridSymbols[script->m_recoveryObjects[i].first];
            grid += " ";
            grid += g_gridSymbols[script->m_recoveryObjects[i].second];
            if (i + 1 != script->m_recoveryObjects.size())
                grid += "\n ";
        }
        fprintf(file, "[greed]\n`%s`\n", grid.c_str());
    }
    if (!script->m_vecTile.empty())             // +0x70 [tile]
    {
        fwrite("[tile]\n", 1, 7, file);
        for (size_t i = 0; i < script->m_vecTile.size(); ++i)
            fprintf(file, "`%s`\n", script->m_vecTile[i].c_str());
        fwrite("[/tile]\n", 1, 8, file);
    }
    if (!script->m_vecExtendedTile.empty())     // +0x7c [extended tile]
    {
        fwrite("[extended tile]\n", 1, 16, file);
        for (size_t i = 0; i < script->m_vecExtendedTile.size(); ++i)
            fprintf(file, "`%s`\n", script->m_vecExtendedTile[i].c_str());
        fwrite("[/extended tile]\n", 1, 17, file);
    }
    if (script->m_element != 4)                 // +0x6c != Clear 值
        fprintf(file, "[elemental property] `%s`\n",
                getElementName(script->m_element));
    if (script->m_sightScroll[0] != defaults.m_sightScroll[0])
        fprintf(file, "[far sight scroll] %d\n", script->m_sightScroll[0]);
    if (script->m_sightScroll[1] != defaults.m_sightScroll[1])
        fprintf(file, "[middle sight scroll] %d\n", script->m_sightScroll[1]);
    if (script->m_sightScroll[2] != defaults.m_sightScroll[2])
        fprintf(file, "[near sight scroll] %d\n", script->m_sightScroll[2]);
    if (!script->m_vecBackAni.empty())          // +0xa0 [background animation]
    {
        fwrite("[background animation]\n", 1, 23, file);
        for (size_t i = 0; i < script->m_vecBackAni.size(); ++i)
        {
            const BackAniScript& ani = script->m_vecBackAni[i];
            fprintf(file, "`%s` `%s` `%s`\n",
                    ani.m_filename.c_str(),
                    getDrawLayerName(static_cast<ENUM_DRAWLAYER>(ani.m_layer)),
                    ani.m_orderUpper ? "[upper]" : "[below]");
        }
        fwrite("[/background animation]\n", 1, 24, file);
    }
    if (!script->m_vecTownMovableArea.empty())  // +0xdc [town movable area]
    {
        fwrite("[town movable area]\n", 1, 20, file);
        for (size_t i = 0; i < script->m_vecTownMovableArea.size(); ++i)
        {
            const TownPath& tp = script->m_vecTownMovableArea[i];
            fprintf(file, "%d %d %d %d %d %d\n",
                    tp.m_x, tp.m_y, tp.m_w, tp.m_h, tp.m_field1c, tp.m_field20);
        }
        fwrite("[/town movable area]\n", 1, 21, file);
    }
    if (script->m_vecVirtualMovableArea.empty())    // +0xe8：空 → [pathgate pos]
    {
        fwrite("[pathgate pos]\n", 1, 15, file);
        for (int i = 0; i < 4; ++i)                 // words 0x20+i*8 / 0x104+i*8
            fprintf(file, "%d %d\n",
                    script->m_pathgatePos[i * 2],
                    script->m_pathgatePos[i * 2 + 1]);
    }
    else                                        // 非空 → [virtual movable area]
    {
        fwrite("[virtual movable area]\n", 1, 23, file);
        for (size_t i = 0; i < script->m_vecVirtualMovableArea.size(); ++i)
        {
            const MapArea& area = script->m_vecVirtualMovableArea[i];
            fprintf(file, "%d %d %d %d\n",
                    area.m_x, area.m_y, area.m_w, area.m_h);
        }
        fwrite("[/virtual movable area]\n", 1, 24, file);
    }
    if (!script->m_vecGuildAgitEntrance.empty())    // +0xf4
    {
        fwrite("[guild agit entrance info]\n", 1, 27, file);
        for (size_t i = 0; i < script->m_vecGuildAgitEntrance.size(); ++i)
        {
            const TownPath& tp = script->m_vecGuildAgitEntrance[i];
            fprintf(file, "%d %d %d %d %d %d\n",
                    tp.m_x, tp.m_y, tp.m_w, tp.m_h, tp.m_field1c, tp.m_field20);
        }
        fwrite("[/guild agit entrance info]\n", 1, 28, file);
    }
    {                                           // MapArea@0x140 四字段任一非零
        if (script->m_dungeonStartArea.m_x || script->m_dungeonStartArea.m_y ||
            script->m_dungeonStartArea.m_w || script->m_dungeonStartArea.m_h)
            fprintf(file, "[dungeon start area] %d %d %d %d\n",
                    script->m_dungeonStartArea.m_x,
                    script->m_dungeonStartArea.m_y,
                    script->m_dungeonStartArea.m_w,
                    script->m_dungeonStartArea.m_h);
        if (script->m_summonStartArea.m_x || script->m_summonStartArea.m_y ||
            script->m_summonStartArea.m_w || script->m_summonStartArea.m_h)
            fprintf(file, "[summon start area] %d %d %d %d\n",
                    script->m_summonStartArea.m_x,
                    script->m_summonStartArea.m_y,
                    script->m_summonStartArea.m_w,
                    script->m_summonStartArea.m_h);
    }
    if (!script->m_vecSound.empty())            // +0x194 [sound]
    {
        fwrite("[sound]\n", 1, 8, file);
        for (size_t i = 0; i < script->m_vecSound.size(); ++i)
            fprintf(file, "`%s`\n", script->m_vecSound[i].c_str());
        fwrite("[/sound]\n", 1, 9, file);
    }
    if (!script->m_listAnimation.empty())       // +0x1c [animation]
    {
        fwrite("[animation]\n", 1, 12, file);
        std::string layerName;
        for (std::list<STAssignAnimation>::const_iterator it =
                 script->m_listAnimation.begin();
             it != script->m_listAnimation.end(); ++it)
        {
            layerName = getDrawLayerName(static_cast<ENUM_DRAWLAYER>(it->m_layer));
            fprintf(file, "`%s` `%s` %d %d %d\n",
                    it->m_name.c_str(), layerName.c_str(),
                    it->m_field08, it->m_field0c, it->m_field10);
        }
        fwrite("[/animation]\n", 1, 13, file);
    }
    if (!script->m_listQuestAnimation.empty())  // +0x24 [quest animation]
    {
        fwrite("[quest animation]\n", 1, 18, file);
        std::string layerName;
        for (std::list<STQuestAnimation>::const_iterator it =
                 script->m_listQuestAnimation.begin();
             it != script->m_listQuestAnimation.end(); ++it)
        {
            layerName = getDrawLayerName(static_cast<ENUM_DRAWLAYER>(it->m_layer));
            fprintf(file, "`%s` `%s` %d %d %d %d\n",
                    it->m_name.c_str(), layerName.c_str(),
                    it->m_field08, it->m_field0c, it->m_field10, it->m_field14);
        }
        fwrite("[/quest animation]\n", 1, 19, file);
    }
    if (!script->m_listPassiveObject.empty())   // +0x2c [passive object]
    {
        fwrite("[passive object]\n", 1, 17, file);
        for (std::list<STAssignPassiveObject>::const_iterator it =
                 script->m_listPassiveObject.begin();
             it != script->m_listPassiveObject.end(); ++it)
            fprintf(file, "%d %d %d %d\n", it->m_0, it->m_4, it->m_8, it->m_c);
        fwrite("[/passive object]\n", 1, 18, file);
    }
    if (!script->m_vecMonster.empty())          // +0x34 [monster]
    {
        fwrite("[monster]\n", 1, 10, file);
        for (std::vector<STAssignMonster>::const_iterator it =
                 script->m_vecMonster.begin();
             it != script->m_vecMonster.end(); ++it)
        {
            bool isChampion = false;            // cStack_5d
            std::string typeTok;                // [fixed]/[half]/[ratio]
            std::string roleTok;
            if (it->m_20 == 0)      typeTok = "`[fixed]`";
            else if (it->m_20 == 1) typeTok = "`[half]`";
            else if (it->m_20 == 2) typeTok = "`[ratio]`";
            if (it->m_24 == 0)      roleTok = "`[normal]`";
            else if (it->m_24 == 1) { roleTok = "`[champion]`"; isChampion = true; }
            else if (it->m_24 == 3) roleTok = "`[boss]`";
            fprintf(file,
                    "%d %d %d %d %d %d %d %d %s %s%c",
                    it->m_0, static_cast<int>(it->m_4 != 0),
                    it->m_8, it->m_c, it->m_10, it->m_14, it->m_18, it->m_1c,
                    typeTok.c_str(), roleTok.c_str(),
                    isChampion ? ' ' : '\n');
            if (isChampion)
                WriteChampionFeatures(file, *it);
        }
        fwrite("[/monster]\n", 1, 11, file);
    }
    if (!script->m_vecMonster.empty())          // [monster team]/AI 段
    {
        bool hasTeam = false;
        for (size_t i = 0; i < script->m_vecMonster.size(); ++i)
            if (script->m_vecMonster[i].m_34 != 0)
                hasTeam = true;
        if (hasTeam)
        {
            // m_34==1 → `[zone defence]` %d %d（+0x38/+0x3c）
            // m_34==2 → `[roaming]` + 计数与 pair 序列（此处按 ORIG 主序输出）
            fwrite("`[zone defence]`\n", 1, 17, file);
            for (size_t i = 0; i < script->m_vecMonster.size(); ++i)
            {
                const STAssignMonster& mon = script->m_vecMonster[i];
                if (mon.m_34 == 1)
                    fprintf(file, " %d %d", mon.m_38, mon.m_3c);
                else if (mon.m_34 == 2)
                {
                    fprintf(file, "`[roaming]` ");
                    fprintf(file, " %d",
                            static_cast<int>(mon.m_vec40.size()));
                    for (size_t k = 0; k < mon.m_vec40.size(); ++k)
                        fprintf(file, " %d", mon.m_vec40[k].first);
                    for (size_t k = 0; k < mon.m_vec40.size(); ++k)
                        fprintf(file, " %d", mon.m_vec40[k].second);
                }
            }
            fputc('\n', file);
            fwrite("`[roaming]`\n", 1, 17, file);
        }
    }
    {                                           // m_4c 任一非默认(100) → team 表
        bool anyTeam = false;
        for (size_t i = 0; i < script->m_vecMonster.size(); ++i)
            if (script->m_vecMonster[i].m_4c != 100)
                anyTeam = true;
        if (anyTeam)
        {
            fwrite("`[monster team]`\n", 1, 15, file);
            for (size_t i = 0; i < script->m_vecMonster.size(); ++i)
                fprintf(file, "%d\n", script->m_vecMonster[i].m_4c);
            fwrite("`[/monster team]`\n", 1, 16, file);
        }
    }
    if (script->m_championRate != defaults.m_championRate)      // +0x4c
        fprintf(file, "[champion] %d\n", script->m_championRate);
    if (!script->m_listItem.empty())            // +0x50 [item]
    {
        fwrite("[item]\n", 1, 7, file);
        for (std::list<STAssignItem>::const_iterator it =
                 script->m_listItem.begin();
             it != script->m_listItem.end(); ++it)
        {
            fprintf(file, "%d %d %d %d %d %d %d %d %d %d",
                    it->m_field00, it->m_field04, it->m_field08,
                    it->m_field0c, it->m_field10, it->m_field14,
                    it->m_field18, it->m_field1c, it->m_field20,
                    static_cast<int>(it->m_generateInfos.size()));
            fputc('\n', file);
            for (size_t k = 0; k < it->m_generateInfos.size(); ++k)
                fprintf(file, "%d %d\n",
                        it->m_generateInfos[k].m_field00,
                        it->m_generateInfos[k].m_field04);
            fputc('\n', file);
        }
        fwrite("[/item]\n", 1, 8, file);
    }
    if (!script->m_listSpecialPassive.empty())  // +0x58 [special passive object]
    {
        fwrite("[special passive object]\n", 1, 25, file);
        for (std::list<STAssignSpecialPassiveObj>::const_iterator it =
                 script->m_listSpecialPassive.begin();
             it != script->m_listSpecialPassive.end(); ++it)
        {
            if (it->m_vec10.size() == 0)
                fprintf(file, "%d %d %d %d %d\n",
                        it->m_0, it->m_4, it->m_8, it->m_c,
                        static_cast<int>(it->m_vec10.size()));
            else
            {
                fprintf(file, "%d %d %d %d\n",
                        it->m_0, it->m_4, it->m_8, it->m_c);
                fprintf(file, "%d\n", static_cast<int>(it->m_vec10.size()));
            }
            for (size_t k = 0; k < it->m_vec10.size(); ++k)
            {
                const SPOBJ_ACTION_DATAS& act = it->m_vec10[k];
                static const char* const actionHeads[4] = {
                    "`[monster]` ", "`[trap]` ", "`[item]` ", "`[quest]` "
                };
                for (int j = 0; j < 6; ++j)
                {
                    int v = (j == 0) ? act.m_type :
                            (j == 1) ? act.m_4 :
                            (j == 2) ? act.m_8 :
                            (j == 3) ? act.m_c :
                            (j == 4) ? act.m_10 : act.m_14;
                    if (j == 0)
                    {
                        if (v >= 0 && v <= 3)
                            fwrite(actionHeads[v], 1,
                                   v == 0 ? 12 : (v == 3 ? 10 : 9), file);
                    }
                    else
                        fprintf(file, "%d ", v);
                }
                fputc('\n', file);
            }
        }
        fwrite("[/special passive object]\n", 1, 26, file);
    }
    if (!script->m_vecNpc.empty())              // +0x60 [NPC]
    {
        fwrite("[NPC]\n", 1, 6, file);
        for (size_t i = 0; i < script->m_vecNpc.size(); ++i)
        {
            const AssignNpc& npc = script->m_vecNpc[i];
            std::string dirTok;
            dirTok = (npc.m_dir == 0) ? "`[left]`" : "`[right]`";
            fprintf(file, "%d `%s` %d %d %d %d\n",
                    npc.m_npcIdx, dirTok.c_str() + 1,
                    npc.m_x, npc.m_y, npc.m_z,
                    npc.m_npcIdx);
        }
        fwrite("[/NPC]\n", 1, 7, file);
    }
    if (script->m_vecMonsterSpawnPos.size() == 0)   // +0x250 空 → ai character?
    {
        if (!script->m_vecAICharacter.empty())      // +0x25c [ai character]
        {
            fwrite("[ai character]\n", 1, 15, file);
            for (size_t i = 0; i < script->m_vecAICharacter.size(); ++i)
            {
                const STAICharacterMapArrangeData& ai = script->m_vecAICharacter[i];
                fprintf(file, "%d %d %d %d ",
                        ai.m_type, ai.m_field04, ai.m_field08, ai.m_field0c);
                if (ai.m_camp == 0)
                    fwrite("`[character]`", 1, 13, file);
                else if (ai.m_camp == 100)
                    fwrite("`[monster]`", 1, 11, file);
                else if (ai.m_camp == 200)
                    fwrite("`[neutral]`", 1, 11, file);
                fputc(' ', file);
                if (ai.m_roleType == 5)          fwrite("`[normal]`", 1, 10, file);
                else if (ai.m_roleType == 6)     fwrite("`[champion]`", 1, 12, file);
                else if (ai.m_roleType == 7)     fwrite("`[super champion]`", 1, 18, file);
                else if (ai.m_roleType == 8)     fwrite("`[boss]`", 1, 8, file);
                else                             fwrite("`[normal]`", 1, 10, file);
                fputc(' ', file);
                fprintf(file, "%d %d\n",
                        static_cast<int>(ai.m_posX), static_cast<int>(ai.m_posY));
            }
            fwrite("[/ai character]\n", 1, 16, file);
        }
    }
    else                                        // [monster spawn pos]
    {
        fwrite("[monster spawn pos]\n", 1, 20, file);
        fprintf(file, "%d\n", static_cast<int>(script->m_vecMonsterSpawnPos.size()));
        for (size_t i = 0; i < script->m_vecMonsterSpawnPos.size(); ++i)
            fprintf(file, "%d %d %d\n",
                    script->m_vecMonsterSpawnPos[i].m_field00,
                    script->m_vecMonsterSpawnPos[i].m_field04,
                    script->m_vecMonsterSpawnPos[i].m_field08);
        fwrite("[/monster spawn pos]\n", 1, 21, file);
    }
    if (!script->m_mapLoadingImagePath.empty()) // +0x26c
        fprintf(file, "[map loading image path] %s\n",
                script->m_mapLoadingImagePath.c_str());
    if (script->m_vecEventMonsterPos.empty())   // +0x278 空 → [move layered map]
    {
        if (script->m_moveLayeredType != 4)
        {
            fwrite("[move layered map]\n", 1, 19, file);
            if (script->m_allDeadCasePassable)
                fwrite("\t[all dead case passable]\n", 1, 26, file);
            if (script->m_moveLayeredType == 1)
                fprintf(file, "\t[zpos under] %d\n", script->m_zposUnder);
            fwrite("[/move layered map]\n", 1, 20, file);
        }
    }
    else                                        // [event monster position]
    {
        fwrite("[event monster position]\n", 1, 25, file);
        for (size_t i = 0; i < script->m_vecEventMonsterPos.size(); ++i)
            fprintf(file, "%d %d %d\n",
                    script->m_vecEventMonsterPos[i].m_field00,
                    script->m_vecEventMonsterPos[i].m_field04,
                    script->m_vecEventMonsterPos[i].m_field08);
        fwrite("[/event monster position]\n", 1, 26, file);
    }
    if (!script->m_vecStaticStartPos.empty())   // +0x2e8
    {
        fwrite("[static player start pos]\n", 1, 26, file);
        for (int i = 0; i < 4 && i < static_cast<int>(script->m_vecStaticStartPos.size()); ++i)
        {
            const StaticStartPos& pos = script->m_vecStaticStartPos[i];
            fprintf(file, "\t[player %d] %d %d %d\n",
                    pos.m_playerIdx + 1, pos.m_x, pos.m_y, pos.m_z);
        }
        fwrite("[/static player start pos]\n", 1, 27, file);
    }
    {                                           // screen edge（对照 defaults）
        bool chL = script->m_screenEdgeLeft != defaults.m_screenEdgeLeft;
        bool chR = script->m_screenEdgeRight != defaults.m_screenEdgeRight;
        bool chT = script->m_screenEdgeTop != defaults.m_screenEdgeTop;
        bool chB = script->m_screenEdgeBottom != defaults.m_screenEdgeBottom;
        if (chL || chR || chT || chB)
        {
            fwrite("[customized screen edge]\n", 1, 25, file);
            if (chL) fprintf(file, "[left] %d\n", script->m_screenEdgeLeft);
            if (chR) fprintf(file, "[right] %d\n", script->m_screenEdgeRight);
            if (chT) fprintf(file, "[top] %d\n", script->m_screenEdgeTop);
            if (chB) fprintf(file, "[bottom] %d\n", script->m_screenEdgeBottom);
            fwrite("[/customized screen edge]\n", 1, 26, file);
        }
    }
    if (script->m_blockUseActiveSkill)
        fwrite("[block use active skill]\n", 1, 25, file);
    if (script->m_blockUseStackableItem)
        fwrite("[block use stackable item]\n", 1, 27, file);

    fclose(file);                               // func_0x0807dea0
    convertBinaryToText(szSaveFileName);
    return true;
}
