# CS3339 Homework 3 — Cache Simulator

**Author:** Dhrubo  
**Course:** CS3339, Spring 2026

## Description

A cache simulator written in C++ that models the behavior of a CPU cache. It takes cache configuration parameters and a sequence of memory address references, then reports whether each reference is a hit or miss.

## Building

```bash
make
```

This produces the `cache_sim` executable.

To clean build artifacts:

```bash
make clean
```

## Usage

### Basic usage

```bash
./cache_sim num_entries associativity input_file
```

- `num_entries` — total number of cache entries (must be a power of two)
- `associativity` — cache associativity (must evenly divide num_entries)
- `input_file` — text file with space-separated word addresses

### Example

```bash
echo "1 3 5 1 3 1" > input0
./cache_sim 4 2 input0
cat cache_sim_output
```

Output:

```
1 : MISS
3 : MISS
5 : MISS
1 : MISS
3 : MISS
1 : HIT
```

### Extra Credit Options

```bash
./cache_sim num_entries associativity input_file [options]
```

| Option | Description |
|--------|-------------|
| `-b block_size` | Set block size in words (default: 1) |
| `-l2 L2_entries L2_assoc` | Enable L2 cache with given configuration |
| `-c` | Classify misses as compulsory, capacity, or conflict |

#### Multi-word blocks

```bash
./cache_sim 4 2 input_file -b 4
```

#### Miss classification

```bash
./cache_sim 4 2 input_file -c
```

#### Multi-level cache

```bash
./cache_sim 4 2 input_file -l2 16 4
```

#### All options combined

```bash
./cache_sim 4 2 input_file -b 2 -l2 16 4 -c
```

## Output

Results are written to `cache_sim_output`. Each line has the format:

```
[ADDR] : [HIT or MISS]
```

With extra credit enabled, miss lines may include classification and L2 status:

```
[ADDR] : MISS (COMPULSORY) L2:MISS
```

## Files

- `main.cpp` — entry point, argument parsing, simulation loop
- `cache_class.h` — class declarations for Entry and Cache
- `cache_class.cpp` — class implementations (LRU replacement, miss classification)
- `Makefile` — build configuration
