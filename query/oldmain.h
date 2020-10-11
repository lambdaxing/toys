#include "Query.h"
#include <stack>
#include <cctype>

// global variables
std::stack<Query> queryStack;			// ������������ stack
std::stack<char> operatorStack;			// ����������ŵ� stack

// ��������
void runQuery(char);
void andQuery();
void notQuery();
void orQuery();

Query inputAndRun();					// �����ѯ��䣬���ز�ѯ���
void runQueries(std::ifstream&);		// �������û��Ľ���

int main(int argc, char* argv[])
{
	// �����ļ��������������
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

	// ���в�ѯ����
	runQueries(file);
}

void runQueries(std::ifstream& infile)
{
	// infile ��һ�� ifstream,ָ������Ҫ������ļ�
	TextQuery tq(infile);	// �����ļ���������ѯ map
	// ���û���������ʾ�û������ѯ����䣬��ɲ�ѯ����ӡ���
	while (true) {
		std::cout << "Please put brackets and executing Query for:";
		Query q = inputAndRun();
		print(std::cout, q.eval(tq)) << std::endl;
	}
}

Query inputAndRun()
{
	char c;									// ����ĵ����ַ�
	std::string word;						// �� c �ϳɵĵ���
	char oper;								// �����

	while (std::cin >> std::noskipws >> c) {
		if (c == '\n') break;
		if (isalpha(c))
			word.push_back(c);	// ������ĸ���뵽 word �У�Ȼ�������ȡ
		else if (c == '(' || c == '&' || c == '|' || c == '~') {	// ��Щ�����ͳͳ�� operatorstack
			if (!word.empty())
			{// ��ǰһ�����ʵĲ�ѯ��� wordquery �� querystack
				queryStack.push(Query(word));
				word = "";			// ���� word
			}
			operatorStack.push(c);	// �� ����� �� operatorstack
		}
		else if (c == ')') { // ��ʼ����
			// !!! ���֮ǰ word ������©
			if (!word.empty())
			{// ��ǰһ�����ʵĲ�ѯ��� wordquery �� querystack
				queryStack.push(Query(word));
				word = "";			// ���� word
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
	if (oper == '(')		// '(' ֱ����ȥ
		return;
	else if (oper == '~')
		notQuery();
	else if (oper == '|')
		orQuery();
	else if (oper == '&')
		andQuery();
}
void notQuery()
{// �� queryStack ���� query ������ ~ ����󣬽����ջ
	Query q = queryStack.top();
	queryStack.pop();
	queryStack.push(~q);
}
void andQuery()
{// �� queryStack ���� query ������ & ����󣬽����ջ
	Query b = queryStack.top();
	queryStack.pop();
	Query a = queryStack.top();
	queryStack.pop();

	queryStack.push(a & b);
}
void orQuery()
{// �� queryStack ���� query ������ | ����󣬽����ջ
	Query b = queryStack.top();
	queryStack.pop();
	Query a = queryStack.top();
	queryStack.pop();

	queryStack.push(a | b);
}