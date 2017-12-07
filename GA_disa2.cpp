//#pragma GCC optimize(2)

#include<iostream>
#include<cstdlib>
#include<fstream>
#include<cstring>
#include<cstdio>
#include<algorithm>
#include<utility>
#include<vector>
#include<stack>
#include<string>
#include<iterator>
#include<bitset>
#include<math.h>
#include<time.h>
#include<stdlib.h>
#include<functional>

#define MAXN 4010
#define MAXM 1000010
#define EPS 1e-8
#define TOTAL_VERTEX 4005
using namespace std;
typedef long long LL;

clock_t start,finish;
double duration;

int n;
int m;
//vector<int> g[MAXN];
bitset<TOTAL_VERTEX> ans;
int d[MAXN];
bool mat[MAXN][MAXN];

//int used[MAXN];

bitset<TOTAL_VERTEX> a[MAXN];
//bitset<TOTAL_VERTEX> temp_a[MAXN*MAXN];

double a_score[MAXM];
double score_sort[MAXM];

int init_group_size;
int one_bit_num;
int now_group_size;
int max_group_size;
int cross_over_pair;
int max_clique_size;
int max_iterate;
double cross_over_rate;
double mutate_rate;//突变个体比例，至少要小于1/(n*n)
double mutate_p;//每一位的变异概率
double p_survive;
double one_bit_rate;
double connect_rate;
double disaster_rate;
int disaster_period;

/*
void find_clique_BF(int x)
{
	int i,j;
	for(i=1;i<=n;i++)
	{
		if(d[i]>=(x-1))
		{
			memset(used,0,sizeof(used));
			//dfs_BF()
		}
	}
}

bool check()
{
	int i,j;
	for(i=0;i<now_ans.size();i++)
	{
		int x=now_ans[i];
		for(j=i;j<now_ans.size();j++)
		{
			int y=now_ans[j];
			if(!mat[x][y])
				return 0;
		}
	}
	return 1;
}

*/

void generate_ont_bit_max(int group_size,int one_bit) //generate_one_hot
{
	ans.reset();

	int i,j;
	vector<int> temp;
	for(i=1;i<=n;i++) temp.push_back(i);

	for(i=1;i<=group_size;i++)
	{
		random_shuffle(temp.begin(),temp.end());
		a[i].reset();
		for(j=1;j<=one_bit;j++)
		{
			a[i].set(temp[j]);// set bit_pos j as 1 
		}

		//cout<<a[i]<<endl;
	}
	//cout<<"Generation complete!"<<endl;
}



void generate_one_hot() //generate_one_hot
{
	ans.reset();

	int i,j;

/*
	vector<int> temp;
	for(i=1;i<=n;i++) temp.push_back(i);
	random_shuffle(temp.begin(),temp.end());
*/

	for(i=1;i<=n;i++)
	{
		a[i].reset();//ignore the bit_0
		a[i].set(i);// set bit_pos j as 1 

		//cout<<a[i]<<endl;
	}
	cout<<"Generation with one-hot complete!"<<endl;
}

int sub_d[MAXN];
double score_next(int x)//设置为边数比点数
{
	int i,j;
	bitset<TOTAL_VERTEX> y=a[x];
	vector<int> temp,temp_y;

	for(i=1;i<=n;i++)
		if(a[x][i]==1)
		{
			temp.push_back(i);
			sub_d[i]=0;
		}
	for(i=0;i<temp.size();i++)
	{
		for(j=0;j<temp.size();j++)
		{
			if(mat[temp[i]][temp[j]])
			{
				sub_d[temp[i]]++;
			}
		}
	}

	for(i=0;i<temp.size();i++)
	{
		if(sub_d[temp[i]] >= ceil(connect_rate*(a[x].count()))  )//考虑只有2个点的情形
		{//0.5的这个值可以随意调整，最后的出口保证答案一定是团
			temp_y.push_back(temp[i]);
		}
	}

	int edge_cnt=0;

	for(i=0;i<temp_y.size();i++)
		for(j=i+1;j<temp_y.size();j++)
			edge_cnt+=mat[temp_y[i]][temp_y[j]];
			
	return edge_cnt*1.0/a[x].count();
}


double score(int x)//设置为边数比点数
{
	int i,j;
	int cnt_edge=0;

	vector<int> temp;

	for(i=1;i<=n;i++)
		if(a[x][i]==1)
			temp.push_back(i);

	for(i=0;i<temp.size();i++)
	{
		for(j=i;j<temp.size();j++)
		{
			cnt_edge+=mat[temp[i]][temp[j]];
		}
	}

	if(a[x].count()==0) return 0;
	//cout<<"edges:"<<cnt_edge<<endl;
	return cnt_edge*1.0/a[x].count();
}

/*
double score_old(bitset<TOTAL_VERTEX> &x)//设置为边数比点数
{
	int i,j;
	int cnt_edge=0;

	vector<int> temp;

	for(i=1;i<=n;i++)
		if(x[i]==1)
			temp.push_back(i);

	for(i=0;i<temp.size();i++)
	{
		for(j=i;j<temp.size();j++)
		{
			cnt_edge+=mat[temp[i]][temp[j]];
		}
	}

	if(x.count()==0) return 0;
	//cout<<"edges:"<<cnt_edge<<endl;
	return cnt_edge*1.0/x.count();
}
*/

void substitute()
{
	int i,j;
	int score_max_ind=1;
	double score_max=0;
	double score_min=1e18;

	for(i=1;i<=now_group_size;i++)
	{
		a_score[i]=score(i);

		//cout<<a[i]<<endl;
		//cout<<i<<" "<<a_score[i]<<endl;

		if(score_max < a_score[i])
		{
			score_max = a_score[i];
			score_max_ind = i; 
		}
		score_min=min(score_min,a_score[i]);
	}

	cout<<"Max score now is:"<<score_max<<endl;

	for(i=1;i<=now_group_size;i++)
	{
		if(fabs(a_score[i]-score_max)<=EPS) continue;
		else 
		{
			if(fabs(a_score[i]-score_min)<=EPS)
			{
				a[i]=a[score_max_ind];
			}
		}
	}

	// cout<<"substitution complete!"<<endl;

}

bitset<TOTAL_VERTEX> have_sex_ret;



void have_sex(int x,int y)
{

	int i,j;

	//cout<<"having sex~"<<endl;

	//vector<int> temp;
	//for(i=1;i<=n;i++) temp.push_back(i);
	//vector<int> one_to_n;
	//for(i=1;i<=n;i++) one_to_n.push_back(i);
	//random_shuffle(one_to_n.begin(),one_to_n.end());
	
	//int host=rand()%2; //以谁为主要母体
	have_sex_ret=a[x];
	//(score(x)>score(y))?a[x]:a[y];

	for(i=0;i<floor(n * cross_over_rate);i++)
	{
		//cout<<i<<endl;
		//cout<<one_to_n[0]<<endl;
		j=rand()%n+1;

		int op1,op2;
		op1=a[x][j];
		op2=a[y][j];
		if(op1 != op2)
		{
			have_sex_ret.flip(j);
		}
	}
	
}
/*
void have_sex(bitset<TOTAL_VERTEX> &x,bitset<TOTAL_VERTEX> &y)
{
	//ret.reset();
	//if(rand()%2) ret=x;
	//else ret=y;

	int i,j;

	//cout<<"having sex~"<<endl;

	vector<int> temp;
	for(i=1;i<=n;i++) temp.push_back(i);
	random_shuffle(temp.begin(),temp.end());
	
	int host=rand()%2; //以谁为主要母体
	have_sex_ret=(score(x)>score(y))?x:y;

	for(i=0;i<floor(n * cross_over_rate);i++)
	{
		int op1,op2;
		op1=x[temp[i]];
		op2=y[temp[i]];
		if(op1 != op2)
		{
			have_sex_ret.flip(temp[i]);
		}
	}
	
}

*/
void cross_over_random()//两两交配，随机等概率交叉互换，亲本保留
{
	int i,j;
	//int next_group_size=(LL)now_group_size*(now_group_size+1)/2;

	int now_ind=now_group_size;

	for(i=1;i<=cross_over_pair;i++)
	{	
		int op1=rand()%now_group_size+1;
		int op2=rand()%now_group_size+1;

		now_ind++;
		have_sex(op1,op2);
		a[now_ind]=have_sex_ret;
	}

	now_group_size=now_ind;

	//cout<<"Cross-over complete!"<<endl;

}

/*
void cross_over()//两两交配，随机等概率交叉互换，亲本保留
{
	int i,j;
	//int next_group_size=(LL)now_group_size*(now_group_size+1)/2;

	int now_ind=now_group_size;

	cout<<"now group:"<<now_group_size<<endl;

	for(i=1;i<=now_group_size;i++)
	{
		for(j=i+1;j<=now_group_size;j++)
		{
			now_ind++;
			have_sex(a[i],a[j]);
			a[now_ind]=have_sex_ret;
		}
	}

	now_group_size=now_ind;
	//cout<<"next group:"<<now_group_size<<endl;
	
	//cout<<"Cross-over complete!"<<endl;

}
*/
void mutate()
{
	int i,j;

	//cout<<"Every-bit-mutate-rate:"<<mutate_p<<endl;



	for(i=1;i<=now_group_size/10;i++)
	{
		for(j=1;j<=n;j++)
		{
			if(rand()%10000000*1.0/10000000 < mutate_p)
			{
				a[rand()%now_group_size+1].flip(j);
			}
		}
	}

	//cout<<"Mutate complete!"<<endl;
}


int t_for_sel[MAXN];
int t_cnt;

void select()
{
	int i,j;
	double total_score=0;
	if(now_group_size<=max_group_size) return;


	//cout<<"selecting:"<<now_group_size<<endl;
	for(i=1;i<=now_group_size;i++)
	{
		a_score[i]=score(i);
		total_score+=a_score[i];
		score_sort[i]=a_score[i]; //可以优化！！！
	}
	sort(score_sort+1,score_sort+now_group_size+1,greater<double>());

	//cout<<max_group_size<<endl;
	//int next_group_size=floor(sqrt(2.0*max_group_size));
	
	int next_group_size=max_group_size;

	//cout<<"coutnt "<<next_group_size<<endl;
	double min_score=score_sort[next_group_size];
	int now_ind=0;

	//vector<int> temp;

	t_cnt=0;
	for(i=1;i<=now_group_size;i++)
	{
		if(a_score[i]>=min_score)
		{
			t_cnt++;
			//temp.push_back(i);
			t_for_sel[t_cnt]=i;
		//	now_ind++;
		//	temp_a[now_ind]=a[temp[i]];
		}
	}

	for(i=1;i<=t_cnt;i++)
	{
		if(i!=t_for_sel[i])
			a[i]=a[t_for_sel[i]];
	}

	int kind_cnt=0;
	for(i=1;i<=now_group_size;i++)
	{
		if(score_sort[i]!=score_sort[i-1])
		{
			kind_cnt++;
		}
	}


	now_group_size=next_group_size;
	cout<<"Different kind rate:"<<kind_cnt*1.0/next_group_size<<endl;
}


void select_wheel()
{
	int i,j;
	double total_score=0;
	if(now_group_size<=max_group_size) return;

	for(i=1;i<=now_group_size;i++)
	{
		a_score[i]=score(i);
		total_score+=a_score[i];
		score_sort[i]=a_score[i]; //可以优化！！！
	}
	sort(score_sort+1,score_sort+now_group_size+1,greater<double>());


	int next_group_size=max_group_size;

	//cout<<"coutnt "<<next_group_size<<endl;
	//double min_score=score_sort[next_group_size];

	int now_ind=0;


	vector<int> temp;

	for(i=1;i<=now_group_size;i++)
	{
		//if((rand()%1000000*1.0/1000000) < a_score[i]*1.0/total_score )
		for(j=1;j<=a_score[i]*2;j++)
		{
			temp.push_back(i);
		}	
	}
	random_shuffle(temp.begin(),temp.end());

	for(i=0;i<next_group_size;i++)
	{
		a[i+1]=a[temp[i]];
	}

	//cout<<"next:"<<next_group_size<<endl;
	now_group_size=next_group_size;

	int kind_cnt=0;
	for(i=1;i<=now_group_size;i++)
	{
		if(score_sort[i]!=score_sort[i-1])
		{
			kind_cnt++;
		}
	}


	now_group_size=next_group_size;
	cout<<"Different kind rate:"<<kind_cnt*1.0/next_group_size<<endl;
	//cout<<"Select complete!"<<endl;
}



int t_check[MAXN];
int t_check_cnt;

int t_check_y[MAXN];
int t_check_y_cnt;

bitset<TOTAL_VERTEX> check(int x)
{
	int i,j;
	bitset<TOTAL_VERTEX> y=a[x];
	vector<int> temp_y;

	//memset(sub_d,0,sizeof(sub_d));

	t_check_cnt=0;

	for(i=1;i<=n;i++)
		if(a[x][i]==1)
		{
			t_check_cnt++;
			t_check[t_check_cnt]=i;
			//temp.push_back(i);
			sub_d[i]=0;
		}
	for(i=1;i<=t_check_cnt;i++)
	{
		for(j=1;j<=t_check_cnt;j++)
		{
			if(mat[t_check[i]][t_check[j]])
			{
				sub_d[t_check[i]]++;
			}
		}
	}

	for(i=1;i<=t_check_cnt;i++)
	{
		if(sub_d[t_check[i]] < ceil(connect_rate*(a[x].count()))  )//考虑只有2个点的情形
		{//0.5的这个值可以随意调整，最后的出口保证答案一定是团
			y.reset(t_check[i]);
		}
	}


	t_check_y_cnt=0;

	for(i=1;i<=n;i++)
		if(y[i]==1)
		{
			t_check_y_cnt++;
			t_check_y[t_check_y_cnt]=i;
		}
			//temp_y.push_back(i);

	for(i=1;i<=t_check_y_cnt;i++)
		for(j=i+1;j<=t_check_y_cnt;j++)
			if(!mat[t_check_y[i]][t_check_y[j]])
			{
				//return ;
				y.reset();
				return y;
				//break;
			}
	//return y.count();
	return y;
}
/*
int check(bitset<TOTAL_VERTEX> &x)
{
	int i,j;
	bitset<TOTAL_VERTEX> y=x;
	vector<int> temp,temp_y;

	memset(sub_d,0,sizeof(sub_d));

	for(i=1;i<=n;i++)
		if(x[i]==1)
			temp.push_back(i);

	for(i=0;i<temp.size();i++)
	{
		for(j=0;j<temp.size();j++)
		{
			if(mat[temp[i]][temp[j]])
			{
				sub_d[temp[i]]++;
			}
		}
	}

	for(i=0;i<temp.size();i++)
	{
		if(sub_d[temp[i]] < ceil(connect_rate*(x.count()))  )//考虑只有2个点的情形
		{//0.5的这个值可以随意调整，最后的出口保证答案一定是团
			y.reset(temp[i]);
		}
	}

	for(i=1;i<=n;i++)
		if(y[i]==1)
			temp_y.push_back(i);

	for(i=0;i<temp_y.size();i++)
		for(j=i+1;j<temp_y.size();j++)
			if(!mat[temp_y[i]][temp_y[j]])
			{
				return 0;
				//y.reset();
				//break;
			}
	return y.count();
	//return y;
}
*/
void read_data(int T)
{
	int i,j;


	string data_name="frb100-40";
	string root_dir="/Users/mac/Desktop/MaxClique/data/";
	string file_name;

	memset(mat,0,sizeof(mat));

	//one_to_n.clear();
	//for(i=1;i<=n;i++) one_to_n.push_back(i);

	file_name=root_dir+data_name+"-clq/"+data_name+"-"+char(T+'0')+".clq";
	const char *file=file_name.c_str();

	freopen(file,"r",stdin);

	char op1[10],op2[10];
	int op3,op4;
	scanf("%s%s%d%d",op1,op2,&n,&m);

	for(i=1;i<=n;i++)
	{
		//g[i].clear();
		//g[i].push_back(i);
		mat[i][i]=1;
	}
		//cout<<g[i].size()<<endl;

	for(i=1;i<=m;i++)
	{
		scanf("%s%d%d",op1,&op3,&op4);

		//cout<<op3<<" "<<op4<<endl;

		//g[op3].push_back(op4);
		//g[op4].push_back(op3);
		
		mat[op3][op4]=1;
		mat[op4][op3]=1;
		
		d[op3]++;
		d[op4]++;
	}
	//cout<<"Read data complete!"<<endl;
}

double solve_mutate_p(double MR)
{
	double temp_A;
	temp_A=(1.0/n)*log(1-2*MR/(n*n));
	return 1-exp(temp_A);
}

void set_const()
{
	/*
	max_group_size=1000;

	max_clique_size=100;
	//init_group_size=100;
	//one_bit_num_min=floor(0.7*max_clique_size);
	//one_bit_num_max=floor(0.7*max_clique_size);
	cross_over_pair=1000;
	cross_over_rate=0.99999;
	p_survive=0.9;
	max_iterate=5000;

	mutate_rate=3e-3;
	//mutate_p=solve_mutate_p(mutate_rate);
	mutate_p=3*1e-3;
	one_bit_rate=9;

	connect_rate=0.95; //应该动态调整
	*/
	//要保证diffent kind rate 在0.9左右，mutate不能太大，cross-over=n*n/2最佳
	max_group_size=800;

	max_clique_size=100;
	//init_group_size=100;
	//one_bit_num_min=floor(0.7*max_clique_size);
	//one_bit_num_max=floor(0.7*max_clique_size);
	cross_over_pair=5000;
	//cross_over_rate=0.01
	//cross_over_rate=0.2; //23代24个
	//cross_over_rate=0.4;
	//cross_over_rate=0.6;
	cross_over_rate=0.01/3;//90代24个，最大25个
	//cross_over_rate
	p_survive=0.9;
	max_iterate=10000;

	//mutate_rate=3e-1;
	//mutate_p=solve_mutate_p(mutate_rate);
	mutate_p=1e-3;
	one_bit_rate=0.5;

	connect_rate=0.999; //应该动态调整

	disaster_rate=0.5;
	disaster_period=100;

}

void set_const_iter(int x)
{
	/*
	max_group_size=1000;

	max_clique_size=100;
	//init_group_size=100;
	//one_bit_num_min=floor(0.7*max_clique_size);
	//one_bit_num_max=floor(0.7*max_clique_size);
	cross_over_pair=1000;
	cross_over_rate=0.99999;
	p_survive=0.9;
	max_iterate=5000;

	mutate_rate=3e-3;
	//mutate_p=solve_mutate_p(mutate_rate);
	mutate_p=3*1e-3;
	one_bit_rate=9;

	connect_rate=0.95; //应该动态调整
	*/
	//要保证diffent kind rate 在0.9左右，mutate不能太大，cross-over=n*n/2最佳
	max_group_size=500;

	max_clique_size=100;
	//init_group_size=100;
	//one_bit_num_min=floor(0.7*max_clique_size);
	//one_bit_num_max=floor(0.7*max_clique_size);
	cross_over_pair=2000;
	//cross_over_rate=0.01
	//cross_over_rate=0.2; //23代24个
	//cross_over_rate=0.4;
	//cross_over_rate=0.6;
	
	//cross_over_rate
	p_survive=0.9;
	max_iterate=10000;

	double k,b;
	
	double max_cross=0.01;
	double min_cross=0.0001;

	k=(min_cross-max_cross)/(max_iterate-1);
	b=max_cross-k;

	cross_over_rate=k*x+b;//90代24个，最大25个

	cout<<"Crossover rate:"<<cross_over_rate<<endl;
	

	//mutate_rate=3e-1;
	//mutate_p=solve_mutate_p(mutate_rate);
	mutate_p=3e-3;
	one_bit_rate=0.5;

	connect_rate=0.999; //应该动态调整

	disaster_rate=0.5;
	disaster_period=100;
}


int t_dis[MAXN];
int t_dis_cnt;

void disaster(int x)
{
	if(x%disaster_period!=0) return;
	
	int i,j;

	for(i=1;i<=floor(now_group_size*disaster_rate);i++)//去除1/3人口，用重新生成的方式淘汰得分低的。
	{
		int op1=rand()%now_group_size+1;
		a[op1].reset();
		for(j=1;j<=one_bit_rate*max_clique_size;j++)
		{
			int op2=rand()%n+1;
			a[i].set(op2);// set bit_pos j as 1 
		}
		//cout<<a[i]<<endl;
	}
	cout<<"Disaster happended!"<<endl;
}

void print_time()
{

	cout<<max_group_size<<endl;

	finish=clock();
	duration=(double)(finish-start)/CLOCKS_PER_SEC;
	printf("Time cost: %.2lf\n",duration);
}


int temp_y_safe[MAXN];
int temp_y_cnt;

bool check_for_safe(bitset<TOTAL_VERTEX> &x)
{
	int i,j;
	//vector<int> temp_y;

	temp_y_cnt=0;
	for(i=1;i<=n;i++)
		if(x[i]==1)
		{
			temp_y_cnt++;
			temp_y_safe[temp_y_cnt]=i;
			//temp_y.push_back(i);
		}
	for(i=1;i<=temp_y_cnt;i++)
		for(j=i+1;j<=temp_y_cnt;j++)
			if(!mat[temp_y_safe[i]][temp_y_safe[j]])
			{
				return 0;
			}
		/*
	for(i=0;i<temp_y.size();i++)
		for(j=i+1;j<temp_y.size();j++)
			if(!mat[temp_y[i]][temp_y[j]])
			{
				return 0;
			}
			*/
	return 1;
}

void print_ans(int x)
{
	//if(x%5!=0) return;

	int i;
	cout<<"The MaxClique size is:"<<ans.count()<<endl;
	cout<<"The MaxClique vertex are:"<<endl;
	for(i=0;i<n;i++)
	{
		if(ans[i]==1)
		{
			cout<<i<<" ";
		}
	}
	cout<<endl;
	if(check_for_safe(ans))
	{
		cout<<"Absolutely right!"<<endl;
	}
	else 
	{
		cout<<"This answer is wrong!"<<endl;
	}
}

int main()
{
	srand(2333);

	int T,i,j;
	
	for(T=1;T<=5;T++)
	{
		start=clock();
		cout<<"Test "<<T<<" start!"<<endl;

		read_data(T);
		print_time();


		set_const_iter(1);
		//set_const();
		
		generate_one_hot();
		//generate_ont_bit_max(max_group_size,one_bit_rate*max_clique_size);

		cout<<"****"<<endl;

		print_time();

		
		now_group_size=n;

		int iter;
		for(iter=1;iter<=max_iterate;iter++)
		{

			set_const_iter(iter);

			cout<<"++++++++++ Generation:"<<iter<<" begins! ++++++++++"<<endl;
			
			
			substitute();//用评分最高的个体替换所有最差的个体。
			//print_time();
			cross_over_random();
			//print_time();
			mutate();
			//print_time();
			
			select();

			//disaster(iter);
			//select_wheel();
			//print_time();

			for(i=1;i<=now_group_size;i++)
			{
				a[i]=check(i); //cure the not healthy gene
				
				//int dd=check(i);
				int dd=a[i].count();
				/*
				if(!dd)
				{
					a[i].reset();
					continue;
				}
				*/
				if(ans.count() < dd )
				{
					ans=a[i];
				}
				
			}
			print_ans(iter);
			
			cout<<"---------- Generation:"<<iter<<" ends! ----------"<<endl;
			
		}
	
		int max_ans_size;
		int max_ans_ind;

		for(i=1;i<=now_group_size;i++)
		{
			//a[i]=check(ai); //find the max clique in a[i]
			a[i]=check(i);
			if(ans.count()<a[i].count() )
			{
				ans=a[i];
			}
		}

		print_ans(iter);
		finish=clock();
		duration=(double)(finish-start)/CLOCKS_PER_SEC;
		printf("Test %d total time cost: %.2lf\n",T,duration);
		
		
		cout<<"********************************************************************************"<<endl;
	}



	return 0;
}
