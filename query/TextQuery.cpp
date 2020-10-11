#include "TextQuery.h"

// 读取输入文件并建立单词到行号的映射
TextQuery::TextQuery(std::ifstream& is) :file(new std::vector<std::string>)
{
	std::string text;
	while (std::getline(is, text)) {		// 对文件中的每一行
		file->push_back(text);			// 保存此行的文本
		int n = file->size() - 1;		// 当前行号
		std::istringstream line(text);	// 将行文本分解为单词
		std::string word;
		while (line >> word) {			// 对行中的每个单词
			// 如果单词不在 wm 中，以之为下标在 wm 中添加一项
			auto& lines = wm[word];		// lines 是一个 shared_ptr
			if (!lines)	// 在我们第一次遇到这个单词时，此指针为空
				lines.reset(new std::set<line_no>);	// 分配一个新的 set
			lines->insert(n);		// 将此行号插入 set 中
		}
	}
}

QueryResult TextQuery::query(const std::string& sought) const
{
	// 如果未找到 sought，我们将返回一个指向此 set 的指针
	static std::shared_ptr<std::set<line_no>> nodata(new std::set<line_no>);
	// 使用 find 而不是下标运算符来查找单词，避免将单词添加到 wm 中！
	auto loc = wm.find(sought);
	if (loc == wm.end())
		return QueryResult(sought, nodata, file);		// 未找到
	else
		return QueryResult(sought, loc->second, file);
}

std::string make_plural(std::size_t ctr, const std::string& word, const std::string& ending)
{
	return (ctr > 1) ? word + ending : word;
}

std::ostream& print(std::ostream& os, const QueryResult& qr)
{
	// 如果找到了单词，打印出现的次数和所有出现的位置
	os << qr.sought << " occurs " << qr.lines->size() << " " << make_plural(qr.lines->size(), "time", "s") << std::endl;
	// 打印单词出现的每一行
	for (auto num : *qr.lines)	// 对 set 中的每个单词
		// 避免行号从0开始给用户带来的困惑
		os << "\t(line " << num + 1 << ") " << *(qr.file->begin() + num) << std::endl;
	return os;
}
