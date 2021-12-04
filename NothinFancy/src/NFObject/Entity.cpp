#include "nf/Entity.h"

#include<vector>
#include "glm/gtx/quaternion.hpp"

#include "nf/Application.h"
#include "nf/Model.h"
#include "nf/Shader.h"

namespace nf {
	Entity::Entity() :
		m_constructed(false),
		m_member(false),
		m_type(Type::STATIC),
		m_model(nullptr),
		m_position(0.0),
		m_rotation(degToQuat({ 0.0 })),
		m_scale(1.0),
		m_update(false)
	{
		if (!Application::getApp() || !Application::getApp()->getCurrentState())
			m_member = true;
	}

	void Entity::create(Asset* modelAsset, const Vec3& position, Type type) {
		if (m_constructed)
			NFError("Entity already created!");
		m_constructed = true;
		setPosition(position);
		m_type = type;
		AModel* model;
		if ((model = dynamic_cast<AModel*>(modelAsset)) == nullptr)
			NFError("Non-model asset passed to Entity::create!");
		if (model->alreadyLoaded) {
			m_model = model->loadedModel;
		}
		else {
			bool convex = false, triangle = false;
			if (m_type == Entity::Type::STATIC || m_type == Entity::Type::DYNAMIC)
				convex = true;
			else if (m_type == Entity::Type::MAP)
				triangle = true;
			m_model = new Model(model, convex, triangle);
			model->alreadyLoaded = true;
			model->loadedModel = m_model;
		}

		if (type != Type::DETAIL)
			Application::getApp()->getPhysicsEngine()->addActor(this);

		if (m_member)
			Application::getApp()->getCurrentState()->m_nfObjects.push_back(this);
		else
			Application::getApp()->getCurrentState()->m_entsToDelete.push_back(this);
	}

	bool Entity::isConstructed() {
		return m_constructed;
	}

	Entity::Type Entity::getType() {
		return m_type;
	}

	void Entity::setPosition(float x, float y, float z) {
		m_position = { x, y, z };
		m_update = true;
	}

	void Entity::setPosition(const Vec3& position) {
		m_position = position;
		m_update = true;
	}

	void Entity::setPositionPhysics(const Vec3& position) {
		m_position = position;
	}

	void Entity::setRotation(float x, float y, float z) {
		m_rotation = degToQuat({ x, y, z });
		m_update = true;
	}

	void Entity::setRotation(const Vec3& rotation) {
		m_rotation = degToQuat(rotation);
		m_update = true;
	}

	void Entity::setRotationPhysics(const Vec4& rotation) {
		m_rotation = rotation;
	}

	void Entity::setScale(float x) {
		m_scale = { x, x, x };
		m_update = true;
	}

	void Entity::setScale(float x, float y, float z) {
		m_scale = { x, y, z };
		m_update = true;
	}

	void Entity::setScale(const Vec3& scale) {
		m_scale = scale;
		m_update = true;
	}

	void Entity::setVelocity(float x, float y, float z) {
		setVelocity(Vec3(x, y, z));
	}

	void Entity::setVelocity(const Vec3& velocity) {
		Application::getApp()->getPhysicsEngine()->setActorVelocity(this, velocity);
	}

	void Entity::setMass(float mass) {
		Application::getApp()->getPhysicsEngine()->setActorMass(this, mass);
	}

	bool Entity::needsPhysicsUpdate() {
		if (m_update) {
			m_update = false;
			return true;
		}
		return false;
	}

	const Vec3& Entity::getPosition() {
		return m_position;
	}

	const Vec4& Entity::getRotation() {
		return m_rotation;
	}

	const Vec3& Entity::getScale() {
		return m_scale;
	}

	void Entity::render(Shader* shader, bool onlyDepth) {
		shader->bind();
		glm::mat4 model = getModelMatrix();
		shader->setUniform("model[0]", model);
		m_model->render(shader, onlyDepth, 1);
	}

	Model* Entity::getModel() const {
		return m_model;
	}

	const glm::mat4 Entity::getModelMatrix() {
		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(m_position.x, m_position.y, m_position.z));

		glm::quat rotQ = glm::quat(m_rotation.w, m_rotation.x, m_rotation.y, m_rotation.z);
		glm::mat4 rot = glm::toMat4(rotQ);
		model *= rot;

		model = glm::scale(model, glm::vec3(m_scale.x, m_scale.y, m_scale.z));
		return model;
	}

	void Entity::destroy() {
		if (m_constructed && m_model) {
			Application::getApp()->getCurrentState()->m_modelsToDelete.insert(m_model);
			m_model = nullptr;
		}
		m_constructed = false;
		m_type = Type::STATIC;
		m_position = Vec3(0.0);
		m_rotation = degToQuat({ 0.0 });
		m_scale = Vec3(1.0);
	}

	Entity::~Entity() {
		destroy();
	}
}