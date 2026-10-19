
## 2026-10-19 client-state repair review

This review started from two observed client failures after source-built SDK
deployment: the character page was unusable, then purchases and sorts did not
refresh inventory state. The earlier layout/static-init PASS was retracted and
the original/rebuilt packet paths were exercised again.

Confirmed repairs:

- `default_user_id_crc` is dynamically initialized from `DEFAULT_USR_ID`
  (`0x00bc614e`) and equals `0x5beab0b1`, matching ORIG
  `0x000bed93..0x000beda9`. This fixed the character-selection state; the user
  subsequently confirmed entry into the game.
- `CMars::Mars` now processes a non-16-byte tail through `Seattos` as ORIG
  `0x001f03d2..0x001f0402`. The old code left the final 9 bytes of the first
  purchase `UPDATE_ITEM_LIST(14)` encrypted, yielding CRC `0xdbff7803` instead
  of `0xc2205564`.
- Cast, D3des161, Feal8, Shift, Blowfish 8/16/24/32/56, DesNew and Gramary now
  use their exact ORIG tail processor, base pointer and key-size arguments.
- `DPVMBaseEngine::fix_encrypt` reserves `buf_len + 0x11a`, not `buf_len + 17`;
  `DPClientStatPtc` compares `fix_encrypt_count_`, not the dynamic count.
- `ZEN_Bus_MMAPPipe` uses the 96-byte LFS `stat` ABI expected by the reused
  `ZEN_OS` wrappers and reproduces ORIG's create/resume/attach copy direction.
  The old 88-byte caller let the 96-byte callee overwrite saved registers.
- Every original CU's independent status/iostream initialization effects were
  restored; counter `TSSSDK_ANTIBOT_LOST_PKG_NUM` is `430009`.

Executed evidence after the final rebuild:

- release/debug: `compiled=228 failed=0`, `link_returncode=0`;
- 33 captured ORIG DP packets: decrypt output and CRC equal; the 33 recovered
  inner packets re-encrypt byte-for-byte equal with deterministic randomness;
- those rebuilt outputs passed the original client `TerSafe.dll` consumer:
  `check30=1` for 33/33;
- differential cipher matrix: captured 1, `Cipher` 15,330, `CCryptData` 11,826,
  direct wrappers 11,388 cases, zero mismatches; both directions, remainder and
  large lengths, varied keys, pre/post canaries;
- `fix_encrypt` capacity guard: 602/602 equal;
- startup IPC/status lifecycle: ORIG/rebuilt rc 0, zero failures/canary damage;
- handler timer/stat probe: 276 checks, zero failures, both negative controls
  detected; send-tail probe: 28 checks, zero failures, mutant detected;
- DP config oracle: pure 694 and full 270 result lines PASS;
- strict audit: all hard categories PASS, including five per-CU stat/stat64 ABI
  comparisons and 119/119 static-init mappings. Shape/guard notes remain in the
  generated report and are not silently promoted to exact code generation.

The rebuilt library was then deployed explicitly (SHA-256
`a9728bd68a48e068c3c29496206886bce16202b808de644aac2ca0ba21996882`) and the
user performed two purchases and an inventory sort with the original 1031
client. The user confirmed that all operations succeeded and the inventory
refreshed. The captured session (`rebuilt_inventory_session`, pcap SHA-256
`bb8bba3d9b8919518e904e9b0d10174339485ca8b4fa13e4593476b95e2fc8ca`) contains
34 DP561 frames; all pass CRC, original and rebuilt decryption agree, and the
inventory operations are followed by eight `UPDATE_ITEM_LIST(14)` frames and a
full `ITEM_LIST(13)` after the sort. See
`acceptance/20261019_server/rebuilt_inventory_session/acceptance.json`.

This verifies the tested online inventory flow, not full SDK semantic
equivalence. Dynamic decrypt, untested error paths and other SDK behaviours
remain outside this acceptance.
