#include "pch.h"
#include "engine/physics_debug_drawer.hpp"
#include "engine/IGraphics.h"

PhysicsDebugDrawer::PhysicsDebugDrawer(IGraphics& graphics) : _graphics(graphics)
{
}

void PhysicsDebugDrawer::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	b2Vec2 p1 = vertices[vertexCount - 1];
	for (size_t i = 0; i < vertexCount; ++i)
	{
		b2Vec2 p2 = vertices[i];
		DrawSegment(p1, p2, color);
		p1 = p2;
	}
}

void PhysicsDebugDrawer::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	DrawPolygon(vertices, vertexCount, color);
}

void PhysicsDebugDrawer::DrawCircle(const b2Vec2& center, float radius, const b2Color& color)
{
}

void PhysicsDebugDrawer::DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color)
{
}

void PhysicsDebugDrawer::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
	_graphics.DrawLine(XMFLOAT2{ p1.x, p1.y }, XMFLOAT2{ p2.x, p2.y });
}

void PhysicsDebugDrawer::DrawTransform(const b2Transform& xf)
{
}

void PhysicsDebugDrawer::DrawPoint(const b2Vec2& p, float size, const b2Color& color)
{
}
