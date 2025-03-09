#include <bits/stdc++.h>
#include <fstream>
using namespace std;
#include "registers.h"
#include "isa.h"
#include "memory.h"



long long pc = 0; // program counter


vector<long long> pc_count;

vector<pair<pair<string, string>, string>> Display;


std::string BinaryToHex(std::string s) {
    if (s.size() != 32) return "Error";

    std::string ret = "0x";
    std::map<std::string, std::string> m = {
        {"0000", "0"}, {"0001", "1"}, {"0010", "2"}, {"0011", "3"},
        {"0100", "4"}, {"0101", "5"}, {"0110", "6"}, {"0111", "7"},
        {"1000", "8"}, {"1001", "9"}, {"1010", "A"}, {"1011", "B"},
        {"1100", "C"}, {"1101", "D"}, {"1110", "E"}, {"1111", "F"}
    };

    for (int i = 0; i < 32; i += 4) {
        std::string temp = s.substr(i, 4);
        if (m.find(temp) != m.end()) {  // Ensure valid key exists
            ret += m.at(temp);
        } else {
            return "error";  // Handle unexpected input
        }
    }

    return ret;
}


string EncodeImmediate(const string& immediateValueStr, int choice) {
    string binaryResult ;
    // Step 2: Convert the immediate value string to an integer
    int immediateValue = data(immediateValueStr);
    if (immediateValue == ERROR_VALUE) {
        return "error";
    }

    switch (choice) {
        case 1: {  // I-type (12-bit)
            // Check if the immediate value is within the valid range (-2048 to 2047)
            if (immediateValue < -2048 || immediateValue > 2047) {
                return "error";
            }
            // Convert to 12-bit two's complement binary string
            for (int i = 11; i >= 0; i--) {
                binaryResult += ((immediateValue >> i) & 1) ? '1' : '0';
            }
            break;
        }

        case 2: {  // SB-type (13-bit for branch instructions)
            // Check if the immediate value is within the valid range (-4096 to 4095)
            if (immediateValue < -4096 || immediateValue > 4095) {
                return "error";
            }
            // Convert to 13-bit two's complement binary string
            for (int i = 12; i >= 0; i--) {
                binaryResult += ((immediateValue >> i) & 1) ? '1' : '0';
            }
            break;
        }

        case 3: {  // U-type (upper 20-bit)
            // Shift left by 12 bits
            immediateValue <<= 12;
            // Convert to 20-bit binary string (bits 31-12)
            for (int i = 31; i >= 12; i--) {
                binaryResult += ((immediateValue >> i) & 1) ? '1' : '0';
            }
            break;
        }

        default:
            return "error"; // Invalid choice
    }

    return binaryResult;
}


// convert assembly to machine code
void encodeRTypeInstruction(string instructionText) {
    string binaryInstruction;
    replace(instructionText.begin(), instructionText.end(), ',', ' ');
    istringstream instructionStream(instructionText); // Create a string stream for parsing
    string token;
    instructionStream >> token; // Read the instruction mnemonic
    string instructionMnemonic = token;

//Get the opcode
    string opc = opcode(instructionMnemonic);
// Read and encode the destination register (rd)
    instructionStream >> token;
    string rd = getReg(token);
//  Read and encode the first source register (rs1)
    instructionStream >> token;
    string rs1 = getReg(token);
// Read and encode the second source register (rs2)
    instructionStream >> token;
    string rs2 = getReg(token);
//  Get the func7 and func3 fields
    string f7 = Func7(instructionMnemonic);
    string f3 = Func3(instructionMnemonic);
    binaryInstruction = f7+rs2+rs1+f3+rd+opc;

    if (!all_of(binaryInstruction.begin(),binaryInstruction.end(),::isdigit)){
        Display.push_back({{"error", "error"}, instructionText});
        return;
    }

 // Format the binary instruction for display
    string formattedInstruction = 
        binaryInstruction.substr(0, 7) + "-" +
        binaryInstruction.substr(7, 5) + "-" +
        binaryInstruction.substr(12, 5) + "-" +
        binaryInstruction.substr(17, 3) + "-" +
        binaryInstruction.substr(20, 5) + "-" +
        binaryInstruction.substr(25, 7);

    Display.push_back({{binaryInstruction, formattedInstruction}, instructionText});
    pc_count.push_back(pc);
}
void encodeITypeInstruction(string instructionText, bool isOffsetBased = false) {
    string binaryInstruction;
    replace(instructionText.begin(), instructionText.end(), ',', ' ');
    istringstream instructionStream(instructionText); // Create a string stream for parsing
    string token;
    instructionStream >> token; // Read the instruction mnemonic
    string instructionMnemonic = token;

    // Get the opcode
    string opc= opcode(instructionMnemonic);

    //  Get the destination register (rd)
    instructionStream >> token;
    string rd = getReg(token);

    // Get the function code (funct3)
    string f3 = Func3(instructionMnemonic);

    //  Get the source register (rs1) and immediate (offset)
    instructionStream >> token;
    

    string offset, reg;
    string rs1,immed;
    if (isOffsetBased) {
        // Handle offset-based addressing (e.g., lb x1, 10(x2))
        size_t index = 0;
        while (token[index] != '(') {
            offset += token[index];
            index++;
        }
        index++;
        while (token[index] != ')') {
            reg += token[index];
            index++;
        }
        rs1 = getReg(reg); // rs1
        immed = EncodeImmediate(offset,1);   // imm
    } else {
        // Handle direct immediate (e.g., addi x1, x2, 10)
        rs1= getReg(token); // rs1
        instructionStream >> token;
        immed= EncodeImmediate(token,1); // imm
    }

    //  Construct the 32-bit binary instruction
    binaryInstruction = immed+rs1+f3+rd+opc;

    // Ensure the final binary string is exactly 32 bits
    if (binaryInstruction.size() != 32) {
        Display.push_back({{"error", "error"}, instructionText});
        return;
    }

    if (!all_of(binaryInstruction.begin(),binaryInstruction.end(),::isdigit)){
        Display.push_back({{"error", "error"}, instructionText});
        return;
    }
// Format the binary instruction for display
    string formattedInstruction =
        binaryInstruction.substr(0, 12) + "-" +
        binaryInstruction.substr(12, 5) + "-" +
        binaryInstruction.substr(17, 3) + "-" +
        binaryInstruction.substr(20, 5) + "-" +
        binaryInstruction.substr(25, 7);

    Display.push_back({{binaryInstruction,formattedInstruction}, instructionText});
    pc_count.push_back(pc);
}

void encodeSTypeInstruction(string instructionText) {
    string binaryInstruction;
    replace(instructionText.begin(), instructionText.end(), ',', ' ');
    istringstream instructionStream(instructionText); // Create a string stream for parsing
    string token;
    instructionStream >> token; // Read the instruction mnemonic
    string instructionMnemonic = token;


    // Get the opcode and func3
    string opc = opcode(instructionMnemonic);
    string f3 = Func3(instructionMnemonic);

    // Get the source register (rs2)
    instructionStream >> token;
    string rs2 = getReg(token);

    // Get the offset and base register (rs1)
    instructionStream >> token;
    string offset, baseRegister;
    size_t index = 0;
    while (token[index] != '(') {
        offset += token[index];
        index++;
    }
    index++;
    while (token[index] != ')') {
        baseRegister += token[index];
        index++;
    }

    string rs1 = getReg(baseRegister); // rs1
    string immed = EncodeImmediate(offset,1);      // offset

    //  Break the immediate field into parts
    string imm_7 = immed.substr(0, 7); // Lower 7 bits
    string imm_5 = immed.substr(7, 5); // Upper 5 bits

    binaryInstruction = imm_7+rs2+rs1+f3+imm_5+opc;


 if (!all_of(binaryInstruction.begin(),binaryInstruction.end(),::isdigit)){
        Display.push_back({{"error", "error"}, instructionText});
        return;
    }
// Format the binary instruction for display
    string formattedInstruction =
        binaryInstruction.substr(0, 7) + "-" +
        binaryInstruction.substr(7, 5) + "-" +
        binaryInstruction.substr(12, 5) + "-" +
        binaryInstruction.substr(17, 3) + "-" +
        binaryInstruction.substr(20, 5) + "-" +
        binaryInstruction.substr(25, 7);

    Display.push_back({{binaryInstruction, formattedInstruction}, instructionText});
    pc_count.push_back(pc);
}

void encodeSBTypeInstruction(string instructionText) {
    replace(instructionText.begin(), instructionText.end(), ',', ' ');
    istringstream instructionStream(instructionText); // Create a string stream for parsing
    string token;
    instructionStream >> token; // Read the instruction mnemonic
    string instructionMnemonic = token;


    // Get the opcode and func3
    string opc = opcode(instructionMnemonic);
    string f3= Func3(instructionMnemonic);

    // Get the first source register (rs1)
    instructionStream >> token;
    string rs1 = getReg(token);

    //  Get the second source register (rs2)
    instructionStream >> token;
    string rs2= getReg(token);

    //  Get the immediate value (branch offset)
    instructionStream >> token;

    // Check if the immediate value is valid
    bool isImmediateNumeric = all_of(token.begin(), token.end(), ::isdigit);
    if (label.find(token) == label.end() && !isImmediateNumeric) {
        Display.push_back({{"error","error"},instructionText});
        return;
    }

    long long immediateValue;
    if (label.find(token) != label.end()) {
        immediateValue = label[token] - pc; // Calculate offset from label
    } else {
        immediateValue = stoi(token); // Use immediate value directly
    }

    // Ensure the immediate value is a multiple of 4 (aligned)
    if (immediateValue % 4 != 0) {
        Display.push_back({{"error","error"},instructionText});
        return;
    }

    //  Encode the immediate value
    string immediateBinary = EncodeImmediate(to_string(immediateValue),2);

    //  Split the immediate into the required format
    string immed_7;
    immed_7 = immediateBinary.substr(0, 1); // Bit 12
    immed_7 += immediateBinary.substr(2, 6); // Bits 10:5
    string immed_5;
    immed_5 += immediateBinary.substr(8, 4); // Bits 4:1
    immed_5 += immediateBinary[1]; // Bit 11

    //  Combine the components into the final binary instruction
    string binaryInstruction = immed_7+rs2+rs1+f3+immed_5+opc;
     if (!all_of(binaryInstruction.begin(),binaryInstruction.end(),::isdigit)){
        Display.push_back({{"error", "error"}, instructionText});
        return;
    }


    // Format the binary instruction for display
    string formattedInstruction =
        binaryInstruction.substr(0, 1) + "-" + // Bit 12
        binaryInstruction.substr(1, 6) + "-" + // Bits 10:5
        binaryInstruction.substr(7, 5) + "-" + // Bits 4:1
        binaryInstruction.substr(12, 5) + "-" + // rs2
        binaryInstruction.substr(17, 3) + "-" + // func3
        binaryInstruction.substr(20, 4) + "-" + // rs1
        binaryInstruction.substr(24, 1) + "-" + // Bit 11
        binaryInstruction.substr(25, 7); // opcode

    Display.push_back({{binaryInstruction, formattedInstruction}, instructionText});
    pc_count.push_back(pc);
}
void encodeUTypeInstruction(string instructionText) {
    replace(instructionText.begin(), instructionText.end(), ',', ' ');
    istringstream instructionStream(instructionText); // Create a string stream for parsing
    string token;
    instructionStream >> token; // Read the instruction mnemonic
    string instructionMnemonic = token;

    //  Get the opcode
    string opc = opcode(instructionMnemonic);

    //  Get the destination register (rd)
    instructionStream >> token;
    string rd = getReg(token);

    // Get the immediate value
    instructionStream >> token;
    string immed = EncodeImmediate(token,3);

    //  Combine the components into the final binary instruction
    string binaryInstruction = immed+rd+opc;
     if (!all_of(binaryInstruction.begin(),binaryInstruction.end(),::isdigit)){
        Display.push_back({{"error", "error"}, instructionText});
        return;
    }

    // Format the binary instruction for display
    string formattedInstruction =
        binaryInstruction.substr(0, 20) + "-" +
        binaryInstruction.substr(20, 5) + "-" +
        binaryInstruction.substr(25, 7);

    Display.push_back({{binaryInstruction, formattedInstruction}, instructionText});
    pc_count.push_back(pc);
}
void encodeUJTypeInstruction(string instructionText) {
    replace(instructionText.begin(), instructionText.end(), ',', ' ');
    istringstream instructionStream(instructionText); // Create a string stream for parsing
    string token;
    instructionStream >> token; // Read the instruction mnemonic
    string instructionMnemonic = token;



    // Get the opcode
    string opc= opcode(instructionMnemonic);

    //  Get the destination register (rd)
    instructionStream >> token;
    string rd = getReg(token);

    // Get the immediate value (jump target)
    instructionStream >> token;

    // Check if the immediate value is valid
    bool isImmediateNumeric = all_of(token.begin(), token.end(), ::isdigit);
    if (label.find(token) == label.end() && !isImmediateNumeric) {
        Display.push_back({{"error","error"},instructionText});
        return;
    }

    long long immediateValue;
    if (label.find(token) != label.end()) {
        immediateValue = label[token] - pc; // Calculate offset from label
    } else {
        immediateValue = stoi(token); // Use immediate value directly
    }

    // Ensure the immediate value is a multiple of 4 (aligned)
    if (immediateValue % 4 != 0) {
        Display.push_back({{"error","error"},instructionText});        
        return;
    }

    //  Encode the immediate value
    string inter = "";
    long long jumpOffset = label[token] - pc;
    for (int i = 20; i >= 1; i--) {
        if ((1LL << i) & jumpOffset) {
            inter += '1';
        } else {
            inter += '0';
        }
    }

    //  Reorganize the immediate value bits
    string immed= "";
    immed += inter[0]; // Bit 20
    immed += inter.substr(10, 10); // Bits 10:1
    immed += inter[9];             // Bit 11
    immed += inter.substr(1, 8);   // Bits 19:12


    //  Combine the components into the final binary instruction
    string binaryInstruction = immed+rd+opc;
    if (!all_of(binaryInstruction.begin(),binaryInstruction.end(),::isdigit)){
        Display.push_back({{"error", "error"}, instructionText});
        return;
    }


    // Format the binary instruction for display
    string formattedInstruction =
        binaryInstruction.substr(0, 1) + "-" + // Bit 20
        binaryInstruction.substr(1, 10) + "-" + // Bits 10:1
        binaryInstruction.substr(11, 1) + "-" + // Bit 11
        binaryInstruction.substr(12, 8) + "-" + // Bits 19:12
        binaryInstruction.substr(20, 5) + "-" + // rd
        binaryInstruction.substr(25, 7); // opcode

    Display.push_back({{binaryInstruction, formattedInstruction}, instructionText});
    pc_count.push_back(pc);
}
void assemble(string txt)
{
    istringstream iss(txt); // create string stream
    string line;
    iss >> line;
    string keep = line;
    if (keep[keep.size() - 1] != ':')
        iss >> line;
    if (line == ":" || keep[keep.size() - 1] == ':')
    {
        return;
    }
    line = keep;
    if (line == "add" || line == "sub" || line == "xor" || line == "mul" || line == "div" || line == "rem" || line == "srl" || line == "sll" || line == "slt" || line == "or" || line == "and" || line == "sra")
    {
        encodeRTypeInstruction(txt);
    }
    else if (line == "addi" || line == "andi" || line == "ori" || line == "jalr")
    {
        encodeITypeInstruction(txt);
    }
    else if (line == "lb" || line == "ld" || line == "lh" || line == "lw")
    {
        encodeITypeInstruction(txt,true);
    }
    else if (line == "sb" || line == "sw" || line == "sd" || line == "sh")
    {
        encodeSTypeInstruction(txt);
    }
    else if (line == "beq" || line == "bne" || line == "bge" || line == "blt")
    {
        encodeSBTypeInstruction(txt);
    }
    else if (line == "jal")
    {
        encodeUJTypeInstruction(txt);
    }
    else if (line == "auipc" || line == "lui")
    {
        encodeUTypeInstruction(txt);
    }
    else
    {
        Display.push_back({{"Unknown operand type","Unknown operand type"},txt});
    }
    pc += 4; // after each operation
    return;
}
void read(string inputText) {
    istringstream inputStream(inputText); // Create a string stream for parsing
    string token;
    inputStream >> token; // Read the first token (label or instruction)

    // Handle labels (e.g., "label:")
    string labelName = token;
    if (token.back() == ':') { // Check if the token ends with ':'
        labelName = token.substr(0, token.size() - 1); // Remove the colon
    }

    // If the first token was not a label, read the next token
    if (token.back() != ':') {
        inputStream >> token;
    }

    //  Check if the line contains only a label
    if (token == ":" || token.back() == ':') {
        // Store the label and its corresponding program counter (PC) value
        label[labelName] = pc;
        return; // Exit if the line contains only a label
    }

    //  Increment the program counter (PC) for instructions
    pc += 4;
}
int main()
{
 ofstream op("output.mc");
    ifstream file("input.asm");
    op << "PC"
       << "    "
       << "MACHINE CODE"
       << "  "
       << "INSTRUCTIONS"
       <<"              "
       <<"FIELDS\n";

    string txt;
    long long mem_arr[204];
    for (long long i = 0; i < 204; i++)
        mem_arr[i] = 0;
    long long flag = 0;
    long long memory = Start; // memory
    while (getline(file, txt))
    {
        if (txt == ".data")
        {

            while (getline(file, txt))
            {

                if (txt == ".text")
                {
                    // getline(file, txt);
                    break;
                }
                mem(txt, mem_arr);
            }
        }
        else if (txt == ".text")
            continue;
        else
            read(txt);
    }
    file.close();

    // we can get a read of the file beforehand as a cheat to get the labels
    // then we simply reset the program counter.

    ifstream file2("input.asm");

    pc = 0;

    while (getline(file2, txt))
    {
        if (txt == ".data")
        {
            while (getline(file2, txt))
            {
                if (txt == ".text")
                {
                    // getline(file2, txt);
                    break;
                }
            }
        }
        else if (txt == ".text")
            continue;
        else
            assemble(txt);
    }

    // for now print data here only for testing
    // cout << store.size() << endl;
    long long ct = 0;
    for (auto i : Display)
    {
        if (i.first.first[0] == '0' || i.first.first[0] == '1')
        {
            op << "0x";
            op << hex << pc_count[ct] << "   ";
           
            op << BinaryToHex(i.first.first) <<"  "<<i.second<<"    "<<"#"<<i.first.second<< endl;
        }
        else
        {
            op << i.first.first<< endl;
            // break;
        }
        ct++;
    }
    op << "\n\n";
    op << "------------------------------------------------------------------------------------------------------------------------\n\n";
    op << "Memory from 0x10000000\n\n";
    
    // end of printing

    displayMemoryInWordFormat(mem_arr,op);
    /*for (auto i : label)
        cout << i.first << endl;*/
    op.close();
    file2.close();
    return 0;
}