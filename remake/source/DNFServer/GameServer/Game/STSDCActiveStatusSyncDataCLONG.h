// HdrCreate3: ORIG exported symbols, first .h declaration.
// Evidence: nm ORIG + docs/class_func_reports/STSDCActiveStatusSyncDataCLONG/operator=.md
#ifndef DNF_GAME_STSDCACTIVESTATUSSYNCDATACLONG_H_
#define DNF_GAME_STSDCACTIVESTATUSSYNCDATACLONG_H_

class STSDCActiveStatusSyncDataSend;  // 布局见 DataManagerInitDepends.h:456（packed 0x35）

// ORIG T 0x0808d0a6 _ZN30STSDCActiveStatusSyncDataCLONGaSERK29STSDCActiveStatusSyncDataSend
// 布局证据（operator= 逐字段写入）：int@0x00/0x04（Send+0 byte 符号扩展）、uint@0x08
// （Send+2 零扩展）、int@0x0c/0x10（Send+3/+7 dword）、int@0x14/0x18（Send+0xb/0xc byte 符号
// 扩展）、int@0x1c（Send+0xd dword）、int@0x20/0x24/0x28（Send+0x11/0x15/0x19 dword）、
// int@0x2c..0x40（Send+0x1d/0x21/0x25/0x29/0x2d/0x31 dword）——共 17 个 int/uint，覆盖
// 0x00..0x43；sizeof 仅证实 ≥0x44，未证实总尺寸，不加 sizeof 断言。
class STSDCActiveStatusSyncDataCLONG
{
public:
    int  m_field00;   // +0x00 = (int)(char)send[0]
    int  m_field04;   // +0x04 = (int)(char)send[1]
    unsigned int m_field08;  // +0x08 = (uint)send[2]
    int  m_field0c;   // +0x0c = send dword@0x03
    int  m_field10;   // +0x10 = send dword@0x07
    int  m_field14;   // +0x14 = (int)(char)send[0xb]
    int  m_field18;   // +0x18 = (int)(char)send[0xc]
    int  m_field1c;   // +0x1c = send dword@0x0d
    int  m_field20;   // +0x20 = send dword@0x11
    int  m_field24;   // +0x24 = send dword@0x15
    int  m_field28;   // +0x28 = send dword@0x19
    int  m_field2c;   // +0x2c = send dword@0x1d
    int  m_field30;   // +0x30 = send dword@0x21
    int  m_field34;   // +0x34 = send dword@0x25
    int  m_field38;   // +0x38 = send dword@0x29
    int  m_field3c;   // +0x3c = send dword@0x2d
    int  m_field40;   // +0x40 = send dword@0x31

    STSDCActiveStatusSyncDataCLONG& operator=(STSDCActiveStatusSyncDataSend const&);  // ORIG T 0x0808d0a6
};

#endif  // DNF_GAME_STSDCACTIVESTATUSSYNCDATACLONG_H_
