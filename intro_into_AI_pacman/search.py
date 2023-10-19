# search.py
# ---------
# Licensing Information:  You are free to use or extend these projects for
# educational purposes provided that (1) you do not distribute or publish
# solutions, (2) you retain this notice, and (3) you provide clear
# attribution to UC Berkeley, including a link to http://ai.berkeley.edu.
# 
# Attribution Information: The Pacman AI projects were developed at UC Berkeley.
# The core projects and autograders were primarily created by John DeNero
# (denero@cs.berkeley.edu) and Dan Klein (klein@cs.berkeley.edu).
# Student side autograding was added by Brad Miller, Nick Hay, and
# Pieter Abbeel (pabbeel@cs.berkeley.edu).


"""
In search.py, you will implement generic search algorithms which are called by
Pacman agents (in searchAgents.py).
"""

import util

class SearchProblem:
    """
    This class outlines the structure of a search problem, but doesn't implement
    any of the methods (in object-oriented terminology: an abstract class).

    You do not need to change anything in this class, ever.
    """

    def getStartState(self):
        """
        Returns the start state for the search problem.
        """
        util.raiseNotDefined()

    def isGoalState(self, state):
        """
          state: Search state

        Returns True if and only if the state is a valid goal state.
        """
        util.raiseNotDefined()

    def getSuccessors(self, state):
        """
          state: Search state

        For a given state, this should return a list of triples, (successor,
        action, stepCost), where 'successor' is a successor to the current
        state, 'action' is the action required to get there, and 'stepCost' is
        the incremental cost of expanding to that successor.
        """
        util.raiseNotDefined()

    def getCostOfActions(self, actions):
        """
         actions: A list of actions to take

        This method returns the total cost of a particular sequence of actions.
        The sequence must be composed of legal moves.
        """
        util.raiseNotDefined()


def tinyMazeSearch(problem):
    """
    Returns a sequence of moves that solves tinyMaze.  For any other maze, the
    sequence of moves will be incorrect, so only use this for tinyMaze.
    """
    from game import Directions
    s = Directions.SOUTH
    w = Directions.WEST
    return  [s, s, w, s, w, w, s, w]

def depthFirstSearch(problem):
    
    initial_state= problem.getStartState()

    # node = <state , action , parent_node>
    class Pnode:
        def __init__(self, state, action, parent):
            self.state = state
            self.action = action 
            self.parent = parent

        def return_action_list(self):
            action_list = []
            node = self

            while node:
                if node.action:
                    action_list.append(node.action)

                node = node.parent
            
            action_list.reverse()
            return action_list

    initial_node = Pnode(initial_state, None, None)

    frontier = util.Stack()
    visited_nodes = []
    frontier.push(initial_node)
    
    while not frontier.isEmpty():
    
        node = frontier.pop()

        if node.state not in visited_nodes :

            visited_nodes.append(node.state)

        if problem.isGoalState(node.state):
            return node.return_action_list()
        else:
            Successors = problem.getSuccessors(node.state)

            for succesor in Successors:
                child = Pnode(succesor[0], succesor[1], node)

                if child.state not in visited_nodes:
                    
                    
                    checkStack = frontier
                    for n in checkStack.list:
                        if n.state == child.state:
                            break
                    frontier.push(child)



def breadthFirstSearch(problem):
   
    
    initial_state= problem.getStartState()

    # node = <state , action , parent_node>
    class Pnode:
        def __init__(self, state, action, parent):
            self.state = state
            self.action = action 
            self.parent = parent

        def return_action_list(self):
            action_list = []
            node = self

            while node:
                if node.action:
                    action_list.append(node.action)

                node = node.parent
            
            action_list.reverse()
            return action_list

    initial_node = Pnode(initial_state, None, None)

    frontier = util.Queue()
    visited_nodes = []
    frontier.push(initial_node)
    
    while not frontier.isEmpty():
    
        node = frontier.pop()
        visited_nodes.append(node.state)

        if problem.isGoalState(node.state):
            return node.return_action_list()
        else:
            Successors = problem.getSuccessors(node.state)

            for succesor in Successors:
                child = Pnode(succesor[0], succesor[1], node)

                if child.state not in visited_nodes:
                    
                    
                    checkStack = frontier
                    for n in checkStack.list:
                        if n.state == child.state:
                            break
                    frontier.push(child)
    
def uniformCostSearch(problem):
    
    util.raiseNotDefined()

              
    

def nullHeuristic(state, problem=None):
    """
    A heuristic function estimates the cost from the current state to the nearest
    goal in the provided SearchProblem.  This heuristic is trivial.
    """
    return 0

def aStarSearch(problem, heuristic=nullHeuristic):
   
    frontier = util.PriorityQueue()
    initial_state = problem.getStartState()
    # node = < state, actions, cost>
    initial_node = (initial_state, [], 0)
    visited=[]
    frontier.push(initial_node, 0)

    while not frontier.isEmpty():

        node = frontier.pop()

        if problem.isGoalState(node[0]):
            return node[1]

        if node[0] not in visited:
            visited.append(node[0])

            Succesors = problem.getSuccessors(node[0])
            for succesor in Succesors:        
                cost = node[2] + succesor[2]
                total = cost + heuristic(succesor[0], problem)
                frontier.push((succesor[0], node[1]+[succesor[1]],cost), total)

      
        
            

# Abbreviations
bfs = breadthFirstSearch
dfs = depthFirstSearch
astar = aStarSearch
ucs = uniformCostSearch
