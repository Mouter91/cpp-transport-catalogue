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
    class ArrayValue;
    class DictValue;
    class DictKey;
public:
    Builder();
    Node Build();
    Builder& Key(std::string key);
    Builder& Value(Node::Value value);
    DictItemContext StartDict() {
        AddObject(Dict{}, /* one_shot */ false);
        return DictItemContext(*this);
    }
    ArrayItemContext StartArray() {
        AddObject(Array{}, /* one_shot */ false);
        return ArrayItemContext(*this);
    }
    Builder& EndDict();
    Builder& EndArray();

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
    protected:
        Builder& builder_;
    };

    class DictItemContext : public BaseContext {
    public:
        explicit DictItemContext(Builder& builder): BaseContext(builder) {}
        DictKey Key(const std::string& key) {
            return DictKey(builder_.Key(key));
        }
        Builder& EndDict() {
            return builder_.EndDict();
        }
    };

    class ArrayItemContext : public BaseContext {
    public:
        explicit ArrayItemContext(Builder& builder): BaseContext(builder) {}
        ArrayValue Value(Node::Value value) {
            return ArrayValue(builder_.Value(value));
        }
        DictItemContext StartDict() {
            return builder_.StartDict();
        }
        ArrayItemContext StartArray() {
            return builder_.StartArray();
        }
        Builder& EndArray() {
            return builder_.EndArray();
        }
    };


    class ArrayValue : public BaseContext  {
    public:
        explicit ArrayValue(Builder& builder): BaseContext(builder) {}

        ArrayValue Value(Node::Value value){
            return ArrayValue(builder_.Value(value));
        }
        DictItemContext StartDict(){
            return DictItemContext(builder_.StartDict());
        }
        ArrayItemContext StartArray();
        Builder& EndArray(){
            return builder_.EndArray();
        }
    };


    class DictValue : public BaseContext  {
    public:
        explicit DictValue(Builder& builder): BaseContext(builder) {}

        DictKey Key(std::string key) {
            return DictKey(builder_.Key(std::move(key)));
        }
        Builder& EndDict() {
            return builder_.EndDict();
        }
    };

    class DictKey: public BaseContext {
    public:
        explicit DictKey(Builder& builder): BaseContext(builder) {}
        DictValue Value(Node::Value value) {
            return DictValue(builder_.Value(value));
        }
        DictItemContext StartDict() {
            return builder_.StartDict();
        }
        ArrayItemContext StartArray(){
            return builder_.StartArray();
        };
    };
};


}  // namespace json
