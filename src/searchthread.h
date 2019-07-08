/*
 * CrazyAra, a deep learning chess variant engine
 * Copyright (C) 2018 Johannes Czech, Moritz Willig, Alena Beyer
 * Copyright (C) 2019 Johannes Czech
 *
 * CrazyAra is free software: You can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * @file: searchthread.h
 * Created on 23.05.2019
 * @author: queensgambit
 *
 * Handles the functionality of a single search thread in the tree.
 */

#ifndef SEARCHTHREAD_H
#define SEARCHTHREAD_H

#include "node.h"
#include "constants.h"
#include "neuralnetapi.h"
#include "config/searchlimits.h"

class SearchThread
{
private:
    Node* rootNode;
    NeuralNetAPI *netBatch;
    unsigned int batchSize;

    float *inputPlanes; //[NB_VALUES_TOTAL]; //34][8][8];
//    StateListPtr& states; // = StateListPtr(new std::deque<StateInfo>(1)); // Drop old and create a new one
//    states
    // list of all node objects which have been selected for expansion
    std::vector<Node*> newNodes;
    std::vector<Node*> transpositionNodes;
    std::vector<Node*> collisionNodes;
    std::vector<Node*> terminalNodes;

    std::vector<Node*> parentNode;


    // stores the corresponding value-Outputs and probability-Outputs of the nodes stored in the vector "newNodes"
    // sufficient memory according to the batch-size will be allocated in the constructor
    NDArray valueOutputs;
    NDArray probOutputs;

    const float virtualLoss;
    bool isRunning;

    unordered_map<Key, Node*> *hashTable;
    SearchLimits *searchLimits;

    inline Node* get_new_child_to_evaluate(unsigned int &childIdx, bool &isCollision,  bool &isTerminal, size_t &depth);
    void set_NN_results_to_child_nodes();
    void backup_value_outputs(const float virtualLoss);
    void backup_collisions(const float virtualLoss);
    void revert_virtual_loss_for_collision(const float virtualLoss);

    /**
     * @brief create_new_node Creates a new node which will be added to the tree
     * @param newPos Board position which belongs to the node
     * @param parentNode Parent node of the new node
     * @param childIdx Index on how to visit the child node from its parent
     * @param numberNewNodes Index of the new node in the current batch
     */
    inline void create_new_node(Board* newPos, Node* parentNode, size_t childIdx, size_t numberNewNodes);

    /**
     * @brief copy_node Copies the node with the NN evaluation based on a preexisting node
     * @param it Iterator which from the hash table
     * @param newPos Board position which belongs to the node
     * @param parentNode Parent node of the new node
     * @param childIdx Index on how to visit the child node from its parent
     */
    inline void copy_node(const unordered_map<Key,Node*>::const_iterator &it, Board* newPos, Node* parentNode, size_t childIdx);
public:
    SearchThread(NeuralNetAPI *netBatch, unsigned int batchSize, const float virtualLoss, unordered_map<Key, Node*> *hashTable);
    void run_single_playout();

    /**
     * @brief create_mini_batch Creates a mini-batch of new unexplored nodes.
     * Terminal node are immediatly backpropagated without requesting the NN.
     * If the node was found in the hash-table it's value is backpropagated without requesting the NN.
     * If a collision occurs (the same node was selected multiple times), it will be added to the collisionNodes vector
     */
    void create_mini_batch();

    void thread_iteration();
    void setRootNode(Node *value);
    void set_search_limits(SearchLimits *s);
    bool getIsRunning() const;
    void setIsRunning(bool value);
    Node* getRootNode() const;
    SearchLimits *getSearchLimits() const;
};

void go(SearchThread *t);


#endif // SEARCHTHREAD_H
