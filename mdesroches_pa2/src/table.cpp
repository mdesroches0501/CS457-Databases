#include "table.h"

Table::Table() {}

Table::Table(const std::vector<NamedType>& types) { header = types; }

/*---------------------------------------------------------------------------------
 |  Function: load()
 |
 |  Purpose:  loads input from file and parses it into object
 |
 |  Parameters: filename - the filename of the table
 |
 |  Returns:  NONE
 *--------------------------------------------------------------------------------*/
void Table::load(const std::string& filename) {
  std::fstream file(filename, std::ios::in);

  std::string line;

  {  // header
    std::getline(file, line);
    std::stringstream stream(line);
    if (line != "\n" && !line.empty()) {
      line = std::string(line.begin(), line.end() - 1);
      while (stream.good()) {
        NamedType type;
        stream >> type;
        header.push_back(type);
      }
    }
  }

  {  // rows
    while (file.good()) {
      std::getline(file, line);
      std::stringstream stream(line);
      if (line == "\n" || line.empty()) {
        break;
      }

      std::vector<Literal> row;
      while (stream.good()) {
        Literal value;
        stream >> value;

        row.push_back(value);
      }

      rows[row[0].value.ival] = row;
    }
  }
  file.close();
}

/*---------------------------------------------------------------------------------
 |  Function: save()
 |
 |  Purpose: saves the data stored in the object to the selected file
 |
 |  Parameters: filename - the name of the file to store the table data to
 |
 |  Returns:  NONE
 *--------------------------------------------------------------------------------*/
void Table::save(const std::string& filename) {
  std::fstream file(filename, std::ios::out);

  for (int i = 0; i < header.size(); i++) {
    file << header[i];
    if (i != header.size() - 1) {
      file << " ";
    }
  }

  file << std::endl;

  for (const auto& row : rows) {
    for (int i = 0; i < row.second.size(); i++) {
      file << row.second[i];
      if (i != row.second.size() - 1) {
        file << " ";
      }
    }
    file << std::endl;
  }
  file.close();
}

/*---------------------------------------------------------------------------------
 |  Function: insert()
 |
 |  Purpose:  insert values into the table object
 |
 |  Parameters: values - values to insert
 |
 |  Returns:  NONE
 *--------------------------------------------------------------------------------*/
void Table::insert(const std::vector<Literal>& values) {
  int pid = values[0].value.ival;

  int index = 0;
  for (const auto& type : header) {
    if (!(values[index].type == type.type)) {
      std::cerr << "Incompatible type." << std::endl;
      return;
    }
    ++index;
  }

  rows[pid] = values;
}

std::vector<std::vector<Literal> > Table::select(
    const std::vector<std::string>& selectors) {
  std::vector<std::vector<Literal> > result;

  std::vector<int> indices;
  int i = 0;
  for (const auto& head : header) {
    if (selectors.empty() || std::find(selectors.begin(), selectors.end(),
                                       head.name) != selectors.end()) {
      indices.push_back(i);
    }
    ++i;
  }

  for (const auto& row : rows) {
    std::vector<Literal> row_selection;
    for (int index : indices) {
      row_selection.push_back(row.second[index]);
    }
    result.push_back(row_selection);
  }

  return result;
}

/*---------------------------------------------------------------------------------
 |  Function: update()
 |
 |  Purpose:  updates filtered rows with selected values
 |
 |  Parameters: update_list - the list of values to update with
 |
 |  Returns:  NONE
 *--------------------------------------------------------------------------------*/
void Table::update(const std::vector<FieldQuery>& update_list) {
  std::unordered_map<std::string, Literal> map;
  for (const auto& i : update_list) {
    map[i.name] = i.literal;
  }

  std::vector<int> indices;
  {
    int i = 0;
    for (const auto& head : header) {
      if (map.find(head.name) != map.end()) {
        indices.push_back(i);
      }
      ++i;
    }
  }

  for (int i : filtered_pids) {
    for (int index : indices) {
      rows[i][index] = map[header[index].name];
    }
  }
}

void Table::delete_from() {
  for (int i : filtered_pids) {
    rows.erase(i);
  }
}

/*---------------------------------------------------------------------------------
 |  Function: filter()
 |
 |  Purpose:  filter the rows based on filters passed to the function
 |
 |  Parameters: filters - the values to filter by
 |
 |  Returns:  NONE
 *--------------------------------------------------------------------------------*/
void Table::filter(const std::vector<FieldQuery>& filters) {
  std::unordered_map<std::string, FieldQuery> filter_map;

  for (auto& i : filters) {
    filter_map[i.name] = i;
  }

  filtered_pids.clear();

  for (const auto& row : rows) {
    bool should_filter = true;
    for (int i = 0; i < row.second.size(); i++) {
      if (filter_map.find(header[i].name) != filter_map.end()) {
        FieldQuery filter = filter_map[header[i].name];
        switch (filter.comparrison) {
          case EQ: {
            should_filter &= row.second[i] == filter.literal;
            break;
          }
          case LT: {
            should_filter &= row.second[i] < filter.literal;
            break;
          }
          case GT: {
            should_filter &= row.second[i] > filter.literal;
            break;
          }
          case NE: {
            should_filter &= row.second[i] != filter.literal;
            break;
          }
        }
      }
    }
    if (should_filter) {
      filtered_pids.push_back(row.first);
    }
  }
}

#pragma region TYPE_OPERATORS

bool operator==(const Type& a, const Type& b) {
  if (a.primative == VARCHAR) {
    return b.primative == VARCHAR && a.length <= b.length;
  }

  if (a.primative == INT) {
    return b.primative == INT || b.primative == FLOAT;
  }
  return b.primative == b.primative;
}

bool operator==(const Literal& a, const Literal& b) {
  float b_val = 0;
  if (b.type.primative == INT) {
    b_val = b.value.ival;
  }
  if (b.type.primative == FLOAT) {
    b_val = b.value.fval;
  }

  switch (a.type.primative) {
    case INT: {
      return a.value.ival == b_val;
    }
    case FLOAT: {
      return a.value.fval == b_val;
    }
    case VARCHAR: {
      return *a.value.sval == *b.value.sval;
    }
  }
  return false;
}

bool operator<(const Literal& a, const Literal& b) {
  float b_val = 0;
  if (b.type.primative == INT) {
    b_val = b.value.ival;
  }
  if (b.type.primative == FLOAT) {
    b_val = b.value.fval;
  }

  switch (a.type.primative) {
    case INT: {
      return a.value.ival < b_val;
    }
    case FLOAT: {
      return a.value.fval < b_val;
    }
    case VARCHAR: {
      return *a.value.sval < *b.value.sval;
    }
  }
  return false;
}

bool operator>(const Literal& a, const Literal& b) {
  float b_val = 0;
  if (b.type.primative == INT) {
    b_val = b.value.ival;
  }
  if (b.type.primative == FLOAT) {
    b_val = b.value.fval;
  }

  switch (a.type.primative) {
    case INT: {
      return a.value.ival > b_val;
    }
    case FLOAT: {
      return a.value.fval > b_val;
    }
    case VARCHAR: {
      return *a.value.sval > *b.value.sval;
    }
  }
  return false;
}

bool operator!=(const Literal& a, const Literal& b) {
  float b_val = 0;
  if (b.type.primative == INT) {
    b_val = b.value.ival;
  }
  if (b.type.primative == FLOAT) {
    b_val = b.value.fval;
  }

  switch (a.type.primative) {
    case INT: {
      return a.value.ival != b_val;
    }
    case FLOAT: {
      return a.value.fval != b_val;
    }
    case VARCHAR: {
      return *a.value.sval != *b.value.sval;
    }
  }
  return false;
}

std::ostream& operator<<(std::ostream& stream, const NamedType& type) {
  stream << type.name << " " << type.type.primative << " " << type.type.length;

  return stream;
}

std::istream& operator>>(std::istream& stream, NamedType& type) {
  stream >> type.name;

  unsigned u;
  stream >> u;

  type.type.primative = (PrimativeType)u;

  stream >> type.type.length;

  return stream;
}

std::ostream& operator<<(std::ostream& stream, const Literal& literal) {
  switch (literal.type.primative) {
    case INT: {
      stream << literal.value.ival;
      break;
    }
    case FLOAT: {
      stream << literal.value.fval;
      break;
    }
    case VARCHAR: {
      stream << '\'' << *literal.value.sval << '\'';
      break;
    }
  }

  return stream;
}

std::ostream& operator<<(std::ostream& stream,
                         const std::vector<Literal>& values) {
  stream << "( ";
  for (const auto& i : values) {
    stream << i << " ";
  }
  stream << ")";

  return stream;
}

std::istream& operator>>(std::istream& stream, Literal& literal) {
  std::string value;
  stream >> value;

  if (value.find_first_not_of("0123456789") == std::string::npos) {
    literal.type = Type{INT};
    literal.value.ival = std::stoi(value, nullptr, 10);
  } else if (value.find_first_not_of(".0123456789") == std::string::npos) {
    literal.type = Type{FLOAT};
    literal.value.fval = std::stof(value);
  } else {
    literal.type = Type{VARCHAR, (int)(value.size() - 2)};
    literal.value.sval = new std::string(value.begin() + 1, value.end() - 1);
  }

  return stream;
}

std::ostream& operator<<(std::ostream& stream,
                         const std::vector<FieldQuery>& list) {
  for (const auto& i : list) {
    stream << i.name << " = " << i.literal << " ";
  }
  return stream;
}
#pragma endregion