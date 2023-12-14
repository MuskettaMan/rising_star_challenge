#include "pch.h"
#include "engine/IRenderable.h"

const Transform2D& IRenderable::GetTransform() const
{
	return _transform;
}

void IRenderable::SetPosition(float x, float y)
{
	_transform.positionX = x;
	_transform.positionY = y;
}

void IRenderable::SetRotation(float r)
{
	_transform.rotation = r;
}

void IRenderable::SetScale(float sx, float sy)
{
	_transform.scaleX = sx;
	_transform.scaleY = sy;
}
