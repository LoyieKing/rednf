void CMissionList_Charac::ResetMissionList(CUser& user)
{
    // N7 封顶：唯一残差=虚调用 callee 寄存器互换（N7，探针 /tmp/l22p/vprobe*.cpp）。
    _reset();
    MakeMissionList_forNewbies(user);
}