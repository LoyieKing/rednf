void CGlobalEffectManager::checkTimeout(unsigned int type, unsigned int curSec)
{
    PacketGuard packet;
    packet.put_header( 0, 0xb5);
    packet.put_byte( 0);
    // put_int 占位计数；随后用 put_int(index,&count) 回填。
    int countIndex = packet.get_index();
    // [判据⑩/顺序] ORIG 0x084b7ae3/0x084b7aea/0x084b7aee 顺序 = count 清零 →
    //   bool 预清 → put_int(0) ⇒ 两局部声明须在 put_int(0) 之前。
    int count = 0;
    // [判据⑫ clr_b orig=1 our=0] ORIG 0x084b7ae3/0x084b7aea 为
    //   `movl $0x0,-0x10` + `movb $0x0,-0x9`（count 之后紧邻的具名 bool 预清，
    //   全函数无读取点 = 原码遗留死变量）⇒ 补回以复现该条指令。
    bool reserved = false;
    (void)reserved;
    packet.put_int( 0);

    for (std::vector<CGlobalEffect>::iterator it = m_activeEffects.begin();
         it != m_activeEffects.end(); )
    {
        if (it->getType() == type && it->isTimeout(curSec))
        {
            it->makeNotifyInfo(packet);
            ++count;
            it = m_activeEffects.erase(it);
        }
        else
        {
            ++it;
        }
    }

    packet.put_int( countIndex, count);
    packet.finalize( true);
    G_GameWorld()->send_all(packet);
