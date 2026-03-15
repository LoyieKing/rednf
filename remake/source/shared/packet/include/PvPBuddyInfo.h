//
// Created by loyieking on 24-3-30.
//

#ifndef REDNF_PVPBUDDYINFO_H
#define REDNF_PVPBUDDYINFO_H

#include "ReverseEngineerLib.h"

#pragma pack(push, 1)
class PvPBuddyInfo {
public:
    char server_id;        // offset 0x00
    char channel_no;       // offset 0x01
    bool isOnline;         // offset 0x02
    int charac_no;         // offset 0x03
    char buddy_n_user_id_what[0x1e];  // offset 0x07
    char variable_what1;        // offset 0x25
    char buddy_n_user_what2;        // offset 0x26
    char variable_what2;        // offset 0x27
    char buddy_n_user_what3;        // offset 0x28
};
#pragma pack(pop)

TEST_CLASS_SIZE(PvPBuddyInfo, 0x29);

#endif  // REDNF_PVPBUDDYINFO_H
