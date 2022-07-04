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

    bool IsInt() const;
            
    bool IsDouble() const;
            
    bool IsPureDouble() const;
            
    bool IsBool() const;
            
    bool IsString() const;
            
    bool IsNull() const;
            
    bool IsArray() const;
            
    bool IsMap() const;

    /////////
    const Array& AsArray() const;
            
    const Dict& AsMap() const;
            
    int AsInt() const;
            
    const std::string& AsString() const;
            
    bool AsBool() const;
            
    double AsDouble() const;
    
    const Value& GetValue() const {
        return *this; 
    }
       
    bool operator==(const Node& rhs) const; /*{
        return GetValue() == rhs.GetValue();
    }*/
   
    bool operator!=(const Node& rhs) const; /*{
        return GetValue() != rhs.GetValue();
    }*/

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