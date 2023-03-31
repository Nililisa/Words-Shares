//  Partie implantation du module holdall.
#include <stdbool.h>
#include "holdall.h"

//  struct holdall, holdall : implantation par liste dynamique simplement
//    chainée. L'insertion a lieu en queue si la macroconstante
//    HOLDALL_INSERT_TAIL est définie, en tête sinon.

typedef struct choldall choldall;

struct choldall {
  void *value;
  choldall *next;
};

struct holdall {
  choldall *head;
#ifdef HOLDALL_INSERT_TAIL
  choldall *tail;
#endif
  size_t count;
};

holdall *holdall_empty(void) {
  holdall *ha = malloc(sizeof *ha);
  if (ha == NULL) {
    return NULL;
  }
  ha->head = NULL;
#ifdef HOLDALL_INSERT_TAIL
  ha->tail = NULL;
#endif
  ha->count = 0;
  return ha;
}

int holdall_put(holdall *ha, void *ptr) {
  choldall *p = malloc(sizeof *p);
  if (p == NULL) {
    return -1;
  }
  p->value = ptr;
#ifdef HOLDALL_INSERT_TAIL
  p->next = NULL;
  if (ha->tail == NULL) {
    ha->head = p;
  } else {
    ha->tail->next = p;
  }
  ha->tail = p;
#else
  p->next = ha->head;
  ha->head = p;
#endif
  ha->count += 1;
  return 0;
}

size_t holdall_count(holdall *ha) {
  return ha->count;
}

int holdall_apply(holdall *ha, int (*fun)(void *)) {
  for (const choldall *p = ha->head; p != NULL; p = p->next) {
    int r = fun(p->value);
    if (r != 0) {
      return r;
    }
  }
  return 0;
}

int holdall_apply_context(holdall *ha,
    void *context, void *(*fun1)(void *context, void *ptr),
    int (*fun2)(void *ptr, void *resultfun1)) {
  for (const choldall *p = ha->head; p != NULL; p = p->next) {
    int r = fun2(p->value, fun1(context, p->value));
    if (r != 0) {
      return r;
    }
  }
  return 0;
}

int holdall_apply_context2(holdall *ha,
    void *context1, void *(*fun1)(void *context1, void *ptr),
    void *context2, int (*fun2)(void *context2, void *ptr, void *resultfun1)) {
  for (const choldall *p = ha->head; p != NULL; p = p->next) {
    int r = fun2(context2, p->value, fun1(context1, p->value));
    if (r != 0) {
      return r;
    }
  }
  return 0;
}

void holdall_dispose(holdall **haptr) {
  if (*haptr == NULL) {
    return;
  }
  choldall *p = (*haptr)->head;
  while (p != NULL) {
    choldall *t = p;
    p = p->next;
    free(t);
  }
  free(*haptr);
  *haptr = NULL;
}

int holdall_move_head_tail(holdall *hasrc, holdall *hadest) {
  if (holdall_count(hasrc) == 0) {
    return 0;
  }
  if (holdall_count(hadest) == 0) {
    hadest->head = hasrc->head;
  } else {
    hadest->tail->next = hasrc->head;
  }
  hadest->tail = hasrc->head;
  hasrc->head = hasrc->head->next;
  hadest->tail->next = NULL;
  --hasrc->count;
  ++hadest->count;
  return 0;
}

// holdall_split : scinde en deux fourretout ha1 et ha2 le fourretout ha
int holdall_split(holdall *ha, holdall *ha1, holdall *ha2) {
  if (holdall_count(ha) == 0) {
    return 0;
  }
  if (holdall_count(ha) == 1) {
    ha1->head = ha->head;
    ha1->tail = ha->tail;
    ha->head = NULL;
    ha->tail = NULL;
    --ha->count;
    ++ha1->count;
    return 0;
  }
  choldall *p = ha->head;
  choldall **pp1 = &ha1->head;
  choldall **pp2 = &ha2->head;
  while (p != NULL && p->next != NULL) {
    *pp1 = p;
    *pp2 = p->next;
    p = p->next->next;
    if (p == NULL || p->next == NULL) {
      ha1->tail = *pp1;
      ha2->tail = *pp2;
    }
    pp1 = &(*pp1)->next;
    pp2 = &(*pp2)->next;
    ++ha1->count;
    ++ha2->count;
    ha->count -= 2;
  }
  if (holdall_count(ha) != 0) {
    *pp1 = p;
    ha1->tail = *pp1;
    ++ha1->count;
    --ha->count;
  }
  ha2->tail->next = NULL;
  ha1->tail->next = NULL;
  ha->head = NULL;
  ha->tail = NULL;
  return 0;
}

int holdall_sort(holdall *ha, int (*compar)(const void *, const void *)) {
  if (holdall_count(ha) <= 1) {
    return 0;
  }
  holdall *ha1 = holdall_empty();
  if (ha1 == NULL) {
    return -1;
  }
  holdall *ha2 = holdall_empty();
  if (ha2 == NULL) {
    holdall_dispose(&ha1);
    return -1;
  }
  holdall_split(ha, ha1, ha2);
  holdall_sort(ha1, compar);
  holdall_sort(ha2, compar);
  while (holdall_count(ha1) != 0 && holdall_count(ha2) != 0) {
    if (compar(ha1->head->value, ha2->head->value) > 0) {
      holdall_move_head_tail(ha1, ha);
    } else {
      holdall_move_head_tail(ha2, ha);
    }
  }
  while (holdall_count(ha1) != 0) {
    holdall_move_head_tail(ha1, ha);
  }
  while (holdall_count(ha2) != 0) {
    holdall_move_head_tail(ha2, ha);
  }
  holdall_dispose(&ha1);
  holdall_dispose(&ha2);
  return 0;
}
