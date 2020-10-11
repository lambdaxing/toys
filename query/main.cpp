#include "Query.h"
#include <stack>
#include <cctype>

// global variables
std::stack<Query> queryStack;			// ������������ stack
std::stack<std::string> operatorStack;	// ����������ŵ� stack
std::vector<std::string> opers;			// �����ѯ���

// ��������
void runQuery(const std::string&);
void andQuery();
void notQuery();
void orQuery();

void runQueries(std::ifstream&);	// �������û��Ľ���
void input();						// �������ѯ���ָ��˳�򱣴浽 opers ��
Query run();						// ��ѯ������

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
	// infile ��һ�� ifstream,ָ������Ҫ������ļ�
	TextQuery tq(infile);	// �����ļ���������ѯ map
	// ���û���������ʾ�û������ѯ����䣬��ɲ�ѯ����ӡ���
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

		// ��һ������ǰ����ղ�ѯ���
		opers.clear();
	}
}

void input()
{
	char c;									// ����ĵ����ַ�
	std::string word;						// �� c �ϳɵĵ���

	while (std::cin >> std::noskipws >> c) {
		if (c == '\n') break;
		if (isalpha(c)) word.push_back(c);	// ������ĸ���뵽 word �У�Ȼ�������ȡ
		else if (c == '(' || c == ')' || c == '&' || c == '|' || c == '~') {
			if (!word.empty())
			{// ��ǰһ�������� opers
				opers.push_back(word);
				word = "";			// ���� word
			}
			opers.push_back(std::string(1, c));
		}
	}
	if (!word.empty())	// ���һ�����ʴ���
		opers.push_back(word);
}

Query run()
{
	std::string word;
	std::string oper;

	// �������ѯ����еĵ��ʺ����������������
	for (auto i = opers.crbegin(); i != opers.crend(); ++i)
	{
		if (*i == ")" || *i == "&" || *i == "|")	// ��Щ�����ͳͳ�� operatorstack
			operatorStack.push(*i);
		else if (*i == "~")		//���� ~ ʱ������query����queryStack�У���ֱ������
			runQuery("~");
		else if (*i == "(") { // ��ʼ����
			if(operatorStack.empty())
				throw std::invalid_argument("statement error diagnosis.");
			oper = operatorStack.top();
			operatorStack.pop();
			runQuery(oper);
		}
		else
			queryStack.push(*i);
	}

	// �������ʣ��������
	while (!operatorStack.empty()) {
		oper = operatorStack.top();
		operatorStack.pop();
		runQuery(oper);
	}

	return queryStack.top();
}

void runQuery(const std::string& oper)
{
	if (oper == ")")		// ")" ֱ����ȥ
		return;
	else if (oper == "~")
		notQuery();
	else if (oper == "|")
		orQuery();
	else if (oper == "&")
		andQuery();
}
void notQuery()
{// �� queryStack ���� query ������ ~ ����󣬽����ջ
	if (queryStack.empty())
		throw std::invalid_argument("statement error diagnosis.");

	Query q = queryStack.top();
	queryStack.pop();
	queryStack.push(~q);
}
void andQuery()
{// �� queryStack ���� query ������ & ����󣬽����ջ
	if (queryStack.size() < 2)
		throw std::invalid_argument("statement error diagnosis.");

	Query a = queryStack.top();
	queryStack.pop();
	Query b = queryStack.top();
	queryStack.pop();

	queryStack.push(a & b);
}
void orQuery()
{// �� queryStack ���� query ������ | ����󣬽����ջ
	if (queryStack.size() < 2)
		throw std::invalid_argument("statement error diagnosis.");

	Query a = queryStack.top();
	queryStack.pop();
	Query b = queryStack.top();
	queryStack.pop();

	queryStack.push(a | b);
}
