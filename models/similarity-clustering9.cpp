#include <bits/stdc++.h>

using namespace std;

const int maxn = 100000;
const int maxs = 400;
const int LIMIT_W = 2000;

/* hyper-parameters */
int K;

/* input files */
char *graphFile;
char *weightFile;

vector<int> e[maxn]; // 正向边，从父亲指向儿子 
vector<int> re[maxn]; // 反向边 
vector<int> neighbor[maxn]; // 距离不超过K的邻居 
vector<int> Package[maxn];
bitset<maxn> successor[maxn];

bool vzt[maxn];
int m, n, weight[maxn], pa[maxn], maxCounter;
long long W;

void buildGraph(){
	FILE* f1=fopen(graphFile,"r");
	int u,v;
	while(~fscanf(f1, "%d%d", &u, &v) && u && v)
	{
		m++;
		assert(u < v);
		e[u].push_back(v);
		re[v].push_back(u);
	}
	fclose(f1);
	
	f1=fopen(weightFile,"r");
	while(~fscanf(f1, "%d%d", &u, &v) && u && v)
	{
		weight[u] = v;
		W += v;
		n = max(n, u);
		assert(n < maxn);
	}
	fclose(f1);
}

void findSuccessor(int u, int verbose = 0){
	if(vzt[u]) return;
	vzt[u] = 1;
	for(int i=0; i<e[u].size(); i++)
	{
		int v = e[u][i];
		successor[u][v] = 1;
		findSuccessor(v);
		successor[u] |= successor[v];
	}
	
	/* debug */
	if(verbose)
		printf("Node %d has %d Successors\n", u, successor[u].count());
}

void findNeighbor(int u, int k, int verbose = 0){
	static int q[maxn], h, t;
	static int dist[maxn];
	static int lastVis[maxn];
	
	q[h = t = 1] = u;
	lastVis[u] = u; dist[u]=1;
	
	while(h <= t)
	{
		int x = q[h++];
		if(x != u) neighbor[u].push_back(x);
		if(dist[x] > k) continue;
		
		for(int i = 0; i < e[x].size(); i++)
			if(lastVis[e[x][i]] != u)
			{
				int y = e[x][i];
				lastVis[y] = u;
				dist[y] = dist[x] + 1;
				q[++t] = y;
			}
		
		for(int i = 0; i < re[x].size(); i++)
			if(lastVis[re[x][i]] != u)
			{
				int y = re[x][i]; 
				lastVis[y] = u;
				dist[y] = dist[x] + 1;
				q[++t] = y;
			}
	}
	
	/* debug */
	if(u==1) puts("");
	if(verbose)
		printf("Dist %d from Node %d: %d\n", k, u, neighbor[u].size());
	maxCounter = max(maxCounter, (int)neighbor[u].size());
}

double Similarity(int x, int y){
	int a=(successor[x] & successor[y]).count();
	int b=(successor[x] | successor[y]).count();
	if(b) return 1.*a;
	return -1;
}

/* union-find set */
void initPackage(){
	for(int i = 1; i <= n; i++) pa[i] = i;
}
int getRoot(int k){
	return k==pa[k]?k:pa[k]=getRoot(pa[k]);
}

/* neightbor pair */
struct Data{
	double sim;
	int u, v;
	bool operator < (const Data &rhs) const {
		return sim > rhs.sim;
	}
}data[maxn*maxs];

/* package info */
struct PACK{
	int sizSucc;
	int index;
	bool operator < (const PACK &rhs) const {
		return sizSucc > rhs.sizSucc;
	}
}Pinfo[maxn];

int main(int argc,char *argv[]) {
	graphFile=argv[1];
	weightFile=argv[2];
	buildGraph();
	sscanf(argv[3],"%d",&K);
	
	/* Successor set */
	puts("finding successors...");
	for(int u = 1; u <= n; u++)
	{
		if(u%1000==0) printf("%d complete\n", u);
		findSuccessor(u, 0);
	}
//	system("pause");
	
	/* Process neightbor nodes K steps from u */
	printf("finding neighbors in %d steps...\n", K);
	int cntPair = 0;
	for(int u = 1; u <= n; u++)
	{
		if(u%1000==0)
		{
			printf("%d complete\n", u);
			printf("max neighbor Counter so far: %d\n", maxCounter);
		}
		findNeighbor(u, K, 0);
		
		for(int i = 0; i < neighbor[u].size(); i++)
		{
			data[++cntPair] = (Data){ Similarity(u, neighbor[u][i]), u, neighbor[u][i] };
		}
	}
	sort(data+1, data+cntPair+1);
//	system("pause");
	
	/* Stage 1: Package all nodes ONCE */
	puts("packaging all nodes ONCE...");
	initPackage();
	for(int i = 1; i <= cntPair; i++)
	{
		int fu = getRoot(data[i].u);
		int fv = getRoot(data[i].v);
		if(fu != fv && weight[fu] + weight[fv] <= LIMIT_W)
		{
			pa[fu] = fv;
			weight[fv] += weight[fu];
		}
	}
	
	for(int u = 1; u <= n; u++)
	{
		int p = getRoot(u);
		Package[p].push_back(u);
	}
//	system("pause");
	
	/* Stage 2: Copy packages */
	/* method 1: sort packages by size of merged successor set */
	/* method 2: ??? */
	puts("copying best packages...");
	int cntPackage = 0;
	for(int i = 1; i <= n; i++) if(Package[i].size())
	{
		static bitset<maxn> b;
		b = 0;
		for(int j = 0; j < Package[i].size(); j++) b |= successor[Package[i][j]];
		Pinfo[++cntPackage] = (PACK){ b.count(), i };
	}
	sort(Pinfo+1, Pinfo+cntPackage+1);
	
	int extra = 1;
	long long accW = 0;
	while(accW + weight[Pinfo[extra].index]*2 <= W * 1.5)
	{
		accW += weight[Pinfo[extra].index]*2;
		extra++;
	}
//	system("pause");
	
	/* Output */ 
	freopen("result.txt", "w", stdout); 
	
	printf("%d\n", 2*extra - 2);
	
	for(int i = 1; i < extra; i++)
	{
		int tmpSize = Package[Pinfo[i].index].size(); 
		printf("%d ", tmpSize);
		for(int j = 0; j < tmpSize; j++)
			printf("%d ", Package[Pinfo[i].index][j]);
		puts("");
	}
	
	for(int i = 1; i < extra; i++)
	{
		int tmpSize = Package[Pinfo[i].index].size();
		printf("%d ", tmpSize);
		for(int j = 0; j < tmpSize; j++)
			printf("%d ", Package[Pinfo[i].index][j]);
		puts("");
	}
}
