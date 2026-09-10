void SDCActiveStatusSyncCommon::makeEtcParameters(
    ENUM_ACTIVESTATUS status, int level, void* param, std::vector<int>* out,
    int* outPtr)
// 0x0808d4b4：先回填 outPtr，再转发三参版本（param 作 DataSend 视图）。
{
    // ORIG 0x0808d4b4 entry：out 形参死 spill 命名槽 -0xc（P0：mov 0x18,%eax;
    // mov %eax,-0xc 先于 outPtr 检查）。
    std::vector<int>* pOut = out;
    if (outPtr != 0)
    {
        *outPtr = getDuration(status, level, param);
    }
    // ORIG 0x0808d4b4 无条件转发三参版本（无 out != 0 检查）。
    makeEtcParameters(status, level, *pOut);
