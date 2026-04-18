#ifndef CACHE_CLASS_H
#define CACHE_CLASS_H

#include <iostream>
#include <fstream>
#include <set>
#include <string>
using namespace std;

class Entry {
public:
  Entry();
  ~Entry();
  void display(ofstream& outfile);
  void set_tag(unsigned _tag) { tag = _tag; }
  unsigned get_tag() { return tag; }
  void set_valid(bool _valid) { valid = _valid; }
  bool get_valid() { return valid; }
  void set_ref(int _ref) { ref = _ref; }
  int get_ref() { return ref; }
private:
  bool valid;
  unsigned tag;
  int ref;
};

class Cache {
public:
  Cache(int num_entries, int assoc, int block_size = 1);
  ~Cache();
  void display(ofstream& outfile);
  int get_index(unsigned long addr);
  unsigned get_tag(unsigned long addr);
  unsigned long get_block_addr(unsigned long addr);

  bool hit(unsigned long addr);
  void update(unsigned long addr);

  int get_num_entries() { return num_entries; }
  int get_assoc() { return assoc; }
  int get_block_size() { return block_size; }
  int get_num_sets() { return num_sets; }

private:
  int assoc;
  unsigned num_entries;
  int num_sets;
  int block_size;
  int ref_counter;
  Entry **entries;
};

// Miss classification types
enum MissType { COMPULSORY, CAPACITY, CONFLICT };

string miss_type_str(MissType t);

#endif
