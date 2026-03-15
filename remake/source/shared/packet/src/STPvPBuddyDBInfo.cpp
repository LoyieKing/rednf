//
// Created by loyieking on 24-3-24.
//

#include "STPvPBuddyDBInfo.h"

#include <cstring>
STPvPBuddyDBInfo::STPvPBuddyDBInfo() {
    this->server_id = 0;
    this->user_m_id = 0;
    this->charac_no = 0;
    this->variable_what1 = 0;
    this->buddy_n_user_what2 = 0;
    this->variable_what2 = 0;
    this->buddy_n_user_what3 = 0;
    memset(this->buddy_n_user_id_what, 0, sizeof(this->buddy_n_user_id_what));
}
STPvPBuddyDBInfo::~STPvPBuddyDBInfo() {}