#pragma once

#include <string>
#include <vector>
#include "json.h"

namespace json {

class Builder {
private:
    class BaseContext;
    class DictItemContext;
    class ArrayItemContext;
    class ValueItemContext;
public:
    Builder();
    Node Build();
    ValueItemContext Key(std::string key);
    ValueItemContext Value(Node::Value value);
    DictItemContext StartDict();
    ArrayItemContext StartArray();
    BaseContext  EndDict();
    BaseContext  EndArray();

private:
    Node root_;
    std::vector<Node*> nodes_stack_;

    Node::Value& GetCurrentValue();
    const Node::Value& GetCurrentValue() const;

    void AssertNewObjectContext() const;
    void AddObject(Node::Value value, bool one_shot);

    class BaseContext {
    public:
        explicit BaseContext(Builder& builder) : builder_(builder) {}

        Node Build() {
            return builder_.Build();
        }

        BaseContext Key(const std::string& key) {
            return BaseContext(builder_.Key(key));
        }
        BaseContext Value(Node::Value value) {
            return BaseContext(builder_.Value(value));
        }
        DictItemContext StartDict() {
            return builder_.StartDict();
        }
        ArrayItemContext StartArray() {
            return builder_.StartArray();
        }
        BaseContext EndDict() {
            return builder_.EndDict();
        }
        BaseContext EndArray() {
            return builder_.EndArray();
        }

    protected:
        Builder& builder_;
    };

    class DictItemContext : public BaseContext {
    public:
        explicit DictItemContext(Builder& builder): BaseContext(builder) {}
        Node Build() = delete;
        BaseContext Value(Node::Value value) = delete;
        BaseContext EndArray() = delete;
        DictItemContext StartDict() = delete;
        ArrayItemContext StartArray() = delete;
    };

    class ArrayItemContext : public BaseContext {
    public:
        explicit ArrayItemContext(Builder& builder): BaseContext(builder) {}
        Node Build() = delete;
        ValueItemContext Value(Node::Value value) {
            return builder_.Value(value);
        }
        BaseContext EndArray() = delete;
        DictItemContext StartDict() = delete;
        ArrayItemContext StartArray() = delete;
    };


    class ValueItemContext  : public BaseContext  {
    public:
        explicit ValueItemContext(Builder& builder): BaseContext(builder) {}
        BaseContext Value(Node::Value value) {
            return BaseContext::Value(std::move(value));
        }
        Node Build() = delete;
        ValueItemContext  Key(std::string key) = delete;
        BaseContext EndDict() = delete;
        BaseContext EndArray() = delete;
    };
};


}
