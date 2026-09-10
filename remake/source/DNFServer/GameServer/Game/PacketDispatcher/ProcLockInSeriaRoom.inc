int CLocalChinaErrorDispatcher::ProcLockInSeriaRoom(CUser& /*user*/, ENUM_CMDPACKET /*cmd*/) const { return 0; }

// ORIG 0x812af6a 逐指令转写：find 命中后按 b0..b3 执行，非 0 结果短路返回。
