/**
 * Morph Targets����N���X.
 * �t�F�C�V�����A�j���[�V�����Ȃǂ̃|���S�����b�V���̒��_�ό`�̃T�|�[�g.
 */
#ifndef _MORPHTARGETS_CTRL_H
#define _MORPHTARGETS_CTRL_H

#include "GlobalHeader.h"
#include <vector>

//-------------------------------------------------.
class CMorphTargetsData
{
public:
	std::string name;						// target��.
	std::vector<int> vIndices;				// �Ή����钸�_�C���f�b�N�X.
	std::vector<sxsdk::vec3> vertices;		// ���_���W.
	std::vector<sxsdk::vec3> normals;		// �@��.

	float weight;							// �E�G�C�g�l.

public:
	CMorphTargetsData ();

	void clear ();
};

//-------------------------------------------------.
class CMorphTargetsCtrl
{
private:
	sxsdk::shape_class* m_pTargetShape;						// �^�[�Q�b�g�ƂȂ�|���S�����b�V���`��.
	std::vector<sxsdk::vec3> m_orgVertices;					// �I���W�i���̂��ׂĂ̒��_���W.

	std::vector<CMorphTargetsData> m_morphTargetsData;		// MorphTargets�̃��X�g.

	int m_selectTargetIndex;								// �I������Ă���Target�ԍ�.

public:
	CMorphTargetsCtrl ();

	void clear ();

	//---------------------------------------------------------------.
	// �o�^/�ҏW�p.
	//---------------------------------------------------------------.
	/**
	 * �Ώی`��̃|�C���^���擾.
	 */
	sxsdk::shape_class* getTargetShape () { return m_pTargetShape; }

	/**
	 * �I���W�i���̒��_���W���擾.
	 */
	const std::vector<sxsdk::vec3>& getOrgVertices () const { return m_orgVertices;  }

	/**
	 * Morph Target�̏����擾.
	 */
	const CMorphTargetsData& getMorphTargetData (const int tIndex) const { return m_morphTargetsData[tIndex]; }

	/**
	 * �Ώۂ̃|���S�����b�V���`��N���X��n��.
	 * ����͕ό`�O�̂��̂ŁA������Ăяo������Ɉʒu�ړ������I�𒸓_��target�Ƃ��ēo�^���Ă���.
	 * @param[in] pShaoe   �Ώی`��.
	 */
	bool setupShape (sxsdk::shape_class* pShape);

	/**
	 * base�̒��_���W���i�[�Bstream����̓ǂݍ��ݎ��ɌĂ΂��.
	 */
	void setOrgVertices (const std::vector<sxsdk::vec3>& vertices);

	/**
	 * �I�𒸓_���W��MorphTargets�̒��_�Ƃ��Ēǉ�.
	 * @param[in] name      target��.
	 * @param[in] indices   �o�^���钸�_�C���f�b�N�X (�I�����ꂽ���_).
	 * @param[in] vertices  �o�^���钸�_���W.
	 * @return Morph Targets�ԍ�.
	 */
	int appendTargetVertices (const std::string& name, const std::vector<int>& indices, const std::vector<sxsdk::vec3>& vertices);

	/**
	 * �I�𒸓_���W��MorphTargets�̒��_�Ƃ��čX�V.
	 * �X�V���̒��_�̓E�G�C�g�l1.0�Ƃ���.
	 * @param[in] indices   �X�V���钸�_�C���f�b�N�X (�I�����ꂽ���_).
	 * @param[in] vertices  �X�V���钸�_���W.
	 * @return Morph Targets�ԍ�.
	 */
	int updateTargetVertices (const int tIndex, const std::vector<int>& indices, const std::vector<sxsdk::vec3>& vertices);

	/**
	 * Morph Targets�̐�.
	 */
	int getTargetsCount () const;

	/**
	 * Morph Target�̖��O���擾.
	 * @param[in]  tIndex    Morph Targets�ԍ�.
	 */
	const std::string getTargetName (const int tIndex) const;

	/**
	 * Morph Target�̖��O���w��.
	 * @param[in]  tIndex    Morph Targets�ԍ�.
	 * @param[in]  name      ���O.
	 */
	void setTargetName (const int tIndex, const std::string& name);

	/**
	 * Morph Targets�̒��_���W���擾.
	 * @param[in]  tIndex    Morph Targets�ԍ�.
	 * @param[out] indices   ���_�C���f�b�N�X���Ԃ�.
	 * @param[out] vertices  ���_���W���Ԃ�.
	 */
	bool getTargetVertices (const int tIndex, std::vector<int>& indices, std::vector<sxsdk::vec3>& vertices);

	/**
	 * Morph Targets�̃E�G�C�g�l���w��.
	 * @param[in]  tIndex    Morph Targets�ԍ�.
	 * @param[in]  weight    �E�G�C�g�l(0.0 - 1.0).
	 */
	bool setTargetWeight (const int tIndex, const float weight);

	/**
	 * Morph Targets�̃E�G�C�g�l���擾.
	 * @param[in]  tIndex    Morph Targets�ԍ�.
	 * @return �E�G�C�g�l.
	 */
	float getTargetWeight (const int tIndex) const;

	/**
	 * �w���Morph Target�����폜.
	 * @param[in]  tIndex    Morph Targets�ԍ�.
	 */
	bool removeTarget (const int tIndex);

	/**
	 * Morph Targets�����폜���āA���̒��_�ɖ߂�.
	 * @param[in] restoreVertices  ���_�����ɖ߂��ꍇ��true.
	 */
	void removeMorphTargets (const bool restoreVertices = true);

	/**
	 * ���ׂẴE�G�C�g�l��0�ɂ���B.
	 * ���̌��updateMesh���ĂԂƁA�E�G�C�g�O�̒��_�ƂȂ�.
	 */
	void setZeroAllWeight ();

	/**
	 * �d�����_���}�[�W����.
	 * �|���S�����b�V���́usxsdk::polygon_mesh_class::cleanup_redundant_vertices�v�Ɠ����ŁAMorph Targets���l����������.
	 */
	bool cleanupRedundantVertices ();

	/**
	 * Morph Targets�̏����Am_pTargetShape�̃|���S�����b�V�����X�V.
	 */
	void updateMesh ();

	//---------------------------------------------------------------.
	// Stream�ۑ�/�ǂݍ��ݗp.
	//---------------------------------------------------------------.
	/**
	 * Morph Targets��������.
	 */
	bool hasMorphTargets (sxsdk::shape_class& shape);

	/**
	 * ���݂�Morph Targets����stream�ɕۑ�.
	 */
	void writeMorphTargetsData ();

	/**
	 * Morph Targets����stream����ǂݍ���.
	 */
	bool readMorphTargetsData (sxsdk::shape_class& shape);

	//---------------------------------------------------------------.
	// UI�p.
	//---------------------------------------------------------------.
	/**
	 * �w���Target��I����Ԃɂ���B-1�̏ꍇ�͑I���Ȃ�.
	 */
	void setSelectTargetIndex (const int tIndex = -1);

	/**
	 * �I������Ă���Target�ԍ����擾.
	 */
	int getSelectTargetIndex () const;

};

#endif
