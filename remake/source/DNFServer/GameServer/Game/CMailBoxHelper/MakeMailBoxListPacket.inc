void CMailBoxHelper::MakeMailBoxListPacket(CUser* user, PacketGuard* packet)
{
    // ORIG 0x08555118
    CMailBox* box = (CMailBox*)user->GetMailBox();
    if (box == 0)
        return;

    long nowSec = GlobalData::s_systemTime_.getCurSec();
    packet->put_header(0, 0x61);
    int count = 0;
    int idx = packet->get_index();
    packet->put_byte(box->GetRecvSize());
    packet->put_byte(0);
    // P28/P13：end() 只求值一次（ORIG 循环前单次 end 构造）；P292：
    // FindPackageLoadLack 返回 bool，ORIG `call; test %al; jne <++it>`
    // ⇒ 源码为 continue 形（非 `==0` 取反物化）。
    MailMap::const_iterator it = mail_map(box).begin();
    MailMap::const_iterator endIt = mail_map(box).end();
    for (; it != endIt; ++it)
    {
        if (box->FindPackageLoadLack(it->second->GetLetterId()))
            continue;
        ++count;
        it->second->MakePacket(packet, nowSec, user);
    }
    packet->put_byte(idx, count);
    packet->put_short(box->GetNotLoadedMailCount());
