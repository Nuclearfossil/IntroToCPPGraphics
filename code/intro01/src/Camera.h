#ifndef __CAMERA_H__
#define __CAMERA_H__

class Camera
{
public:
	Camera(void);
	Camera(Camera &_camera);
	~Camera(void);

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	DirectX::XMFLOAT3 GetPosition() { DirectX::XMFLOAT3(m_positionX, m_positionY, m_positionZ); }
	DirectX::XMFLOAT3 GetRotation() { DirectX::XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ); }

	void Render();
	const DirectX::XMMATRIX& GetViewMatrix() { return m_viewMatrix; }
    const DirectX::XMMATRIX& GetProjMatrix() { return m_projMatrix; }

private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	DirectX::XMMATRIX m_viewMatrix;
    DirectX::XMMATRIX m_projMatrix;
};

#endif // __CAMERA_H__