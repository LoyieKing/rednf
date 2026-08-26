#ifndef GAME_WONGWORK_CCAPCHADATAMANAGER_H_
#define GAME_WONGWORK_CCAPCHADATAMANAGER_H_

#include <deque>

// ORIG 全局 POD（#pragma pack(2)；dtor W 0x082bcda4 空）。布局自 GlobalData
// GenerateCaptchaData 生产面 + reqHumanCertify 0x0867f26e 消费面双实证：
// +0x00 ushort m_text[5]（memcpy 0xa 至 user+0x8e0ec）、+0x0a int m_size、
// +0x0e uchar m_image[]（put_binary 载荷，长度 m_size）。
#pragma pack(push, 2)
struct MSG_UDP_CAPCHA_CERTIFY
{
    unsigned short m_text[5];             // +0x00
    int m_size;                           // +0x0a
    unsigned char m_image[0x339e - 0xe];  // +0x0e
};
#pragma pack(pop)

namespace WongWork
{

// ORIG sizeof 0x28 = std::deque<MSG_UDP_CAPCHA_CERTIFY>（全局 0x941f7a0）。
// C1 0x082a7002 / D1 0x082a7016 / isEmpty W 0x086977fa /
// peekData T 0x08299f74（随机取一条）/ push T 0x08299f3a（push_back 后
// 超 0x28 条弹出最旧）。
class CCapchaDataManager
{
public:
    CCapchaDataManager() {}
    ~CCapchaDataManager() {}

    bool isEmpty();
    MSG_UDP_CAPCHA_CERTIFY* peekData();
    void push(const MSG_UDP_CAPCHA_CERTIFY& msg);

    std::deque<MSG_UDP_CAPCHA_CERTIFY> m_queue;   // +0x00（0x28）
};

}  // namespace WongWork

#endif  // GAME_WONGWORK_CCAPCHADATAMANAGER_H_
