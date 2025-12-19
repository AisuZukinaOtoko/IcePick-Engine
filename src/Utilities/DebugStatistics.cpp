#include <string>
#include "DebugStatistics.h"
#include "Assert.h"

#ifndef RELEASE
struct CaptureEntry {
	enum EntryType { VALUE = 0, TIME_PROFILE };
	EntryType type;
	ManualTimer timer;
	CaptureEntry(EntryType type) {
		this->type = type;
		timer.begin();
	}
};
static std::vector<unsigned int> CaptureParentIndices;
static DebugCapture RootCaptureNode = { "Root", "" };
static DebugCapture SavedCaptured = { "Root", "" };

static void TraverseInsert(DebugCapture& parentCapture, DebugCapture& insertCapture, unsigned int index) {
	if (index == CaptureParentIndices.size()) {
		unsigned int insertIndex = parentCapture.ChildCaptures.size();
		if (insertCapture.Type == DebugCapture::TIME_PROFILE)
			insertCapture.Timer.begin();
		parentCapture.ChildCaptures.push_back(insertCapture);
		CaptureParentIndices.push_back(insertIndex);
		return;
	}
	unsigned int childIndex = CaptureParentIndices[index];
	TraverseInsert(parentCapture.ChildCaptures[childIndex], insertCapture, index + 1);
}

static void TraversePop(DebugCapture& parentCapture, unsigned int index) {
	if (index == CaptureParentIndices.size()) {
		if (parentCapture.Type == DebugCapture::TIME_PROFILE) {
			float captureTime = parentCapture.Timer.finish();
			parentCapture.PropertyValue = std::to_string(captureTime / 1000.0f) + "ms";
		}
		CaptureParentIndices.pop_back();
		return;
	}
	unsigned int childIndex = CaptureParentIndices[index];
	TraversePop(parentCapture.ChildCaptures[childIndex], index + 1);
}

static unsigned int GetChildIndex(DebugCapture& parentCapture, unsigned int index) {
	if (index == CaptureParentIndices.size())
		return parentCapture.ChildCaptures.size();

	unsigned int childIndex = CaptureParentIndices[index];
	return GetChildIndex(parentCapture.ChildCaptures[childIndex], index + 1);
}

void IP_CORE_PROFILE_BEGIN(std::string propertyName) {
	DebugCapture insert(propertyName);
	TraverseInsert(RootCaptureNode, insert, 0);
}

//void IP_CORE_PROFILE_PUSH(std::string propertyName) {
//	DebugCapture insert(propertyName);
//	TraverseInsert(RootCaptureNode, insert, 0);
//	std::cout << "-----------------------New Push-----------------------"  << std::endl;
//}

void IP_CORE_PROFILE_POP() {
	IP_ASSERT(CaptureParentIndices.size() != 0, "Cannot pop profile stack.");
	TraversePop(RootCaptureNode, 0);
	//CaptureParentIndices.pop_back();
	//ManualTimer timer = CaptureTimers.top();
}

void IP_CORE_PROFILE_CLEAR() {
	RootCaptureNode.ChildCaptures.clear();
	CaptureParentIndices.clear();
}

void IP_CORE_PROFILE_LOG(std::string propertyName, int propertyValue) {
	IP_CORE_PROFILE_LOG(propertyName, std::to_string(propertyValue));
}

void IP_CORE_PROFILE_LOG(std::string propertyName, float propertyValue) {
	IP_CORE_PROFILE_LOG(propertyName, std::to_string(propertyValue));
}

void IP_CORE_PROFILE_LOG(std::string propertyName, std::string propertyValue) {
	DebugCapture insert(propertyName, propertyValue);
	TraverseInsert(RootCaptureNode, insert, 0);
}

#else
void IP_CORE_PROFILE_BEGIN(std::string propertyName) {}
void IP_CORE_PROFILE_POP() {}
void IP_CORE_PROFILE_CLEAR() {}
void IP_CORE_PROFILE_LOG(std::string propertyName, int propertyValue) {}
void IP_CORE_PROFILE_LOG(std::string propertyName, float propertyValue) {}
void IP_CORE_PROFILE_LOG(std::string propertyName, std::string propertyValue) {}
void IP_CORE_PROFILE_CAPTURE() {}
#endif

const DebugCapture& IP_GET_CORE_PROFILE() {
	return SavedCaptured;
}

void IP_CORE_PROFILE_CAPTURE() {
	SavedCaptured = RootCaptureNode;
}