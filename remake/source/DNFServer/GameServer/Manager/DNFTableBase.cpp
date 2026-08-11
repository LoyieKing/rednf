// df_manager_r — CTableBase / CDNFException（ORIG DNFTableBase.cpp）
#include "DNFTableBase.h"

#include <stdio.h>

#include "DNFFileLog.h"

CTableBase::CTableBase() {}
CTableBase::~CTableBase() {}

int CTableBase::Load_Txt_Table_Data(const char* fileName, int idx)
{
    int count = 0;
    FILE* f;
    if ((f = fopen(fileName, "rb")) == NULL)
        return -1;
    char buf[0x400];
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
    // ORIG：operator() 实参（m_msg.c_str()）在临时构造前求值并装入 ebx
    // （callee-saved），返回处再次调用 c_str() —— 宏临时对象形式逐条一致。
    DNF_LOG_SCOPE_LINE(0x1a, "./log/Except", "%s", m_msg.c_str());
    return m_msg.c_str();
}
