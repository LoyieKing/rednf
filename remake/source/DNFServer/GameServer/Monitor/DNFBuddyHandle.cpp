// df_monitor_r — DNFBuddyHandle（从 MonitorTypes/App/Table 拆分）
#include <stdio.h>
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

#include "DNFBuddyHandle.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"
#include "DNFBuddy.h"
#include "DNFMember.h"
#include "DNFPacketTranslater.h"
#include "DNFServerHandler.h"
#include "DNFUser.h"

CBuddyHandle::CBuddyHandle()
{
    m_prUser = 0;
    m_buddyDBFlag = 0;
}

unsigned short CBuddyHandle::GetBuddyDBFlag() { return m_buddyDBFlag; }

void CBuddyHandle::SetBuddyDBFlag(unsigned short flag) { m_buddyDBFlag |= flag; }

CBuddyHandle::~CBuddyHandle() { reset(0, 0); }

// [DNF-NONIDENTICAL] DNF-MON-DIFF-0009 | monitor | 与ORIG差异=DIFF | CBuddyHandle::addDB | 详见 function_reports/monitor/_ZN12CBuddyHandle5addDBEP14CServerHandlerPc.md
int CBuddyHandle::addDB(CServerHandler* handler, char* name)
{
    // [MonitorRest 本轮] 150/161 DIFF ndiff=68（未归零）：ORIG 的 std::string key 槽 -0x14/alloc -0xd，
    //   我方 key=-0x10/alloc=-0x19；且 log 分支 ORIG `mov $0x1,%eax` vs 我方 `%ebx`（EH 路径寄存器选择）。
    //   已试无效/劣化：① 删 `std::allocator<char> alloc` 改用默认构造（68→71）；
    //   ② string 具名→临时。
    //   [MonitorRest W3 本轮] ndiff 68→**29**（长度 161→155，ORIG 150）。两步（均保留）：
    //   ③ **迭代器外提到 key 作用域之外**（`iterator it;` + 块内 `it = find(key)` + 块外比较）⇒
    //      复现 ORIG 的 `find → ~string → ~allocator → end() → operator!=` 调用序（判据：ORIG 的
    //      end()/!=() 位于 key 析构之后，说明比较是独立语句、不在 find 行内）；
    //   ④ `strlen(name) < 0x1e {…} return -1;` 改为 **`strlen(name) >= 0x1e { return -1; }` + 裸块**
    //      ⇒ 复现 ORIG 的 `jbe`（跳过 -1 返回）而非我方 `ja` 跳到函数尾。
    //   余 29 = find 的 sret 临时（我方 `it = find()` 经临时再拷贝；ORIG 直接落 it 槽）+
    //   我方多一次 `_Rb_tree_iterator::C1()` 默认构造（ORIG 无）⇒ 需"声明即初始化且 key 已析构"，
    //   两者在 C++ 作用域下互斥，已穷举 3 形态。
    if (m_prUser == 0 || m_prUser->GetUniqCharNo() == 0)
    {
        DNF_LOG_SCOPE_LINE(0x5a, "./log/buddy", "Buddy::addDB m_prUser is NULL");
        return 1;
    }
    if (name == 0)
    {
        return 1;
    }
    if (m_buddies.size() > 0x1f)
    {
        return 4;
    }
    std::map<std::string, CBuddy*>::iterator it =
        m_buddies.find(std::string(name, std::allocator<char>()));
    if (it != m_buddies.end())
    {
        return 0x12;
    }
    if (strcmp(name, m_prUser->GetCharName()) == 0)
    {
        return 1;
    }
    if (strlen(name) >= 0x1e)
    {
        return -1;
    }
    {
        Packet_DBMW_Add_Buddy pkt;
        pkt.m_uniqCharNo = m_prUser->GetUniqCharNo();
        memcpy(pkt.m_charName, name, 0x1d);
        handler->SendToDB(&pkt);
        return 0;
    }
}

int CBuddyHandle::delDB(CServerHandler* handler, char* name)
{
    if (m_prUser == 0 || m_prUser->GetUniqCharNo() == 0)
    {
        DNF_LOG_SCOPE_LINE(0xb7, "./log/buddy", "Buddy::addDB m_prUser is NULL");
        return 1;
    }
    if (name == 0)
    {
        return 1;
    }
    if (strlen(name) > 0x1d)
    {
        return -1;
    }
    std::map<std::string, CBuddy*>::iterator it = m_buddies.find(name);
    if (it == m_buddies.end())
    {
        return 0x15;
    }
    Packet_DBMW_Del_Buddy pkt;
    pkt.m_uniqCharNo = m_prUser->GetUniqCharNo();
    pkt.m_buddyCharNo = ((STBuddyDBInfo*)it->second->getBuddyDBInfo())->m_characNo;
    memcpy(pkt.m_charName, name, 0x1d);
    handler->SendToDB(&pkt);
    return 0;
}

void CBuddyHandle::setBuddyCharName(int charNo, const std::string& newName)
{
    for (std::map<std::string, CBuddy*>::iterator it = m_buddies.begin();
         it != m_buddies.end(); ++it)
    {
        // P0 槽-局部律：ORIG 该值经具名槽 -0x2c 往返 ⇒ 源码有具名局部且声明在 if 之前。
        CBuddy* buddy = it->second;
        if (buddy != 0)
        {
            if (charNo == ((STBuddyDBInfo*)buddy->getBuddyDBInfo())->m_characNo)
            {
                m_buddies.erase(it);
                m_buddies.insert(std::make_pair(newName, buddy));
                break;
            }
        }
    }
}

int CBuddyHandle::getBuddysCharNo(unsigned int* out)
{
    if (m_buddies.empty())
    {
        return 0;
    }
    int count = 0;
    std::map<std::string, CBuddy*>::iterator it = m_buddies.begin();
    std::map<std::string, CBuddy*>::iterator end = m_buddies.end();
    for (; it != end; ++it)
    {
        CBuddy* b = it->second;
        if (b != 0)
        {
            out[count] = ((STBuddyDBInfo*)b->getBuddyDBInfo())->m_characNo;
        }
        count++;
        if (0x20 < count)
        {
            DNF_LOG_SCOPE_LINE(0x135,"./log/buddy", "CBuddyHandle::GetBuddysCharNo iCnt(%d) > MAX_BUDDY_COUNT(%d)",
                count, 0x20);
            return 0x20;
        }
    }
    return count;
}

int CBuddyHandle::getBuddys(CBuddy** out)
{
    if (m_buddies.empty())
    {
        return 0;
    }
    int count = 0;
    std::map<std::string, CBuddy*>::iterator it = m_buddies.begin();
    std::map<std::string, CBuddy*>::iterator end = m_buddies.end();
    for (; it != end; ++it)
    {
        out[count] = it->second;
        count++;
        if (0x20 < count)
        {
            DNF_LOG_SCOPE_LINE(0x153,"./log/buddy", "CBuddyHandle::GetBuddysCharNo iCnt(%d) > MAX_BUDDY_COUNT(%d)",
                count, 0x20);
            return 0x20;
        }
    }
    return count;
}

int CBuddyHandle::del(std::string name)
{
    std::map<std::string, CBuddy*>::iterator it = m_buddies.find(name);
    if (it == m_buddies.end())
    {
        return 1;
    }
    if (it->second != 0)
    {
        delete it->second;
    }
    m_buddies.erase(it);
    return 0;
}

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-MON-DIFF-0007 已消除）
bool CBuddyHandle::addFromCash(CBuddy* buddy)
{
    if (m_buddies.size() > 0x1f)
    {
        return 0;
    }
    return m_buddies.insert(
               std::make_pair(((STBuddyDBInfo*)buddy->getBuddyDBInfo())->m_name,
                              buddy))
        .second;
}

void CBuddyHandle::insert(CBuddy* buddy)
{
    if (m_buddies.size() > 0x1f)
    {
        return;
    }
    m_buddies.insert(
        std::make_pair(((STBuddyDBInfo*)buddy->getBuddyDBInfo())->m_name, buddy));
}

CBuddy* CBuddyHandle::findBuddy(std::string name)
{
    if (m_buddies.empty())
    {
        return 0;
    }
    std::map<std::string, CBuddy*>::iterator it = m_buddies.find(name);
    if (it != m_buddies.end())
    {
        return it->second;
    }
    return 0;
}

CBuddy* CBuddyHandle::findBuddyByCharNo(unsigned int charNo)
{
    std::map<std::string, CBuddy*>::iterator it;
    std::map<std::string, CBuddy*>::iterator it2 = m_buddies.begin();
    for (; it2 != m_buddies.end(); ++it2)
    {
        CBuddy* buddy = it2->second;
        if (buddy != 0 &&
            ((STBuddyDBInfo*)buddy->getBuddyDBInfo())->m_characNo == charNo)
        {
            return it2->second;
        }
    }
    return 0;
}

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-MON-DIFF-0008 已消除）
void CBuddyHandle::printBuddys(char* out)
{
    if (m_prUser != 0)
    {
        if (m_buddies.empty())
        {
        }
        else
        {
            std::map<std::string, CBuddy*>::iterator it = m_buddies.begin();
            std::map<std::string, CBuddy*>::iterator end = m_buddies.end();
            while (it != end)
            {
                CBuddy* buddy = it->second;
                DNF_LOG_SCOPE_LINE(0x16e,"./log/buddy",
                    "[%s] name(%s) fname(%s) flevel(%d) fjob(%d) fgrowtype(%d) fcharNo(%d) "
                    "fsex(%d)",
                    out, m_prUser->GetCharName(), (char*)buddy->getBuddyDBInfo(),
                    (int)((STBuddyDBInfo*)buddy->getBuddyDBInfo())->m_lev,
                    (int)((STBuddyDBInfo*)buddy->getBuddyDBInfo())->m_job,
                    (int)((STBuddyDBInfo*)buddy->getBuddyDBInfo())->m_growType,
                    ((STBuddyDBInfo*)buddy->getBuddyDBInfo())->m_characNo,
                    (int)((STBuddyDBInfo*)buddy->getBuddyDBInfo())->m_sex);
                ++it;
            }
        }
    }
    return;
}

int CBuddyHandle::add(std::string name, STBuddyDBInfo& info)
{
    // [MonitorRest 本轮] 103/104 DIFF ndiff=18（未归零）：唯一差异是 buddy 局部槽位
    //   （ORIG buddy=-0x34 最深，我方=-0x2c；两侧槽集合完全相同）。
    //   已试无效：① `buddy` 声明下移到 try 内；② insert 返回 pair 拆成先声明后赋值（→113 劣化）；
    //   ③ 恢复外层块包裹。登记为「BIND/临时对象分配顺序」族。
    if (m_buddies.size() > 0x1f)
    {
        return 0;
    }
    CBuddy* buddy;
    try
    {
        buddy = new CBuddy(info);
        if (m_buddies.insert(std::make_pair(name, buddy)).second)
        {
            return 1;
        }
        delete buddy;
        return 0;
    }
    catch (...)
    {
        return 0;
    }
}

void CBuddyHandle::reset(CUser* user, bool flag)
{
    m_prUser = user;
    m_buddyDBFlag = 0;
    clear(flag);
}

void CBuddyHandle::clear(bool flag)
{
    if (m_buddies.empty())
    {
    }
    else
    {
        if (flag)
        {
            for (std::map<std::string, CBuddy*>::iterator it = m_buddies.begin();
                 it != m_buddies.end(); ++it)
            {
                if (it->second != 0)
                {
                    delete it->second;
                }
            }
        }
        m_buddies.clear();
    }
    return;
}
