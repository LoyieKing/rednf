bool CStomach::IsSaveStomach(CREATURE_STATE state)
{
    // b28d-37 [N85 封顶，ORIG 0x8336dee]：x87 比较两向（ORIG fucompp/fnstsw/sahf/sete
    // vs 4.4 fucomip/fstp/setae；P230 换 -march=i486 可消，本波不改路由）。
    // ORIG 决策树 `cmp $1,%eax; je; cmp $1,%eax; jl; cmp $3,%eax; jg; jmp` 为
    // 形参载入寄存器比较且无槽往返（P251/P286：无槽多路比较 ⇒ switch；if 链会被
    // 4.4 折成内存直比并多出 `int s` 槽），故按 ORIG 还原 switch(state) 形。
    bool saveOk = true;
    switch (state)
    {
    case CREATURE_STATE_1:
        if (m_value <= 0.0f)   // ORIG test $0x5（C0|C2）：<=0 不可保存；NaN 走 unordered 保持可保存
            saveOk = false;
        break;
    case CREATURE_STATE_2:
    case CREATURE_STATE_3:
        if (m_value == 100.0f)
            saveOk = false;
        break;
    default:
        break;
    }
    return saveOk;
