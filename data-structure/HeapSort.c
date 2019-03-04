//
// Created by PC on 2018/11/1.
// ������ ����
//Traversing
#include <stdio.h>
#include <stdlib.h>

#define DataType int

void Traversing(int array[], int size);

void HeapSort(int array[], int size);

void HeapShift(int array[], int size, int start);

void swap(int *a, int *b);

int main() {
    int size = 9;
    DataType A[9] = {24, 58, 17, 79, 27, 30, 8, 80, 12};
    Traversing(A, size);
    HeapSort(A, size);

    Traversing(A, size);
    return 0;
}

// ��0��ʼ���
void HeapSort(int array[], int size) {
    // ������ʼ��
    for (int i = size / 2 - 1; i >= 0; i--) {
        HeapShift(array, size - 1, i);
    }
    Traversing(array, size);
    for (int j = size - 1; j > 0; j--) {
        // �Ѷ�Ԫ�غͶ��е����һ��Ԫ�ؽ���
        swap(&array[0], &array[j]);
        // ���µ����ṹ��ʹ���������Ѷ���
        HeapShift(array, j - 1, 0);
    }
}

void HeapShift(int array[], int size, int start) {
    int dad = start; // ���׽ڵ�
    int son = 2 * dad + 1; // ���ӽڵ�
    while (son <= size) {
        // ���ӽڵ�����ֵ
        if (son + 1 <= size && array[son] < array[son + 1]) {
            son++;
        }
        if (array[dad] > array[son]) {
            return;
        } else {
            swap(&array[dad], &array[son]);


            // �鿴�����Ƿ����Ӹ��ṹ���ң����ǣ������µ�����
            dad = son;
            son = 2 * dad + 1;
        }
    }
}

void swap(int *a, int *b) {
    *a = *a + *b;
    *b = *a - *b;
    *a = *a - *b;
}

void Traversing(int array[], int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");
}