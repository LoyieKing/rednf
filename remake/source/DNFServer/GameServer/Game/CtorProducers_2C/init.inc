void CMCAPManager::init()
{
    // ORIG 0x080f8aba 逐指令转写。两次 memset(pointPerHackType_,0,0x3cf0) 忠实保留：
    // 中间 23 个具名下标写在 ORIG 里即为第二次 memset 前的死存储，语义照录。
    memset(pointPerHackType_, 0, sizeof(pointPerHackType_));
    pointPerHackType_[204] = 500;    // +0x330
    pointPerHackType_[111] = 250;    // +0x1bc
    pointPerHackType_[109] = 250;    // +0x1b4
    pointPerHackType_[954] = 3;      // +0xee8
    pointPerHackType_[503] = 1000;   // +0x7dc
    pointPerHackType_[403] = 20;     // +0x64c
    pointPerHackType_[806] = 200;    // +0xc98
    pointPerHackType_[207] = 250;    // +0x33c
    pointPerHackType_[301] = 250;    // +0x4b4
    pointPerHackType_[502] = 250;    // +0x7d8
    pointPerHackType_[302] = 500;    // +0x4b8
    pointPerHackType_[303] = 1000;   // +0x4bc
    pointPerHackType_[404] = 20;     // +0x650
    pointPerHackType_[103] = 3;      // +0x19c
    pointPerHackType_[104] = 3;      // +0x1a0
    pointPerHackType_[105] = 3;      // +0x1a4
    pointPerHackType_[101] = 3;      // +0x194
    pointPerHackType_[102] = 3;      // +0x198
    pointPerHackType_[201] = 500;    // +0x324
    pointPerHackType_[505] = 0;      // +0x7e4
    pointPerHackType_[306] = 0;      // +0x4c8
    pointPerHackType_[808] = 10000;  // +0xca0
    pointPerHackType_[112] = 500;    // +0x1c0
    memset(pointPerHackType_, 0, sizeof(pointPerHackType_));
}