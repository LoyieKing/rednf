SIPGData* CIPGMgr::CIPGWorker::ProcIPGSync(SIPGData* data)
{
    // ORIG 帧 0xc8（仅一次 8 字节出参调用，其余为死局部）：P31 dummy local
    // 撑帧对齐（ORIG 0x08105258，ndiff 残余即 sub $0xc8 vs $0x18）。
    char framePad[0xb0];
    (void)framePad;
    // [N3 2026-09-11 P139] ORIG 发射 `_ZZN8WongWork3IPG7CIPGMgr10CIPGWorker11ProcIPGSyncEPNS0_8SIPGDataEE19__PRETTY_FUNCTION__`
    //   @0x08b43020（size 0x63）但函数体内 **0 引用**（objdump [0x08105258,0x08105275] 搜 0x8b43020 命中 0）
    //   ⇒ 死引用，只补符号不产生指令。
    (void)sizeof(__PRETTY_FUNCTION__);
    return ProcIPGSyncForTaiwan(data);
}
