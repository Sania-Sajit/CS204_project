#ifndef MEMORY_H
#define MEMORY_H
#include <climits>
#include <fstream>
#include <map>
#include <unordered_map>
#include <string>
#include <sstream>
#include <iomanip> // for std::setw and std::setfill
#include <algorithm> // for std::all_of
const int ERROR_VALUE = -1;
using namespace std;
long long Start = (1LL << 28) + 200;
long long size1 = 0;
map<string, long long> label;

// Function to parse data values from different formats (hex, binary, decimal)
int data(string inputValue) {
    int numericValue = 0;
//  Handle hexadecimal format (e.g., 0x1A)
    if (inputValue.size() > 2 && inputValue[0] == '0' && inputValue[1] == 'x') {
        if (inputValue.size() > 10) {
            return ERROR_VALUE; // Hexadecimal value too large
        }
        string hexDigits = inputValue.substr(2); // Extract hex digits
        numericValue = stoi(hexDigits, 0, 16); // Convert hex to integer
    }
    //  Handle binary format 
    else if (inputValue[0] == '0' && inputValue[1]=='b') {
        if (inputValue.size() > 34) {
            return ERROR_VALUE; // Binary value too large
        }
        string binaryDigits = inputValue.substr(2); // Extract binary digits
        numericValue = stoi(binaryDigits, 0, 2); // Convert binary to integer
    }
    //  Handle negative numbers 
    else if (inputValue[0] == '-') {
        string positiveNumber = inputValue.substr(1); // Remove the negative sign
        numericValue = -stoi(positiveNumber); // Convert to negative integer
        if (numericValue < INT_MIN) {
            return ERROR_VALUE; // Negative value out of range
        }
    }
    //  Handle decimal numbers
    else if (all_of(inputValue.begin(),inputValue.end(),::isdigit)){
        numericValue = stoi(inputValue); // Convert decimal string to integer
    }
    // Handle ASCII characters 
    else {
        for (char ch : inputValue) {
            numericValue += (int)ch; // Sum ASCII values of characters
        }
    }
    return numericValue;

}


// add data to memory
void mem(string txt, long long mem_arr[200])
{
 istringstream iss(txt);
    string line;
    // first is label
    iss >> line;
    string temp = line;

    if (line[line.size() - 1] == ':')
    {
        temp= "";
        for (long long i = 0; i < line.size() - 1; i++)
            temp += line[i];
    }
    label[temp] = Start + size1; // marking for la
    if (line[line.size() - 1] != ':')
        iss >> line; // jump to next line

    iss >> line;
    if (line == ".byte")
    {
        while (iss >> line)
        {
            if (line == ",")
                continue;
            long long get = data(line);
            mem_arr[size1] = get;
            size1++;
        }
    }
    else if (line == ".word")
    {
        while (iss >> line)
        {
            if (line == ",")
                continue;
            long long get = data(line);

            // now we have data in integer format and need to break datat into 4 parts
            long long mask = 255; // extraxt 8 bits
            for (long long i = 0; i < 4; i++)
            {
                long long g = mask & get;
                get >>= 8;
                mem_arr[size1] = g;
                size1++;
                // we have added to one byte
            }
        }
    }
    else if (line == ".asciiz")
    {
        while (iss >> line)
        {
            if (line == ",")
                continue;
            long long get = 0;
            for (long long i = 1; i < line.size() - 1; i++)
            {
                get = (long long)line[i];
                mem_arr[size1] = get;
                size1++; // assign to each charatcer one block
            }
        }
    }
    else if (line == ".half")
    {
        while (iss >> line)
        {
            if (line == ",")
                continue;
            long long get = data(line);

            // now we have data in integer format and need to break datat into 2 parts
            long long mask = 255; // extraxt 8 bits
            for (long long i = 0; i < 2; i++)
            {
                long long g = mask & get;
                get >>= 8;
                mem_arr[size1] = g;
                size1++;
                // we have added to one byte
            }
        }
    }
    else if (line == ".dword")
    {
        while (iss >> line)
        {
            if (line == ",")
                continue;
            long long get = data(line);

            // now we have data in integer format and need to break datat into 8 parts
            long long mask = 255; // extraxt 8 bits
            for (long long i = 0; i < 8; i++)
            {
                long long g = mask & get;
                get >>= 8;
                mem_arr[size1] = g;
                size1++;
                // we have added to one byte
            }
        }
    }
    return;
}
// dsiplay the memeory for user purposes
void displayMemoryInWordFormat(long long mem_arr[], ofstream& op) {
    long long Start = (1LL << 28) + 200; // Starting memory address (0x10000000)
    for (long long i = 203; i >= 0; i -= 4) {
        op << "0x";
        op << hex << Start << "    "; // Print memory address

        // Combine 4 bytes into a 32-bit word (little-endian format)
        long long word = 0;
        for (long long j = i; j >= i - 3; j--) {
            word = (word << 8) | mem_arr[j]; // Shift left by 8 bits and OR with the current byte
        }

        // Display the word in hexadecimal format (e.g., 0x00000000)
        op << "0x" << setw(8) << setfill('0') << hex << word << endl;

        Start -= 4; // Move to the next 32-bit word
    }
}
#endif