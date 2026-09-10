int DisPatcher_IncreStatus::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }

// [b28d-29] N82/P168a：帧 0x6c vs 0x7c + 取反物化（ORIG test/je 直判 vs 4.4 xor/jne），语义集合全等  (ORIG 0x081ee010 insn 342->357)
