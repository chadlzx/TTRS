#include <iostream>
#include <cmath>
#include <cstring>
#include <cstdio>

using namespace std;

int h[1000005];
int value[1000005];
int c;
int sum[1000005];
int NumChild[1000005];
int tag[1000005];
int father[1000005];

struct edge {
    int to;
    int next;
}e[1000005 * 2];

void addedge(int from, int to) {
    c++;
    e[c] = (edge){to, h[from]};
    h[from] = c;
}

class Tree{
public:
    int N, M ,root;

    void Build() {
        c = -1;
        cin >> N >> M >> root;
        for (int i = 1; i <= N; i++) {
            cin >> value[i];
        }
        for (int i = 1; i <= N; i++) {
            tag[i] = 0;
            h[i] = -1;
        }
        for (int i = 1; i <= N-1; i++) {
            int from, to;
            cin >> from >> to;
            addedge(from, to);
            addedge(to, from);
        }

        dfs(root, 0);
    }

    int dfs(int p, int fa) {
        father[p] = fa;
        sum[p] = value[p];
        NumChild[p] = 0;
        for (int i = h[p]; i != -1; i = e[i].next) {
            if (e[i].to != fa) {
                NumChild[p]++;
                sum[p] += dfs(e[i].to, p);
            }
        }
        return sum[p];
    }

    int Query(int p) {
        int Sum = (NumChild[p]+1) * tag[p];
        for (int i = h[p]; i != -1; i = e[i].next) {
            if (e[i].to != father[p]) {
                Sum += Query(e[i].to);
            }
        }
        return Sum;
    }

    void Solve() {
        int op1, op2, op3;
        for (int i = 1; i <= M; i++) {
            cin >> op1;
            if (op1 == 1) {
                cin >> op2 >> op3;
                tag[op2] += op3;
            }
            else if(op1 == 2) {
                cin >> op2;
                int SumTag = 0;
                int p = op2;
                while (p != root) {
                    SumTag += tag[p];
                    p = father[p];
                }
                cout << SumTag * (NumChild[op2] + 1) + sum[op2] + Query(op2) - (NumChild[op2]+1) * tag[op2]<< "\n";
            }
        }
    }

}aa;

/*
 *
10 14 9
12 -6 -4 -3 12 8 9 6 6 2
8 2
2 10
8 6
2 7
7 1
6 3
10 9
2 4
10 5
1 4 -1
2 2
1 7 -1
2 10
1 10 5
2 1
1 7 -5
2 5
1 1 8
2 7
1 8 8
2 2
1 5 5
2 6
 */

int main() {
    aa.Build();
    aa.Solve();
    return 0;
}


/*//
// Created by yujie6 on 19-5-8.
//
#include <iostream>
#include <cmath>
#include <cstring>

using namespace std;

int s[100005];
int a[100005];
int fa[100005];
int n;

inline int max(int r, int s) { return (r > s) ? r : s; }

void Build() {
    cin >> n;
    for (int i = 2; i <= n; i++) {
        cin >> fa[i];
    }
    for (int i = 1; i <= n; i++) {
        cin >> s[i];
    }
}

void Solve() {

}

int main() {
    ios::sync_with_stdio(false);
    Build();
    Solve();
    return 0;
}*/
