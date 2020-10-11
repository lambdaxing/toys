#include "Query.h"
#include <stack>
#include <cctype>

// global variables
std::stack<Query> queryStack;			// 保存运算结果的 stack
std::stack<std::string> operatorStack;	// 保存运算符号的 stack
std::vector<std::string> opers;			// 保存查询语句

// 三个运算
void runQuery(const std::string&);
void andQuery();
void notQuery();
void orQuery();

void runQueries(std::ifstream&);	// 处理与用户的交互
void input();						// 将输入查询语句分割，按顺序保存到 opers 中
Query run();						// 查询主程序

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "error: No file name arg.";
		return 0;
	}
	auto file = std::ifstream(argv[1]);
	if (file.fail())
	{
		std::cout << "error: Open file " << argv[1] << " failed.";
		return 0;
	}
	runQueries(file);
}

void runQueries(std::ifstream& infile)
{
	// infile 是一个 ifstream,指向我们要处理的文件
	TextQuery tq(infile);	// 保存文件并建立查询 map
	// 与用户交互：提示用户输入查询的语句，完成查询并打印结果
	while (true) {
		std::cout << "Please put brackets and executing Query for:";
		input();
		try {
			auto q = run();
			print(std::cout, q.eval(tq)) << std::endl;
		}
		catch (...)
		{
			std::cout << "statement error diagnosis!" << std::endl;
		}

		// 下一次输入前，清空查询语句
		opers.clear();
	}
}

void input()
{
	char c;									// 读入的单个字符
	std::string word;						// 由 c 合成的单词

	while (std::cin >> std::noskipws >> c) {
		if (c == '\n') break;
		if (isalpha(c)) word.push_back(c);	// 单个字母加入到 word 中，然后继续读取
		else if (c == '(' || c == ')' || c == '&' || c == '|' || c == '~') {
			if (!word.empty())
			{// 将前一个单词入 opers
				opers.push_back(word);
				word = "";			// 重置 word
			}
			opers.push_back(std::string(1, c));
		}
	}
	if (!word.empty())	// 最后一个单词存在
		opers.push_back(word);
}

Query run()
{
	std::string word;
	std::string oper;

	// 反向处理查询语句中的单词和运算符（从右往左）
	for (auto i = opers.crbegin(); i != opers.crend(); ++i)
	{
		if (*i == ")" || *i == "&" || *i == "|")	// 这些运算符统统入 operatorstack
			operatorStack.push(*i);
		else if (*i == "~")		//遇到 ~ 时，运算query已在queryStack中，可直接运算
			runQuery("~");
		else if (*i == "(") { // 开始运算
			if(operatorStack.empty())
				throw std::invalid_argument("statement error diagnosis.");
			oper = operatorStack.top();
			operatorStack.pop();
			runQuery(oper);
		}
		else
			queryStack.push(*i);
	}

	// 处理可能剩余的运算符
	while (!operatorStack.empty()) {
		oper = operatorStack.top();
		operatorStack.pop();
		runQuery(oper);
	}

	return queryStack.top();
}

void runQuery(const std::string& oper)
{
	if (oper == ")")		// ")" 直接略去
		return;
	else if (oper == "~")
		notQuery();
	else if (oper == "|")
		orQuery();
	else if (oper == "&")
		andQuery();
}
void notQuery()
{// 从 queryStack 弹出 query ，进行 ~ 运算后，结果入栈
	if (queryStack.empty())
		throw std::invalid_argument("statement error diagnosis.");

	Query q = queryStack.top();
	queryStack.pop();
	queryStack.push(~q);
}
void andQuery()
{// 从 queryStack 弹出 query ，进行 & 运算后，结果入栈
	if (queryStack.size() < 2)
		throw std::invalid_argument("statement error diagnosis.");

	Query a = queryStack.top();
	queryStack.pop();
	Query b = queryStack.top();
	queryStack.pop();

	queryStack.push(a & b);
}
void orQuery()
{// 从 queryStack 弹出 query ，进行 | 运算后，结果入栈
	if (queryStack.size() < 2)
		throw std::invalid_argument("statement error diagnosis.");

	Query a = queryStack.top();
	queryStack.pop();
	Query b = queryStack.top();
	queryStack.pop();

	queryStack.push(a | b);
}
