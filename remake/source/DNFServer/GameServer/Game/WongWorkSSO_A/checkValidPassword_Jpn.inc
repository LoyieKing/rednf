// ---- checkValidPassword_Jpn(...)  ORIG 0x081b25ec（键名位 · P327：定义前移）
// [Q8 2026-09-11 旧N判词重扫] 旧判=NEAR → n1_v 新口径 AE（gap=+0，blocks=0）⇒ 旧判词作废。
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
    // [M1S5 2026-09-11 本波复核] report 文本口径 DIFF 的残余**全部是 .o 未重定位
    //   显示形**：`call 4b <+0x37>` ↔ ORIG `call 86031f6 <getGameStartTimeEjRlS1_…>`
    //   、`mov 0x0,%eax` ↔ ORIG `mov 0x941f75c,%eax`（GlobalData::s_GM_Manager）、
    //   `movl $0x3a0/$0x0,0x4(%esp)` ↔ ORIG `$0x8bb6780/$0x8bb5b40`（两串经
    //   .o/.rodata 逐字节核对 = `bool WongWork::CSimpleSSO::checkValidPassword_Jpn
    //   (unsigned int, const char*, const char*, WongWork::CSimpleSSO::stOutputData_t&)`
    //   与 `[!]GM User Check Password Sucess=> m_id(%d)`，全等）。TU 口径不在
    //   非 identical 列表（identical/AE 已闭合）⇒ 零改动。
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
}