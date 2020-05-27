/*
  CrazyAra, a deep learning chess variant engine
  Copyright (C) 2018       Johannes Czech, Moritz Willig, Alena Beyer
  Copyright (C) 2019-2020  Johannes Czech

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

/*
 * @file: mctsagent.h
 * Created on 13.05.2019
 * @author: queensgambit
 *
 * The MCTSAgent runs playouts/simulations in the search tree and updates the node statistics.
 * The final move is chosen according to the visit count of each direct child node and optionally taken Q-values into account.
 * One playout is defined as expanding one new node in the tree.
 * In the case of chess this means evaluating a new board position.
 * For more details and the mathematical equations please refer to our Journal Paper:
 * https://arxiv.org/abs/1908.06660 as well as the official DeepMind-papers.
 */

#ifndef MCTSAGENT_H
#define MCTSAGENT_H

#include "position.h"
#include "agent.h"
#include "../evalinfo.h"
#include "../node.h"
#include "../board.h"
#include "../nn/neuralnetapi.h"
#include "config/searchsettings.h"
#include "config/searchlimits.h"
#include "config/playsettings.h"
#include "../searchthread.h"
#include "../manager/statesmanager.h"
#include "../manager/timemanager.h"
#include "../manager/threadmanager.h"
#include "util/gcthread.h"


class MCTSAgent : public Agent
{
private:
    NeuralNetAPI* netSingle;

    SearchSettings* searchSettings;
    vector<SearchThread*> searchThreads;

    float inputPlanes[NB_VALUES_TOTAL];
    float valueOutput;
    unique_ptr<float[]> probOutputs;

    unique_ptr<TimeManager> timeManager;

    Node* rootNode;
    Board* rootPos;
    // The oldes root node stores a reference to the node with with the current root nodes is based on.
    // This is used in the case of tree reusage. The old subtree cannot be cleared immediatly because of
    // stateInfos for 3-fold repetition, but can be cleared as soon as the tree cannot be reused anymore.
    Node* oldestRootNode;

    // stores the pointer to the root node which will become the new root
    Node* ownNextRoot;
    // stores the pointer to the root node which will become the new root for opponents turn
    Node* opponentsNextRoot;

    MapWithMutex mapWithMutex;
    StatesManager* states;
    float lastValueEval;

    // boolean which indicates if the same node was requested twice for analysis
    bool reusedFullTree;

    // boolean which can be triggered by "stop" from std-in to stop the current search
    bool isRunning;

    // saves the overall nps for each move during the game
    float overallNPS;
    size_t avgDepth;
    size_t maxDepth;
    size_t tbHits;
    size_t nbNPSentries;

    unique_ptr<ThreadManager> threadManager;
    GCThread<Node> gcThread;

public:
    MCTSAgent(NeuralNetAPI* netSingle,
              vector<unique_ptr<NeuralNetAPI>>& netBatches,
              SearchSettings* searchSettings,
              PlaySettings* playSettings,
              StatesManager* states);
    ~MCTSAgent();
    MCTSAgent(const MCTSAgent&) = delete;
    MCTSAgent& operator=(MCTSAgent const&) = delete;

    void evaluate_board_state() override;

    /**
     * @brief run_mcts_search Starts the MCTS serach using all available search threads
     * @param evalInfo Evaluation struct which is updated during search
     */
    void run_mcts_search();

    void stop() override;

    /**
     * @brief print_root_node Prints out the root node statistics (visits, q-value, u-value)
     *  by calling the stdout operator for the Node class
     * @param ownMove Defines if the move shall be applied to the current root or the first potential root
     */
    void print_root_node();

    void apply_move_to_tree(Move move, bool ownMove) override;

    /**
     * @brief clear_game_history Traverses all root positions for the game and calls clear_subtree() for each of them
     */
    void clear_game_history();

    /**
     * @brief is_policy_map Checks if the current loaded network uses policy map representation.
     * @return True, if policy map else false
     */
    bool is_policy_map();

    /**
     * @brief get_name Returns the name specification of the MCTSAgent using the CrazyAra version ID and loaded neural net
     * @return
     */
    string get_name() const;

    Node *get_opponents_next_root() const;

    Node* get_root_node() const;

    string get_device_name() const;

    float get_dirichlet_noise() const;

    float get_q_value_weight() const;

    /**
     * @brief update_q_value_weight Updates the Q-value weights for the search (used for quick search)
     * @param value New value to set
     */
    void update_q_value_weight(float value);

    /**
     * @brief update_dirichlet_epsilon Updates the amount of dirichlet noise (used for quick search)
     * @param value New value to set
     */
    void update_dirichlet_epsilon(float value);
    Board *get_root_pos() const;
    bool is_running() const;

    /**
     * @brief update_stats Updates the avg depth, max depth and tablebase hits statistics
     */
    void update_stats();

private:
    /**
     * @brief reuse_tree Checks if the postion is know and if the tree or parts of the tree can be reused.
     * The old tree or former subtrees will be freed from memory.
     * @param pos Requested board position
     * @return Number of nodes that have already been explored before the serach
     */
    inline size_t init_root_node(Board* pos);

    /**
     * @brief get_new_root_node Returns the pointer of the new root node for the given position in the case
     * it was either the old root node or an element of the potential root node list.
     * Otherwise a nullptr will be returned. The old tree is deleted except the game nodes.
     * @param pos Requested board position
     * @return Pointer to root node or nullptr
     */
    inline Node* get_root_node_from_tree(Board* pos);

    /**
     * @brief create_new_root_node Creates a new root node for the given board position and requests the neural network for evaluation
     * @param pos Board position
     */
    inline void create_new_root_node(Board *pos);

    /**
     * @brief delete_old_tree Clear the old tree except the gameNodes (rootNode, opponentNextRoot)
     */
    void delete_old_tree();

    /**
     * @brief sleep_and_log_for Sleeps for a given amout of ms while every update interval ms the eval info will be updated an printed to stdout
     * @param evalInfo Evaluation information
     * @param timeMS Given amout of milli-seconds to sleep
     */
    void sleep_and_log_for(size_t timeMS, size_t updateIntervalMS=1000);

    /**
     * @brief update_nps_measurement Updates the overall nps by a rolling average
     * @param curNPS New NPS measurement
     */
    void update_nps_measurement(float curNPS);
};

#endif // MCTSAGENT_H
