#include<iostream>
#include<unordered_map>
#include<set>
#include<utility>
#include<array>
#include<queue>

struct teaminf
{
    std::string name;
    int passed;
    std::multiset<int, std::greater<int>>pass_time;//让通过时间降序排序，注意可能存在重复元素，因此要用multiset
    int time_punished;

    bool operator<(const teaminf& other) const
    {
        if (passed != other.passed) { return passed > other.passed; }
        else if (time_punished != other.time_punished) { return time_punished < other.time_punished; }
        else if (pass_time != other.pass_time)
        {
            auto it1 = pass_time.begin();
            auto it2 = other.pass_time.begin();
            while (it1 != pass_time.end() && it2 != other.pass_time.end())
            {
                if (*it1 != *it2)
                {
                    return *it1 < *it2;
                }
                ++it1;
                ++it2;
            }
        }// 挨个比较set中的元素
        else { return name < other.name; }
        return false;
    }
} teamlist[10005];

struct teamsub//记录最后一次提交
{
    int prosub1[26][5];//每道题每种状态的最后一次提交,1-4表示四种状态，0表示所有状态
    std::string prosub2[26];//每道题最后一次提交状态
    char totalsub1[5];//所有题目最后一次提交的题号
    int totalsub2[5];//所有题目最后一次提交的时间
    std::string totalsub3;//所有题目最后一次提交的状态
};

struct problem
{
    int submit_times[26];
    bool is_right[26];//是否通过
    int freeze_sub[26];//封榜后提交次数
    int have_freeze;//是否有冻结题目
    std::array<std::queue<std::pair<int, int>>, 26> freezepro;//记录封榜之后的提交
};


std::string word;//用于读入无用字符串
bool isStart = false;//用于判断比赛是否开始
bool isFreeze = false;//用于判断是否处于封榜状态
int duration_time, problem_count;//比赛时间范围和题号范围
int team_num = 0;//每支队伍的编号

std::unordered_map<std::string, int>teamid;//每支队伍到数组中下标编号的映射
std::set<teaminf>newteamlist;//即时更新排名
std::unordered_map<std::string, int>ranking;//记录每只队伍排名
std::unordered_map<int, std::string>inverse;
std::unordered_map < std::string, std::set<std::pair<int, int>>>freezesub;//储存封榜后的提交，其中pair中第一个int是题号，第二个int是评测状态
std::unordered_map<std::string, teamsub>lastsub;//记录最后一次提交
std::unordered_map<std::string, problem>print;

void Addteam()
{
    std::string team_name;
    std::cin >> team_name;
    if (!isStart)
    {
        if (teamid.count(team_name) == 0)
        {
            teaminf inf = { team_name,0,{},0 };
            teamlist[team_num] = inf;
            newteamlist.insert(inf);
            teamid[team_name] = team_num;
            lastsub[team_name] = {};
            print[team_name] = {};
            freezesub[team_name] = {};
            std::cout << "[Info]Add successfully.\n";
            team_num++;
        }
        else
        {
            std::cout << "[Error]Add failed: duplicated team name.\n";
        }
    }
    else
    {
        std::cout << "[Error]Add failed: competition has started.\n";
    }
}

void Startgame()
{
    if (isStart)
    {
        std::cin >> word >> word >> word >> word;
        std::cout << "[Error]Start failed: competition has started.\n";
    }
    else
    {
        std::cin >> word >> duration_time >> word >> problem_count;
        isStart = true;
        std::cout << "[Info]Competition starts.\n";
        int rank = 1;
        for (auto it = newteamlist.begin(); it != newteamlist.end(); it++)
        {
            std::string name = (*it).name;
            ranking[name] = rank;
            rank++;
        }
    }
}

void Submit()
{
    std::string team_name, submit_status;
    char problem_name;
    int time, status;
    std::cin >> problem_name >> word >> team_name >> word >> submit_status >> word >> time;
    if (time >= 1 && time <= duration_time)
    {
        switch (submit_status[0])
        {
            case 'A':status = 1; break;
            case 'W':status = 2; break;
            case 'R':status = 3; break;
            case 'T':status = 4; break;
        }
        auto &a= lastsub[team_name];
        lastsub[team_name].prosub1[problem_name - 'A'][status] = time;//0-based 题号从0开始
        lastsub[team_name].prosub1[problem_name - 'A'][0] = time;
        lastsub[team_name].prosub2[problem_name - 'A'] = submit_status;
        lastsub[team_name].totalsub1[status] = lastsub[team_name].totalsub1[0] = problem_name;
        lastsub[team_name].totalsub2[status] = lastsub[team_name].totalsub2[0] = time;
        lastsub[team_name].totalsub3 = submit_status;
        if (print[team_name].is_right[problem_name - 'A']) {}
        else
        {
            if (!isFreeze)//封榜前提交
            {
                if (status == 1)
                {
                    print[team_name].is_right[problem_name - 'A'] = true;
                    teaminf tem = teamlist[teamid[team_name]];
                    newteamlist.erase(tem);
                    tem.passed++;
                    tem.time_punished += print[team_name].submit_times[problem_name - 'A'] * 20 + time;
                    tem.pass_time.insert(time);
                    newteamlist.insert(tem);
                    teamlist[teamid[team_name]] = tem;
                }
                else
                {
                    print[team_name].submit_times[problem_name - 'A']++;
                }
            }
            else//封榜后提交
            {
                print[team_name].freeze_sub[problem_name - 'A']++;
                print[team_name].have_freeze++;
                //将封榜后提交另外储存起来
                print[team_name].freezepro[problem_name - 'A'].push(std::make_pair(status, time));
            }
        }
    }
}

void Flush()
{
    int rank = 1;
    for (const auto& it : newteamlist)
    {
        ranking[it.name] = rank;
        inverse[rank] = it.name;
        rank++;
    }
    std::cout << "[Info]Flush scoreboard.\n";
}

void Freeze()
{
    if (isFreeze)
    {
        std::cout << "[Error]Freeze failed: scoreboard has been frozen.\n";
    }
    else
    {
        isFreeze = true;
        std::cout << "[Info]Freeze scoreboard.\n";
    }
}

void Printlist()
{
    for (int rank = 1; rank <= team_num; rank++)
    {
        std::string name = inverse[rank];
        teaminf problem = teamlist[teamid[name]];
        std::cout << inverse[rank] << " " << rank << " " << problem.passed << " " << problem.time_punished;
        for (int i = 0; i < problem_count; i++)
        {
            if (print[name].is_right[i])
            {
                if (print[name].submit_times[i] == 0)
                {
                    std::cout << " +";
                }
                else
                {
                    std::cout << " +" << print[name].submit_times[i];
                }
            }
            else if (print[name].freeze_sub[i] == 0)
            {
                if (print[name].submit_times[i] == 0)
                {
                    std::cout << " .";
                }
                else
                {
                    std::cout << " -" << print[name].submit_times[i];
                }
            }
            else
            {
                if (print[name].submit_times[i] == 0)
                {
                    std::cout << " 0/" << print[name].freeze_sub[i];
                }
                else
                {
                    std::cout << " -" << print[name].submit_times[i] << "/" << print[name].freeze_sub[i];
                }
            }
        }
        std::cout << "\n";
    }
}

void Scroll()
{
    if (!isFreeze)
    {
        std::cout << "[Error]Scroll failed: scoreboard has not been frozen.\n";
    }
    else
    {
        std::cout << "[Info]Scroll scoreboard.\n";
        int rank1 = 1;
        for (const auto& s : newteamlist)
        {
            ranking[s.name] = rank1;
            inverse[rank1] = s.name;
            rank1++;
        }//先刷新一下榜单
        Printlist();
        auto it = newteamlist.end();
        it--;//最后一名的迭代器
        while (true)//直到遍历到第一名并且第一名不存在冻结题目
        {
            teaminf now = *it;
            std::string id1 = now.name;
            if (print[id1].have_freeze > 0)//如果有冻结的题目，每次只解冻一道
            {
                for (int i = 0; i < problem_count; i++)
                {
                    if (print[id1].freeze_sub[i] > 0)
                    {
                        while (!print[id1].freezepro[i].empty())
                        {
                            int prostatus = print[id1].freezepro[i].front().first;
                            int protime = print[id1].freezepro[i].front().second;
                            print[id1].freezepro[i].pop();
                            print[id1].freeze_sub[i]--;
                            print[id1].have_freeze--;
                            if(print[id1].is_right[i]){}
                            else
                            {
                                if (prostatus == 1)
                                {
                                    print[id1].is_right[i] = true;
                                    now.passed++;
                                    now.pass_time.insert(protime);
                                    now.time_punished += 20 * print[id1].submit_times[i] + protime;
                                }
                                else
                                {
                                    print[id1].submit_times[i]++;
                                }
                            }
                        }
                        break;
                    }
                }
                teamlist[teamid[id1]] = now;
                if (it == newteamlist.begin())//如果已经是第一名就不用和其他队伍比较
                {
                    newteamlist.erase(it);
                    newteamlist.insert(now);
                }
                else
                {
                    newteamlist.erase(it++);
                    it--;
                    teaminf up = *it;//原榜单上上一支队伍
                    if (now < up)//与排行榜上上一个队伍比较
                    {
                        auto iter = newteamlist.lower_bound(now);
                        std::string id2 = (*iter).name;
                        std::cout << id1 << " " << id2 << " " << now.passed << " " << now.time_punished << "\n";
                        newteamlist.insert(now);
                    }
                    else
                    {
                        newteamlist.insert(now);
                        it++;
                    }
                }
            }
            else if (it != newteamlist.begin())
            {
                it--;
            }
            else
            {
                break;
            }
        }
        int rank2 = 1;
        for (const auto& s : newteamlist)
        {
            ranking[s.name] = rank2;
            inverse[rank2] = s.name;
            rank2++;
        }//再刷新一下榜单
        Printlist();
        isFreeze = false;
    }
}

void Query_rank()
{
    std::string team_name;
    std::cin >> team_name;
    if (ranking.find(team_name) != ranking.end())
    {
        std::cout << "[Info]Complete query ranking.\n";
        if (isFreeze) { std::cout << "[Warning]Scoreboard is frozen. The ranking may be inaccurate until it were scrolled.\n"; }
        std::cout << team_name << " NOW AT RANKING " << ranking[team_name] << "\n";
    }
    else
    {
        std::cout << "[Error]Query ranking failed: cannot find the team.\n";
    }
}

void Query_submission()
{
    std::string team_name, Problem_name, Submit_status;
    int status;
    std::cin >> team_name >> word >> Problem_name >> word >> Submit_status;
    switch (Submit_status[8])
    {
        case 'c':status = 1; break;//AC
        case 'r':status = 2; break;//WA
        case 'u':status = 3; break;//RE
        case 'i':status = 4; break;//TLE
        default:status = 0;
    }
    if (ranking.count(team_name) > 0)
    {
        std::cout << "[Info]Complete query submission.\n";
        if (Problem_name == "PROBLEM=ALL")
        {
            if (lastsub[team_name].totalsub1[status] > 0)
            {
                if (Submit_status == "STATUS=ALL")
                {
                    std::cout << team_name << " " << lastsub[team_name].totalsub1[status] << " " << lastsub[team_name].totalsub3 << " " << lastsub[team_name].totalsub2[0] << "\n";
                }
                else
                {
                    std::cout << team_name << " " << lastsub[team_name].totalsub1[status] << " " << Submit_status.substr(7) << " " << lastsub[team_name].totalsub2[status] << "\n";
                }
            }
            else
            {
                std::cout << "Cannot find any submission.\n";
            }
        }
        else
        {
            if (lastsub[team_name].prosub1[Problem_name[8] - 'A'][status] > 0)
            {
                if (Submit_status == "STATUS=ALL")
                {
                    std::cout << team_name << " " << Problem_name.substr(8) << " " << lastsub[team_name].prosub2[Problem_name[8] - 'A'] << " " << lastsub[team_name].prosub1[Problem_name[8] - 'A'][status] << "\n";
                }
                else
                {
                    std::cout << team_name << " " << Problem_name.substr(8) << " " << Submit_status.substr(7) << " " << lastsub[team_name].prosub1[Problem_name[8] - 'A'][status] << "\n";
                }
            }
            else
            {
                std::cout << "Cannot find any submission.\n";
            }
        }
    }
    else
    {
        std::cout << "[Error]Query submission failed: cannot find the team.\n";
    }
}

int main()
{
    //freopen("1.in", "r", stdin);
    //freopen("1.txt", "w", stdout);
    std::string mode;
    while (std::cin >> mode)
    {
        if (mode == "END")
        {
            std::cout << "[Info]Competition ends.\n";
            break;
        }
        else if (mode == "ADDTEAM")
        {
            Addteam();
        }
        else if (mode == "START")
        {
            Startgame();
        }
        else if (mode == "SUBMIT")
        {
            Submit();
        }
        else if (mode == "FLUSH")
        {
            Flush();
        }
        else if (mode == "FREEZE")
        {
            Freeze();
        }
        else if (mode == "SCROLL")
        {
            Scroll();
        }
        else if (mode == "QUERY_RANKING")
        {
            Query_rank();
        }
        else if (mode == "QUERY_SUBMISSION")
        {
            Query_submission();
        }
    }
}
