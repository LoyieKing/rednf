// df_monitor_r — LimitNpcBuyItem（从 MonitorTypes/App/Table 拆分）
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

#include "LimitNpcBuyItem.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"

LimitNpcBuyItemManager::LimitNpcBuyItemManager() {}

LimitNpcBuyItemManager::~LimitNpcBuyItemManager() {}

int LimitNpcBuyItemManager::sellNpcLimitBuyItem(LimitNpcBuyItemInfo* info)
{
    register unsigned int total;
    register unsigned int maxCount;
    std::map<unsigned int, NpcBuyLimitItem>::iterator it = m_items.find(info->m_itemId);
    if (it == m_items.end())
    {
        return 0x11;
    }
    if (it->first == 0)
    {
        return 0x11;
    }
    if (it->second.m_sellCount >= it->second.m_maxCount)
    {
        return 0x5f;
    }
    it->second.m_sellCount += info->m_count;
    total = it->second.m_sellCount;
    maxCount = it->second.m_maxCount;
    DNF_LOG_SCOPE_LINE(0x23, "./log/NpcBuyLimitItem",
        "Sell-> characNo: %u, itemId: %u, buyCount: %u, maxCount: %u, totalSellCount: %u)",
        info->m_charNo, info->m_itemId, info->m_count, maxCount, total);
    return 0;
}

void LimitNpcBuyItemManager::undoNpcLimitBuyItem(LimitNpcBuyItemUpdate* info)
{
    std::map<unsigned int, NpcBuyLimitItem>::iterator it = m_items.find(info->m_itemId);
    if (it == m_items.end())
    {
        return;
    }
    if (it->second.m_sellCount < info->m_cancelCount)
    {
        return;
    }
    it->second.m_sellCount -= info->m_cancelCount;
    DNF_LOG_SCOPE_LINE(0x34, "./log/NpcBuyLimitItem",
        "Undo-> characNo: %u, errorNo: %u, itemId: %u, cancelCount: %u, maxCount: %u, totalSellCount: %u)",
        info->m_charNo, info->m_errorNo, info->m_itemId, info->m_cancelCount,
        it->second.m_maxCount, it->second.m_sellCount);
}

void LimitNpcBuyItemManager::registItem(NpcBuyLimitItem& item)
{
    std::map<unsigned int, NpcBuyLimitItem>::iterator it = m_items.find(item.m_itemId);
    if (it == m_items.end())
    {
        m_items.insert(std::make_pair(item.m_itemId, item));
    }
}

void LimitNpcBuyItemManager::registItemClear()
{
    m_items.clear();
}

void LimitNpcBuyItemManager::getNpcLimitBuyItemInfoAll(LimitNpcBuyItemInfoAll* out)
{
    for (std::map<unsigned int, NpcBuyLimitItem>::iterator it = m_items.begin();
         it != m_items.end(); ++it)
    {
        if (it->first != 0)
        {
            if (out->m_count < 0)
            {
                return;
            }
            if (0x1d < out->m_count)
            {
                return;
            }
            *(unsigned int*)((char*)out + out->m_count * 0xc + 0x1a) = it->first;
            *(unsigned int*)((char*)out + out->m_count * 0xc + 0x1e) = it->second.m_maxCount;
            *(unsigned int*)((char*)out + out->m_count * 0xc + 0x22) = it->second.m_sellCount;
            out->m_count = out->m_count + 1;
        }
    }
}

void LimitNpcBuyItemManager::getNpcLimitBuyItemCount(unsigned int itemId,
                                                     LimitNpcBuyItemChangeInfo& out)
{
    std::map<unsigned int, NpcBuyLimitItem>::iterator it = m_items.find(itemId);
    if (it == m_items.end())
    {
        return;
    }
    out.m_itemId = itemId;
    register int maxCount = (int)it->second.m_maxCount;
    register int sellCount = (int)it->second.m_sellCount;
    out.m_fieldE = maxCount - sellCount;
}

LimitNpcBuyItemChangeInfo::LimitNpcBuyItemChangeInfo() : PacketHeader(0x27db, 0x12)
{
    ((RA_UINT<10>*)this)->v = 0;
    ((RA_INT<14>*)this)->v = 0;
}

LimitNpcBuyItemRequestInfo::LimitNpcBuyItemRequestInfo() : PacketHeader(0x27d8, 10) {}
