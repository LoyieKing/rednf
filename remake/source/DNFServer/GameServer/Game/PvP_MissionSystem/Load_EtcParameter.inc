bool PvP_MissionSystem::Load_EtcParameter()
{
    MissionParameterScript param;
    // b28d-61 [P292/P63，ORIG 0x085e1ffx `call; xor $0x1,%eax; test %al,%al;
    // je +0x38`]：具名 bool ok + return ok 使 4.4 产槽物化 `mov %al,-0x9;
    // cmpb $0,-0x9`；改负条件早返（bool 直判 + 两臂共享 ebx 出口）。
    if (!importPvpMissionParameterScript(param))
    {
        puts("Mission Parameter Error!");
        return false;
    }
    copy_missionParameter(param);
    return true;
