// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081de72c orig/ours=197/173 ndiff-blocks=17
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,-0xc(%ebp) | mov    -0xc(%ebp),%eax | mov    -0xc(%ebp),%eax
// [SweepDispatchers3 2026-09-09] 修真缺口：两循环改 ORIG 迭代器形（begin 一次 + 每轮
//   end() + it->first/second；m_items 体 4 组「*it 后 ++it」）+ size()!=0 前置物化
//   （setne/test/je）。gap 23→0（196=196），DIFF→AE（P175/P167 族，报告 send.md 已读）。
void Dispatcher_Exchange_Random_Item_Reward::send(CUser* user, ParamBase& param)
{
    stExchangeRandomItemReward& out = static_cast<stExchangeRandomItemReward&>(param);
    PacketGuard packet;
    packet.put_header(1, 0x1c1);
    if (out.m_4 == 0)
    {
        packet.put_byte(1);
        packet.put_byte((unsigned int)out.m_rewards.size());
        // ORIG 0x81de7a7：先 size()!=0 物化（setne/test/je），再迭代器遍历
        // （begin 一次、每轮 end() 重取、it->first/second）。
        if (out.m_rewards.size() != 0)
        {
            for (std::vector<std::pair<int, int> >::iterator it = out.m_rewards.begin();
                 it != out.m_rewards.end(); ++it)
            {
                packet.put_int(it->first);
                packet.put_int(it->second);
            }
        }
        packet.put_byte((unsigned int)(unsigned char)out.m_9);
        if (out.m_9 != 0)
        {
            // ORIG：迭代器循环体 4 组「*it 后 ++it」（short/int/int/short），无边界保护。
            for (std::vector<int>::iterator it = out.m_items.begin();
                 it != out.m_items.end(); )
            {
                packet.put_short(*it);
                ++it;
                packet.put_int(*it);
                ++it;
                packet.put_int(*it);
                ++it;
                packet.put_short(*it);
                ++it;
            }
        }
    }
    else
    {
        packet.put_byte(0);
        packet.put_byte(out.m_4);
    }
    packet.finalize(true);
    user->Send(packet);
}