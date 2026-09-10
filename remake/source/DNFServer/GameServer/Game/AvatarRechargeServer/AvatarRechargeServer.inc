AvatarRechargeServer::AvatarRechargeServer()
    : m_enduranceReduceRate(0),
      m_bUseEnduranceReduceRate(false)
{
    // [b28c-L9] 旧「构造序跨版本 N」判词作废：ORIG C1 0x08190200 的
    // `mov this; add $0x4,%eax; call ARAD::SCRIPT::AvatarRecharge::C1` 先于 vptr、
    // 且 vptr 在三个 call 之后才写 ⇒ 脚本子对象是**基类**而非成员。
    // 按 Itanium primary-base 规则把 AvatarRecharge 提为第一基类（非动态 ⇒ 不占
    // vptr；ScriptInterface 动态 ⇒ primary base，vptr@+0x00），
    // sizeof 0x5c/全偏移不变（头内 static_assert 自证）⇒ 21=21 AE。
