#pragma once

#include <iostream>
#include <string>
#include <vector>

enum PrimativeType { INT, FLOAT, VARCHAR };

enum Comparrison { LT, EQ, GT, NE };

struct Type {
  Type() = default;
  Type(PrimativeType _primative, int _length = 0)
      : primative(_primative), length(_length) {}
  PrimativeType primative;
  int length;
};

bool operator==(const Type& a, const Type& b);

struct NamedType {
  NamedType() = default;
  NamedType(const std::string& _name, Type _type) : name(_name), type(_type) {}
  Type type;
  std::string name;
};

std::ostream& operator<<(std::ostream& stream, const NamedType& type);
std::istream& operator>>(std::istream& stream, NamedType& type);

union PrimativeValue {
  int ival;
  float fval;
  std::string* sval;
};

struct Literal {
  Literal() = default;
  Literal(PrimativeValue _val, Type _type) : value(_val), type(_type) {}
  PrimativeValue value;
  Type type;
};

bool operator==(const Literal& a, const Literal& b);
bool operator<(const Literal& a, const Literal& b);
bool operator>(const Literal& a, const Literal& b);
bool operator!=(const Literal& a, const Literal& b);

std::ostream& operator<<(std::ostream& stream, const Literal& literal);
std::ostream& operator<<(std::ostream& stream,
                         const std::vector<Literal>& values);
std::istream& operator>>(std::istream& stream, Literal& literal);

struct FieldQuery {
  FieldQuery() = default;
  FieldQuery(const FieldQuery& other) {
    name = other.name;
    comparrison = other.comparrison;
    literal = other.literal;
  }
  FieldQuery(std::string _name, Comparrison comp, Literal _literal)
      : name(_name), comparrison(comp), literal(_literal) {}
  std::string name;
  Comparrison comparrison;
  Literal literal;
};

std::ostream& operator<<(std::ostream& stream,
                         const std::vector<FieldQuery>& list);
struct Query {
  enum Command { CREATE, DROP, USE, INSERT, SELECT, UPDATE, DELETE } command;

  enum Object {
    TABLE,
    DATABASE,
  } object;

  std::string name;
  std::vector<NamedType> type_list;
  std::vector<Literal> value_list;
  std::vector<std::string> field_selectors;
  std::vector<FieldQuery> update_fields;
  std::vector<FieldQuery> filter_fields;
};