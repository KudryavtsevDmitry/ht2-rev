# Game code conversion batches

Counts come from `py tools/conversion_inventory.py`, using original addresses
and the game units in `units.csv`. MFC, C runtime, compression libraries,
iostreams and import thunks are excluded. Exception funclets outside the game
address range still need an ownership audit. Counts measure procedures, not
their size, and include game-specific container instantiations.

Every completed batch is committed locally without pushing. `truck.ini` is
machine-local and is excluded from these commits.

| Batch | Procedures converted | Game procedures still in MASM | Validation |
|---|---:|---:|---|
| `a35b054`: scene state and collision helpers | 4 | 5,241 | Clean C++ rebuild; MASM engine fallback; New Game and clean close after 10 seconds |
| `edbf3ea`: scene commands and texture animation | 5 | 5,236 | Clean C++ rebuild; MASM engine fallback; 108 texture comparisons; New Game and clean close after 10 seconds |
| Effect drawing, fading, quad animation and movement; game exit hook | 6 | 5,230 | C++ build; MASM effects/application fallback; 108 quad and 108 texture comparisons; New Game and clean closes after 10 and 11 seconds |
| Scheme input Environment/Port factories and value-list range erasure | 3 | 5,227 | C++ build; MASM evaluator/item fallback; list-erasure comparisons covering immediate and shared values; New Game and clean close after 10 seconds. All procedures assigned to `sch_istr.cpp` are converted. |
| AI task shuffle initialization and screen-message buffer lifetime | 5 | 5,222 | C++ build; MASM AI task fallback; 55-word shuffle state and index comparison after three resets; New Game and clean close after 10 seconds. All procedures assigned to `AI_TASK.CPP` are converted. |

The effect batch exposed an intermittent shutdown fault. A debugger trace
showed the task pool freeing a queued node before the application destructor
used it. The game now destroys its views in its ExitInstance hook, before
static pool teardown. The MFC ExitInstance and the C runtime remain intact.
The smoke test now fails on a nonzero process exit code.
