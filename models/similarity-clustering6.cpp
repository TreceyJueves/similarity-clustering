#include <bits/stdc++.h>

using namespace std;

const int maxn = 100000;
const int maxb = 200000;
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
vector<int> Package[maxb];
bitset<maxn> successor[maxn];

bool vzt[maxn];
int copied[maxn];
int m, n, weight[maxb], pa[maxb], maxCounter;
long long W;
int extra;

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
	for(unsigned i=0; i<e[u].size(); i++)
	{
		int v = e[u][i];
		successor[u][v] = 1;
		findSuccessor(v);
		successor[u] |= successor[v];
	}
	
	/* debug */
	if(verbose)
		printf("Node %d has %llu Successors\n", u, successor[u].count());
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
		
		for(unsigned i = 0; i < e[x].size(); i++)
			if(lastVis[e[x][i]] != u)
			{
				int y = e[x][i];
				lastVis[y] = u;
				dist[y] = dist[x] + 1;
				q[++t] = y;
			}
		
		for(unsigned i = 0; i < re[x].size(); i++)
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
		printf("Dist %d from Node %d: %llu\n", k, u, neighbor[u].size());
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
	for(int i = 1; i < n + extra; i++) pa[i] = i;
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

/* node info */
struct NODE{
	long long unsigned sizSucc;
	int index;
	bool operator < (const NODE &rhs) const {
		return sizSucc > rhs.sizSucc;
	}
}Ninfo[maxn];

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
		Ninfo[u]=(NODE){successor[u].count(),u};
	}
	sort(Ninfo+1, Ninfo+n+1); 
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
		
		for(unsigned i = 0; i < neighbor[u].size(); i++)
		{
			data[++cntPair] = (Data){ Similarity(u, neighbor[u][i]), u, neighbor[u][i] };
		}
	}
	sort(data+1, data+cntPair+1);
//	system("pause");
	
	/*Stage 1: Copy nodes */
	puts("copying most significant nodes...");
	long long accW = 0;
	extra = 1;
	while(accW + weight[Ninfo[extra].index] <= W / 2)
	{
		accW += weight[Ninfo[extra].index];
		weight[n + extra] = weight[Ninfo[extra].index];
		copied[Ninfo[extra].index] = n + extra;
		extra++;
	}
	
	/* Stage 2: Package all nodes */
	puts("packaging all nodes...");
	initPackage();
	for(int i = 1; i <= cntPair; i++)
	{
		int u = data[i].u;
		int v = data[i].v;
		int u2 = copied[u];
		int v2 = copied[v];
		int fu = getRoot(u);
		int fv = getRoot(v);
		if(fu == fv)
			continue;
		if(weight[fu] + weight[fv] <= LIMIT_W)
		{
			pa[fu] = fv;
			weight[fv] += weight[fu];
			continue;
		}
		if(u2)
		{
			int fu2 = getRoot(u2);
			if(fu2 == fv)
				continue;
			if(weight[fu2] + weight[fv] <= LIMIT_W)
			{
				pa[fu2] = fv;
				weight[fv] += weight[fu2];
				continue;
			}
		}
		if(v2)
		{
			int fv2 = getRoot(v2);
			if(fu == fv2)
				continue;
			if(weight[fu] + weight[fv2] <= LIMIT_W)
			{
				pa[fu] = fv2;
				weight[fv2] += weight[fu];
				continue;
			}
		}
		if(u2 && v2)
		{
			int fu2 = getRoot(u2);
			int fv2 = getRoot(v2);
			if(fu2 == fv2)
				continue;
			if(weight[fu2] + weight[fv2] <= LIMIT_W)
			{
				pa[fu2] = fv2;
				weight[fv2] += weight[fu2];
				continue;
			}
		}
	}
	
	for(int u = 1; u < n + extra; u++)
	{
		int p = getRoot(u);
		Package[p].push_back(u);
	}
	int cntPackage = 0;
	for(int u = 1; u < n + extra; u++)
		if(Package[u].size())
			cntPackage++;
	/* Output */ 
	freopen("result.txt", "w", stdout); 
	
	printf("%d\n", cntPackage);
	
	for(int i = 1; i < n + extra; i++)
	{
		int tmpSize = Package[i].size();
		if(!tmpSize) continue;
		printf("%d ", tmpSize/*,weight[i]*/);
		for(int j = 0; j < tmpSize; j++)
		{
			int node = Package[i][j];
			if(node>n)
				node = Ninfo[node-n].index;
			printf("%d ", node);
		}
		puts("");
	}
}
