// ORIG W 0x085578ae (size 0x65)：前 0x28 字节清零（7 dword + byte@0x1c +
// dword@0x20/0x24）。语义映射（AddNewMail.md 装载证据）：+0x00 mailIndex、
// +0x04 senderCharacNo、+0x08 packageMoney/senderNamePtr（CMailBox.cpp 与
// CMailBoxHelper.cpp 两 TU 对该槽/后段视图分歧，见 NOTES 上报）、+0x0c letterId、
// +0x10 remainSec、+0x14 packageType、+0x18 period、+0x1c unlimit(byte)、
// +0x20/+0x24 指针字段。
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
}