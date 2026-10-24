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
