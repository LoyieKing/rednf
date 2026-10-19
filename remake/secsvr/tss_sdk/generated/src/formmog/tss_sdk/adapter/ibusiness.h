// Auto-generated header stub from DWARF info
// Original path: src/formmog/tss_sdk/adapter/ibusiness.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_FORMMOG_TSS_SDK_ADAPTER_IBUSINESS_H_H_
#define SECSVR_SRC_FORMMOG_TSS_SDK_ADAPTER_IBUSINESS_H_H_

// sizeof = 6
struct SSafeModeHead { // line 33
public:
short unsigned int MsgType;
unsigned int Uin;
};
// sizeof = 38
struct SSafeModeAck { // line 39
public:
short unsigned int IPCDays;
signed char ProtStatLevel;
signed char HasVerifyMB;
short unsigned int EventType;
signed char Reserve[];
};
// sizeof = 38
struct SSafeModeUpdate { // line 48
public:
short unsigned int IPCDays;
signed char ProtStatLevel;
signed char HasVerifyMB;
short unsigned int EventType;
signed char Reserve[];
};
// sizeof = 38
union SSafeModeBody { // line 57
public:
SSafeModeUpdate RspSafeModeUpdate;
SSafeModeAck RspSafeModeAck;
};
// sizeof = 44
struct SSafeModePkg { // line 66
public:
SSafeModeHead Head;
SSafeModeBody Body;
};
// sizeof = 28
struct tagAntibotPunishInfo { // line 117
public:
int iGameID;
int iBanTimeSecond;
int iVerifyTimeout;
int iVerifyMaxErrorCount;
int iVerifyLevel;
unsigned int punish_info_len;
const unsigned char *punish_info;
};
typedef tagAntibotPunishInfo AntibotPunishInfo;

#endif // SECSVR_SRC_FORMMOG_TSS_SDK_ADAPTER_IBUSINESS_H_H_
