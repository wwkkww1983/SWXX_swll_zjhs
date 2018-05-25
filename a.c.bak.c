#include<stdio.h>
#include<math.h>
#include<string.h>
#include<stdlib.h>
#define N 13
void input(double *,double *);
void sort(double * ,double *);//排序函数
/**
 * **********计算函数*******************
 * calc_xy    --->   计算X，Y
 * calc_P     --->   计算P
 * calc_apb   --->   计算alpha,P,beta
 * calc_alpha --->   计算alpha
 * calc_beta  --->   计算beta
 * calc_Y_c   --->   计算最终的水位Y_c
 * calc_A     --->   计算系数A[i]
 * calc_Q_c   --->   计算最终的流量Qc
 * *************************************
**/
void calc_xy(double *, double *);//
void calc_P(double *,double *, double(*)[], double *,int );//
void calc_apb(double (*)[], double *,double [],double [],int );//
double calc_alpha(double *, double (*)[], int);//
double calc_beta(double (*)[]);//
double calc_Y_c(double (*)[], double *,int,int);//
double calc_A(double (*)[], double *, int);//
double calc_Q_c(double *, int );//
double sum(double[], int);//求和函数

/**
 * *******检验函数*********
 * FH    -->   符号检验
 * SX    -->   适线检验
 * PLSZ  -->   偏离数值检验
 * T     -->   t(学生氏)检验
 * ***********************
**/
int chk_FH(double *, double *);
int chk_SX();
int chk_PLSZ();
int chk_T();
void output();
/**定义全局变量**/
//int m = 3;
int main(void)
{
	/**变量的定义**/
	int res_FH = 0, res_SX = 0, res_PLSZ = 0, res_T = 0;
	int m = 1;//拟合阶数
	double Z[N] = {2.71, 2.64, 2.88, 2.94, 3,    3.21, 3.1,  3.34, 3.47, 3.61, 3.52, 3.76, 3.87};
	double Q[N] = {4.47, 2.19, 11,   14.3, 18.5, 32.1, 24.2, 43.5, 62.4, 86.7, 71.4, 119,   148};
	double X[N] = {0}, Y[N] = {0}, Y_c[N]={0}, Qc[N]={0}, P[m+1][N], alpha[m], beta[m], A[m+1]; // *p_z = Z, *p_q = Q, *p_x = Z, *p_y = Q, , (*p_p)[N] = P, //这种定义会修改z，q的数据
	/**初始化二维数组P[][]**/
	for (int i = 0; i < m + 1; i++)
	{
		for (int j = 0; j < N; j++)
		{
			P[i][j] = 1;
		}
	}
	/**输入函数**/
	//input(Z,Q);
	/**排序**/
	sort(Z,Q);
	/** debug
	for (int i = 0; i < N; i++)
	{
		printf("<debug--main>  Z[%d]==%lf\t", i, Z[i]);
		printf("<debug--main>  Q[%d]==%lf\n", i, Q[i]);
	}
	**/
	/**将Z，Q的值赋给X，Y**/
	for (int i = 0; i < N; i++)
	{
		X[i] = Z[i];
		Y[i] = Q[i];
	}
	/**计算X，Y**/
	calc_xy(X, Y);

	for (int m=1;;m++)
	{
		printf("<debug--main>m==%d\n", m);
		/**计算alpha，beta，P**/
		calc_apb(P,X,alpha,beta,m);

		/**计算系数A[]**/
		for (int i = 0; i < m+1; i++)
		{
			A[i]=calc_A(P, Y, i);
			//printf("<debug--main>  A[%d]=%lf\n", i, A[i]);
		}
		/**计算拟合水位Yc**/
		for (int i = 0; i < N; i++)
		{
			Y_c[i] = calc_Y_c(P, A,i,m);
			//printf("<debug--main>  Y_c[%d]=%lf\n", i, Y_c[i]);
		}
		/**计算拟合流量Qc**/
		for (int i = 0; i < N; i++)
		{
			Qc[i] = calc_Q_c(Y_c,i);
			//printf("<debug--main>  Q_c[%d]=%lf\n", i, Qc[i]);
		}
		/**
		 * ===检验环节===
		 * chk_FH()为符号检验
		 * chk_SX()为适线检验
		 * chk_PLSZ()为偏离数值检验
		 * chk_T()为t(学生氏)检验
		**/
		res_FH = chk_FH(Q, Qc);
		if (res_FH==0)
		{
			printf("符号检验失败！\n");
			continue;
		}
		else
		{
			printf("符号检验通过!\n");
		}

		res_SX = chk_SX(Y, Y_c);
		if (res_SX==0)
		{
			printf("适线检验失败！\n");
			continue;
		}
		else
		{
			printf("适线检验通过!\n");
		}

		res_PLSZ = chk_PLSZ(Q, Qc);
		if (res_PLSZ==0)
		{
			printf("偏离数值检验失败！\n");
			continue;
		}
		else
		{
			printf("偏离数值检验通过!\n");
		}
	}
		//system("pause");
		return 0;
}

/**输入函数**/
void input(double *p_z, double *p_q)
{



	printf("begain input!\n开始水位输入！\n");
	for (int i = 0; i < N;i++)
	{
		printf("请输入第[%d]个水位Z[%d]:", i, i);
		scanf("%lf", p_z++);
		//printf("<debug>x=%d\n", p_x);
	}
	for (int i = 0; i < N;i++)
	{
		printf("请输入第[%d]个流量Q[%d]:", i+1, i+1);
		scanf("%lf", p_q++);
	}
}
/**排序函数，采用冒泡排序法**/
void sort(double *p_z, double *p_q )
{
	double tmp = 0;
	//printf("\n开始排序\n");
	for (int i = 0; i < N - 1 ; i++)
	{
		for (int j = 0; j < N-1-i; j++)
		{
			if (p_z[j]>p_z[j+1])
			{
				tmp = p_z[j];
				p_z[j] = p_z[j+1];
				p_z[j+1] = tmp;
				tmp = p_q[j];
				p_q[j] = p_q[j + 1];
				p_q[j + 1] = tmp;
			}
		}
	}
	//printf("排序结束\n");
}

/**计算X，Y**/
void calc_xy(double *p_x, double *p_y)
{
	double Z_0 = 2.3;
	//计算X=Ln[Z-Z_0]，Y=Ln[Q]
	for (int i = 0; i < N; i++)
	{
		p_x[i] = log(p_x[i] - Z_0);
		printf("<debug>Ln[%lf]==%lf\n", p_x[i], p_x[i]);
		p_y[i] = log(p_y[i]);
		printf("<debug>Ln[%lf]==%lf\n", p_y[i], p_y[i]);
	}

}
/**计算alpha，beta，P**/
void calc_apb(double (*p_p)[N], double *p_x,double *p_alpha,double *p_beta,int m)
{
	//printf("<debug--apb>  m=%d\tm_1=%d\n", m, );
	//已知P0[0~N-1]=1,先计算alpha[1],然后计算P1[i]，然后计算beta[1],然后计算alpha[2]，然后计算P2[i],然后计算beta[2].....
	for (int i = 0; i < m; i++)
	{
		p_alpha[i] = calc_alpha(p_x, p_p, i);
		printf("<debug--all>  p_alpha[%d]=%lf\n", i, p_alpha[i]);
		/**debug
		for (int i = 0; i < m+1; i++)
		{
			for (int j = 0; j < N; j++)
			{
				printf("<debug--all>  alpha_pp[%d][%d]==%lf\n", i, j, p_p[i][j]);
			}
		}
		**/
		/**
		 * debug
		for (int j = 0; j < m+1; j++)
		{
			printf("<debug--all> alpha[%d]=%lf\n", j, p_alpha[j]);
		}
		**/
		//calc pi
		calc_P(p_x, p_alpha, p_p, p_beta,m+1);

		for (int j = 0; j < m+1; j++)
		{
			for (int k = 0; k < N; k++)
			{
				printf("<debug--all>  p_pp[%d][%d]==%lf\n", j, k, p_p[j][k]);
			}
		}

		p_beta[i + 1] = calc_beta(p_p);
		printf("<debug--all>  i=%d,m=%d\n", i, m);
		/**
		if ((i + 1) == m)
		{
			printf("<debug--all>  enter if\n");
			calc_P(p_x, p_alpha, p_p, p_beta);
		}
		**/
		/**debug
		for (int j = 0; j < m; j++)
		{
			printf("<debug--all>  beta[%d]==%lf\n", j, p_beta[j]);
		}
		for (int j = 0; j < m+1; j++)
		{
			for (int k = 0; k < N; k++)
			{
				printf("<debug--all>  beat_pp[%d][%d]==%lf\n", j, k, p_p[j][k]);
			}
		}
		**/
	printf("<debug--apb> finish calc_apb\n");
		printf("\n");
	}
}



/**
 * 计算Pi
 * 需要Xi，alpha，beta以及P[i-1]
**/
void calc_P(double *p_x,double *p_alpha,double (*p_p)[N],double *p_beta,int flag_p)
{
	/**
	 * flag_p用来控制p_p（包括alpha数组）
	 * 由于计算P[i]时使用的alpha为alpha[i-1],使用的beta为beta[i-2],因此令flag_p初始值为2
	**/
	printf("<debug--p> flag_p==%d\n", flag_p);
	//static int flag_p = 2;
	printf("<debug--p> alpha[%d]=%lf\n", flag_p - 2, p_alpha[flag_p - 2]);
	printf("<debug--p> beta[%d]=%lf\n", flag_p - 2, p_beta[flag_p - 2]);
	for (int j = 0; j < N; j++)
	{
		p_p[flag_p-1][j] = p_p[flag_p - 2][j] * (p_x[j] - p_alpha[flag_p - 2]) - p_beta[flag_p - 2] * p_p[flag_p - 3][j];
		//printf("<debug--p> p_p[%d][%d]==%lf\n", flag_p-1, j, p_p[flag_p-1][j]);
	}
	//flag_p++;
}

/**
 * 计算alpha
 *
**/
double calc_alpha(double *p_x, double (*p_p)[N], int flag_alpha)
{
	/**
	 * 	此函数计算alpha并返回alpha
	 * 	sum_up表示计算式分子，p_p_alpha[N]用来在不影响原始p字符串的情况下拷贝用于计算的一行P
	 * 	flag_alpha用来控制上述的一行p_p_alpha[N],再每次调用此函数后增加一
	**/
	double alpha = 0, sum_up = 0, sum_down = 0, p_p_alpha[N] = {0};
	//static int flag_alpha = 0;     //改进： 传递参数flag_alpha,由上层函数控制
	for (int j = 0; j < N; j++)
	{
		p_p_alpha[j] = p_p[flag_alpha][j];
		//printf("<debug--alpha>  p_p[%d][%d]==%lf\n", flag_alpha, j, p_p[flag_alpha][j]);
	}
	/**计算α[i]**/
	for (int j = 0; j < N; j++)
	{
		sum_up = sum_up + p_x[j] * p_p_alpha[j] * p_p_alpha[j];
		sum_down = sum_down + p_p_alpha[j] * p_p_alpha[j];
	}
	alpha = sum_up / sum_down;
	//printf("<debug--alpha> alpha=%lf\n", alpha);
	//flag_alpha++;
	return alpha;
}

/**
 * 计算beta   需要P
 * beta=sum(Pi^2)/sum(P[i-1]^2)
**/
double calc_beta(double (*p_p)[N])
{
	/**此处用静态变量flag_beta来控制用于计算的两行，每次使用此函数fla_beta增加一
	 * 注：此设计不利于函数的方便使用
	 * 后面改进
	**/
	static int flag_beta = 2;
	double beta = 0, sum_up = 0, sum_down = 0, p_p_beta[2][N] = {0};
	/**
	for (int i = 0; i < m+1; i++)
	{
		for (int j = 0; j < N; j++)
		{
			printf("<debug--beta>  beat_pp[%d][%d]==%lf\n", i, j, p_p[i][j]);
		}
	}
	**/
	/*由于计算beta只需要最新的两行P，因此这里cp最新的两行P*/
	for (int j = 0; j < N; j++)
	{
		p_p_beta[0][j] = p_p[flag_beta-2][j];
		p_p_beta[1][j] = p_p[flag_beta-1][j];
	}
	/**正经计算beta的分子与分母**/
	for (int j = 0; j < N; j++)
	{
		sum_up = sum_up + p_p_beta[1][j] * p_p_beta[1][j];
		sum_down = sum_down + p_p_beta[0][j] * p_p_beta[0][j];
	}
	beta = sum_up / sum_down;
	//printf("<debug--beta> beta=%lf\n", beta);
	flag_beta++;
	return beta;
}

/**
 * 计算系数A[0],A[1],A[2]...
 * 需要P以及Y
**/
double calc_A(double (*p_p)[N], double *p_y, int flag_a)
{
	double A=0, sum_up = 0, sum_down = 0, p_p_A[N] = {0};
	for (int i = 0; i < N; i++)
	{
		p_p_A[i] = p_p[flag_a][i];
	}
	for (int i = 0; i < N;i++)
	{
	sum_up += p_p_A[i] * p_y[i];
	sum_down += p_p_A[i] * p_p_A[i];
	}
	A = sum_up / sum_down;
	flag_a++;
	return A;
}

/**
 * 计算Yc
 * 用到的数据为Pi，A      公式为Y=A[0]P[0][0]+A[1]P[1][0]+A[2]P[2][0]+...
 * 其中j用来指定  A[j]   P[i][j]
**/
double calc_Y_c(double (*p_p)[N], double *p_a, int j,int m)
{
	//printf("<debug--Y_c>  enter Y_c\n");
	double sum_Y_c = 0;
		sum_Y_c = 0;
		for (int i=0 ; i < m + 1; i++)
		{
			//printf("<debug--y_c>  A[%d]=%lf\n", i,p_a[i]);
			//printf("<debug--y_c>  P[%d][%d]=%lf\n",i, j, p_p[i][j]);
			sum_Y_c += p_a[i] * p_p[i][j];
			//printf("<debug--y_c>  sum_Y_c=%lf\n", sum_Y_c);
		}
		return sum_Y_c;
	//printf("<<debug>>Y_c==%lf\n\n",Y_c);
}

/**
 * 计算Qc
 * 用到的数据为Yc，此处传递的i是用来制定计算Q[i]
**/
double calc_Q_c(double *p_Y_c, int i)
{
	double Q_c = 0;
	Q_c = exp(p_Y_c[i]);
	//printf("<debug--Qc>  Q_c=%lf\n", Q_c);
	return Q_c;
}

/**
 * 符号检验
 * 需要Q,Qc
**/
int chk_FH(double *Q, double *Q_c)
{
	int result = 0, flag = 0;
	char accdny = 'Y';
	float k_0 = 0, k_1 = 0, k_min = 0, XZXSP[3] = {0.05, 0.10, 0.25}, XZXSP_tmp = 0;
	double u = 0, u_std[3] = {1.96, 1.64, 1.15};

	for (int i = 0; i < N; i++)
	{
		if (Q[i] > Q_c[i])
	  	{
			k_0++;
	  	} else if (Q[i] < Q_c[i])
	  	{
      		k_1++;
      	} else
	  	{
      		k_0 += 0.5;
     		k_1 += 0.5;
      	}
  	}
	if (k_0<=k_1)
	{
		k_min = k_0;
	}else
	{
		k_min = k_1;
	}
	printf("k_0=%f\tk_1=%f\n", k_0,k_1);
	printf("默认显著性水平α=0.05，是否接受？(接受请输入Y，否则为N)\n");
	scanf("%c", &accdny);
	printf("accdny==%d\n", accdny);
	if (accdny == 'N' || accdny == 'n')
	{
		printf("请输入显著性水平α(0.05or0.10or0.25):\n");
		scanf("%f", &XZXSP_tmp);
		for (int i = 0; i < 3; i++)
		{
			if (XZXSP_tmp==XZXSP[i])
			{
				flag = i;
			}
		}
	}
	printf("<debug--fh>  N==%lf\tk_min==%lf\t\n", (double)N, (double)k_min);
	u = (0.5 * (double)N - (double)k_min - 0.5) / (0.5 * sqrt((double)N));
	printf("u==%lf\tu_std==%f\n", u, u_std[flag]);
	if (u < u_std[flag])
	{
		result = 1;
	}else
	{
		result = 0;
	}
	return result;
}
/**
 * 适线检验
 * 需要Qc
**/
int chk_SX(double *Y, double *Y_c) {
	double r[N - 1] = {0}, u = 0, u_std[2] = {1.64, 1.28};
	float k_0 = 0, k_1 = 0, k_min=0, XZXSP[3] = {0.05, 0.10, 0.25}, XZXSP_tmp = 0;
	int result = 0, flag=0;
	char accdny = 'Y';
	//printf("<debug--sx>  Y[1]=%lf\n", Y[1]);
	for (int i = 0; i < N; i++)
	{
		r[i] = Y[i] - Y_c[i];
		if (i>=1&&r[i]*r[i-1]<0)
		{
			k_1 += 1;
		}else if(i>=1&&r[i]*r[i-1]>0)
		{
			k_0 += 1;
		}
	}
	if (k_0<k_1)
	{
		k_min = k_0;
	}else
	{
		k_min = k_1;
	}
	printf("默认显著性水平α=0.05，是否接受？(接受请输入Y，否则为N)\n");
	scanf("%c", &accdny);
	printf("accdny==%d\n", accdny);
	if (accdny == 'N' || accdny == 'n')
	{
		printf("请输入显著性水平α(0.05or0.10or0.25):\n");
		scanf("%f", &XZXSP_tmp);

		for (int i = 0; i < 3; i++)
		{
			if (XZXSP_tmp==XZXSP[i])
				{
					flag = i;
				}
		}
	}
	printf("<debug--sx>  N==%lf\tk_min==%lf\t\n", (double)N, (double)k_min);
	u = (0.5 * ((double)(N - 1)) - (double)k_min - 0.5) / (0.5 * sqrt((double)(N - 1)));
	printf("u==%lf\tu_std==%f\n", u, u_std[flag]);
	if (u < u_std[flag])
	{
		result = 1;
	}else
	{
		result = 0;
	}
	return result;
}

/**
 * ===此处进行偏离数值检验===
 * t=P_ave/S_p_ave   --->   统计量
 * P_ave=sum_P[i]/N   --->   测点与关系曲线的偏离值的平均值
 * P[i]=(Q[i]-Qc[i])/Qc[i]   --->   测点与曲线的偏离值
 * S_p_ave=S/sqrt(N)=sqrt(sum((Pi-P_ave)^2)/(n*(n-1)))   --->   P_ave的标准差
 * 其中，sum((P[i]-P_ave)^2)有一个简便算法
 * 即   sum((P[i]-P_ave)^2)=sum(P[i]^2)-N*P_ave^2   注：参考 《一种求sum((P[i]-P_ave)^2)的简便算法》，王君，苑洪洁，于汪洋，《黑龙江水专学报》第30卷第4期
 *
**/
int chk_PLSZ(double *Q,double *Qc)
{
	/**此处P[N]为保存P[i]的数组，sum_p为求平均值时的和，sum_P_2为P[i]的平方**/
	double P[N] = {0}, sum_P = 0, P_ave = 0, S_p_ave = 0, sum_P_2 = 0, t = 0;
	/**此处t_std[][]为显著性水平以及自由度的临界值t[1-α/2]表**/
	double t_std[4][11] = { {4.30, 3.18, 2.78, 2.57, 2.45, 2.31, 2.23, 2.13, 2.09, 2.04, 2.00},
							{2.92, 2.35, 2.13, 2.02, 1.94, 1.86, 1.81, 1.75, 1.73, 1.70, 1.67},
							{1.89, 1.89, 1.53, 1.48, 1.44, 1.40, 1.37, 1.34, 1.33, 1.31, 1.30},
							{1.39, 1.39, 1.19, 1.10, 1.13, 1.11, 1.09, 1.07, 1.06, 1.06, 1.05}};
	/**K[11]为上述表的K值，XZXSP[4]为上述表的α值，此处采用‘《基于最小二乘法的绳套型水位流量关系最优定线研究》(董晓华)’一文中提供的相关表**/
	float k[11] = {2, 3, 4, 5, 6, 8, 10, 15, 20, 30, 60}, XZXSP[4] = {0.05, 0.10, 0.20, 0.30}, XZXSP_tmp = 0;
	char accdny = 'Y';//用于判断用户接受or拒绝默认值
	/*flag_alpha为定位上述表行的标志，lag_k为定位上述表列的标志，result为最终判断结果，成功返回1，否则返回0，k_calc为用于计算的K，其值为N-1*/
	int flag_alpha=0, flag_k=0, result = 0, k_calc=0;
	/**
	 * debug
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 11; j++)
		{
			printf("%lf\t", t_std[i][j]);
		}
		printf("\n");
	}
	**/
	printf("默认显著性水平α=0.05，是否接受？(接受请输入Y，否则为N)\n");
	scanf("%c", &accdny);
	//printf("<debug--plsz>  accdny==%d\n", accdny);
	if (accdny == 'N' || accdny == 'n')
	{
			printf("请输入显著性水平α(0.05 or 0.10 or 0.20 or 0.30):\n");
			scanf("%f", &XZXSP_tmp);
			for (int i = 0; i < 4; i++)
			{
				if (XZXSP_tmp==XZXSP[i])
				{
					flag_alpha = i;
				}
				else
				{
					printf("输入显著性水平错误! 请重新输入！\n");
					return 0;
				}
			}

	}
	/*计算P[i]*/
	for (int i = 0; i < N; i++)
	{
		P[i] = (Q[i] - Qc[i]) / Qc[i];
		printf("<debug--plsz>  P[%d]==%lf\t", i, P[i]);
		sum_P += P[i];
		printf("<debug--plsz>  sum_P==%lf\n", sum_P);
		sum_P_2 += P[i] * P[i];
	}
	P_ave = sum_P / N;
	S_p_ave = sum_P_2 - N * P_ave * P_ave;
	t = P_ave / S_p_ave;
	/*取绝对值*/
	if (t<0)
	{
		t = -t;
	}
	printf("<debug--plsz>  t=%lf\n", t);
	k_calc = N - 1;
	/*计算并比较t与t_std*/
	for (int i = 0; i < 11; i++)
	{
		//由于上表前五列为连续K，因此不用参与else里的计算，此处yongk_calc判断
		if (k_calc <= 6 && k_calc==k[i])
		{
			flag_k = i;
			//判断结果
			if (t<t_std[flag_alpha][flag_k])
			{
				result = 1;
			}
		//若所得k_calc在表中没有直接对应的数值，则采用按权重分配相邻两数据之间的差值的办法
		}else if (k_calc > 6 && k_calc >= k[i] && k_calc <= k[i+1])
		{
			int n = 0;
			n = k[i + 1] - k[i];
			float t_std_calc = 0;
			t_std_calc = t_std[flag_alpha][i] + ((k_calc-k[i])/(n)) * (t_std[flag_alpha][i + 1] - t_std[flag_alpha][i]);
			//判断结果
			if (t<t_std_calc)
			{
				result = 1;
			}
		}
	}
	return result;
}
//冗余代码备份
/**
 *
 *
int main() {
  double Q[N] = {2.41, 2.63, 2.99}, Q_c[N] = {2.401, 2.633, 3.0};
  double res_FH[N - 1] = {0};
  int result_FH = 0, result_SX = 0, result_PLSZ = 0, result_T = 0;
  printf("To be a good coder.\n");
  result_FH = chk_FH(Q, Q_c);
  result_SX = chk_SX();
  return 0;
}




 *
//function sum
double sum(double *num, int n)
{
	double sum = 0;
	for (int i = 0; i < n; i++)
	{
		sum = sum + num[i];
	}
	return sum;
}
**/
/**计算Pi**/
//P0=1，计算Pi=(X-αi)*P(i-1)-β(i-1)*P(i-2),需要Xi，αi=sum(Xi*Pii^2)/sum(Pii^2)，βj=sum(Pji^2)/sum(P(j-1)i^2
/**
 *
void calc_P(double *p_x, double (*p_p)[N], double alpha[],double beta[])
{
	double p_p_p_a[N] = {0}, p_p_p_b_1[N] = {0}, p_p_p_b_2[N] = {0};
	printf("enter calc_P\n");
	for (int i = 0, ai = 0, bi = 0; i < m; i++, ai++, bi++)
	{
		**
		 *	将p_p的每一行分别赋值给ppp数组，以此计算alpha
		 * 《《《《《如何以指针的形式，用指针的偏移来代替循环》》》》》


		for (int j = 0; j < N; j++)
		{
			//printf("<debug> pp[%d][%d]=%lf\n", i, j, p_p[i][j]);
			p_p_p_a[j] = p_p[i][j];
			p_p_p_b_1[j] = p_p[i][j];
			p_p_p_b_2[j] = p_p[i+1][j];
			//printf("<debug> *PPP=%lf\n", p_p_p_a[j]);
		}
		**
		 * 计算alpha
		**/
		//alpha[ai] = calc_alpha(p_x, p_p_p_a);
		//printf("<debug>alpha[%d]=%lf\n", i, alpha[i]);

		/**
		 * 计算P[i]
		**
		alpha[0] = -0.164687, alpha[1] = -0.404995;
		for (int j = 0; j < N; j++)
		{
			p_p[i + 1][j] = (p_x[j] - alpha[ai])* p_p[i][j] - beta[i] * p_p[i - 1][j];
			//printf("<debug>p_p[%d][%d]=%lf\n", i+1, j, p_p[i+1][j]);
		}
		**
		 * 计算beta
		**
		beta[bi+1] = calc_beta(p_p_p_b_2,p_p_p_b_1);
		for (int i = 0; i < N; i++)
		{
			printf("<debug>  pppb _ 2[%d]=%lf\n", i, p_p_p_b_2[i]);
			printf("<debug>  pppb _ 1[%d]=%lf\n", i, p_p_p_b_1[i]);
		}
		printf("<debug>beta[%d]=%lf\n", bi+1, beta[bi+1]);
		printf("\n");

	}
}
**/
