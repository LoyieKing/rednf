// CFieldLengthChecker.h — ORIG 符号对齐（C1/C2/D1/D2/AddField 全部 W 弱符号，
// 实现在 CFieldLengthChecker.cpp；无 vptr → 非虚析构）。
#ifndef DNF_GAME_CFIELDLENGTHCHECKER_H_
#define DNF_GAME_CFIELDLENGTHCHECKER_H_

#include <vector>
#include <string>

class CFieldLengthChecker
{
public:
    CFieldLengthChecker();
    ~CFieldLengthChecker();
    void AddField(char const*);
private:
    std::vector<std::string> m_fieldNames_;   // +0x00（ctor/dtor 只处理这一个成员）
    static std::vector<std::string> fieldNames_;    // ORIG @0x94734a0
    static std::vector<unsigned int> fieldLength_;  // ORIG @0x94734ac
};

#endif // DNF_GAME_CFIELDLENGTHCHECKER_H_
