#include <stdio.h>
#include <stdlib.h>

#include "rbtree.h"

/**
 * 트리 생성 후 생성된 트리의 포인터 반환
 *   -> [nil노드]를 만들어 트리 포인터의 속성 포인터인 nil포인터와 root포인터가 
 *   -> [nil노드]를 가리키게함 (nil노드의 색은 BLACK)
*/
rbtree *new_rbtree(void)
{
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  node_t *nil = (node_t *)calloc(1, sizeof(node_t));
  p->nil = nil;
  p->nil->color = RBTREE_BLACK;
  p->root = nil;

  return p;
}

/**
 * t의 속성(root, nil)들이 가리키고있는 노드들의 메모리 반환 및
 * t가 가리키고 있는 트리에 할당된 메모리 반환
*/
void delete_rbtree(rbtree *t)
{
  remove_subtree(t, t->root); //트리안에 있는 노드들 삭제
  free(t->nil);
  free(t);
}

/**
 * 해당 트리 안에 존재하는 모든 노드들을 메모리 반환
 * (후위 순회)
*/
void remove_subtree(rbtree *t, node_t *node)
{
  if (node != t->nil)
  {
    remove_subtree(t, node->left);
    remove_subtree(t, node->right);
    free(node);
  }
}

/**
 * 노드 x를 기준으로 좌로 회전
 *   -> x 노드의 오른쪽 자식을 y
 *   -> 결과 : y의 왼쪽 자식이 x가 된다.
*/
void left_rotate(rbtree *t, node_t *x)
{
  node_t *y = x->right;
  x->right = y->left;

  if (y->left != t->nil)
  {
    y->left->parent = x;
  }

  y->parent = x->parent;

  if (x->parent == t->nil)
  {
    t->root = y;
  }
  else if (x == x->parent->left)
  {
    x->parent->left = y;
  }
  else
  {
    x->parent->right = y;
  }
  y->left = x;
  x->parent = y;
}

/**
 * 노드 x를 기준으로 우로 회전
 *   -> x 노드의 왼쪽 자식을 y
 *   -> 결과 : y의 오른쪽 자식이 x가 된다.
*/
void right_rotate(rbtree *t, node_t *x)
{
  node_t *y = x->left;
  x->left = y->right;

  if (y->right != t->nil)
  {
    y->right->parent = x;
  }
  y->parent = x->parent;

  if (x->parent == t->nil)
  {
    t->root = y;
  }
  else if (x == x->parent->right)
  {
    x->parent->right = y;
  }
  else
  {
    x->parent->left = y;
  }

  y->right = x;
  x->parent = y;
}

/**
 * 새로운 노드가 삽입되었을 때, RB_TREE 규칙을 만족시키기 위한 재조립
*/
void rbtree_insert_fixup(rbtree *t, node_t *z)
{
  node_t *uncle_node = NULL;
  while (z->parent->color == RBTREE_RED) // RB_TREE 조건 4(빨간색 노드의 자식노드 양쪽은 언제나 모두 검정색)에 부합하지 않음
  {
    if (z->parent == z->parent->parent->left) //삽입노드의 부모가 왼쪽 자식일 경우
    {
      uncle_node = z->parent->parent->right;
      if (uncle_node->color == RBTREE_RED) // 삼촌노드가 빨간색일 때(Case 1)
      {
        // 부모노드와 삼촌노드의 색깔을 검정색으로 바꾸고,
        // 할아버지노드의 색을 빨간색으로 바꿈
        z->parent->color = RBTREE_BLACK;
        uncle_node->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent; //이제, 할아버지 노드가 빨간색이 되었을 때 문제를 해결
      }
      else //삼촌노드가 검정색일때(Case 2, Case 3)
      {
        if (z == z->parent->right) // Case 2 : 삽입노드가 [부모노드] - [할아버지노드]와 일직선상에 있지 않은 경우
        {
          z = z->parent;
          left_rotate(t, z); // 좌로 회전하여 Case 3으로 만들어 줌
        }
        // Case 3 : 부모노드와 할아버지노드의 색을 바꾼 뒤, 우로 회전(할아버지 기준)
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        right_rotate(t, z->parent->parent);
      }
    }
    else //삽입노드의 부모가 오른쪽 자식일 때
    {
      uncle_node = z->parent->parent->left;
      if (uncle_node->color == RBTREE_RED)
      {
        z->parent->color = RBTREE_BLACK;
        uncle_node->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }
      else
      {
        // 삼촌노드가 검정색일 때(Case 2, Case 3)
        if (z == z->parent->left) // Case 2 : 삽입노드가 [부모노드] - [할아버지노드]와 일직선상에 있지 않은 경우
        {
          z = z->parent;
          right_rotate(t, z); // 우로 회전하여 Case 3을 만들어 줌
        }
        // Case 3 : 부모노드와 할아버지노드의 색을 바꾼 뒤, 좌로 회전(할아버지 기준)
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        left_rotate(t, z->parent->parent);
      }
    }
  }

  t->root->color = RBTREE_BLACK;
}

/**
 * t가 가리키는 rb트리에 key값을 가진 노드를 삽입
*/
node_t *rbtree_insert(rbtree *t, const key_t key)
{
  node_t *x = t->root; //현재 root값을 가리키는 노드 포인터 변수 x 선언
  node_t *y = t->nil;  //현재 nil값을 가리키는 노드 포인터 변수 y 선언

  // 삽입 위치 탐색(=> x는 노드가 삽입될 위치(nil), y는 그때 x의 부모노드를 가리키고 있음)
  while (x != t->nil)
  {
    y = x;
    if (key < x->key)
    {
      x = x->left;
    }
    else //트리에 이미 존재하는 키 값이면 그의 오른쪽 자식으로 삽입
    {
      x = x->right;
    }
  }

  // 삽입 노드 위치 설정 후, 노드 생성 및 기존 트리와 연결
  node_t *new_node = (node_t *)calloc(1, sizeof(node_t)); //삽입할 노드를 위한 메모리 할당
  new_node->key = key;
  new_node->parent = y;

  if (y == t->nil)
  {
    t->root = new_node;
  }
  else if (new_node->key < y->key)
  {
    y->left = new_node;
  }
  else
  {
    y->right = new_node;
  }
  new_node->left = t->nil;
  new_node->right = t->nil;
  new_node->color = RBTREE_RED; // 삽입노드 색상은 RED(Black_hegiht 조건을 만족시키기 위해)

  rbtree_insert_fixup(t, new_node);
  return t->root;
}

/**
 * 재귀로 key값을 가진 노드를 탐색하여 있을 경우, 해당 노드의 포인터를 반환
*/
node_t *search_node(rbtree *t, node_t *curr, const key_t key)
{
  if (curr == t->nil) //curr가 트리의 nil을 가리키고 있으면
  {
    return NULL; // 해당 트리안에 노드가 없으므로 NULL 반환
  }
  else if (curr->key == key) // curr의 key값이 찾으려는 key값과 같은 경우
  {
    return curr;
  }
  // 재귀 활용하여 탐색
  else if (key < curr->key) //찾으려는 key값이 현재 가리키고있는 노드의 key값보다 작은 경우
  {
    return search_node(t, curr->left, key);
  }
  else //찾으려는 key값이 현재 가리키고있는 노드의 key값보다 큰 경우
  {
    return search_node(t, curr->right, key);
  }
}

/**
 * 트리에서 key값을 가진 노드를 찾아 해당 노드의 포인터를 반환하는
 * 메서드 search_node 활용 
*/
node_t *rbtree_find(const rbtree *t, const key_t key)
{
  return search_node(t, t->root, key);
}

/**
 *  x노드를 root로 하는 서브트리 내에서 최솟값 반환 
 */
node_t *tree_min(rbtree *t, node_t *x)
{
  while (x->left != t->nil)
  {
    x = x->left;
  }
  return x;
}

/** 
 * x노드를 root로 하는 서브트리 내에서 최댓값 반환 
 */
node_t *tree_max(rbtree *t, node_t *x)
{
  while (x->right != t->nil)
  {
    x = x->right;
  }
  return x;
}

/** 
 * 트리 내에서 최솟값 반환 
 */
node_t *rbtree_min(const rbtree *t)
{
  node_t *min_node = t->root;
  while (min_node->left != t->nil)
  {
    min_node = min_node->left;
  }
  return min_node;
}

/** 
 * 트리 내에서 최댓값 반환 
 */
node_t *rbtree_max(const rbtree *t)
{
  node_t *max_node = t->root;
  while (max_node->right != t->nil)
  {
    max_node = max_node->right;
  }
  return max_node;
}

/**
 * 기존 노드자리(old)를 새로운 노드(new)로 교체
 *   -> old 노드의 부모가 new 노드를 가리키도록 하는 함수
*/
void rb_transplant(rbtree *t, node_t *old, node_t *new)
{
  if (old->parent == t->nil) // 기존 노드가 root인 경우
  {
    t->root = new;
  }
  else if (old == old->parent->left) // 기존노드가 왼쪽자식인 경우
  {
    old->parent->left = new;
  }
  else // 기존노드가 오른쪽자식인 경우
    old->parent->right = new;
  new->parent = old->parent;
}

/**
 * RB트리 내에서의 노드 삭제는 RB트리 규칙을 만족해야하기 떄문에
 * 각각의 Case별(총 4가지)로 나누어 해결 
*/
void rb_delete_fixup(rbtree *t, node_t *x)
{
  node_t *w = NULL; // x노드의 형제 노드
  while (x != t->root && x->color == RBTREE_BLACK)
  {
    if (x == x->parent->left) // x노드가 왼쪽 자식일 경우
    {
      w = x->parent->right;

      if (w->color == RBTREE_RED) // Case 1 : 형제노드가 빨간색인 경우
      {
        // 좌로 회전하고 나서 RBTree 규칙(#5)을 꺠지 않기 위해 먼저 부모와 형제의 색을 미리 바꿔줌.
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(t, x->parent);
        w = x->parent->right;
      }
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK) // Case 2 : 형제노드 자식 둘(left, right) 다 검정색
      {
        //x에 있는 Extra Black과 형제노드의 검정색에서 색을 빠지게하여 Extra Black을 부모에게 위임
        w->color = RBTREE_RED;
        x = x->parent;
      }
      else
      {
        if (w->right->color == RBTREE_BLACK) // Case 3 : 형제의 오른쪽 자식이 검정색일 경우
        {                                    //           -> Case 4로 변환
          w->left->color == RBTREE_BLACK;
          w->color == RBTREE_RED;
          right_rotate(t, w);
          w = x->parent->right;
        }
        // Case 4 : 형제의 오른쪽 자식이 빨간색인 경우
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        left_rotate(t, x->parent);
        x = t->root;
      }
    }
    else
    {
      w = x->parent->left;
      if (w->color == RBTREE_RED)
      {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t, x->parent);
        w = x->parent->left;
      }
      if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK)
      {
        w->color = RBTREE_RED;
        x = x->parent;
      }
      else
      {
        if (w->left->color == RBTREE_BLACK)
        {
          w->right->color == RBTREE_BLACK;
          w->color == RBTREE_RED;
          left_rotate(t, w);
          w = x->parent->left;
        }
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        right_rotate(t, x->parent);
        x = t->root;
      }
    }
  }
  x->color = RBTREE_BLACK;
}

/**
 * rbtree에서 z노드 삭제
 *   : 이진탐색트리에서의 노드 삭제랑 같음(삭제할 노드의 대체노드를 찾고 대체할 노드와 삭제할 노드의 자리를 바꿔줌)
 *   -> z : 삭제하려는 노드
 *   -> y : 대체 노드(Successor), 삭제노드의 오른쪽 서브트리안에서 가장 작은 key값을 가진 노드
 *   -> x : 대체 노드(Successor)의 자식, 삭제하고 난 후 그 자식들을 본래 트리에 이어주기 위해 존재
*/
int rbtree_erase(rbtree *t, node_t *z)
{
  if (z == NULL)
  {
    return 0;
  }
  node_t *x = NULL;
  node_t *y = z;
  color_t deleted_color = y->color; // 삭제하려는 색

  if (z->left == t->nil) // z노드의 왼쪽 자식이 없는 경우
  {
    x = z->right;
    rb_transplant(t, z, z->right);
  }
  else if (z->right == t->nil) // z노드의 오른쪽 자식이 없는 경우
  {
    x = z->left;
    rb_transplant(t, z, z->left);
  }
  else // z노드의 자식이 2개(left, right)인 경우
  {
    y = tree_min(t, z->right); // 대체노드(Successor)를 오른쪽 서브트리의 가장 작은 값으로 설정
    // y = tree_max(t, z->left); // 대체노드를 왼쪽 서브트리의 가장 큰 값으로 설정
    deleted_color = y->color; // 삭제하려는 색 = 대채노드((Successor)의 색
    x = y->right;

    if (y->parent == z) // 삭제노드의 자식이 2개인데 그 자식들 중에 대체노드(Successor)가 있는 경우
    {
      x->parent = y;
    }
    else
    {
      rb_transplant(t, y, y->right); // 대체노드와 대체노드의 오른쪽 자식과 바꿔줌
      y->right = z->right;           // (-> 이진탐색트리 안에서의 노드 삭제를 안정적으로 하기 위해)
      y->right->parent = y;
    }

    rb_transplant(t, z, y); // 삭제할 노드와 대체노드의 자리를 바꿔줌
    y->left = z->left;
    y->left->parent = y;
    y->color = z->color;
  }

  if (deleted_color == RBTREE_BLACK) // 삭제하려는 색이 검정색일 경우
    rb_delete_fixup(t, x);           // 현재 x 노드의 위치부터 fixup 과정 시작(현재 x노드의 위치는 대체노드의 자리)

  free(z); // 삭제되는 노드의 할당된 메모리 해제
  return 0;
}

/**
 * 트리를 중위순회하며 배열로 만듦
*/
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{
  int idx = 0;
  int *pidx = &idx;
  inorder_to_array(t, t->root, pidx, arr);
  return 0;
}

/**
 * 재귀 활용하여
 * 중위 순회
 *   -> 중위순회하는 이유 : 결과값이 크기 순서대로 나옴
*/
void inorder_to_array(rbtree *t, node_t *curr, int *pidx, key_t *arr)
{
  if (curr != t->nil)
  {
    inorder_to_array(t, curr->left, pidx, arr);
    arr[(*pidx)++] = curr->key;
    inorder_to_array(t, curr->right, pidx, arr);
  }
}
