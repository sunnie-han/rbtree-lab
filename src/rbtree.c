#include "rbtree.h"

#include <stdlib.h>

/** 추가 함수 */
void free_node(rbtree *t, node_t * node);
void rbtree_rotate_left(rbtree *t, node_t *node);
void rbtree_rotate_right(rbtree *t, node_t *node);
void rbtree_insert_fixup(rbtree *t, node_t *node);
void rbtree_delete_fixup(rbtree *t, node_t *node);
node_t *tree_successor(rbtree *t, node_t *node);
int in_order(node_t *root, node_t *nil, key_t *arr, const size_t n, int index);

rbtree *new_rbtree(void) {
    rbtree *p = (rbtree *)calloc(1, sizeof(rbtree)); // 트리 메모리 할당
    // TODO: initialize struct if needed
    if (p == NULL){ // null 체크 (메모리 할당 체크)
        return NULL;
    }
    // 초기화
    p->nil = (node_t *)calloc(1, sizeof(node_t)); // sentinel node 메모리 할당
    p->root = p->nil;
    p->nil->color = RBTREE_BLACK;

    return p;
}
/** (추가 함수) 해당 노드와 그 이하 노드의 메모리를 반환 */
void free_node(rbtree *t, node_t *node) {
  if (node == t->nil) {
    return;
  }
  free_node(t, node->left);
  free_node(t, node->right);
  free(node);
  node = NULL;
}

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  free_node(t, t->root);
  free(t->nil);
  t->nil = NULL;
  free(t);
  t = NULL;
}

/** (추가 함수) 왼쪽 회전 */
void rbtree_rotate_left(rbtree *t, node_t *node) {
    node_t *right = node->right; // 회전 시킬 오른쪽 노드 지정
    node->right = right->left; // 회전 시킬 오른쪽 노드의 왼쪽 자식을 노드의 오른쪽으로 지정
    if (right->left != t->nil) // 회전 시킬 오른쪽 노드의 왼쪽 자식이 널노드가 아니면
		right->left->parent = node; // 그 노드의 부모를 노드로 지정

	right->parent = node->parent; // 오른쪽노드의 부모를 노드의 부모로 지정

	if (node->parent == t->nil) { // 노드의 부모가 널노드이면 현 노드는 루트
		t -> root = right; // 따라서 오른쪽 노드를 트리의 루트로 지정
	} else {
		if (node == node->parent->left) { // 노드가 부모의 왼쪽 자식이면
			node->parent->left = right; // 오른쪽 노드를 왼쪽 자식으로 지정
		} else  {
			node->parent->right = right; // 오른쪽 노드를 오른쪽 자식으로 지정
		}
	}
	right->left = node; // 오른쪽 노드의 왼쪽 자식으로 현 노드를 지정
	node->parent = right; // 현노드의 부모로 오른쪽 노드를 지정
    
}
/** (추가 함수) 오른쪽 회전 : 왼쪽 회전과 방향만 반대이고 같은 처리 */
void rbtree_rotate_right(rbtree *t, node_t *node) {
    node_t *left = node->left;
	node->left = left->right;
	if (left->right != t->nil)
		left->right->parent = node;

	left->parent = node->parent;

	if (node->parent == t->nil) {
		t -> root = left;
	} else {
		if (node == node->parent->right) {
			node->parent->right = left;
		} else  {
			node->parent->left = left;
		}
	}
	left->right = node;
	node->parent = left;
}

/** (추가 함수) 노드 삽입 후 트리 보정 */
void rbtree_insert_fixup(rbtree *t, node_t *node) {
	node_t *uncle; // 노드의 부모의 형제

	while (node != t->root && node->parent->color == RBTREE_RED) {  // 노드와 그 부모가 모두 적색 (특성 4 위반)
		if (node->parent == node->parent->parent->left) {           // 부모가 조부모의 왼쪽 자식이면
			uncle = node->parent->parent->right;                    // 삼촌은 그 오른쪽

			if (uncle->color == RBTREE_RED) {                       // 삼촌도 적색이면 (경우 1)
                // 부모와 삼촌을 흑색으로 바꿈
				node->parent->color = RBTREE_BLACK;
				uncle->color = RBTREE_BLACK;
                // 조부모의 색을 적색으로 바꿈
				node->parent->parent->color = RBTREE_RED;
                // 조부모에서 다시 보정 필요여부 확인
				node = node->parent->parent;
			} else {				                                // 삼촌은 흑색일 때
				if (node == node->parent->right) {                  // 노드가 부모의 오른쪽자식이면 (경우 2)
					node = node->parent;                            // 부모를 기준으로
					rbtree_rotate_left(t, node);                    // 왼쪽 회전
				}
                // 노드가 부모의 왼쪽자식 (경우 3)
                // 부모를 흑색으로 바꿈
				node->parent->color = RBTREE_BLACK;
                // 조부모를 적색으로 바꿈
				node->parent->parent->color = RBTREE_RED;
				rbtree_rotate_right(t, node->parent->parent);       // 조부모를 기준으로 오른쪽 회전
			}
		} else {
            // 부모가 오른쪽 자식인 경우와 방향만 반대이고 같은 처리
			uncle = node->parent->parent->left;

			if (uncle->color == RBTREE_RED) {
				node->parent->color = RBTREE_BLACK;
				uncle->color = RBTREE_BLACK;

				node->parent->parent->color = RBTREE_RED;

				node = node->parent->parent;
			} else {
				if (node == node->parent->left) {
					node = node->parent;
					rbtree_rotate_right(t, node);
				}
				node->parent->color = RBTREE_BLACK;
				node->parent->parent->color = RBTREE_RED;
				rbtree_rotate_left(t, node->parent->parent);
			}
		}
	}
    // 루트는 검정색 (특성 2) ---> (?) 노드가 바로 루트에 들어가는 경우가 있나? => 있음 트리의 첫번째 노드를 삽입할 때!
	t->root->color = RBTREE_BLACK;
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
    // TODO: implement insert
    // 삽입 할 node 메모리 할당
    node_t *new_node = (node_t *)calloc(1, sizeof(node_t));
    // 키 설정
    new_node->key = key;

    node_t *parent = t->nil;                    // 삽입할 노드의 부모 노드가 될 노드
    node_t *node = t->root;                     // 삽입할 위치

    while (node != t->nil) {                    // 삽일할 위치(node)가 경계 노드면 그곳이 삽입할 위치
        parent = node;
        if (new_node->key < node->key) {        // 확인하고 있는 위치의 노드의 키값보다 작으면
            node = node->left;                  // 그 노드의 왼쪽 서브트리 탐색
        } else {
            node = node->right;                 // 그 노드의 오른쪽 서브트리 탐색
        }
    }
    new_node->parent = parent;                  // 새 노드의 부모를 찾은 삽입할 위치의 부모로 설정
    if (parent == t->nil) {                     // 부모가 경계 노드이면
        t->root = new_node;                     // 해당 노드가 루트 노드
    } else {
        if (new_node->key < parent->key) {      // 부모의 키값보다 작으면
            parent->left = new_node;            // 왼쪽 자식
        } else {                                // 크거나 같으면
            parent->right = new_node;           // 오른쪽 자식
        }
    }
    // 삽일 할 노드 설정 (색, 자식 노드)
    new_node->color = RBTREE_RED;
    new_node->left = t->nil;
    new_node->right = t->nil;

    // 트리 보정
    rbtree_insert_fixup(t, new_node);

    return new_node; // 새로 추가된 노드의 포인터를 리턴
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
    // TODO: implement find
    node_t *node = t->root;                         // 루트부터 검색
    while (node != t->nil && node->key != key) {    // 검색대상 노드가 경계노드가 아니거나 키값이 일치하지 않는 동안 탐색
        if (key < node->key) {                      // 찾는 키값보다 작으면 왼쪽 서브트리를 탐색
            node = node->left;
        } else {                                    // 크면 오른쪽 서브트리를 탐색
            node = node->right;
        }
    }
    if (node == t->nil) {                           // 검색한 결과가 경계노드이면 탐색 못한 경우
        return NULL;
    } else {                                        // 검색한 결과가 경계노드가 아니면 그 노드를 반환
        return node;
    }
}

node_t *rbtree_min(const rbtree *t) {
    // TODO: implement find
    // 제일 왼쪽 끝의 노드를 반환
    node_t *node = t->root; // 루트부터 탐색
    while (node->left != t->nil) {
        node = node->left;
    }
    return node;
}

node_t *rbtree_max(const rbtree *t) {
    // TODO: implement find
    // 제일 왼쪽 끝의 노드를 반환
    node_t *node = t->root; // 루트부터 탐색
    while (node->right != t->nil) {
        node = node->right;
    }
    return node;
}

/** (추가 함수) 노드의 직후 원소를 리턴 */
node_t *tree_successor(rbtree *t, node_t *node) {
    // 해당 노드의 오른쪽 자식이 있으면
    node_t *result;
    if (node->right != t->nil) {
        // tree-minimum(x) => 노드 x를 루트로 하는 트리의 최솟값을 리턴
        result = node->right;           // 오른쪽 트리의 루트부터 탐색
        while (result->left != t->nil) {
            result = result->left;
        }
        return result;
    }
    // 해당 노드의 오른쪽 자식이 없으면
    result = node->parent;              // 해당노드보다 커지는 부모 노드(*자기자신도 조상으로 간주)를 탐색
    // 노드가 부모노드의 왼쪽자식이 되는 순간의 부모노드가 직후 노드
    while (result != t->nil && node == result->right) {
        node = result;
        result = result->parent;
    }
    return result;
}

/** (추가 함수) 노드 삭제 후 트리 보정 */
void rbtree_delete_fixup(rbtree *t, node_t *node){
    while (node != t->root && node->color == RBTREE_BLACK) {    // 루트가 아닌 이중 흑색
        // 형제는 경계 노드(nil)일 수 없음 (node가 흑색이라 형제가 nil이면 흑색 노드의 갯수가 달라지기 때문)
        node_t *sibling;
        if (node == node->parent->left) {
            sibling = node->parent->right;
            if (sibling->color == RBTREE_RED) {
                sibling->color = RBTREE_BLACK;
                node->parent->color = RBTREE_RED;
                rbtree_rotate_left(t, node->parent);
                sibling = node->parent->right;
            }
            if (sibling->left->color == RBTREE_BLACK && sibling->right->color == RBTREE_BLACK) {
                sibling->color = RBTREE_RED;
                node = node->parent;
            } else {
                if (sibling->right->color == RBTREE_BLACK) {
                    sibling->left->color = RBTREE_BLACK;
                    sibling->color = RBTREE_RED;
                    rbtree_rotate_right(t, sibling);
                    sibling = node->parent->right;
                }
                sibling->color = node->parent->color;
                node->parent->color = RBTREE_BLACK;
                sibling->right->color = RBTREE_BLACK;
                rbtree_rotate_left(t, node->parent);
                node = t->root;
            }
        } else {
            sibling = node->parent->left;
            if (sibling->color == RBTREE_RED) {
                sibling->color = RBTREE_BLACK;
                node->parent->color = RBTREE_RED;
                rbtree_rotate_right(t, node->parent);
                sibling = node->parent->left;
            }
            if (sibling->left->color == RBTREE_BLACK && sibling->right->color == RBTREE_BLACK) {
                sibling->color = RBTREE_RED;
                node = node->parent;
            } else {
                if (sibling->left->color == RBTREE_BLACK) {
                    sibling->right->color = RBTREE_BLACK;
                    sibling->color = RBTREE_RED;
                    rbtree_rotate_left(t, sibling);
                    sibling = node->parent->left;
                }
                sibling->color = node->parent->color;
                node->parent->color = RBTREE_BLACK;
                sibling->left->color = RBTREE_BLACK;
                rbtree_rotate_right(t, node->parent);
                node = t->root;
            }
        }
    }
    node->color = RBTREE_BLACK; // node가 적색이면서 흑색 (적색) 이거나 루트
}

int rbtree_erase(rbtree *t, node_t *p) {
    // TODO: implement erase
    node_t *y;  // 노드 y = 제거할 노드
    if (p->left == t->nil || p->right == t->nil) {  // p가 자식이 하나이거나 없는 노드이면
        y = p;                                      // 제거 대상은 해당 노드
    } else {                        // 두 자식을 가지는 노드이면
        y = tree_successor(t, p);   // 제거 대상은 노드 p의 직후 원소
    }
    node_t *x; // 노드 x = 노드 y의 부모와 연결할 y의 자식 노드 
    if (y->left != t->nil) {    // y가 nil이 아닌 왼쪽 자식이 있는 경우 (y는 무조건 하나 이하의 자식만 가짐)
        x = y->left;
    } else {                    // 그 외의 경우
        x = y->right;
    }
    x->parent = y->parent;      // y의 부모를 y의 자식(x)의 부모로 연결 (?)x가 nil일 때 nil의 부모를 y이 부모로 되는데 문제없나? => nil의 부모는 쓸일 없으니까 무관?

    if (y->parent == t->nil) {  // y가 루트인 경우
        t->root = x;            // x를 트리의 루트로 설정
    } else {
        // 제거할 노드(y)의 자식을 y의 부모의 자식으로 연결
        if (y == y->parent->left) {
            y->parent->left = x;
        } else {
            y->parent->right = x;
        }
    }

    if (y != p) {           // p가 자식 노드 두개를 가지는 경우
        p->key = y->key;    // p의 부속데이터를 y로 대체
    }

    if (y->color == RBTREE_BLACK) { // 삭제할 노드가 흑색이면 특성 5(혹은 특성 1,2,4)를 위반하므로 트리 보정
        rbtree_delete_fixup(t, x);
    }

    free(y);  // 삭제할 노드의 메모리를 반환
    y = NULL;
    
    return 0;
}

/** (추가 함수) 중위 순회해서 배열에 저장 */
int in_order(node_t *root, node_t *nil, key_t *arr, const size_t n, int index) {
    if (root == nil || index > n) return index;

    index = in_order(root->left, nil, arr, n, index);
    if (index < n) {
        arr[index++] = root->key;
    }
    index = in_order(root->right, nil, arr, n, index);
    return index;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  // 중위 순회
  in_order(t->root, t->nil, arr, n, 0);

  return 0;
}
