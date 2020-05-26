#include<cstdio>
#include<algorithm>
#include<cstring>
#include<iostream>
#include<cstring>
#include<string>
#include<cmath>
#include<ctime>
#include<set>
#include<vector>
#include<map>
#include<queue>

#define N 1500005
#define M 8000005

#define ls (t<<1)
#define rs ((t<<1)|1)
#define mid ((l+r)>>1)

#define mk make_pair
#define pb push_back
#define fi first
#define se second

using namespace std;

int i,j,m,n,p,k,weight[N],Num,x,y,vis[N],has[N],deg[N],Q[N],Compo;

vector<int>v[N],pack[N];

int Rand()
{
		return (rand()<<15)+rand();
}

int Real_Work()
{
		int i,j;
	   for (i=1;i<=n;++i) has[i]=deg[i]=0;
	   for (i=1;i<=Num;++i)
	   		if (!vis[i])
	   			for (j=0;j<(int)pack[i].size();++j) has[pack[i][j]]=1;
	   	for (i=1;i<=n;++i)
	   		for (j=0;j<(int)v[i].size();++j) deg[v[i][j]]++;
	   	for (i=1,Q[0]=0;i<=n;++i) if (!deg[i]) Q[++Q[0]]=i;
	   	for (int l=1;l<=Q[0];++l)
	   	{
	   		int p=Q[l];
	   		for (i=0;i<(int)v[p].size();++i)
	   		{
	   				int k=v[p][i];
	   				deg[k]--;
	   				has[k]&=has[p];
	   				if (deg[k]==0) Q[++Q[0]]=k;
	   		}
	   	}
	   	int ans=0;
	   	for (i=1;i<=n;++i) ans+=has[i];
	   	return ans;
	   		
}
int Work_Random(double rate)
{
		int i;
		for (i=1;i<=Num;++i) vis[i]=0;
		for (i=1;i<=(int)(Num*rate);++i)
		{
				int x=Rand()%Num+1;
				while (vis[x]) x=Rand()%Num+1;
				vis[x]=1;
		}
		return Real_Work();
}

int Work_Sequ(double rate)
{
		int i;
		for (i=1;i<=Num;++i) vis[i]=0;
		int str=Rand()%(int)(Num-Num*(rate*1.1))+1;
		for (i=1;i<=(int)(Num*rate);++i) vis[str+i-1]=1;
		return Real_Work();
}

void Start()
{
	  int i;
      double T1=0,T2=0,T3=0;
	  for (i=0;i<128;++i)
	  {
	  		T1+=Work_Random(0.1)/128.;
	  		T2+=Work_Random(0.2)/128.;
	  		T3+=Work_Random(0.3)/128.;
//	  		fprintf(stderr,"Random round %d successful!\n",i); 
	  }
	  fprintf(stderr,"Random test success!\n");
	  printf("Random Result:%.10lf\n",(T1+T2+T3)/3./n);
	  printf("Result on 10%:%.10lf\n",T1/n);
	  printf("Result on 20%:%.10lf\n",T2/n);
	  printf("Result on 30%:%.10lf\n",T3/n);
	  T1=T2=T3=0;
	  for (i=0;i<128;++i)
	  {
	  		T1+=Work_Sequ(0.1)/128.;
	  		T2+=Work_Sequ(0.2)/128.;
	  		T3+=Work_Sequ(0.3)/128.;
//	  		fprintf(stderr,"Sequ round %d successful!\n",i); 
	  }
	  fprintf(stderr,"Sequence test success!\n");
	  printf("Sequ Result:%.10lf\n",(T1+T2+T3)/3./n);
	  printf("Result on 10%:%.10lf\n",T1/n);
	  printf("Result on 20%:%.10lf\n",T2/n);
	  printf("Result on 30%:%.10lf\n",T3/n);
}

int main(int argc, char* argv[])
{
	freopen("Result.txt","w",stdout); 
	if (argc!=4)
	{
		puts("You need exactly 3 arguments for the program!");
		return 0;
	}
	srand((int)time(0));
	freopen(argv[1],"r",stdin); //����Graph
	for (;;)
	{
		scanf("%d%d",&x,&y);
		if (x==0&&y==0) break;
		v[x].pb(y);
	}
	freopen(argv[2],"r",stdin); //����Ȩ�� 
	for (;;)
	{
		scanf("%d%d",&x,&y);
		if (x==0&&y==0) break;
		weight[x]=y; n=max(n,max(x,y)); 
	}
	for (i=1;i<=n;++i) weight[0]+=weight[i];
	freopen(argv[3],"r",stdin);
	scanf("%d",&Num);
	long long tmp=0;
	for (i=1;i<=Num;++i)
	{
		int sum,temp=0;
		scanf("%d",&sum);
		for (;sum--;)
		{
			scanf("%d",&x);		
			if (x<1||x>n)
			{
					puts("Error id");
					return 0;
			}
			temp+=weight[x];
			tmp+=weight[x];
			pack[i].pb(x);
		}
		if (temp>2000)
		{
				printf("Pack "); printf("%d ",i); printf("is more than 2000!"); 
				return 0;
		}
	}
	if (tmp>1.5*weight[0])
	{
			puts("total weight is bigger than 1.5W!");
			return 0;
	}
	fprintf(stderr,"Read Successful!\nYou have %d packages,Origin Weight=%lld,total Weight=%lld\n",Num,weight[0],tmp); 
	Start();
}

