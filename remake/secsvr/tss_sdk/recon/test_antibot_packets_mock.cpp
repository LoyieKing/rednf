// Mock-link behavior check for the reconstructed Antibot packet methods.
#include <cstdio>
#include <cstring>
#include <stdint.h>

class AntibotInterface {
public:
    int OnRecvBufferFromClient(int, int, unsigned int, unsigned char *, unsigned short);
    int RecvBufferForEncrypt(unsigned int, unsigned char *, unsigned int, unsigned char *, unsigned int &);
    int RecvBufferForDecrypt(unsigned int, unsigned char *, unsigned short, unsigned char *, unsigned short &);
    int OnNotifyServer(int, int, unsigned int, unsigned char *, short);
};

namespace ZEN_Trace_LogMsg {
void debug_infoex(const char *, ...) {}
void debug_debugEx(const char *, ...) {}
void debug_errorex(const char *, ...) {}
}

// ABI-compatible 28-byte function table: notify is slot +0x10.
struct MockTssSdk {
    void *init;
    void *init_plugins;
    void *fini;
    void *proc;
    int (*notify)(int plugin_id, void *data);
    void *get_last_error_code;
    void *get_last_error;
};

static MockTssSdk g_table;
void *g_tss_sdk = &g_table;

static int g_notify_calls = 0;
static int g_plugin_id = -1;
static int g_msg_id = -1;
static int g_notify_result = 0;

static int MockNotify(int plugin_id, void *data) {
    ++g_notify_calls;
    g_plugin_id = plugin_id;
    const uint32_t *pkg = static_cast<const uint32_t *>(data);
    g_msg_id = static_cast<int>(pkg[0]);
    // Simulate the proto-proc plugin: set result and output size fields.
    if (g_msg_id == 307) {
        reinterpret_cast<int *>(data)[1] = 0;         // down_pkg_.encrypt_result_
        reinterpret_cast<uint32_t *>(data)[8] = 16;   // output_buf_size_
    } else if (g_msg_id == 308) {
        reinterpret_cast<int *>(data)[1] = 0;         // dp_pkg_.decrypt_result_
        reinterpret_cast<uint32_t *>(data)[6] = 16;   // output_buf_size_ (low 16 used)
    } else if (g_msg_id == 306) {
        reinterpret_cast<int *>(data)[1] = 0;         // notify_result_
    }
    return g_notify_result;
}

int main() {
    g_table.notify = MockNotify;
    AntibotInterface a;
    unsigned char in[8] = {1,2,3,4,5,6,7,8};
    unsigned char out[16] = {0};
    unsigned int out_len = sizeof(out);
    unsigned short out_len16 = sizeof(out);

    int r = a.OnRecvBufferFromClient(0, 0, 0x11223344, in, sizeof(in));
    if (r != 0 || g_notify_calls != 1 || g_plugin_id != 3 || g_msg_id != 309) {
        std::printf("FAIL OnRecv r=%d calls=%d plugin=%d msg=%d\n", r, g_notify_calls, g_plugin_id, g_msg_id);
        return 1;
    }
    r = a.RecvBufferForEncrypt(7, in, sizeof(in), out, out_len);
    if (r != 0 || g_notify_calls != 2 || g_plugin_id != 3 || g_msg_id != 307) {
        std::printf("FAIL Encrypt r=%d calls=%d plugin=%d msg=%d\n", r, g_notify_calls, g_plugin_id, g_msg_id);
        return 1;
    }
    r = a.RecvBufferForDecrypt(8, in, sizeof(in), out, out_len16);
    if (r != 0 || g_notify_calls != 3 || g_plugin_id != 3 || g_msg_id != 308) {
        std::printf("FAIL Decrypt r=%d calls=%d plugin=%d msg=%d\n", r, g_notify_calls, g_plugin_id, g_msg_id);
        return 1;
    }
    r = a.OnNotifyServer(0, 0, 0xAABBCCDD, in, sizeof(in));
    if (r != 0 || g_notify_calls != 4 || g_plugin_id != 3 || g_msg_id != 306) {
        std::printf("FAIL Notify r=%d calls=%d plugin=%d msg=%d\n", r, g_notify_calls, g_plugin_id, g_msg_id);
        return 1;
    }
    std::printf("OK plugins=%d calls=%d last_msg=%d\n", g_plugin_id, g_notify_calls, g_msg_id);
    return 0;
}
