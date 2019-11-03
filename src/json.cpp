#include "json.h"

using namespace std;

namespace Json {

Node LoadArray(istream& input) {
  vector<Node> result;

  for (char c; input >> c && c != ']'; ) {
    if (c != ',') {
      input.putback(c);
    }
    result.push_back(LoadNode(input));
  }

  return Node(move(result));
}

Node LoadBool(istream& input) {
  string s;
  while (isalpha(input.peek())) {
    s.push_back(input.get());
  }
  return Node(s == "true");
}

Node LoadNumber(istream& input) {
  bool is_negative = false;
  if (input.peek() == '-') {
    is_negative = true;
    input.get();
  }
  int int_part = 0;
  while (isdigit(input.peek())) {
    int_part *= 10;
    int_part += input.get() - '0';
  }
  if (input.peek() != '.') {
    return Node(int_part * (is_negative ? -1 : 1));
  }
  input.get();  // '.'
  double result = int_part;
  double frac_mult = 0.1;
  while (isdigit(input.peek())) {
    result += frac_mult * (input.get() - '0');
    frac_mult /= 10;
  }
  return Node(result * (is_negative ? -1 : 1));
}

Node LoadString(istream& input) {
  string line;
  getline(input, line, '"');
  return Node(move(line));
}

Node LoadDict(istream& input) {
  Dict result;

  for (char c; input >> c && c != '}'; ) {
    if (c == ',') {
      input >> c;
    }

    string key = LoadString(input).AsString();
    input >> c;
    result.emplace(move(key), LoadNode(input));
  }

  return Node(move(result));
}

Node LoadNode(istream& input) {
  char c;
  input >> c;

  if (c == '[') {
    return LoadArray(input);
  } else if (c == '{') {
    return LoadDict(input);
  } else if (c == '"') {
    return LoadString(input);
  } else if (c == 't' || c == 'f') {
    input.putback(c);
    return LoadBool(input);
  } else {
    input.putback(c);
    return LoadNumber(input);
  }
}

Document Load(istream& input) {
  return Document{LoadNode(input)};
}

ostream &operator<<(ostream &output, const string &str) {
  output << '"';
  for (const auto &symbol : str) {
    output << symbol;
  }
  output << '"';
  return output;
}

ostream &operator<<(std::ostream &output, const std::vector<Node>& nodes) {
  output << '[';
  bool first = true;
  for (const Node& node : nodes) {
    if (!first) {
      output << ", ";
    }
    first = false;
    output << node;
  }
  output << ']';
  return output;
}

ostream &operator<<(ostream &output, const Dict &dict) {
  output << '{';
  bool first = true;
  for (const auto& [key, node]: dict) {
    if (!first) {
      output << ", ";
    }
    first = false;
    output << key << ": " << node;
  }
  output << '}';
  return output;
}

ostream &operator<<(ostream &output, const Node &node) {
  visit([&output](const auto &value) { output << value; },
        node.GetBase());
  return output;
}

ostream &operator<<(ostream &output, const Document &doc) {
  output << doc.GetRoot();
  return output;
}

template <typename T>
bool operator==(const std::vector<T> &lhs, const std::vector<T> &rhs) {
  if (lhs.size() != rhs.size())
    return false;
  for (size_t i = 0; i < lhs.size(); ++i) {
    if (!(lhs[i] == rhs[i]))
      return false;
  }
  return true;
}

template <typename K, typename V>
bool operator==(const std::map<K, V> &lhs, const std::map<K, V> &rhs) {
  if (lhs.size() != rhs.size())
    return false;
  for (const auto &[key, value] : lhs) {
    if (rhs.count(key) == 0 || !(rhs.at(key) == value))
      return false;
  }
  return true;
}

bool operator==(const Json::Node &lhs, const Json::Node &rhs) {
  if (lhs.IsArray() && rhs.IsArray()) {
    return  lhs.AsArray() == rhs.AsArray();
  } else if (lhs.IsMap() && rhs.IsMap()) {
    return lhs.AsMap() == rhs.AsMap();
  } else if (lhs.IsNum() && rhs.IsNum()) {
    const double Tolerance = 1e-4;
    return  std::abs(lhs.AsDouble() - rhs.AsDouble()) < Tolerance;
  } else if (lhs.IsBool() && rhs.IsBool()) {
    return  lhs.AsBool() == rhs.AsBool();
  } else if (lhs.IsString() && rhs.IsString()) {
    return  lhs.AsString() == rhs.AsString();
  } else {
    return false;
  }
}
}
