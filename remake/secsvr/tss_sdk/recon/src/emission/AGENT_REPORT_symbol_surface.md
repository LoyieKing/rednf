# Symbol-surface emission — crypto static data, Comm_App_Frame instantiations, zen primes, hex_to_str

Workbench: `tools/tss_sdk_recon` (WSL `/mnt/d/Docs/dnf_client_test/dnf_client_remake/tools/tss_sdk_recon`).
Target: `/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394` (ELF32 i386, GCC 4.1.0 build).
Toolchain for the emission: `sh c5build.sh` (vendor GCC 4.1.2, `-m32 -O2 -fPIC -fno-implement-inlines -std=gnu++98`).

## Files created

| file | contents |
| --- | --- |
| `recon/src/emission/crypto_static_data.h` | 7 cipher classes with the 45 missing static data members, ORIG DWARF types, one private `assert_size_<member>` typedef per member |
| `recon/src/emission/crypto_static_data.cpp` | the 45 definitions, bytes read from the ORIG ELF; `crypto_static_data_dump()` under `-DEMISSION_DUMP_CRYPTO` |
| `recon/src/emission/comm_app_frame_instances.cpp` | the 25 `Comm_App_Frame::appdata_encode/decode<T>` explicit instantiations |
| `recon/src/emission/sec_proto_surface_models.h` | the 5 `sec_proto` models with no header anywhere in the reconstructed trees (size-asserted) |
| `recon/src/emission/zen_hashtable_prime_instances.cpp` | the 2 `ZEN_LIB::shm_hashtable_expire<...>::get_next_prime(unsigned int)` instantiations |
| `recon/src/emission/zen_string_util_hex.cpp` | `hex_to_str` + its function-local static `tag` |
| `recon/src/emission/AGENT_REPORT_symbol_surface.tsv` | 76 `EMITTED` rows + 133 `MISSING` rows (per-symbol status, ORIG address/size, our size, binding, whether it was in the current missing list, owning emission CU) |

Scratch tooling (not deliverables) lives in `build/emission/`: `gen_crypto_data.py`, `cmp_crypto_bytes.py`, `check_crypto_dump.py`, `run_crypto_dump.zsh`, `gen_diff_table.py`, `diff_harness.cpp`, `run_diff.zsh`, `check_emission_syms.zsh`, `gen_tsv.py`, `verify_all.zsh`.

## 1. Crypto static data — 45 symbols

Types are the ORIG DWARF types of the defining CUs (`src/des2p.cpp`, `desbig.cpp`, `desmo.cpp`, `despc.cpp`, `destoo.cpp`, `fastdes.cpp`, `gost.cpp`; the same declarations appear in the unity CU `src/TenCrypt.cpp`), extracted with `build/emission/findvar2.py`; e.g. `CDes2p::FUCKING_P2_E` is `const unsigned char[48]`, `CDesBig::FUCKING_SX_TAB` `const unsigned int[8][256]`, `CFastDes::hKS_C4` `const unsigned int[7][16]`, `CFastDes::wL_I8` `const unsigned int[86]`, `CGost::FUCKING_WZ_SP` `const unsigned char[8][16]`. Every declared size equals the `nm -DCS` size of the ORIG symbol (the generator aborts otherwise) and each class carries a compile-time `assert_size_<member>` typedef, so a drift fails the build instead of silently emitting a wrong-sized object.

Bytes were read from the ORIG ELF at the symbol address (section per symbol recorded in the `.cpp` header comments) and written as integer literals; the arrays are little-endian on i386, so the image is reproduced exactly. 20 080 bytes total.

Verification (all executed):

```
sh c5build.sh recon/src/emission/crypto_static_data.cpp /tmp/emission_crypto.o   # rc=0
python3 build/emission/cmp_crypto_bytes.py <obj>   ->  identical=45 mismatched=0
zsh build/emission/run_crypto_dump.zsh             ->  records=45 identical=45   # runtime dump vs dd(ORIG)
```

`cmp_crypto_bytes.py` compares name, ORIG size and the raw bytes of the object's section against ORIG. `run_crypto_dump.zsh` compiles the TU with `-DEMISSION_DUMP_CRYPTO` with the vendor compiler, links it (host 32-bit linker, because the c5root chroot has no `/lib/libc.so.6`), runs it and compares each record against `dd`-read ORIG bytes.

## 2. `Comm_App_Frame::appdata_encode/decode<T>` — 25 symbols

25 of the 32 ORIG instantiations were missing; the other 7 (`RecvDataFromSDKReq`, `SendToSDK`, `ReportPkgTime`, `UserInputCheckPkg`, `DelUserReq`, `AddUserReq`, `PunishClientReq`) were already emitted by the plugin CUs. The template body is the reused `comm_app_frame.h` (identical in the gunnersvr/secagent trees; the reuse object was built from the gunnersvr copy) — it only uses `LEN_OF_APPFRAME_HEAD = 0x32`, `frame_length_` (+0x00) and `frame_appdata_` (+0x32), so the instantiations are byte-identical for either copy. The ORIG addresses of all 25 are recorded per instantiation in the file.

**Header choice.** The ORIG protocol headers are not in any reconstructed tree, so the closest reconstructions were used and each layout was checked against ORIG DWARF (`build/emission/typedefs.py`):

* `src/protocol/common/comm_proto_public_cfgsvr.h` — `CfgFileReq`(154), `CfgFileRsp`(64162), `CfgsvrInfoRsp`(16), `CheckCfgFileUpdateReq`(294922), `CheckCfgFileUpdateRsp`(262148), `DeployInfoReq`(6), `DeployInfoRsp`(114), `SvcIdReq`(4), `SvcIdRsp`(22), `SvcIpInfoReq`(6), `SvcIpInfoRsp`(20) — all sizes verified with a compile-only static-assert probe (`build/emission/probe_vendor_layout.cpp`, rc=0);
* `src/protocol/common/comm_proto_public_logsvr.h` — `LogSvrCommBillRecord`(2048);
* `src/protocol/formmog/formmog_proto_public_secsvr.h` — `ReportErrReq`(8), `ReportLostAntibotPkg`(12), `ReqSecsvrForwardData`(32773);
* `src/protocol/formmog/formmog_proto_public_tsssdk.h` — `DPCaptchaResultRsp`(5), `DpBanModeReq`(264), `DpDropCmdlistReq`(808), `SafemodeLimitStatusRsp`(1038), `SafemodeStatusRsp`(42);
* `sec_proto_surface_models.h` — `LATransAdaptDPCmdMonitor`(14), `LATransAdaptDPCmdBufferMonitor`(32786), `ReqSafemodeTransAdaptTradeNotify`(2073), `ReqUICTransAdaptMsgForwardPkg`(33044), `SecsvrCaptchaReq`(541).

The five modelled types have **no** header in any tree: the four formmog ones exist only as per-CU reconstructions (`recon/src/origsrc/formmog_proto_public_{latransadapt,safemodetransadapt,uictransadapt}.h`) whose family-private `tsf4g_tdr` compatibility headers are mutually exclusive inside one TU (`tsf4g_tdr_compat_latrans.h` / `_client.h` each define `tsf4g_tdr::TdrError`/`TdrWriteBuf` under their own guard), and `SecsvrCaptchaReq` exists only inside `formmog_proto_public_secsvr_extra.cpp`. Their layouts were copied verbatim from those files and are asserted against the ORIG DWARF sizes in the header. No type is declared twice in the TU.

Verification:

```
sh c5build.sh recon/src/emission/comm_app_frame_instances.cpp <out.o> \
    -I /home/loyieking/dnf_workspace/dnf_secsvr/source/secagent          # rc=0
nm --defined-only <obj> | grep -c appdata_                               # 25, all 'W'
zsh build/emission/run_diff.zsh   ->  differential checks=157 failures=0
                                      (encode ok=71 err=13; decode ok=32 err=12)
```

`diff_harness.cpp` dlopens ORIG (`RTLD_GLOBAL`) and the emission `.so`, then for every instantiation compares ORIG's and our function on identical 64 KiB frame buffers and identical protocol objects: return value, size code and the whole buffer (encode, `cap ∈ {0xffbd,0x100,16}`, `data_start ∈ {0,7}`), and for the 11 decode-only instantiations both a valid payload produced by ORIG's own `sec_proto::<T>::pack` and a deliberately invalid payload. `sec_proto::<T>::construct/pack` are dlsym'd from ORIG, so both sides marshal identical objects. Code size differs by ±1 byte from ORIG (133/134 encode, 110/109 decode) — GCC 4.1.2 vs the ORIG's 4.1.0, no behavioural difference.

## 3. `ZEN_LIB::shm_hashtable_expire<...>::get_next_prime(unsigned int)` — 2 symbols

Emitted from the real reconstructed zenlib header `src/commlib/zenlib/zen_shm_hash_expire.h` (secagent tree). `get_next_prime` only reads its argument and the static prime table, so the six `tss_sdk_impl` template arguments are declared incomplete in the TU — no layout is invented and the mangled names come out exactly as ORIG's:

```
_ZN7ZEN_LIB20shm_hashtable_expireIN12tss_sdk_impl16UicUserInputInfoEyNS_9smem_hashIyEENS1_21UicExtractUserInfoKeyESt8equal_toIyENS1_14UicTimeOutProcEE14get_next_primeEj
_ZN7ZEN_LIB20shm_hashtable_expireIN12tss_sdk_impl8UserInfoEyNS_9smem_hashIyEENS1_15ExtractUserInfoESt8equal_toIyENS1_11TimeOutProcEE14get_next_primeEj
```

Both are `W` in ORIG (0x000ca0f0, 0x000cf210) and `W` here (0x3b bytes vs ORIG 0x3f). The ORIG disassembly was checked against the reconstructed body: linear scan over a 70-entry (`cmp eax,0x46`) table, `table[i] >= num -> return table[i]`, otherwise `return 0xfffffffb` = `(size_t)-5` — exactly the header's `num_primes_list` loop, so no body reconstruction was needed.

Divergence worth recording: our copies of the function-local static table (`...get_next_primeEjE15num_primes_list`) are emitted as weak objects (`V`), while ORIG does not export them at all (`nm <orig> | grep num_primes_list` = 0 matches; ORIG addresses the table GOT-relative inside the function). They are an artefact of the vague linkage of an explicitly instantiated member of a class template, not a behaviour difference; if a strict "no new exported symbols" rule is applied, this is the one place where the emission adds two weak data symbols.

## 4. `hex_to_str`

ORIG: `_Z10hex_to_strPKhjPcj`, weak, 0x000a2be0, 0x82 bytes, from `zen_string_util.h:481` (CU `dp_bill_adapter.cpp`), plus its local static `_ZZ10hex_to_strPKhjPcjE3tag` (4 bytes, `V`, `.data`) which `dlopen`+`dlsym` confirms points at `"0123456789ABCDEF"`.

Body reconstructed from the ORIG disassembly (instruction-by-instruction comment in the file): capacity check `2*buf_len + 1 > dest_len -> return -1` (unsigned), then `dest[2i] = tag[buf[i] >> 4]`, `dest[2i+1] = tag[buf[i] & 0xf]`, then `dest[2*buf_len] = 0`, `return 0`; the ORIG re-loads `tag` on every iteration, which the source reproduces by indexing the static directly. It is written as `inline __attribute__((used))` because that is what gives the ORIG bindings: function `W`, local static `V` (a plain `__attribute__((weak))` definition leaves the static local, and `inline` alone would emit nothing under `-fno-implement-inlines`). Our code is 0x85 bytes vs ORIG's 0x82.

Verification: the hex matrix of the differential harness — buffer lengths 0, 1, 2, 3, 15, 16, 17, 255, 256 × destination capacity exact / capacity−1 / generous, with 16-byte sentinel fences on both sides of the destination — 29 cases, all identical (return value and every byte of the fenced region, so the capacity−1 case also proves neither side writes out of bounds).

## 5. Residual missing list — 133 names

`/tmp/fl_missing.txt` is regenerated by other agents and is not stable: it held 454 names at the start of this session and 196 when this report was written (its `/tmp` files are owned by another user in this container — `full_link.sh` itself failed with `Permission denied` on `/tmp/fl_have.txt`, `/tmp/fl_orig.txt` and `/tmp/tss_full_all.o`, and `recon/link_check.sh` failed on `/tmp/recon_src_tdirty_oidirty.cpp.o`; neither file was edited, per the assignment). The residual below is therefore computed against the current list: 196 − the 63 names of that list which this emission provides = **133** (`/tmp/em_surface_residual.txt`, and the `MISSING` rows of the TSV). Of our 76 emitted symbols, 74 exist in ORIG's dynamic symbol table with the identical mangled name (the two exceptions are the `num_primes_list` statics of §3); the 13 emitted names absent from the current list are the 11 decode-only `appdata_decode` instantiations plus those two statics — i.e. the current list was produced from a link state that already covered them.

| n | residual group | why it is not emittable here |
| --- | --- | --- |
| 27 | `std::_Rb_tree<...>` member functions | Out-of-line copies of libstdc++ member templates instantiated by CUs that are not reconstructed yet; they can only be produced by a TU that actually uses the same `std::map`/`std::set` instantiation (the codegen depends on the owning CU's call sites and comparator objects). |
| 25 | `std::__uninitialized_fill_n_a<...>`, `std::fill<...>`, `std::fill_n<...>` | Same: emitted as a side effect of `std::vector`/`std::hash_map` growth in the owning CU (`ZEN_STATUS_ITEM_WITHNAME`, `BuddyBufDesc` …). A hand-written instantiation would have to reproduce the caller's allocator/iterator types; feasible follow-up, not a data-only emission. |
| 17 | `std::vector<...>` members (`_M_fill_insert`, `reserve`, …) | Same as above. |
| 10 | `__gnu_cxx::hashtable<std::pair<void* const, BuddyBufDesc*>, ...>` (`clear`, `erase`, `find_or_insert`, `resize`) | The `BuddyAlloc` hash map. The class type and `BuddyPointerHashFun` exist in `recon/src/commlib/comm_buddy_alloc.{h,cpp}`; these four members are simply not forced by the reconstruction, so the fix is to add explicit instantiations in that CU (or a sibling TU that includes the same header) — outside this assignment's five deliverables. |
| 9 + 4 + 2 | `__gnu_cxx::__normal_iterator<...>`, `__gnu_cxx::_Hashtable_node<...>`, `std::list<...>` | Same class of libstdc++ side-effect symbols as the first row. |
| 2 | `guard variable for Singleton<...>::object_creator` | Explicitly out of scope: the guard exists only because ORIG runs an unknown dynamic initializer for `Singleton<T>::object_creator`; emitting the guard without that initializer (or with an invented one) would be fabrication. |
| 3 + 2 | `vtable for tss_dpsdk::IDPHacker`, `vtable for IAntibotInterface`, `vtable for CVMBase`, `typeinfo`/`typeinfo name for IAntibotInterface` | A vtable is emitted with its key function; the key functions of these classes live in CUs that are not reconstructed, and the vtables reference the not-yet-emitted slots. |
| 5 + 3 | `tss_dp_comm::{do_reload_dp_config, reload_dp_config, init_dp_config, get_comm_mem, get_cmd_id}`, `tss_dp_comm::DP_CONFIG::glb_config_*` | The `dp_config` CU itself is not reconstructed yet (a peer is working on it); the statics need that CU's initializers. |
| 4 | `cmp_dpsubcmdlist`, `cmp_uint32`, `ZEN_Sockaddr_In::get_host_addr() const`, `tss_sdk_release_busi_interf`/`tss_get_sdk` | Function bodies of not-yet-reconstructed CUs; nothing to emit without writing the bodies (a different work item). |
| 4+ | `dea_b`, `dea_er`, `dea_ksp`, `dea_ls`, … | Data of the Tencrypt `dea_*` globals: the same byte-extraction technique as §1 applies, but they belong to cipher classes outside the seven listed in this assignment. |
| 5 | `__bss_start`, `_edata`, `_end`, `_init`, `_fini` | Linker-provided symbols of the final shared object; no relocatable recon object can define them. |

All "why" statements above are analysis of the symbol names plus the reconstructed tree; only the counts, the sizes and the test results in §1–§4 are executed evidence.

## Commands to integrate and re-verify

```sh
# in tools/tss_sdk_recon (WSL)
sh c5build.sh recon/src/emission/crypto_static_data.cpp         /tmp/emission_crypto.o
sh c5build.sh recon/src/emission/zen_string_util_hex.cpp        /tmp/emission_hex.o
sh c5build.sh recon/src/emission/comm_app_frame_instances.cpp   /tmp/emission_appdata.o \
   -I /home/loyieking/dnf_workspace/dnf_secsvr/source/secagent
sh c5build.sh recon/src/emission/zen_hashtable_prime_instances.cpp /tmp/emission_primes.o \
   -I /home/loyieking/dnf_workspace/dnf_secsvr/source/secagent

# either add the four objects to recon/link_check.sh's object list, or merge them:
ld -r -o /tmp/tss_recon_all_plus.o /tmp/tss_recon_all.o \
   /tmp/emission_crypto.o /tmp/emission_hex.o /tmp/emission_appdata.o /tmp/emission_primes.o
bash full_link.sh /tmp/tss_recon_all_plus.o /tmp/tss_full_all.o

# byte + behaviour verification
python3 build/emission/cmp_crypto_bytes.py /tmp/emission_crypto.o
zsh build/emission/run_crypto_dump.zsh
zsh build/emission/run_diff.zsh          # 157 differential checks
zsh build/emission/check_emission_syms.zsh /tmp/emission_*.o
```
