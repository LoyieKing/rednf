// Differential oracle: reconstructed formmog_proto_public_latransadapt.cpp (82
// symbols) plus formmog_proto_public_secsvr_extra.cpp (the 45 supplemental
// secsvr symbols) vs ORIG libtss_sdk.so.1.0.394.
//
// ORIG is dlopen()ed RTLD_GLOBAL so that the reconstructed objects' references
// to the out-of-line tsf4g_tdr runtime (TdrBufUtil::printVariable/printArray,
// TdrWriteBuf::textize) bind to the ORIG implementation - that is also an ABI
// check of the TdrWriteBuf/TdrReadBuf layouts in tsf4g_tdr_compat*.h; the
// reconstructed objects are dlopen()ed RTLD_LOCAL and called through dlsym.
// Mirrors test_uic_oracle.cpp; see AGENT_REPORT_genproto_latrans.md.
//
// The 5 secsvr-extra structs are declared here as well (the extra TU carries
// them inline, the generated header is not in the tree); the static assertions
// below pin them to the DWARF sizes before anything runs.
#include <dlfcn.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "formmog_proto_public_latransadapt.h"

namespace sec_proto {

#pragma pack(push, 1)

struct SecsvrCaptchaReq {
    uint16_t channel_id_;
    uint16_t room_id_;
    uint32_t version_;
    uint16_t bus_id_;
    uint32_t time_stamp_;
    uint32_t client_ip_;
    uint16_t max_pic_change_num_;
    uint16_t max_verify_num_;
    uint32_t timeout_;
    uint8_t busi_id_;
    uint16_t busi_info_len_;
    uint8_t busi_info_[512];
};

struct SecsvrCaptchaResultRsp {
    uint32_t version_;
    uint16_t room_id_;
    uint16_t bus_id_;
    uint32_t time_stamp_;
    uint32_t client_ip_;
    uint32_t app_id_;
    uint8_t result_;
    uint8_t msg_len_;
    uint8_t msg_[512];
};

struct SecsvrGreenUnionRsp {
    uint8_t result_;
    uint8_t flag_;
    uint16_t world_;
    uint8_t start_src_;
};

struct SecsvrImInfoNotifyReq {
    uint16_t msg_type_id_;
    uint16_t world_id_;
    uint16_t nick_name_len_;
    uint8_t nick_name_buf_[32];
};

struct SecsvrUserInfoNotifyReq {
    uint16_t msg_type_id_;
    uint16_t world_id_;
    uint32_t data_len_;
    uint8_t data_[2048];
};

#pragma pack(pop)

}  // namespace sec_proto

typedef char oracle_assert_1[sizeof(sec_proto::LATransAdaptDPData) == 32009 ? 1 : -1];
typedef char oracle_assert_2[sizeof(sec_proto::LATransAdaptAntibotRPCodeResult) == 32006 ? 1 : -1];
typedef char oracle_assert_3[sizeof(sec_proto::LATransAdaptAntibotErrMsg) == 1028 ? 1 : -1];
typedef char oracle_assert_4[sizeof(sec_proto::LATransAdaptAntibotRoleName) == 1026 ? 1 : -1];
typedef char oracle_assert_5[sizeof(sec_proto::LATransAdaptAntibotSigInfo) == 10252 ? 1 : -1];
typedef char oracle_assert_6[sizeof(sec_proto::LATransAdaptAntibotClientIP) == 21 ? 1 : -1];
typedef char oracle_assert_7[sizeof(sec_proto::LATransAdaptAntibotRPFMC) == 10246 ? 1 : -1];
typedef char oracle_assert_8[sizeof(sec_proto::LATransAdaptDPCmdMonitor) == 14 ? 1 : -1];
typedef char oracle_assert_9[sizeof(sec_proto::LATransAdaptDPCmdBufferMonitor) == 32786 ? 1 : -1];
typedef char oracle_assert_10[sizeof(sec_proto::SecsvrCaptchaReq) == 541 ? 1 : -1];
typedef char oracle_assert_11[sizeof(sec_proto::SecsvrCaptchaResultRsp) == 534 ? 1 : -1];
typedef char oracle_assert_12[sizeof(sec_proto::SecsvrGreenUnionRsp) == 5 ? 1 : -1];
typedef char oracle_assert_13[sizeof(sec_proto::SecsvrImInfoNotifyReq) == 38 ? 1 : -1];
typedef char oracle_assert_14[sizeof(sec_proto::SecsvrUserInfoNotifyReq) == 2056 ? 1 : -1];

using namespace sec_proto;

namespace {

const char *kOrigPath =
    "/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394";
const char *kReconPath = "/tmp/liblatrans_recon.so";

const unsigned kCapPack = 65536;         // > 32786, the largest wire frame
const unsigned kCapVis = 262144;         // > 32768 * sizeof(" 0x%02x")
const unsigned kShortVis = 64;           // forces the writeCharWithNull path
const unsigned kIters = 32;
int g_cases = 0;
int g_fail = 0;

uint32_t g_rng = 0x9e3779b9u;

uint32_t rnd() {
    g_rng = g_rng * 1103515245u + 12345u;
    return g_rng >> 8;
}

void fill_random(void *p, size_t n) {
    unsigned char *b = static_cast<unsigned char *>(p);
    for (size_t i = 0; i < n; ++i) b[i] = (unsigned char)(rnd() & 0xff);
}

// NUL-terminated fill for the char[] members.
void fill_text(void *p, size_t n) {
    char *b = static_cast<char *>(p);
    for (size_t i = 0; i < n; ++i) b[i] = (char)(1 + (rnd() % 0x7e));
    if (n) b[n - 1] = 0;
}

// mode 0 -> 0, mode 1 -> the whole array, mode 2 -> above the array bound
// (exercises TDR_ERR_REFER_SURPASS_COUNT, never used for writing) when the
// length field can hold such a value, otherwise random within [0, cap].
unsigned pick_len(unsigned mode, unsigned cap, unsigned long long field_max) {
    if (mode == 0) return 0;
    if (mode == 1) return cap;
    if (mode == 2 && (unsigned long long)cap + 1 <= field_max) {
        unsigned long long v = (unsigned long long)cap + 1 + (rnd() % 1024);
        if (v > field_max) v = field_max;
        return (unsigned)v;
    }
    return rnd() % (cap + 1);
}

// Never write past the array even when the declared length is above the bound:
// the count guard must reject the frame before any byte of the array is used.
unsigned fill_len(unsigned n, unsigned cap) { return n > cap ? cap : n; }

typedef int (*pack_t)(const void *, char *, unsigned, unsigned *, unsigned);
typedef int (*unpack_t)(void *, const char *, unsigned, unsigned *, unsigned);
typedef int (*vis_t)(const void *, char *, unsigned, unsigned *, int, char);
typedef const char *(*vise_t)(const void *, char *, unsigned, unsigned *, int,
                              char);
typedef int (*con_t)(void *);

// The three exported char*-overloads of a class, mangled by hand: the classes
// live directly in namespace sec_proto at namespace scope.
void sym_pack(char *out, size_t n, const char *cls) {
    snprintf(out, n, "_ZNK9sec_proto%u%s4packEPcjPjj", (unsigned)strlen(cls), cls);
}
void sym_unpack(char *out, size_t n, const char *cls) {
    snprintf(out, n, "_ZN9sec_proto%u%s6unpackEPKcjPjj", (unsigned)strlen(cls), cls);
}
void sym_vis(char *out, size_t n, const char *cls) {
    snprintf(out, n, "_ZNK9sec_proto%u%s9visualizeEPcjPjic", (unsigned)strlen(cls), cls);
}
void sym_vis_ex(char *out, size_t n, const char *cls) {
    snprintf(out, n, "_ZNK9sec_proto%u%s12visualize_exEPcjPjic",
             (unsigned)strlen(cls), cls);
}
void sym_construct(char *out, size_t n, const char *cls) {
    snprintf(out, n, "_ZN9sec_proto%u%s9constructEv", (unsigned)strlen(cls),
             cls);
}

void check(const char *label, const char *what, int a, int b, unsigned ua,
           unsigned ub, const char *ba, const char *bb, unsigned n) {
    ++g_cases;
    if (a != b || ua != ub || memcmp(ba, bb, n) != 0) {
        ++g_fail;
        unsigned d = n;
        for (unsigned i = 0; i < n; ++i) {
            if (ba[i] != bb[i]) {
                d = i;
                break;
            }
        }
        unsigned x = d < n ? (unsigned char)ba[d] : 0;
        unsigned y = d < n ? (unsigned char)bb[d] : 0;
        printf("FAIL %-30s %-10s ret %d/%d used %u/%u first-diff %u (%02x/%02x)\n",
               label, what, a, b, ua, ub, d, x, y);
    }
}

// ---------------------------------------------------------------------------
// length-field policies per class (random values + explicit edge values)
// ---------------------------------------------------------------------------

void fix_dpdata(LATransAdaptDPData &s, unsigned mode) {
    memset(&s, 0, sizeof(s));
    s.room_id_ = (uint16_t)rnd();
    s.time_stamp_ = rnd();
    s.dp_msg_id_ = (uint8_t)rnd();
    unsigned n = pick_len(mode, 32000, 0xffffu);
    s.data_len_ = (uint16_t)n;
    fill_random(s.data_, fill_len(n, 32000));
}

void fix_rpcode(LATransAdaptAntibotRPCodeResult &s, unsigned mode) {
    memset(&s, 0, sizeof(s));
    s.time_stamp_ = rnd();
    unsigned n = pick_len(mode, 32000, 0xffffu);
    s.rpcode_result_data_len_ = (uint16_t)n;
    fill_random(s.rpcode_result_data_, fill_len(n, 32000));
}

void fix_errmsg(LATransAdaptAntibotErrMsg &s, unsigned mode) {
    memset(&s, 0, sizeof(s));
    s.err_id_ = (int16_t)rnd();
    unsigned n = pick_len(mode, 1024, 0xffffu);
    s.err_msg_len_ = (uint16_t)n;
    fill_text(s.err_msg_, fill_len(n, 1024));
}

void fix_rolename(LATransAdaptAntibotRoleName &s, unsigned mode) {
    memset(&s, 0, sizeof(s));
    unsigned n = pick_len(mode, 1024, 0xffffu);
    s.role_name_len_ = (uint16_t)n;
    fill_text(s.role_name_, fill_len(n, 1024));
}

void fix_siginfo(LATransAdaptAntibotSigInfo &s, unsigned mode) {
    memset(&s, 0, sizeof(s));
    s.total_size_ = (uint16_t)rnd();
    s.ip_ = rnd();
    s.mac_id_ = rnd();
    unsigned n = pick_len(mode, 10240, 0xffffu);
    s.sig_len_ = (uint16_t)n;
    fill_random(s.sig_, fill_len(n, 10240));
}

void fix_clientip(LATransAdaptAntibotClientIP &s, unsigned mode) {
    memset(&s, 0, sizeof(s));
    s.flag_ = (uint8_t)rnd();
    s.client_intranet_ip_ = rnd();
    fill_random(s.client_key_signature_, sizeof(s.client_key_signature_));
}

void fix_rpfmc(LATransAdaptAntibotRPFMC &s, unsigned mode) {
    memset(&s, 0, sizeof(s));
    s.uin_ = rnd();
    unsigned n = pick_len(mode, 10240, 0xffffu);
    s.result_len_ = (uint16_t)n;
    fill_random(s.result_, fill_len(n, 10240));
}

void fix_dpcmd(LATransAdaptDPCmdMonitor &s, unsigned mode) {
    memset(&s, 0, sizeof(s));
    s.room_id_ = rnd();
    s.time_stamp_ = rnd();
    s.monitor_cmd_id_ = (int32_t)rnd();
    s.is_need_encrypt_ = (uint8_t)rnd();
    s.is_encrypted_ = (uint8_t)rnd();
}

void fix_dpbuf(LATransAdaptDPCmdBufferMonitor &s, unsigned mode) {
    memset(&s, 0, sizeof(s));
    s.room_id_ = rnd();
    s.time_stamp_ = rnd();
    s.monitor_cmd_id_ = (int32_t)rnd();
    s.is_need_encrypt_ = (uint8_t)rnd();
    s.is_encrypted_ = (uint8_t)rnd();
    unsigned n = pick_len(mode, 32768, 0xffffffffull);
    s.data_len_ = n;
    fill_random(s.data_, fill_len(n, 32768));
}

void fix_captchareq(SecsvrCaptchaReq &s, unsigned mode) {
    memset(&s, 0, sizeof(s));
    s.channel_id_ = (uint16_t)rnd();
    s.room_id_ = (uint16_t)rnd();
    s.version_ = rnd();
    s.bus_id_ = (uint16_t)rnd();
    s.time_stamp_ = rnd();
    s.client_ip_ = rnd();
    s.max_pic_change_num_ = (uint16_t)rnd();
    s.max_verify_num_ = (uint16_t)rnd();
    s.timeout_ = rnd();
    s.busi_id_ = (uint8_t)rnd();
    unsigned n = pick_len(mode, 512, 0xffffu);
    s.busi_info_len_ = (uint16_t)n;
    fill_random(s.busi_info_, fill_len(n, 512));
}

void fix_captcharsps(SecsvrCaptchaResultRsp &s, unsigned mode) {
    memset(&s, 0, sizeof(s));
    s.version_ = rnd();
    s.room_id_ = (uint16_t)rnd();
    s.bus_id_ = (uint16_t)rnd();
    s.time_stamp_ = rnd();
    s.client_ip_ = rnd();
    s.app_id_ = rnd();
    s.result_ = (uint8_t)rnd();
    // msg_len_ is uint8_t: the array bound (512) is unreachable, so the
    // over-bound policy does not exist for this type.
    unsigned n = pick_len(mode == 2 ? 3 : mode, 255, 0xffu);
    s.msg_len_ = (uint8_t)n;
    fill_random(s.msg_, fill_len(n, 512));
}

void fix_greenunion(SecsvrGreenUnionRsp &s, unsigned mode) {
    memset(&s, 0, sizeof(s));
    s.result_ = (uint8_t)rnd();
    s.flag_ = (uint8_t)rnd();
    s.world_ = (uint16_t)rnd();
    s.start_src_ = (uint8_t)rnd();
}

void fix_iminfo(SecsvrImInfoNotifyReq &s, unsigned mode) {
    memset(&s, 0, sizeof(s));
    s.msg_type_id_ = (uint16_t)rnd();
    s.world_id_ = (uint16_t)rnd();
    unsigned n = pick_len(mode, 32, 0xffffu);
    s.nick_name_len_ = (uint16_t)n;
    fill_text(s.nick_name_buf_, fill_len(n, 32));
}

void fix_userinfo(SecsvrUserInfoNotifyReq &s, unsigned mode) {
    memset(&s, 0, sizeof(s));
    s.msg_type_id_ = (uint16_t)rnd();
    s.world_id_ = (uint16_t)rnd();
    unsigned n = pick_len(mode, 2048, 0xffffffffull);
    s.data_len_ = n;
    fill_random(s.data_, fill_len(n, 2048));
}

// ---------------------------------------------------------------------------
// driver
// ---------------------------------------------------------------------------

template <typename T>
void run_class(const char *label, const char *cls, void *orig, void *recon,
               void (*fix)(T &, unsigned)) {
    char sp[128], su[128], sv[128], sx[128], sc[128];
    sym_pack(sp, sizeof(sp), cls);
    sym_unpack(su, sizeof(su), cls);
    sym_vis(sv, sizeof(sv), cls);
    sym_vis_ex(sx, sizeof(sx), cls);
    sym_construct(sc, sizeof(sc), cls);
    pack_t pk_o = reinterpret_cast<pack_t>(dlsym(orig, sp));
    pack_t pk_r = reinterpret_cast<pack_t>(dlsym(recon, sp));
    unpack_t up_o = reinterpret_cast<unpack_t>(dlsym(orig, su));
    unpack_t up_r = reinterpret_cast<unpack_t>(dlsym(recon, su));
    vis_t vi_o = reinterpret_cast<vis_t>(dlsym(orig, sv));
    vis_t vi_r = reinterpret_cast<vis_t>(dlsym(recon, sv));
    vise_t vx_o = reinterpret_cast<vise_t>(dlsym(orig, sx));
    vise_t vx_r = reinterpret_cast<vise_t>(dlsym(recon, sx));
    con_t cn_o = reinterpret_cast<con_t>(dlsym(orig, sc));
    con_t cn_r = reinterpret_cast<con_t>(dlsym(recon, sc));
    if (!pk_o || !pk_r || !up_o || !up_r || !vi_o || !vi_r || !vx_o || !vx_r ||
        !cn_o || !cn_r) {
        printf("MISSING %s syms (%d%d%d%d%d%d%d%d%d%d)\n", label, pk_o != 0,
               pk_r != 0, up_o != 0, up_r != 0, vi_o != 0, vi_r != 0,
               vx_o != 0, vx_r != 0, cn_o != 0, cn_r != 0);
        ++g_fail;
        return;
    }
    char *bo = new char[kCapPack];
    char *br = new char[kCapPack];
    char *vo = new char[kCapVis];
    char *vr = new char[kCapVis];
    char *so = new char[kCapVis];
    char *sr = new char[kCapVis];
    T *src = new T;
    T *uo = new T;
    T *ur = new T;
    for (unsigned it = 0; it < kIters; ++it) {
        const unsigned mode = it % 8 == 3 ? 0 : (it % 8 == 5 ? 1 : (it % 8 == 7 ? 2 : 3));
        memset(bo, 0, kCapPack);
        memset(br, 0, kCapPack);
        memset(vo, 0, kCapVis);
        memset(vr, 0, kCapVis);
        memset(so, 0, kCapVis);
        memset(sr, 0, kCapVis);
        memset(src, 0, sizeof(T));
        memset(uo, 0, sizeof(T));
        memset(ur, 0, sizeof(T));
        fix(*src, mode);

        // 1. pack into a full-size buffer
        unsigned uuo = 0, uur = 0;
        int ro = pk_o(src, bo, kCapPack, &uuo, 1);
        int rr = pk_r(src, br, kCapPack, &uur, 1);
        check(label, "pack", ro, rr, uuo, uur, bo, br, kCapPack);

        // 2. pack into a deliberately truncated buffer (0..64 bytes)
        unsigned su = rnd() % 65;  // same size for both sides
        unsigned uuS1 = 0xdeadbeefu, uuS2 = 0xdeadbeefu;
        int rs1 = pk_o(src, so, su, &uuS1, 1);
        int rs2 = pk_r(src, sr, su, &uuS2, 1);
        check(label, "pack-short", rs1, rs2, uuS1, uuS2, so, sr, kShortVis);

        // 3. unpack the identical frame produced above
        if (ro == 0 && rr == 0) {
            unsigned uu2 = 0, uu3 = 0;
            int ro2 = up_o(uo, bo, uuo, &uu2, 1);
            int rr2 = up_r(ur, bo, uuo, &uu3, 1);
            check(label, "unpack", ro2, rr2, uu2, uu3,
                  reinterpret_cast<const char *>(uo),
                  reinterpret_cast<const char *>(ur), sizeof(T));

            // 4. truncated frame
            if (uuo > 0) {
                memset(uo, 0, sizeof(T));
                memset(ur, 0, sizeof(T));
                unsigned uu4 = 0, uu5 = 0;
                int ro3 = up_o(uo, bo, uuo - 1, &uu4, 1);
                int rr3 = up_r(ur, bo, uuo - 1, &uu5, 1);
                check(label, "unpack-trunc", ro3, rr3, uu4, uu5,
                      reinterpret_cast<const char *>(uo),
                      reinterpret_cast<const char *>(ur), sizeof(T));
            }
            // 5. empty frame
            memset(uo, 0, sizeof(T));
            memset(ur, 0, sizeof(T));
            unsigned uu6 = 0, uu7 = 0;
            int ro4 = up_o(uo, bo, 0, &uu6, 1);
            int rr4 = up_r(ur, bo, 0, &uu7, 1);
            check(label, "unpack-empty", ro4, rr4, uu6, uu7,
                  reinterpret_cast<const char *>(uo),
                  reinterpret_cast<const char *>(ur), sizeof(T));
        }

        // 6. visualize into a full-size buffer
        unsigned uvo = 0, uvr = 0;
        int rvo = vi_o(src, vo, kCapVis, &uvo, 2, '\n');
        int rvr = vi_r(src, vr, kCapVis, &uvr, 2, '\n');
        check(label, "visualize", rvo, rvr, uvo, uvr, vo, vr, kCapVis);

        // 7. visualize into a buffer that runs out (separator write included)
        unsigned uvs = 0, uvt = 0;
        int rvs = vi_o(src, so, kShortVis, &uvs, 2, '\n');
        int rvt = vi_r(src, sr, kShortVis, &uvt, 2, '\n');
        check(label, "vis-short", rvs, rvt, uvs, uvt, so, sr, kShortVis);

        // 8. NULL-buffer guards (TDR_ERR_NULL_POINTER_PARAMETER, nothing written)
        unsigned uu8 = 0xdeadbeefu, uu9 = 0xdeadbeefu;
        int rn1 = pk_o(src, NULL, kCapPack, &uu8, 1);
        int rn2 = pk_r(src, NULL, kCapPack, &uu9, 1);
        check(label, "pack-null", rn1, rn2, uu8, uu9, bo, br, 1);
        int rn3 = up_o(uo, NULL, 0, &uu8, 1);
        int rn4 = up_r(ur, NULL, 0, &uu9, 1);
        check(label, "unpack-null", rn3, rn4, uu8, uu9, bo, br, 1);
        int rn5 = vi_o(src, NULL, kCapVis, &uu8, 2, '\n');
        int rn6 = vi_r(src, NULL, kCapVis, &uu9, 2, '\n');
        check(label, "vis-null", rn5, rn6, uu8, uu9, bo, br, 1);

        // 9. visualize_ex(char*,...): NUL-terminated return value + used size
        unsigned uxo = 0, uxr = 0;
        const char *po = vx_o(src, so, kShortVis, &uxo, 2, '\n');
        const char *pr = vx_r(src, sr, kShortVis, &uxr, 2, '\n');
        check(label, "vis-ex", po ? (int)strlen(po) : -1,
              pr ? (int)strlen(pr) : -1, uxo, uxr, so, sr, kShortVis);

        // 10. visualize_ex() with size 0 -> static "", usedSize untouched
        unsigned uyo = 0xdeadbeefu, uyr = 0xdeadbeefu;
        po = vx_o(src, so, 0, &uyo, 2, '\n');
        pr = vx_r(src, sr, 0, &uyr, 2, '\n');
        check(label, "vis-ex-empty", po ? (int)strlen(po) : -1,
              pr ? (int)strlen(pr) : -1, uyo, uyr, so, sr, 8);

        // 11. construct() must zero the whole object on both sides
        memset(uo, 0xaa, sizeof(T));
        memset(ur, 0xaa, sizeof(T));
        int rc1 = cn_o(uo);
        int rc2 = cn_r(ur);
        check(label, "construct", rc1, rc2, 0, 0,
              reinterpret_cast<const char *>(uo),
              reinterpret_cast<const char *>(ur), sizeof(T));
    }
    delete[] bo;
    delete[] br;
    delete[] vo;
    delete[] vr;
    delete[] so;
    delete[] sr;
    delete src;
    delete uo;
    delete ur;
}

}  // namespace

int main() {
    void *orig = dlopen(kOrigPath, RTLD_NOW | RTLD_GLOBAL);
    if (!orig) {
        printf("dlopen ORIG failed: %s\n", dlerror());
        return 2;
    }
    void *recon = dlopen(kReconPath, RTLD_NOW | RTLD_LOCAL);
    if (!recon) {
        printf("dlopen recon failed: %s\n", dlerror());
        return 2;
    }
    printf("loaded; sizeof latrans=%u/%u/%u/%u/%u/%u/%u/%u/%u svx=%u/%u/%u/%u/%u\n",
           (unsigned)sizeof(LATransAdaptDPData),
           (unsigned)sizeof(LATransAdaptAntibotRPCodeResult),
           (unsigned)sizeof(LATransAdaptAntibotErrMsg),
           (unsigned)sizeof(LATransAdaptAntibotRoleName),
           (unsigned)sizeof(LATransAdaptAntibotSigInfo),
           (unsigned)sizeof(LATransAdaptAntibotClientIP),
           (unsigned)sizeof(LATransAdaptAntibotRPFMC),
           (unsigned)sizeof(LATransAdaptDPCmdMonitor),
           (unsigned)sizeof(LATransAdaptDPCmdBufferMonitor),
           (unsigned)sizeof(SecsvrCaptchaReq),
           (unsigned)sizeof(SecsvrCaptchaResultRsp),
           (unsigned)sizeof(SecsvrGreenUnionRsp),
           (unsigned)sizeof(SecsvrImInfoNotifyReq),
           (unsigned)sizeof(SecsvrUserInfoNotifyReq));
    fflush(stdout);

    run_class<LATransAdaptDPData>("LATransAdaptDPData",
                                  "LATransAdaptDPData", orig, recon, fix_dpdata);
    run_class<LATransAdaptAntibotRPCodeResult>(
        "LATransAdaptAntibotRPCodeResult", "LATransAdaptAntibotRPCodeResult",
        orig, recon, fix_rpcode);
    run_class<LATransAdaptAntibotErrMsg>("LATransAdaptAntibotErrMsg",
                                         "LATransAdaptAntibotErrMsg", orig,
                                         recon, fix_errmsg);
    run_class<LATransAdaptAntibotRoleName>("LATransAdaptAntibotRoleName",
                                           "LATransAdaptAntibotRoleName", orig,
                                           recon, fix_rolename);
    run_class<LATransAdaptAntibotSigInfo>("LATransAdaptAntibotSigInfo",
                                          "LATransAdaptAntibotSigInfo", orig,
                                          recon, fix_siginfo);
    run_class<LATransAdaptAntibotClientIP>("LATransAdaptAntibotClientIP",
                                           "LATransAdaptAntibotClientIP", orig,
                                           recon, fix_clientip);
    run_class<LATransAdaptAntibotRPFMC>("LATransAdaptAntibotRPFMC",
                                        "LATransAdaptAntibotRPFMC", orig, recon,
                                        fix_rpfmc);
    run_class<LATransAdaptDPCmdMonitor>("LATransAdaptDPCmdMonitor",
                                        "LATransAdaptDPCmdMonitor", orig, recon,
                                        fix_dpcmd);
    run_class<LATransAdaptDPCmdBufferMonitor>(
        "LATransAdaptDPCmdBufferMonitor", "LATransAdaptDPCmdBufferMonitor", orig,
        recon, fix_dpbuf);
    run_class<SecsvrCaptchaReq>("SecsvrCaptchaReq", "SecsvrCaptchaReq", orig,
                                recon, fix_captchareq);
    run_class<SecsvrCaptchaResultRsp>("SecsvrCaptchaResultRsp",
                                      "SecsvrCaptchaResultRsp", orig, recon,
                                      fix_captcharsps);
    run_class<SecsvrGreenUnionRsp>("SecsvrGreenUnionRsp", "SecsvrGreenUnionRsp",
                                   orig, recon, fix_greenunion);
    run_class<SecsvrImInfoNotifyReq>("SecsvrImInfoNotifyReq",
                                     "SecsvrImInfoNotifyReq", orig, recon,
                                     fix_iminfo);
    run_class<SecsvrUserInfoNotifyReq>("SecsvrUserInfoNotifyReq",
                                       "SecsvrUserInfoNotifyReq", orig, recon,
                                       fix_userinfo);

    printf("cases=%d failures=%d\n", g_cases, g_fail);
    return g_fail ? 1 : 0;
}
