// df_manager_r — CTableBase / CDNFException（ORIG DNFTableBase.cpp）
#include "DNFTableBase.h"

#include <stdio.h>

#include "DNFFileLog.h"

CTableBase::CTableBase() {}
CTableBase::~CTableBase() {}

int CTableBase::Load_Txt_Table_Data(const char* fileName, int idx)
{
    FILE* f = fopen(fileName, "r");
    if (!f)
        return -1;
    char buf[0x400];
    int count = 0;
    while (!feof(f) && fgets(buf, 0x400, f))
    {
        if (buf[0] == '#')
            continue;
        if (count >= idx)
            return -2;
        if (Parse_Table(buf, count))
            count++;
    }
    fclose(f);
    return count;
}

CDNFException::CDNFException(const std::string& msg) : m_msg(msg) {}
CDNFException::~CDNFException() throw() {}

const char* CDNFException::what() const throw()
{
    CMyFileLog log("what", 0x1a);
    log("./log/Except", "%s", m_msg.c_str());
    return m_msg.c_str();
}
