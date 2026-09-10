void CAssaultPlace::Reset()
{
    // ORIG 0x82e6abe/af：函数头 `movl $0,-0xc` 双写 = 外提 i 的 dead init +
    // 第一循环 init；第二循环（0x82e6ba7）复用同槽第三写 ⇒ 共享具名 i。
    int i = 0;
    for (i = 0; i < 8; ++i)
    {
        if (m_assaulters[i].GetUser() != 0)
            m_assaulters[i].GetUser()->SetAssaultPlace(0);
        m_assaulters[i].Reset();
    }
    m_field100 = 0;
    m_field104 = 0;
    m_authenValue = 0;
    m_state = (ENUM_ASSAULT_STATE)0;
    // ORIG 0x82e6b5d：Point2d(0,0) 临时构造（C1Eii sret -0x14/-0x10）后
    // 两字拷入 0x110/0x114 ⇒ 临时对象赋值形。
    m_posBase = Point2d(0, 0);
    m_pos.m_x = 0;
    m_pos.m_y = 0;
    // ORIG 0x82e6ba7：teamFlag 第二循环复用 i 槽。
    for (i = 0; i <= 1; ++i)
        m_teamFlag[i] = 0;
    m_winTeam = (ENUM_ASSAULT_TEAM)0;
    m_checkUser = 0;   // ORIG *(int*)(this+0x134)=0：该槽用户指针清零
    m_abuseSec = 0;
