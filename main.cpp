//Construction 8 - branch operator.
//select <arithmetic expression>
//case <const> <operators>
//	[case <const> <operators>]
//	[default <operators>]
//	end

//<logical expression> → <comparison expression> | <unary logical operation><logical expression> | <logical expression><binary logical operation><logical expression>
//<comparison expression> → <operand> | <operand><comparision operation><operand>
//<comparison operation> → < | > | = | <>
//<unary logical operation> → not
//<binary logical operation> → and|or
//<operand> → <identifier> | <const>
//<operators> → <operators>;<operator> | <operator>
//<operator> → <identifier> = <arithmetic expression>
//<arithmetic expression> → <operand> | <operand><arithmetic operation><arithmetic expression>
//<arithmetic operation> → + | - | / | *

#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>

using namespace std;

enum LexemType { l_SELECT, l_CASE, l_DEFAULT, l_NOT, l_AND, l_OR, l_END, l_CONST, l_DELIMITER, l_VAR, l_ASSIGN, l_ARITHMETIC, l_COMPARE, l_ERROR, l_UNKNOWN };
struct Lexem {
	char* value;
	LexemType type;
	unsigned int length;
};

ostream& operator<<(ostream& os, LexemType l) {
	switch (l)
	{
	case l_SELECT: os << "SELECT KEYWORD";
		break;
	case l_CASE: os << "CASE KEYWORD";
		break;
	case l_DEFAULT: os << "DEFAULT KEYWORD";
		break;
	case l_END: os << "END KEYWORD";
		break;
	case l_CONST: os << "CONST";
		break;
	case l_VAR: os << "IDENTIFIER";
		break;
	case l_ASSIGN: os << "ASSIGN";
		break;
	case l_ARITHMETIC: os << "ARITHMETIC OPERATOR";
		break;
	case l_COMPARE: os << "COMPARISON OPERATOR";
		break;
	case l_ERROR: os << "ERROR LEXEME";
		break;
	case l_UNKNOWN: os << "UNKNOWN LEXEME";
		break;
	case l_NOT: os << "NOT KEYWORD";
		break;
	case l_OR: os << "OR KEYWORD";
		break;
	case l_AND: os << "AND KEYWORD";
		break;
	case l_DELIMITER: os << "DELIMITER";
		break;
	default: os << "LEXEME";
		break;
	}
	return os;
}

ostream& operator<<(ostream& os, Lexem& l) {
	os << "[" << l.type << ": \"" << l.value << "\"]";
	return os;
}

// Чтение всего текстового файла
char* readFile(const char filename[]) {
	ifstream in(filename, ios::binary); // Open input stream from file

	in.seekg(0, ios::end);
	int length = in.tellg();
	char* str = new char[length + 1];
	in.seekg(0, ios::beg);
	in.read(str, length);
	str[length] = '\0';

	in.close();

	for (size_t i = 0; i < length; ++i)
		cout << str[i];
	cout << endl << endl;

	return str;
}

enum l_State { START, WORD, CONST, SPECIAL, SPECIAL2, ARITHMETIC, ERROR, DELIMITER }; // Состояния конечного автомата
const int statesCount = 8;

int stateTable[] = {
	WORD,		WORD,		ERROR,		WORD,		WORD,		WORD,		ERROR,		WORD,		// letters
	ARITHMETIC, ARITHMETIC, ARITHMETIC, ARITHMETIC,	ARITHMETIC,	ARITHMETIC, ARITHMETIC,	ARITHMETIC, // -+/*
	CONST,		WORD,		CONST,		CONST,		CONST,		CONST,		ERROR,		CONST,		// digit
	SPECIAL,	SPECIAL,	SPECIAL,	SPECIAL,	SPECIAL,	SPECIAL,	SPECIAL,	SPECIAL,	// <
	SPECIAL2,	SPECIAL2,	SPECIAL2,	SPECIAL2,	SPECIAL2,	SPECIAL2,	SPECIAL2,	SPECIAL2,	// =
	SPECIAL2,	SPECIAL2,	SPECIAL2,	SPECIAL2,	SPECIAL2,	SPECIAL2,	SPECIAL2,	SPECIAL2,	// >
	DELIMITER,  DELIMITER,  DELIMITER,  DELIMITER,  DELIMITER,  DELIMITER,  DELIMITER,  DELIMITER,  // ;
	START,		START,		START,		START,		START,		START,		START,		START,		// '\n', '\r', '\t', ' '
	ERROR,      ERROR,      ERROR,      ERROR,      ERROR,      ERROR,      ERROR,      ERROR       // another
};

l_State updateState(l_State cur, char ch) {
	int stringNumber = 0;
	if (isalpha(ch)) stringNumber = 0;
	else if (ch == '-' || ch == '+' || ch == '/' || ch == '*')  stringNumber = 1;
	else if (isdigit(ch))  stringNumber = 2;
	else if (ch == '<')  stringNumber = 3;
	else if (ch == '=')  stringNumber = 4;
	else if (ch == '>')  stringNumber = 5;
	else if (ch == ';')  stringNumber = 6;
	else if (ch == '\n' || ch == '\r' || ch == '\t' || ch == ' ') stringNumber = 7;
	else stringNumber = 8;
	return (l_State)stateTable[stringNumber * statesCount + cur];
}

// Добавление лексемы
void addLexem(vector<Lexem>& arr, char* inputString, int startIndex, int length, l_State state) {
	Lexem curr;
	curr.value = new char[length + 1];
	strncpy_s(&curr.value[0], length + 1, &inputString[0] + startIndex, length);
	curr.length = length;
	switch (state)
	{
	case WORD:
		curr.type = LexemType::l_UNKNOWN;
		break;
	case SPECIAL2: case SPECIAL:
		curr.type = LexemType::l_COMPARE;
		break;
	case CONST:
		curr.type = LexemType::l_CONST;
		break;
	case ARITHMETIC:
		curr.type = LexemType::l_ARITHMETIC;
		break;
	case ERROR:
		curr.type = LexemType::l_ERROR;
		break;
	case DELIMITER:
		curr.type = LexemType::l_DELIMITER;
		break;
	}
	arr.push_back(curr);
}

// Лексический анализ
vector<Lexem>* lexAnalysis(char* inputString) {
	vector<Lexem> *arr = new vector<Lexem>();
	l_State currState = l_State::START, prevState;
	int startIndex = 0, currIndex = 0;

	while (inputString[currIndex] != '\0')
	{
		prevState = currState;
		char ch = inputString[currIndex];
		currState = updateState(currState, ch);

		if (prevState == l_State::ARITHMETIC || prevState == l_State::DELIMITER) {
			int length = 1;
			addLexem(*arr, inputString, startIndex, length, prevState);
			startIndex = currIndex;
		}
		else if (prevState == l_State::SPECIAL2) {
			int length = currIndex - startIndex;;
			addLexem(*arr, inputString, startIndex, length, prevState);
			startIndex = currIndex;
		}
		else if (prevState == l_State::SPECIAL && currState != l_State::SPECIAL2) {
			int length = 1;
			addLexem(*arr, inputString, startIndex, length, prevState);
			startIndex = currIndex;
		}
		else if (prevState == l_State::START && currState == l_State::ERROR)
			startIndex = currIndex;
		else if (prevState != currState && currState != l_State::ERROR && prevState != l_State::SPECIAL) {
			if (prevState != l_State::START) {
				int length = currIndex - startIndex;
				addLexem(*arr, inputString, startIndex, length, prevState);
			}
			startIndex = currIndex;
		}
		if (currState != l_State::START && inputString[currIndex + 1] == '\0') {
			int length = currIndex - startIndex + ((inputString[currIndex + 1] == '\0') ? 1 : 0);
			addLexem(*arr, inputString, startIndex, length, currState);
		}
		currIndex++;
	}
	return arr;
}

// Классификация ключевых слов и некоторых специальных символов
void lexClassification(vector<Lexem>& arr) {
	for (vector<Lexem>::iterator i = arr.begin(); i < arr.end(); ++i) {
		if ((*i).type == l_UNKNOWN) {
			if (strcmp(i->value, "select") == 0) i->type = l_SELECT;
			else if (strcmp(i->value, "case") == 0) i->type = l_CASE;
			else if (strcmp(i->value, "default") == 0) i->type = l_DEFAULT;
			else if (strcmp(i->value, "end") == 0) i->type = l_END;
			else if (strcmp(i->value, "not") == 0) i->type = l_NOT;
			else if (strcmp(i->value, "and") == 0) i->type = l_AND;
			else if (strcmp(i->value, "or") == 0) i->type = l_OR;
			else i->type = l_VAR;
		}
		else if (i->type == l_COMPARE && strcmp(i->value, "=") == 0) i->type = l_ASSIGN;
	}
}

// Вывод массива лексем
void print(ostream& out, vector<Lexem>& arr) {
	for (vector<Lexem>::iterator i = arr.begin(); i < arr.end(); ++i)
		out << setw(15) << left << i->value << " " << i->type << endl;
}

enum s_State { s_START, wait_math_exp, wait_math_exp_, waiting_const, waiting_statements, waiting_statement,
	wait_math_exp1, wait_math_exp_1, WME1, ME1, s_delimeter1,
	waiting_const2, waiting_statements2, waiting_statement2, wait_math_exp2, wait_math_exp_2, WME2, ME2, s_delimeter2,
	waiting_statements3, waiting_statement3, wait_math_exp3, wait_math_exp_3, WME3, ME3, s_delimeter3, ERR, FINAL
};

s_State prevB = s_START;
const int colCount = 9;

int s_stateTable[] = {
	wait_math_exp,  ERR,            ERR,                  ERR,    ERR,                 ERR,           ERR,                 ERR,            ERR,
	ERR,            ERR,	        ERR,                  ERR,    wait_math_exp_,      ERR,           wait_math_exp_,      ERR,            ERR,
	ERR,            waiting_const,  ERR,                  ERR,    ERR,	               ERR,	          ERR,                 ERR,	           wait_math_exp,
	ERR,            ERR,	        ERR,                  ERR,    waiting_statements,  ERR,	          ERR,                 ERR,	           ERR,
	wait_math_exp,	ERR,	        ERR,                  ERR,    ERR,	               ERR,	          waiting_statement,   ERR,	           ERR,
	ERR,	        ERR,	        ERR,                  ERR,    ERR,	               ERR,	          ERR,                 wait_math_exp1, ERR,
	ERR,	        ERR,	        ERR,                  ERR,    wait_math_exp_1,	   ERR,	          wait_math_exp_1,     ERR,	           ERR,
	ERR,	        waiting_const2,	waiting_statements3,  ERR,	  ERR,                 s_delimeter1,  ERR,                 ERR,	           WME1,
	ERR,	        ERR,	        ERR,	              ERR,	  ME1,                 ERR,           ME1,                 ERR,	           ERR,
	ERR,	        waiting_const2,	waiting_statements3,  FINAL,  ERR,                 s_delimeter1,  ERR,                 ERR,	           ERR,
	ERR,	        ERR,	        ERR,	              ERR,	  ERR,                 ERR,           waiting_statement,   ERR,	           ERR,
	ERR,	        ERR,	        ERR,	              ERR,	  waiting_statements2, ERR,           ERR,                 ERR,	           ERR,
	wait_math_exp,	ERR,	        ERR,	              ERR,	  ERR,                 ERR,           waiting_statement2,  ERR,	           ERR,
	ERR,	        ERR,	        ERR,	              ERR,	  ERR,	               ERR,           ERR,                 wait_math_exp2, ERR,
	ERR,            ERR,	        ERR,	              ERR,	  wait_math_exp_2,     ERR,           wait_math_exp_2,     ERR,	           ERR,
	ERR,            waiting_const2,	waiting_statements3,  FINAL,  ERR,                 s_delimeter2,  ERR,                 ERR,	           WME2,
	ERR,            ERR,	        ERR,	              ERR,	  ME2,                 ERR,           ME2,                 ERR,	           ERR,
	ERR,            waiting_const2,	waiting_statements3,  FINAL,  ERR,                 s_delimeter2,  ERR,                 ERR,	           ERR,
	ERR,            ERR,	        ERR,	              ERR,	  ERR,                 ERR,           waiting_statement2,  ERR,	           ERR,
	wait_math_exp,  ERR,	        ERR,	              ERR,	  ERR,                 ERR,           waiting_statement3,  ERR,	           ERR,
	ERR,            ERR,	        ERR,	              ERR,	  ERR,                 ERR,           ERR,                 wait_math_exp3, ERR,
	ERR,            ERR,	        ERR,	              ERR,	  wait_math_exp_3,     ERR,           wait_math_exp_3,     ERR,            ERR,
	ERR,            ERR,	        ERR,	              FINAL,  ERR,                 s_delimeter3,  ERR,                 ERR,            WME3,
	ERR,            ERR,	        ERR,	              ERR,	  ME3,                 ERR,           ME3,                 ERR,            ERR,
	ERR,            ERR,	        ERR,	              FINAL,  ERR,                 s_delimeter3,  ERR,                 ERR,            ERR,
	ERR,            ERR,	        ERR,	              ERR,	  ERR,                 ERR,           waiting_statement3,  ERR,            ERR
};

s_State update_sState(s_State curr, Lexem& lex) {
	int colNum = 0;
	if (lex.type == LexemType::l_SELECT)
		colNum = 0;
	else if (lex.type == LexemType::l_CASE)
		colNum = 1;
	else if (lex.type == LexemType::l_DEFAULT)
		colNum = 2;
	else if (lex.type == LexemType::l_END)
		colNum = 3;
	else if (lex.type == LexemType::l_CONST)
		colNum = 4;
	else if (lex.type == LexemType::l_DELIMITER)
		colNum = 5;
	else if (lex.type == LexemType::l_VAR)
		colNum = 6;
	else if (lex.type == LexemType::l_ASSIGN)
		colNum = 7;
	else if (lex.type == LexemType::l_ARITHMETIC)
		colNum = 8;
	if ((s_State)s_stateTable[curr * colCount + colNum] == wait_math_exp)
		prevB = curr;
	return (s_State)s_stateTable[curr * colCount + colNum];
}

void errPrint(s_State& prev, int& num) {
	switch (prev)
	{
	case s_START:
	{
		cout << "ERROR in " << num + 1 << " lexem: waiting for SELECT" << endl;
		break;
	}
	case wait_math_exp:
	{
		cout << "EROOR in " << num + 1 << " lexem: waiting for CONST or VAR" << endl;
		break;
	}
	case wait_math_exp_:
	{
		cout << "EROOR in " << num + 1 << " lexem: waiting for CASE or ARITHMETIC" << endl;
		break;
	}
	case waiting_const:
	{
		cout << "EROOR in " << num + 1 << " lexem: waiting for CONST" << endl;
		break;
	}
	case waiting_statements:
	{
		cout << "EROOR in " << num + 1 << " lexem: waiting for VAR" << endl;
		break;
	}
	case waiting_statement:
	{
		cout << "EROOR in " << num + 1 << " lexem: waiting for '='" << endl;
		break;
	}
	case wait_math_exp1:
	{
		cout << "EROOR in " << num + 1 << " lexem: waiting for CONST or VAR" << endl;
		break;
	}
	case wait_math_exp_1:
	{
		cout << "EROOR in " << num + 1 << " lexem: waiting for CASE or DEFAULT or ';' or ARITHMETIC" << endl;
		break;
	}
	case WME1:
	{
		cout << "EROOR in " << num + 1 << " lexem: waiting for CONST or VAR" << endl;
		break;
	}
	case ME1:
	{
		cout << "EROOR in " << num + 1 << " lexem: waiting for END or CASE or DEFAULT or ';'" << endl;
		break;
	}
	case s_delimeter1:
	{
		cout << "EROOR in " << num + 1 << " lexem: waiting for VAR" << endl;
		break;
	}
	case waiting_const2:
	{
		cout << "EROOR in " << num + 1 << " lexem: waiting for CONST" << endl;
		break;
	}
	case waiting_statements2:
	{
		cout << "EROOR in " << num + 1 << " lexem: waiting for VAR" << endl;
		break;
	}
	case waiting_statement2:
	{
		cout << "EROOR in " << num + 1 << " lexem: waiting for '='" << endl;
		break;
	}
	case wait_math_exp2:
	{
		cout << "EROOR in " << num + 1 << " lexem: waiting for CONST or VAR" << endl;
		break;
	}
	case wait_math_exp_2:
	{
		cout << "EROOR in " << num + 1 << " lexem: waiting for END or CASE or DEFAULT or ';' or ARITHMETIC" << endl;
		break;
	}
	case WME2:
	{
		cout << "EROOR in " << num + 1 << " lexem: waiting for CONST or VAR" << endl;
		break;
	}
	case ME2:
	{
		cout << "EROOR in " << num + 1 << " lexem: waiting for END or CASE or DEFAULT or ';'" << endl;
		break;
	}
	case s_delimeter2:
	{
		cout << "EROOR in " << num + 1 << " lexem: waiting for VAR" << endl;
		break;
	}
	case waiting_statements3:
	{
		cout << "EROOR in " << num + 1 << " lexem: waiting for VAR" << endl;
		break;
	}
	case waiting_statement3:
	{
		cout << "EROOR in " << num + 1 << " lexem: waiting for '='" << endl;
		break;
	}
	case wait_math_exp3:
	{
		cout << "EROOR in " << num + 1 << " lexem: waiting for CONST or VAR" << endl;
		break;
	}
	case wait_math_exp_3:
	{
		cout << "EROOR in " << num + 1 << " lexem: waiting for END or ';' or ARITHMETIC" << endl;
		break;
	}
	case WME3:
	{
		cout << "EROOR in " << num + 1 << " lexem: waiting for CONST or VAR" << endl;
		break;
	}
	case ME3:
	{
		cout << "EROOR in " << num + 1 << " lexem: waiting for END or ';'" << endl;
		break;
	}
	case s_delimeter3:
	{
		cout << "EROOR in " << num + 1 << " lexem: waiting for VAR" << endl;
		break;
	}
	}
}



void syntaxAnalisys(vector<Lexem>& arr) {
	int counter = 0;
	int currIndex = 0;
	bool isValid = true;
	s_State curr = s_START, prev = s_START;

	while (currIndex < arr.size())
	{
		prev = curr;
		curr = update_sState(curr, arr[currIndex]);
		if (curr == ERR) {
			isValid = false;
			errPrint(prev, currIndex);
			while (arr[currIndex].type != l_END && arr[currIndex].type != l_DEFAULT && arr[currIndex].type != l_CASE && currIndex < arr.size() - 1)
				++currIndex;
			if (arr[currIndex].type == l_DEFAULT)
				curr = waiting_statements3;
			if (arr[currIndex].type == l_CASE)
				curr = waiting_const2;
			if (arr[currIndex].type == l_END) {
				curr = waiting_statements;
				if (currIndex + 1 < arr.size() && arr[currIndex + 1].type == l_DELIMITER)
					++currIndex;
				if (counter == 0) {
					cout << "Unless 'end'" << endl << endl;
				}
				else
					--counter;
			}
		}
		else if (arr[currIndex].type == l_SELECT) {
			counter++;
		}
		else if (arr[currIndex].type == l_END) {
			if (currIndex + 1 < arr.size() && arr[currIndex + 1].type == l_DELIMITER)
				++currIndex;
			if (counter == 0) {
				cout << "unless 'end'" << endl << endl;
			}
			else
				--counter;
			curr = waiting_statements;
		}
		/*else if (arr[currIndex].type == l_END && currIndex + 1 < arr.size()) {
			curr = s_START;
			if (isValid)
				cout << "ALL IS GOOD" << endl << endl;
			else
				cout << "WRONG" << endl << endl;
		}*/
		/*else if (arr[currIndex].type == l_END && currIndex + 1 < arr.size() && (arr[currIndex + 1].type == l_DELIMITER || arr[currIndex + 1].type == l_END || arr[currIndex + 1].type == l_CASE || arr[currIndex + 1].type == l_DEFAULT)) {
			if (arr[currIndex + 1].type == l_DELIMITER && prevB == waiting_statements)
				curr = ME1;
			if (arr[currIndex + 1].type == l_DELIMITER && prevB == waiting_statements2)
				curr = ME2;
			if (arr[currIndex + 1].type == l_DELIMITER && prevB == waiting_statements3)
				curr = ME2;
			if (arr[currIndex + 1].type == l_END && (prevB == waiting_statements3 || prevB == waiting_statements2 || prevB == waiting_statements))
				curr = ME1;
			if (arr[currIndex + 1].type == l_CASE && (prevB == waiting_statements || prevB == waiting_statements2))
				curr = wait_math_exp_1;
			if (arr[currIndex + 1].type == l_CASE && prevB == waiting_statements3)
				curr = wait_math_exp_3;
			if (arr[currIndex + 1].type == l_DEFAULT && prevB == waiting_statements3)
				curr = wait_math_exp_3;
			if (arr[currIndex + 1].type == l_DEFAULT && (prevB == waiting_statements || prevB == waiting_statements2))
				curr = wait_math_exp_2;
		}*/
		++currIndex;
	}
	if (isValid && !counter)
		cout << "ALL IS GOOD" << endl;
	if (counter > 0)
		cout << "Less 'end'" << endl;
}


int main() {
	char *input = readFile("input.txt");

	vector<Lexem>* lexems = lexAnalysis(input);
	lexClassification(*lexems);

	ofstream out("output.txt");
	print(out, *lexems);
	out.close();

	print(cout, *lexems);
	
	cout << endl << endl;

	syntaxAnalisys(*lexems);

	system("pause");
	for (size_t i = 0; i < lexems->size(); i++)
		delete lexems->at(i).value;
	delete lexems;
	delete[] input;
	return 0;
}