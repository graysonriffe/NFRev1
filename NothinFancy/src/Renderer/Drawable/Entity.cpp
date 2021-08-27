#include "Entity.h"

namespace nf {
	Entity::Entity() :
		m_model(nullptr),
		m_position(0.0),
		m_rotation(0.0),
		m_scale(1.0)
	{

	}

	void Entity::create(const void* vertexBufferData, const size_t vertexBufferSize, const void* indexBufferData, size_t indexBufferCount, const void* textureCoordinatesBufferData, size_t textureCoordinatesBufferSize, const char* textureName) {
		m_model = new Model;
		m_model->create(vertexBufferData, vertexBufferSize, indexBufferData, indexBufferCount, textureCoordinatesBufferData, textureCoordinatesBufferSize, textureName);
	}

	void Entity::setPosition(float x, float y, float z) {
		m_position = { x, y, z };
	}

	void Entity::setRotation(float x, float y, float z) {
		m_rotation = { x, y, z };
	}

	void Entity::setScale(float x) {
		m_rotation = { x, x, x };
	}

	void Entity::setScale(float x, float y, float z) {
		m_scale = { x, y, z };
	}

	void Entity::bind(Shader* shader) {
		m_model->bind();
		shader->bind();
		setModelMatrix(shader);
	}

	Model* Entity::getModel() const {
		return m_model;
	}

	void Entity::setModelMatrix(Shader* shader) {
		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(m_position.x, m_position.y, m_position.z));
		model = glm::rotate(model, glm::radians(m_rotation.x), glm::vec3(1.0, 0.0, 0.0));
		model = glm::rotate(model, glm::radians(m_rotation.y), glm::vec3(0.0, 1.0, 0.0));
		model = glm::rotate(model, glm::radians(m_rotation.z), glm::vec3(0.0, 0.0, 1.0));
		model = glm::scale(model, glm::vec3(m_scale.x, m_scale.y, m_scale.z));
		shader->setUniform("model", model);
	}

	Entity::~Entity() {

	}
}