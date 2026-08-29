// CountryCode —— ORIG-symbol-aligned declarations.
// Layout/签名证据：docs/class_func_reports/CountryCode/（汇编+反编译 C）；
// 静态成员初值证据：_Z41__static_initialization_and_destruction_0ii @0x08ad3356。
#ifndef DNF_GAME_COUNTRYCODE_H_
#define DNF_GAME_COUNTRYCODE_H_

#include <string>

class CountryCode
{
public:
    CountryCode(char const*);
    CountryCode(wchar_t const*);
    CountryCode(unsigned char);
    CountryCode();

    void setInternalValue(unsigned char);
    CountryCode& operator=(char const*);
    CountryCode& operator=(wchar_t const*);
    std::string getAlpha2Code() const;      // ORIG tstring（sret 返回对象）
    std::string getCountryName() const;     // ORIG tstring
    unsigned char getInternalValue() const; // ORIG byte
    unsigned char const* getAlpha2CodeByte() const; // ORIG byte* → &ccTable[idx].alpha2_1
    bool isNone() const;                    // ORIG bool（test/sete）
    bool operator==(CountryCode) const;
    bool operator!=(CountryCode) const;

    static CountryCode none;    // 0x08ad3356：C1Eh(0x00) @ 0x9501a20
    static CountryCode global;  // C1Eh(0xf1) @ 0x9501a21
    static CountryCode korea;   // C1Eh(0x76) @ 0x9501a22
    static CountryCode japan;   // C1Eh(0x6e) @ 0x9501a23
    static CountryCode china;   // C1Eh(0x2f) @ 0x9501a24
    static CountryCode taiwan;  // C1Eh(0xdb) @ 0x9501a25
    static CountryCode usa;     // C1Eh(0xe0) @ 0x9501a26

private:
    unsigned char m_idx;        // +0x00（1 字节；报告 Ghidra idx_）
};

#endif // DNF_GAME_COUNTRYCODE_H_
