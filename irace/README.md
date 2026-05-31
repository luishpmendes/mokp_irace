# iRace Tuning for MOKP Solvers

This folder contains iRace configurations for **offline algorithm configuration** (parameter tuning) of multi-objective metaheuristics applied to the Multi-Objective Knapsack Problem (MOKP).

## Supported Algorithms

| Algorithm | Scenario File | Parameters File | Target Runner |
|-----------|---------------|-----------------|---------------|
| NSGA-II   | `nsga2-scenario.txt` | `nsga2-parameters.txt` | `nsga2-tunner.sh` |
| NS-BRKGA (legacy) | `nsbrkga-scenario.txt` | `nsbrkga-parameters.txt` | `nsbrkga-tunner.sh` |
| NS-BRKGA Stage 1 | `nsbrkga-scenario-stage1.txt` | `nsbrkga-parameters-stage1.txt` | `nsbrkga-tunner-stage1.sh` |
| NS-BRKGA Stage 2 | `nsbrkga-scenario-stage2.txt` | `nsbrkga-parameters-stage2.txt` | `nsbrkga-tunner-stage2.sh` |
| NS-BRKGA Stage 3 | `nsbrkga-scenario-stage3.txt` | `nsbrkga-parameters-stage3.txt` | `nsbrkga-tunner-stage3.sh` |
| NS-BRKGA Stage 4 | `nsbrkga-scenario-stage4.txt` | `nsbrkga-parameters-stage4.txt` | `nsbrkga-tunner-stage4.sh` |
| NS-BRKGA Stage 5 | `nsbrkga-scenario-stage5.txt` | `nsbrkga-parameters-stage5.txt` | `nsbrkga-tunner-stage5.sh` |
| NS-BRKGA Stage 6 | `nsbrkga-scenario-stage6.txt` | `nsbrkga-parameters-stage6.txt` | `nsbrkga-tunner-stage6.sh` |
| MOEA/D    | `moead-scenario.txt` | `moead-parameters.txt` | `moead-tunner.sh` |
| NSPSO     | `nspso-scenario.txt` | `nspso-parameters.txt` | `nspso-tunner.sh` |
| IHS       | `ihs-scenario.txt` | `ihs-parameters.txt` | `ihs-tunner.sh` |
| MHACO     | `mhaco-scenario.txt` | `mhaco-parameters.txt` | `mhaco-tunner.sh` |

## NS-BRKGA Ablation Study

The NS-BRKGA ablation study uses a **six-stage feature ladder** to incrementally unlock NS-BRKGA mechanisms. Each stage has its own parameter file, scenario file, and target runner:

| Stage | Feature Unlocked | Key Parameters Added |
|-------|-----------------|---------------------|
| 1 — Vanilla | Baseline: single pop, fixed elite size | `elites_percentage` (synthetic, mapped to min+max) |
| 2 — Dynamic Elite + Diversity | Dynamic elite sizing, diversity-aware selection | `min/max_elites_percentage`, `diversity_type` |
| 3 — Multiple Populations | Multi-population with exchange | `num_populations`, `exchange_interval`, `num_exchange_individuals` |
| 4 — Path Relinking | Path relinking between elites | `pr_type`, `pr_dist_func`, `pr_percentage`, `pr_interval` |
| 5 — Shaking | Population shaking | `shake_interval`, `shake_intensity`, `shake_distribution` |
| 6 — Full NS-BRKGA | Partial reset (all features on) | `reset_interval`, `reset_intensity` + `shake < reset` constraint |

### Instance Split

- **`train-instances.txt`** — Training instances for iRace tuning (same as `instances.txt`)
- **`test-instances.txt`** — Held-out instances for independent validation via `testing_fromlog()`

### Running the Ablation

Run all six stages (plus all other solvers) in parallel with testing:
```bash
cd irace
chmod +x *-tunner*.sh irace_runner.sh
./irace_runner.sh
```

Run a single stage:
```bash
cd irace
Rscript -e "library(irace); irace::irace_cmdline(c('--scenario','nsbrkga-scenario-stage1.txt'))"
```

Dry-run validation:
```bash
cd irace
irace --check --scenario nsbrkga-scenario-stage1.txt
```

## Folder Contents

- **`*-scenario.txt`** — iRace scenario configuration (paths, budget, log file)
- **`*-parameters.txt`** — Parameter space definition (name, switch, type, range, constraints)
- **`*-tunner.sh`** — Target runner script that iRace calls to evaluate a configuration
- **`instances.txt`** — List of training instance filenames (legacy, one per line)
- **`train-instances.txt`** — Training instances for staged ablation
- **`test-instances.txt`** — Held-out test instances for staged ablation
- **`irace_runner.sh`** — Orchestration script: parallel tuning + sequential testing

## Prerequisites

1. **R** with the `irace` package installed:
   ```bash
   Rscript -e "install.packages('irace', repos='https://cloud.r-project.org')"
   ```

2. **Compiled solver binaries** in `../bin/exec/`:
   - `nsga2_solver_exec`, `nsbrkga_solver_exec`, etc.
   - `hypervolume_calculator_exec`

3. **Training instances** in `../instances/` matching the names in `instances.txt`.

## How iRace Calls the Target Runner

iRace invokes the target runner with:
```
./target-runner <config_id> <instance_id> <seed> <instance_path> <params...>
```

- **`<instance_path>`** is formed as `trainInstancesDir/instance_name` (e.g., `../instances/kroAB100.txt`)
- Since `maxTime` is set, the runner **must** print two values: `cost time`
  - `cost`: Negative hypervolume (minimized by iRace → maximized HV)
  - `time`: Elapsed seconds (integer)

On failure, runners return a large penalty (`1e18`) to avoid crashing iRace.

## Quick Start

Run tuning from inside the `irace/` directory:

**NSGA-II:**
```bash
Rscript -e "library(irace); irace::irace_cmdline(c('--scenario','nsga2-scenario.txt'))"
```

**NS-BRKGA:**
```bash
Rscript -e "library(irace); irace::irace_cmdline(c('--scenario','nsbrkga-scenario.txt'))"
```

Optional: redirect output to a log:
```bash
Rscript -e "library(irace); irace::irace_cmdline(c('--scenario','nsga2-scenario.txt'))" 2>&1 | tee nsga2-tuning.log
```

## Notes on Parameter Files

### Format
Each parameter is defined as:
```
name  "switch"  type  (min, max)  [condition]
```
where `type` is: `i` (integer), `r` (real), `c` (categorical).

### NSGA-II Parameters
- **`population_size_factor`**: The runner multiplies this by 4 → actual `population_size` (range 100–500).
- Other params: `crossover_probability`, `crossover_distribution`, `mutation_probability`, `mutation_distribution`.

### NS-BRKGA Parameters
- Uses the same `population_size_factor` convention.
- **Forbidden constraints** (in `[forbidden]` section):
  - `min_elites_percentage >= max_elites_percentage` — rejected
  - `num_elite_parents > num_total_parents` — rejected
  - `shake_interval >= reset_interval` — rejected

## Output Artifacts

- **`irace-*.Rdata`**: The `logFile` containing all iRace results. Load in R to inspect elite configurations:
  ```r
  load("irace-nsga2.Rdata")
  print(iraceResults$allElites[[length(iraceResults$allElites)]])
  ```

- **Temporary folders** (`irace_nsga2/`, `irace_nsbrkga/`, etc.): Created during runs for Pareto files and hypervolume output; cleaned up automatically.

## Troubleshooting

| Issue | Fix |
|-------|-----|
| `Permission denied` | Make runner executable: `chmod +x *-tunner.sh` |
| `command not found` errors | Ensure solver binaries are built in `../bin/exec/` |
| iRace reports non-numeric output | Runner must print exactly `cost time` (two numbers) |
| `No such file or directory` | Check working directory; run from inside `irace/` |
| Instance not found | Verify entries in `instances.txt` match files in `../instances/` |