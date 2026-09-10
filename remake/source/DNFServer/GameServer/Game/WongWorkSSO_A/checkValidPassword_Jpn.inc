bool WongWork::CSimpleSSO::checkValidPassword_Jpn(unsigned int mId, char const* a,
                                                  char const* b, stOutputData_t& out)
{
    // [b28d-solo-WongWorkSSO_A] ORIG 0x081b25ec：long 左值直传（非 cast 引用形）。
    long v2 = 0;  // loginTime&  → ORIG -0x1c(%ebp)（movl $0 在前）
    long v1 = 0;  // expireTime& → ORIG -0x20(%ebp)
    getGameStartTime(mId, v2, v1, out);
    // ORIG 判定形：call IsGm 后直接 test %al（bool 返回直测，无 XOR 前缀）
    // + je 跳尾 mov$1 —— if(IsGm){...} 正极性 + 尾部 mov $1;jmp 中继保持
    //（ORIG +0xae..+0xba，中继属在册 N 跨版本 lowering 保留）。
    // [b27 封顶] 残差 = getGameStartTime 实参装载序（ORIG 0x18 槽先 → lea
    // -0x20；4.4 恒 lea 先 → 0x18 后，P75 跨版本实参调度序不可定向）+
    // 尾 mov$1 中继缺失（if 形无中继，early-return 形反生 XOR 前缀——两形
    // 探针均不匹配 ORIG 组合）。语义/调用链/其余指令全等。
    if (GlobalData::s_GM_Manager->IsGm(mId))
    {
        if (strncmp(a, b, 0x84) == 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x2e, 0)(
                "[!]GM User Check Password Sucess=> m_id(%d)", mId);
        }
        // [b28d-D13] ORIG 0x081b263a mov $1,jmp 中继：if 内独立 return true
        // （ORIG 0x081b263c 尾部再一条 mov $1 共享出口）。
        return true;
    }
    return true;
