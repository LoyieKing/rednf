// [P8S5 2026-09-11 负结论·N82 4.1 寻址分解族] ORIG 0x846ee93/96/9c：
//   `mov -0xc(%ebp),%edx; add $0x14aa,%edx; mov 0x1c(%eax,%edx,8),%eax`；
//   我方 = `mov 0xa56c(%eax,%edx,8),%eax`（0x14aa*8+0x1c = 0xa56c，同址）。
//   探针实测（/tmp/r5/p8s5/disp.cpp：struct{char pad[0xA550]; unsigned long x[32][2];}
//   + `o[i]=s->x[i][1]`）4.4 恒产 disp 形 `mov 0xa554(%edx,%ecx,8)` ⇒ 源码不可造，回滚态保留。
//   本波未改（gap +1 唯一残差）
void CGuildServerProxy::SendGuildExpLimit()
