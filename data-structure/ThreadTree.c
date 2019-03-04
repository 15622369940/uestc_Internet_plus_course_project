//
// Created by PC on 2018/11/8.
//

#include <stdio.h>
#include <stdlib.h>

typedef int DataType;
typedef struct TTNode {
    struct TTNode *lchild, *rchild;
    int ltag;
    int rtag;
    DataType data;
} TTNode, *TTNodeP;

TTNodeP pre; // ȫ�ֱ�������¼ǰ��

void NodeVisit(DataType data);      // ��ӡֵ

void InOrder(TTNodeP TreeRoot);     // �������

void TTreeInsertNode(TTNodeP *TreeRoot, DataType data);

TTNodeP initNode(DataType data);

void Inthread(TTNodeP TreeRoot);

TTNodeP InPre(TTNodeP Node);

TTNodeP InNext(TTNodeP Node);

TTNodeP TinFirst(TTNodeP Node);

void TinOrder(TTNodeP TreeRoot);

#define size 11

int main() {
    TTNodeP TTree = NULL;
    DataType A[size] = {1, 6, 18, 3, 7, 17, 20, 2, 4, 13, 9};
    for (int i = 0; i < size; ++i) {
        TTreeInsertNode(&TTree, A[i]);
    }
    // �������
    InOrder(TTree);
    printf("-->InOrder\n");

    pre = NULL;
    Inthread(TTree);

    TTNodeP p = TTree->rchild->lchild->lchild;

    TTNodeP q = InPre(p);
    if (q != NULL) {
        printf("\nǰ��Ϊ%d", q->data);
    } else {
        printf("\nǰ��ΪNULL");
    }

    q = InNext(p);
    if (q != NULL) {
        printf("\n���Ϊ%d", q->data);
    } else {
        printf("\n���ΪNULL");
    }

    q = TinFirst(TTree);
    printf("\n��һ�����Ϊ%d", q->data);

    printf("\n");
    TinOrder(TTree);

    return 0;
}

// ���ݽڵ��ӡ
void TinOrder(TTNodeP TreeRoot) {
    if (TreeRoot == NULL) {
        return;
    }
    do{
        printf("%d ", TreeRoot->data);
        TreeRoot = InNext(TreeRoot);
    }while (TreeRoot!= NULL);
}

// ������������ĵ�һ���ڵ�
TTNodeP TinFirst(TTNodeP Node) {
    if (Node == NULL) {
        return NULL;
    }
    while (Node->lchild != NULL && Node->ltag == 0) {
        Node = Node->lchild;
    }
    return Node;
}

// ���ؽڵ�ĺ��
TTNodeP InNext(TTNodeP Node) {
    if (Node == NULL) {
        return NULL;
    }
    if (Node->rtag == 1) {
        return Node->rchild;
    } else {
        if (Node->rchild != NULL) {
            Node = Node->rchild;
            while (Node->lchild != NULL && Node->ltag == 0) {
                Node = Node->lchild;
            }
        } else {
            return NULL;
        }
    }
    return Node;
}

// ���ؽڵ��ǰ��
TTNodeP InPre(TTNodeP Node) {
    if (Node == NULL) {
        return NULL;
    }
    pre = NULL;
    if (Node->ltag == 1) {
        return Node->lchild;
    } else {
        Node = Node->lchild;
        while (Node->rchild != NULL && Node->rtag == 0) {
            Node = Node->rchild;
            pre = Node;
        }
    }
    return pre;
}

// ����������
void Inthread(TTNodeP TreeRoot) {
    if (TreeRoot != NULL) { // ��ΪNULL���ܼ���
        Inthread(TreeRoot->lchild);
        if (TreeRoot->lchild == NULL) { // ����ýڵ�����Ϊ�գ���lchildָ����ǰ��
            TreeRoot->ltag = 1;
            TreeRoot->lchild = pre;
        }
        if (pre != NULL && pre->rchild == NULL) { // ���ǰ���Һ���Ϊ�գ�����rhildָ��ýڵ�
            pre->rtag = 1;
            pre->rchild = TreeRoot;
        }
        pre = TreeRoot; // ������������ʱ���ı�ǰ��
        Inthread(TreeRoot->rchild);
    }
}

// ���ն������������㷨���ɸ�������������������������ȽϺ��ж϶Դ�
void TTreeInsertNode(TTNodeP *TreeRoot, DataType data) {
    // �½�һ���ڵ�
    TTNodeP Node = initNode(data);
    // ���������û�д����򽫸��½��Ľڵ㸳ֵ����
    if (*TreeRoot == NULL) {
        *TreeRoot = Node;
    } // �²���Ľڵ���ֵ�ȸ�С��ȣ��߸������
    else if (data <= (*TreeRoot)->data) {
        // �������ݹ������
        if ((*TreeRoot)->lchild == NULL) {
            (*TreeRoot)->lchild = Node;
        } else { // ��ݹ�
            TTreeInsertNode(&((*TreeRoot)->lchild), data);
        }
    } else {
        // �������ݹ������
        if ((*TreeRoot)->rchild == NULL) {
            (*TreeRoot)->rchild = Node;
        } else { // �ҵݹ�
            TTreeInsertNode(&((*TreeRoot)->rchild), data);
        }
    }
}

// �������
void InOrder(TTNodeP TreeRoot) {
    if (TreeRoot != NULL) {
        if (TreeRoot->ltag == 0) {
            InOrder(TreeRoot->lchild);
        }
        NodeVisit(TreeRoot->data);
        if (TreeRoot->rtag == 0) {
            InOrder(TreeRoot->rchild);
        }
    }
}

// �ڵ��ʼ��
TTNodeP initNode(DataType data) {
    TTNode *L = (TTNode *) malloc(sizeof(TTNode));
    L->lchild = L->rchild = NULL;
    L->ltag = L->rtag = 0;
    L->data = data;
    return L;
}

// ��ӡֵ
void NodeVisit(DataType data) {
    printf("%d ", data);
}