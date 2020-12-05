#pragma once

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fstream>
#include <regex>
#include <sstream>
#include <streambuf>

#include "peglib.h"
#include "table.h"

class Database {
 public:
  static void Initialize();

  Database(const std::string& filename = "manager.db");

  // CREATE
  void create_database(const std::string& database);
  void create_table(const std::string& table,
                    const std::vector<NamedType>& type_list);

  // DROP
  void drop_database(const std::string& database);
  void drop_table(const std::string& table);

  // USE
  void use_database(const std::string& database);

  // INSERT
  void insert_into_table(const std::string& table,
                         const std::vector<Literal>& values);

  // SELECT
  void select_table(const std::string& table,
                    const std::vector<std::string>& selectors);

  // UPDATE
  void update_table(const std::string& table,
                    const std::vector<FieldQuery>& update_list);

  // DELETE
  void delete_from(const std::string& table);

  // OPTIONS
  void options(const std::string& table,
               const std::vector<FieldQuery>& filters);

  void run(std::istream& file);

  ~Database();

 private:
  static peg::parser parser;
  static std::unordered_map<std::string, Table*> cached_tables;

  std::unordered_map<std::string, Table*> active_tables;

#pragma region QUERY_HANDLERS

  static Query* handle_query_selection(const peg::SemanticValues& sv);
  static std::vector<NamedType>* handle_type_list(
      const peg::SemanticValues& sv);
  static NamedType* handle_named_type(const peg::SemanticValues& sv);
  static Type* handle_type(const peg::SemanticValues& sv);
  static std::vector<Literal>* handle_value_list(const peg::SemanticValues& sv);
  static std::vector<std::string>* handle_selectors(
      const peg::SemanticValues& sv);
  static std::vector<FieldQuery>* handle_options(const peg::SemanticValues& sv);
  static std::vector<FieldQuery>* handle_query_list(
      const peg::SemanticValues& sv);
  static FieldQuery* handle_field_query(const peg::SemanticValues& sv);
  static Comparrison handle_comparrison(const peg::SemanticValues& sv);
  static Literal* handle_literal(const peg::SemanticValues& sv);
  static int handle_int_literal(const peg::SemanticValues& sv);
  static float handle_float_literal(const peg::SemanticValues& sv);
  static std::string* handle_string_literal(const peg::SemanticValues& sv);
  static std::string* handle_db_name(const peg::SemanticValues& sv);

#pragma endregion

#pragma region HELPERS

  bool database_exists(const std::string& path);
  bool table_exists(const std::string& path);
  bool table_active(const std::string& table);
  bool database_active();

#pragma endregion

  std::string active_db;
  const std::string DATABASE_PATH = "../databases/";
};