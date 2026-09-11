// ORIG 0x081de012（报告 Dispatcher_Register_Quick_Party/process.md）：
// 以 MSG+0xe(short)/+0x10(byte) 构造 STQuickPartyPoolMap_Key（默认 0/0xff 后覆盖，
// 与 ORIG 0x082344ca 一致），match_proper_party(key, party, user) 写 param+0x4。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081de012 orig/ours=57/58 ndiff-blocks=14
//   帧差 sub $24 vs $38 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %ebx | sub    $0x24,%esp | mov    %eax,-0x14(%ebp)
int Dispatcher_Register_Quick_Party::process(CUser* user, MSG_BASE& msg,
                                             ParamBase& param)
{
    // ORIG 槽序：err(-0x14) → msg 引用(-0x10) → param 引用(-0xc) → key 对象(-0x18 起)；
    //   原先把 pref/mref 声明在 err 之前，两个引用各上移一格并把帧撑到 0x38（ORIG 0x24）。
    int err = check_error(user, msg, param);
    if (err > 0)
    {
        return err;
    }
    if (err < 0)
    {
        return -1;
    }
    MSG_QUICK_PARTY& mref = reinterpret_cast<MSG_QUICK_PARTY&>(msg);  // 裸偏移→具名字段（同偏移同宽度）
    stQuickPartyParam& pref = static_cast<stQuickPartyParam&>(param);  // 裸偏移→具名字段（同偏移同宽度）
    QuickParty::STQuickPartyPoolMap_Key key;
    key.m_dungeonIndex = mref.m_0e;
    key.m_dungeonDiff = mref.m_10;
    // ORIG +0x6e：GetParty→ebx 跨 receiver 链保活（P52 单语句链式；无 party/gm/mgr 具名槽）
    // [N7D·221↔279 联判负结论] ORIG 0x081de0b4 `mov -0xc(%ebp),%edx; mov %eax,0x4(%edx)`
    //   （直存）vs 我方中转（+1 条）：真因为 ORIG 该调用点看到的 match_proper_party 声明为
    //   int（与定义 TU 的 ENUM_ERROR 不一致，跨 TU/ODR）⇒ 单 TU 保 prett 保真不可复现。
    //   (int) cast 经探针实测（probe_enum_cast.cc）不改变展开形态（nocast 与 cast 逐指令相同）。
    pref.m_4 = G_CGameManager()->GetQuickPartySystemManager()->match_proper_party(
        key, (CParty*)user->GetParty(), user);
    return 0;
}