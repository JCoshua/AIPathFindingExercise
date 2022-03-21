#include "NodeGraph.h"
#include <raylib.h>
#include <xlocinfo>

DynamicArray<NodeGraph::Node*> reconstructPath(NodeGraph::Node* start, NodeGraph::Node* end)
{
	DynamicArray<NodeGraph::Node*> path;
	NodeGraph::Node* currentNode = end;

	while (currentNode != start->previous)
	{
		currentNode->color = 0x00FF00FF;
		path.insert(currentNode, 0);
		currentNode = currentNode->previous;
	}

	return path;
}

float diagonalDistance(NodeGraph::Node* node, NodeGraph::Node* goal, float cardinalCost, float diagonalCost)
{
	float displacementX = abs(node->position.x - goal->position.x);
	float displacementY = abs(node->position.y - goal->position.y);

	return cardinalCost * (displacementX + displacementY) + (diagonalCost - 2 * cardinalCost) * fmin(displacementX, displacementY);
}

float NodeGraph::calculateManhattanDistance(Node* firstNode, Node* secondNode)
{
	return abs(firstNode->position.x - secondNode->position.x) + abs(firstNode->position.y - secondNode->position.y);
}

void sortFScore(DynamicArray<NodeGraph::Node*>& nodes)
{
	NodeGraph::Node* key = nullptr;
	int j = 0;

	for (int i = 1; i < nodes.getLength(); i++) {
		key = nodes[i];
		j = i - 1;
		while (j >= 0 && nodes[j]->fScore > key->fScore) {
			nodes[j + 1] = nodes[j];
			j--;
		}

		nodes[j + 1] = key;
	}
}

DynamicArray<NodeGraph::Node*> NodeGraph::findPath(Node* start, Node* goal)
{
	resetGraphScore(start);
	//Insert algorithm here
	DynamicArray<NodeGraph::Node*> openedList;
	DynamicArray<NodeGraph::Node*> closedList;
	Node* currentNode = start;
	openedList.addItem(currentNode);
	start->color = 0x00FF00FF;

	while (openedList.getLength() > 0)
	{
		//Sort the opened List
		sortFScore(openedList);

		//Get the new current node from the openedList
		currentNode = openedList.getItem(0);
		currentNode->color = 0xFFFF00FF;
		for (int i = 0; i < currentNode->edges.getLength(); i++)
		{
			//If the current edge of the current node has not been added to the open or closed list
			if (!closedList.contains(currentNode->edges.getItem(i).target) && currentNode->edges.getItem(i).target->walkable)
			{
				if (!openedList.contains(currentNode->edges.getItem(i).target) || 
					(openedList.contains(currentNode->edges.getItem(i).target) && currentNode->edges.getItem(i).target->gScore > currentNode->gScore + currentNode->edges.getItem(i).cost))
				{
					//Make the item's previous node be the current node
					currentNode->edges.getItem(i).target->previous = currentNode;

					float hScore = diagonalDistance(currentNode->edges.getItem(i).target, goal, currentNode->edges.getItem(i).cost, currentNode->edges.getItem(i).cost * 1.5f);
					//Make the node's gScore equal to the current gScore plus the cost
					currentNode->edges.getItem(i).target->fScore = currentNode->gScore + currentNode->edges.getItem(i).cost + hScore;

				}
				if (!openedList.contains(currentNode->edges.getItem(i).target))
				{
					//Add the item to the openedList
					openedList.addItem(currentNode->edges.getItem(i).target);
				}
			}
		}

		//Add the current node to the closed list and remove it from the opened List
		openedList.remove(currentNode);
		closedList.addItem(currentNode);

		if(currentNode == goal)
			return reconstructPath(start, goal);
	}

	return reconstructPath(start,goal);
}

void NodeGraph::drawGraph(Node* start)
{
	DynamicArray<Node*> drawnList = DynamicArray<Node*>();
	drawConnectedNodes(start, drawnList);
}

void NodeGraph::drawNode(Node* node, float size)
{
	static char buffer[10];
	sprintf_s(buffer, "%.0f", node->fScore);

	//Draw the circle for the outline
	DrawCircle((int)node->position.x, (int)node->position.y, size + 1, GetColor(node->color));
	//Draw the inner circle
	DrawCircle((int)node->position.x, (int)node->position.y, size, GetColor(node->color));
	//Draw the text
	DrawText(buffer, (int)node->position.x, (int)node->position.y, .8f, BLACK);
}

void NodeGraph::drawConnectedNodes(Node* node, DynamicArray<Node*>& drawnList)
{
	drawnList.addItem(node);
	if (node->walkable)
		drawNode(node, 8);

	for (int i = 0; i < node->edges.getLength(); i++)
	{
		Edge e = node->edges[i];
		////Draw the Edge
		//DrawLine((int)node->position.x, (int)node->position.y, (int)e.target->position.x, (int)e.target->position.y, WHITE);
		////Draw the cost
		//MathLibrary::Vector2 costPos = { (node->position.x + e.target->position.x) / 2, (node->position.y + e.target->position.y) / 2 };
		//static char buffer[10];
		//sprintf_s(buffer, "%.0f", e.cost);
		//DrawText(buffer, (int)costPos.x, (int)costPos.y, 16, RAYWHITE);
		//Draw the target node
		if (!drawnList.contains(e.target)) {
			drawConnectedNodes(e.target, drawnList);
		}
	}
}

void NodeGraph::resetGraphScore(Node * start)
{
	DynamicArray<Node*> resetList = DynamicArray<Node*>();
	resetConnectedNodes(start, resetList);
}

void NodeGraph::resetConnectedNodes(Node* node, DynamicArray<Node*>& resetList)
{
	resetList.addItem(node);

	for (int i = 0; i < node->edges.getLength(); i++)
	{
		node->edges[i].target->gScore = 0;
		node->edges[i].target->hScore = 0;
		node->edges[i].target->fScore = 0;
		node->edges[i].target->color = 0xFFFFFFFF;

		//Draw the target node
		if (!resetList.contains(node->edges[i].target)) {
			resetConnectedNodes(node->edges[i].target, resetList);
		}
	}
}