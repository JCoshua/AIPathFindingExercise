#include "NodeGraph.h"
#include <raylib.h>
#include <xlocinfo>

DynamicArray<NodeGraph::Node*> reconstructPath(NodeGraph::Node* start, NodeGraph::Node* end)
{
	DynamicArray<NodeGraph::Node*> path;
	NodeGraph::Node* currentNode = end;

	while (currentNode != start->previous)
	{
		currentNode->color = 0xFFFF00;
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
	//Insert algorithm here
	DynamicArray<NodeGraph::Node*> openedList;
	DynamicArray<NodeGraph::Node*> closedList;
	Node* currentNode = start;
	openedList.addItem(currentNode);

	while (openedList.getLength() > 0)
	{
		//Get the new current node from the openedList
		currentNode = openedList.getItem(0);

		for (int i = 0; i < currentNode->edges.getLength(); i++)
		{
			//If the current edge of the current node has not been added to the open or closed list
			if (!closedList.contains(currentNode->edges.getItem(i).target))
			{
				if (!openedList.contains(currentNode->edges.getItem(i).target) || 
					(openedList.contains(currentNode->edges.getItem(i).target) && currentNode->edges.getItem(i).target->gScore > currentNode->gScore + currentNode->edges.getItem(i).cost))
				{
					//Make the item's previous node be the current node
					currentNode->edges.getItem(i).target->previous = currentNode;

					//Make the node's gScore equal to the current gScore plus the cost
					currentNode->edges.getItem(i).target->gScore = currentNode->gScore + currentNode->edges.getItem(i).cost;
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

		//Sort the opened List
		int j = 0;
		for (int i = 0; i < openedList.getLength(); i++)
		{
			j = i - 1;

			while (j >= 0 && openedList.getItem(j)->gScore > openedList.getItem(i)->gScore)
			{
				Node* nodeToChange = openedList.getItem(j + 1);
				nodeToChange = openedList.getItem(j);
				j--;
			}

			Node* nodeToChange = openedList.getItem(j + 1);
			nodeToChange = openedList.getItem(i);
		}
	}

	return reconstructPath(start,goal);
}

void NodeGraph::drawGraph(Node* start)
{
	DynamicArray<Node*> drawnList = DynamicArray<Node*>();
	drawConnectedNodes(start, drawnList);
}

void NodeGraph::drawNode(Node* node, int color)
{
	static char buffer[10];
	sprintf_s(buffer, "%.0f", node->gScore);

	//Draw the circle for the outline
	DrawCircle((int)node->position.x, (int)node->position.y, 16, GetColor(color));
	//Draw the inner circle
	DrawCircle((int)node->position.x, (int)node->position.y, 14, BLACK);
	//Draw the text
	DrawText(buffer, (int)node->position.x, (int)node->position.y, 24, RAYWHITE);
}

void NodeGraph::drawConnectedNodes(Node* node, DynamicArray<Node*>& drawnList)
{
	drawnList.addItem(node);

	for (int i = 0; i < node->edges.getLength(); i++)
	{
		Edge e = node->edges[i];
		//Draw the Edge
		DrawLine((int)node->position.x, (int)node->position.y, (int)e.target->position.x, (int)e.target->position.y, WHITE);
		//Draw the cost
		MathLibrary::Vector2 costPos = { (node->position.x + e.target->position.x) / 2, (node->position.y + e.target->position.y) / 2 };
		static char buffer[10];
		sprintf_s(buffer, "%.0f", e.cost);
		DrawText(buffer, (int)costPos.x, (int)costPos.y, 16, RAYWHITE);
		//Draw the target node
		if (!drawnList.contains(e.target)) {
			drawConnectedNodes(e.target, drawnList);
		}
	}
}
