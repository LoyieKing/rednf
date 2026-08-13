#ifndef HW_SPEC_RESEARCHER_H_
#define HW_SPEC_RESEARCHER_H_

#include <map>

class CServerHandler;

// ---- HWSpec：0xc ----
struct HWSpec
{
    HWSpec();
    unsigned char m_category1;  // +0（log_hardware_ting.category1，0xff 默认）
    unsigned int m_category2;   // +4（log_hardware_ting.category2，0xffffffff 默认）
    unsigned int m_category3;   // +8（log_hardware_ting.category3，0xffffffff 默认）
};

// ---- ErrorValue：0x8 ----
struct ErrorValue
{
    ErrorValue();
    ErrorValue(unsigned short value, unsigned int param);
    unsigned short m_errorLine;  // +0（log_packet_dispatcher_error_line.error_line，0xffff 默认）
    unsigned int m_errorCode;    // +4（log_packet_dispatcher_error_line，0xffffffff 默认）
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
    unsigned char m_specSaveTick;                        // +0x48（DBSaveProcess 保存计数）
    int m_field4c;                                       // +0x4c
    std::map<STErrorStatic, unsigned int> m_errorSpec;   // +0x50
    unsigned char m_errorSaveTick;                       // +0x68（DBSaveProcess 保存计数）
};

#endif // HW_SPEC_RESEARCHER_H_
