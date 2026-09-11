int GameWorld::GetChannelUserCount(ENUM_SERVER_GROUP group, stOccStatisticByAge& age)
{
    int count = 0;
    // ORIG 0x86cb627: for-init 内声明迭代器（技巧25，begin sret 直落 -0x20 免默认ctor+拷贝）；
    // 0x86cb642 end sret=-0x1c(先)、0x86cb6d2 ppEi sret=-0x18(后)、ne 调用@0x86cb717
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); it++)
    {
        // ORIG 0x86cb655 user 具名槽 -0x10
        CUser* user = it->second;
        if (user != NULL)
        {
            // b28c-lane3：历史 register bool asm("al") 绑定按 P0/N96 清除（自然形）。
            // ORIG 0x86cb669-0x86cb671 `cmp 0xc(%ebp),%eax; sete %al; test %al,%al; je`。
            if (user->GetServerGroup() == group)
            {
                count++;                       // ORIG 0x86cb673
                unsigned int userAge = user->GetAge();  // ORIG 0x86cb682 槽 -0xc；0x86cb689 `jbe`=unsigned ⇒ 技巧5
                // ORIG 0x86cb685-0x86cb6a2 >99 分支 fall-through 在前（技巧17）
                if (userAge > 99)                        // ORIG 0x86cb685 cmpl $0x63; 0x86cb689 jbe=unsigned 比较
                {
                    age.m_age[99]++;
                }
                else if (userAge == 0)         // ORIG 0x86cb6a5
                {
                    age.m_age[0]++;
                }
                else
                {
                    age.m_age[userAge - 1]++;
                }
            }
        }
    }
    return count;
}