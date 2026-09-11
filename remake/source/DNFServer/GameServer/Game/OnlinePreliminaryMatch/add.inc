int TeamInfoSecond::add(CUser* user)
{
    for (int i = 0; i < 4; ++i)
    {
        /* [b28c-L26 翻案·档②] ORIG @0855fe18 为 `movzbl 0xc(%eax,%edx,8),%eax;
         * xor $0x1,%eax; test %al,%al; je` 寄存器 xor 物化。旧判词（b28 早期
         * N96+P168a④）称"需 register 绑定、清绑后无源形"，实测四形（==1/!=1/
         * ^1!=0/裸 !field）恒折 cmp 或缺 xor——前提不成立：按 lane20A 定稿判例
         * （CSharedServerMessageManager::m_isSet uchar→bool ⇒ 裸 `!field` 自然
         * 复现 movzbl;xor$1;test;je，ndiff 3→0），把 TeamSeatInfo::m_flag
         * （OnlinePreliminary.h，非冻结）uchar→bool 后本件 **28=28、diff 仅剩
         * call/jmp 重定位伪差（AE）**；同 TU fresh 联判（P211 陷阱法）：
         * identical 42 不变 / ae 43→45 / near 8 不变 / diff 12→10 ⇒ 净 +2 AE、
         * 零回退（_GetLastUser 的 `^1` 段一并受益）。*/
        if (!m_seats[i].m_flag)
        {
            m_seats[i].m_flag = 1;
            m_seats[i].m_user = user;
            return 1;
        }
    }
    return 0;
}