// Auto-generated stub from DWARF info
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag435/src/formmog/secagent/secagent_gamesvr_channel.cpp
// Compiler: GNU C++ 4.1.0 (SUSE Linux)
// 函数体暂为空；仅保留签名、参数名与局部变量名。

#include "src/commlib/framework/comm_predefine.h"
#include "src/commlib/zenlib/zen_trace_log_msg.h"
#include "src/commlib/zenlib/zen_shm_lockfree_deque.h"
#include "src/commlib/zenlib/zen_os_adapt_file.h"
#include "src/commlib/framework/comm_app_frame.h"
#include "src/protocol/formmog/formmog_conf_secagent.h"
#include "src/protocol/formmog/formmog_proto_public_tsssdk.h"
#include "src/formmog/secagent/secagent_gamesvr_channel.h"
#include "src/formmog/secagent/secagent_dpsdk_info_mgr.h"
#include "src/formmog/secagent/secagent_app.h"

// line 40
const unsigned int SecAgentGamesvrChannel::GAMEBUSSIZE;

// line 43
const unsigned int SecAgentGamesvrChannel::RECV_BUF_SIZE;

// line 51
const unsigned int SecAgentGamesvrChannel::MAX_CHANNELS_COUNT;

// line 52
const unsigned int SecAgentGamesvrChannel::MIN_CHANNEL_ID;

// line 53
const unsigned int SecAgentGamesvrChannel::MAX_CHANNEL_ID;

// line 7
SecAgentGamesvrChannel::SecAgentGamesvrChannel() {
}

// line 8
SecAgentGamesvrChannel::~SecAgentGamesvrChannel() {
}

// line 126
int SecAgentGamesvrChannel::close() {
    return 0;
}

// line 132
int SecAgentGamesvrChannel::getMaxChannelID() {
    return max_channel_id_;
}

// line 138
int SecAgentGamesvrChannel::getMinChannelID() {
    return MIN_CHANNEL_ID;
}

// line 103
int SecAgentGamesvrChannel::send(unsigned int channel_id, const void *send_buf,
                                 unsigned int send_buf_size) {
    ZEN_Trace_LogMsg::debug_debugEx("channel_id[%d] will send data", channel_id);
    if (channel_id < MIN_CHANNEL_ID || channel_id > MAX_CHANNELS_COUNT) {
        ZEN_Trace_LogMsg::debug_errorex("[%s], channel_id[%d] too big",
                                        "int SecAgentGamesvrChannel::send(unsigned int, const void*, size_t)",
                                        channel_id);
        return 0x493f1;
    }
    if (send_buf_size > RECV_BUF_SIZE) {
        ZEN_Trace_LogMsg::debug_errorex("[%s], send buf to bus fail. buf too big. buf_size=%u",
                                        "int SecAgentGamesvrChannel::send(unsigned int, const void*, size_t)",
                                        send_buf_size);
        return 0x493f1;
    }
    ZEN_BusPipe_TwoWay *node = bus_channel_ + channel_id;
    ZEN_LIB::shm_dequechunk *send_pipe =
        reinterpret_cast<ZEN_LIB::shm_dequechunk *>(reinterpret_cast<char *>(node) + 0x200c);
    if (send_pipe->push_end(reinterpret_cast<const ZEN_LIB::dequechunk_node *>(send_buf))) {
        return 0;
    }
    size_t free_size = send_pipe->freesize();
    ZEN_Trace_LogMsg::debug_alertex("[zenlib] %u Pipe is full or data small?,",
                                    0, *(const unsigned int *)send_buf, free_size);
    return -1;
}

// line 82
int SecAgentGamesvrChannel::recv(unsigned int channel_id, void **recv_buf,
                                 unsigned int *recv_buf_size) {
    *recv_buf = NULL;
    *recv_buf_size = RECV_BUF_SIZE;
    ZEN_BusPipe_TwoWay *node = bus_channel_ + channel_id;
    ZEN_LIB::shm_dequechunk *recv_pipe =
        reinterpret_cast<ZEN_LIB::shm_dequechunk *>(reinterpret_cast<char *>(node) + 0x2010);
    if (recv_pipe->empty()) {
        return -1;
    }
    recv_pipe->pop_front(reinterpret_cast<ZEN_LIB::dequechunk_node *>(this));
    *recv_buf = this;
    *recv_buf_size = *(unsigned int *)this;
    return 0;
}

// line 36
int SecAgentGamesvrChannel::open(unsigned int channel_id) {
    char dir_path[4096];
    char file_name[4096];
    dir_path[0] = '\0';
    file_name[0] = '\0';
    snprintf(dir_path, sizeof(dir_path) - 1, "/dev/shm/sec");
    snprintf(file_name, sizeof(file_name) - 1, "sec/tss_sdk_bus_%d", channel_id);
    if (ZEN_OS::access(dir_path, 0) != 0) {
        if (ZEN_OS::mkdir(dir_path, 0x1ed) != 0) {
            ZEN_Trace_LogMsg::debug_errorex("create path fail, path=%s", dir_path);
            return 0x4940c;
        }
    }
    SecAgentDPsdkInfoMgr::LPCONFIG conf = SecAgentApp::instance()->get_config();
    ZEN_BusPipe_TwoWay *node = bus_channel_ + channel_id;
    return node->initialize(file_name, GAMEBUSSIZE, GAMEBUSSIZE, RECV_BUF_SIZE,
                            conf->if_restore_gmsvr_channel_, conf->if_check_pthread_, 2);
}

// line 15
int SecAgentGamesvrChannel::open() {
    int ret = 0;
    SecAgentDPsdkInfoMgr::LPCONFIG conf = SecAgentApp::instance()->get_config();
    unsigned int channel_count = conf->gamesvr_channel_num_;
    max_channel_id_ = channel_count + 1;
    char *raw_buf = new char[max_channel_id_ * sizeof(ZEN_BusPipe_TwoWay) + 4];
    ZEN_BusPipe_TwoWay *node_array =
        reinterpret_cast<ZEN_BusPipe_TwoWay *>(raw_buf + 4);
    *(unsigned int *)raw_buf = max_channel_id_;
    if (channel_count != 0xffffffff) {
        for (unsigned int i = 0; i < channel_count; ++i) {
            new (&node_array[i]) ZEN_BusPipe_TwoWay();
        }
    }
    bus_channel_ = node_array;
    for (unsigned int channel_id = 1; channel_id < max_channel_id_; ++channel_id) {
        ret = open(channel_id);
        if (ret != 0) {
            ZEN_Trace_LogMsg::debug_errorex("[%s], open channel fail, channel_id[%d]. ret=%d",
                                            "int SecAgentGamesvrChannel::open()",
                                            channel_id, ret);
            break;
        }
    }
    return ret;
}

// line 69 (template instantiations)
template<typename T>
int SecAgentGamesvrChannel::send(unsigned int uin, unsigned int cmd, unsigned int appid,
                                 unsigned int channel_id, const T &pkg) {
    Comm_App_Frame *frame = reinterpret_cast<Comm_App_Frame *>(send_buf_);
    frame->init_framehead(0x10000, 0, cmd);
    frame->frame_uin_ = uin;
    frame->app_id_ = appid;
    frame->appdata_encode(0x10000, pkg, 0, NULL);
    return send(channel_id, send_buf_, frame->frame_length_);
}

template int SecAgentGamesvrChannel::send<sec_proto::SendToSDK>(
    unsigned int, unsigned int, unsigned int, unsigned int, const sec_proto::SendToSDK &);

template int SecAgentGamesvrChannel::send<sec_proto::PunishClientReq>(
    unsigned int, unsigned int, unsigned int, unsigned int, const sec_proto::PunishClientReq &);
