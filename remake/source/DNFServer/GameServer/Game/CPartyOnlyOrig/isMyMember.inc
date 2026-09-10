int CParty::isMyMember(unsigned int userNo)
{
    for (int i = 0; i <= 3; i++)
    {
        // [判据②岛形+拆&&] ORIG O@25 `sete%al;test;je`。
        if (_checkValidUser(i))
        {
            // b28d-45 P0/N96 清绑：`register bool hit asm("al")` 删除，裸形
            // （P158 bool 局部形实测反劣 46→45 AE，已回退；ORIG O@25
            // sete%al;test;je 岛形在 4.4 无绑定下不可复现，按 N96/N18a 封顶）。
            if ((static_cast<unsigned int>(
                     m_member[i].m_pUser->get_unique_id()) & 0xffff) == userNo)
            {
                return 1;
            }
        }
    }
    return 0;
