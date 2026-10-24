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
| 8 | | `CopterPanel_Draw`, `Road_FindPositionRoute`, `Ai_CreatePlayersRegular`, `CopterScreen_Start`, `NetDialog_Draw`, `Road_FindRoute` and 13 more | 44,310 | 49.62% |

Tests per batch: C++ build, new-game drives and a load-game run. `Player_Save` is not covered (no save is written).

Batch 8 plan (target over 2% = 44,180 bytes; 19 procs, 44,678 bytes = 2.02%; game code in C++ 47.60% -> 49.62%, left in MASM 52.40% -> 50.38%): `FUN_56bc80` 3,904, `FUN_4858b0` 3,888, `FUN_5a7b50` 3,488, `$L_422f20` 2,832, `FUN_4ebd00` 2,656, `FUN_5dc4d0` 2,614, `FUN_484c80` 2,560, `FUN_4927d0` 2,384, `FUN_56e5a0` 2,304, `FUN_5e6e90` 2,208, `$L_4a77f0` 2,192, `$L_51c0a0` 2,000, `FUN_5cfb30` 1,920, `FUN_4e35cf` 1,840, `$L_4a9090` 1,744, `$L_4aee50` 1,584, `FUN_482d20` 1,536, `FUN_4a5db0` 1,520, `FUN_4b20d0` 1,504.

Batch 8 done: `FUN_4ebd00` (the vehicle constructor) was swapped for `$L_519800` (2,288, the network dialog), because its code continues past ENDP through x87-heavy split pieces. 19 procs, 44,310 bytes = 2.01%; 47.60% -> 49.62%. Drives and the load test passed.
