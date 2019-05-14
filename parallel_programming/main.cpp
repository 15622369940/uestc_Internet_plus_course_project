<<<<<<< HEAD
#include "mpi.h"
#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "error.h"

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

#define BLOCK_LOW(id, p, n) ((id) * (n) / (p))
#define BLOCK_HIGH(id, p, n) (BLOCK_LOW((id) + 1, p, n) - 1)
#define BLOCK_SIZE(id, p, n) (BLOCK_LOW((id) + 1) - BLOCK_LOW(id))
#define BLCOK_OWNER(index, p, n) (((p)* (index) +1 ) -1 / (n))
#define MIN(a, b) ((a)<(b)?(a):(b))

int main(int argc, char *argv[]) {
    int count;        /* Local prime count */
    double elapsed_time; /* Parallel execution time */
    int first;        /* Index of first multiple */
    int global_count; /* Global prime count */
    int high_value;   /* Highest value on this proc */
    int id;           /* Process ID number */
    int index;        /* Index of current prime */
    int low_value;    /* Lowest value on this proc */
    char *marked;       /* Portion of 2,...,'n' */
    int n;            /* Sieving from 2, ..., 'n' */
    int p;            /* Number of processes */
    int proc0_size;   /* Size of proc 0's subarray */
    int prime;        /* Current prime */
    int size;         /* Elements in 'marked' */

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
//    low_value = 2 + id * (n - 1) / p;//���̵ĵ�һ����
//    high_value = 1 + (id + 1) * (n - 1) / p;//���̵����һ����
//    size = high_value - low_value + 1;    //���̴���������С

    int N = n - 1;
    low_value = 2 + id * (N / p) + MIN(id, N % p);//���̵ĵ�һ����
    high_value = 2 + (id + 1) * (N / p) + MIN(id + 1, N % p) - 1;//���̵����һ����
    size = high_value - low_value + 1;    //���̴���������С


    // Bail out if all the primes used for sieving are not all held by process 0
    proc0_size = (n - 1) / p;

    // �����̫�����
    if ((2 + proc0_size) < (int) sqrt((double) n)) {
        if (!id) printf("Too many processes \n");
        MPI_Finalize();
        exit(1);
    }

    // allocate this process 's share of the array
    marked = (char *) malloc(size);
    if (marked == nullptr) {
        printf("Cannot allocate enough memory \n");
        MPI_Finalize();
        exit(1);
    }

    // �ȼٶ����е�������������
    for (int i = 0; i < size; i++) marked[i] = 0;

    // ������ʼ��Ϊ0
    if (!id) index = 0;

    // ��2��ʼ��Ѱ
    prime = 2;
    do {
        /*ȷ���ý����������ĵ�һ���������±� */
        // ���������n*n>low_value��n*(n-i)���������
        // ��n*nΪ�ý����еĵ�һ������
        // ���±�Ϊn*n-low_value
        if (prime * prime > low_value) {
            first = prime * prime - low_value;
        } else {
            // ����Сֵlow_valueΪ�������ı���
            // ���һ������Ϊlow_value�������±�Ϊ0
            if (!(low_value % prime)) first = 0;
                // ����Сֵlow_value���Ǹ������ı���
                // ��ô��һ���������±�Ϊ��������ȥ������ֵ
            else first = prime - (low_value % prime);
        }

        // �ӵ�һ��������ʼ����Ǹ������ı���Ϊ������
        for (int i = first; i < size; i += prime) marked[i] = 1;

        // ֻ��id=0�Ľ��̲ŵ��ã������ҵ���һ������λ��
        if (!id) {
            while (marked[++index]);
            prime = index + 2;
        }

        // ֻ��id=0�Ľ��̲ŵ��ã����ڽ���һ�������㲥��ȥ
        if (p > 1) {
            MPI_Bcast(&prime, 1, MPI_INT, 0, MPI_COMM_WORLD);
        }

    } while (prime * prime <= n);

    // ����ǽ������0�Ž���
//    printf("id: %d, low: %d, high: %d, size: %d\n", id, low_value, high_value, size);
    count = 0;
    for (int i = 0; i < size; i++)
        if (marked[i] == 0) {
            count++;
//            printf("%d ", i + low_value);
        }
//    printf("\n");
    if (p > 1) MPI_Reduce(&count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // stop the timer
    elapsed_time += MPI_Wtime();

    // print the results
    if (!id) {
        printf("%d primes are less than or equal to %d \n", global_count, n);
        printf("Total elapsed time: %10.6f\n", elapsed_time);
    }
=======
// #include <mpi.h>
// #include <math.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>

// /************************************************
// MPI_BCAST(buffer,count,datatype,root,comm)
//     IN/OUT��buffer����  ͨ����Ϣ����������ʼ��ַ(�ɱ�)
//     IN������ count��  �� ͨ����Ϣ�������е����ݸ���(����)
//     IN ������datatype ��ͨ����Ϣ�������е���������(���)
//     IN������ root��  �������͹㲥�ĸ������к�(����)
//     IN ������comm   ����ͨ����(���)
// int MPI_Bcast(void* buffer,int count,MPI_Datatype datatype,int root, MPI_Comm comm)

// MPI_BCAST�Ǵ�һ�����к�Ϊroot�Ľ��̽�һ����Ϣ�㲥���͵����ڵ����н���,
// ��������������.����ʱ�������г�Ա��ʹ��ͬһ��comm��root,
// �����ǽ�����ͨ����Ϣ�������е���Ϣ�������������н�����ȥ.
// **********************************************/

// #define BLOCK_LOW(id, p, n) ((id) * (n) / (p))
// #define BLOCK_HIGH(id, p, n) (BLOCK_LOW((id) + 1, p, n) - 1)
// #define BLOCK_SIZE(id, p, n) (BLOCK_LOW((id) + 1) - BLOCK_LOW(id))
// #define BLCOK_OWNER(index, p, n) (((p)* (index) +1 ) -1 / (n))
// #define MIN(a, b) ((a)<(b)?(a):(b))

// int main(int argc, char *argv[]) {
//     int count;        /* Local prime count */
//     double elapsed_time; /* Parallel execution time */
//     int first;        /* Index of first multiple */
//     int global_count; /* Global prime count */
//     int high_value;   /* Highest value on this proc */
//     int id;           /* Process ID number */
//     int index;        /* Index of current prime */
//     int low_value;    /* Lowest value on this proc */
//     char *marked;       /* Portion of 2,...,'n' */
//     int n, m;            /* Sieving from 2, ..., 'n' */
//     int p;            /* Number of processes */
//     int proc0_size;   /* Size of proc 0's subarray */
//     int prime;        /* Current prime */
//     int size;         /* Elements in 'marked' */

//     // ��ʼ��
//     // MPI��������ʱ���Զ�����������ͨ������
//     // MPI_COMM_WORLD:��������������MPI����
//     // MPI_COMM_SELF���е������̶��Թ��ɣ��������Լ�
//     MPI_Init(&argc, &argv);

//     // MPI_Barrier��MPI�е�һ�������ӿ�
//     // ��ʾ��ֹ����ֱ��communicator�����н�����ɵ���
//     MPI_Barrier(MPI_COMM_WORLD);

//     // MPI_COMM_RANK �õ������̵Ľ��̺ţ����̺�ȡֵ��ΧΪ 0, ��, np-1
//     MPI_Comm_rank(MPI_COMM_WORLD, &id);

//     // MPI_COMM_SIZE �õ����вμ�����Ľ��̵ĸ���
//     MPI_Comm_size(MPI_COMM_WORLD, &p);

//     // MPI_WTIME����һ���ø�������ʾ������
//     // ����ʾ�ӹ�ȥĳһʱ�̵�����ʱ����������ʱ��
//     elapsed_time = -MPI_Wtime();

//     // ��������Ϊ2���ļ����Լ������ģn
//     if (argc != 2) {
//         if (!id) printf("Command line: %s <m> \n", argv[0]);
//         // ����MPIϵͳ
//         MPI_Finalize();
//         exit(1);
//     }

//     // atoi(��ʾ ascii to integer)�ǰ��ַ���ת������������һ������
//     n = atoi(argv[1]);


//     low_value = 3 + 2 * (id * (n) / p);//���̵ĵ�һ����
//     high_value = 3 + 2 * ((id + 1) * (n) / p - 1);//���̵����һ����
//     size = (high_value - low_value) / 2 + 1;    //���̴����������С

//     marked = (char *) malloc(size);
//     if (marked == NULL) {
//         printf("Cannot allocate enough memory \n");
//         MPI_Finalize();
//         exit(1);
//     }


//     for (int i = 0; i < size; i++) {
//         marked[i] = 0;
//     }
//     if (!id) index = 0;
//     prime = 2;
//     do {
//         if (prime * prime > low_value) {
//             first = prime * prime - low_value;
//         } else {
//             if (!(low_value % prime)) first = 0;
//             else first = prime - (low_value % prime);
//         }
//         for (int i = first; i < size; i += prime) marked[i] = i;
//         if (!id) {
//             while (marked[++index]); /* find the smallest unmarked */
//             prime = index + 2;
//         }
//         MPI_Bcast(&prime, 1, MPI_INT, 0, MPI_COMM_WORLD);
//     } while (prime * prime <= n);

//     count = 0;
//     for (int i = 0; i < size; i++)
//         if (!marked[i]) count++;
//     MPI_Reduce(&count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
//     elapsed_time += MPI_Wtime();
//     if (!id) {
//         printf("%d primes are less than or equal to %d \n", global_count, n);
//         printf("Total elapsed time: %10.f\n", elapsed_time);
//     }
//     MPI_Finalize();
//     return 0;
// }


#include <stdint.h>
#include <iostream>
#include <mpi.h>
#include <stdint.h>
using namespace std;

int main(int argc,char* argv[]){
    cout<<"hello"<<endl;
    int myid, numprocs;
    int  namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Get_processor_name(processor_name,&namelen); 
    std::cout<<"Hello World! Process"<<myid<<"of"<<numprocs<<" on "<<processor_name<<std::endl;
>>>>>>> 7204af4e94cc719eed7ef6963f1c0c2d760f8b31
    MPI_Finalize();

    // ��׷�ӵķ�ʽ���ļ�
//    char str1[30] = "../output/record.init.";
//    char str2[10] = ".txt";
//    char filename[50];
//    sprintf(filename, "%s%d%s", str1, p, str2);
//    FILE *fp;
//    if ((fp = fopen(filename,"a+")) == nullptr){
//        printf("fail to open file");
//        exit(0);
//    }
//    fprintf(fp, "%d %d %10.6f\n", p, n, elapsed_time);
//    fclose(fp);
    return 0;
}
