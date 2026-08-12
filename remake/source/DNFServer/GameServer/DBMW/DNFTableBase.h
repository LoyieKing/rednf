#ifndef DBMW_DNFTABLEBASE_H_
#define DBMW_DNFTABLEBASE_H_

#include "DBMWCommon.h"


class CDNFException : public std::exception
{
public:
    CDNFException(const std::string& msg);
    virtual ~CDNFException() throw();
    virtual const char* what() const throw();
private:
    std::string m_msg;  // +4
};

class CTableBase
{
public:
    CTableBase();
    virtual ~CTableBase();
    virtual int Load_Table(const std::string& fileName) = 0;   // vptr+0x8
    virtual bool Parse_Table(char* data, int size) = 0;        // vptr+0xc（ORIG 返回 bool）
    int Load_Txt_Table_Data(const char* fileName, int idx);    // 非虚
};

#endif  // DBMW_DNFTABLEBASE_H_
