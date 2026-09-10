void StageControl::onClearReward(StageEndType::T endType)
{
    if (endType < 0)
        goto island1;
    if (endType <= 2)
        goto island2;
    // [b28d-sweep] P286 单 case switch 实测：得 `mov 0xc(%ebp),%eax; cmp $0x3,%eax`
    //   但整函数 27→28 条（第二次比较前仍重载形参，ORIG 26 条为单次装载三连比较）
    //   ⇒ 反向回退，维持 P85「4.1 跨语句 %eax 保活不可复现」封顶。
    if (endType != 3)
        goto island3;
    // ORIG 0x0813060e：this+4 直访问 m_procStage 槽（无 getProcStage call）、
    // mov (%eax) 直读 m_index（无 getIndex call）、第二参透传 endType（0x81330629）。
    reinterpret_cast<ProcStage*>(m_procStage)->setState(ProcStage::State::S_N6);
    Timer_StageControl::RegistTimerEPLP(m_index, endType);
island1:
    ;
island2:
    ;
island3:
    ;
