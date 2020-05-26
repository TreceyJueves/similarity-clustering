#include<vector>
#include<cstdio>
#include<cassert>
#include<cstring>
#include<algorithm>
using namespace std;
#define maxn 200005
char graphFile[]="Out_OutGraph_Basketball_480_Slice16_Gop32_3.log";
vector<int> e[maxn];
vector<int> re[maxn];
vector<int> blockSize;
bool vzt[maxn];
int nodes;
void dfs(int u)
{
	nodes++;
	vzt[u]=true;
	for(unsigned i=0;i<e[u].size();i++)
		if(!vzt[e[u][i]])
			dfs(e[u][i]);
	for(unsigned i=0;i<re[u].size();i++)
		if(!vzt[re[u][i]])
			dfs(re[u][i]);
}
int main()
{
	FILE* f1=fopen(graphFile,"r");
	int u,v;
	int m=0,n=0;
	while(~fscanf(f1,"%d%d",&u,&v)&&u&&v)
	{
		m++;
		assert(u<v);
		e[u].push_back(v);
		re[v].push_back(u); 
		n=max(n,u);
		n=max(n,v);
		assert(n<maxn);
	}
	fclose(f1);
	for(int i=1;i<=n;i++)
		if(!vzt[i])
		{
			nodes=0;
			dfs(i);
		//	if(nodes == 1)
		//		printf("single node: %d", i);
			blockSize.push_back(nodes);
		}
	printf("%llu\n",blockSize.size());
	for(unsigned i=0;i<blockSize.size();i++)
		printf("%d ",blockSize[i]);
}

