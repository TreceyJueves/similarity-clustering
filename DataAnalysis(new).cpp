#include <bits/stdc++.h>

using namespace std;

const int maxn = 200005;

//char graphFile[]="Out_OutGraph_Basketball_480_Slice16_Gop8_10.log";
char graphFile[]="Out_OutGraph_Basketball_480_Slice16_Gop32_3.log";

vector<int> e[maxn]; // 正向边，从父亲指向儿子 
vector<int> re[maxn]; // 反向边 
vector<int> blockSize; // 弱联通块大小 

bool vzt[maxn];
int blockSizeCounter;
int m, n, maxCounter, sumCounter;

void addEdge(){
	FILE* f1=fopen(graphFile,"r");
	int u,v;
	while(~fscanf(f1, "%d%d", &u, &v) && u && v)
	{
		m++;
		assert(u < v);
		e[u].push_back(v);
		re[v].push_back(u);
		n=max(n, max(u, v));
		assert(n < maxn);
	}
	fclose(f1);
}

void dfsBlock(int u)
{
	blockSizeCounter++;
	vzt[u]=true;
	for(unsigned i=0;i<e[u].size();i++)
		if(!vzt[e[u][i]])
			dfsBlock(e[u][i]);
	for(unsigned i=0;i<re[u].size();i++)
		if(!vzt[re[u][i]])
			dfsBlock(re[u][i]);
}

void countKDist(int u, int k, int verbose = 0){
	static queue<int> q;
	static int dist[maxn];
	int counter = 0;
	
	memset(dist, 0, sizeof dist);
	q.push(u); dist[u]=1;
	
	while(!q.empty())
	{
		int x=q.front(); q.pop();
		if(dist[x] > k) continue;
		for(unsigned i=0; i<e[x].size(); i++)
			if(!dist[e[x][i]])
			{
				dist[e[x][i]] = dist[x] + 1;
				q.push(e[x][i]);
				counter++;
			}
		for(unsigned i=0; i<re[x].size(); i++)
			if(!dist[re[x][i]])
			{
				dist[re[x][i]] = dist[x] + 1;
				q.push(re[x][i]);
				counter++;
			}
	}
	
	if(verbose)
		printf("Dist %d from Node %d: %d\n", k, u, counter);
	sumCounter += counter;
	maxCounter = max(maxCounter, counter);
}

int main()
{
	addEdge();
	
	// Block statistics
	for(int i = 1; i <= n; i++) 
		if(!vzt[i])
		{
			blockSizeCounter=0;
			dfsBlock(i);
			blockSize.push_back(blockSizeCounter);
		}
	// Output graph info
	printf("[File %s]\n", graphFile);
	printf("Nodes: %d\n", n); 
	printf("Blocks: %llu\n",blockSize.size());
	printf("Size of Each Block:\n");
	sort(blockSize.begin(), blockSize.end());
	for(unsigned i = int(blockSize.size())-1; ~i; i--)
		printf("%d ",blockSize[i]);
	puts("");
	
	//freopen("a.txt", "w", stdout);
	// Counting nodes K steps from i
	int K;
	scanf("%d", &K); 
	for(int i=1; i<=n; i++)
	{
		countKDist(i, K, 0);
	}
	
	printf("[File %s]\n", graphFile);
	printf("MaxCounter: %d\n", maxCounter);
	printf("Aveerage: %lf\n", 1.*sumCounter/n);
}

