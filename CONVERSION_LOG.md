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

Tests per batch: C++ build, new-game drives and a load-game run. `Player_Save` is not covered (no save is written).
