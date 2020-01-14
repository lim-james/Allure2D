#ifndef GRAPH_SCENE_H
#define GRAPH_SCENE_H

#include "Scene.h"

#include "Graph.h"
#include "Camera.h"
#include "Transform.h"

#include <map>

class GraphScene : public Scene {

	Graph graph;

	Camera* camera;

	bool mouseMoved, mouseDown;
	Transform* selected;

	std::map<unsigned, Node*> entityNodeMap;

public:

	void Awake() override;
	void Update(const float& dt) override;

private:

	void CreateNode(Node * node);
	void CreateEdge(Edge * edge);

	void CursorPositionHandler(Events::Event* event);

	void OnMouseOverHandler(unsigned entity);
	void OnMouseOutHandler(unsigned entity);
	void OnMouseDownHandler(unsigned entity);
	void OnMouseUpHandler(unsigned entity);
	void OnClick(unsigned entity);

};

#endif
