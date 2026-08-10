#ifndef DNF_TABLE_BASE_H_
#define DNF_TABLE_BASE_H_

#include <exception>
#include <string>

class CTableBase
{
public:
    CTableBase();
    virtual ~CTableBase();
    int Load_Txt_Table_Data(const char* path, int maxCount);
    virtual void Load_Table(const std::string& path) = 0;
    virtual bool Parse_Table(char* line, int idx) = 0;
};

// CDNFException：std::exception + std::string 消息@4
class CDNFException : public std::exception
{
public:
    CDNFException(const std::string& msg);
    virtual ~CDNFException() throw();
    virtual const char* what() const throw();
    std::string m_msg;
};

#endif // DNF_TABLE_BASE_H_
