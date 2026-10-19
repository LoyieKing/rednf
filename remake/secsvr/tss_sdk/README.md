# TSS SDK / server shared-object reconstruction

Status: **server replacement deployed; the tested 1031 inventory session is
accepted, full semantic coverage is not claimed**.
This WSL directory is the authoritative server SDK source/build workbench.
The earlier Windows `dnf_client_remake/tools/tss_sdk_recon` copy is historical
and is no longer an implementation location.

`build_server.py` recompiles reconstructed/generated/reused sources and links
with `-Wl,-z,defs`, without loading ORIG to satisfy SDK symbols. It records
each compiler invocation and refuses failed compiles or unresolved imports.
The source-built library links and passes the audit; it was deployed on
2026-10-19.

`deploy_server.py` records runtime state, stops the two supervised game
channels, saves the previous library, atomically replaces the actual chroot
library target, and restarts the channels. `--rollback` restores the frozen
reference. Deployment has been exercised; `--rollback` has not.

The original SDK is preserved at `reference/libtss_sdk.so.1.0.394.orig`.
Its SHA-256 was checked against the deployed original before replacement:
`8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7`.
Baseline process/service mappings are in
`acceptance/20261019_server/baseline.json`.

Acceptance was exercised on 2026-10-19: independent relocation passed, both
game channels restarted and mapped the source-built library, and the original
1031 client completed the tested purchase/sort inventory flow. A login that
bypasses SDK processing still does not establish SDK correctness. Source
details, the online capture and remaining gaps are recorded in
`recon/README.md` and
`acceptance/20261019_server/rebuilt_inventory_session/`; coverage is not a
completion metric.

## Target

- `/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394`
- SHA-256 `8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7`
- 31,904,293 bytes; ELF32 i386 shared object; not stripped; DWARF present
- 7,307 defined symbols; 6,017 dynamic defined symbols
- Compiler: GCC 4.1.0 (SUSE Linux); several `Tencrypt` CUs list GCC 4.1.2
- Deployed 2026-10-19: source-built SHA-256
  `a9728bd68a48e068c3c29496206886bce16202b808de644aac2ca0ba21996882`
  (the original above remains the oracle and rollback image).

## What exists now

`gen_dwarf_stubs.py` (copied from `dnf_secsvr/toolchain`) produced **214 C++
translation-unit stubs** and **68 emitted headers** under
`build/tmp/tss_sdk_stubs`:

- 282 generated files, 4.2 MiB
- function signatures, source line numbers, parameter/local names
- empty function bodies; absolute CI include paths; **not buildable yet**

`manifest.json` records the target fingerprint, generator counts, critical
translation units, blockers and verification plan.

`dwarf_full_extract.py` extracts class/struct layouts from DWARF for one TU. For
example `dpinterface.cpp` yields `DPInterface` (12 bytes: `is_inited_`,
`msg_handler_`, `hack_manager_`) and `tss_sdk_impl::UserInfo` (400 bytes) with
member offsets.

`gen_headers.py` (also from `dnf_secsvr/toolchain`) emitted **492 headers** with
794 types, 800 function declarations and 639 variables, including:
`dpinterface.h`, `dp_msg_packer.h`, `user_info.h`, `antibotinterface.h` and
`cryptdata.h`. They still carry absolute CI include paths and some anonymous-
struct namespace artifacts; normalization is the next build gate.

## Source reuse audit

129 of 214 CU basenames have candidates in
`/home/loyieking/dnf_workspace/dnf_secsvr`; 85 have no source file anywhere under
`dnf_workspace`. The latter are TSS-SDK-specific units such as
`antibotinterface.cpp`, `dpinterface.cpp`, `cryptdata.cpp`,
`tss_sdk_predefine.cpp` and the `dp_*` plugin family. The matched 129 still need
per-file identity checks (the server source tree is evidence, not a proven
bit-identical source drop).

## Critical units

- `src/formmog/tss_sdk/plugins/proto_proc/dpinterface.cpp`
- `src/formmog/tss_sdk/plugins/proto_proc/dp_msg_packer.cpp`
- `src/formmog/tss_sdk/plugins/proto_proc/dp_msg_handler.cpp`
- `src/formmog/tss_sdk/plugins/proto_proc/dp_code_mgr.cpp`
- `src/formmog/tss_sdk/plugins/proto_proc/dp_config.cpp`
- `src/formmog/tss_sdk/plugins/user_control/user_info.cpp`
- `src/VMCore/cryptdata.cpp`
- `src/antibotinterface.cpp`

`DPInterface` has three function bodies relevant to the client protocol:
`recv_buffer_for_encrypt`, `recv_buffer_for_decrypt`, `on_recv_msg`;
`DPVMBaseEngine` owns `encrypt`/`decrypt` and the VM/code managers.

## Reproduce

```sh
# in WSL, or copy the ELF to Windows and use a Python with pyelftools
python gen_dwarf_stubs.py libtss_sdk.so.1.0.394 build/tmp/tss_sdk_stubs
python dwarf_full_extract.py libtss_sdk.so.1.0.394 dpinterface.cpp out.json
```

## Next gates

1. Normalize generated includes and emit class headers for the critical types.
2. Reuse/verify matching `dnf_secsvr` sources; do not claim identity from basename.
3. Reconstruct the 85 TSS-SDK-specific CUs in dependency order, beginning with
   `user_info`/`dp_config`/`dp_code_mgr`, then crypto (`cryptdata`, `VMCore`),
   then `dpinterface` and message packing.
4. Compare symbols, relocations, disassembly and live oracle behavior; only then
   claim a function is identical.
