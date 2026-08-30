#ifndef DNF_GAME_RAND_H_
#define DNF_GAME_RAND_H_
int get_rand_int(int range);
int GetRandomRange(int a, int b);   // ORIG W 0x08539d90 _Z14GetRandomRangeii
int roundFloat(float v);            // ORIG W 0x0853abe5 _Z10roundFloaf（round 后 (int) 截断返回，调用点直接用 eax）
#endif
