// Shared TSS SDK frame payload used by the proto_proc and captcha plugin TUs.
// Layout verified against ORIG DWARF (sec_proto::RecvDataFromSDKReq, 32785 B):
//   channel_id_+0 u16, room_id_+2 u16, time_stamp_+4 u32,
//   to_busi_id_+8 u16, from_busi_id_+10 u16, to_secsvr_seq_+12 u32,
//   data_len_+16 u16, data_+18 u8[32767].
#ifndef TSS_SDK_RECON_SDK_RECV_PKG_H_
#define TSS_SDK_RECON_SDK_RECV_PKG_H_

#include <stdint.h>

namespace sec_proto {

#pragma pack(push, 1)
struct RecvDataFromSDKReq {
    uint16_t channel_id_;       // +0
    uint16_t room_id_;          // +2
    uint32_t time_stamp_;       // +4
    uint16_t to_busi_id_;       // +8
    uint16_t from_busi_id_;     // +10
    uint32_t to_secsvr_seq_;    // +12
    uint16_t data_len_;         // +16
    uint8_t data_[32767];       // +18, sizeof 32785

    int pack(char *buf, unsigned int buf_len,
             unsigned int *used_len, unsigned int cut_version) const;
};
#pragma pack(pop)

}  // namespace sec_proto

#endif  // TSS_SDK_RECON_SDK_RECV_PKG_H_
