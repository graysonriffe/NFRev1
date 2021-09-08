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

	void Entity::create(Asset* modelAsset, Asset* textureAsset) {
		m_constructed = true;
		AModel& model = *(AModel*)modelAsset;
		//TODO: Change this when doing materials
		if (model.alreadyLoaded && textureAsset == nullptr) {
			m_model = model.loadedModel;
		}
		else {
			if (!textureAsset)
				Error("No texture given to Entity create function on new model load!");
			ATexture& texture = *(ATexture*)textureAsset;
			std::string obj = model.data;
			m_model = new Model;
			std::vector<float> vb;
			std::vector<unsigned int> ib;
			size_t ibCount = 0;
			std::vector<float> tc;
			std::vector<float> vn;
			parseOBJ(obj, vb, ib, ibCount, tc, vn);
			m_model->create(&vb[0], vb.size() * sizeof(float), &ib[0], ibCount, &vn[0], vn.size() * sizeof(float), &tc[0], tc.size() * sizeof(float), &texture);
			model.alreadyLoaded = true;
			model.loadedModel = m_model;
		}
		m_model->setBaseAsset(model.isBaseAsset);
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

	void Entity::destroy() {
		m_constructed = false;
		if(m_model && !m_model->isBaseAsset())
			delete m_model;
		m_model = nullptr;
		m_position.x = 0.0;
		m_position.y = 0.0;
		m_position.z = 0.0;
		m_rotation.x = 0.0;
		m_rotation.y = 0.0;
		m_rotation.z = 0.0;
		m_scale.x = 1.0;
		m_scale.y = 1.0;
		m_scale.z = 1.0;
	}

	Entity::~Entity() {

	}
}