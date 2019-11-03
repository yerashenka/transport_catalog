#pragma once

#include "utils.h"
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace Json {

  class Node;
  using Dict = std::map<std::string, Node>;

  class Node : std::variant<std::vector<Node>, Dict, bool, int, double, std::string> {
  public:
    using variant::variant;
    const variant &GetBase() const { return *this; }

    bool IsArray() const { return std::holds_alternative<std::vector<Node>>(*this); }
    bool IsMap() const { return std::holds_alternative<std::map<std::string, Node>>(*this); }
    bool IsInt() const { return std::holds_alternative<int>(*this);}
    bool IsDouble() const { return std::holds_alternative<double>(*this);}
    bool IsNum() const { return IsDouble() || IsInt(); }
    bool IsBool() const { return std::holds_alternative<bool>(*this); }
    bool IsString() const { return std::holds_alternative<std::string>(*this); }

    const auto& AsArray() const { return std::get<std::vector<Node>>(*this); }
    const auto& AsMap() const { return std::get<Dict>(*this); }
    bool AsBool() const { return std::get<bool>(*this); }
    int AsInt() const { return std::get<int>(*this); }
    double AsDouble() const {
        return std::holds_alternative<double>(*this) ? std::get<double>(*this) : std::get<int>(*this);
    }
    const auto& AsString() const { return std::get<std::string>(*this); }
  };

  class Document {
  public:
    explicit Document(Node root) : root(move(root)) {}

    const Node &GetRoot() const {
      return root;
    }

  private:
    Node root;
  };

  Node LoadNode(std::istream& input);
  Document Load(std::istream& input);

  std::ostream &operator<<(std::ostream &output, const std::string &s);
  std::ostream &operator<<(std::ostream &output, const Node &node);
  std::ostream &operator<<(std::ostream &output, const std::vector<Node>& nodes);
  std::ostream &operator<<(std::ostream &output, const Dict &dict);
  std::ostream &operator<<(std::ostream &output, const Document &doc);

  bool operator==(const Json::Node &lhs, const Json::Node &rhs);
}

