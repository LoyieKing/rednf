#ifndef DNFTABLEBASE_H_
#define DNFTABLEBASE_H_

#include <map>
#include <string>
#include <vector>

#include "PacketHeader.h"

class CDNFException;
class CTableBase;

// from GuildTable.h
class CTableBase
{
public:
    CTableBase();
    virtual ~CTableBase();
    int Load_Txt_Table_Data(const char* path, int maxCount);
    virtual void Load_Table(const std::string& path) = 0;
    virtual bool Parse_Table(char* line, int idx) = 0;
};

// from GuildTable.h
class CDNFException : public std::exception
{
public:
    CDNFException(const std::string& msg);
    virtual ~CDNFException() throw();
    virtual const char* what() const throw();
    std::string m_msg;
};

#endif
