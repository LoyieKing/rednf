// Complete declarations for the AntibotInterface hierarchy reconstructed from
// libtss_sdk.so.1.0.394
// (SHA-256 8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7).
//
// This header is the single declaration owner for every reconstructed TU:
//   adapter/antibotinterface_ctor_agent.cpp     (C1/C2, D0/D1/D2)
//   adapter/antibotinterface_trivial.cpp        (six trivial slots)
//   adapter/antibot_client_agent.cpp            (Initialize/AddClient/DeleteClient)
//   adapter/antibot_packets_agent.cpp           (four packet slots)
//   adapter/antibot_policy_agent.cpp            (five policy slots)
//   plugins/proto_proc/tss_sdk_exports_timer_agent.cpp (factory + g_antibot_if)
//   closing/vtable_surface.cpp                  (base vtable/typeinfo owner)
//
// Evidence:
//   DWARF iantibotinterface.h:82: IAntibotInterface, sizeof 4, vptr only, 18
//   pure virtual slots in the order below.  There is no virtual destructor:
//   ORIG vtable 0x00292d20 (address point 0x00292d28) has exactly 18 entries,
//   all __cxa_pure_virtual (ORIG relocations 0x00292d28..0x00292d6c).
//   DWARF antibotinterface.h: class AntibotInterface sizeof 4 derives from
//   IAntibotInterface.  ORIG vtable 0x00292ca0 (address point 0x00292ca8) is
//   the 18 business slots followed by ~AntibotInterface D1 at 0x00292cf0 and
//   D0 at 0x00292cf4.  The D1/D0 pair follows the base slots because the
//   destructor is the only virtual introduced by the derived class.
//
// The hidden IAntibotInterface constructor is emitter-only: it is defined in
// closing/vtable_surface.cpp because the all-pure base has no key function and
// ORIG emits its vtable/typeinfo as weak objects.  It is not part of the
// exported symbol surface.  The derived constructor calls it as its base
// constructor and then installs the derived vtable, exactly as required for a
// 4-byte object with no data members beyond the shared vptr.

#ifndef TSS_SDK_ADAPTER_IF_ANTIBOTINTERFACE_H_
#define TSS_SDK_ADAPTER_IF_ANTIBOTINTERFACE_H_

class IBusiness;
struct TUserInfo;
struct SSafeModePkg;

class __attribute__((visibility("default"))) IAntibotInterface {
public:
    __attribute__((visibility("hidden"))) IAntibotInterface();

    virtual int Initialize(IBusiness *business, char *log_path) = 0;
    virtual int AddClient(int game_id, int room_id, unsigned int uin,
                          unsigned int client_ip, TUserInfo *user_info) = 0;
    virtual int DeleteClient(int game_id, int room_id, unsigned int uin) = 0;
    virtual int OnRecvBufferFromClient(int game_id, int room_id,
                                       unsigned int uin,
                                       unsigned char *buf,
                                       unsigned short buf_len) = 0;
    virtual int ProcessAntiBotInput() = 0;
    virtual int CheckTimer(int game_id, int room_id, unsigned int uin) = 0;
    virtual void SetLogPath(char *log_path) = 0;
    virtual void ShowInfo() = 0;
    virtual int P2PAddClient(unsigned int uin, int game_id,
                             unsigned int room_id) = 0;
    virtual int P2PDeleteClient(unsigned int uin) = 0;
    virtual int RecvBufferForEncrypt(unsigned int uin, unsigned char *in,
                                     unsigned int in_len, unsigned char *out,
                                     unsigned int &out_len) = 0;
    virtual int RecvBufferForDecrypt(unsigned int uin, unsigned char *in,
                                     unsigned short in_len, unsigned char *out,
                                     unsigned short &out_len) = 0;
    virtual int IsCmdInUpList(unsigned int uin, int cmd_id) = 0;
    virtual int IsCmdInDownList(unsigned int uin, int cmd_id) = 0;
    virtual int OnNotifyServer(int game_id, int room_id, unsigned int uin,
                               unsigned char *buf, short buf_len) = 0;
    virtual int AntiCheatPacket(unsigned int uin, int cmd_id) = 0;
    virtual int QueryProtectPolicy(int game_id, int room_id, unsigned int uin,
                                   SSafeModePkg *pkg) = 0;
    virtual int ForwardData(int game_id, int room_id, unsigned int uin,
                            unsigned int cmd_id, unsigned int data_len,
                            const unsigned char *data) = 0;
};

class __attribute__((visibility("default"))) AntibotInterface
    : public IAntibotInterface {
public:
    AntibotInterface();
    virtual ~AntibotInterface();

    virtual int Initialize(IBusiness *business, char *log_path);
    virtual int AddClient(int game_id, int room_id, unsigned int uin,
                          unsigned int client_ip, TUserInfo *user_info);
    virtual int DeleteClient(int game_id, int room_id, unsigned int uin);
    virtual int OnRecvBufferFromClient(int game_id, int room_id,
                                       unsigned int uin,
                                       unsigned char *buf,
                                       unsigned short buf_len);
    virtual int ProcessAntiBotInput();
    virtual int CheckTimer(int game_id, int room_id, unsigned int uin);
    virtual void SetLogPath(char *log_path);
    virtual void ShowInfo();
    virtual int P2PAddClient(unsigned int uin, int game_id,
                             unsigned int room_id);
    virtual int P2PDeleteClient(unsigned int uin);
    virtual int RecvBufferForEncrypt(unsigned int uin, unsigned char *in,
                                     unsigned int in_len, unsigned char *out,
                                     unsigned int &out_len);
    virtual int RecvBufferForDecrypt(unsigned int uin, unsigned char *in,
                                     unsigned short in_len, unsigned char *out,
                                     unsigned short &out_len);
    virtual int IsCmdInUpList(unsigned int uin, int cmd_id);
    virtual int IsCmdInDownList(unsigned int uin, int cmd_id);
    virtual int OnNotifyServer(int game_id, int room_id, unsigned int uin,
                               unsigned char *buf, short buf_len);
    virtual int AntiCheatPacket(unsigned int uin, int cmd_id);
    virtual int QueryProtectPolicy(int game_id, int room_id, unsigned int uin,
                                   SSafeModePkg *pkg);
    virtual int ForwardData(int game_id, int room_id, unsigned int uin,
                            unsigned int cmd_id, unsigned int data_len,
                            const unsigned char *data);
};

typedef char iantibotinterface_size_check[
    sizeof(IAntibotInterface) == 4 ? 1 : -1];
typedef char antibotinterface_size_check[
    sizeof(AntibotInterface) == 4 ? 1 : -1];

#endif  // TSS_SDK_ADAPTER_IF_ANTIBOTINTERFACE_H_
