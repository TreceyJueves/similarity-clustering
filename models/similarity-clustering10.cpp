#include <bits/stdc++.h>

using namespace std;

const int maxn=100000;
const int LIMIT_W=2000;

#define long long long

/* input files */
char *graphFile;
char *weightFile;

vector<int> e[maxn]; // 正向边，从父亲指向儿子
vector<int> re[maxn]; // 反向边
vector<int> neighbor[maxn]; // 距离不超过K的邻居
vector<int> successor[maxn];
vector<int> Package[maxn];

bool vzt[maxn];
int m,n,weight[maxn],pa[maxn];
unsigned long W;
unsigned long sumSuccSize[maxn];

void merge(vector<int> &dest,vector<int> &src)
{
	auto *temp=new vector<int>();
	int p1=0,p2=0;
	const int n1=dest.size(),n2=src.size();
	while(p1<n1&&p2<n2)
	{
		int pushVal;
		if(dest[p1]<src[p2])
			pushVal=dest[p1];
		else pushVal=src[p2];
		temp->push_back(pushVal);
		while(p1<n1&&dest[p1]==pushVal)
			p1++;
		while(p2<n2&&src[p2]==pushVal)
			p2++;
	}
	while(p1<n1)
		temp->push_back(dest[p1++]);
	while(p2<n2)
		temp->push_back(src[p2++]);
	dest.clear();
	for(int i:*temp)
		dest.push_back(i);
	delete temp;
}

int intersectSize(const vector<int> &a,const vector<int> &b)
{
	int count=0;
	int p1=0;
	const int n1=a.size();
	for(int i:b)
	{
		while(p1<n1&&a[p1]<i)
			p1++;
		if(a[p1]==i)
			count++;
	}
	return count;
}

void buildGraph()
{
	FILE *f1=fopen(graphFile,"r");
	int u,v;
	while(~fscanf(f1,"%d%d",&u,&v)&&u&&v)
	{
		m++;
		assert(u<v);
		e[u].push_back(v);
		re[v].push_back(u);
	}
	fclose(f1);
	
	f1=fopen(weightFile,"r");
	while(~fscanf(f1,"%d%d",&u,&v)&&u&&v)
	{
		weight[u]=v;
		W+=v;
		n=max(n,u);
		assert(n<maxn);
	}
	fclose(f1);
}

void findSuccessor(int u)
{
	if(vzt[u]) return;
	vzt[u]=true;
	successor[u].push_back(u);
	for(int v:e[u])
	{
		findSuccessor(v);
		merge(successor[u],successor[v]);
	}
}

double getSimilarity(int x,int y)
{
	double a=intersectSize(successor[x],successor[y]);
	double b=successor[x].size() + successor[y].size() - a;
	return fabs(b)>1e-5 ? a/*/b*/:-1;
}

struct MergeLink
{
	double sim;
	int u,v;
	unsigned su,sv;
	
	MergeLink(int u,int v) : u(u),v(v)
	{
		sim=getSimilarity(u,v);
		su=successor[u].size();
		sv=successor[v].size();
	}
	
	bool upToDate()
	{
		return su==successor[u].size()&&sv==successor[v].size();
	}
	
	bool operator<(const MergeLink &rhs) const
	{
		return sim<rhs.sim;
	}
};

priority_queue<MergeLink> links;

struct CopyCandidate
{
	unsigned long significance;
	int index;
	
	bool operator<(const CopyCandidate &rhs) const
	{
		return significance>rhs.significance;
	}
} candidates[maxn];

void init()
{
	for(int u=1;u<=n;u++)
	{
		pa[u]=u;
		sumSuccSize[u]=successor[u].size();
		for(int v:e[u])
		{
			neighbor[u].push_back(v);
			links.push(MergeLink(u,v));
		}
		for(int f:re[u])
			neighbor[u].push_back(f);
		sort(neighbor[u].begin(),neighbor[u].end());
		vector<int>::iterator it=unique(neighbor[u].begin(),neighbor[u].end());
		neighbor[u].erase(it,neighbor[u].end());
		
	}
}

int getRoot(int k)
{
	return k==pa[k] ? k:pa[k]=getRoot(pa[k]);
}

void remove(vector<int> &vec,int x)
{
	vector<int>::iterator it;
	it=lower_bound(vec.begin(),vec.end(),x);
	vec.erase(it);
}

void insert(vector<int> &vec,int x)
{
	vector<int>::iterator it=lower_bound(vec.begin(),vec.end(),x);
	if(*it==x)
		return;
	vec.insert(it,x);
}

void merge(int p1,int p2)
{
	merge(successor[p1],successor[p2]);
	merge(neighbor[p1],neighbor[p2]);
	sumSuccSize[p1]+=sumSuccSize[p2];
	weight[p1]+=weight[p2];
	pa[p2]=p1;
	//O(n^2)
	for(int aNeighborOf2:neighbor[p2])
	{
		remove(neighbor[aNeighborOf2],p2);
		insert(neighbor[aNeighborOf2],p1);
	}
	remove(neighbor[p1],p1);
	//O(n^2)
	for(int aNeighborOf1:neighbor[p1])
		links.push(MergeLink(p1,aNeighborOf1));
}

int main(int argc,char *argv[])
{
	if(argc>=3)
	{
		graphFile=argv[1];
		weightFile=argv[2];
	}
	else
	{
		graphFile=new char[50];
		weightFile=new char[50];
		gets(graphFile);
		gets(weightFile);
	}
	buildGraph();
	/* Successor set */
	puts("finding successors...");
	for(int u=1;u<=n;u++)
		findSuccessor(u);
	puts("Initializing...");
	init();
	puts("Creating original packages...");
	while(!links.empty())
	{
		MergeLink e=links.top();
		links.pop();
		int u=e.u;
		int v=e.v;
		if(u==getRoot(u)&&v==getRoot(v)&&e.upToDate()&&
		   weight[u] + weight[v]<=LIMIT_W)
			merge(u,v);
	}
	for(int u=1;u<=n;u++)
	{
		int p=getRoot(u);
		Package[p].push_back(u);
	}
	
	/*Stage 2*/
	puts("copying best packages...");
	int cntPackage=0;
	for(int i=1;i<=n;i++)
		if(i==getRoot(i))
			candidates[++cntPackage]=(CopyCandidate) {successor[i].size()/*sumSuccSize[i]*/,i};
	sort(candidates + 1,candidates + cntPackage + 1);
	
	int extra=1;
	unsigned long accW=0;
	while(accW + weight[candidates[extra].index]<=W/2)
	{
		accW+=weight[candidates[extra].index];
		extra++;
	}
	
	/* Output */
	freopen("result.txt","w",stdout);
	
	printf("%d\n",cntPackage + extra - 1);
	
	for(int i=1;i<=cntPackage;i++)
	{
		int tmpSize=Package[candidates[i].index].size();
		printf("%d ",tmpSize);
		for(int j=0;j<tmpSize;j++)
			printf("%d ",Package[candidates[i].index][j]);
		puts("");
	}
	
	for(int i=1;i<extra;i++)
	{
		int tmpSize=Package[candidates[i].index].size();
		printf("%d ",tmpSize);
		for(int j=0;j<tmpSize;j++)
			printf("%d ",Package[candidates[i].index][j]);
		puts("");
	}
}
