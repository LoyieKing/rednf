// df_coserver_r — CUserManager（ORIG DNFUserManager.cpp）
#include <utility>

#include "DNFUser.h"
#include "DNFGameServer.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

CUserManager::CUserManager()
{
    m_app = 0;
}

CUserManager::~CUserManager()
{
    for (int i = 0; i < 10; i++)
    {
        m_maps[i].clear();
    }
}

void CUserManager::Init(CApplication* app)
{
    m_app = app;
}

bool CUserManager::DeleteUser(Packet_DoubleCheck_UDP_Logout* pkt)
{
    unsigned int user_id = pkt->m_userID;
    std::map<const unsigned int, CUser>::iterator it = m_maps[user_id % 10].find(user_id);
    if (it == m_maps[user_id % 10].end())
    {
        return 0;
    }
    m_maps[user_id % 10].erase(it);
    return 1;
}

void CUserManager::CreateUser(unsigned int user_id, CGameServer* gs)
{
    bool eq = m_maps[user_id % 10].insert(
        std::pair<const unsigned int, CUser>(
            std::make_pair<const unsigned int&, CUser>(user_id, CUser(gs)))).second;
    if (!eq)
    {
        register char* num = NumberToString(user_id, 0);
        DNF_LOG_SCOPE_LINE(0x5f, "./log/LoginErr",
            "CreateUser() uDBID(%s) is already exist at m_mapUsers!", num);
    }
    return;
}

bool CUserManager::FindUser(unsigned int user_id, CUser& out)
{
    std::map<const unsigned int, CUser>::const_iterator it = m_maps[user_id % 10].find(user_id);
    if (it == m_maps[user_id % 10].end())
    {
        return 0;
    }
    // ORIG 实测尾部为：mov 0x10(%ebp),%edx; mov 0x4(%eax),%eax;
    // mov %eax,(%edx)（先取 out 地址再取 m_gs 值）。整结构赋值
    // （CUser 仅一个指针成员）可复现该求值顺序；逐成员赋值会反序。
    out = it->second;
    return 1;
}
