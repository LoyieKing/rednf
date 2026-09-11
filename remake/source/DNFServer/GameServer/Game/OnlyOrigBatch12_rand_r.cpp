// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 OnlyOrigBatch12_rand_r/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include <algorithm>
#include <list>
#include <map>
#include <queue>
#include <string>
#include <utility>
#include <vector>

#include "CEnvironment.h"        // CEnvironment / less_TCHAR / ENUM_SERVER_GROUP
#include "DNFLexWrapper.h"       // DNFLexWrapper::setStringTableFromBuffer
#include "LogManager.h"          // LogManager::logFormat
#include "PackSet.h"             // PackSet
#include "NpcScript.h"           // STScriptFileList / loadRDARScriptListFile
#include "CreatureScript.h"      // MODULE_NAME_TAG（ORIG nm 权威名）
#include "GreyImage.h"           // GreyImage
#include "ServicePolicy.h"       // ServicePolicy
#include "ServiceRestrictManager.h"  // RestrictCategory::Enum
#include "CDataManager.h"        // rewardSubKey / stLimitItemUsageInfo
#include "CItemList.h"           // stCeraShopBonusInfo_t
#include "CGuildServerProxy.h"   // stGMInfo_t
#include "Quest.h"               // stQuestTargetCharacter_t
#include "TagRecommendNewbieOption.h"  // tagRecommendNewbieOptionTable
#include "Interval.h"                // IntervalSet

// ---------------------------------------------------------------------------
// 跨 TU 符号（ORIG 地址随注）
// ---------------------------------------------------------------------------
extern char g_szScriptFileBaseDirectory[0x104];   // B 0x949b180（ScriptExtras.cpp）
extern std::vector<std::string> g_szScriptFilePriorityBaseDirectory;  // B 0x949b168
#include "OnlyOrigBatch12_rand_r/getScriptFileFullPath.inc"
                           const char* dir, const char* name);        // T 0x088bba4e
#include "OnlyOrigBatch12_rand_r/loadRDARScriptListFile.inc"
                            const char* listPath, std::map<int, int>* unused);
// g_script_package 的定义已按 ORIG TU（RDARScript.cpp）拆至
// OnlyOrigBatch12_server_utility.cpp；本文件仅引用。
extern PackSet g_script_package;                  // ORIG B 0x09494fe0

// ===========================================================================
// timeGetTime  ORIG T 0x0808bc74：仅返回 0。
// （本文件首强实体 ⇒ GI 键名，ORIG TU = rand_r.cpp）
// ===========================================================================
unsigned int timeGetTime()
{
    return 0;
}

// [WorldLane 符号普查 · FragC 2026-09-11 代落] 删 `static bool is_base64` 副本：
// ORIG 全库仅 1 份（t 0x0807e9f8，唯一调用点在 EncodeBase64 域）；我方真身 =
// PacketTypes.cpp:952（定义）+ :973（调用）。本 TU 无调用点，删除后份数 1:1。

#include "OnlyOrigBatch12_rand_r/splitPathByDirectory.inc"


#include "OnlyOrigBatch12_rand_r/isEndModuleTag.inc"


#include "OnlyOrigBatch12_rand_r/isValidExtension.inc"


// ===========================================================================
// isExistScriptFile  ORIG t 0x088c01e2：getScriptFileFullPath + access(F_OK)。
// （ORIG TU = RDARScript.cpp）
// ===========================================================================
static bool isExistScriptFile(const char* dir, const char* name)
{
    char fullPath[0x200];
    getScriptFileFullPath(fullPath, 0x200, dir, name);
    // ORIG 0x088c01e2 尾：`sete %al; test %al,%al; je; mov$1; jmp; mov$0` =
    // 布尔经分支物化返（P56/P102）；直 `return access(...)==0` 被 4.4 折成直接
    // sete 返 → 改 if(==0)return true; return false; 分支物化形。
    if (access(fullPath, 0) == 0)
        return true;
    return false;
}

// ===========================================================================
// isExistScriptPVF  ORIG t 0x088c023c：path+name 拼串，拆出目录/文件名
// （重组后等价原串），查 g_script_package。（ORIG TU = RDARScript.cpp）
// ===========================================================================
static bool isExistScriptPVF(const char* path, const char* name)
{
    char buf[0x200];
    snprintf(buf, sizeof(buf), "%s%s", path, name);

    std::string s1;
    std::string s2;
    int len = static_cast<int>(strlen(buf));
    while (len >= 0)
    {
        if (len == 0)
        {
            s2 = buf;
            s1 = "";
            break;
        }
        if (buf[len] == '/' || buf[len] == '\\')
        {
            s2 = buf + len + 1;
            buf[len + 1] = '\0';
            s1 = buf;
            break;
        }
        --len;
    }
    s1 += s2;
    // ORIG 0x088c03c4：`test %al,%al; je; mov $1,%ebx; jmp; mov $0,%ebx` 分支物化
    // （P56：bool 返回写 if(cond)return true; return false;）。
    if (g_script_package.is_exist(s1.c_str()))
        return true;
    return false;
}

// ===========================================================================
// （loadStringTable 已按 R3 批准迁至 ScriptExtras.cpp —— ORIG RDARScript.cpp 域）
// ===========================================================================
// makeMenuActionHashMap  ORIG T 0x088dcf25：
//   非空先 clear；26 项 g_MainHudMenuActionString[i] → map[key]=i。
//   文件级表对应 ORIG _ZL25g_MainHudMenuActionString @0x93f4dc0（26 x char*）。
// （ORIG TU = RDARScriptChannel.cpp）
// ===========================================================================
static const char* g_MainHudMenuActionString[26] = {
    "MYINFO_WINDOW",           "INVENTORY_WINDOW",
    "QUEST_WINDOW",            "SKILL_WINDOW",
    "EXPERT_WINDOW",           "MERNCENARY_WINDOW",
    "EMOTICON",                "TODAYPARTNER_WINDOW",
    "FINDPARTY_WINDOW",        "COMMUNITY_WINDOW",
    "MAIL_WINDOW",             "PVPCHANNEL_SELECT_WINDOW",
    "ADVANCE_ALTAR_WINDOW",    "AUCTION_WINDOW",
    "POINT_AUCTION_WINDOW",    "ACTION_POINT_WINDOW",
    "GUIDEBOOK_WINDOW",        "ITEMDICTIONARY_WINDOW",
    "MOVIE_CAPTURE",           "GOBLINPAD_WINDOW",
    "GAMEOPTION_WINDOW",       "HOTKEY_WINDOW",
    "SELECT_CHARACTER_WINDOW", "CHANNEL_SELECT_WINDOW",
    "GO_TO_TOWN",              "EXIT_GAME",
};

#include "OnlyOrigBatch12_rand_r/makeMenuActionHashMap.inc"


// [WorldLane 符号普查 · FragC 2026-09-11 代落] 删 `static int parseStringLine`
// 副本：ORIG 全库仅 1 份（t 0x08aa6551，1 处调用）；我方真身 =
// RDARScriptStringManager.cpp:47（定义）+ :482（活调用）。本 TU 无调用点。

#include "OnlyOrigBatch12_rand_r/save_pid.inc"


#include "OnlyOrigBatch12_rand_r/send_term_signal.inc"


// ===========================================================================
// script_tag  ORIG T 0x08a0c8b2 / 0x08a0c8fa。（ORIG TU = RDARScriptObject.cpp）
// ===========================================================================
struct script_tag
{
    // [b28c-lane17] ORIG 0x08a0c8b2/0x08a0c8fa 参数在 0x8(%ebp)（无 this 槽）
    // ⇒ 二者是 static 成员函数（static 不入 mangling，符号名不变）。
    static bool isCloseString(std::string& str);
    static bool isTypeString(std::string& str);
};

#include "OnlyOrigBatch12_rand_r/isCloseString.inc"


#include "OnlyOrigBatch12_rand_r/isTypeString.inc"


// ===========================================================================
// saveGreyImage  ORIG T 0x0829d605：写 TGA（18 字节头 + 自底向上 RGBA）。
// （ORIG TU = App.cpp）
// ===========================================================================
// [b28d-solo-OnlyOrigBatch12] 头段/字段/RGBA 序已归位（TGA packed 具名结构体、
// (v<<16)|(v<<8)|v|0xff000000、int 乘法 fwrite size；P167 EXTRA memset 已消）。
// 残余 2 类 N 封顶：①内层 `buf + x*4` 4.4 恒 `mov buf,%edx; mov x,%eax;
// shl $2,%eax; lea (%edx,%eax,1),%eax`（ORIG `mov x,%eax; shl $2,%eax;
// add buf_mem,%eax`；N4/N18a 地址折叠，探针 /tmp/probe_buf.cpp fA-fH 八形 +
// /tmp/b28d12/probe_grey{,2,3}.cpp w1-w9 九形——含 `x*4+buf` 交换加法、
// `(T*)((unsigned int)buf+(unsigned int)x*4)` 整型加法、`&buf[x*4]`、具名 p
// 共 20 形全败）；②`(v<<16)` 4.4 发 `mov %edx,%ecx; shl $0x10,%ecx` vs
// ORIG `shl $0x10,%edx; mov %edx,%ecx`（N86/N7 寄存器分配，探针
// /tmp/probe_rgba.cpp hA-hD 四形 + probe_grey{,2,3}.cpp 各变体同形）。
namespace
{
struct TgaHeader
{
    unsigned char idLength;          // +0x00
    unsigned char colorMapType;      // +0x01
    unsigned char imageType;         // +0x02
    unsigned short colorMapFirst;    // +0x03
    unsigned short colorMapLength;   // +0x05
    unsigned char colorMapEntrySize; // +0x07
    unsigned short xOrigin;          // +0x08
    unsigned short yOrigin;          // +0x0a
    unsigned short width;            // +0x0c
    unsigned short height;           // +0x0e
    unsigned char depth;             // +0x10
    unsigned char descriptor;        // +0x11
} __attribute__((packed));
}  // namespace

#include "OnlyOrigBatch12_rand_r/saveGreyImage.inc"


#include "OnlyOrigBatch12_rand_r/isServiceServerGroup.inc"


// [WorldLane 符号普查 · FragC 2026-09-11 代落] 删 `static void
// setPolicyApplyValue` 副本：ORIG 全库仅 1 份（t 0x0816d490，3 处调用）；我方
// 活副本 = SecurityRestrictPolicy.cpp:33（+ :86 调用）与 ServiceRestrictManager.cpp:42
//（+ :135/:215 两调用，两份实现逐字相同，属 TU 拆分镜像）。本 TU 无调用点。

// ===========================================================================
// stNPCEventItemInfo::operator=  ORIG W 0x0891b52e（按值参数）。
// （ORIG TU = RDARScriptEventCharacterParameter.cpp）
// ===========================================================================
stNPCEventItemInfo& stNPCEventItemInfo::operator=(stNPCEventItemInfo other)
{
    m_itemId = other.m_itemId;
    m_count = other.m_count;
    return *this;
}

// ===========================================================================
bool rewardSubKey::operator<(const rewardSubKey& other) const
{
    // ORIG 布局：== fall-through 到 m_action 块，!= jne 后置 m_type 块。
    // [b28d-solo-OnlyOrigBatch12] 字段按 P248 档②改 signed char（CDataManager.h
    // 已广播）⇒ 无 cast 形直接 movzbl→edx + cmpb + setl，逐条命中 ORIG。
    if (m_type == other.m_type)
        return m_action < other.m_action;
    return m_type < other.m_type;
}

#include "OnlyOrigBatch12_rand_r/clear.inc"

