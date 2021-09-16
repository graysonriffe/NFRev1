#include "Entity.h"

#include<vector>

#include "Application.h"
#include "Model.h"
#include "Shader.h"

namespace nf {
	Entity::Entity() :
		m_constructed(false),
		m_model(nullptr),
		m_position(0.0),
		m_rotation(0.0),
		m_scale(1.0)
	{

	}

	void Entity::create(Asset* modelAsset) {
		if (m_constructed)
			Error("Entity already created!");
		m_constructed = true;
		AModel* model;
		if ((model = dynamic_cast<AModel*>(modelAsset)) == nullptr)
			Error("Non-model asset passed to Entity::create!");
		if (model->alreadyLoaded) {
			m_model = model->loadedModel;
		}
		else {
			m_model = new Model(model);
			model->alreadyLoaded = true;
			model->loadedModel = m_model;
		}

		if (!Application::getApp()->getCurrentState()->isRunning())
			Application::getApp()->getCurrentState()->m_nfObjects.push_back(this);
	}

	bool Entity::isConstructed() {
		return m_constructed;
	}

	void Entity::setPosition(double x, double y, double z) {
		m_position = { x, y, z };
	}

	void Entity::setPosition(const Vec3& position) {
		m_position = position;
	}

	void Entity::setRotation(double x, double y, double z) {
		m_rotation = { x, y, z };
	}

	void Entity::setRotation(const Vec3& rotation) {
		m_rotation = rotation;
	}

	void Entity::setScale(double x) {
		m_scale = { x, x, x };
	}

	void Entity::setScale(double x, double y, double z) {
		m_scale = { x, y, z };
	}

	void Entity::setScale(const Vec3& scale) {
		m_scale = scale;
	}

	void Entity::render(Shader* shader, bool onlyDepth) {
		shader->bind();
		setModelMatrix(shader);
		m_model->render(shader, onlyDepth);
	}

	Model* Entity::getModel() const {
		return m_model;
	}

	void Entity::setModelMatrix(Shader* shader) {
		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(m_position.x, m_position.y, m_position.z));
		model = glm::rotate(model, glm::radians((float)m_rotation.x), glm::vec3(1.0, 0.0, 0.0));
		model = glm::rotate(model, glm::radians((float)m_rotation.y), glm::vec3(0.0, 1.0, 0.0));
		model = glm::rotate(model, glm::radians((float)m_rotation.z), glm::vec3(0.0, 0.0, 1.0));
		model = glm::scale(model, glm::vec3(m_scale.x, m_scale.y, m_scale.z));
		shader->setUniform("model", model);
	}

	void Entity::destroy() {
		m_constructed = false;
		if (m_model && !m_model->isBaseAsset())
			delete m_model;
		m_model = nullptr;
		m_position = Vec3(0.0);
		m_rotation = Vec3(0.0);
		m_scale = Vec3(1.0);
	}

	Entity::~Entity() {
		destroy();
	}
}