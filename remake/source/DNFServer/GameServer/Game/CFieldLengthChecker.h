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

    // ORIG 0x0861ca94 sync_script::DisplayMaxLength（非成员函数）直接引用
    // 0x94734a0/0x94734ac ⇒ 二者在 ORIG 中可被非成员访问，须为 public。
    static std::vector<std::string> fieldNames_;    // ORIG @0x94734a0
    static std::vector<unsigned int> fieldLength_;  // ORIG @0x94734ac
private:
    std::vector<std::string> m_fieldNames_;   // +0x00（ctor/dtor 只处理这一个成员）
};

#endif // DNF_GAME_CFIELDLENGTHCHECKER_H_
