#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>
#include <utility>

namespace json {

    class Node;
    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;

    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    class Node final
        : private std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string> {
    public:
        using variant::variant;
        using Value = variant;

    bool IsInt() const{
        return std::holds_alternative<int>(*this);//почему не работает или нельзя поставит *this в скобках - выглядит круче
    }
    bool IsDouble() const{
        return (std::holds_alternative<double>(*this) || IsInt());
    }
    bool IsPureDouble() const{
        return std::holds_alternative<double>(*this);
    }
    bool IsBool() const{
        return std::holds_alternative<bool>(*this);
    }
    bool IsString() const{
        return std::holds_alternative<std::string>(*this);
    }
    bool IsNull() const{
        return std::holds_alternative<std::nullptr_t>(*this);
    }
    bool IsArray() const{
        return std::holds_alternative<Array>(*this);
    }
    bool IsMap() const{
        return std::holds_alternative<Dict>(*this);
    }

    /////////
    const Array& AsArray() const{
        return(!IsArray()) ? throw std::logic_error("Array error") : std::get<Array>(*this);
    }
    const Dict& AsMap() const{
        return (!IsMap()) ? throw std::logic_error("Map error") : std::get<Dict>(*this);
    }
    int AsInt() const{
        return (!IsInt()) ? throw std::logic_error("Int error") : std::get<int>(*this);
    }
    const std::string& AsString() const{
        return (!IsString()) ? throw std::logic_error("String error") : std::get<std::string>(*this);
    }
    bool AsBool() const{
        return (!IsBool()) ? throw std::logic_error("Bool error") :
        std::get<bool>(*this);
    }
    double AsDouble() const{
        return(!IsDouble()) ? throw std::logic_error("Double error") :
        IsPureDouble() ? std::get<double>(*this) : AsInt();
    }
    
    const Value& GetValue() const {
        return *this; 
    }
    
   
    bool operator==(const Node& rhs) const{
        return GetValue() == rhs.GetValue();}
   
    bool operator!=(const Node& rhs) const{
        return GetValue() != rhs.GetValue();}

};

class Document {
public:
    explicit Document(Node root) :root_(std::move(root)){}

    const Node& GetRoot() const{
        return root_;
    }
    
    bool operator == (const Document& rhs) const{
        return root_ == rhs.root_;
    }
    
    bool operator !=(const Document& rhs) const{
        return !(root_ == rhs.root_);
    }

private:
    Node root_;
};

Document Load(std::istream& input);

void Print(const Document& doc, std::ostream& output);

}  // namespace json
