#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

/************************************************
MPI_BCAST(buffer,count,datatype,root,comm)
IN/OUT��buffer����  ͨ����Ϣ����������ʼ��ַ(�ɱ�)
IN������ count��  �� ͨ����Ϣ�������е����ݸ���(����)
IN ������datatype ��ͨ����Ϣ�������е���������(���)
IN������ root��  �������͹㲥�ĸ������к�(����)
IN ������comm   ����ͨ����(���)
int MPI_Bcast(void* buffer,int count,MPI_Datatype datatype,int root, MPI_Comm comm)

MPI_BCAST�Ǵ�һ�����к�Ϊroot�Ľ��̽�һ����Ϣ�㲥���͵����ڵ����н���,
��������������.����ʱ�������г�Ա��ʹ��ͬһ��comm��root,
�����ǽ�����ͨ����Ϣ�������е���Ϣ�������������н�����ȥ.

��Լ���� MPI_Reduce()����ͨ�����ڸ����̵�ͬһ�����������Լ���㣬����ָ���Ľ������������
MPI_METHOD MPI_Reduce(
_In_range_(!= , recvbuf) _In_opt_ const void* sendbuf,  // ָ���������ݵ�ָ��
_When_(root != MPI_PROC_NULL, _Out_opt_) void* recvbuf, // ָ��������ݵ�ָ�룬����������ŵĵط�
_In_range_(>= , 0) int count,                           // ���ݳߴ磬���Խ��ж���������������Ĺ�Լ
_In_ MPI_Datatype datatype,                             // ��������
_In_ MPI_Op op,                                         // ��Լ��������
_mpi_coll_rank_(root) int root,                         // Ŀ����̺ţ���ż������Ľ���
_In_ MPI_Comm comm                                      // ͨ����
);
**********************************************/

#define MIN(a, b) ((a)<(b)?(a):(b))

int main(int argc, char *argv[]) {
	int count;              /* Local prime count */
	double elapsed_time;    /* Parallel execution time */
	int first;              /* Index of first multiple */
	int global_count;       /* Global prime count */
	char *marked;           /* Portion of 2,...,'n' */
	int id;                 /* Process ID number */
	int index;              /* Index of current prime */
	int low_index;          /* Lowest index on this proc */
	int low_value;          /* Lowest value on this proc */
	int high_index;         /* Highest index on this proc */
	int high_value;         /* Highest value on this proc */
	int n;                  /* Sieving from 2, ..., 'n' */
	int p;                  /* Number of processes */
	int proc0_size;         /* Size of proc 0's subarray */
	int prime;              /* Current prime */
	int size;               /* Elements in 'marked' */
	int sub_size;           /* size of sub_array */
	char *sub_marked;       /* sub_mark array */
	int sub_low_index;      /* Lowest index on sub_array */
	int sub_low_value;      /* Lowest array on sub_array */
	int sub_high_index;     /* Highest index on sub_array */
	int sub_high_value;     /* Highest index on sub_array */

	// ��ʼ��
	// MPI��������ʱ���Զ�����������ͨ������
	// MPI_COMM_WORLD:��������������MPI����
	// MPI_COMM_SELF���е������̶��Թ��ɣ��������Լ�
	MPI_Init(&argc, &argv);

	// MPI_COMM_RANK �õ������̵Ľ��̺ţ����̺�ȡֵ��ΧΪ 0, ��, np-1
	MPI_Comm_rank(MPI_COMM_WORLD, &id);

	// MPI_COMM_SIZE �õ����вμ�����Ľ��̵ĸ���
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	// MPI_Barrier��MPI�е�һ�������ӿ�
	// ��ʾ��ֹ����ֱ��communicator�����н�����ɵ���
	MPI_Barrier(MPI_COMM_WORLD);

	// MPI_WTIME����һ���ø�������ʾ������
	// ����ʾ�ӹ�ȥĳһʱ�̵�����ʱ����������ʱ��

	elapsed_time = -MPI_Wtime();

	// ��������Ϊ2���ļ����Լ������ģn
	if (argc != 2) {
		if (!id) printf("Command line: %s <m> \n", argv[0]);
		// ����MPIϵͳ
		MPI_Finalize();
		exit(1);
	}

	// ��ʾ�� <= n������
	n = atoi(argv[1]);

	// Bail out if all the primes used for sieving are not all held by process 0
	proc0_size = (n - 1) / p;

	// �����̫�����
	if ((2 + proc0_size) < (int)sqrt((double)n)) {
		if (!id) printf("Too many processes \n");
		MPI_Finalize();
		exit(1);
	}

	/*
	* ����ǰsqrt(n)�ڵ�����
	*/
	int sub_n = (int)sqrt((double)n);
	int sub_N = (sub_n - 1) / 2;

	sub_low_index = 0 * (sub_N / p) + MIN(0, sub_N % p); // ���̵ĵ�һ����������
	sub_high_index = 1 * (sub_N / p) + MIN(1, sub_N % p) - 1; // ���̵����һ����������
	sub_low_value = sub_low_index * 2 + 3; //���̵ĵ�һ����
	sub_high_value = (sub_high_index + 1) * 2 + 1;//���̵����һ����
	sub_size = (sub_high_value - sub_low_value) / 2 + 1;    //���̴���������С

	// Bail out if all the primes used for sieving are not all held by process 0
	proc0_size = (sub_n - 1) / p;

	// �����̫�����
	if ((2 + proc0_size) < (int)sqrt((double)sub_n)) {
		if (!id) printf("Too many processes \n");
		MPI_Finalize();
		exit(1);
	}

	sub_marked = (char *)malloc(sub_n);
	if (sub_marked == nullptr) {
		printf("Cannot allocate enough memory \n");
		MPI_Finalize();
		exit(1);
	}

	// �ȼٶ����е�������������
	for (int i = 0; i < sub_size; i++) sub_marked[i] = 0;

	// ������ʼ��Ϊ0
	index = 0;

	prime = 3;
	do {
		// ��С���鿪ʼ��ֻ�����е�һ������
		first = (prime * prime - sub_low_value) / 2;
		// �ӵ�һ��������ʼ����Ǹ������ı���Ϊ������
		for (int i = first; i < sub_size; i += prime) {
			sub_marked[i] = 1;

		}
		while (sub_marked[++index]);
		prime = index * 2 + 3; // ��ʼ��ƫ��
	} while (prime * prime <= sub_n);

	int N = (n - 1) / 2;
	low_index = id * (N / p) + MIN(id, N % p); // ���̵ĵ�һ����������
	high_index = (id + 1) * (N / p) + MIN(id + 1, N % p) - 1; // ���̵����һ����������
	low_value = low_index * 2 + 3; //���̵ĵ�һ����
	high_value = (high_index + 1) * 2 + 1;//���̵����һ����
	size = (high_value - low_value) / 2 + 1;    //���̴���������С

	// allocate this process 's share of the array
	marked = (char *)malloc(size);
	if (marked == nullptr) {
		printf("Cannot allocate enough memory \n");
		MPI_Finalize();
		exit(1);
	}

	// �ȼٶ����е�������������
	for (int i = 0; i < size; i++) marked[i] = 0;

	// ������ʼ��Ϊ0
	index = 0;

	// ��3��ʼ��Ѱ��firstΪ��һ������������λ��
	prime = 3;
	do {
		/*ȷ���ý����������ĵ�һ���������±� */
		// ���������n*n>low_value��n*(n-i)���������
		// ��n*nΪ�ý����еĵ�һ������
		// ���±�Ϊn*n-low_value���������������С�������Գ���2
		if (prime * prime > low_value) {
			first = (prime * prime - low_value) / 2;
		}
		else {
			// ����Сֵlow_valueΪ�������ı���
			// ���һ������Ϊlow_value�������±�Ϊ0
			if (!(low_value % prime)) first = 0;
			// ����Сֵlow_value���Ǹ������ı���
			// ����������Ϊż������ô��һ��������������Ϊ��������ȥ�������2
			else if (low_value % prime % 2 == 0) first = prime - ((low_value % prime) / 2);
			// ����Сֵlow_value���Ǹ������ı���
			// ��ô��һ���������±�Ϊ��������ȥ������ֵ���������������С�������Գ���2
			else first = (prime - (low_value % prime)) / 2;
		}

		// �ӵ�һ��������ʼ����Ǹ������ı���Ϊ������
		for (int i = first; i < size; i += prime) marked[i] = 1;

		while (sub_marked[++index]);
		prime = index * 2 + 3;
		

	} while (prime * prime <= n);

	// ����ǽ������0�Ž���
	count = 0;
	for (int i = 0; i < size; i++)
	if (marked[i] == 0) {
		count++;
	}
	printf("id: %d, low: %d, high: %d, size: %d count: %d\n", id, low_value, high_value, size, count);

	MPI_Reduce(&count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	// stop the timer
	elapsed_time += MPI_Wtime();
	MPI_Finalize();

	// print the results
	if (!id) {
		printf("%d primes are less than or equal to %d \n", global_count, n);
		printf("Total elapsed time: %10.6f\n", elapsed_time);

		// ��׷�ӵķ�ʽ���ļ�
		char str1[40] = "./output/record.cancel_Bcast.";
		char str2[10] = ".txt";
		char filename[50];
		sprintf_s(filename, "%s%d%s", str1, p, str2);
		FILE *fp;
		int err = fopen_s(&fp, filename, "a+");
		if (err == 0) {
			fprintf(fp, "%d %d %10.6f\n", p, n, elapsed_time);
			fclose(fp);
		}
		else{
			printf("fail to open file");
			exit(0);
		}
	}


	return 0;
}
