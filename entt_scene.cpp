#include <algorithm>
#include <iostream>
#include <optional>
#include <type_traits>
#include <vector>

#include <cassert>
#include <cmath>

#include "entt/entt.hpp"

//////////////////////////////////////////////////////////////////////////

// Just a very minimal definition of a 3D vector.
struct Vec3 {
    float x = 0;
    float y = 0;
    float z = 0;

    static const Vec3 zero;
    static const Vec3 one;
};

const Vec3 Vec3::zero = {0, 0, 0};
const Vec3 Vec3::one = {1, 1, 1};

Vec3 operator+(const Vec3 &a, const Vec3 &b) { return {a.x + b.x, a.y + b.y, a.z + b.z}; }

std::ostream &operator<<(std::ostream &out, const Vec3 &v)
{
    return out << "Vec3: " << v.x << " " << v.y << " " << v.z;
}

//////////////////////////////////////////////////////////////////////////

// In this minimal example, Transform only contains the position.
struct Transform {
    Vec3 position = Vec3::zero;
};

// Operator for combining Transforms.
Transform operator*(const Transform &a, const Transform &b) { return {a.position + b.position}; }

std::ostream &operator<<(std::ostream &out, const Transform &t) { return out << "Transform: " << t.position; }

//////////////////////////////////////////////////////////////////////////

// A SceneNode contains an entity's local Transform as well as references to
// parent and child nodes. Additionally it provides a reference to the
// corresponding entity. Ownership is managed by the entity component system.
//
// The following invariants are maintained:
// - Parent and child references are kept consistent.
// - Combined parent transforms are cached. This cache is invalided
//   automatically.
class SceneNode
{
  public:
    ~SceneNode()
    {
        if (m_parent) {
            m_parent->removeChild(this);
        }

        for (const auto &child : m_children) {
            child->clearParent();
        }
    }

    entt::entity entity() const { return m_entity; }

    const Transform &transform() const { return m_transform; }

    void setTransform(const Transform &transform)
    {
        invalidateChildrenCachedParentTransform();
        m_transform = transform;
    }

    Transform parentTransform() const
    {
        if (!m_cachedParentTransform) {
            m_cachedParentTransform = m_parent ? m_parent->globalTransform() : Transform{};
        }

        return *m_cachedParentTransform;
    }

    Transform globalTransform() const { return parentTransform() * m_transform; }

    SceneNode *parent() const { return m_parent; }

    const std::vector<SceneNode *> &children() const { return m_children; }

    void addChild(SceneNode *child)
    {
        // For simplicity we only allow adding orphans.
        assert(!child->m_parent);

        child->setParent(this);
        m_children.push_back(child);
    }

    void removeChild(SceneNode *child)
    {
        assert(child->m_parent == this);

        auto it = std::find(m_children.cbegin(), m_children.cend(), child);
        if (it == m_children.cend()) {
            assert(false && "Parent-child-invariant is broken!");
            return;
        }

        child->clearParent();

        m_children.erase(it);
    }

  private:
    entt::entity m_entity;

    Transform m_transform;

    SceneNode *m_parent = nullptr;
    std::vector<SceneNode *> m_children;

    void setParent(SceneNode *parent)
    {
        invalidateCachedParentTransform();
        m_parent = parent;
    }

    void clearParent() { setParent(nullptr); }

    mutable std::optional<Transform> m_cachedParentTransform;

    void invalidateCachedParentTransform()
    {
        m_cachedParentTransform.reset();
        invalidateChildrenCachedParentTransform();
    }

    void invalidateChildrenCachedParentTransform()
    {
        for (const auto &child : m_children) {
            child->invalidateCachedParentTransform();
        }
    }

    friend void linkSceneNodeWithEntity(entt::registry &, entt::entity);
};

//////////////////////////////////////////////////////////////////////////

// Ensure components are not relocated in memory. This allows us to use regular
// points pointing to them.
template <>
struct entt::component_traits<SceneNode> : entt::basic_component_traits {
    using in_place_delete = std::true_type;
};

// Links an entity with its corresponding SceneNode. This function is used
// automatically by the registry using the provide callback mechanism.
void linkSceneNodeWithEntity(entt::registry &reg, entt::entity e) { reg.get<SceneNode>(e).m_entity = e; }

void registerSceneNodeCallbacks(entt::registry &reg)
{
    reg.on_construct<SceneNode>().connect<&linkSceneNodeWithEntity>();
    reg.on_update<SceneNode>().connect<&linkSceneNodeWithEntity>();
}

void unregisterSceneNodeCallbacks(entt::registry &reg)
{
    reg.on_construct<SceneNode>().disconnect<&linkSceneNodeWithEntity>();
    reg.on_update<SceneNode>().disconnect<&linkSceneNodeWithEntity>();
}

//////////////////////////////////////////////////////////////////////////

int main()
{
    entt::registry reg;
    registerSceneNodeCallbacks(reg);

    // Note the use of pointers for SceneNodes as the SceneNode interface
    // prefers pointers over references.

    auto ship = reg.create();
    auto *shipNode = &reg.emplace<SceneNode>(ship);

    auto captain = reg.create();
    auto *captainNode = &reg.emplace<SceneNode>(captain);

    // connect captain with his ship
    {
        shipNode->addChild(captainNode);

        assert(shipNode->children().at(0)->entity() == captain);
        assert(captainNode->parent()->entity() == ship);
    }

    // sail the sea
    {
        shipNode->setTransform({42, 42, 42});

        assert(captainNode->transform().position.x == 0); // local
        assert(captainNode->globalTransform().position.x == 42);
    }

    // ship sinks :(
    {
        reg.destroy(ship);

        assert(captainNode->parent() == nullptr);
        assert(captainNode->transform().position.x == captainNode->globalTransform().position.x);
    }
}
