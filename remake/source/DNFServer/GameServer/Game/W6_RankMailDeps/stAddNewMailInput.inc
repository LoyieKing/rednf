WongWork::CMailBox::stAddNewMailInput::stAddNewMailInput()
{
    // 具名字段清零（SecuLink 批次裸偏移清理；字段布局/宽度证据见
    // CMailBoxHelper.h stAddNewMailInput 注释——AddNewMail ORIG 0x08551f5a）。
    // ORIG 清零序：+0x00..+0x18 七 dword、+0x1c 一 byte、+0x20/+0x24 两 dword。
    m_mailIndex = 0;
    m_senderCharacNo = 0;
    m_packageMoney = 0;
    m_letterId = 0;
    m_remainSec = 0;
    m_packageType = 0;
    m_period = 0;
    m_unlimit = 0;
    m_senderName = 0;
    m_packageItem = 0;
