//
// Created by loyieking on 24-3-24.
//

#include "STPvPBuddyDBInfo.h"

#include <cstring>
STPvPBuddyDBInfo::STPvPBuddyDBInfo() {
    this->server_id = 0;
    this->user_m_id = 0;
    this->charac_no = 0;
    this->m_level = 0;
    this->m_job = 0;
    this->m_growType = 0;
    this->m_sex = 0;
    memset(this->m_name, 0, sizeof(this->m_name));
}
