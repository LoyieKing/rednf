// ============================================================================
// OnlyOrigBatch12_rand_r.cpp —— 由 OnlyOrigBatch12.cpp 按 ORIG TU 边界拆出。
// 目标 ORIG TU：rand_r.cpp（ORIG GI 键 = _Z11timeGetTimev @0x0808be25）。
// 本文件首个 file-scope 强符号·外部链接定义 = timeGetTime ⇒
//   _GLOBAL__I__Z11timeGetTimev（P327）。
// 其余并入本文件的定义均排在 timeGetTime 之后，不影响 GI 选名；它们各自的
// ORIG TU 归属见逐条注释（拆分前同属 OnlyOrigBatch12.cpp）。
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
#include "CreatureScript.h"      // g_moduleNameTag
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
void getScriptFileFullPath(char* out, unsigned int size,
                           const char* dir, const char* name);        // T 0x088bba4e
bool loadRDARScriptListFile(STScriptFileList* list, const char* dir,
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

// ===========================================================================
// is_base64  ORIG t 0x0807e9f8：isalnum(c) || c=='+' || c=='/'。
// （ORIG TU = EncodeBase64.cpp）
// ===========================================================================
static bool is_base64(unsigned char c)
{
    return isalnum(c) || c == '+' || c == '/';
}

// ===========================================================================
// splitPathByDirectory  ORIG T 0x088bbc30：返回最后一个 '/' 或 '\\' 的下标，
// 无分隔符返回 -1。（ORIG TU = RDARScript.cpp）
// ===========================================================================
int splitPathByDirectory(const char* path)
{
    int last = -1;
    for (const char* p = path; *p != '\0'; ++p)
    {
        if (*p == '/' || *p == '\\')
            last = static_cast<int>(p - path);
    }
    return last;
}

// ===========================================================================
// isEndModuleTag  ORIG T 0x08a97610：*tag == MODULE_NAME_TAG[idx*2+1]。
// （ORIG TU = RDARScriptSkill.cpp）
// ===========================================================================
bool isEndModuleTag(std::string* tag, int idx)
{
    return *tag == g_moduleNameTag[idx][1];
}

// ===========================================================================
// isValidExtension  ORIG T 0x08a3c67d：
//   n1==0 → true；n1<n2 → false；否则 a.substr(n1-n2, n2) == b。
// （ORIG TU = RDARScriptObject.cpp）
// ===========================================================================
bool isValidExtension(const std::string& a, const std::string& b)
{
    // ORIG 0x8a3c6b7 `cmp -0xc(%ebp),%eax; jge`（有符号序）⇒ 长度局部为
    // signed int（size_t 局部 4.4 恒 jae 无符号序，P196 反演）。
    int n1 = (int)a.length();
    int n2 = (int)b.length();
    if (n1 == 0)
        return true;
    if (n1 < n2)
        return false;
    std::string s = a.substr(n1 - n2, n2);
    return s.compare(b) == 0;
}

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

void makeMenuActionHashMap(std::map<std::string, int>& menuMap)
{
    if (!menuMap.empty())
        menuMap.clear();
    for (int i = 0; i <= 0x19; ++i)
        menuMap.insert(std::make_pair(g_MainHudMenuActionString[i], i));
}

// ===========================================================================
// parseStringLine  ORIG t 0x08aa6551：脚本行 "key>value" 解析。
//   跳过前导空白；\r/\n/EOF/注释 返回 0；无 '>' 或 key 长 1 → -1；
//   key 长 > 0x29 → -2；'\\n' 转义换行；输出 *p1=start、*p2=value。
// （ORIG TU = RDARScriptStringManager.cpp）
// ===========================================================================
// [b28d-solo-OnlyOrigBatch12] AE：P167 调用点差集 CLEAR，clean 口径已离开
// DIFF/NEAR（清单 NEAR 25 为历史快照）。ORIG 0x08aa6551
static int parseStringLine(char* line, char** p1, char** p2)
{
    char* p = line;
    while (*p != '\0')
    {
        if (*p == '\r' || *p == '\n')
            return 0;
        if (*p == ' ' || *p == '\t')
            ++p;
        else
            break;
    }
    if (*p == '\0' || (*p == '/' && *(p + 1) == '/'))
        return 0;

    char* start = p;
    char* keyEnd = 0;
    while (*p != '\0')
    {
        if (*p == '>')
        {
            *p = '\0';
            ++p;
            keyEnd = p;
            break;
        }
        ++p;
    }
    if (keyEnd == 0 || keyEnd - start == 1)
        return -1;
    if (keyEnd - start - 1 > 0x28)
        return -2;

    for (; *p != '\0'; ++p)
    {
        if (*p == '\r' || *p == '\n')
        {
            *p = '\0';
            break;
        }
        if (*p == '\\' && *(p + 1) == 'n')
        {
            *p = '\n';
            char* q = p + 1;
            while (*q != '\0')
            {
                *q = *(q + 1);
                ++q;
            }
        }
    }
    *p1 = start;
    *p2 = keyEnd;
    return 0;
}

// ===========================================================================
// save_pid  ORIG T 0x0829eeb2：写 "./pid/<name>.pid"（O_CREAT|O_RDWR 0x42,
// 0644），内容 "%ld\n" getpid()。（ORIG TU = App.cpp）
// ===========================================================================
int save_pid()
{
    char path[0x1e];
    memset(path, 0, sizeof(path));
    sprintf(path, "./pid/%s.pid", G_CEnvironment()->get_file_name());
    ssize_t n;
    int fd = open(path, 0x42, 0x1a4);
    if (fd < 0)
        return 0;
    char buf[0x200];
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%ld\n", static_cast<long>(getpid()));
    n = write(fd, buf, strlen(buf));
    if (n < 0)
    {
        close(fd);
        return 0;
    }
    return 1;
}

// ===========================================================================
// send_term_signal  ORIG T 0x0829e5de：读 pid/<name>.pid，kill(pid,15)。
// （ORIG TU = App.cpp）
// ===========================================================================
// [b28d-solo-OnlyOrigBatch12] AE：P309 头部声明序倒置翻正（原 NEAR 9）。
// ORIG 槽序 f(-0xc)/r(-0x10)/pid(-0x14)/path(-0x32..-0x15) 即「先声明浅」；
// 4.4 对函数头声明亦按先声明浅（P309，推翻旧 N95/P112 判词），故声明序改为
// r,f,pid,path——同层 int/指针中先声明者深（r 先于 f 才得 r@-0x10、f@-0xc），
// pid 先于 path 才得 pid@-0x14、path@-0x32。四槽与 ORIG 逐一对齐、帧 0x48
// 不变、助记符序列全等。ORIG 0x0829e5de
int send_term_signal()
{
    // pid 无预清零（ORIG 无 movl $0,-0x14），f/r 声明赋值分离（P198 孪生）。
    int r;
    FILE* f;
    int pid;
    char path[0x1e];
    memset(path, 0, sizeof(path));
    sprintf(path, "pid/%s.pid", G_CEnvironment()->get_file_name());
    f = fopen(path, "r");
    if (!f)
    {
        printf("%s process id file open \xbd\xc7\xc6\xd0\n", path);
        return 0;
    }
    fscanf(f, "%d", &pid);
    if (pid <= 0)
    {
        fclose(f);
        printf("%d\xb9\xf8\xc0\xc7 \xc0\xdf\xb8\xf8\xb5\xc8 process id\n", pid);
        return 0;
    }
    r = kill(pid, 15);
    if (r < 0)
    {
        fclose(f);
        printf("%d\xb9\xf8 process\xb7\xce \xc1\xbe\xb7\xe1 signal \xbc\xdb\xbd\xc5 \xbd\xc7\xc6\xd0", pid);
        return 0;
    }
    fclose(f);
    return 0;
}

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

bool script_tag::isCloseString(std::string& str)
{
    // [b28c-lane17] ORIG 0x08a0c8b2：`movb $0x0,-0x9` 预清 + 条件置 1 +
    // 尾 `movzbl -0x9` ⇒ bool 汇聚局部形；嵌套 if 才产 ORIG 的
    // `seta;test;je` / `sete;test;je` 双物化（&& 被 4.4 折成 jbe/jne）。
    bool result = false;
    if (str.size() > 2)
    {
        if (str.at(1) == '/')
            result = true;
    }
    return result;
}

bool script_tag::isTypeString(std::string& str)
{
    // [b28c-lane17] ORIG 0x08a0c8fa：同 isCloseString（size()>1 && at(0)=='['）。
    bool result = false;
    if (str.size() > 1)
    {
        if (str.at(0) == '[')
            result = true;
    }
    return result;
}

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

int saveGreyImage(const GreyImage& img, const char* path)
{
    FILE* f = fopen(path, "wb");
    if (!f)
        return 0;
    TgaHeader hdr;
    hdr.idLength = 0;
    hdr.colorMapType = 0;
    hdr.imageType = 2;
    hdr.colorMapFirst = 0;
    hdr.colorMapLength = 0;
    hdr.colorMapEntrySize = 0;
    hdr.xOrigin = 0;
    hdr.yOrigin = 0;
    hdr.width = static_cast<unsigned short>(img.getWidth());
    hdr.height = static_cast<unsigned short>(img.getHeight());
    hdr.depth = 0x20;
    hdr.descriptor = 8;
    fwrite(&hdr, 1, 0x12, f);

    unsigned char* buf = new unsigned char[img.getWidth() * 4];
    int y = img.getHeight() - 1;
    while (y >= 0)
    {
        const unsigned char* row =
            static_cast<const unsigned char*>(img.getImage()) + img.getWidth() * y;
        for (int x = 0; x < img.getWidth(); ++x)
        {
            unsigned char v = static_cast<unsigned char>(~row[x]);
            *reinterpret_cast<unsigned int*>(buf + x * 4) =
                (static_cast<unsigned int>(v) << 16) |
                (static_cast<unsigned int>(v) << 8) |
                static_cast<unsigned int>(v) | 0xff000000u;
        }
        fwrite(buf, 1, img.getWidth() * 4, f);
        --y;
    }
    delete[] buf;
    fclose(f);
    return 1;
}

// ===========================================================================
// isServiceServerGroup  ORIG T 0x0889456f：枚举值 ∈ {0,7,8,10,13,0x12,0x65}
// 时返回 false，否则 true。（ORIG TU = CSCommonFunction.cpp）
// ===========================================================================
bool isServiceServerGroup(ENUM_SERVER_GROUP group)
{
    // ORIG：无参数槽（比较直读 0x8(%ebp)）；结果字节槽 movb $1 初值、
    // 命中 movb $0，尾 movzbl 返回。
    bool result = true;
    if (group == 8 || group == 10 || group == 7 || group == 13 ||
        group == 0 || group == 0x12 || group == 0x65)
        result = false;
    return result;
}

// ===========================================================================
// setPolicyApplyValue  ORIG t 0x0816d490（ServiceRestrictManager.cpp 域）：
//   把 value 串逐字符 '0'/'1' 写入 policy 的 short 向量（index 1 起）；
//   越界值记日志并置 0；index 越界即停。
// ===========================================================================
static void setPolicyApplyValue(RestrictCategory::Enum category,
                                ServicePolicy& policy,
                                const std::string& value)
{
    if (value.empty())
        return;
    int idx = 1;
    size_t limit = policy.size();
    // [b28d-solo-OnlyOrigBatch12] ORIG 0x0816d490：`call ppEv(++it)` 先于
    // `addl $1,-0x14(++idx)` ⇒ 两增量同在 for 增量表达式（逗号）中，body 尾
    // 不再 ++idx（P28 增量侧推论）。残余 `mov policy,%eax` vs ORIG
    // `mov idx,%edx` 先载为 4.4 receiver-this 先载固有（P52/P256 封顶）。
    for (std::string::const_iterator it = value.begin(); it != value.end(); ++it, ++idx)
    {
        int v = static_cast<int>(*it) - '0';
        if (v > 1 || v < 0)
        {
            LogManager::logFormat(
                1, "localtaiwan/System/ServiceRestrictManager.cpp",
                "void setPolicyApplyValue(RestrictCategory::Enum, ServicePolicy&, std::string const&)",
                0x22,
                "[Taiwan, Restrict] Invalid server value. restrict:%d,%d,%d",
                static_cast<int>(category), idx, v);
            v = 0;
        }
        if (static_cast<size_t>(idx) >= limit)
            break;
        policy[idx] = static_cast<short>(v);
    }
}

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

// ===========================================================================
// tagRecommendNewbieOptionTable::clear  ORIG W 0x0891ad8c：
//   先 push_back 11 个空 vector，再逐项 clear + reserve(6)。
// （ORIG TU = RDARScriptEventCharacterParameter.cpp）
// ===========================================================================
void tagRecommendNewbieOptionTable::clear()
{
    for (int i = 0; i <= 0xa; ++i)
        push_back(std::vector<tagRecommendNewbieOption>());
    for (int i = 0; i <= 0xa; ++i)
    {
        operator[](i).clear();
        operator[](i).reserve(6);
    }
}
