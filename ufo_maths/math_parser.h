#pragma once
#include <string>
#include <cmath>
#include <vector>
#include "../utils/console.h"
#include <memory>
#include <algorithm>

namespace ufo::Maths{

class MathNode{
public:
    std::vector<std::unique_ptr<MathNode>> nodes;

    virtual float calculate(){
        Console::PrintLine("MathNode: Faulty calculation");
        return 0.0f;
    }

    virtual void Print(int _depth){
        for(const auto& node : nodes){
            node->Print(_depth+1);
        }
    }

};

class AddNode : public MathNode{
public:
    float calculate() override{
        return nodes[0]->calculate() + nodes[1]->calculate();
    }
    void Print(int _depth){
        MathNode::Print(_depth);
        Console::PrintLine("AddNode");

    }

};

class SubtractionNode : public MathNode{
public:
    float calculate() override{
        return nodes[0]->calculate() - nodes[1]->calculate();
    }
    void Print(int _depth){
        MathNode::Print(_depth);
        Console::PrintLine("SubtractionNode");

    }

};

class SinNode : public MathNode{
public:
    float calculate() override{
        return std::sin(nodes[0]->calculate());
    }

    void Print(int _depth){

    }
};

class NumberNode : public MathNode{
public:
    float value = 0.0f;

    NumberNode(float _value) : value{_value}{}

    float calculate() override{
        return value;
    }

    void Print(int _depth){
        MathNode::Print(_depth);
        Console::PrintLine("NumberNode", value);
    }
};

std::unique_ptr<MathNode> ParseTokensRecursive(std::vector<std::string>& _expression){
    Console::PrintLine("ParseTokensRecursive");
    for(const auto& e : _expression) Console::PrintLine(e);

    std::unique_ptr<MathNode> node = std::make_unique<MathNode>();

    std::string operation;

    std::vector<std::string> left_side_words;
    std::vector<std::string> right_side_words;

    for(int i = _expression.size()-1; i != -1; i--){
        const std::string w = _expression[i];
        if(operation.empty()){
            if(w == "+"){
                operation = w;
                continue;
            }
        }

        if(operation == "+"){
            left_side_words.push_back(w);
        }
        else{
            right_side_words.push_back(w);
        }
    }

    std::reverse(right_side_words.begin(), right_side_words.end());
    std::reverse(left_side_words.begin(), left_side_words.end());

    if(operation == "+"){
        node = std::make_unique<AddNode>();
        node->nodes.push_back(ParseTokensRecursive(left_side_words));
        node->nodes.push_back(ParseTokensRecursive(right_side_words));
    }
    else{

        node = std::make_unique<NumberNode>(std::stoi(right_side_words[0]));
    }

    return std::move(node);
}

inline float ParseExpression(const std::string& _expression){

    std::string separators = "+-*/^()";

    std::string word;
    std::vector<std::string> words;

    for(const char c : _expression){
        if(separators.find(c) != separators.npos){
            if(word != "") words.push_back(word);
            word = "";
            words.push_back(std::string(&c));
            continue;
        }
        word+=c;
    }
    words.push_back(word);

    for(const auto& w : words){
        Console::PrintLine("'",w,"'");
    }

    std::unique_ptr<MathNode> node = std::make_unique<MathNode>();

    node = ParseTokensRecursive(words);
    node->Print(0);

    return node->calculate();

}

}
