#include "database.h"

peg::parser Database::parser;
std::unordered_map<std::string, Table*> Database::cached_tables;

void Database::Initialize() {
  // SQL Grammar
  auto grammar = R"(
    start <- (line)+
    line <- comment [\n] / query_line ';' / '.exit' / [ \n\r\t]+
    query_line <- query options / '.exit' / '--' [^\n]*
    comment <- '--' [^\n]*
    query <- 'create' 'database' db_name 
      / 'create' 'table' db_name '(' type_list ')'
      / 'drop' 'database' db_name
      / 'drop' 'table' db_name
      / 'use' db_name
      / 'insert' 'into' db_name 'values' '(' value_list ')'
      / 'select' selectors 'from' db_name
      / 'update' db_name 'set' query_list
      / 'delete' 'from' db_name
    type_list <- named_type (',' named_type)*
    named_type <- db_name type
    type <- 'int' / 'float' / 'varchar' '(' int_literal ')'
    value_list <- literal (',' literal)*
    selectors <- db_name (',' db_name)* / '*'
    options <- 'where' query_list /
    query_list <- field_query (',' field_query)*
    field_query <- db_name comparrison literal
    comparrison <- < '=' > / < '<' > / < '>' > / < '!=' >
    literal <- float_literal / int_literal / string_literal
    float_literal <- < [0-9]*[.][0-9]+ >
    int_literal <- < [0-9]+ >
    string_literal <- < ['][^']+['] >
    db_name <- < [0-9a-zA-Z_]+ >
    %whitespace <- [ \t\n\r]*
  )";

  // Parser error logging
  parser.log = [](size_t line, size_t col, const std::string& msg) {
    std::cerr << line << ":" << col << ": " << msg << "\n";
  };

  parser.enable_packrat_parsing();

  bool ok = parser.load_grammar(grammar); // Throwing error

  assert(ok == true);

  parser["query"] = handle_query_selection;
  parser["type_list"] = handle_type_list;
  parser["named_type"] = handle_named_type;
  parser["type"] = handle_type;
  parser["value_list"] = handle_value_list;
  parser["selectors"] = handle_selectors;
  parser["options"] = handle_options;
  parser["query_list"] = handle_query_list;
  parser["field_query"] = handle_field_query;
  parser["comparrison"] = handle_comparrison;
  parser["literal"] = handle_literal;
  parser["int_literal"] = handle_int_literal;
  parser["float_literal"] = handle_float_literal;
  parser["string_literal"] = handle_string_literal;
  parser["db_name"] = handle_db_name;
}

// Functions to handle parsing of the grammar
#pragma region QUERY_HANDLERS

Query* Database::handle_query_selection(const peg::SemanticValues& sv) {
  Query* result = new Query();

  switch (sv.choice()) {
    case 0: {  // create database
      result->command = Query::Command::CREATE;
      result->object = Query::Object::DATABASE;
      result->name = *peg::any_cast<std::string*>(sv[0]);

      break;
    }
    case 1: {  // create table
      result->command = Query::Command::CREATE;
      result->object = Query::Object::TABLE;
      result->name = *peg::any_cast<std::string*>(sv[0]);

      result->type_list = *peg::any_cast<std::vector<NamedType>*>(sv[1]);
      break;
    }
    case 2: {  // drop database
      result->command = Query::Command::DROP;
      result->object = Query::Object::DATABASE;
      result->name = *peg::any_cast<std::string*>(sv[0]);
      break;
    }
    case 3: {  // drop table
      result->command = Query::Command::DROP;
      result->object = Query::Object::TABLE;
      result->name = *peg::any_cast<std::string*>(sv[0]);
      break;
    }
    case 4: {  // use table
      result->command = Query::Command::USE;
      result->object = Query::Object::DATABASE;
      result->name = *peg::any_cast<std::string*>(sv[0]);

      break;
    }
    case 5: {  // insert
      result->command = Query::Command::INSERT;
      result->object = Query::Object::TABLE;
      result->name = *peg::any_cast<std::string*>(sv[0]);
      result->value_list = *peg::any_cast<std::vector<Literal>*>(sv[1]);
      break;
    }
    case 6: {  // select
      result->command = Query::Command::SELECT;
      result->object = Query::Object::TABLE;
      result->name = *peg::any_cast<std::string*>(sv[1]);
      std::vector<std::string>* selectors =
          peg::any_cast<std::vector<std::string>*>(sv[0]);
      result->field_selectors =
          selectors == nullptr ? std::vector<std::string>() : *selectors;
      break;
    }
    case 7: {  // update
      result->command = Query::Command::UPDATE;
      result->object = Query::Object::TABLE;
      result->name = *peg::any_cast<std::string*>(sv[0]);

      result->update_fields = *peg::any_cast<std::vector<FieldQuery>*>(sv[1]);
      break;
    }
    case 8: {  // delete
      result->command = Query::Command::DELETE;
      result->object = Query::Object::TABLE;
      result->name = *peg::any_cast<std::string*>(sv[0]);
      break;
    }
  }
  return result;
}

std::vector<NamedType>* Database::handle_type_list(
    const peg::SemanticValues& sv) {
  std::vector<NamedType>* result = new std::vector<NamedType>();

  for (int i = 0; i < sv.size(); i++) {
    NamedType* type = peg::any_cast<NamedType*>(sv[i]);

    result->push_back(*type);
  }

  return result;
}

Type* Database::handle_type(const peg::SemanticValues& sv) {
  switch (sv.choice()) {
    case 0: {  // int
      return new Type{INT};
    }
    case 1: {  // float
      return new Type{FLOAT};
    }
    case 2: {  // varchar
      return new Type{VARCHAR, peg::any_cast<int>(sv[0])};
    }
    default: { return nullptr; }
  }
}

NamedType* Database::handle_named_type(const peg::SemanticValues& sv) {
  std::string* name = peg::any_cast<std::string*>(sv[0]);
  Type* type = peg::any_cast<Type*>(sv[1]);

  return new NamedType{*name, *type};
}

std::vector<Literal>* Database::handle_value_list(
    const peg::SemanticValues& sv) {
  std::vector<Literal>* result = new std::vector<Literal>();

  for (int i = 0; i < sv.size(); i++) {
    Literal* literal = peg::any_cast<Literal*>(sv[i]);

    result->push_back(*literal);
  }

  return result;
}

std::vector<std::string>* Database::handle_selectors(
    const peg::SemanticValues& sv) {
  std::vector<std::string>* result = new std::vector<std::string>();

  if (sv.choice() == 0) {
    for (int i = 0; i < sv.size(); i++) {
      std::string* name = peg::any_cast<std::string*>(sv[i]);
      result->push_back(*name);
    }
    return result;
  } else {
    return nullptr;
  }
}

std::vector<FieldQuery>* Database::handle_options(
    const peg::SemanticValues& sv) {
  if (sv.choice() == 0) {
    return peg::any_cast<std::vector<FieldQuery>*>(sv[0]);
  } else {
    return nullptr;
  }
}

std::vector<FieldQuery>* Database::handle_query_list(
    const peg::SemanticValues& sv) {
  std::vector<FieldQuery>* result = new std::vector<FieldQuery>();
  for (int i = 0; i < sv.size(); i++) {
    FieldQuery* query = peg::any_cast<FieldQuery*>(sv[i]);
    result->push_back(*query);
  }

  return result;
}

FieldQuery* Database::handle_field_query(const peg::SemanticValues& sv) {
  std::string* name = peg::any_cast<std::string*>(sv[0]);
  Comparrison comp = peg::any_cast<Comparrison>(sv[1]);
  Literal* literal = peg::any_cast<Literal*>(sv[2]);

  return new FieldQuery{*name, comp, *literal};
}

Comparrison Database::handle_comparrison(const peg::SemanticValues& sv) {
  switch (sv.choice()) {
    case 0: {
      return EQ;
    }
    case 1: {
      return LT;
    }
    case 2: {
      return GT;
    }
    default: { return NE; }
  }
}

Literal* Database::handle_literal(const peg::SemanticValues& sv) {
  PrimativeValue value;
  switch (sv.choice()) {
    case 0: {  // int
      value.fval = peg::any_cast<float>(sv[0]);
      return new Literal{value, Type{FLOAT}};
    }
    case 1: {  // float
      value.ival = peg::any_cast<int>(sv[0]);
      return new Literal{value, Type{INT}};
    }
    case 2: {  // string
      value.sval = peg::any_cast<std::string*>(sv[0]);
      return new Literal{value, Type{VARCHAR, (int)value.sval->size()}};
    }
    default: { return nullptr; }
  }
}

int Database::handle_int_literal(const peg::SemanticValues& sv) {
  return std::stoi(sv.token());
}

float Database::handle_float_literal(const peg::SemanticValues& sv) {
  return std::stof(sv.token());
}

std::string* Database::handle_string_literal(const peg::SemanticValues& sv) {
  std::string token = sv.token();
  return new std::string(token.begin() + 1, token.end() - 1);
}

std::string* Database::handle_db_name(const peg::SemanticValues& sv) {
  return new std::string(sv.token());
}

#pragma endregion

Database::Database(const std::string& filename) {
  // constructor
  std::fstream file(filename, std::ios::in);

  // Switches based on command and calls appropriate functions
  parser["query_line"] = [this](const peg::SemanticValues& sv) {
    if (sv.choice() == 0) {
      Query* query = peg::any_cast<Query*>(sv[0]);
      std::vector<FieldQuery>* filters =
          peg::any_cast<std::vector<FieldQuery>*>(sv[1]);

      if (filters != nullptr) {
        options(query->name, *filters);
      }

      switch (query->command) {
        case Query::Command::CREATE: {
          if (query->object == Query::Object::DATABASE) {
            this->create_database(query->name);
          } else {
            this->create_table(query->name, query->type_list);
          }
          break;
        }
        case Query::Command::DROP: {
          if (query->object == Query::Object::DATABASE) {
            this->drop_database(query->name);
          } else {
            this->drop_table(query->name);
          }
          break;
        }
        case Query::Command::USE: {
          this->use_database(query->name);
          break;
        }
        case Query::Command::INSERT: {
          this->insert_into_table(query->name, query->value_list);
          break;
        }
        case Query::Command::SELECT: {
          this->select_table(query->name, query->field_selectors);
          break;
        }
        case Query::Command::UPDATE: {
          this->update_table(query->name, query->update_fields);
          break;
        }
        case Query::Command::DELETE: {
          this->delete_from(query->name);
        }
      }
    }
  };
}

#pragma region CREATE
/*---------------------------------------------------------------------------------
 |  Function: create_database()
 |
 |  Purpose:  creates database named by third arg from user. Checks for
 duplicate |            file before creating as a directory
 |
 |  Parameters: database - the name of the database to create
 |
 |  Returns:  NONE
 *--------------------------------------------------------------------------------*/
void Database::create_database(const std::string& database) {
  std::string path = DATABASE_PATH + database;

  if (database_exists(path)) {
    std::cerr << "Database " << database << " already exists." << std::endl;
    return;
  }

  mkdir(path.c_str(), 0777);
  std::cout << "Creating database " << database << std::endl;
}

/*---------------------------------------------------------------------------------
 |  Function: create_table()
 |
 |  Purpose:  creates a table as a file under a database directory
 |
 |  Parameters: table - the name of the table to create givin user's third arg
 |              columns - creates columns within text file with parameter given
 |                        by user in parenthesis
 |  Returns:  NONE
 *--------------------------------------------------------------------------------*/
void Database::create_table(const std::string& table,
                            const std::vector<NamedType>& type_list) {
  std::string path = DATABASE_PATH + active_db + "/" + table;

  if (!database_active()) {
    std::cerr << "No active database selected." << std::endl;
    return;
  }

  if (table_exists(path)) {
    std::cerr << "Table " << table << " already exists." << std::endl;
    return;
  }

  try {
    Table* table_obj = new Table(type_list);
    cached_tables[table] = table_obj;
    table_obj->save(path);

    std::cout << "Created table " << table << std::endl;
  } catch (std::exception& e) {
    std::cerr << "Error while creating table: " << e.what() << std::endl;
  }
}

#pragma endregion

#pragma region DROP

/*---------------------------------------------------------------------------------
 |  Function: drop_database()
 |
 |  Purpose:  deletes existing database
 |
 |  Parameters: database - database to be deleted if it exists or there are no
 |                         tables inside of directory
 |
 |  Returns:  NONE
 *--------------------------------------------------------------------------------*/
void Database::drop_database(const std::string& database) {
  std::string path = DATABASE_PATH + database;

  if (!database_exists(path)) {
    std::cerr << "Database does not exist." << std::endl;
    return;
  }
  std::cout << "Attempting to drop database " << path << std::endl;
  if (rmdir(path.c_str()) == -1) {
    std::cerr << "Error: " << strerror(errno) << std::endl;
  } else {
    std::cout << "Dropping database: " << database << std::endl;
  }
}

/*---------------------------------------------------------------------------------
 |  Function: drop_table()
 |
 |  Purpose:  checks if in active database. If so, removes files("tables")
 |
 |  Parameters: table - the name of the table to check for existance
 |
 |  Returns:  NONE
 *--------------------------------------------------------------------------------*/
void Database::drop_table(const std::string& table) {
  std::string path = DATABASE_PATH + active_db + "/" + table;

  if (active_db.empty())
    std::cout << "No active Database " << std::endl;

  else {
    if (remove(path.c_str()) != 0)
      perror("Error deleting file ");
    else
      puts("File successfully deleted");
  }
}

#pragma endregion

#pragma region USE

/*---------------------------------------------------------------------------------
 |  Function: use_database()
 |
 |  Purpose:  sets active database if it exists for table access
 |
 |  Parameters: database - the name of the database to set as "active"
 |
 |  Returns:  NONE
 *--------------------------------------------------------------------------------*/
void Database::use_database(const std::string& database) {
  std::string path = DATABASE_PATH + database;

  if (!database_exists(path)) {
    std::cerr << "Database does not exist." << std::endl;
    return;
  }

  std::cout << "Using database " << database << std::endl;
  active_db = database;

  DIR* dir;
  struct dirent* ent;
  if ((dir = opendir(path.c_str())) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      if (cached_tables.find(ent->d_name) != cached_tables.end()) {
        active_tables[ent->d_name] = cached_tables[ent->d_name];
      } else {
        Table* table = new Table();
        table->load(path + "/" + ent->d_name);

        cached_tables[ent->d_name] = table;
        active_tables[ent->d_name] = table;
      }
    }
    closedir(dir);
  }
}

#pragma endregion

#pragma region INSERT

/*---------------------------------------------------------------------------------
 |  Function: insert_into_table()
 |
 |  Purpose:  Inserts rows into the selected table
 |
 |  Parameters: table - the selected table, values - the values being inserted
 |
 |  Returns:  NONE
 *--------------------------------------------------------------------------------*/
void Database::insert_into_table(const std::string& table,
                                 const std::vector<Literal>& values) {
  std::string path = DATABASE_PATH + active_db + "/" + table;

  if (!database_active()) {
    std::cerr << "No active database." << std::endl;
    return;
  }

  if (!table_active(table)) {
    if (!table_exists(path)) {
      std::cerr << "Table " << table << " does not exist." << std::endl;
      return;
    }

    Table* table_obj = new Table();
    table_obj->load(path);
    active_tables[table] = table_obj;
    cached_tables[table] = table_obj;
  }

  std::cout << "Inserting " << values << " into " << table << std::endl;
  Table* active_table = active_tables[table];
  active_table->insert(values);
  active_table->save(path);
}

#pragma endregion

#pragma region SELECT

/*---------------------------------------------------------------------------------
 |  Function: select table()
 |
 |  Purpose:  check to see if table already exists and theres an active
 database. |            If so, selects table and prints information in file to
 terminal
 |
 |  Parameters: table - the name of the table to check for existance
 |
 |  Returns:  NONE
 *--------------------------------------------------------------------------------*/
void Database::select_table(const std::string& table,
                            const std::vector<std::string>& selectors) {
  std::string path = DATABASE_PATH + active_db + "/" + table;

  if (!database_active()) {
    std::cerr << "No active database." << std::endl;
    return;
  }

  if (!table_active(table)) {
    if (!table_exists(path)) {
      std::cerr << "Table " << table << " does not exist." << std::endl;
      return;
    }

    Table* table_obj = new Table();
    table_obj->load(path);

    active_tables[table] = table_obj;
    cached_tables[table] = table_obj;
  }

  std::cout << "Selecting ";
  if (selectors.size()) {
    std::cout << "(";
    for (int i = 0; i < selectors.size(); i++) {
      std::cout << selectors[i];
      if (i != selectors.size() - 1) {
        std::cout << " ";
      }
    }
    std::cout << ")";
  } else {
    std::cout << "*";
  }
  std::cout << " from " << table << std::endl;

  Table* active_table = active_tables[table];
  std::vector<std::vector<Literal> > selection =
      active_table->select(selectors);

  for (const auto& i : selection) {
    std::cout << i << std::endl;
  }
}

#pragma endregion

#pragma region UPDATE

/*---------------------------------------------------------------------------------
 |  Function: update_table()
 |
 |  Purpose:  updates rows in the selected table
 |
 |  Parameters: table - the name of the table to edit
 |              update_list - the values to update
 |
 |  Returns:  NONE
 *--------------------------------------------------------------------------------*/
void Database::update_table(const std::string& table,
                            const std::vector<FieldQuery>& update_list) {
  std::string path = DATABASE_PATH + active_db + "/" + table;

  if (!database_active()) {
    std::cerr << "No active database." << std::endl;
    return;
  }

  if (!table_active(table)) {
    if (!table_exists(path)) {
      std::cerr << "Table " << table << " does not exist." << std::endl;
      return;
    }

    Table* table_obj = new Table();
    table_obj->load(path);

    active_tables[table] = table_obj;
    cached_tables[table] = table_obj;
  }

  std::cout << "Updating table " << table << " with " << update_list
            << std::endl;

  Table* active_table = active_tables[table];
  active_table->update(update_list);
  active_table->save(path);
}

#pragma endregion

#pragma region DELETE

/*---------------------------------------------------------------------------------
 |  Function: delete_from()
 |
 |  Purpose:  deletes rows based on filter
 |
 |  Parameters: table - the name of the table to edit
 |
 |  Returns:  NONE
 *--------------------------------------------------------------------------------*/
void Database::delete_from(const std::string& table) {
  std::string path = DATABASE_PATH + active_db + "/" + table;

  if (!database_active()) {
    std::cerr << "No active database." << std::endl;
    return;
  }

  if (!table_active(table)) {
    if (!table_exists(path)) {
      std::cerr << "Table " << table << " does not exist." << std::endl;
      return;
    }

    Table* table_obj = new Table();
    table_obj->load(path);

    active_tables[table] = table_obj;
    cached_tables[table] = table_obj;
  }

  std::cout << "Deleting from " << table << std::endl;

  Table* active_table = active_tables[table];
  active_table->delete_from();
  active_table->save(path);
}

#pragma endregion

#pragma region HELPERS

bool Database::database_exists(const std::string& path) {
  struct stat info;

  if (stat(path.c_str(), &info) != 0) {
    return false;
  } else if (info.st_mode & S_IFDIR) {
    return true;
  } else {
    return false;
  }
}

/*---------------------------------------------------------------------------------
 |  Function: table_exists()
 |
 |  Purpose:  check to see if table already exists
 |
 |  Parameters: table - the name of the table to check for existance
 |
 |  Returns:  True if table exists, false if the table does not exist
 *--------------------------------------------------------------------------------*/
bool Database::table_exists(const std::string& path) {
  struct stat info;
  return stat(path.c_str(), &info) == 0;
}

bool Database::table_active(const std::string& table) {
  if (active_tables.find(table) != active_tables.end()) {
    return true;
  }
  return false;
}

bool Database::database_active() { return !active_db.empty(); }

#pragma endregion

/*---------------------------------------------------------------------------------
 |  Function: options()
 |
 |  Purpose:  filters the selected table with the given filters
 |
 |  Parameters: table - the name of the table to edit, filters - the values to
 |                      filter by
 |
 |  Returns:  NONE
 *--------------------------------------------------------------------------------*/
void Database::options(const std::string& table,
                       const std::vector<FieldQuery>& filters) {
  std::string path = DATABASE_PATH + active_db + "/" + table;

  if (!database_active()) {
    std::cerr << "No active database." << std::endl;
    return;
  }

  if (!table_active(table)) {
    if (!table_exists(path)) {
      std::cerr << "Table " << table << " does not exist." << std::endl;
      return;
    }

    Table* table_obj = new Table();
    table_obj->load(path);

    active_tables[table] = table_obj;
    cached_tables[table] = table_obj;
  }

  Table* active_table = active_tables[table];
  active_table->filter(filters);
}

/*---------------------------------------------------------------------------------
 |  Function: run()
 |
 |  Purpose:  runs the database, parses input from file
 |
 |  Parameters: file  - istream object to stream from
 |
 |  Returns:  NONE
 *--------------------------------------------------------------------------------*/
void Database::run(std::istream& file) {
  std::string str((std::istreambuf_iterator<char>(file)),
                  std::istreambuf_iterator<char>());

  parser.parse(str.c_str());
}

Database::~Database() {
  // destructor
}