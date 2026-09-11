// ---------------------------------------------------------------------------
// DenyPacket  ORIG T 0x859092a (0x104)
// 纯谓词：id 命中封禁集合返回 true。二分比较树结构与 ORIG 一致：
//   true 集合 = {0x05..0x06, 0x1a..0x1b, 0x1d, 0x42, 0x44, 0x53..0x54,
//                0x61, 0x66, 0x69, 0x8a, 0xac, 0xb9..0xc1, 0xcc..0xcd,
//                0xe1, 0xee, 0x100, 0x104, 0x107, 0x10f..0x111,
//                0x120, 0x13c, 0x145}
// ---------------------------------------------------------------------------
// [b28d-solo-GameSecurity] 修法：if-else 二分链 → 稀疏 switch（case 按值升序）。
// ORIG 0x0859092a 每个树节点为 `cmp $K,%eax; je L_true; cmp $K,%eax; jg R`——双 cmp
// 同常量是 GCC switch 决策树签名（P17/P261）；if 链恒多 20 条。
bool DenyPacket(int id)
{
    switch (id)
    {
        case 0x05: case 0x06:
        case 0x1a: case 0x1b:
        case 0x1d:
        case 0x42:
        case 0x44:
        case 0x53: case 0x54:
        case 0x61:
        case 0x66:
        case 0x69:
        case 0x8a:
        case 0xac:
        case 0xb9: case 0xba: case 0xbb: case 0xbc: case 0xbd:
        case 0xbe: case 0xbf: case 0xc0: case 0xc1:
        case 0xcc: case 0xcd:
        case 0xe1:
        case 0xee:
        case 0x100:
        case 0x104:
        case 0x107:
        case 0x10f: case 0x110: case 0x111:
        case 0x120:
        case 0x13c:
        case 0x145:
            return true;
        default:
            return false;
    }
}