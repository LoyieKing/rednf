// ORIG 0x084b91e8（Init.md）：挂接 user 指针，清零疲劳计数。
void CHades::Init(CUser* user)
{
    m_user = user;
    m_useFatigueCnt = 0;
}