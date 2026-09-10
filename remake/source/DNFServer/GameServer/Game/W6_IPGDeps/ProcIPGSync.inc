SIPGData* CIPGMgr::CIPGWorker::ProcIPGSync(SIPGData* data)
{
    // ORIG 帧 0xc8（仅一次 8 字节出参调用，其余为死局部）：P31 dummy local
    // 撑帧对齐（ORIG 0x08105258，ndiff 残余即 sub $0xc8 vs $0x18）。
    char framePad[0xb0];
    (void)framePad;
    return ProcIPGSyncForTaiwan(data);
