#include <stdlib.h>

#include <stdio.h>

#include <string.h>

//  Basic queue for the entrances of the car parks

// Queue item

struct item

{

    // Variable for the data of the node

    char *data;

    // Next node in the queue/linked list

    struct item *next;
};

typedef struct item item;

// Queue

struct queue

{

    // Number of items in the queue

    int count;

    // Rear item of the queue

    item *rear;

    // Front item of the queue

    item *front;
};

typedef struct queue queue;

// Function to construct a queue

void initQueue(queue *queue)

{

    // Set variables to 0 and NULL

    queue->rear = NULL;

    queue->front = NULL;

    queue->count = 0;
}

// Check if queue is empty

int isEmpty(queue *queue)

{
    // return value based on count

    return (queue->count == 0);
};

// Get queue size

int queueSize(queue *queue)
{
    // Return queue count variable

    return (queue->count);
}

// Display all in linked list

void showQueue(item *front)

{

    // If front is null then print nothing, otherwise print whole queue

    if (front == NULL)

    {

        printf("Queue is empty\n");
    }

    else

    {

        printf("%s\n", front->data);

        // If next is not null, repeat

        if (front->next != NULL)

        {

            showQueue(front->next);
        }
    }
}

// Add to queue, only takes

void addToQueue(queue *queue, char *newData)

{

    // Create new temp item pointer

    item *temp;

    // Allocate memory for temp

    temp = (item *)malloc(sizeof(item));

    temp->data = malloc(sizeof(char) * 7);

    temp->next = malloc(sizeof(item));

    // Assign new value temp

    temp->data = newData;

    // Next is null as at the end of queue

    temp->next = NULL;

    // Check if front and rear of the queue is empty

    if (!isEmpty(queue))

    {

        // If they are then the queue is empty and need to allocate new node to front and back

        // Add to rear and rear - next

        queue->rear->next = temp;

        // Assign temp variable to rear or queue

        queue->rear = temp;

        // Queue is empty
    }

    else

    {

        queue->front = queue->rear = temp;
    }

    // Increment count

    queue->count++;
}

// Remove item from queue

char *removeFromQueue(queue *queue)

{

    if (!isEmpty(queue))
    {

        // Get item to be removed pointer to free

        item *removedItem = malloc(sizeof(item));

        char *removedData = malloc(sizeof(char) * 7);

        // Get char array to return

        removedItem = queue->front;

        if (queue->count == 1)
        {

            queue->front = NULL;

            // Take 1 away from count

            free(removedItem);

            queue->count--;
        }
        else
        {

            // Move queue

            queue->front = removedItem->next;

            // Take 1 away from count

            free(removedItem);

            queue->count--;
        }

        // Return char array from deallocated data

        return removedData;
    }
    else
    {

        printf("Queue is empty");

        return NULL;
    }
}

