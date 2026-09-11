// ORIG SendAll @ 0x0834048a：+0x1c=0；G_GameWorld()->send_all(*this)；返回 1。
bool CPacketRespondent::SendAll()
{
    m_autoSend = 0;
    G_GameWorld()->send_all(*this);
    return true;
}