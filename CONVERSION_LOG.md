# Conversion log

Game code is `0x401000`–`0x61c4c0` (2,208,961 bytes); MFC, CRT and other libraries are excluded.
Earlier small-procedure batches are in the git history.

| Batch | Commit | Converted | Size | Game code in C++ |
|---|---|---|---:|---:|
| start | | | | 43.00% |
| 1 | `bb571ad` | `Dd_ErrorText`, `Light_ReachesSphere` | 8,704 | 43.67% |
| 2 | `37de692` | `Hud_DrawLights`, `Hud_DrawOverlays` | 6,896 | |
| 3 | `3ae2540` | `Game_StartWorld`, `Game_CreateObject` | 5,744 | 43.93% |
| 4 | `fa15440` | 13 number natives | 8,688 | 44.32% |
| 5 | `ab3d130` | 49 number, pair, symbol and char natives | 26,864 | 45.54% |
| 6 | `cb97d56` | 31 string, vector and port natives, `Sch_Construct` | 23,200 | 46.59% |
| 7 | `f3126ae` | `Reader_Read`, `Sch_Write`, `Grd_DxErrorName`, `Player_Save`, `Player_Load`, 8 list natives | 22,288 | 47.60% |
| 8 | `3a916f2` | `CopterPanel_Draw`, `Road_FindPositionRoute`, `Ai_CreatePlayersRegular`, `CopterScreen_Start`, `NetDialog_Draw`, `Road_FindRoute` and 13 more | 44,310 | 49.62% |
| 9 | `586e7e8` | 22 shell screens, the game menu, dealers, video options, texture stages and upload, route costs, player creation and swap, weather, save reading, grid refinement | 44,560 | 51.64% |
| 10 | `b0f4423` | 36 procs: menus and shell screens, video open, effects and particle pools, the interpreter run loop, `-` and `eqv?`, moving-object bases, camera matrices, texture surfaces, bitmap text, rectangle fill and image tiling, AI net messages and save/load pieces | 44,576 | 53.66% |
| 11 | | 44 procs: big-integer add and multiply, the number and list readers, set!, call/cc, env define and set, RLE image drawing, scroll bars, texture probes, the DirectDraw display, TV cameras, weather update, vehicle destructor, history and network packing, dashboard, joystick open, AI hiring, saves and loads, STL sorts and inserts | 44,695 | 55.68% |

Tests per batch: C++ build, then one new-game drive (batches 1-10 also ran a second drive and a load-game run). `Player_Save` is not covered (no save is written).

Batch 8 plan (target over 2% = 44,180 bytes; 19 procs, 44,678 bytes = 2.02%; game code in C++ 47.60% -> 49.62%, left in MASM 52.40% -> 50.38%): `FUN_56bc80` 3,904, `FUN_4858b0` 3,888, `FUN_5a7b50` 3,488, `$L_422f20` 2,832, `FUN_4ebd00` 2,656, `FUN_5dc4d0` 2,614, `FUN_484c80` 2,560, `FUN_4927d0` 2,384, `FUN_56e5a0` 2,304, `FUN_5e6e90` 2,208, `$L_4a77f0` 2,192, `$L_51c0a0` 2,000, `FUN_5cfb30` 1,920, `FUN_4e35cf` 1,840, `$L_4a9090` 1,744, `$L_4aee50` 1,584, `FUN_482d20` 1,536, `FUN_4a5db0` 1,520, `FUN_4b20d0` 1,504.

Batch 8 done: `FUN_4ebd00` (the vehicle constructor) was swapped for `$L_519800` (2,288, the network dialog), because its code continues past ENDP through x87-heavy split pieces. 19 procs, 44,310 bytes = 2.01%; 47.60% -> 49.62%. Drives and the load test passed.

Batch 9 plan (target over 2% = 44,180 bytes; 22 procs, 45,280 bytes = 2.05%; game code in C++ 49.62% -> 51.67%, left in MASM 50.38% -> 48.33%; game tests once after all are converted): `$L_521430` 2,512, `$L_422400` 2,400, `FUN_56b0a0` 2,384, `FUN_4daf40` 2,352, `FUN_5eb7a0` 2,320, `FUN_486800` 2,272, `FUN_4738c0` 2,208, `$L_525250` 2,176, `FUN_47a230` 2,160, `FUN_53d9d0` 2,144, `$L_5220c0` 2,080, `$L_5162f0` 2,080, `FUN_5b0810` 2,064, `$L_511c50` 2,064, `FUN_605900` 2,048, `FUN_51c980` 1,888, `FUN_5a7430` 1,824, `FUN_524b30` 1,824, `FUN_531290` 1,696, `FUN_4f6cf0` 1,680, `FUN_488a90` 1,568, `FUN_5c1d40` 1,536.

Batch 9 done: `FUN_53d9d0` (2,144, a dashboard text overlay) was swapped for `FUN_5bb660` (1,424, texture upload), because it draws five text lines from stack buffers nothing fills (uninitialized stack). 22 procs, 44,560 bytes = 2.02%; 49.62% -> 51.64%. Built after every proc, tested once at the end: two drives and the load test passed. Also fixed in batch 8's `CopterScreen_Start`: `Camera_Get` takes one argument; the 0 pushed before it is `FUN_5975e0`'s third.

Batch 10 plan (target over 2% = 44,180 bytes; 36 procs, 44,576 bytes = 2.02%; game code in C++ 51.64% -> 53.66%, left in MASM 48.36% -> 46.34%; game tests once after all are converted): `FUN_5c13a0` 1,520, `FUN_56d810` 1,488, `FUN_50c9d0` 1,488, `FUN_4b9570` 1,488, `FUN_517eb0` 1,456, `FUN_50f990` 1,456, `$L_526c90` 1,456, `FUN_5e7930` 1,440, `FUN_51add0` 1,408, `$L_5d8d10` 1,408, `FUN_4a6af0` 1,392, `FUN_5a37f0` 1,360, `FUN_4729e0` 1,328, `FUN_532430` 1,312, `FUN_5b81c0` 1,280, `FUN_544920` 1,232, `FUN_5d0990` 1,200, `FUN_5b9020` 1,168, `FUN_4a63d0` 1,168, `FUN_480680` 1,152, `FUN_56d0f0` 1,136, `$L_491ce0` 1,136, `FUN_5dfa90` 1,120, `FUN_46d7f0` 1,120, `FUN_5ae0e0` 1,104, `FUN_5975e0` 1,088, `FUN_519300` 1,088, `FUN_4c0e30` 1,088, `FUN_5bae30` 1,072, `FUN_57d060` 1,072, `FUN_57cb70` 1,072, `FUN_481b20` 1,072, `$L_4954f0` 1,072, `FUN_5ea1b0` 1,056, `FUN_5be8c0` 1,040, `FUN_5b6f90` 1,040.

Batch 10 done: all 36 planned procs, 44,576 bytes; 51.64% -> 53.66%. Built after every proc, tested once at the end. The first test hung on the loading screen and then crashed with an access violation in `FUN_57d860`: `FUN_4011e0` (`_Array_ForEach@16`) is stdcall, but the new moving-object constructors and the effects pools called it through `CallC`, so the stack was popped twice. `CallStd` fixed it; the drives and the load test then passed. Check every new callee's convention (`_X@N` is stdcall, `@X@N` is fastcall) before choosing `CallC`. `Load_Present` (`FUN_481b20`) and `Ai_NetMessage` (`FUN_46d7f0`) are not covered by the tests (no presents in the save, no network game).

Batch 11 plan (target over 2% = 44,180 bytes; 42 procs, 45,036 bytes = 2.04%; game code in C++ 53.66% -> 55.70%, left in MASM 46.34% -> 44.30%; the largest remaining procs with at most 30 x87 instructions; game test once after all are converted: one new-game drive with steering and braking): `FUN_5b8980` 1,696, `$L_524530` 1,536, `FUN_5787a0` 1,456, `FUN_4d8b20` 1,440, `$L_51f760` 1,409, `FUN_4890b0` 1,280, `$L_527240` 1,275, `FUN_4a3180` 1,232, `FUN_543e00` 1,188, `FUN_4b1720` 1,136, `$L_4d76b0` 1,115, `FUN_5443e0` 1,108, `FUN_5a6420` 1,088, `FUN_4b9b90` 1,040, `$L_4a8c20` 1,040, `FUN_5d1030` 1,024, `FUN_4754d0` 1,024, `$L_5bf340` 1,024, `FUN_5acd40` 1,008, `$L_578290` 1,008, `$L_526080` 1,008, `FUN_596470` 992, `FUN_47cf50` 992, `FUN_4d8350` 977, `FUN_544df0` 976, `FUN_541e80` 976, `FUN_46f0a0` 976, `FUN_56fc10` 960, `FUN_555550` 960, `FUN_4b2e90` 944, `FUN_48afa0` 944, `FUN_476bc0` 944, `$L_51a0f0` 944, `$L_4a9ac0` 944, `FUN_4878b0` 940, `FUN_50d860` 928, `$L_576830` 928, `$L_429930` 928, `FUN_5de0a0` 912, `FUN_53d530` 912, `FUN_481790` 912, `FUN_45f2c0` 912.

Batch 11 plan revised: six procs are split (their code jumps to labels past ENDP) and are swapped out: `$L_51f760`, `$L_527240`, `FUN_543e00`, `$L_4d76b0`, `FUN_5443e0`, `FUN_4d8350`. In: `FUN_554710` 896, `FUN_4a3ac0` 896, `FUN_48fa90` 896, `FUN_489c30` 891, `FUN_5d6e40` 880, `FUN_48b350` 864, `FUN_480b00` 864, `FUN_5b04c0` 848. Now 44 procs, 44,999 bytes = 2.04%; 53.66% -> 55.70%.

Batch 11 done: 44 procs, 44,695 bytes; 53.66% -> 55.68%. Two more swaps while converting: `FUN_5a6420` (stack slots reused across pushes in ways that make float bits act as counters) for `$L_596f80`, and `FUN_48fa90` (recursive B-spline basis with 580-byte polynomial temporaries) for `FUN_4dd2a0`. Inner functions converted with their PROCs: `$L_5b8fb0` (Gfx_MarkPixel, in FUN_5b8980) and `$L_4d8b60` (Present_OnEvent, in FUN_4d8b20), wired by hand with EXTERN / TEXTEQU. `$L_429930` passes `FUN_429760` the count the original `FUN_429340` left in ECX. The info screen's network setup buffer is now zeroed before sending (the original sent stack bytes). The first test crashed in the road file reader: `List489_Insert` takes the end node by value, not its address; the drive test then passed.

Batch 12 plan (target over 2% = 44,180 bytes; 60 procs, 44,627 bytes = 2.02%; game code in C++ 55.68% -> 57.70%, left in MASM 44.32% -> 42.30%; the largest remaining procs with at most 30 x87 instructions, split procs screened out; one new-game drive test after all are converted): `FUN_4b32d0` 832, `FUN_4904e0` 832, `FUN_45fdd0` 832, `$L_4a8740` 832, `FUN_5dbf00` 816, `FUN_4a3790` 816, `FUN_46c4a0` 816, `FUN_469520` 816, `$L_49c230` 816, `FUN_546d10` 800, `FUN_4b3b10` 800, `FUN_5ea9b0` 784, `FUN_4b1d40` 784, `FUN_48f2c0` 784, `FUN_481330` 784, `FUN_5e29b0` 768, `FUN_5ae580` 768, `FUN_503ac0` 768, `FUN_4d5c50` 768, `FUN_47c9a0` 768, `FUN_465a20` 768, `FUN_4654c0` 768, `FUN_464fe0` 768, `$L_5d72a0` 768, `FUN_45b570` 760, `FUN_5b49e0` 752, `FUN_58ae30` 752, `FUN_48f730` 750, `FUN_5f8bf0` 736, `FUN_574530` 736, `FUN_4e0280` 736, `FUN_48c260` 736, `FUN_46f840` 736, `FUN_45bbc0` 736, `FUN_5c3c50` 720, `FUN_5465c0` 720, `FUN_489900` 720, `FUN_484810` 720, `FUN_470490` 720, `FUN_46fe40` 720, `FUN_4a1a40` 717, `FUN_5ed6b0` 704, `FUN_530d40` 704, `FUN_48d8f0` 704, `FUN_48bd80` 704, `FUN_47c0a0` 704, `FUN_46ede0` 704, `FUN_46ea10` 704, `FUN_5c5910` 688, `FUN_584570` 688, `FUN_4d90c0` 688, `FUN_482a70` 688, `FUN_47cca0` 688, `$L_56aa70` 688, `$L_4a8220` 688, `FUN_5c49b0` 672, `FUN_4eb770` 672, `FUN_48fe80` 672, `FUN_47eed0` 672, `FUN_47de60` 672.

Batch 12 done: 60 procs, 44,243 bytes = 2.00%; 55.68% -> 57.68%. Five planned procs were swapped out: `FUN_5e29b0` (falls through past its ENDP into the next code), `FUN_4d5c50` (by-value position structs passed to asm callees with long argument lists), `FUN_45bbc0` (an insertion sort with a by-value 24-byte comparator and 32-byte position copies), `FUN_530d40` (the truck model table, left in asm on purpose earlier) and `FUN_48d8f0` (built on the B-spline basis `FUN_48fa90` that batch 11 left in asm). In their place the rest of the 12- and 40-byte record sorts: `FUN_47f4b0` 656, `FUN_47dba0` 656, `FUN_470110` 656, `FUN_46f580` 656, and `FUN_479cb0` 672 (orders loader). `Waypoints_Sort` in ai_drivers.cpp was a raw alias of `FUN_47c0a0` and now names `Road40_StableSort`; `Dd_EnumDevice` in directdraw.cpp is now the converted `$L_5d72a0`. Built after every proc, tested once at the end: the new-game drive and the load test passed the first time (exit 0, HUD diff 2.6 at the end, the load test at 940 as before).

Batch 13 plan (target over 2% = 44,180 bytes; 75 procs, 45,169 bytes = 2.04%; game code in C++ 57.68% -> 59.72%, left in MASM 42.32% -> 40.28%; the next largest candidates with at most 30 x87 instructions; game tests once after all are converted): `$L_4a97c0` 672, `$L_52ebc0` 670, `$L_528820` 670, `FUN_531000` 656, `FUN_4c7d90` 656, `FUN_4b6b90` 656, `FUN_4a2ef0` 656, `FUN_470e10` 656, `FUN_46dc50` 656, `$L_53e510` 656, `$L_51fd70` 656, `FUN_5cc6b0` 640, `FUN_5bd2d0` 640, `FUN_5bcc60` 640, `FUN_578fe0` 640, `FUN_4de290` 640, `FUN_4a0870` 640, `FUN_46d570` 640, `$L_56dde0` 640, `$L_4a9ed0` 640, `$L_4e2225` 633, `FUN_4dde70` 631, `FUN_606760` 624, `FUN_5c5c30` 624, `FUN_587e20` 624, `FUN_52fdb0` 624, `FUN_52dc50` 624, `FUN_4d0bc0` 624, `FUN_4a6880` 624, `$L_506fc0` 624, `$L_56ce61` 623, `FUN_48a2c0` 608, `FUN_48a000` 608, `FUN_45e8f0` 608, `FUN_45d8f0` 608, `FUN_45d630` 608, `$L_4a7590` 608, `FUN_5ffdf0` 597, `FUN_609de0` 592, `FUN_5d7750` 592, `FUN_5c24a0` 592, `FUN_52fa70` 592, `FUN_471ec0` 592, `$L_5f3190` 592, `$L_549275` 587, `FUN_4aa460` 583, `FUN_584c70` 578, `$L_4ac6b5` 577, `FUN_5adea0` 576, `FUN_56f6d0` 576, `FUN_4b38d0` 576, `FUN_4a55c0` 576, `FUN_4731e0` 576, `$L_5f2800` 576, `$L_4df300` 576, `FUN_5301a0` 565, `FUN_584820` 562, `FUN_5cf6f0` 560, `FUN_5b4d40` 560, `FUN_585da0` 560, `FUN_4c8640` 560, `FUN_4b36a0` 560, `FUN_48e190` 560, `FUN_4833a0` 560, `FUN_47be40` 560, `FUN_476f70` 560, `FUN_45cf80` 560, `FUN_45c8d0` 560, `$L_58fbc0` 560, `$L_4e1f75` 557, `$L_56e11d` 545, `FUN_50bda0` 544, `FUN_4b1500` 544, `$L_5825a0` 544, `$L_5db79c` 535.

Batch 13 plan revised: 11 procs jump to labels past their ENDP (split) and are swapped out: `$L_52ebc0`, `$L_528820`, `$L_4e2225`, `FUN_4dde70`, `FUN_584c70`, `$L_4ac6b5`, `FUN_5301a0`, `FUN_584820`, `$L_4e1f75`, `$L_56e11d`, `$L_5db79c`. In: `FUN_5d2c00` 528, `FUN_4b5b70` 528, `FUN_48a7c0` 528, `FUN_4872f0` 528, `FUN_479aa0` 528, `$L_5a5400` 528, `FUN_600c60` 512, `FUN_5d1c50` 512, `FUN_46ab50` 512, `FUN_45e0a0` 512, `FUN_45dbb0` 512, `FUN_45b8b0` 512. Now 76 procs, 44,886 bytes = 2.03%; 57.68% -> 59.71%.

Batch 13 plan extended: while converting, 13 procs were swapped out as too tangled (see the done row); added to stay over 2%: `$L_5c0350` 512, `$L_4e0950` 512, `$L_4028f0` 512, `$L_5dd94d` 499, `FUN_60bbb0` 496, `FUN_601060` 496, `FUN_5eb4b0` 496, `FUN_5ead50` 496, `FUN_5e69a0` 496, `FUN_59a780` 496, `FUN_556a70` 496, `FUN_4b4c20` 496, `FUN_4a1710` 496, `FUN_4901b0` 496, `FUN_48b8a0` 496, `FUN_470ae0` 496, `FUN_45eba0` 496 (taking what is needed).

Batch 13 done: 77 procs, 44,549 bytes = 2.02%; 57.68% -> 59.70%. Swapped out while converting: `FUN_4d0bc0` (by-value copies into a 44-byte-argument init), `FUN_52fa70` (old iostream `ifstream` internals), `FUN_471ec0` (by-value ids and the STL lock), `FUN_4aa460` (44-byte frame insert with exception temporaries), `FUN_5cf6f0` and `FUN_5d1c50` (software span rasterizers), `FUN_50bda0` (mirrored blit with mixed strides), `FUN_48a7c0` (x87 register juggling), `FUN_4872f0` and `FUN_46ab50` (32-byte structs by value), `FUN_5eb4b0`, `FUN_5ead50` and `FUN_60bbb0` (left for later); `FUN_45e520` was added. Seven data words in the DirectInput format table at 64b988 named `$L_601100` (a label inside `FUN_601060`) now hold the constant 0x601100 itself, as the original bytes do. `AiStatic_ConstructOnce` called `$L_4028f0` through an `ASM_VAR`; it now calls `RopeHash_Destroy`. The message loader reads the end-of-file flag of the game's FILE (+12 bit 4) as the original does: the C runtime here has no `feof`. The network creation record and the leave-race message 954 are now zero-filled (the original sent stack bytes). Built after every proc, tested once at the end: two drives and the load test passed the first time.

Batch 14 plan: 60 procs of 400-900 bytes, 47,879 bytes (2.17%); 59.70% -> 61.87% (at least 44,180 bytes = 2% needed). Screened for split procs (jumps to labels outside the proc, last line not ret/jmp) and jump tables. `FUN_614960` 896, `FUN_601890` 896, `FUN_5f2a40` 896, `FUN_4dc070` 896, `FUN_48fa90` 896, `FUN_4dc410` 880, `FUN_4cb560` 880, `$L_59c830` 880, `$L_5fc360` 864, `$L_55de40` 864, `FUN_5af000` 848, `FUN_5a8e90` 848, `FUN_5a0ca0` 848, `FUN_545ef0` 848, `$L_5f4530` 848, `$L_4c3480` 848, `FUN_5ad180` 832, `FUN_53d1f0` 832, `FUN_4ccf30` 832, `$L_5f3a60` 832, `$L_4c2380` 832, `FUN_614570` 816, `FUN_601c70` 816, `FUN_5de950` 816, `FUN_5bb260` 816, `FUN_548ec0` 816, `FUN_615350` 800, `FUN_606d50` 800, `FUN_5f88d0` 800, `$L_582140` 800, `FUN_5b9850` 784, `FUN_5940f0` 784, `FUN_586830` 784, `FUN_506750` 784, `FUN_4f6860` 784, `FUN_5c7110` 768, `FUN_5a9240` 768, `FUN_557090` 768, `FUN_4d5c50` 768, `FUN_4ca4e0` 768, `$L_59dbc0` 768, `$L_59d1a0` 768, `$L_59cba0` 768, `$L_4e6e6a` 766, `$L_5112b7` 761, `FUN_4601c0` 752, `$L_583d40` 752, `$L_4dd610` 752, `FUN_60be40` 736, `FUN_608180` 736, `FUN_57d580` 736, `FUN_45bbc0` 736, `FUN_5b9b60` 720, `FUN_582960` 720, `FUN_567dc0` 720, `FUN_4da4e0` 720, `$L_4f3620` 720, `FUN_616ca0` 704, `FUN_614f70` 704, `FUN_5cdcd0` 704.

Batch 14 extended (7 split or too tangled procs swapped out, see the done row): `FUN_530d40` 704, `FUN_4be2c0` 704, `FUN_48d8f0` 704, `FUN_48ce40` 704 and further 400-720-byte candidates until 2% is reached.

Batch 14 done: 56 procs, 44,478 bytes = 2.01%; 59.70% -> 61.71%. Swapped out: `$L_5112b7` and `$L_4f3620` (tails of other procs, running on their callers' frames), `$L_55de40`, `$L_4c2380`, `FUN_5bb260` (x87 register juggling with by-value matrices, a trig texture), `FUN_4d5c50` (refcounted by-value records), `FUN_567dc0` (road-parameter records with unknown vtables); added `FUN_4be2c0`, `FUN_48d8f0`, `FUN_501b90`. `$L_5f3a60` holds a second entry `$L_5f3bc0` (called from asm): it became `Light_ReadOld`, with its TEXTEQU added by hand in the ELSE block. `$L_4e6e6a` is four unreferenced debug-built world queries whose runner always answers 0. The candidate screen now also rejects jump tables (it missed the `DWORD PTR` spelling). Tested once at the end: drive and load passed the first time.

Batch 15 plan: 88 procs of 380-720 bytes, 48493 bytes; 61.71% -> ~63.9% (at least 44,179 bytes = 2% needed). Same screen as batch 14 plus jump tables. `FUN_530d40` 704, `FUN_5ff5e0` 688, `FUN_5c6580` 688, `FUN_4cd770` 688, `$L_5fb9e0` 688, `FUN_6027b0` 685, `FUN_5eaf40` 672, `FUN_581430` 672, `$L_5fc780` 672, `$L_584eb2` 670, `FUN_586240` 656, `FUN_4fab20` 656, `FUN_5fd870` 640, `FUN_5fd5f0` 640, `FUN_5c6830` 640, `FUN_5a89a0` 640, `FUN_4d60c0` 640, `$L_59e050` 640, `$L_59bc50` 640, `FUN_5bcff0` 624, `$L_56ce61` 623, `FUN_57e2e0` 608, `FUN_549c00` 608, `FUN_536100` 608, `FUN_4f01b0` 608, `$L_4fbb7e` 594, `FUN_5c7dc0` 592, `FUN_52fa70` 592, `FUN_471ec0` 592, `FUN_46add0` 592, `$L_594400` 592, `FUN_4aa460` 583, `FUN_53b180` 576, `FUN_4691a0` 576, `FUN_5cf6f0` 560, `FUN_549960` 560, `FUN_4f09f0` 560, `$L_4d98c0` 560, `FUN_5864d0` 544, `FUN_581ea0` 544, `FUN_50bda0` 544, `FUN_464880` 544, `$L_5825a0` 544, `$L_584a52` 542, `FUN_567bb0` 528, `FUN_4872f0` 528, `FUN_5d1c50` 512, `FUN_4f4d60` 512, `FUN_46ab50` 512, `$L_56f450` 512, `$L_5dd94d` 499, `FUN_60bbb0` 496, `FUN_5eb4b0` 496, `FUN_5ead50` 496, `FUN_5ad780` 496, `FUN_5a4ca0` 496, `FUN_4f0800` 496, `FUN_4f0530` 496, `$L_5fb550` 496, `$L_5e1750` 496, `$L_58dff0` 496, `$L_4c41c0` 496, `$L_4b5f70` 496, `$L_458f20` 496, `FUN_5d79d0` 489, `$L_545367` 489, `FUN_600e60` 480, `FUN_5ff3b0` 480, `FUN_5dd0d0` 480, `FUN_5b9e30` 480, `FUN_4e10d0` 480, `FUN_4cf400` 480, `FUN_4b2ca0` 480, `FUN_45ed90` 480, `FUN_45e710` 480, `$L_5c4130` 480, `$L_5ca70c` 468, `FUN_479520` 467, `FUN_5d1a80` 464, `FUN_5c5740` 464, `FUN_5c2800` 464, `FUN_56f910` 464, `FUN_533340` 464, `FUN_4f0df0` 464, `FUN_488450` 464, `$L_59d590` 464, `$L_581060` 464, `$L_4a0af0` 464.


Batch 15 done: 84 procs, 44,425 bytes = 2.01%; 61.71% -> 63.72%. Many planned procs were swapped out as tails of other procs (`$L_584eb2`, `$L_56ce61`, `$L_4fbb7e`, `$L_584a52`, `$L_5dd94d`, `$L_545367`, `$L_5ca70c`, `$L_5e2cb0`) or too tangled (`FUN_5c6580`, `FUN_4cd770`, `FUN_5eaf40`, `FUN_5c6830`, `FUN_4d60c0`, `FUN_57e2e0`, `FUN_600e60`, `FUN_4cf400`, `FUN_5d1a80`, `FUN_4709b0` whose inner `$L_470ad0` is used elsewhere); added destructors of AI vectors and lists, the TGA reader `FUN_5e8160` (2816 bytes), the material selection `FUN_5b1020` (1824), the cell merge `FUN_60a460`, the span fillers `FUN_5cf480`, `FUN_5cf920`, `FUN_5cf6f0`, `FUN_5d1740`, `FUN_5d18d0`, `FUN_5d1c50`, `FUN_5d1e50`, the grid layout `FUN_5bdf80` and smaller ones. Hand fixes: `FUN_4e10d0` has inner entries `$L_4e1120`/`$L_4e1150` (TEXTEQUs added in the ELSE block); ASM_FUNC/ASM_VAR references to converted labels repointed by hand (`Copter_EnumCallback`, `g_rf4a0af0`, `g_rf493120`); two Msavi strings renamed for a name clash. The first test crashed in `Player_UpdateOrders`: `FUN_45cdc0` takes five arguments (the random state is the fifth), read wrongly from the ESP offsets; frame-relative dumps (`norm.py`) are now used for argument offsets. Then drive and load passed.

Batch 16 plan: 98 candidate procs of 250-2100 bytes with at most 60 x87 instructions, 51,278 bytes; 63.72% -> ~65.8% (at least 44,180 bytes = 2% needed, base 1,407,640). Same screen as batch 15; argument offsets read from frame-relative dumps (norm.py). `FUN_4bcd00` 2000, `FUN_5b73a0` 1872, `$L_4e29a6` 1423, `$L_520700` 1408, `$L_514d10` 1376, `FUN_4bd850` 1296, `$L_4279d0` 1296, `$L_4e3cff` 1242, `$L_432dd0` 1136, `$L_5c4f10` 1024, `FUN_5e58b0` 1008, `FUN_58b120` 976, `$L_4ce300` 960, `FUN_532950` 928, `FUN_4d5c50` 768, `FUN_5c6580` 688, `FUN_5eaf40` 672, `FUN_4d0bc0` 624, `$L_56ce61` 623, `FUN_471ec0` 592, `FUN_52fa70` 592, `FUN_4aa460` 583, `FUN_581ea0` 544, `FUN_50bda0` 544, `FUN_4872f0` 528, `FUN_46ab50` 512, `FUN_5eb4b0` 496, `FUN_60bbb0` 496, `FUN_4cf400` 480, `FUN_5d1a80` 464, `$L_576670` 448, `FUN_4f0fc0` 448, `FUN_48e700` 448, `FUN_542280` 448, `$L_5fb820` 448, `FUN_5c85a0` 448, `FUN_4cf080` 448, `FUN_45e370` 432, `$L_5f93d0` 432, `$L_59c510` 432, `FUN_5e6650` 432, `FUN_580dd0` 432, `FUN_507ad0` 432, `FUN_467820` 432, `$L_5f08d0` 432, `FUN_507df0` 432, `FUN_57e140` 416, `$L_5914c0` 416, `FUN_48dbb0` 416, `$L_573e40` 416, `$L_5e2cb0` 416, `FUN_5c3ac0` 400, `$L_587ac0` 400, `FUN_5df3d0` 400, `$L_59d010` 400, `FUN_5812a0` 400, `FUN_48e470` 400, `FUN_5c34a0` 400, `FUN_5c55b0` 400, `FUN_5fdaf0` 384, `$L_4e4c0e` 372, `$L_528de2` 366, `$L_4d88b9` 359, `FUN_45f990` 352, `FUN_5a50f0` 352, `$L_5ff250` 352, `$L_5fbc90` 352, `FUN_5ce6e0` 352, `FUN_57c5f0` 336, `FUN_4be0e0` 336, `FUN_4870e0` 336, `FUN_48de40` 320, `$L_4cb310` 320, `FUN_605310` 312, `FUN_4e7a19` 308, `FUN_573d10` 304, `FUN_4709b0` 304, `FUN_4eaf98` 303, `FUN_4897b0` 291, `FUN_54a120` 288, `$L_49c610` 288, `$L_4a80b0` 288, `FUN_48f5d0` 288, `FUN_6000c0` 272, `$L_4a85d0` 272, `FUN_490820` 272, `FUN_5dbbb0` 272, `FUN_45bab0` 272, `$L_401990` 272, `FUN_604b40` 270, `$L_5f18aa` 262, `$L_4e4364` 260, `FUN_48e600` 256, `$L_4c3cf0` 256, `FUN_48a580` 256, `FUN_5c5f60` 256, `FUN_491590` 256, `$L_569550` 256.

Batch 16 done: 69 procs, 44,417 bytes = 2.01%; 63.72% -> 65.73%. Only part of the plan was used: AI procs whose by-value handles shift the ESP offsets (`$L_4279d0`, `$L_432dd0`, `FUN_46ab50`, `FUN_4d5c50`, `FUN_4d0bc0`, `FUN_4cf400`, `FUN_573d10`), iostream users (`FUN_52fa70`, `FUN_507df0`), x87-stack-juggling ones (`FUN_5c7410`, `FUN_48ce40`, `FUN_506ae0`, `$L_5ded40`) and an EH insert (`FUN_4aa460`) were left; added instead float procs up to 130 x87 instructions: trails (`FUN_4bcd00` 2000, `FUN_4bd850`), the line drawer `FUN_5b73a0` (1872), the debug position check and stack log (`$L_4e29a6`, `$L_4e3cff`), the filling station and input screens (`$L_520700`, `$L_514d10`), the face readers, nearest contacts and neighbours, the cheat-code input handler `FUN_532950`, camera apply/follow, the cab view sway `FUN_5354d0` (1440), path direction and side distance, spot/point lights, sparks, flashers, smoke, fire sources, turrets, tanks and more. Hand fixes: `Body_Place` clashed with an existing name (TEXTEQU and two KING_ALIAS repointed to `Body_PlaceAt`); ASM_VAR `g_ws_573e40` repointed to `MissionObj_Event`; COM calls need stdcall (`CallVirt` is thiscall: a local `Sc_Com` helper). norm.py now tracks args left on the stack across callee-popped calls. Tested at two midpoints and at the end: drive and load passed.

Batch 17 plan: from 142 screened candidate procs of 250-4944 bytes with at most 200 x87 instructions (141,975 bytes), at least 44,180 bytes = 2% (base 1,452,057); 65.73% -> ~67.7%. Largest first where the frame is tractable; argument offsets from norm.py. `FUN_529890` 4944, `FUN_607410` 2896, `FUN_5b5710` 2576, `FUN_5b3a40` 2176, `FUN_4cfe70` 2096, `FUN_504d30` 2048, `$L_4ea365` 2035, `$L_420ed0` 1984, `FUN_615da0` 1920, `FUN_567440` 1904, `FUN_4bba00` 1888, `FUN_5b42c0` 1824, `FUN_4ce6d0` 1792, `FUN_5455d0` 1776, `FUN_4fadb0` 1728, `FUN_5291a0` 1712, `FUN_59f870` 1712, `$L_5ded40` 1680, `FUN_59a030` 1648, `FUN_4decc0` 1600, `FUN_4d3240` 1600, `FUN_613b50` 1584, `FUN_4f17a0` 1584, `FUN_58f560` 1568, `FUN_4cf620` 1520, `FUN_4f3000` 1520, `FUN_4cb8d0` 1520, `FUN_5c7800` 1472, `$L_4db8a0` 1472, `$L_5cdf90` 1456, `FUN_54d0b0` 1424, `FUN_4f1240` 1376, `FUN_4bb4c0` 1344, `FUN_506210` 1344, `FUN_608530` 1312, `FUN_54cb90` 1312, `$L_4279d0` 1296, `FUN_502270` 1296, `FUN_4f1fa0` 1264, `FUN_558a30` 1264, `FUN_55cc30` 1264, `FUN_55f000` 1248, `FUN_5fbdf0` 1232, `$L_59b670` 1216, `FUN_52b0e0` 1200, `FUN_4c03d0` 1200, `FUN_4d57c0` 1168, `$L_5a0ff0` 1168, `FUN_55f5f0` 1152, `FUN_506ae0` 1136, `$L_432dd0` 1136, `$L_59e490` 1120, `FUN_4d9460` 1120, `$L_4c1e20` 1104, `FUN_4ef7c0` 1104, `FUN_585150` 1104, `$L_4befa0` 1104, `FUN_468d60` 1088, `FUN_4de550` 1088, `FUN_501010` 1088, `FUN_4cdc90` 1088, `FUN_48d100` 1072, `$L_4c8190` 1056, `$L_4c18b0` 1056, `$L_59c000` 1056, `$L_59afd0` 1056, `$L_5c4f10` 1024, `FUN_52bd20` 1024, `FUN_5924d0` 1024, `FUN_504930` 1024, `$L_4c2c30` 1024, `FUN_5a4880` 1024, `FUN_4efc10` 1008, `FUN_4bffb0` 1008, `$L_5f9f20` 1008, `$L_4e9e97` 999, `FUN_53ece0` 992, `FUN_5cec90` 992, `FUN_5c7410` 992, `FUN_4644f0` 912, `FUN_4d4700` 912, `$L_590810` 912, `$L_4c2380` 832, `FUN_5bb260` 816, `FUN_4d5c50` 768, `FUN_567dc0` 720, `FUN_48ce40` 704, `FUN_4cd770` 688, `FUN_5c6580` 688, `FUN_5eaf40` 672, `FUN_4d60c0` 640, `FUN_5c6830` 640, `FUN_4d0bc0` 624, `$L_56ce61` 623, `FUN_57e2e0` 608, `$L_594400` 592, `FUN_46add0` 592, `FUN_52fa70` 592, `FUN_4aa460` 583, `FUN_4691a0` 576, `FUN_464880` 544, `FUN_46ab50` 512, `FUN_4cf400` 480, `FUN_5d1a80` 464, `$L_576670` 448, `$L_5fb820` 448, `$L_5f93d0` 432, `FUN_507ad0` 432, `FUN_507df0` 432, `FUN_48dbb0` 416, `$L_5e2cb0` 416, `FUN_5c3ac0` 400, `FUN_5c34a0` 400, `FUN_5c55b0` 400, `$L_528de2` 366, `$L_4d88b9` 359, `$L_5ff250` 352, `$L_4cb310` 320, `FUN_605310` 312, `FUN_573d10` 304, `FUN_4709b0` 304, `FUN_4eaf98` 303, `FUN_4897b0` 291, `FUN_54a120` 288, `FUN_48f5d0` 288, `$L_4a80b0` 288, `$L_49c610` 288, `FUN_6000c0` 272, `FUN_5dbbb0` 272, `FUN_490820` 272, `FUN_45bab0` 272, `$L_4a85d0` 272, `$L_401990` 272, `FUN_604b40` 270, `$L_5f18aa` 262, `$L_4e4364` 260, `FUN_48e600` 256, `$L_4c3cf0` 256, `FUN_5c5f60` 256, `FUN_491590` 256, `FUN_48a580` 256, `$L_569550` 256.
Batch 17 done: 33 procs, 44,335 bytes = 2.01%; 65.73% -> 67.74%. Fewer but larger procs than batch 16: the polygon rasterizer `FUN_5b42c0` (1824), the wheels' ground contacts `FUN_504d30` (2048), tyre forces `FUN_4f4f60` (2240) with the grip curves `FUN_4f4d60`, drive and brake forces `FUN_4f3000`, the wheel-against-segment contact `FUN_4fbdd0` (2784), body overlap tests `FUN_591c30` / `FUN_5924d0`, sphere sweeps against polygons (`FUN_5f5370`, `FUN_5f4b30`) and the ground (`FUN_6157a0`), polygon preparation `FUN_5b22f0`, spark bursts, fountains, beams, plume quads, splashes, leaves, light shafts, sprites, falling pieces, the cab hit shake, car hit events, scenery dents, the camera view angle, the music thread and network value writes. Left: AI procs with by-value handles (`FUN_5291a0`), static-init draw loops (`$L_5aa750`, `FUN_5c7410`), the entangled tail `$L_4f3620`, `FUN_48d100` and `FUN_5c55b0` (offsets not trustworthy), `FUN_608fa0`. Hand fixes: `Bodies_Touch` clashed with an existing name (the new one is `Bodies_TouchTest`); the `$L_4cb310` callback and the `$L_5b2940` atexit handler are referenced through their C functions; ASM procs called from C need `ASM_FUNC` (an `extern "C" FUN_x` alone does not link). Tested at four midpoints and at the end: drive and load passed.

Batch 18 plan: from 169 screened candidate procs of 250-8208 bytes (no EH frames or iostream; struct copies and static-init guards allowed; 249,457 bytes), at least 44,180 bytes = 2% (base 1,496,392); 67.74% -> ~69.7%. Largest tractable first, offsets checked by hand after calls to arg-popping ASM procs. `$L_536360` 8208, `$L_4c5b30` 7456, `FUN_53b4c0` 7040, `FUN_579aa0` 6192, `FUN_55b6f0` 5440, `FUN_557530` 4544, `$L_4d1360` 4496, `FUN_547030` 4272, `FUN_551970` 4048, `FUN_57b2d0` 3984, `FUN_599210` 3584, `FUN_5f5a80` 3488, `FUN_54e180` 3488, `$L_4fe18e` 3410, `FUN_561f30` 3312, `FUN_505530` 3296, `FUN_58c1e0` 3232, `FUN_4f4110` 3152, `FUN_548250` 3104, `$L_4f5c77` 3049, `FUN_4d3880` 2848, `FUN_4f7380` 2704, `FUN_4ca7e0` 2704, `FUN_5b5710` 2576, `$L_4c9a10` 2528, `FUN_5b3010` 2432, `$L_5aa750` 2384, `FUN_4f2670` 2288, `$L_58d770` 2176, `$L_4dca30` 2160, `FUN_608fa0` 2096, `FUN_589640` 2096, `$L_4cbfd0` 2064, `$L_4ea365` 2035, `$L_554c20` 1984, `$L_420ed0` 1984, `FUN_5855a0` 1936, `FUN_615da0` 1920, `$L_4da7e0` 1888, `FUN_4bf7a0` 1872, `$L_58738c` 1844, `FUN_5b4fc0` 1824, `FUN_4ce6d0` 1792, `FUN_48c710` 1792, `$L_5a6ad0` 1792, `FUN_5455d0` 1776, `FUN_4fadb0` 1728, `FUN_5b2950` 1712, `FUN_59f870` 1712, `FUN_5291a0` 1712, `FUN_59a030` 1648, `FUN_4decc0` 1600, `FUN_4d3240` 1600, `$L_531ce8` 1592, `FUN_613b50` 1584, `FUN_4f17a0` 1584, `FUN_58f560` 1568, `$L_4c5060` 1568, `$L_4e96e8` 1532, `$L_4d7d2c` 1524, `FUN_4cf620` 1520, `$L_500939` 1463, `$L_5cdf90` 1456, `FUN_4f1240` 1376, `FUN_506210` 1344, `$L_59eac0` 1344, `FUN_54cb90` 1312, `FUN_502270` 1296, `$L_4279d0` 1296, `$L_57d87f` 1265, `FUN_55cc30` 1264, `FUN_558a30` 1264, `FUN_4f1fa0` 1264, `FUN_55f000` 1248, `FUN_5fbdf0` 1232, `$L_4d9f40` 1232, `FUN_58d2b0` 1216, `$L_59b670` 1216, `FUN_52b0e0` 1200, `FUN_4c03d0` 1200, `$L_4e8644` 1181, `FUN_4d57c0` 1168, `$L_5a0ff0` 1168, `FUN_55f5f0` 1152, `FUN_506ae0` 1136, `$L_535b11` 1135, `$L_59e490` 1120, `FUN_585150` 1104, `FUN_4ef7c0` 1104, `$L_4c1e20` 1104.

Batch 18 done: 20 procs (and 4 inner entries), 44,218 bytes = 2.00%; 67.74% -> 69.74%. Big pieces: the car types ini loader `FUN_508300` (8,864; only runs when vehicle.tech is stale and the developer file truck2.tch exists, so retail installs never reach past its first fatal check, which matches), the movable object types reader `FUN_50a740` (3,360, same dev-only path), the car part finder `FUN_53fdb0` (7,056), the wheel surface handler `FUN_502780` (3,456), the ground decal grid `FUN_57b2d0` (3,984), the wall sweep test `FUN_5f5a80` (3,488), the debug warning-log/socket/filter-window block `$L_4ea365` (2,035; the filter windows are referenced as `$L_4eaa41`, `$L_4eaa77`, `$L_4eaab0`, `$L_4eab02`: hand TEXTEQUs, PUBLICs dropped, C ASM_VARs repointed), the unreferenced vehicle-log writers `$L_4e9e97`, plus terrain normals, splash and hit spots, gear choice, spark lines, backdrops, plumes, patch laying, span filling, scene object dumps and a ray test. Fix during testing: `Plane_Intersect` takes the output pointer before its two by-value points (cdb found the fault at 0x56d670). Skipped: AI by-value procs (`FUN_4d4a90`, `FUN_4f2670` register-resident), x87-register-heavy draws (`FUN_5c6830`, `FUN_5ca8e0`), terrain height pair (`FUN_615da0`, `FUN_616850`: arguments pushed across branches). Tested at 6 midpoints and at the end: drive and load passed.

Batch 19 plan: from 165 unconverted procs of 1,000+ bytes without EH frames (441616 bytes in all), at least 44,180 bytes = 2% (base 1,540,610); 69.74% -> ~71.7%. What is left is x87-heavy physics, drawing and car code; one proc at a time with ESP tracked by hand where norm.py drifts, testing after each risky one. `FUN_538370` 11712, `$L_56364a` 10646, `FUN_558f20` 9840, `FUN_617180` 9824, `$L_54a380` 9040, `FUN_5f6820` 8368, `$L_536360` 8208, `FUN_57eae0` 7534, `FUN_552940` 7472, `$L_4c5b30` 7456, `FUN_53b4c0` 7040, `$L_5c8ae0` 7040, `FUN_579aa0` 6192, `$L_4f9489` 5783, `FUN_55b6f0` 5440, `$L_55fa7f` 5233, `FUN_5427a0` 5136, `FUN_588090` 4896, `FUN_52c120` 4656, `FUN_4f8260` 4649, `FUN_557530` 4544, `$L_4d1360` 4496, `FUN_4fca10` 4416, `FUN_5ca8e0` 4272, `FUN_547030` 4272, `$L_534443` 4183, `FUN_551970` 4048, `$L_4d6510` 4016, `FUN_569ab0` 3920, `FUN_54f040` 3904, `FUN_5ccb20` 3856, `$L_5ec1d7` 3770, `FUN_533590` 3763, `FUN_4c8af0` 3648, `FUN_599210` 3584, `FUN_54e180` 3488, `$L_4fe18e` 3410, `FUN_4d4a90` 3376, `FUN_561f30` 3312, `FUN_505530` 3296, `FUN_58c1e0` 3232, `FUN_4f4110` 3152, `FUN_548250` 3104, `$L_4f5c77` 3049, `$L_5fdc9c` 2936, `FUN_4e8ae1` 2923, `FUN_619c20` 2848, `FUN_4d3880` 2848, `$L_4ecbc2` 2822, `FUN_503e30` 2816, `FUN_5e05c0` 2800, `FUN_566970` 2768, `FUN_4f7380` 2704, `FUN_4ca7e0` 2704, `$L_4e7baf` 2632, `FUN_5b5710` 2576, `$L_4c9a10` 2528, `FUN_58a020` 2526, `FUN_5b3010` 2432, `$L_5aa750` 2384, `$L_54d840` 2368, `FUN_4f2670` 2288, `$L_595290` 2256, `$L_58d770` 2176, `$L_4dca30` 2160, `FUN_53d9d0` 2144, `FUN_5e5cd0` 2128, `FUN_608fa0` 2096, `FUN_589640` 2096, `FUN_4f38f0` 2080.

Batch 19 done: 13 procs, 44,816 bytes = 2.03%; 69.74% -> 71.77%. Mostly large vehicle physics and drawing: the vehicle's state reset `FUN_5427a0` (5,136), its draw `FUN_547030` (4,272) with the debug-view markers, the two-body impulse solver `FUN_557530` (4,544), sparks, wheel dust and vapour `FUN_551970` (4,048), the frame step `FUN_54e180` (3,488), the contact record `FUN_548250` (3,104), the light beam effect `FUN_4c8af0` (3,648), keeping the camera out of objects `FUN_599210` (3,584, with its static point table), a moving body's ground step `FUN_58c1e0` (3,232), the velocity and spin limiter `FUN_4f4110` (3,152), the per-frame body setup `FUN_4f7380` (2,704), car shadows `FUN_4ca7e0` (2,704) and parked car setup `FUN_52b0e0` (1,200). norm.py loses track of the stack after asm thiscalls that pop their arguments (`FUN_549e60`, `FUN_503e30`, `Light_Set@24`) and after an indirect thiscall with no arguments; offsets past those points were fixed by hand. Skipped: `FUN_54f040` (passes an uninitialized stack vector to `Room_ObjectAt`, so its second road position depends on leftover stack data), `FUN_533590` (continues into `$L_534443` past its ENDP), `FUN_505530` (left for later: its offsets drift 16 bytes at each of two `FUN_503e30` calls), `FUN_5a6420` (left for later: drift after an indirect thiscall), `FUN_561f30` (the helicopter step, not attempted yet). Tested after every risky proc and at the end: drive and load passed.

Batch 20 plan: at least 44,180 bytes = 2%; 71.77% -> ~73.8%. From the unconverted procs of 1,800-3,400 bytes without jump tables, taken in this order as each turns out tractable: `FUN_561f30` 3312, `FUN_505530` 3296, `FUN_4e8ae1` 2923, `FUN_4d3880` 2848, `$L_4ecbc2` 2822, `FUN_5b5710` 2576, `FUN_58a020` 2526, `FUN_5b3010` 2432, `$L_5aa750` 2384, `$L_58d770` 2176, `$L_4dca30` 2160, `FUN_608fa0` 2096, `FUN_4f38f0` 2080, `$L_4cbfd0` 2064, `FUN_55d5d0` 2052, `FUN_5a9540` 2048, `FUN_58e7f0` 2016, `$L_554c20` 1984, `$L_420ed0` 1984, `FUN_590d20` 1952, `FUN_5855a0` 1936, `$L_4da7e0` 1888, `FUN_4bf7a0` 1872, `FUN_58baa0` 1856, `$L_58738c` 1844, `FUN_5b4fc0` 1824.

Batch 20 done: 24 procs, 45,040 bytes = 2.04%; 71.77% -> 73.81%. FUN_4d3880 Ai_LookAhead, FUN_5291a0 Map_BuildRoads,
FUN_5455d0 Car_AddMass, FUN_590d20 Item_DrawBody, FUN_58baa0 Body_Bounce, FUN_5b4fc0 View_SphereClip,
FUN_5855a0 Item_BodyContact, FUN_4bf7a0 Fx_EmitterStep, FUN_4fadb0 Body_FrameIntegrate, FUN_4f38f0 Wheels_ForceTorque,
FUN_59a030 Cam_ChaseStep, FUN_608fa0 Grid_NewSplit, FUN_5a9540 Weather_ApplySky, $L_554c20 Car_Hit,
$L_4dca30 Fx_ExplosionStep, $L_4cbfd0 Fx_ReflectionQuad, $L_4da7e0 Fx_BurstStep, $L_5a6ad0 Sun_FlareStep,
FUN_561f30 Copter_FlightStep (3312), FUN_58d2b0 Body_UpdatePlanes, FUN_4f1fa0 Wheels_OnPlane,
FUN_5c6ac0 Ribbon_DrawSegment, FUN_4ef7c0 Body_StepVectors. Skipped: FUN_4e8ae1, FUN_58a020 (split, code past ENDP),
$L_4ecbc2, $L_58738c (fragments), FUN_5b5710, FUN_5b2950, FUN_4ce6d0, FUN_4d2b30, FUN_48c710, FUN_59f870, $L_5aa750,
FUN_5a6420 (stack drift around by-value / zero-arg indirect calls, or debug-only), $L_420ed0 (AI by-value Int_Copy).

Batch 21 plan: more than 44,180 bytes from the remaining 1.0-1.6 KB x87 procs with few calls (FUN_506210, FUN_502270, $L_59eac0, $L_59e490, $L_5a0ff0, FUN_58f560, $L_4c5060, ...), one per piece, screened for jumps outside the proc and stack drift.

Batch 21 done: 56 procs, 44,264 bytes = 2.00%; 73.81% -> 75.81%. FUN_502270 Patch_SegmentHits, FUN_506210 Body_GroundNormal,
$L_59eac0 Puff_DriftStep, $L_59e490 Puff_CircleStep, $L_4c5060 Fx_GlareDraw, $L_5a0ff0 Fx_GlowDraw, FUN_58f560 Body_FindOverlap,
FUN_5fbdf0 Face_RayHit, FUN_55f000 Trailer_HitchCheck, $L_4d9f40 Fx_FireStep, $L_59b670 Puff_BlowStep, FUN_54cb90 Car_LookAtSign,
FUN_4f1240 Body_PutDown, FUN_55cc30 Car_HitchImpulse, FUN_506ae0 Body_SlideMove, FUN_585150 Item_PointContacts,
FUN_4f2670 Wheels_StepFrames (2288), FUN_4c03d0 Fx_SmokeSheets, $L_4befa0 Fx_MistStep, $L_4c1280 Fx_SourcePoint,
FUN_616850 Terrain_HeightAt, $L_4bea60 Fx_ShaftsStep, FUN_5665c0 Copter_HeadForTarget, $L_5f9f20 Lod_Draw,
$L_5cdf90 Obj_BillboardDraw, FUN_4decc0 Cab_ViewSway, FUN_5aebc0 Clip_EdgeVertex, FUN_5c6830 Strip_DrawArch,
FUN_57e2e0 Obj_HoldOnRail, FUN_4ef480 Wheels_LoadShares, FUN_5c82f0 Sign_Swing, FUN_595d20 ReplayCam_NextShot,
$L_584060 Item_EndLeg, FUN_4e4468 Clock_Text, FUN_46b140 Rope_SetCache, FUN_5f21e0 Slot_CastRay, FUN_556780 Limit_Share,
$L_4cbec0 Fx_ScrapeSpark, $L_4c3cf0 Fx_SquareSpot, $L_4c22b0 Fx_SeaSpot, FUN_4e28b0 Vec3_OutOfBox, FUN_45f650 Far_Compare,
FUN_5bdc80 Control_FitSize, $L_569550 Terrain_RayCallback, FUN_4e794f Vec3_Soften, $L_5c0700 List_KeepSelection,
FUN_45e2a0 IntroSort_ByKey, FUN_45f8d0 IntroSort_By504, FUN_4ee540 Body_SetMass, FUN_609990 Grid_SplitEdge,
FUN_577ab0 FfForce_Construct, FUN_592f90 PtrList_Append (with its comparator $L_592fe0 Rec_CompareF4), $L_4df8e0 Init_696a00,
FUN_5d8710 Window_ConstructSub, FUN_48eda0 VPoly_Evaluate, $L_5dbe10 EventRing_Push.
Switch-default paths reading uninitialized locals ($L_4c5060, $L_5a0ff0) now leave zeros. Callbacks used as data were rebound
(g_fiCb4cbec0, g_ws_569550, g_movbodCmp592fe0); atexit stubs inside converted procs got C equivalents; stale PUBLIC lines for
$L_5c0700 / $L_592fe0 / $L_6f67d8 removed.
Skipped: $L_5aa750, $L_549580, FUN_558a30, FUN_4d3240, FUN_615da0, FUN_4f17a0 (stack offsets shifted by callee-popped or by-value
calls, not mappable with confidence); FUN_4fdc80, FUN_4f5820 (fall through past ENDP); $L_4f3620, $L_5a6050, $L_5ca70c (fragments);
FUN_5af9e0, FUN_5af3e0, $L_5fae60 (jump tables with x87 values across branches or code past the table); $L_4c9a10 (10-argument
interleaved pushes); FUN_5bb260 (uninitialized format code); FUN_4cdc90, FUN_4cd770, FUN_567dc0, FUN_465fd0, $L_526bc0, FUN_45bab0,
FUN_4ac3d0, FUN_4b54b0, FUN_600e60, FUN_5586f0, FUN_471780, FUN_477b60, FUN_4d0bc0 (AI / STL by-value copies); FUN_52fa70,
FUN_507df0 (inlined iostreams with SEH); FUN_5c55b0, FUN_5c3ac0, FUN_605310, FUN_602320 (network / sound with SEH or throws);
$L_4c3df0 (Light_Set drift); FUN_48d100 (by-value double vectors); FUN_5fedbc (stores the caller's ECX); FUN_579aa0 (too long).

Batch 22 plan: more than 44,180 bytes from the remaining larger float procs ($L_58d770, FUN_5b3010, FUN_579aa0, FUN_52c120, ...) with hand-mapped stack offsets where norm drifts.

Batch 22 done: 12 procs, 44,528 bytes (2.02%), 75.81% -> 77.83%. $L_58d770 Body_SettleOnPlane (2176), FUN_5b3010
Sprite_DrawStrip (2432), FUN_579aa0 Decal_BuildOnObjects (6192), FUN_52c120 ParkedCar_DrawShadow (4656), $L_54a380
Vehicle_FrameStep (9040, the vehicle's per-frame step), FUN_505530 Car_PlaceOnGround (3296), FUN_55b6f0 Hitch_Align
(5440), FUN_589640 Item_SweepPoints (2096), FUN_503e30 Wheel_FindGround (2816), FUN_566970 Copter_Frame (2768),
FUN_595eb0 TvCamera_StartMode (1472), FUN_53d9d0 Dash_DrawOverlays (2144). Drive and load tests pass (one drive run
ended at room_ap_048, two reruns normal).
norm.py counts ASM callees (no @N) as caller-cleaned, so offsets drift by their ret N after calls such as FUN_5662f0
and FUN_503e30; a copy reading each callee's ret N from king.masm fixed it. FUN_53d9d0 draws five text buffers that
nothing in it fills (kept uninitialized as in the original).
Skipped: FUN_4ff800, FUN_58a020 (run past ENDP into other procs); FUN_538370 (11,712, sounds and force feedback,
too long for the remainder); FUN_558f20 (9840, hitch solver); $L_4c5b30 (shared tail with different pushes);
FUN_59f870 (reuses argument slots as temporaries).

Batch 23 plan: more than 44,180 bytes, 77.83% -> about 79.85%: FUN_538370 (11,712, vehicle sounds and force feedback),
FUN_558f20 (9840, hitch solver), FUN_569ab0 (3920), FUN_4fca10 (4416), $L_534443 (4183), FUN_533590 (3763), and
smaller ones ($L_54d840, FUN_5b2950, FUN_568090, FUN_53f0c0) as needed; offsets from norm2.py.

Batch 23 done: 42 procs, 44,272 bytes (2.00%), 77.83% -> 79.83%. FUN_538370 Vehicle_Sounds (11,712: the vehicle's
sounds and the steering wheel's force feedback with its log), FUN_503500 Wheel_RollDrag (1184), FUN_55f5f0
Truck_HitchTrailer (1152), gzip 1.2.4's inflate.c (8 procs, 4784: huft_build / huft_free / inflate_codes / stored /
fixed / dynamic / block / inflate), $L_5a9dc0 Weather_Frame (1648), FUN_568090 Copter_Warnings (1520), $L_58e290
Body_ResetPoints (1376), FUN_5ccb20 Sparks_AlongPolygon (3856), FUN_619c20 Terrain_PlacePlants (2848), FUN_4ce6d0
Car_DrawForces (1792), FUN_5b2950 Mesh_Draw (1712), FUN_53e7a0 Cars_DumpToLog (1344), FUN_48c710 Road_NearestOn
(1792), $L_595290 Mirror_Draw (2256) and gzip's trees.c, bits.c, longest_match and fill_window (21 procs, 5296).
Drive and load tests pass (one drive ended at room_ap_048 without moving, two reruns normal).
Particles_StartAt (media.cpp) now returns FUN_5cc680's result: its asm callers chain it as the next emitter.
The C wrapper Terrain_Call617180 is declared stdcall but FUN_617180 takes ECX; Terrain_PlacePlants calls
FUN_617180 directly as thiscall (the wrapper is unchanged).
norm2.py now also counts `sub ESP,n` (n <= 64) as pushed bytes and gives jump-table case labels the depth at the
`jmp D[...]`; it still misses callee pops after a label reached by jmp with pushes (FUN_619c20 +20, $L_595290 +12,
FUN_5b2950 +12: mapped by hand).
Skipped: FUN_558f20 (9840, iterative hitch solver, only with a trailer), $L_4d1360, FUN_4d4a90, FUN_569ab0,
FUN_468d60, $L_420ed0 (AI handles and by-value road positions), FUN_4ff800, FUN_58a020, FUN_576d70, FUN_5e5cd0
(entangled or nested procs), FUN_6217b6 (the CRT's printf engine), FUN_59f870 (argument slots as temporaries) and
the many $L_ tails without a prologue.

Batch 24 plan: more than 44,180 bytes, 79.83% -> about 81.85%: FUN_617180 (9824, terrain plant patches), FUN_5f6820
(8368), $L_536360 (8208), FUN_552940 (7472), FUN_53b4c0 (7040), FUN_588090 (4896), swapping any that turn out
entangled; offsets from norm2.py.

Batch 24 done: 10 procs, 44,464 bytes (2.01%), 79.83% -> 81.84%. FUN_617180 Patch_AddPlant (9824: a plant's
quads by type and grid parity), FUN_5f6820 Shape_SweepHit (8368: a point swept against a sphere, cylinder, plane,
polygon list or wall chain), FUN_552940 Car_LampShadows (7472: the lamps' shadow nodes and their glows, decals and
lights), FUN_588090 Item_KnockOver (4896), FUN_5e05c0 Dbg_OnInput (2800: the debug window's mouse look, zoom,
screenshots and stats box), FUN_615da0 Grid_HeightAt (1920), FUN_5b5710 Render_PortalView (2576), FUN_5af9e0
Mesh_TexCoords (1856), $L_5aa750 Water_Draw (2384) and $L_54d840 Car_HandleEvent (2368).
Drive and load tests pass (one drive ended at room_ap_048 without moving, the rerun normal).
norm3.py (norm2.py with the callees' own stack pops) gave the offsets.
framework.cpp now takes Water_Draw's address directly: its ASM_VAR label of $L_5aa750 no longer links once the
label is converted. scene.cpp's new code uses local vector aliases (Swp_*) because including vecmath.h there
clashes with its own declarations.
Skipped: FUN_5ca8e0 (FUN_5ca660 calls the label $L_5cb970 inside it), FUN_53b4c0 and FUN_54f040 (AI road
positions by value), FUN_4fca10 (runs on into $L_4fe18e), FUN_4e8ae1 (runs on into $L_4e964c); $L_536360 not
reached.

Batch 25 plan: more than 44,180 bytes, 81.84% -> about 83.85%: $L_536360 (8208), FUN_57eae0 (7534), $L_5c8ae0
(7040), FUN_4f8260 (4649), $L_4d6510 (4016), FUN_533590 (3763) and smaller ones as needed; offsets from norm3.py.

Batch 25 done: 27 procs, 44,816 bytes (2.03%), 81.84% -> 83.87%. $L_536360 Player_Frame (8208: the player's
frame: zoom, free camera, view, light, AI position, logs), FUN_53b4c0 Player_Controls (7040: pedals, steering,
gears and the debug keys), $L_4d6510 Ai_DriverStep (4016), FUN_54f040 Car_GiveWay (3904), $L_4c9a10
Weather_LampRain (2528), FUN_53f0c0 Cars_DumpHistory (1632), FUN_4d3240 Trail_Add (1600), FUN_4f17a0
Body_PlaceAtFrame (1584), FUN_4cf620 Ai_CarFrame (1520), FUN_4cdc90 Road_TurnAhead (1088), FUN_48d100
Polyline_Advance (1072), FUN_4644f0 Road_MiddleOfThree, $L_590810 Body_OntoPlane, FUN_4d4700 Ai_TargetSpeed (912
each), FUN_5bb260 Texture_LoadRows (816), FUN_4d5c50 Ai_CarTargetSpeed (768), FUN_567dc0 Copter_FindLanding (720),
FUN_48ce40 Polyline_Nearest (704), FUN_4cd770 Road_LateralOffset (688), FUN_4d60c0 Ai_ThrottleBrake (640),
FUN_46add0 Holders_SortHeapByDistance, $L_594400 Sparks_Frame (592 each), FUN_464880
Holders_PartitionByDistance (544), FUN_50b620 DMat_Determinant (496), FUN_4cf400 Car_StoreState (480),
$L_5fb820 Portal_Draw (448), FUN_549e60 Lamp_Flash (400).
The load test position is exact (507.330231,940.087585). The drive test varies a lot today: the committed batch 23
and 24 builds also fall through the ground at room_ap_048 in some runs.
Load-test drifts found and fixed: Body_OntoPlane's lookup point is the vtbl+72 matrix's position (out + 36), not
a buffer of its own; Ai_CarFrame computes k^(dt*m) with fyl2x/f2xm1 kept on the FPU (one asm helper, As_Pow);
Car_GiveWay reads the RoomInfo's +32 that Road_GetRoadParameters leaves unwritten on one branch (now 0).
Bisecting: replace a proc's `IFDEF ASM_<GROUP>` in king.masm with `IF 1` to run its asm body alone.
framework.cpp / vehicle.cpp / effects.cpp now take Water_Draw's, Game_TruckMessage's and Car_WorldEvent's
addresses directly ($L_ procs converted earlier). Int_Copy (FUN_41d300) arguments are plain ints passed by value.
Skipped: $L_5c8ae0 (argument pushes shared across its seven cases), $L_5fae60 (a vtable thunk $L_5fb500 inside
it), FUN_558a30 (trailer only, fastcall leftovers), FUN_4e8ae1, FUN_4fca10, FUN_57eae0, FUN_4f8260, FUN_533590
(run on into the next proc).

Batch 26 plan: more than 44,180 bytes, 83.87% -> about 85.9%, from the 76 procs with EH frames left (82,517
bytes): exceptions are always fatal here, so the C++ keeps only the normal path (constructor / destructor calls)
and drops the frame's state writes; FUN_4bba00 (1888), FUN_4cb8d0 (1520), FUN_4cfe70 (2096), FUN_5b3a40 (2176),
FUN_567440 (1904), FUN_56ffd0 (6832), FUN_529890 (4944), $L_597b00 (4304) and onwards.
Batch 26 done: 20 procs, 44,240 bytes (2.00%), 83.87% -> 85.87%, all with EH frames (the C keeps the normal path:
constructors, the destructor calls at each exit). FUN_56ffd0 Game_PlayerMessage (6832: laps, refuelling, repairs,
presents, orders, speed cameras), FUN_529890 Map_Draw (4944), $L_597b00 Game_FrameKeys (4304: hotkeys, the marine
task by zone, the network position block, the tide and fog), FUN_5ef830 Scene_ReadNode (3728: the 42 node kinds),
FUN_607410 Grid_ForceSplit (2896), FUN_5ee580 Overlays_Load (2800: CLUSTER / OVERLAY / ROOM blocks), FUN_5b3a40
Render_TexPolygon (2176), FUN_4cfe70 Car_HitsAndFrame (2096), FUN_567440 Bullet_Fly (1904), FUN_4bba00 Fx_Burst
(1888), $L_5ded40 Camera_Collide (1680), FUN_4cb8d0 Car_ScrapeSparks (1520), $L_4db8a0 Wreck_Frame (1472),
FUN_4bb4c0 Fx_DebrisBurst (1344), FUN_608530 Grid_NewCell (1312), $L_432dd0 Ai_UpdateChasedCars (1136, the
state.dump trace kept), FUN_4d9460 Blow_Construct (1120), $L_5c4f10 Media_Frame (1024), $L_5ef410
ListHead_Construct and $L_5e6980 Rec5e6980_Clear (32 each; the C statics that took their labels now take the
functions).
Draw_Dispatch (FUN_5b3a00) is now __fastcall: the original passes the renderer through in ECX, which the old
__stdcall version only kept by luck. Gate_602bd0 likewise became __fastcall Blob_Set(s, 0, 0): its callers pass
the stream in ECX (Media_Frame first called it with CallC and crashed on exit).
Fx_Burst's speed factor now multiplies z unrounded (fst keeps it on the FPU), as Fx_DebrisBurst's does.
Grid_ForceSplit: the stack of cells (7242a0) holds 0 for a missing neighbour, so the second pop happens whatever
the first gave; skipping it passed 0 to Grid_SplitEdge (exit crash, found from the event log's fault offset).
Load test: exact, but the same exe now drifts in about 1 of 3 runs (also with all batch-26 procs in asm and with
the batch-25 exe built from HEAD in a worktree), so any exact run counts. Bisecting with `IF 1` misses callers in
other C files: integ re-aliases their ASM_PROC to the C name.
Skipped: FUN_4ebd00, FUN_543e00, FUN_5443e0, FUN_4d8350 (end in a fatal call and run on into the next proc),
FUN_4ac3d0 (jumps into the tail $L_4ac6b5), the $L_ tails without a prologue ($L_550358, $L_4ed6c8, $L_5280e2 ..);
left for later: FUN_61a8a0 (4096, the grid's construction), FUN_54ff80, FUN_4d0bc0, FUN_52fa70, FUN_4aa460.

Batch 27 plan: more than 44,180 bytes, 85.87% -> about 87.9%, from the 183 unconverted procs that end cleanly
(98,818 bytes), the ones skipped before: FUN_61a8a0 (4096), $L_4c5b30 (7456), $L_4d1360 (4496), FUN_569ab0 (3920),
FUN_4d4a90 (3376), FUN_5e5cd0 (2128), $L_420ed0 (1984), FUN_4d2b30 (1808), FUN_59f870 (1712), FUN_5af3e0 (1536),
FUN_576d70 (1472), $L_4279d0, FUN_5a6420, FUN_468d60, FUN_5c7410 and smaller, raw listing checked where norm3.py
drifts.

Batch 27 done: 44,384 bytes, 85.87% -> 87.88%. FUN_61a8a0 TGridMesh_Build (4096), FUN_569ab0 Dash_Draw (3920),
$L_527240..$L_52773b Garage_Message (1424), FUN_5301a0..$L_530751 Controls_ReadDefFile (2720), FUN_4d2b30
Ai_DebugOverlay (1808), FUN_59f870 Cone_SetPlanes (1712), FUN_531cc0..$L_531ce8 Viewer_AttachVehicle (1632),
FUN_5af3e0 Render_LayerUVs (1536), FUN_543e00..$L_5442a4 Vehicle_Construct (1504), FUN_5443e0..$L_544834
Vehicle_ConstructCopter (1344), $L_4279d0 Ai_FillStartGrid (1296), FUN_57d860..$L_57d87f MovingItem_Init0 (1296),
FUN_584c70..$L_584eb2 Item_SphereContact (1248), FUN_55d140..$L_55d59d Tractor_Move (1168), FUN_584820..$L_584a52
Item_PointContact (1104), FUN_468d60 Holders_MedianByDistance (1088), FUN_5c7410 Strip_Draw (992),
FUN_4dde70..$L_4de0e7 CabView_Setup (960), $L_4c3df0 Ring50_DropPoint (912), $L_5451c0..$L_545367 Copter_LoadHook
(912), $L_55de40 Truck_PlaceWithTrailer (864), FUN_561800..$L_561a7c Copter_Init0 (848), FUN_596b30 PathCam_Frame
(800), $L_549580 Car_FlasherKick (784), FUN_4e21c0 Trash_CreateGuest (768), FUN_5491f0 Car_ResetSane (720),
FUN_5f1db0 Scene_ReadChildren (720), $L_52ebc0 TrashBox_LoadHook (688), FUN_4e1f10 Trash_CreateHost (688),
FUN_52fa70 Ini_ReadSections (592, the old ifstream getline loop as fread, a last line cut by the end of the file
dropped as before), FUN_51d6d0 Save_WriteGame (544), FUN_507df0 Ini_ReadInts (432, the istrstream kept, its
destructors at +12 as in the asm), FUN_5ea690 Cluster_ConstructEmpty (416), FUN_571c20 Semitrailer_Create (256),
FUN_4be8d0 Snow_Construct (368), FUN_5e4f60 Resources_AddTable (368), $L_5ee070 Cluster_Attach (352),
FUN_5d2540 / FUN_5d2650 NameHash_Add / _AddList (272, 208), FUN_5c4d30 Node_ReadHistory (240), FUN_501630
Cab_ReadSettings (224), FUN_4eb4a0 Switch3_Set (224), $L_5c5ea0 Node_NetName (192), $L_5d9a20 Room_QueueDelete
(144). Also fixed TGrid_MakeNode (terrain_grid.cpp): it read its point from the uninitialized out buffer instead of
the second argument. Skipped: $L_4d76b0 (the AI player-entry handler, many unconverted callees), FUN_562c20
(x87 loop returning in ST0), FUN_558a30 (126 x87), FUN_5db710 / $L_5f1720 / $L_571d60 (jump outside their range),
FUN_45a7d0 / FUN_4b54b0 / FUN_600e60 (Rec60 by value, STL deque and vector internals). Load test exact
(507.330231,940.087585, the stalled run) with exit code 0; three drifting runs in a row before the last ones were
noise (exact again next run).

Batch 28 done: 44,276 bytes (87.88% -> about 89.9%). $L_4d1360 Cop_Step (4,496, the police car's chase, warnings,
fines and shots; its largest proc), FUN_58a020..$L_58a9fe Box_PointContacts (3576), FUN_4d4a90 Car_GearBlockMask
(3376), $L_4c5b30 LampGlows_Draw (7456), FUN_58e7f0..$L_58f1cd MovingItem_CheckFrame (2544), FUN_5e5cd0 MeshDraw_SetFlags
+ $L_5e5eb0 Mesh_BuildVertices (2128), FUN_4eec80..$L_4ef1f7 Trailer_SaddleLoad (1424, dead code after the fatal
not carried), $L_56e060..$L_56e54f AnmObj_OnLoad (1344), $L_52ef80 TreeBox_CreateHost (1328), FUN_558a30
Tractor_PlaceTrailer (1264), $L_4e19b0..$L_4e1d07 App_Start (992), FUN_5917f0..$L_59199f Body_PointContact (846),
FUN_4d0bc0 ParkedCar_Destroy (624), FUN_5daa30 Fw_MakeCluster (608), FUN_4d08d0 Fx_MakePlayerVehicle (576),
FUN_4691a0 Records_PartitionNear (576), FUN_48a7c0 Rational_ToDouble (528), FUN_46ab50 Records_PartialSortNear
(512), $L_454ca0 Road_NearestCurve (480), $L_5da410 Wnd_Resize (480), FUN_542580 Car_SetInertia (448), $L_576670
Bus_OnFare (448), FUN_507ad0 Ini_ReadFloats (432), FUN_469a20 Rec1592_Swap (416), FUN_459c50 RopeConcat_Destroy
(416), FUN_48dbb0 Curve_Construct (416), FUN_5c3ac0 Net_CheckSession (400), FUN_459e90 RopeSubstr_Destroy +
$L_459ff0 RopeFn_ResetVtbl (368), Rec4d0760_FreeVectors (368), FUN_4709b0 RoadNode_Construct (304), FUN_573d10
Vehicle573d10_Destroy (304), $L_5fac60 Sprite_Draw (320), $L_5a0510 Effect_DriftStep (320), FUN_48f5d0 /
FUN_490820 / FUN_48a580 Vec580 / Vec24 / VecDw_CopyConstruct (288, 272, 256), $L_4a80b0 Native_Apply (288),
$L_49c610 Nat_IsString (288), FUN_54a120 Flasher_Construct (288), FUN_5dbbb0 Fw_ResetRenderer (272), $L_4a85d0
Frame_StepSequence (272), FUN_6000c0 Rec56_CopyConstruct (272), FUN_604b40 Ds_Open (270), FUN_4592b0 Hash_NewNode
(256), Task491590_Destruct (256), FUN_48e600 VPoly_SquaredDerivative (256), FUN_5c5f60 NetMoving_Construct (256).
Ds_Open's int throws are caught by its own catch (int) funclet ($L_604c4e: buffers released, the play lock
deleted, the code returned); the C version does that directly. FUN_605310 Sound_PlayBuffer was converted and
reverted: its catch(...) also catches the access violation of locking a null buffer on every exit, which C
can't do without __try (no _except_handler3 in this build). Also fixed Save_WriteGame (batch 27): its COM calls
went through the __thiscall CallVirt and now use the stdcall Sv_Com. A coverage run showed many of these are not
reached by the load test (trailers, copters, police, script natives, garage, net, body contacts). Load test exact
(507.330231,940.087585) with exit code 0.

Batch 29 done: 44,369 bytes (about 89.9% -> 91.9%). Largest: FUN_4ebd00..$L_4ed6c8 PhysBody_Construct (10,192,
the vehicle body built from its type record, four PROCs split at fatal calls). Then FUN_4fca10..$L_4fdb50
Body_NextFrame (4720), FUN_4fdc80..$L_4fe18e Body_PlainStep (4704), FUN_4ff800..$L_5006aa Car_DriveStep (4176,
clutch, automatic gears and drive force), FUN_4f5820..$L_4f5c77 Body_StartSteps (4160), FUN_586df0..$L_58738c
MovingItem_Frame (3280), FUN_4fb470..$L_4fbb7e Body_EndSteps (2400), FUN_55d5d0..$L_55ddfe Tractor_Frame (2160),
FUN_562eb0..$L_56337b Copter_CircleTouch (1504), FUN_57e540..$L_57e9ca Body_SurfaceResponse (1440), FUN_5a6420
Dial_Update (1088), FUN_590520..$L_590791 Body_UpdateLying (752), FUN_4eacce..$L_4eaf8f History_Record (714),
FUN_562c20..$L_562e94 Copter_SideDrift (656), FUN_4aa460 Elems44_InsertAux (583), FUN_4fef20..$L_4ff139
Body_StateStep (560), FUN_467580..$L_46776a OrderInfos_UninitCopy (512), FUN_467000..$L_467163 /
FUN_47e260..$L_47e3c3 Orders144_UninitCopy / _UninitCopyB (384 each). Most ranges are functions the disassembler
split at calls it took as noreturn (World_Fatal, __CIpow, Sys_Fatal); the real end was found by following every
jump out of the range. The order copiers' catch funclets are still named by their EH tables, so their ELSE blocks
define the funclet label with LABEL NEAR. Body_PlainStep first crashed at exit with the position still exact: its
KING_ALIAS `_Ps_Query` already existed in scenery.cpp for another function, and the linker took that one. A scan of
all aliases found two older clashes, fixed by renaming: `_Gm_First` (effects.cpp, batch 28, Car_GearBlockMask now
calls Obj541970_First as the asm does) and `_Ce_Fatal` (clusters.cpp, batch 27). The trailer, copter, dial,
police-free and sub-step paths are probably not reached by the load test. Load test exact (507.330231,940.087585)
with exit code 0.

Batch 30 done: 44,528 bytes (about 91.9% -> 93.9%). Largest: FUN_533590..$L_53549a Car_PlaceOnRoad (8000, the
vehicle put back on the road: tries along its line, places from the room, rays down, sideways slides against
other cars' boxes and the car's points), FUN_57eae0..$L_58084e Bodies_Impulse (7648, two bodies' impulse at a
contact) and FUN_54ff80..$L_550358 Car_AfterPhysics (6480). Those three were written with a new symbolic x87
lifter (lift.py in the session scratchpad) that replays the FPU stack over a listing range and prints C in the
asm's operation order; the biggest frames stay a byte array with V/D/I slot accessors. Then the .rmp resource
loader FUN_5ec0f0..$L_5ed091 Image_ReadResources (5488), FUN_5f0b10..$L_5f0d41 Mesh_PrepareDraw (2560),
FUN_527f30..$L_5280e2 GameWindow_Construct (2288, the main menu and shell screens), FUN_5dae90..$L_5db637
Module_Load (2000, *.b3d, image, overlays, color keys), FUN_4d8350..$L_4d88b9 Media_VehicleEvent (1744),
FUN_5e3bc0..$L_5e3c22 App_SetupVideo (1408), FUN_5e29b0..$L_5e2cb0 App_InitInstance (1184, MFC calls through
mangled-name aliases), $L_511170..$L_5112b7 App_ShowEscMenu (1088), $L_5ddc50..$L_5ddd68 App_ShowQuitPanel
(1040), $L_528820..$L_528bd6 GameWindow_Draw (1008, info screens and the intro movie), FUN_5db710..$L_5db9b8
Module_LoadImage (736), $L_528ca0..$L_528de2 MovieScreen_Frame (688), FUN_56ce40..$L_56ce61 Hud_Frame (656) and
$L_51e140..$L_51e2d3 SaveScreen_Command (512). Checked by eye: the main menu (all eight buttons) and the Esc menu
(a copy of the test script presses Esc 22 s into the loaded game: smoke_esc.ps1 -EscAt) and the HUD. Not reached
by the tests: the placement on the road, the quit panel, the save screen's commands, the trailer paths of the
vehicle event. Left in asm on purpose: the span fillers at $L_5fdc70, the deque insert FUN_4ac3d0 (by-value
iterators), the rope compare FUN_45a7d0, $L_5dd8c0 (starts at a bare label outside any PROC), the adjustor thunks
at $L_55fa7f. Load test exact (507.330231,940.087585) with exit code 0.

Batch 31 done: 44,228 bytes (about 93.9% -> 95.9%). Largest: FUN_4f8260..$L_4f9489 Body_SubStep (10432, a
vehicle sub-step with its wheels on springs: wheel contacts, spring and damper forces, pressed-in wheels pushed
out by an impulse, contact force and torque sums), FUN_4e7b4d..$L_4e8644 Body_MovingPointImpulse (3988),
$L_582c30..$L_583c18 MovingItem_Step (4096), FUN_4e8ae1..$L_4e964c Body_FixedPointImpulse (3005), then
MovingBody_Settle (1840), SoundSource_Update (1712), Car_PedalStep (1696), App_UpdateCamera, Windscreen_Frame
(1648 each), $L_4e969e..$L_4e96e8 Body_ContactImpulseB (1606, unreferenced), Cab_Sway (1296), DxError_Name (1264),
Media_ReadValue (1120), ReplayWnd_Key (1008), NetVehIn_Get (944), Cab_EyeFrame (816), Wheels_FrameSpin (768),
MovingItem_PointBounce (736), Slot_SegmentCast (704), Car_DropToGround (672), Sky_LightColours (640),
MovingItem_BodyContact (528), Line_ClipEdge (496), Cheat_KeyTyped (368), Msg_NextType, Dash_TextPlace (272
each), Angle_F2Pi (269), Msg_SkipRecord (240) and Copter_SetState (144).
Fixes: since batch 22, FUN_589640 (Item_SweepPoints) had been pointed at FUN_585150's C++ (_Item_PointContacts),
so item point sweeps ran the wrong function and the drive test truck went through trees and fell through the
ground (Z about -600; the original ends at Z 20.9). With that fixed, MovingItem_PointBounce's frame needed 12
more bytes (asm ESP offsets taken after three pushes). Drive test now checks the end height.

Batch 32 done: about 24 KB. Largest: $L_5c8ae0 TreeGen_Draw (7040, the $$TreeGenerator1 handler: swaying trunk
on two springs, crown by shape, ribbons/beams and plumes; the asm frame kept as a byte array), FUN_5ca8e0
TreeGen_DrawStill (4272, with the inner $L_5cb970 Light_Channel), $L_4d76b0..$L_4d7d2c Media_AiEvent (3184, the
world side of AI player events: car objects, routes, notices, engine idle), $L_420ed0 Ai_CheckPlayersNear (1984),
then MeshObject_Draw, Sound_PanShares, MovingItem_Settle, FTree_Run / TrashBox_Run / Scenery_Run, Net_FillSendBuffer,
Net_HostReply, FilePort_ReadHeader, Explosion_Construct, Polyline_Construct, List_NewNode and Copter_PlaneCircle.
A first List_NewNode passed an extra argument to the two-register @Stl_Lock@8 and crashed the world load (found
with cdb). The load test drifts now and then even on the batch 31 build; it was exact on repeated runs. Left:
FUN_576d70 (nested hand-edited PROCs), $L_5d7b90/$L_5dd8c0 (start at bare labels), FUN_6047d0/FUN_605310 (real
try/catch), FUN_462850 (rope, throws).
