// Program for creating AVL (balanced) binary trees
// Additionally added inorder, preorder and postorder traversals to
// check the order of the created tree
#include <stdio.h>
#include <stdlib.h>
// Define an arbitrary initial length for the traversal queue
#define QUEUE_LENGTH 100
// Define a macro for max, since it's a simple function, I avoid using libraries
#define max(a, b) ((a) > (b) ? (a) : (b))

typedef struct node {
  int data;
  int height;
  struct node *left;
  struct node *right;
} Node;

typedef struct queue {
  Node **array;
  int front;
  int rear;
  int capacity;
} Queue;

// Main methods
Node *insert(Node *n, int value);
Node *delete(Node *n, int value);
Queue *createQueue(int capacity);
void enqueue(Queue *q, Node *n);
Node *dequeue(Queue *q);
int isQueueEmpty(Queue *q);

int getHeight(Node *n);
int getBalanceFactor(Node *n);
Node *getMinimum(Node *n);
void updateHeight(Node *n);
void bftTraversal(Node *n);
void showInorder(Node *n);
void showPreorder(Node *n);
void showPostorder(Node *n);
void freeTree(Node *n);

// Rotation types
// 1 - Simple right rotation: fixes "left-left" imbalance
// 2 - Simple left rotation: fixes "right-right" imbalance
// 3 - Double left-right rotation: fixes "left-right" imbalance
// 4 - Double right-left rotation: fixes "right-left" imbalance

// Simple rotations
Node *rotateRight(Node *n);
Node *rotateLeft(Node *n);
// Double rotations
Node *rotateLeftRight(Node *n);
Node *rotateRightLeft(Node *n);

int main() {
  // Instantiate the main tree
  // NOTE: The traversal queue is instantiated inside the bftTraversal function
  Node *tree = NULL;
  // Variable for each input
  int x;
  // Answer to continue or finish (y/Y to continue)
  char answer;

  // Loop...
  do {
    printf("Enter a number: ");
    if (scanf("%d", &x) != 1) {
      printf("Invalid input. Try again.\n");
      // Clear input buffer
      while (getchar() != '\n') {
        continue;
      }
    }
    // Insert the entered data into the tree. Remember that insert (like
    // rotation functions) returns pointers to Nodes, so the tree can
    // rotate if necessary and reassign pointers
    tree = insert(tree, x);
    printf("Do you want to enter another number? (y/n): ");
    scanf(" %c", &answer);
  } while (answer == 'y' || answer == 'Y');

  // Delete a node from the tree
  printf("Enter a number to delete: ");
  if (scanf("%d", &x) != 1) {
    printf("Invalid input. Try again.\n");
    // Clear input buffer
    while (getchar() != '\n') {
      continue;
    }
  }
  tree = delete(tree, x);

  // Print the entire bft traversal
  printf("\nBFT traversal of the balanced tree:\n");
  bftTraversal(tree);

  // Also print any traversal we want
  // NOTE: Only enabling inorder to have at least one activated
  printf("Inorder traversal of the balanced tree:\n");
  showInorder(tree);

  // printf("Preorder traversal of the balanced tree:\n");
  // showPreorder(tree);

  // printf("Postorder traversal of the balanced tree:\n");
  // showPostorder(tree);

  // Free all tree memory
  freeTree(tree);

  // Finish
  return 0;
}

// Returns the height of the node. This data is used to calculate the distance
// to the deepest node from this one
int getHeight(Node *n) {
  if (n == NULL) {
    return 0;
  }
  return n->height;
}

// Returns the difference between left and right node heights.
// Must check if it's -1, 0 or 1 (or |1|)
int getBalanceFactor(Node *n) {
  if (n == NULL) {
    return 0;
  }
  return getHeight(n->left) - getHeight(n->right);
}

// Helper function to update height
void updateHeight(Node *n) {
  n->height = max(getHeight(n->left), getHeight(n->right)) + 1;
}

// Simple right rotation: fixes "left-left" imbalance
Node *rotateRight(Node *n) {
  printf("Right rotation on node (%d)\n", n->data);
  Node *aux1 = n->left;
  Node *aux2 = aux1->right;

  aux1->right = n;
  n->left = aux2;

  // Update height of n
  updateHeight(n);
  // Update height of aux1 (left child of n)
  updateHeight(aux1);

  return aux1;
}

// Simple left rotation: fixes "right-right" imbalance
Node *rotateLeft(Node *n) {
  printf("Left rotation on node (%d)\n", n->data);
  Node *aux1 = n->right;
  Node *aux2 = aux1->left;

  aux1->left = n;
  n->right = aux2;

  // Update height of n
  updateHeight(n);
  // Update height of aux1 (right child of n)
  updateHeight(aux1);

  return aux1;
}

// Double left-right rotation: fixes "left-right" imbalance
Node *rotateLeftRight(Node *n) {
  n->left = rotateLeft(n->left);
  return rotateRight(n);
}

// Double right-left rotation: fixes "right-left" imbalance
Node *rotateRightLeft(Node *n) {
  n->right = rotateRight(n->right);
  return rotateLeft(n);
}

// Inserts a value into the AVL tree and rebalances if necessary
Node *insert(Node *n, int value) {
  if (n == NULL) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->data = value;
    newNode->height = 1;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
  }

  if (value < n->data) {
    n->left = insert(n->left, value);
  } else if (value > n->data) {
    n->right = insert(n->right, value);
  } else {
    return n;
  }

  // Update node height
  updateHeight(n);

  // Calculate balance factor
  int balance = getBalanceFactor(n);

  // Case 1: Left Left
  if (balance > 1 && value < n->left->data) {
    return rotateRight(n);
  }
  // Case 2: Right Right
  if (balance < -1 && value > n->right->data) {
    return rotateLeft(n);
  }
  // Case 3: Left Right
  if (balance > 1 && value > n->left->data) {
    return rotateLeftRight(n);
  }
  // Case 4: Right Left
  if (balance < -1 && value < n->right->data) {
    return rotateRightLeft(n);
  }

  return n;
}

// Gets the minimum node (leftmost)
Node *getMinimum(Node *n) {
  Node *current = n;
  while (current->left != NULL) {
    current = current->left;
  }
  return current;
}

// Deletes a value from the AVL tree and rebalances if necessary
Node *delete(Node *n, int value) {
  if (n == NULL) {
    return n;
  }

  // Check if there's a smaller value on the left or right
  if (value < n->data) {
    n->left = delete(n->left, value);
  } else if (value > n->data) {
    n->right = delete(n->right, value);
  } else {
    // Case 1: Node with no children or one child
    if (n->left == NULL) {
      Node *successor = n->right;
      free(n);
      return successor;
    } else if (n->right == NULL) {
      Node *successor = n->left;
      free(n);
      return successor;
    }

    // Case 2: Node with two children
    Node *successor = getMinimum(n->right);
    n->data = successor->data;
    n->right = delete(n->right, successor->data);
  }

  // Update node height
  updateHeight(n);

  // Calculate balance factor
  int balance = getBalanceFactor(n);

  // Case 1: Left Left
  if (balance > 1 && getBalanceFactor(n->left) >= 0) {
    return rotateRight(n);
  }
  // Case 2: Right Right
  if (balance < -1 && getBalanceFactor(n->right) <= 0) {
    return rotateLeft(n);
  }
  // Case 3: Left Right
  if (balance > 1 && getBalanceFactor(n->left) < 0) {
    n->left = rotateLeft(n->left);
    return rotateRight(n);
  }
  // Case 4: Right Left
  if (balance < -1 && getBalanceFactor(n->right) > 0) {
    n->right = rotateRight(n->right);
    return rotateLeft(n);
  }

  return n;
}

// Creates a queue with the given capacity
Queue *createQueue(int capacity) {
  // Create the queue and assign all initial parameters
  Queue *q = (Queue *)malloc(sizeof(Queue));
  q->capacity = capacity;
  q->front = 0;
  q->rear = -1;
  q->array = (Node **)malloc(sizeof(Node *) * capacity);
  return q;
}

// Checks if the queue is empty
int isQueueEmpty(Queue *q) { return q->rear < q->front; }

// Adds a node to the queue
void enqueue(Queue *q, Node *n) {
  // Check if queue is full (rear equals capacity - 1)
  if (q->rear == q->capacity - 1) {
    // Dynamically grow it if more space is needed
    q->capacity *= 2;
    q->array = realloc(q->array, sizeof(Node *) * q->capacity);
  }
  q->rear++;
  q->array[q->rear] = n;
}

// Removes and returns a node from the queue
Node *dequeue(Queue *q) {
  // Check if queue is empty (front equals rear)
  if (isQueueEmpty(q)) {
    printf("Empty queue\n");
    return NULL;
  }
  Node *n = q->array[q->front];
  q->front++;
  return n;
}

// Breadth-first traversal method to efficiently traverse horizontally and print each node
void bftTraversal(Node *n) {
  if (n == NULL) {
    return;
  }
  // Create a queue of length 100 to start
  Queue *q = createQueue(QUEUE_LENGTH);
  // Enqueue the initial node for bft traversal
  enqueue(q, n);
  // While the queue is not empty...
  while (!isQueueEmpty(q)) {
    // With dequeue() we return the current node
    Node *current = dequeue(q);
    // Print the current node's data
    printf("(%d)->", current->data);
    // And while there are more children, keep enqueueing/dequeueing and
    // traversing/printing
    if (current->left != NULL) {
      enqueue(q, current->left);
    }
    if (current->right != NULL) {
      enqueue(q, current->right);
    }
  }
  printf("\n");
  free(q->array);
  free(q);
}

// Prints inorder traversal
void showInorder(Node *n) {
  if (n != NULL) {
    showInorder(n->left);
    printf("(%d) ", n->data);
    showInorder(n->right);
  }
}

// Prints preorder traversal
void showPreorder(Node *n) {
  if (n != NULL) {
    printf("(%d) ", n->data);
    showPreorder(n->left);
    showPreorder(n->right);
  }
}

// Prints postorder traversal
void showPostorder(Node *n) {
  if (n != NULL) {
    showPostorder(n->left);
    showPostorder(n->right);
    printf("(%d) ", n->data);
  }
}

// Function to free all tree memory for safety
void freeTree(Node *n) {
  if (n == NULL) {
    return;
  }
  freeTree(n->left);
  freeTree(n->right);
  printf("\nFreeing node memory: (%d)", n->data);
  free(n);
}
