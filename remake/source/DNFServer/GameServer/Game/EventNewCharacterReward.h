// df_game_r 还原 —— EventNewCharacterReward（新手角色活动奖励，事件实体）。
// 布局镜像 CEventBase（+0x00 vptr / +0x04 stADDINFO / +0x08 m_isEventing），
// 自身字段：+0x0c m_job（活动角色职业）、+0x10 m_field10。
// 仅本批次实现 isEventCharacter(int)（0x0816bcea）；其余方法（ctor/dtor/
// StartEvent/EndEvent/eventCharacterJob/eventExpRatio）由后续批次补全。
#ifndef GAME_EVENTNEWCHARACTERREWARD_H_
#define GAME_EVENTNEWCHARACTERREWARD_H_

class EventNewCharacterReward
{
private:
    // ORIG 布局镜像（CEventBase 基类：+0x00 vptr / +0x04 stADDINFO / +0x08 isEventing）
    char m_vptr[4];            // +0x00
    unsigned short m_addInfo0; // +0x04
    unsigned short m_addInfo1; // +0x06
    char m_isEventing;         // +0x08
    char m_pad09[3];           // +0x09..+0x0b
    int m_job;                 // +0x0c（isEventCharacter 读取；ctor 置 0xffffffff）
    int m_field10;             // +0x10

public:
    // EventNewCharacterReward::isEventCharacter @ 0x0816bcea —— param 是否活动角色职业。
    bool isEventCharacter(int job);
};

#endif  // GAME_EVENTNEWCHARACTERREWARD_H_