#include "TextQuery.h"

// ��ȡ�����ļ����������ʵ��кŵ�ӳ��
TextQuery::TextQuery(std::ifstream& is) :file(new std::vector<std::string>)
{
	std::string text;
	while (std::getline(is, text)) {		// ���ļ��е�ÿһ��
		file->push_back(text);			// ������е��ı�
		int n = file->size() - 1;		// ��ǰ�к�
		std::istringstream line(text);	// �����ı��ֽ�Ϊ����
		std::string word;
		while (line >> word) {			// �����е�ÿ������
			// ������ʲ��� wm �У���֮Ϊ�±��� wm �����һ��
			auto& lines = wm[word];		// lines ��һ�� shared_ptr
			if (!lines)	// �����ǵ�һ�������������ʱ����ָ��Ϊ��
				lines.reset(new std::set<line_no>);	// ����һ���µ� set
			lines->insert(n);		// �����кŲ��� set ��
		}
	}
}

QueryResult TextQuery::query(const std::string& sought) const
{
	// ���δ�ҵ� sought�����ǽ�����һ��ָ��� set ��ָ��
	static std::shared_ptr<std::set<line_no>> nodata(new std::set<line_no>);
	// ʹ�� find �������±�����������ҵ��ʣ����⽫������ӵ� wm �У�
	auto loc = wm.find(sought);
	if (loc == wm.end())
		return QueryResult(sought, nodata, file);		// δ�ҵ�
	else
		return QueryResult(sought, loc->second, file);
}

std::string make_plural(std::size_t ctr, const std::string& word, const std::string& ending)
{
	return (ctr > 1) ? word + ending : word;
}

std::ostream& print(std::ostream& os, const QueryResult& qr)
{
	// ����ҵ��˵��ʣ���ӡ���ֵĴ��������г��ֵ�λ��
	os << qr.sought << " occurs " << qr.lines->size() << " " << make_plural(qr.lines->size(), "time", "s") << std::endl;
	// ��ӡ���ʳ��ֵ�ÿһ��
	for (auto num : *qr.lines)	// �� set �е�ÿ������
		// �����кŴ�0��ʼ���û�����������
		os << "\t(line " << num + 1 << ") " << *(qr.file->begin() + num) << std::endl;
	return os;
}
