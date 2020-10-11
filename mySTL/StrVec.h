#ifndef STRVEC_H
#define STRVEC_H

#include <memory>
#include <string>

class StrVec
{
public:
    StrVec():       // allocator 成员默认初始化
        elements(nullptr),first_free(nullptr),cap(nullptr){}
    StrVec(const StrVec&);
    StrVec(StrVec&&);
    StrVec& operator=(const StrVec&);
    StrVec& operator=(StrVec&&);
    ~StrVec();

    void push_back(const std::string&);
    void push_back(std::string&&);
    size_t size() const { return first_free - elements; }
    size_t capacity() const { return cap - elements; }
    std::string* begin() const { return elements; }
    std::string* end() const { return first_free; }

private:
    static std::allocator<std::string> alloc;   // 分配元素。一个 allocator 对象可多次分配，可声明为 static 。
    void chk_n_alloc()
    {
        if (size() == capacity()) reallocate();
    }
    // 工具函数
    std::pair<std::string*, std::string*> alloc_n_copy(const std::string*, const std::string*);
    void free();                    // 销毁元素并释放内存
    void reallocate();              // 获得更多内存并拷贝已有元素
    std::string* elements;          // 指向数组首元素的指针
    std::string* first_free;        // 指向数组第一个空闲元素的指针
    std::string* cap;               // 指向数组尾后位置的指针
};

void StrVec::push_back(const std::string& s)
{
    chk_n_alloc();      // 确保有空间容纳新元素
    // 在 first_free 指向的元素中构造 s 的副本
    alloc.construct(first_free++, s);
}

void StrVec::push_back(std::string&& s)
{
    chk_n_alloc();
    alloc.construct(first_free++, std::move(s));
}

std::pair<std::string*, std::string*> StrVec::alloc_n_copy(const std::string* b, const std::string* e)
{
    // 分配空间保存给定范围中的元素
    auto data = alloc.allocate(e - b);
    // 初始化并返回一个 pair，该 pair 由 data 和 uninitialized_copy 的返回值构成
    return { data, std::uninitialized_copy(b, e, data) };
}

void StrVec::free()
{
    // 不能传递给 deallocate 一个空指针，如果 elements 为 0, 函数什么也不做
    if (elements)
    {
        // 逆序销毁元素
        for (auto p = first_free; p != elements;/* 空 */)
            alloc.destroy(--p);
        alloc.deallocate(elements, cap - elements);
    }
}

StrVec::StrVec(const StrVec& s)
{
    auto newdata = alloc_n_copy(s.begin(), s.end());
    elements = newdata.first;
    first_free = cap = newdata.second;
}

StrVec::StrVec(StrVec&& s) noexcept     // 移动操作不应抛出任何异常
    : elements(s.elements), first_free(s.first_free), cap(s.cap)
{
    // 令 s 进入这样的状态——对其运行析构函数是安全的
    s.elements = s.first_free = s.cap = nullptr;
}

StrVec::~StrVec() { free(); }

StrVec& StrVec::operator=(const StrVec& rhs)
{
    // 调用 alloc_n_copy 分配内存，大小与 rhs 中元素占用空间一样多
    auto data = alloc_n_copy(rhs.begin(), rhs.end());
    free();
    elements = data.first;
    first_free = cap = data.second;
    return *this;
}

StrVec& StrVec::operator=(StrVec&& rhs) noexcept
{
    // 直接检测自赋值
    if (this != &rhs)
    {
        free();                     // 释放已有元素
        elements = rhs.elements;    // 从 rhs 接管资源
        first_free = rhs.first_free;
        cap = rhs.cap;
        rhs.elements = rhs.first_free = rhs.cap = nullptr;
    }
    return *this;
}

void StrVec::reallocate()
{
    // 我们将分配当前大小两倍的内存空间
    auto newcapacity = size() ? 2 * size() : 1;
    // 分配新内存
    auto first = alloc.allocate(newcapacity);
    
    // 移动元素
    auto last = std::uninitialized_copy(std::make_move_iterator(begin()), std::make_move_iterator(end()), first);
    
    // 释放旧空间
    free();
    elements = first;
    first_free = last;
    cap = elements + newcapacity;
}


#endif