#include "Query.h"
#include <stack>
#include <cctype>

// global variables
std::stack<Query> queryStack;			// 保存运算结果的 stack
std::stack<char> operatorStack;			// 保存运算符号的 stack

// 三个运算
void runQuery(char);
void andQuery();
void notQuery();
void orQuery();

Query inputAndRun();					// 读入查询语句，返回查询结果
void runQueries(std::ifstream&);		// 处理与用户的交互

int main(int argc, char* argv[])
{
	// 读入文件，处理错误输入
	if (argc < 2)
	{
		std::cout << "error: No file name arg.";
		return 0;
	}

	auto file = std::ifstream(argv[1]);

	if (file.fail())
	{
		std::cout << "error: Open file " << argv[1] << " fail.";
		return 0;
	}

	// 运行查询程序
	runQueries(file);
}

void runQueries(std::ifstream& infile)
{
	// infile 是一个 ifstream,指向我们要处理的文件
	TextQuery tq(infile);	// 保存文件并建立查询 map
	// 与用户交互：提示用户输入查询的语句，完成查询并打印结果
	while (true) {
		std::cout << "Please put brackets and executing Query for:";
		Query q = inputAndRun();
		print(std::cout, q.eval(tq)) << std::endl;
	}
}

Query inputAndRun()
{
	char c;									// 读入的单个字符
	std::string word;						// 由 c 合成的单词
	char oper;								// 运算符

	while (std::cin >> std::noskipws >> c) {
		if (c == '\n') break;
		if (isalpha(c))
			word.push_back(c);	// 单个字母加入到 word 中，然后继续读取
		else if (c == '(' || c == '&' || c == '|' || c == '~') {	// 这些运算符统统入 operatorstack
			if (!word.empty())
			{// 将前一个单词的查询结果 wordquery 入 querystack
				queryStack.push(Query(word));
				word = "";			// 重置 word
			}
			operatorStack.push(c);	// 此 运算符 入 operatorstack
		}
		else if (c == ')') { // 开始运算
			// !!! 这儿之前 word 不能遗漏
			if (!word.empty())
			{// 将前一个单词的查询结果 wordquery 入 querystack
				queryStack.push(Query(word));
				word = "";			// 重置 word
			}
			oper = operatorStack.top();
			operatorStack.pop();
			runQuery(oper);
		}
	}

	if (!word.empty())
		queryStack.push(Query(word));

	while (!operatorStack.empty()) {
		oper = operatorStack.top();
		operatorStack.pop();
		runQuery(oper);
	}

	return queryStack.top();
}

void runQuery(char oper)
{
	if (oper == '(')		// '(' 直接略去
		return;
	else if (oper == '~')
		notQuery();
	else if (oper == '|')
		orQuery();
	else if (oper == '&')
		andQuery();
}
void notQuery()
{// 从 queryStack 弹出 query ，进行 ~ 运算后，结果入栈
	Query q = queryStack.top();
	queryStack.pop();
	queryStack.push(~q);
}
void andQuery()
{// 从 queryStack 弹出 query ，进行 & 运算后，结果入栈
	Query b = queryStack.top();
	queryStack.pop();
	Query a = queryStack.top();
	queryStack.pop();

	queryStack.push(a & b);
}
void orQuery()
{// 从 queryStack 弹出 query ，进行 | 运算后，结果入栈
	Query b = queryStack.top();
	queryStack.pop();
	Query a = queryStack.top();
	queryStack.pop();

	queryStack.push(a | b);
}