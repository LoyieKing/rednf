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
    unsigned int idx = 0;
    for (std::map<unsigned int, CItemLimitEdition*>::const_iterator it = m_items.begin(),
             endIt = m_items.end(); it != endIt; ++it)
    {
        CItemLimitEdition* item = it->second;
        pkt.m_records[idx].m_ipgno = item->getIPGNO();
        pkt.m_records[idx].m_sellNum = item->getSellNum();
        pkt.m_records[idx].m_sellComplete = item->isSellComplete();
        idx++;
    }
    pkt.m_count = idx;
}

void CItemLimitEditionMgr::makeItemLimitEditionSellStartPacket(
    Packet_Item_Limit_Edition_Sell_Start& pkt) const
{
    int idx = 0;
    for (std::map<unsigned int, CItemLimitEdition*>::const_iterator it = m_items.begin(),
             endIt = m_items.end(); it != endIt; ++it)
    {
        CItemLimitEdition* item = it->second;
        item->makeItemInfo(*(stItemLimitEditionItemInfo_t*)&pkt.m_items[idx]);
        idx++;
    }
    pkt.m_count = idx;
}

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0 STRICT | CItemLimitEditionMgr::registItem（原 DNF-MON-NEAR-0011 已消除）
void CItemLimitEditionMgr::registItem(const stItemLimitEditionItemInfo_t& info)
{
    {
        // 该内层块为 ORIG 槽位所需（S1：作用域深度决定栈槽）；去掉块后 found 槽由
        // -0x3c 变为 -0x30，本件退回 NEAR(ndiff=10)。
        const std::map<unsigned int, CItemLimitEdition*>::iterator found =
            m_items.find(*(unsigned int*)&info);
        if (found == m_items.end() && m_items.size() > 0x1b)
        {
            return;
        }
    }
    CItemLimitEdition* item = new CItemLimitEdition(info);
    const std::map<unsigned int, CItemLimitEdition*>::iterator it =
        m_items.find(item->getIPGNO());
    if (it != m_items.end())
    {
        register CItemLimitEdition* old = it->second;
        delete old;
        m_items.erase(it);
    }
    m_items.insert(std::make_pair(item->getIPGNO(), item));
}

void CItemLimitEditionMgr::removeItem(unsigned int ipgno)
{
    std::map<unsigned int, CItemLimitEdition*>::iterator it = m_items.find(ipgno);
    if (it != m_items.end())
    {
        register CItemLimitEdition* item = it->second;
        delete item;
        m_items.erase(it);
    }
}

bool CItemLimitEditionMgr::updateItem(unsigned int ipgno, unsigned int sellNum)
{
    std::map<unsigned int, CItemLimitEdition*>::iterator it = m_items.find(ipgno);
    if (it == m_items.end())
    {
        return false;
    }
    it->second->updateSellNum(sellNum);
    return true;
}

CItemLimitEdition* CItemLimitEditionMgr::getItemInfo(unsigned int ipgno) const
{
    std::map<unsigned int, CItemLimitEdition*>::const_iterator it = m_items.find(ipgno);
    if (it == m_items.end())
    {
        return 0;
    }
    return it->second;
}

bool CItemLimitEditionMgr::isEmpty() const
{
    return (char)m_items.empty();
}

void CItemLimitEditionMgr::clear()
{
    for (std::map<unsigned int, CItemLimitEdition*>::const_iterator it = m_items.begin(),
             endIt = m_items.end(); it != endIt; ++it)
    {
        register CItemLimitEdition* item = it->second;
        delete item;
    }
    m_items.clear();
}

// [DNF-NONIDENTICAL] DNF-MON-DIFF-0126 | monitor | 与ORIG差异=DIFF | CItemLimitEditionMgr::processScheduledJob | 详见 function_reports/monitor/_ZN20CItemLimitEditionMgr19processScheduledJobEP12CApplicationb.md
// [W5-MonitorRest] ndiff 69→**63** ✓（长度 194→190，ORIG 183）。已落地**机理 O**：pkt2 段删 `CServerHandler* h`
//   具名局部、改链式 `app->Get_ServerHandler()->SendToDB(&pkt2);` / `...SendAllTcpGameServer(&pkt2);`
//   ⇒ ORIG 该两处无 -0xc 槽、`mov %eax,(%esp)` 直接用返回值。
//   边界：**同一函数内另有一处同形 `h`（pkt 段，SendAllTcpGameServer(&pkt)）不可同样内联** ⇒ 试后 63→**170** 已回退（该处 ORIG 反而有 h）。
void CItemLimitEditionMgr::processScheduledJob(CApplication* app, bool flag)
{
    time_t now;
    if (m_items.empty())
    {
        return;
    }
    now = time(0);
    if (now - m_lastTime <= 4 && !flag)
    {
        return;
    }
    {
        m_lastTime = now;
        Packet_Item_Limit_Edition_Sell_end pkt;
        // ORIG 直接用包字段作计数器（pkt ctor 已把整包清零）：-0x9c 即 pkt+0xe(m_sellEndNum)，
        // 无独立 count 局部、无 `= 0` 初始化指令。
        // ORIG 在循环前缓存 end()（call 序 begin→C1→end→C1，本 TU 其它函数同形）；
        // 写成条件内调用 m_items.end() 会让 end()/const_iterator 转换的调用序后移。
        // 已试零变化的形态：直接初始化 `it(m_items.begin())`、以及 `it`/`endIt` 分句声明后再 for
        // （两者 TU 级与 ELF 级 ndiff 均恒为 57/69）⇒ 残留的 3 个迭代器临时槽属 gcc 对
        // 「iterator → const_iterator 用户转换」的展开路径差异，非源码可调。
        for (std::map<unsigned int, CItemLimitEdition*>::const_iterator it = m_items.begin(),
                 endIt = m_items.end(); it != endIt; ++it)
        {
            if ((int)it->second->getSellEndTime() < (int)now)
            {
                pkt.m_ipgNo[pkt.m_sellEndNum] = it->second->getIPGNO();
                pkt.m_sellEndNum++;
            }
        }
        if (pkt.m_sellEndNum != 0)
        {
            CServerHandler* h = app->Get_ServerHandler();
            h->SendAllTcpGameServer(&pkt);
            for (unsigned int i = 0; i < pkt.m_sellEndNum; i++)
            {
                std::map<unsigned int, CItemLimitEdition*>::iterator f =
                    m_items.find(pkt.m_ipgNo[i]);
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
            pkt2.m_serverGroup = (unsigned int)app->Get_ServerGroup();
            makeItemLimitEditionUpdatePacket(pkt2);
            app->Get_ServerHandler()->SendToDB(&pkt2);
            app->Get_ServerHandler()->SendAllTcpGameServer(&pkt2);
        }
    }
}

CItemLimitEdition::~CItemLimitEdition() {}

CItemLimitEdition::CItemLimitEdition(const stItemLimitEditionItemInfo_t& info)
{
    *((stItemLimitEditionItemInfo_t*)this) = info;
    m_sellNum = info.m_sellNum;
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

// [DNF-NONIDENTICAL] DNF-MON-NEAR-0013 | monitor | 与ORIG差异=NEAR | CItemLimitEdition::makeItemInfo | 详见 function_reports/monitor/_ZNK17CItemLimitEdition12makeItemInfoER28stItemLimitEditionItemInfo_t.md
void CItemLimitEdition::makeItemInfo(stItemLimitEditionItemInfo_t& info) const
{
    // 残差说明（标记见函数上方 DNF-MON-NEAR-0013；本件仍在 `compare_svc_full --non-id` 清单内）：
    // mdiff_ta 报 ndiff=2 = 唯一 1 组载入序互换。
    // 与 ORIG 仅差块拷贝的两条参数载入次序（ORIG 先 dst 0xc(%ebp) 后 src 0x8(%ebp)），其余 23 条逐条相同。
    // 已试 6 形态无效：memcpy 显式 · __builtin_memcpy · *(T*)&info = *(const T*)this（含等价嵌套）·
    //   引用绑定中转（帧 +4）· 内层块作用域 · 两层 cast 书写/嵌套变体。
    // 不可试 3 形态（前提不成立）：info = *this / static_cast<const T&>(*this) / const T& src = *this
    //   —— class CItemLimitEdition 无基类，与 stItemLimitEditionItemInfo_t 为布局镜像的独立类型 ⇒ 无隐式转换。
    // 结论：gcc block-move 求值序族，合法写法唯一、载入序由编译器决定 ⇒ 不可调 ✗ 勿重试。
    info = *((const stItemLimitEditionItemInfo_t*)this);
    info.m_sellNum = getSellNum();
}

unsigned int CItemLimitEdition::getSellNum() const { return m_sellNum; }

void CItemLimitEdition::updateSellNum(unsigned int num)
{
    m_sellNum = num;
}
