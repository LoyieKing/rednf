#ifndef SCRIPTDATA_H_
#define SCRIPTDATA_H_

// ScriptData：全局配置数据
// 布局：0:int relayNum=100; 4/6:ushort port; 8:ipA[17]; 0x1a:ushort;
//       0x1c:ushort; 0x1e:ipB[17]; 0x30/0x32:ushort; 0x34:bool
struct ScriptData
{
    int mRelayNum;
    unsigned short mPortTcp;
    unsigned short mPortUdp;
    char mServerIpA[17];
    unsigned short mReservedA;
    unsigned short mReservedB;
    char mServerIpB[17];
    unsigned short mReservedC;
    unsigned short mReservedD;
    bool mFlag;

    void clear();
    ScriptData();
    ~ScriptData();
};

ScriptData* G_ScriptData();

#endif // SCRIPTDATA_H_
