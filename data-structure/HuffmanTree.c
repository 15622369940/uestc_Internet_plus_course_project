//
// Created by PC on 2018/11/1.
// �Լ�д�ģ��õ������߼��ṹ
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXNUM 10000

const int leftCode = 0;
const int rightCode = 1;

typedef struct HTNode {
    int character; // �ýڵ���ַ�
    int weight; // Ȩ��
    struct HTNode *lchild, *rchild;
} HTNode, *HTNodeP;

typedef struct Hcode {
    int bit;
} Hcode;

void CreateHuffmanTree(HTNodeP *TreeRoot, struct HTNode array[], int size);

int findMin(struct HTNode array[], int size);

HTNodeP initHNode(int weight, int character);

void initCounter(struct HTNode array[], const int size);

int getMinWPL(HTNodeP *TreeRoot, int depth);

void HuffmanCoding(HTNodeP TreeRoot, struct Hcode codeArray[], int code);

int main() {
    HTNodeP HFTree = NULL;
    char string[100] = "AADBBccEEEEEE";
    const int size = 26;
    HTNode counter[size];
    initCounter(counter, size);

    // ֻͳ��A-Z�Ĵ�Сд�ַ�����
    printf("����ǰ�ַ���");
    for (int j = 0; string[j] != '\0'; j++) {
        int asciiNum = toascii(string[j]);
        if (asciiNum >= 97 && asciiNum <= 122) { // A��ascii��Ϊ97��Z��ascii��Ϊ122
            asciiNum -= 32;
        }
        asciiNum -= 65; // a��ascii��Ϊ65
        counter[asciiNum].weight += 1;
        printf("%c", string[j]);
    }

    CreateHuffmanTree(&HFTree, counter, size);

    int minWPL = getMinWPL(&HFTree, 0);

    printf("\nminWPL is: %d", minWPL);

    Hcode codeArray[size];
    int code = 0;
    HuffmanCoding(HFTree, codeArray, code);
    printf("\n������ַ���");

    // ת��
    for (int j = 0; string[j] != '\0'; j++) {
        int asciiNum = toascii(string[j]);
        if (asciiNum >= 97 && asciiNum <= 122) { // A��ascii��Ϊ97��Z��ascii��Ϊ122
            asciiNum -= 32;
        }
        asciiNum -= 65; // a��ascii��Ϊ65
        printf("%d ", codeArray[asciiNum].bit);
    }

    int z = 0;
    return 0;
}

// ��ʼ��������
void initCounter(struct HTNode array[], const int size) {
    for (int i = 0; i < size; ++i) {
        array[i].weight = 0;
        array[i].character = i;
        array[i].lchild = array[i].rchild = NULL;
    }
}

// ����������
void HuffmanCoding(HTNodeP TreeRoot, struct Hcode codeArray[], int code) {
    if (TreeRoot->lchild != NULL && TreeRoot->rchild != NULL) {
        HuffmanCoding(TreeRoot->lchild, codeArray, 10 * code + leftCode);
        HuffmanCoding(TreeRoot->rchild, codeArray, 10 * code + rightCode);
    } else {
        int index = TreeRoot->character;
        codeArray[index].bit = code;
    }
}

// �����������
void CreateHuffmanTree(HTNodeP *TreeRoot, struct HTNode array[], int size) {
    struct HTNode copyArray[size]; // ��ʼ��
    // ��������
    memcpy(copyArray, array, size * sizeof(struct HTNode));

    // ����С����������������±�
    int minIndex1 = findMin(copyArray, size);
    int minIndex2 = findMin(copyArray, size);

    // ֻʣ��һ�������򷵻�
    if (minIndex2 == -1) {
        return;
    }

    // ��ʼ��������������С���ĸ��ڵ�
    HTNodeP Node = initHNode(array[minIndex1].weight + array[minIndex2].weight, -1);
    // �����������
    if (array[minIndex1].lchild == NULL && array[minIndex1].rchild == NULL) {
        Node->lchild = initHNode(array[minIndex1].weight, minIndex1);
    } else {
        Node->lchild = &array[minIndex1];
    }
    // ��������Һ���
    if (array[minIndex2].lchild == NULL && array[minIndex2].rchild == NULL) {
        Node->rchild = initHNode(array[minIndex2].weight, minIndex2);
    } else {
        Node->rchild = &array[minIndex2];
    }
    // ���������µĽڵ�
    *TreeRoot = Node;

    // ��ԭ�����Ϊ�µ�������Ȩֵ����
    copyArray[minIndex1] = *Node;

    // �ݹ�
    CreateHuffmanTree(TreeRoot, copyArray, size);
}

// ������С��Ȩ·������
int getMinWPL(HTNodeP *TreeRoot, int depth) {
    int left, right;
    if ((*TreeRoot)->lchild != NULL && (*TreeRoot)->rchild != NULL) {
        left = getMinWPL(&((*TreeRoot)->lchild), depth + 1);
        right = getMinWPL(&((*TreeRoot)->rchild), depth + 1);
    } else {
        return (*TreeRoot)->weight * depth;
    }
    return left + right;

}


HTNodeP initHNode(int weight, int character) {
    HTNode *Node = (HTNode *) malloc(sizeof(HTNode));
    Node->lchild = NULL;
    Node->rchild = NULL;
    Node->weight = weight;
    Node->character = character;
    return Node;
}

// �ҵ���Сֵ�����Сֵ����
int findMin(struct HTNode array[], int size) {
    int min = MAXNUM;
    int minIndex = -1;
    for (int i = 0; i < size; ++i) {
        if (array[i].weight != 0) {
            if (array[i].weight < min) {
                min = array[i].weight;
                minIndex = i;
            }
        }
    }
    array[minIndex].weight = 0; // ɾ��һ����
    return minIndex == -1 ? -1 : minIndex; // ���ظ������±�
}

