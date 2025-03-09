#ifndef REGISTERS_H
#define REGISTERS_H
using namespace std;
#include <vector>
#include <string>
#include <algorithm>

static vector<string> registers = {
    "x0", "x1", "x2", "x3", "x4", "x5", "x6", "x7",
    "x8", "x9", "x10", "x11", "x12", "x13", "x14", "x15",
    "x16", "x17", "x18", "x19", "x20", "x21", "x22", "x23",
    "x24", "x25", "x26", "x27", "x28", "x29", "x30", "x31"
};

string getReg(string registerName) {
    string binaryResult;
    //  Check if the register exists in the mapping
    if (find(registers.begin(),registers.end(),registerName) == registers.end()) {
        return "error";
    }
 //  Extract the register number (remove 'x' and any trailing comma)
    
   if(registerName.back()==','){ registerName.pop_back();}
   string registerNumberStr=registerName.substr(1);
    
    //  Convert the register number to an integer
    int registerNumber = stoi(registerNumberStr);

   //  Convert the register number to a 5-bit binary string
    for (int i = 4; i >= 0; i--) {
        binaryResult += (registerNumber & (1 << i)) ? '1' : '0';
    }

    return binaryResult;
}
#endif