// ORIG T 0x0859d656（CheckMemberArea.md）：以首个有效成员区域为基准逐一比较。
// [Q5BigTURest.DisPatcherCore 2026-09-11 返回类型轴] 与 CParty.h:387（bool）同步：
// ORIG 定义 0x0859d656 为 `setne %al` + `mov $0x1/%0x0,%eax`（0/1 语义），调用点
// 0x081ca16f `xor $0x1,%eax; test %al,%al; je` 直证 bool；返回类型不入 mangled ⇒ 零 ABI。
bool CParty::CheckMemberArea()
{
    int area = -1;
    for (int i = 0; i <= 3; i++)
    {
        if (!_checkValidUser(i))
        {
            continue;
        }
        if (area == -1)
        {
            area = m_member[i].m_pUser->get_area(false);
        }
        else if (area != m_member[i].m_pUser->get_area(false))
        {
            return 0;
        }
    }
    return 1;
}