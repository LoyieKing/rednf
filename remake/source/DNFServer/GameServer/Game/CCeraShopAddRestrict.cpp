// ============================================================================
// df_game_r 还原 —— CerashopAddRestrict / CItemLimitEdition / CeraShop 事件
// （G4 商城/交易批次）
// 参照 docs/class_func_reports/CerashopAddRestrict*.md、CItemLimitEdition*.md、
// CCeraShopBonusItemEvent.md、CEventCeraShopRewardPoint.md + ORIG 反汇编。
// 用户限购数据位于 CUser +0x8d1e4（4 张 map）。
// ============================================================================

#include <ctime>
#include <cstring>
#include <map>
#include <string>
#include <vector>

#include "CUser.h"
#include "CDataManager.h"
#include "CInventory.h"
#include "InterfacePacketBuf.h"
#include "Arad_DataManager.h"

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


// ARAD::Singleton 模板定义（与 GlobalData.cpp 一致；模板允许跨 TU 重复定义）。
// 供 ARAD::Singleton<CerashopAddRestrict::Manager>::Get() 实例化，产出 ORIG 符号
// _ZN4ARAD9SingletonIN19CerashopAddRestrict7ManagerEE3GetEv（W @ 0x080e274d）。
namespace ARAD
{
template <class T> T* Singleton<T>::Get()
{
    static T inst;
    return &inst;
}
}

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
    bool flag = false;
    while (ScanType(section, flag)) {
        if (section == "[account restrict]") {
            LoadScriptDaily(RestrictType_Account);
        } else if (section == "[charac restrict]") {
            LoadScriptDaily(RestrictType_Charac);
        }
    }
    return 0;
}

void ScriptLoader::LoadScriptDaily(RestrictType type)
{

    bool ok = false;
    while (true) {
        int v0 = ScanInt(&ok);
        if (!ok)
            break;
        InfoDaily* info = new InfoDaily;
        info->m_type = type;
        info->m_ipgNo = (unsigned int)v0;
        info->m_field8 = ScanInt(&ok);
        short timeBuf[8];
        extern int readTime(short* buf);
        readTime(timeBuf);
        extern unsigned int uint32Time(short* buf);
        info->m_start = (int)uint32Time(timeBuf);
        readTime(timeBuf);
        info->m_end = (int)uint32Time(timeBuf);
        info->m_limit = ScanInt(&ok);
        info->m_field18 = ScanInt(&ok);
        m_infoMap[info->m_ipgNo] = info;
    }
}

void ScriptLoader::ClearScript()
{
    m_infoMap.clear();
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

// ---- readTime / uint32Time / GetCalcDay ----
int readTime(short* out)
{

    bool ok = false;
    int n = 0;
    out[n++] = (short)ScanInt(&ok);
    if (!ok)
        return n;
    out[n++] = (short)ScanInt(&ok);
    if (!ok)
        return n;
    out[n++] = (short)ScanInt(&ok);
    if (!ok)
        return n;
    out[n++] = (short)ScanInt(&ok);
    return n;
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

// 用户侧地图（CUser::m_ceraUserInfo @ +0x8d1e4）
static UserInfo& UserCeraInfo(CUser* u)
{
    return u->m_ceraUserInfo;
}

Manager::Manager()
{
}

Manager::~Manager()
{
}

bool Manager::Load()
{
    return m_loader.LoadScript() == 0;
}

int Manager::Destroy()
{
    return 1;
}

void Manager::FindIpgNo(unsigned int ipgNo)
{
    m_loader.FindIpgNo(ipgNo);
}

void Manager::CheckBuyableProduct(CUser* user, unsigned int ipgNo,
                                  unsigned int itemIdx, int type)
{
    if (type == 0) {
        UserCeraInfo(user).m_field30.clear();
        UserCeraInfo(user).m_field48.clear();
    }
    InfoDaily* info = m_loader.GetRestrictInfo(ipgNo);
    if (info == 0)
        return;
    if (info->m_type == 0) {
        // 账号级：检查/登记账号限购记录
        std::map<unsigned int, paramDaily*>& m = UserCeraInfo(user).m_accountDaily;
        std::map<unsigned int, paramDaily*>::iterator it = m.find(itemIdx);
        paramDaily* daily;
        if (it == m.end()) {
            daily = new paramDaily;
            daily->m_ipgNo = itemIdx;
            daily->m_count = 0;
            daily->m_nextDate = 0;
            m[itemIdx] = daily;
        } else {
            daily = it->second;
        }
        UserCeraInfo(user).m_field30[itemIdx] =
            daily ? (unsigned int)daily->m_count : 0;
    } else {
        std::map<unsigned int, paramDaily*>& m = UserCeraInfo(user).m_characDaily;
        std::map<unsigned int, paramDaily*>::iterator it = m.find(itemIdx);
        paramDaily* daily;
        if (it == m.end()) {
            daily = new paramDaily;
            daily->m_ipgNo = itemIdx;
            daily->m_count = 0;
            daily->m_nextDate = 0;
            m[itemIdx] = daily;
        } else {
            daily = it->second;
        }
        UserCeraInfo(user).m_field48[itemIdx] =
            daily ? (unsigned int)daily->m_count : 0;
    }
}

void Manager::UpdateBuyableRestrictItem(CUser* user, unsigned int ipgNo,
                                        unsigned int itemIdx)
{
    // DB 请求经 DB_CerashopAddRestrict.cpp 的真实成员函数投递（ORIG 0x080e17ba）。
    DBInputData data;
    if (InputBuyInfo(user, ipgNo, itemIdx, data) != 0) {
        DB_SaveCerashopAddRestrictType::makeRequest(user, data);
        SendSyncPacket(user);
    }
}

void Manager::ClearBuyRestrictItem(CUser* user)
{
    UserInfo& info = UserCeraInfo(user);
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

void Manager::makeSyncPacket(CUser* user)
{
    PacketGuard packet;
    InterfacePacketBuf* buf = (InterfacePacketBuf*)&packet;
    buf->put_header(0, 0x122);
    std::map<unsigned int, paramDaily*>& m = UserCeraInfo(user).m_accountDaily;
    buf->put_short((int)m.size());
    for (std::map<unsigned int, paramDaily*>::iterator it = m.begin();
         it != m.end(); ++it) {
        buf->put_int(it->first);
        buf->put_int(it->second ? it->second->m_count : 0);
    }
    buf->finalize(true);
    user->Send(packet);
}

int Manager::InputBuyInfo(CUser* user, unsigned int ipgNo, unsigned int itemIdx,
                          DBInputData& out)
{
    InfoDaily* info = m_loader.GetRestrictInfo(ipgNo);
    if (info == 0)
        return 0;
    out.m_pInfo = info;
    std::map<unsigned int, paramDaily*>& m =
        info->m_type == 0 ? UserCeraInfo(user).m_accountDaily
                          : UserCeraInfo(user).m_characDaily;
    std::map<unsigned int, paramDaily*>::iterator it = m.find(itemIdx);
    paramDaily* daily;
    if (it == m.end()) {
        daily = new paramDaily;
        daily->m_ipgNo = itemIdx;
        daily->m_count = 1;
        daily->m_nextDate = 0;
        m[itemIdx] = daily;
    } else {
        daily = it->second;
        daily->m_count += 1;
    }
    return 1;
}

}  // namespace CerashopAddRestrict

// ============================================================================
// CItemLimitEdition / CItemLimitEditionMgr
// ============================================================================
struct stItemLimitEditionItemInfo_t
{
    int m_field0;                 // +0x00
    unsigned long m_itemIdx;      // +0x04
    int m_field8;                 // +0x08
    char m_fieldc;                // +0x0c
    int m_field10;                // +0x10
    int m_field14;                // +0x14
    char m_pad18[0x34];           // +0x18
    int m_field4c;                // +0x4c
    int m_field50;                // +0x50
    char m_pad54[0x2c];           // +0x54（条目尺寸 0x80）
};

class CItemLimitEdition
{
public:
    CItemLimitEdition(const stItemLimitEditionItemInfo_t& info);
    ~CItemLimitEdition();

    void updateCurSellNum(unsigned int count);
    void makeInfoPacket(PacketGuard& packet) const;

    int m_field0;                  // +0x00
    unsigned long m_itemIdx;       // +0x04
    int m_field8;                  // +0x08
    char m_fieldc;                 // +0x0c
    int m_field10;                 // +0x10
    int m_field14;                 // +0x14
    char m_pad18[0x30];            // +0x18
    int m_field48;                 // +0x48（curSellNum）
    int m_field4c;                 // +0x4c
    int m_field50;                 // +0x50
    char m_pad54[0x2c];            // +0x54（尺寸 0x80）
};

CItemLimitEdition::CItemLimitEdition(const stItemLimitEditionItemInfo_t& info)
{
    memcpy(this, &info, 0x80);
}

CItemLimitEdition::~CItemLimitEdition()
{
}

void CItemLimitEdition::updateCurSellNum(unsigned int count)
{
    m_field48 = (int)count;
}

void CItemLimitEdition::makeInfoPacket(PacketGuard& packet) const
{
    InterfacePacketBuf* buf = (InterfacePacketBuf*)&packet;
    buf->put_int(m_field0);
    buf->put_item_idx(m_itemIdx);
    buf->put_int(m_field8);
    buf->put_byte(m_fieldc);
    buf->put_int(m_field10);
    buf->put_int(m_field14);
    buf->put_int(m_field48);
    buf->put_int(m_field4c);
    buf->put_int(m_field50);
}

class CItemLimitEditionMgr
{
public:
    CItemLimitEditionMgr();
    ~CItemLimitEditionMgr();

    void registItem(const stItemLimitEditionItemInfo_t& info);
    void removeItem(unsigned int itemIdx);
    void updateItem(unsigned int itemIdx, unsigned int count);
    void clear();

    std::map<unsigned int, CItemLimitEdition*> m_items;
};

CItemLimitEditionMgr::CItemLimitEditionMgr()
{
}

CItemLimitEditionMgr::~CItemLimitEditionMgr()
{
    clear();
}

void CItemLimitEditionMgr::registItem(const stItemLimitEditionItemInfo_t& info)
{
    unsigned int itemIdx = (unsigned int)info.m_itemIdx;
    std::map<unsigned int, CItemLimitEdition*>::iterator it = m_items.find(itemIdx);
    if (it != m_items.end())
        delete it->second;
    m_items[itemIdx] = new CItemLimitEdition(info);
}

void CItemLimitEditionMgr::removeItem(unsigned int itemIdx)
{
    std::map<unsigned int, CItemLimitEdition*>::iterator it = m_items.find(itemIdx);
    if (it != m_items.end()) {
        delete it->second;
        m_items.erase(it);
    }
}

void CItemLimitEditionMgr::updateItem(unsigned int itemIdx, unsigned int count)
{
    std::map<unsigned int, CItemLimitEdition*>::iterator it = m_items.find(itemIdx);
    if (it != m_items.end())
        it->second->updateCurSellNum(count);
}

void CItemLimitEditionMgr::clear()
{
    for (std::map<unsigned int, CItemLimitEdition*>::iterator it = m_items.begin();
         it != m_items.end(); ++it)
        delete it->second;
    m_items.clear();
}

// ============================================================================
// CCeraShopBonusItemEvent / CEventCeraShopRewardPoint（商城事件）
// ============================================================================
class CEventBase
{
public:
    CEventBase() {}
    ~CEventBase() {}
    void SetEventFlag(bool flag) { m_flag = flag; }
    char m_flag;
};

class CCeraShopBonusItemEvent : public CEventBase
{
public:
    CCeraShopBonusItemEvent();
    ~CCeraShopBonusItemEvent();
    void StartEvent();
    void StartEvent(unsigned short bonusRate);
    void EndEvent();
    int getBonusRate();
    char m_padC[4];   // +0x0c（bonusRate）
};

CCeraShopBonusItemEvent::CCeraShopBonusItemEvent()
{
    m_flag = 0;
    m_padC[0] = 0;
}

CCeraShopBonusItemEvent::~CCeraShopBonusItemEvent()
{
}

void CCeraShopBonusItemEvent::StartEvent()
{
}

void CCeraShopBonusItemEvent::StartEvent(unsigned short bonusRate)
{
    SetEventFlag(true);
    if (bonusRate == 0 || 1000 < bonusRate) {
        cMyTrace tr("StartEvent", 0x1a, 0);
        tr("Can not start Cera shop bonus item event. Bonus rate is too big(%d)",
           (unsigned int)bonusRate);
    } else {
        *(unsigned int*)m_padC = (unsigned int)bonusRate;
        cMyTrace tr("StartEvent", 0x20, 0);
        tr("Cera shop bonus item event Start");
    }
}

void CCeraShopBonusItemEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr("EndEvent", 0x26, 0);
    tr("Cera shop bonus item event End");
}

int CCeraShopBonusItemEvent::getBonusRate()
{
    return *(int*)m_padC;
}

class CEventCeraShopRewardPoint : public CEventBase
{
public:
    CEventCeraShopRewardPoint();
    ~CEventCeraShopRewardPoint();
    void StartEvent();
    void StartEvent(unsigned int param);
    void EndEvent();
    void SendReward(CUser* user);

    int m_fieldC;    // +0x0c（ceraPoint）
    int m_field10;   // +0x10（timing）
};

CEventCeraShopRewardPoint::CEventCeraShopRewardPoint()
{
    m_field10 = 0;
    m_fieldC = 0;
}

CEventCeraShopRewardPoint::~CEventCeraShopRewardPoint()
{
}

void CEventCeraShopRewardPoint::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr("virtual void CEventCeraShopRewardPoint::StartEvent()", 0x13a, 0);
    tr("[Taiwan, CeraShopRewardPoint] Start event. Timing:%d, CeraPoint:%d ",
       m_field10, m_fieldC);
}

void CEventCeraShopRewardPoint::StartEvent(unsigned int param)
{
    m_field10 = param & 0xffff;
    m_fieldC = (int)(param >> 0x10);
    StartEvent();
}

void CEventCeraShopRewardPoint::EndEvent()
{
    m_field10 = 0;
    m_fieldC = 0;
    SetEventFlag(false);
}

void CEventCeraShopRewardPoint::SendReward(CUser* user)
{
    (void)user;
}
