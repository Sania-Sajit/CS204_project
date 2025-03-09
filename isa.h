#ifndef ISA_H
#define ISA_H

#include <map>
#include<unordered_map>
#include <string>
using namespace std;

map<string,string> opcodeMap = {
    {"rem", "0110011"}, {"sra", "0110011"}, {"add", "0110011"}, {"sll", "0110011"},
    {"slt", "0110011"}, {"xor", "0110011"}, {"srl", "0110011"}, {"or", "0110011"},
    {"and", "0110011"}, {"sub", "0110011"}, {"mul", "0110011"}, {"div", "0110011"},
    {"addi", "0010011"}, {"andi", "0010011"}, {"ori", "0010011"},
    {"lb", "0000011"}, {"ld", "0000011"}, {"lw", "0000011"}, {"lh", "0000011"}, 
    {"jalr", "1100111"}, 
    {"beq", "1100011"}, {"bne", "1100011"}, {"bge", "1100011"}, {"blt", "1100011"},
    {"sb", "0100011"}, {"sh", "0100011"}, {"sw", "0100011"}, {"sd", "0100011"},
    {"auipc", "0010111"},
    {"lui", "0110111"},  
    {"jal", "1101111"}
};

string opcode(const string &inp) {
    string s;
    if (opcodeMap.find(inp) != opcodeMap.end()) {
        s += opcodeMap[inp];  // Append the opcode
    } else {
        s = "error";  // If instruction not found, return "error"
    }
    return s;
}

// Function to get func7 field for R-type instructions
string Func7(const string& inp) {
    static const unordered_map<string, string> func7_map = {
        {"sub", "0100000"}, {"sra", "0100000"},
        {"add", "0000000"}, {"sll", "0000000"},
        {"slt", "0000000"}, {"xor", "0000000"},
        {"srl", "0000000"}, {"or", "0000000"},
        {"and", "0000000"},
        {"mul", "0000001"}, {"div", "0000001"},
        {"rem", "0000001"}
    };

    auto it = func7_map.find(inp);
    return (it != func7_map.end()) ? it->second : "error";
}


// Function to get func3 field for various instruction types
string Func3(const string& a) {
    static const unordered_map<string, string> func3_map = {
        {"add", "000"}, {"sub", "000"}, {"mul", "000"}, {"sb", "000"},
        {"xor", "100"}, {"div", "100"},
        {"srl", "101"}, {"sra", "101"},
        {"sll", "001"}, {"sh", "001"},
        {"slt", "010"}, {"sw", "010"},
        {"or", "110"}, {"rem", "110"},
        {"and", "111"},
        {"sd", "011"},
        {"lb", "000"}, {"jalr", "000"}, {"addi", "000"},
        {"lh", "001"},
        {"lw", "010"},
        {"ld", "011"},
        {"ori", "110"},
        {"andi", "111"},
        {"beq", "000"},
        {"bne", "001"},
        {"blt", "100"},
        {"bge", "101"}
    };

    auto it = func3_map.find(a);
    return (it != func3_map.end()) ? it->second : "error";
}


#endif