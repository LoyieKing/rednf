#ifndef DNF_TICK_HANDLER_H_
#define DNF_TICK_HANDLER_H_

class CApplication;

// CFrameCountHandler：0x30
class CFrameCountHandler
{
public:
    CFrameCountHandler();
    void InitFrameCountInfo(CApplication* app, unsigned int frameCount, unsigned short tick);
    CFrameCountHandler* GetFrameCountInfo();
    void SaveProcess();
    char m_state0;             // +0
    unsigned int m_tick;       // +4
    unsigned int m_framePerTick; // +8
    unsigned int m_startTime;  // +0xc
    unsigned int m_endTime;    // +0x10
    unsigned int m_frameCount; // +0x14
    unsigned int m_fps;        // +0x18
    char m_pad[0x8];           // +0x1c
    char m_state;              // +0x24
    char m_counter1;           // +0x25
    char m_counter2;           // +0x26
    char m_pad2;               // +0x27
    char m_writeTick;          // +0x28
    char m_pad3[3];            // +0x29
    unsigned int m_value;      // +0x2c
};

#endif // DNF_TICK_HANDLER_H_
