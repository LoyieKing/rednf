// df_game_r：Gm_List 独立编译单元。
// ---- b28d-61 判词汇总（本 TU 2 件非 identical，全 N 封顶）----
// SendGMList_To_UpperServer 0x08299d54（P167 MISSING+EXTRA：Packet_Sync_GM_List
//   构造/发送块差异，N80 变参求值序）、LoadGmListFromDb 0x08299a10
//   （P167 CLEAR 38/38；N78/N89 迭代器+局部初始化 lowering）。
// ORIG SendGMList_To_UpperServer @0x8299d54：
//   serverGroup 0=监视服务器、1=公会服务器；每 20 个 GM 一包（Packet_Sync_GM_List，
//   header 0x1f4d/0x70），m_pageNo 递增；末批不足 20 也发送。
#include "CGuildServerProxy.h"   // Gm_List / WongWork::CGMAccounts 权威声明

#include <list>

#include "CGuildServerProxy.h"      // WongWork::CGMAccounts / stGMInfo_t / proxies
#include "CMonitorServerProxy.h"
#include "CServerProxyMgr.h"
#include "CEnvironment.h"
#include "GlobalData.h"
#include "Packet_Sync_GM_List.h"
#include "DBMgr.h"
#include "MySQL.h"
#include "CInventory.h"   // NumberToString

// 类声明权威点在 CGuildServerProxy.h；此处仅实现。
void Gm_List::SendGMList_To_UpperServer(int param)
{
    if (param != 0 && param != 1)
        return;

    int serverGroup = G_CEnvironment()->get_server_group();
    const std::list<WongWork::CGMAccounts::stGMInfo_t> gmList =
        GlobalData::s_pGMAccounts_->GetGMList();

    Packet_Sync_GM_List packet;
    std::list<WongWork::CGMAccounts::stGMInfo_t>::const_iterator it = gmList.begin();
    int count = 0;
    while (it != gmList.end())
    {
        packet.m_gmIds[count] = it->mid;
        packet.m_gmNames[count] = static_cast<char>(it->type);
        ++count;
        if (count == 0x14)
        {
            packet.m_gmCount = 0x14;
            if (param == 1)
            {
                CGuildServerProxy* proxy =
                    GlobalData::s_guild_proxy_mgr->GetServerProxy(
                        static_cast<ENUM_SERVER_GROUP>(serverGroup));
                proxy->SendTcpPacket(reinterpret_cast<char*>(&packet), 0x70);
            }
            else if (param == 0)
            {
                CMonitorServerProxy* proxy =
                    GlobalData::s_monitor_proxy_mgr->GetServerProxy(
                        static_cast<ENUM_SERVER_GROUP>(serverGroup));
                proxy->SendTcpPacket(reinterpret_cast<char*>(&packet), 0x70);
            }
            ++packet.m_pageNo;
            count = 0;
        }
        ++it;
    }
    if (count != 0)
    {
        packet.m_gmCount = static_cast<unsigned char>(count);
        if (param == 1)
        {
            CGuildServerProxy* proxy =
                GlobalData::s_guild_proxy_mgr->GetServerProxy(
                    static_cast<ENUM_SERVER_GROUP>(serverGroup));
            proxy->SendTcpPacket(reinterpret_cast<char*>(&packet), 0x70);
        }
        else if (param == 0)
        {
            CMonitorServerProxy* proxy =
                GlobalData::s_monitor_proxy_mgr->GetServerProxy(
                    static_cast<ENUM_SERVER_GROUP>(serverGroup));
            proxy->SendTcpPacket(reinterpret_cast<char*>(&packet), 0x70);
        }
    }
}


// ---- 生命周期 / DB 装载 ----

// ORIG 0x08299912：m_db = DBMgr::GetDBHandle((ENUM_DB_HANDLE_IDX)6, (ENUM_SERVER_GROUP)0)
// 后立即 LoadGmListFromDb()。
Gm_List::Gm_List()
{
    m_db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)6,
                                             (ENUM_SERVER_GROUP)0);
    LoadGmListFromDb();
}

// ORIG 0x08299948：D1 为 deleting 形态（cmpl $0,0x8(%ebp); je;
// call D1(自身递归); call _ZdlPv）——源码为 `delete this` 展开。
Gm_List::~Gm_List() { delete this; }

// ORIG 0x081094ea：push_back({mid, level})。
void WongWork::CGMAccounts::LoadGmList(unsigned int mid, int level)
{
    // ORIG 0x081094ea：POD 直接两 store（mid 先、level 后），无 ctor call
    // （ORIG nm 无 stGMInfo_t C1 符号，聚合 POD）。
    stGMInfo_t info;
    info.mid = mid;
    info.type = static_cast<unsigned int>(level);
    m_gmList.push_back(info);
}

// ORIG 0x08109510。
void WongWork::CGMAccounts::clearGmList()
{
    m_gmList.clear();
}

// ORIG 0x0829996c：查询 "seLect m_id, level from gm_manifest"；失败 trace 返回；
// 成功先 clearGmList，逐行 fetch 取 m_id/level 并 LoadGmList，累计 m_id 日志串；
// 若 get_channel_no()==1 再 SendGMList_To_UpperServer(0) 同步监视服务器。
void Gm_List::LoadGmListFromDb()
{
    bool ok = false;
    m_db->set_query("seLect m_id, level from gm_manifest");
    // b28d-61 [P198① 预清必写两句式，ORIG 0x08299axx `call exec; mov %al,-0x11;
    // movzbl -0x11,%eax; xor $1`]：旧单句 `if (!m_db->exec(true))` 直用返回值，
    // 缺槽回读两段。
    ok = m_db->exec(true);
    if (!ok)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x15d4, 9, true, true);
        trace("Failed to get GM list from DB.");
    }
    else
    {
        std::string idLog;   // ORIG：exec 成功分支内构造（0x08109a4c 区）
        WongWork::CGMAccounts* accounts = GlobalData::s_pGMAccounts_;
        accounts->clearGmList();
        int rows = m_db->get_n_rows();
        for (int row = 0; row < rows; ++row)
        {
            unsigned int mid = 0;
            int level = 0;
            if (!m_db->fetch() || !m_db->get_int(0, mid))
            {
                cMyTrace trace(__PRETTY_FUNCTION__, 0x15e5, 9, true, true);
                trace("Failed to get GM's m_id from DB.");
                return;
            }
            {
                cMyTrace trace(__PRETTY_FUNCTION__, 0x15ec, 0);
                trace("GM M_ID : %s", NumberToString(mid, 0));
            }
            {
                cMyTrace trace(__PRETTY_FUNCTION__, 0x15f2, 9, true, true);
                if (!m_db->get_int(1, level))
                {
                    trace("Failed to get GM's lv from DB.");
                    return;
                }
            }
            accounts->LoadGmList(mid, level);
            idLog.append(std::string(NumberToString(mid, 0)) + ", ");   // ORIG: _ZStpl + append
        }
        ok = true;
        cMyTrace trace(__PRETTY_FUNCTION__, 0x15f9, 9, true, true);
        trace("\t- GM m_id: %s", idLog.c_str());
        if (G_CEnvironment()->get_channel_no() == 1)
            SendGMList_To_UpperServer(0);
    }
    (void)ok;
}
