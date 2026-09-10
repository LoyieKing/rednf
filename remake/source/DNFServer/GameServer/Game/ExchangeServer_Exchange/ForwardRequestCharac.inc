bool CExchangeServer::ForwardRequestCharac(int a, int b, void* data,
                                           int len)
{
    // P190 撑帧：ORIG(4.1) sub $0x38 vs 4.4 紧凑 $0x28；两个 double 局部（仅取址）
    // 触发 4.4 8B 对齐撑帧 +0x10 精确命中，outgoing %esp 槽位不受影响
    // （探针 /tmp/l11_pad.cpp f1=0x28/f2=0x38）。
    double framePad1, framePad2;
    (void)&framePad1;
    (void)&framePad2;
    return ForwardRequestCharac_forchina(a, b, data, len);
