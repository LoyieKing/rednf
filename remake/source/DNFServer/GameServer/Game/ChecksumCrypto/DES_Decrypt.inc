/* gen_des4.py 从 ORIG 反汇编逐指令转写（SemA04 2026-09-12）；禁止手改 */
#ifndef DES_SHIFT_DEFINED
#define DES_SHIFT_DEFINED
static unsigned int shift_shr(unsigned int v, unsigned int n){ n&=31; return n?(v>>n):v; }
static unsigned int shift_shl(unsigned int v, unsigned int n){ n&=31; return n?(v<<n):v; }
static unsigned int shift_sar(unsigned int v, unsigned int n){ n&=31; return n?(unsigned int)((int)v>>n):v; }
static unsigned int shift_ror(unsigned int v, unsigned int n){ n&=31; return n?((v>>n)|(v<<(32-n))):v; }
static unsigned int shift_rol(unsigned int v, unsigned int n){ n&=31; return n?((v<<n)|(v>>(32-n))):v; }
#endif
void DES_Decrypt(void* rkArea, unsigned char* block)
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

  /* @80acf59: push %ebp */
  /* @80acf5a: mov %esp,%ebp */
    r_ebp = r_esp;
  /* @80acf5c: sub $0x20,%esp */
    r_esp = (r_esp - 0x20u);
  /* @80acf5f: mov 0x8(%ebp),%eax */
    r_eax = (unsigned int)(size_t)a0;
  /* @80acf62: mov %eax,-0xc(%ebp) */
    s_m0xc = r_eax;
  /* @80acf65: mov 0xc(%ebp),%eax */
    r_eax = (unsigned int)(size_t)a1;
  /* @80acf68: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80acf6a: ror $0x18,%eax */
    r_eax = (r_eax >> 24) | (r_eax << 8);
  /* @80acf6d: mov %eax,%edx */
    r_edx = r_eax;
  /* @80acf6f: and $0xff00ff,%edx */
    r_edx = (r_edx & 0xff00ffu);
  /* @80acf75: mov 0xc(%ebp),%eax */
    r_eax = (unsigned int)(size_t)a1;
  /* @80acf78: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80acf7a: ror $0x8,%eax */
    r_eax = (r_eax >> 8) | (r_eax << 24);
  /* @80acf7d: and $0xff00ff00,%eax */
    r_eax = (r_eax & 0xff00ff00u);
  /* @80acf82: or %edx,%eax */
    r_eax = (r_eax | r_edx);
  /* @80acf84: mov %eax,-0x18(%ebp) */
    s_m0x18 = r_eax;
  /* @80acf87: mov 0xc(%ebp),%eax */
    r_eax = (unsigned int)(size_t)a1;
  /* @80acf8a: add $0x4,%eax */
    r_eax = (r_eax + 0x4u);
  /* @80acf8d: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80acf8f: ror $0x18,%eax */
    r_eax = (r_eax >> 24) | (r_eax << 8);
  /* @80acf92: mov %eax,%edx */
    r_edx = r_eax;
  /* @80acf94: and $0xff00ff,%edx */
    r_edx = (r_edx & 0xff00ffu);
  /* @80acf9a: mov 0xc(%ebp),%eax */
    r_eax = (unsigned int)(size_t)a1;
  /* @80acf9d: add $0x4,%eax */
    r_eax = (r_eax + 0x4u);
  /* @80acfa0: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80acfa2: ror $0x8,%eax */
    r_eax = (r_eax >> 8) | (r_eax << 24);
  /* @80acfa5: and $0xff00ff00,%eax */
    r_eax = (r_eax & 0xff00ff00u);
  /* @80acfaa: or %edx,%eax */
    r_eax = (r_eax | r_edx);
  /* @80acfac: mov %eax,-0x1c(%ebp) */
    s_m0x1c = r_eax;
  /* @80acfaf: mov -0x1c(%ebp),%eax */
    r_eax = s_m0x1c;
  /* @80acfb2: shr $0x4,%eax */
    r_eax >>= 4;
  /* @80acfb5: xor -0x18(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x18);
  /* @80acfb8: and $0xf0f0f0f,%eax */
    r_eax = (r_eax & 0xf0f0f0fu);
  /* @80acfbd: mov %eax,-0x8(%ebp) */
    s_m0x8 = r_eax;
  /* @80acfc0: mov -0x8(%ebp),%eax */
    r_eax = s_m0x8;
  /* @80acfc3: xor %eax,-0x18(%ebp) */
    s_m0x18 = (s_m0x18 ^ r_eax);
  /* @80acfc6: mov -0x8(%ebp),%eax */
    r_eax = s_m0x8;
  /* @80acfc9: shl $0x4,%eax */
    r_eax <<= 4;
  /* @80acfcc: xor %eax,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c ^ r_eax);
  /* @80acfcf: mov -0x18(%ebp),%eax */
    r_eax = s_m0x18;
  /* @80acfd2: shr $0x10,%eax */
    r_eax >>= 16;
  /* @80acfd5: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80acfd8: and $0xffff,%eax */
    r_eax = (r_eax & 0xffffu);
  /* @80acfdd: mov %eax,-0x8(%ebp) */
    s_m0x8 = r_eax;
  /* @80acfe0: mov -0x8(%ebp),%eax */
    r_eax = s_m0x8;
  /* @80acfe3: xor %eax,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c ^ r_eax);
  /* @80acfe6: mov -0x8(%ebp),%eax */
    r_eax = s_m0x8;
  /* @80acfe9: shl $0x10,%eax */
    r_eax <<= 16;
  /* @80acfec: xor %eax,-0x18(%ebp) */
    s_m0x18 = (s_m0x18 ^ r_eax);
  /* @80acfef: mov -0x1c(%ebp),%eax */
    r_eax = s_m0x1c;
  /* @80acff2: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80acff5: xor -0x18(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x18);
  /* @80acff8: and $0x33333333,%eax */
    r_eax = (r_eax & 0x33333333u);
  /* @80acffd: mov %eax,-0x8(%ebp) */
    s_m0x8 = r_eax;
  /* @80ad000: mov -0x8(%ebp),%eax */
    r_eax = s_m0x8;
  /* @80ad003: xor %eax,-0x18(%ebp) */
    s_m0x18 = (s_m0x18 ^ r_eax);
  /* @80ad006: mov -0x8(%ebp),%eax */
    r_eax = s_m0x8;
  /* @80ad009: shl $0x2,%eax */
    r_eax <<= 2;
  /* @80ad00c: xor %eax,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c ^ r_eax);
  /* @80ad00f: mov -0x18(%ebp),%eax */
    r_eax = s_m0x18;
  /* @80ad012: shr $0x8,%eax */
    r_eax >>= 8;
  /* @80ad015: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80ad018: and $0xff00ff,%eax */
    r_eax = (r_eax & 0xff00ffu);
  /* @80ad01d: mov %eax,-0x8(%ebp) */
    s_m0x8 = r_eax;
  /* @80ad020: mov -0x8(%ebp),%eax */
    r_eax = s_m0x8;
  /* @80ad023: xor %eax,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c ^ r_eax);
  /* @80ad026: mov -0x8(%ebp),%eax */
    r_eax = s_m0x8;
  /* @80ad029: shl $0x8,%eax */
    r_eax <<= 8;
  /* @80ad02c: xor %eax,-0x18(%ebp) */
    s_m0x18 = (s_m0x18 ^ r_eax);
  /* @80ad02f: mov -0x1c(%ebp),%eax */
    r_eax = s_m0x1c;
  /* @80ad032: shr %eax */
    r_eax >>= 1;
  /* @80ad034: xor -0x18(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x18);
  /* @80ad037: and $0x55555555,%eax */
    r_eax = (r_eax & 0x55555555u);
  /* @80ad03c: mov %eax,-0x8(%ebp) */
    s_m0x8 = r_eax;
  /* @80ad03f: mov -0x8(%ebp),%eax */
    r_eax = s_m0x8;
  /* @80ad042: xor %eax,-0x18(%ebp) */
    s_m0x18 = (s_m0x18 ^ r_eax);
  /* @80ad045: mov -0x8(%ebp),%eax */
    r_eax = s_m0x8;
  /* @80ad048: add %eax,%eax */
    r_eax = (r_eax + r_eax);
  /* @80ad04a: xor %eax,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c ^ r_eax);
  /* @80ad04d: rorl $0x1d,-0x18(%ebp) */
    s_m0x18 = (s_m0x18 >> 29) | (s_m0x18 << 3);
  /* @80ad051: rorl $0x1d,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c >> 29) | (s_m0x1c << 3);
  /* @80ad055: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ad058: add $0x78,%eax */
    r_eax = (r_eax + 0x78u);
  /* @80ad05b: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ad05d: xor -0x18(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x18);
  /* @80ad060: mov %eax,-0x10(%ebp) */
    s_m0x10 = r_eax;
  /* @80ad063: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ad066: add $0x7c,%eax */
    r_eax = (r_eax + 0x7cu);
  /* @80ad069: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ad06b: xor -0x18(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x18);
  /* @80ad06e: mov %eax,-0x14(%ebp) */
    s_m0x14 = r_eax;
  /* @80ad071: rorl $0x4,-0x14(%ebp) */
    s_m0x14 = (s_m0x14 >> 4) | (s_m0x14 << 28);
  /* @80ad075: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad078: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ad07b: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad07e: mov 0x8b19e20(,%eax,4),%edx */
    r_edx = k_desSPt[(int)r_eax + 0];
  /* @80ad085: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad088: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ad08b: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad08e: sub $0xffffff80,%eax */
    r_eax = (r_eax - 0xffffff80u);
  /* @80ad091: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad098: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad09a: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad09d: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ad0a0: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad0a3: add $0x100,%eax */
    r_eax = (r_eax + 0x100u);
  /* @80ad0a8: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad0af: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad0b1: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad0b4: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ad0b7: add $0x180,%eax */
    r_eax = (r_eax + 0x180u);
  /* @80ad0bc: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad0c3: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad0c5: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad0c8: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ad0cb: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad0ce: add $0x40,%eax */
    r_eax = (r_eax + 0x40u);
  /* @80ad0d1: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad0d8: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad0da: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad0dd: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ad0e0: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad0e3: add $0xc0,%eax */
    r_eax = (r_eax + 0xc0u);
  /* @80ad0e8: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad0ef: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad0f1: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad0f4: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ad0f7: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad0fa: add $0x140,%eax */
    r_eax = (r_eax + 0x140u);
  /* @80ad0ff: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad106: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad108: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad10b: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ad10e: add $0x1c0,%eax */
    r_eax = (r_eax + 0x1c0u);
  /* @80ad113: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad11a: xor %edx,%eax */
    r_eax = (r_eax ^ r_edx);
  /* @80ad11c: xor %eax,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c ^ r_eax);
  /* @80ad11f: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ad122: add $0x70,%eax */
    r_eax = (r_eax + 0x70u);
  /* @80ad125: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ad127: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80ad12a: mov %eax,-0x10(%ebp) */
    s_m0x10 = r_eax;
  /* @80ad12d: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ad130: add $0x74,%eax */
    r_eax = (r_eax + 0x74u);
  /* @80ad133: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ad135: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80ad138: mov %eax,-0x14(%ebp) */
    s_m0x14 = r_eax;
  /* @80ad13b: rorl $0x4,-0x14(%ebp) */
    s_m0x14 = (s_m0x14 >> 4) | (s_m0x14 << 28);
  /* @80ad13f: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad142: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ad145: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad148: mov 0x8b19e20(,%eax,4),%edx */
    r_edx = k_desSPt[(int)r_eax + 0];
  /* @80ad14f: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad152: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ad155: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad158: sub $0xffffff80,%eax */
    r_eax = (r_eax - 0xffffff80u);
  /* @80ad15b: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad162: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad164: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad167: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ad16a: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad16d: add $0x100,%eax */
    r_eax = (r_eax + 0x100u);
  /* @80ad172: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad179: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad17b: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad17e: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ad181: add $0x180,%eax */
    r_eax = (r_eax + 0x180u);
  /* @80ad186: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad18d: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad18f: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad192: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ad195: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad198: add $0x40,%eax */
    r_eax = (r_eax + 0x40u);
  /* @80ad19b: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad1a2: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad1a4: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad1a7: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ad1aa: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad1ad: add $0xc0,%eax */
    r_eax = (r_eax + 0xc0u);
  /* @80ad1b2: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad1b9: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad1bb: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad1be: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ad1c1: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad1c4: add $0x140,%eax */
    r_eax = (r_eax + 0x140u);
  /* @80ad1c9: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad1d0: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad1d2: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad1d5: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ad1d8: add $0x1c0,%eax */
    r_eax = (r_eax + 0x1c0u);
  /* @80ad1dd: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad1e4: xor %edx,%eax */
    r_eax = (r_eax ^ r_edx);
  /* @80ad1e6: xor %eax,-0x18(%ebp) */
    s_m0x18 = (s_m0x18 ^ r_eax);
  /* @80ad1e9: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ad1ec: add $0x68,%eax */
    r_eax = (r_eax + 0x68u);
  /* @80ad1ef: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ad1f1: xor -0x18(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x18);
  /* @80ad1f4: mov %eax,-0x10(%ebp) */
    s_m0x10 = r_eax;
  /* @80ad1f7: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ad1fa: add $0x6c,%eax */
    r_eax = (r_eax + 0x6cu);
  /* @80ad1fd: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ad1ff: xor -0x18(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x18);
  /* @80ad202: mov %eax,-0x14(%ebp) */
    s_m0x14 = r_eax;
  /* @80ad205: rorl $0x4,-0x14(%ebp) */
    s_m0x14 = (s_m0x14 >> 4) | (s_m0x14 << 28);
  /* @80ad209: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad20c: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ad20f: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad212: mov 0x8b19e20(,%eax,4),%edx */
    r_edx = k_desSPt[(int)r_eax + 0];
  /* @80ad219: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad21c: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ad21f: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad222: sub $0xffffff80,%eax */
    r_eax = (r_eax - 0xffffff80u);
  /* @80ad225: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad22c: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad22e: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad231: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ad234: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad237: add $0x100,%eax */
    r_eax = (r_eax + 0x100u);
  /* @80ad23c: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad243: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad245: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad248: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ad24b: add $0x180,%eax */
    r_eax = (r_eax + 0x180u);
  /* @80ad250: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad257: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad259: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad25c: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ad25f: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad262: add $0x40,%eax */
    r_eax = (r_eax + 0x40u);
  /* @80ad265: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad26c: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad26e: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad271: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ad274: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad277: add $0xc0,%eax */
    r_eax = (r_eax + 0xc0u);
  /* @80ad27c: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad283: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad285: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad288: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ad28b: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad28e: add $0x140,%eax */
    r_eax = (r_eax + 0x140u);
  /* @80ad293: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad29a: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad29c: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad29f: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ad2a2: add $0x1c0,%eax */
    r_eax = (r_eax + 0x1c0u);
  /* @80ad2a7: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad2ae: xor %edx,%eax */
    r_eax = (r_eax ^ r_edx);
  /* @80ad2b0: xor %eax,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c ^ r_eax);
  /* @80ad2b3: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ad2b6: add $0x60,%eax */
    r_eax = (r_eax + 0x60u);
  /* @80ad2b9: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ad2bb: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80ad2be: mov %eax,-0x10(%ebp) */
    s_m0x10 = r_eax;
  /* @80ad2c1: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ad2c4: add $0x64,%eax */
    r_eax = (r_eax + 0x64u);
  /* @80ad2c7: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ad2c9: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80ad2cc: mov %eax,-0x14(%ebp) */
    s_m0x14 = r_eax;
  /* @80ad2cf: rorl $0x4,-0x14(%ebp) */
    s_m0x14 = (s_m0x14 >> 4) | (s_m0x14 << 28);
  /* @80ad2d3: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad2d6: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ad2d9: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad2dc: mov 0x8b19e20(,%eax,4),%edx */
    r_edx = k_desSPt[(int)r_eax + 0];
  /* @80ad2e3: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad2e6: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ad2e9: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad2ec: sub $0xffffff80,%eax */
    r_eax = (r_eax - 0xffffff80u);
  /* @80ad2ef: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad2f6: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad2f8: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad2fb: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ad2fe: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad301: add $0x100,%eax */
    r_eax = (r_eax + 0x100u);
  /* @80ad306: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad30d: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad30f: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad312: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ad315: add $0x180,%eax */
    r_eax = (r_eax + 0x180u);
  /* @80ad31a: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad321: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad323: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad326: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ad329: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad32c: add $0x40,%eax */
    r_eax = (r_eax + 0x40u);
  /* @80ad32f: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad336: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad338: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad33b: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ad33e: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad341: add $0xc0,%eax */
    r_eax = (r_eax + 0xc0u);
  /* @80ad346: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad34d: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad34f: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad352: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ad355: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad358: add $0x140,%eax */
    r_eax = (r_eax + 0x140u);
  /* @80ad35d: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad364: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad366: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad369: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ad36c: add $0x1c0,%eax */
    r_eax = (r_eax + 0x1c0u);
  /* @80ad371: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad378: xor %edx,%eax */
    r_eax = (r_eax ^ r_edx);
  /* @80ad37a: xor %eax,-0x18(%ebp) */
    s_m0x18 = (s_m0x18 ^ r_eax);
  /* @80ad37d: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ad380: add $0x58,%eax */
    r_eax = (r_eax + 0x58u);
  /* @80ad383: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ad385: xor -0x18(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x18);
  /* @80ad388: mov %eax,-0x10(%ebp) */
    s_m0x10 = r_eax;
  /* @80ad38b: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ad38e: add $0x5c,%eax */
    r_eax = (r_eax + 0x5cu);
  /* @80ad391: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ad393: xor -0x18(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x18);
  /* @80ad396: mov %eax,-0x14(%ebp) */
    s_m0x14 = r_eax;
  /* @80ad399: rorl $0x4,-0x14(%ebp) */
    s_m0x14 = (s_m0x14 >> 4) | (s_m0x14 << 28);
  /* @80ad39d: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad3a0: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ad3a3: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad3a6: mov 0x8b19e20(,%eax,4),%edx */
    r_edx = k_desSPt[(int)r_eax + 0];
  /* @80ad3ad: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad3b0: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ad3b3: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad3b6: sub $0xffffff80,%eax */
    r_eax = (r_eax - 0xffffff80u);
  /* @80ad3b9: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad3c0: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad3c2: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad3c5: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ad3c8: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad3cb: add $0x100,%eax */
    r_eax = (r_eax + 0x100u);
  /* @80ad3d0: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad3d7: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad3d9: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad3dc: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ad3df: add $0x180,%eax */
    r_eax = (r_eax + 0x180u);
  /* @80ad3e4: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad3eb: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad3ed: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad3f0: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ad3f3: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad3f6: add $0x40,%eax */
    r_eax = (r_eax + 0x40u);
  /* @80ad3f9: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad400: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad402: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad405: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ad408: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad40b: add $0xc0,%eax */
    r_eax = (r_eax + 0xc0u);
  /* @80ad410: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad417: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad419: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad41c: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ad41f: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad422: add $0x140,%eax */
    r_eax = (r_eax + 0x140u);
  /* @80ad427: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad42e: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad430: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad433: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ad436: add $0x1c0,%eax */
    r_eax = (r_eax + 0x1c0u);
  /* @80ad43b: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad442: xor %edx,%eax */
    r_eax = (r_eax ^ r_edx);
  /* @80ad444: xor %eax,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c ^ r_eax);
  /* @80ad447: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ad44a: add $0x50,%eax */
    r_eax = (r_eax + 0x50u);
  /* @80ad44d: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ad44f: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80ad452: mov %eax,-0x10(%ebp) */
    s_m0x10 = r_eax;
  /* @80ad455: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ad458: add $0x54,%eax */
    r_eax = (r_eax + 0x54u);
  /* @80ad45b: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ad45d: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80ad460: mov %eax,-0x14(%ebp) */
    s_m0x14 = r_eax;
  /* @80ad463: rorl $0x4,-0x14(%ebp) */
    s_m0x14 = (s_m0x14 >> 4) | (s_m0x14 << 28);
  /* @80ad467: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad46a: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ad46d: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad470: mov 0x8b19e20(,%eax,4),%edx */
    r_edx = k_desSPt[(int)r_eax + 0];
  /* @80ad477: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad47a: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ad47d: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad480: sub $0xffffff80,%eax */
    r_eax = (r_eax - 0xffffff80u);
  /* @80ad483: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad48a: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad48c: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad48f: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ad492: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad495: add $0x100,%eax */
    r_eax = (r_eax + 0x100u);
  /* @80ad49a: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad4a1: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad4a3: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad4a6: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ad4a9: add $0x180,%eax */
    r_eax = (r_eax + 0x180u);
  /* @80ad4ae: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad4b5: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad4b7: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad4ba: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ad4bd: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad4c0: add $0x40,%eax */
    r_eax = (r_eax + 0x40u);
  /* @80ad4c3: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad4ca: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad4cc: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad4cf: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ad4d2: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad4d5: add $0xc0,%eax */
    r_eax = (r_eax + 0xc0u);
  /* @80ad4da: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad4e1: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad4e3: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad4e6: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ad4e9: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad4ec: add $0x140,%eax */
    r_eax = (r_eax + 0x140u);
  /* @80ad4f1: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad4f8: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad4fa: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad4fd: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ad500: add $0x1c0,%eax */
    r_eax = (r_eax + 0x1c0u);
  /* @80ad505: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad50c: xor %edx,%eax */
    r_eax = (r_eax ^ r_edx);
  /* @80ad50e: xor %eax,-0x18(%ebp) */
    s_m0x18 = (s_m0x18 ^ r_eax);
  /* @80ad511: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ad514: add $0x48,%eax */
    r_eax = (r_eax + 0x48u);
  /* @80ad517: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ad519: xor -0x18(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x18);
  /* @80ad51c: mov %eax,-0x10(%ebp) */
    s_m0x10 = r_eax;
  /* @80ad51f: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ad522: add $0x4c,%eax */
    r_eax = (r_eax + 0x4cu);
  /* @80ad525: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ad527: xor -0x18(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x18);
  /* @80ad52a: mov %eax,-0x14(%ebp) */
    s_m0x14 = r_eax;
  /* @80ad52d: rorl $0x4,-0x14(%ebp) */
    s_m0x14 = (s_m0x14 >> 4) | (s_m0x14 << 28);
  /* @80ad531: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad534: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ad537: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad53a: mov 0x8b19e20(,%eax,4),%edx */
    r_edx = k_desSPt[(int)r_eax + 0];
  /* @80ad541: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad544: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ad547: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad54a: sub $0xffffff80,%eax */
    r_eax = (r_eax - 0xffffff80u);
  /* @80ad54d: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad554: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad556: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad559: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ad55c: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad55f: add $0x100,%eax */
    r_eax = (r_eax + 0x100u);
  /* @80ad564: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad56b: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad56d: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad570: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ad573: add $0x180,%eax */
    r_eax = (r_eax + 0x180u);
  /* @80ad578: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad57f: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad581: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad584: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ad587: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad58a: add $0x40,%eax */
    r_eax = (r_eax + 0x40u);
  /* @80ad58d: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad594: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad596: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad599: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ad59c: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad59f: add $0xc0,%eax */
    r_eax = (r_eax + 0xc0u);
  /* @80ad5a4: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad5ab: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad5ad: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad5b0: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ad5b3: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad5b6: add $0x140,%eax */
    r_eax = (r_eax + 0x140u);
  /* @80ad5bb: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad5c2: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad5c4: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad5c7: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ad5ca: add $0x1c0,%eax */
    r_eax = (r_eax + 0x1c0u);
  /* @80ad5cf: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad5d6: xor %edx,%eax */
    r_eax = (r_eax ^ r_edx);
  /* @80ad5d8: xor %eax,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c ^ r_eax);
  /* @80ad5db: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ad5de: add $0x40,%eax */
    r_eax = (r_eax + 0x40u);
  /* @80ad5e1: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ad5e3: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80ad5e6: mov %eax,-0x10(%ebp) */
    s_m0x10 = r_eax;
  /* @80ad5e9: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ad5ec: add $0x44,%eax */
    r_eax = (r_eax + 0x44u);
  /* @80ad5ef: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ad5f1: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80ad5f4: mov %eax,-0x14(%ebp) */
    s_m0x14 = r_eax;
  /* @80ad5f7: rorl $0x4,-0x14(%ebp) */
    s_m0x14 = (s_m0x14 >> 4) | (s_m0x14 << 28);
  /* @80ad5fb: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad5fe: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ad601: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad604: mov 0x8b19e20(,%eax,4),%edx */
    r_edx = k_desSPt[(int)r_eax + 0];
  /* @80ad60b: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad60e: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ad611: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad614: sub $0xffffff80,%eax */
    r_eax = (r_eax - 0xffffff80u);
  /* @80ad617: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad61e: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad620: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad623: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ad626: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad629: add $0x100,%eax */
    r_eax = (r_eax + 0x100u);
  /* @80ad62e: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad635: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad637: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad63a: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ad63d: add $0x180,%eax */
    r_eax = (r_eax + 0x180u);
  /* @80ad642: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad649: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad64b: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad64e: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ad651: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad654: add $0x40,%eax */
    r_eax = (r_eax + 0x40u);
  /* @80ad657: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad65e: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad660: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad663: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ad666: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad669: add $0xc0,%eax */
    r_eax = (r_eax + 0xc0u);
  /* @80ad66e: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad675: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad677: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad67a: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ad67d: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad680: add $0x140,%eax */
    r_eax = (r_eax + 0x140u);
  /* @80ad685: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad68c: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad68e: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad691: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ad694: add $0x1c0,%eax */
    r_eax = (r_eax + 0x1c0u);
  /* @80ad699: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad6a0: xor %edx,%eax */
    r_eax = (r_eax ^ r_edx);
  /* @80ad6a2: xor %eax,-0x18(%ebp) */
    s_m0x18 = (s_m0x18 ^ r_eax);
  /* @80ad6a5: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ad6a8: add $0x38,%eax */
    r_eax = (r_eax + 0x38u);
  /* @80ad6ab: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ad6ad: xor -0x18(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x18);
  /* @80ad6b0: mov %eax,-0x10(%ebp) */
    s_m0x10 = r_eax;
  /* @80ad6b3: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ad6b6: add $0x3c,%eax */
    r_eax = (r_eax + 0x3cu);
  /* @80ad6b9: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ad6bb: xor -0x18(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x18);
  /* @80ad6be: mov %eax,-0x14(%ebp) */
    s_m0x14 = r_eax;
  /* @80ad6c1: rorl $0x4,-0x14(%ebp) */
    s_m0x14 = (s_m0x14 >> 4) | (s_m0x14 << 28);
  /* @80ad6c5: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad6c8: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ad6cb: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad6ce: mov 0x8b19e20(,%eax,4),%edx */
    r_edx = k_desSPt[(int)r_eax + 0];
  /* @80ad6d5: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad6d8: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ad6db: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad6de: sub $0xffffff80,%eax */
    r_eax = (r_eax - 0xffffff80u);
  /* @80ad6e1: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad6e8: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad6ea: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad6ed: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ad6f0: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad6f3: add $0x100,%eax */
    r_eax = (r_eax + 0x100u);
  /* @80ad6f8: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad6ff: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad701: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad704: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ad707: add $0x180,%eax */
    r_eax = (r_eax + 0x180u);
  /* @80ad70c: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad713: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad715: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad718: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ad71b: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad71e: add $0x40,%eax */
    r_eax = (r_eax + 0x40u);
  /* @80ad721: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad728: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad72a: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad72d: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ad730: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad733: add $0xc0,%eax */
    r_eax = (r_eax + 0xc0u);
  /* @80ad738: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad73f: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad741: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad744: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ad747: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad74a: add $0x140,%eax */
    r_eax = (r_eax + 0x140u);
  /* @80ad74f: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad756: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad758: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad75b: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ad75e: add $0x1c0,%eax */
    r_eax = (r_eax + 0x1c0u);
  /* @80ad763: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad76a: xor %edx,%eax */
    r_eax = (r_eax ^ r_edx);
  /* @80ad76c: xor %eax,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c ^ r_eax);
  /* @80ad76f: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ad772: add $0x30,%eax */
    r_eax = (r_eax + 0x30u);
  /* @80ad775: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ad777: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80ad77a: mov %eax,-0x10(%ebp) */
    s_m0x10 = r_eax;
  /* @80ad77d: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ad780: add $0x34,%eax */
    r_eax = (r_eax + 0x34u);
  /* @80ad783: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ad785: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80ad788: mov %eax,-0x14(%ebp) */
    s_m0x14 = r_eax;
  /* @80ad78b: rorl $0x4,-0x14(%ebp) */
    s_m0x14 = (s_m0x14 >> 4) | (s_m0x14 << 28);
  /* @80ad78f: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad792: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ad795: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad798: mov 0x8b19e20(,%eax,4),%edx */
    r_edx = k_desSPt[(int)r_eax + 0];
  /* @80ad79f: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad7a2: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ad7a5: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad7a8: sub $0xffffff80,%eax */
    r_eax = (r_eax - 0xffffff80u);
  /* @80ad7ab: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad7b2: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad7b4: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad7b7: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ad7ba: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad7bd: add $0x100,%eax */
    r_eax = (r_eax + 0x100u);
  /* @80ad7c2: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad7c9: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad7cb: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad7ce: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ad7d1: add $0x180,%eax */
    r_eax = (r_eax + 0x180u);
  /* @80ad7d6: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad7dd: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad7df: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad7e2: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ad7e5: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad7e8: add $0x40,%eax */
    r_eax = (r_eax + 0x40u);
  /* @80ad7eb: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad7f2: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad7f4: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad7f7: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ad7fa: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad7fd: add $0xc0,%eax */
    r_eax = (r_eax + 0xc0u);
  /* @80ad802: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad809: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad80b: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad80e: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ad811: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad814: add $0x140,%eax */
    r_eax = (r_eax + 0x140u);
  /* @80ad819: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad820: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad822: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad825: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ad828: add $0x1c0,%eax */
    r_eax = (r_eax + 0x1c0u);
  /* @80ad82d: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad834: xor %edx,%eax */
    r_eax = (r_eax ^ r_edx);
  /* @80ad836: xor %eax,-0x18(%ebp) */
    s_m0x18 = (s_m0x18 ^ r_eax);
  /* @80ad839: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ad83c: add $0x28,%eax */
    r_eax = (r_eax + 0x28u);
  /* @80ad83f: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ad841: xor -0x18(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x18);
  /* @80ad844: mov %eax,-0x10(%ebp) */
    s_m0x10 = r_eax;
  /* @80ad847: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ad84a: add $0x2c,%eax */
    r_eax = (r_eax + 0x2cu);
  /* @80ad84d: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ad84f: xor -0x18(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x18);
  /* @80ad852: mov %eax,-0x14(%ebp) */
    s_m0x14 = r_eax;
  /* @80ad855: rorl $0x4,-0x14(%ebp) */
    s_m0x14 = (s_m0x14 >> 4) | (s_m0x14 << 28);
  /* @80ad859: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad85c: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ad85f: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad862: mov 0x8b19e20(,%eax,4),%edx */
    r_edx = k_desSPt[(int)r_eax + 0];
  /* @80ad869: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad86c: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ad86f: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad872: sub $0xffffff80,%eax */
    r_eax = (r_eax - 0xffffff80u);
  /* @80ad875: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad87c: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad87e: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad881: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ad884: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad887: add $0x100,%eax */
    r_eax = (r_eax + 0x100u);
  /* @80ad88c: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad893: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad895: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad898: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ad89b: add $0x180,%eax */
    r_eax = (r_eax + 0x180u);
  /* @80ad8a0: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad8a7: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad8a9: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad8ac: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ad8af: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad8b2: add $0x40,%eax */
    r_eax = (r_eax + 0x40u);
  /* @80ad8b5: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad8bc: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad8be: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad8c1: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ad8c4: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad8c7: add $0xc0,%eax */
    r_eax = (r_eax + 0xc0u);
  /* @80ad8cc: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad8d3: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad8d5: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad8d8: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ad8db: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad8de: add $0x140,%eax */
    r_eax = (r_eax + 0x140u);
  /* @80ad8e3: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad8ea: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad8ec: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad8ef: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ad8f2: add $0x1c0,%eax */
    r_eax = (r_eax + 0x1c0u);
  /* @80ad8f7: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad8fe: xor %edx,%eax */
    r_eax = (r_eax ^ r_edx);
  /* @80ad900: xor %eax,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c ^ r_eax);
  /* @80ad903: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ad906: add $0x20,%eax */
    r_eax = (r_eax + 0x20u);
  /* @80ad909: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ad90b: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80ad90e: mov %eax,-0x10(%ebp) */
    s_m0x10 = r_eax;
  /* @80ad911: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ad914: add $0x24,%eax */
    r_eax = (r_eax + 0x24u);
  /* @80ad917: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ad919: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80ad91c: mov %eax,-0x14(%ebp) */
    s_m0x14 = r_eax;
  /* @80ad91f: rorl $0x4,-0x14(%ebp) */
    s_m0x14 = (s_m0x14 >> 4) | (s_m0x14 << 28);
  /* @80ad923: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad926: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ad929: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad92c: mov 0x8b19e20(,%eax,4),%edx */
    r_edx = k_desSPt[(int)r_eax + 0];
  /* @80ad933: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad936: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ad939: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad93c: sub $0xffffff80,%eax */
    r_eax = (r_eax - 0xffffff80u);
  /* @80ad93f: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad946: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad948: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad94b: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ad94e: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad951: add $0x100,%eax */
    r_eax = (r_eax + 0x100u);
  /* @80ad956: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad95d: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad95f: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad962: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ad965: add $0x180,%eax */
    r_eax = (r_eax + 0x180u);
  /* @80ad96a: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad971: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad973: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad976: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ad979: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad97c: add $0x40,%eax */
    r_eax = (r_eax + 0x40u);
  /* @80ad97f: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad986: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad988: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad98b: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ad98e: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad991: add $0xc0,%eax */
    r_eax = (r_eax + 0xc0u);
  /* @80ad996: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad99d: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad99f: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad9a2: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ad9a5: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad9a8: add $0x140,%eax */
    r_eax = (r_eax + 0x140u);
  /* @80ad9ad: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad9b4: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ad9b6: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ad9b9: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ad9bc: add $0x1c0,%eax */
    r_eax = (r_eax + 0x1c0u);
  /* @80ad9c1: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ad9c8: xor %edx,%eax */
    r_eax = (r_eax ^ r_edx);
  /* @80ad9ca: xor %eax,-0x18(%ebp) */
    s_m0x18 = (s_m0x18 ^ r_eax);
  /* @80ad9cd: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ad9d0: add $0x18,%eax */
    r_eax = (r_eax + 0x18u);
  /* @80ad9d3: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ad9d5: xor -0x18(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x18);
  /* @80ad9d8: mov %eax,-0x10(%ebp) */
    s_m0x10 = r_eax;
  /* @80ad9db: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ad9de: add $0x1c,%eax */
    r_eax = (r_eax + 0x1cu);
  /* @80ad9e1: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ad9e3: xor -0x18(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x18);
  /* @80ad9e6: mov %eax,-0x14(%ebp) */
    s_m0x14 = r_eax;
  /* @80ad9e9: rorl $0x4,-0x14(%ebp) */
    s_m0x14 = (s_m0x14 >> 4) | (s_m0x14 << 28);
  /* @80ad9ed: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ad9f0: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ad9f3: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ad9f6: mov 0x8b19e20(,%eax,4),%edx */
    r_edx = k_desSPt[(int)r_eax + 0];
  /* @80ad9fd: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ada00: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ada03: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ada06: sub $0xffffff80,%eax */
    r_eax = (r_eax - 0xffffff80u);
  /* @80ada09: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ada10: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ada12: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ada15: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ada18: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ada1b: add $0x100,%eax */
    r_eax = (r_eax + 0x100u);
  /* @80ada20: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ada27: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ada29: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ada2c: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ada2f: add $0x180,%eax */
    r_eax = (r_eax + 0x180u);
  /* @80ada34: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ada3b: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ada3d: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ada40: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80ada43: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ada46: add $0x40,%eax */
    r_eax = (r_eax + 0x40u);
  /* @80ada49: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ada50: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ada52: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ada55: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80ada58: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ada5b: add $0xc0,%eax */
    r_eax = (r_eax + 0xc0u);
  /* @80ada60: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ada67: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ada69: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ada6c: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80ada6f: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ada72: add $0x140,%eax */
    r_eax = (r_eax + 0x140u);
  /* @80ada77: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ada7e: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80ada80: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ada83: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80ada86: add $0x1c0,%eax */
    r_eax = (r_eax + 0x1c0u);
  /* @80ada8b: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80ada92: xor %edx,%eax */
    r_eax = (r_eax ^ r_edx);
  /* @80ada94: xor %eax,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c ^ r_eax);
  /* @80ada97: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ada9a: add $0x10,%eax */
    r_eax = (r_eax + 0x10u);
  /* @80ada9d: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80ada9f: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80adaa2: mov %eax,-0x10(%ebp) */
    s_m0x10 = r_eax;
  /* @80adaa5: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80adaa8: add $0x14,%eax */
    r_eax = (r_eax + 0x14u);
  /* @80adaab: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80adaad: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80adab0: mov %eax,-0x14(%ebp) */
    s_m0x14 = r_eax;
  /* @80adab3: rorl $0x4,-0x14(%ebp) */
    s_m0x14 = (s_m0x14 >> 4) | (s_m0x14 << 28);
  /* @80adab7: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80adaba: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80adabd: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80adac0: mov 0x8b19e20(,%eax,4),%edx */
    r_edx = k_desSPt[(int)r_eax + 0];
  /* @80adac7: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80adaca: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80adacd: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80adad0: sub $0xffffff80,%eax */
    r_eax = (r_eax - 0xffffff80u);
  /* @80adad3: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80adada: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80adadc: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80adadf: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80adae2: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80adae5: add $0x100,%eax */
    r_eax = (r_eax + 0x100u);
  /* @80adaea: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80adaf1: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80adaf3: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80adaf6: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80adaf9: add $0x180,%eax */
    r_eax = (r_eax + 0x180u);
  /* @80adafe: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80adb05: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80adb07: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80adb0a: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80adb0d: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80adb10: add $0x40,%eax */
    r_eax = (r_eax + 0x40u);
  /* @80adb13: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80adb1a: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80adb1c: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80adb1f: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80adb22: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80adb25: add $0xc0,%eax */
    r_eax = (r_eax + 0xc0u);
  /* @80adb2a: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80adb31: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80adb33: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80adb36: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80adb39: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80adb3c: add $0x140,%eax */
    r_eax = (r_eax + 0x140u);
  /* @80adb41: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80adb48: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80adb4a: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80adb4d: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80adb50: add $0x1c0,%eax */
    r_eax = (r_eax + 0x1c0u);
  /* @80adb55: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80adb5c: xor %edx,%eax */
    r_eax = (r_eax ^ r_edx);
  /* @80adb5e: xor %eax,-0x18(%ebp) */
    s_m0x18 = (s_m0x18 ^ r_eax);
  /* @80adb61: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80adb64: add $0x8,%eax */
    r_eax = (r_eax + 0x8u);
  /* @80adb67: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80adb69: xor -0x18(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x18);
  /* @80adb6c: mov %eax,-0x10(%ebp) */
    s_m0x10 = r_eax;
  /* @80adb6f: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80adb72: add $0xc,%eax */
    r_eax = (r_eax + 0xcu);
  /* @80adb75: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80adb77: xor -0x18(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x18);
  /* @80adb7a: mov %eax,-0x14(%ebp) */
    s_m0x14 = r_eax;
  /* @80adb7d: rorl $0x4,-0x14(%ebp) */
    s_m0x14 = (s_m0x14 >> 4) | (s_m0x14 << 28);
  /* @80adb81: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80adb84: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80adb87: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80adb8a: mov 0x8b19e20(,%eax,4),%edx */
    r_edx = k_desSPt[(int)r_eax + 0];
  /* @80adb91: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80adb94: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80adb97: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80adb9a: sub $0xffffff80,%eax */
    r_eax = (r_eax - 0xffffff80u);
  /* @80adb9d: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80adba4: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80adba6: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80adba9: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80adbac: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80adbaf: add $0x100,%eax */
    r_eax = (r_eax + 0x100u);
  /* @80adbb4: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80adbbb: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80adbbd: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80adbc0: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80adbc3: add $0x180,%eax */
    r_eax = (r_eax + 0x180u);
  /* @80adbc8: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80adbcf: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80adbd1: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80adbd4: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80adbd7: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80adbda: add $0x40,%eax */
    r_eax = (r_eax + 0x40u);
  /* @80adbdd: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80adbe4: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80adbe6: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80adbe9: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80adbec: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80adbef: add $0xc0,%eax */
    r_eax = (r_eax + 0xc0u);
  /* @80adbf4: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80adbfb: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80adbfd: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80adc00: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80adc03: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80adc06: add $0x140,%eax */
    r_eax = (r_eax + 0x140u);
  /* @80adc0b: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80adc12: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80adc14: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80adc17: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80adc1a: add $0x1c0,%eax */
    r_eax = (r_eax + 0x1c0u);
  /* @80adc1f: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80adc26: xor %edx,%eax */
    r_eax = (r_eax ^ r_edx);
  /* @80adc28: xor %eax,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c ^ r_eax);
  /* @80adc2b: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80adc2e: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80adc30: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80adc33: mov %eax,-0x10(%ebp) */
    s_m0x10 = r_eax;
  /* @80adc36: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80adc39: add $0x4,%eax */
    r_eax = (r_eax + 0x4u);
  /* @80adc3c: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80adc3e: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80adc41: mov %eax,-0x14(%ebp) */
    s_m0x14 = r_eax;
  /* @80adc44: rorl $0x4,-0x14(%ebp) */
    s_m0x14 = (s_m0x14 >> 4) | (s_m0x14 << 28);
  /* @80adc48: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80adc4b: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80adc4e: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80adc51: mov 0x8b19e20(,%eax,4),%edx */
    r_edx = k_desSPt[(int)r_eax + 0];
  /* @80adc58: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80adc5b: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80adc5e: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80adc61: sub $0xffffff80,%eax */
    r_eax = (r_eax - 0xffffff80u);
  /* @80adc64: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80adc6b: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80adc6d: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80adc70: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80adc73: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80adc76: add $0x100,%eax */
    r_eax = (r_eax + 0x100u);
  /* @80adc7b: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80adc82: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80adc84: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80adc87: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80adc8a: add $0x180,%eax */
    r_eax = (r_eax + 0x180u);
  /* @80adc8f: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80adc96: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80adc98: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80adc9b: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80adc9e: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80adca1: add $0x40,%eax */
    r_eax = (r_eax + 0x40u);
  /* @80adca4: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80adcab: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80adcad: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80adcb0: shr $0xa,%eax */
    r_eax >>= 10;
  /* @80adcb3: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80adcb6: add $0xc0,%eax */
    r_eax = (r_eax + 0xc0u);
  /* @80adcbb: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80adcc2: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80adcc4: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80adcc7: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80adcca: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80adccd: add $0x140,%eax */
    r_eax = (r_eax + 0x140u);
  /* @80adcd2: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80adcd9: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80adcdb: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80adcde: shr $0x1a,%eax */
    r_eax >>= 26;
  /* @80adce1: add $0x1c0,%eax */
    r_eax = (r_eax + 0x1c0u);
  /* @80adce6: mov 0x8b19e20(,%eax,4),%eax */
    r_eax = k_desSPt[(int)r_eax + 0];
  /* @80adced: xor %edx,%eax */
    r_eax = (r_eax ^ r_edx);
  /* @80adcef: xor %eax,-0x18(%ebp) */
    s_m0x18 = (s_m0x18 ^ r_eax);
  /* @80adcf2: rorl $0x3,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c >> 3) | (s_m0x1c << 29);
  /* @80adcf6: rorl $0x3,-0x18(%ebp) */
    s_m0x18 = (s_m0x18 >> 3) | (s_m0x18 << 29);
  /* @80adcfa: mov -0x18(%ebp),%eax */
    r_eax = s_m0x18;
  /* @80adcfd: shr %eax */
    r_eax >>= 1;
  /* @80adcff: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80add02: and $0x55555555,%eax */
    r_eax = (r_eax & 0x55555555u);
  /* @80add07: mov %eax,-0x4(%ebp) */
    s_m0x4 = r_eax;
  /* @80add0a: mov -0x4(%ebp),%eax */
    r_eax = s_m0x4;
  /* @80add0d: xor %eax,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c ^ r_eax);
  /* @80add10: mov -0x4(%ebp),%eax */
    r_eax = s_m0x4;
  /* @80add13: add %eax,%eax */
    r_eax = (r_eax + r_eax);
  /* @80add15: xor %eax,-0x18(%ebp) */
    s_m0x18 = (s_m0x18 ^ r_eax);
  /* @80add18: mov -0x1c(%ebp),%eax */
    r_eax = s_m0x1c;
  /* @80add1b: shr $0x8,%eax */
    r_eax >>= 8;
  /* @80add1e: xor -0x18(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x18);
  /* @80add21: and $0xff00ff,%eax */
    r_eax = (r_eax & 0xff00ffu);
  /* @80add26: mov %eax,-0x4(%ebp) */
    s_m0x4 = r_eax;
  /* @80add29: mov -0x4(%ebp),%eax */
    r_eax = s_m0x4;
  /* @80add2c: xor %eax,-0x18(%ebp) */
    s_m0x18 = (s_m0x18 ^ r_eax);
  /* @80add2f: mov -0x4(%ebp),%eax */
    r_eax = s_m0x4;
  /* @80add32: shl $0x8,%eax */
    r_eax <<= 8;
  /* @80add35: xor %eax,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c ^ r_eax);
  /* @80add38: mov -0x18(%ebp),%eax */
    r_eax = s_m0x18;
  /* @80add3b: shr $0x2,%eax */
    r_eax >>= 2;
  /* @80add3e: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80add41: and $0x33333333,%eax */
    r_eax = (r_eax & 0x33333333u);
  /* @80add46: mov %eax,-0x4(%ebp) */
    s_m0x4 = r_eax;
  /* @80add49: mov -0x4(%ebp),%eax */
    r_eax = s_m0x4;
  /* @80add4c: xor %eax,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c ^ r_eax);
  /* @80add4f: mov -0x4(%ebp),%eax */
    r_eax = s_m0x4;
  /* @80add52: shl $0x2,%eax */
    r_eax <<= 2;
  /* @80add55: xor %eax,-0x18(%ebp) */
    s_m0x18 = (s_m0x18 ^ r_eax);
  /* @80add58: mov -0x1c(%ebp),%eax */
    r_eax = s_m0x1c;
  /* @80add5b: shr $0x10,%eax */
    r_eax >>= 16;
  /* @80add5e: xor -0x18(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x18);
  /* @80add61: and $0xffff,%eax */
    r_eax = (r_eax & 0xffffu);
  /* @80add66: mov %eax,-0x4(%ebp) */
    s_m0x4 = r_eax;
  /* @80add69: mov -0x4(%ebp),%eax */
    r_eax = s_m0x4;
  /* @80add6c: xor %eax,-0x18(%ebp) */
    s_m0x18 = (s_m0x18 ^ r_eax);
  /* @80add6f: mov -0x4(%ebp),%eax */
    r_eax = s_m0x4;
  /* @80add72: shl $0x10,%eax */
    r_eax <<= 16;
  /* @80add75: xor %eax,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c ^ r_eax);
  /* @80add78: mov -0x18(%ebp),%eax */
    r_eax = s_m0x18;
  /* @80add7b: shr $0x4,%eax */
    r_eax >>= 4;
  /* @80add7e: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80add81: and $0xf0f0f0f,%eax */
    r_eax = (r_eax & 0xf0f0f0fu);
  /* @80add86: mov %eax,-0x4(%ebp) */
    s_m0x4 = r_eax;
  /* @80add89: mov -0x4(%ebp),%eax */
    r_eax = s_m0x4;
  /* @80add8c: xor %eax,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c ^ r_eax);
  /* @80add8f: mov -0x4(%ebp),%eax */
    r_eax = s_m0x4;
  /* @80add92: shl $0x4,%eax */
    r_eax <<= 4;
  /* @80add95: xor %eax,-0x18(%ebp) */
    s_m0x18 = (s_m0x18 ^ r_eax);
  /* @80add98: mov 0xc(%ebp),%eax */
    r_eax = (unsigned int)(size_t)a1;
  /* @80add9b: mov -0x1c(%ebp),%edx */
    r_edx = s_m0x1c;
  /* @80add9e: ror $0x18,%edx */
    r_edx = (r_edx >> 24) | (r_edx << 8);
  /* @80adda1: mov %edx,%ecx */
    r_ecx = r_edx;
  /* @80adda3: and $0xff00ff,%ecx */
    r_ecx = (r_ecx & 0xff00ffu);
  /* @80adda9: mov -0x1c(%ebp),%edx */
    r_edx = s_m0x1c;
  /* @80addac: ror $0x8,%edx */
    r_edx = (r_edx >> 8) | (r_edx << 24);
  /* @80addaf: and $0xff00ff00,%edx */
    r_edx = (r_edx & 0xff00ff00u);
  /* @80addb5: or %ecx,%edx */
    r_edx = (r_edx | r_ecx);
  /* @80addb7: mov %edx,(%eax) */
    *(unsigned int*)((char*)r_eax + 0) = r_edx;
  /* @80addb9: mov 0xc(%ebp),%eax */
    r_eax = (unsigned int)(size_t)a1;
  /* @80addbc: add $0x4,%eax */
    r_eax = (r_eax + 0x4u);
  /* @80addbf: mov -0x18(%ebp),%edx */
    r_edx = s_m0x18;
  /* @80addc2: ror $0x18,%edx */
    r_edx = (r_edx >> 24) | (r_edx << 8);
  /* @80addc5: mov %edx,%ecx */
    r_ecx = r_edx;
  /* @80addc7: and $0xff00ff,%ecx */
    r_ecx = (r_ecx & 0xff00ffu);
  /* @80addcd: mov -0x18(%ebp),%edx */
    r_edx = s_m0x18;
  /* @80addd0: ror $0x8,%edx */
    r_edx = (r_edx >> 8) | (r_edx << 24);
  /* @80addd3: and $0xff00ff00,%edx */
    r_edx = (r_edx & 0xff00ff00u);
  /* @80addd9: or %ecx,%edx */
    r_edx = (r_edx | r_ecx);
  /* @80adddb: mov %edx,(%eax) */
    *(unsigned int*)((char*)r_eax + 0) = r_edx;
  /* @80adddd: movl $0x0,-0x10(%ebp) */
    s_m0x10 = 0x0u;
  /* @80adde4: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80adde7: mov %eax,-0x14(%ebp) */
    s_m0x14 = r_eax;
  /* @80addea: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80added: mov %eax,-0x18(%ebp) */
    s_m0x18 = r_eax;
  /* @80addf0: mov -0x18(%ebp),%eax */
    r_eax = s_m0x18;
  /* @80addf3: mov %eax,-0x1c(%ebp) */
    s_m0x1c = r_eax;
  /* @80addf6: leave  */
  /* @80addf7: ret  */
}
