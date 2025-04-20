#pragma once
#include "../Utilities/Clock.h"
#include "Entity.h"

template <typename T>
class Ref {
public:
	Ref(T* ref) : m_Data(ref), m_Index(-1) {}
	T* operator->() { return m_Data; }
	void AssignIndex(int index) { m_Index = index; }
	bool IsValid() { return (m_Data != nullptr) || (m_Index != -1); }
private:
	T* m_Data;
	int m_Index;
};

namespace IcePick {
	class Scene {
	public:
		void OnBegin();
		void OnUpdate(DeltaTime dt);
		void OnPreRender();
		void OnUIRender();
		void OnEnd();
		void LoadFromDisk(const char* path);
	private:
		UUID m_SceneID;
	};
}