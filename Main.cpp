#include <bits/stdc++.h>
using namespace std;

// For convenience
typedef long long ll;
#define nl "\n"
#define INPUT_FILE "input.txt"
#define OUTPUT_FILE "output.txt"

// Class for a instruction
class InsDecode
{
private:
    string command, hex, binary, opcode;
    string rs1, rs2, rd, imm, ins;
    ll func3, func7;
    char format;
    bool valid;

public:
    string label;
    int toLabel;
    // In case, instruction does not make sense.
    void badIns()
    {
        valid = 0;
        cout << "Bad Instruction ";
    }

    // Converts binary(string) to decimal(long long)
    ll BinToDec(string s, bool sign)
    {
        ll size = s.size();
        ll ans = 0;
        for (ll i = size - 1; i >= 0; i--)
        {
            if (s[i] == '1')
                ans += 1 << (size - i - 1);
        }
        if (sign == 1 && s[0] == '1')
            ans -= 1 << size;
        return ans;
    }

    // Converts decimal(long long) to binary(string)
    string DecToBin(ll dec)
    {
        string bin = "";
        for (ll i = 0; i <= 3; i++)
        {
            string app = (dec % 2) ? "1" : "0";
            bin.append(app);
            dec /= 2;
        }
        return bin;
    }

    // Constructor for the class. Takes input and converts to binary.
    InsDecode(string line)
    {
        valid = 1;
        hex = line;
        if (hex.size() != 8)
            badIns();
        else
        {
            for (ll i = 0; i <= 7; i++)
            {
                char C = hex[i];
                ll num;
                if (C <= 57 && C >= 48)
                    num = C - 48;
                else if (C <= 70 && C >= 65)
                    num = C - 55;
                else if (C <= 102 && C >= 97)
                    num = C - 87;
                else
                {
                    badIns();
                    break;
                }
                string bin = DecToBin(num);
                reverse(bin.begin(), bin.end());
                binary.append(bin);
            }
        }
    }

    // Calculates immediate value, depending on the format of the instruction
    string check_imm()
    {
        bool sign = 1;
        string ans = "";
        if (format == 'I')
        {
            if (opcode == "0010011" && (func3 == 1 || func3 == 5))
            {
                ans = binary.substr(6, 6);
                sign = 0;
            }
            else
                ans = binary.substr(0, 12);
        }
        else if (format == 'U')
        {
            ans = binary.substr(0, 20);
            sign = 0;
        }
        else if (format == 'S')
            ans = binary.substr(0, 7).append(binary.substr(20, 5));
        else if (format == 'B')
            ans = binary.substr(0, 1).append(binary.substr(24, 1)).append(binary.substr(1, 6)).append(binary.substr(20, 4)).append("0");
        else if (format == 'J')
            ans = binary.substr(0, 1).append(binary.substr(12, 8)).append(binary.substr(11, 1)).append(binary.substr(1, 10)).append("0");
        ll immediate = BinToDec(ans, sign);
        if (format == 'J' || format == 'B')
        {
            toLabel = immediate;
            label = "Not NULL";
        }
        else
            label = "NULL";
        return to_string(immediate);
    }

    // Merges all required values such as rd, rs1 .etc.
    void insMerge()
    {
        switch (format)
        {
        case 'R':
            command = ins + " " + rd + ", " + rs1 + ", " + rs2;
            break;
        case 'I':
            if (opcode == "0000011")
                command = ins + " " + rd + ", " + imm + "(" + rs1 + ")";
            else
                command = ins + " " + rd + ", " + rs1 + ", " + imm;
            break;
        case 'U':
            command = ins + " " + rd + ", " + imm;
            break;
        case 'B':
            command = ins + " " + rs1 + ", " + rs2 + ", " + imm;
            break;
        case 'S':
            command = ins + " " + rs2 + ", " + imm + "(" + rs1 + ")";
            break;
        case 'J':
            command = ins + " " + rd + ", " + imm;
            break;
        }
    }

    // Checks opcode and sets format of the instruction
    void check_Format()
    {
        map<string, char> opcodeList = {
            {"0110011", 'R'},
            {"1101111", 'J'},
            {"0100011", 'S'},
            {"1100011", 'B'},
            {"0110111", 'U'},
            //{"0010111", 'U'},
            {"0010011", 'I'},
            {"0000011", 'I'},
            {"1100111", 'I'},
            //{"1110011", 'I'}
        };
        opcode = binary.substr(25, 7);
        auto it = opcodeList.find(opcode);
        if (it == opcodeList.end())
            badIns();
        else
            format = it->second;
    }

    // Checks opcode, func3, func7 and sets instruction
    void checkIns()
    {
        map<string, string> insList = {
            {"011001100", "add"},
            {"0110011032", "sub"},
            {"011001110", "sll"},
            //{"011001120", "slt"},
            //{"011001130", "sltu"},
            {"011001140", "xor"},
            {"011001150", "srl"},
            {"0110011532", "sra"},
            {"011001160", "or"},
            {"011001170", "and"},
            {"1101111", "jal"},
            {"01000110", "sb"},
            {"01000111", "sh"},
            {"01000112", "sw"},
            {"01000113", "sd"},
            {"11000110", "beq"},
            {"11000111", "bne"},
            {"11000114", "blt"},
            {"11000115", "bge"},
            {"11000116", "bltu"},
            {"11000117", "bgeu"},
            {"0110111", "lui"},
            //{"0010111", "auipc"},
            {"00100110", "addi"},
            {"001001110", "slli"},
            //{"00100112", "slti"},
            //{"00100113", "sltiu"},
            {"00100114", "xori"},
            {"001001150", "srli"},
            // Risc-V card says srai should be "0010011532" but Ripes gives "0010011516"
            {"0010011516", "srai"}, 
            {"00100116", "ori"},
            {"00100117", "andi"},
            {"00000110", "lb"},
            {"00000111", "lh"},
            {"00000112", "lw"},
            {"00000113", "ld"},
            {"00000114", "lbu"},
            {"00000115", "lhu"},
            {"00000116", "lwu"},
            {"11001110", "jalr"}};
        string insMain = opcode;
        if (format != 'J' && format != 'U')
            insMain.append(to_string(func3));
        if (format == 'R')
            insMain.append(to_string(func7));
        else if (opcode == "0010011" && (func3 == 1 || func3 == 5))
            insMain.append(to_string(BinToDec(binary.substr(0, 6), 0)));
        auto it = insList.find(insMain);
        if (it == insList.end())
            badIns();
        else
            ins = it->second;
    }

    // Main code for class(each line).
    string solve(map<ll, ll> *labels, ll lineNo)
    {
        if (valid == 0)
            return "NULL";
        else
        {
            check_Format();
            if (valid == 0)
                return "NULL";

            // Set all required variables

            if (format != 'U' && format != 'J')
                rs1 = to_string(BinToDec(binary.substr(12, 5), 0)).insert(0, "x");
            if (format != 'U' && format != 'I' && format != 'J')
                rs2 = to_string(BinToDec(binary.substr(7, 5), 0)).insert(0, "x");
            if (format != 'B' && format != 'S')
                rd = to_string(BinToDec(binary.substr(20, 5), 0)).insert(0, "x");
            if (format == 'R')
                func7 = BinToDec(binary.substr(0, 7), 0);
            if (format != 'U' && format != 'J')
                func3 = BinToDec(binary.substr(17, 3), 0);
            imm = check_imm();
            checkIns();
            if (valid == 0)
                return "NULL";

            // Replace immediate with label if needed.
            if (!label.compare("Not NULL"))
            {
                toLabel = lineNo + toLabel / 4;
                labels->insert(make_pair(toLabel, labels->size() + 1));
                auto it = labels->find(toLabel);
                if (it != labels->end())
                    imm = "L" + to_string(it->second);
            }
            insMerge();
            return command;
        }
    }
};

// Add Labels at start of line
void startLabels(map<ll, ll> labels)
{
    ifstream input(OUTPUT_FILE);
    ofstream output("temp.txt");
    string line;
    ll lineNo = 0;
    while (getline(input, line))
    {
        lineNo++;
        auto it = labels.find(lineNo);
        if (it != labels.end())
            output << "L" << to_string(it->second) << ": ";
        output << line << nl;
    }
    output.close();
    input.close();
    remove(OUTPUT_FILE);
    rename("temp.txt", OUTPUT_FILE);
}

// Main for program
int main()
{
    ifstream input(INPUT_FILE);
    ofstream output(OUTPUT_FILE);
    string line;
    map<ll, ll> labels;
    ll lineNo = 0, fileLine = 0;
    while (getline(input, line))
    {
        lineNo++;
        fileLine++;
        InsDecode *Fac = new InsDecode(line);
        line = Fac->solve(&labels, lineNo);
        if (line.compare("NULL"))
            output << line << nl;
        else
        { // In case of error in line
            cout << "at Line " << fileLine << nl;
            lineNo--;
        }
        delete (Fac);
    }
    output.close();
    input.close();
    startLabels(labels);
    return 0;
}