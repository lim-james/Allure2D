#include "TableViewCell.h"

#include "Layers.h"
#include "CellScript.h"
#include "SpriteRender.h"
#include "Text.h"
#include "Layout.h"
#include "Animator.h"
#include "Button.h"

Transform * TableViewCell::Create() {
	const unsigned entity = entities->Create();
	entities->SetLayer(entity, UI);

	Transform* const transform = entities->GetComponent<Transform>(entity);

	CellScript* const script = entities->AddComponent<CellScript>(entity);
	script->SetActive(true);

	SpriteRender* const render = entities->AddComponent<SpriteRender>(entity);
	render->SetActive(true);
	render->tint = vec4f(0.2f, 0.2f, 0.2f,1.f);
	script->background = render;

	Button* const button = entities->AddComponent<Button>(entity);
	button->SetActive(true);
	script->button = button;

	{
		const unsigned label = entities->Create();
		entities->SetLayer(label, UI);

		Text* const text = entities->AddComponent<Text>(label);
		text->SetActive(true);
		text->text = "Insert title here";
		text->SetFont(font);
		text->paragraphAlignment = PARAGRAPH_LEFT;
		script->titleLabel = text;

		Layout* const layout = entities->AddComponent<Layout>(label);
		layout->SetActive(true);
		layout->AddConstraint(Constraint(LEFT_ANCHOR, transform, LEFT_ANCHOR, 1.f, 1.f, uiCamera));
		layout->AddConstraint(Constraint(RIGHT_ANCHOR, transform, RIGHT_ANCHOR, 1.f, -1.f, uiCamera));
		layout->AddConstraint(Constraint(TOP_ANCHOR, transform, TOP_ANCHOR, 1.f, 0.f, uiCamera));
		layout->AddConstraint(Constraint(BOTTOM_ANCHOR, transform, BOTTOM_ANCHOR, 1.f, 0.f, uiCamera));
	}

	//{
	//	const unsigned entity = entities->Create();
	//	entities->SetLayer(entity, UI);

	//	Transform* const transform = entities->GetComponent<Transform>(entity);

	//	Layout* const layout = entities->AddComponent<Layout>(entity);
	//	layout->SetActive(true);
	//	layout->AddConstraint(Constraint{ RIGHT_ANCHOR, transform, LEFT_ANCHOR, 1.f, 2.f, uiCamera });
	//	layout->AddConstraint(Constraint{ CENTER_Y_ANCHOR, transform, CENTER_Y_ANCHOR, 1.f, 0.f, uiCamera });

	//	Text* const text = entities->AddComponent<Text>(entity);
	//	text->SetActive(true);
	//	text->SetFont(font);
	//	text->text = "S+";
	//}

	Animator* const animation = entities->AddComponent<Animator>(entity);
	animation->SetActive(true);
	script->animator = animation;

	return transform;
}
