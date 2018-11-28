//
//  Assignment_5.h
//  Assignment 5
//
//  Created by ashwin haritsa on 9/10/18.
//  Copyright © 2018 ashwin haritsa. All rights reserved.
//

#ifndef Assignment_5_h
#define Assignment_5_h
#include <stdio.h>

/* creates a new structure called Node which will be used to create new nodes when inserting into the priority queue */
typedef struct mystruct Node;
struct mystruct{
    double priority;
    Node* next;
    void* data;
};








#endif /* Assignment_1C_h */
