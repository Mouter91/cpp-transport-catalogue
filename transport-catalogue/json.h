#pragma once

#include <map>
#include <string>
#include <vector>
#include <variant>
#include <exception>
#include <iostream>

namespace json {
class Node;
using Dict = std::map<std::string, Node>;
using Array = std::vector<Node>;

class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

struct VisitorNode {
    std::ostream& out_;

    void operator()(const std::string& value);
    void operator()(double value);
    void operator()(int value);
    void operator()(bool value);
    void operator()(Dict dict);
    void operator()(Array array);
    void operator()(std::nullptr_t);
};


void PrintString(const std::string& str, std::ostream& output);

using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;

class Node : public Value {
public:
    using Value::Value;
    

    bool IsNull() const { return std::holds_alternative<std::nullptr_t>(*this); }
    bool IsInt() const { return std::holds_alternative<int>(*this); }
    bool IsDouble() const { return IsInt() || IsPureDouble(); }
    bool IsPureDouble() const { return std::holds_alternative<double>(*this); }
    bool IsString() const { return std::holds_alternative<std::string>(*this); }
    bool IsArray() const { return std::holds_alternative<Array>(*this); }
    bool IsMap() const { return std::holds_alternative<Dict>(*this); }
    bool IsBool() const { return std::holds_alternative<bool>(*this); }


    int AsInt() const;
    double AsDouble() const;
    const std::string& AsString() const;
    const Array& AsArray() const;
    const Dict& AsMap() const;
    bool AsBool() const;

    bool operator==(const Node& other) const {
        return static_cast<const Value&>(*this) == static_cast<const Value&>(other);
    }

    bool operator!=(const Node& other) const {
        return !(*this == other);
    }
};

class Document {
public:
    Document() = default;
    explicit Document(Node root);

    const Node& GetRoot() const;
    friend bool operator==(const Document& lhs, const Document& rhs);

private:
    Node root_;
};

Document Load(std::istream& input);

void Print(const Document& doc, std::ostream& output);

}
