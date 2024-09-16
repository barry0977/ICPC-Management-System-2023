#include <iostream>
#include <string>
#include <unordered_map>
#include <set>
#include <algorithm>
using namespace std;

struct student
{
    string name;
    char gender;
    int Class;
    int sc[9];
    int ave;

    bool operator<(const student& other) const
    {
        if (ave != other.ave) { return ave > other.ave; }
        else if (sc[0] != other.sc[0]) { return sc[0] > other.sc[0]; }
        else if (sc[1] != other.sc[1]) { return sc[1] > other.sc[1]; }
        else if (sc[2] != other.sc[2]) { return sc[2] > other.sc[2]; }
        else if (sc[3] != other.sc[3]) { return sc[3] > other.sc[3]; }
        else if (sc[4] != other.sc[4]) { return sc[4] > other.sc[4]; }
        else if (sc[5] != other.sc[5]) { return sc[5] > other.sc[5]; }
        else if (sc[6] != other.sc[6]) { return sc[6] > other.sc[6]; }
        else if (sc[7] != other.sc[7]) { return sc[7] > other.sc[7]; }
        else if (sc[8] != other.sc[8]) { return sc[8] > other.sc[8]; }
        else { return this->name < other.name; }
    }
};

unordered_map<string, student> lis;
unordered_map<string, int> ranking;  // 记录名字到排名
unordered_map<int, string> inverse;  // 记录排名到名字
set<student> s;  // 对学生进行即时排序
bool isStart = false;

int main()
{
    while (true)
    {
        string mode;
        cin >> mode;
        if (mode == "END") break;

        if (mode == "ADD")
        {
            string name;
            char g;
            int c, ss[10], sum = 0;
            cin >> name >> g >> c;
            for (int i = 0; i < 9; i++)
            {
                cin >> ss[i];
                sum += ss[i];
            }
            student inf = { name, g, c, ss[0], ss[1], ss[2], ss[3], ss[4], ss[5], ss[6], ss[7], ss[8], sum / 9 };
            if (isStart) { cout << "[Error]Cannot add student now.\n"; }
            else {
                if (lis.count(name) == 0) { lis.insert({ name,inf }); }
                else { cout << "[Error]Add failed.\n"; }
            }
        }

        if (mode == "START")
        {
            isStart = true;
            for (const auto& iter : lis)
            {
                const student& a = iter.second;
                s.insert(a);
                int rank = 1;
                for (const auto& it : s)
                {
                    ranking[it.name] = rank;
                    inverse[rank] = it.name;
                    rank++;
                }
            }
        }

        if (mode == "UPDATE")
        {
            string name;
            int code, score, sum = 0;
            cin >> name >> code >> score;
            if (lis.count(name) > 0)
            {
                s.erase(lis[name]);
                lis[name].sc[code] = score;
                for (int i = 0; i < 9; i++)
                {
                    sum += lis[name].sc[i];
                }
                lis[name].ave = sum / 9;
                s.insert(lis[name]);
            }
            else { cout << "[Error]Update failed.\n"; }
        }

        if (mode == "FLUSH")
        {
            int rank = 1;
            for (const auto& it : s)
            {
                ranking[it.name] = rank;
                inverse[rank] = it.name;
                rank++;
            }
        }

        if (mode == "PRINTLIST")
        {
            int rank = 1;
            while (inverse.count(rank) != 0)
            {
                const string& na = inverse[rank];
                const student& st = lis[na];
                cout << rank << " " << st.name << " " << ((st.gender == 'M') ? "male" : "female") << " " << st.Class << " " << st.ave << "\n";
                rank++;
            }
        }

        if (mode == "QUERY")
        {
            string name;
            cin >> name;
            if (lis.count(name) > 0)
            {
                cout << "STUDENT " << name << " NOW AT RANKING " << ranking[name] << "\n";
            }
            else
            {
                cout << "[Error]Query failed." << "\n";
            }
        }
    }
    return 0;
}
/*
当使用范围-based for 循环遍历一个容器时，可以使用 const auto& 来声明迭代器变量。目的是为了避免不必要的复制。每个部分的含义：
const：表示迭代器引用的内容是只读的，不能通过迭代器修改容器中的元素。
auto：表示编译器会根据初始化表达式自动推断出迭代器的类型。
&：表示引用，使得我们可以通过迭代器直接访问容器中的元素，而不需要进行复制。
在这种情况下，iter 是一个指向容器中键值对的迭代器，而 const auto& 的类型推断会使其成为一个 const 引用，从而避免了对容器中元素的不必要复制

在 "START" 模式下，使用 const auto & 来遍历 lis 容器，并使用 const student & 引用来访问学生对象。
在 "FLUSH" 模式下，使用 const auto & 引用遍历 s 容器，并直接更新 ranking 和 inverse 容器。
在 "PRINTLIST" 模式下，使用 const string & 引用来访问学生的姓名，并使用 const student & 引用来访问学生对象。
在 "QUERY" 模式下，使用 const string & 引用来访问学生的姓名*/