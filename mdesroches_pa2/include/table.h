#pragma once

#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>

#include "types.h"

struct TableRow {};

class Table {
 public:
  Table();
  Table(const std::vector<NamedType>& types);

  void load(const std::string& filename);
  void save(const std::string& filename);
  void insert(const std::vector<Literal>& values);
  std::vector<std::vector<Literal> > select(
      const std::vector<std::string>& selectors);
  void update(const std::vector<FieldQuery>& update_list);
  void delete_from();

  void filter(const std::vector<FieldQuery>& filters);

 private:
  std::vector<NamedType> header;
  std::unordered_map<int, std::vector<Literal> > rows;
  std::vector<int> filtered_pids;
};