#include "cache_class.h"
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>
#include <set>
#include <sstream>
#include <string>
using namespace std;

void print_usage() {
  cout << "Usage:" << endl;
  cout << "  ./cache_sim num_entries associativity filename [options]" << endl;
  cout << endl;
  cout << "Options:" << endl;
  cout << "  -b block_size          Set block size in words (default: 1)" << endl;
  cout << "  -l2 L2_entries L2_assoc  Add an L2 cache" << endl;
  cout << "  -c                     Classify misses (compulsory/capacity/conflict)" << endl;
}

int main(int argc, char* argv[]) {
  if (argc < 4) {
    print_usage();
    return 0;
  }

  /* required args */
  unsigned entries = atoi(argv[1]);
  unsigned assoc = atoi(argv[2]);
  string input_filename = argv[3];

  /* optional args */
  int block_size = 1;
  bool use_l2 = false;
  unsigned l2_entries = 0;
  unsigned l2_assoc = 0;
  bool classify = false;

  for (int i = 4; i < argc; i++) {
    if (strcmp(argv[i], "-b") == 0 && i + 1 < argc) {
      block_size = atoi(argv[++i]);
    } else if (strcmp(argv[i], "-l2") == 0 && i + 2 < argc) {
      use_l2 = true;
      l2_entries = atoi(argv[++i]);
      l2_assoc = atoi(argv[++i]);
    } else if (strcmp(argv[i], "-c") == 0) {
      classify = true;
    }
  }

  /* print config to stdout (informational only) */
  cout << "=== Cache Simulator Configuration ===" << endl;
  cout << "L1 Entries: " << entries << ", Associativity: " << assoc
       << ", Block Size: " << block_size << endl;
  if (use_l2)
    cout << "L2 Entries: " << l2_entries << ", Associativity: " << l2_assoc << endl;
  if (classify)
    cout << "Miss classification: ENABLED" << endl;
  cout << "Input File: " << input_filename << endl;

  /* open input file */
  ifstream input;
  input.open(input_filename);
  if (!input.is_open()) {
    cerr << "Could not open input file " << input_filename << ". Exiting ..." << endl;
    exit(1);
  }

  /* read all memory addresses — supports space-separated and/or newline-separated,
     and both decimal and hex (0x prefix) */
  vector<unsigned long> addresses;
  string line;
  while (getline(input, line)) {
    if (line.empty()) continue;
    istringstream iss(line);
    string token;
    while (iss >> token) {
      unsigned long addr = strtoul(token.c_str(), nullptr, 0);
      addresses.push_back(addr);
    }
  }
  input.close();

  /* create L1 cache */
  Cache L1(entries, assoc, block_size);

  /* create L2 cache if requested */
  Cache* L2 = nullptr;
  if (use_l2) {
    L2 = new Cache(l2_entries, l2_assoc, block_size);
  }

  /*
   * For miss classification, we need:
   * 1) A set of all block addresses seen so far (to detect compulsory misses)
   * 2) A fully-associative cache with the same number of entries as L1
   *    (to distinguish capacity vs conflict misses)
   *    - If the FA cache also misses -> capacity miss
   *    - If the FA cache hits but real cache misses -> conflict miss
   */
  Cache* fa_cache = nullptr;
  set<unsigned long> seen_blocks;
  if (classify) {
    fa_cache = new Cache(entries, entries, block_size);
  }

  /* open output file */
  ofstream outfile("cache_sim_output");
  if (!outfile.is_open()) {
    cerr << "Could not open output file cache_sim_output. Exiting ..." << endl;
    exit(1);
  }

  /* simulate each memory reference — write results to cache_sim_output */
  for (int i = 0; i < (int)addresses.size(); i++) {
    unsigned long a = addresses[i];
    bool l1_hit = L1.hit(a);

    if (l1_hit) {
      // L1 HIT
      outfile << a << " : HIT";
      L1.update(a);
      if (use_l2) {
        L2->update(a);
      }
    } else {
      // L1 MISS
      outfile << a << " : MISS";

      // Classify the miss if enabled
      if (classify) {
        unsigned long block_addr = L1.get_block_addr(a);
        if (seen_blocks.find(block_addr) == seen_blocks.end()) {
          outfile << " (COMPULSORY)";
        } else if (!fa_cache->hit(a)) {
          outfile << " (CAPACITY)";
        } else {
          outfile << " (CONFLICT)";
        }
      }

      L1.update(a);

      // Check L2 on L1 miss
      if (use_l2) {
        bool l2_hit = L2->hit(a);
        if (l2_hit) {
          outfile << " L2:HIT";
        } else {
          outfile << " L2:MISS";
        }
        L2->update(a);
      }
    }

    outfile << endl;

    // Track seen blocks for compulsory detection
    if (classify) {
      unsigned long block_addr = L1.get_block_addr(a);
      seen_blocks.insert(block_addr);
      fa_cache->update(a);
    }
  }

  outfile.close();
  cout << "Output written to cache_sim_output" << endl;

  /* cleanup */
  if (L2) delete L2;
  if (fa_cache) delete fa_cache;

  return 0;
}
