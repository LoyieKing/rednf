int DisPatcher_MakePVP::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }

// [b28d-29] P30/N4 + N82：ORIG 结构体基址 lea -0x1b + add $0xd 两步 vs 4.4 折 lea -0xe；槽平移 0xd，语义集合全等  (ORIG 0x081ed6c8 insn 152->151)
