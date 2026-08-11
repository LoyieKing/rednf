// df_statics_r — DNFTableBase（ORIG DNFTableBase.o 拆分）
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "DNFTableBase.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

CDNFException::CDNFException(const std::string& msg)
    : std::exception(), m_msg(msg)
{
}
const char* CDNFException::what() const throw()
{
    register const char* msg = m_msg.c_str();
    DNF_LOG_SCOPE_LINE(0x1a, "./log/Except", "%s", msg);
    return m_msg.c_str();
}
CDNFException::~CDNFException() throw()
{
}
CTableBase::~CTableBase()
{
}
int CTableBase::Load_Txt_Table_Data(const char* path, int maxCount)
{
    int count = 0;
    FILE* f;
    if ((f = fopen(path, "rb")) == 0)
    {
        return -1;
    }
    else
    {
        char line[1024];
        while (!feof(f) && fgets(line, 0x400, f) != 0)
        {
            if (line[0] == '#')
            {
                continue;
            }
            if (count >= maxCount)
            {
                return -2;
            }
            if (Parse_Table(line, count))
            {
                count++;
            }
        }
        fclose(f);
    }
    return count;
}
CTableBase::CTableBase()
{
}
