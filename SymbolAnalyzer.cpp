/*  
    CPSC-323 - ASSIGNMENT 3
    SAMANTHA IBASITAS 889328258 AND STEVEN PHAM 889423067
    05/16/21
    final interation
*/  

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
using namespace std;

struct codeword {
	string token;
	string lexeme;
};

struct identifier {
    string lexeme;
    int address;
    string type;
};

struct instruction {
    int index;
    string cmd;
    int value;
};

// fsm[current_state][input_category]
int fsm [8][8] =    {{3, 1, 2, 2, 0, 5, -1, 7},
                    {1, 1, 2, 2, 2, 5, -1, 2},
                    {3, 1, 2, 2, 0, 5, -1, 7},
                    {3, -1, 4, 4, 4, 5, -1, 4},
                    {3, 1, 2, 2, 0, 5, -1, 7},
                    {5, 5, 5, 5, 5, 6, 6, 7},
                    {3, 1, 2, 2, 0, 5, -1, 7},
                    {-1, -1, -1, -1, -1, -1, -1, -1}};

int current_state = 0;
int input_category;
bool comment = false;
string keywords[21] = {"int", "float", "bool", "true", "false", "if", "else", "then", "endif", "endelse", "while", "whileend", "do", "enddo", "for", "endfor", "STDinput", "STDoutput", "and", "or", "not"};
int MEMORY_ADDRESS = 5000;
int STACK_INDEX = 1;
int END_LOOPS = -1;
string ASSIGNING_INT = "";
string ASSIGNING_ID = "";
string ID_DECLARATION = "ERROR";
string DOUBLE_OPERATOR = "ERROR";

int idType = 0;

// ADD TO VECTOR WITH TOKEN AND LEXEME LABELED
void saveUnit(string toke, string lex, vector<codeword> &_codewords, vector<identifier> &_symboltable, vector<instruction> &_instructions) {
    codeword newEntry;
    newEntry.token = toke;
    newEntry.lexeme = lex;

    // if the token is a keyword that is about to declare an identifier
    if(newEntry.token == "KEYWORD" && (newEntry.lexeme == "int" || newEntry.lexeme == "float" || newEntry.lexeme == "bool")) {
        ID_DECLARATION = newEntry.lexeme;
    }
    _codewords.push_back(newEntry);

    // if the token being saved is an identifier
    if(newEntry.token == "IDENTIFIER" && (ID_DECLARATION != "ERROR"))
    {
        // check if the identifier has been declared
        for (int i = 0; i < _symboltable.size(); i++) {
            if(newEntry.lexeme == _symboltable[i].lexeme) {
                return;
            }
        }

        // adds new identifier to the symbol table
        identifier newID;
        newID.lexeme = lex;
        newID.address = MEMORY_ADDRESS;
        MEMORY_ADDRESS++;
        newID.type = ID_DECLARATION;
        _symboltable.push_back(newID);
    }
}

// ADD TO INSTRUCTION VECTOR
void saveCMD(string cmd, int num, vector<identifier> &_symboltable, vector<instruction> &_instructions) {
    instruction newCMD;
    newCMD.index = STACK_INDEX;
    STACK_INDEX++;
    newCMD.cmd = cmd;
    newCMD.value = num;

    _instructions.push_back(newCMD);
}

// TAKE INPUT AND CATEGORIZE IT TO THE FSM COLUMN
int checkInput(char input, vector<codeword> &_codewords, vector<identifier> &_symboltable, vector<instruction> &_instructions) {

    // NUMBER [0]
    if(isdigit(input))
    {
        input_category = 0;
    }

    // CHARACTER [1]
    if(isalpha(input))
    {
        input_category = 1;
    }

    // OPERATORS [2]
    char operators[9] = {'*', '+', '-', '=', '/', '>', '<', '%', '^'};
    for (int i = 0; i < sizeof(operators) / sizeof(operators[0]); i++) {
        if (input == operators[i]) {
            input_category = 2;
        }
    }

    // SEPARATOR [3]
    char separators[11] = {'\'', '(', ')', '{', '}', '[', ']', ',', ':', ';'};
    for (int i = 0; i < sizeof(separators) / sizeof(separators[0]); i++) {
        if (input == separators[i]) {
            input_category = 3;
        }
    }

    // 2ND ! [6]
    if(input == '!' && comment == true)
    {
        input_category = 6;
        return input_category;
    }

    // ! [5]
    if(input == '!' && comment == false)
    {
        input_category = 5;
        return input_category;
    }

    // space [4]
    if (input == ' ' or input == '\t' or input == '\n') {
        input_category = 4;
    }

    return input_category;
}

// SWITCH CASES FOR NEW STATE
void switchState(int new_state, string &word, char input, vector<codeword> &_codewords, vector<identifier> &_symboltable, vector<instruction> &_instructions) {
    switch(new_state){
        /*case 0:
            current_state = 0;
            break;*/

        case 1: // Build word
            word += input;
            current_state = 1;
            break;

        case 2: // End word
            // word is a KEYWORD
            //string keywords[21] = {"int", "float", "bool", "true", "false", "if", "else", "then", "endif", "endelse", "while", "whileend", "do", "enddo", "for", "endfor", "STDinput", "STDoutput", "and", "or", "not"};
            for(int i = 0; i < sizeof(keywords)/sizeof(keywords[0]); i++) {
                if(word == keywords[i]) { 
                    saveUnit("KEYWORD", word, _codewords, _symboltable, _instructions);
                    word.erase();
                }
            }

            // word is an IDENTIFIER
            if(word != "") {
                saveUnit("IDENTIFIER", word, _codewords, _symboltable, _instructions);
            }
            word.erase();

            // if triggered by OPERATOR
            if(input_category == 2)
            {
                string temp(1,input);
                saveUnit("OPERATOR", temp, _codewords, _symboltable, _instructions);
                word.erase();
            }

            // if triggered by SEPARATOR
            if(input_category == 3)
            {
                string temp(1,input);
                saveUnit("SEPARATOR", temp, _codewords, _symboltable, _instructions);
                word.erase();
            }
        
            // if triggered by SPACE
            if(input_category == 4)
            {
                word.erase();
            }

            current_state = 2;
            break;

        case 3: // Build number
            word += input;
            current_state = 3;
            break;

        case 4: // End number
            saveUnit("REAL", word, _codewords, _symboltable, _instructions);

            // if triggered by OPERATOR
            if(input_category == 2)
            {
                string temp(1,input);
                saveUnit("OPERATOR", temp, _codewords, _symboltable, _instructions);
                word.erase();
            }

            // if triggered by SEPARATOR
            if(input_category == 3)
            {
                string temp(1,input);
                saveUnit("SEPARATOR", temp, _codewords, _symboltable, _instructions);
                word.erase();
            }

            // if triggered by SPACE
            if(input_category == 4)
            {
                word.erase();
            }

            current_state = 4;
            break;

        case 5: // Build comment
            comment = true;
            word.erase();
            current_state = 5;
            break;

        case 6: // End comment
            comment = false;
            word.erase();
            current_state = 6;
            break;

        case 7: // End of line/file
            ID_DECLARATION = "ERROR";
            // ends unfinished word
            if(current_state == 1)
            {
                // word is a KEYWORD
                for(int i = 0; i < sizeof(keywords)/sizeof(keywords[0]); i++) {
                if(word == keywords[i]) { 
                    saveUnit("KEYWORD", word, _codewords, _symboltable, _instructions);
                    word.erase();
                    }
                }

                // word is an IDENTIFIER
                if(word != "") {
                    saveUnit("IDENTIFIER", word, _codewords, _symboltable, _instructions);
                }
                word.erase();
                break;
            }

            // ends unfinished number
            if(current_state == 3)
            {
                saveUnit("REAL", word, _codewords, _symboltable, _instructions);
                current_state = 7;
                word.erase();
                break;
            }

            // ends line comment
            if(current_state == 5)
            {
                comment = false;
                current_state = 7;
                break;
            }
            break;
    }
}

// ITERATES CHARACTER BY CHARACTER
void lexer(string &line, vector<codeword> &_codewords, vector<identifier> &_symboltable, vector<instruction> &_instructions) {
    string word;
    char character;
    for(int i = 0; i < line.length() ; i++) {
        character = line[i];
        switchState(fsm[current_state][checkInput(character, _codewords, _symboltable, _instructions)], word, character, _codewords, _symboltable, _instructions);
    }

    // END OF FILE [7]
    switchState(fsm[current_state][7], word, character, _codewords, _symboltable, _instructions);
}

// GRAMMER RULES
string rules(string &token, string &lexeme, vector<codeword> &_codewords, vector<identifier> &_symboltable, vector<instruction> &_instructions) {

    // if identifier
    if(!token.compare("IDENTIFIER")) {
        // if very first identifier
        DOUBLE_OPERATOR = "ERROR";
        ASSIGNING_ID = lexeme;
        if(!lexeme.compare("input")) {
            saveCMD("STDIN", -1, _symboltable, _instructions);
        }
        else if(!lexeme.compare("output")) {
            saveCMD("STDOUT", -1, _symboltable, _instructions);
        }
        else {
            for(int i = 0; i < _symboltable.size(); i++) {
                if(!lexeme.compare(_symboltable[i].lexeme)) {
                    saveCMD("POPM/PUSHM", _symboltable[i].address, _symboltable, _instructions);
                }
            }
        }

        if(idType == 0) {            
            idType = 1;
            return "<Statement List> -> <Statement> | <Statement> <Statement List>\n\t\t<Statement> -> <Compound> | <Assign> | <If> | <Return> | <Print> | <Scan> | <While>\n\t\t<Assign> -> <Identifier> = <Expression>;";
        }
        // if starting identifier
        else if(idType == 1) {
            return "<Statement> -> <Compound> | <Assign> | <If> | <Return> | <Print> | <Scan> | <While>\n\t\t<Assign> -> <Identifier> = <Expression>;";
        }
        // if first identifier in operation
        else if(idType == 2) {
            return "<Expression> -> <Term> <ExpressionPrime>\n\t\t<Term> -> <Factor> <TermPrime>\n\t\t<Factor> -> - <Primary> | <Primary>\n\t\t<Primary> -> <Identifier> | <Integer> | <Identifier> ( <IDs> ) | ( <Expression> ) | <Real> | true | false";
        }
        // if after + or -
        else if(idType == 3) {
            return "<Term> -> <Factor> <TermPrime>\n\t\t<Factor> -> - <Primary> | <Primary>\n\t\t<Primary> -> <Identifier> | <Integer> | <Identifier> ( <IDs> ) | ( <Expression> ) | <Real> | true | false;";
        }
        // if after * or /
        else if(idType == 4) {
            return "<Factor> -> - <Primary> | <Primary>\n\t\t<Primary> -> <Identifier> | <Integer> | <Identifier> ( <IDs> ) | ( <Expression> ) | <Real> | true | false";
        }
        return "ERROR, Identifier type not found";
    }

    // keyword WHILE to flag LABEL in stack
    else if(!lexeme.compare("while")) {
        DOUBLE_OPERATOR = "ERROR";
        saveCMD("LABEL", -1, _symboltable, _instructions);
    }

    // start of loop JUMPZ
    else if(!lexeme.compare("{")) {
        DOUBLE_OPERATOR = "ERROR";
        saveCMD("JUMPZ", -1, _symboltable, _instructions);
    }

    // end of loop JUMP
    else if(!lexeme.compare("}")) {
        DOUBLE_OPERATOR = "ERROR";
        int label_index = -1;
        for (int i = _instructions.size()-1; i > 0; i--) {
            // searches for LABEL index
            if(_instructions[i].cmd == "LABEL") {
                label_index = _instructions[i].index;
                i = 0;
            }
        }
        saveCMD("JUMP", label_index, _symboltable, _instructions);
    }

    // =
    else if(!lexeme.compare("=")) {
        idType = 2;
        if(DOUBLE_OPERATOR == "=") {
            saveCMD("EQU", -1, _symboltable, _instructions);
            DOUBLE_OPERATOR = "ERROR";
            return "";
        }
        else if(DOUBLE_OPERATOR == ">") {
            _instructions.pop_back();
            saveCMD("GEQ", -1, _symboltable, _instructions);
            DOUBLE_OPERATOR = "ERROR";
            return "";
        }
        else if(DOUBLE_OPERATOR == "<") {
            _instructions.pop_back();
            saveCMD("LEQ", -1, _symboltable, _instructions);
            DOUBLE_OPERATOR = "ERROR";
            return "";
        }
        else if(DOUBLE_OPERATOR == "^") {
            saveCMD("NEQ", -1, _symboltable, _instructions);
            DOUBLE_OPERATOR = "ERROR";
            return "";
        }
        DOUBLE_OPERATOR = "=";
        return "";
    }

    // if + or -
    else if(!lexeme.compare("+") || !lexeme.compare("-")) {
        DOUBLE_OPERATOR = "ERROR";
        idType = 3;

        if(!lexeme.compare("+")) {
            saveCMD("ADD", -1, _symboltable, _instructions);
        }
        else {
            saveCMD("SUB", -1, _symboltable, _instructions);
        }

        return "<Empty> -> Epsilon\n\t\t<TermPrime> -> * <Factor> <TermPrime> | / <Factor> <TermPrime> | <Empty>\n\t\t<Empty> -> Epsilon\n\t\t<ExpressionPrime> -> + <Term> <ExpressionPrime> | - <Term> <ExpressionPrime> | <Empty>";
    }

    // if * or /
    else if(!lexeme.compare("*") || !lexeme.compare("/")) {
        DOUBLE_OPERATOR = "ERROR";
        idType = 4;

        if(!lexeme.compare("*")) {
            saveCMD("MUL", -1, _symboltable, _instructions);
        }
        else {
            saveCMD("DIV", -1, _symboltable, _instructions);
        }

        return "<Empty> -> Epsilon\n\t\t<TermPrime> -> * <Factor> <TermPrime> | / <Factor> <TermPrime> | <Empty>;";
    }

    // if ;
    else if(!lexeme.compare(";")) {
        DOUBLE_OPERATOR = "ERROR";
        idType = 1;
        return "<Empty> -> Epsilon\n\t\t<TermPrime> -> * <Factor> <TermPrime> | / <Factor> <TermPrime> | <Empty>\n\t\t<Empty> -> Epsilon\n\t\t<ExpressionPrime> -> + <Term> <ExpressionPrime> | - <Term> <ExpressionPrime> | <Empty>\n\t\t<Empty> -> Epsilon";
    }

    // if < > ^
    else if(!lexeme.compare(">")) {
        saveCMD("GRT", -1, _symboltable, _instructions);
        DOUBLE_OPERATOR = ">";
        return "";
    }
    else if(!lexeme.compare("<")) {
        saveCMD("LES", -1, _symboltable, _instructions);
        DOUBLE_OPERATOR = "<";
        return "";
    }
    else if(!lexeme.compare("^")) {
        DOUBLE_OPERATOR = "^";
        return "";
    }

    // if number, build and save
    else if(!lexeme.compare("0") || !lexeme.compare("1") || !lexeme.compare("2") || !lexeme.compare("3") || !lexeme.compare("4") || !lexeme.compare("5") || !lexeme.compare("6") || !lexeme.compare("7") || !lexeme.compare("8") || !lexeme.compare("9")) {
        ASSIGNING_INT += lexeme;
        for(int i = 0; i < _codewords.size(); i++){
            if(_codewords[i].lexeme == ASSIGNING_INT) {
                // push int
                saveCMD("PUSHI", stoi(ASSIGNING_INT), _symboltable, _instructions);
                ASSIGNING_INT = "";
                DOUBLE_OPERATOR = "ERROR";
                return "";
            }
        }
    }

    /* if characters, build identifier and save
    else {
        for(int i = 0; i < lexeme.size(); i++) {
            cout << lexeme[i] << endl;
            if(isalpha(lexeme[i])) {
                ASSIGNING_ID += lexeme[i];
                for(int j = 0; j < _symboltable.size(); j++){
                    if(_symboltable[j].lexeme == ASSIGNING_ID) {
                        // push int
                        saveCMD("POP/PUSH", _symboltable[j].address, _symboltable, _instructions);
                        ASSIGNING_ID = "";
                        DOUBLE_OPERATOR = "ERROR";
                        return "";
                    }
                }
            }
        }
    }*/
    DOUBLE_OPERATOR = "ERROR";
    return "";
}

int main()
{
	// READ FILE
    ifstream inputFile ("test.txt");
	if (!inputFile.is_open()) {
    		cout << "Input file failed to open.\n";
   		 exit(0);
  	}

	// VARIABLES
	vector<codeword> _codewords;
    vector<identifier> _symboltable;
    vector<instruction> _instructions;
    string line;

    // READING FILE BY LINE -> WORDS -> ANALYZE
    while (getline(inputFile, line)) {
	    lexer(line, _codewords, _symboltable, _instructions);
    }

    // OUTPUT FILE AND PRINTS VECTOR
    ofstream outputFile ("output.txt");
    if (outputFile.is_open()) {
        //outputFile << "TOKENS \t\t\tLexemes\n" << endl;
        for (int i = 0; i < _codewords.size(); i++) {

            // PRINTS TOKEN AND LEXEME
            //outputFile << "Token: " << _codewords[i].token << "\t\tLexeme: " << _codewords[i].lexeme << "\n";

            // PRINTS GRAMMAR RULES
            //outputFile << "\t\t" << rules(_codewords[i].token, _codewords[i].lexeme) << "\n" << endl;
            rules(_codewords[i].token, _codewords[i].lexeme, _codewords, _symboltable, _instructions);
        }

        // PRINTS COMMANDS TABLE
        outputFile << setw(2) << "#" << setw(15) << "ASSEMBLY" << setw(10) << "MEMORY" << endl;
        for (int i = 0; i < _instructions.size(); i++) {

            // updates JUMPZ
            if(_instructions[i].cmd == "JUMPZ") {
                for(int j = i; j < _instructions.size(); j++) {
                    if(_instructions[j].cmd == "JUMP") {
                        END_LOOPS = _instructions[j].index + 1;
                        _instructions[i].value = END_LOOPS;
                        j = _instructions.size() + 5;
                    }
                }
            }


            outputFile << setw(2) << _instructions[i].index << setw(15) << _instructions[i].cmd << setw(10);
            // prints memory if there is any
            if(_instructions[i].value != -1) {
                outputFile << _instructions[i].value << endl;
            }
            else {
                outputFile << endl;
            }
        }

        // PRINTS SYMBOL TABLE
        outputFile << setw(10) << "------------------------------\nIDENTIFIER" << setw(10) << "MEMORY" << setw(10) << "TYPE" << endl;
        for (int i = 0; i < _symboltable.size(); i++) {
            outputFile << setw(10) << _symboltable[i].lexeme << setw(10) << _symboltable[i].address << setw(10)  << _symboltable[i].type << endl;
        }
        outputFile.close();
    }
    return 0;
}