int Manager::pause(CUser* user, bool flag)
{
    if (user == 0)
    {
        return 0;
    }
    StageControl* control = user->getAdvanceAltar();
    if (control != 0)   // ORIG control 分支 je 前跳尾部 body（4.4 !=0 包裹复现）
    {
        // ORIG 无 procStage 槽无 flag 规范化（P0）：getProcStage 结果经 (%esp)
        // 出参槽直传，flag 压参单条 movzbl（bool→bool 无 test/setne）。
        return control->getProcStage()->pause(flag);
    }
    return 0;
