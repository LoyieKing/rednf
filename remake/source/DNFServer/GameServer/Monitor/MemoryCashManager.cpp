// df_monitor_r — MemoryCashManager（从 MonitorTypes/App/Table 拆分）
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

#include "MemoryCashManager.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"
#include "BlackUser.h"
#include "CashObject.h"
#include "DNFApplication.h"
#include "DNFBuddy.h"
#include "DNFMember.h"
#include "DNFMemberManager.h"
#include "DNFUser.h"

void CMemoryCashManager::Init(CApplication* app)
{
    m_app = app;
    resetCashCnt();
}

CMemoryCashManager::CMemoryCashManager()
{
    m_app = 0;
}

CMemoryCashManager::~CMemoryCashManager() {}

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-MON-DIFF-0117 已消除）| CMemoryCashManager::ProcessLifeTimeOut
void CMemoryCashManager::ProcessLifeTimeOut()
{
    if (m_app)
    {
        for (std::map<unsigned int, CCashObject*>::iterator it = m_cashObjects.begin();
             it != m_cashObjects.end(); )
        {
            CCashObject* obj = it->second;
            if (obj->IsLifeTimeOut())
            {
                obj->DeleteMemberObject();
                obj->DeleteBuddys();
                obj->DeleteBlackUsers();
                m_cashObjects.erase(it++);
                delete obj;
            }
            else
            {
                ++it;
            }
        }
    }
    return;
}

// [DNF-NONIDENTICAL] DNF-MON-DIFF-0118 | monitor | 与ORIG差异=DIFF | CMemoryCashManager::QueryCashMemoryMember | 详见 function_reports/monitor/_ZN18CMemoryCashManager21QueryCashMemoryMemberEP5CUser.md
// [P-DIFF] 190/190 ndiff=6。余差 2 处：(a) 地址计算求值序（ORIG `imul/add 0x20/add dbInfo(内存)/add 8`；
//   我方先 `mov dbInfo,%eax`）；(b) 循环尾 1 条 jmp。已试不利（均回退）：dbInfo 声明为 STMemberDBInfo*（189 条）、
//   `(char*)dbInfo + i*0x27 + 0x28`（188 条）、C1「去 cast 层」修法。
char CMemoryCashManager::QueryCashMemoryMember(CUser* user)
{
    if (m_cashObjects.empty())
    {
        return 0;
    }
    unsigned int dbid = user->GetDBID();
    std::map<unsigned int, CCashObject*>::iterator it = m_cashObjects.find(dbid);
    if (it != m_cashObjects.end())
    {
        CCashObject* obj = it->second;
        if (obj->GetCharacNo() != user->GetUniqCharNo())
        {
            obj->DeleteMemberObject();
            return 0;
        }
        CMemberManager* mgr = m_app->Get_MemberManager();
        CMember* member = obj->GetMemberObject();
        if (member != 0)
        {
            unsigned int* dbInfo = member->GetMemberDBInfoW();
            std::string name;
            if (QueryUpdatedCharacName(*dbInfo, name))
            {
                memset((char*)dbInfo + 5, 0, 0x1e);
                strncpy((char*)dbInfo + 5, name.c_str(), 0x1d);
            }
            for (int i = 0; i < (int)((STMemberDBInfo*)dbInfo)->m_count27; i++)
            {
                unsigned int* sub = (unsigned int*)&((STMemberDBInfo*)dbInfo)->m_lowers[i];
                if (*sub != 0)
                {
                    if (QueryUpdatedCharacName(*sub, name))
                    {
                        memset((char*)sub + 5, 0, 0x1e);
                        strncpy((char*)sub + 5, name.c_str(), 0x1d);
                    }
                }
            }
        }
        char ok = mgr->LoadMemberFromCash(user, obj->GetMemberObject());
        if (ok != 0)
        {
            incMemberCashHitCnt();
        }
        obj->ClearMemberObject();
        return ok;
    }
    return 0;
}

// [DNF-NONIDENTICAL] DNF-MON-DIFF-0120 | monitor | 与ORIG差异=DIFF | CMemoryCashManager::QueryCashMemoryBuddyInfo | 详见 function_reports/monitor/_ZN18CMemoryCashManager24QueryCashMemoryBuddyInfoEP5CUser.md
// [P-DIFF] 180/180 ndiff=**4**（原 12 → 7 → 4）。已落地：
//   ① charNo 声明为 int（ORIG 的寄存器映射 ebx/esi 需此类型组合）；
//   ② **机理 B 显式寄存器绑定**：`register unsigned int uniqNo __asm__("ebx")` ⇒ uniqNo 落 %ebx 与 ORIG 一致（ndiff 7→4，长度 180 不变）。
//   边界实证（同一件三变体）：只绑 charNo→%esi ⇒ 180→**184**、ndiff **61**（帧 -0xa8→-0xb8 整体劣化）；
//     两个都绑 ⇒ 180→182、ndiff 13 ⇒ **仅在 uniqNo 单点绑定有效**，charNo 必须留给 GCC 自由分配。
//   余差 = 循环尾 1 条 jmp（ORIG 有）+ charNo 载入多一条中转（ORIG `mov 0x22(%eax),%esi` 一条；
//     我方 `mov 0x22(%eax),%eax; mov %eax,%esi` 两条）。
bool CMemoryCashManager::QueryCashMemoryBuddyInfo(CUser* user)
{
    if (m_cashObjects.empty())
    {
        return 0;
    }
    unsigned int dbid = user->GetDBID();
    std::map<unsigned int, CCashObject*>::iterator it = m_cashObjects.find(dbid);
    if (it != m_cashObjects.end())
    {
        CCashObject* obj = it->second;
        if (obj->GetCharacNo() != user->GetUniqCharNo())
        {
            obj->DeleteBuddys();
            return 0;
        }
        CBuddy* buddies[32];
        int count = obj->GetBuddysObject(buddies);
        for (int i = 0; i < count; i++)
        {
            if (buddies[i] != 0)
            {
                std::string name;
                if (QueryUpdatedCharacName(
                        ((STBuddyDBInfo*)buddies[i]->getBuddyDBInfo())->m_characNo, name))
                {
                    memset(buddies[i]->getBuddyDBInfo(), 0, 0x1e);
                    strncpy((char*)buddies[i]->getBuddyDBInfo(), name.c_str(), 0x1d);
                }
                user->AddBuddyFromCash(buddies[i]);
                register unsigned int uniqNo __asm__("ebx") = user->GetUniqCharNo();
                register int charNo =
                    ((STBuddyDBInfo*)buddies[i]->getBuddyDBInfo())->m_characNo;
                m_app->Get_BuddyRegisterManager()->addBuddyRegister(charNo, uniqNo);
            }
        }
        if (count != 0)
        {
            CUserManager* mgr = m_app->Get_UserManager();
            mgr->SendConnectedBuddysList(user);
        }
        user->SetBuddyDBFlag(4);
        incBuddyCashHitCnt();
        return 1;
    }
    return 0;
}

// [DNF-NONIDENTICAL] DNF-MON-DIFF-0119 | monitor | 与ORIG差异=DIFF | CMemoryCashManager::QueryCashMemoryBlackList | 详见 function_reports/monitor/_ZN18CMemoryCashManager24QueryCashMemoryBlackListEP5CUser.md
// [P-DIFF] 173/173 ndiff=4（原 172/ndiff=10）。已落地两处：
//   ① **落点 D 的迭代版**：循环体 `if (bu == 0) { continue; }` + 裸复合块包 body ⇒ 复现 ORIG 循环体末尾 join jmp
//      （长度 172→173；VillageLane `UpdateHuntingPoint` 独立复现同形）。
//   ② **(a2) 公共代码归属修正**：`RegisterToCashBlackList/ClearMapBlackUsers/SetBlackListDBFlag/inc/return 1`
//      原被我方误放在 `if (obj != 0)` **块内**，ORIG 是 `if` **之后**的公共代码（ORIG `je <#147>` 指向该公共块、
//      我方曾指 `return 0`）⇒ 移出后 ndiff 5→4。**这是行为差异，非 RA 差异**。
//   余差 1 处 = 实参求值序（ORIG 先 `mov key,%eax` 再 `lea &name`；我方相反）；已试 `const key`/`int key` 无效。
//   2026-09-15 追加（机理 B 边界）：显式绑定不适用 —— `register unsigned int key __asm__("eax")` ⇒ 长度 173→**171**、
//     ndiff 4→**45**（eax 被占用引发全函数分配劣化，已回退）⇒ 该件仅剩「实参求值序 + %edx/%eax 选择」轴，无源码可调面。
//   反例（切勿批量套用 D 形态）：QueryCashMemoryMember（→191/16）与 BuddyInfo（→181/14）均劣化，已回退。
bool CMemoryCashManager::QueryCashMemoryBlackList(CUser* user)
{
    unsigned int dbid = user->GetDBID();
    std::map<unsigned int, CCashObject*>::iterator it = m_cashObjects.find(dbid);
    if (it != m_cashObjects.end())
    {
        CCashObject* obj = it->second;
        if (obj != 0)
        {
            std::map<unsigned int, CBlackUser*>* blackMap = obj->GetBlackUsersObject();
            for (std::map<unsigned int, CBlackUser*>::iterator bi = blackMap->begin();
                 bi != blackMap->end(); ++bi)
            {
                unsigned int key = (*bi).first;
                CBlackUser* bu = (*bi).second;
                if (bu == 0)
                {
                    continue;
                }
                {
                    std::string name;
                    if (QueryUpdatedCharacName(key, name))
                    {
                        char buf[0x1e] = {0};
                        strncpy(buf, name.c_str(), 0x1d);
                        bu->ChangeCharName(buf);
                    }
                }
            }
        }
        user->RegisterToCashBlackList(*obj->GetBlackUsersObject());
        obj->ClearMapBlackUsers();
        user->SetBlackListDBFlag(4);
        incBlackListCashHitCnt();
        return 1;
    }
    return 0;
}

bool CMemoryCashManager::QueryUpdatedCharacName(unsigned int charNo, std::string& name)
{
    std::map<unsigned int, std::string>::iterator it = m_names.find(charNo);
    if (m_names.end() == it)
    {
        return false;
    }
    name = (*it).second;
    return true;
}

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（本轮翻正：end()==it 比较方向 + (*it).second + 形参顶层 const 选非模板 pair ctor）
void CMemoryCashManager::InsertUpdatedCharacName(const unsigned int dbid, const std::string& name)
{
    std::map<unsigned int, std::string>::iterator it = m_names.find(dbid);
    if (m_names.end() == it)
    {
        m_names.insert(std::pair<const unsigned int, std::string>(dbid, name));
    }
    else
    {
        (*it).second = name;
    }
}

void CMemoryCashManager::incMemberCashHitCnt()
{
    m_memberCashHitCnt = m_memberCashHitCnt + 1;
}

void CMemoryCashManager::incBuddyCashHitCnt()
{
    m_buddyCashHitCnt = m_buddyCashHitCnt + 1;
}

void CMemoryCashManager::incBlackListCashHitCnt()
{
    m_blackListCashHitCnt = m_blackListCashHitCnt + 1;
}

void CMemoryCashManager::incBuddyCashCnt()
{
    m_buddyCashCnt = m_buddyCashCnt + 1;
}

void CMemoryCashManager::incMemberCashCnt()
{
    m_memberCashCnt = m_memberCashCnt + 1;
}

void CMemoryCashManager::incBlackListCashCnt()
{
    m_blackListCashCnt = m_blackListCashCnt + 1;
}

bool CMemoryCashManager::IsRightObject(CUser* user, CMember* member, bool& flag1, bool& flag2,
                                       bool& flag3)
{
    bool ret = false;
    flag1 = false;
    flag3 = false;
    flag2 = false;
    std::map<unsigned int, CCashObject*>::iterator it = m_cashObjects.find(user->GetDBID());
    if (it != m_cashObjects.end())
    {
        return 0;
    }
    if ((user->GetBuddyDBFlag() & 4) != 0)
    {
        flag1 = true;
        ret = true;
    }
    if ((user->GetBlackListDBFlag() & 4) != 0)
    {
        flag3 = true;
        ret = true;
    }
    if (user->GetMemberKey() != 0)
    {
        if ((user->GetMemberDBFlag() & 4) != 0)
        {
            if (member != 0 && member->IsAbleToRegisterMember() != 0)
            {
                flag2 = true;
                ret = true;
            }
        }
    }
    return ret;
}

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-MON-DIFF-0121 已消除）| CMemoryCashManager::InsertCashMemorySetCharacterObject
// [P-NOTE] 137/137 ndiff=0（mdiff_ta STRICT）。翻正 = 两处联动，缺一即 NEAR：
//   ① 删去重建期多加的 `char reservedSlot[8]` 占位（该占位只在「具名 r」前提下才把 local/obj 推低 8）；
//   ② `std::pair<map::iterator,bool> r = insert(...); if (!r.second)` 就地内联为 `if (!insert(...).second)`
//      ⇒ 具名局部 r 消失，两个 8 字节标量块（r 块 ↔ local/obj 块）的栈分配次序与 ORIG 一致。
//   对照实测：只删占位 → 10；只内联 r（留占位）→ 10；两者同时 → 0。
//   前轮基础修复仍有效：IsRightObject 返回类型 bool（调用点 sete→test）、正向 if 包裹 + `!r.second` 双 return（xor $0x1 形态）。
int CMemoryCashManager::InsertCashMemorySetCharacterObject(CUser* user, CMember* member,
                                                           bool& flag1, bool& flag2)
{
    bool local = false;
    if (IsRightObject(user, member, flag1, flag2, local))
    {
        CCashObject* obj = new CCashObject;
        if (flag1 || flag2)
        {
            obj->SetCharacNo(user->GetUniqCharNo());
        }
        if (flag2 && member != 0)
        {
            obj->SetMemberObject(member);
            incMemberCashCnt();
        }
        if (flag1)
        {
            CBuddy* buddies[0x20];
            int n = user->GetBuddys(buddies);
            obj->SetBuddysObject(buddies, n);
            incBuddyCashCnt();
        }
        if (!m_cashObjects.insert(std::make_pair(user->GetDBID(), obj)).second)
        {
            delete obj;
            return 0;
        }
        return 1;
    }
    return 0;
}

bool CMemoryCashManager::SetUserObject(CUser* user)
{
    std::map<unsigned int, CCashObject*>::iterator it = m_cashObjects.find(user->GetDBID());
    if (it != m_cashObjects.end())
    {
        CCashObject* obj = it->second;
        obj->SetBlackUsersObject(*user->GetMapBlackList());
        incBlackListCashCnt();
        obj->SetLifeTime(5);
        return 1;
    }
    return 0;
}

void CMemoryCashManager::DeleteCashObjecct(unsigned int dbid)
{
    std::map<unsigned int, CCashObject*>::iterator it = m_cashObjects.find(dbid);
    if (it != m_cashObjects.end())
    {
        delete it->second;
        m_cashObjects.erase(it);
    }
}

void CMemoryCashManager::resetCashCnt()
{
    m_cashCnt34 = 0x1e;
    m_buddyCashCnt = 0;
    m_memberCashCnt = 0;
    m_blackListCashCnt = 0;
    m_buddyCashHitCnt = 0;
    m_memberCashHitCnt = 0;
    m_blackListCashHitCnt = 0;
}
