// Auto-generated header stub from DWARF info
// Original path: output/formmog/tss_sdk/adapter/release/include/iantibotinterface.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_FORMMOG_TSS_SDK_ADAPTER_RELEASE_INCLUDE_IANTIBOTINTERFACE_H_H_
#define SECSVR_OUTPUT_FORMMOG_TSS_SDK_ADAPTER_RELEASE_INCLUDE_IANTIBOTINTERFACE_H_H_

// sizeof = 76
struct TUserInfo { // line 19
public:
char szPlayerName[];
int iUserID;
unsigned int uiSvrIp;
unsigned int uiCltIp;
};
// sizeof = 4
struct IAntibotInterface { // line 82
public:
int (***_vptr.IAntibotInterface)(...);
IAntibotInterface();
IAntibotInterface(const IAntibotInterface &arg0);
virtual int Initialize(IBusiness *arg0, char *arg1); // line 86
virtual int AddClient(int arg0, int arg1, unsigned int arg2, unsigned int arg3, TUserInfo *arg4); // line 105
virtual int DeleteClient(int arg0, int arg1, unsigned int arg2); // line 106
virtual int OnRecvBufferFromClient(int arg0, int arg1, unsigned int arg2, unsigned char *arg3, short unsigned int arg4); // line 107
virtual int ProcessAntiBotInput(); // line 108
virtual int CheckTimer(int arg0, int arg1, unsigned int arg2); // line 109
virtual void SetLogPath(char *arg0); // line 110
virtual void ShowInfo(); // line 111
virtual int P2PAddClient(unsigned int arg0, int arg1, unsigned int arg2); // line 114
virtual int P2PDeleteClient(unsigned int arg0); // line 115
virtual int RecvBufferForEncrypt(unsigned int arg0, unsigned char *arg1, unsigned int arg2, unsigned char *arg3, unsigned int &arg4); // line 116
virtual int RecvBufferForDecrypt(unsigned int arg0, unsigned char *arg1, short unsigned int arg2, unsigned char *arg3, short unsigned int &arg4); // line 117
virtual int IsCmdInUpList(unsigned int arg0, int arg1); // line 131
virtual int IsCmdInDownList(unsigned int arg0, int arg1); // line 136
virtual int OnNotifyServer(int arg0, int arg1, unsigned int arg2, unsigned char *arg3, short int arg4); // line 139
virtual int AntiCheatPacket(unsigned int arg0, int arg1); // line 147
virtual int QueryProtectPolicy(int arg0, int arg1, unsigned int arg2, SSafeModePkg *arg3); // line 163
virtual int ForwardData(int arg0, int arg1, unsigned int arg2, unsigned int arg3, unsigned int arg4, const unsigned char *arg5); // line 186
};

#endif // SECSVR_OUTPUT_FORMMOG_TSS_SDK_ADAPTER_RELEASE_INCLUDE_IANTIBOTINTERFACE_H_H_
