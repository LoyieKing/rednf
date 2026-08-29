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
    if (it == m_infoMap.end())
        return 0;
    return it->second;
}

bool ScriptLoader::FindIpgNo(unsigned int ipgNo)
{
    return m_infoMap.find(ipgNo) != m_infoMap.end();
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
        result->m_count += (int)count;
    } else {
        paramDaily* pd = new paramDaily;
        pd->m_ipgNo = 0;
        pd->m_count = 0;
        pd->m_nextDate = 0;
        pd->m_count = (int)count;
        pd->m_ipgNo = key;
        std::pair<unsigned int, paramDaily*> p(key, pd);
        m.insert(p);
        result = pd;
    }
    result->m_nextDate = GetCalcDay(info);
    return result;
}

// ORIG 0x080e09bc：基类 ARAD::ScriptInterface 仅置 vptr（4 字节），随后清
// +0x08..+0x18 五个标量并构造 ScriptLoader(+0x04)。
Manager::Manager()
    : m_field8(0), m_fieldc(0), m_field10(0), m_field14(0), m_field18(0)
{
    // ORIG 0x080e09bc：ScriptInterface 基类 vptr 由基类构造写入；随后清
    // +0x08..+0x18 五个标量并构造 ScriptLoader(+0x04)（成员自动构造）。
}

Manager::~Manager()
{
}

bool Manager::Load()
{
    // ORIG 0x080e0e08：printf("[!] Loading CeraShop Add Restrict Script ...") 后
    // 调 ScriptLoader::LoadScript()；非 0 返回 0，否则 1。
    printf("[!] Loading CeraShop Add Restrict Type script - \n");  // ORIG 串无 \n，仅尾随空格
    return m_loader.LoadScript() == 0;
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
    if (info->m_type == 0)
        return FindUserInfoDetail(ui.m_field30, ui.m_accountDaily,
                                  ipgNo, info, itemIdx);
    if (info->m_type == 1)
        return FindUserInfoDetail(ui.m_field48, ui.m_characDaily,
                                  ipgNo, info, itemIdx);
    return 0;
}

// ORIG T 0x080e0d14（LinkE2.JpnExImpl 批次修正）：static 成员；定义体内经
// ARAD::Singleton<Manager>::Get() 调 FindIpgNo/InputBuyInfo/SendSyncPacket。
void Manager::UpdateBuyableRestrictItem(CUser* user, unsigned int ipgNo,
                                        unsigned int itemIdx)
{
    if (!ARAD::Singleton<Manager>::Get()->FindIpgNo(ipgNo))
        return;
    DBInputData data;
    if (ARAD::Singleton<Manager>::Get()->InputBuyInfo(user, ipgNo, itemIdx, data) != 0) {
        // DB 请求经 DB_CerashopAddRestrict.cpp 的真实成员函数投递（ORIG 0x080e17ba）。
        DB_SaveCerashopAddRestrictType::makeRequest(user, data);
        ARAD::Singleton<Manager>::Get()->SendSyncPacket(user);
    }
}

void Manager::ClearBuyRestrictItem(CUser* user)
{
    UserInfo& info = user->m_ceraUserInfo;
    info.m_accountDaily.clear();
    info.m_characDaily.clear();
    info.m_field30.clear();
    info.m_field48.clear();
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
int Manager::InputBuyInfo(CUser* user, unsigned int ipgNo, unsigned int itemIdx,
                          DBInputData& out)
{
    InfoDaily* info = m_loader.GetRestrictInfo(ipgNo);
    if (info == 0)
        return 0;
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
        return 0;
    }
    if (out.m_result == 0)
        return 0;
    return 1;
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
        unsigned int* elem = reinterpret_cast<unsigned int*>(
            reinterpret_cast<char*>(sig) + 8 + i * 0xc);
        pd->m_ipgNo = elem[0];
        pd->m_count = elem[1];
        pd->m_nextDate = elem[2];
        dailyMap.insert(
            std::map<unsigned int, paramDaily*>::value_type(elem[0], pd));
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
        cMyTrace tr(__FUNCTION__, 0x1a, 0);
        tr("Can not start Cera shop bonus item event. Bonus rate is too big(%d)",
           (unsigned int)param.m_duration);
    } else {
        m_rate = (unsigned int)param.m_duration;
        cMyTrace tr(__FUNCTION__, 0x20, 0);
        tr("Cera shop bonus item event Start");
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
