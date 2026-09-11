// ORIG advancealtar::ProcStage::pause(bool) @ 0x812eaa6：
//   ProcStage+0x04 为 stage state；flag=true 时 state 3→2，false 时 2/1→3。
// ProcStage 位于 StageControl+0x04。
int procStagePause(advancealtar::StageControl* stage, bool flag)
{
    if (stage == 0)
    {
        return 0;
    }
    advancealtar::ProcStage* ps = stage->getProcStage();
    int state = ps->getState();
    if (flag)
    {
        if (state != 3)
        {
            return 0;
        }
        ps->setState(static_cast<advancealtar::ProcStage::State::T>(2));
    }
    else
    {
        if (state != 2 && state != 1)
        {
            return 0;
        }
        ps->setState(static_cast<advancealtar::ProcStage::State::T>(3));
    }
    return 1;
}