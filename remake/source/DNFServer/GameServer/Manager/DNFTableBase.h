#ifndef DNF_TABLE_BASE_H_
#define DNF_TABLE_BASE_H_

#include <exception>
#include <string>

// ---- CDNFException：std::exception + std::string ----
class CDNFException : public std::exception
{
public:
    CDNFException(const std::string& msg);
    virtual ~CDNFException() throw();
    virtual const char* what() const throw();
private:
    std::string m_msg;  // +4
};

// ---- CTableBase ----
class CTableBase
{
public:
    CTableBase();
    virtual ~CTableBase();
    virtual int Load_Table(const std::string& fileName) = 0;
    virtual int Parse_Table(char* data, int size) = 0;
    int Load_Txt_Table_Data(const char* fileName, int idx);
};

#endif  // DNF_TABLE_BASE_H_
