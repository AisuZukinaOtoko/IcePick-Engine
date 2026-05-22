#define VIEW_PICKING

uniform uint u_EntityId;
uniform uint u_MaterialSlotIndex;

uvec4 GetEntityMatSlot() {
	return uvec4(u_EntityId, u_MaterialSlotIndex, 0, 0);
}