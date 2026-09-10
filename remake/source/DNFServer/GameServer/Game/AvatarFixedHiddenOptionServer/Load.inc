bool AvatarFixedHiddenOptionServer::Load()
{
    cMyTrace trace(__PRETTY_FUNCTION__, 0x1a, 9, true, false);
    trace("\t- Loading Avatar Hidden Option - ");
    // [b28c-16] P84/P102：ORIG 短路结果先物化 bool 局部（loadScript!=0 与
    // Init()==0 各置失败标志 1，再统一 test/je 二次物化返回），4.4 直返
    // `a==0 && b!=0` 折叠丢二次 bool 物化与 xor 取反。
    // [b28c-16 修正] 具名 bool 局部反多 -0x9 槽 + setne 物化（帧 0x38→0x48）。
    // ORIG 实为「失败标志汇聚 eax → 尾部 !r 归一」：`jA L1(mov$1) / Init→xor$1
    // →je L0(mov$0) / L2: test;je L3; mov$0 / L3: mov$1` ⇒ 源形 = `!(A || B)`。
    // [b28d-56 判词·AE] 旧判 N 作废：真因是 Init 返回类型 int（非 bool）致调用点
    // 无 xor 段。ORIG 0x0817e1ae 段为 `call loadScript; test %eax,%eax; jne <fail>；
    // call Init; xor $1,%eax; test %al,%al; je <ok>；fail: mov $1; jmp；ok: mov $0;
    // test %al,%al; je <ret1>; mov $0; ret1: mov $1`（P214① 正条件短路物化）。
    // 探针 /tmp/lane56/load.cpp L1/L4 逐条复现；依赖 Init 返回类型 bool（已改）。
    if (loadScript() != 0 || !Init())
        return false;
    return true;
