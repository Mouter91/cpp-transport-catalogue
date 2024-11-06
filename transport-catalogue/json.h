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

class Node {
public:
    using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;

    Node() : value_(nullptr) {}
    Node(std::nullptr_t) : value_(nullptr) {}
    Node(int value) : value_(value) {}
    Node(double value) : value_(value) {}
    Node(const std::string& value) : value_(value) {}
    Node(Array array) : value_(std::move(array)) {}
    Node(Dict dict) : value_(std::move(dict)) {}
    Node(bool value) : value_(value) {}

    const Value& GetValue() const;

    bool IsNull() const { return std::holds_alternative<std::nullptr_t>(value_); }
    bool IsInt() const { return std::holds_alternative<int>(value_); }
    bool IsDouble() const { return IsInt() || IsPureDouble(); }
    bool IsPureDouble() const { return std::holds_alternative<double>(value_); }
    bool IsString() const { return std::holds_alternative<std::string>(value_); }
    bool IsArray() const { return std::holds_alternative<Array>(value_); }
    bool IsMap() const { return std::holds_alternative<Dict>(value_); }
    bool IsBool() const { return std::holds_alternative<bool>(value_); }


    int AsInt() const;
    double AsDouble() const;
    const std::string& AsString() const;
    const Array& AsArray() const;
    const Dict& AsMap() const;
    bool AsBool() const;

    bool operator==(const Node& other) const {
        return value_ == other.value_;
    }

    bool operator!=(const Node& other) const {
        return !(*this == other);
    }


private:
    Value value_;
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
