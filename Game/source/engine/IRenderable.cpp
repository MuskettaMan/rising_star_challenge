#include "pch.h"
#include "engine/IRenderable.h"

const Transform2D& IRenderable::GetTransform() const
{
	return _transform;
}

void IRenderable::SetPosition(float x, float y)
{
	_transform.position.x = x;
	_transform.position.y = y;
}

void IRenderable::SetRotation(float r)
{
	_transform.rotation = r;
}

void IRenderable::SetScale(float sx, float sy)
{
	_transform.scale.x = sx;
	_transform.scale.y = sy;
}
