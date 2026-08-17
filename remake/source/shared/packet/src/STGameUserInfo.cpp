//
// Created by loyieking on 24-3-24.
//

#include "STGameUserInfo.h"

#include <cstring>
STGameUserInfo::STGameUserInfo() {
    this->server_id = 0;
    this->user_m_id = 0;
    // 来自反编译/DWARF 的推断：m_uid 初始化为 0xffffffff (-1)，不是 0
    this->m_uid = -1;
    this->channel_no = 0;
    this->charac_no = 0;
    this->m_level = 0;
    this->m_job = 0;
    this->m_growType = 0;
    this->m_sex = 0;
    memset(this->m_name, 0, sizeof(this->m_name));
}
