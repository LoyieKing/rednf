// ORIG 0x080f49e0。转发到 importEventUpgradeJarScript((EventUpgradeJarInfo*)this, path)。
// 第一个参数 scriptDir 未使用（ORIG 仅用第二个）。
// ORIG 返回 int（call 后 test %al,%al;je;mov$1;jmp;mov$0 的无 xor 物化，P9）：
// 直接 return bool 生 movzbl %al; 需三元 ?1:0 强制分支化转换。
int Event_Upgrade_Jar::LoadScript(const char* /*scriptDir*/, const char* path)
{
    return importEventUpgradeJarScript(this, path) ? 1 : 0;
}