// Differential oracle: reconstructed formmog_proto_tss_sdk_client.cpp vs ORIG
// (libtss_sdk.so.1.0.394, tag 394).
//
// ORIG is dlopen()ed RTLD_GLOBAL so the reconstructed object's TdrBufUtil
// references resolve to the ORIG runtime; the reconstructed object is
// dlopen()ed RTLD_LOCAL and called through dlsym (mangled names are built from
// the class name + member suffix, so every name is checked to resolve).
//
// Compared per case, on identical inputs:
//   pack(char*, size, &used, cutVer)   ret, used, full output buffer
//   unpack(buf, used, &used, cutVer)   ret, used, full struct bytes
//   visualize(char*, size, &used, ..)  ret, used, full output buffer
//
// Safety of the random inputs (so neither side reads past an array):
//   * every length field is drawn inside its declared array bound:
//     error_msg_len_ <= 128, need_encrypt_count_ <= 128,
//     client_error_count_ <= 128, up/down_cmd_count_ <= 128,
//     pieceDataSize <= 51200, pad_len_ <= 255 (array 256);
//   * every byte-array payload is NUL-terminated (harmless for raw byte
//     arrays, required for the "string" arrays);
//   * union selectors are exercised explicitly (all 15 DPInterData arms, all
//     3 TClientPkg arms, several TClientFrame selectors) plus out-of-range
//     selectors to differential the default arm;
//   * TClientPkg::head.body_len_ is also corrupted deliberately (0xffff, 0,
//     exact) to differential the pre-body length check.
#include <dlfcn.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "formmog_proto_tss_sdk_client.h"

using namespace sec_proto;

namespace {

const char *kOrigPath =
    "/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394";
const char *kReconPath = "/tmp/libclient_recon.so";

const unsigned kCap = 1u << 18;  // 262144: worst visualize is ~205 KB
const unsigned kIters = 120;
int g_cases = 0;
int g_fail = 0;

uint32_t g_rng = 0x394c11e7u;
uint32_t rnd() {
    g_rng = g_rng * 1103515245u + 12345u;
    return g_rng >> 8;
}
unsigned pick(unsigned n) {
    return n ? (unsigned)(rnd() % n) : 0u;
}

// Printable payload, always NUL-terminated (last byte), like a C string.
void fill_text(uint8_t *p, unsigned n) {
    for (unsigned i = 0; i + 1 < n; ++i) p[i] = (uint8_t)(1 + (rnd() % 0x7e));
    if (n) p[n - 1] = 0;
}

typedef int (*pack_t)(const void *, char *, unsigned, unsigned *, unsigned);
typedef int (*unpack_t)(void *, const char *, unsigned, unsigned *, unsigned);
typedef int (*vis_t)(const void *, char *, unsigned, unsigned *, int, char);
typedef int (*pack_sel_t)(const void *, int64_t, char *, unsigned, unsigned *,
                          unsigned);
typedef int (*unpack_sel_t)(void *, int64_t, const char *, unsigned,
                            unsigned *, unsigned);
typedef int (*vis_sel_t)(const void *, int64_t, char *, unsigned, unsigned *,
                         int, char);

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
        printf("FAIL %-22s %-10s ret %d/%d used %u/%u first-diff %u (%02x/%02x)\n",
               label, what, a, b, ua, ub, d, x, y);
    }
}

// _ZNK/_ZN + 9sec_proto + <len><Class> + <tail>
void mangle(char *out, unsigned n, bool is_const, const char *cls,
            const char *tail) {
    snprintf(out, n, "%s9sec_proto%d%s%s", is_const ? "_ZNK" : "_ZN",
             (int)strlen(cls), cls, tail);
}

// ---------------------------------------------------------------------------
// random but in-bounds struct fillers (struct is zeroed by the caller)
// ---------------------------------------------------------------------------

void fix_client_code(DPClientCode &x) { x.code_id_ = rnd(); }

void fix_bin_count(DPBinCount &x) { x.bin_count_ = (uint16_t)rnd(); }

void fix_bin_file(DPBinFile &x) {
    x.length = (uint16_t)rnd();
    x.bin_code_id_ = (uint16_t)rnd();
}

void fix_config(DPConfigData &x) {
    x.proto_check_switch_ = (uint8_t)rnd();
    x.seq_check_switch_ = (uint8_t)rnd();
    x.raw_check_switch_ = (uint8_t)rnd();
}

void fix_dyn_head(DPDynamicAlgorithmHead &x) {
    x.code_id_ = rnd();
    x.crc_ = rnd();
}

void fix_fix_head(DPFixAlgorithmHead &x) {
    x.key_id_ = rnd();
    x.algorithm_type_ = rnd();
    x.crc = rnd();
}

void fix_decrypt(DPClientDecryptInfo &x) {
    x.decrypt_id_ = rnd();
    x.encrypt_id_ = rnd();
}

void fix_short_cmd(DpClientShortCmdData &x) {
    x.cmd_id_ = (uint16_t)rnd();
    x.cmd_count_ = (uint16_t)rnd();
}

void fix_cmd(DPClientCmdData &x) {
    x.cmd_id_ = rnd();
    x.cmd_count_ = rnd();
}

void fix_log(DPClientLogPtc &x) {
    x.error_id_ = rnd();
    x.is_encrypt_ = (uint8_t)rnd();
    unsigned n = pick(129);  // error_msg_[128]
    x.error_msg_len_ = (uint8_t)n;
    fill_text(x.error_msg_, n);
}

void fix_request(DPProtocolRequestCode &x) {
    x.size = (uint16_t)rnd();
    x.encrypt_code_id_ = rnd();
    x.decrypt_code_id_ = rnd();
    x.bin_file_id_ = (uint16_t)rnd();
}

void fix_inter_head(DPInterDataHead &x) {
    x.packet_size_ = (uint16_t)rnd();
    x.protocol_type_ = (uint16_t)rnd();
    x.data_type_ = (uint8_t)rnd();
}

void fix_frame_head(TClientFrameHead &x) {
    x.packetLen = rnd();
    x.totalSize = (uint16_t)rnd();
    x.protoId = (uint16_t)rnd();
}

void fix_safe_mode(TSafeMode &x) { x.iSafemodeLevel = rnd(); }

void fix_special(TSpecialControl &x) {
    x.shPacketSize = (uint16_t)rnd();
    x.shPacketType = (uint16_t)rnd();
    x.chDataType = (uint8_t)rnd();
    x.cmdid = rnd();
    x.bufLen = rnd();
}

void fix_revive(TReviveMouse &x) {
    x.cmdID = rnd();
    x.isHint = rnd();
}

void fix_pkg_head(ClientPkgHead &x) {
    x.protocol_id_ = (uint16_t)rnd();
    x.seq_ = rnd();
    x.sln_id_ = (uint16_t)rnd();
    x.sub_sln_id_ = (uint16_t)rnd();
    x.body_len_ = (uint16_t)rnd();
}

void fix_init_data(DPClientInitDataPtc &x) {
    fill_text(x.code_ver_, 16);
    x.client_info_ = rnd();
    x.client_user_id_ = rnd();
    x.client_code_id_ = rnd();
    x.server_code_id_ = rnd();
    x.bin_code_id_ = (uint16_t)rnd();
    x.bin_count_ = (uint16_t)rnd();
}

void fix_piece(TClientFramePiece &x) {
    x.totalPiece = (uint16_t)rnd();
    x.pieceSerial = (uint16_t)rnd();
    x.totalCrc = rnd();
    unsigned n = pick(51201);  // pieceData[51200]
    x.pieceDataSize = (uint16_t)n;
    fill_text(x.pieceData, n);
}

void fix_cmd_list(DPCmdList &x) {
    unsigned up = pick(129), down = pick(129);  // [128] each
    x.up_cmd_count_ = (uint16_t)up;
    x.down_cmd_count_ = (uint16_t)down;
    for (unsigned i = 0; i < up; ++i) x.up_cmd_list_[i] = rnd();
    for (unsigned i = 0; i < down; ++i) x.down_cmd_list_[i] = rnd();
}

void fix_server_code(DPServerCode &x) {
    fill_text(x.client_version_, 16);
    x.code_id_ = rnd();
}

void fix_stat(DPClientStatPtc &x) {
    x.internal_msg_send_count_ = rnd();
    x.internal_msg_recv_count_ = rnd();
    x.game_pkg_recv_count_ = rnd();
    x.fix_encrypt_count_ = rnd();
    x.game_fix_decrypt_count_ = rnd();
    x.game_dynamic_decrypt_count_ = rnd();
    x.seq_less_count_ = rnd();
    x.seq_greater_count_ = rnd();
    x.seq_discard_count_ = rnd();
    x.down_cmd_list_crc_ = rnd();
    unsigned ne = pick(129);   // need_encrypt_list_[128]
    x.need_encrypt_count_ = (uint16_t)ne;
    for (unsigned i = 0; i < ne; ++i) fix_cmd(x.need_encrypt_list_[i]);
    unsigned ce = pick(129);   // client_error_list_[128]
    x.client_error_count_ = (uint16_t)ce;
    for (unsigned i = 0; i < ce; ++i) fix_short_cmd(x.client_error_list_[i]);
}

void fix_algorithm(DPAlgorithmData &x) {
    x.usr_id_crc_ = rnd();
    x.custom_data_ = rnd();
    unsigned n = pick(256);  // pad_data_[256]; pad_len_ is uint8_t
    x.pad_len_ = (uint8_t)n;
    fill_text(x.pad_data_, n);
}

// ---------------------------------------------------------------------------
// union / container fillers
// ---------------------------------------------------------------------------

const int kInterSels[] = {1, 2, 3, 4, 5, 6, 9, 10, 11, 12, 13, 14, 15, 17, 20};
const int kInterBad[] = {0, 7, 8, 16, 18, 19, 21, 0xffff};
const int kPkgSels[] = {0x135, 0x136, 0x137};
const int kPkgBad[] = {0, 1, 0x134, 0x138, 0xffff};
const int kFrameSels[] = {0, 1, 0x135, 0x9999};

void fix_body(DPInterDataBody &b, int sel) {
    memset(&b, 0, sizeof(b));
    switch (sel) {
        case 1: fix_client_code(b.client_code_); break;
        case 2: fix_dyn_head(b.dynamic_algorithm_data_); break;
        case 3: fix_bin_file(b.bin_file_); break;
        case 4: fix_cmd_list(b.cmd_list); break;
        case 5: fix_bin_count(b.bin_count_); break;
        case 6: fix_server_code(b.server_code_); break;
        case 9: fix_request(b.request_code_); break;
        case 10: fix_init_data(b.init_data_ptc_); break;
        case 11: fix_stat(b.client_stat_ptc_); break;
        case 12: fix_decrypt(b.decrypt_info_); break;
        case 13: fix_log(b.client_log_ptc_); break;
        case 14: fill_text(b.cs_test_data_, 10); break;
        case 15: fill_text(b.sc_test_data_, 10); break;
        case 17: fix_fix_head(b.fix_algorithm_data_); break;
        case 20: fix_config(b.config_data_); break;
        default: break;  // default arm: no member touched, both sides agree
    }
}

void fix_pkg_body(TClientPkgBody &b, int sel) {
    memset(&b, 0, sizeof(b));
    switch (sel) {
        case 0x135: fix_revive(b.reviseMouse); break;
        case 0x136: fix_special(b.specialControl); break;
        case 0x137: fix_safe_mode(b.safeMode); break;
        default: break;
    }
}

void fix_frame_body(TClientFrameBody &b, int sel) {
    (void)sel;  // ORIG ignores the selector for the single-arm union
    memset(&b, 0, sizeof(b));
    fix_piece(b.framePiece);
}

void fix_interdata(DPInterData &p) {
    static unsigned c = 0;
    int sel = kInterSels[c++ % (sizeof(kInterSels) / sizeof(kInterSels[0]))];
    memset(&p, 0, sizeof(p));
    p.head.packet_size_ = (uint16_t)rnd();
    p.head.protocol_type_ = (uint16_t)sel;
    p.head.data_type_ = (uint8_t)rnd();
    fix_body(p.body, sel);
}

void fix_frame(TClientFrame &f) {
    memset(&f, 0, sizeof(f));
    f.head.packetLen = rnd();
    f.head.totalSize = (uint16_t)rnd();
    f.head.protoId = (uint16_t)rnd();
    fix_piece(f.body.framePiece);
}

void fix_pkg(TClientPkg &p) {
    static unsigned c = 0;
    int sel = kPkgSels[c++ % (sizeof(kPkgSels) / sizeof(kPkgSels[0]))];
    memset(&p, 0, sizeof(p));
    p.head.protocol_id_ = (uint16_t)rnd();
    p.head.seq_ = rnd();
    p.head.sln_id_ = (uint16_t)rnd();
    p.head.sub_sln_id_ = (uint16_t)sel;
    p.head.body_len_ = (uint16_t)rnd();
    fix_pkg_body(p.body, sel);
}

// ---------------------------------------------------------------------------
// harness
// ---------------------------------------------------------------------------

template <typename T>
void run_plain(const char *label, const char *cls, void *orig, void *recon,
               void (*fix)(T &)) {
    char sp[256], su[256], sv[256];
    mangle(sp, sizeof(sp), true, cls, "4packEPcjPjj");
    mangle(su, sizeof(su), false, cls, "6unpackEPKcjPjj");
    mangle(sv, sizeof(sv), true, cls, "9visualizeEPcjPjic");
    pack_t pk_o = reinterpret_cast<pack_t>(dlsym(orig, sp));
    pack_t pk_r = reinterpret_cast<pack_t>(dlsym(recon, sp));
    unpack_t up_o = reinterpret_cast<unpack_t>(dlsym(orig, su));
    unpack_t up_r = reinterpret_cast<unpack_t>(dlsym(recon, su));
    vis_t vi_o = reinterpret_cast<vis_t>(dlsym(orig, sv));
    vis_t vi_r = reinterpret_cast<vis_t>(dlsym(recon, sv));
    if (!pk_o || !pk_r || !up_o || !up_r || !vi_o || !vi_r) {
        printf("MISSING %s %s\n", label, sp);
        ++g_fail;
        return;
    }
    char *bo = new char[kCap];
    char *br = new char[kCap];
    char *vo = new char[kCap];
    char *vr = new char[kCap];
    T *src = new T;
    T *uo = new T;
    T *ur = new T;
    for (unsigned it = 0; it < kIters; ++it) {
        memset(bo, 0, kCap);
        memset(br, 0, kCap);
        memset(vo, 0, kCap);
        memset(vr, 0, kCap);
        memset(src, 0, sizeof(T));
        memset(uo, 0, sizeof(T));
        memset(ur, 0, sizeof(T));
        fix(*src);
        unsigned uuo = 0, uur = 0, uu2 = 0, uu3 = 0;
        int ro = pk_o(src, bo, kCap, &uuo, 1);
        int rr = pk_r(src, br, kCap, &uur, 1);
        check(label, "pack", ro, rr, uuo, uur, bo, br, kCap);
        if (ro == 0 && rr == 0) {
            int ro2 = up_o(uo, bo, uuo, &uu2, 1);
            int rr2 = up_r(ur, bo, uuo, &uu3, 1);
            check(label, "unpack", ro2, rr2, uu2, uu3,
                  reinterpret_cast<const char *>(uo),
                  reinterpret_cast<const char *>(ur), sizeof(T));
            // Short-buffer differential: a truncated prefix must fail (or
            // succeed) identically and consume the same prefix.  Both sides
            // bound every read by the size passed here, so this is safe.
            if (uuo > 1) {
                const unsigned short_size[2] = {uuo / 2, uuo - 1};
                for (unsigned ti = 0; ti < 2; ++ti) {
                    unsigned ts = short_size[ti];
                    memset(uo, 0, sizeof(T));
                    memset(ur, 0, sizeof(T));
                    unsigned a = 0, b = 0;
                    int sa = up_o(uo, bo, ts, &a, 1);
                    int sb = up_r(ur, bo, ts, &b, 1);
                    check(label, ti ? "unpack-short1" : "unpack-short2", sa,
                          sb, a, b, reinterpret_cast<const char *>(uo),
                          reinterpret_cast<const char *>(ur), sizeof(T));
                }
            }
        }
        unsigned uvo = 0, uvr = 0;
        int rvo = vi_o(src, vo, kCap, &uvo, 2, '\n');
        int rvr = vi_r(src, vr, kCap, &uvr, 2, '\n');
        check(label, "visualize", rvo, rvr, uvo, uvr, vo, vr, kCap);
    }
    delete[] bo;
    delete[] br;
    delete[] vo;
    delete[] vr;
    delete src;
    delete uo;
    delete ur;
}

// Union bodies: pack/unpack/visualize take an explicit int64_t selector.
template <typename T>
void run_sel(const char *label, const char *cls, void *orig, void *recon,
             const int *sels, unsigned nsel, void (*fix)(T &, int)) {
    char sp[256], su[256], sv[256];
    mangle(sp, sizeof(sp), true, cls, "4packExPcjPjj");
    mangle(su, sizeof(su), false, cls, "6unpackExPKcjPjj");
    mangle(sv, sizeof(sv), true, cls, "9visualizeExPcjPjic");
    pack_sel_t pk_o = reinterpret_cast<pack_sel_t>(dlsym(orig, sp));
    pack_sel_t pk_r = reinterpret_cast<pack_sel_t>(dlsym(recon, sp));
    unpack_sel_t up_o = reinterpret_cast<unpack_sel_t>(dlsym(orig, su));
    unpack_sel_t up_r = reinterpret_cast<unpack_sel_t>(dlsym(recon, su));
    vis_sel_t vi_o = reinterpret_cast<vis_sel_t>(dlsym(orig, sv));
    vis_sel_t vi_r = reinterpret_cast<vis_sel_t>(dlsym(recon, sv));
    if (!pk_o || !pk_r || !up_o || !up_r || !vi_o || !vi_r) {
        printf("MISSING %s %s\n", label, sp);
        ++g_fail;
        return;
    }
    char *bo = new char[kCap];
    char *br = new char[kCap];
    char *vo = new char[kCap];
    char *vr = new char[kCap];
    T *src = new T;
    T *uo = new T;
    T *ur = new T;
    for (unsigned it = 0; it < kIters; ++it) {
        for (unsigned si = 0; si < nsel; ++si) {
            int sel = sels[si];
            memset(bo, 0, kCap);
            memset(br, 0, kCap);
            memset(vo, 0, kCap);
            memset(vr, 0, kCap);
            memset(src, 0, sizeof(T));
            memset(uo, 0, sizeof(T));
            memset(ur, 0, sizeof(T));
            fix(*src, sel);
            unsigned uuo = 0, uur = 0, uu2 = 0, uu3 = 0;
            int ro = pk_o(src, sel, bo, kCap, &uuo, 1);
            int rr = pk_r(src, sel, br, kCap, &uur, 1);
            check(label, "pack", ro, rr, uuo, uur, bo, br, kCap);
            if (ro == 0 && rr == 0) {
                int ro2 = up_o(uo, sel, bo, uuo, &uu2, 1);
                int rr2 = up_r(ur, sel, bo, uuo, &uu3, 1);
                check(label, "unpack", ro2, rr2, uu2, uu3,
                      reinterpret_cast<const char *>(uo),
                      reinterpret_cast<const char *>(ur), sizeof(T));
            }
            unsigned uvo = 0, uvr = 0;
            int rvo = vi_o(src, sel, vo, kCap, &uvo, 2, '\n');
            int rvr = vi_r(src, sel, vr, kCap, &uvr, 2, '\n');
            check(label, "visualize", rvo, rvr, uvo, uvr, vo, vr, kCap);
        }
    }
    delete[] bo;
    delete[] br;
    delete[] vo;
    delete[] vr;
    delete src;
    delete uo;
    delete ur;
}

// TClientPkg: differential the stamped body_len_ and the pre-body length check.
void run_pkg_len(void *orig, void *recon) {
    char sp[256], su[256];
    mangle(sp, sizeof(sp), true, "TClientPkg", "4packEPcjPjj");
    mangle(su, sizeof(su), false, "TClientPkg", "6unpackEPKcjPjj");
    pack_t pk_o = reinterpret_cast<pack_t>(dlsym(orig, sp));
    unpack_t up_o = reinterpret_cast<unpack_t>(dlsym(orig, su));
    unpack_t up_r = reinterpret_cast<unpack_t>(dlsym(recon, su));
    if (!pk_o || !up_o || !up_r) {
        printf("MISSING TClientPkgLen %s\n", sp);
        ++g_fail;
        return;
    }
    char *bo = new char[kCap];
    char *bb = new char[kCap];
    TClientPkg *src = new TClientPkg;
    TClientPkg *uo = new TClientPkg;
    TClientPkg *ur = new TClientPkg;
    for (unsigned it = 0; it < kIters; ++it) {
        memset(bo, 0, kCap);
        memset(src, 0, sizeof(*src));
        fix_pkg(*src);
        unsigned uuo = 0;
        int ro = pk_o(src, bo, kCap, &uuo, 1);
        ++g_cases;
        if (ro != 0 || uuo < 12) {
            ++g_fail;
            printf("FAIL PkgStamp pack ret=%d used=%u\n", ro, uuo);
            continue;
        }
        // Stamp itself: ORIG must have written used - startPos at +10.
        unsigned stamp = (unsigned)(unsigned char)bo[10] * 256u +
                         (unsigned char)bo[11];
        ++g_cases;
        if (stamp != uuo - 12) {
            ++g_fail;
            printf("FAIL PkgStamp value %u expected %u\n", stamp, uuo - 12);
        }
        static const unsigned kBad[] = {0xffffu, 0u, 0};
        for (unsigned vi = 0; vi < 3; ++vi) {
            unsigned blen = vi == 2 ? uuo - 12 : kBad[vi];
            memcpy(bb, bo, uuo);
            bb[10] = (char)(blen >> 8);
            bb[11] = (char)(blen & 0xff);
            memset(uo, 0, sizeof(*uo));
            memset(ur, 0, sizeof(*ur));
            unsigned a = 0, b = 0;
            int ra = up_o(uo, bb, uuo, &a, 1);
            int rb = up_r(ur, bb, uuo, &b, 1);
            check(vi == 2 ? "PkgLenExact" : (vi == 1 ? "PkgLenZero" : "PkgLenBig"),
                  "unpack", ra, rb, a, b,
                  reinterpret_cast<const char *>(uo),
                  reinterpret_cast<const char *>(ur), sizeof(*uo));
        }
    }
    delete[] bo;
    delete[] bb;
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
    printf("loaded; sizes: DPInterData=%u TClientFrame=%u TClientPkg=%u "
           "DPInterDataBody=%u TClientFrameBody=%u TClientPkgBody=%u\n",
           (unsigned)sizeof(DPInterData), (unsigned)sizeof(TClientFrame),
           (unsigned)sizeof(TClientPkg), (unsigned)sizeof(DPInterDataBody),
           (unsigned)sizeof(TClientFrameBody), (unsigned)sizeof(TClientPkgBody));

    run_plain<DPClientCode>("DPClientCode", "DPClientCode", orig, recon,
                            fix_client_code);
    run_plain<DPBinCount>("DPBinCount", "DPBinCount", orig, recon,
                          fix_bin_count);
    run_plain<DPBinFile>("DPBinFile", "DPBinFile", orig, recon, fix_bin_file);
    run_plain<DPConfigData>("DPConfigData", "DPConfigData", orig, recon,
                            fix_config);
    run_plain<DPDynamicAlgorithmHead>("DPDynamicAlgorithmHead",
                                      "DPDynamicAlgorithmHead", orig, recon,
                                      fix_dyn_head);
    run_plain<DPFixAlgorithmHead>("DPFixAlgorithmHead", "DPFixAlgorithmHead",
                                  orig, recon, fix_fix_head);
    run_plain<DPClientDecryptInfo>("DPClientDecryptInfo", "DPClientDecryptInfo",
                                   orig, recon, fix_decrypt);
    run_plain<DpClientShortCmdData>("DpClientShortCmdData",
                                    "DpClientShortCmdData", orig, recon,
                                    fix_short_cmd);
    run_plain<DPClientCmdData>("DPClientCmdData", "DPClientCmdData", orig,
                               recon, fix_cmd);
    run_plain<DPClientLogPtc>("DPClientLogPtc", "DPClientLogPtc", orig, recon,
                              fix_log);
    run_plain<DPProtocolRequestCode>("DPProtocolRequestCode",
                                     "DPProtocolRequestCode", orig, recon,
                                     fix_request);
    run_plain<DPInterDataHead>("DPInterDataHead", "DPInterDataHead", orig,
                               recon, fix_inter_head);
    run_plain<TClientFrameHead>("TClientFrameHead", "TClientFrameHead", orig,
                                recon, fix_frame_head);
    run_plain<TSafeMode>("TSafeMode", "TSafeMode", orig, recon, fix_safe_mode);
    run_plain<TSpecialControl>("TSpecialControl", "TSpecialControl", orig,
                               recon, fix_special);
    run_plain<TReviveMouse>("TReviveMouse", "TReviveMouse", orig, recon,
                            fix_revive);
    run_plain<ClientPkgHead>("ClientPkgHead", "ClientPkgHead", orig, recon,
                             fix_pkg_head);
    run_plain<DPClientInitDataPtc>("DPClientInitDataPtc", "DPClientInitDataPtc",
                                   orig, recon, fix_init_data);
    run_plain<TClientFramePiece>("TClientFramePiece", "TClientFramePiece", orig,
                                 recon, fix_piece);
    run_plain<DPCmdList>("DPCmdList", "DPCmdList", orig, recon, fix_cmd_list);
    run_plain<DPServerCode>("DPServerCode", "DPServerCode", orig, recon,
                            fix_server_code);
    run_plain<DPClientStatPtc>("DPClientStatPtc", "DPClientStatPtc", orig,
                               recon, fix_stat);
    run_plain<DPAlgorithmData>("DPAlgorithmData", "DPAlgorithmData", orig,
                               recon, fix_algorithm);
    run_plain<DPInterData>("DPInterData", "DPInterData", orig, recon,
                           fix_interdata);
    run_plain<TClientFrame>("TClientFrame", "TClientFrame", orig, recon,
                            fix_frame);
    run_plain<TClientPkg>("TClientPkg", "TClientPkg", orig, recon, fix_pkg);

    run_sel<DPInterDataBody>(
        "BodySels", "DPInterDataBody", orig, recon, kInterSels,
        sizeof(kInterSels) / sizeof(kInterSels[0]), fix_body);
    run_sel<DPInterDataBody>(
        "BodyBadSels", "DPInterDataBody", orig, recon, kInterBad,
        sizeof(kInterBad) / sizeof(kInterBad[0]), fix_body);
    run_sel<TClientPkgBody>("PkgBodySels", "TClientPkgBody", orig, recon,
                            kPkgSels,
                            sizeof(kPkgSels) / sizeof(kPkgSels[0]),
                            fix_pkg_body);
    run_sel<TClientPkgBody>("PkgBodyBad", "TClientPkgBody", orig, recon,
                            kPkgBad, sizeof(kPkgBad) / sizeof(kPkgBad[0]),
                            fix_pkg_body);
    run_sel<TClientFrameBody>("FrameBodySels", "TClientFrameBody", orig, recon,
                              kFrameSels,
                              sizeof(kFrameSels) / sizeof(kFrameSels[0]),
                              fix_frame_body);

    run_pkg_len(orig, recon);

    printf("cases=%d failures=%d\n", g_cases, g_fail);
    return g_fail ? 1 : 0;
}
