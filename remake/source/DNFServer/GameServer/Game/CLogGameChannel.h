#ifndef _WONGWORK_CLOGGAMECHANNEL_H_
#define _WONGWORK_CLOGGAMECHANNEL_H_

namespace WongWork
{
class CLogGameChannel
{
public:
    CLogGameChannel();
    void StartLogTimer();
    void IncInUser();
    void IncOutUser();

private:
    char m_pad[0x10];
};
}

#endif // _WONGWORK_CLOGGAMECHANNEL_H_
