# Game code conversion batches

Counts come from `py tools/conversion_inventory.py`, using original addresses
and the game units in `units.csv`. MFC, C runtime, compression libraries,
iostreams and import thunks are excluded. Exception funclets outside the game
address range still need an ownership audit. Counts measure procedures, not
their size, and include game-specific container instantiations.

Every completed batch is committed locally without pushing. `truck.ini` is
machine-local and is excluded from these commits.

| Batch | Procedures converted | Game procedures still in MASM | Role in the game | Validation |
|---|---:|---:|---|---|
| `a35b054`: scene state and collision helpers | 4 | 5,241 | Saves scene object state and manages collision pairs and shapes used by the engine. | Clean C++ rebuild; MASM engine fallback; New Game and clean close after 10 seconds |
| `edbf3ea`: scene commands and texture animation | 5 | 5,236 | Routes scene commands and advances frames of animated textures. | Clean C++ rebuild; MASM engine fallback; 108 texture comparisons; New Game and clean close after 10 seconds |
| Effect drawing, fading, quad animation and movement; game exit hook | 6 | 5,230 | Draws and fades visual effects, moves animated quads, and destroys game views before shutdown. | C++ build; MASM effects/application fallback; 108 quad and 108 texture comparisons; New Game and clean closes after 10 and 11 seconds |
| Scheme input Environment/Port factories and value-list range erasure | 3 | 5,227 | Creates script input environments and ports; removes script values while maintaining reference counts. | C++ build; MASM evaluator/item fallback; list-erasure comparisons covering immediate and shared values; New Game and clean close after 10 seconds. All procedures assigned to `sch_istr.cpp` are converted. |
| AI task shuffle initialization and screen-message buffer lifetime | 5 | 5,222 | Initializes AI shuffle state and allocates or releases the buffer for game screen messages. | C++ build; MASM AI task fallback; 55-word shuffle state and index comparison after three resets; New Game and clean close after 10 seconds. All procedures assigned to `AI_TASK.CPP` are converted. |
| Marine mesh allocation, initialization, budgeted update and node removal | 4 | 5,218 | Creates marine mesh buffers, updates mesh work within a time budget, and removes triangle nodes and their neighbor links. | Clean C++ rebuild; MASM marine fallback; 384 node-removal comparisons; New Game and clean close after 10 seconds. All procedures assigned to `Greed.cpp` are converted. |
| Marine priority refresh, refinement scheduling and mesh output | 3 | 5,215 | Updates marine mesh priorities, schedules refinement work, and emits mesh vertices and triangles to its callback sink. | C++ build; MASM marine fallback; 1,536 comparisons covering node removal, priority callbacks, timer wraparound, NaN scores, refinement limits and mesh output; New Game and clean close after 10 seconds. All procedures in the unit at `0x60cfa0` are converted. |
| Marine queue priority replacement and node/edge selection | 4 | 5,211 | Replaces marine node and edge priorities and chooses which queued element to process next. | C++ build; MASM marine fallback; 512 selection and 48 priority-update comparisons covering equal and unordered scores, signed zero, infinities, callback order and count wraparound; 1,536 marine regressions; New Game and clean close after 10 seconds. |
| Marine intrusive node removal and node/edge insertion | 3 | 5,208 | Keeps marine node and edge queue links correct when elements are inserted, reprioritized or removed. | C++ build; MASM marine fallback; 756 insertion and 84 removal comparisons covering circular lists, both roots, unordered priorities and rebuild thresholds; 2,096 earlier marine regressions; New Game and clean close after 10 seconds. |
| Marine node/edge queue rebuild and sorted-run merging | 2 | 5,206 | Merges the marine subsystem's sorted queues after their update thresholds, preserving priority order. | C++ build; MASM marine fallback; 1,296 full-state comparisons covering empty lists, sorted runs, ties, infinities and unordered scores; 2,936 earlier marine regressions; New Game and clean close after 10 seconds. |
| Marine task mode, activation, pending values, registration and bounds | 8 | 5,198 | Controls marine task modes, activation delays, pending values, registration flags and stored bounds. | C++ build; MASM marine fallback; 1,024 full-state and return-value comparisons covering load thresholds, immediate/delayed activation and deactivation, mode values and bounds; 1,296 rebuild regressions; New Game and clean close after 10 seconds. |
| Marine point buffers, bounds setup, callback forwarding and mesh release | 10 | 5,188 | Builds marine point buffers, updates bounds, forwards geometry callbacks, and releases the task's mesh. | C++ build; MASM marine fallback; 320 comparisons covering allocation failure, callback-driven mesh replacement, bounds flags, point copies including signaling NaNs, and null/non-null release; 1,024 state regressions; New Game and clean close after 10 seconds. |
| Marine unused-vertex cleanup and edge draining | 2 | 5,186 | Removes vertices no longer referenced by triangles, compacts the vertex table and repairs moved indices; processes queued marine edges until none remain. | C++ build; MASM marine fallback; 1,533 vertex-reference/allocation-failure comparisons and 9 edge-draining comparisons; 320 callback regressions; New Game and clean close after 10 seconds. |

The effect batch exposed an intermittent shutdown fault. A debugger trace
showed the task pool freeing a queued node before the application destructor
used it. The game now destroys its views in its ExitInstance hook, before
static pool teardown. The MFC ExitInstance and the C runtime remain intact.
The smoke test now fails on a nonzero process exit code.
