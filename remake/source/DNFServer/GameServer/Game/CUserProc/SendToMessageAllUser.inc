void APSystem::CUserProc::SendToMessageAllUser(const char* msg, const char* fmt)
{
    // [b28c-4 P223/BM8] ORIG 0x08122572：两条件各带尾 nop 岛（L1 为 `nop;jmp leave`、
    // L2 为 `nop` fall-through）⇒ 双 goto 尾标签串复现（探针 /tmp/l4/islands.cpp fB）。
    if (msg == 0)
        goto L1;
    if (fmt == 0)
        goto L2;
    {
        char buf[1024];
        sprintf(buf, fmt, msg);
        // [b28c-4] ORIG `movl $str,-0x18`：未使用指针须"声明后赋值"——初始化形被
        // 4.4 折叠为 movl $0（探针 /tmp/l4/lit2.cpp h1）。
        const char* pMsgName;
        // [P6AuditOldN-S1 2026-09-11 修正 · 语义轴串引用] ORIG 0x081225a3
        //   `c7 45 e8 a0 d4 b5 08` = `movl $0x8b5d4a0,-0x18(%ebp)`；objdump -s 实读
        //   0x8b5d4a0 处首字节为 **0x00**（空串），相邻的 "chn_game_server_msg_82"
        //   在 +1（0x8b5d4a1，串池共享存储）⇒ ORIG 引用的是 `""` 而非该邻串。
        //   旧形误引邻串（strall N-only 1 条）⇒ 改回空串，指令形与串集合同时对齐。
        pMsgName = "";
        int msgLen = strlen(buf);
        int a = 0;
        int b = 0;
    }
L1:;
L2:;
}