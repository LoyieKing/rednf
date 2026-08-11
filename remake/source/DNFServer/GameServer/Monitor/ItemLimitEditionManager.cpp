// df_monitor_r — ItemLimitEditionManager（从 MonitorTypes/App/Table 拆分）
#include <stdio.h>
#include "RawAccess.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <signal.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <algorithm>

#include "ItemLimitEditionManager.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"
#include "DNFApplication.h"
#include "DNFPacketTranslater.h"
#include "DNFServerHandler.h"

CItemLimitEditionMgr::CItemLimitEditionMgr() { m_lastTime = time(0); }

CItemLimitEditionMgr::~CItemLimitEditionMgr() { clear(); }

void CItemLimitEditionMgr::makeItemLimitEditionUpdatePacket(
    Packet_Item_Limit_Edition_Update& pkt) const
{
    int idx = 0;
    for (std::map<unsigned int, CItemLimitEdition*>::const_iterator it = m_items.begin();
         it != m_items.end(); ++it)
    {
        CItemLimitEdition* item = it->second;
        *(unsigned int*)((char*)&pkt + idx * 9 + 0x12) = item->getIPGNO();
        *(unsigned int*)((char*)&pkt + idx * 9 + 0x16) = item->getSellNum();
        *(char*)((char*)&pkt + idx * 9 + 0x1a) = (char)item->isSellComplete();
        idx++;
    }
    ((RA_INT<14>*)&pkt)->v = idx;
}

void CItemLimitEditionMgr::makeItemLimitEditionSellStartPacket(
    Packet_Item_Limit_Edition_Sell_Start& pkt) const
{
    int idx = 0;
    for (std::map<unsigned int, CItemLimitEdition*>::const_iterator it = m_items.begin();
         it != m_items.end(); ++it)
    {
        it->second->makeItemInfo(*(stItemLimitEditionItemInfo_t*)((char*)&pkt + idx * 0x48 + 0xf));
        idx++;
    }
    ((RA_INT<11>*)&pkt)->v = idx;
}

void CItemLimitEditionMgr::registItem(const stItemLimitEditionItemInfo_t& info)
{
    bool error = false;
    std::map<unsigned int, CItemLimitEdition*>::iterator it = m_items.find(*(unsigned int*)&info);
    if (it == m_items.end() && 0x1b < m_items.size())
    {
        error = true;
    }
    if (!error)
    {
        CItemLimitEdition* item = new CItemLimitEdition(info);
        unsigned int ipgno = item->getIPGNO();
        std::map<unsigned int, CItemLimitEdition*>::iterator it2 = m_items.find(ipgno);
        if (it2 != m_items.end())
        {
            if (it2->second != 0)
            {
                delete it2->second;
            }
            m_items.erase(it2);
        }
        m_items.insert(std::pair<const unsigned int, CItemLimitEdition*>(item->getIPGNO(), item));
    }
}

void CItemLimitEditionMgr::removeItem(unsigned int ipgno)
{
    std::map<unsigned int, CItemLimitEdition*>::iterator it = m_items.find(ipgno);
    if (it != m_items.end())
    {
        CItemLimitEdition* item = it->second;
        if (item != 0)
        {
            delete item;
        }
        m_items.erase(it);
    }
}

bool CItemLimitEditionMgr::updateItem(unsigned int ipgno, unsigned int sellNum)
{
    std::map<unsigned int, CItemLimitEdition*>::iterator it = m_items.find(ipgno);
    if (it != m_items.end())
    {
        it->second->updateSellNum(sellNum);
        return true;
    }
    return false;
}

CItemLimitEdition* CItemLimitEditionMgr::getItemInfo(unsigned int ipgno) const
{
    std::map<unsigned int, CItemLimitEdition*>::const_iterator it = m_items.find(ipgno);
    if (it != m_items.end())
    {
        return it->second;
    }
    return 0;
}

char CItemLimitEditionMgr::isEmpty() const
{
    return (char)m_items.empty();
}

void CItemLimitEditionMgr::clear()
{
    for (std::map<unsigned int, CItemLimitEdition*>::const_iterator it = m_items.begin();
         it != m_items.end(); ++it)
    {
        if (it->second != 0)
        {
            delete it->second;
        }
    }
    m_items.clear();
}

void CItemLimitEditionMgr::processScheduledJob(CApplication* app, bool flag)
{
    time_t now;
    if (!m_items.empty() && (now = time(0), now - m_lastTime > 4 || flag))
    {
        m_lastTime = now;
        Packet_Item_Limit_Edition_Sell_end pkt;
        unsigned int expired[30];
        unsigned int count = 0;
        for (std::map<unsigned int, CItemLimitEdition*>::const_iterator it = m_items.begin();
             it != m_items.end(); ++it)
        {
            if (it->second->getSellEndTime() < (unsigned int)now)
            {
                expired[count] = it->second->getIPGNO();
                count++;
            }
        }
        if (count != 0)
        {
            CServerHandler* h = app->Get_ServerHandler();
            h->SendAllTcpGameServer(&pkt);
            for (unsigned int i = 0; i < count; i++)
            {
                std::map<unsigned int, CItemLimitEdition*>::iterator f = m_items.find(expired[i]);
                if (f != m_items.end())
                {
                    CItemLimitEdition* item = f->second;
                    if (item != 0)
                    {
                        delete item;
                    }
                    m_items.erase(f);
                }
            }
        }
        if (!m_items.empty())
        {
            Packet_Item_Limit_Edition_Update pkt2;
            pkt2.m_fieldA = (unsigned int)app->Get_ServerGroup();
            makeItemLimitEditionUpdatePacket(pkt2);
            CServerHandler* h = app->Get_ServerHandler();
            h->SendToDB(&pkt2);
            h = app->Get_ServerHandler();
            h->SendAllTcpGameServer(&pkt2);
        }
    }
}

CItemLimitEdition::~CItemLimitEdition() {}

CItemLimitEdition::CItemLimitEdition(const stItemLimitEditionItemInfo_t& info)
{
    *((stItemLimitEditionItemInfo_t*)this) = info;
    m_sellNum = ((RA_UINT<24>*)&info)->v;
}

unsigned int CItemLimitEdition::getSellEndTime() const { return m_sellEndTime; }

unsigned int CItemLimitEdition::getIPGNO() const { return m_ipgno; }

char CItemLimitEdition::isSellComplete() const
{
    if (m_sellLimit == -1)
    {
        return 0;
    }
    return (char)((int)getSellNum() >= m_sellLimit);
}

void CItemLimitEdition::makeItemInfo(stItemLimitEditionItemInfo_t& info) const
{
    *((stItemLimitEditionItemInfo_t*)&info) = *((const stItemLimitEditionItemInfo_t*)this);
    ((RA_UINT<24>*)&info)->v = getSellNum();
}

unsigned int CItemLimitEdition::getSellNum() const { return m_sellNum; }

void CItemLimitEdition::updateSellNum(unsigned int num)
{
    m_sellNum = num;
}
