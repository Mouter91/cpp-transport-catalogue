#include "json.h"

#include <iterator>

namespace json {

namespace {
using namespace std::literals;

Node LoadNode(std::istream& input);
Node LoadString(std::istream& input);

std::string LoadLiteral(std::istream& input) {
    std::string s;
    while (std::isalpha(input.peek())) {
        s.push_back(static_cast<char>(input.get()));
    }
    return s;
}

Node LoadArray(std::istream& input) {
    std::vector<Node> result;

    for (char c; input >> c && c != ']';) {
        if (c != ',') {
            input.putback(c);
        }
        result.push_back(LoadNode(input));
    }
    if (!input) {
        throw ParsingError("Array parsing error"s);
    }
    return Node(std::move(result));
}

Node LoadDict(std::istream& input) {
    Dict dict;

    for (char c; input >> c && c != '}';) {
        if (c == '"') {
            std::string key = LoadString(input).AsString();
            if (input >> c && c == ':') {
                if (dict.find(key) != dict.end()) {
                    throw ParsingError("Duplicate key '"s + key + "' have been found");
                }
                dict.emplace(std::move(key), LoadNode(input));
            } else {
                throw ParsingError(": is expected but '"s + c + "' has been found"s);
            }
        } else if (c != ',') {
            throw ParsingError(R"(',' is expected but ')"s + c + "' has been found"s);
        }
    }
    if (!input) {
        throw ParsingError("Dictionary parsing error"s);
    }
    return Node(std::move(dict));
}

Node LoadString(std::istream& input) {
    auto it = std::istreambuf_iterator<char>(input);
    auto end = std::istreambuf_iterator<char>();
    std::string s;
    while (true) {
        if (it == end) {
            throw ParsingError("String parsing error");
        }
        const char ch = *it;
        if (ch == '"') {
            ++it;
            break;
        } else if (ch == '\\') {
            ++it;
            if (it == end) {
                throw ParsingError("String parsing error");
            }
            const char escaped_char = *(it);
            switch (escaped_char) {
            case 'n':
                s.push_back('\n');
                break;
            case 't':
                s.push_back('\t');
                break;
            case 'r':
                s.push_back('\r');
                break;
            case '"':
                s.push_back('"');
                break;
            case '\\':
                s.push_back('\\');
                break;
            default:
                throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
            }
        } else if (ch == '\n' || ch == '\r') {
            throw ParsingError("Unexpected end of line"s);
        } else {
            s.push_back(ch);
        }
        ++it;
    }

    return Node(std::move(s));
}

Node LoadBool(std::istream& input) {
    const auto s = LoadLiteral(input);
    if (s == "true"sv) {
        return Node{true};
    } else if (s == "false"sv) {
        return Node{false};
    } else {
        throw ParsingError("Failed to parse '"s + s + "' as bool"s);
    }
}

Node LoadNull(std::istream& input) {
    if (auto literal = LoadLiteral(input); literal == "null"sv) {
        return Node{nullptr};
    } else {
        throw ParsingError("Failed to parse '"s + literal + "' as null"s);
    }
}

Node LoadNumber(std::istream& input) {
    std::string parsed_num;

    // Считывает в parsed_num очередной символ из input
    auto read_char = [&parsed_num, &input] {
        parsed_num += static_cast<char>(input.get());
        if (!input) {
            throw ParsingError("Failed to read number from stream"s);
        }
    };

    // Считывает одну или более цифр в parsed_num из input
    auto read_digits = [&input, read_char] {
        if (!std::isdigit(input.peek())) {
            throw ParsingError("A digit is expected"s);
        }
        while (std::isdigit(input.peek())) {
            read_char();
        }
    };

    if (input.peek() == '-') {
        read_char();
    }

    if (input.peek() == '0') {
        read_char();
    } else {
        read_digits();
    }

    bool is_int = true;

    if (input.peek() == '.') {
        read_char();
        read_digits();
        is_int = false;
    }

    if (int ch = input.peek(); ch == 'e' || ch == 'E') {
        read_char();
        if (ch = input.peek(); ch == '+' || ch == '-') {
            read_char();
        }
        read_digits();
        is_int = false;
    }

    try {
        if (is_int) {
            try {
                return std::stoi(parsed_num);
            } catch (...) {
            }
        }
        return std::stod(parsed_num);
    } catch (...) {
        throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
    }
}

Node LoadNode(std::istream& input) {
    char c;
    if (!(input >> c)) {
        throw ParsingError("Unexpected EOF"s);
    }
    switch (c) {
    case '[':
        return LoadArray(input);
    case '{':
        return LoadDict(input);
    case '"':
        return LoadString(input);
    case 't':
    case 'f':
        input.putback(c);
        return LoadBool(input);
    case 'n':
        input.putback(c);
        return LoadNull(input);
    default:
        input.putback(c);
        return LoadNumber(input);
    }
}
}
//++++++++++++++++++++++++++++++++>Visitor<+++++++++++++++++++++++++++++++++++++++++++++++
void VisitorNode::operator()(std::nullptr_t) {
    out_ << "null";
}

void VisitorNode::operator()(Array array) {
    out_ << "[\n";
    bool first = true;
    for (const auto& node : array) {
        if (!first) {
            out_ << ", \n";
        }
        first = false;
        std::visit(VisitorNode{out_}, node.GetValue());
    }
    out_ << "]\n";
}

void VisitorNode::operator()(Dict dict) {
    out_ << "{\n";
    bool first = true;
    for (const auto& [key, value] : dict) {
        if (!first) {
            out_ << ", \n";
        }
        first = false;
        out_ << "\"" << key << "\": ";
        std::visit(VisitorNode{out_}, value.GetValue());
    }
    out_ << "}\n";
}

void VisitorNode::operator()(bool value) {
    out_ << (value ? "true" : "false");
}

void VisitorNode::operator()(int value) {
    out_ << value;
}

void VisitorNode::operator()(double value) {
    out_ << value;
}

void VisitorNode::operator()(const std::string& value) {
    PrintString(value, out_);
}

void PrintString(const std::string& str, std::ostream& output) {
    output << '"';
    for (const char ch : str) {
        switch (ch) {
        case '\n': output << "\\n"; break;
        case '\r': output << "\\r"; break;
        case '\t': output << "\\t"; break;
        case '\"': output << "\\\""; break;
        case '\\': output << "\\\\"; break;
        default: output << ch; break;
        }
    }
    output << '"';
}

//+++++++++++++++++++++++++++++++++++>>Node<<++++++++++++++++++++++++++++++++++++++++++++++++++
const Node::Value& Node::GetValue() const {
    return value_;
}

//++++++++++++++++++++++++++++++++++++++++Check+++++++++++++++++++++++++++++++++++++
int Node::AsInt() const {
    if (!IsInt()) {
        throw std::logic_error("Node is not an integer");
    }
    return std::get<int>(value_);
}

double Node::AsDouble() const {
    if (!IsDouble()) {
        throw std::logic_error("Node is not a double");
    }
    return IsPureDouble() ? std::get<double>(value_) : static_cast<double>(AsInt());
}

const std::string& Node::AsString() const {
    if (!IsString()) {
        throw std::logic_error("Node is not a string");
    }
    return std::get<std::string>(value_);
}

const Array& Node::AsArray() const {
    if (!IsArray()) {
        throw std::logic_error("Node is not an array");
    }
    return std::get<Array>(value_);
}

const Dict& Node::AsMap() const {
    if (!IsMap()) {
        throw std::logic_error("Node is not a map");
    }
    return std::get<Dict>(value_);
}

bool Node::AsBool() const {
    // Предположим, что Node хранит значение типа std::variant, в который может входить bool
    if (std::holds_alternative<bool>(value_)) {
        return std::get<bool>(value_);
    }
    throw std::logic_error("Node does not hold a boolean value");
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Document::Document(Node root)
    : root_(std::move(root)) {
}

bool operator==(const Document& lhs, const Document& rhs) {
    return lhs.GetRoot() == rhs.GetRoot();
}

const Node& Document::GetRoot() const {
    return root_;
}

Document Load(std::istream& input) {
    return Document{LoadNode(input)};
}

void Print(const Document& doc, std::ostream& output) {
    std::visit(VisitorNode{output}, doc.GetRoot().GetValue());
}

}
