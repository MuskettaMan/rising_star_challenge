
#pragma once

class ITexture;

class IShader
{
public:

	IShader(std::shared_ptr<ITexture> TextureIn);
	virtual void Update() = 0;

	const std::shared_ptr<ITexture> GetTexture() const;

protected: 

	std::shared_ptr<ITexture> _texture;

};

