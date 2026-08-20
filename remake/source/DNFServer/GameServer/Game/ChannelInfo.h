#ifndef DNF_GAME_CHANNEL_INFO_H_
#define DNF_GAME_CHANNEL_INFO_H_

#include <string>

// channel_script_t::getChannelInfo 返回的记录前缀。
// 字段依据 ORIG 对 +0x00/+0x04/+0x08 的访问建模；后续脚本字段保持不透明。
struct channel_info_view
{
    int m_channelNo;       // +0x00
    int m_channelType;     // +0x04
    std::string m_name;    // +0x08
};

#endif
