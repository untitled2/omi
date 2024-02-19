#include <stdlib.h>
#include <stdio.h>

typedef struct node {
	int val;
	struct node* next;
	struct node* prev;
} Node;

void printList(Node *head)
{
	printf("Forward: ");
	while (head->next) {
		printf("%d ", head->val);
		head = head->next;
	}
	printf("%d ", head->val);
	printf("\nBackward: ");
	while (head) {
		printf("%d ", head->val);
		head = head->prev;
	}
	printf("\n\n");
}

/* yes, I am aware of Linux's overcommit */
void *mallocOrDie(size_t n)
{
	void *p = malloc(n);
	if (!p)
		abort();

	return p;
}

void InitializeToNull(Node** ptr)
{
	*ptr = NULL;
}

void AddAfter(Node** head, int position, Node node)
{
	Node *new_node = mallocOrDie(sizeof(Node));
	new_node->val = node.val;

	/* no head */
	if (!*head) {
		InitializeToNull(&new_node->prev);
		InitializeToNull(&new_node->next);
		*head = new_node;

		return;
	}

	/* insert before head */
	if (position <= 0) {
		InitializeToNull(&new_node->prev);
		new_node->next = *head;
		(*head)->prev = new_node;
		*head = new_node;

		return;
	}

	/* insert after pos or at the end */
	Node *p = *head;
	while (p->next && --position)
		p = p->next;

	new_node->prev = p;
	new_node->next = p->next;
	if (p->next)
		p->next->prev = new_node;
	p->next = new_node;
}

void RemoveNode(Node** head, int val)
{
	while ((*head) && (*head)->val == val) {
		*head = (*head)->next;
		free((*head)->prev);
		InitializeToNull(&(*head)->prev);
	}

	Node *prev;
	Node *curr = *head;
	while (curr) {
		if (curr->val == val) {
			prev->next = curr->next;
			if (curr->next)
				curr->next->prev = prev;
			free(curr);
			curr = prev->next;
		} else {
			prev = curr;
			curr = curr->next;
		}
	}
}

int main(void)
{
	Node *head;
	InitializeToNull(&head);

	printf("Adding 0\n");
	Node node = { .val = 0 };
	AddAfter(&head, 10, node);
	printList(head);

	printf("Adding 1\n");
	node.val = 1;
	AddAfter(&head, 10, node);
	printList(head);

	printf("Adding 2 after 0\n");
	node.val = 2;
	AddAfter(&head, 0, node);
	printList(head);

	printf("Adding 3\n");
	node.val = 3;
	AddAfter(&head, 10, node);
	printList(head);

	printf("Adding 3 after 0\n");
	node.val = 3;
	AddAfter(&head, 0, node);
	printList(head);

	printf("Adding 4\n");
	node.val = 4;
	AddAfter(&head, 10, node);
	printList(head);

	printf("Removing 0\n");
	RemoveNode(&head, 0);
	printList(head);

	printf("Removing 4\n");
	RemoveNode(&head, 4);
	printList(head);

	printf("Removing 0\n");
	RemoveNode(&head, 0);
	printList(head);

	printf("Removing 1\n");
	RemoveNode(&head, 1);
	printList(head);

	printf("Removing 3\n");
	RemoveNode(&head, 3);
	printList(head);

	return 0;
}

