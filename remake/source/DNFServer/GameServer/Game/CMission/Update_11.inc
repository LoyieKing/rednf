// [P8S1 2026-09-11 本波复核] 维持封顶：N168a③ cwtl 跨版本 sext 折叠(ORIG 0x085e3d58 movzwl;cwtl;add)；P139/P339/P341/disp 折叠/掩码 cast
//   诸新钥匙逐条核过，无翻案。证据见 res_1.tsv 同件 note。
int CMission_within::Update(MissionInfo& info,
                            const MissionClearCondition_Parameter& param) const
{
    // [b28c-35] N：ORIG 0x085e3d58 首段 `movzwl 0x4(%eax),%eax; cwtl; add %eax,%eax`
    // （short 提升两段），4.4 恒折 `movzwl; add` 无 cwtl。探针 /tmp/l35_probe_short.cpp
    // f1..f6 六形（(int)/(short)/(unsigned short)/复合/op= 等）全部无 cwtl ⇒ 跨版本
    // zext/sext 折叠，源形不可造。语义等价（仅低 16 位存回，负值结果一致）。
    // [2026-09-10 复核] 改把 (short) cast 放在**乘法内侧**（旧形 cast 在外，4.4 视
    //   `(int)ushort` 为免指令 ⇒ 缺 cwtl）；`(short)info.m_count * 2` 才是 ORIG 的
    //   「读 ushort + 符号扩展 + 乘 2」三段源形。
    //   **实测（第 7 形）仍无 cwtl**（ourN 仍 32 vs ORIG 33）⇒ 4.4 对 `(short)ushort`
    //   恒折 movzwl 直用（前端已知低 16 位语义），与 P168a③ 同族 ⇒ N 封顶；语义等价
    //   （仅低 16 位存回，(int)*2 与 (short)*2 低 16 位一致）。
    info.m_count = (short)((short)info.m_count * 2);
    info.m_count = (short)(info.m_count +
                           ((param.m_victory & 1) ? 1 : 0));
    ++info.m_streak;
    return 1;
}