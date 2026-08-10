// Reconstructed from secagent binary DWARF + disassembly.
// Original path: src/formmog/secagent/secagent_gamesvr_channel.h
#ifndef SECSVR_SRC_FORMMOG_SECAGENT_SECAGENT_GAMESVR_CHANNEL_H_H_
#define SECSVR_SRC_FORMMOG_SECAGENT_SECAGENT_GAMESVR_CHANNEL_H_H_

#include <stddef.h>

#include "output/commlib/zenlib/release/include/zen_bus_two_way.h"
#include "output/commlib/zenlib/release/include/zen_lock_thread_mutex.h"
#include "src/commlib/framework/comm_app_frame.h"

// sizeof = 131108
struct SecAgentGamesvrChannel { // line 5
private:
    static const unsigned int GAMEBUSSIZE = 0x800000; // line 40
    static const unsigned int RECV_BUF_SIZE = 65535;  // line 43
    char recv_buf_[RECV_BUF_SIZE];        // line 44 @0
    char send_buf_[RECV_BUF_SIZE];        // line 45 @65535
    ZEN_Thread_Mutex thread_mutex_;       // line 48 @131072
    static const unsigned int MAX_CHANNELS_COUNT = 12; // line 51
    static const unsigned int MIN_CHANNEL_ID = 1;      // line 52
    static const unsigned int MAX_CHANNEL_ID = 13;     // line 53
    ZEN_BusPipe_TwoWay *bus_channel_;     // line 55 @131100
    unsigned int max_channel_id_;         // line 56 @131104
public:
    SecAgentGamesvrChannel(); // line 7
    ~SecAgentGamesvrChannel(); // line 8
    int open(); // line 12
    int recv(unsigned int channel_id, void **recv_buf, unsigned int *recv_buf_size); // line 15
    int send(unsigned int channel_id, const void *send_buf, unsigned int send_buf_size); // line 18
    int close(); // line 26
    int getMaxChannelID(); // line 28
    int getMinChannelID(); // line 29
private:
    int open(unsigned int channel_id); // line 32
    SecAgentGamesvrChannel(const SecAgentGamesvrChannel &arg0); // line 59
    SecAgentGamesvrChannel & operator=(const SecAgentGamesvrChannel &arg0); // line 60
public:
    template<typename T>
    int send(unsigned int uin, unsigned int cmd, unsigned int appid,
             unsigned int channel_id, const T &pkg); // line 69
};

#endif // SECSVR_SRC_FORMMOG_SECAGENT_SECAGENT_GAMESVR_CHANNEL_H_H_
