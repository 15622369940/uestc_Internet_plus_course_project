//
// Created by PC on 2018/11/3.
// ��Դ���·��Dijkestra�㷨
//

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define VEX_NUM 6  // �ڵ����
#define MAXINT 100000 // ���������
#define DataType int

typedef int dataType;

typedef struct graph {
    char vexs[VEX_NUM]; // ��������VEX_NUMΪ������Ŀ
    dataType arcs[VEX_NUM][VEX_NUM];  // �ڽӾ���
} Mgraph, *MgraphP;


void graphInit(MgraphP *graph, int row, int col); // ��ʼ���ڽӾ���

MgraphP graphCreate();  // ��ʼ��ͼ

void Traversing(dataType *array, int row, int col); // ����

void Dijkstra(MgraphP graph, int v0, int *path, dataType *dist); // ��Դ���·��

void printPath(int v0, int *path, int *dist); // ��ӡ�ýڵ㵽���ڵ�����·��

void createDisFile(MgraphP *graph, int row, int col);

void createPathFile(MgraphP *graph, int v0, int *path, dataType *dist);

int main() {
    MgraphP graph = graphCreate();
    printf("Init:\n");

    graphInit(&graph, VEX_NUM, VEX_NUM);
    createDisFile(&graph, VEX_NUM, VEX_NUM);

    Traversing(&(graph->arcs[0][0]), VEX_NUM, VEX_NUM);

    int path[VEX_NUM]; // �������·����ǰ��
    int dist[VEX_NUM]; // �������·������
    int v0 = 4; // ��ʼ�ڵ�

    Dijkstra(graph, v0, path, dist);

    printPath(v0, path, dist);
    createPathFile(&graph, v0, path, dist);

    return 0;
}

// ���ӻ�����
void createPathFile(MgraphP *graph, int v0, int *path, dataType *dist) {
    for (int i = 0; i < VEX_NUM; i++) {
        if (v0 != i) {
            // �����ļ�
            char *filename = (char *) malloc(100);
            sprintf(filename, "./dotFile/v%d_to_v%d.dot", v0, i);
            FILE *fp = fopen(filename, "w"); // �ļ�ָ��
            if (fp == NULL) {   // ΪNULL�򷵻�
                printf("File cannot open!");
                exit(0);
            }
            char *title = (char *) malloc(100);
            sprintf(title, "v%d -> v%d", v0, i);    // �ļ���
            fprintf(fp, "digraph G {\n");   // ��ͷ

            // ����ԭʼ���ͼ
            for (int j = 0; j < VEX_NUM; j++) {
                fprintf(fp, "v%d[shape=ellipse];\n", j);
                for (int k = 0; k < VEX_NUM; k++) {
                    if ((*graph)->arcs[j][k] < MAXINT) {
                        fprintf(fp, "v%d->v%d[label=\"%d\"];\n", j, k, (*graph)->arcs[j][k]);
                    }
                }
            }

            fprintf(fp, "graph[label=\"%s, minDist: %d\"];\n", title, dist[i]); // �������ɣ���¼��̾���
            fprintf(fp, "edge[labelfontcolor=red,fontcolor=red,color=red];\n");
            int temp = path[i];
            // ��ʼ·��
            fprintf(fp, "v%d[color=red];\n", i);
            fprintf(fp, "v%d->v%d[color=red,label=\"%d\"];\n", temp, i, (*graph)->arcs[temp][i]);
            // ����ǰ��·�������
            while (v0 != temp) {
                fprintf(fp, "v%d[color=red];\n", temp);
                fprintf(fp, "v%d->v%d[label=\"%d\"];\n", path[temp], temp,
                        (*graph)->arcs[path[temp]][temp]);
                temp = path[temp];
            }
            // ���սڵ���
            fprintf(fp, "v%d[color=red];\n", v0);
            fprintf(fp, "}\n"); // ��β
            fclose(fp); // �ر�IO
            char *order = (char *) malloc(100);
            sprintf(order, "dot -Tpng ./dotFile/v%d_to_v%d.dot -o ./dotFile/v%d_to_v%d.png", v0, i, v0, i);
            system(order);
        }
    }
}

// ��ӡ�ýڵ㵽���ڵ�����·��
void printPath(int v0, int *path, int *dist) {
    for (int i = 0; i < VEX_NUM; i++) {
        // ����ӡ�ýڵ㵽���ڵ�����·��
        if (v0 != i) {
            printf("v%d -> v%d, minDist: %d, path: v%d <- ", v0, i, dist[i], i);
            int temp = path[i];
            // ����ýڵ��ǰ��������Դ�ڵ㣬������ݹ����
            while (v0 != temp) {
                printf("v%d <- ", temp);
                // ��¼ǰ��
                temp = path[temp];
            }
            printf("v%d", v0);
            printf("\n");
        }
    }
}

// ��Դ���·��
void Dijkstra(MgraphP graph, int v0, int *path, dataType *dist) {
    for (int i = 0; i < VEX_NUM; i++) {
        // ��ʼ������
        dist[i] = graph->arcs[v0][i];
        // ������ɴ��ǰ����Ϊ-1
        if (dist[i] < MAXINT) { // ����ɴ��ǰ����Ϊ�õ�Դ��ʼ��
            path[i] = v0;
        } else {  // ������ɴ��ǰ����Ϊ-1
            path[i] = -1;
        }
    }

    int s[VEX_NUM] = {0}; // ��¼û�б������Ľڵ㣬��ʼΪ0
    dist[v0] = 0; // �ڵ㵽�����·������Ϊ0
    s[v0] = 1; // �ýڵ��Ѿ�������ɣ���Ϊ1

    for (int i = 1; i < VEX_NUM; i++) { // ѭ��ʣ��n-1���ڵ�
        int v = -1; // ��¼���λ�ã���ʼ��Ϊ-1
        int min = MAXINT; // ��¼�����Сֵ����ʼ��Ϊ���ֵ
        for (int j = 0; j < VEX_NUM; j++) {
            if (s[j] != 1 && dist[j] < min) { // �ýڵ�δ�����������Ҹ�ֵ��С
                min = dist[j]; // ��¼��ֵ
                v = j;  // ��¼�õ�λ��
            }
        }

        // ��������ڸ�ֵ��������
        if (v == -1) {
            break;
        }

        // �����������
        s[v] = 1; // ��ʾ�õ��Ѿ�����
        for (int j = 0; j < VEX_NUM; j++) {
            // ����õ�û�����������Ҹ��º��·����ԭ·���̣����滻
            if (s[j] != 1 && (min + graph->arcs[v][j] < dist[j])) {
                dist[j] = min + graph->arcs[v][j];
                path[j] = v;
            }
        }
    }
}

// ����
void Traversing(dataType *array, int row, int col) {
    for (int i = 0; i < row; i++) {
        printf("v%d          ", i);
        for (int j = 0; j < col; j++) {
            printf("%d ", *(array + i * col + j));
        }
        printf("\n");
    }
}

// ��ʼ��ͼ
MgraphP graphCreate() {
    Mgraph *graph = (Mgraph *) malloc(sizeof(Mgraph)); // ����ڵ�ռ�
    if (!graph) { // �ж��Ƿ����㹻���ڴ�ռ�
        printf("�����ڴ�ռ�ʧ�ܣ�\n");
    }
    return graph;
}

// ��ʼ���ڽӾ���
void graphInit(MgraphP *graph, int row, int col) {
    int size = 100;
    DataType A[size];
    // ��ȡ�ļ����ݣ�����������A��
    FILE *fp = fopen("./data/distance.txt", "r+");
    if (fp == NULL)
        exit(0x01);
    DataType keynumber;
    for (int i = 0; i < size; i++) {
        fscanf(fp, "%d", &keynumber);
        A[i] = keynumber;
    }
    fclose(fp);
    // ����ȡ�����ݸ�ֵ��graph��
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            (*graph)->arcs[i][j] = A[i * 10 + j];
        }
    }
}

// �������ͼ
void createDisFile(MgraphP *graph, int row, int col) {
    FILE *fp = fopen("./dotFile/Dijkestra_Dis.dot", "w");
    if (fp == NULL) {
        printf("File cannot open!");
        exit(0);
    }
    fprintf(fp, "digraph G {\n");
    for (int i = 0; i < row; i++) {
        fprintf(fp, "v%d[shape=ellipse];\n", i);
        for (int j = 0; j < col; j++) {
            if ((*graph)->arcs[i][j] < MAXINT) {
                fprintf(fp, "v%d->v%d\n[label=\"%d\"];\n", i, j, (*graph)->arcs[i][j]);
            }
        }
    }
    fprintf(fp, "}\n");
    fclose(fp);
    system("dot -Tpng ./dotFile/Dijkestra_Dis.dot -o ./dotFile/Dijkestra_Dis.png");
}

