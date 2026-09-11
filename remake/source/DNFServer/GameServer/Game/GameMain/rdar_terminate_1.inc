// ORIG 0x0829e80b（_Z14rdar_terminatev，T，87 条）：
//   buf[30] 清零；sprintf("./pid/%s.pid", G_CEnvironment()->get_file_name())；
//   printf("???%s\n")；remove 失败 → perror 风格 strerror 输出，
//   成功 → puts RDAR TERMINATED；arrange_users；TerminateServers()。
void rdar_terminate()
{
    char buf[30];
    std::memset(buf, 0, sizeof(buf));
    std::sprintf(buf, "./pid/%s.pid", G_CEnvironment()->get_file_name());
    // [P6S4 2026-09-11 §6.5 串审计] ORIG 0x0829e857 `movl $0x8c18209,(%esp)` ⇒ ORIG 串
    // 实际字节 `????%s\n`（**四个** ?，见 ORIG .rodata 0x8c18209 dump）—— 原写
    // `???%s\n`（少一个 ?，运行期可见）已按字节改正。
    std::printf("????%s\n", buf);
    if (std::remove(buf) == -1)
    {
        std::printf("FAIL TO DELETE PID FILE ERROR: %s\n",
                    strerror(*__errno_location()));
    }
    else
    {
        std::puts("RDAR PROCESS IS TERMINATED.");
    }
    G_GameWorld()->arrange_users();
    TerminateServers();
}