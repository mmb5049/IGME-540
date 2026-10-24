#pragma once

#include <memory>
#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include <memory>

class GameEntity
{
private:
    Transform transform;
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Material> material;

public:
    GameEntity(
        std::shared_ptr<Mesh> mesh,
        std::shared_ptr<Material> material);

    std::shared_ptr<Mesh> GetMesh();
    Transform& GetTransform();
    std::shared_ptr<Material> GetMaterial();

    void SetMaterial(std::shared_ptr<Material> material);

    void Draw();
};