#include "cache_class.h"
#include <climits>

// ---- Entry implementation ----

Entry::Entry() {
  valid = false;
  tag = 0;
  ref = 0;
}

Entry::~Entry() {}

void Entry::display(ofstream& outfile) {
  outfile << "V: " << valid << " Tag: " << tag << " Ref: " << ref << endl;
}

// ---- Cache implementation ----

Cache::Cache(int _num_entries, int _assoc, int _block_size) {
  num_entries = _num_entries;
  assoc = _assoc;
  block_size = _block_size;
  num_sets = num_entries / assoc;
  ref_counter = 0;

  // Allocate 2D array: entries[way][set_index]
  entries = new Entry*[assoc];
  for (int i = 0; i < assoc; i++) {
    entries[i] = new Entry[num_sets];
  }
}

Cache::~Cache() {
  for (int i = 0; i < assoc; i++) {
    delete[] entries[i];
  }
  delete[] entries;
}

void Cache::display(ofstream& outfile) {
  for (int i = 0; i < assoc; i++) {
    outfile << "Way " << i << ":" << endl;
    for (int j = 0; j < num_sets; j++) {
      outfile << "  Set " << j << " -> ";
      entries[i][j].display(outfile);
    }
  }
}

unsigned long Cache::get_block_addr(unsigned long addr) {
  // Strip block offset: divide by block_size
  return addr / block_size;
}

int Cache::get_index(unsigned long addr) {
  unsigned long block_addr = get_block_addr(addr);
  return block_addr % num_sets;
}

unsigned Cache::get_tag(unsigned long addr) {
  unsigned long block_addr = get_block_addr(addr);
  return block_addr / num_sets;
}

bool Cache::hit(unsigned long addr) {
  int index = get_index(addr);
  unsigned tag = get_tag(addr);

  // Search all ways in this set for a matching valid entry
  for (int i = 0; i < assoc; i++) {
    if (entries[i][index].get_valid() && entries[i][index].get_tag() == tag) {
      return true;
    }
  }
  return false;
}

void Cache::update(unsigned long addr) {
  int index = get_index(addr);
  unsigned tag = get_tag(addr);

  ref_counter++;

  // If already in cache (hit), just update ref
  for (int i = 0; i < assoc; i++) {
    if (entries[i][index].get_valid() && entries[i][index].get_tag() == tag) {
      entries[i][index].set_ref(ref_counter);
      return;
    }
  }

  // Miss — look for an invalid (empty) slot first
  for (int i = 0; i < assoc; i++) {
    if (!entries[i][index].get_valid()) {
      entries[i][index].set_valid(true);
      entries[i][index].set_tag(tag);
      entries[i][index].set_ref(ref_counter);
      return;
    }
  }

  // All ways valid — evict LRU (lowest ref value)
  int lru_way = 0;
  int min_ref = entries[0][index].get_ref();
  for (int i = 1; i < assoc; i++) {
    if (entries[i][index].get_ref() < min_ref) {
      min_ref = entries[i][index].get_ref();
      lru_way = i;
    }
  }

  entries[lru_way][index].set_tag(tag);
  entries[lru_way][index].set_ref(ref_counter);
}

// ---- Miss classification helper ----

string miss_type_str(MissType t) {
  switch (t) {
    case COMPULSORY: return "COMPULSORY";
    case CAPACITY:   return "CAPACITY";
    case CONFLICT:   return "CONFLICT";
  }
  return "UNKNOWN";
}
