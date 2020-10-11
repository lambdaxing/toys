#ifndef TEXT_QUERY_H
#define TEXT_QUERY_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <set>
#include <iterator>
#include <algorithm>

class QueryResult;

class TextQuery {
public:
	using line_no = std::vector<std::string>::size_type;
	TextQuery(std::ifstream&);
	QueryResult query(const std::string&) const;
private:
	std::shared_ptr<std::vector<std::string>> file;		// �����ļ�
	// ÿ�����ʵ������ڵ��кŵļ��ϵ�ӳ��
	std::map<std::string, std::shared_ptr<std::set<line_no>>> wm;
};

class QueryResult {
	using ResultIter = std::set<TextQuery::line_no>::iterator;
	friend std::ostream& print(std::ostream&, const QueryResult&);
public:
	QueryResult(std::string s, std::shared_ptr<std::set<TextQuery::line_no>> p, std::shared_ptr<std::vector<std::string>> f) :sought(s), lines(p), file(f) {}

	ResultIter begin() const { return lines->begin(); }
	ResultIter end() const { return lines->end(); }
	std::shared_ptr<std::vector<std::string>> get_file() const { return  file;	/*std::make_shared<std::vector<std::string>>(file);*/ }
private:
	std::string sought;		// ��ѯ����
	std::shared_ptr<std::set<TextQuery::line_no>> lines;		// ���ֵ��к�
	std::shared_ptr<std::vector<std::string>> file;				// �����ļ� 
};

std::string make_plural(std::size_t ctr, const std::string& word, const std::string& ending);
std::ostream& print(std::ostream&, const QueryResult&);

#endif // !TEXT_QUERY_H