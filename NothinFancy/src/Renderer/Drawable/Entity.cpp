#include "Entity.h"

#include<vector>

namespace nf {
	Entity::Entity() :
		m_model(nullptr),
		m_position(0.0),
		m_rotation(0.0),
		m_scale(1.0)
	{

	}

	void Entity::create(Asset* modelAsset, Asset* textureAsset) {
		AModel& model = *(AModel*)modelAsset;
		if (model.alreadyLoaded) {
			m_model = model.loadedModel;
			return;
		}
		if (!textureAsset)
			Error("No texture given to Entity create function on new model load!");
		ATexture& texture = *(ATexture*)textureAsset;
		std::string obj = model.data;
		m_model = new Model;
		std::vector<float> vb;
		std::vector<unsigned int> ib;
		size_t ibCount = 0;
		std::vector<float> tc;
		parseOBJ(obj, vb, ib, ibCount, tc);
		m_model->create(&vb[0], vb.size() * sizeof(float), &ib[0], ibCount, &tc[0], tc.size() * sizeof(float), texture.data, texture.size);
		model.alreadyLoaded = true;
		model.loadedModel = m_model;
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
		m_rotation = { x, x, x };
	}

	void Entity::setScale(double x, double y, double z) {
		m_scale = { x, y, z };
	}

	void Entity::setScale(const Vec3& scale) {
		m_scale = scale;
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
		model = glm::rotate(model, glm::radians((float)m_rotation.x), glm::vec3(1.0, 0.0, 0.0));
		model = glm::rotate(model, glm::radians((float)m_rotation.y), glm::vec3(0.0, 1.0, 0.0));
		model = glm::rotate(model, glm::radians((float)m_rotation.z), glm::vec3(0.0, 0.0, 1.0));
		model = glm::scale(model, glm::vec3(m_scale.x, m_scale.y, m_scale.z));
		shader->setUniform("model", model);
	}

	Entity::~Entity() {

	}
}