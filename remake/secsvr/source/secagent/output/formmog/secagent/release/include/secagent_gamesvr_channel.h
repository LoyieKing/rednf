// Auto-generated header stub from DWARF info
// Original path: output/formmog/secagent/release/include/secagent_gamesvr_channel.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_FORMMOG_SECAGENT_RELEASE_INCLUDE_SECAGENT_GAMESVR_CHANNEL_H_H_
#define SECSVR_OUTPUT_FORMMOG_SECAGENT_RELEASE_INCLUDE_SECAGENT_GAMESVR_CHANNEL_H_H_

#include <stddef.h>

// sizeof = 131108
struct SecAgentGamesvrChannel { // line 5
private:
static const unsigned int GAMEBUSSIZE;
static const unsigned int RECV_BUF_SIZE;
char recv_buf_[];
char send_buf_[];
ZEN_Thread_Mutex thread_mutex_;
static const unsigned int MAX_CHANNELS_COUNT;
static const unsigned int MIN_CHANNEL_ID;
static const unsigned int MAX_CHANNEL_ID;
ZEN_BusPipe_TwoWay *bus_channel_;
unsigned int max_channel_id_;
public:
SecAgentGamesvrChannel(); // line 7
~SecAgentGamesvrChannel(); // line 8
int open(); // line 12
int recv(unsigned int arg0, void **arg1, size_t *arg2); // line 15
int send(unsigned int arg0, const void *arg1, size_t arg2); // line 18
int close(); // line 26
int getMaxChannelID(); // line 28
int getMinChannelID(); // line 29
private:
int open(unsigned int arg0); // line 32
SecAgentGamesvrChannel(const SecAgentGamesvrChannel &arg0); // line 59
SecAgentGamesvrChannel & operator=(const SecAgentGamesvrChannel &arg0); // line 60
public:
int send<sec_proto::SendToSDK>(unsigned int uin, unsigned int cmd, unsigned int appid, unsigned int channel_id, const /*anon struct*/ int &pkg); // line 69
};
extern size_t len; // line 71
extern const unsigned int RECV_BUF_SIZE; // line 43
extern const unsigned int MAX_CHANNELS_COUNT; // line 51
extern const unsigned int MIN_CHANNEL_ID; // line 52
extern const unsigned int GAMEBUSSIZE; // line 40
extern const unsigned int MAX_CHANNEL_ID; // line 53


// inferred

#endif // SECSVR_OUTPUT_FORMMOG_SECAGENT_RELEASE_INCLUDE_SECAGENT_GAMESVR_CHANNEL_H_H_
