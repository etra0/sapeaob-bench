# Benchmark of sapeAOB.

![](https://github.com/etra0/sapeaob-bench/blob/master/output.png)

The benchmarking process is rather a naive one. We generate a 10MB array with a signature placed anywhere, then run the `sapeaob-benchmark` with that data.
This benchmark include other two libraries:

* otis.h: [Frans Bouma](github.com/FransBouma/)'s implementation which he happily provided to me to do this benchmark, Thanks a lot Frans!
* [ModUtils](https://github.com/CookiePLMonster/ModUtils): Silent's implementation to pattern searching. 

# Doing the experiment yourself.
### 0. Cloning the repo
Make sure you're cloning with the recursive option or update the submodules after. You can do this by using `git clone --recursive <REPOSITORY>`.

### 1. Generate the data
First, you need to generate the random data with the signature in a random place, to do this, you can do
```bash
cd data
python generate_data.py
```
This requires Python 3.9+.

### 2. Building

This project uses Cmake. You can either use Visual Studio or do it in the terminal by:
```bash
mkdir build && cd build
cmake .. -GNinja
ninja
```

### 3. Generating the plot

You can also generate the plot. This requires `pandas`, `matplotlib` and `seaborn` to be installed in your current python environment.
Then, you can do
```bash
python tools/plot.py <DIRECTORY_OF_THE_BENCHMARK_BINARY>
# For example
python tools/plot.py builds/src/sapeaob-benchmark.exe
```
