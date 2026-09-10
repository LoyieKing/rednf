void STCharacterScript::Clear()
{
    // [round1 判词更新] 旧 N82「帧/槽平移封顶」判词废弃。本波按 ORIG 逐条复刻：
    //  ① 循环变量 i/j 提函数头（判据⑩ 槽序反转律：i 先声明占深槽 -0x10、
    //     j 后声明占浅槽 -0xc，与 ORIG 0x08367abb/0x08367b34 一致）；
    //     `i = 0` 初始化语句 + `for (i = 0; …)` 双清零复现 ORIG 0x08367abb/0x08367ac2
    //     两条 `movl $0x0,-0x10(%ebp)`（P191 配套）。
    //  ② i<=6 循环改连续数组 `m_status[i]`（m_initialStatus +0x24 与
    //     m_growStatus[6] +0x7b 同型连续，stride 0x57）：复现 ORIG 0x08367b40
    //     `imul $0x57; add $0x20; add this; add $4` 统一寻址，删除 i==0 三元分支。
    //  ③ m_attackMotion.clear() 移到 ORIG 真实位置（+0xb44，m_motionBuff 之后、
    //     m_etcMotion 之前）；旧码错放在 i<=6 循环之后。
    //  ④ m_jumpSpeed 留在尾部（ORIG 0x08367e06 段）——保持源码原语句序。
    m_job = static_cast<ENUM_CHARACTERJOB>(0xb);   // ORIG 0x08367ab2 movl $0xb
    m_width = 0;
    m_height = 0;
    int i = 0;
    int j;
    for (i = 0; i <= 5; ++i)
    {
        m_growtypeName[i] = "";
        for (j = 0; j <= 1; ++j)
            m_awakeningName[i][j] = "";
    }
    for (i = 0; i <= 6; ++i)
    {
        m_status[i].Clear();
        m_skillList[i].clear();
        m_etcStats[i].clear();
        m_secondGrow[i].clear();
    }
    m_motionWaiting = "";
    m_motionMove = "";
    m_motionSit = "";
    for (i = 0; i <= 1; ++i)                 // +0xae0（ORIG i<=1 循环）
        m_motionDamage[i] = "";
    m_motionDown = "";
    m_motionOverturn = "";
    m_motionJump = "";
    m_motionJumpattack = "";
    m_motionRest = "";
    for (i = 0; i <= 3; ++i)                 // +0xb04/+0xb14（ORIG i<=3 同循环）
    {
        m_throwMotion1[i] = "";
        m_throwMotion2[i] = "";
    }
    m_motionDash = "";
    m_motionDashattack = "";
    m_motionGetitem = "";
    m_motionBuff = "";
    m_attackMotion.clear();                  // +0xb44（ORIG 0x08367d6b 段）
    m_etcMotion.clear();                     // +0xb50
    m_jumpattackInfo = "";
    m_dashattackInfo = "";
    m_attackInfo.clear();
    m_etcAttackInfo.clear();
    m_weaponWav.clear();
    m_scriptPath = "";
    m_jumpSpeed = 100;
    m_fieldbd4 = 0;
    m_fieldbd8 = 1.0f;
    m_combo.clear();
