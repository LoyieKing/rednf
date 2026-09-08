// ============================================================================
// df_game_r 还原 —— CerashopAddRestrict / CItemLimitEdition / CeraShop 事件
// （G4 商城/交易批次）
// 参照 docs/class_func_reports/CerashopAddRestrict*.md、CItemLimitEdition*.md、
// CCeraShopBonusItemEvent.md + ORIG 反汇编。
// 用户限购数据位于 CUser +0x8d1e4（4 张 map）。
// CEventCeraShopRewardPoint 已拆分至 CEventCeraShopRewardPoint.h/cpp
// （CEventBase 权威声明见 CGuildServerProxy.h，本文件内仅保留
//  CCeraShopBonusItemEvent 及其局部基类）。
// ============================================================================

#include <ctime>
#include <cstring>
#include <map>
#include <string>
#include "RDARScriptStringManager.h"
#include "CGameManager.h"
#include "CInventory.h"

#include "CUser.h"
#include "CDataManager.h"
#include "CInventory.h"
#include "InterfacePacketBuf.h"
#include "Arad_DataManager.h"
#include "CItemLimitEdition.h"
#include "CEnvironment.h"   // G_CEnvironment()（GetCalcDay 读 env+0x37c）
#include "LogManager.h"
#include "CCeraShopBonusItemEvent.h"   // 权威声明（P104 ODR 收敛，2026-08-29 EventManagerWave）
// DNFLexWrapper.h 与本文件 include 链冲突，ScanTypeOrInt 用本地声明
//（定义见 DNFLexWrapperHelpers.cpp，ORIG 0x088bceaf）。
int ScanTypeOrInt(bool& isType, std::string& s, bool& isInt, bool consume);

// DB 商城限购请求类（权威定义见 DB_CerashopAddRestrict.cpp，ORIG T @ 0x080e1ff6/0x080e17ba）。
class DB_LoadCerashopAddRestrictType
{
public:
    static void makeRequest(CUser* user);
};
class DB_SaveCerashopAddRestrictType
{
public:
    static void makeRequest(CUser* user, CerashopAddRestrict::DBInputData& data);
};


// ARAD::Singleton 模板定义已在 Arad_DataManager.h 提供（唯一）；此处不再定义。
// 供 ARAD::Singleton<CerashopAddRestrict::Manager>::Get() 隐式实例化，产出 ORIG 符号
// _ZN4ARAD9SingletonIN19CerashopAddRestrict7ManagerEE3GetEv（W @ 0x080e274d）。

// 全局 C++ 权威签名（由 DNFLexWrapperHelpers.cpp 提供，匹配 ORIG mangled 符号）：
bool loadRDARScriptFile(const char* directory, const char* path);
bool ScanType(std::string& token, bool consume);
int ScanInt(bool* ok);
bool ScanStr(std::string* value);

// DB 请求类（定义于 DB_CerashopAddRestrict.cpp，ORIG 0x080e1ff6/0x080e17ba）。
// 全局作用域类；SAVE 参数类型为 CerashopAddRestrict::DBInputData。
class DB_LoadCerashopAddRestrictType;
class DB_SaveCerashopAddRestrictType;

namespace CerashopAddRestrict
{

int ScriptLoader::LoadScript()
{
    if (!loadRDARScriptFile("Etc/", "NewCashShop_Restrict.etc"))
        return -1;
    std::string section;
    while (ScanType(section, true)) {
        if (section == "[account restrict]") {
            if (LoadScriptDaily(RestrictType::RestrictType_Account) < 0)
                return -1;
        } else if (section == "[character restrict]") {
            if (LoadScriptDaily(RestrictType::RestrictType_Charac) < 0)
                return -1;
        }
    }
    return 0;
}

int ScriptLoader::LoadScriptDaily(RestrictType::T type)
{
    // ORIG 0x08abf4de：首 token 用 ScanTypeOrInt(isType, s, ok, true) 解析
    // （支持类型名/整数），isType 或 !ok 即整函数返回 0；
    // 字段顺序 ipgNo, fieldA, fieldB, t1, t2, fieldC（fieldA/fieldB 在时间前）；
    // readTime 失败 → 返回 -1；find 命中（重复 ipgNo）→ printf + 返回 -1；
    // 末尾 m_infoMap.insert(make_pair(ipgNo, info))。
    bool isType = false;
    std::string str;
    bool ok = false;
    while (true) {
        int v0 = ScanTypeOrInt(isType, str, ok, true);
        if (isType)
            return 0;
        if (!ok)
            return 0;
        unsigned int ipgNo = (unsigned int)v0;
        int fieldA = ScanInt((bool*)0);
        int fieldB = ScanInt((bool*)0);
        short t1[8], t2[8];
        extern int readTime(short* buf);
        if (!readTime(t1))
            return -1;
        if (!readTime(t2))
            return -1;
        extern unsigned int uint32Time(short* buf);
        int start = (int)uint32Time(t1);
        int end = (int)uint32Time(t2);
        int fieldC = ScanInt((bool*)0);
        if (m_infoMap.find(ipgNo) != m_infoMap.end())
        {
            printf("[character restrict]already registered ipg_no:%u\n", ipgNo);
            return -1;
        }
        InfoDaily* info = new InfoDaily(type, ipgNo, (unsigned int)fieldA,
                                        start, end, fieldB, fieldC);
        m_infoMap.insert(std::make_pair(ipgNo, info));
    }
    return 0;
}

void ScriptLoader::ClearScript()
{
}

InfoDaily* ScriptLoader::GetRestrictInfo(unsigned int ipgNo)
{
    std::map<unsigned int, InfoDaily*>::iterator it = m_infoMap.find(ipgNo);
    // [b28c-4 P222] ORIG 0x08abf766：`je 零块(fall-through); body; jmp end` 布局
    // ⇒ 正向包裹 `if (it != end) return it->second;`（否定早返会产反向 je 布局）。
    if (it != m_infoMap.end())
        return it->second;
    return 0;
}

bool ScriptLoader::FindIpgNo(unsigned int ipgNo)
{
    // ORIG 0x8abf70a（Ghidra C）：`return it == end() ? 0 : 1`，即 `find() != end()`
    // （未找到=1）。[b28c-4] ORIG 尾为 operator== + test/je + mov$0/$1 常量岛 ⇒
    // 保留分支的 if 形（直接 `!=` 表达式被 4.4 折成 xor;ret，少 4 条）。
    if (m_infoMap.find(ipgNo) == m_infoMap.end())
        return false;
    return true;
}

// ---- readTime / uint32Time ----
int readTime(short* out)
{
    bool ok = false;
    int n = 0;
    try
    {
        out[n++] = (short)ScanInt(&ok);
        if (!ok)
            throw "start year";
        out[n++] = (short)ScanInt(&ok);
        if (!ok)
            throw "start month";
        out[n++] = (short)ScanInt(&ok);
        if (!ok)
            throw "start day";
        out[n++] = (short)ScanInt(&ok);
        if (!ok)
            throw "start time";
    }
    catch (const char* msg)
    {
        LogManager::logFormat(1, "../RDARScriptCeraShopAddRestrictType.cpp", "readTime",
                              37, "readTime() Load fail. %s", msg);
        return 0;
    }
    return 1;
}

unsigned int uint32Time(short* buf)
{
    time_t t = time(0);
    struct tm* tm = localtime(&t);
    tm->tm_year = buf[0] - 1900;
    tm->tm_mon = buf[1] - 1;
    tm->tm_mday = buf[2];
    tm->tm_hour = buf[3];
    tm->tm_min = 0;
    tm->tm_sec = 0;
    return (unsigned int)mktime(tm);
}

// ---- 命名空间级辅助（ORIG 报告 CerashopAddRestrict/*.md）----

// ORIG 0x080e0a43（static，L mangled；FindUserInfoDetail.md 全文转写）：
//   idxMap.find(key)：未命中 insert(key,0) 取返回迭代器；it->second += add；
//   dailyMap.find(key)：命中 → now<nextDate 时 (count+idx)<=field8 → 1 否则 0；
//     超期 → count=0，idx<=field8 → 1 否则 0；未命中 → field8<idx → 0 否则 1。
static int FindUserInfoDetail(
    std::map<unsigned int, unsigned int>& idxMap,
    std::map<unsigned int, paramDaily*>& dailyMap,
    unsigned int key, InfoDaily* info, unsigned int add)
{
    std::map<unsigned int, unsigned int>::iterator it = idxMap.find(key);
    if (it == idxMap.end()) {
        it = idxMap.insert(
                 std::map<unsigned int, unsigned int>::value_type(key, 0)).first;
    }
    it->second += add;

    std::map<unsigned int, paramDaily*>::iterator it2 = dailyMap.find(key);
    if (it2 != dailyMap.end()) {
        paramDaily* daily = it2->second;
        time_t now = time(0);
        if (now < daily->m_nextDate) {
            if ((unsigned int)(daily->m_count + it->second) <=
                (unsigned int)info->m_field8)
                return 1;
        } else {
            daily->m_count = 0;
            if ((unsigned int)it->second <= (unsigned int)info->m_field8)
                return 1;
        }
        return 0;
    }
    if ((unsigned int)info->m_field8 < (unsigned int)it->second)
        return 0;
    return 1;
}

// ORIG 0x080e10d7（static，L mangled；GetCalcDay.md 全文转写）：
//   days=(m_end-m_start)/0x15180+1；m_limit>0 → days=m_limit；t=m_start，
//   循环 t+=days*0x15180 至 t>now；localtime_r 置 tm_hour=G_CEnvironment()+0x37c、
//   min/sec=0，mktime 返回。
static int GetCalcDay(const InfoDaily* info)
{
    int days = (info->m_end - info->m_start) / 0x15180 + 1;
    if (info->m_limit > 0)
        days = info->m_limit;
    time_t t = (time_t)info->m_start;
    time_t now = time(0);
    while (t <= now)
        t += days * 0x15180;
    struct tm local44;
    localtime_r(&t, &local44);
    local44.tm_hour = G_CEnvironment()->m_serverEnvir.m_fatigueTime;  // env+0x37c
    local44.tm_min = 0;
    local44.tm_sec = 0;
    return (int)mktime(&local44);
}

// ORIG 0x080e1196（命名空间级，T mangled；InputBuyInfoDetail.md 全文转写）：
//   m.find(key)：命中 → m_count += count；未命中 → new paramDaily 清零、
//   m_count=count、m_ipgNo=key 后 insert；尾 m_nextDate=GetCalcDay(info)，返回 pd。
paramDaily* InputBuyInfoDetail(
    std::map<unsigned int, paramDaily*>& m,
    const InfoDaily* info, unsigned int key, unsigned int count)
{
    paramDaily* result = 0;
    std::map<unsigned int, paramDaily*>::iterator it = m.find(key);
    if (it != m.end()) {
        result = it->second;
        // [b28c-4 P41/P211] ORIG 0x080e11fb 段 = `mov 0x4(%eax),%eax; mov %eax,%edx;
        // add 0x14(%ebp),%edx`（内存直加）；`(int)count` 反使 4.4 先载 eax。
        result->m_count += count;
    } else {
        paramDaily* pd = new paramDaily;
        pd->m_ipgNo = 0;
        pd->m_count = 0;
        pd->m_nextDate = 0;
        pd->m_count = count;
        pd->m_ipgNo = key;
        // [b28c-4 P192] 必须用 map 的 value_type（pair<const uint,paramDaily*>）直构：
        // 写 std::pair<unsigned int,...> 使 insert(const value_type&) 需转换构造，
        // 多一对 sret+拷贝（+3 insn、帧 0x38→0x48）。
        std::map<unsigned int, paramDaily*>::value_type p(key, pd);
        m.insert(p);
        result = pd;
    }
    result->m_nextDate = GetCalcDay(info);
    return result;
}

// [b28c-4] ORIG 0x080e09bc 实形 = ScriptInterface::C1（写基类 vptr）→
// Singleton<Manager>::C1（空）→ 派生 vptr → 5 条 `mov 0x8(%ebp),%eax;
// movl $0x0,0x8|0xc|0x10|0x14|0x18(%eax)` → ScriptLoader::C1(this+4)。
// 该 5 条落在 map(+4) 的 _Rb_tree_impl 内部（color/parent/left/right/
// node_count），是 4.1 对成员 value-init 的**前置 zero-init**（P90/N77 族），
// 非 Manager 成员——原 5 个 int 成员系臆造，已由 Main 代落删除
// （硬证据：单例 obj nm -S size=0x1c = vptr4 + m_loader 0x18）。
Manager::Manager()
    // [b28c-4] 试 value-init 形复现 ORIG 4.1 对 m_loader 的前置 zero-init
    //（P90/N77：4.4 按 CWG434 可能仍不 emit）。
    : m_loader()
{
}

Manager::~Manager()
{
}

bool Manager::Load()
{
    // ORIG 0x080e0e08：printf("[!] Loading CeraShop Add Restrict Script ...") 后
    // 调 ScriptLoader::LoadScript()；非 0 返回 0，否则 1。
    printf("[!] Loading CeraShop Add Restrict Type script - ");  // ORIG 串无 \n，仅尾随空格（带 \n 会被 GCC 转 puts）
    // [b28c-4 P0] 存量 `register unsigned char nz asm("al")` 已清除（全域禁绑，
    // N96 登记），按 P168a① 复测自然形：ORIG 尾 test/setne/test/je 常量岛。
    if (m_loader.LoadScript() != 0)
        return 0;
    return 1;
}

int Manager::Destroy()
{
    // ORIG 0x080e0e40：返回 1；经 ScriptInterface 虚槽调用时返回值被忽略，
    // 槽签名 void（Arad_DataManager.h ScriptInterface）。
    return 1;
}

bool Manager::FindIpgNo(unsigned int ipgNo)
{
    return m_loader.FindIpgNo(ipgNo);
}

// ORIG 0x080e0bee（报告 CheckBuyableProduct.md 全文转写）：
//   type==0 → 清 m_field30/m_field48；GetRestrictInfo 未命中 → 1；
//   info->m_end < time(0) → 0；m_type==0/1 → FindUserInfoDetail(...)；否则 0。
int Manager::CheckBuyableProduct(CUser* user, unsigned int ipgNo,
                                 unsigned int itemIdx, int type)
{
    UserInfo& ui = user->m_ceraUserInfo;
    if (type == 0) {
        ui.m_field30.clear();
        ui.m_field48.clear();
    }
    InfoDaily* info = m_loader.GetRestrictInfo(ipgNo);
    if (info == 0)
        return 1;
    if (info->m_end < (int)time(0))
        return 0;
    // [b28c-4 P46/P208] ORIG 0x080e0bee：m_type 单次装载 + `test;je / cmp $1;je /
    // jmp 零块` 决策链，body 全部后置 ⇒ switch 形（if 链被 4.4 就地展开成反向
    // `jne skip` 且二次装载 m_type）。
    switch (info->m_type) {
    case 0:
        return FindUserInfoDetail(ui.m_field30, ui.m_accountDaily,
                                  ipgNo, info, itemIdx);
    case 1:
        return FindUserInfoDetail(ui.m_field48, ui.m_characDaily,
                                  ipgNo, info, itemIdx);
    default:
        return 0;
    }
}

// ORIG T 0x080e0d14（LinkE2.JpnExImpl 批次修正）：static 成员；定义体内经
// ARAD::Singleton<Manager>::Get() 调 FindIpgNo/InputBuyInfo/SendSyncPacket。
void Manager::UpdateBuyableRestrictItem(CUser* user, unsigned int ipgNo,
                                        unsigned int itemIdx)
{
    if (!ARAD::Singleton<Manager>::Get()->FindIpgNo(ipgNo))
        return;
    DBInputData data;
    // [b28c-4 P0] 存量 `register unsigned char ok asm("eax")` 已清除（全域禁绑，
    // N96 登记）；ORIG 调用点是 int callee 返回值的 al 直测（P3），改表达式直读
    // 复测。data 槽基：ORIG -0x10（sizeof 已实证 0x8，见 CUser.h b28c 代落注）。
    if (ARAD::Singleton<Manager>::Get()->InputBuyInfo(user, ipgNo, itemIdx, data)) {
        // DB 请求经 DB_CerashopAddRestrict.cpp 的真实成员函数投递（ORIG 0x080e17ba）。
        DB_SaveCerashopAddRestrictType::makeRequest(user, data);
        // ORIG 直接调 static SendSyncPacket（无 this、无 Get()；注释 0x817915c 实证）
        SendSyncPacket(user);
    }
}

void Manager::ClearBuyRestrictItem(CUser* user)
{
    // [b28c-4 P183] ORIG 0x080e0da4 每处从 user 重算绝对偏移（add $0x8d1e4/...），
    // 无引用缓存槽（引用形多 -0xc 槽 + 帧 0x28→0x18 差）。
    user->m_ceraUserInfo.m_accountDaily.clear();
    user->m_ceraUserInfo.m_characDaily.clear();
    user->m_ceraUserInfo.m_field30.clear();
    user->m_ceraUserInfo.m_field48.clear();
}

void Manager::LoadRequestBuyRestrictItem(CUser* user)
{
    // DB 请求经 DB_CerashopAddRestrict.cpp 的真实成员函数投递（ORIG 0x080e1ff6）。
    DB_LoadCerashopAddRestrictType::makeRequest(user);
}

void Manager::SendSyncPacket(CUser* user)
{
    // ORIG：ARAD::Singleton<CerashopAddRestrict::Manager>::Get()->makeSyncPacket(user)
    // 符号 _ZN4ARAD9SingletonIN19CerashopAddRestrict7ManagerEE3GetEv（W @ 0x080e274d）
    ARAD::Singleton<CerashopAddRestrict::Manager>::Get()->makeSyncPacket(user);
}

// ORIG 0x080e0e68（报告 GetItemListForPacket.md 全文转写）：
//   遍历 m（begin/end 各求值一次，while 循环后缀 ++）：
//   GetRestrictInfo(it->first) 命中且 info->m_field8 <= it->second->m_count
//   （无符号比较 setbe）时 out.push_back(it->first)。
void Manager::GetItemListForPacket(
    std::vector<unsigned int, std::allocator<unsigned int> >& out,
    std::map<unsigned int, paramDaily*,
             std::less<unsigned int>,
             std::allocator<std::pair<unsigned int const, paramDaily*> > >& m)
{
    std::map<unsigned int, paramDaily*>::iterator it = m.begin();
    std::map<unsigned int, paramDaily*>::iterator end = m.end();
    while (it != end) {
        InfoDaily* info = m_loader.GetRestrictInfo(it->first);
        if (info != 0) {
            if ((unsigned int)info->m_field8 <= (unsigned int)it->second->m_count)
                out.push_back(it->first);
        }
        it++;
    }
}

// ORIG 0x080e0f3a（报告 makeSyncPacket.md 全文转写）：
//   m_accountDaily.size()+m_characDaily.size() 非零时，vector 收集两表
//   GetItemListForPacket 结果；非空则 PacketGuard 组 0x222 头：
//   put_int(size) + 逐项 put_int，finalize(true) 后 CUser::Send。
void Manager::makeSyncPacket(CUser* user)
{
    UserInfo& info = user->m_ceraUserInfo;
    int total = (int)info.m_accountDaily.size() + (int)info.m_characDaily.size();
    if (total != 0) {
        std::vector<unsigned int> list;
        GetItemListForPacket(list, info.m_accountDaily);
        GetItemListForPacket(list, info.m_characDaily);
        if (!list.empty()) {
            PacketGuard packet;
            InterfacePacketBuf* buf = (InterfacePacketBuf*)&packet;
            buf->clear();
            buf->put_header(0, 0x222);
            buf->put_int((int)list.size());
            for (unsigned int i = 0; i < list.size(); ++i)
                buf->put_int((int)list[i]);
            buf->finalize(true);
            user->Send(packet);
        }
    }
}

// ORIG 0x080e129a（报告 InputBuyInfo.md 全文转写）：
//   GetRestrictInfo 未命中 → 0；out.m_pInfo=info；m_type==0/1 时先在
//   m_field30/m_field48 中 find(ipgNo) 命中则 erase，再
//   InputBuyInfoDetail(...) 并把返回值写入 out.m_result；
//   尾判 out.m_result==0 → 0，否则 1；其它 m_type → 0。
// [b28c-4 档①] 返回类型 int→bool（Main 代落声明 CUser.h:1522）：ORIG 0x080e129a
// 出口仅 `mov $0x0/$0x1,%eax`，且唯一调用点 UpdateBuyableRestrictItem 0x080e0d14 为
// `call; test %al,%al; je` 单字节直判——int callee 在 4.4 恒多 setne 归一段
//（探针 /tmp/l4/al.cpp m1-m4 四形全败），唯 bool callee 复现。
bool Manager::InputBuyInfo(CUser* user, unsigned int ipgNo, unsigned int itemIdx,
                           DBInputData& out)
{
    InfoDaily* info = m_loader.GetRestrictInfo(ipgNo);
    if (info == 0)
        return false;
    out.m_pInfo = info;
    UserInfo& ui = user->m_ceraUserInfo;
    if (info->m_type == 0) {
        std::map<unsigned int, unsigned int>::iterator it = ui.m_field30.find(ipgNo);
        if (it != ui.m_field30.end())
            ui.m_field30.erase(it);
        out.m_result = (int)InputBuyInfoDetail(ui.m_accountDaily, info,
                                               ipgNo, itemIdx);
    } else if (info->m_type == 1) {
        std::map<unsigned int, unsigned int>::iterator it = ui.m_field48.find(ipgNo);
        if (it != ui.m_field48.end())
            ui.m_field48.erase(it);
        out.m_result = (int)InputBuyInfoDetail(ui.m_characDaily, info,
                                               ipgNo, itemIdx);
    } else {
        return false;
    }
    if (out.m_result == 0)
        return false;
    return true;
}

// IsBuyableRestrictItem —— ORIG 0x080e0ce2 (T)
// 静态转发：ARAD::Singleton<Manager>::Get()->CheckBuyableProduct(user, ipgNo, itemIdx, type)。
// 返回 CheckBuyableProduct 的结果（1=可购买）；调用方（ItemVendingMachine::
// ProcessBuyRequestEx）取反判定「不可购买」。
int Manager::IsBuyableRestrictItem(CUser* user, unsigned int ipgNo,
                                   unsigned int itemIdx, int type)
{
    return ARAD::Singleton<CerashopAddRestrict::Manager>::Get()
        ->CheckBuyableProduct(user, ipgNo, itemIdx, type);
}

// [b28c-4] ORIG 0x080e142a 载荷元素具名视图（0xc 步长：+0x00 ipgNo / +0x04 count /
// +0x08 nextDate）。ORIG 对元素三字段是 disp 直读（mov (d)/0x4(d)/0x8(d)），
// 指针下标形会被 4.4 展成 add+mov。
struct ParamDailyElem
{
    unsigned int m_ipgNo;
    unsigned int m_count;
    unsigned int m_nextDate;
};

// ORIG 0x080e142a sz 0xcb（_ZN19CerashopAddRestrict7Manager11LoadProcessE
// RSt3mapIjjSt4lessIjESaISt4pairIKjjEEERS1_IjPNS_10paramDailyES3_SaIS4_IS5_SB_EEEP35SIG_LOAD_CERASHOP_ADD_RESTRICT_TYPE；
// objdump 转写，无独立报告）：idxMap.clear()；循环 i=0..sig+0x00 的 count
// （unsigned 比较）：new paramDaily（+0x00/+0x04/+0x08 清零），从 sig+0x08
// 起以 0xc 步长取第 i 元素的 3 个 uint 依序填入
// pd->{m_ipgNo,m_count,m_nextDate}，再 dailyMap.insert(pair(elem[0], pd))。
// idxMap 仅被 clear（ORIG 无其它访问）。
void CerashopAddRestrict::Manager::LoadProcess(
    std::map<unsigned int, unsigned int>& idxMap,
    std::map<unsigned int, CerashopAddRestrict::paramDaily*>& dailyMap,
    SIG_LOAD_CERASHOP_ADD_RESTRICT_TYPE* sig)
{
    idxMap.clear();
    for (unsigned int i = 0; *(unsigned int*)sig > i; ++i)
    {
        paramDaily* pd = new paramDaily;
        pd->m_ipgNo = 0;
        pd->m_count = 0;
        pd->m_nextDate = 0;
        // [b28c-4] 求值序照 ORIG：i*0xc 先算、再 +sig、再 +8（写 `8 + i*0xc` 会被
        // 4.4 折成 lea base+index，且元素访问改具名视图成员以产 disp 直读）。
        ParamDailyElem* elem = reinterpret_cast<ParamDailyElem*>(
            reinterpret_cast<char*>(sig) + i * 0xc + 8);
        pd->m_ipgNo = elem->m_ipgNo;
        pd->m_count = elem->m_count;
        pd->m_nextDate = elem->m_nextDate;
        // [b28c-4] ORIG pair 首参照 `mov -0x24(%ebp),%eax`（=&pd->m_ipgNo）⇒
        // 实参是 pd->m_ipgNo 左值（值与 elem[0] 同、地址不同）。
        dailyMap.insert(
            std::map<unsigned int, paramDaily*>::value_type(pd->m_ipgNo, pd));
    }
}

}  // namespace CerashopAddRestrict
// ============================================================================
// CCeraShopBonusItemEvent / CEventCeraShopRewardPoint（商城事件）
// ============================================================================
// 类声明见 CCeraShopBonusItemEvent.h（权威，继承 CGuildServerProxy.h 的
// CEventBase；vtable 序 / +0x0c m_rate 布局依 ORIG 报告）。方法定义如下。

// ORIG C1 0x0810a2dc：仅 CEventBase::CEventBase() + 本类 vptr，无成员初始化
// （InitEventManager new 0x10；m_rate 由 StartEvent(Word_Param) 延迟写入）。
CCeraShopBonusItemEvent::CCeraShopBonusItemEvent()
{
}

CCeraShopBonusItemEvent::~CCeraShopBonusItemEvent()
{
}

void CCeraShopBonusItemEvent::StartEvent()
{
}

// ORIG StartEvent(Word_Param) 0x0810a34c：SetEventFlag(true)；参数低 ushort
// （m_duration）为 0 或 >1000 → 日志且不写 m_rate；否则写 +0x0c 并记日志。
void CCeraShopBonusItemEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    if (param.m_duration == 0 || 1000 < param.m_duration) {
        // [b28c-30] P227 全表达式临时链：ORIG 0x0810a361 段 vararg
        // `(unsigned int)param.m_duration` 在 ctor 前求值并入 %ebx 跨 ctor 保活
        // （push %ebx/sub $0x34 三件套）；具名 tr 两语句使 4.4 ctor 后重取。
        cMyTrace(__FUNCTION__, 0x1a, 0)
            ("Can not start Cera shop bonus item event. Bonus rate is too big(%d)",
             (unsigned int)param.m_duration);
    } else {
        m_rate = (unsigned int)param.m_duration;
        cMyTrace(__FUNCTION__, 0x20, 0)
            ("Cera shop bonus item event Start");
    }
}

void CCeraShopBonusItemEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__FUNCTION__, 0x26, 0);
    tr("Cera shop bonus item event End");
}
// getBonusRate（ORIG W 0x0817a2e2）：直返 +0x0c bonusRate（mov 0xc(%eax),%eax;
// ret；P9 直返形态，StartEvent(unsigned short) 以 4 字节写入同一字段）。
int CCeraShopBonusItemEvent::getBonusRate() const
{
    return (int)m_rate;
}
