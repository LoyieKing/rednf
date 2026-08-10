#ifndef HW_SPEC_RESEARCHER_H_
#define HW_SPEC_RESEARCHER_H_

#include <map>

class CServerHandler;

// ---- HWSpec：0xc ----
struct HWSpec
{
    HWSpec();
    unsigned char m_field0;   // +0（0xff）
    unsigned int m_field4;    // +4（0xffffffff）
    unsigned int m_field8;    // +8（0xffffffff）
};

// ---- ErrorValue：0x8 ----
struct ErrorValue
{
    ErrorValue();
    ErrorValue(unsigned short value, unsigned int param);
    unsigned short m_field0;  // +0（0xffff）
    unsigned int m_field4;    // +4（0xffffffff）
};

// ---- STSpecStatic：0xc ----
struct STSpecStatic : public HWSpec
{
    STSpecStatic(const HWSpec& spec);
    bool operator<(const STSpecStatic& other) const;
};

// ---- STErrorStatic：0x8 ----
struct STErrorStatic : public ErrorValue
{
    STErrorStatic(const ErrorValue& value);
    bool operator<(const STErrorStatic& other) const;
};

// ---- CHWSpecResearcher：0x6C ----
class CHWSpecResearcher
{
public:
    CHWSpecResearcher();
    ~CHWSpecResearcher();
    void DBSaveProcess(CServerHandler* handler);
    void ResetErrorSpec();
    void SendDBMWHWSpec(CServerHandler* handler, unsigned char param);
    void WriteSpecStatics(unsigned char param, const HWSpec& spec);
    void SendDBMWErrorLine(CServerHandler* handler);
    void WriteErrorLineStatics(unsigned short param, int value);
    void ResetSpec();
    std::map<STSpecStatic, unsigned int> m_spec[3];      // +0x0/0x18/0x30
    char m_field48;                                      // +0x48
    int m_field4c;                                       // +0x4c
    std::map<STErrorStatic, unsigned int> m_errorSpec;   // +0x50
    char m_field68;                                      // +0x68
};

#endif // HW_SPEC_RESEARCHER_H_
