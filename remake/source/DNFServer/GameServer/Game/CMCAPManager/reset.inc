// ORIG W 0x8694fa0：+0x00=1、+0x04=1000、+0x08=+0x00（当前倍率拷贝）、
// +0x0c=1、+0x10=0、+0x14=0（逐 dword/byte 直写，无 memset）。
void CMCAPManager::reset()
{
    m_point = 1;
    m_rate = 0x3e8;
    m_field08 = static_cast<int>(m_point);
    m_field0c = 1;
    m_field10 = 0;
    m_saveCleanPadPoint = false;
}