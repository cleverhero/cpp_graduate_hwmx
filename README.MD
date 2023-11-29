# HWMX homework project. CPP-Graduate cource.

#### Building Manually

```bash
❯ rm -rf build && mkdir build
❯ cd build
❯ cmake ..
❯ cmake --build .
```

#### Running the tests

```bash
❯ ctest
```

#### Running the CLI Executable

```bash
❯ ./det
❯ 2 42 1 1 1
```

```bash
❯ ./optimal_order
❯ 5 30 35 15 5 10
```

#### Running benchmark

```bash
❯ ./benchmarks/base_vs_lazy {N} {K}
```

```bash
❯ ./benchmarks/matrix_chain_bench {N} {K} {MAX_SIZE}
```
