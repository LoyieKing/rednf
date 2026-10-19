// Reconstructed translation unit
//   src/formmog/tss_sdk/plugins/proto_proc/dp_config.cpp
// from libtss_sdk.so.1.0.394 (ELF32 i386, GCC 4.1.0, -O2 -fPIC).
//
// ORIG CU address range: 0x000a8100 .. 0x000a9262 (next CU starts at
// 0x000a9264).  Every function below is the DWARF low_pc/high_pc symbol of
// that CU; the source-level behavior was decoded instruction by instruction
// from the ORIG and the static data / GOT slots were resolved against the
// ORIG's own symbol table:
//
//   cmp_uint32          0x000a8100  0x17   dp_config.cpp:13
//   cmp_dpsubcmdlist    0x000a8120  0x19   dp_config.cpp:19
//   init_dp_config(    0x000a8160  0x43   dp_config.cpp:238
//       DPCmdLists&)
//   get_comm_mem        0x000a86c0  0xd4   dp_config.cpp:379
//   merge_path          0x000a87a0  0xc2   dp_config.cpp:218
//   get_cmd_id          0x000a8870  0x18e  dp_config.cpp:142
//   sort_dp_config      0x000a8a00  0x2e3  dp_config.cpp:31
//   do_reload_dp_config 0x000a8cf0  0x2a7  dp_config.cpp:329
//   reload_dp_config    0x000a8fa0  0x84   dp_config.cpp:362
//   init_dp_config()    0x000a9030  0x232  dp_config.cpp:393
//
// ORIG globals owned here (ORIG .bss / .data.rel.ro):
//   tss_dp_comm::DP_CONFIG::glb_dp_config_             0x002bb4c0
//   tss_dp_comm::DP_CONFIG::glb_dp_config_head_        0x002bb4c4
//   tss_dp_comm::DP_CONFIG::glb_config_update_time_    0x002bb4c8
//   s_dp_comm_head_                                     0x002bbaa0
//   s_dp_config_                                        0x002bbab0
//   tss_dp_comm::dp_config_lock                         0x002bbac0
//   tss_dp_comm::dp_config_head_                        0x002bbac8
//   tss_dp_comm::dp_comm_config_head                    0x00292344
//   tss_dp_comm::dp_comm_config                         0x00292348
//
// cmp_dphackfragment (0x000a8140) is the same ORIG CU symbol and is already
// provided by recon/src/formmog/tss_sdk/plugins/proto_proc/dp_hack_cmp_agent.cpp;
// it is declared here and called by sort_dp_config, not defined twice.
//
// Shared-memory boundary: get_comm_mem() and init_dp_config() operate on the
// ORIG's file-backed mappings /tmp/dp_comm_config_head (DP_CONFIG, 280 bytes)
// and /tmp/dp_comm_config (DP_CONFIG_BODY, 22848 bytes).  This TU does not
// unlink, truncate or otherwise "fix up" those files at build/link time.
// init_dp_config() is the only entry point that maps them; reload_dp_config()
// only writes through the pointers that init_dp_config() installed.  The
// differential oracle therefore runs the init/reload scenario inside a
// private user+mount namespace with a private tmpfs mounted on /tmp.

#include "src/closing/dp_config.h"
#include "src/origsrc/formmog_proto_tss_sdk_client.h"
#include "src/commlib/zenlib/zen_lock_guard.h"

#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "src/commlib/zenlib/zen_os_adapt_dirent.h"
#include "src/commlib/zenlib/zen_server_status.h"

// ORIG dp_config.cpp static-init block: the 16-entry COMM_STAT_ITEM_WITH_NAME
// array is spelled out in this TU (below); the missing piece was the TU-local
// __ioinit that GCC 4.1's <iostream> emitted and GCC 13 no longer does.
#include "src/ioinit_block.h"

// ---------------------------------------------------------------------------
// sec_proto packet boundary
//
// The full sec_proto header (src/origsrc/formmog_proto_tss_sdk_client.h) and
// the shared configuration header cannot be included in the same TU: each
// brings its own complete tsf4g_tdr model (compat_client.h vs compat.h) with
// the same class names.  The ORIG's do_reload_dp_config() only needs the
// 1585-byte DPInterData view and the two pack() entry points, so the model
// below mirrors the DWARF layout of the already reconstructed protocol header
// and calls its exported methods.  The static assertions make a layout drift a
// compile error instead of a silent wire change.
// ---------------------------------------------------------------------------


typedef char assert_dp_inter_head_size[sizeof(sec_proto::DPInterDataHead) == 5 ? 1 : -1];
typedef char assert_dp_inter_cmdlist_size[sizeof(sec_proto::DPCmdList) == 1028 ? 1 : -1];
typedef char assert_dp_inter_body_size[sizeof(sec_proto::DPInterDataBody) == 1580 ? 1 : -1];
typedef char assert_dp_inter_size[sizeof(sec_proto::DPInterData) == 1585 ? 1 : -1];
typedef char assert_dp_inter_head_body_off[offsetof(sec_proto::DPInterData, body) == 5 ? 1 : -1];
typedef char assert_dp_inter_up_list_off[offsetof(sec_proto::DPCmdList, up_cmd_list_) == 4 ? 1 : -1];
typedef char assert_dp_inter_down_list_off[offsetof(sec_proto::DPCmdList, down_cmd_list_) == 516 ? 1 : -1];

// ---------------------------------------------------------------------------
// ORIG static objects, in the ORIG's declaration order.
//
// The 16 ZEN_STATUS_ITEM_WITHNAME entries are the static
// COMM_STAT_ITEM_WITH_NAME array from output/commlib/framework/.../
// comm_stat_define.h (ORIG .bss 0x002bb4e0, 1472 bytes, ids 0x238d..0x239c,
// STATICS_PER_FIVE_MINTUES, names as they appear in .rodata).  The shared
// comm_stat_define.h is not on this TU's include path, so the identical
// initializer list is spelled here; the element type is the shared
// zen_server_status.h type, not a private copy.
// ---------------------------------------------------------------------------
static ZEN_STATUS_ITEM_WITHNAME COMM_STAT_ITEM_WITH_NAME[16] = {
    ZEN_STATUS_ITEM_WITHNAME(0x238d, STATICS_PER_FIVE_MINTUES, "COMM_STAT_RECV_PKG_COUNT"),
    ZEN_STATUS_ITEM_WITHNAME(0x238e, STATICS_PER_FIVE_MINTUES, "COMM_STAT_SEND_PKG_SUCC"),
    ZEN_STATUS_ITEM_WITHNAME(0x238f, STATICS_PER_FIVE_MINTUES, "COMM_STAT_SEND_PKG_FAIL"),
    ZEN_STATUS_ITEM_WITHNAME(0x2390, STATICS_PER_FIVE_MINTUES, "COMM_STAT_TRANS_PROC_SUCC"),
    ZEN_STATUS_ITEM_WITHNAME(0x2391, STATICS_PER_FIVE_MINTUES, "COMM_STAT_TRANS_PROC_FAIL"),
    ZEN_STATUS_ITEM_WITHNAME(0x2392, STATICS_PER_FIVE_MINTUES, "COMM_STAT_TRANS_PROC_TIMEOUT"),
    ZEN_STATUS_ITEM_WITHNAME(0x2393, STATICS_PER_FIVE_MINTUES, "COMM_STAT_TRANS_PROC_ERRNO"),
    ZEN_STATUS_ITEM_WITHNAME(0x2394, STATICS_PER_FIVE_MINTUES, "COMM_STAT_TRANS_USE_TIME"),
    ZEN_STATUS_ITEM_WITHNAME(0x2395, STATICS_PER_FIVE_MINTUES, "COMM_STAT_APP_ALIVE"),
    ZEN_STATUS_ITEM_WITHNAME(0x2396, STATICS_PER_FIVE_MINTUES, "COMM_STAT_BILL_COUNT"),
    ZEN_STATUS_ITEM_WITHNAME(0x2397, STATICS_PER_FIVE_MINTUES, "COMM_STAT_SYS_CPU_RATIO"),
    ZEN_STATUS_ITEM_WITHNAME(0x2398, STATICS_PER_FIVE_MINTUES, "COMM_STAT_PROCESS_CPU_RATIO"),
    ZEN_STATUS_ITEM_WITHNAME(0x2399, STATICS_PER_FIVE_MINTUES, "COMM_STAT_SYS_MEM_USE_RATIO"),
    ZEN_STATUS_ITEM_WITHNAME(0x239a, STATICS_PER_FIVE_MINTUES, "COMM_STAT_AVAILABLE_MEM_SIZE"),
    ZEN_STATUS_ITEM_WITHNAME(0x239b, STATICS_PER_FIVE_MINTUES, "COMM_STAT_RECV_PKG_BYTES"),
    ZEN_STATUS_ITEM_WITHNAME(0x239c, STATICS_PER_FIVE_MINTUES, "COMM_STAT_SEND_PKG_BYTES"),
};

static ZEN_ShareMem_Mmap s_dp_comm_head_;
static ZEN_ShareMem_Mmap s_dp_config_;

namespace tss_dp_comm {

// ORIG .data.rel.ro: 4-byte pointers to the hardcoded /tmp names.
/* ORIG .data.rel.ro 的 4 字节对象（本地符号，名字即这两个命名空间成员名）；
   GCC 13 会把 static const 折叠掉/改名，故用 asm 标注固定 ORIG 符号名。 */
static const char *const dp_comm_config_head
    __asm__("_ZN11tss_dp_comm19dp_comm_config_headE")
    __attribute__((used)) = "/tmp/dp_comm_config_head";
static const char *const dp_comm_config
    __asm__("_ZN11tss_dp_comm14dp_comm_configE")
    __attribute__((used)) = "/tmp/dp_comm_config";

static ZEN_File_Lock dp_config_lock;
static DP_CONFIG_BODY *dp_config_head_ = NULL;

DPConfig *DP_CONFIG::glb_dp_config_ = NULL;
DP_CONFIG *DP_CONFIG::glb_dp_config_head_ = NULL;
unsigned int DP_CONFIG::glb_config_update_time_ = 0;

// These two are local (.symtab 't') in the ORIG, not dynamic exports.
static int merge_path(const char *base, char *relate, unsigned int len);
static int init_dp_config(DPCmdLists &cmd_list);

}  // namespace tss_dp_comm



// ---------------------------------------------------------------------------
// Comparator functions
//
// All three ORIG bodies are the same 7-instruction shape:
//   lhs = *(u32*)lf; rhs = *(u32*)rt; return (rhs < lhs) ? 1 : -1;
// i.e. a descending comparator that never returns 0.
// ---------------------------------------------------------------------------
int cmp_uint32(const void *lf, const void *rt) {
    const unsigned int lhs = *static_cast<const unsigned int *>(lf);
    const unsigned int rhs = *static_cast<const unsigned int *>(rt);
    return (rhs < lhs) ? 1 : -1;
}

int cmp_dpsubcmdlist(const void *lf, const void *rt) {
    const tss_dp_comm::DPSubCmdList *lhs =
        static_cast<const tss_dp_comm::DPSubCmdList *>(lf);
    const tss_dp_comm::DPSubCmdList *rhs =
        static_cast<const tss_dp_comm::DPSubCmdList *>(rt);
    return (rhs->cmd_id < lhs->cmd_id) ? 1 : -1;
}

// ORIG 0x000a8160.  Note: the ORIG indexes cmd_lists with the unvalidated
// cur_posi and reloads the selected length on every iteration.
int tss_dp_comm::init_dp_config(tss_dp_comm::DPCmdLists &cmd_list) {
    const unsigned int idx = cmd_list.cur_posi;
    tss_dp_comm::DPValueArray_128 &list = cmd_list.cmd_lists[idx];

    cmd_list.cur_crc = 0;
    for (int i = 0; i < list.length; ++i) {
        cmd_list.cur_crc ^= list.values[i];
    }
    return 0;
}

// ---------------------------------------------------------------------------
// get_comm_mem / merge_path / get_cmd_id
// ---------------------------------------------------------------------------

// ORIG 0x000a86c0.  First attempt restores an existing mapping (no truncate);
// only if that fails does it retry with truncate/create.
void *tss_dp_comm::get_comm_mem(ZEN_ShareMem_Mmap &mmap, const char *path,
                                unsigned int size) {
    if (mmap.open(path, size, true, false, true, NULL, 0) == 0) {
        return mmap.addr();
    }
    if (mmap.open(path, size, false, false, true, NULL, 0) == 0) {
        return mmap.addr();
    }
    ZEN_Trace_LogMsg::debug_errorex(
        "open mmap file fail. filepath=%s, err=%d", path, errno);
    return NULL;
}

// ORIG 0x000a87a0.  Relative paths are resolved against the directory of
// `base` (ZEN_OS::dirname keeps the trailing slash, hence the direct concat).
int tss_dp_comm::merge_path(const char *base, char *relate, unsigned int len) {
    if (relate[0] != '\\' && relate[1] != ':' && relate[0] != '/') {
        char temp_path[256];
        memset(temp_path, 0, sizeof(temp_path));
        ZEN_OS::dirname(base, temp_path, sizeof(temp_path));
        strncat(temp_path, relate, 255);
        strncpy(relate, temp_path, len - 1);
    }
    return 0;
}

// ORIG 0x000a8870.  cmd_id_type selects the wire width/endianness:
//   1 u8, 2 u16 LE, 3 u24 LE, 4 u32 LE, 5 u16 BE, 6 u24 BE, 7 u32 BE.
// Types 3 and 6 are byte-composed into cmd_id; type 7 reads a full u32 even
// though its bound check only requires offset + 2 bytes (ORIG behavior kept).
int tss_dp_comm::get_cmd_id(const tss_dp_comm::DPGameCmdStruct &cmd_struct,
                            unsigned char *buf, unsigned short len,
                            unsigned int &cmd_id) {
    int res = -1;
    if (cmd_struct.cmd_id_type == 0) {
        return res;
    }

    const unsigned int offset = cmd_struct.cmd_id_offset;
    unsigned char *data = buf + offset;

    switch (cmd_struct.cmd_id_type) {
    case 1:
        if (len < offset + 1) return res;
        cmd_id = data[0];
        res = 0;
        break;

    case 2:
        if (len < offset + 2) return res;
        cmd_id = static_cast<unsigned int>(data[0]) |
                 (static_cast<unsigned int>(data[1]) << 8);
        res = 0;
        break;

    case 3:
        if (static_cast<int>(len) < static_cast<int>(offset) + 3) return res;
        cmd_id = static_cast<unsigned int>(data[0]) |
                 (static_cast<unsigned int>(data[1]) << 8) |
                 (static_cast<unsigned int>(data[2]) << 16);
        res = 0;
        break;

    case 4:
        if (len < offset + 4) return res;
        cmd_id = static_cast<unsigned int>(data[0]) |
                 (static_cast<unsigned int>(data[1]) << 8) |
                 (static_cast<unsigned int>(data[2]) << 16) |
                 (static_cast<unsigned int>(data[3]) << 24);
        res = 0;
        break;

    case 5:
        if (len < offset + 2) return res;
        cmd_id = (static_cast<unsigned int>(data[0]) << 8) |
                 static_cast<unsigned int>(data[1]);
        res = 0;
        break;

    case 6:
        if (static_cast<int>(len) < static_cast<int>(offset) + 3) return res;
        cmd_id = (static_cast<unsigned int>(data[0]) << 16) |
                 (static_cast<unsigned int>(data[1]) << 8) |
                 static_cast<unsigned int>(data[2]);
        res = 0;
        break;

    case 7:
        if (len < offset + 2) return res;
        cmd_id = (static_cast<unsigned int>(data[0]) << 24) |
                 (static_cast<unsigned int>(data[1]) << 16) |
                 (static_cast<unsigned int>(data[2]) << 8) |
                 static_cast<unsigned int>(data[3]);
        res = 0;
        break;

    default:
        break;
    }
    return res;
}

// ---------------------------------------------------------------------------
// sort_dp_config
//
// qsort call order and element sizes are the ORIG immediates:
//   42 = sizeof(DPSubCmdList), 16 = sizeof(DPHackFragment), 4 = u32.
// ---------------------------------------------------------------------------
void sort_dp_config(tss_dp_comm::DPConfig &dp_config) {
    qsort(dp_config.cmd_hack.drop_subcmd_list,
          dp_config.cmd_hack.drop_subcmd_count,
          sizeof(tss_dp_comm::DPSubCmdList), cmp_dpsubcmdlist);
    qsort(dp_config.cmd_hack.forward_subcmd_list,
          dp_config.cmd_hack.forward_subcmd_count,
          sizeof(tss_dp_comm::DPSubCmdList), cmp_dpsubcmdlist);
    qsort(dp_config.cmd_hack.up_cmd_hack.hack_fragments,
          dp_config.cmd_hack.up_cmd_hack.fragment_count,
          sizeof(tss_dp_comm::DPHackFragment), cmp_dphackfragment);
    qsort(dp_config.cmd_hack.down_cmd_hack.hack_fragments,
          dp_config.cmd_hack.down_cmd_hack.fragment_count,
          sizeof(tss_dp_comm::DPHackFragment), cmp_dphackfragment);
    qsort(dp_config.cmd_hack.raw_cmd_hack.hack_fragments,
          dp_config.cmd_hack.raw_cmd_hack.fragment_count,
          sizeof(tss_dp_comm::DPHackFragment), cmp_dphackfragment);
    qsort(dp_config.cmd_hack.delay_cmd.safemode_drop_cmdlist.values,
          dp_config.cmd_hack.delay_cmd.safemode_drop_cmdlist.length,
          sizeof(unsigned int), cmp_uint32);

    for (unsigned int i = 0; i < dp_config.cmd_hack.drop_subcmd_count; ++i) {
        qsort(dp_config.cmd_hack.drop_subcmd_list[i].subcmd_list.values,
              dp_config.cmd_hack.drop_subcmd_list[i].subcmd_list.length,
              sizeof(unsigned int), cmp_uint32);
    }
    for (unsigned int i = 0; i < dp_config.cmd_hack.forward_subcmd_count; ++i) {
        qsort(dp_config.cmd_hack.forward_subcmd_list[i].subcmd_list.values,
              dp_config.cmd_hack.forward_subcmd_list[i].subcmd_list.length,
              sizeof(unsigned int), cmp_uint32);
    }
    for (unsigned int i = 0; i < dp_config.dp_code_info.up_cmd_list.lists_count; ++i) {
        qsort(dp_config.dp_code_info.up_cmd_list.cmd_lists[i].values,
              dp_config.dp_code_info.up_cmd_list.cmd_lists[i].length,
              sizeof(unsigned int), cmp_uint32);
    }
    for (unsigned int i = 0; i < dp_config.dp_code_info.down_cmd_list.lists_count; ++i) {
        qsort(dp_config.dp_code_info.down_cmd_list.cmd_lists[i].values,
              dp_config.dp_code_info.down_cmd_list.cmd_lists[i].length,
              sizeof(unsigned int), cmp_uint32);
    }
    qsort(dp_config.uin_monitor_list.values,
          dp_config.uin_monitor_list.length,
          sizeof(unsigned int), cmp_uint32);
}

// ---------------------------------------------------------------------------
// prepare_code_info (ORIG lines 248..292, inlined into do_reload_dp_config)
//
// Packs the selected up/down command lists and the relative code/bill paths
// into the config's own cmd_list_buf, then rewrites the 5-byte packet header
// with the final packet size.
// ---------------------------------------------------------------------------
namespace {

void prepare_code_info(tss_dp_comm::DPConfig &config) {
    sec_proto::DPInterData inter_data;
    tss_dp_comm::init_dp_config(config.dp_code_info.up_cmd_list);
    tss_dp_comm::init_dp_config(config.dp_code_info.down_cmd_list);

    inter_data.head.protocol_type_ = 4;
    inter_data.head.data_type_ = 0;

    const tss_dp_comm::DPCmdLists &up = config.dp_code_info.up_cmd_list;
    const tss_dp_comm::DPValueArray_128 &up_list = up.cmd_lists[up.cur_posi];
    inter_data.body.cmd_list.up_cmd_count_ = up_list.length;
    for (int i = 0; i < up_list.length; ++i) {
        inter_data.body.cmd_list.up_cmd_list_[i] = up_list.values[i];
    }

    const tss_dp_comm::DPCmdLists &down = config.dp_code_info.down_cmd_list;
    const tss_dp_comm::DPValueArray_128 &down_list =
        down.cmd_lists[down.cur_posi];
    inter_data.body.cmd_list.down_cmd_count_ = down_list.length;
    for (int i = 0; i < down_list.length; ++i) {
        inter_data.body.cmd_list.down_cmd_list_[i] = down_list.values[i];
    }

    unsigned int used = 0;
    inter_data.pack(
        reinterpret_cast<char *>(config.dp_code_info.cmd_list_buf), 0x2800,
        &used, 0);
    config.dp_code_info.cmd_list_buf_size =
        static_cast<uint16_t>(used);
    inter_data.head.packet_size_ = static_cast<uint16_t>(used);
    inter_data.head.pack(
        reinterpret_cast<char *>(config.dp_code_info.cmd_list_buf), 0x2800,
        NULL, 0);

    if (config.dp_code_info.code_dir_count != 0) {
        for (unsigned int i = 0; i < config.dp_code_info.code_dir_count; ++i) {
            tss_dp_comm::merge_path(tss_dp_comm::DP_CONFIG::glb_dp_config_head_
                                        ->dp_config_path,
                                    config.dp_code_info.code_dir_path[i], 0x100);
        }
    }
    tss_dp_comm::merge_path(
        tss_dp_comm::DP_CONFIG::glb_dp_config_head_->dp_config_path,
        config.dp_log_conf.bill_path, 0x100);
}

}  // namespace

// ---------------------------------------------------------------------------
// do_reload_dp_config
//
// Reloads the on-disk XML into a stack DPConfig, sorts/prefixes it, packs the
// command lists into cmd_list_buf and copies the 22842-byte result into the
// mapped shared configuration.  On parse failure the mapped config is left
// untouched, but the two timestamps are still advanced (ORIG behavior).
// ---------------------------------------------------------------------------
int tss_dp_comm::do_reload_dp_config() {
    int res = 0;

    if (dp_config_head_->cur_update_time_ ==
        DP_CONFIG::glb_dp_config_head_->update_time) {
        DP_CONFIG::glb_config_update_time_ =
            DP_CONFIG::glb_dp_config_head_->update_time;
        return res;
    }

    DPConfig temp_config;
    memset(&temp_config, 0, sizeof(temp_config));

    res = temp_config.fromXmlFile(
        DP_CONFIG::glb_dp_config_head_->dp_config_path,
        tsf4g_tdr::LIST_ENTRY, 0);
    if (res != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "do_reload_dp_config fail ,res=%d, path=%s", res,
            DP_CONFIG::glb_dp_config_head_->dp_config_path);
    } else {
        sort_dp_config(temp_config);
        prepare_code_info(temp_config);
        memcpy(DP_CONFIG::glb_dp_config_, &temp_config, sizeof(temp_config));
    }

    ZEN_Trace_LogMsg::debug_debugEx(
        "read cmd_hack up_cmd_hack's fragment count:%u",
        static_cast<unsigned int>(
            DP_CONFIG::glb_dp_config_->cmd_hack.up_cmd_hack.fragment_count));

    dp_config_head_->cur_update_time_ =
        DP_CONFIG::glb_dp_config_head_->update_time;
    DP_CONFIG::glb_config_update_time_ =
        DP_CONFIG::glb_dp_config_head_->update_time;
    return res;
}

// ---------------------------------------------------------------------------
// reload_dp_config
//
// Publishes the head's update_time, skips when the mapped config is already
// at that revision, otherwise reloads under the file lock.
// ---------------------------------------------------------------------------
int tss_dp_comm::reload_dp_config() {
    DP_CONFIG::glb_config_update_time_ =
        DP_CONFIG::glb_dp_config_head_->update_time;
    if (dp_config_head_->cur_update_time_ ==
        DP_CONFIG::glb_dp_config_head_->update_time) {
        return 0;
    }

    ZEN_Write_Guard<ZEN_File_Lock> guard(dp_config_lock);
    return do_reload_dp_config();
}

// ---------------------------------------------------------------------------
// init_dp_config
//
// One-shot initialization: ensures the parent directory exists, opens the
// lock file /tmp/dp_comm_config_head, maps the 280-byte header and the
// 22848-byte body, then performs the first reload.
// ---------------------------------------------------------------------------
int tss_dp_comm::init_dp_config() {
    if (DP_CONFIG::glb_dp_config_ != NULL) {
        ZEN_Trace_LogMsg::debug_errorex(
            "init_dp_config fail, already init");
        return -1;
    }

    char temp_path[256];
    memset(temp_path, 0, sizeof(temp_path));
    ZEN_OS::dirname(dp_comm_config_head, temp_path, sizeof(temp_path));

    if (access(temp_path, F_OK) != 0) {
        if (ZEN_OS::mkdir(temp_path, 0755) != 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "mkdir dpconfig fail. temp_path=%s", temp_path);
            return -1;
        }
    }

    const int open_ret =
        dp_config_lock.open(dp_comm_config_head, O_RDWR | O_CREAT, 0660);
    if (open_ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex("open lock fail. ret=%d", open_ret);
        return -1;
    }

    int res = 0;
    {
        ZEN_Write_Guard<ZEN_File_Lock> guard(dp_config_lock);

        ZEN_Trace_LogMsg::debug_debugEx(
            "init_dp_config, get dp config head begin, head size %u",
            static_cast<unsigned int>(sizeof(DP_CONFIG)));
        DP_CONFIG::glb_dp_config_head_ = static_cast<DP_CONFIG *>(
            get_comm_mem(s_dp_comm_head_, dp_comm_config_head,
                         sizeof(DP_CONFIG)));
        if (DP_CONFIG::glb_dp_config_head_ == NULL) {
            res = -1;
        } else {
            ZEN_Trace_LogMsg::debug_debugEx(
                "init_dp_config, get dp config head end, game id %d, world id "
                "%d, secagent_id %u, dp config file path %s",
                static_cast<int>(DP_CONFIG::glb_dp_config_head_->game_id),
                static_cast<int>(DP_CONFIG::glb_dp_config_head_->world_id),
                DP_CONFIG::glb_dp_config_head_->secagent_id,
                DP_CONFIG::glb_dp_config_head_->dp_config_path);

            ZEN_Trace_LogMsg::debug_debugEx(
                "init_dp_config, get dp config begin, config size %u",
                static_cast<unsigned int>(sizeof(DP_CONFIG_BODY)));
            dp_config_head_ = static_cast<DP_CONFIG_BODY *>(
                get_comm_mem(s_dp_config_, dp_comm_config,
                             sizeof(DP_CONFIG_BODY)));
            if (dp_config_head_ == NULL) {
                res = -1;
            } else {
                DP_CONFIG::glb_dp_config_ = &dp_config_head_->dp_config_;
                ZEN_Trace_LogMsg::debug_debugEx(
                    "init_dp_config, get dp config end, game id %d, bill path "
                    "%s",
                    static_cast<int>(
                        DP_CONFIG::glb_dp_config_->dp_log_conf.game_id),
                    DP_CONFIG::glb_dp_config_->dp_log_conf.bill_path);
                res = do_reload_dp_config();
            }
        }
    }
    return res;
}
