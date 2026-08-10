// df_manager_r — CKillUSRConfig / ST_KillUSRConfig（ORIG DNFKillUserConfig.cpp）
#include "DNFKillUserConfig.h"

#include <new>
#include <stdio.h>
#include <stdlib.h>

#include "DNFTableBase.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

ST_KillUSRConfig::ST_KillUSRConfig()
{
    *(int*)((char*)this + 0) = 0;
    *(int*)((char*)this + 4) = 0;
    *(int*)((char*)this + 8) = 0;
    *(int*)((char*)this + 0xc) = 0;
}

CKillUSRConfig::CKillUSRConfig() {}
CKillUSRConfig::~CKillUSRConfig()
{
    Clear_Table();
}
int CKillUSRConfig::Load_Table(const std::string& fileName)
{
    int n = Load_Txt_Table_Data(fileName.c_str(), 0x64);
    if (n > 0 && n <= 0x64)
        return 1;
    CMyFileLog log("CKillUSRConfig::Load_Table", 0x5b);
    log("./log/Config", "CKillUSRConfig Load_Table() fail(%d)", n);
    throw CDNFException("CKillUSRConfig::Load_Table() fail!");
}
int CKillUSRConfig::Parse_Table(char* data, int size)
{
    if (data[0] == '#')
        return 0;
    char* fields[4];
    if (DNFFLib::ExplodeString(data, " \t\r\n\"", fields, 4) == 4)
    {
        ST_KillUSRConfig* kc = new (std::nothrow) ST_KillUSRConfig;
        if (!kc)
            return 0;
        kc->m_type = atoi(fields[0]);
        kc->m_field4 = atoi(fields[1]);
        kc->m_field8 = atoi(fields[2]);
        kc->m_fieldC = atoi(fields[3]);
        m_list.push_back(kc);
        return 1;
    }
    return 0;
}
void CKillUSRConfig::Clear_Table()
{
    if (!m_list.empty())
    {
        for (std::vector<ST_KillUSRConfig*>::iterator it = m_list.begin();
             it != m_list.end(); ++it)
        {
            ST_KillUSRConfig* p = *it;
            delete p;
            p = 0;
        }
        m_list.clear();
    }
}

void* CKillUSRConfig::GetInfo() const { return (void*)&m_list; }

// 强制发出原版存在的 libstdc++ 分配器弱符号
template class std::allocator<ST_KillUSRConfig*>;
