/* gen_des4.py 从 ORIG 反汇编逐指令转写（SemA04 2026-09-12）；禁止手改 */
#ifndef DES_SHIFT_DEFINED
#define DES_SHIFT_DEFINED
static unsigned int shift_shr(unsigned int v, unsigned int n){ n&=31; return n?(v>>n):v; }
static unsigned int shift_shl(unsigned int v, unsigned int n){ n&=31; return n?(v<<n):v; }
static unsigned int shift_sar(unsigned int v, unsigned int n){ n&=31; return n?(unsigned int)((int)v>>n):v; }
static unsigned int shift_ror(unsigned int v, unsigned int n){ n&=31; return n?((v>>n)|(v<<(32-n))):v; }
static unsigned int shift_rol(unsigned int v, unsigned int n){ n&=31; return n?((v<<n)|(v>>(32-n))):v; }
#endif
void DES_Encrypt(void* rkArea, unsigned char* block)
{
    void* a0 = (void*)rkArea;
    void* a1 = (void*)block;
    void* a2 = 0; void* a3 = 0;
    (void)a3;
    unsigned int r_eax, r_ebx, r_ecx, r_edx, r_esi, r_edi, r_ebp, r_esp;
    (void)r_ebp; (void)r_esp;
    unsigned int s_m0x10 = 0;
    unsigned int s_m0x14 = 0;
    unsigned int s_m0x18 = 0;
    unsigned int s_m0x1c = 0;
    unsigned int s_m0x4 = 0;
    unsigned int s_m0x8 = 0;
    unsigned int s_m0xc = 0;

  /* @80ac0ba: push %ebp */
  /* @80ac0bb: mov %esp,%ebp */
    r_ebp = r_esp;
  /* @80ac0bd: sub $0x20,%esp */
    r_esp = (r_esp - 0x20u);
  /* @80ac0c0: mov 0x8(%ebp),%eax */
    r_eax = (unsigned int)(size_t)a0;
  /* @80ac0c3: mov %eax,-0xc(%ebp) */
    s_m0xc = r_eax;
  /* @80ac0c6: mov 0xc(%ebp),%eax */
    r_eax = (unsigned int)(size_t)a1;
  /* @80ac0c9: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ac0cb: ror $0x18,%eax */
    r_eax = (r_eax >> 24) | (r_eax << 8);
  /* @80ac0ce: mov %eax,%edx */
    r_edx = r_eax;
  /* @80ac0d0: and $0xff00ff,%edx */
    r_edx = (r_edx & 0xff00ffu);
  /* @80ac0d6: mov 0xc(%ebp),%eax */
    r_eax = (unsigned int)(size_t)a1;
  /* @80ac0d9: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ac0db: ror $0x8,%eax */
    r_eax = (r_eax >> 8) | (r_eax << 24);
  /* @80ac0de: and $0xff00ff00,%eax */
    r_eax = (r_eax & 0xff00ff00u);
  /* @80ac0e3: or %edx,%eax */
    r_eax = (r_eax | r_edx);
  /* @80ac0e5: mov %eax,-0x18(%ebp) */
    s_m0x18 = r_eax;
  /* @80ac0e8: mov 0xc(%ebp),%eax */
    r_eax = (unsigned int)(size_t)a1;
  /* @80ac0eb: add $0x4,%eax */
    r_eax = (r_eax + 0x4u);
  /* @80ac0ee: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ac0f0: ror $0x18,%eax */
    r_eax = (r_eax >> 24) | (r_eax << 8);
  /* @80ac0f3: mov %eax,%edx */
    r_edx = r_eax;
  /* @80ac0f5: and $0xff00ff,%edx */
    r_edx = (r_edx & 0xff00ffu);
  /* @80ac0fb: mov 0xc(%ebp),%eax */
    r_eax = (unsigned int)(size_t)a1;
  /* @80ac0fe: add $0x4,%eax */
    r_eax = (r_eax + 0x4u);
  /* @80ac101: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ac103: ror $0x8,%eax */
    r_eax = (r_eax >> 8) | (r_eax << 24);
  /* @80ac106: and $0xff00ff00,%eax */
    r_eax = (r_eax & 0xff00ff00u);
  /* @80ac10b: or %edx,%eax */
    r_eax = (r_eax | r_edx);
  /* @80ac10d: mov %eax,-0x1c(%ebp) */
    s_m0x1c = r_eax;
  /* @80ac110: mov -0x1c(%ebp),%eax */
    r_eax = s_m0x1c;
  /* @80ac113: shr $0x4,%eax */
    r_eax >>= 4;
  /* @80ac116: xor -0x18(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x18);
  /* @80ac119: and $0xf0f0f0f,%eax */
    r_eax = (r_eax & 0xf0f0f0fu);
  /* @80ac11e: mov %eax,-0x8(%ebp) */
    s_m0x8 = r_eax;
  /* @80ac121: mov -0x8(%ebp),%eax */
    r_eax = s_m0x8;
  /* @80ac124: xor %eax,-0x18(%ebp) */
    s_m0x18 = (s_m0x18 ^ r_eax);
  /* @80ac127: mov -0x8(%ebp),%eax */
    r_eax = s_m0x8;
  /* @80ac12a: shl $0x4,%eax */
    r_eax <<= 4;
  /* @80ac12d: xor %eax,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c ^ r_eax);
  /* @80ac130: mov -0x18(%ebp),%eax */
    r_eax = s_m0x18;
  /* @80ac133: shr $0x10,%eax */
    r_eax >>= 16;
  /* @80ac136: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80ac139: and $0xffff,%eax */
    r_eax = (r_eax & 0xffffu);
  /* @80ac13e: mov %eax,-0x8(%ebp) */
    s_m0x8 = r_eax;
  /* @80ac141: mov -0x8(%ebp),%eax */
    r_eax = s_m0x8;
  /* @80ac144: xor %eax,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c ^ r_eax);
  /* @80ac147: mov -0x8(%ebp),%eax */
    r_eax = s_m0x8;
  /* @80ac14a: shl $0x10,%eax */
    r_eax <<= 16;
  /* @80ac14d: xor %eax,-0x18(%ebp) */
    s_m0x18 = (s_m0x18 ^ r_eax);
  /* @80ac150: mov -0x1c(%ebp),%eax */
    r_eax = s_m0x1c;
  /* @80ac153: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ac156: xor -0x18(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x18);
  /* @80ac159: and $0x33333333,%eax */
    r_eax = (r_eax & 0x33333333u);
  /* @80ac15e: mov %eax,-0x8(%ebp) */
    s_m0x8 = r_eax;
  /* @80ac161: mov -0x8(%ebp),%eax */
    r_eax = s_m0x8;
  /* @80ac164: xor %eax,-0x18(%ebp) */
    s_m0x18 = (s_m0x18 ^ r_eax);
  /* @80ac167: mov -0x8(%ebp),%eax */
    r_eax = s_m0x8;
  /* @80ac16a: shl $0x2,%eax */
    r_eax <<= 2;
  /* @80ac16d: xor %eax,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c ^ r_eax);
  /* @80ac170: mov -0x18(%ebp),%eax */
    r_eax = s_m0x18;
  /* @80ac173: shr $0x8,%eax */
    r_eax >>= 8;
  /* @80ac176: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80ac179: and $0xff00ff,%eax */
    r_eax = (r_eax & 0xff00ffu);
  /* @80ac17e: mov %eax,-0x8(%ebp) */
    s_m0x8 = r_eax;
  /* @80ac181: mov -0x8(%ebp),%eax */
    r_eax = s_m0x8;
  /* @80ac184: xor %eax,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c ^ r_eax);
  /* @80ac187: mov -0x8(%ebp),%eax */
    r_eax = s_m0x8;
  /* @80ac18a: shl $0x8,%eax */
    r_eax <<= 8;
  /* @80ac18d: xor %eax,-0x18(%ebp) */
    s_m0x18 = (s_m0x18 ^ r_eax);
  /* @80ac190: mov -0x1c(%ebp),%eax */
    r_eax = s_m0x1c;
  /* @80ac193: shr %eax */
    r_eax >>= 1;
  /* @80ac195: xor -0x18(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x18);
  /* @80ac198: and $0x55555555,%eax */
    r_eax = (r_eax & 0x55555555u);
  /* @80ac19d: mov %eax,-0x8(%ebp) */
    s_m0x8 = r_eax;
  /* @80ac1a0: mov -0x8(%ebp),%eax */
    r_eax = s_m0x8;
  /* @80ac1a3: xor %eax,-0x18(%ebp) */
    s_m0x18 = (s_m0x18 ^ r_eax);
  /* @80ac1a6: mov -0x8(%ebp),%eax */
    r_eax = s_m0x8;
  /* @80ac1a9: add %eax,%eax */
    r_eax = (r_eax + r_eax);
  /* @80ac1ab: xor %eax,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c ^ r_eax);
  /* @80ac1ae: rorl $0x1d,-0x18(%ebp) */
    s_m0x18 = (s_m0x18 >> 29) | (s_m0x18 << 3);
  /* @80ac1b2: rorl $0x1d,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c >> 29) | (s_m0x1c << 3);
  /* @80ac1b6: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ac1b9: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ac1bb: xor -0x18(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x18);
  /* @80ac1be: mov %eax,-0x10(%ebp) */
    s_m0x10 = r_eax;
  /* @80ac1c1: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ac1c4: add $0x4,%eax */
    r_eax = (r_eax + 0x4u);
  /* @80ac1c7: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ac1c9: xor -0x18(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x18);
  /* @80ac1cc: mov %eax,-0x14(%ebp) */
    s_m0x14 = r_eax;
  /* @80ac1cf: rorl $0x4,-0x14(%ebp) */
    s_m0x14 = (s_m0x14 >> 4) | (s_m0x14 << 28);
  /* @80ac1d3: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac1d6: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ac1d9: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac1dc: mov 0x8b19e20(,%eax,4),%edx */
    r_edx = k_desSPt[(int)r_eax + 0];
  /* @80ac1e3: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac1e6: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ac1e9: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac1ec: sub $0xffffff80,%eax */
    r_eax = (r_eax - 0xffffff80u);
  /* @80ac1ef: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac1f6: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac1f8: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac1fb: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ac1fe: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac201: add $0x100,%eax */
    r_eax = (r_eax + 0x100u);
  /* @80ac206: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac20d: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac20f: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac212: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ac215: add $0x180,%eax */
    r_eax = (r_eax + 0x180u);
  /* @80ac21a: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac221: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac223: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ac226: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ac229: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac22c: add $0x40,%eax */
    r_eax = (r_eax + 0x40u);
  /* @80ac22f: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac236: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac238: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ac23b: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ac23e: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac241: add $0xc0,%eax */
    r_eax = (r_eax + 0xc0u);
  /* @80ac246: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac24d: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac24f: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ac252: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ac255: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac258: add $0x140,%eax */
    r_eax = (r_eax + 0x140u);
  /* @80ac25d: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac264: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac266: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ac269: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ac26c: add $0x1c0,%eax */
    r_eax = (r_eax + 0x1c0u);
  /* @80ac271: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac278: xor %edx,%eax */
    r_eax = (r_eax ^ r_edx);
  /* @80ac27a: xor %eax,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c ^ r_eax);
  /* @80ac27d: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ac280: add $0x8,%eax */
    r_eax = (r_eax + 0x8u);
  /* @80ac283: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ac285: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80ac288: mov %eax,-0x10(%ebp) */
    s_m0x10 = r_eax;
  /* @80ac28b: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ac28e: add $0xc,%eax */
    r_eax = (r_eax + 0xcu);
  /* @80ac291: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ac293: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80ac296: mov %eax,-0x14(%ebp) */
    s_m0x14 = r_eax;
  /* @80ac299: rorl $0x4,-0x14(%ebp) */
    s_m0x14 = (s_m0x14 >> 4) | (s_m0x14 << 28);
  /* @80ac29d: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac2a0: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ac2a3: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac2a6: mov 0x8b19e20(,%eax,4),%edx */
    r_edx = k_desSPt[(int)r_eax + 0];
  /* @80ac2ad: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac2b0: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ac2b3: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac2b6: sub $0xffffff80,%eax */
    r_eax = (r_eax - 0xffffff80u);
  /* @80ac2b9: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac2c0: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac2c2: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac2c5: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ac2c8: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac2cb: add $0x100,%eax */
    r_eax = (r_eax + 0x100u);
  /* @80ac2d0: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac2d7: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac2d9: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac2dc: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ac2df: add $0x180,%eax */
    r_eax = (r_eax + 0x180u);
  /* @80ac2e4: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac2eb: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac2ed: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ac2f0: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ac2f3: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac2f6: add $0x40,%eax */
    r_eax = (r_eax + 0x40u);
  /* @80ac2f9: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac300: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac302: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ac305: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ac308: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac30b: add $0xc0,%eax */
    r_eax = (r_eax + 0xc0u);
  /* @80ac310: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac317: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac319: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ac31c: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ac31f: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac322: add $0x140,%eax */
    r_eax = (r_eax + 0x140u);
  /* @80ac327: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac32e: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac330: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ac333: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ac336: add $0x1c0,%eax */
    r_eax = (r_eax + 0x1c0u);
  /* @80ac33b: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac342: xor %edx,%eax */
    r_eax = (r_eax ^ r_edx);
  /* @80ac344: xor %eax,-0x18(%ebp) */
    s_m0x18 = (s_m0x18 ^ r_eax);
  /* @80ac347: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ac34a: add $0x10,%eax */
    r_eax = (r_eax + 0x10u);
  /* @80ac34d: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ac34f: xor -0x18(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x18);
  /* @80ac352: mov %eax,-0x10(%ebp) */
    s_m0x10 = r_eax;
  /* @80ac355: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ac358: add $0x14,%eax */
    r_eax = (r_eax + 0x14u);
  /* @80ac35b: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ac35d: xor -0x18(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x18);
  /* @80ac360: mov %eax,-0x14(%ebp) */
    s_m0x14 = r_eax;
  /* @80ac363: rorl $0x4,-0x14(%ebp) */
    s_m0x14 = (s_m0x14 >> 4) | (s_m0x14 << 28);
  /* @80ac367: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac36a: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ac36d: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac370: mov 0x8b19e20(,%eax,4),%edx */
    r_edx = k_desSPt[(int)r_eax + 0];
  /* @80ac377: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac37a: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ac37d: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac380: sub $0xffffff80,%eax */
    r_eax = (r_eax - 0xffffff80u);
  /* @80ac383: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac38a: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac38c: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac38f: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ac392: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac395: add $0x100,%eax */
    r_eax = (r_eax + 0x100u);
  /* @80ac39a: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac3a1: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac3a3: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac3a6: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ac3a9: add $0x180,%eax */
    r_eax = (r_eax + 0x180u);
  /* @80ac3ae: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac3b5: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac3b7: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ac3ba: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ac3bd: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac3c0: add $0x40,%eax */
    r_eax = (r_eax + 0x40u);
  /* @80ac3c3: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac3ca: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac3cc: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ac3cf: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ac3d2: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac3d5: add $0xc0,%eax */
    r_eax = (r_eax + 0xc0u);
  /* @80ac3da: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac3e1: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac3e3: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ac3e6: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ac3e9: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac3ec: add $0x140,%eax */
    r_eax = (r_eax + 0x140u);
  /* @80ac3f1: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac3f8: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac3fa: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ac3fd: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ac400: add $0x1c0,%eax */
    r_eax = (r_eax + 0x1c0u);
  /* @80ac405: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac40c: xor %edx,%eax */
    r_eax = (r_eax ^ r_edx);
  /* @80ac40e: xor %eax,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c ^ r_eax);
  /* @80ac411: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ac414: add $0x18,%eax */
    r_eax = (r_eax + 0x18u);
  /* @80ac417: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ac419: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80ac41c: mov %eax,-0x10(%ebp) */
    s_m0x10 = r_eax;
  /* @80ac41f: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ac422: add $0x1c,%eax */
    r_eax = (r_eax + 0x1cu);
  /* @80ac425: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ac427: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80ac42a: mov %eax,-0x14(%ebp) */
    s_m0x14 = r_eax;
  /* @80ac42d: rorl $0x4,-0x14(%ebp) */
    s_m0x14 = (s_m0x14 >> 4) | (s_m0x14 << 28);
  /* @80ac431: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac434: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ac437: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac43a: mov 0x8b19e20(,%eax,4),%edx */
    r_edx = k_desSPt[(int)r_eax + 0];
  /* @80ac441: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac444: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ac447: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac44a: sub $0xffffff80,%eax */
    r_eax = (r_eax - 0xffffff80u);
  /* @80ac44d: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac454: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac456: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac459: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ac45c: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac45f: add $0x100,%eax */
    r_eax = (r_eax + 0x100u);
  /* @80ac464: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac46b: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac46d: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac470: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ac473: add $0x180,%eax */
    r_eax = (r_eax + 0x180u);
  /* @80ac478: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac47f: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac481: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ac484: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ac487: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac48a: add $0x40,%eax */
    r_eax = (r_eax + 0x40u);
  /* @80ac48d: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac494: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac496: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ac499: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ac49c: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac49f: add $0xc0,%eax */
    r_eax = (r_eax + 0xc0u);
  /* @80ac4a4: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac4ab: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac4ad: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ac4b0: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ac4b3: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac4b6: add $0x140,%eax */
    r_eax = (r_eax + 0x140u);
  /* @80ac4bb: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac4c2: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac4c4: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ac4c7: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ac4ca: add $0x1c0,%eax */
    r_eax = (r_eax + 0x1c0u);
  /* @80ac4cf: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac4d6: xor %edx,%eax */
    r_eax = (r_eax ^ r_edx);
  /* @80ac4d8: xor %eax,-0x18(%ebp) */
    s_m0x18 = (s_m0x18 ^ r_eax);
  /* @80ac4db: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ac4de: add $0x20,%eax */
    r_eax = (r_eax + 0x20u);
  /* @80ac4e1: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ac4e3: xor -0x18(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x18);
  /* @80ac4e6: mov %eax,-0x10(%ebp) */
    s_m0x10 = r_eax;
  /* @80ac4e9: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ac4ec: add $0x24,%eax */
    r_eax = (r_eax + 0x24u);
  /* @80ac4ef: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ac4f1: xor -0x18(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x18);
  /* @80ac4f4: mov %eax,-0x14(%ebp) */
    s_m0x14 = r_eax;
  /* @80ac4f7: rorl $0x4,-0x14(%ebp) */
    s_m0x14 = (s_m0x14 >> 4) | (s_m0x14 << 28);
  /* @80ac4fb: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac4fe: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ac501: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac504: mov 0x8b19e20(,%eax,4),%edx */
    r_edx = k_desSPt[(int)r_eax + 0];
  /* @80ac50b: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac50e: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ac511: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac514: sub $0xffffff80,%eax */
    r_eax = (r_eax - 0xffffff80u);
  /* @80ac517: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac51e: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac520: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac523: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ac526: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac529: add $0x100,%eax */
    r_eax = (r_eax + 0x100u);
  /* @80ac52e: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac535: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac537: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac53a: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ac53d: add $0x180,%eax */
    r_eax = (r_eax + 0x180u);
  /* @80ac542: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac549: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac54b: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ac54e: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ac551: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac554: add $0x40,%eax */
    r_eax = (r_eax + 0x40u);
  /* @80ac557: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac55e: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac560: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ac563: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ac566: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac569: add $0xc0,%eax */
    r_eax = (r_eax + 0xc0u);
  /* @80ac56e: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac575: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac577: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ac57a: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ac57d: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac580: add $0x140,%eax */
    r_eax = (r_eax + 0x140u);
  /* @80ac585: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac58c: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac58e: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ac591: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ac594: add $0x1c0,%eax */
    r_eax = (r_eax + 0x1c0u);
  /* @80ac599: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac5a0: xor %edx,%eax */
    r_eax = (r_eax ^ r_edx);
  /* @80ac5a2: xor %eax,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c ^ r_eax);
  /* @80ac5a5: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ac5a8: add $0x28,%eax */
    r_eax = (r_eax + 0x28u);
  /* @80ac5ab: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ac5ad: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80ac5b0: mov %eax,-0x10(%ebp) */
    s_m0x10 = r_eax;
  /* @80ac5b3: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ac5b6: add $0x2c,%eax */
    r_eax = (r_eax + 0x2cu);
  /* @80ac5b9: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ac5bb: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80ac5be: mov %eax,-0x14(%ebp) */
    s_m0x14 = r_eax;
  /* @80ac5c1: rorl $0x4,-0x14(%ebp) */
    s_m0x14 = (s_m0x14 >> 4) | (s_m0x14 << 28);
  /* @80ac5c5: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac5c8: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ac5cb: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac5ce: mov 0x8b19e20(,%eax,4),%edx */
    r_edx = k_desSPt[(int)r_eax + 0];
  /* @80ac5d5: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac5d8: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ac5db: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac5de: sub $0xffffff80,%eax */
    r_eax = (r_eax - 0xffffff80u);
  /* @80ac5e1: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac5e8: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac5ea: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac5ed: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ac5f0: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac5f3: add $0x100,%eax */
    r_eax = (r_eax + 0x100u);
  /* @80ac5f8: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac5ff: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac601: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac604: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ac607: add $0x180,%eax */
    r_eax = (r_eax + 0x180u);
  /* @80ac60c: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac613: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac615: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ac618: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ac61b: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac61e: add $0x40,%eax */
    r_eax = (r_eax + 0x40u);
  /* @80ac621: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac628: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac62a: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ac62d: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ac630: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac633: add $0xc0,%eax */
    r_eax = (r_eax + 0xc0u);
  /* @80ac638: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac63f: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac641: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ac644: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ac647: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac64a: add $0x140,%eax */
    r_eax = (r_eax + 0x140u);
  /* @80ac64f: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac656: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac658: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ac65b: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ac65e: add $0x1c0,%eax */
    r_eax = (r_eax + 0x1c0u);
  /* @80ac663: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac66a: xor %edx,%eax */
    r_eax = (r_eax ^ r_edx);
  /* @80ac66c: xor %eax,-0x18(%ebp) */
    s_m0x18 = (s_m0x18 ^ r_eax);
  /* @80ac66f: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ac672: add $0x30,%eax */
    r_eax = (r_eax + 0x30u);
  /* @80ac675: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ac677: xor -0x18(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x18);
  /* @80ac67a: mov %eax,-0x10(%ebp) */
    s_m0x10 = r_eax;
  /* @80ac67d: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ac680: add $0x34,%eax */
    r_eax = (r_eax + 0x34u);
  /* @80ac683: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ac685: xor -0x18(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x18);
  /* @80ac688: mov %eax,-0x14(%ebp) */
    s_m0x14 = r_eax;
  /* @80ac68b: rorl $0x4,-0x14(%ebp) */
    s_m0x14 = (s_m0x14 >> 4) | (s_m0x14 << 28);
  /* @80ac68f: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac692: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ac695: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac698: mov 0x8b19e20(,%eax,4),%edx */
    r_edx = k_desSPt[(int)r_eax + 0];
  /* @80ac69f: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac6a2: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ac6a5: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac6a8: sub $0xffffff80,%eax */
    r_eax = (r_eax - 0xffffff80u);
  /* @80ac6ab: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac6b2: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac6b4: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac6b7: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ac6ba: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac6bd: add $0x100,%eax */
    r_eax = (r_eax + 0x100u);
  /* @80ac6c2: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac6c9: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac6cb: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac6ce: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ac6d1: add $0x180,%eax */
    r_eax = (r_eax + 0x180u);
  /* @80ac6d6: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac6dd: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac6df: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ac6e2: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ac6e5: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac6e8: add $0x40,%eax */
    r_eax = (r_eax + 0x40u);
  /* @80ac6eb: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac6f2: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac6f4: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ac6f7: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ac6fa: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac6fd: add $0xc0,%eax */
    r_eax = (r_eax + 0xc0u);
  /* @80ac702: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac709: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac70b: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ac70e: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ac711: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac714: add $0x140,%eax */
    r_eax = (r_eax + 0x140u);
  /* @80ac719: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac720: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac722: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ac725: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ac728: add $0x1c0,%eax */
    r_eax = (r_eax + 0x1c0u);
  /* @80ac72d: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac734: xor %edx,%eax */
    r_eax = (r_eax ^ r_edx);
  /* @80ac736: xor %eax,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c ^ r_eax);
  /* @80ac739: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ac73c: add $0x38,%eax */
    r_eax = (r_eax + 0x38u);
  /* @80ac73f: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ac741: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80ac744: mov %eax,-0x10(%ebp) */
    s_m0x10 = r_eax;
  /* @80ac747: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ac74a: add $0x3c,%eax */
    r_eax = (r_eax + 0x3cu);
  /* @80ac74d: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ac74f: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80ac752: mov %eax,-0x14(%ebp) */
    s_m0x14 = r_eax;
  /* @80ac755: rorl $0x4,-0x14(%ebp) */
    s_m0x14 = (s_m0x14 >> 4) | (s_m0x14 << 28);
  /* @80ac759: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac75c: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ac75f: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac762: mov 0x8b19e20(,%eax,4),%edx */
    r_edx = k_desSPt[(int)r_eax + 0];
  /* @80ac769: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac76c: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ac76f: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac772: sub $0xffffff80,%eax */
    r_eax = (r_eax - 0xffffff80u);
  /* @80ac775: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac77c: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac77e: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac781: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ac784: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac787: add $0x100,%eax */
    r_eax = (r_eax + 0x100u);
  /* @80ac78c: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac793: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac795: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac798: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ac79b: add $0x180,%eax */
    r_eax = (r_eax + 0x180u);
  /* @80ac7a0: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac7a7: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac7a9: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ac7ac: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ac7af: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac7b2: add $0x40,%eax */
    r_eax = (r_eax + 0x40u);
  /* @80ac7b5: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac7bc: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac7be: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ac7c1: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ac7c4: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac7c7: add $0xc0,%eax */
    r_eax = (r_eax + 0xc0u);
  /* @80ac7cc: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac7d3: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac7d5: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ac7d8: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ac7db: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac7de: add $0x140,%eax */
    r_eax = (r_eax + 0x140u);
  /* @80ac7e3: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac7ea: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac7ec: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ac7ef: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ac7f2: add $0x1c0,%eax */
    r_eax = (r_eax + 0x1c0u);
  /* @80ac7f7: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac7fe: xor %edx,%eax */
    r_eax = (r_eax ^ r_edx);
  /* @80ac800: xor %eax,-0x18(%ebp) */
    s_m0x18 = (s_m0x18 ^ r_eax);
  /* @80ac803: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ac806: add $0x40,%eax */
    r_eax = (r_eax + 0x40u);
  /* @80ac809: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ac80b: xor -0x18(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x18);
  /* @80ac80e: mov %eax,-0x10(%ebp) */
    s_m0x10 = r_eax;
  /* @80ac811: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ac814: add $0x44,%eax */
    r_eax = (r_eax + 0x44u);
  /* @80ac817: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ac819: xor -0x18(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x18);
  /* @80ac81c: mov %eax,-0x14(%ebp) */
    s_m0x14 = r_eax;
  /* @80ac81f: rorl $0x4,-0x14(%ebp) */
    s_m0x14 = (s_m0x14 >> 4) | (s_m0x14 << 28);
  /* @80ac823: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac826: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ac829: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac82c: mov 0x8b19e20(,%eax,4),%edx */
    r_edx = k_desSPt[(int)r_eax + 0];
  /* @80ac833: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac836: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ac839: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac83c: sub $0xffffff80,%eax */
    r_eax = (r_eax - 0xffffff80u);
  /* @80ac83f: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac846: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac848: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac84b: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ac84e: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac851: add $0x100,%eax */
    r_eax = (r_eax + 0x100u);
  /* @80ac856: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac85d: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac85f: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac862: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ac865: add $0x180,%eax */
    r_eax = (r_eax + 0x180u);
  /* @80ac86a: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac871: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac873: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ac876: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ac879: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac87c: add $0x40,%eax */
    r_eax = (r_eax + 0x40u);
  /* @80ac87f: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac886: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac888: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ac88b: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ac88e: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac891: add $0xc0,%eax */
    r_eax = (r_eax + 0xc0u);
  /* @80ac896: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac89d: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac89f: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ac8a2: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ac8a5: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac8a8: add $0x140,%eax */
    r_eax = (r_eax + 0x140u);
  /* @80ac8ad: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac8b4: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac8b6: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ac8b9: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ac8bc: add $0x1c0,%eax */
    r_eax = (r_eax + 0x1c0u);
  /* @80ac8c1: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac8c8: xor %edx,%eax */
    r_eax = (r_eax ^ r_edx);
  /* @80ac8ca: xor %eax,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c ^ r_eax);
  /* @80ac8cd: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ac8d0: add $0x48,%eax */
    r_eax = (r_eax + 0x48u);
  /* @80ac8d3: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ac8d5: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80ac8d8: mov %eax,-0x10(%ebp) */
    s_m0x10 = r_eax;
  /* @80ac8db: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ac8de: add $0x4c,%eax */
    r_eax = (r_eax + 0x4cu);
  /* @80ac8e1: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ac8e3: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80ac8e6: mov %eax,-0x14(%ebp) */
    s_m0x14 = r_eax;
  /* @80ac8e9: rorl $0x4,-0x14(%ebp) */
    s_m0x14 = (s_m0x14 >> 4) | (s_m0x14 << 28);
  /* @80ac8ed: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac8f0: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ac8f3: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac8f6: mov 0x8b19e20(,%eax,4),%edx */
    r_edx = k_desSPt[(int)r_eax + 0];
  /* @80ac8fd: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac900: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ac903: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac906: sub $0xffffff80,%eax */
    r_eax = (r_eax - 0xffffff80u);
  /* @80ac909: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac910: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac912: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac915: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ac918: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac91b: add $0x100,%eax */
    r_eax = (r_eax + 0x100u);
  /* @80ac920: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac927: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac929: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac92c: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ac92f: add $0x180,%eax */
    r_eax = (r_eax + 0x180u);
  /* @80ac934: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac93b: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac93d: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ac940: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ac943: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac946: add $0x40,%eax */
    r_eax = (r_eax + 0x40u);
  /* @80ac949: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac950: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac952: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ac955: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ac958: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac95b: add $0xc0,%eax */
    r_eax = (r_eax + 0xc0u);
  /* @80ac960: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac967: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac969: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ac96c: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ac96f: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac972: add $0x140,%eax */
    r_eax = (r_eax + 0x140u);
  /* @80ac977: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac97e: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac980: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ac983: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ac986: add $0x1c0,%eax */
    r_eax = (r_eax + 0x1c0u);
  /* @80ac98b: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac992: xor %edx,%eax */
    r_eax = (r_eax ^ r_edx);
  /* @80ac994: xor %eax,-0x18(%ebp) */
    s_m0x18 = (s_m0x18 ^ r_eax);
  /* @80ac997: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ac99a: add $0x50,%eax */
    r_eax = (r_eax + 0x50u);
  /* @80ac99d: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ac99f: xor -0x18(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x18);
  /* @80ac9a2: mov %eax,-0x10(%ebp) */
    s_m0x10 = r_eax;
  /* @80ac9a5: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ac9a8: add $0x54,%eax */
    r_eax = (r_eax + 0x54u);
  /* @80ac9ab: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ac9ad: xor -0x18(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x18);
  /* @80ac9b0: mov %eax,-0x14(%ebp) */
    s_m0x14 = r_eax;
  /* @80ac9b3: rorl $0x4,-0x14(%ebp) */
    s_m0x14 = (s_m0x14 >> 4) | (s_m0x14 << 28);
  /* @80ac9b7: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac9ba: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ac9bd: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac9c0: mov 0x8b19e20(,%eax,4),%edx */
    r_edx = k_desSPt[(int)r_eax + 0];
  /* @80ac9c7: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac9ca: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ac9cd: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac9d0: sub $0xffffff80,%eax */
    r_eax = (r_eax - 0xffffff80u);
  /* @80ac9d3: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac9da: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac9dc: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac9df: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ac9e2: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac9e5: add $0x100,%eax */
    r_eax = (r_eax + 0x100u);
  /* @80ac9ea: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ac9f1: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ac9f3: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac9f6: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ac9f9: add $0x180,%eax */
    r_eax = (r_eax + 0x180u);
  /* @80ac9fe: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80aca05: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80aca07: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80aca0a: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80aca0d: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80aca10: add $0x40,%eax */
    r_eax = (r_eax + 0x40u);
  /* @80aca13: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80aca1a: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80aca1c: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80aca1f: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80aca22: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80aca25: add $0xc0,%eax */
    r_eax = (r_eax + 0xc0u);
  /* @80aca2a: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80aca31: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80aca33: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80aca36: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80aca39: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80aca3c: add $0x140,%eax */
    r_eax = (r_eax + 0x140u);
  /* @80aca41: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80aca48: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80aca4a: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80aca4d: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80aca50: add $0x1c0,%eax */
    r_eax = (r_eax + 0x1c0u);
  /* @80aca55: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80aca5c: xor %edx,%eax */
    r_eax = (r_eax ^ r_edx);
  /* @80aca5e: xor %eax,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c ^ r_eax);
  /* @80aca61: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80aca64: add $0x58,%eax */
    r_eax = (r_eax + 0x58u);
  /* @80aca67: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80aca69: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80aca6c: mov %eax,-0x10(%ebp) */
    s_m0x10 = r_eax;
  /* @80aca6f: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80aca72: add $0x5c,%eax */
    r_eax = (r_eax + 0x5cu);
  /* @80aca75: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80aca77: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80aca7a: mov %eax,-0x14(%ebp) */
    s_m0x14 = r_eax;
  /* @80aca7d: rorl $0x4,-0x14(%ebp) */
    s_m0x14 = (s_m0x14 >> 4) | (s_m0x14 << 28);
  /* @80aca81: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80aca84: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80aca87: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80aca8a: mov 0x8b19e20(,%eax,4),%edx */
    r_edx = k_desSPt[(int)r_eax + 0];
  /* @80aca91: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80aca94: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80aca97: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80aca9a: sub $0xffffff80,%eax */
    r_eax = (r_eax - 0xffffff80u);
  /* @80aca9d: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80acaa4: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80acaa6: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80acaa9: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80acaac: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80acaaf: add $0x100,%eax */
    r_eax = (r_eax + 0x100u);
  /* @80acab4: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80acabb: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80acabd: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80acac0: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80acac3: add $0x180,%eax */
    r_eax = (r_eax + 0x180u);
  /* @80acac8: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80acacf: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80acad1: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80acad4: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80acad7: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80acada: add $0x40,%eax */
    r_eax = (r_eax + 0x40u);
  /* @80acadd: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80acae4: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80acae6: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80acae9: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80acaec: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80acaef: add $0xc0,%eax */
    r_eax = (r_eax + 0xc0u);
  /* @80acaf4: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80acafb: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80acafd: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80acb00: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80acb03: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80acb06: add $0x140,%eax */
    r_eax = (r_eax + 0x140u);
  /* @80acb0b: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80acb12: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80acb14: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80acb17: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80acb1a: add $0x1c0,%eax */
    r_eax = (r_eax + 0x1c0u);
  /* @80acb1f: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80acb26: xor %edx,%eax */
    r_eax = (r_eax ^ r_edx);
  /* @80acb28: xor %eax,-0x18(%ebp) */
    s_m0x18 = (s_m0x18 ^ r_eax);
  /* @80acb2b: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80acb2e: add $0x60,%eax */
    r_eax = (r_eax + 0x60u);
  /* @80acb31: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80acb33: xor -0x18(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x18);
  /* @80acb36: mov %eax,-0x10(%ebp) */
    s_m0x10 = r_eax;
  /* @80acb39: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80acb3c: add $0x64,%eax */
    r_eax = (r_eax + 0x64u);
  /* @80acb3f: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80acb41: xor -0x18(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x18);
  /* @80acb44: mov %eax,-0x14(%ebp) */
    s_m0x14 = r_eax;
  /* @80acb47: rorl $0x4,-0x14(%ebp) */
    s_m0x14 = (s_m0x14 >> 4) | (s_m0x14 << 28);
  /* @80acb4b: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80acb4e: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80acb51: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80acb54: mov 0x8b19e20(,%eax,4),%edx */
    r_edx = k_desSPt[(int)r_eax + 0];
  /* @80acb5b: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80acb5e: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80acb61: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80acb64: sub $0xffffff80,%eax */
    r_eax = (r_eax - 0xffffff80u);
  /* @80acb67: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80acb6e: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80acb70: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80acb73: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80acb76: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80acb79: add $0x100,%eax */
    r_eax = (r_eax + 0x100u);
  /* @80acb7e: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80acb85: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80acb87: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80acb8a: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80acb8d: add $0x180,%eax */
    r_eax = (r_eax + 0x180u);
  /* @80acb92: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80acb99: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80acb9b: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80acb9e: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80acba1: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80acba4: add $0x40,%eax */
    r_eax = (r_eax + 0x40u);
  /* @80acba7: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80acbae: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80acbb0: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80acbb3: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80acbb6: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80acbb9: add $0xc0,%eax */
    r_eax = (r_eax + 0xc0u);
  /* @80acbbe: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80acbc5: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80acbc7: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80acbca: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80acbcd: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80acbd0: add $0x140,%eax */
    r_eax = (r_eax + 0x140u);
  /* @80acbd5: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80acbdc: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80acbde: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80acbe1: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80acbe4: add $0x1c0,%eax */
    r_eax = (r_eax + 0x1c0u);
  /* @80acbe9: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80acbf0: xor %edx,%eax */
    r_eax = (r_eax ^ r_edx);
  /* @80acbf2: xor %eax,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c ^ r_eax);
  /* @80acbf5: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80acbf8: add $0x68,%eax */
    r_eax = (r_eax + 0x68u);
  /* @80acbfb: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80acbfd: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80acc00: mov %eax,-0x10(%ebp) */
    s_m0x10 = r_eax;
  /* @80acc03: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80acc06: add $0x6c,%eax */
    r_eax = (r_eax + 0x6cu);
  /* @80acc09: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80acc0b: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80acc0e: mov %eax,-0x14(%ebp) */
    s_m0x14 = r_eax;
  /* @80acc11: rorl $0x4,-0x14(%ebp) */
    s_m0x14 = (s_m0x14 >> 4) | (s_m0x14 << 28);
  /* @80acc15: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80acc18: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80acc1b: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80acc1e: mov 0x8b19e20(,%eax,4),%edx */
    r_edx = k_desSPt[(int)r_eax + 0];
  /* @80acc25: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80acc28: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80acc2b: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80acc2e: sub $0xffffff80,%eax */
    r_eax = (r_eax - 0xffffff80u);
  /* @80acc31: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80acc38: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80acc3a: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80acc3d: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80acc40: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80acc43: add $0x100,%eax */
    r_eax = (r_eax + 0x100u);
  /* @80acc48: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80acc4f: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80acc51: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80acc54: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80acc57: add $0x180,%eax */
    r_eax = (r_eax + 0x180u);
  /* @80acc5c: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80acc63: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80acc65: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80acc68: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80acc6b: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80acc6e: add $0x40,%eax */
    r_eax = (r_eax + 0x40u);
  /* @80acc71: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80acc78: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80acc7a: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80acc7d: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80acc80: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80acc83: add $0xc0,%eax */
    r_eax = (r_eax + 0xc0u);
  /* @80acc88: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80acc8f: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80acc91: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80acc94: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80acc97: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80acc9a: add $0x140,%eax */
    r_eax = (r_eax + 0x140u);
  /* @80acc9f: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80acca6: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80acca8: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80accab: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80accae: add $0x1c0,%eax */
    r_eax = (r_eax + 0x1c0u);
  /* @80accb3: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80accba: xor %edx,%eax */
    r_eax = (r_eax ^ r_edx);
  /* @80accbc: xor %eax,-0x18(%ebp) */
    s_m0x18 = (s_m0x18 ^ r_eax);
  /* @80accbf: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80accc2: add $0x70,%eax */
    r_eax = (r_eax + 0x70u);
  /* @80accc5: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80accc7: xor -0x18(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x18);
  /* @80accca: mov %eax,-0x10(%ebp) */
    s_m0x10 = r_eax;
  /* @80acccd: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80accd0: add $0x74,%eax */
    r_eax = (r_eax + 0x74u);
  /* @80accd3: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80accd5: xor -0x18(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x18);
  /* @80accd8: mov %eax,-0x14(%ebp) */
    s_m0x14 = r_eax;
  /* @80accdb: rorl $0x4,-0x14(%ebp) */
    s_m0x14 = (s_m0x14 >> 4) | (s_m0x14 << 28);
  /* @80accdf: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80acce2: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80acce5: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80acce8: mov 0x8b19e20(,%eax,4),%edx */
    r_edx = k_desSPt[(int)r_eax + 0];
  /* @80accef: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80accf2: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80accf5: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80accf8: sub $0xffffff80,%eax */
    r_eax = (r_eax - 0xffffff80u);
  /* @80accfb: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80acd02: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80acd04: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80acd07: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80acd0a: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80acd0d: add $0x100,%eax */
    r_eax = (r_eax + 0x100u);
  /* @80acd12: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80acd19: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80acd1b: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80acd1e: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80acd21: add $0x180,%eax */
    r_eax = (r_eax + 0x180u);
  /* @80acd26: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80acd2d: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80acd2f: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80acd32: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80acd35: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80acd38: add $0x40,%eax */
    r_eax = (r_eax + 0x40u);
  /* @80acd3b: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80acd42: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80acd44: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80acd47: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80acd4a: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80acd4d: add $0xc0,%eax */
    r_eax = (r_eax + 0xc0u);
  /* @80acd52: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80acd59: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80acd5b: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80acd5e: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80acd61: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80acd64: add $0x140,%eax */
    r_eax = (r_eax + 0x140u);
  /* @80acd69: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80acd70: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80acd72: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80acd75: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80acd78: add $0x1c0,%eax */
    r_eax = (r_eax + 0x1c0u);
  /* @80acd7d: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80acd84: xor %edx,%eax */
    r_eax = (r_eax ^ r_edx);
  /* @80acd86: xor %eax,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c ^ r_eax);
  /* @80acd89: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80acd8c: add $0x78,%eax */
    r_eax = (r_eax + 0x78u);
  /* @80acd8f: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80acd91: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80acd94: mov %eax,-0x10(%ebp) */
    s_m0x10 = r_eax;
  /* @80acd97: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80acd9a: add $0x7c,%eax */
    r_eax = (r_eax + 0x7cu);
  /* @80acd9d: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80acd9f: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80acda2: mov %eax,-0x14(%ebp) */
    s_m0x14 = r_eax;
  /* @80acda5: rorl $0x4,-0x14(%ebp) */
    s_m0x14 = (s_m0x14 >> 4) | (s_m0x14 << 28);
  /* @80acda9: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80acdac: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80acdaf: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80acdb2: mov 0x8b19e20(,%eax,4),%edx */
    r_edx = k_desSPt[(int)r_eax + 0];
  /* @80acdb9: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80acdbc: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80acdbf: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80acdc2: sub $0xffffff80,%eax */
    r_eax = (r_eax - 0xffffff80u);
  /* @80acdc5: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80acdcc: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80acdce: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80acdd1: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80acdd4: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80acdd7: add $0x100,%eax */
    r_eax = (r_eax + 0x100u);
  /* @80acddc: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80acde3: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80acde5: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80acde8: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80acdeb: add $0x180,%eax */
    r_eax = (r_eax + 0x180u);
  /* @80acdf0: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80acdf7: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80acdf9: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80acdfc: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80acdff: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ace02: add $0x40,%eax */
    r_eax = (r_eax + 0x40u);
  /* @80ace05: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ace0c: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ace0e: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ace11: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ace14: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ace17: add $0xc0,%eax */
    r_eax = (r_eax + 0xc0u);
  /* @80ace1c: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ace23: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ace25: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ace28: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ace2b: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ace2e: add $0x140,%eax */
    r_eax = (r_eax + 0x140u);
  /* @80ace33: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ace3a: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ace3c: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ace3f: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ace42: add $0x1c0,%eax */
    r_eax = (r_eax + 0x1c0u);
  /* @80ace47: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ace4e: xor %edx,%eax */
    r_eax = (r_eax ^ r_edx);
  /* @80ace50: xor %eax,-0x18(%ebp) */
    s_m0x18 = (s_m0x18 ^ r_eax);
  /* @80ace53: rorl $0x3,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c >> 3) | (s_m0x1c << 29);
  /* @80ace57: rorl $0x3,-0x18(%ebp) */
    s_m0x18 = (s_m0x18 >> 3) | (s_m0x18 << 29);
  /* @80ace5b: mov -0x18(%ebp),%eax */
    r_eax = s_m0x18;
  /* @80ace5e: shr %eax */
    r_eax >>= 1;
  /* @80ace60: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80ace63: and $0x55555555,%eax */
    r_eax = (r_eax & 0x55555555u);
  /* @80ace68: mov %eax,-0x4(%ebp) */
    s_m0x4 = r_eax;
  /* @80ace6b: mov -0x4(%ebp),%eax */
    r_eax = s_m0x4;
  /* @80ace6e: xor %eax,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c ^ r_eax);
  /* @80ace71: mov -0x4(%ebp),%eax */
    r_eax = s_m0x4;
  /* @80ace74: add %eax,%eax */
    r_eax = (r_eax + r_eax);
  /* @80ace76: xor %eax,-0x18(%ebp) */
    s_m0x18 = (s_m0x18 ^ r_eax);
  /* @80ace79: mov -0x1c(%ebp),%eax */
    r_eax = s_m0x1c;
  /* @80ace7c: shr $0x8,%eax */
    r_eax >>= 8;
  /* @80ace7f: xor -0x18(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x18);
  /* @80ace82: and $0xff00ff,%eax */
    r_eax = (r_eax & 0xff00ffu);
  /* @80ace87: mov %eax,-0x4(%ebp) */
    s_m0x4 = r_eax;
  /* @80ace8a: mov -0x4(%ebp),%eax */
    r_eax = s_m0x4;
  /* @80ace8d: xor %eax,-0x18(%ebp) */
    s_m0x18 = (s_m0x18 ^ r_eax);
  /* @80ace90: mov -0x4(%ebp),%eax */
    r_eax = s_m0x4;
  /* @80ace93: shl $0x8,%eax */
    r_eax <<= 8;
  /* @80ace96: xor %eax,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c ^ r_eax);
  /* @80ace99: mov -0x18(%ebp),%eax */
    r_eax = s_m0x18;
  /* @80ace9c: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ace9f: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80acea2: and $0x33333333,%eax */
    r_eax = (r_eax & 0x33333333u);
  /* @80acea7: mov %eax,-0x4(%ebp) */
    s_m0x4 = r_eax;
  /* @80aceaa: mov -0x4(%ebp),%eax */
    r_eax = s_m0x4;
  /* @80acead: xor %eax,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c ^ r_eax);
  /* @80aceb0: mov -0x4(%ebp),%eax */
    r_eax = s_m0x4;
  /* @80aceb3: shl $0x2,%eax */
    r_eax <<= 2;
  /* @80aceb6: xor %eax,-0x18(%ebp) */
    s_m0x18 = (s_m0x18 ^ r_eax);
  /* @80aceb9: mov -0x1c(%ebp),%eax */
    r_eax = s_m0x1c;
  /* @80acebc: shr $0x10,%eax */
    r_eax >>= 16;
  /* @80acebf: xor -0x18(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x18);
  /* @80acec2: and $0xffff,%eax */
    r_eax = (r_eax & 0xffffu);
  /* @80acec7: mov %eax,-0x4(%ebp) */
    s_m0x4 = r_eax;
  /* @80aceca: mov -0x4(%ebp),%eax */
    r_eax = s_m0x4;
  /* @80acecd: xor %eax,-0x18(%ebp) */
    s_m0x18 = (s_m0x18 ^ r_eax);
  /* @80aced0: mov -0x4(%ebp),%eax */
    r_eax = s_m0x4;
  /* @80aced3: shl $0x10,%eax */
    r_eax <<= 16;
  /* @80aced6: xor %eax,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c ^ r_eax);
  /* @80aced9: mov -0x18(%ebp),%eax */
    r_eax = s_m0x18;
  /* @80acedc: shr $0x4,%eax */
    r_eax >>= 4;
  /* @80acedf: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80acee2: and $0xf0f0f0f,%eax */
    r_eax = (r_eax & 0xf0f0f0fu);
  /* @80acee7: mov %eax,-0x4(%ebp) */
    s_m0x4 = r_eax;
  /* @80aceea: mov -0x4(%ebp),%eax */
    r_eax = s_m0x4;
  /* @80aceed: xor %eax,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c ^ r_eax);
  /* @80acef0: mov -0x4(%ebp),%eax */
    r_eax = s_m0x4;
  /* @80acef3: shl $0x4,%eax */
    r_eax <<= 4;
  /* @80acef6: xor %eax,-0x18(%ebp) */
    s_m0x18 = (s_m0x18 ^ r_eax);
  /* @80acef9: mov 0xc(%ebp),%eax */
    r_eax = (unsigned int)(size_t)a1;
  /* @80acefc: mov -0x1c(%ebp),%edx */
    r_edx = s_m0x1c;
  /* @80aceff: ror $0x18,%edx */
    r_edx = (r_edx >> 24) | (r_edx << 8);
  /* @80acf02: mov %edx,%ecx */
    r_ecx = r_edx;
  /* @80acf04: and $0xff00ff,%ecx */
    r_ecx = (r_ecx & 0xff00ffu);
  /* @80acf0a: mov -0x1c(%ebp),%edx */
    r_edx = s_m0x1c;
  /* @80acf0d: ror $0x8,%edx */
    r_edx = (r_edx >> 8) | (r_edx << 24);
  /* @80acf10: and $0xff00ff00,%edx */
    r_edx = (r_edx & 0xff00ff00u);
  /* @80acf16: or %ecx,%edx */
    r_edx = (r_edx | r_ecx);
  /* @80acf18: mov %edx,(%eax) */
    *(unsigned int*)((char*)r_eax + 0) = r_edx;
  /* @80acf1a: mov 0xc(%ebp),%eax */
    r_eax = (unsigned int)(size_t)a1;
  /* @80acf1d: add $0x4,%eax */
    r_eax = (r_eax + 0x4u);
  /* @80acf20: mov -0x18(%ebp),%edx */
    r_edx = s_m0x18;
  /* @80acf23: ror $0x18,%edx */
    r_edx = (r_edx >> 24) | (r_edx << 8);
  /* @80acf26: mov %edx,%ecx */
    r_ecx = r_edx;
  /* @80acf28: and $0xff00ff,%ecx */
    r_ecx = (r_ecx & 0xff00ffu);
  /* @80acf2e: mov -0x18(%ebp),%edx */
    r_edx = s_m0x18;
  /* @80acf31: ror $0x8,%edx */
    r_edx = (r_edx >> 8) | (r_edx << 24);
  /* @80acf34: and $0xff00ff00,%edx */
    r_edx = (r_edx & 0xff00ff00u);
  /* @80acf3a: or %ecx,%edx */
    r_edx = (r_edx | r_ecx);
  /* @80acf3c: mov %edx,(%eax) */
    *(unsigned int*)((char*)r_eax + 0) = r_edx;
  /* @80acf3e: movl $0x0,-0x10(%ebp) */
    s_m0x10 = 0x0u;
  /* @80acf45: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80acf48: mov %eax,-0x14(%ebp) */
    s_m0x14 = r_eax;
  /* @80acf4b: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80acf4e: mov %eax,-0x18(%ebp) */
    s_m0x18 = r_eax;
  /* @80acf51: mov -0x18(%ebp),%eax */
    r_eax = s_m0x18;
  /* @80acf54: mov %eax,-0x1c(%ebp) */
    s_m0x1c = r_eax;
  /* @80acf57: leave  */
  /* @80acf58: ret  */
}
