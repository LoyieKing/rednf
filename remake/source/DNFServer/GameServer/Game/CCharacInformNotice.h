#ifndef DNF_GAME_CCHARACINFORMNOTICE_H_
#define DNF_GAME_CCHARACINFORMNOTICE_H_

// ORIG C1 0x080e4790：CData ctor + vptr + _reset()。
// 布局：pack(1) CData（vptr+0 / m_bIsSave+4）后 m_flags[0x10] @+0x05。
#include <cstring>
#include <vector>
#include "CBoosterGage.h"

class CUser;

#pragma pack(push, 1)
class CCharacInformNotice : public charac_expand::CData
{
public:
    CCharacInformNotice();
    virtual ~CCharacInformNotice();
    unsigned char getInformNoticeFlag(int);
    void sendInformNoticeFlag(CUser*);
    bool checkInformNoticeFlag(unsigned int bit, unsigned int byteIdx);
    void handleInformNoticeFlag(unsigned int flag);
    void getInformNoticeFlagIndex(std::vector<unsigned char, std::allocator<unsigned char> >&);
    virtual void _reset();
    virtual bool loadData(CUser*, char*);
    virtual bool _saveData(CUser*);
    virtual void getData(char*) const;

    unsigned char m_flags[0x10];  // +0x05（ORIG _reset 0x80e4ab8 memset +5,0x10）
    char m_pad15[3];              // +0x15..0x18（ORIG new 0x18=24 实证：pack(1)
                                   //   5+16 后尾对齐 3——同 CItemDictionary 模式）
};
#pragma pack(pop)

#endif // DNF_GAME_CCHARACINFORMNOTICE_H_
