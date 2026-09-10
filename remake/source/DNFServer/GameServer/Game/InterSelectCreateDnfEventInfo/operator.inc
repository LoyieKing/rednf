void InterSelectCreateDnfEventInfo::Reward::operator()(CUser* user)
{
    PacketGuard guard;
    guard.put_header(0, 0x22d);
    guard.put_short(m_rate);
    // [b28d-56 判词·AE] ORIG 0x8160cb4 段逐条：`call; xor $1,%eax; test %al,%al;
    // je L0`（P248 档① bool callee + 裸 `!`）→ `movzbl 0x2(%eax); test %al,%al;
    // je L0`（m_flag 直测 !=0）→ `mov $1; jmp L1 / L0: mov $0; L1: test %al;
    // je put0; put1; jmp; put0:`（P214① 正条件 `&&` 物化，put1 直落、put0 后置）。
    // 依赖：CUser::GetEventCreateDnfReward char→bool（Main 已代落，P292）。
    // 探针 /tmp/lane56/sc.cpp g1 逐条复现。
    if (!user->GetEventCreateDnfReward() && m_flag != 0)
    {
        guard.put_byte(1);
    }
    else
    {
        guard.put_byte(0);
    }
    guard.finalize(true);
    user->Send(guard);
