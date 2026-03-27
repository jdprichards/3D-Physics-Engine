#include "Light.h"

Light::Light(EntityTemplate& entityTemplate, EntityID ID, const Matrix4x4& transform, ColourRGBA colour, const std::string name)
	: Entity(entityTemplate, ID, transform), mColour{ colour }
{
	std::string tempName;

	int nameInt = 0;

	if (name == "")
		tempName = "Light";

	for (auto& [id, entity] : gEntityManager->GetEntities())
	{
		if (entity->GetName() == tempName)
		{
			nameInt += 1;
		}

		if (name == "")
			tempName = "Light";
		else
			tempName = name;

		if (nameInt > 0)
			tempName = tempName + std::to_string(nameInt);
	}

	mName = tempName;

	mIntensity = Transform().GetScale().x * Transform().GetScale().x;

	RenderGroup() = 1;

	mEntityType = EEntityType::Light;
}